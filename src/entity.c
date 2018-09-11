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

static
void ecs_entity_copy_row(
    EcsEntity *entity,
    EcsTable *to_table,
    void *to_row,
    EcsTable *from_table,
    void *from_row)
{
    EcsWorld *world = entity->world;
    EcsArray *from_set = ecs_world_get_components(world, entity->table_hash);

    int column = 0, to_column;
    EcsIter it = ecs_array_iter(from_set, &entityptr_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);

        to_column = ecs_table_find_column(to_table, e);
        if (to_column == -1) {
            continue;
        }

        void *to_ptr = ecs_table_column(to_table, to_row, to_column);
        void *from_ptr = ecs_table_column(from_table, from_row, column);
        size_t size = ecs_table_column_size(from_table, column);
        memcpy(to_ptr, from_ptr, size);

        column ++;
    }
}

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from)
{
    entity->row = to;
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

EcsEntity* ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsEntity *result = ecs_new(world, id);
    _ecs_add(result, world->type);
    ecs_commit(result);
    EcsType *type_data = ecs_get(result, world->type);
    type_data->size = size;
    return result;
}

void _ecs_stage(
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

void* _ecs_add(
    EcsEntity *entity,
    EcsEntity *component)
{
    _ecs_stage(entity, component);
    if (ecs_commit(entity) != EcsOk) {
        return NULL;
    }

    return ecs_get(entity, component);
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

            void *row = ecs_table_insert(table, entity);
            if (current) {
                ecs_entity_copy_row(entity, table, row, current, entity->row);
            }

            entity->row = row;
            entity->table_hash = entity->stage_hash;
            entity->stage_hash = 0;
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
