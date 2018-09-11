#include <reflecs/reflecs.h>
#include <reflecs/vector.h>
#include "entity.h"
#include "table.h"

EcsEntity *EcsType_e;

/* Generate random id */
static
char* random_id(
    uint16_t n)
{
    static char *alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static char *alphanum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890";
    uint16_t i;
    char *result = malloc(n + 1);

    result[0] = alpha[rand() % strlen(alpha)];
    for (i = 1; i < n; i++) {
        result[i] = alphanum[rand() % strlen(alphanum)];
    }

    result[i] = '\0';

    return result;
}

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from)
{

}

EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id)
{
    EcsEntity *result = ecs_vector_add(world->entities, &entities_vec_params);
    if (id) {
        result->id = strdup(id);
    } else {
        result->id = random_id(8);
    }

    result->base = NULL;
    result->world = world;
    result->stage_hash = 0;
    result->table_hash = 0;

    uint64_t hash = 0;
    ecs_hash(id, strlen(id), &hash);
    ecs_map_set(world->entities_map, hash, result);

    return result;
}

void ecs_add(
    EcsEntity *entity,
    EcsEntity *component)
{
    EcsWorld *world = entity->world;
    EcsArray *set = NULL;

    if (entity->stage_hash) {
        set = ecs_map_get(world->components_map, entity->stage_hash);
    } else if (entity->table_hash) {
        set = ecs_map_get(world->components_map, entity->table_hash);
    }

    entity->stage_hash = ecs_world_component_set_hash(world, set, component);
}

void ecs_commit(
    EcsEntity *entity)
{

}

void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component)
{
    return NULL;
}

void ecs_init(void)
{
}

void ecs_fini(void)
{
}
