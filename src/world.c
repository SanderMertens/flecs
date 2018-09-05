#include <ecs/vector.h>
#include <ecs/map.h>
#include "world.h"
#include "table.h"
#include "entity.h"

const EcsVectorParams entities_vec_params = {
    .element_size = sizeof(EcsEntity),
    .chunk_count = ECS_INITIAL_CHUNK_COUNT
};

const EcsVectorParams tables_vec_params = {
    .element_size = sizeof(EcsTable),
    .chunk_count = ECS_INITIAL_CHUNK_COUNT
};

EcsWorld* ecs_world_new(
    uint32_t initial_size)
{
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->entities = ecs_vector_new(&entities_vec_params);
    result->entities_map = ecs_map_new(initial_size);
    result->tables = ecs_vector_new(&tables_vec_params);
    result->tables_map = ecs_map_new(ECS_INITIAL_TABLE_COUNT);
    return result;
}

EcsEntity* ecs_world_add_entity(
    EcsWorld *world)
{
    return ecs_vector_add(world->entities, &entities_vec_params);
}

void ecs_world_finalize_entity(
    EcsWorld *world,
    EcsEntity *entity)
{
    ecs_map_set(world->entities_map, entity->id_hash, entity);
}

EcsTable *ecs_world_add_table(
    EcsWorld *world)
{
    return ecs_vector_add(world->tables, &tables_vec_params);
}

void ecs_world_finalize_table(
    EcsWorld *world,
    EcsTable *table)
{
    ecs_map_set(world->tables_map, table->components_hash, table);
}

void ecs_world_remove_table(
    EcsWorld *world,
    EcsTable *table)
{
    ecs_vector_remove(world->tables, &tables_vec_params, &table);
}
