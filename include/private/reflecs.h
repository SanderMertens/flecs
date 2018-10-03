#ifndef REFLECS_PRIVATE_H
#define REFLECS_PRIVATE_H

#include "types.h"


/* -- World API -- */

EcsHandle ecs_world_new_handle(
    EcsWorld *world);

EcsFamily ecs_world_register_family(
    EcsWorld *world,
    EcsHandle to_add,
    EcsArray *set);

EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsFamily family_id);


/* -- Table API -- */

EcsResult ecs_table_init(
    EcsWorld *world,
    EcsTable *table);

EcsResult ecs_table_init_w_size(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *family,
    uint32_t size);

uint32_t ecs_table_insert(
    EcsTable *table,
    EcsHandle entity);

void *ecs_table_get(
    EcsTable *table,
    uint32_t index);

uint32_t ecs_table_column_offset(
    EcsTable *table,
    EcsHandle component);

bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components);


/* -- System API -- */

EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsHandle system,
    EcsTable *table);


/* -- Private utilities -- */

void ecs_hash(
    const void *key,
    size_t length,
    uint32_t *result);

EcsRow ecs_to_row(
    uint64_t value);

uint64_t ecs_from_row(
    EcsRow row);

#endif
