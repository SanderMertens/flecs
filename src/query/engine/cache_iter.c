/**
 * @file query/engine/cache_iter.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

static
void flecs_query_update_node_up_trs(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_table_match_t *node)
{
    ecs_termset_t fields = node->up_fields & node->set_fields;
    if (fields) {
        const ecs_query_impl_t *impl = ctx->query;
        ecs_query_cache_t *cache = impl->cache;
        const ecs_query_t *q = cache->query;
        int32_t f, field_count = q->field_count;
        int8_t *field_map = cache->field_map;
        for (f = 0; f < field_count; f ++) {
            if (!(fields & (1llu << f))) {
                continue;
            }

            ecs_entity_t src = node->sources[f];
            if (src) {
                ecs_record_t *r = flecs_entities_get(ctx->world, src);
                ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
                if (r->table != node->tables[f]) {
                    ecs_component_record_t *idr = flecs_components_get(
                        ctx->world, q->ids[f]);
                    const ecs_table_record_t *tr = node->trs[f] = 
                        flecs_component_get_table(idr, r->table);
                    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                    ctx->it->trs[field_map ? field_map[f] : f] = tr;
                    node->tables[f] = r->table;
                }
            }
        }
    }
}

static
ecs_query_cache_table_match_t* flecs_query_cache_next(
    const ecs_query_run_ctx_t *ctx,
    bool match_empty)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    repeat: {
        ecs_query_cache_table_match_t *node = qit->node;
        ecs_query_cache_table_match_t *prev = qit->prev;

        if (prev != qit->last) {
            ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
            ctx->vars[0].range.table = node->table;
            it->group_id = node->group_id;
            qit->node = node->next;
            qit->prev = node;
            if (node) {
                if (!ecs_table_count(node->table)) {
                    if (!match_empty) {
                        if (ctx->query->pub.flags & EcsQueryHasMonitor) {
                            flecs_query_sync_match_monitor(
                                flecs_query_impl(qit->query), node);
                        }
                        goto repeat;
                    }
                }
            }
            return node;
        }
    }

    return NULL;
}

static
ecs_query_cache_table_match_t* flecs_query_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_iter_t *it = ctx->it;
    if (!redo) {
        ecs_var_t *var = &ctx->vars[0];
        ecs_table_t *table = var->range.table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION, 
            "the variable set on the iterator is missing a table");

        ecs_query_cache_table_t *qt = flecs_query_cache_get_table(
            ctx->query->cache, table);
        if (!qt) {
            return NULL;
        }

        ecs_query_iter_t *qit = &it->priv_.iter.query;
        qit->prev = NULL;
        qit->node = qt->first;
        qit->last = qt->last;
    }

    return flecs_query_cache_next(ctx, true);
}

static
void flecs_query_cache_init_mapped_fields(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_table_match_t *node)
{
    ecs_iter_t *it = ctx->it;
    const ecs_query_impl_t *impl = ctx->query;
    ecs_query_cache_t *cache = impl->cache;
    int32_t i, field_count = cache->query->field_count;
    int8_t *field_map = cache->field_map;

    for (i = 0; i < field_count; i ++) {
        int8_t field_index = field_map[i];
        it->trs[field_index] = node->trs[i];

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
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_cache_table_match_t *node = flecs_query_cache_next(ctx,
        ctx->query->pub.flags & EcsQueryMatchEmptyTables);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(ctx, node);
    ctx->vars[0].range.count = node->count;
    ctx->vars[0].range.offset = node->offset;

    flecs_query_update_node_up_trs(ctx, node);

    return true;
}

/* Iterate cache for query that's entirely cached */
bool flecs_query_is_cache_search(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_cache_table_match_t *node = flecs_query_cache_next(ctx,
        ctx->query->pub.flags & EcsQueryMatchEmptyTables);
    if (!node) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    it->trs = node->trs;
    it->ids = node->ids;
    it->sources = node->sources;
    it->set_fields = node->set_fields;
    it->up_fields = node->up_fields;

    flecs_query_update_node_up_trs(ctx, node);

    return true;
}

/* Test if query that is entirely cached matches constrained $this */
bool flecs_query_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_query_cache_table_match_t *node = flecs_query_test(ctx, redo);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(ctx, node);
    flecs_query_update_node_up_trs(ctx, node);

    return true;
}

/* Test if query that is entirely cached matches constrained $this */
bool flecs_query_is_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_query_cache_table_match_t *node = flecs_query_test(ctx, redo);
    if (!node) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    it->trs = node->trs;
    it->ids = node->ids;
    it->sources = node->sources;

    flecs_query_update_node_up_trs(ctx, node);

    return true;
}
