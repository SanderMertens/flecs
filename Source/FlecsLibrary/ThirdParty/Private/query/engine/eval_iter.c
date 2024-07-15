/**
 * @file query/engine/eval_iter.c
 * @brief Query iterator.
 */

#include "../../private_api.h"

static
void flecs_query_iter_init(
    ecs_query_run_ctx_t *ctx)
{
    ecs_assert(ctx->written != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_iter_t *it = ctx->it;

    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_flags64_t it_written = it->constrained_vars;
    ctx->written[0] = it_written;
    if (it_written && ctx->query->src_vars) {
        /* If variables were constrained, check if there are any table
         * variables that have a constrained entity variable. */
        ecs_var_t *vars = ctx->vars;
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
            tvar->range = flecs_range_from_entity(vars[var_id].entity, ctx);
            ctx->written[0] |= (1ull << var->table_id); /* Mark as written */
        }
    }

    ecs_flags32_t flags = q->flags;
    ecs_query_cache_t *cache = query->cache;
    if (flags & EcsQueryIsTrivial && !cache) {
        if ((flags & EcsQueryMatchOnlySelf)) {
            if (it_written) {
                flecs_query_set_iter_this(it, ctx);

                if (flags & EcsQueryMatchWildcards) {
                    it->flags |= EcsIterTrivialTestWildcard;
                    flecs_query_setids(&query->ops[0], false, ctx);
                } else {
                    it->flags |= EcsIterTrivialTest;
                    flecs_query_setids(&query->ops[0], false, ctx);
                }
            } else {
                if (flags & EcsQueryMatchWildcards) {
                    it->flags |= EcsIterTrivialSearchWildcard;
                    flecs_query_setids(&query->ops[0], false, ctx);
                } else if (flags & EcsQueryNoData) {
                    it->flags |= EcsIterTrivialSearchNoData;
                    flecs_query_setids(&query->ops[0], false, ctx);
                } else {
                    it->flags |= EcsIterTrivialSearch;
                    flecs_query_setids(&query->ops[0], false, ctx);
                }
            }
        }
    }

    if (cache) {
        cache->prev_match_count = cache->match_count;
    }

    flecs_iter_validate(it);
}

bool flecs_query_next_instanced(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, qit->query);
    ecs_query_run_ctx_t ctx;
    ctx.world = it->real_world;
    ctx.query = impl;
    ctx.it = it;
    ctx.vars = qit->vars;
    ctx.query_vars = qit->query_vars;
    ctx.written = qit->written;
    ctx.op_ctx = qit->op_ctx;
    ctx.qit = qit;
    const ecs_query_op_t *ops = qit->ops;

    bool redo = true;
    if (!(it->flags & EcsIterIsValid)) {
        ecs_assert(impl != NULL, ECS_INVALID_PARAMETER, NULL);
        flecs_query_iter_init(&ctx);
        redo = false;
    } else {
        it->frame_offset += it->count;

        if (!(it->flags & EcsIterSkip)) {
            flecs_query_mark_fields_dirty(impl, it);
            if (qit->prev) {
                if (ctx.query->pub.flags & EcsQueryHasMonitor) {
                    flecs_query_sync_match_monitor(impl, qit->prev);
                }
            }
        }

        it->flags &= ~EcsIterSkip;
    }

    /* Specialized iterator modes for trivial queries */
    if (it->flags & EcsIterTrivialSearch) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.query->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search(ctx.query, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialSearchNoData) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.query->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search_nodata(ctx.query, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialSearchWildcard) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.query->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search_w_wildcards(ctx.query, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialTest) {
        int32_t fields = ctx.query->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_test(ctx.query, &ctx, !redo, mask)) {
            goto done;
        }
        return true;
    } else if (it->flags & EcsIterTrivialTestWildcard) {
        int32_t fields = ctx.query->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_test_w_wildcards(ctx.query, &ctx, !redo, mask)) {
            goto done;
        }
        return true;
    }

    /* Default iterator mode */
    if (flecs_query_run_until(redo, &ctx, ops, -1, qit->op, impl->op_count - 1)) {
        ecs_assert(ops[ctx.op_index].kind == EcsQueryYield, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_query_set_iter_this(it, &ctx);
        ecs_assert(it->count >= 0, ECS_INTERNAL_ERROR, NULL);
        qit->op = flecs_itolbl(ctx.op_index - 1);
        return true;
    }

done:
    flecs_query_mark_fixed_fields_dirty(impl, it);
    if (ctx.query->monitor) {
        flecs_query_update_fixed_monitor(
            ECS_CONST_CAST(ecs_query_impl_t*, ctx.query));
    }

    ecs_iter_fini(it);
    return false;
}

bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, flecs_query_next_instanced(it));
error:
    return false;
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

    int32_t i, var_count = impl->var_count, op_count = impl->op_count;
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
    it.query = q;
    it.system = q->entity;
    it.next = ecs_query_next;
    it.fini = flecs_query_iter_fini;
    it.field_count = q->field_count;
    it.sizes = q->sizes;
    it.set_fields = q->set_fields;
    it.up_fields = 0;
    flecs_query_apply_iter_flags(&it, q);

    flecs_iter_init(it.world, &it, 
        flecs_iter_cache_ids |
        flecs_iter_cache_columns |
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
