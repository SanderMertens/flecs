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

uint32_t ecs_table_insert(
    EcsTable *table,
    EcsHandle entity);

void *ecs_table_get(
    EcsTable *table,
    uint32_t index);

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
