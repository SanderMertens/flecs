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


/* -- Poly component -- */

static ECS_COPY(EcsPoly, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "poly component cannot be copied");
})

static ECS_MOVE(EcsPoly, dst, src, {
    if (dst->poly && (dst->poly != src->poly)) {
        flecs_poly_dtor_t *dtor = flecs_get_dtor(dst->poly);
        ecs_assert(dtor != NULL, ECS_INTERNAL_ERROR, NULL);
        dtor[0](dst->poly);
    }

    dst->poly = src->poly;
    src->poly = NULL;
})

static ECS_DTOR(EcsPoly, ptr, {
    if (ptr->poly) {
        flecs_poly_dtor_t *dtor = flecs_get_dtor(ptr->poly);
        ecs_assert(dtor != NULL, ECS_INTERNAL_ERROR, NULL);
        dtor[0](ptr->poly);
    }
})


/* -- Builtin triggers -- */

static
void flecs_assert_relation_unused(
    ecs_world_t *world, 
    ecs_entity_t rel,
    ecs_entity_t trait)
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
    in_use |= ecs_id_in_use(world, rel);

    if (in_use) {
        char *r_str = ecs_get_path(world, rel);
        char *p_str = ecs_get_path(world, trait);

        ecs_throw(ECS_INVALID_OPERATION, 
            "cannot change trait '%s' for '%s': component is already in use",
            p_str, r_str);
        
        ecs_os_free(r_str);
        ecs_os_free(p_str);
    }

error:
    return;
}

static
bool flecs_set_id_flag(
    ecs_world_t *world,
    ecs_component_record_t *cr, 
    ecs_flags32_t flag,
    ecs_entity_t trait)
{
    (void)trait;

    if (!(cr->flags & flag)) {
        cr->flags |= flag;
        if (flag == EcsIdSparse) {
            flecs_component_init_sparse(world, cr);
        }

        if (flag == EcsIdDontFragment) {
            flecs_component_record_init_dont_fragment(world, cr);
        }

        if (flag == EcsIdExclusive) {
            flecs_component_record_init_exclusive(world, cr);
        }

        return true;
    }

    return false;
}

static
bool flecs_unset_id_flag(
    ecs_component_record_t *cr, 
    ecs_flags32_t flag)
{
    if (cr->flags & EcsIdMarkedForDelete) {
        /* Don't change flags for record that's about to be deleted */
        return false;
    }

    if ((cr->flags & flag)) {
        cr->flags &= ~flag;
        return true;
    }
    return false;
}

typedef struct ecs_on_trait_ctx_t {
    ecs_flags32_t flag, not_flag;
} ecs_on_trait_ctx_t;

static
bool flecs_trait_can_add_after_query(
    ecs_entity_t trait)
{
    if (trait == EcsWith) {
        return true;
    }

    return false;
}

static
void flecs_register_flag_for_trait(
    ecs_iter_t *it,
    ecs_entity_t trait,
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
            if (flag == EcsIdOnInstantiateInherit) {
                if (e < FLECS_HI_COMPONENT_ID) {
                    world->non_trivial_lookup[e] |= EcsNonTrivialIdInherit;
                }
            }

            if (!(world->flags & EcsWorldInit) && !flecs_trait_can_add_after_query(trait)) {
                ecs_check(!flecs_component_is_trait_locked(world, e), ECS_INVALID_OPERATION, 
                    "cannot set '%s' trait for component '%s' because it is already"
                        " queried for (apply traits before creating queries)",
                            flecs_errstr(ecs_get_path(world, trait)),
                            flecs_errstr_1(ecs_id_str(world, e)));
            }

            ecs_component_record_t *cr = flecs_components_get(world, e);
            if (cr) {
                changed |= flecs_set_id_flag(world, cr, flag, trait);
            }

            cr = flecs_components_get(world, ecs_pair(e, EcsWildcard));
            if (cr) {
                do {
                    changed |= flecs_set_id_flag(world, cr, flag, trait);
                } while ((cr = flecs_component_first_next(cr)));
            }

            if (entity_flag) flecs_add_flag(world, e, entity_flag);
        } else if (event == EcsOnRemove) {
            ecs_component_record_t *cr = flecs_components_get(world, e);
            
            if (cr) changed |= flecs_unset_id_flag(cr, not_flag);
            cr = flecs_components_get(world, ecs_pair(e, EcsWildcard));
            if (cr) {
                do {
                    changed |= flecs_unset_id_flag(cr, not_flag);
                } while ((cr = flecs_component_first_next(cr)));
            }
        }

        if (changed) {
            flecs_assert_relation_unused(world, e, trait);
        }
    }
error:
    return;
}

static
void flecs_register_final(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        if (flecs_components_get(world, ecs_pair(EcsIsA, e)) != NULL) {
            ecs_throw(ECS_INVALID_OPERATION,
                "cannot change trait 'Final' for '%s': already inherited from",
                    flecs_errstr(ecs_get_path(world, e)));
        }

        ecs_check(!flecs_component_is_trait_locked(world, e), ECS_INVALID_OPERATION, "cannot change "
            "trait 'Final' for '%s': already queried for (apply traits "
            "before creating queries)", 
                flecs_errstr(ecs_get_path(world, e)));

        error:
            continue;
    }
}

static
void flecs_register_tag(ecs_iter_t *it) {
    flecs_register_flag_for_trait(it, EcsPairIsTag, EcsIdPairIsTag, EcsIdPairIsTag, 0);

    /* Ensure that all id records for tag have type info set to NULL */
    ecs_world_t *world = it->real_world;
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (it->event == EcsOnAdd) {
            ecs_component_record_t *cr = flecs_components_get(world, 
                ecs_pair(e, EcsWildcard));
            if (cr) {
                do {
                    if (cr->type_info != NULL) {
                        flecs_assert_relation_unused(world, e, EcsPairIsTag);
                    }
                    cr->type_info = NULL;
                } while ((cr = flecs_component_first_next(cr)));
            }
        }
    }
}

static
void flecs_register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 0);
    flecs_register_flag_for_trait(it, EcsOnDelete, 
        ECS_ID_ON_DELETE_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteMask,
        EcsEntityIsId);
}

static
void flecs_register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 0);
    flecs_register_flag_for_trait(it, EcsOnDeleteTarget, 
        ECS_ID_ON_DELETE_TARGET_FLAG(ECS_PAIR_SECOND(id)),
        EcsIdOnDeleteTargetMask,
        EcsEntityIsId);  
}

static
void flecs_register_on_instantiate(ecs_iter_t *it) {
    ecs_id_t id = ecs_field_id(it, 0);
    flecs_register_flag_for_trait(it, EcsOnInstantiate, 
        ECS_ID_ON_INSTANTIATE_FLAG(ECS_PAIR_SECOND(id)),
        0, 0);
}

static
void flecs_register_trait(ecs_iter_t *it) {
    ecs_on_trait_ctx_t *ctx = it->ctx;
    flecs_register_flag_for_trait(
        it, it->ids[0], ctx->flag, ctx->not_flag, 0);
}

static
void flecs_register_trait_pair(ecs_iter_t *it) {
    ecs_on_trait_ctx_t *ctx = it->ctx;
    flecs_register_flag_for_trait(
        it, ecs_pair_first(it->world, it->ids[0]), ctx->flag, ctx->not_flag, 0);
}

static
void flecs_register_slot_of(ecs_iter_t *it) {
    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_add_id(it->world, it->entities[i], EcsDontFragment);
        ecs_add_id(it->world, it->entities[i], EcsExclusive);
    }
}

static
void flecs_on_symmetric_add_remove(ecs_iter_t *it) {
    ecs_entity_t pair = ecs_field_id(it, 0);

    if (!ECS_HAS_ID_FLAG(pair, PAIR)) {
        /* If relationship was not added as a pair, there's nothing to do */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t rel = ECS_PAIR_FIRST(pair);
    ecs_entity_t tgt = ecs_pair_second(world, pair);
    ecs_entity_t event = it->event;


    if (tgt) {
        int i, count = it->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t subj = it->entities[i];
            if (event == EcsOnAdd) {
                if (!ecs_has_id(it->real_world, tgt, ecs_pair(rel, subj))) {
                    ecs_add_pair(it->world, tgt, rel, subj);   
                }
            } else {
                if (ecs_has_id(it->real_world, tgt, ecs_pair(rel, subj))) {
                    ecs_remove_pair(it->world, tgt, rel, subj);   
                }
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
            .entity = ecs_entity(world, { .parent = r }),
            .query.terms[0] = { .id = ecs_pair(r, EcsWildcard) },
            .callback = flecs_on_symmetric_add_remove,
            .events = {EcsOnAdd, EcsOnRemove}
        });
    } 
}

#ifdef FLECS_DEBUG
static
void flecs_on_singleton_add_remove(ecs_iter_t *it) {
    ecs_entity_t component = ecs_field_id(it, 0);
    ecs_world_t *world = it->real_world;

    (void)world;

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        if (ECS_IS_PAIR(component)) {
            ecs_entity_t relationship = ECS_PAIR_FIRST(component);
            e = (uint32_t)e;
            ecs_check(relationship == e, ECS_CONSTRAINT_VIOLATED,
                "cannot add singleton pair '%s' to entity '%s': singleton "
                "component '%s' must be added to itself",
                    flecs_errstr(ecs_id_str(world, component)),
                    flecs_errstr_1(ecs_get_path(world, it->entities[i])),
                    flecs_errstr_2(ecs_get_path(it->world, relationship)));
            (void)relationship;
        } else {
            ecs_check(component == e, ECS_CONSTRAINT_VIOLATED,
                "cannot add singleton component '%s' to entity '%s': singleton"
                " component must be added to itself", 
                    flecs_errstr(ecs_get_path(it->world, component)),
                    flecs_errstr_1(ecs_get_path(it->world, it->entities[i])));
        }

    error:
        continue;
    }
}
#endif

static
void flecs_register_singleton(ecs_iter_t *it) {
    ecs_world_t *world = it->real_world;

    (void)world;

    flecs_register_flag_for_trait(it, EcsSingleton, EcsIdSingleton, 0, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t component = it->entities[i];
        (void)component;

        /* Create observer that enforces that singleton is only added to self */
#ifdef FLECS_DEBUG
        ecs_observer(world, {
            .entity = ecs_entity(world, { .parent = component }),
            .query.terms[0] = { 
                .id = component, .src.id = EcsThis|EcsSelf 
            },
            .callback = flecs_on_singleton_add_remove,
            .events = {EcsOnAdd}
        });

        ecs_observer(world, {
            .entity = ecs_entity(world, { .parent = component }),
            .query.terms[0] = { 
                .id = ecs_pair(component, EcsWildcard), .src.id = EcsThis|EcsSelf 
            },
            .callback = flecs_on_singleton_add_remove,
            .events = {EcsOnAdd}
        });
#endif
    }
}

static
void flecs_on_component(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsComponent *c = ecs_field(it, EcsComponent, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];

        uint32_t component_id = (uint32_t)e; /* Strip generation */
        ecs_assert(component_id < ECS_MAX_COMPONENT_ID, ECS_OUT_OF_RANGE,
            "component id must be smaller than %u", ECS_MAX_COMPONENT_ID);
        (void)component_id;

        if (it->event != EcsOnRemove) {
            ecs_entity_t parent = ecs_get_target(world, e, EcsChildOf, 0);
            if (parent) {
                ecs_add_id(world, parent, EcsModule);
            }
        }

        if (it->event == EcsOnSet) {
            if (flecs_type_info_init_id(
                world, e, c[i].size, c[i].alignment, NULL))
            {
                flecs_assert_relation_unused(world, e, ecs_id(EcsComponent));
            }
        } else if (it->event == EcsOnRemove) {
            #ifdef FLECS_DEBUG
            if (ecs_should_log(0)) {
                char *path = ecs_get_path(world, e);
                ecs_trace("unregistering component '%s'", path);
                ecs_os_free(path);
            }
            #endif
            if (!ecs_vec_count(&world->store.marked_ids)) {
                flecs_type_info_free(world, e);
            } else {
                ecs_vec_append_t(&world->allocator, 
                    &world->store.deleted_components, ecs_entity_t)[0] = e;
            }
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

static
void flecs_disable_observer(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->world;
    ecs_entity_t evt = it->event;

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        flecs_observer_set_disable_bit(world, it->entities[i], 
            EcsObserverIsDisabled, evt == EcsOnAdd);
    }
}

static
void flecs_disable_module_observers(
    ecs_world_t *world, 
    ecs_entity_t module,
    bool should_disable)
{
    ecs_iter_t child_it = ecs_children(world, module);
    while (ecs_children_next(&child_it)) {
        ecs_table_t *table = child_it.table;
        bool table_disabled = table->flags & EcsTableIsDisabled;
        int32_t i;

        /* Recursively walk modules, don't propagate to disabled modules */
        if (ecs_table_has_id(world, table, EcsModule) && !table_disabled) {    
            for (i = 0; i < child_it.count; i ++) {
                flecs_disable_module_observers(
                    world, child_it.entities[i], should_disable);
            }
            continue;
        }

        /* Only disable observers */
        if (!ecs_table_has_id(world, table, EcsObserver)) {
            continue;
        }

        for (i = 0; i < child_it.count; i ++) {
            flecs_observer_set_disable_bit(world, child_it.entities[i], 
                EcsObserverIsParentDisabled, should_disable);
        }
    }
}

static
void flecs_disable_module(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        flecs_disable_module_observers(
            it->real_world, it->entities[i], it->event == EcsOnAdd);
    }
}

static
void flecs_register_ordered_children(ecs_iter_t *it) {
    int32_t i;
    if (it->event == EcsOnAdd) {
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t parent = it->entities[i];
            ecs_component_record_t *cr = flecs_components_ensure(
                it->world, ecs_childof(parent));
            if (!(cr->flags & EcsIdOrderedChildren)) {
                flecs_ordered_children_init(it->world, cr);
                flecs_ordered_children_populate(it->world, cr);
                cr->flags |= EcsIdOrderedChildren;
            }
        }
    } else if (!(it->real_world->flags & EcsWorldFini)) {
        ecs_assert(it->event == EcsOnRemove, ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t parent = it->entities[i];
            ecs_component_record_t *cr = flecs_components_get(
                it->world, ecs_childof(parent));
            if (cr && (cr->flags & EcsIdOrderedChildren)) {
                flecs_ordered_children_clear(cr);
                cr->flags &= ~EcsIdOrderedChildren;
            }
        }
    }
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

    ecs_column_t *columns = table->data.columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = flecs_entities_ensure(world, entity);
    ecs_assert(record->table == &world->store.root, ECS_INTERNAL_ERROR, NULL);
    flecs_table_delete(
        world, &world->store.root, ECS_RECORD_TO_ROW(record->row), false);
    record->table = table;

    int32_t index = flecs_table_append(world, table, entity, false, false);
    record->row = ECS_ROW_TO_RECORD(index, 0);

    EcsComponent *component = columns[0].data;
    component[index].size = size;
    component[index].alignment = alignment;

    const char *name = &symbol[3]; /* Strip 'Ecs' */
    ecs_size_t symbol_length = ecs_os_strlen(symbol);
    ecs_size_t name_length = symbol_length - 3;

    EcsIdentifier *name_col = columns[1].data;
    uint64_t name_hash = flecs_hash(name, name_length);
    name_col[index].value = ecs_os_strdup(name);
    name_col[index].length = name_length;
    name_col[index].hash = name_hash;
    name_col[index].index_hash = 0;

    ecs_hashmap_t *name_index = flecs_table_get_name_index(world, table);
    name_col[index].index = name_index;
    flecs_name_index_ensure(name_index, entity, name, name_length, name_hash);

    EcsIdentifier *symbol_col = columns[2].data;
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

    /* Initialize id records cached on world */
    world->cr_childof_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsChildOf, EcsWildcard));
    world->cr_childof_wildcard->flags |= EcsIdOnDeleteTargetDelete | 
        EcsIdOnInstantiateDontInherit | EcsIdTraversable | EcsIdPairIsTag | 
        EcsIdExclusive;

    ecs_component_record_t *cr = flecs_components_ensure(
        world, ecs_pair_t(EcsIdentifier, EcsWildcard));
    cr->flags |= EcsIdOnInstantiateDontInherit;
    world->cr_identifier_name = 
        flecs_components_ensure(world, ecs_pair_t(EcsIdentifier, EcsName));
    world->cr_childof_0 = flecs_components_ensure(world, 
        ecs_pair(EcsChildOf, 0));

    /* Initialize root table */
    flecs_init_root_table(world);

    ecs_id_t ids[] = {
        ecs_id(EcsComponent), 
        ecs_pair_t(EcsIdentifier, EcsName),
        ecs_pair_t(EcsIdentifier, EcsSymbol),
        ecs_pair(EcsChildOf, EcsFlecsCore),
    };

    ecs_type_t array = {
        .array = ids,
        .count = 4
    };

    ecs_table_t *result = flecs_table_find_or_create(world, &array);

    /* Preallocate enough memory for initial components */
    ecs_vec_t v_entities = ecs_vec_from_entities(result);
    ecs_vec_init_t(NULL, &v_entities, ecs_entity_t, EcsFirstUserComponentId);
    
    {
        ecs_column_t *column = &result->data.columns[0];
        ecs_vec_t v = ecs_vec_from_column_t(column, result, EcsComponent);
        ecs_vec_init_t(NULL, &v, EcsComponent, EcsFirstUserComponentId);
        ecs_assert(v.count == v_entities.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(v.size == v_entities.size, ECS_INTERNAL_ERROR, NULL);
        column->data = v.array;
    }
    {
        ecs_column_t *column = &result->data.columns[1];
        ecs_vec_t v = ecs_vec_from_column_t(column, result, EcsIdentifier);
        ecs_vec_init_t(NULL, &v, EcsIdentifier, EcsFirstUserComponentId);
        ecs_assert(v.count == v_entities.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(v.size == v_entities.size, ECS_INTERNAL_ERROR, NULL);
        column->data = v.array;
    }
    {
        ecs_column_t *column = &result->data.columns[2];
        ecs_vec_t v = ecs_vec_from_column_t(column, result, EcsIdentifier);
        ecs_vec_init_t(NULL, &v, EcsIdentifier, EcsFirstUserComponentId);
        ecs_assert(v.count == v_entities.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(v.size == v_entities.size, ECS_INTERNAL_ERROR, NULL);
        column->data = v.array;
    }

    result->data.entities = v_entities.array;
    result->data.count = 0;
    result->data.size = v_entities.size;
    
    return result;
}

/* Make entities alive before the root table is initialized */
static
void flecs_bootstrap_make_alive(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_table_t *root = &world->store.root;
    flecs_entities_make_alive(world, e);

    ecs_record_t *r = flecs_entities_ensure(world, e);
    ecs_assert(r->table == NULL || r->table == root, 
        ECS_INTERNAL_ERROR, NULL);

    if (r->table == NULL) {
        r->table = root;
        r->row = flecs_ito(uint32_t, root->data.count);

        ecs_vec_t v_entities = ecs_vec_from_entities(root);
        ecs_entity_t *array = ecs_vec_append_t(NULL, &v_entities, ecs_entity_t);
        array[0] = e;

        root->data.entities = v_entities.array;
        root->data.count = v_entities.count;
        root->data.size = v_entities.size;
    }
}

static
void flecs_bootstrap_entity(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    ecs_entity_t parent)
{    
    flecs_bootstrap_make_alive(world, id);
    ecs_add_pair(world, id, EcsChildOf, parent);
    ecs_set_name(world, id, name);

    ecs_assert(ecs_get_name(world, id) != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!parent || parent == EcsFlecsCore) {
        ecs_assert(ecs_lookup(world, name) == id, 
            ECS_INTERNAL_ERROR, NULL);
    }
}

static
void flecs_bootstrap_sanity_check(
    ecs_world_t *world)
{
    (void)world;
#ifdef FLECS_DEBUG
    int32_t i, e, count = flecs_sparse_count(&world->store.tables);
    int32_t total_count = 0;

    for (i = -1; i < count; i ++) {
        ecs_table_t *table;
        if (i == -1) {
            table = &world->store.root;
        } else {
            table = flecs_sparse_get_dense_t(
                &world->store.tables, ecs_table_t, i);
        }
        for (e = 0; e < table->data.count; e ++) {
            ecs_record_t *r = flecs_entities_get(
                world, table->data.entities[e]);
            ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(r->table == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ECS_RECORD_TO_ROW(r->row) == e, 
                ECS_INTERNAL_ERROR, NULL);
            total_count ++;
        }
    }

    count = flecs_entities_count(world);
    ecs_assert(count == total_count, ECS_INTERNAL_ERROR, NULL);

    for (i = 1; i < count; i ++) {
        ecs_entity_t entity = flecs_entities_ids(world)[i];
        ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_record_t *r = flecs_entities_get(world, entity);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(r->dense == (i + 1), ECS_INTERNAL_ERROR, NULL);
        ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(r->table->data.entities[ECS_RECORD_TO_ROW(r->row)] == entity,
            ECS_INTERNAL_ERROR, NULL);
    }
#endif
}

void flecs_bootstrap(
    ecs_world_t *world)
{
    ecs_log_push();

    ecs_set_name_prefix(world, "Ecs");

    /* Ensure builtin ids are alive */
    flecs_bootstrap_make_alive(world, ecs_id(EcsComponent));
    flecs_bootstrap_make_alive(world, ecs_id(EcsIdentifier));
    flecs_bootstrap_make_alive(world, ecs_id(EcsPoly));
    flecs_bootstrap_make_alive(world, ecs_id(EcsParent));
    flecs_bootstrap_make_alive(world, ecs_id(EcsTreeSpawner));
    flecs_bootstrap_make_alive(world, ecs_id(EcsDefaultChildComponent));
    flecs_bootstrap_make_alive(world, EcsFinal);
    flecs_bootstrap_make_alive(world, EcsName);
    flecs_bootstrap_make_alive(world, EcsSymbol);
    flecs_bootstrap_make_alive(world, EcsAlias);
    flecs_bootstrap_make_alive(world, EcsChildOf);
    flecs_bootstrap_make_alive(world, EcsFlecs);
    flecs_bootstrap_make_alive(world, EcsFlecsCore);
    flecs_bootstrap_make_alive(world, EcsOnAdd);
    flecs_bootstrap_make_alive(world, EcsOnRemove);
    flecs_bootstrap_make_alive(world, EcsOnSet);
    flecs_bootstrap_make_alive(world, EcsOnDelete);
    flecs_bootstrap_make_alive(world, EcsPanic);
    flecs_bootstrap_make_alive(world, EcsFlag);
    flecs_bootstrap_make_alive(world, EcsIsA);
    flecs_bootstrap_make_alive(world, EcsWildcard);
    flecs_bootstrap_make_alive(world, EcsAny);
    flecs_bootstrap_make_alive(world, EcsCanToggle);
    flecs_bootstrap_make_alive(world, EcsTrait);
    flecs_bootstrap_make_alive(world, EcsRelationship);
    flecs_bootstrap_make_alive(world, EcsTarget);
    flecs_bootstrap_make_alive(world, EcsSparse);
    flecs_bootstrap_make_alive(world, EcsDontFragment);
    flecs_bootstrap_make_alive(world, EcsObserver);
    flecs_bootstrap_make_alive(world, EcsPairIsTag);

    /* Register type information for builtin components */
    flecs_type_info_init(world, EcsComponent, { 
        .ctor = flecs_default_ctor,
        .on_set = flecs_on_component,
        .on_remove = flecs_on_component
    });

    flecs_type_info_init(world, EcsIdentifier, {
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsIdentifier),
        .copy = ecs_copy(EcsIdentifier),
        .move = ecs_move(EcsIdentifier),
        .on_set = ecs_on_set(EcsIdentifier),
        .on_remove = ecs_on_set(EcsIdentifier)
    });

    flecs_type_info_init(world, EcsPoly, {
        .ctor = flecs_default_ctor,
        .copy = ecs_copy(EcsPoly),
        .move = ecs_move(EcsPoly),
        .dtor = ecs_dtor(EcsPoly)
    });

    flecs_type_info_init(world, EcsParent, { 
        .ctor = flecs_default_ctor
    });

    flecs_type_info_init(world, EcsDefaultChildComponent, { 
        .ctor = flecs_default_ctor,
    });

    /* Create and cache often used id records on world */
    flecs_components_init(world);

    /* Create table for builtin components. This table temporarily stores the 
     * entities associated with builtin components, until they get moved to 
     * other tables once properties are added (see below) */
    ecs_table_t *table = flecs_bootstrap_component_table(world);
    assert(table != NULL);

    /* Bootstrap builtin components */
    flecs_bootstrap_builtin_t(world, table, EcsIdentifier);
    flecs_bootstrap_builtin_t(world, table, EcsComponent);
    flecs_bootstrap_builtin_t(world, table, EcsPoly);
    flecs_bootstrap_builtin_t(world, table, EcsParent);
    flecs_bootstrap_builtin_t(world, table, EcsTreeSpawner);
    flecs_bootstrap_builtin_t(world, table, EcsDefaultChildComponent);

    /* Initialize default entity id range */
    world->info.last_component_id = EcsFirstUserComponentId;
    flecs_entities_max_id(world) = EcsFirstUserEntityId;
    world->info.min_id = 0;
    world->info.max_id = 0;

    /* Register observer for trait before adding EcsPairIsTag */
    ecs_observer(world, {
        .query.terms[0] = { .id = EcsPairIsTag },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_tag,
        .yield_existing = true,
        .global_observer = true
    });

    /* Populate core module */
    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);
    flecs_bootstrap_tag(world, EcsAlias);

    flecs_bootstrap_tag(world, EcsParentDepth);
    flecs_bootstrap_tag(world, EcsQuery);
    flecs_bootstrap_tag(world, EcsObserver);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrivate);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsSlotOf);
    flecs_bootstrap_tag(world, EcsDisabled);
    flecs_bootstrap_tag(world, EcsNotQueryable);
    flecs_bootstrap_tag(world, EcsEmpty);

    /* Initialize builtin modules */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);
    ecs_add_pair(world, EcsFlecs, EcsOnDelete, EcsPanic);

    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);
    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);

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
    flecs_bootstrap_trait(world, EcsTransitive);
    flecs_bootstrap_trait(world, EcsReflexive);
    flecs_bootstrap_trait(world, EcsSymmetric);
    flecs_bootstrap_trait(world, EcsSingleton);
    flecs_bootstrap_trait(world, EcsFinal);
    flecs_bootstrap_trait(world, EcsInheritable);
    flecs_bootstrap_trait(world, EcsPairIsTag);
    flecs_bootstrap_trait(world, EcsExclusive);
    flecs_bootstrap_trait(world, EcsAcyclic);
    flecs_bootstrap_trait(world, EcsTraversable);
    flecs_bootstrap_trait(world, EcsWith);
    flecs_bootstrap_trait(world, EcsOneOf);
    flecs_bootstrap_trait(world, EcsCanToggle);
    flecs_bootstrap_trait(world, EcsTrait);
    flecs_bootstrap_trait(world, EcsRelationship);
    flecs_bootstrap_trait(world, EcsTarget);
    flecs_bootstrap_trait(world, EcsOnDelete);
    flecs_bootstrap_trait(world, EcsOnDeleteTarget);
    flecs_bootstrap_trait(world, EcsOnInstantiate);
    flecs_bootstrap_trait(world, EcsSparse);
    flecs_bootstrap_trait(world, EcsDontFragment);

    flecs_bootstrap_tag(world, EcsRemove);
    flecs_bootstrap_tag(world, EcsDelete);
    flecs_bootstrap_tag(world, EcsPanic);

    flecs_bootstrap_tag(world, EcsOverride);
    flecs_bootstrap_tag(world, EcsInherit);
    flecs_bootstrap_tag(world, EcsDontInherit);

    flecs_bootstrap_tag(world, EcsOrderedChildren);

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
    flecs_bootstrap_entity(world, EcsMonitor, "EcsMonitor", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableCreate, "OnTableCreate", EcsFlecsCore);
    flecs_bootstrap_entity(world, EcsOnTableDelete, "OnTableDelete", EcsFlecsCore);

    /* Constant tag */
    flecs_bootstrap_entity(world, EcsConstant, "constant", EcsFlecsCore);

    /* Sync properties of ChildOf and Identifier with bootstrapped flags */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteTarget, EcsDelete);
    ecs_add_id(world, EcsChildOf, EcsTrait);
    ecs_add_id(world, EcsChildOf, EcsAcyclic);
    ecs_add_id(world, EcsChildOf, EcsTraversable);
    ecs_add_pair(world, EcsChildOf, EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, ecs_id(EcsIdentifier), EcsOnInstantiate, EcsDontInherit);

    /* Register observers for components/relationship properties. Most observers
     * set flags on an component record when a trait is added to a component, which
     * allows for quick trait testing in various operations. */
    ecs_observer(world, {
        .query.terms = {{ .id = EcsFinal }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_final,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t inheritable_trait = { EcsIdInheritable, 0 };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsInheritable }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_trait,
        .ctx = &inheritable_trait,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsOnDelete, EcsWildcard) }
        },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_on_delete,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsOnDeleteTarget, EcsWildcard) }
        },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_on_delete_object,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsOnInstantiate, EcsWildcard) }
        },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_on_instantiate,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsSymmetric }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_symmetric,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsSingleton }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_singleton,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t traversable_trait = { EcsIdTraversable, EcsIdTraversable };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsTraversable }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_trait,
        .ctx = &traversable_trait,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t exclusive_trait = { EcsIdExclusive, 0 };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsExclusive  }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_register_trait,
        .ctx = &exclusive_trait,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t toggle_trait = { EcsIdCanToggle, 0 };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsCanToggle }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_trait,
        .ctx = &toggle_trait,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t with_trait = { EcsIdWith, 0 };
    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsWith, EcsWildcard) },
        },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_trait_pair,
        .ctx = &with_trait,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t sparse_trait = { EcsIdSparse, 0 };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsSparse }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_trait,
        .ctx = &sparse_trait,
        .global_observer = true
    });

    static ecs_on_trait_ctx_t dont_fragment_trait = { EcsIdDontFragment, 0 };
    ecs_observer(world, {
        .query.terms = {{ .id = EcsDontFragment }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_trait,
        .ctx = &dont_fragment_trait,
        .global_observer = true
    });

    ecs_observer(world, {
        .query.terms = {{ .id = EcsOrderedChildren }},
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd,  EcsOnRemove},
        .callback = flecs_register_ordered_children,
        .global_observer = true
    });

    /* Entities used as slot are marked as exclusive to ensure a slot can always
     * only point to a single entity. */
    ecs_observer(world, {
        .query.terms = {
            { .id = ecs_pair(EcsSlotOf, EcsWildcard) }
        },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_register_slot_of,
        .global_observer = true
    });

    /* Define observer to make sure that adding a module to a child entity also
     * adds it to the parent. */
    ecs_observer(world, {
        .query.terms = {{ .id = EcsModule } },
        .query.flags = EcsQueryMatchPrefab|EcsQueryMatchDisabled,
        .events = {EcsOnAdd},
        .callback = flecs_ensure_module_tag,
        .global_observer = true
    });

    /* Observer that tracks whether observers are disabled */
    ecs_observer(world, {
        .query.terms = {
            { .id = EcsObserver },
            { .id = EcsDisabled },
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_disable_observer,
        .global_observer = true
    });

    /* Observer that tracks whether modules are disabled */
    ecs_observer(world, {
        .query.terms = {
            { .id = EcsModule },
            { .id = EcsDisabled },
        },
        .events = {EcsOnAdd, EcsOnRemove},
        .callback = flecs_disable_module,
        .global_observer = true
    });

    /* Exclusive properties */
    ecs_add_id(world, EcsChildOf, EcsExclusive);
    ecs_add_id(world, EcsOnDelete, EcsExclusive);
    ecs_add_id(world, EcsOnDeleteTarget, EcsExclusive);
    ecs_add_id(world, EcsOnInstantiate, EcsExclusive);
    ecs_add_id(world, EcsParentDepth, EcsExclusive);

    /* Unqueryable entities */
    ecs_add_id(world, EcsThis, EcsNotQueryable);
    ecs_add_id(world, EcsWildcard, EcsNotQueryable);
    ecs_add_id(world, EcsAny, EcsNotQueryable);
    ecs_add_id(world, EcsVariable, EcsNotQueryable);

    /* Tag relationships (relationships that should never have data) */
    ecs_add_id(world, EcsIsA, EcsPairIsTag);
    ecs_add_id(world, EcsChildOf, EcsPairIsTag);
    ecs_add_id(world, EcsSlotOf, EcsPairIsTag);
    ecs_add_id(world, EcsDependsOn, EcsPairIsTag);
    ecs_add_id(world, EcsFlag, EcsPairIsTag);
    ecs_add_id(world, EcsWith, EcsPairIsTag);

    /* Relationships */
    ecs_add_id(world, EcsChildOf, EcsRelationship);
    ecs_add_id(world, EcsIsA, EcsRelationship);
    ecs_add_id(world, EcsSlotOf, EcsRelationship);
    ecs_add_id(world, EcsDependsOn, EcsRelationship);
    ecs_add_id(world, EcsWith, EcsRelationship);
    ecs_add_id(world, EcsOnDelete, EcsRelationship);
    ecs_add_id(world, EcsOnDeleteTarget, EcsRelationship);
    ecs_add_id(world, EcsOnInstantiate, EcsRelationship);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsRelationship);

    /* Targets */
    ecs_add_id(world, EcsOverride, EcsTarget);
    ecs_add_id(world, EcsInherit, EcsTarget);
    ecs_add_id(world, EcsDontInherit, EcsTarget);

    /* Traversable relationships are always acyclic */
    ecs_add_pair(world, EcsTraversable, EcsWith, EcsAcyclic);

    /* Transitive relationships are always Traversable */
    ecs_add_pair(world, EcsTransitive, EcsWith, EcsTraversable);

    /* DontFragment components are always sparse */
    ecs_add_pair(world, EcsDontFragment, EcsWith, EcsSparse);

    /* DontInherit components */
    ecs_add_pair(world, EcsPrefab, EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, ecs_id(EcsComponent), EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, EcsOnDelete, EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, EcsExclusive, EcsOnInstantiate, EcsDontInherit);
    ecs_add_pair(world, EcsDontFragment, EcsOnInstantiate, EcsDontInherit);

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

    /* Private properties */
    ecs_add_id(world, ecs_id(EcsPoly), EcsPrivate);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsPrivate);

    /* Inherited components */
    ecs_add_pair(world, EcsIsA, EcsOnInstantiate, EcsInherit);
    ecs_add_pair(world, EcsDependsOn, EcsOnInstantiate, EcsInherit);
    
    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_entity_name(world);
    flecs_bootstrap_parent_component(world);
    flecs_bootstrap_spawner(world);

    ecs_set_scope(world, 0);
    ecs_set_name_prefix(world, NULL);

    ecs_assert(world->cr_childof_wildcard != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->cr_isa_wildcard != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Verify that all entities are where they're supposed to be */
    flecs_bootstrap_sanity_check(world);

    ecs_log_pop();
}
