/**
 * @file addons/rules/engine.c
 * @brief Iterator for trivial queries.
 */

#include "rules.h"

#ifdef FLECS_RULES

static
bool flecs_rule_trivial_init(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    int32_t t, count = filter->term_count;
    ecs_term_t *terms = filter->terms;

    for (t = 0; t < count; t ++) {
        ecs_term_t *term = &terms[t];
        if (!term->idr) {
            term->idr = flecs_id_record_get(world, term->id);
            if (!term->idr) {
                /* Id doesn't exist, so query can't match */
                return false;
            }
        }
    }

    return true;
}

bool flecs_rule_trivial_test(
    const ecs_rule_t *rule,
    const ecs_rule_run_ctx_t *ctx,
    bool first,
    int32_t term_count)
{
    if (first) {
        const ecs_filter_t *filter = &rule->filter;
        int32_t t;
        ecs_term_t *terms = filter->terms;
        ecs_iter_t *it = ctx->it;

        if (!flecs_rule_trivial_init(ctx->world, filter)) {
            return false;
        }

        ecs_table_t *table = ctx->vars[0].range.table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION, NULL);

        for (t = 0; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            const ecs_table_record_t *tr = flecs_id_record_get_table(
                term->idr, table);
            if (!tr) {
                return false;
            }

            it->columns[t] = tr->index + 1;
            if (it->count && tr->column != -1) {
                it->ptrs[t] = ecs_vec_get(
                    &table->data.columns[tr->column].data,
                    it->sizes[t],
                    it->offset);
            }
        }

        it->table = table;
        it->entities = &flecs_table_entities_array(table)[it->offset];
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_trivial_search_init(
    const ecs_rule_run_ctx_t *ctx,
    ecs_rule_trivial_ctx_t *op_ctx,
    const ecs_filter_t *filter,
    bool first)
{
    if (first) {
        ecs_term_t *terms = filter->terms;
        if (!flecs_rule_trivial_init(ctx->world, filter)) {
            return false;
        }

        if (filter->flags & EcsFilterMatchEmptyTables) {
            if (!flecs_table_cache_all_iter(&terms[0].idr->cache, &op_ctx->it)){
                return false;
            }
        } else {
            if (!flecs_table_cache_iter(&terms[0].idr->cache, &op_ctx->it)) {
                return false;
            }
        }
    }

    return true;
}

bool flecs_rule_trivial_search(
    const ecs_rule_t *rule,
    const ecs_rule_run_ctx_t *ctx,
    ecs_rule_trivial_ctx_t *op_ctx,
    bool first,
    int32_t term_count)
{
    const ecs_filter_t *filter = &rule->filter;
    int32_t t;
    ecs_term_t *terms = filter->terms;
    ecs_iter_t *it = ctx->it;

    if (!flecs_rule_trivial_search_init(ctx, op_ctx, filter, first)) {
        return false;
    }

    do {
        const ecs_table_record_t *tr = flecs_table_cache_next(
            &op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        ecs_table_t *table = tr->hdr.table;
        if (table->flags & (EcsTableIsPrefab|EcsTableIsDisabled)) {
            continue;
        }

        for (t = 1; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            const ecs_table_record_t *tr_with = flecs_id_record_get_table(
                term->idr, table);
            if (!tr_with) {
                break;
            }

            it->columns[t] = tr_with->index + 1;
            if (tr_with->column != -1) {
                it->ptrs[t] = ecs_vec_first(
                    &table->data.columns[tr_with->column].data);
            }
        }

        if (t == term_count) {
            ctx->vars[0].range.table = table;
            ctx->vars[0].range.count = 0;
            ctx->vars[0].range.offset = 0;
            it->columns[0] = tr->index + 1;
            if (tr->column != -1) {
                it->ptrs[0] = ecs_vec_first(
                    &table->data.columns[tr->column].data);
            }
            break;
        }
    } while (true);

    return true;
}

bool flecs_rule_trivial_search_w_wildcards(
    const ecs_rule_t *rule,
    const ecs_rule_run_ctx_t *ctx,
    ecs_rule_trivial_ctx_t *op_ctx,
    bool first,
    int32_t term_count)
{
    bool result = flecs_rule_trivial_search(
        rule, ctx, op_ctx, first, term_count);
    if (result) {
        ecs_iter_t *it = ctx->it;
        ecs_table_t *table = ctx->vars[0].range.table;
        int32_t t;
        for (t = 0; t < term_count; t ++) {
            it->ids[t] = table->type.array[it->columns[t] - 1];
        }
    }

    return result;
}

bool flecs_rule_trivial_test_w_wildcards(
    const ecs_rule_t *rule,
    const ecs_rule_run_ctx_t *ctx,
    bool first,
    int32_t term_count)
{
    bool result = flecs_rule_trivial_test(
        rule, ctx, first, term_count);
    if (result) {
        ecs_iter_t *it = ctx->it;
        ecs_table_t *table = ctx->vars[0].range.table;
        int32_t t;
        for (t = 0; t < term_count; t ++) {
            it->ids[t] = table->type.array[it->columns[t] - 1];
        }
    }

    return result;
}

bool flecs_rule_trivial_search_nodata(
    const ecs_rule_t *rule,
    const ecs_rule_run_ctx_t *ctx,
    ecs_rule_trivial_ctx_t *op_ctx,
    bool first,
    int32_t term_count)
{
    const ecs_filter_t *filter = &rule->filter;
    int32_t t;
    ecs_term_t *terms = filter->terms;
    ecs_iter_t *it = ctx->it;

    if (!flecs_rule_trivial_search_init(ctx, op_ctx, filter, first)) {
        return false;
    }

    do {
        const ecs_table_record_t *tr = flecs_table_cache_next(
            &op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        ecs_table_t *table = tr->hdr.table;
        if (table->flags & (EcsTableIsPrefab|EcsTableIsDisabled)) {
            continue;
        }

        for (t = 1; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            const ecs_table_record_t *tr_with = flecs_id_record_get_table(
                term->idr, table);
            if (!tr_with) {
                break;
            }

            it->columns[t] = tr_with->index + 1;
        }

        if (t == term_count) {
            ctx->vars[0].range.table = table;
            ctx->vars[0].range.count = 0;
            ctx->vars[0].range.offset = 0;
            it->columns[0] = tr->index + 1;
            break;
        }
    } while (true);

    return true;
}

#endif
