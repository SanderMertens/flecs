#ifndef REFLECS_PRIVATE_H
#define REFLECS_PRIVATE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"



/* -- Private entity API -- */

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from);

void* ecs_entity_get(
    EcsWorld *world,
    EcsEntity *entity,
    EcsHandle h_component);

/* -- Private component API -- */

EcsArray* ecs_components_diff(
    EcsArray *to,
    EcsArray *from);

bool ecs_components_is_union_empty(
    EcsArray *to,
    EcsArray *from);


/* -- Private system API -- */

EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table);

void ecs_system_notify(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table,
    EcsEntity *entity);

void ecs_run_job(
    EcsWorld *world,
    EcsJob *job);

void ecs_schedule_jobs(
    EcsWorld *world,
    EcsHandle system);

void ecs_run_jobs(
    EcsWorld *world,
    EcsHandle system);


/* -- Private world API -- */

EcsArray* ecs_world_get_components(
    EcsWorld *world,
    uint64_t components_hash);

uint64_t ecs_world_components_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsHandle to_add);

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
    EcsHandle component_type);

void* ecs_table_insert(
    EcsTable *table,
    EcsHandle entity);

void ecs_table_remove(
    EcsTable *table,
    void *row);

int32_t ecs_table_find_column(
    EcsTable *table,
    EcsHandle component);

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

void ecs_table_add_on_init(
    EcsTable *table,
    EcsHandle system);

void ecs_table_add_on_deinit(
    EcsTable *table,
    EcsHandle system);


/* -- Private utilities -- */

void ecs_hash(
    const void *key,
    size_t length,
    uint64_t *result);


#endif
