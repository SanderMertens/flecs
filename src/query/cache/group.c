/**
 * @file query/cache/group.c
 * @brief Adding/removing tables to query groups.
 */

#include "../../private_api.h"

/* Get group id for table. */
static
uint64_t flecs_query_cache_get_group_id(
    const ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    if (cache->group_by_callback) {
        return cache->group_by_callback(cache->query->world, table, 
            cache->group_by, cache->group_by_ctx);
    } else {
        return 0;
    }
}

/* Get group for group id. */
ecs_query_cache_group_t* flecs_query_cache_get_group(
    const ecs_query_cache_t *cache,
    uint64_t group_id)
{
    if (!group_id) {
        return ECS_CONST_CAST(ecs_query_cache_group_t*, &cache->default_group);
    }

    return ecs_map_get_deref(
        &cache->groups, ecs_query_cache_group_t, group_id);
}

/* Insert group in list that's ordered by group id */
static
void flecs_query_cache_group_insert(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group) 
{
    bool desc = false; /* Descending order */
    if (cache->cascade_by) {
        desc = (cache->query->terms[
            cache->cascade_by - 1].src.id & EcsDesc) != 0;
    }

    if (!group->info.id) {
        /* Default group is always inserted */
        return;
    }

    ecs_query_cache_group_t *cur = cache->first_group, *prev = NULL;
    do {
        ecs_assert(cur->info.id != group->info.id, ECS_INTERNAL_ERROR, NULL);
        bool insert = cur->info.id > group->info.id;
        if (desc) {
            insert = !insert; /* Works since ids can't be the same. */
        }

        if (insert) {
            if (prev) {
                prev->next = group;
            } else {
                cache->first_group = group;
            }
            group->next = cur;
            return;
        }

        prev = cur;
    } while ((cur = cur->next));

    prev->next = group;
    ecs_assert(group->next == NULL, ECS_INTERNAL_ERROR, NULL);
}

/* Make sure a group exists for the provided group id. */
static
ecs_query_cache_group_t* flecs_query_cache_ensure_group(
    ecs_query_cache_t *cache,
    uint64_t group_id)
{
    if (!group_id) {
        ecs_query_cache_group_t *group = &cache->default_group;
        if (!group->info.table_count) {
            if (ecs_map_is_init(&cache->groups)) {
                ecs_query_cache_group_t **group_ptr = ecs_map_ensure_ref(
                    &cache->groups, ecs_query_cache_group_t, 0);
                *group_ptr = group;
            }
        }

        if (cache->on_group_create) {
            group->info.ctx = cache->on_group_create(
                cache->query->world, 0, cache->group_by_ctx);
        }

        return group;
    }

    ecs_query_cache_group_t *group = ecs_map_get_deref(&cache->groups, 
        ecs_query_cache_group_t, group_id);

    if (!group) {
        group = ecs_map_insert_alloc_t(&cache->groups, 
            ecs_query_cache_group_t, group_id);
        ecs_os_zeromem(group);

        ecs_allocator_t *a = &cache->query->real_world->allocator;
        if (flecs_query_cache_is_trivial(cache)) {
            ecs_vec_init_t(a, &group->tables, ecs_query_triv_cache_match_t, 0);
        } else {
            ecs_vec_init_t(a, &group->tables, ecs_query_cache_match_t, 0);
        }

        group->info.id = group_id;

        flecs_query_cache_group_insert(cache, group);


        if (cache->on_group_create) {
            group->info.ctx = cache->on_group_create(
                cache->query->world, group_id, cache->group_by_ctx);
        }
    }

    return group;
}

/* Free group resources. */
static
void flecs_query_cache_group_fini(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group)
{
    if (cache->on_group_delete) {
        cache->on_group_delete(cache->query->world, group->info.id,
            group->info.ctx, cache->group_by_ctx);
    }

    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);

    ecs_allocator_t *a = &cache->query->real_world->allocator;
    ecs_vec_fini(a, &group->tables, elem_size);

    if (group != &cache->default_group) {
        ecs_map_remove_free(&cache->groups, group->info.id);
    } else {
        if (ecs_map_is_init(&cache->groups)) {
            ecs_map_remove(&cache->groups, 0);
        }
    }
}

/* Remove group from cache. */
static
void flecs_query_cache_remove_group(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group)
{
    ecs_query_cache_group_t *cur = cache->first_group, *prev = NULL;

    do {
        if (cur == group) {
            if (prev) {
                prev->next = group->next;
            } else {
                cache->first_group = group->next;
            }
            break;
        }
        
        prev = cur;
    } while ((cur = cur->next));
    
    /* If this is the default_group, make sure next is set to NULL since we 
     * never delete the default group. */
    group->next = NULL;

    /* ensure group was found */
    ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!cache->first_group) {
        cache->first_group = &cache->default_group;
    }

    flecs_query_cache_group_fini(cache, group);
}

/* Get cache entry for table. */
ecs_query_cache_table_t* flecs_query_cache_get_table(
    const ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    return ecs_map_get_ptr(&cache->tables, table->id);
}

/* Get match for table cache entry. */
ecs_query_cache_match_t* flecs_query_cache_match_from_table(
    const ecs_query_cache_t *cache,
    const ecs_query_cache_table_t *qt)
{
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    return ecs_vec_get(&qt->group->tables, elem_size, qt->index);
}

/* Add table to query group. */
static
ecs_query_cache_match_t* flecs_query_cache_add_table_to_group(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group,
    ecs_query_cache_table_t *qt,
    ecs_table_t *table)
{
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_allocator_t *a = &cache->query->real_world->allocator;

    ecs_query_cache_match_t *result = ecs_vec_append(
        a, &group->tables, elem_size);
    ecs_os_memset(result, 0, elem_size);
    result->base.table = table;

    qt->group = group;
    qt->index = ecs_vec_count(&group->tables) - 1;

    group->info.table_count ++;
    group->info.match_count ++;
    cache->match_count ++;

    return result;
}

/* Remove table from query group. */
static
void flecs_query_cache_remove_table_from_group(
    ecs_query_cache_t *cache,
    ecs_query_cache_group_t *group,
    int32_t index)
{
    cache->match_count ++;

    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_vec_remove(&group->tables, elem_size, index);
    int32_t count = ecs_vec_count(&group->tables);

    if (!count) {
        flecs_query_cache_remove_group(cache, group);
        return;
    }

    if (index != count) {
        /* The element now points to the previously last table in the group. 
         * Update the entry of that table to the new index. */
        ecs_query_cache_match_t *match = ecs_vec_get(
            &group->tables, elem_size, index);
        ecs_query_cache_table_t *qt_other = flecs_query_cache_get_table(
            cache, match->base.table);
        ecs_assert(qt_other != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(qt_other->index == ecs_vec_count(&group->tables), 
            ECS_INTERNAL_ERROR, NULL);
        qt_other->index = index;
    }

    group->info.table_count --;
    group->info.match_count ++;
}

/* Add matched table to cache. */
ecs_query_cache_match_t* flecs_query_cache_add_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    ecs_assert(ecs_map_get(&cache->tables, table->id) == NULL, 
        ECS_INTERNAL_ERROR, NULL);

    if (!ecs_map_count(&cache->tables) && cache->entity) {
        ecs_remove_id(cache->query->world, cache->entity, EcsEmpty);
    }

    uint64_t group_id = flecs_query_cache_get_group_id(cache, table);

    ecs_query_cache_group_t *group = flecs_query_cache_ensure_group(
        cache, group_id);
    ecs_assert(group != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_allocator_t *a = &cache->query->real_world->allocator;

    ecs_query_cache_table_t *qt = flecs_alloc_t(a, ecs_query_cache_table_t);
    qt->group = group;
    qt->index = ecs_vec_count(&group->tables);
    ecs_map_insert_ptr(&cache->tables, table->id, qt);

    return flecs_query_cache_add_table_to_group(cache, group, qt, table);
}

/* Move table to a different group. This can happen if the value returned by 
 * group_by_callback changed for a table. Typically called during rematch. */
static
void flecs_query_cache_move_table_to_group(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_t *qt,
    ecs_query_cache_group_t *group)
{
    ecs_query_cache_match_t *src_match = 
        flecs_query_cache_match_from_table(cache, qt);

    /* Cache values of previous group since add_table_to_group will modify qt */
    ecs_query_cache_group_t *src_group = qt->group;
    int32_t src_index = qt->index;

    ecs_table_t *table = src_match->base.table;

    /* Add table to new group */
    ecs_query_cache_match_t *dst_match = 
        flecs_query_cache_add_table_to_group(cache, group, qt, table);
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_os_memcpy(dst_match, src_match, elem_size);
    ecs_assert(qt->group == group, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_query_cache_match_from_table(cache, qt) == dst_match, 
        ECS_INTERNAL_ERROR, NULL);
    
    /* Remove table from old group */
    flecs_query_cache_remove_table_from_group(cache, src_group, src_index);
}

/* Make sure a cache entry exists for table. */
ecs_query_cache_match_t* flecs_query_cache_ensure_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    ecs_query_cache_table_t *qt = flecs_query_cache_get_table(cache, table);
    if (qt) {
        /* Ensure existing table is in the right group */
        if (cache->group_by_callback) {
            uint64_t group_id = flecs_query_cache_get_group_id(cache, table);
            ecs_query_cache_group_t *group = flecs_query_cache_ensure_group(
                cache, group_id);
            ecs_assert(group != NULL, ECS_INTERNAL_ERROR, NULL);

            if (group != qt->group) {
                flecs_query_cache_move_table_to_group(cache, qt, group);
            }
        }

        return flecs_query_cache_match_from_table(cache, qt);
    }

    return flecs_query_cache_add_table(cache, table);
}

/* Remove matched table from cache. */
void flecs_query_cache_remove_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    ecs_query_cache_table_t *qt = flecs_query_cache_get_table(cache, table);
    if (!qt) {
        return;
    }

    ecs_query_cache_group_t *group = qt->group;
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);

    ecs_query_cache_match_t *match = ecs_vec_get(
        &group->tables, elem_size, qt->index);

    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(match->base.table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(match->base.table->id == table->id, ECS_INTERNAL_ERROR, NULL);

    flecs_query_cache_match_fini(cache, match);

    flecs_query_cache_remove_table_from_group(cache, group, qt->index);

    ecs_allocator_t *a = &cache->query->real_world->allocator;
    flecs_free_t(a, ecs_query_cache_table_t, qt);

    ecs_map_remove(&cache->tables, table->id);
}

/* Remove all groups from the cache. Typically called during query cleanup. */
static
void flecs_query_cache_remove_all_groups(
    ecs_query_cache_t *cache)
{
    ecs_query_cache_group_t *cur = cache->first_group, *next = cur;

    while ((cur = next)) {
        next = cur->next;
        flecs_query_cache_group_fini(cache, cur);
    }

    cache->first_group = &cache->default_group;
    cache->default_group.next = NULL;
}

/* Remove all tables from the cache. Typically called during query cleanup. */
void flecs_query_cache_remove_all_tables(
    ecs_query_cache_t *cache)
{
    ecs_allocator_t *a = &cache->query->real_world->allocator;
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);

    ecs_query_cache_group_t *cur = cache->first_group;
    do {
        int32_t i, count = ecs_vec_count(&cur->tables);
        for (i = 0; i < count; i ++) {
            flecs_query_cache_match_fini(
                cache, ecs_vec_get(&cur->tables, elem_size, i));
        }

        ecs_vec_fini(a, &cur->tables, elem_size);
    } while ((cur = cur->next));

    ecs_map_iter_t it = ecs_map_iter(&cache->tables);
    while (ecs_map_next(&it)) {
        ecs_query_cache_table_t *qt = ecs_map_ptr(&it);
        flecs_free_t(a, ecs_query_cache_table_t, qt);
    }

    ecs_map_clear(&cache->tables);

    flecs_query_cache_remove_all_groups(cache);

    ecs_assert(ecs_map_count(&cache->groups) == 0, ECS_INTERNAL_ERROR, NULL);
}
