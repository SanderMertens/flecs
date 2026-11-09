#ifndef FLECS_WORLD_GUARD_H
#define FLECS_WORLD_GUARD_H

#if defined(FLECS_REST) || defined(FLECS_SCRIPT)

int32_t flecs_guard_depth(
    const ecs_world_t *world,
    ecs_entity_t entity);

char* flecs_guard_new_path(
    ecs_world_t *world,
    ecs_entity_t scope,
    const char *path,
    const char *sep);

char* flecs_guard_id_valid(
    const ecs_world_t *world,
    ecs_id_t id);

char* flecs_guard_component(
    ecs_world_t *world,
    ecs_id_t id);

char* flecs_guard_add(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

char* flecs_guard_set(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    const void *value);

char* flecs_guard_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id);

char* flecs_guard_delete(
    ecs_world_t *world,
    ecs_entity_t entity);

char* flecs_guard_enable(
    ecs_world_t *world,
    ecs_entity_t entity);

char* flecs_guard_rename(
    ecs_world_t *world,
    ecs_entity_t entity);

#ifdef FLECS_SCRIPT
char* flecs_guard_script(
    ecs_world_t *world,
    ecs_entity_t entity);
#endif

#endif

#endif
