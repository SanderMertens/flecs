/**
 * @file query/engine/change_detection.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

typedef struct {
    ecs_table_t *table;
    int32_t column;
} flecs_table_column_t;

static
void flecs_query_get_column_for_field(
    const ecs_query_t *q,
    ecs_query_cache_table_match_t *match,
    int32_t field,
    flecs_table_column_t *out)
{
    ecs_assert(field >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field < q->field_count, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t src = match->sources[field];
    ecs_table_t *table = NULL;
    int32_t column = -1;

    if (!src) {
        table = match->table;
        column = match->storage_columns[field];
        if (column == -2) {
            /* Shared field */
            column = -1;
        }
    } else {
        ecs_record_t *r = flecs_entities_get(q->world, src);
        table = r->table;

        int32_t ref_index = match->columns[field];
        ecs_ref_t *ref = ecs_vec_get_t(&match->refs, ecs_ref_t, ref_index);
        if (ref->id != 0) {
            ecs_ref_update(q->world, ref);
            column = ref->tr->column;
        }
    }

    out->table = table;
    out->column = column;
}

/* Get match monitor. Monitors are used to keep track of whether components 
 * matched by the query in a table have changed. */
static
bool flecs_query_get_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);
    if (match->monitor) {
        return false;
    }

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t *monitor = flecs_balloc(&cache->allocators.monitors);
    monitor[0] = 0;

    /* Mark terms that don't need to be monitored. This saves time when reading
     * and/or updating the monitor. */
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

        /* If term isn't read, don't monitor */
        if (q->terms[i].inout != EcsIn && 
            q->terms[i].inout != EcsInOut &&
            q->terms[i].inout != EcsInOutDefault) {
            continue;
        }

        /* Don't track fields that aren't set */
        if (!(match->set_fields & (1llu << field))) {
            continue;
        }

        ecs_assert(match->columns != NULL, ECS_INTERNAL_ERROR, NULL);
        int32_t column = match->columns[field];
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
        (void)column;

        flecs_query_get_column_for_field(q, match, field, &tc);
        if (tc.column == -1) {
            continue; /* Don't track terms that aren't stored */
        }

        monitor[field + 1] = 0;
    }

    match->monitor = monitor;

    impl->pub.flags |= EcsQueryHasMonitor;

    return true;
}

/* Get monitor for fixed query terms. Fixed terms are handled separately as they
 * don't require a query cache, and fixed terms aren't stored in the cache. */
static
bool flecs_query_get_fixed_monitor(
    ecs_query_impl_t *impl,
    bool check)
{
    ecs_query_t *q = &impl->pub;
    ecs_world_t *world = q->world;
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
            continue; /* If term doesn't read data there's nothing to track */
        }

        if (!(term->src.id & EcsIsEntity)) {
            continue; /* Not a term with a fixed source */
        }

        ecs_entity_t src = ECS_TERM_REF_ID(&term->src);
        ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_record_t *r = flecs_entities_get(world, src);
        if (!r || !r->table) {
            continue; /* Entity is empty, nothing to track */
        }

        ecs_id_record_t *idr = flecs_id_record_get(world, term->id);
        if (!idr) {
            continue; /* If id doesn't exist, entity can't have it */
        }

        ecs_table_record_t *tr = flecs_id_record_get_table(idr, r->table);
        if (!tr) {
            continue; /* Entity doesn't have the component */
        }

        /* Copy/check column dirty state from table */
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

bool flecs_query_update_fixed_monitor(
    ecs_query_impl_t *impl)
{
    return flecs_query_get_fixed_monitor(impl, false);
}

bool flecs_query_check_fixed_monitor(
    ecs_query_impl_t *impl)
{
    return flecs_query_get_fixed_monitor(impl, true);
}


/* Check if single match term has changed */
static
bool flecs_query_check_match_monitor_term(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *match,
    int32_t field)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_get_match_monitor(impl, match)) {
        return true;
    }

    int32_t *monitor = match->monitor;
    int32_t state = monitor[field];
    if (state == -1) {
        return false;
    }

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = match->table;
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

static
bool flecs_query_check_cache_monitor(
    ecs_query_impl_t *impl)
{
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);

    /* If the match count changed, tables got matched/unmatched for the
     * cache, so return that the query has changed. */
    if (cache->match_count != cache->prev_match_count) {
        return true;
    }

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&cache->cache, &it)) {
        ecs_query_cache_table_t *qt;
        while ((qt = flecs_table_cache_next(&it, ecs_query_cache_table_t))) {
            if (flecs_query_check_table_monitor(impl, qt, -1)) {
                return true;
            }
        }
    }

    return false;
}

static
void flecs_query_init_query_monitors(
    ecs_query_impl_t *impl)
{
    /* Change monitor for cache */
    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        ecs_query_cache_table_match_t *cur = cache->list.first;

        /* Ensure each match has a monitor */
        for (; cur != NULL; cur = cur->next) {
            ecs_query_cache_table_match_t *match = 
                (ecs_query_cache_table_match_t*)cur;
            flecs_query_get_match_monitor(impl, match);
        }
    }
}

static
bool flecs_query_check_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *match,
    const ecs_iter_t *it)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_get_match_monitor(impl, match)) {
        return true;
    }

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t *monitor = match->monitor;
    ecs_table_t *table = match->table;
    int32_t *dirty_state = NULL;
    if (table) {
        dirty_state = flecs_table_get_dirty_state(
            cache->query->world, table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        if (monitor[0] != dirty_state[0]) {
            return true;
        }
    }

    bool is_this = false;
    const ecs_query_t *query = cache->query;
    ecs_world_t *world = query->world;
    int32_t i, j, field_count = query->field_count;
    int32_t *storage_columns = match->storage_columns;
    ecs_entity_t *sources = match->sources;
    int32_t *columns = it ? it->columns : match->columns;
    ecs_flags64_t set_fields = it ? it->set_fields : match->set_fields;
    
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_t *refs = &match->refs;
    for (i = 0; i < field_count; i ++) {
        int32_t mon = monitor[i + 1];
        if (mon == -1) {
            continue;
        }

        if (!(set_fields & (1llu << i))) {
            continue;
        }

        int32_t column = storage_columns[i];
        ecs_entity_t src = sources[i];
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

        column = columns[i];
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);

        /* Find term index from field index, which differ when using || */
        int32_t term_index = i;
        if (query->terms[i].field_index != i) {
            for (j = i; j < query->term_count; j ++) {
                if (query->terms[j].field_index == i) {
                    term_index = j;
                    break;
                }
            }
        }

        is_this = ecs_term_match_this(&query->terms[term_index]);

        /* Flattened fields are encoded by adding field_count to the column
         * index of the parent component. */
        ecs_assert(column <= field_count, ECS_INTERNAL_ERROR, NULL);

        if (is_this) {
            /* Component reached through traversal from this */
            int32_t ref_index = column;
            ecs_ref_t *ref = ecs_vec_get_t(refs, ecs_ref_t, ref_index);
            if (ref->id != 0) {
                ecs_ref_update(world, ref);
                ecs_table_record_t *tr = ref->tr;
                ecs_table_t *src_table = tr->hdr.table;
                column = tr->index;
                column = ecs_table_type_to_column_index(src_table, column);
                int32_t *src_dirty_state = flecs_table_get_dirty_state(
                    world, src_table);
                if (mon != src_dirty_state[column + 1]) {
                    return true;
                }
            }
        } else {
            /* Component from static source */
            ecs_entity_t fixed_src = match->sources[i];
            ecs_table_t *src_table = ecs_get_table(world, fixed_src);
            ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
            column = ecs_table_type_to_column_index(src_table, column);
            int32_t *src_dirty_state = flecs_table_get_dirty_state(
                world, src_table);
            if (mon != src_dirty_state[column + 1]) {
                return true;
            }
        }
    }

    return false;
}

/* Check if any term for matched table has changed */
bool flecs_query_check_table_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_t *table,
    int32_t field)
{
    ecs_query_cache_table_match_t *cur, *end = table->last->next;

    for (cur = table->first; cur != end; cur = cur->next) {
        ecs_query_cache_table_match_t *match = 
            (ecs_query_cache_table_match_t*)cur;
        if (field == -1) {
            if (flecs_query_check_match_monitor(impl, match, NULL)) {
                return true;
            }
        } else {
            if (flecs_query_check_match_monitor_term(impl, match, field)) {
                return true;
            } 
        }
    }

    return false;
}

void flecs_query_mark_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it)
{
    ecs_query_t *q = &impl->pub;

    /* Evaluate all writeable non-fixed fields, set fields */
    ecs_termset_t write_fields = 
        (ecs_termset_t)(q->write_fields & ~q->fixed_fields & it->set_fields);
    if (!write_fields) {
        return;
    }

    ecs_world_t *world = q->world;
    int16_t i, field_count = q->field_count;
    for (i = 0; i < field_count; i ++) {
        if (!(write_fields & flecs_ito(uint32_t, 1 << i))) {
            continue; /* If term doesn't write data there's nothing to track */
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
                /* Shared fields that aren't marked explicitly as out/inout 
                 * default to readonly */
                continue;
            }
        }

        int32_t type_index = it->columns[i];
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

void flecs_query_mark_fixed_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it)
{
    /* This function marks fields dirty for terms with fixed sources. */
    ecs_query_t *q = &impl->pub;
    ecs_termset_t fixed_write_fields = q->write_fields & q->fixed_fields;
    if (!fixed_write_fields) {
        return;
    }

    ecs_world_t *world = q->world;
    int32_t i, field_count = q->field_count;
    for (i = 0; i < field_count; i ++) {
        if (!(fixed_write_fields & flecs_ito(uint32_t, 1 << i))) {
            continue; /* If term doesn't write data there's nothing to track */
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

        ecs_assert(it->columns[i] >= 0, ECS_INTERNAL_ERROR, NULL);
        int32_t column = table->column_map[it->columns[i]];
        dirty_state[column + 1] ++;
    }
}

/* Synchronize match monitor with table dirty state */
void flecs_query_sync_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!match->monitor) {
        if (impl->pub.flags & EcsQueryHasMonitor) {
            flecs_query_get_match_monitor(impl, match);
        } else {
            return;
        }
    }

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t *monitor = match->monitor;
    ecs_table_t *table = match->table;
    if (table) {
        int32_t *dirty_state = flecs_table_get_dirty_state(
            cache->query->world, table);
        ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        monitor[0] = dirty_state[0]; /* Did table gain/lose entities */
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

    /* If query reads terms with fixed sources, check those first as that's 
     * cheaper than checking entries in the cache. */
    if (impl->monitor) {
        if (flecs_query_check_fixed_monitor(impl)) {
            return true;
        }
    }

    /* Check cache for changes. We can't detect changes for terms that are not
     * cached/cacheable and don't have a fixed source, since that requires 
     * storing state per result, which doesn't happen for uncached queries. */
    if (impl->cache) {
        /* If we're checking the cache, make sure that tables are in the correct
         * empty/non-empty lists. */
        flecs_process_pending_tables(q->world);

        if (!(impl->pub.flags & EcsQueryHasMonitor)) {
            flecs_query_init_query_monitors(impl);
        }

        /* Check cache entries for changes */
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

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *impl = flecs_query_impl(qit->query);
    ecs_query_t *q = &impl->pub;

    /* First check for changes for terms with fixed sources, if query has any */
    if (q->read_fields & q->fixed_fields) {
        /* Detecting changes for uncached terms is costly, so only do it once 
         * per iteration. */
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
        ecs_query_cache_table_match_t *qm = 
            (ecs_query_cache_table_match_t*)it->priv_.iter.query.prev;
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

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    if (it->instance_count > it->count) {
        qit->skip_count ++;
        if (qit->skip_count == it->instance_count) {
            /* For non-instanced queries, make sure all entities are skipped */
            it->flags |= EcsIterSkip;
        }
    } else {
        it->flags |= EcsIterSkip;
    }
}
