#include "reflecs.h"

EcsEntity *EcsComponent_e;

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

    result->world = world;
    result->stage_hash = 0;
    result->table_hash = 0;

    uint64_t hash = 0;
    ecs_hash(id, strlen(id), &hash);
    ecs_map_set(world->entities_map, hash, result);

    return result;
}

void ecs_stage(
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

void* ecs_add(
    EcsEntity *entity,
    EcsEntity *component)
{
    ecs_stage(entity, component);
    if (ecs_commit(entity) != EcsOk) {
        return NULL;
    }

    return ecs_get(entity, component);
}

static
void ecs_run_init_systems(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity *entity)
{
    EcsArray *new = ecs_world_get_components(world, table->components_hash);
    EcsArray *old = ecs_world_get_components(world, entity->table_hash);
    EcsArray *diff;

    if (!old) {
        diff = new;
    } else {
        diff = ecs_components_diff(new, old);
    }

    if (diff) {
        EcsIter it = ecs_vector_iter(table->init_systems, &entityptr_vec_params);
        while (ecs_iter_hasnext(&it)) {
            EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
            EcsSystem *system_data = ecs_get(e, world->system);

            if (!ecs_components_is_union_empty(diff, system_data->components)) {
                ecs_system_notify(e, table, entity);
            }
        }
    }

    if (diff != new) {
        ecs_array_free(diff);
    }
}

static
void ecs_run_deinit_systems(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity *entity)
{
    EcsArray *old = ecs_world_get_components(world, table->components_hash);
    EcsArray *new = ecs_world_get_components(world, entity->stage_hash);
    EcsArray *diff;

    if (!old) {
        diff = new;
    } else {
        diff = ecs_components_diff(new, old);
    }

    if (diff) {
        EcsIter it = ecs_vector_iter(table->deinit_systems, &entityptr_vec_params);
        while (ecs_iter_hasnext(&it)) {
            EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
            EcsSystem *system_data = ecs_get(e, world->system);

            if (!ecs_components_is_union_empty(diff, system_data->components)) {
                ecs_system_notify(e, table, entity);
            }
        }
    }

    if (diff != new) {
        ecs_array_free(diff);
    }
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
                if (ecs_vector_count(current->deinit_systems)) {
                    ecs_run_deinit_systems(world, current, entity);
                }
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

            if (ecs_vector_count(table->init_systems)) {
                ecs_run_init_systems(world, table, entity);
            }

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

const char* ecs_get_id(
    EcsEntity *entity)
{
    return entity->id;
}

EcsWorld* ecs_get_world(
    EcsEntity *entity)
{
    return entity->world;
}

void ecs_init(void)
{
}

void ecs_fini(void)
{
}
