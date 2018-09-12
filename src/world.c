#include "reflecs.h"

const EcsVectorParams entities_vec_params = {
    .element_size = sizeof(EcsEntity),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

const EcsVectorParams tables_vec_params = {
    .element_size = sizeof(EcsTable),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

const EcsVectorParams entityptr_vec_params = {
    .element_size = sizeof(EcsEntity*),
    .chunk_count = REFLECS_INITIAL_CHUNK_COUNT
};

const EcsArrayParams entityptr_arr_params = {
    .element_size = sizeof(EcsEntity*)
};

static
int compare_ptr(
    const void *p1,
    const void *p2)
{
    return *(void**)p1 - *(void**)p2;
}

static
uint64_t hash_entity_array(
    EcsEntity** array,
    uint32_t count)
{
    uint64_t hash = 0;
    int i;
    for (i = 0; i < count; i ++) {
        ecs_hash(array[i], sizeof(void*), &hash);
    }
    return hash;
}

static
EcsResult ecs_world_init_component(
    EcsWorld *world)
{
    uint64_t stage_hash;

    EcsEntity *component_e = ecs_new(world, "EcsComponent");
    world->component = component_e;

    ecs_stage(component_e, component_e);
    stage_hash = component_e->stage_hash;

    EcsTable *table =
        ecs_table_new_w_size(world, stage_hash, sizeof(EcsComponent));

    ecs_map_set(world->tables_map, stage_hash, table);

    if (ecs_commit(world->component) != EcsOk) {
        return EcsError;
    }

    EcsComponent *type_data = ecs_get(component_e, component_e);
    if (!type_data) {
        return EcsError;
    }

    type_data->size = sizeof(EcsComponent);

    return EcsOk;
}

static
EcsResult ecs_world_init_system(
    EcsWorld *world)
{
    EcsEntity *system_e = ecs_new(world, "EcsSystem");
    world->system = system_e;

    EcsComponent *component_data = ecs_add(system_e, world->component);
    if (!component_data) {
        return EcsError;
    }

    component_data->size = sizeof(EcsSystem);

    return EcsOk;
}

static
EcsResult ecs_world_init(
    EcsWorld *world)
{
    if (ecs_world_init_component(world) != EcsOk) {
        return EcsError;
    }

    if (ecs_world_init_system(world) != EcsOk) {
        return EcsError;
    }

    return EcsOk;
}

EcsWorld* ecs_world_new(void)
{
    EcsWorld *result = malloc(sizeof(EcsWorld));
    result->entities = ecs_vector_new(&entities_vec_params);
    result->tables = ecs_vector_new(&tables_vec_params);
    result->systems = ecs_vector_new(&entityptr_vec_params);
    result->entities_map = ecs_map_new(REFLECS_INITIAL_ENTITY_COUNT);
    result->tables_map = ecs_map_new(REFLECS_INITIAL_TABLE_COUNT);
    result->components_map = ecs_map_new(REFLECS_INITIAL_COMPONENT_SET_COUNT);
    result->context = NULL;
    ecs_world_init(result);
    return result;
}

uint64_t ecs_world_components_hash(
    EcsWorld *world,
    EcsArray *set,
    EcsEntity *to_add)
{
    uint64_t stage_hash = 0;
    uint32_t count = ecs_array_count(set);
    EcsEntity *new_set[count + 1];
    void *new_buffer = new_set;

    if (to_add) {
        void *buffer = ecs_array_buffer(set);
        if (count) {
            memcpy(new_set, buffer, sizeof(EcsEntity*) * count);
        }
        new_set[count] = to_add;
        qsort(new_set, count + 1, sizeof(EcsEntity*), compare_ptr);
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
        stage_set = ecs_array_new_from_buffer(count, &entityptr_arr_params, new_buffer);
        ecs_map_set(world->components_map, stage_hash, stage_set);
    }

    return stage_hash;
}

EcsArray* ecs_world_get_components(
    EcsWorld *world,
    uint64_t components_hash)
{
    return ecs_map_get(world->components_map, components_hash);
}

EcsTable *ecs_world_lookup_table(
    EcsWorld *world,
    uint64_t components_hash)
{
    return ecs_map_get(world->tables_map, components_hash);
}

EcsTable *ecs_world_create_table(
    EcsWorld *world,
    uint64_t components_hash)
{
    EcsTable *table = ecs_table_new(world, components_hash);
    if (!table) {
        return NULL;
    }

    ecs_map_set(world->tables_map, components_hash, table);

    EcsIter it = ecs_vector_iter(world->systems, &entityptr_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsEntity *e = *(EcsEntity**)ecs_iter_next(&it);
        ecs_system_notify_create_table(e, table);
    }

    return table;
}

void ecs_world_progress(
    EcsWorld *world)
{
    EcsIter it = ecs_vector_iter(world->systems, &entityptr_vec_params);

    while (ecs_iter_hasnext(&it)) {
        EcsEntity *sys = *(EcsEntity**)ecs_iter_next(&it);
        ecs_system_run(sys);
    }
}

EcsEntity* ecs_world_lookup(
    EcsWorld *world,
    const char *id)
{
    uint64_t hash = 0;
    ecs_hash(id, strlen(id), &hash);
    return ecs_map_get(world->entities_map, hash);
}

void* ecs_world_get_context(
    EcsWorld *world)
{
    return world->context;
}

void ecs_world_set_context(
    EcsWorld *world,
    void *context)
{
    world->context = context;
}

EcsIter _ecs_world_iter_systems(
    EcsWorld *world,
    EcsVectorIter *iter_data)
{
    return _ecs_vector_iter(world->systems, &entityptr_vec_params, iter_data);
}
