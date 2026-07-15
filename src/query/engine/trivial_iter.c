/**
 * @file query/engine/trivial_iter.c
 * @brief Iterator for trivial queries.
 */

#include "../../private_api.h"

void flecs_query_trivial_set_ids(
    ecs_query_run_ctx_t *ctx)
{
    const ecs_query_t *q = &ctx->query->pub;
    ecs_iter_t *it = ctx->it;

    int32_t i;
    for (i = 0; i < q->term_count; i ++) {
        const ecs_term_t *term = &q->terms[i];
        it->ids[term->field_index] = term->id;
    }
}

static bool flecs_query_trivial_search_init(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    const ecs_query_t *query,
    bool redo,
    ecs_flags64_t term_set)
{
    if (!redo) {
        /* Find first trivial term */
        int32_t t = 0;
        if (term_set) {
            for (; t < query->term_count; t ++) {
                if (term_set & (1llu << t)) {
                    break;
                }
            }
        }

        ecs_assert(t != query->term_count, ECS_INTERNAL_ERROR, NULL);
        op_ctx->start_from = t;

        ecs_component_record_t *cr = flecs_components_get(ctx->world, query->terms[t].id);
        if (!cr) {
            return false;
        }

        if (query->flags & EcsQueryMatchEmptyTables) {
            if (!flecs_table_cache_queryable_iter(&cr->cache, &op_ctx->it, 
                EcsTableEmpty|EcsTableNotEmpty))
            {
                return false;
            }
        } else {
            if (!flecs_table_cache_queryable_iter(&cr->cache, &op_ctx->it, 
                EcsTableNotEmpty)) 
            {
                return false;
            }
        }

        /* Find next term to evaluate once */
        for (t = t + 1; t < query->term_count; t ++) {
            if (term_set & (1llu << t)) {
                break;
            }
        }

        op_ctx->first_to_eval = t;

        for (t = 0; t < query->term_count; t++) {
            if (term_set && !(term_set & (1llu << t))) {
                continue;
            }

            cr = flecs_query_impl(query)->cr_cache[t] = flecs_components_get(
                ctx->world, query->terms[t].id);
            if (!cr) {
                return false;
            }
        }
    }

    return true;
}

bool flecs_query_is_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo)
{
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_iter_t *it = ctx->it;
    int32_t t, term_count = query->pub.term_count;

    if (!flecs_query_trivial_search_init(ctx, op_ctx, q, redo, 0)) {
        return false;
    }

    uint64_t q_filter = q->bloom_filter;
    ecs_component_record_t **cr_cache = query->cr_cache;

next:
    {
        const ecs_table_cache_elem_t *elem = flecs_table_cache_next(
            &op_ctx->it);
        if (!elem) {
            return false;
        }

        ecs_table_t *table = elem->table;
        if (!flecs_table_bloom_filter_test(table, q_filter)) {
            goto next;
        }

        int16_t *columns = ECS_CONST_CAST(int16_t*, it->columns);
        for (t = 1; t < term_count; t ++) {
            ecs_component_record_t *cr = cr_cache[t];
            ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

            const ecs_table_cache_elem_t *elem_with = flecs_table_cache_get_elem(
                &cr->cache, table);
            if (!elem_with) {
                goto next;
            }

            it->trs[t] = elem_with->tr;
            columns[t] = elem_with->column;
        }

        it->table = table;
        it->count = ecs_table_count(table);
        it->entities = ecs_table_entities(table);
        it->trs[0] = elem->tr;
        columns[0] = elem->column;
    }

    return true;
}

bool flecs_query_trivial_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo,
    ecs_flags64_t term_set)
{
    if (redo) {
        return false;
    } else {
        const ecs_query_impl_t *impl = ctx->query;
        const ecs_query_t *q = &impl->pub;
        const ecs_term_t *terms = q->terms;
        ecs_iter_t *it = ctx->it;
        int32_t t, term_count = impl->pub.term_count;

        ecs_table_t *table = it->table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION,
            "the iterator constraint is missing a table");

        if (!flecs_table_bloom_filter_test(table, q->bloom_filter)) {
            return false;
        }

        int16_t *columns = ECS_CONST_CAST(int16_t*, it->columns);
        for (t = 0; t < term_count; t ++) {
            if (!(term_set & (1llu << t))) {
                continue;
            }

            const ecs_term_t *term = &terms[t];
            ecs_component_record_t *cr = flecs_components_get(q->world, term->id);
            if (!cr) {
                return false;
            }

            const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
            if (!tr) {
                return false;
            }

            it->trs[term->field_index] = tr;
            columns[term->field_index] = tr->column;
        }

        it->entities = ecs_table_entities(table);
        if (it->entities) {
            it->entities = &it->entities[it->offset];
        }

        return true;
    }
}

#ifdef FLECS_QUERY_PLANS

void flecs_query_trivial_set_iter_this(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_var_t *var = &ctx->vars[0];
    const ecs_table_range_t *range = &var->range;
    ecs_table_t *table = range->table;
    int32_t count = range->count;
    if (table) {
        if (!count) {
            count = ecs_table_count(table);
        }
        it->table = table;
        it->offset = range->offset;
        it->count = count;
        it->entities = ecs_table_entities(table);
        if (it->entities) {
            it->entities += it->offset;
        }
    } else if (count == 1) {
        it->count = 1;
        it->entities = &ctx->vars[0].entity;
    }
}

bool flecs_query_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo,
    ecs_flags64_t term_set)
{
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_iter_t *it = ctx->it;
    int32_t t, term_count = query->pub.term_count;

    if (!flecs_query_trivial_search_init(ctx, op_ctx, q, redo, term_set)) {
        return false;
    }

    uint64_t q_filter = q->bloom_filter;
    const ecs_term_t *terms = q->terms;

    do {
        const ecs_table_cache_elem_t *elem = flecs_table_cache_next(
            &op_ctx->it);
        if (!elem) {
            return false;
        }

        ecs_table_t *table = elem->table;
        if (!flecs_table_bloom_filter_test(table, q_filter)) {
            continue;
        }

        int16_t *columns = ECS_CONST_CAST(int16_t*, it->columns);
        for (t = op_ctx->first_to_eval; t < term_count; t ++) {
            if (!(term_set & (1llu << t))) {
                continue;
            }

            ecs_component_record_t *cr = query->cr_cache[t];
            ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

            const ecs_table_cache_elem_t *elem_with = flecs_table_cache_get_elem(
                &cr->cache, table);
            if (!elem_with) {
                break;
            }

            const ecs_term_t *term = &terms[t];
            it->trs[term->field_index] = elem_with->tr;
            columns[term->field_index] = elem_with->column;
        }

        if (t == term_count) {
            ctx->vars[0].range.table = table;
            ctx->vars[0].range.count = 0;
            ctx->vars[0].range.offset = 0;
            const ecs_term_t *term = &terms[op_ctx->start_from];
            it->trs[term->field_index] = elem->tr;
            columns[term->field_index] = elem->column;
            break;
        }
    } while (true);

    return true;
}

#endif // FLECS_QUERY_PLANS
