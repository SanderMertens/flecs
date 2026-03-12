/**
 * @file query/cache/change_detection.c
 * @brief Tracks component modifications via per-column dirty counters.
 *
 * Each cache match stores a snapshot of table dirty_state counters. Comparing
 * stored vs current counters reveals whether matched components changed.
 */

#include "../../private_api.h"

typedef struct {
    ecs_table_t *table;
    int32_t column;
} flecs_table_column_t;

/* Get table and column index for a query field. */
static
void flecs_query_get_column_for_field(
    const ecs_query_t *q,
    ecs_query_cache_match_t *match,
    int32_t field,
    flecs_table_column_t *out)
{
    ecs_assert(field >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field < q->field_count, ECS_INTERNAL_ERROR, NULL);
    (void)q;

    const ecs_table_record_t *tr = match->base.trs[field];
    ecs_table_t *table = tr->hdr.table;
    int32_t column = tr->column;

    out->table = table;
    out->column = column;
}

/* Get or initialize the change monitor for a cache match. */
static
bool flecs_query_get_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    bool is_trivial = flecs_query_cache_is_trivial(cache);
    ecs_assert(!is_trivial, ECS_INVALID_OPERATION, 
        "query was not created with change detection enabled");

    if (match->_monitor) {
        return false;
    }

    int32_t *monitor = flecs_balloc(&cache->allocators.monitors);
    monitor[0] = 0;

    /* Fields that aren't read or aren't set get -1 (skip) */
    const ecs_query_t *q = cache->query;
    int32_t i, field = -1, term_count = q->term_count;
    flecs_table_column_t tc;

    for (i = 0; i < term_count; i ++) {
        if (field == q->terms[i].field_index) {
            if (monitor[field + 1] != -1) {
                continue;
            }
        }

        field = q->terms[i].field_index;
        monitor[field + 1] = -1;

        if (q->terms[i].inout != EcsIn &&
            q->terms[i].inout != EcsInOut &&
            q->terms[i].inout != EcsInOutDefault) {
            continue;
        }

        if (!is_trivial) {
            if (!(match->base.set_fields & (1llu << field))) {
                continue;
            }
        }

        flecs_query_get_column_for_field(q, match, field, &tc);
        if (tc.column == -1) {
            continue;
        }

        monitor[field + 1] = 0;
    }

    match->_monitor = monitor;

    impl->pub.flags |= EcsQueryHasChangeDetection;

    return true;
}

/* Get or initialize the change monitor for fixed-source query terms. */
static
bool flecs_query_get_fixed_monitor(
    ecs_query_impl_t *impl,
    bool check)
{
    ecs_query_t *q = &impl->pub;
    ecs_world_t *world = q->real_world;
    ecs_term_t *terms = q->terms;
    int32_t i, term_count = q->term_count;

    if (!impl->monitor) {
        impl->monitor = flecs_alloc_n(&impl->stage->allocator, 
            int32_t, q->field_count);
        check = false; /* If the monitor is new, initialize it with dirty state */
    }

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        int16_t field_index = term->field_index;

        if (!(q->read_fields & flecs_ito(uint32_t, 1 << field_index))) {
            continue;
        }

        if (!(term->src.id & EcsIsEntity)) {
            continue;
        }

        ecs_entity_t src = ECS_TERM_REF_ID(&term->src);
        ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_record_t *r = flecs_entities_get(world, src);
        if (!r || !r->table) {
            continue;
        }

        ecs_component_record_t *cr = flecs_components_get(world, term->id);
        if (!cr) {
            continue;
        }

        const ecs_table_record_t *tr = flecs_component_get_table(cr, r->table);
        if (!tr) {
            continue;
        }

        int32_t *dirty_state = flecs_table_get_dirty_state(world, r->table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!check) {
            impl->monitor[field_index] = dirty_state[tr->column + 1];
        } else {
            if (impl->monitor[field_index] != dirty_state[tr->column + 1]) {
                return true;
            }
        }
    }

    return !check;
}

/* Synchronize fixed-source monitor with current dirty state. */
bool flecs_query_update_fixed_monitor(
    ecs_query_impl_t *impl)
{
    return flecs_query_get_fixed_monitor(impl, false);
}

/* Check if any fixed-source field has changed. */
bool flecs_query_check_fixed_monitor(
    ecs_query_impl_t *impl)
{
    return flecs_query_get_fixed_monitor(impl, true);
}

/* Check if a single field in a match monitor has changed. */
static
bool flecs_query_check_match_monitor_term(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *match,
    int32_t field)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INVALID_OPERATION, 
        "query was not created with change detection enabled");

    if (flecs_query_get_match_monitor(impl, match)) {
        return true;
    }

    int32_t *monitor = match->_monitor;
    int32_t state = monitor[field];
    if (state == -1) {
        return false;
    }

    ecs_table_t *table = match->base.table;
    if (table) {
        int32_t *dirty_state = flecs_table_get_dirty_state(
            cache->query->world, table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        if (!field) {
            return monitor[0] != dirty_state[0];
        }
    } else if (!field) {
        return false;
    }

    flecs_table_column_t cur;
    flecs_query_get_column_for_field(
        &impl->pub, match, field - 1, &cur);
    ecs_assert(cur.column != -1, ECS_INTERNAL_ERROR, NULL);

    return monitor[field] != flecs_table_get_dirty_state(
        cache->query->world, cur.table)[cur.column + 1];
}

/* Check if any table in the cache has changed. */
static
bool flecs_query_check_cache_monitor(
    ecs_query_impl_t *impl)
{
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Match count change means tables were matched/unmatched */
    if (cache->match_count != cache->prev_match_count) {
        return true;
    }

    const ecs_query_cache_group_t *cur = cache->first_group;
    do {
        int32_t i, count = ecs_vec_count(&cur->tables);
        for (i = 0; i < count; i ++) {
            ecs_query_cache_match_t *qm = 
                ecs_vec_get_t(&cur->tables, ecs_query_cache_match_t, i);
            if (flecs_query_check_table_monitor(impl, qm, -1)) {
                return true;
            }

            if (qm->wildcard_matches) {
                ecs_query_cache_match_t *wc_qms = 
                    ecs_vec_first(qm->wildcard_matches);
                int32_t j, wc_count = ecs_vec_count(qm->wildcard_matches);
                for (j = 0; j < wc_count; j ++) {
                    if (flecs_query_check_table_monitor(impl, &wc_qms[j], -1)) {
                        return true;
                    }
                }
            }
        }
    } while ((cur = cur->next));

    return false;
}

/* Initialize change monitors for all elements in the query cache. */
static
void flecs_query_init_query_monitors(
    ecs_query_impl_t *impl)
{
    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        const ecs_query_cache_group_t *cur = cache->first_group;
        do {
            int32_t i, count = ecs_vec_count(&cur->tables);
            for (i = 0; i < count; i ++) {
                ecs_query_cache_match_t *qm = 
                    ecs_vec_get_t(&cur->tables, ecs_query_cache_match_t, i);
                flecs_query_get_match_monitor(impl, qm);

                if (qm->wildcard_matches) {
                    ecs_query_cache_match_t *wc_qms = 
                        ecs_vec_first(qm->wildcard_matches);
                    int32_t j, wc_count = ecs_vec_count(qm->wildcard_matches);
                    for (j = 0; j < wc_count; j ++) {
                        flecs_query_get_match_monitor(impl, &wc_qms[j]);
                    }
                }
            }
        } while ((cur = cur->next));
    }
}

/* Check if a specific cache match has changed. */
static
bool flecs_query_check_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *match,
    const ecs_iter_t *it)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INVALID_OPERATION, 
        "query was not created with change detection enabled");

    if (flecs_query_get_match_monitor(impl, match)) {
        return true;
    }

    int32_t *monitor = match->_monitor;
    ecs_table_t *table = match->base.table;
    int32_t *dirty_state = NULL;
    if (table) {
        dirty_state = flecs_table_get_dirty_state(
            cache->query->world, table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        if (monitor[0] != dirty_state[0]) {
            return true;
        }
    }

    const ecs_query_t *query = cache->query;
    ecs_world_t *world = query->world;
    int32_t i, field_count = query->field_count; 
    const ecs_table_record_t **trs = it ? it->trs : match->base.trs;
    bool trivial_cache = flecs_query_cache_is_trivial(cache);

    ecs_entity_t *sources = NULL;
    ecs_flags64_t set_fields = 0;

    if (it) {
        set_fields = it->set_fields;
    } else if (!trivial_cache) {
        set_fields = match->base.set_fields;
    } else {
        set_fields = (1llu << field_count) - 1;
    }

    if (!trivial_cache) {
        sources = match->_sources;
    }

    ecs_assert(trs != NULL, ECS_INTERNAL_ERROR, NULL);

    for (i = 0; i < field_count; i ++) {
        int32_t mon = monitor[i + 1];
        if (mon == -1) {
            continue;
        }

        if (!(set_fields & (1llu << i))) {
            continue;
        }

        int32_t column = trs[i]->column;
        ecs_entity_t src = sources ? sources[i] : 0;
        if (!src) {
            if (column >= 0) {
                /* owned component */
                ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
                if (mon != dirty_state[column + 1]) {
                    return true;
                }
                continue;
            } else if (column == -1) {
                continue; /* owned but not a component */
            }
        }

        if (trivial_cache) {
            continue;
        }

        /* Component from non-this source */
        ecs_assert(match->_sources != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_entity_t fixed_src = match->_sources[i];
        ecs_table_t *src_table = ecs_get_table(world, fixed_src);
        ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t *src_dirty_state = flecs_table_get_dirty_state(
            world, src_table);
        if (mon != src_dirty_state[column + 1]) {
            return true;
        }
    }

    return false;
}

/* Check if one or more fields of a specific match have changed. */
static
bool flecs_query_check_table_monitor_match(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *qm,
    int32_t field)
{
    if (field == -1) {
        if (flecs_query_check_match_monitor(impl, qm, NULL)) {
            return true;
        }
    } else {
        if (flecs_query_check_match_monitor_term(impl, qm, field)) {
            return true;
        } 
    }

    return false;
}

/* Check if a table match and its wildcard matches have changed. */
bool flecs_query_check_table_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *qm,
    int32_t field)
{
    if (flecs_query_check_table_monitor_match(impl, qm, field)) {
        return true;
    }

    if (qm->wildcard_matches) {
        ecs_query_cache_match_t *wc_qms = ecs_vec_first(qm->wildcard_matches);
        int32_t i, count = ecs_vec_count(qm->wildcard_matches);
        for (i = 0; i < count; i ++) {
            if (flecs_query_check_table_monitor_match(impl, &wc_qms[i], field)) {
                return true;
            }
        }
    }

    return false;
}

/* Mark writable non-fixed fields as dirty after iteration. */
void flecs_query_mark_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it)
{
    ecs_query_t *q = &impl->pub;

    ecs_termset_t write_fields =
        (ecs_termset_t)(q->write_fields & ~q->fixed_fields & it->set_fields);
    if (!write_fields || (it->flags & EcsIterNoData)) {
        return;
    }

    ecs_world_t *world = q->real_world;
    int16_t i, field_count = q->field_count;
    for (i = 0; i < field_count; i ++) {
        ecs_termset_t field_bit = (ecs_termset_t)(1u << i);
        if (!(write_fields & field_bit)) {
            continue;
        }

        ecs_entity_t src = it->sources[i];
        ecs_table_t *table;
        if (!src) {
            table = it->table;
        } else {
            ecs_record_t *r = flecs_entities_get(world, src);
            if (!r || !(table = r->table)) {
                continue;
            }

            if (q->shared_readonly_fields & flecs_ito(uint32_t, 1 << i)) {
                /* Shared fields default to readonly unless explicitly out/inout */
                continue;
            }
        }

        const ecs_table_record_t *tr = it->trs[i];
        if (!tr) {
            continue; /* Non-fragmenting component */
        }

        int32_t type_index = it->trs[i]->index;
        ecs_assert(type_index >= 0, ECS_INTERNAL_ERROR, NULL);
        
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t *dirty_state = table->dirty_state;
        if (!dirty_state) {
            continue;
        }

        ecs_assert(type_index < table->type.count, ECS_INTERNAL_ERROR, NULL);
        int32_t column = table->column_map[type_index];
        dirty_state[column + 1] ++;
    }
}

/* Mark writable fixed-source fields as dirty. */
void flecs_query_mark_fixed_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it)
{
    ecs_query_t *q = &impl->pub;
    ecs_termset_t fixed_write_fields = q->write_fields & q->fixed_fields;
    if (!fixed_write_fields) {
        return;
    }

    ecs_world_t *world = q->real_world;
    int32_t i, field_count = q->field_count;
    for (i = 0; i < field_count; i ++) {
        if (!(fixed_write_fields & flecs_ito(uint32_t, 1 << i))) {
            continue;
        }

        ecs_entity_t src = it->sources[i];
        ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_record_t *r = flecs_entities_get(world, src);
        ecs_table_t *table;
        if (!r || !(table = r->table)) {
            /* If the field is optional, it's possible that it didn't match */
            continue;
        }

        int32_t *dirty_state = table->dirty_state;
        if (!dirty_state) {
            continue;
        }

        ecs_assert(it->trs[i]->column >= 0, ECS_INTERNAL_ERROR, NULL);
        int32_t column = table->column_map[it->trs[i]->column];
        dirty_state[column + 1] ++;
    }
}

/* Synchronize match monitor with current table dirty state. */
void flecs_query_sync_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INVALID_OPERATION, 
        "query was not created with change detection enabled");

    if (!match->_monitor) {
        if (impl->pub.flags & EcsQueryHasChangeDetection) {
            flecs_query_get_match_monitor(impl, match);
        } else {
            return;
        }
    }

    int32_t *monitor = match->_monitor;
    ecs_table_t *table = match->base.table;
    if (table) {
        int32_t *dirty_state = flecs_table_get_dirty_state(
            cache->query->world, table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        monitor[0] = dirty_state[0]; /* Index 0 = entity add/remove counter */
    }

    ecs_query_t *q = cache->query;
    {
        flecs_table_column_t tc;
        int32_t t, term_count = q->term_count;
        for (t = 0; t < term_count; t ++) {
            int32_t field = q->terms[t].field_index;
            if (monitor[field + 1] == -1) {
                continue;
            }

            flecs_query_get_column_for_field(q, match, field, &tc);

            /* Query for cache should never point to stage */
            ecs_assert(q->world == q->real_world, ECS_INTERNAL_ERROR, NULL);

            monitor[field + 1] = flecs_table_get_dirty_state(
                q->world, tc.table)[tc.column + 1];
        }
    }

    cache->prev_match_count = cache->match_count;
}

bool ecs_query_changed(
    ecs_query_t *q)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_query_impl_t *impl = flecs_query_impl(q);

    ecs_assert(q->cache_kind != EcsQueryCacheNone, ECS_INVALID_OPERATION, 
        "change detection is only supported on cached queries");

    if (q->read_fields & q->fixed_fields) {
        if (!impl->monitor) {
            flecs_query_get_fixed_monitor(impl, false);
        }
    }

    /* Fixed-source checks are cheaper than scanning cache entries */
    if (impl->monitor) {
        if (flecs_query_check_fixed_monitor(impl)) {
            return true;
        }
    }

    /* Check cache entries. Non-cached/non-fixed terms cannot be tracked. */
    if (impl->cache) {
        if (!(impl->pub.flags & EcsQueryHasChangeDetection)) {
            flecs_query_init_query_monitors(impl);
        }

        return flecs_query_check_cache_monitor(impl);
    }

    return false;
}

bool ecs_iter_changed(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_UNSUPPORTED, NULL);
    ecs_check(ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_query_impl_t *impl = flecs_query_impl(it->query);
    ecs_query_t *q = &impl->pub;

    /* Check fixed-source fields first (cheap: no per-result state needed) */
    if (q->read_fields & q->fixed_fields) {
        /* Only compute once per iteration (cached in flags) */
        if (!(it->flags & EcsIterFixedInChangeComputed)) {
            it->flags |= EcsIterFixedInChangeComputed;
            ECS_BIT_COND(it->flags, EcsIterFixedInChanged, 
                flecs_query_check_fixed_monitor(impl));
        }

        if (it->flags & EcsIterFixedInChanged) {
            return true;
        }
    }

    /* If query has a cache, check for changes in current matched result */
    if (impl->cache) {
        ecs_query_cache_match_t *qm = 
            (ecs_query_cache_match_t*)it->priv_.iter.query.elem;
        ecs_check(qm != NULL, ECS_INVALID_PARAMETER, NULL);
        return flecs_query_check_match_monitor(impl, qm, it);
    }

error:
    return false;
}

void ecs_iter_skip(
    ecs_iter_t *it)
{
    ecs_assert(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INVALID_PARAMETER, NULL);
    it->flags |= EcsIterSkip;
}
