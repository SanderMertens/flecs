#include "private_api.h"

/* -- Component lifecycle -- */

/* Component lifecycle actions for EcsIdentifier */
static ECS_DTOR(EcsIdentifier, ptr, {
    ecs_os_strset(&ptr->value, NULL);
})

static ECS_COPY(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, src->value);
    dst->hash = src->hash;
    dst->length = src->length;
    dst->index_hash = src->index_hash;
    dst->index = src->index;
})

static ECS_MOVE(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, NULL);
    dst->value = src->value;
    dst->hash = src->hash;
    dst->length = src->length;
    dst->index_hash = src->index_hash;
    dst->index = src->index;

    src->value = NULL;
    src->hash = 0;
    src->index_hash = 0;
    src->index = 0;
    src->length = 0;
})

static
void ecs_on_set(EcsIdentifier)(ecs_iter_t *it) {
    EcsIdentifier *ptr = ecs_term(it, EcsIdentifier, 1);
    
    ecs_world_t *world = it->real_world;
    ecs_entity_t evt = it->event;
    ecs_id_t evt_id = it->event_id;
    ecs_entity_t kind = ECS_PAIR_SECOND(evt_id); /* Name, Symbol, Alias */

    ecs_id_t pair = ecs_childof(0);

    ecs_hashmap_t *name_index = NULL;
    if (kind == EcsSymbol) {
        name_index = &world->symbols;
    } else if (kind == EcsAlias) {
        name_index = &world->aliases;
    } else if (kind == EcsName) {
        ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_search(world, it->table, ecs_childof(EcsWildcard), &pair);
        ecs_assert(pair != 0, ECS_INTERNAL_ERROR, NULL);

        if (evt == EcsOnSet) {
            name_index = flecs_ensure_id_name_index(world, pair);
        } else {
            name_index = flecs_get_id_name_index(world, pair);
        }
    }

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        EcsIdentifier *cur = &ptr[i];
        uint64_t hash;
        ecs_size_t len;
        const char *name = cur->value;

        if (cur->index && cur->index != name_index) {
            /* If index doesn't match up, the value must have been copied from
             * another entity, so reset index & cached index hash */
            cur->index = NULL;
            cur->index_hash = 0;
        }

        if (cur->value && (evt == EcsOnSet)) {
            len = cur->length = ecs_os_strlen(name);
            hash = cur->hash = flecs_hash(name, len);
        } else {
            len = cur->length = 0;
            hash = cur->hash = 0;
            cur->index = NULL;
        }

        if (name_index) {
            uint64_t index_hash = cur->index_hash;
            ecs_entity_t e = it->entities[i];

            if (hash != index_hash) {
                if (index_hash) {
                    flecs_name_index_remove(name_index, e, index_hash);
                }
                if (hash) {
                    flecs_name_index_ensure(name_index, e, name, len, hash);
                    cur->index_hash = hash;
                    cur->index = name_index;
                }
            } else {
                /* Name didn't change, but the string could have been 
                 * reallocated. Make sure name index points to correct string */
                flecs_name_index_update_name(name_index, e, hash, name);
            }
        }
    }
}

/* Component lifecycle actions for EcsTrigger */
static void ecs_on_remove(EcsTrigger)(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsTrigger *ptr = ecs_term(it, EcsTrigger, 1);
    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        if (ptr[i].trigger) {
            flecs_trigger_fini(world, (ecs_trigger_t*)ptr[i].trigger);
        }
    }
}

/* Component lifecycle actions for EcsObserver */
static void ecs_on_remove(EcsObserver)(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsObserver *ptr = ecs_term(it, EcsObserver, 1);
    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        if (ptr[i].observer) {
            flecs_observer_fini(world, (ecs_observer_t*)ptr[i].observer);
        }
    }
}

/* -- Builtin triggers -- */

static
void assert_relation_unused(
    ecs_world_t *world, 
    ecs_entity_t rel,
    ecs_entity_t property)
{
    if (world->is_fini) {
        return;
    }
    if (ecs_id_in_use(world, ecs_pair(rel, EcsWildcard))) {
        char *r_str = ecs_get_fullpath(world, rel);
        char *p_str = ecs_get_fullpath(world, property);

        ecs_throw(ECS_ID_IN_USE, 
            "cannot change property '%s' for relation '%s': already in use",
            p_str, r_str);
        
        ecs_os_free(r_str);
        ecs_os_free(p_str);
    }

error:
    return;
}

static
bool set_id_flag(
    ecs_id_record_t *idr, 
    ecs_flags32_t flag)
{
    if (!(idr->flags & flag)) {
        idr->flags |= flag;
        return true;
    }
    return false;
}

static
bool unset_id_flag(
    ecs_id_record_t *idr, 
    ecs_flags32_t flag)
{
    if ((idr->flags & flag)) {
        idr->flags &= ~flag;
        return true;
    }
    return false;
}

static
void register_id_flag_for_relation(
    ecs_iter_t *it,
    ecs_entity_t prop,
    ecs_flags32_t flag,
    ecs_flags32_t not_flag,
    ecs_flags32_t entity_flag)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        bool changed = false;

        if (event == EcsOnAdd) {
            ecs_id_record_t *idr = flecs_ensure_id_record(world, e);
            changed |= set_id_flag(idr, flag);
            idr = flecs_ensure_id_record(world, ecs_pair(e, EcsWildcard));
            do {
                changed |= set_id_flag(idr, flag);
            } while ((idr = idr->first.next));
            if (entity_flag) flecs_add_flag(world, e, entity_flag);
        } else if (event == EcsOnRemove) {
            ecs_id_record_t *idr = flecs_get_id_record(world, e);
            if (idr) changed |= unset_id_flag(idr, not_flag);
            idr = flecs_get_id_record(world, ecs_pair(e, EcsWildcard));
            if (idr) {
                do {
                    changed |= unset_id_flag(idr, not_flag);
                } while ((idr = idr->first.next));
            }
        }

        if (changed) {
            assert_relation_unused(world, e, prop);
        }
    }
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
                "cannot change property 'Final' for '%s': already inherited from",
                    e_str);
            ecs_os_free(e_str);
        error:
            continue;
        }
    }
}

static
void register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    register_id_flag_for_relation(it, EcsOnDelete, 
        ECS_ID_ON_DELETE_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteMask,
        EcsEntityObservedId);
}

static
void register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    register_id_flag_for_relation(it, EcsOnDeleteObject, 
        ECS_ID_ON_DELETE_OBJECT_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteObjectMask,
        EcsEntityObservedId);  
}

static
void register_acyclic(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsAcyclic, EcsIdAcyclic, 
        EcsIdAcyclic, 0);
}

static
void register_tag(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsTag, EcsIdTag, ~EcsIdTag, 0);

    /* Ensure that all id records for tag have type info set to NULL */
    ecs_world_t *world = it->real_world;
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (it->event == EcsOnAdd) {
            ecs_id_record_t *idr = flecs_get_id_record(world, 
                ecs_pair(e, EcsWildcard));
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            do {
                if (idr->type_info != NULL) {
                    assert_relation_unused(world, e, EcsTag);
                }
                idr->type_info = NULL;
            } while ((idr = idr->first.next));
        }
    }
}

static
void register_exclusive(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsExclusive, EcsIdExclusive, 
        EcsIdExclusive, 0);
}

static
void register_dont_inherit(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsDontInherit, 
        EcsIdDontInherit, EcsIdDontInherit, 0);
}

static
void register_with(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsWith, EcsIdWith, 0, 0);
}

static
void register_union(ecs_iter_t *it) {
    register_id_flag_for_relation(it, EcsUnion, EcsIdUnion, 0, 0);
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
void on_set_component(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsComponent *c = ecs_term(it, EcsComponent, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        if (flecs_init_type_info_id(world, e, c[i].size, c[i].alignment, NULL)){
            assert_relation_unused(world, e, ecs_id(EcsComponent));
        }
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

/* -- Triggers for keeping hashed ids in sync -- */

static
void on_parent_change(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_table_t *other_table = it->other_table, *table = it->table;

    int32_t col = ecs_search(it->real_world, table, 
        ecs_pair(ecs_id(EcsIdentifier), EcsName), 0);
    bool has_name = col != -1;
    bool other_has_name = ecs_search(it->real_world, other_table,
        ecs_pair(ecs_id(EcsIdentifier), EcsName), 0) != -1;

    if (!has_name && !other_has_name) {
        /* If tables don't have names, index does not need to be updated */
        return;
    }

    ecs_id_t to_pair = it->event_id;
    ecs_id_t from_pair = ecs_childof(0);

    /* Find the other ChildOf relationship */
    ecs_search(it->real_world, other_table,
        ecs_pair(EcsChildOf, EcsWildcard), &from_pair);

    bool to_has_name = has_name, from_has_name = other_has_name;
    if (it->event == EcsOnRemove) {
        if (from_pair != ecs_childof(0)) {
            /* Because ChildOf is an exclusive relationship, events always come
             * in OnAdd/OnRemove pairs (add for the new, remove for the old 
             * parent). We only need one of those events, so filter out the
             * OnRemove events except for the case where a parent is removed and
             * not replaced with another parent. */
            return;
        }

        ecs_id_t temp = from_pair;
        from_pair = to_pair;
        to_pair = temp;

        to_has_name = other_has_name;
        from_has_name = has_name;
    }

    /* Get the table column with names */
    const EcsIdentifier *names = ecs_iter_column(it, EcsIdentifier, col);

    ecs_hashmap_t *from_index = 0;
    if (from_has_name) {
        from_index = flecs_get_id_name_index(world, from_pair);
    }
    ecs_hashmap_t *to_index = NULL;
    if (to_has_name) {
        to_index = flecs_ensure_id_name_index(world, to_pair);
    }

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        const EcsIdentifier *name = &names[i];

        uint64_t index_hash = name->index_hash;
        if (from_index && index_hash) {
            flecs_name_index_remove(from_index, e, index_hash);
        }
        const char *name_str = name->value;
        if (to_index && name_str) {
            ecs_assert(name->hash != 0, ECS_INTERNAL_ERROR, NULL);
            flecs_name_index_ensure(
                to_index, e, name_str, name->length, name->hash);
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

    ecs_column_t *columns = table->data.columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_ensure(world, entity);
    record->table = table;

    int32_t index = flecs_table_append(world, table, entity, record, false);
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
    name_col[index].index_hash = 0;
    name_col[index].index = NULL;

    EcsIdentifier *symbol_col = ecs_vector_first(columns[2].data, EcsIdentifier);
    symbol_col[index].value = ecs_os_strdup(symbol);
    symbol_col[index].length = symbol_length;
    symbol_col[index].hash = flecs_hash(symbol, symbol_length);    
    symbol_col[index].index_hash = 0;
    symbol_col[index].index = NULL;
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
    /* Before creating the table, ensure component ids are alive */
    ecs_ensure(world, ecs_id(EcsComponent));
    ecs_ensure(world, EcsFinal);
    ecs_ensure(world, ecs_id(EcsIdentifier));
    ecs_ensure(world, EcsName);
    ecs_ensure(world, EcsSymbol);
    ecs_ensure(world, EcsAlias);
    ecs_ensure(world, EcsChildOf);
    ecs_ensure(world, EcsFlecsCore);
    ecs_ensure(world, EcsOnDelete);
    ecs_ensure(world, EcsPanic);
    ecs_ensure(world, EcsWildcard);
    ecs_ensure(world, EcsAny);

    /* Before creating table, manually set flags for ChildOf/Identifier, as this
     * can no longer be done after they are in use. */
    ecs_id_record_t *idr = flecs_ensure_id_record(world, EcsChildOf);
    idr->flags |= EcsIdOnDeleteObjectDelete | EcsIdDontInherit |
        EcsIdAcyclic | EcsIdTag;
    idr = flecs_ensure_id_record(world, ecs_pair(EcsChildOf, EcsWildcard));
    idr->flags |= EcsIdOnDeleteObjectDelete | EcsIdDontInherit |
        EcsIdAcyclic | EcsIdTag | EcsIdExclusive;

    idr = flecs_ensure_id_record(
        world, ecs_pair(ecs_id(EcsIdentifier), EcsWildcard));
    idr->flags |= EcsIdDontInherit;

    world->idr_childof_0 = flecs_ensure_id_record(world, 
        ecs_pair(EcsChildOf, 0));

    ecs_id_t ids[] = {
        ecs_id(EcsComponent), 
        EcsFinal,
        ecs_pair(ecs_id(EcsIdentifier), EcsName),
        ecs_pair(ecs_id(EcsIdentifier), EcsSymbol),
        ecs_pair(EcsChildOf, EcsFlecsCore),
        ecs_pair(EcsOnDelete, EcsPanic)
    };
    
    ecs_type_t array = {
        .array = ids,
        .count = 6
    };

    ecs_table_t *result = flecs_table_find_or_create(world, &array);
    ecs_data_t *data = &result->data;

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
    
    ecs_add_pair(world, id, EcsChildOf, parent);
    ecs_set_name(world, id, name);
    ecs_set_symbol(world, id, symbol);

    ecs_assert(ecs_get_name(world, id) != NULL, ECS_INTERNAL_ERROR, NULL);

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

    ecs_ensure(world, EcsWildcard);
    ecs_ensure(world, EcsAny);

    /* Bootstrap builtin components */
    flecs_init_type_info(world, EcsComponent, { 
        .ctor = ecs_default_ctor 
    });

    flecs_init_type_info(world, EcsIdentifier, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(EcsIdentifier),
        .copy = ecs_copy(EcsIdentifier),
        .move = ecs_move(EcsIdentifier),
        .on_set = ecs_on_set(EcsIdentifier),
        .on_remove = ecs_on_set(EcsIdentifier)
    });

    flecs_init_type_info(world, EcsTrigger, {
        .ctor = ecs_default_ctor,
        .on_remove = ecs_on_remove(EcsTrigger)
    });

    flecs_init_type_info(world, EcsObserver, {
        .ctor = ecs_default_ctor,
        .on_remove = ecs_on_remove(EcsObserver)
    });

    flecs_init_type_info(world, EcsComponentLifecycle, { 0 });
    flecs_init_type_info(world, EcsType, { 0 });
    flecs_init_type_info(world, EcsQuery, { 0 });
    flecs_init_type_info(world, EcsIterable, { 0 });

    /* Cache often used id records on world */
    world->idr_wildcard = flecs_ensure_id_record(world, EcsWildcard);
    world->idr_wildcard_wildcard = flecs_ensure_id_record(world, 
        ecs_pair(EcsWildcard, EcsWildcard));
    world->idr_any = flecs_ensure_id_record(world, EcsAny);

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

    world->info.last_component_id = EcsFirstUserComponentId;
    world->info.last_id = EcsFirstUserEntityId;
    world->info.min_id = 0;
    world->info.max_id = 0;

    /* Make EcsOnAdd, EcsOnSet events iterable to enable .yield_existing */
    ecs_set(world, EcsOnAdd, EcsIterable, { .init = on_event_iterable_init });
    ecs_set(world, EcsOnSet, EcsIterable, { .init = on_event_iterable_init });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_id(EcsComponent), .subj.set.mask = EcsSelf },
        .events = {EcsOnSet},
        .callback = on_set_component,
        .yield_existing = true
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsTag, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = register_tag,
        .yield_existing = true
    });

    /* Populate core module */
    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);
    flecs_bootstrap_tag(world, EcsAlias);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrivate);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsDisabled);

    /* Initialize builtin modules */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);

    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);
    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);

    ecs_add_pair(world, EcsFlecsHidden, EcsChildOf, EcsFlecs);
    ecs_set_name(world, EcsFlecsHidden, "hidden");
    ecs_add_id(world, EcsFlecsHidden, EcsModule);

    /* Initialize builtin entities */
    bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    bootstrap_entity(world, EcsAny, "_", EcsFlecsCore);
    bootstrap_entity(world, EcsThis, "This", EcsFlecsCore);
    bootstrap_entity(world, EcsVariable, "$", EcsFlecsCore);

    /* Component/relationship properties */
    flecs_bootstrap_tag(world, EcsTransitive);
    flecs_bootstrap_tag(world, EcsReflexive);
    flecs_bootstrap_tag(world, EcsSymmetric);
    flecs_bootstrap_tag(world, EcsFinal);
    flecs_bootstrap_tag(world, EcsDontInherit);
    flecs_bootstrap_tag(world, EcsTag);
    flecs_bootstrap_tag(world, EcsUnion);
    flecs_bootstrap_tag(world, EcsExclusive);
    flecs_bootstrap_tag(world, EcsAcyclic);
    flecs_bootstrap_tag(world, EcsWith);
    flecs_bootstrap_tag(world, EcsOneOf);

    flecs_bootstrap_tag(world, EcsOnDelete);
    flecs_bootstrap_tag(world, EcsOnDeleteObject);
    flecs_bootstrap_tag(world, EcsRemove);
    flecs_bootstrap_tag(world, EcsDelete);
    flecs_bootstrap_tag(world, EcsPanic);

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

    /* Tag relations (relations that should never have data) */
    ecs_add_id(world, EcsIsA, EcsTag);
    ecs_add_id(world, EcsChildOf, EcsTag);
    ecs_add_id(world, EcsDefaultChildComponent, EcsTag);
    ecs_add_id(world, EcsUnion, EcsTag);

    /* Exclusive properties */
    ecs_add_id(world, EcsChildOf, EcsExclusive);
    ecs_add_id(world, EcsOnDelete, EcsExclusive);
    ecs_add_id(world, EcsOnDeleteObject, EcsExclusive);
    ecs_add_id(world, EcsDefaultChildComponent, EcsExclusive);
    ecs_add_id(world, EcsOneOf, EcsExclusive);

    /* Sync properties of ChildOf and Identifier with bootstrapped flags */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteObject, EcsDelete);
    ecs_add_id(world, EcsChildOf, EcsAcyclic);
    ecs_add_id(world, EcsChildOf, EcsDontInherit);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsDontInherit);

    /* The (IsA, *) id record is used often in searches, so cache it */
    world->idr_isa_wildcard = flecs_ensure_id_record(world, 
        ecs_pair(EcsIsA, EcsWildcard));

    ecs_trigger_init(world, &(ecs_trigger_desc_t) {
        .term = { 
            .id = ecs_pair(EcsChildOf, EcsWildcard),
            .subj.set.mask = EcsSelf
        },
        .events = { EcsOnAdd, EcsOnRemove },
        .yield_existing = true,
        .callback = on_parent_change
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsFinal, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_final
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDelete, EcsWildcard), .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = register_on_delete
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDeleteObject, EcsWildcard), .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = register_on_delete_object
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsAcyclic, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = register_acyclic
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

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsWith, EcsWildcard), .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_with
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = EcsUnion, .subj.set.mask = EcsSelf },
        .events = {EcsOnAdd},
        .callback = register_union
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

    /* Acyclic components */
    ecs_add_id(world, EcsIsA, EcsAcyclic);
    ecs_add_id(world, EcsWith, EcsAcyclic);

    /* DontInherit components */
    ecs_add_id(world, EcsDisabled, EcsDontInherit);
    ecs_add_id(world, EcsPrefab, EcsDontInherit);

    /* Transitive relations are always Acyclic */
    ecs_add_pair(world, EcsTransitive, EcsWith, EcsAcyclic);

    /* Transitive relations */
    ecs_add_id(world, EcsIsA, EcsTransitive);
    ecs_add_id(world, EcsIsA, EcsReflexive);

    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_hierarchy(world);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}
