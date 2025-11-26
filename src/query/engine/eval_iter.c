/**
 * @file query/engine/eval_iter.c
 * @brief Query iterator.
 */

#include "../../private_api.h"

static
void flecs_query_iter_run_ctx_init(
    ecs_iter_t *it,
    ecs_query_run_ctx_t *ctx)
{
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, it->query);
    ctx->world = it->real_world;
    ctx->query = impl;
    ctx->it = it;
    ctx->vars = qit->vars;
    ctx->query_vars = qit->query_vars;
    ctx->written = qit->written;
    ctx->op_ctx = qit->op_ctx;
    ctx->qit = qit;
}

void flecs_query_iter_constrain(
    ecs_iter_t *it)
{
    ecs_query_run_ctx_t ctx;
    flecs_query_iter_run_ctx_init(it, &ctx);
    ecs_assert(ctx.written != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_query_impl_t *query = ctx.query;
    const ecs_query_t *q = &query->pub;
    ecs_flags64_t it_written = it->constrained_vars;
    ctx.written[0] = it_written;
    if (it_written && ctx.query->src_vars) {
        /* If variables were constrained, check if there are any table
         * variables that have a constrained entity variable. */
        ecs_var_t *vars = ctx.vars;
        int32_t i, count = q->field_count;
        for (i = 0; i < count; i ++) {
            ecs_var_id_t var_id = query->src_vars[i];
            ecs_query_var_t *var = &query->vars[var_id];

            if (!(it_written & (1ull << var_id)) || 
                (var->kind == EcsVarTable) || (var->table_id == EcsVarNone)) 
            {
                continue;
            }

            /* Initialize table variable with constrained entity variable */
            ecs_var_t *tvar = &vars[var->table_id];
            tvar->range = flecs_range_from_entity(ctx.world, vars[var_id].entity);
            ctx.written[0] |= (1ull << var->table_id); /* Mark as written */
        }
    }

    /* This function can be called multiple times when setting variables, so
     * reset flags before setting them. */
    it->flags &= ~(EcsIterTrivialTest|EcsIterTrivialCached|
        EcsIterTrivialSearch);

    /* Figure out whether this query can utilize specialized iterator modes for
     * improved performance. */
    ecs_flags32_t flags = q->flags;
    ecs_flags32_t trivial_flags = EcsQueryIsTrivial|EcsQueryMatchOnlySelf;
    ecs_query_cache_t *cache = query->cache;

    if (it_written) {
        /* When we're testing against an entity or table, set the $this
         * variable in advance since it won't change later on. This 
         * initializes it.count, it.entities and it.table. */
        flecs_query_set_iter_this(it, &ctx);

        if (!cache) {
            if ((flags & (trivial_flags)) == trivial_flags) {
                if (!(flags & EcsQueryMatchWildcards)) {
                    it->flags |= EcsIterTrivialTest;
                    flecs_query_setids(NULL, false, &ctx);
                }
            }
        } else if (flags & EcsQueryIsCacheable) {
            if (!query->ops) {
                if (!cache->order_by_callback && 
                    (cache->query->flags & EcsQueryTrivialCache && 
                    !(query->pub.flags & EcsQueryHasChangeDetection))) 
                {
                    it->flags |= EcsIterTrivialTest|EcsIterTrivialCached|
                        EcsIterTrivialChangeDetection;
                    it->ids = cache->query->ids;
                    it->sources = cache->sources;
                    it->set_fields = flecs_uto(uint32_t, (1llu << it->field_count) - 1);
                } else {
                    it->flags |= EcsIterTrivialTest|EcsIterCached;
                }
            }
        }
    } else {
        if (!cache) { 
            if ((flags & (trivial_flags)) == trivial_flags) {
                if (!(flags & EcsQueryMatchWildcards)) {
                    it->flags |= EcsIterTrivialSearch|
                        EcsIterTrivialChangeDetection;
                    flecs_query_setids(NULL, false, &ctx);
                }
            }
        } else if (flags & EcsQueryIsCacheable) {
            if (!query->ops) {
                if (!cache->order_by_callback && 
                    (cache->query->flags & EcsQueryTrivialCache && 
                    !(query->pub.flags & EcsQueryHasChangeDetection))) 
                {
                    it->flags |= EcsIterTrivialSearch|EcsIterTrivialCached|
                        EcsIterTrivialChangeDetection;
                    it->ids = cache->query->ids;
                    it->sources = cache->sources;
                    it->set_fields = flecs_uto(uint32_t, (1llu << it->field_count) - 1);
                } else {
                    it->flags |= EcsIterTrivialSearch|EcsIterCached;
                }
            }
        }
    }
}

static
void flecs_query_change_detection(
    ecs_iter_t *it,
    ecs_query_iter_t *qit,
    ecs_query_impl_t *impl)
{
    /* Change detection */
    if (!(it->flags & EcsIterSkip)) {
        /* Mark table columns that are written to dirty */
        flecs_query_mark_fields_dirty(impl, it);
        if (qit->elem) {
            if (impl->pub.flags & EcsQueryHasChangeDetection) {
                /* If this query uses change detection, synchronize the
                 * monitor for the iterated table with the query */
                flecs_query_sync_match_monitor(impl, qit->elem);
            }
        }
    }
}

static
void flecs_query_self_change_detection(
    ecs_iter_t *it,
    ecs_query_iter_t *qit,
    ecs_query_impl_t *impl)
{
    if (!it->table->dirty_state) {
        return;
    }

    flecs_query_change_detection(it, qit, impl);
}

bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_query_next || 
        it->next == flecs_query_trivial_cached_next ||
        it->next == flecs_default_next_callback,
            ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, it->query);
    ecs_assert(impl != NULL, ECS_INVALID_OPERATION, 
        "cannot call ecs_query_next on invalid iterator");

    ecs_query_run_ctx_t ctx;
    flecs_query_iter_run_ctx_init(it, &ctx);

    bool redo = it->flags & EcsIterIsValid;
    if (redo) {
        if (it->flags & EcsIterTrivialChangeDetection) {
            flecs_query_self_change_detection(it, qit, impl);
        } else {
            flecs_query_change_detection(it, qit, impl);
        }
    }

    it->flags &= ~(EcsIterSkip);
    it->flags |= EcsIterIsValid;
    it->frame_offset += it->count;

    /* Specialized iterator modes. When a query doesn't use any advanced 
     * features, it can call specialized iterator functions directly instead of
     * going through the dispatcher of the query engine. 
     * The iterator mode is set during iterator initialization. Besides being
     * determined by the query, there are different modes for searching and 
     * testing, where searching returns all matches for a query, whereas testing
     * tests a single table or table range against the query. */

    if (it->flags & EcsIterTrivialCached) {
        /* Trivial cache iterator. Only supported for search */
        ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);

        if (it->flags & EcsIterTrivialSearch) {
            if (flecs_query_is_trivial_cache_search(&ctx)) {
                return true;
            }
        } else if (it->flags & EcsIterTrivialTest) {
            if (flecs_query_is_trivial_cache_test(&ctx, redo)) {
                return true;
            }
        }
    } else if (it->flags & EcsIterCached) {
        /* Cached iterator modes */
        if (it->flags & EcsIterTrivialSearch) {
            ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);
            if (flecs_query_is_cache_search(&ctx)) {
                goto trivial_search_yield;
            }
        } else if (it->flags & EcsIterTrivialTest) {
            ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);
            if (flecs_query_is_cache_test(&ctx, redo)) {
                goto yield;
            }
        }
    } else {
        /* Uncached iterator modes */
        if (it->flags & EcsIterTrivialSearch) {
            ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
            if (flecs_query_is_trivial_search(&ctx, op_ctx, redo)) {
                goto yield;
            }
        } else if (it->flags & EcsIterTrivialTest) {
            ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);

            int32_t fields = ctx.query->pub.term_count;
            ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
            if (flecs_query_trivial_test(&ctx, redo, mask)) {
                goto yield;
            }
        } else {
            const ecs_query_op_t *ops = qit->ops;

            /* Default iterator mode. This enters the query VM dispatch loop. */
            if (flecs_query_run_until(
                redo, &ctx, ops, -1, qit->op, impl->op_count - 1)) 
            {
                ecs_assert(ops[ctx.op_index].kind == EcsQueryYield, 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_query_set_iter_this(it, &ctx);
                ecs_assert(it->count >= 0, ECS_INTERNAL_ERROR, NULL);
                qit->op = flecs_itolbl(ctx.op_index - 1);
                goto yield;
            }
        }
    }

    /* Done iterating */
    flecs_query_mark_fixed_fields_dirty(impl, it);
    if (ctx.query->monitor) {
        flecs_query_update_fixed_monitor(
            ECS_CONST_CAST(ecs_query_impl_t*, ctx.query));
    }

    it->flags |= EcsIterSkip; /* Prevent change detection on fini */

    ecs_iter_fini(it);
    ecs_os_linc(&it->real_world->info.queries_ran_total);
    return false;

trivial_search_yield:
    it->table = ctx.vars[0].range.table;
    it->count = ecs_table_count(it->table);
    it->entities = ecs_table_entities(it->table);

yield:
    return true;
}

bool flecs_query_trivial_cached_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, it->query);
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_run_ctx_t ctx;
    flecs_query_iter_run_ctx_init(it, &ctx);

    bool redo = it->flags & EcsIterIsValid;
    if (redo) {
        flecs_query_self_change_detection(it, qit, impl);
    }

    it->flags &= ~(EcsIterSkip);
    it->flags |= EcsIterIsValid;
    it->frame_offset += it->count;

    ecs_assert(it->flags & EcsIterTrivialCached, ECS_INVALID_OPERATION,
        "query does not have trivial cache, use ecs_query_next instead");
    ecs_assert(it->flags & EcsIterTrivialSearch, ECS_INVALID_OPERATION,
        "iterator has constrained variables, use ecs_query_next instead");
    ecs_assert(impl->ops == NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_is_trivial_cache_search(&ctx)) {
        return true;
    }

    it->flags |= EcsIterSkip; /* Prevent change detection on fini */

    ecs_iter_fini(it);
    ecs_os_linc(&it->real_world->info.queries_ran_total);
    return false;
}

void flecs_query_op_ctx_fini(
    ecs_iter_t *it,
    const ecs_query_op_t *op,
    ecs_query_op_ctx_t *ctx)
{
    switch(op->kind) {
    case EcsQueryTrav: {
        ecs_allocator_t *a = flecs_query_get_allocator(it);
        flecs_query_trav_cache_fini(a, &ctx->is.trav.cache);
        break;
    }
    case EcsQueryUp:
    case EcsQuerySelfUp:
    case EcsQueryTreeUp:
    case EcsQueryTreeSelfUp:
    case EcsQueryTreeUpPre:
    case EcsQueryTreeSelfUpPre:
    case EcsQueryTreeUpPost:
    case EcsQueryTreeSelfUpPost:
    case EcsQuerySparseUp:
    case EcsQuerySparseSelfUp: {
        ecs_allocator_t *a = flecs_query_get_allocator(it);
        ecs_query_up_ctx_t *op_ctx = &ctx->is.up;
        ecs_query_up_impl_t *impl = op_ctx->impl;
        if (impl) {
            ecs_trav_up_cache_t *cache = &impl->cache;
            if (cache->dir == EcsTravDown) {
                flecs_query_down_cache_fini(a, cache);
            } else {
                flecs_query_up_cache_fini(cache);
            }
            flecs_free_t(a, ecs_query_up_impl_t, impl);
        }
        break;
    }
    default:
        break;
    }
}

static
void flecs_query_iter_fini_ctx(
    ecs_iter_t *it,
    ecs_query_iter_t *qit)
{
    const ecs_query_impl_t *query = flecs_query_impl(it->query);
    int32_t i, count = query->op_count;
    ecs_query_op_t *ops = query->ops;
    ecs_query_op_ctx_t *ctx = qit->op_ctx;

    for (i = 0; i < count; i ++) {
        ecs_query_op_t *op = &ops[i];
        flecs_query_op_ctx_fini(it, op, &ctx[i]);
    }
}

static
void flecs_query_iter_fini(
    ecs_iter_t *it)
{
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    const ecs_query_t *q = it->query;
    ecs_assert(q != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_assert(q, ecs_query_t);
    int32_t op_count = flecs_query_impl(q)->op_count;
    int32_t var_count = flecs_query_impl(q)->var_count;

    if (it->flags & EcsIterIsValid) {
        flecs_query_change_detection(it, qit, flecs_query_impl(q));
    }

#ifdef FLECS_DEBUG
    if (it->flags & EcsIterProfile) {
        char *str = ecs_query_plan_w_profile(q, it);
        printf("%s\n", str);
        ecs_os_free(str);
    }

    flecs_iter_free_n(qit->profile, ecs_query_op_profile_t, op_count);
#endif

    flecs_query_iter_fini_ctx(it, qit);
    flecs_iter_free_n(qit->vars, ecs_var_t, var_count);
    flecs_iter_free_n(qit->written, ecs_write_flags_t, op_count);
    flecs_iter_free_n(qit->op_ctx, ecs_query_op_ctx_t, op_count);

    qit->vars = NULL;
    qit->written = NULL;
    qit->op_ctx = NULL;
    it->query = NULL;
}

static
void flecs_query_validate_final_fields(
    const ecs_query_t *q)
{
    (void)q;
#ifdef FLECS_DEBUG
    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_world_t *world = q->real_world;

    if (!impl->final_terms) {
        return;
    }

    if (!world->cr_isa_wildcard) {
        return;
    }

    int32_t i, count = impl->pub.term_count;
    ecs_term_t *terms = impl->pub.terms;
    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_id_t id = term->id;

        if (!(impl->final_terms & (1ull << i))) {
            continue;
        }

        if (ECS_IS_PAIR(id)) {
            id = ECS_PAIR_FIRST(id);
        }

        if (ecs_id_is_wildcard(id)) {
            continue;
        }

        if (flecs_components_get(world, ecs_pair(EcsIsA, id))) {
            char *query_str = ecs_query_str(q);
            char *id_str = ecs_id_str(world, id);
            ecs_abort(ECS_INVALID_OPERATION, 
                    "query '%s' was created before '(IsA, %s)' relationship, "
                    "create query after adding inheritance relationship "
                    "or add 'Inheritable' trait to '%s'", 
                        query_str, id_str, id_str);
            ecs_os_free(id_str);
            ecs_os_free(query_str);
        }
    }
#endif
}

ecs_iter_t flecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q)
{
    ecs_iter_t it = {0};
    ecs_query_iter_t *qit = &it.priv_.iter.query;
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);

#ifdef FLECS_DEBUG
    flecs_check_exclusive_world_access_write(q->real_world);
#endif

    flecs_query_validate_final_fields(q);
    
    flecs_poly_assert(q, ecs_query_t);
    ecs_query_impl_t *impl = flecs_query_impl(q);

    int32_t i, var_count = impl->var_count;
    int32_t op_count = impl->op_count ? impl->op_count : 1;
    it.world = ECS_CONST_CAST(ecs_world_t*, world);

    /* If world passed to iterator is the real world, but query was created from
     * a stage, stage takes precedence. */
    if (flecs_poly_is(it.world, ecs_world_t) && 
        flecs_poly_is(q->world, ecs_stage_t)) 
    {
        it.world = ECS_CONST_CAST(ecs_world_t*, q->world);
    }

    it.real_world = q->real_world;
    ecs_assert(flecs_poly_is(it.real_world, ecs_world_t),
        ECS_INTERNAL_ERROR, NULL);
    ecs_check(!(it.real_world->flags & EcsWorldMultiThreaded) || 
        it.world != it.real_world, ECS_INVALID_PARAMETER, 
            "create iterator for stage when world is in multithreaded mode");

    it.query = q;
    it.system = q->entity;
    it.next = ecs_query_next;
    it.fini = flecs_query_iter_fini;
    it.field_count = q->field_count;
    it.sizes = q->sizes;
    it.set_fields = q->set_fields;
    it.ref_fields = q->fixed_fields | q->row_fields | q->var_fields;
    it.row_fields = q->row_fields;
    it.up_fields = 0;
    flecs_query_apply_iter_flags(&it, q);

    bool fully_cached = (q->flags & EcsQueryIsCacheable) && !(q->flags & EcsQueryCacheWithFilter);
    flecs_iter_init(it.world, &it, !impl->cache || !fully_cached);

    qit->query_vars = impl->vars;
    qit->ops = impl->ops;

    if (q->flags & EcsQueryMatchEmptyTables) {
        it.flags |= EcsIterMatchEmptyTables;
    }

    flecs_query_cache_iter_init(&it, qit, impl);

    if (var_count) {
        qit->vars = flecs_iter_calloc_n(&it, ecs_var_t, var_count);
    }

    if (op_count) {
        qit->written = flecs_iter_calloc_n(&it, ecs_write_flags_t, op_count);
        qit->op_ctx = flecs_iter_calloc_n(&it, ecs_query_op_ctx_t, op_count);
    }

#ifdef FLECS_DEBUG
    qit->profile = flecs_iter_calloc_n(&it, ecs_query_op_profile_t, op_count);
#endif

    for (i = 1; i < var_count; i ++) {
        qit->vars[i].entity = EcsWildcard;
    }

    /* Set flags for unconstrained query iteration. Can be reinitialized when
     * variables are constrained on iterator. */
    flecs_query_iter_constrain(&it);
error:
    return it;
}

ecs_iter_t ecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(q != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Ok, only for stats */
    ecs_os_linc(&ECS_CONST_CAST(ecs_query_t*, q)->eval_count);

    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        /* If monitors changed, do query rematching */
        ecs_flags32_t flags = q->flags;
        if (!(ecs_world_get_flags(world) & EcsWorldReadonly) && 
             (flags & EcsQueryHasRefs)) 
        {
            flecs_eval_component_monitors(q->world);
        }
    }

    return flecs_query_iter(world, q);
}
