/**
 * @file query/cache/component_monitor.c
 * @brief Component monitor implementation.
 */

#include "../../private_api.h"

#ifdef FLECS_CACHED_QUERIES

/* Evaluate component monitor. If a monitored entity changed, it will have set a
 * flag in one of the world's component monitors. Queries can register
 * themselves with component monitors to determine whether they need to rematch
 * with tables. */
static
void flecs_eval_component_monitor(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);

    if (!world->monitors.is_dirty) {
        return;
    }

    world->info.eval_comp_monitors_total ++;

    ecs_os_perf_trace_push("flecs.component_monitor.eval");

    world->monitors.is_dirty = false;

    ecs_map_iter_t it = ecs_map_iter(&world->monitors.monitors);
    while (ecs_map_next(&it)) {
        ecs_monitor_t *m = ecs_map_ptr(&it);
        if (!m->is_dirty) {
            continue;
        }

        m->is_dirty = false;

        int32_t i, count = ecs_vec_count(&m->queries);
        ecs_query_t **elems = ecs_vec_first(&m->queries);
        for (i = 0; i < count; i ++) {
            ecs_query_t *q = elems[i];
            flecs_poly_assert(q, ecs_query_t);
            flecs_query_rematch(world, q);
        }
    }

    ecs_os_perf_trace_pop("flecs.component_monitor.eval");
}

static
void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_map_t *monitors = &world->monitors.monitors;

    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    if (ecs_map_is_init(monitors)) {
        ecs_monitor_t *m = ecs_map_get_deref(monitors, ecs_monitor_t, id);
        if (m) {
            if (!world->monitors.is_dirty) {
                world->monitor_generation ++;
            }
            m->is_dirty = true;
            world->monitors.is_dirty = true;
        }
    }
}

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_assert(query, ecs_query_t);

    ecs_map_t *monitors = &world->monitors.monitors;
    ecs_map_init_if(monitors, &world->allocator);
    ecs_monitor_t *m = ecs_map_ensure_alloc_t(monitors, ecs_monitor_t, id);
    ecs_vec_init_if_t(&m->queries, ecs_query_t*);
    ecs_query_t **q = ecs_vec_append_t(
        &world->allocator, &m->queries, ecs_query_t*);
    *q = query;
}

void flecs_monitor_unregister(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_assert(query, ecs_query_t);

    ecs_map_t *monitors = &world->monitors.monitors;
    if (!ecs_map_is_init(monitors)) {
        return;
    }

    ecs_monitor_t *m = ecs_map_get_deref(monitors, ecs_monitor_t, id);
    if (!m) {
        return;
    }

    int32_t i, count = ecs_vec_count(&m->queries);
    ecs_query_t **queries = ecs_vec_first(&m->queries);
    for (i = 0; i < count; i ++) {
        if (queries[i] == query) {
            ecs_vec_remove_t(&m->queries, ecs_query_t*, i);
            count --;
            break;
        }
    }

    if (!count) {
        ecs_vec_fini_t(&world->allocator, &m->queries, ecs_query_t*);
        ecs_map_remove_free(monitors, id);
    }

    if (!ecs_map_count(monitors)) {
        ecs_map_fini(monitors);
    }
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
void flecs_update_component_monitor_w_array(
    ecs_world_t *world,
    ecs_type_t *ids)
{
    if (!ids) {
        return;
    }

    int i;
    for (i = 0; i < ids->count; i ++) {
        ecs_entity_t id = ids->array[i];

        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            flecs_monitor_mark_dirty(world,
                ecs_pair(ECS_PAIR_FIRST(id), EcsWildcard));
        }

        flecs_monitor_mark_dirty(world, id);
    }
}

void flecs_update_component_monitors(
    ecs_world_t *world,
    ecs_type_t *added,
    ecs_type_t *removed)
{
    flecs_update_component_monitor_w_array(world, added);
    flecs_update_component_monitor_w_array(world, removed);
}

void flecs_eval_component_monitors(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_eval_component_monitor(world);
}

#endif
