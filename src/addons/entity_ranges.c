/**
 * @file addons/entity_ranges.c
 * @brief Entity id range management.
 */

#include "../private_api.h"

#ifdef FLECS_ENTITY_RANGES

void flecs_entity_ranges_init(
    ecs_allocator_t *allocator,
    ecs_entity_index_t *index)
{
    index->active_range = NULL;
    ecs_vec_init_t(allocator, &index->ranges, ecs_entity_range_t*, 0);
}

void flecs_entity_ranges_fini(
    ecs_entity_index_t *index)
{
    int32_t r, range_count = ecs_vec_count(&index->ranges);
    ecs_entity_range_t **ranges = ecs_vec_first_t(
        &index->ranges, ecs_entity_range_t*);
    for (r = 0; r < range_count; r ++) {
        ecs_vec_fini_t(index->allocator, &ranges[r]->recycled, uint64_t);
        flecs_free_t(index->allocator, ecs_entity_range_t, ranges[r]);
    }
    ecs_vec_fini_t(index->allocator, &index->ranges, ecs_entity_range_t*);
}

static
void flecs_entity_index_remove_not_alive(
    ecs_entity_index_t *index,
    ecs_record_t *r)
{
    int32_t not_alive = index->alive_count;
    int32_t last = ecs_vec_count(&index->dense) - 1;
    if (not_alive != last) {
        uint64_t *ids = ecs_vec_first_t(&index->dense, uint64_t);
        uint64_t e_last = ids[last];
        ecs_record_t *r_last = flecs_entity_index_get_any(index, e_last);
        r_last->dense = not_alive;
        ids[not_alive] = e_last;
    }
    ecs_vec_set_count_t(index->allocator, &index->dense, uint64_t, last);
    r->dense = 0;
}

static
ecs_entity_range_t* flecs_entity_index_find_range(
    ecs_entity_index_t *index,
    uint32_t id)
{
    int32_t count = ecs_vec_count(&index->ranges);
    if (!count) {
        return NULL;
    }

    ecs_entity_range_t **ranges = ecs_vec_first_t(
        &index->ranges, ecs_entity_range_t*);
    int32_t lo = 0, hi = count - 1;
    while (lo <= hi) {
        int32_t mid = (lo + hi) / 2;
        ecs_entity_range_t *r = ranges[mid];
        if (id < r->min) {
            hi = mid - 1;
        } else if (r->max && id > r->max) {
            lo = mid + 1;
        } else {
            return r;
        }
    }

    return NULL;
}

void flecs_entity_ranges_on_delete(
    ecs_entity_index_t *index,
    uint64_t entity,
    ecs_record_t *r)
{
    ecs_entity_range_t *active = index->active_range;
    if (!active) {
        /* If no entity range is active, we're done. */
        return;
    }

    uint32_t id = (uint32_t)entity;
    if (id >= active->min && (!active->max || id <= active->max)) {
        /* Id falls within the active range, also nothing else to be done. */
        return;
    }

    /* Entity falls outside of the active range. Remove the recycled id from
     * the dense vector, and move it to the recycled id vector of the correct
     * range. */
    flecs_entity_index_remove_not_alive(index, r);

    ecs_entity_range_t *range = flecs_entity_index_find_range(index, id);
    if (range) {
        ecs_vec_append_t(
            index->allocator, &range->recycled, uint64_t)[0] =
                ECS_GENERATION_INC(entity);
    }
}

void flecs_entity_index_set_range(
    ecs_entity_index_t *index,
    ecs_entity_range_t *range)
{
#ifdef FLECS_DEBUG
    /* Verify that the range was created by ecs_entity_range_new */
    {
        int32_t i, count = ecs_vec_count(&index->ranges);
        ecs_entity_range_t **ranges = ecs_vec_first_t(
            &index->ranges, ecs_entity_range_t*);
        bool found = false;
        for (i = 0; i < count; i ++) {
            if (ranges[i] == range) {
                found = true;
                break;
            }
        }
        ecs_assert(found, ECS_INVALID_PARAMETER,
            "range was not created with ecs_entity_range_new");
        (void)found;
    }
#endif

    ecs_allocator_t *a = index->allocator;
    ecs_entity_range_t *prev = index->active_range;
    int32_t alive_count = index->alive_count;
    int32_t dense_count = ecs_vec_count(&index->dense);
    int32_t not_alive_count = dense_count - alive_count;
    uint64_t *ids = ecs_vec_first_t(&index->dense, uint64_t);

    /* Save current not-alive entries to previous range (if any) */
    if (prev) {
        if (not_alive_count > 0) {
            ecs_vec_set_count_t(a, &prev->recycled, uint64_t, not_alive_count);
            uint64_t *dst = ecs_vec_first_t(&prev->recycled, uint64_t);
            ecs_os_memcpy_n(dst, &ids[alive_count], uint64_t, not_alive_count);
        } else {
            ecs_vec_set_count_t(a, &prev->recycled, uint64_t, 0);
        }
        prev->cur = index->max_id;
    }

    /* Clear not-alive entries from entity index */
    {
        int32_t i;
        for (i = alive_count; i < dense_count; i ++) {
            uint32_t id = (uint32_t)ids[i];
            int32_t page_index = (int32_t)(id >> FLECS_ENTITY_PAGE_BITS);
            if (page_index < ecs_vec_count(&index->pages)) {
                ecs_entity_index_page_t *page = ecs_vec_get_t(
                    &index->pages, ecs_entity_index_page_t*, page_index)[0];
                if (page) {
                    page->records[id & FLECS_ENTITY_PAGE_MASK].dense = 0;
                }
            }
        }
        ecs_vec_set_count_t(a, &index->dense, uint64_t, alive_count);
    }

    /* Load new range's recycled entries into entity index not-alive section */
    {
        int32_t recycled_count = ecs_vec_count(&range->recycled);
        if (recycled_count > 0) {
            int32_t new_dense_count = alive_count + recycled_count;
            ecs_vec_set_count_t(a, &index->dense, uint64_t, new_dense_count);
            ids = ecs_vec_first_t(&index->dense, uint64_t);

            uint64_t *src = ecs_vec_first_t(&range->recycled, uint64_t);
            ecs_os_memcpy_n(
                &ids[alive_count], src, uint64_t, recycled_count);

            int32_t i;
            for (i = 0; i < recycled_count; i ++) {
                uint32_t id = (uint32_t)src[i];
                ecs_entity_index_page_t *page =
                    flecs_entity_index_ensure_page(index, id);
                ecs_record_t *r = &page->records[id & FLECS_ENTITY_PAGE_MASK];
                r->dense = alive_count + i;
                ecs_assert(r->table == NULL, ECS_INTERNAL_ERROR, NULL);
            }

            ecs_vec_set_count_t(a, &range->recycled, uint64_t, 0);
        }
    }

    index->max_id = range->cur;
    index->active_range = range;
}

const ecs_entity_range_t* ecs_entity_range_new(
    ecs_world_t *world,
    uint32_t min,
    uint32_t max)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(min > 0, ECS_INVALID_PARAMETER, "min must be > 0");
    ecs_check(!max || max >= min, ECS_INVALID_PARAMETER,
        "max must be >= min or 0");

    /* Validate no overlap with existing ranges */
    ecs_entity_index_t *index = ecs_eis(world);
    ecs_check(flecs_entity_index_not_alive_count(index) == 0,
        ECS_INVALID_OPERATION,
        "cannot create entity range after entities have been deleted");

    bool first_range = ecs_vec_count(&index->ranges) == 0;

    /* If this is the first range and it covers the default [1, ...] id space,
     * it must be large enough to contain all currently alive entities. */
    if (first_range && min == 1) {
        ecs_check(!max || max >= index->max_id, ECS_INVALID_OPERATION,
            "range [%u, %u] cannot be below last alive entity id %u",
                min, max, (uint32_t)index->max_id);
    }

    /* If this is the first range to be created and it does not start at 1,
     * create a default range for the [1, min) id space and activate it, so
     * that the default id allocation behavior is preserved. */
    if (first_range && min > 1) {
        ecs_entity_range_t *default_range = ECS_CONST_CAST(ecs_entity_range_t*,
            ecs_entity_range_new(world, 1, min - 1));
        default_range->cur = index->max_id;
        flecs_entity_index_set_range(index, default_range);
    }

    ecs_allocator_t *a = &world->allocator;

    int32_t count = ecs_vec_count(&index->ranges);
    if (count > 0) {
        ecs_entity_range_t **ranges = ecs_vec_first_t(
            &index->ranges, ecs_entity_range_t*);
        int32_t i;
        for (i = 0; i < count; i ++) {
            ecs_entity_range_t *existing = ranges[i];
            /* Two ranges overlap if one starts before the other ends */
            bool overlap;
            if (!existing->max && !max) {
                overlap = true;
            } else if (!existing->max) {
                overlap = max >= existing->min;
            } else if (!max) {
                overlap = min <= existing->max;
            } else {
                overlap = min <= existing->max && max >= existing->min;
            }
            ecs_check(!overlap, ECS_INVALID_PARAMETER,
                "range [%u, %u] overlaps with existing range [%u, %u]",
                    min, max, existing->min, existing->max);
            (void)overlap;
        }
    }

    ecs_entity_range_t *range = flecs_walloc_t(world, ecs_entity_range_t);
    range->min = min;
    range->max = max;
    range->cur = min - 1;
    ecs_vec_init_t(a, &range->recycled, uint64_t, 0);

    /* Insert into sorted ranges vec (sorted by min) */
    ecs_vec_append_t(a, &index->ranges, ecs_entity_range_t*)[0] = range;

    if (count > 0) {
        ecs_entity_range_t **ranges = ecs_vec_first_t(
            &index->ranges, ecs_entity_range_t*);

        /* Find insertion point and shift elements */
        int32_t i;
        for (i = count; i > 0; i --) {
            if (ranges[i - 1]->min <= min) {
                break;
            }
            ranges[i] = ranges[i - 1];
        }
        ranges[i] = range;
    }

    /* If this is the first range and it covers the default [1, ...] id space,
     * activate it so the default id allocation behavior is preserved. */
    if (first_range && min == 1) {
        range->cur = index->max_id;
        flecs_entity_index_set_range(index, range);
    }

    return range;
error:
    return NULL;
}

void ecs_entity_range_set(
    ecs_world_t *world,
    const ecs_entity_range_t *range)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(range != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_entity_index_set_range(ecs_eis(world),
        ECS_CONST_CAST(ecs_entity_range_t*, range));

error:
    return;
}

const ecs_entity_range_t* ecs_entity_range_get(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return ecs_eis(world)->active_range;
error:
    return NULL;
}

#endif
