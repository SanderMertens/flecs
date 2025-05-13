/**
 * @file query/engine/cache_iter.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

static
void flecs_query_update_node_up_trs(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_cache_match_t *node)
{
    ecs_os_perf_trace_push("flecs.query.update_node_up_trs");
    
    const ecs_query_impl_t *impl = ctx->query;
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INTERNAL_ERROR, NULL);

    ecs_termset_t fields = node->_up_fields & node->_set_fields;
    if (fields) {
        const ecs_query_t *q = cache->query;
        int32_t f, field_count = q->field_count;
        int8_t *field_map = cache->field_map;
        for (f = 0; f < field_count; f ++) {
            if (!(fields & (1llu << f))) {
                continue;
            }

            ecs_entity_t src = node->_sources[f];
            if (src) {
                ecs_record_t *r = flecs_entities_get(ctx->world, src);
                ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
                if (r->table != node->_tables[f]) {
                    ecs_component_record_t *cr = flecs_components_get(
                        ctx->world, q->ids[f]);
                    const ecs_table_record_t *tr = node->base.trs[f] = 
                        flecs_component_get_table(cr, r->table);
                    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                    ctx->it->trs[field_map ? field_map[f] : f] = tr;
                    node->_tables[f] = r->table;
                }
            }
        }
    }

    ecs_os_perf_trace_pop("flecs.query.update_node_up_trs");
}

static
ecs_query_cache_match_t* flecs_query_cache_next(
    const ecs_query_run_ctx_t *ctx,
    bool always_match_empty)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    repeat: {
        ecs_query_cache_match_t *node = qit->node;
        ecs_query_cache_match_t *prev = qit->prev;

        if (prev != qit->last) {
            ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
            ctx->vars[0].range.table = node->base.table;
            qit->node = node->base.next;
            qit->prev = node;
            if (node) {
                if (!ecs_table_count(node->base.table)) {
                    if (!(always_match_empty || (it->flags & EcsIterMatchEmptyTables))) {
                        if (ctx->query->pub.flags & EcsQueryHasChangeDetection) {
                            flecs_query_sync_match_monitor(
                                flecs_query_impl(it->query), node);
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
ecs_query_cache_match_t* flecs_query_trivial_cache_next(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    repeat: {
        ecs_query_cache_match_t *node = qit->node;
        ecs_query_cache_match_t *prev = qit->prev;

        if (prev != qit->last) {
            ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
            qit->node = node->base.next;
            qit->prev = node;

            ecs_table_t *table = it->table = node->base.table;
            int32_t count = it->count = ecs_table_count(table);

            if (!count) {
                if (!(it->flags & EcsIterMatchEmptyTables)) {
                    goto repeat;
                }
            }

            it->entities = ecs_table_entities(table);
            it->trs = node->base.trs;

            return node;
        }
    }

    return NULL;
}

static
ecs_query_cache_match_t* flecs_query_test(
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
    ecs_query_cache_match_t *node)
{
    ecs_iter_t *it = ctx->it;
    const ecs_query_impl_t *impl = ctx->query;
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INTERNAL_ERROR, NULL);

    int32_t i, field_count = cache->query->field_count;
    int8_t *field_map = cache->field_map;

    for (i = 0; i < field_count; i ++) {
        int8_t field_index = field_map[i];
        it->trs[field_index] = node->base.trs[i];

        it->ids[field_index] = node->_ids[i];
        it->sources[field_index] = node->_sources[i];

        ecs_termset_t bit = (ecs_termset_t)(1u << i);
        ecs_termset_t field_bit = (ecs_termset_t)(1u << field_index);

        ECS_TERMSET_COND(it->set_fields, field_bit, node->_set_fields & bit);
        ECS_TERMSET_COND(it->up_fields, field_bit, node->_up_fields & bit);
    }
}

/* Iterate cache for query that's partially cached */
bool flecs_query_cache_search(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(!flecs_query_cache_is_trivial(ctx->query->cache), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_match_t *node = flecs_query_cache_next(ctx, false);
    if (!node) {
        return false;
    }

    ecs_os_perf_trace_push("flecs.query.cache_search");

    flecs_query_cache_init_mapped_fields(ctx, node);
    ctx->vars[0].range.count = node->_count;
    ctx->vars[0].range.offset = node->_offset;

    flecs_query_update_node_up_trs(ctx, node);

    ecs_os_perf_trace_pop("flecs.query.cache_search");

    return true;
}

/* Iterate cache for query that's entirely cached */
bool flecs_query_is_cache_search(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(!flecs_query_cache_is_trivial(ctx->query->cache), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_match_t *node = flecs_query_cache_next(ctx, false);
    if (!node) {
        return false;
    }

    ecs_os_perf_trace_push("flecs.query.cache_search");

    ecs_iter_t *it = ctx->it;
    it->trs = node->base.trs;
    it->ids = node->_ids;
    it->sources = node->_sources;
    it->set_fields = node->_set_fields;
    it->up_fields = node->_up_fields;

    flecs_query_update_node_up_trs(ctx, node);

    ecs_os_perf_trace_pop("flecs.query.cache_search");

    return true;
}

/* Iterate cache for query that's entirely cached */
bool flecs_query_is_trivial_cache_search(
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_trivial_cache_next(ctx) != NULL;
}

/* Test if query that is entirely cached matches constrained $this */
bool flecs_query_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_query_cache_match_t *node = flecs_query_test(ctx, redo);
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
    ecs_assert(!flecs_query_cache_is_trivial(ctx->query->cache), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_match_t *node = flecs_query_test(ctx, redo);
    if (!node) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    it->trs = node->base.trs;
    it->ids = node->_ids;
    it->sources = node->_sources;

    flecs_query_update_node_up_trs(ctx, node);

    return true;
}

bool flecs_query_is_trivial_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_assert(flecs_query_cache_is_trivial(ctx->query->cache), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_iter_t *it = ctx->it;
    if (!redo) {
        ecs_var_t *var = &ctx->vars[0];
        ecs_table_t *table = var->range.table;
        ecs_assert(table != NULL, ECS_INVALID_OPERATION, 
            "the variable set on the iterator is missing a table");

        ecs_query_cache_table_t *qt = flecs_query_cache_get_table(
            ctx->query->cache, table);
        if (!qt) {
            return false;
        }

        /* Trivial queries can only have a single match per table */
        ecs_assert(qt->first == qt->last, ECS_INTERNAL_ERROR, NULL);

        it->trs = qt->first->base.trs;
        return true;
    }

    return false;
}
