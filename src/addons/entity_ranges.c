/**
 * @file addons/entity_ranges.c
 * @brief Entity id range management.
 */

#include "../private_api.h"

#ifdef FLECS_ENTITY_RANGES

static bool flecs_entity_range_contains(
    const ecs_entity_range_t *range,
    uint32_t id)
{
    return id >= range->min && (!range->max || id <= range->max);
}

static ecs_entity_range_t* flecs_entity_index_find_range(
    const ecs_entity_index_t *index,
    uint32_t id)
{
    ecs_entity_range_t **ranges = ecs_vec_first_t(
        &index->ranges, ecs_entity_range_t*);
    int32_t lo = 0, hi = ecs_vec_count(&index->ranges) - 1;
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

void flecs_entity_ranges_on_ensure(
    ecs_entity_index_t *index,
    uint32_t id)
{
    ecs_entity_range_t *active = index->active_range;
    if (!active || flecs_entity_range_contains(active, id)) {
        index->max_id = id > index->max_id ? id : index->max_id;
        return;
    }

    ecs_entity_range_t *range = flecs_entity_index_find_range(index, id);
    if (range) {
        range->cur = id > range->cur ? id : range->cur;
    }
}

void flecs_entity_ranges_on_delete(
    ecs_entity_index_t *index,
    uint64_t entity,
    ecs_record_t *r)
{
    ecs_entity_range_t *active = index->active_range;
    uint32_t id = (uint32_t)entity;
    if (!active || flecs_entity_range_contains(active, id)) {
        /* No active range or id falls within it, nothing to be done. */
        return;
    }

    /* Entity falls outside of the active range. Remove the recycled id from
     * the dense vector, and move it to the recycled id vector of the correct
     * range. */
    int32_t last = ecs_vec_count(&index->dense) - 1;
    uint64_t *ids = ecs_vec_first_t(&index->dense, uint64_t);
    flecs_entity_index_get_any(index, ids[last])->dense = r->dense;
    ids[r->dense] = ids[last];
    ecs_vec_set_count_t(index->allocator, &index->dense, uint64_t, last);
    r->dense = 0;

    ecs_entity_range_t *range = flecs_entity_index_find_range(index, id);
    if (range) {
        ecs_vec_append_t(
            index->allocator, &range->recycled, uint64_t)[0] =
                ECS_GENERATION_INC(entity);
    }
}

#define flecs_entity_ranges_assert_registered(index, range)\
    ecs_assert(flecs_entity_index_find_range(index, (range)->min) == (range),\
        ECS_INVALID_PARAMETER, "range was not created with ecs_entity_range_new")

void flecs_entity_index_set_range(
    ecs_entity_index_t *index,
    ecs_entity_range_t *range)
{
    flecs_entity_ranges_assert_registered(index, range);

    ecs_allocator_t *a = index->allocator;
    ecs_entity_range_t *prev = index->active_range;
    int32_t alive_count = index->alive_count;
    int32_t dense_count = ecs_vec_count(&index->dense);
    int32_t not_alive_count = dense_count - alive_count;
    uint64_t *ids = ecs_vec_first_t(&index->dense, uint64_t);

    /* Save current not-alive entries to previous range (if any) */
    if (prev) {
        ecs_vec_set_count_t(a, &prev->recycled, uint64_t, not_alive_count);
        if (not_alive_count) {
            ecs_os_memcpy_n(ecs_vec_first(&prev->recycled), &ids[alive_count],
                uint64_t, not_alive_count);
        }

        prev->cur = index->max_id;
    }

    /* Clear not-alive entries from entity index */
    int32_t i;
    for (i = alive_count; i < dense_count; i ++) {
        flecs_entity_index_get_any(index, ids[i])->dense = 0;
    }

    /* Load new range's recycled entries into entity index not-alive section */
    int32_t recycled_count = ecs_vec_count(&range->recycled);
    ecs_vec_set_count_t(a, &index->dense, uint64_t,
        alive_count + recycled_count);
    ids = ecs_vec_first_t(&index->dense, uint64_t);
    uint64_t *src = ecs_vec_first_t(&range->recycled, uint64_t);
    for (i = 0; i < recycled_count; i ++) {
        uint32_t id = (uint32_t)(ids[alive_count + i] = src[i]);
        ecs_record_t *r = &flecs_entity_index_ensure_page(index, id)->records[
            id & FLECS_ENTITY_PAGE_MASK];
        ecs_assert(r->table == NULL, ECS_INTERNAL_ERROR, NULL);
        r->dense = alive_count + i;
    }

    ecs_vec_clear(&range->recycled);

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

    int32_t i, count = ecs_vec_count(&index->ranges);
    ecs_entity_range_t **ranges = ecs_vec_first_t(
        &index->ranges, ecs_entity_range_t*);
    for (i = 0; i < count; i ++) {
        /* Two ranges overlap if one contains the start of the other */
        ecs_check(!flecs_entity_range_contains(ranges[i], min) &&
            (ranges[i]->min < min || (max && ranges[i]->min > max)),
            ECS_INVALID_PARAMETER,
            "range [%u, %u] overlaps with existing range [%u, %u]",
                min, max, ranges[i]->min, ranges[i]->max);
    }

    ecs_entity_range_t *range = flecs_walloc_t(world, ecs_entity_range_t);
    range->min = min;
    range->max = max;
    range->cur = min - 1;
    ecs_vec_init_t(&world->allocator, &range->recycled, uint64_t, 0);

    /* Insert into sorted ranges vec (sorted by min) */
    ecs_vec_append_t(&world->allocator, &index->ranges, ecs_entity_range_t*);
    ranges = ecs_vec_first_t(&index->ranges, ecs_entity_range_t*);
    for (i = count; i > 0 && ranges[i - 1]->min > min; i --) {
        ranges[i] = ranges[i - 1];
    }

    ranges[i] = range;

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

ecs_entity_t ecs_new_from_range(
    ecs_world_t *world,
    const ecs_entity_range_t *range)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(range != NULL, ECS_INVALID_PARAMETER, NULL);

    flecs_stage_from_world(&world);

    flecs_check_exclusive_world_access_write(world);

    ecs_check(!(world->flags & EcsWorldMultiThreaded),
        ECS_INVALID_OPERATION, "cannot create entities in multithreaded mode");

    ecs_entity_index_t *index = ecs_eis(world);
    flecs_entity_ranges_assert_registered(index, range);

    ecs_entity_range_t *r = ECS_CONST_CAST(ecs_entity_range_t*, range);
    ecs_entity_t e;
    if (r == index->active_range) {
        e = flecs_entity_index_new_id(index);
    } else if (ecs_vec_count(&r->recycled)) {
        e = ecs_vec_last_t(&r->recycled, uint64_t)[0];
        ecs_vec_remove_last(&r->recycled);
        flecs_entity_index_ensure(index, e);
    } else {
        ecs_assert(r->cur < (r->max ? r->max : UINT32_MAX),
            ECS_OUT_OF_RANGE, NULL);
        e = ++ r->cur;
        ecs_assert(!flecs_entity_index_exists(index, e), ECS_INVALID_OPERATION,
            "new entity %u id already in use (likely due to overlapping "
                "ranges)", (uint32_t)e);
        flecs_entity_index_ensure(index, e);
    }

    flecs_add_to_root_table(world, e);
    return e;
error:
    return 0;
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
