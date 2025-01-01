/**
 * @file query/engine/trivial_iter.c
 * @brief Iterator for trivial queries.
 */

#include "../../private_api.h"

static
bool flecs_query_trivial_search_init(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    const ecs_query_t *query,
    bool redo,
    ecs_flags64_t term_set)
{
    if (!redo) {
        /* Find first trivial term*/
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

        ecs_id_record_t *idr = flecs_id_record_get(ctx->world, query->ids[t]);
        if (!idr) {
            return false;
        }

        if (query->flags & EcsQueryMatchEmptyTables) {
            if (!flecs_table_cache_all_iter(&idr->cache, &op_ctx->it)){
                return false;
            }
        } else {
            if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
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
    }

    return true;
}

bool flecs_query_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo,
    ecs_flags64_t term_set)
{
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    const ecs_term_t *terms = q->terms;
    ecs_iter_t *it = ctx->it;
    int32_t t, term_count = query->pub.term_count;

    if (!flecs_query_trivial_search_init(ctx, op_ctx, q, redo, term_set)) {
        return false;
    }

    do {
        const ecs_table_record_t *tr = flecs_table_cache_next(
            &op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        ecs_table_t *table = tr->hdr.table;
        if (table->flags & (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)) {
            continue;
        }

        for (t = op_ctx->first_to_eval; t < term_count; t ++) {
            if (!(term_set & (1llu << t))) {
                continue;
            }

            const ecs_term_t *term = &terms[t];
            ecs_id_record_t *idr = flecs_id_record_get(ctx->world, term->id);
            if (!idr) {
                break;
            }

            const ecs_table_record_t *tr_with = flecs_id_record_get_table(
                idr, table);
            if (!tr_with) {
                break;
            }

            it->trs[term->field_index] = tr_with;
        }

        if (t == term_count) {
            ctx->vars[0].range.table = table;
            ctx->vars[0].range.count = 0;
            ctx->vars[0].range.offset = 0;
            it->trs[op_ctx->start_from] = tr;
            break;
        }
    } while (true);

    return true;
}

bool flecs_query_is_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo)
{
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    const ecs_id_t *ids = q->ids;
    ecs_iter_t *it = ctx->it;
    int32_t t, term_count = query->pub.term_count;

    if (!flecs_query_trivial_search_init(ctx, op_ctx, q, redo, 0)) {
        return false;
    }

next:
    {
        const ecs_table_record_t *tr = flecs_table_cache_next(
            &op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        ecs_table_t *table = tr->hdr.table;
        if (table->flags & (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)) {
            goto next;
        }

        for (t = 1; t < term_count; t ++) {
            ecs_id_record_t *idr = flecs_id_record_get(ctx->world, ids[t]);
            if (!idr) {
                return false;
            }

            const ecs_table_record_t *tr_with = flecs_id_record_get_table(
                idr, table);
            if (!tr_with) {
                goto next;
            }

            it->trs[t] = tr_with;
        }

        it->table = table;
        it->count = ecs_table_count(table);
        it->entities = ecs_table_entities(table);
        it->trs[0] = tr;
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
            "the variable set on the iterator is missing a table");

        for (t = 0; t < term_count; t ++) {
            if (!(term_set & (1llu << t))) {
                continue;
            }

            const ecs_term_t *term = &terms[t];
            ecs_id_record_t *idr = flecs_id_record_get(q->world, term->id);
            if (!idr) {
                return false;
            }

            const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
            if (!tr) {
                return false;
            }

            it->trs[term->field_index] = tr;
        }

        it->entities = ecs_table_entities(table);
        if (it->entities) {
            it->entities = &it->entities[it->offset];
        }

        return true;
    }
}
