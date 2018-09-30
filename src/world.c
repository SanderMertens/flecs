#include "include/private/reflecs.h"
#include <string.h>

const EcsArrayParams table_arr_params = {
    .element_size = sizeof(EcsTable)
};

const EcsArrayParams handle_arr_params = {
    .element_size = sizeof(EcsHandle)
};

/** Comparator function for handles */
static
int compare_handle(
    const void *p1,
    const void *p2)
{
    return *(EcsHandle*)p1 - *(EcsHandle*)p2;
}

/** Hash array of handles */
static
uint32_t hash_handle_array(
    EcsHandle* array,
    uint32_t count)
{
    uint32_t hash = 0;
    int i;
    for (i = 0; i < count; i ++) {
        ecs_hash(&array[i], sizeof(EcsHandle), &hash);
    }
    return hash;
}

static
void bootstrap_component_table(
    EcsWorld *world,
    uint64_t family_id)
{
    EcsTable *result = ecs_array_add(&world->table_db, &table_arr_params);
    result->family_id = family_id;
    result->family = ecs_map_get_ptr(world->family_index, family_id);
    result->columns = malloc(sizeof(uintptr_t));
    result->columns[0] = sizeof(EcsComponent);
    result->row_params.element_size = sizeof(EcsComponent) + sizeof(EcsHandle);
    result->rows = ecs_array_new(&result->row_params, ECS_TABLE_INITIAL_ROW_COUNT);
    ecs_map_set_ptr(world->table_index, family_id, result);
}

/** Bootstrap the EcsComponent component */
static
EcsResult bootstrap_component(
    EcsWorld *world)
{
    EcsHandle handle = ecs_new(world);
    ecs_stage(world, handle, handle);
    uint64_t family_id = ecs_map_get(world->staging_index, handle);

    world->component = handle;

    ecs_world_register_family(world, handle, NULL);
    bootstrap_component_table(world, family_id);

    if (ecs_commit(world, world->component) != EcsOk) {
        return EcsError;
    }

    EcsComponent *type_data = ecs_get(world, handle, handle);
    if (!type_data) {
        return EcsError;
    }

    type_data->size = sizeof(EcsComponent);

    return EcsOk;
}

/** Generic function for initializing built-in components */
static
EcsHandle init_component(
    EcsWorld *world,
    size_t size)
{
    EcsHandle handle = ecs_new(world);

    EcsComponent *component_data = ecs_add(world, handle, world->component);
    if (!component_data) {
        return EcsError;
    }

    component_data->size = size;

    return handle;
}

/** Generic function to add identifier to builtin entities */
static
void init_id(
    EcsWorld *world,
    EcsHandle handle,
    const char *id)
{
    EcsId *id_data = ecs_add(world, handle, world->id);
    if (!id_data) {
        abort();
    }
    id_data->id = (char*)id;
}

/* -- Private functions -- */

EcsHandle ecs_world_new_handle(
    EcsWorld *world)
{
    return ++ world->last_handle;
}

EcsFamily ecs_world_register_family(
    EcsWorld *world,
    EcsHandle to_add,
    EcsArray *set)
{
    uint32_t stage_hash = 0;
    uint32_t count = ecs_array_count(set);
    EcsHandle new_set[count + 1];
    void *new_buffer = new_set;

    if (to_add) {
        void *buffer = ecs_array_buffer(set);
        if (count) {
            memcpy(new_set, buffer, sizeof(EcsHandle) * count);
        }
        new_set[count] = to_add;
        qsort(new_set, count + 1, sizeof(EcsHandle), compare_handle);
        stage_hash = hash_handle_array(new_set, count + 1);
        count ++;
    } else if (set) {
        void *buffer = ecs_array_buffer(set);
        stage_hash = hash_handle_array(buffer, count);
        new_buffer = buffer;
    } else {
        return 0;
    }

    EcsArray *stage_set = ecs_map_get_ptr(world->family_index, stage_hash);
    if (!stage_set) {
        stage_set = ecs_array_new_from_buffer(&handle_arr_params, count, new_buffer);
        ecs_map_set_ptr(world->family_index, stage_hash, stage_set);
    }

    return stage_hash;
}

EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsTable *result = ecs_map_get_ptr(world->table_index, family_id);
    if (!result) {
        result = ecs_array_add(&world->table_db, &table_arr_params);
        result->family_id = family_id;
        if (ecs_table_init(world, result) != EcsOk) {
            return NULL;
        }
        ecs_map_set_ptr(world->table_index, family_id, result);
    }

    return result;
}

union RowUnion {
    EcsRow row;
    uint64_t value;
};

EcsRow ecs_to_row(
    uint64_t value)
{
    union RowUnion u = {.value = value};
    return u.row;
}

uint64_t ecs_from_row(
    EcsRow row)
{
    union RowUnion u = {.row = row};
    return u.value;
}


/* -- Public functions -- */

EcsWorld *ecs_init(void) {
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->table_db = ecs_array_new(&table_arr_params, ECS_WORLD_INITIAL_TABLE_COUNT);
    result->entity_index = ecs_map_new(ECS_WORLD_INITIAL_ENTITY_COUNT);
    result->table_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->family_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->staging_index = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    result->component = 0;
    result->system = 0;
    result->id = 0;
    result->last_handle = 0;

    bootstrap_component(result);

    //result->system = init_component(result, sizeof(EcsSystem));
    result->id = init_component(result, sizeof(EcsId));

    //init_id(result, result->system, "EcsSystem");
    init_id(result, result->id, "EcsId");

    return result;
}

EcsResult ecs_fini(
    EcsWorld *world)
{
    ecs_array_free(world->table_db);
    ecs_map_free(world->entity_index);
    ecs_map_free(world->table_index);
    ecs_map_free(world->family_index);
    ecs_map_free(world->staging_index);
    free(world);
    return EcsOk;
}
