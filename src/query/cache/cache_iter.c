/**
 * @file query/cache/cache_iter.c
 * @brief Cache iterator functions.
 */

#include "../../private_api.h"

/* Initialize cached query iterator. */
void flecs_query_cache_iter_init(
    ecs_iter_t *it,
    ecs_query_iter_t *qit,
    ecs_query_impl_t *impl)
{
    ecs_query_cache_t *cache = impl->cache;
    if (!cache) {
        return;
    }

    qit->group = cache->first_group;
    qit->tables = &qit->group->tables;
    qit->all_tables = qit->tables;
    qit->cur = 0;

    /* If query uses order_by, iterate the array with ordered table slices. */
    if (cache->order_by_callback) {
        /* Check if query needs sorting. */
        flecs_query_cache_sort_tables(it->real_world, impl);
        qit->tables = &cache->table_slices;
        qit->all_tables = qit->tables;
        qit->group = NULL;
    }

    cache->prev_match_count = cache->match_count;
}

/* Find next match in cache. This function is called for non-trivial caches and
 * handles features like wildcards, up traversal and grouping. */
static
ecs_query_cache_match_t* flecs_query_cache_next(
    const ecs_query_run_ctx_t *ctx,
    bool always_match_empty)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    repeat: {
        if (qit->cur >= ecs_vec_count(qit->tables)) {
            /* We're iterating the table vector of the group */
            if (qit->tables == qit->all_tables) {   
                /* If a group is set, we might have to iterate multiple groups */
                ecs_query_cache_group_t *group = qit->group;
                if (!group || qit->iter_single_group) {
                    return NULL;
                }

                /* Check if this was the last group to iterate */
                qit->group = group->next;
                if (!qit->group) {
                    return NULL;
                }

                /* Prepare iterator for the next group */
                qit->all_tables = qit->tables = &qit->group->tables;
                qit->cur = 0;

                /* Not common, but can happen if a query uses group_by and there
                 * are no tables in the default group (group id 0). */
                if (!ecs_vec_count(qit->tables)) {
                    goto repeat;
                }

            /* We're iterating a wildcard table vector */
            } else {
                qit->tables = qit->all_tables;
                qit->cur = qit->all_cur;
                goto repeat;
            }
        }

        /* Get currently iterated cache element */
        ecs_query_cache_match_t *qm = 
            ecs_vec_get_t(qit->tables, ecs_query_cache_match_t, qit->cur);

        /* Check if table is empty and whether we need to skip it */
        ecs_table_t *table = qm->base.table;
        if (!ecs_table_count(table)) {
            if (!(always_match_empty || (it->flags & EcsIterMatchEmptyTables))) {
                if (ctx->query->pub.flags & EcsQueryHasChangeDetection) {
                    ecs_query_impl_t *impl = flecs_query_impl(it->query);
                    flecs_query_sync_match_monitor(impl, qm);

                    if (qm->wildcard_matches) {
                        ecs_query_cache_match_t *wc_qms =
                            ecs_vec_first(qm->wildcard_matches);
                        int32_t j, wc_count =
                            ecs_vec_count(qm->wildcard_matches);
                        for (j = 0; j < wc_count; j ++) {
                            flecs_query_sync_match_monitor(impl, &wc_qms[j]);
                        }
                    }
                }
                qit->cur ++;
                goto repeat;
            }
        }

        qit->elem = qm;
    
        /* If there are multiple matches for table iterate those first. */
        if (qm->wildcard_matches) {
            qit->tables = qm->wildcard_matches;
            qit->all_cur = qit->cur + 1;
            qit->cur = 0;
        } else {
            qit->cur ++;
        }

        ctx->vars[0].range.table = table;

        return qm;
    }
}

/* Find next match in trivial cache. A trivial cache doesn't have to handle
 * wildcards, multiple groups or fields matched through up traversal. */
static
ecs_query_cache_match_t* flecs_query_trivial_cache_next(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    repeat: {
        if (qit->cur == ecs_vec_count(qit->tables)) {
            return NULL;
        }

        ecs_query_triv_cache_match_t *qm = ecs_vec_get_t(
            qit->tables, ecs_query_triv_cache_match_t, qit->cur);
        ecs_table_t *table = it->table = qm->table;
        int32_t count = it->count = ecs_table_count(table);

        qit->cur ++;

        if (!count) {
            if (!(it->flags & EcsIterMatchEmptyTables)) {
                goto repeat;
            }
        }

        it->entities = ecs_table_entities(table);
        it->columns = qm->columns;
        it->set_fields = qm->set_fields;

        return qit->elem = (ecs_query_cache_match_t*)qm;
    }
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
        ecs_assert(qt->group != NULL, ECS_INTERNAL_ERROR, NULL);
        qit->group = qt->group;
        qit->tables = &qt->group->tables;
        qit->cur = qt->index;
    }

    ecs_query_cache_match_t *qm = flecs_query_cache_next(ctx, true /* always match empty */);
    if (redo && qm) {
        if (qm->base.table != it->table) {
            return NULL;
        }
    }

    return qm;
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
    int16_t *columns = ECS_CONST_CAST(int16_t*, it->columns);

    for (i = 0; i < field_count; i ++) {
        int8_t field_index = field_map[i];
        it->trs[field_index] = node->_trs ? node->_trs[i] : NULL;
        columns[field_index] = node->base.columns[i];

        it->ids[field_index] = node->_ids[i];
        it->sources[field_index] = node->_sources[i];

        ecs_termset_t bit = (ecs_termset_t)(1u << i);
        ecs_termset_t field_bit = (ecs_termset_t)(1u << field_index);

        ECS_TERMSET_COND(it->set_fields, field_bit, node->base.set_fields & bit);
        ECS_TERMSET_COND(it->up_fields, field_bit, node->_up_fields & bit);
    }
}

/* Reset the cache iteration cursor to the start of the iteration. This is
 * called when the cache operation is entered fresh (redo == false), which
 * happens when a preceding operation in the plan yields a new result. */
static
void flecs_query_cache_iter_restart(
    const ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_cache_t *cache = ctx->query->cache;

    if (qit->iter_single_group) {
        qit->tables = qit->all_tables;
        qit->cur = 0;
    } else if (cache->order_by_callback) {
        qit->tables = qit->all_tables = &cache->table_slices;
        qit->group = NULL;
        qit->cur = 0;
    } else {
        qit->group = cache->first_group;
        qit->tables = qit->all_tables = &qit->group->tables;
        qit->cur = 0;
    }
}

/* Iterate cache for query that's partially cached */
bool flecs_query_cache_search(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_assert(!flecs_query_cache_is_trivial(ctx->query->cache),
        ECS_INTERNAL_ERROR, NULL);

    if (!redo) {
        flecs_query_cache_iter_restart(ctx);
    }

    ecs_query_cache_match_t *node = flecs_query_cache_next(ctx, false);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(ctx, node);
    ctx->vars[0].range.count = node->_count;
    ctx->vars[0].range.offset = node->_offset;

    return true;
}

/* Iterate cache for query that's entirely cached */
bool flecs_query_is_cache_search(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_assert(!flecs_query_cache_is_trivial(ctx->query->cache),
        ECS_INTERNAL_ERROR, NULL);

    if (!redo) {
        flecs_query_cache_iter_restart(ctx);
    }

    ecs_query_cache_match_t *node = flecs_query_cache_next(ctx, false);
    if (!node) {
        return false;
    }

    ctx->vars[0].range.count = node->_count;
    ctx->vars[0].range.offset = node->_offset;

    ecs_iter_t *it = ctx->it;
    it->trs = node->_trs;
    it->columns = node->base.columns;
    it->ids = node->_ids;
    it->sources = node->_sources;
    it->set_fields = node->base.set_fields;
    it->up_fields = node->_up_fields;

#ifdef FLECS_DEBUG
    it->flags |= EcsIterImmutableCacheData;
#endif

    return true;
}

/* Iterate trivial cache for query that's entirely cached */
bool flecs_query_is_trivial_cache_search(
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_trivial_cache_next(ctx) != NULL;
}

/* Test if query that is partially cached matches constrained $this */
bool flecs_query_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo)
{
    ecs_query_cache_match_t *node = flecs_query_test(ctx, redo);
    if (!node) {
        return false;
    }

    flecs_query_cache_init_mapped_fields(ctx, node);

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
    it->trs = node->_trs;
    it->columns = node->base.columns;
    it->ids = node->_ids;
    it->sources = node->_sources;
    it->set_fields = node->base.set_fields;

#ifdef FLECS_DEBUG
    it->flags |= EcsIterImmutableCacheData;
#endif

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

        ecs_query_cache_t *cache = ctx->query->cache;
        ecs_query_cache_table_t *qt = flecs_query_cache_get_table(cache, table);
        if (!qt) {
            return false;
        }

        ecs_query_cache_match_t *qm =
            flecs_query_cache_match_from_table(cache, qt);
        it->columns = qm->base.columns;
        it->set_fields = qm->base.set_fields;
        return true;
    }

    return false;
}
