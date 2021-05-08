#include "private_api.h"

/* Global type variables */
ecs_type_t ecs_type(EcsComponent);
ecs_type_t ecs_type(EcsType);
ecs_type_t ecs_type(EcsName);
ecs_type_t ecs_type(EcsQuery);
ecs_type_t ecs_type(EcsTrigger);
ecs_type_t ecs_type(EcsPrefab);

/* Component lifecycle actions for EcsName */
static ECS_CTOR(EcsName, ptr, {
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

static ECS_DTOR(EcsName, ptr, {
    ecs_os_free(ptr->alloc_value);
    ecs_os_free(ptr->symbol);
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

static ECS_COPY(EcsName, dst, src, {
    if (dst->alloc_value) {
        ecs_os_free(dst->alloc_value);
        dst->alloc_value = NULL;
    }

    if (dst->symbol) {
        ecs_os_free(dst->symbol);
        dst->symbol = NULL;
    }
    
    if (src->alloc_value) {
        dst->alloc_value = ecs_os_strdup(src->alloc_value);
        dst->value = dst->alloc_value;
    } else {
        dst->alloc_value = NULL;
        dst->value = src->value;
    }

    if (src->symbol) {
        dst->symbol = ecs_os_strdup(src->symbol);
    }
})

static ECS_MOVE(EcsName, dst, src, {
    if (dst->alloc_value) {
        ecs_os_free(dst->alloc_value);
    }
    if (dst->symbol) {
        ecs_os_free(dst->symbol);
    }

    dst->value = src->value;
    dst->alloc_value = src->alloc_value;
    dst->symbol = src->symbol;

    src->value = NULL;
    src->alloc_value = NULL;
    src->symbol = NULL;
})

/* Component lifecycle actions for EcsTrigger */
static ECS_CTOR(EcsTrigger, ptr, {
    ptr->trigger = NULL;
})

static ECS_DTOR(EcsTrigger, ptr, {
    ecs_trigger_fini(world, (ecs_trigger_t*)ptr->trigger);
})

static ECS_COPY(EcsTrigger, dst, src, {
    ecs_abort(ECS_INVALID_OPERATION, "Trigger component cannot be copied");
})

static ECS_MOVE(EcsTrigger, dst, src, {
    if (dst->trigger) {
        ecs_trigger_fini(world, (ecs_trigger_t*)dst->trigger);
    }
    dst->trigger = src->trigger;
    src->trigger = NULL;
})

static
void register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = ecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        r = ecs_ensure_id_record(it->world, ecs_pair(e, EcsWildcard));
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        ecs_set_watch(it->world, e);
    }
}

static
void register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = ecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete_object = ECS_PAIR_OBJECT(id);

        ecs_set_watch(it->world, e);
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
    const char *id,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = ecs_table_get_or_create_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Create record in entity index */
    ecs_record_t *record = ecs_eis_ensure(world, entity);
    record->table = table;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_append(world, table, data, entity, record, false);
    record->row = index + 1;

    /* Set size and id */
    EcsComponent *c_info = ecs_vector_first(columns[0].data, EcsComponent);
    EcsName *id_data = ecs_vector_first(columns[1].data, EcsName);
    
    c_info[index].size = size;
    c_info[index].alignment = alignment;
    id_data[index].value = &id[ecs_os_strlen("Ecs")]; /* Skip prefix */
    id_data[index].symbol = ecs_os_strdup(id);
    id_data[index].alloc_value = NULL;
}

/** Create type for component */
ecs_type_t ecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_table_find_or_create(world, &(ecs_entities_t){
        .array = (ecs_entity_t[]){entity},
        .count = 1
    });

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Bootstrap types for builtin components and tags */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = ecs_bootstrap_type(world, ecs_id(EcsComponent));
    ecs_type(EcsType) = ecs_bootstrap_type(world, ecs_id(EcsType));
    ecs_type(EcsName) = ecs_bootstrap_type(world, ecs_id(EcsName));
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
    ecs_entity_t entities[] = {ecs_id(EcsComponent), ecs_id(EcsName), ecs_pair(EcsChildOf, EcsFlecsCore)};
    ecs_entities_t array = {
        .array = entities,
        .count = 3
    };

    ecs_table_t *result = ecs_table_find_or_create(world, &array);
    ecs_data_t *data = ecs_table_get_or_create_data(result);

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

static
void bootstrap_entity(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    ecs_entity_t parent)
{
    ecs_set(world, id, EcsName, {.value = name});
    ecs_assert(ecs_get_name(world, id) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_add_pair(world, id, EcsChildOf, parent);

    if (!parent || parent == EcsFlecsCore) {
        ecs_assert(ecs_lookup_fullpath(world, name) == id, 
            ECS_INTERNAL_ERROR, NULL);
    }
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

    bootstrap_component(world, table, EcsName);
    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsQuery);
    bootstrap_component(world, table, EcsTrigger);

    ecs_set_component_actions(world, EcsName, {
        .ctor = ecs_ctor(EcsName),
        .dtor = ecs_dtor(EcsName),
        .copy = ecs_copy(EcsName),
        .move = ecs_move(EcsName)
    });

    ecs_set_component_actions(world, EcsTrigger, {
        .ctor = ecs_ctor(EcsTrigger),
        .dtor = ecs_dtor(EcsTrigger),
        .copy = ecs_copy(EcsTrigger),
        .move = ecs_move(EcsTrigger)
    });        

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

    /* Initialize scopes */
    ecs_set(world, EcsFlecs, EcsName, {.value = "flecs"});
    ecs_add_id(world, EcsFlecs, EcsModule);
    ecs_set(world, EcsFlecsCore, EcsName, {.value = "core"});
    ecs_add_id(world, EcsFlecsCore, EcsModule);
    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);

    /* Initialize builtin entities */
    bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    bootstrap_entity(world, EcsSingleton, "$", EcsFlecsCore);
    bootstrap_entity(world, EcsThis, "This", EcsFlecsCore);
    bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    bootstrap_entity(world, EcsTransitive, "Transitive", EcsFlecsCore);
    bootstrap_entity(world, EcsFinal, "Final", EcsFlecsCore);
    bootstrap_entity(world, EcsIsA, "IsA", EcsFlecsCore);
    bootstrap_entity(world, EcsChildOf, "ChildOf", EcsFlecsCore);
    bootstrap_entity(world, EcsOnDelete, "OnDelete", EcsFlecsCore);
    bootstrap_entity(world, EcsOnDeleteObject, "OnDeleteObject", EcsFlecsCore);
    bootstrap_entity(world, EcsRemove, "Remove", EcsFlecsCore);
    bootstrap_entity(world, EcsDelete, "Delete", EcsFlecsCore);
    bootstrap_entity(world, EcsThrow, "Throw", EcsFlecsCore);

    /* Mark entities as transitive */
    ecs_add_id(world, EcsIsA, EcsTransitive);

    /* Mark entities as final */
    ecs_add_id(world, ecs_id(EcsComponent), EcsFinal);
    ecs_add_id(world, ecs_id(EcsName), EcsFinal);
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

    /* Removal of ChildOf objects (parents) deletes the subject (child) */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteObject, EcsDelete);  

    ecs_set_scope(world, 0);

    ecs_log_pop();
}
