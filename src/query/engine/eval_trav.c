/**
 * @file query/engine/eval_trav.c
 * @brief Transitive/reflexive relationship traversal (EcsQueryTrav instruction).
 *
 * Reflexive: entity matches if it equals the target. Transitive: walks the
 * relationship graph to find indirect matches.
 */

#include "../../private_api.h"

/* Check reflexive relationship match for a fixed source entity. */
static
bool flecs_query_trav_fixed_src_reflexive(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_table_t *table = range->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_entity_t *entities = ecs_table_entities(table);
    int32_t count = range->count;
    if (!count) {
        count = ecs_table_count(table);
    }

    int32_t i = range->offset, end = i + count;
    for (; i < end; i ++) {
        if (entities[i] == second) {
            /* Reflexive match: target entity found in table */
            break;
        }
    }
    if (i == end) {
        return false;
    }
    if (count > 1) {
        /* Narrow range to the single entity matched by reflexive property */
        ecs_assert(flecs_query_ref_flags(op->flags, EcsQuerySrc) & EcsQueryIsVar, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_var_t *var = &ctx->vars[op->src.var];
        ecs_table_range_t *var_range = &var->range;
        var_range->offset = i;
        var_range->count = 1;
        var->entity = entities[i];
    }

    flecs_query_set_trav_match(op, NULL, trav, second, ctx);
    return true;
}

/* Check reflexive relationship match for an unknown source entity. */
static
bool flecs_query_trav_unknown_src_reflexive(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_assert(flecs_query_ref_flags(op->flags, EcsQuerySrc) & EcsQueryIsVar,
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_id_t src_var = op->src.var;
    flecs_query_var_set_entity(op, src_var, second, ctx);
    flecs_query_var_get_table(src_var, ctx);

    ecs_table_t *table = ctx->vars[src_var].range.table;
    if (table) {
        if (flecs_query_table_filter(table, op->other, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
        {
            return false;
        }
    }

    flecs_query_set_trav_match(op, NULL, trav, second, ctx);
    return true;
}

/* Traverse upward from a fixed source to match a fixed second entity. */
static
bool flecs_query_trav_fixed_src_up_fixed_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false; /* If everything's fixed, can only have a single result */
    }

    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t f_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);
    ecs_flags16_t f_src = flecs_query_ref_flags(op->flags, EcsQuerySrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;

    ecs_table_record_t *tr = NULL;
    ecs_search_relation(ctx->world, table, 0, 
        ecs_pair(trav, second), trav, EcsSelf|EcsUp, NULL, NULL, &tr);

    if (!tr) {
        if (op->match_flags & EcsTermReflexive) {
            return flecs_query_trav_fixed_src_reflexive(op, ctx,
                &range, trav, second);
        } else {
            return false;
        }
    }

    flecs_query_set_trav_match(op, tr, trav, second, ctx);
    return true;
}

/* Find sources that transitively reach a fixed second entity. Builds a
 * down-cache from the target, then selects tables with each intermediate pair. */
static
bool flecs_query_trav_unknown_src_up_fixed_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t f_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        ecs_record_t *r_second = flecs_entities_get(ctx->world, second);
        bool traversable = r_second && r_second->row & EcsEntityIsTraversable;
        bool reflexive = op->match_flags & EcsTermReflexive;
        if (!traversable && !reflexive) {
            trav_ctx->cache.id = 0;

            /* Non-traversable entity: fall back to regular select */
            return flecs_query_select(op, redo, ctx);
        }

        flecs_query_get_trav_down_cache(ctx, &trav_ctx->cache, trav, second);
        trav_ctx->index = 0;

        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->index = -1; /* Sentinel: reflexive result pending */
            if(flecs_query_trav_unknown_src_reflexive(
                op, ctx, trav, second))
            {
                return true;
            }
        }
    } else {
        if (!trav_ctx->cache.id) {
            return flecs_query_select(op, redo, ctx);
        }
    }

    if (trav_ctx->index == -1) {
        redo = false; /* First result after handling reflexive relationship */
        trav_ctx->index = 0;
    }

    int32_t count = ecs_vec_count(&trav_ctx->cache.entities);
    ecs_trav_elem_t *elems = ecs_vec_first(&trav_ctx->cache.entities);
    for (; trav_ctx->index < count; trav_ctx->index ++) {
        ecs_trav_elem_t *el = &elems[trav_ctx->index];
        trav_ctx->and.cr = el->cr; /* prevents lookup by select */
        if (flecs_query_select_w_id(op, redo, ctx, ecs_pair(trav, el->entity),
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))) 
        {
            return true;
        }
        
        redo = false;
    }

    return false;
}

/* Yield the next reflexive source entity from a range during traversal. */
static
bool flecs_query_trav_yield_reflexive_src(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav)
{
    ecs_var_t *vars = ctx->vars;
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);
    int32_t offset = trav_ctx->offset, count = trav_ctx->count;
    bool src_is_var = op->flags & (EcsQueryIsVar << EcsQuerySrc);

    if (trav_ctx->index >= (offset + count)) {
        if (src_is_var) {
            ecs_var_id_t src_var = op->src.var;
            vars[src_var].range.offset = offset;
            vars[src_var].range.count = count;
            vars[src_var].entity = 0;
        }
        return false;
    }

    ecs_entity_t entity = ecs_table_entities(range->table)[trav_ctx->index];
    flecs_query_set_trav_match(op, NULL, trav, entity, ctx);

    /* Hijack existing variable to return one result at a time */
    if (src_is_var) {
        ecs_var_id_t src_var = op->src.var;
        ecs_table_t *table = vars[src_var].range.table;
        ecs_assert(!table || table == ecs_get_table(ctx->world, entity),
            ECS_INTERNAL_ERROR, NULL);
        (void)table;
        vars[src_var].entity = entity;
        vars[src_var].range = flecs_range_from_entity(ctx->world, entity);
    }

    return true;
}

/* Walk up the relationship graph from a fixed source to discover all
 * transitively reachable target entities. Yields reflexive matches first. */
static
bool flecs_query_trav_fixed_src_up_unknown_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t f_src = flecs_query_ref_flags(op->flags, EcsQuerySrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        flecs_query_get_trav_up_cache(ctx, &trav_ctx->cache, trav, table);
        trav_ctx->index = 0;
        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->yield_reflexive = true;
            trav_ctx->index = range.offset;
            trav_ctx->offset = range.offset;
            trav_ctx->count = range.count ? range.count : ecs_table_count(table);
        }
    } else {
        trav_ctx->index ++;
    }

    if (trav_ctx->yield_reflexive) {
        if (flecs_query_trav_yield_reflexive_src(op, ctx, &range, trav)) {
            return true;
        }
        trav_ctx->yield_reflexive = false;
        trav_ctx->index = 0;
    }

    if (trav_ctx->index >= ecs_vec_count(&trav_ctx->cache.entities)) {
        return false;
    }

    ecs_trav_elem_t *el = ecs_vec_get_t(
        &trav_ctx->cache.entities, ecs_trav_elem_t, trav_ctx->index);
    flecs_query_set_trav_match(op, el->tr, trav, el->entity, ctx);
    return true;
}

/* Evaluate a transitive/reflexive traversal operation. */
bool flecs_query_trav(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];

    if (!flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (!flecs_ref_is_written(op, &op->second, EcsQuerySecond, written)) {
            ecs_abort(ECS_INTERNAL_ERROR,
                "invalid instruction sequence: unconstrained traversal");
        } else {
            return flecs_query_trav_unknown_src_up_fixed_second(op, redo, ctx);
        }
    } else {
        if (!flecs_ref_is_written(op, &op->second, EcsQuerySecond, written)) {
            return flecs_query_trav_fixed_src_up_unknown_second(op, redo, ctx);
        } else {
            return flecs_query_trav_fixed_src_up_fixed_second(op, redo, ctx);
        }
    }
}
