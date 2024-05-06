/**
 * @file world.h
 * @brief World-level API.
 */

#ifndef FLECS_WORLD_H
#define FLECS_WORLD_H

/* Get current stage */
ecs_stage_t* flecs_stage_from_world(
    ecs_world_t **world_ptr);

/* Get current thread-specific stage from readonly world */
ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world);

/* Get component callbacks */
const ecs_type_info_t *flecs_type_info_get(
    const ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks */
ecs_type_info_t* flecs_type_info_ensure(
    ecs_world_t *world,
    ecs_entity_t component);

bool flecs_type_info_init_id(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_size_t size,
    ecs_size_t alignment,
    const ecs_type_hooks_t *li);

#define flecs_type_info_init(world, T, ...)\
    flecs_type_info_init_id(world, ecs_id(T), ECS_SIZEOF(T), ECS_ALIGNOF(T),\
        &(ecs_type_hooks_t)__VA_ARGS__)

void flecs_type_info_fini(
    ecs_type_info_t *ti);

void flecs_type_info_free(
    ecs_world_t *world,
    ecs_entity_t component);

void flecs_eval_component_monitors(
    ecs_world_t *world);

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t id);

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query);

void flecs_monitor_unregister(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query);

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event);

void flecs_register_table(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_unregister_table(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_set_empty(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_process_pending_tables(
    const ecs_world_t *world);

/* Convenience macro's for world allocator */
#define flecs_walloc(world, size)\
    flecs_alloc(&world->allocator, size)
#define flecs_walloc_t(world, T)\
    flecs_alloc_t(&world->allocator, T)
#define flecs_walloc_n(world, T, count)\
    flecs_alloc_n(&world->allocator, T, count)
#define flecs_wcalloc(world, size)\
    flecs_calloc(&world->allocator, size)
#define flecs_wfree_t(world, T, ptr)\
    flecs_free_t(&world->allocator, T, ptr)
#define flecs_wcalloc_n(world, T, count)\
    flecs_calloc_n(&world->allocator, T, count)
#define flecs_wfree(world, size, ptr)\
    flecs_free(&world->allocator, size, ptr)
#define flecs_wfree_n(world, T, count, ptr)\
    flecs_free_n(&world->allocator, T, count, ptr)
#define flecs_wrealloc(world, size_dst, size_src, ptr)\
    flecs_realloc(&world->allocator, size_dst, size_src, ptr)
#define flecs_wrealloc_n(world, T, count_dst, count_src, ptr)\
    flecs_realloc_n(&world->allocator, T, count_dst, count_src, ptr)
#define flecs_wdup(world, size, ptr)\
    flecs_dup(&world->allocator, size, ptr)
#define flecs_wdup_n(world, T, count, ptr)\
    flecs_dup_n(&world->allocator, T, count, ptr)

#endif
