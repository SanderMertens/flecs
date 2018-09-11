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

    entity->stage_hash = ecs_world_components_hash(world, set, component);
}

EcsResult ecs_commit(
    EcsEntity *entity)
{
    EcsWorld *world = entity->world;

    if (entity->stage_hash) {
        if (entity->stage_hash != entity->table_hash) {
            EcsTable *current = NULL;
            if (entity->table_hash) {
                current = ecs_world_lookup_table(world, entity->table_hash);
            }

            EcsTable *table = ecs_world_lookup_table(world, entity->stage_hash);
            if (!table) {
                table = ecs_world_create_table(world, entity->stage_hash);
                if (!table) {
                    return EcsError;
                }
            }

            entity->row = ecs_table_insert(table, entity);
            entity->table_hash = entity->stage_hash;
        }
    }

    return EcsOk;
}

void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component)
{
    if (!entity->table_hash) {
        return NULL;
    }

    EcsWorld *world = entity->world;
    EcsArray *components = ecs_world_get_components(world, entity->table_hash);
    if (!components) {
        return NULL;
    }

    uint32_t column = 0;
    EcsIter it = ecs_array_iter(components, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
        if (e == component) {
            break;
        }
        column ++;
    }

    EcsTable *table = ecs_world_lookup_table(world, entity->table_hash);
    if (!table) {
        return NULL;
    }

    return ECS_OFFSET(entity->row, table->columns[column]);
}

EcsEntity* ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    uint64_t hash = 0;
    ecs_hash(id, strlen(id), &hash);
    return ecs_map_get(world->entities_map, hash);
}

void ecs_init(void)
{
}

void ecs_fini(void)
{
}
