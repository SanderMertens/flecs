#include "include/private/reflecs.h"

const EcsVectorParams entities_vec_params = {
    .element_size = sizeof(EcsEntity),
    .chunk_count = REFLECS_ENTITIES_CHUNK_COUNT
};

const EcsVectorParams tables_vec_params = {
    .element_size = sizeof(EcsTable),
    .chunk_count = REFLECS_TABLES_CHUNK_COUNT
};

const EcsVectorParams handle_vec_params = {
    .element_size = sizeof(EcsEntity*),
    .chunk_count = REFLECS_SYSTEMS_CHUNK_COUNT
};

const EcsArrayParams handle_arr_params = {
    .element_size = sizeof(EcsEntity*)
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
uint64_t hash_entity_array(
    EcsHandle* array,
    uint32_t count)
{
    uint64_t hash = 0;
    int i;
    for (i = 0; i < count; i ++) {
        ecs_hash(&array[i], sizeof(EcsHandle), &hash);
    }
    return hash;
}

/** Bootstrap the EcsComponent component */
static
EcsResult ecs_world_bootstrap_component(
    EcsWorld *world)
{
    EcsHandle component_h = ecs_new(world);
    EcsEntity *component_e = ecs_map_get(world->entities_map, component_h);
    ecs_stage(world, component_h, component_h);
    uint64_t stage_hash = component_e->stage_hash;

    world->component = component_h;

    EcsTable *table =
        ecs_table_new_w_size(world, stage_hash, sizeof(EcsComponent));

    ecs_map_set(world->tables_map, stage_hash, table);

    if (ecs_commit(world, world->component) != EcsOk) {
        return EcsError;
    }

    EcsComponent *type_data = ecs_get(world, component_h, component_h);
    if (!type_data) {
        return EcsError;
    }

    type_data->size = sizeof(EcsComponent);

    return EcsOk;
}

/** Generic function for initializing built-in components */
static
EcsHandle ecs_world_init_component(
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
void ecs_world_init_id(
    EcsWorld *world,
    EcsHandle handle,
    const char *id)
{
    EcsId *id_data = ecs_add(world, handle, world->id);
    id_data->id = (char*)id;
}

/** Initialize world */
static
EcsResult ecs_world_init(
    EcsWorld *world)
{
    if (ecs_world_bootstrap_component(world) != EcsOk) {
        return EcsError;
    }

    world->system = ecs_world_init_component(world, sizeof(EcsSystem));
    world->id = ecs_world_init_component(world, sizeof(EcsId));

    ecs_world_init_id(world, world->system, "EcsSystem");
    ecs_world_init_id(world, world->id, "EcsId");

    return EcsOk;
}

/** Register new component array hash with world */
uint64_t ecs_world_components_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsHandle to_add)
{
    uint64_t stage_hash = 0;
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
        stage_hash = hash_entity_array(new_set, count + 1);
        count ++;
    } else if (set) {
        void *buffer = ecs_array_buffer(set);
        stage_hash = hash_entity_array(buffer, count);
        new_buffer = buffer;
    } else {
        return 0;
    }

    EcsArray *stage_set = ecs_map_get(world->components_map, stage_hash);
    if (!stage_set) {
        stage_set = ecs_array_new_from_buffer(count, &handle_arr_params, new_buffer);
        ecs_map_set(world->components_map, stage_hash, stage_set);
    }

    return stage_hash;
}

/** Get components array */
EcsArray* ecs_world_get_components(
    EcsWorld *world,
    uint64_t components_hash)
{
    return ecs_map_get(world->components_map, components_hash);
}

/** Lookup table with components hash */
EcsTable *ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t components_hash)
{
    return ecs_map_get(world->tables_map, components_hash);
}

/** Create a new table from a components array */
EcsTable *ecs_world_create_table(
    EcsWorld *world,
    uint64_t components_hash)
{
    EcsTable *table = ecs_table_new(world, components_hash);
    if (!table) {
        return NULL;
    }

    ecs_map_set(world->tables_map, components_hash, table);

    EcsIter it = ecs_vector_iter(world->periodic_systems, &handle_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, table);
    }

    it = ecs_vector_iter(world->other_systems, &handle_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);
        ecs_system_notify_create_table(world, system, table);
    }

    return table;
}


/* -- Public functions -- */

EcsWorld* ecs_init(void)
{
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->entities = ecs_vector_new(&entities_vec_params);
    result->tables = ecs_vector_new(&tables_vec_params);
    result->periodic_systems = ecs_vector_new(&handle_vec_params);
    result->other_systems = ecs_vector_new(&handle_vec_params);
    result->worker_threads = NULL;
    result->entities_map = ecs_map_new(REFLECS_INITIAL_ENTITY_COUNT);
    result->tables_map = ecs_map_new(REFLECS_INITIAL_TABLE_COUNT);
    result->components_map = ecs_map_new(REFLECS_INITIAL_COMPONENT_SET_COUNT);
    result->context = NULL;
    ecs_world_init(result);
    return result;
}

void ecs_fini(
    EcsWorld *world)
{
    ecs_vector_free(world->entities);
    ecs_vector_free(world->tables);
    ecs_vector_free(world->periodic_systems);
    ecs_vector_free(world->other_systems);
    ecs_map_free(world->entities_map);
    ecs_map_free(world->tables_map);
    ecs_map_free(world->components_map);
    if (world->worker_threads) {
        ecs_set_threads(world, 0);
    }

    free(world);
}

void ecs_progress(
    EcsWorld *world)
{
    EcsIter it = ecs_vector_iter(world->periodic_systems, &handle_vec_params);
    bool has_threads = ecs_vector_count(world->worker_threads) != 0;

    while (ecs_iter_hasnext(&it)) {
        EcsHandle system = *(EcsHandle*)ecs_iter_next(&it);

        if (has_threads) {
            ecs_schedule_system(world, system);
        } else {
            ecs_run_system(world, system, NULL);
        }
    }
}

void* ecs_get_context(
    EcsWorld *world)
{
    return world->context;
}

void ecs_set_context(
    EcsWorld *world,
    void *context)
{
    world->context = context;
}

EcsHandle ecs_lookup(
    EcsWorld *world,
    const char *id)
{
    EcsIter it = ecs_vector_iter(world->entities, &entities_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = ecs_iter_next(&it);
        EcsId *e_id = ecs_entity_get(world, e, world->id);
        if (e_id && !strcmp(id, e_id->id)) {
            return *(EcsHandle*)e->row;
        }
    }

    return 0;
}
