/**
 * @file query/engine/eval_populate.c
 * @brief Populate data fields.
 */

#include "../../private_api.h"

void flecs_query_populate_field_from_range(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    const ecs_table_record_t *tr)
{
    ecs_assert(range->table != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t column = tr->column;
    if (column == -1) {
        return;
    }

    if (range->count) {
        ecs_assert(tr->hdr.table == range->table, ECS_INTERNAL_ERROR, NULL);
        it->ptrs[field_index] = ECS_ELEM(
            range->table->data.columns[column].data.array,
            it->sizes[field_index],
            range->offset);
    }
}

void flecs_query_populate_shared_field(
    ecs_iter_t *it,
    ecs_entity_t src,
    int8_t field_index,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_record_t *r = flecs_entities_get(ctx->world, src);
    ecs_table_t *src_table = r->table;
    const ecs_table_record_t *tr = it->trs[field_index];
    if (tr) {
        ecs_assert(tr->column < src_table->column_count, 
            ECS_INTERNAL_ERROR, NULL);
        int32_t column = tr->column;
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);

        ecs_assert(tr->hdr.table == src_table, ECS_INTERNAL_ERROR, NULL);
        it->ptrs[field_index] = ecs_vec_get(
            &src_table->data.columns[column].data,
            it->sizes[field_index],
            ECS_RECORD_TO_ROW(r->row));

        ECS_TERMSET_SET(it->shared_fields, 1u << field_index);
    }
}

static
void flecs_query_populate_field(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    ecs_query_run_ctx_t *ctx)
{
    ecs_assert(ecs_field_is_set(it, field_index), ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t src = it->sources[field_index];
    if (!src) {
        const ecs_table_record_t *tr = it->trs[field_index];
        flecs_query_populate_field_from_range(it, range, field_index, tr);
        ECS_TERMSET_CLEAR(it->shared_fields, 1u << field_index);
    } else {
        flecs_query_populate_shared_field(it, src, field_index, ctx);
    }
}

bool flecs_query_populate(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    if (!redo) {
        ecs_iter_t *it = ctx->it;
        if (it->flags & EcsIterNoData) {
            return true;
        }

        const ecs_query_impl_t *query = ctx->query;
        const ecs_query_t *q = &query->pub;
        int8_t i, field_count = q->field_count;
        ecs_flags64_t data_fields = op->src.entity; /* Bitset with fields to set */
        ecs_table_range_t *range = &ctx->vars[0].range;
        ecs_table_t *table = range->table;
        if (table && !range->count) {
            range->count = ecs_table_count(table);
        }

        /* Only populate fields that are set */
        data_fields &= it->set_fields;
        for (i = 0; i < field_count; i ++) {
            if (!(data_fields & (1llu << i))) {
                continue;
            }

            flecs_query_populate_field(it, range, i, ctx);
        }

        return true;
    } else {
        return false;
    }
}

bool flecs_query_populate_self(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    if (!redo) {
        const ecs_query_impl_t *query = ctx->query;
        const ecs_query_t *q = &query->pub;
        int32_t i, field_count = q->field_count;
        ecs_flags64_t data_fields = op->src.entity; /* Bitset with fields to set */
        ecs_iter_t *it = ctx->it;

        ecs_table_range_t *range = &ctx->vars[0].range;
        ecs_table_t *table = range->table;
        if (!table->column_map) {
            return true;
        }

        if (!ecs_table_count(table)) {
            return true;
        }

        /* Only populate fields that can be set */
        data_fields &= it->set_fields;
        for (i = 0; i < field_count; i ++) {
            if (!(data_fields & (1llu << i))) {
                continue;
            }

            ecs_assert(it->trs[i] != NULL, ECS_INTERNAL_ERROR, NULL);

            int32_t column = it->trs[i]->column;
            if (column != -1) {
                it->ptrs[i] = ECS_ELEM(
                    table->data.columns[column].data.array,
                    it->sizes[i],
                    range->offset);
            }
        }

        return true;
    } else {
        return false;
    }
}

bool flecs_query_populate_sparse(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    if (!redo) {
        ecs_iter_t *it = ctx->it;
        if (it->flags & EcsIterNoData) {
            return true;
        }

        ecs_world_t *world = ctx->world;
        const ecs_query_impl_t *query = ctx->query;
        const ecs_query_t *q = &query->pub;
        int8_t i, field_count = q->field_count;
        ecs_flags64_t data_fields = op->src.entity; /* Bitset with fields to set */

        /* Only populate fields that are set */
        data_fields &= it->set_fields;
        for (i = 0; i < field_count; i ++) {
            if (!(data_fields & (1llu << i))) {
                continue;
            }

            ecs_entity_t src = it->sources[i];
            if (!src) {
                src = ctx->vars[0].entity;
            }

            ecs_id_record_t *idr = flecs_id_record_get(world, it->ids[i]);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            it->ptrs[i] = flecs_sparse_get_any(idr->sparse, 0, src);
        }

        return true;
    } else {
        return false;
    }
}
