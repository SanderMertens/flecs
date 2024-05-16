/**
 * @file query/engine/cache_iter.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

static
ecs_query_cache_table_match_t* flecs_query_next(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_cache_table_match_t *node = qit->node;
    ecs_query_cache_table_match_t *prev = qit->prev;

    if (prev != qit->last) {
        ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
        ctx->vars[0].range.table = node->table;
        it->group_id = node->group_id;
        it->instance_count = 0;
        qit->node = node->next;
        qit->prev = node;
        return node;
    }

    return NULL;
}

static
ecs_query_cache_table_match_t* flecs_query_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first)
{
    ecs_iter_t *it = ctx->it;
    if (first) {
        ecs_var_t *var = &ctx->vars[0];
        ecs_table_t *table = var->range.table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION, 
            "the variable set on the iterator is missing a table");

        ecs_query_cache_table_t *qt = flecs_query_cache_get_table(
            impl->cache, table);
        if (!qt) {
            return NULL;
        }

        ecs_query_iter_t *qit = &it->priv_.iter.query;
        qit->prev = NULL;
        qit->node = qt->first;
        qit->last = qt->last;
    }

    return flecs_query_next(ctx);
}

static
void flecs_query_populate_ptrs(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    ecs_query_cache_table_match_t *node)
{
    int32_t i, field_count = it->field_count;
    ecs_data_t *data = &table->data;
    for (i = 0; i < field_count; i ++) {
        ECS_TERMSET_CLEAR(it->shared_fields, 1u << i);

        int32_t storage_column = node->storage_columns[i];
        ecs_size_t size = it->sizes[i];
        if (storage_column < 0 || !size) {
            /* Tag / no data */
            it->ptrs[i] = NULL;
            continue;
        }

        it->ptrs[i] = ecs_vec_get(&data->columns[storage_column].data,
            it->sizes[i], offset);
    }
}

static
void flecs_query_populate_ptrs_w_field_map(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    ecs_query_cache_table_match_t *node,
    int8_t *field_map,
    int32_t field_count)
{
    int32_t i;
    ecs_data_t *data = &table->data;
    for (i = 0; i < field_count; i ++) {
        int32_t storage_column = node->storage_columns[i];
        int8_t field_index = field_map[i];
        ecs_size_t size = it->sizes[field_index];
        if (storage_column < 0 || !size) {
            /* Tag / no data */
            it->ptrs[field_index] = NULL;
            continue;
        }

        it->ptrs[field_index] = ecs_vec_get(&data->columns[storage_column].data,
            size, offset);
    }
}

static
void flecs_query_populate_ptrs_w_shared(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    ecs_query_cache_table_match_t *node,
    int8_t *field_map,
    int8_t field_count)
{
    ecs_ref_t *refs = ecs_vec_first(&node->refs);
    int8_t i;
    for (i = 0; i < field_count; i ++) {
        int32_t column = node->columns[i];
        int8_t field_index = i;
        if (field_map) {
            field_index = field_map[i];
        }

        ecs_size_t size = it->sizes[field_index];
        if (!ecs_field_is_set(it, i) || !size) {
            /* Tag / no data */
            it->ptrs[field_index] = NULL;
            continue;
        }

        ecs_entity_t src = node->sources[i];
        if (src != 0) {
            ecs_ref_t *ref = &refs[column];
            if (ref->id) {
                it->ptrs[field_index] = (void*)ecs_ref_get_id(
                    it->real_world, ref, ref->id);
                ECS_TERMSET_SET(it->shared_fields, 1u << i);
            } else {
                it->ptrs[field_index] = NULL;
            }
        } else {
            int32_t storage_column = node->storage_columns[i];
            if (storage_column < 0) {
                it->ptrs[field_index] = NULL;
                continue;
            }
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
            it->ptrs[field_index] = ecs_vec_get(
                &table->data.columns[storage_column].data, size, offset);
            ECS_TERMSET_CLEAR(it->shared_fields, 1u << i);
        }
    }
}

/* Populate for query that is partially cached.
 * This requires a mapping from cached fields to query fields. */
static
void flecs_query_cache_data_populate(
    const ecs_query_impl_t *impl,
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_table_match_t *node)
{
    ecs_table_t *table = node->table;
    ecs_var_t *var = &ctx->vars[0];
    int32_t count = var->range.count, offset = var->range.offset;

    if (!count) {
        count = ecs_table_count(table);
    }

    /* If NoData flag is set on iterator, don't populate fields */
    if (ECS_BIT_IS_SET(it->flags, EcsIterNoData) || !count) {
        return;
    }

    ecs_query_t *cache_query = impl->cache->query;
    if (!ecs_vec_count(&node->refs)) {
        flecs_query_populate_ptrs_w_field_map(
            it, table, offset, node, impl->field_map, cache_query->field_count);
    } else {
        flecs_query_populate_ptrs_w_shared(
            it, table, offset, node, impl->field_map, cache_query->field_count);
    }
}

/* Populate for query that is entirely cached.
 * Cached fields the same as query fields. */
static
void flecs_query_is_cache_data_populate(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_table_match_t *node)
{
    ecs_table_t *table = node->table;
    ecs_var_t *var = &ctx->vars[0];
    int32_t count = var->range.count, offset = var->range.offset;

    if (!count) {
        count = ecs_table_count(table);
    }

    /* If NoData flag is set on iterator, don't populate fields */
    if (ECS_BIT_IS_SET(it->flags, EcsIterNoData) || !count) {
        return;
    }

    if (!ecs_vec_count(&node->refs)) {
        flecs_query_populate_ptrs(it, table, offset, node);
    } else {
        flecs_query_populate_ptrs_w_shared(it, table, offset, node, NULL, 
            flecs_ito(int8_t, it->field_count));
    }
}

static
void flecs_query_cache_init_mapped_fields(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_table_match_t *node)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_cache_t *cache = impl->cache;
    int32_t i, field_count = cache->query->field_count;
    int8_t *field_map = impl->field_map;

    for (i = 0; i < field_count; i ++) {
        int8_t field_index = field_map[i];
        it->columns[field_index] = node->columns[i];
        it->ids[field_index] = node->ids[i];
        it->sources[field_index] = node->sources[i];

        ecs_termset_t bit = (ecs_termset_t)(1u << i);
        ecs_termset_t field_bit = (ecs_termset_t)(1u << field_index);

        ECS_TERMSET_COND(it->set_fields, field_bit, node->set_fields & bit);
        ECS_TERMSET_COND(it->up_fields, field_bit, node->up_fields & bit);
    }
}

/* Iterate cache for query that's partially cached */
bool flecs_query_cache_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_cache_table_match_t *node = flecs_query_next(ctx);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(impl, ctx, node);

    ctx->vars[0].range.count = node->count;
    ctx->vars[0].range.offset = node->offset;
    return true;
}

/* Iterate cache for query that's entirely cached */
bool flecs_query_is_cache_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx)
{
    (void)impl;

    ecs_query_cache_table_match_t *node = flecs_query_next(ctx);
    if (!node) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    it->columns = node->columns;
    it->ids = node->ids;
    it->sources = node->sources;
    it->set_fields = node->set_fields;
    it->up_fields = node->up_fields;

    ctx->vars[0].range.count = node->count;
    ctx->vars[0].range.offset = node->offset;
    return true;
}

/* Iterate cache for query that's partially cached with data */
bool flecs_query_cache_data_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx)
{
    if (!flecs_query_cache_search(impl, ctx)) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    flecs_query_cache_data_populate(impl, it, ctx, qit->prev);
    return true;
}

/* Iterate cache for query that's entirely cached with data */
bool flecs_query_is_cache_data_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx)
{
    if (!flecs_query_is_cache_search(impl, ctx)) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    flecs_query_is_cache_data_populate(it, ctx, qit->prev);
    return true;
}

/* Test if query that is entirely cached matches constrained $this */
bool flecs_query_cache_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first)
{
    ecs_query_cache_table_match_t *node = flecs_query_test(impl, ctx, first);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(impl, ctx, node);

    return true;
}

/* Test if query that is entirely cached matches constrained $this */
bool flecs_query_is_cache_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first)
{
    ecs_query_cache_table_match_t *node = flecs_query_test(impl, ctx, first);
    if (!node) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    it->columns = node->columns;
    it->ids = node->ids;
    it->sources = node->sources;

    return true;
}

/* Test if query that is partially cached matches constrained $this */
bool flecs_query_cache_data_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first)
{
    if (!flecs_query_cache_test(impl, ctx, first)) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    flecs_query_cache_data_populate(impl, it, ctx, qit->prev);
    return true;
}

/* Test if query that is entirely cached matches constrained $this with data */
bool flecs_query_is_cache_data_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first)
{
    if (!flecs_query_is_cache_test(impl, ctx, first)) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    flecs_query_is_cache_data_populate(it, ctx, qit->prev);
    return true;
}
