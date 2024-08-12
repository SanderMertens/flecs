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
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, qit->query);
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
            tvar->range = flecs_range_from_entity(vars[var_id].entity, &ctx);
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
    ecs_query_cache_t *cache = query->cache;
    if (flags & EcsQueryIsTrivial) {
        if ((flags & EcsQueryMatchOnlySelf)) {
            if (it_written) {
                /* When we're testing against an entity or table, set the $this
                 * variable in advance since it won't change later on. This 
                 * initializes it.count, it.entities and it.table. */
                flecs_query_set_iter_this(it, &ctx);

                if (!cache) {
                    if (!(flags & EcsQueryMatchWildcards)) {
                        it->flags |= EcsIterTrivialTest;
                    }
                } else if (flags & EcsQueryIsCacheable) {
                    it->flags |= EcsIterTrivialTest|EcsIterTrivialCached;
                }
            } else {
                if (!cache) {
                    if (!(flags & EcsQueryMatchWildcards)) {
                        it->flags |= EcsIterTrivialSearch;
                    }
                } else if (flags & EcsQueryIsCacheable) {
                    if (!cache->order_by_callback) {
                        it->flags |= EcsIterTrivialSearch|EcsIterTrivialCached;
                    }
                }
            }

            /* If we're using a specialized iterator mode, make sure to 
             * initialize static component ids. Usually this is the first 
             * instruction of a query plan, but because we're not running the
             * query plan when using a specialized iterator mode, manually call
             * the operation on iterator init. */
            flecs_query_setids(NULL, false, &ctx);
        }
    }
}

bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, qit->query);
    ecs_query_run_ctx_t ctx;
    flecs_query_iter_run_ctx_init(it, &ctx);
    const ecs_query_op_t *ops = qit->ops;

    bool redo = it->flags & EcsIterIsValid;
    if (redo) {
        /* Change detection */
        if (!(it->flags & EcsIterSkip)) {
            /* Mark table columns that are written to dirty */
            flecs_query_mark_fields_dirty(impl, it);
            if (qit->prev) {
                if (ctx.query->pub.flags & EcsQueryHasMonitor) {
                    /* If this query uses change detection, synchronize the
                     * monitor for the iterated table with the query */
                    flecs_query_sync_match_monitor(impl, qit->prev);
                }
            }
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
        /* Cached iterator modes */
        if (it->flags & EcsIterTrivialSearch) {
            if (flecs_query_is_cache_search(&ctx)) {
                goto trivial_search_yield;
            }
        } else if (it->flags & EcsIterTrivialTest) {
            if (flecs_query_is_cache_test(&ctx, redo)) {
                goto yield;
            }
        }
    } else {
        /* Uncached iterator modes */
        if (it->flags & EcsIterTrivialSearch) {
            ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
            if (flecs_query_is_trivial_search(&ctx, op_ctx, redo)) {
                goto yield;
            }
        } else if (it->flags & EcsIterTrivialTest) {
            int32_t fields = ctx.query->pub.term_count;
            ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
            if (flecs_query_trivial_test(&ctx, redo, mask)) {
                goto yield;
            }
        } else {
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

    ecs_iter_fini(it);
    return false;

trivial_search_yield:
    it->table = ctx.vars[0].range.table;
    it->count = ecs_table_count(it->table);
    it->entities = ecs_table_entities(it->table);

yield:
    return true;
}

static
void flecs_query_iter_fini_ctx(
    ecs_iter_t *it,
    ecs_query_iter_t *qit)
{
    const ecs_query_impl_t *query = flecs_query_impl(qit->query);
    int32_t i, count = query->op_count;
    ecs_query_op_t *ops = query->ops;
    ecs_query_op_ctx_t *ctx = qit->op_ctx;
    ecs_allocator_t *a = flecs_query_get_allocator(it);

    for (i = 0; i < count; i ++) {
        ecs_query_op_t *op = &ops[i];
        switch(op->kind) {
        case EcsQueryTrav:
            flecs_query_trav_cache_fini(a, &ctx[i].is.trav.cache);
            break;
        case EcsQueryUp:
        case EcsQuerySelfUp:
        case EcsQueryUpId:
        case EcsQuerySelfUpId: 
        case EcsQueryUnionEqUp:
        case EcsQueryUnionEqSelfUp: {
            ecs_trav_up_cache_t *cache = &ctx[i].is.up.cache;
            if (cache->dir == EcsTravDown) {
                flecs_query_down_cache_fini(a, cache);
            } else {
                flecs_query_up_cache_fini(cache);
            }
            break;
        }
        default:
            break;
        }
    }
}

static
void flecs_query_iter_fini(
    ecs_iter_t *it)
{
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_assert(qit->query != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_assert(qit->query, ecs_query_t);
    int32_t op_count = flecs_query_impl(qit->query)->op_count;
    int32_t var_count = flecs_query_impl(qit->query)->var_count;

#ifdef FLECS_DEBUG
    if (it->flags & EcsIterProfile) {
        char *str = ecs_query_plan_w_profile(qit->query, it);
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
    qit->query = NULL;
}

ecs_iter_t flecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q)
{
    ecs_iter_t it = {0};
    ecs_query_iter_t *qit = &it.priv_.iter.query;
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);
    
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
    it.ref_fields = q->fixed_fields | q->row_fields;
    it.row_fields = q->row_fields;
    it.up_fields = 0;
    flecs_query_apply_iter_flags(&it, q);

    flecs_iter_init(it.world, &it, 
        flecs_iter_cache_ids |
        flecs_iter_cache_trs |
        flecs_iter_cache_sources |
        flecs_iter_cache_ptrs);

    qit->query = q;
    qit->query_vars = impl->vars;
    qit->ops = impl->ops;

    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        qit->node = cache->list.first;
        qit->last = cache->list.last;

        if (cache->order_by_callback && cache->list.info.table_count) {
            flecs_query_cache_sort_tables(it.real_world, impl);
            qit->node = ecs_vec_first(&cache->table_slices);
            qit->last = ecs_vec_last_t(
                &cache->table_slices, ecs_query_cache_table_match_t);
        }

        cache->prev_match_count = cache->match_count;
    }

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

    it.variables = qit->vars;
    it.variable_count = impl->pub.var_count;
    it.variable_names = impl->pub.vars;

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
    
    if (!(q->flags & EcsQueryCacheYieldEmptyTables)) {
        ecs_run_aperiodic(q->real_world, EcsAperiodicEmptyTables);
    }

    /* Ok, only for stats */
    ECS_CONST_CAST(ecs_query_t*, q)->eval_count ++;

    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        /* If monitors changed, do query rematching */
        ecs_flags32_t flags = q->flags;
        if (!(world->flags & EcsWorldReadonly) && flags & EcsQueryHasRefs) {
            flecs_eval_component_monitors(q->world);
        }
    }

    return flecs_query_iter(world, q);
}
