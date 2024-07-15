/**
 * @file query/engine/eval_member.c
 * @brief Component member evaluation.
 */

#include "../../private_api.h"

static
bool flecs_query_member_cmp(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    bool neq)
{
    ecs_table_range_t range;
    if (op->other) {
        ecs_var_id_t table_var = flecs_itovar(op->other - 1);
        range = flecs_query_var_get_range(table_var, ctx);
    } else {
        range = flecs_query_get_range(op, &op->src, EcsQuerySrc, ctx);
    }

    ecs_table_t *table = range.table;
    if (!table) {
        return false;
    }

    ecs_query_membereq_ctx_t *op_ctx = flecs_op_ctx(ctx, membereq);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!range.count) {
        range.count = ecs_table_count(range.table); 
    }

    int32_t row, end = range.count;
    if (end) {
        end += range.offset;
    } else {
        end = ecs_table_count(range.table);
    }

    void *data;
    if (!redo) {
        row = op_ctx->each.row = range.offset;

        /* Get data ptr starting from offset 0 so we can use row to index */
        range.offset = 0;

        /* Populate data field so we have the array we can compare the member
         * value against. */
        void *old_data = it->ptrs[field_index];
        flecs_query_populate_field_from_range(
            it, &range, field_index, it->columns[field_index]);
        data = op_ctx->data = it->ptrs[field_index];

        /* Ensure we only write ptrs when we match data */
        it->ptrs[field_index] = old_data;

        /* Member fields are of type ecs_entity_t */
        #ifdef FLECS_META
        it->ids[field_index] = ecs_id(ecs_entity_t);
        #else
        it->ids[field_index] = 0;
        #endif
    } else {
        row = ++ op_ctx->each.row;
        if (op_ctx->each.row >= end) {
            return false;
        }

        data = op_ctx->data;
    }

    int32_t offset = (int32_t)op->first.entity;
    int32_t size = (int32_t)(op->first.entity >> 32);
    ecs_entity_t *entities = table->data.entities.array;
    ecs_entity_t e = 0;
    ecs_entity_t *val;

    ecs_assert(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL); /* Must be written */
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);

    bool second_written = true;
    if (op->flags & (EcsQueryIsVar << EcsQuerySecond)) {
        uint64_t written = ctx->written[ctx->op_index];
        second_written = written & (1ull << op->second.var);
    }

    if (second_written) {
        ecs_flags16_t second_flags = flecs_query_ref_flags(
            op->flags, EcsQuerySecond);
        ecs_entity_t second = flecs_get_ref_entity(
            &op->second, second_flags, ctx);

        do {
            e = entities[row];

            val = ECS_OFFSET(ECS_ELEM(data, size, row), offset);
            if (val[0] == second || second == EcsWildcard) {
                if (!neq) {
                    goto match;
                }
            } else {
                if (neq) {
                    goto match;
                }
            }

            row ++;
        } while (row < end);

        return false;
    } else {
        e = entities[row];
        val = ECS_OFFSET(ECS_ELEM(data, size, row), offset);
        flecs_query_var_set_entity(op, op->second.var, val[0], ctx);
    }

match:
    if (op->other) {
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
        flecs_query_var_set_entity(op, op->src.var, e, ctx);
    }

    it->ptrs[field_index] = val;
    op_ctx->each.row = row;

    return true;
}

bool flecs_query_member_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_member_cmp(op, redo, ctx, false);
}

bool flecs_query_member_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_member_cmp(op, redo, ctx, true);
}
