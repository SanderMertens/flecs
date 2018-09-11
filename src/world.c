#include <reflecs/vector.h>
#include <reflecs/map.h>
#include "world.h"
#include "table.h"
#include "entity.h"

static const EcsVectorParams entities_vec_params = {
    .element_size = sizeof(EcsEntity),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

static const EcsVectorParams tables_vec_params = {
    .element_size = sizeof(EcsTable),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

EcsWorld* ecs_world_new(void)
{
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->entities = ecs_vector_new(&entities_vec_params);
    result->entities_map = ecs_map_new(REFLECS_INITIAL_ENTITY_COUNT);
    result->tables = ecs_vector_new(&tables_vec_params);
    result->tables_map = ecs_map_new(REFLECS_INITIAL_TABLE_COUNT);
    return result;
}

EcsEntity* ecs_world_alloc_entity(
    EcsWorld *world)
{
    return ecs_vector_add(world->entities, &entities_vec_params);
}

void ecs_world_add_entity(
    EcsWorld *world,
    EcsEntity *entity)
{
    ecs_map_set(world->entities_map, entity->id_hash, entity);
}

EcsTable *ecs_world_alloc_table(
    EcsWorld *world)
{
    return ecs_vector_add(world->tables, &tables_vec_params);
}

void ecs_world_add_table(
    EcsWorld *world,
    EcsTable *table)
{
    ecs_map_set(world->tables_map, table->components_hash, table);
}

void ecs_world_remove_table(
    EcsWorld *world,
    EcsTable *table)
{
    ecs_map_remove(world->tables_map, table->components_hash);
    ecs_vector_remove(world->tables, &tables_vec_params, table);
}

EcsTable* ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t component_hash)
{
    return ecs_map_get(world->tables_map, component_hash);
}
