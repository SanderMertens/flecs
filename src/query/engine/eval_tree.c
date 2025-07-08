/**
 * @file query/engine/eval_tree.c
 * @brief Hierarchy evaluation (ChildOf pairs/Parent components).
 */

#include "../../private_api.h"

bool flecs_query_tree_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    ecs_table_t *table = range.table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->flags & EcsTableHasChildOf) {
        if (op->match_flags & EcsTermMatchAny) {
            return flecs_query_and_any(op, redo, ctx);
        } else {
            return flecs_query_with(op, redo, ctx);
        }
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_assert(ECS_IS_PAIR(id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);

    if (!(table->flags & EcsTableHasParent)) {
        if (!tgt) {
            int8_t field_index = op->field_index;
            it->set_fields &= ~(1u << field_index);
            it->ids[op->field_index] = ecs_childof(EcsWildcard);
            return true;
        }
        return false;
    }

    int32_t parent_column = range.table->component_map[ecs_id(EcsParent)];
    ecs_assert(parent_column != -1, ECS_INTERNAL_ERROR, NULL);
    EcsParent *parents = ecs_table_get_column(
        range.table, parent_column - 1, range.offset);

    if (range.count == 1) {
        ecs_entity_t parent = parents[0].value;
        if (tgt == EcsWildcard) {
            if (op->match_flags & EcsTermMatchAny) {
                it->ids[op->field_index] = ecs_childof(EcsWildcard);
            } else {
                ecs_id_t actual_id = it->ids[op->field_index] = 
                    ecs_childof(parent);
                flecs_query_set_vars(op, actual_id, ctx);
            }
        } else {
            if ((uint32_t)parent != tgt) {
                return false;
            }

            ecs_id_t actual_id = it->ids[op->field_index] = 
                ecs_childof(parent);
            flecs_query_set_vars(op, actual_id, ctx);
        }     
    } else {
        ecs_abort(ECS_UNSUPPORTED, NULL);
    }

    return true;
}
