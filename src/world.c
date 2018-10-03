#include <string.h>
#include <inttypes.h>
#include "include/private/reflecs.h"

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
    EcsArray *family = ecs_map_get(world->family_index, family_id);
    result->family_id = family_id;
    ecs_table_init_w_size(world, result, family, sizeof(EcsComponent));
    result->columns = malloc(sizeof(uint16_t));
    result->columns[0] = sizeof(EcsComponent);
    uint32_t table_index = ecs_array_get_index(world->table_db, &table_arr_params, result);
    ecs_map_set64(world->table_index, family_id, table_index + 1);
}

/** Bootstrap the EcsComponent component */
static
EcsResult bootstrap_component(
    EcsWorld *world)
{
    EcsHandle handle = ecs_new(world, 0);
    ecs_stage(world, handle, handle);
    uint64_t family_id = ecs_map_get64(world->staging_index, handle);

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
    EcsHandle handle = ecs_new(world, 0);

    EcsComponent *component_data = ecs_add(world, handle, world->component);
    if (!component_data) {
        abort();
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

    EcsArray *stage_set = ecs_map_get(world->family_index, stage_hash);
    if (!stage_set) {
        stage_set = ecs_array_new_from_buffer(&handle_arr_params, count, new_buffer);
        ecs_map_set(world->family_index, stage_hash, stage_set);
    }

    return stage_hash;
}

static
EcsTable* ecs_world_create_table(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsTable *result = ecs_array_add(&world->table_db, &table_arr_params);
    result->family_id = family_id;

    if (ecs_table_init(world, result) != EcsOk) {
        return NULL;
    }

    uint32_t table_index = ecs_array_get_index(world->table_db, &table_arr_params, result);
    ecs_map_set64(world->table_index, family_id, table_index + 1);

    EcsIter it = ecs_array_iter(world->periodic_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, result);
    }

    it = ecs_array_iter(world->other_systems, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, result);
    }

    return result;
}

EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsFamily family_id)
{
    uint32_t table_index = ecs_map_get64(world->table_index, family_id);
    if (table_index) {
        return ecs_array_get(world->table_db, &table_arr_params, table_index - 1);
    } else {
        return ecs_world_create_table(world, family_id);
    }

    return NULL;
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

static
void ecs_family_print(
    EcsWorld *world,
    EcsFamily family_id)
{
    EcsArray *family = ecs_map_get(world->family_index, family_id);

    EcsIter it = ecs_array_iter(family, &handle_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle h = *(EcsHandle*)ecs_iter_next(&it);
        EcsId *id = ecs_get(world, h, world->id);
        if (id) {
            printf("%s ", id->id);
        }
    }
}

void ecs_dump(
    EcsWorld *world)
{
    printf("Tables (%d)\n", ecs_map_count(world->table_index));
    EcsIter it = ecs_map_iter(world->table_index);
    while (ecs_iter_hasnext(&it)) {
        uint32_t table_index = (uintptr_t)ecs_iter_next(&it);
        EcsTable *table = ecs_array_get(world->table_db, &table_arr_params, table_index - 1);
        printf("[%u] Rows: %u, Components: ", table->family_id, ecs_array_count(table->rows));
        ecs_family_print(world, table->family_id);
        printf("\n");
    }

    printf("\nEntities (%d)\n", ecs_map_count(world->entity_index));
    it = ecs_map_iter(world->entity_index);
    while (ecs_iter_hasnext(&it)) {
        EcsRow row = ecs_to_row((uintptr_t)ecs_iter_next(&it));
        EcsTable *table = ecs_world_get_table(world, row.family_id);
        void *row_ptr = ecs_table_get(table, row.index);
        EcsHandle h = *(EcsHandle*)row_ptr;

        printf("[%u, %u] %" PRIu64 " - ", row.family_id, row.index, h);
        EcsId *id = ecs_get(world, h, world->id);
        if (id) {
            printf("%s", id->id);
        } else {
            printf("...");
        }
        printf("\n");
    }
}

EcsWorld *ecs_init(void) {
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->table_db = ecs_array_new(&table_arr_params, ECS_WORLD_INITIAL_TABLE_COUNT);
    result->periodic_systems = ecs_array_new(&handle_arr_params, ECS_WORLD_INITIAL_PERIODIC_SYSTEM_COUNT);
    result->other_systems = ecs_array_new(&handle_arr_params, ECS_WORLD_INITIAL_OTHER_SYSTEM_COUNT);
    result->entity_index = ecs_map_new(ECS_WORLD_INITIAL_ENTITY_COUNT);
    result->table_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->family_index = ecs_map_new(ECS_WORLD_INITIAL_TABLE_COUNT * 2);
    result->staging_index = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    result->component = 0;
    result->system = 0;
    result->id = 0;
    result->last_handle = 0;

    bootstrap_component(result);

    result->system = init_component(result, sizeof(EcsSystem));
    result->id = init_component(result, sizeof(EcsId));

    init_id(result, result->component, "EcsComponent");
    init_id(result, result->system, "EcsSystem");
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

EcsHandle ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    EcsIter it = ecs_array_iter(world->table_db, &table_arr_params);
    EcsHandle id_component = world->id;

    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);

        if (ecs_table_column_offset(table, id_component) == -1) {
            continue;
        }

        EcsArrayIter iter_data;
        EcsIter row_it = _ecs_array_iter(
            table->rows, &table->row_params, &iter_data);

        while (ecs_iter_hasnext(&it)) {
            void *row = ecs_iter_next(&row_it);
            EcsHandle h = *(EcsHandle*)row;
            EcsId *id_ptr = ecs_get(world, h, id_component);
            if (!strcmp(id_ptr->id, id)) {
                return h;
            }
        }
    }

    return 0;
}

void ecs_progress(
    EcsWorld *world)
{
    EcsIter it = ecs_array_iter(world->periodic_systems, &handle_arr_params);

    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_run_system(world, system, NULL);
    }
}
