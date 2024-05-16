/**
 * @file query/engine/eval_trav.c
 * @brief Transitive/reflexive relationship traversal.
 */

#include "../../private_api.h"

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
    ecs_entity_t *entities = table->data.entities.array;
    int32_t count = range->count;
    if (!count) {
        count = ecs_table_count(table);
    }

    int32_t i = range->offset, end = i + count;
    for (; i < end; i ++) {
        if (entities[i] == second) {
            /* Even though table doesn't have the specific relationship 
             * pair, the relationship is reflexive and the target entity
             * is stored in the table. */
            break;
        }
    }
    if (i == end) {
        /* Table didn't contain target entity */
        return false;
    }
    if (count > 1) {
        /* If the range contains more than one entity, set the range to
         * return only the entity matched by the reflexive property. */
        ecs_assert(flecs_query_ref_flags(op->flags, EcsQuerySrc) & EcsQueryIsVar, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_var_t *var = &ctx->vars[op->src.var];
        ecs_table_range_t *var_range = &var->range;
        var_range->offset = i;
        var_range->count = 1;
        var->entity = entities[i];
    }

    flecs_query_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

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

    flecs_query_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

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

    /* Check if table has transitive relationship by traversing upwards */
    int32_t column = ecs_search_relation(ctx->world, table, 0, 
        ecs_pair(trav, second), trav, EcsSelf|EcsUp, NULL, NULL, NULL);
    if (column == -1) {
        if (op->match_flags & EcsTermReflexive) {
            return flecs_query_trav_fixed_src_reflexive(op, ctx,
                &range, trav, second);
        } else {
            return false;
        }
    }

    flecs_query_set_trav_match(op, column, trav, second, ctx);
    return true;
}

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

            /* If there's no record for the entity, it can't have a subtree so
             * forward operation to a regular select. */
            return flecs_query_select(op, redo, ctx);
        }

        /* Entity is traversable, which means it could have a subtree */
        flecs_query_get_trav_down_cache(ctx, &trav_ctx->cache, trav, second);
        trav_ctx->index = 0;

        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->index = -1;
            if(flecs_query_trav_unknown_src_reflexive(
                op, ctx, trav, second))
            {
                /* It's possible that we couldn't return the entity required for
                 * reflexive matching, like when it's a prefab or disabled. */
                return true;
            }
        }
    } else {
        if (!trav_ctx->cache.id) {
            /* No traversal cache, which means this is a regular select */
            return flecs_query_select(op, redo, ctx);
        }
    }

    if (trav_ctx->index == -1) {
        redo = false; /* First result after handling reflexive relationship */
        trav_ctx->index = 0;
    }

    /* Forward to select */
    int32_t count = ecs_vec_count(&trav_ctx->cache.entities);
    ecs_trav_elem_t *elems = ecs_vec_first(&trav_ctx->cache.entities);
    for (; trav_ctx->index < count; trav_ctx->index ++) {
        ecs_trav_elem_t *el = &elems[trav_ctx->index];
        trav_ctx->and.idr = el->idr; /* prevents lookup by select */
        if (flecs_query_select_w_id(op, redo, ctx, ecs_pair(trav, el->entity),
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))) 
        {
            return true;
        }
        
        redo = false;
    }

    return false;
}

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
        /* Restore previous offset, count */
        if (src_is_var) {
            ecs_var_id_t src_var = op->src.var;
            vars[src_var].range.offset = offset;
            vars[src_var].range.count = count;
            vars[src_var].entity = 0;
        }
        return false;
    }

    ecs_entity_t entity = ecs_vec_get_t(
        &range->table->data.entities, ecs_entity_t, trav_ctx->index)[0];
    flecs_query_set_trav_match(op, -1, trav, entity, ctx);

    /* Hijack existing variable to return one result at a time */
    if (src_is_var) {
        ecs_var_id_t src_var = op->src.var;
        ecs_table_t *table = vars[src_var].range.table;
        ecs_assert(!table || table == ecs_get_table(ctx->world, entity),
            ECS_INTERNAL_ERROR, NULL);
        (void)table;
        vars[src_var].entity = entity;
        vars[src_var].range = flecs_range_from_entity(entity, ctx);
    }

    return true;
}

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
    flecs_query_set_trav_match(op, el->column, trav, el->entity, ctx);
    return true;
}

bool flecs_query_trav(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];

    if (!flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (!flecs_ref_is_written(op, &op->second, EcsQuerySecond, written)) {
            /* This can't happen, src or second should have been resolved */
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
