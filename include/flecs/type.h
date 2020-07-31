/**
 * @file type.h
 * @brief Type API.
 */

#ifndef FLECS_TYPE_H
#define FLECS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_EXPORT
ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity);

FLECS_EXPORT
ecs_entity_t ecs_type_to_entity(
    ecs_world_t *world,
    ecs_type_t type);

FLECS_EXPORT
char* ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type);

FLECS_EXPORT
ecs_type_t ecs_type_from_str(
    ecs_world_t *world,
    const char *expr);    

FLECS_EXPORT
ecs_type_t ecs_type_find(
    ecs_world_t *world,
    ecs_entity_t *array,
    int32_t count);

FLECS_EXPORT
ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t type_add,
    ecs_type_t type_remove);

FLECS_EXPORT
ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity);

FLECS_EXPORT
ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity);

FLECS_EXPORT
bool ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity);

FLECS_EXPORT
bool ecs_type_has_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has);

FLECS_EXPORT
bool ecs_type_owns_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned);

FLECS_EXPORT
bool ecs_type_owns_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned);

FLECS_EXPORT
ecs_entity_t ecs_type_get_entity_for_xor(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t xor_tag);

FLECS_EXPORT
int32_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t component);

FLECS_EXPORT
int32_t ecs_type_trait_index_of(
    ecs_type_t type, 
    int32_t start_index, 
    ecs_entity_t trait);

#ifdef __cplusplus
}
#endif

#endif
