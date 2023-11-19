/**
 * @file type.h
 * @brief Type utilities
 */

#ifndef FLECS_TYPE_H
#define FLECS_TYPE_H

int flecs_type_find_insert(
    const ecs_type_t *type,
    int32_t offset,
    ecs_id_t to_add);

/* Find location of id in type */
int flecs_type_find(
    const ecs_type_t *type,
    ecs_id_t id);

/* Count number of matching ids */
int flecs_type_count_matches(
    const ecs_type_t *type,
    ecs_id_t wildcard,
    int32_t offset);

/* Create type from source type with id */
int flecs_type_new_with(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t with);

/* Copy type */
ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src);

/* Create type from source type without ids matching wildcard */
int flecs_type_new_filtered(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t wildcard,
    int32_t at);

/* Create type from source type without id */
int flecs_type_new_without(
    ecs_world_t *world,
    ecs_type_t *dst,
    const ecs_type_t *src,
    ecs_id_t without);

/* Free type */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type);

/* Add to type */
void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add);

#endif
