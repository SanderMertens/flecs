#include "private_api.h"

/* Global type variables */
ecs_type_t ecs_type(EcsComponent);
ecs_type_t ecs_type(EcsType);
ecs_type_t ecs_type(EcsName);
ecs_type_t ecs_type(EcsPrefab);

/* Component lifecycle actions for EcsName */
ECS_CTOR(EcsName, ptr, {
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

ECS_DTOR(EcsName, ptr, {
    ecs_os_free(ptr->alloc_value);
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

ECS_COPY(EcsName, dst, src, {
    if (src->alloc_value) {
        dst->alloc_value = ecs_os_strdup(src->alloc_value);
        dst->value = dst->alloc_value;
    } else {
        dst->alloc_value = NULL;
        dst->value = src->value;
    }
    dst->symbol = src->symbol;
})

ECS_MOVE(EcsName, dst, src, {
    dst->value = src->value;
    dst->alloc_value = src->alloc_value;
    dst->symbol = src->symbol;

    src->value = NULL;
    src->alloc_value = NULL;
    src->symbol = NULL;
})


/* -- Bootstrapping -- */

#define bootstrap_component(world, table, name)\
    _bootstrap_component(world, table, ecs_entity(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

void _bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *id,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Create record in entity index */
    ecs_record_t *record = ecs_eis_get_or_create(&world->stage, entity);
    record->table = table;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_append(world, table, data, entity, record);
    record->row = index + 1;

    /* Set size and id */
    EcsComponent *c_info = ecs_vector_first(columns[0].data, EcsComponent);
    EcsName *id_data = ecs_vector_first(columns[1].data, EcsName);
    
    c_info[index].size = size;
    c_info[index].alignment = alignment;
    id_data[index].value = &id[ecs_os_strlen("Ecs")]; /* Skip prefix */
    id_data[index].symbol = id;
    id_data[index].alloc_value = NULL;
}

/** Create type for component */
ecs_type_t ecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_table_find_or_create(world, &world->stage, 
        &(ecs_entities_t){
            .array = (ecs_entity_t[]){entity},
            .count = 1
        }
    );

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Bootstrap types for builtin components and tags */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = ecs_bootstrap_type(world, ecs_entity(EcsComponent));
    ecs_type(EcsType) = ecs_bootstrap_type(world, ecs_entity(EcsType));
    ecs_type(EcsName) = ecs_bootstrap_type(world, ecs_entity(EcsName));
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. 
 * The reason this table is constructed manually is because it requires the size
 * and alignment of the EcsComponent and EcsName components, which haven't been 
 * created yet */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {ecs_entity(EcsComponent), ecs_entity(EcsName), ECS_CHILDOF | EcsFlecsCore};
    ecs_entities_t array = {
        .array = entities,
        .count = 3
    };

    ecs_table_t *result = ecs_table_find_or_create(
        world, &world->stage, &array);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, result);

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsFirstUserComponentId);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsFirstUserComponentId);

    data->columns = ecs_os_malloc(sizeof(ecs_column_t) * 2);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsFirstUserComponentId);
    data->columns[0].size = sizeof(EcsComponent);
    data->columns[0].alignment = ECS_ALIGNOF(EcsComponent);
    data->columns[1].data = ecs_vector_new(EcsName, EcsFirstUserComponentId);
    data->columns[1].size = sizeof(EcsName);
    data->columns[1].alignment = ECS_ALIGNOF(EcsName);

    result->column_count = 2;
    
    return result;
}

void ecs_bootstrap(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = NULL;

    ecs_trace_1("bootstrap core components");
    ecs_log_push();

    /* Create table that will hold components (EcsComponent, EcsName) */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsName);

    world->stats.last_component_id = EcsFirstUserComponentId;
    world->stats.last_id = EcsFirstUserEntityId;
    world->stats.min_id = 0;
    world->stats.max_id = 0;

    bootstrap_types(world);

    ecs_set_scope(world, EcsFlecsCore);

    ecs_bootstrap_tag(world, EcsModule);
    ecs_bootstrap_tag(world, EcsPrefab);
    ecs_bootstrap_tag(world, EcsHidden);
    ecs_bootstrap_tag(world, EcsDisabled);

    ecs_set_component_actions(world, EcsName, {
        .ctor = ecs_ctor(EcsName),
        .dtor = ecs_dtor(EcsName),
        .copy = ecs_copy(EcsName),
        .move = ecs_move(EcsName)
    });

    /* Initialize scopes */
    ecs_set(world, EcsFlecs, EcsName, {.value = "flecs"});
    ecs_add_entity(world, EcsFlecs, EcsModule);
    ecs_set(world, EcsFlecsCore, EcsName, {.value = "core"});
    ecs_add_entity(world, EcsFlecsCore, EcsModule);
    ecs_add_entity(world, EcsFlecsCore, ECS_CHILDOF | EcsFlecs);

    /* Initialize EcsWorld */
    ecs_set(world, EcsWorld, EcsName, {.value = "World"});
    ecs_assert(ecs_get_name(world, EcsWorld) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "World") == EcsWorld, ECS_INTERNAL_ERROR, NULL);
    ecs_add_entity(world, EcsWorld, ECS_CHILDOF | EcsFlecsCore);

    /* Initialize EcsSingleton */
    ecs_set(world, EcsSingleton, EcsName, {.value = "$"});
    ecs_assert(ecs_get_name(world, EcsSingleton) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "$") == EcsSingleton, ECS_INTERNAL_ERROR, NULL);
    ecs_add_entity(world, EcsSingleton, ECS_CHILDOF | EcsFlecsCore);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}
