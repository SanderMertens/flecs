/**
 * @file bootstrap.c
 * @brief Bootstrap entities in the flecs.core namespace.
 * 
 * Before the ECS storage can be used, core entities such first need to be 
 * initialized. For example, components in Flecs are stored as entities in the
 * ECS storage itself with an EcsComponent component, but before this component
 * can be stored, the component itself needs to be initialized.
 * 
 * The bootstrap code uses lower-level APIs to initialize the data structures.
 * After bootstrap is completed, regular ECS operations can be used to create
 * entities and components.
 * 
 * The bootstrap file also includes several lifecycle hooks and observers for
 * builtin features, such as relationship properties and hooks for keeping the
 * entity name administration in sync with the (Identifier, Name) component.
 */

#include "private_api.h"

/* -- Identifier Component -- */
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
    EcsIdentifier *ptr = ecs_field(it, EcsIdentifier, 1);
    
    ecs_world_t *world = it->real_world;
    ecs_entity_t evt = it->event;
    ecs_id_t evt_id = it->event_id;
    ecs_entity_t kind = ECS_PAIR_SECOND(evt_id); /* Name, Symbol, Alias */
    ecs_id_t pair = ecs_childof(0);
    ecs_hashmap_t *index = NULL;

    if (kind == EcsSymbol) {
        index = &world->symbols;
    } else if (kind == EcsAlias) {
        index = &world->aliases;
    } else if (kind == EcsName) {
        ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_search(world, it->table, ecs_childof(EcsWildcard), &pair);
        ecs_assert(pair != 0, ECS_INTERNAL_ERROR, NULL);

        if (evt == EcsOnSet) {
            index = flecs_id_name_index_ensure(world, pair);
        } else {
            index = flecs_id_name_index_get(world, pair);
        }
    }

    int i, count = it->count;

    for (i = 0; i < count; i ++) {
        EcsIdentifier *cur = &ptr[i];
        uint64_t hash;
        ecs_size_t len;
        const char *name = cur->value;

        if (cur->index && cur->index != index) {
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

        if (index) {
            uint64_t index_hash = cur->index_hash;
            ecs_entity_t e = it->entities[i];

            if (hash != index_hash) {
                if (index_hash) {
                    flecs_name_index_remove(index, e, index_hash);
                }
                if (hash) {
                    flecs_name_index_ensure(index, e, name, len, hash);
                    cur->index_hash = hash;
                    cur->index = index;
                }
            } else {
                /* Name didn't change, but the string could have been 
                 * reallocated. Make sure name index points to correct string */
                flecs_name_index_update_name(index, e, hash, name);
            }
        }
    }
}


/* -- Poly component -- */

static ECS_COPY(EcsPoly, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "poly component cannot be copied");
})

static ECS_MOVE(EcsPoly, dst, src, {
    if (dst->poly && (dst->poly != src->poly)) {
        ecs_poly_dtor_t *dtor = ecs_get_dtor(dst->poly);
        ecs_assert(dtor != NULL, ECS_INTERNAL_ERROR, NULL);
        dtor[0](dst->poly);
    }

    dst->poly = src->poly;
    src->poly = NULL;
})

static ECS_DTOR(EcsPoly, ptr, {
    if (ptr->poly) {
        ecs_poly_dtor_t *dtor = ecs_get_dtor(ptr->poly);
        ecs_assert(dtor != NULL, ECS_INTERNAL_ERROR, NULL);
        dtor[0](ptr->poly);
    }
})


/* -- Builtin triggers -- */

static
void flecs_assert_relation_unused(
    ecs_world_t *world, 
    ecs_entity_t rel,
    ecs_entity_t property)
{
    if (world->flags & (EcsWorldInit|EcsWorldFini)) {
        return;
    }

    ecs_vec_t *marked_ids = &world->store.marked_ids;
    int32_t i, count = ecs_vec_count(marked_ids);
    for (i = 0; i < count; i ++) {
        ecs_marked_id_t *mid = ecs_vec_get_t(marked_ids, ecs_marked_id_t, i);
        if (mid->id == ecs_pair(rel, EcsWildcard)) {
            /* If id is being cleaned up, no need to throw error as tables will
             * be cleaned up */
            return;
        }
    }

    bool in_use = ecs_id_in_use(world, ecs_pair(rel, EcsWildcard));
    if (property != EcsUnion) {
        in_use |= ecs_id_in_use(world, rel);
    }
    if (in_use) {
        char *r_str = ecs_get_fullpath(world, rel);
        char *p_str = ecs_get_fullpath(world, property);

        ecs_throw(ECS_ID_IN_USE, 
            "cannot change property '%s' for relationship '%s': already in use",
            p_str, r_str);
        
        ecs_os_free(r_str);
        ecs_os_free(p_str);
    }

error:
    return;
}

static
bool flecs_set_id_flag(
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
bool flecs_unset_id_flag(
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
void flecs_register_id_flag_for_relation(
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
            ecs_id_record_t *idr = flecs_id_record_ensure(world, e);
            changed |= flecs_set_id_flag(idr, flag);
            idr = flecs_id_record_ensure(world, ecs_pair(e, EcsWildcard));
            do {
                changed |= flecs_set_id_flag(idr, flag);
            } while ((idr = idr->first.next));
            if (entity_flag) flecs_add_flag(world, e, entity_flag);
        } else if (event == EcsOnRemove) {
            ecs_id_record_t *idr = flecs_id_record_get(world, e);
            if (idr) changed |= flecs_unset_id_flag(idr, not_flag);
            idr = flecs_id_record_get(world, ecs_pair(e, EcsWildcard));
            if (idr) {
                do {
                    changed |= flecs_unset_id_flag(idr, not_flag);
                } while ((idr = idr->first.next));
            }
        }

        if (changed) {
            flecs_assert_relation_unused(world, e, prop);
        }
    }
}

static
void flecs_register_final(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        if (flecs_id_record_get(world, ecs_pair(EcsIsA, e)) != NULL) {
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
void flecs_register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 1);
    flecs_register_id_flag_for_relation(it, EcsOnDelete, 
        ECS_ID_ON_DELETE_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteMask,
        EcsEntityIsId);
}

static
void flecs_register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 1);
    flecs_register_id_flag_for_relation(it, EcsOnDeleteTarget, 
        ECS_ID_ON_DELETE_OBJECT_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteObjectMask,
        EcsEntityIsId);  
}

static
void flecs_register_traversable(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsAcyclic, EcsIdTraversable, 
        EcsIdTraversable, 0);
}

static
void flecs_register_tag(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsTag, EcsIdTag, ~EcsIdTag, 0);

    /* Ensure that all id records for tag have type info set to NULL */
    ecs_world_t *world = it->real_world;
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (it->event == EcsOnAdd) {
            ecs_id_record_t *idr = flecs_id_record_get(world, 
                ecs_pair(e, EcsWildcard));
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            do {
                if (idr->type_info != NULL) {
                    flecs_assert_relation_unused(world, e, EcsTag);
                }
                idr->type_info = NULL;
            } while ((idr = idr->first.next));
        }
    }
}

static
void flecs_register_exclusive(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsExclusive, EcsIdExclusive, 
        EcsIdExclusive, 0);
}

static
void flecs_register_dont_inherit(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsDontInherit, 
        EcsIdDontInherit, EcsIdDontInherit, 0);
}

static
void flecs_register_always_override(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsAlwaysOverride, 
        EcsIdAlwaysOverride, EcsIdAlwaysOverride, 0);
}

static
void flecs_register_with(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsWith, EcsIdWith, 0, 0);
}

static
void flecs_register_union(ecs_iter_t *it) {
    flecs_register_id_flag_for_relation(it, EcsUnion, EcsIdUnion, 0, 0);
}

static
void flecs_register_slot_of(ecs_iter_t *it) {
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_add_id(it->world, it->entities[i], EcsUnion);
    }
}

static
void flecs_on_symmetric_add_remove(ecs_iter_t *it) {
    ecs_entity_t pair = ecs_field_id(it, 1);

    if (!ECS_HAS_ID_FLAG(pair, PAIR)) {
        /* If relationship was not added as a pair, there's nothing to do */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t rel = ECS_PAIR_FIRST(pair);
    ecs_entity_t obj = ecs_pair_second(world, pair);
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
void flecs_register_symmetric(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t r = it->entities[i];
        flecs_assert_relation_unused(world, r, EcsSymmetric);

        /* Create observer that adds the reverse relationship when R(X, Y) is
         * added, or remove the reverse relationship when R(X, Y) is removed. */
        ecs_observer(world, {
            .entity = ecs_entity(world, {.add = {ecs_childof(r)}}),
            .filter.terms[0] = { .id = ecs_pair(r, EcsWildcard) },
            .callback = flecs_on_symmetric_add_remove,
            .events = {EcsOnAdd, EcsOnRemove}
        });
    } 
}

static
void flecs_on_component(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsComponent *c = ecs_field(it, EcsComponent, 1);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        uint32_t component_id = (uint32_t)e; /* Strip generation */
        ecs_assert(component_id < ECS_MAX_COMPONENT_ID, ECS_OUT_OF_RANGE,
            "component id must be smaller than %u", ECS_MAX_COMPONENT_ID);
        (void)component_id;

        if (it->event == EcsOnSet) {
            if (flecs_type_info_init_id(
                world, e, c[i].size, c[i].alignment, NULL))
            {
                flecs_assert_relation_unused(world, e, ecs_id(EcsComponent));
            }
        } else if (it->event == EcsOnRemove) {
            flecs_type_info_free(world, e);
        }
    }
}

static
void flecs_ensure_module_tag(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
        if (parent) {
            ecs_add_id(world, parent, EcsModule);
        }
    }
}

/* -- Iterable mixins -- */

static
void flecs_on_event_iterable_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly, /* Observable */
    ecs_iter_t *it,
    ecs_term_t *filter)
{
    ecs_iter_poly(world, poly, it, filter);
    it->event_id = filter->id;
}

/* -- Bootstrapping -- */

#define flecs_bootstrap_builtin_t(world, table, name)\
    flecs_bootstrap_builtin(world, table, ecs_id(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

static
void flecs_bootstrap_builtin(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_t *columns = table->data.columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = flecs_entities_ensure(world, entity);
    record->table = table;

    int32_t index = flecs_table_append(world, table, entity, record, false, false);
    record->row = ECS_ROW_TO_RECORD(index, 0);

    EcsComponent *component = ecs_vec_first(&columns[0]);
    component[index].size = size;
    component[index].alignment = alignment;

    const char *name = &symbol[3]; /* Strip 'Ecs' */
    ecs_size_t symbol_length = ecs_os_strlen(symbol);
    ecs_size_t name_length = symbol_length - 3;

    EcsIdentifier *name_col = ecs_vec_first(&columns[1]);
    uint64_t name_hash = flecs_hash(name, name_length);
    name_col[index].value = ecs_os_strdup(name);
    name_col[index].length = name_length;
    name_col[index].hash = name_hash;
    name_col[index].index_hash = 0;
    name_col[index].index = table->_->name_index;
    flecs_name_index_ensure(
        table->_->name_index, entity, name, name_length, name_hash);

    EcsIdentifier *symbol_col = ecs_vec_first(&columns[2]);
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
ecs_table_t* flecs_bootstrap_component_table(
    ecs_world_t *world)
{
    /* Before creating table, manually set flags for ChildOf/Identifier, as this
     * can no longer be done after they are in use. */
    ecs_id_record_t *idr = flecs_id_record_ensure(world, EcsChildOf);
    idr->flags |= EcsIdOnDeleteObjectDelete | EcsIdDontInherit |
        EcsIdTraversable | EcsIdTag;

    /* Initialize id records cached on world */
    world->idr_childof_wildcard = flecs_id_record_ensure(world, 
        ecs_pair(EcsChildOf, EcsWildcard));
    world->idr_childof_wildcard->flags |= EcsIdOnDeleteObjectDelete | 
        EcsIdDontInherit | EcsIdTraversable | EcsIdTag | EcsIdExclusive;
    idr = flecs_id_record_ensure(world, ecs_pair_t(EcsIdentifier, EcsWildcard));
    idr->flags |= EcsIdDontInherit;
    world->idr_identifier_name = 
        flecs_id_record_ensure(world, ecs_pair_t(EcsIdentifier, EcsName));
    world->idr_childof_0 = flecs_id_record_ensure(world, 
        ecs_pair(EcsChildOf, 0));

    ecs_id_t ids[] = {
        ecs_id(EcsComponent), 
        EcsFinal,
        ecs_pair_t(EcsIdentifier, EcsName),
        ecs_pair_t(EcsIdentifier, EcsSymbol),
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
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_init_t(a, &data->entities, ecs_entity_t, EcsFirstUserComponentId);
    ecs_vec_init_t(a, &data->records, ecs_record_t*, EcsFirstUserComponentId);
    ecs_vec_init_t(a, &data->columns[0], EcsComponent, EcsFirstUserComponentId);
    ecs_vec_init_t(a, &data->columns[1], EcsIdentifier, EcsFirstUserComponentId);
    ecs_vec_init_t(a, &data->columns[2], EcsIdentifier, EcsFirstUserComponentId);
    
    return result;
}

static
void flecs_bootstrap_entity(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    ecs_entity_t parent)
{
    char symbol[256];
    ecs_os_strcpy(symbol, "flecs.core.");
    ecs_os_strcat(symbol, name);
    
    ecs_ensure(world, id);
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

    /* Ensure builtin ids are alive */
    ecs_ensure(world, ecs_id(EcsComponent));
    ecs_ensure(world, EcsFinal);
    ecs_ensure(world, ecs_id(EcsIdentifier));
    ecs_ensure(world, EcsName);
    ecs_ensure(world, EcsSymbol);
    ecs_ensure(world, EcsAlias);
    ecs_ensure(world, EcsChildOf);
    ecs_ensure(world, EcsFlecs);
    ecs_ensure(world, EcsFlecsCore);
    ecs_ensure(world, EcsOnAdd);
    ecs_ensure(world, EcsOnRemove);
    ecs_ensure(world, EcsOnSet);
    ecs_ensure(world, EcsUnSet);
    ecs_ensure(world, EcsOnDelete);
    ecs_ensure(world, EcsPanic);
    ecs_ensure(world, EcsFlag);
    ecs_ensure(world, EcsIsA);
    ecs_ensure(world, EcsWildcard);
    ecs_ensure(world, EcsAny);
    ecs_ensure(world, EcsTag);

    /* Register type information for builtin components */
    flecs_type_info_init(world, EcsComponent, { 
        .ctor = ecs_default_ctor,
        .on_set = flecs_on_component,
        .on_remove = flecs_on_component
    });

    flecs_type_info_init(world, EcsIdentifier, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(EcsIdentifier),
        .copy = ecs_copy(EcsIdentifier),
        .move = ecs_move(EcsIdentifier),
        .on_set = ecs_on_set(EcsIdentifier),
        .on_remove = ecs_on_set(EcsIdentifier)
    });

    flecs_type_info_init(world, EcsPoly, {
        .ctor = ecs_default_ctor,
        .copy = ecs_copy(EcsPoly),
        .move = ecs_move(EcsPoly),
        .dtor = ecs_dtor(EcsPoly)
    });

    flecs_type_info_init(world, EcsIterable, { 0 });
    flecs_type_info_init(world, EcsTarget, { 0 });

    /* Create and cache often used id records on world */
    flecs_init_id_records(world);

    /* Create table for builtin components. This table temporarily stores the 
     * entities associated with builtin components, until they get moved to 
     * other tables once properties are added (see below) */
    ecs_table_t *table = flecs_bootstrap_component_table(world);
    assert(table != NULL);

    /* Bootstrap builtin components */
    flecs_bootstrap_builtin_t(world, table, EcsIdentifier);
    flecs_bootstrap_builtin_t(world, table, EcsComponent);
    flecs_bootstrap_builtin_t(world, table, EcsIterable);
    flecs_bootstrap_builtin_t(world, table, EcsPoly);
    flecs_bootstrap_builtin_t(world, table, EcsTarget);

    /* Initialize default entity id range */
    world->info.last_component_id = EcsFirstUserComponentId;
    flecs_entities_max_id(world) = EcsFirstUserEntityId;
    world->info.min_id = 0;
    world->info.max_id = 0;

    /* Make EcsOnAdd, EcsOnSet events iterable to enable .yield_existing */
    ecs_set(world, EcsOnAdd, EcsIterable, { .init = flecs_on_event_iterable_init });
    ecs_set(world, EcsOnSet, EcsIterable, { .init = flecs_on_event_iterable_init });
    
    /* Register observer for tag property before adding EcsTag */
    ecs_observer(world, {
        .entity = ecs_entity(world, {.add = { ecs_childof(EcsFlecsInternals)}}),
        .filter.terms[0] = { .id = EcsTag, .src.flags = EcsSelf },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_tag,
        .yield_existing = true
    });

    /* Populate core module */
    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);
    flecs_bootstrap_tag(world, EcsAlias);

    flecs_bootstrap_tag(world, EcsQuery);
    flecs_bootstrap_tag(world, EcsObserver);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrivate);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsSlotOf);
    flecs_bootstrap_tag(world, EcsDisabled);
    flecs_bootstrap_tag(world, EcsEmpty);

    /* Initialize builtin modules */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);
    ecs_add_pair(world, EcsFlecs, EcsOnDelete, EcsPanic);

    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);
    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);

    ecs_add_pair(world, EcsFlecsInternals, EcsChildOf, EcsFlecsCore);
    ecs_set_name(world, EcsFlecsInternals, "internals");
    ecs_add_id(world, EcsFlecsInternals, EcsModule);

    /* Self check */
    ecs_record_t *r = flecs_entities_get(world, EcsFlecs);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->row & EcsEntityIsTraversable, ECS_INTERNAL_ERROR, NULL);
    (void)r;

    /* Initialize builtin entities */
    flecs_bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsAny, "_", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsThis, "this", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsVariable, "$", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsFlag, "Flag", EcsFlecsCore);

    /* Component/relationship properties */
    flecs_bootstrap_tag(world, EcsTransitive);
    flecs_bootstrap_tag(world, EcsReflexive);
    flecs_bootstrap_tag(world, EcsSymmetric);
    flecs_bootstrap_tag(world, EcsFinal);
    flecs_bootstrap_tag(world, EcsDontInherit);
    flecs_bootstrap_tag(world, EcsAlwaysOverride);
    flecs_bootstrap_tag(world, EcsTag);
    flecs_bootstrap_tag(world, EcsUnion);
    flecs_bootstrap_tag(world, EcsExclusive);
    flecs_bootstrap_tag(world, EcsAcyclic);
    flecs_bootstrap_tag(world, EcsTraversable);
    flecs_bootstrap_tag(world, EcsWith);
    flecs_bootstrap_tag(world, EcsOneOf);

    flecs_bootstrap_tag(world, EcsOnDelete);
    flecs_bootstrap_tag(world, EcsOnDeleteTarget);
    flecs_bootstrap_tag(world, EcsRemove);
    flecs_bootstrap_tag(world, EcsDelete);
    flecs_bootstrap_tag(world, EcsPanic);

    flecs_bootstrap_tag(world, EcsFlatten);
    flecs_bootstrap_tag(world, EcsDefaultChildComponent);

    /* Builtin predicates */
    flecs_bootstrap_tag(world, EcsPredEq);
    flecs_bootstrap_tag(world, EcsPredMatch);
    flecs_bootstrap_tag(world, EcsPredLookup);
    flecs_bootstrap_tag(world, EcsScopeOpen);
    flecs_bootstrap_tag(world, EcsScopeClose);

    /* Builtin relationships */
    flecs_bootstrap_tag(world, EcsIsA);
    flecs_bootstrap_tag(world, EcsChildOf);
    flecs_bootstrap_tag(world, EcsDependsOn);

    /* Builtin events */
    flecs_bootstrap_entity(world, EcsOnAdd, "OnAdd", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnRemove, "OnRemove", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnSet, "OnSet", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsUnSet, "UnSet", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsMonitor, "EcsMonitor", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableCreate, "OnTableCreate", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableDelete, "OnTableDelete", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableEmpty, "OnTableEmpty", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableFill, "OnTableFilled", EcsFlecsCore);

    /* Tag relationships (relationships that should never have data) */
    ecs_add_id(world, EcsIsA, EcsTag);
    ecs_add_id(world, EcsChildOf, EcsTag);
    ecs_add_id(world, EcsSlotOf, EcsTag);
    ecs_add_id(world, EcsDependsOn, EcsTag);
    ecs_add_id(world, EcsFlatten, EcsTag);
    ecs_add_id(world, EcsDefaultChildComponent, EcsTag);
    ecs_add_id(world, EcsUnion, EcsTag);
    ecs_add_id(world, EcsFlag, EcsTag);
    ecs_add_id(world, EcsWith, EcsTag);

    /* Exclusive properties */
    ecs_add_id(world, EcsChildOf, EcsExclusive);
    ecs_add_id(world, EcsOnDelete, EcsExclusive);
    ecs_add_id(world, EcsOnDeleteTarget, EcsExclusive);
    ecs_add_id(world, EcsDefaultChildComponent, EcsExclusive);

    /* Sync properties of ChildOf and Identifier with bootstrapped flags */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteTarget, EcsDelete);
    ecs_add_id(world, EcsChildOf, EcsAcyclic);
    ecs_add_id(world, EcsChildOf, EcsTraversable);
    ecs_add_id(world, EcsChildOf, EcsDontInherit);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsDontInherit);

    /* Create triggers in internals scope */
    ecs_set_scope(world, EcsFlecsInternals);

    /* Term used to also match prefabs */
    ecs_term_t match_prefab = { 
        .id = EcsPrefab, 
        .oper = EcsOptional,
        .src.flags = EcsSelf 
    };

    /* Register observers for components/relationship properties. Most observers
     * set flags on an id record when a property is added to a component, which
     * allows for quick property testing in various operations. */
    ecs_observer(world, {
        .filter.terms = {{ .id = EcsFinal, .src.flags = EcsSelf }, match_prefab },
        .events = {EcsOnAdd},
        .callback = flecs_register_final
    });

    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_pair(EcsOnDelete, EcsWildcard), .src.flags = EcsSelf },
            match_prefab
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_on_delete
    });

    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_pair(EcsOnDeleteTarget, EcsWildcard), .src.flags = EcsSelf },
            match_prefab
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_on_delete_object
    });

    ecs_observer(world, {
        .filter.terms = {
            { .id = EcsTraversable, .src.flags = EcsSelf },
            match_prefab
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_traversable
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsExclusive, .src.flags = EcsSelf  }, match_prefab },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_exclusive
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsSymmetric, .src.flags = EcsSelf  }, match_prefab },
        .events = {EcsOnAdd},
        .callback = flecs_register_symmetric
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsDontInherit, .src.flags = EcsSelf }, match_prefab },
        .events = {EcsOnAdd},
        .callback = flecs_register_dont_inherit
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsAlwaysOverride, .src.flags = EcsSelf } },
        .events = {EcsOnAdd},
        .callback = flecs_register_always_override
    });

    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_pair(EcsWith, EcsWildcard), .src.flags = EcsSelf },
            match_prefab
        },
        .events = {EcsOnAdd},
        .callback = flecs_register_with
    });

    ecs_observer(world, {
        .filter.terms = {{ .id = EcsUnion, .src.flags = EcsSelf }, match_prefab },
        .events = {EcsOnAdd},
        .callback = flecs_register_union
    });

    /* Entities used as slot are marked as exclusive to ensure a slot can always
     * only point to a single entity. */
    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_pair(EcsSlotOf, EcsWildcard), .src.flags = EcsSelf },
            match_prefab
        },
        .events = {EcsOnAdd},
        .callback = flecs_register_slot_of
    });

    /* Define observer to make sure that adding a module to a child entity also
     * adds it to the parent. */
    ecs_observer(world, {
        .filter.terms = {{ .id = EcsModule, .src.flags = EcsSelf }, match_prefab},
        .events = {EcsOnAdd},
        .callback = flecs_ensure_module_tag
    });

    /* Set scope back to flecs core */
    ecs_set_scope(world, EcsFlecsCore);

    /* Traversable relationships are always acyclic */
    ecs_add_pair(world, EcsTraversable, EcsWith, EcsAcyclic);

    /* Transitive relationships are always Traversable */
    ecs_add_pair(world, EcsTransitive, EcsWith, EcsTraversable);

    /* DontInherit components */
    ecs_add_id(world, EcsPrefab, EcsDontInherit);

    /* Acyclic/Traversable components */
    ecs_add_id(world, EcsIsA, EcsTraversable);
    ecs_add_id(world, EcsDependsOn, EcsTraversable);
    ecs_add_id(world, EcsWith, EcsAcyclic);

    /* Transitive relationships */
    ecs_add_id(world, EcsIsA, EcsTransitive);
    ecs_add_id(world, EcsIsA, EcsReflexive);

    /* Exclusive properties */
    ecs_add_id(world, EcsSlotOf, EcsExclusive);
    ecs_add_id(world, EcsOneOf, EcsExclusive);
    ecs_add_id(world, EcsFlatten, EcsExclusive);
    
    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_hierarchy(world);

    ecs_set_scope(world, 0);

    ecs_set_name_prefix(world, NULL);

    ecs_log_pop();
}
