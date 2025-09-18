/**
 * @file query/cache/cache_order_by.c
 * @brief Query sorting (order_by) implenentation.
 */

#include "../../private_api.h"

ECS_SORT_TABLE_WITH_COMPARE(_, flecs_query_cache_sort_table_generic, order_by, static)

static
void flecs_query_cache_sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_index,
    ecs_order_by_action_t compare,
    ecs_sort_table_action_t sort)
{
    int32_t count = ecs_table_count(table);
    if (!count) {
        /* Nothing to sort */
        return;
    }
    
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = table->data.entities;
    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_column_t *column = &table->data.columns[column_index];
        ecs_type_info_t *ti = column->ti;
        size = ti->size;
        ptr = column->data;
    }

    if (sort) {
        sort(world, table, entities, ptr, size, 0, count - 1, compare);
    } else {
        flecs_query_cache_sort_table_generic(
            world, table, entities, ptr, size, 0, count - 1, compare);
    }
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_query_cache_match_t *match;
    ecs_entity_t *entities;
    const void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
    bool shared;
} sort_helper_t;

static
const void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->elem_size >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->row >= 0, ECS_INTERNAL_ERROR, NULL);
    if (helper->shared) {
        return helper->ptr;
    } else {
        return ECS_ELEM(helper->ptr, helper->elem_size, helper->row);
    }
}

static
ecs_entity_t e_from_helper(
    sort_helper_t *helper)
{
    if (helper->row < helper->count) {
        return helper->entities[helper->row];
    } else {
        return 0;
    }
}

static
void flecs_query_cache_build_sorted_table_range(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group)
{
    ecs_world_t *world = cache->query->world;
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldMultiThreaded), ECS_UNSUPPORTED,
        "cannot sort query in multithreaded mode");

    ecs_entity_t id = cache->order_by;
    ecs_order_by_action_t compare = cache->order_by_callback;
    int32_t i, table_count = ecs_vec_count(&group->tables);
    if (!table_count) {
        return;
    }

    ecs_vec_init_if_t(&cache->table_slices, ecs_query_cache_match_t);
    int32_t to_sort = 0;
    int32_t order_by_term = cache->order_by_term;

    sort_helper_t *helper = flecs_alloc_n(
        &world->allocator, sort_helper_t, table_count);
    for (i = 0; i < table_count; i ++) {
        ecs_query_cache_match_t *qm = 
            ecs_vec_get_t(&group->tables, ecs_query_cache_match_t, i);
        ecs_table_t *table = qm->base.table;

        if (ecs_table_count(table) == 0) {
            continue;
        }

        if (id) {
            const ecs_term_t *term = &cache->query->terms[order_by_term];
            int32_t field = term->field_index;
            ecs_size_t size = cache->query->sizes[field];
            ecs_entity_t src = qm->_sources[field];
            if (src == 0) {
                int32_t column_index = qm->base.trs[field]->column;
                ecs_column_t *column = &table->data.columns[column_index];
                helper[to_sort].ptr = column->data;
                helper[to_sort].elem_size = size;
                helper[to_sort].shared = false;
            } else {
                ecs_record_t *r = flecs_entities_get(world, src);
                ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

                if (term->src.id & EcsUp) {
                    ecs_entity_t base = 0;
                    ecs_search_relation(world, r->table, 0, id, 
                        EcsIsA, term->src.id & EcsTraverseFlags, &base, 0, 0);
                    if (base && base != src) { /* Component could be inherited */
                        r = flecs_entities_get(world, base);
                    }
                }

                helper[to_sort].ptr = ecs_table_get_id(
                    world, r->table, id, ECS_RECORD_TO_ROW(r->row));
                helper[to_sort].elem_size = size;
                helper[to_sort].shared = true;
            }
            ecs_assert(helper[to_sort].ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(helper[to_sort].elem_size != 0, ECS_INTERNAL_ERROR, NULL);
        } else {
            helper[to_sort].ptr = NULL;
            helper[to_sort].elem_size = 0;
            helper[to_sort].shared = false;
        }

        helper[to_sort].match = qm;
        helper[to_sort].entities = table->data.entities;
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table);
        to_sort ++;      
    }

    if (!to_sort) {
        goto done;
    }

    ecs_query_cache_match_t *cur = NULL;

    bool proceed;
    do {
        int32_t j, min = 0;
        proceed = true;

        ecs_entity_t e1;
        while (!(e1 = e_from_helper(&helper[min]))) {
            min ++;
            if (min == to_sort) {
                proceed = false;
                break;
            }
        }

        if (!proceed) {
            break;
        }

        for (j = min + 1; j < to_sort; j++) {
            ecs_entity_t e2 = e_from_helper(&helper[j]);
            if (!e2) {
                continue;
            }

            const void *ptr1 = ptr_from_helper(&helper[min]);
            const void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
                e1 = e_from_helper(&helper[min]);
            }
        }

        sort_helper_t *cur_helper = &helper[min];
        if (!cur || cur->base.trs != cur_helper->match->base.trs) {
            cur = ecs_vec_append_t(NULL, &cache->table_slices, 
                ecs_query_cache_match_t);
            *cur = *(cur_helper->match);
            cur->_offset = cur_helper->row;
            cur->_count = 1;
        } else {
            cur->_count ++;
        }

        cur_helper->row ++;
    } while (proceed);

done:
    flecs_free_n(&world->allocator, sort_helper_t, table_count, helper);
}

void flecs_query_cache_build_sorted_tables(
    ecs_query_cache_t *cache)
{
    ecs_vec_clear(&cache->table_slices);

    /* Sort tables in group order */
    ecs_query_cache_group_t *cur = &cache->default_group;
    do {
        flecs_query_cache_build_sorted_table_range(cache, cur);
    } while ((cur = cur->next));
}

void flecs_query_cache_sort_tables(
    ecs_world_t *world,
    ecs_query_impl_t *impl)
{
    ecs_query_cache_t *cache = impl->cache;
    ecs_order_by_action_t compare = cache->order_by_callback;
    if (!compare) {
        return;
    }

    ecs_sort_table_action_t sort = cache->order_by_table_callback;
    ecs_entity_t order_by = cache->order_by;
    int32_t order_by_term = cache->order_by_term;
    ecs_component_record_t *cr = flecs_components_get(world, order_by);

    /* Iterate over non-empty tables. Don't bother with empty tables as they
     * have nothing to sort */

    bool tables_sorted = false;

    ecs_query_cache_group_t *cur = &cache->default_group;
    do {
        int32_t i, count = ecs_vec_count(&cur->tables);
        for (i = 0; i < count; i ++) {
            ecs_query_cache_match_t *qm = 
                ecs_vec_get_t(&cur->tables, ecs_query_cache_match_t, i);
            ecs_table_t *table = qm->base.table;
            bool dirty = false;

            if (flecs_query_check_table_monitor(impl, qm, 0)) {
                tables_sorted = true;
                dirty = true;

                if (!ecs_table_count(table)) {
                    /* If table is empty, there's a chance the query won't 
                     * iterate it so update the match monitor here. */
                    flecs_query_sync_match_monitor(impl, qm);

                    ecs_vec_t *matches = qm->wildcard_matches;
                    if (matches) {
                        int32_t j, qms_count = ecs_vec_count(matches);
                        ecs_query_cache_match_t *qms = ecs_vec_first(matches);
                        for (j = 0; j < qms_count; j ++) {
                            flecs_query_sync_match_monitor(impl, &qms[j]);
                        }
                    }
                }
            }

            int32_t column = -1;
            if (order_by) {
                if (flecs_query_check_table_monitor(
                    impl, qm, order_by_term + 1)) 
                {
                    dirty = true;
                }

                if (dirty) {
                    column = -1;

                    const ecs_table_record_t *tr = flecs_component_get_table(
                        cr, table);
                    if (tr) {
                        column = tr->column;
                    }

                    if (column == -1) {
                        /* Component is shared, no sorting is needed */
                        dirty = false;
                    }
                }
            }

            if (!dirty) {
                continue;
            }

            /* Something has changed, sort the table. Prefers using 
            * flecs_query_cache_sort_table when available */
            flecs_query_cache_sort_table(world, table, column, compare, sort);
            tables_sorted = true;
        }
    } while ((cur = cur->next)); /* Next group */

    if (tables_sorted || cache->match_count != cache->prev_match_count) {
        flecs_query_cache_build_sorted_tables(cache);
        cache->match_count ++; /* Increase version if tables changed */
    }
}
