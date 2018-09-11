#ifndef REFLECS_PRIVATE_H
#define REFLECS_PRIVATE_H

#include <reflecs/reflecs.h>
#include "types.h"


/* -- Private entity API -- */

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from);


/* -- Private system API -- */

EcsResult ecs_system_notify_create_table(
    EcsEntity *system,
    EcsTable *table);

void ecs_system_run(
    EcsEntity *system);


/* -- Private world API -- */

EcsArray* ecs_world_get_components(
    EcsWorld *world,
    uint64_t components_hash);

uint64_t ecs_world_components_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsEntity *to_add);

EcsTable *ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t components_hash);

EcsTable *ecs_world_create_table(
    EcsWorld *world,
    uint64_t components_hash);


/* -- Private table API -- */

EcsTable* ecs_table_new(
    EcsWorld *world,
    uint64_t components_hash);

EcsTable* ecs_table_new_w_size(
    EcsWorld *world,
    uint64_t components_hash,
    size_t size);

void ecs_table_add_component(
    EcsTable *table,
    EcsEntity *component_type);

void* ecs_table_insert(
    EcsTable *table,
    EcsEntity *entity);

void ecs_table_remove(
    EcsTable *table,
    void *row);

int32_t ecs_table_find_column(
    EcsTable *table,
    EcsEntity *component);

bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components);

void* ecs_table_column(
    EcsTable *table,
    void *row,
    uint32_t column);

size_t ecs_table_column_size(
    EcsTable *table,
    uint32_t column);


#endif
