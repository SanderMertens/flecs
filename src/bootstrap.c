#include "private_api.h"

/* -- Component lifecycle -- */

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

static
void ecs_on_set(EcsIdentifier)(ecs_iter_t *it) {
    EcsIdentifier *ptr = ecs_term(it, EcsIdentifier, 1);
    
    for (int i = 0; i < it->count; i ++) {
        EcsIdentifier *cur = &ptr[i];
        if (cur->value) {
            cur->length = ecs_os_strlen(cur->value);
            cur->hash = flecs_hash(cur->value, cur->length);
        } else {
            cur->length = 0;
            cur->hash = 0;
        }
    }
}

/* Component lifecycle actions for EcsTrigger */
static ECS_CTOR(EcsTrigger, ptr, {
    ptr->trigger = NULL;
})

static ECS_DTOR(EcsTrigger, ptr, {
    if (ptr->trigger) {
        flecs_trigger_fini(world, (ecs_trigger_t*)ptr->trigger);
    }
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
    if (ptr->observer) {
        flecs_observer_fini(world, (ecs_observer_t*)ptr->observer);
    }
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


/* -- Builtin triggers -- */

static
void assert_relation_unused(
    ecs_world_t *world, 
    ecs_entity_t rel,
    ecs_entity_t property)
{
    if (flecs_get_id_record(world, ecs_pair(rel, EcsWildcard)) != NULL) {
        char *r_str = ecs_get_fullpath(world, rel);
        char *p_str = ecs_get_fullpath(world, property);

        ecs_throw(ECS_ID_IN_USE, 
            "cannot add property '%s' to relation '%s': already in use",
            p_str, r_str);
        
        ecs_os_free(r_str);
        ecs_os_free(p_str);
    }

error:
    return;
}

static
void register_final(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        if (flecs_get_id_record(world, ecs_pair(EcsIsA, e)) != NULL) {
            char *e_str = ecs_get_fullpath(world, e);
            ecs_throw(ECS_ID_IN_USE,
                "cannot add property 'Final' to '%s': already inherited from",
                    e_str);
            ecs_os_free(e_str);
        error:
            continue;
        }
    }
}

static
void register_on_delete(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_id_t id = ecs_term_id(it, 1);
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        assert_relation_unused(world, e, EcsOnDelete);

        ecs_id_record_t *r = flecs_ensure_id_record(world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->flags |= ECS_ID_ON_DELETE_FLAG(ECS_PAIR_SECOND(id));

        flecs_add_flag(world, e, ECS_FLAG_OBSERVED_ID);
    }
}

static
void register_on_delete_object(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_id_t id = ecs_term_id(it, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        assert_relation_unused(world, e, EcsOnDeleteObject);

        ecs_id_record_t *r = flecs_ensure_id_record(world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->flags |= ECS_ID_ON_DELETE_OBJECT_FLAG(ECS_PAIR_SECOND(id));

        flecs_add_flag(world, e, ECS_FLAG_OBSERVED_ID);
    }    
}

static
void register_exclusive(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        assert_relation_unused(world, e, EcsExclusive);

        ecs_id_record_t *r = flecs_ensure_id_record(world, e);
        r->flags |= ECS_ID_EXCLUSIVE;
    } 
}

static
void register_dont_inherit(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        assert_relation_unused(world, e, EcsDontInherit);

        ecs_id_record_t *r = flecs_ensure_id_record(world, e);
        r->flags |= ECS_ID_DONT_INHERIT;
    } 
}

static
void on_symmetric_add_remove(ecs_iter_t *it) {
    ecs_entity_t pair = ecs_term_id(it, 1);

    if (!ECS_HAS_ROLE(pair, PAIR)) {
        /* If relationship was not added as a pair, there's nothing to do */
        return;
    }

    ecs_entity_t rel = ECS_PAIR_FIRST(pair);
    ecs_entity_t obj = ECS_PAIR_SECOND(pair);
    ecs_entity_t event = it->event;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t subj = it->entities[i];
        if (event == EcsOnAdd) {
            if (!ecs_has_id(it->real_world, obj, ecs_pair(rel, subj))) {
                ecs_add_pair(it->world, obj, rel, subj);   
            }
        } else {
            if (ecs_has_id(it->real_world, obj, ecs_pair(rel, subj))) {
                ecs_remove_pair(it->world, obj, rel, subj);   
            }
        }
    }
}

static
void register_symmetric(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t r = it->entities[i];
        assert_relation_unused(world, r, EcsSymmetric);

        /* Create trigger that adds the reverse relationship when R(X, Y) is
         * added, or remove the reverse relationship when R(X, Y) is removed. */
        ecs_trigger_init(world, &(ecs_trigger_desc_t) {
            .term.id = ecs_pair(r, EcsWildcard),
            .callback = on_symmetric_add_remove,
            .events = {EcsOnAdd, EcsOnRemove}
        });
    } 
}

static
void on_set_component_lifecycle(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsComponentLifecycle *cl = ecs_term(it, EcsComponentLifecycle, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_set_component_actions_w_id(world, e, &cl[i]);   
    }
}

static
void ensure_module_tag(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_object(world, e, EcsChildOf, 0);
        if (parent) {
            ecs_add_id(world, parent, EcsModule);
        }
    }
}


/* -- Iterable mixins -- */

static
void on_event_iterable_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly, /* Observable */
    ecs_iter_t *it,
    ecs_term_t *filter)
{
    ecs_iter_poly(world, poly, it, filter);
    it->event_id = filter->id;
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

    ecs_column_t *columns = table->storage.columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_ensure(world, entity);
    record->table = table;

    int32_t index = flecs_table_append(world, table, &table->storage, 
        entity, record, false);
    record->row = ECS_ROW_TO_RECORD(index, 0);

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
    /* Before creating table, manually set flags for ChildOf/Identifier, as this
     * can no longer be done after they are in use. */
    ecs_id_record_t *childof_idr = flecs_ensure_id_record(world, EcsChildOf);
    childof_idr->flags |= ECS_ID_ON_DELETE_OBJECT_DELETE;
    childof_idr->flags |= ECS_ID_DONT_INHERIT;

    ecs_id_record_t *ident_idr = flecs_ensure_id_record(
        world, ecs_id(EcsIdentifier));
    ident_idr->flags |= ECS_ID_DONT_INHERIT;

    ecs_id_t entities[] = {
        ecs_id(EcsComponent), 
        EcsFinal,
        ecs_pair(ecs_id(EcsIdentifier), EcsName),
        ecs_pair(ecs_id(EcsIdentifier), EcsSymbol),
        ecs_pair(EcsChildOf, EcsFlecsCore),
        ecs_pair(EcsOnDelete, EcsThrow)
    };
    
    ecs_ids_t array = {
        .array = entities,
        .count = 6
    };

    ecs_table_t *result = flecs_table_find_or_create(world, &array);
    ecs_data_t *data = &result->storage;

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsFirstUserComponentId);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsFirstUserComponentId);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsFirstUserComponentId);
    data->columns[1].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    data->columns[2].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    
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
    ecs_log_push();

    ecs_set_name_prefix(world, "Ecs");

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
    bootstrap_component(world, table, EcsIterable);

    ecs_set_component_actions(world, EcsComponent, { .ctor = ecs_default_ctor });

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

    /* Populate core module */
    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrivate);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsDisabled);

    /* Initialize builtin modules */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);

    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);
    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);

    ecs_set_name(world, EcsFlecsHidden, "hidden");
    ecs_add_id(world, EcsFlecsHidden, EcsModule);
    ecs_add_pair(world, EcsFlecsHidden, EcsChildOf, EcsFlecs);

    /* Initialize builtin entities */
    bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    bootstrap_entity(world, EcsThis, "This", EcsFlecsCore);
    bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    bootstrap_entity(world, EcsAny, "_", EcsFlecsCore);

    /* Component/relationship properties */
    flecs_bootstrap_tag(world, EcsTransitive);
    flecs_bootstrap_tag(world, EcsReflexive);
    flecs_bootstrap_tag(world, EcsSymmetric);
    flecs_bootstrap_tag(world, EcsFinal);
    flecs_bootstrap_tag(world, EcsDontInherit);
    flecs_bootstrap_tag(world, EcsTag);
    flecs_bootstrap_tag(world, EcsExclusive);
    flecs_bootstrap_tag(world, EcsAcyclic);
    flecs_bootstrap_tag(world, EcsWith);

    flecs_bootstrap_tag(world, EcsOnDelete);
    flecs_bootstrap_tag(world, EcsOnDeleteObject);
    flecs_bootstrap_tag(world, EcsRemove);
    flecs_bootstrap_tag(world, EcsDelete);
    flecs_bootstrap_tag(world, EcsThrow);

    flecs_bootstrap_tag(world, EcsDefaultChildComponent);

    /* Builtin relations */
    flecs_bootstrap_tag(world, EcsIsA);
    flecs_bootstrap_tag(world, EcsChildOf);

    /* Builtin events */
    bootstrap_entity(world, EcsOnAdd, "OnAdd", EcsFlecsCore);
    bootstrap_entity(world, EcsOnRemove, "OnRemove", EcsFlecsCore);
    bootstrap_entity(world, EcsOnSet, "OnSet", EcsFlecsCore);
    bootstrap_entity(world, EcsUnSet, "UnSet", EcsFlecsCore);
    bootstrap_entity(world, EcsOnTableEmpty, "OnTableEmpty", EcsFlecsCore);
    bootstrap_entity(world, EcsOnTableFill, "OnTableFilled", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnCreateTable, "OnCreateTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTable, "OnDeleteTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnCreateTrigger, "OnCreateTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTrigger, "OnDeleteTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteObservable, "OnDeleteObservable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnComponentLifecycle, "OnComponentLifecycle", EcsFlecsCore);

    /* Transitive relations are always Acyclic */
    ecs_add_pair(world, EcsTransitive, EcsWith, EcsAcyclic);

    /* Transitive relations */
    ecs_add_id(world, EcsIsA, EcsTransitive);
    ecs_add_id(world, EcsIsA, EcsReflexive);

    /* Tag relations (relations that should never have data) */
    ecs_add_id(world, EcsIsA, EcsTag);
    ecs_add_id(world, EcsChildOf, EcsTag);
    ecs_add_id(world, EcsDefaultChildComponent, EcsTag);

    /* Acyclic relations */
    ecs_add_id(world, EcsIsA, EcsAcyclic);
    ecs_add_id(world, EcsChildOf, EcsAcyclic);
    ecs_add_id(world, EcsWith, EcsAcyclic);

    /* Exclusive properties */
    ecs_add_id(world, EcsChildOf, EcsExclusive);
    ecs_add_id(world, EcsOnDelete, EcsExclusive);
    ecs_add_id(world, EcsOnDeleteObject, EcsExclusive);
    ecs_add_id(world, EcsDefaultChildComponent, EcsExclusive);

    /* Make EcsOnAdd, EcsOnSet events iterable to enable .yield_existing */
    ecs_set(world, EcsOnAdd, EcsIterable, { .init = on_event_iterable_init });
    ecs_set(world, EcsOnSet, EcsIterable, { .init = on_event_iterable_init });

    /* Removal of ChildOf objects (parents) deletes the subject (child) */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteObject, EcsDelete); 

    /* ChildOf, Identifier, Disabled and Prefab should never be inherited */
    ecs_add_id(world, EcsChildOf, EcsDontInherit);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsDontInherit);

    /* The (IsA, *) id record is used often in searches, so cache it */
    world->idr_isa_wildcard = flecs_ensure_id_record(world, 
        ecs_pair(EcsIsA, EcsWildcard));

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsFinal, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_final
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDelete, EcsWildcard), .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_on_delete
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDeleteObject, EcsWildcard), .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_on_delete_object
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsExclusive, .subj.set.mask = EcsSelf  },
        .events = {EcsOnAdd},
        .callback = register_exclusive
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsSymmetric, .subj.set.mask = EcsSelf  },
        .events = {EcsOnAdd},
        .callback = register_symmetric
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsDontInherit, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_dont_inherit
    });

    /* Define trigger to make sure that adding a module to a child entity also
     * adds it to the parent. */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsModule, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = ensure_module_tag
    });

    /* Define trigger for when component lifecycle is set for component */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_id(EcsComponentLifecycle), .subj.set.mask = EcsSelf },
        .events = {EcsOnSet},
        .callback = on_set_component_lifecycle
    });  

    ecs_add_id(world, EcsDisabled, EcsDontInherit);
    ecs_add_id(world, EcsPrefab, EcsDontInherit);

    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_hierarchy(world);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}
