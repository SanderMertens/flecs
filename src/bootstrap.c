#include "private_api.h"

/* Component lifecycle actions for EcsIdentifier */
static ECS_CTOR(EcsIdentifier, ptr, {
    ptr->value = NULL;
    ptr->hash = 0;
    ptr->length = 0;
})

static ECS_DTOR(EcsIdentifier, ptr, {
    ecs_os_strset(&ptr->value, NULL);    
})

static ECS_COPY(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, src->value);
    dst->hash = src->hash;
    dst->length = src->length;
})

static ECS_MOVE(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, NULL);
    dst->value = src->value;
    dst->hash = src->hash;
    dst->length = src->length;

    src->value = NULL;
    src->hash = 0;
    src->length = 0;

})

static ECS_ON_SET(EcsIdentifier, ptr, {
    if (ptr->value) {
        ptr->length = ecs_os_strlen(ptr->value);
        ptr->hash = flecs_hash(ptr->value, ptr->length);
    } else {
        ptr->length = 0;
        ptr->hash = 0;
    }
})

/* Component lifecycle actions for EcsTrigger */
static ECS_CTOR(EcsTrigger, ptr, {
    ptr->trigger = NULL;
})

static ECS_DTOR(EcsTrigger, ptr, {
    flecs_trigger_fini(world, (ecs_trigger_t*)ptr->trigger);
})

static ECS_COPY(EcsTrigger, dst, src, {
    ecs_abort(ECS_INVALID_OPERATION, "Trigger component cannot be copied");
})

static ECS_MOVE(EcsTrigger, dst, src, {
    if (dst->trigger) {
        flecs_trigger_fini(world, (ecs_trigger_t*)dst->trigger);
    }
    dst->trigger = src->trigger;
    src->trigger = NULL;
})

/* Component lifecycle actions for EcsObserver */
static ECS_CTOR(EcsObserver, ptr, {
    ptr->observer = NULL;
})

static ECS_DTOR(EcsObserver, ptr, {
    flecs_observer_fini(world, (ecs_observer_t*)ptr->observer);
})

static ECS_COPY(EcsObserver, dst, src, {
    ecs_abort(ECS_INVALID_OPERATION, "Observer component cannot be copied");
})

static ECS_MOVE(EcsObserver, dst, src, {
    if (dst->observer) {
        flecs_observer_fini(world, (ecs_observer_t*)dst->observer);
    }
    dst->observer = src->observer;
    src->observer = NULL;
})

static
void register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = flecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        r = flecs_ensure_id_record(it->world, ecs_pair(e, EcsWildcard));
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        flecs_set_watch(it->world, e);
    }
}

static
void register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = flecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete_object = ECS_PAIR_OBJECT(id);

        flecs_set_watch(it->world, e);
    }    
}

static
void on_set_component_lifecycle( ecs_iter_t *it) {
    EcsComponentLifecycle *cl = ecs_term(it, EcsComponentLifecycle, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_set_component_actions_w_id(world, e, &cl[i]);   
    }
}

/* -- Bootstrapping -- */

#define bootstrap_component(world, table, name)\
    _bootstrap_component(world, table, ecs_id(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

static
void _bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = flecs_table_get_or_create_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_ensure(world, entity);
    record->table = table;

    int32_t index = flecs_table_append(world, table, data, entity, record, false);
    record->row = index + 1;

    EcsComponent *component = ecs_vector_first(columns[0].data, EcsComponent);
    component[index].size = size;
    component[index].alignment = alignment;

    const char *name = &symbol[3]; /* Strip 'Ecs' */
    ecs_size_t symbol_length = ecs_os_strlen(symbol);
    ecs_size_t name_length = symbol_length - 3;

    EcsIdentifier *name_col = ecs_vector_first(columns[1].data, EcsIdentifier);
    name_col[index].value = ecs_os_strdup(name);
    name_col[index].length = name_length;
    name_col[index].hash = flecs_hash(name, name_length);

    EcsIdentifier *symbol_col = ecs_vector_first(columns[2].data, EcsIdentifier);
    symbol_col[index].value = ecs_os_strdup(symbol);
    symbol_col[index].length = symbol_length;
    symbol_col[index].hash = flecs_hash(symbol, symbol_length);    
}

/** Create type for component */
ecs_type_t flecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = flecs_table_find_or_create(world, &(ecs_ids_t){
        .array = (ecs_entity_t[]){entity},
        .count = 1
    });

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. 
 * The reason this table is constructed manually is because it requires the size
 * and alignment of the EcsComponent and EcsIdentifier components, which haven't
 * been created yet */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {
        ecs_id(EcsComponent), 
        ecs_pair(ecs_id(EcsIdentifier), EcsName),
        ecs_pair(ecs_id(EcsIdentifier), EcsSymbol),
        ecs_pair(EcsChildOf, EcsFlecsCore)
    };
    
    ecs_ids_t array = {
        .array = entities,
        .count = 4
    };

    ecs_table_t *result = flecs_table_find_or_create(world, &array);
    ecs_data_t *data = flecs_table_get_or_create_data(result);

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsFirstUserComponentId);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsFirstUserComponentId);

    data->columns = ecs_os_malloc_n(ecs_column_t, 3);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsFirstUserComponentId);
    data->columns[0].size = ECS_SIZEOF(EcsComponent);
    data->columns[0].alignment = ECS_ALIGNOF(EcsComponent);
    data->columns[1].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    data->columns[1].size = ECS_SIZEOF(EcsIdentifier);
    data->columns[1].alignment = ECS_ALIGNOF(EcsIdentifier);
    data->columns[2].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    data->columns[2].size = ECS_SIZEOF(EcsIdentifier);
    data->columns[2].alignment = ECS_ALIGNOF(EcsIdentifier);

    result->column_count = 3;
    
    return result;
}

static
void bootstrap_entity(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    ecs_entity_t parent)
{
    char symbol[256];
    ecs_os_strcpy(symbol, "flecs.core.");
    ecs_os_strcat(symbol, name);

    ecs_set_name(world, id, name);
    ecs_set_symbol(world, id, symbol);

    ecs_assert(ecs_get_name(world, id) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_add_pair(world, id, EcsChildOf, parent);

    if (!parent || parent == EcsFlecsCore) {
        ecs_assert(ecs_lookup_fullpath(world, name) == id, 
            ECS_INTERNAL_ERROR, NULL);
    }
}

void flecs_bootstrap(
    ecs_world_t *world)
{
    ecs_trace_1("bootstrap core components");
    ecs_log_push();

    /* Create table for initial components */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    bootstrap_component(world, table, EcsIdentifier);
    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsComponentLifecycle);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsQuery);
    bootstrap_component(world, table, EcsTrigger);
    bootstrap_component(world, table, EcsObserver);

    ecs_set_component_actions(world, EcsIdentifier, {
        .ctor = ecs_ctor(EcsIdentifier),
        .dtor = ecs_dtor(EcsIdentifier),
        .copy = ecs_copy(EcsIdentifier),
        .move = ecs_move(EcsIdentifier),
        .on_set = ecs_on_set(EcsIdentifier)
    });

    ecs_set_component_actions(world, EcsTrigger, {
        .ctor = ecs_ctor(EcsTrigger),
        .dtor = ecs_dtor(EcsTrigger),
        .copy = ecs_copy(EcsTrigger),
        .move = ecs_move(EcsTrigger)
    }); 

    ecs_set_component_actions(world, EcsObserver, {
        .ctor = ecs_ctor(EcsObserver),
        .dtor = ecs_dtor(EcsObserver),
        .copy = ecs_copy(EcsObserver),
        .move = ecs_move(EcsObserver)
    });            

    world->stats.last_component_id = EcsFirstUserComponentId;
    world->stats.last_id = EcsFirstUserEntityId;
    world->stats.min_id = 0;
    world->stats.max_id = 0;

    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsHidden);
    flecs_bootstrap_tag(world, EcsDisabled);

    /* Initialize scopes */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);
    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);
    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);

    /* Initialize builtin entities */
    bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    bootstrap_entity(world, EcsThis, "This", EcsFlecsCore);
    bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    bootstrap_entity(world, EcsTransitive, "Transitive", EcsFlecsCore);
    bootstrap_entity(world, EcsFinal, "Final", EcsFlecsCore);
    bootstrap_entity(world, EcsTag, "Tag", EcsFlecsCore);

    bootstrap_entity(world, EcsIsA, "IsA", EcsFlecsCore);
    bootstrap_entity(world, EcsChildOf, "ChildOf", EcsFlecsCore);

    bootstrap_entity(world, EcsOnAdd, "OnAdd", EcsFlecsCore);
    bootstrap_entity(world, EcsOnRemove, "OnRemove", EcsFlecsCore);
    bootstrap_entity(world, EcsOnSet, "OnSet", EcsFlecsCore);
    bootstrap_entity(world, EcsUnSet, "UnSet", EcsFlecsCore);

    bootstrap_entity(world, EcsOnDelete, "OnDelete", EcsFlecsCore);

    // bootstrap_entity(world, EcsOnCreateTable, "OnCreateTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTable, "OnDeleteTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnTableEmpty, "OnTableEmpty", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnTableNonEmpty, "OnTableNonEmpty", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnCreateTrigger, "OnCreateTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTrigger, "OnDeleteTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteObservable, "OnDeleteObservable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnComponentLifecycle, "OnComponentLifecycle", EcsFlecsCore);
    
    bootstrap_entity(world, EcsOnDeleteObject, "OnDeleteObject", EcsFlecsCore);

    bootstrap_entity(world, EcsRemove, "Remove", EcsFlecsCore);
    bootstrap_entity(world, EcsDelete, "Delete", EcsFlecsCore);
    bootstrap_entity(world, EcsThrow, "Throw", EcsFlecsCore);


    /* Transitive relations */
    ecs_add_id(world, EcsIsA, EcsTransitive);

    /* Tag relations (relations that cannot have data) */
    ecs_add_id(world, EcsIsA, EcsTag);
    ecs_add_id(world, EcsChildOf, EcsTag);

    /* Final components/relations */
    ecs_add_id(world, ecs_id(EcsComponent), EcsFinal);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsFinal);
    ecs_add_id(world, EcsTransitive, EcsFinal);
    ecs_add_id(world, EcsFinal, EcsFinal);
    ecs_add_id(world, EcsIsA, EcsFinal);
    ecs_add_id(world, EcsOnDelete, EcsFinal);
    ecs_add_id(world, EcsOnDeleteObject, EcsFinal);


    /* Define triggers for when relationship cleanup rules are assigned */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDelete, EcsWildcard)},
        .callback = register_on_delete,
        .events = {EcsOnAdd}
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDeleteObject, EcsWildcard)},
        .callback = register_on_delete_object,
        .events = {EcsOnAdd}
    });

    /* Define trigger for when component lifecycle is set for component */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_id(EcsComponentLifecycle)},
        .callback = on_set_component_lifecycle,
        .events = {EcsOnSet}
    });  

    /* Removal of ChildOf objects (parents) deletes the subject (child) */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteObject, EcsDelete);  

    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_hierarchy(world);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}
