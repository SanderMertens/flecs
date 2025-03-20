/**
 * @file query/engine/eval_sparse.c
 * @brief Sparse component evaluation.
 */

#include "../../private_api.h"

bool flecs_query_sparse_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_mask)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        ecs_component_record_t *cdr = flecs_components_get(ctx->world, id);
        ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);

        op_ctx->sparse = cdr->sparse;
        if (!op_ctx->sparse) {
            return false;
        }

        int32_t count = flecs_sparse_count(op_ctx->sparse);
        if (!count) {
            return false;
        }

        op_ctx->cur = count;
    }

next:
    if (!(op_ctx->cur--)) {
        return false;
    }

    ecs_assert(op_ctx->cur >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(op_ctx->cur < flecs_sparse_count(op_ctx->sparse), 
        ECS_INVALID_OPERATION, "sparse iterator invalidated while iterating");

    ecs_entity_t e = flecs_sparse_ids(op_ctx->sparse)[op_ctx->cur];
    ecs_table_range_t range = flecs_range_from_entity(e, ctx);

    if (flecs_query_table_filter(range.table, op->other, table_mask)) {
        goto next;
    }

    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);
    flecs_query_set_vars(op, it->ids[field_index], ctx);

    return true;
}

bool flecs_query_sparse_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
    bool is_var = op->flags & (EcsQueryIsVar << EcsQuerySrc);

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        ecs_component_record_t *cdr = flecs_components_get(ctx->world, id);
        ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);

        op_ctx->sparse = cdr->sparse;
        if (!op_ctx->sparse) {
            return false;
        }

        ecs_table_range_t range = flecs_query_get_range(
            op, &op->src, EcsQuerySrc, ctx);
        if (!range.count) {
            range.count = ecs_table_count(range.table);
        }

        op_ctx->range = range;
        op_ctx->cur = range.offset - 1;
    }

    int32_t end = op_ctx->range.count + op_ctx->range.offset;

    do {
        op_ctx->cur ++;
        ecs_assert(op_ctx->cur <= end, ECS_INTERNAL_ERROR, NULL);

        if (op_ctx->cur == end) {
            /* Restore range */
            if (is_var) {
                flecs_query_var_narrow_range(op->src.var, op_ctx->range.table, 
                    op_ctx->range.offset, op_ctx->range.count, ctx);
            }
            return false;
        }

        ecs_entity_t e = ecs_table_entities(op_ctx->range.table)[op_ctx->cur];
        bool result = flecs_sparse_get_any(op_ctx->sparse, 0, e) != NULL;

        if (not) {
            result = !result;
        }

        if (result) {
            break;
        }
    } while (true);

    if (is_var) {
        flecs_query_var_narrow_range(op->src.var, op_ctx->range.table, 
            op_ctx->cur, 1, ctx);
    }

    return true;
}

bool flecs_query_sparse_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (!redo) {
            /* Can use regular up traversal since sparse components are resolved
             * by the traversal cache */
            if (!flecs_query_up_with(op, redo, ctx)) {
                return false;
            }

            return true;
        } else {
            return false;
        }
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectUp, FlecsQueryUpSelectSparse);
    }
}

bool flecs_query_sparse_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
        bool is_var = op->flags & (EcsQueryIsVar << EcsQuerySrc);

        if (!redo) {
            op_ctx->self = true;
        }

        if (op_ctx->self) {
            bool result = flecs_query_sparse_with(op, redo, ctx, false);
            if (result) {
                return true;
            }

            op_ctx->self = false;

            if (!flecs_query_up_with(op, false, ctx)) {
                return false;
            }

            ecs_table_range_t range = flecs_query_get_range(
                op, &op->src, EcsQuerySrc, ctx);
            if (!range.count) {
                range.count = ecs_table_count(range.table);
            }
    
            op_ctx->range = range;
            op_ctx->cur = range.offset - 1;
        }

        int32_t end = op_ctx->range.count + op_ctx->range.offset;

next:
        op_ctx->cur ++;

        if (op_ctx->cur == end) {
            /* Restore range */
            if (is_var) {
                flecs_query_var_narrow_range(op->src.var, op_ctx->range.table, 
                    op_ctx->range.offset, op_ctx->range.count, ctx);
            }
            return false;
        }

        ecs_entity_t e = ecs_table_entities(op_ctx->range.table)[op_ctx->cur];
        if (flecs_sparse_get_any(op_ctx->sparse, 0, e)) {
            /* We've already returned entities that have the component, so 
             * filter those out */
            goto next;
        }

        if (is_var) {
            flecs_query_var_narrow_range(op->src.var, op_ctx->range.table, 
                op_ctx->cur, 1, ctx);
        }

        return true;
    } else {
        ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
        bool is_var = op->flags & (EcsQueryIsVar << EcsQuerySrc);

        if (!redo) {
            op_ctx->self = true;
        }

        bool up_redo = true;

        if (op_ctx->self) {
            if (flecs_query_sparse_select(op, redo, ctx, 
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))) 
            {
                return true;
            }

            op_ctx->self = false;
            up_redo = false;

            goto next_table;
        }

next_entity: {
            int32_t end = op_ctx->range.count + op_ctx->range.offset;

            do {
                op_ctx->cur ++;
                ecs_assert(op_ctx->cur <= end, ECS_INTERNAL_ERROR, NULL);
        
                if (op_ctx->cur == end) {
                    op_ctx->cur = op_ctx->prev_cur;
                    op_ctx->range = op_ctx->prev_range;
                    goto next_table;
                }
        
                ecs_entity_t e = ecs_table_entities(op_ctx->range.table)[op_ctx->cur];
                if (!flecs_sparse_get_any(op_ctx->sparse, 0, e)) {
                    /* Filter out entities that have the component */
                    break;
                }
            } while (true);
        
            if (is_var) {
                flecs_query_var_narrow_range(op->src.var, op_ctx->range.table, 
                    op_ctx->cur, 1, ctx);
            }

            return true;
        }

next_table: {
            if (!flecs_query_up_select(op, up_redo, ctx, 
                FlecsQueryUpSelectUp, FlecsQueryUpSelectSparse))
            {
                return false;
            }

            ecs_table_range_t range = flecs_query_get_range(
                op, &op->src, EcsQuerySrc, ctx);
            if (!range.count) {
                range.count = ecs_table_count(range.table);
            }

            op_ctx->prev_cur = op_ctx->cur;
            op_ctx->prev_range = op_ctx->range;

            op_ctx->range = range;
            op_ctx->cur = range.offset - 1;

            goto next_entity;
        }
    }
}

bool flecs_query_sparse(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_sparse_with(op, redo, ctx, false);
    } else {
        return flecs_query_sparse_select(op, redo, ctx,
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
    }
}
