/**
 * @file addons/rules/engine.c
 * @brief Iterator for trivial queries.
 */

#include "rules.h"

#ifdef FLECS_RULES

static
bool flecs_rule_trivial_init(
    ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_filter_t *filter)
{
    int32_t t, count = filter->term_count;
    ecs_term_t *terms = filter->terms;

    for (t = 0; t < count; t ++) {
        ecs_term_t *term = &terms[t];
        it->ids[t] = term->id;
    }

    it->fini = NULL; /* Trivial queries don't need cleanup */

    for (t = 0; t < count; t ++) {
        ecs_term_t *term = &terms[t];
        if (!term->idr) {
            term->idr = flecs_id_record_get(world, term->id);
            if (!term->idr) {
                return false;
            }
        }
    }

    return true;
}

bool flecs_rule_trivial_test(
    const ecs_rule_t *rule,
    ecs_rule_run_ctx_t *ctx,
    bool first)
{
    if (first) {
        const ecs_filter_t *filter = &rule->filter;
        int32_t t, count = filter->term_count;
        ecs_term_t *terms = filter->terms;
        ecs_iter_t *it = ctx->it;

        if (!flecs_rule_trivial_init(ctx->world, it, filter)) {
            return false;
        }

        ecs_table_t *table = ctx->vars[0].range.table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION, NULL);

        for (t = 0; t < count; t ++) {
            ecs_term_t *term = &terms[t];
            const ecs_table_record_t *tr = flecs_id_record_get_table(
                term->idr, table);
            if (!tr) {
                return false;
            }

            it->columns[t] = tr->index + 1;
            if (it->count && tr->column != -1) {
                it->ptrs[t] = ecs_vec_first(
                    &table->data.columns[tr->column].data);
            }
        }

        it->table = table;
        it->entities = flecs_table_entities_array(table);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_trivial_search_init(
    ecs_rule_run_ctx_t *ctx,
    ecs_rule_trivial_ctx_t *op_ctx,
    ecs_iter_t *it,
    const ecs_filter_t *filter,
    bool first)
{
    if (first) {
        ecs_term_t *terms = filter->terms;
        if (!flecs_rule_trivial_init(ctx->world, it, filter)) {
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

        it->offset = 0;
    }

    it->frame_offset += it->count;

    return true;
}

bool flecs_rule_trivial_search(
    const ecs_rule_t *rule,
    ecs_rule_run_ctx_t *ctx,
    bool first)
{
    ecs_rule_trivial_ctx_t *op_ctx = &ctx->op_ctx[0].is.trivial;
    const ecs_filter_t *filter = &rule->filter;
    int32_t t, count = filter->term_count;
    ecs_term_t *terms = filter->terms;
    ecs_iter_t *it = ctx->it;

    if (!flecs_rule_trivial_search_init(ctx, op_ctx, it, filter, first)) {
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

        for (t = 1; t < count; t ++) {
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

        if (t == count) {
            it->table = table;
            it->count = ecs_table_count(table);
            it->entities = flecs_table_entities_array(table);
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
    ecs_rule_run_ctx_t *ctx,
    bool first)
{
    bool result = flecs_rule_trivial_search(rule, ctx, first);
    if (result) {
        const ecs_filter_t *filter = &rule->filter;
        int32_t t, count = filter->term_count;
        ecs_iter_t *it = ctx->it;
        ecs_table_t *table = it->table;
        for (t = 0; t < count; t ++) {
            it->ids[t] = table->type.array[it->columns[t] - 1];
        }
    }

    return result;
}

bool flecs_rule_trivial_search_nodata(
    const ecs_rule_t *rule,
    ecs_rule_run_ctx_t *ctx,
    bool first)
{
    ecs_rule_trivial_ctx_t *op_ctx = &ctx->op_ctx[0].is.trivial;
    const ecs_filter_t *filter = &rule->filter;
    int32_t t, count = filter->term_count;
    ecs_term_t *terms = filter->terms;
    ecs_iter_t *it = ctx->it;

    if (!flecs_rule_trivial_search_init(ctx, op_ctx, it, filter, first)) {
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

        for (t = 1; t < count; t ++) {
            ecs_term_t *term = &terms[t];
            const ecs_table_record_t *tr_with = flecs_id_record_get_table(
                term->idr, table);
            if (!tr_with) {
                break;
            }

            it->columns[t] = tr_with->index + 1;
        }

        if (t == count) {
            it->table = table;
            it->count = ecs_table_count(table);
            it->entities = flecs_table_entities_array(table);
            it->columns[0] = tr->index + 1;
            break;
        }
    } while (true);

    return true;
}

#endif
