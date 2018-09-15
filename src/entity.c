#include "include/private/reflecs.h"

static EcsHandle handle_server = 0;
EcsEntity *EcsComponent_e;


/* -- Private functions -- */

static
EcsHandle new_handle(void)
{
    return ++ handle_server;
}

static
void ecs_entity_copy_row(
    EcsWorld *world,
    EcsEntity *entity,
    EcsTable *to_table,
    void *to_row,
    EcsTable *from_table,
    void *from_row)
{
    EcsArray *from_set = ecs_world_get_components(world, entity->table_hash);

    int column = 0, to_column;
    EcsIter it = ecs_array_iter(from_set, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle component = *(EcsHandle*)ecs_iter_next(&it);

        to_column = ecs_table_find_column(to_table, component);
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

static
void ecs_run_reactive_systems(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity *entity,
    uint64_t new_hash,
    uint64_t old_hash,
    EcsVector *systems)
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
        EcsIter it = ecs_vector_iter(systems, &handle_vec_params);
        while (ecs_iter_hasnext(&it)) {
            EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
            EcsSystem *system_data = ecs_get(world, system, world->system);

            if (!ecs_components_is_union_empty(diff, system_data->components)) {
                ecs_system_notify(world, system, table, entity);
            }
        }
    }

    if (diff != new) {
        ecs_array_free(diff);
    }
}

static
void ecs_run_init_systems(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity *entity)
{
    ecs_run_reactive_systems(
        world, table, entity, table->components_hash, entity->table_hash,
        table->init_systems);
}

static
void ecs_run_deinit_systems(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity *entity)
{
    ecs_run_reactive_systems(
        world, table, entity, entity->stage_hash, table->components_hash,
        table->deinit_systems);
}


/* -- Internal API -- */

void ecs_entity_move(
    EcsEntity *entity,
    void *to,
    void *from)
{
    entity->row = to;
}

void* ecs_entity_get(
    EcsWorld *world,
    EcsEntity *entity,
    EcsHandle h_component)
{
    bool found = false;
    EcsArray *components = ecs_world_get_components(world, entity->table_hash);
    if (!components) {
        return NULL;
    }

    uint32_t column = 0;
    EcsIter it = ecs_array_iter(components, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        if (h == h_component) {
            found = true;
            break;
        }
        column ++;
    }

    if (!found) {
        return NULL;
    }

    EcsTable *table = ecs_world_lookup_table(world, entity->table_hash);
    if (!table) {
        return NULL;
    }

    return ECS_OFFSET(entity->row, table->columns[column]);
}


/* -- Public API -- */

EcsHandle ecs_new(
    EcsWorld *world)
{
    EcsEntity *result = ecs_vector_add(world->entities, &entities_vec_params);
    EcsHandle h_entity = new_handle();

    result->stage_hash = 0;
    result->table_hash = 0;

    ecs_map_set(world->entities_map, h_entity, result);

    return h_entity;
}

void ecs_delete(
    EcsWorld *world,
    EcsHandle h_entity)
{
    EcsEntity *entity = ecs_map_get(world->entities_map, h_entity);
    if (entity) {
        ecs_vector_remove(world->entities, &entities_vec_params, entity);
    }
    ecs_map_remove(world->entities_map, h_entity);
}

EcsResult ecs_stage(
    EcsWorld *world,
    EcsHandle h_entity,
    EcsHandle h_component)
{
    EcsEntity *entity = ecs_map_get(world->entities_map, h_entity);
    if (!entity) {
        return EcsError;
    }

    EcsArray *set = NULL;

    if (entity->stage_hash) {
        set = ecs_map_get(world->components_map, entity->stage_hash);
    } else if (entity->table_hash) {
        set = ecs_map_get(world->components_map, entity->table_hash);
    }

    entity->stage_hash = ecs_world_components_hash(world, set, h_component);

    return EcsOk;
}

void* ecs_add(
    EcsWorld *world,
    EcsHandle h_entity,
    EcsHandle h_component)
{
    ecs_stage(world, h_entity, h_component);

    if (ecs_commit(world, h_entity) != EcsOk) {
        return NULL;
    }

    return ecs_get(world, h_entity, h_component);
}

EcsResult ecs_commit(
    EcsWorld *world,
    EcsHandle h_entity)
{
    EcsEntity *entity = ecs_map_get(world->entities_map, h_entity);

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

            void *row = ecs_table_insert(table, h_entity);
            if (current) {
                ecs_entity_copy_row(world, entity, table, row, current, entity->row);
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
    EcsWorld *world,
    EcsHandle h_entity,
    EcsHandle h_component)
{
    EcsEntity *entity = ecs_map_get(world->entities_map, h_entity);
    if (!entity->table_hash) {
        return NULL;
    }

    return ecs_entity_get(world, entity, h_component);
}
