/**
 * @file type.h
 * @brief Type API.
 *
 * This API contains utilities for working with types. Types are vectors of
 * component ids, and are used most prominently in the API to construct filters.
 */

#ifndef FLECS_TYPE_H
#define FLECS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
char* ecs_type_str(
    const ecs_world_t *world,
    ecs_type_t type);  

FLECS_API
ecs_type_t ecs_type_from_str(
    ecs_world_t *world,
    const char *expr);    

FLECS_API
int32_t ecs_type_index_of(
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id);

FLECS_API
bool ecs_type_has_id(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_id_t id,
    bool owned);

FLECS_API
int32_t ecs_type_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *out);

#ifdef __cplusplus
}
#endif

#endif
