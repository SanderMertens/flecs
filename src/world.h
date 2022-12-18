/**
 * @file world.c
 * @brief World-level API.
 */

#ifndef FLECS_WORLD_H
#define FLECS_WORLD_H

/* Get current stage */
ecs_stage_t* flecs_stage_from_world(
    ecs_world_t **world_ptr);

/* Get current thread-specific stage from readonly world */
const ecs_stage_t* flecs_stage_from_readonly_world(
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

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event);

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

/* Suspend/resume readonly state. To fully support implicit registration of
 * components, it should be possible to register components while the world is
 * in readonly mode. It is not uncommon that a component is used first from
 * within a system, which are often ran while in readonly mode.
 * 
 * Suspending readonly mode is only allowed when the world is not multithreaded.
 * When a world is multithreaded, it is not safe to (even temporarily) leave
 * readonly mode, so a multithreaded application should always explicitly
 * register components in advance. 
 * 
 * These operations also suspend deferred mode.
 */
typedef struct ecs_suspend_readonly_state_t {
    bool is_readonly;
    bool is_deferred;
    int32_t defer_count;
    ecs_entity_t scope;
    ecs_entity_t with;
    ecs_vec_t commands;
    ecs_stack_t defer_stack;
    ecs_stage_t *stage;
} ecs_suspend_readonly_state_t;

ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

/* Convenience macro's for world allocator */
#define flecs_walloc(world, size)\
    flecs_alloc(&world->allocator, size)
#define flecs_walloc_n(world, T, count)\
    flecs_alloc_n(&world->allocator, T, count)
#define flecs_wcalloc(world, size)\
    flecs_calloc(&world->allocator, size)
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
