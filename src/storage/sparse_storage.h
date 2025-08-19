/**
 * @file storage/sparse_storage.h
 * @brief Sparse component storage.
 */

#ifndef FLECS_SPARSE_STORAGE_H
#define FLECS_SPARSE_STORAGE_H

bool flecs_component_sparse_has(
    ecs_component_record_t *cr,
    ecs_entity_t entity);

void* flecs_component_sparse_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    ecs_entity_t entity);

void* flecs_component_sparse_insert(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row);

void* flecs_component_sparse_emplace(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row);

void flecs_component_sparse_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_table_t *table,
    int32_t row);

void flecs_component_sparse_remove_all(
    ecs_world_t *world,
    ecs_component_record_t *cr);

#endif
