/**
 * @file storage/non_fragmenting_childof.h
 * @brief Non-fragmenting storage for hierarchies.
 */

#ifndef FLECS_NON_FRAGMENTING_CHILDOF
#define FLECS_NON_FRAGMENTING_CHILDOF

void flecs_bootstrap_parent_component(
    ecs_world_t *world);

void flecs_on_non_fragmenting_child_move_add(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

void flecs_on_non_fragmenting_child_move_remove(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count);

#endif
