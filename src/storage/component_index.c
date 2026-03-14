/**
 * @file storage/component_index.c
 * @brief Index for looking up tables by component id.
 *
 * Each in-use component id has a component record containing a table cache that
 * maps to all tables with that id. Used by queries and entity component access.
 */

#include "../private_api.h"

/* Get the linked list element in 'cr' at the same offset as 'list' is in 'head'. */
static
ecs_id_record_elem_t* flecs_component_elem(
    ecs_component_record_t *head,
    ecs_id_record_elem_t *list,
    ecs_component_record_t *cr)
{
    return ECS_OFFSET(cr->pair, (uintptr_t)list - (uintptr_t)head->pair);
}

/* Insert a component record into a linked list after the head element. */
static
void flecs_component_elem_insert(
    ecs_component_record_t *head,
    ecs_component_record_t *cr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_elem_t *head_elem = flecs_component_elem(cr, elem, head);
    ecs_component_record_t *cur = head_elem->next;
    elem->next = cur;
    elem->prev = head;
    if (cur) {
        ecs_id_record_elem_t *cur_elem = flecs_component_elem(cr, elem, cur);
        cur_elem->prev = cr;
    }
    head_elem->next = cr;
}

/* Remove a component record from a linked list. */
static
void flecs_component_elem_remove(
    ecs_component_record_t *cr,
    ecs_id_record_elem_t *elem)
{
    ecs_component_record_t *prev = elem->prev;
    ecs_component_record_t *next = elem->next;
    ecs_assert(prev != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_record_elem_t *prev_elem = flecs_component_elem(cr, elem, prev);
    prev_elem->next = next;
    if (next) {
        ecs_id_record_elem_t *next_elem = flecs_component_elem(cr, elem, next);
        next_elem->prev = prev;
    }
}

/* Insert a component record into the appropriate wildcard linked list. */
static
void flecs_insert_id_elem(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_id_t wildcard,
    ecs_component_record_t *wcr)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);
    if (!wcr) {
        wcr = flecs_components_ensure(world, wildcard);
    }
    ecs_assert(wcr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_pair_record_t *pair = cr->pair;
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_insert(wcr, cr, &pair->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_insert(wcr, cr, &pair->second);

        if (cr->flags & EcsIdTraversable) {
            flecs_component_elem_insert(wcr, cr, &pair->trav);
        }
    }
}

/* Remove a component record from the appropriate wildcard linked list. */
static
void flecs_remove_id_elem(
    ecs_component_record_t *cr,
    ecs_id_t wildcard)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);

    ecs_pair_record_t *pair = cr->pair;
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_remove(cr, &pair->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        if (pair->second.prev) {
            flecs_component_elem_remove(cr, &pair->second);
        }

        if (cr->flags & EcsIdTraversable) {
            flecs_component_elem_remove(cr, &pair->trav);
        }
    }
}

/* Compute hash key for a component id, normalizing EcsAny to EcsWildcard and stripping generation. */
static
ecs_id_t flecs_component_hash(
    ecs_id_t id)
{
    id = ecs_strip_generation(id);
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t r = ECS_PAIR_FIRST(id);
        ecs_entity_t t = ECS_PAIR_SECOND(id);

        if (ECS_IS_VALUE_PAIR(id)) {
            id = ecs_value_pair(r, t);
        } else {
            if (r == EcsAny) {
                r = EcsWildcard;
            }
            if (t == EcsAny) {
                t = EcsWildcard;
            }
            id = ecs_pair(r, t);
        }
    }
    return id;
}

/* Initialize sparse storage for a component record. */
void flecs_component_init_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (!ecs_id_is_wildcard(cr->id)) {
        if (!cr->sparse) {
            if (cr->flags & EcsIdSparse) {
                cr->sparse = flecs_walloc_t(world, ecs_sparse_t);
                if (cr->type_info) {
                    flecs_sparse_init(cr->sparse, NULL, NULL, cr->type_info->size);
                } else {
                    flecs_sparse_init(cr->sparse, NULL, NULL, 0);
                }
            }
        }

        if (cr->id < FLECS_HI_COMPONENT_ID) {
            world->non_trivial_lookup[cr->id] |= EcsNonTrivialIdSparse;
        }
    } else if (ECS_IS_PAIR(cr->id)) {
        if (cr->flags & EcsIdDontFragment) {
            if (cr->sparse) {
                ecs_assert(flecs_sparse_count(cr->sparse) == 0,
                    ECS_INTERNAL_ERROR, NULL);
                flecs_sparse_fini(cr->sparse);
            } else {
                cr->sparse = flecs_walloc_t(world, ecs_sparse_t);
            }

            if (cr->flags & EcsIdExclusive) {
                flecs_sparse_init_t(cr->sparse, NULL, NULL, ecs_entity_t);
            } else {
                flecs_sparse_init_t(cr->sparse, NULL, NULL, ecs_type_t);
            }
        }
    }
}

/* Initialize non-fragmenting storage for a component record. */
void flecs_component_record_init_dont_fragment(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (world->cr_non_fragmenting_head) {
        world->cr_non_fragmenting_head->non_fragmenting.prev = cr;
    }

    cr->non_fragmenting.next = world->cr_non_fragmenting_head;
    world->cr_non_fragmenting_head = cr;

    if (cr->id < FLECS_HI_COMPONENT_ID) {
        world->non_trivial_lookup[cr->id] |= EcsNonTrivialIdNonFragmenting;
    }

    flecs_component_init_sparse(world, cr);

    ecs_vec_init_t(&world->allocator, &cr->dont_fragment_tables, uint64_t, 0);
}

/* Clean up non-fragmenting storage and clear associated table edges. */
static
void flecs_component_record_fini_dont_fragment(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (world->cr_non_fragmenting_head == cr) {
        world->cr_non_fragmenting_head = cr->non_fragmenting.next;
    }

    if (cr->non_fragmenting.prev) {
        cr->non_fragmenting.prev->non_fragmenting.next = 
            cr->non_fragmenting.next;
    }

    if (cr->non_fragmenting.next) {
        cr->non_fragmenting.next->non_fragmenting.prev = 
            cr->non_fragmenting.prev;
    }

    int32_t i, count = ecs_vec_count(&cr->dont_fragment_tables);
    uint64_t *tables = ecs_vec_first(&cr->dont_fragment_tables);
    for (i = 0; i < count; i ++) {
        uint64_t table_id = tables[i];
        ecs_table_t *table = NULL;
        if (table_id) {
            table = flecs_sparse_get_t(
                &world->store.tables, ecs_table_t, table_id);
        } else {
            table = &world->store.root;
        }

        if (table) {
            flecs_table_clear_edges_for_id(world, table, cr->id);
        } else {
            /* Table was deleted */
        }
    }

    ecs_vec_fini_t(&world->allocator, &cr->dont_fragment_tables, uint64_t);
}

/* Initialize exclusive relationship storage for a component record. */
void flecs_component_record_init_exclusive(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_component_init_sparse(world, cr);
}

/* Finalize and free sparse storage for a component record.
 * For DontFragment records, removes all sparse entries before cleanup. */
static
void flecs_component_fini_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (cr->sparse) {
        ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
        if (cr->flags & EcsIdDontFragment) {
            flecs_component_sparse_remove_all(world, cr);
        }

        flecs_sparse_fini(cr->sparse);
        flecs_wfree_t(world, ecs_sparse_t, cr->sparse);
    }
}

/* Compute event observer flags for a given component id. */
static
ecs_flags32_t flecs_component_event_flags(
    const ecs_world_t *world,
    ecs_id_t id)
{
    const ecs_observable_t *o = &world->observable;
    ecs_flags32_t result = 0;
    result |= flecs_observers_exist(o, id, EcsOnAdd) * EcsIdHasOnAdd;
    result |= flecs_observers_exist(o, id, EcsOnRemove) * EcsIdHasOnRemove;
    result |= flecs_observers_exist(o, id, EcsOnSet) * EcsIdHasOnSet;
    result |= flecs_observers_exist(o, id, EcsOnTableCreate) * EcsIdHasOnTableCreate;
    result |= flecs_observers_exist(o, id, EcsOnTableDelete) * EcsIdHasOnTableDelete;
    result |= flecs_observers_exist(o, id, EcsWildcard) * (
         EcsIdHasOnAdd
        |EcsIdHasOnRemove
        |EcsIdHasOnSet
        |EcsIdHasOnTableCreate
        |EcsIdHasOnTableDelete);

    return result;
}

/* Initialize ordered children tracking for a component record. */
void flecs_component_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    cr->flags |= EcsIdOrderedChildren;
    flecs_ordered_children_init(world, cr);
}

/* Derive component record flags from the relationship entity's cached trait_flags.
 * If the target owns the type info, re-derives storage flags from the target. */
static
ecs_flags32_t flecs_component_get_flags_intern(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_entity_t tgt,
    const ecs_type_info_t *ti)
{
    ecs_flags32_t result = 0;

    if (id & ECS_ID_FLAGS_MASK) {
        if ((id & ECS_ID_FLAGS_MASK) != ECS_PAIR) {
            return 0;
        }
    }

    ecs_record_t *r = flecs_entities_get_any(world, rel);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;

    result = table->trait_flags;

    if (tgt) {
        if (ti && (ti->component == tgt)) {
            /* Target provides the type info; re-derive Sparse/DontFragment from target */
            result &= ~EcsIdSparse;
            result &= ~EcsIdDontFragment;

            if (ecs_owns_id(world, tgt, EcsSparse)) {
                result |= EcsIdSparse;
            }

            if (ecs_owns_id(world, tgt, EcsDontFragment)) {
                result |= EcsIdDontFragment;
            }
        }
    } else {
        /* Non-pair id: clear flags that only apply to pair relationships */
        result &= ~(EcsIdExclusive|EcsIdTraversable|EcsIdPairIsTag|EcsIdIsTransitive);
    }

    return result;
}

/* Return flags for a component id, looking up or computing them as needed. */
ecs_flags32_t flecs_component_get_flags(
    const ecs_world_t *world,
    ecs_id_t id)
{
    const ecs_component_record_t *cr = flecs_components_get(world, id);
    if (cr) {
        return cr->flags;
    }

    const ecs_type_info_t *ti = flecs_determine_type_info_for_component(world, id);
    ecs_entity_t rel = 0, tgt = 0;

    if (ECS_IS_PAIR(id)) {
        rel = ECS_PAIR_FIRST(id);
        rel = flecs_entities_get_alive(world, rel);

        tgt = ECS_PAIR_SECOND(id);
        tgt = flecs_entities_get_alive(world, tgt);
    } else {
        rel = id & ECS_COMPONENT_MASK;
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);
    }

    return flecs_component_get_flags_intern(world, id, rel, tgt, ti);
}

/* Validate relationship and target constraints for a component record. */
static
void flecs_component_record_check_constraints(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t rel,
    ecs_entity_t tgt)
{
    (void)world;
    (void)cr;
    (void)rel;
    (void)tgt;

#ifdef FLECS_DEBUG
    if (ECS_HAS_ID_FLAG(cr->id, PAIR) && !ECS_IS_PAIR(cr->id)) {
        return;
    }

    if (ECS_IS_PAIR(cr->id)) {
        /* Skip validation for internal (EcsFlag, X) records */
        if (rel == EcsFlag) {
            return;
        }

        if (tgt) {
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

            /* Relationship trait cannot be used as target */
            if (ecs_has_id(world, tgt, EcsRelationship)) {
                if (!ecs_id_is_wildcard(rel) && 
                    !ecs_has_id(world, rel, EcsTrait))
                {
                    ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as target"
                        " in pair '%s': '%s' has the Relationship trait",
                            flecs_errstr(ecs_get_path(world, tgt)),
                            flecs_errstr_1(ecs_id_str(world, cr->id)),
                            flecs_errstr_2(ecs_get_path(world, tgt)));
                }
            }
        }

        if (ecs_has_id(world, rel, EcsTarget)) {
            ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as relationship "
                "in pair '%s': '%s' has the Target trait",
                    flecs_errstr(ecs_get_path(world, rel)),
                    flecs_errstr_1(ecs_id_str(world, cr->id)),
                    flecs_errstr_2(ecs_get_path(world, rel)));
        }

        if (tgt && !ecs_id_is_wildcard(tgt)) {
            /* Validate OneOf constraint */
            ecs_entity_t oneof = flecs_get_oneof(world, rel);
            if (oneof) {
                if (!ecs_has_pair(world, tgt, EcsChildOf, oneof)) {
                    if (oneof == rel) {
                        ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                            "cannot use '%s' as target in pair '%s': "
                            "relationship '%s' has the OneOf trait and '%s' "
                            "is not a child of '%s'",
                                flecs_errstr(ecs_get_path(world, tgt)),
                                flecs_errstr_1(ecs_id_str(world, cr->id)),
                                flecs_errstr_2(ecs_get_path(world, rel)),
                                flecs_errstr_3(ecs_get_path(world, tgt)),
                                flecs_errstr_4(ecs_get_path(world, rel)));
                        } else {
                            ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                                "cannot use '%s' as target in pair '%s': "
                                "relationship '%s' has (OneOf, %s) and '%s' "
                                "is not a child of '%s'",
                                    flecs_errstr(ecs_get_path(world, tgt)),
                                    flecs_errstr_1(ecs_id_str(world, cr->id)),
                                    flecs_errstr_2(ecs_get_path(world, rel)),
                                    flecs_errstr_3(ecs_get_path(world, oneof)),
                                    flecs_errstr_4(ecs_get_path(world, tgt)),
                                    flecs_errstr_5(ecs_get_path(world, oneof)));
                        }
                }
            }

            /* Prevent inheritance from Final targets */
            if (rel == EcsIsA) {
                if (ecs_has_id(world, tgt, EcsFinal)) {
                    ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                        "cannot add '(IsA, %s)': '%s' has the Final trait",
                            flecs_errstr(ecs_get_path(world, tgt)),
                            flecs_errstr_1(ecs_get_path(world, tgt)));
                }

                if (flecs_component_is_trait_locked(world, tgt)) {
                    if (!ecs_has_id(world, tgt, EcsInheritable) && !ecs_has_pair(world, tgt, EcsIsA, EcsWildcard)) {
                        ecs_throw(ECS_INVALID_OPERATION, 
                            "cannot add '(IsA, %s)': '%s' is already queried for",
                                    flecs_errstr(ecs_get_path(world, tgt)),
                                    flecs_errstr_1(ecs_get_path(world, tgt)));
                    }
                }
            }
        }
    } else {
        bool is_tgt = false;
        if (ecs_has_id(world, rel, EcsRelationship) ||
            (is_tgt = ecs_has_id(world, rel, EcsTarget)))
        {
            if (is_tgt) {
                ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                    "cannot use '%s' by itself: it has the Target trait and "
                    "must be used in pair with relationship",
                        flecs_errstr(ecs_get_path(world, rel)));
            } else {
                ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                    "cannot use '%s' by itself: it has the Relationship trait "
                    "and must be used in pair with target",
                        flecs_errstr(ecs_get_path(world, rel)));
            }
        }
    }
error:
    return;
#endif
}

/* Allocate and initialize a new component record for the given id. */
static
ecs_component_record_t* flecs_component_new(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_component_record_t *cr, *cr_t = NULL;
    ecs_id_t hash = flecs_component_hash(id);
    cr = flecs_bcalloc_w_dbg_info(
        &world->allocators.component_record, "ecs_component_record_t");

    if (hash >= FLECS_HI_ID_RECORD_ID) {
        ecs_map_insert_ptr(&world->id_index_hi, hash, cr);
    } else {
        world->id_index_lo[hash] = cr;
    }

    ecs_table_cache_init(world, &cr->cache);

    cr->id = id;
    cr->refcount = 1;

    bool is_wildcard = ecs_id_is_wildcard(id);
    bool is_pair = ECS_IS_PAIR(id);
    bool is_value_pair = ECS_IS_VALUE_PAIR(id);

    ecs_entity_t rel = 0, tgt = 0;
    ecs_table_t *tgt_table = NULL;
    ecs_record_t *tgt_r = NULL;

    if (is_pair) {
        cr->pair = flecs_bcalloc_w_dbg_info(
            &world->allocators.pair_record, "ecs_pair_record_t");
        cr->pair->reachable.current = -1;

        rel = ECS_PAIR_FIRST(id);
        if (!is_value_pair) {
            tgt = ECS_PAIR_SECOND(id);
        }

        /* Link to (R, *) parent and insert into wildcard lists */
        if (!is_wildcard && (rel != EcsFlag)) {
            ecs_id_t parent_id = ecs_pair(rel, EcsWildcard);
            ecs_component_record_t *cr_r = flecs_components_ensure(
                world, parent_id);
            cr->pair->parent = cr_r;
            cr->flags = cr_r->flags;

            flecs_insert_id_elem(world, cr, parent_id, cr_r);

            if (tgt) {
                ecs_id_t wc_tgt = ecs_pair(EcsWildcard, tgt);
                cr_t = flecs_components_ensure(world, wc_tgt);
                flecs_insert_id_elem(world, cr, wc_tgt, cr_t);
            }
        }

        /* Target can be 0: tables without ChildOf use (ChildOf, 0) */
        if (tgt) {
            ecs_entity_t alive_tgt = flecs_entities_get_alive(world, tgt);
            ecs_assert(alive_tgt != 0, ECS_INVALID_PARAMETER,
                "target '%s' of pair '%s' is not alive",
                    flecs_errstr(ecs_id_str(world, tgt)), 
                    flecs_errstr_1(ecs_id_str(world, cr->id)));

            tgt = alive_tgt;

            if (tgt != EcsWildcard) {
                tgt_r = flecs_entities_get(world, tgt);
                ecs_assert(tgt_r != NULL, ECS_INTERNAL_ERROR, NULL);
                tgt_table = tgt_r->table;

                flecs_record_add_flag(tgt_r, EcsEntityIsTarget);

                /* ChildOf traits are known upfront, set directly */
                if (rel == EcsChildOf) {
                    /* Mark prefab children for instance-to-prefab child lookup */
                    if (tgt_table->flags & EcsTableIsPrefab) {
                        cr->flags |= EcsIdPrefabChildren;
                    }

                    /* Init ordered children if parent has OrderedChildren */
                    if (ecs_table_has_id(
                        world, tgt_table, EcsOrderedChildren)) 
                    {
                        flecs_component_ordered_children_init(world, cr);
                    }

                    flecs_component_update_childof_depth(
                        world, cr, tgt, tgt_r);

                    cr->flags |= EcsIdOnDeleteTargetDelete | 
                        EcsIdOnInstantiateDontInherit | EcsIdTraversable | 
                        EcsIdPairIsTag | EcsIdExclusive;

                    if (tgt && tgt != EcsWildcard) {
                        ecs_assert(tgt_r != NULL, ECS_INTERNAL_ERROR, NULL);
                        flecs_record_add_flag(tgt_r, EcsEntityIsTraversable);
                    }
                }
            }
        }
    } else {
        rel = id & ECS_COMPONENT_MASK;
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);   
    }

    /* Wildcards don't need trait flags, ChildOf is already initialized */
    if (rel != EcsWildcard && rel != EcsChildOf) {
        rel = flecs_entities_get_alive(world, rel);

        cr->type_info = flecs_determine_type_info_for_component(world, id);
        cr->flags |= flecs_component_get_flags_intern(
            world, id, rel, tgt, cr->type_info);

        flecs_add_flag(world, rel, EcsEntityIsId);

        if (tgt && tgt != EcsWildcard) {
            if (cr->flags & EcsIdTraversable) {
                ecs_assert(tgt_r != NULL, ECS_INTERNAL_ERROR, NULL);
                flecs_record_add_flag(tgt_r, EcsEntityIsTraversable);
            }
        }

        if (cr->flags & EcsIdSparse) {
            flecs_component_init_sparse(world, cr);
        }

        if (cr->flags & EcsIdDontFragment) {
            flecs_component_record_init_dont_fragment(world, cr);
            
            if (cr_t) {
                /* Let (*, tgt) queries detect non-fragmenting records */
                cr_t->flags |= EcsIdMatchDontFragment;
            }
        }
    }

    cr->flags |= flecs_component_event_flags(world, id);

    flecs_component_record_check_constraints(world, cr, rel, tgt);

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]component record#[normal] %s #[green]created", id_str);
        ecs_os_free(id_str);
    }

#ifdef FLECS_DEBUG_INFO
    cr->str = ecs_id_str(world, cr->id);
#endif

    world->info.id_create_total ++;
    world->info.component_id_count += cr->type_info != NULL;
    world->info.tag_id_count += cr->type_info == NULL;
    world->info.pair_id_count += is_pair;

    return cr;
}

/* Assert that a component record has no tables in its cache. */
static
void flecs_component_assert_empty(
    ecs_component_record_t *cr)
{
    (void)cr;
    ecs_assert(flecs_table_cache_count(&cr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);
}

/* Free a component record and all its associated resources. */
static
void flecs_component_free(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t id = cr->id;

    flecs_component_assert_empty(cr);

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        if (!ecs_id_is_wildcard(id)) {
            if (ECS_PAIR_FIRST(id) != EcsFlag) {
                /* Remove from wildcard lists */
                flecs_remove_id_elem(cr, ecs_pair(rel, EcsWildcard));
                if (!ECS_IS_VALUE_PAIR(id)) {
                    flecs_remove_id_elem(cr, ecs_pair(EcsWildcard, tgt));
                }
            }
        } else {
            ecs_log_push_2();

            /* Wildcard record: release all matching component records */
            if (ECS_PAIR_FIRST(id) == EcsWildcard) {
                /* Iterate (*, Target) list */
                ecs_component_record_t *cur, *next = cr->pair->second.next;
                while ((cur = next)) {
                    flecs_component_assert_empty(cur);
                    next = cur->pair->second.next;
                    flecs_component_release(world, cur);
                }
            } else {
                /* Iterate (Relationship, *) list */
                ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_component_record_t *cur, *next = cr->pair->first.next;
                while ((cur = next)) {
                    flecs_component_assert_empty(cur);
                    next = cur->pair->first.next;
                    flecs_component_release(world, cur);
                }
            }

            ecs_log_pop_2();
        }
    }

    flecs_component_fini_sparse(world, cr);

    if (cr->flags & EcsIdDontFragment) {
        flecs_component_record_fini_dont_fragment(world, cr);
    }

    world->info.id_delete_total ++;
    world->info.pair_id_count -= ECS_IS_PAIR(id);
    world->info.component_id_count -= cr->type_info != NULL;
    world->info.tag_id_count -= cr->type_info == NULL;

    ecs_table_cache_fini(&cr->cache);

    if (cr->pair) {
        flecs_ordered_children_fini(world, cr);
        flecs_name_index_free(cr->pair->name_index);
        ecs_vec_fini_t(&world->allocator, &cr->pair->reachable.ids, 
            ecs_reachable_elem_t);
        flecs_bfree_w_dbg_info(&world->allocators.pair_record, 
                cr->pair, "ecs_pair_record_t");
    }

    ecs_id_t hash = flecs_component_hash(id);
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        ecs_map_remove(&world->id_index_hi, hash);
    } else {
        world->id_index_lo[hash] = NULL;
    }

#ifdef FLECS_DEBUG_INFO
    ecs_os_free(cr->str);
#endif

    flecs_bfree_w_dbg_info(&world->allocators.component_record, 
        cr, "ecs_component_record_t");

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]component record#[normal] %s #[red]deleted", id_str);
        ecs_os_free(id_str);
    }
}

/* Ensure a component record exists for the given id, creating one if needed. */
ecs_component_record_t* flecs_components_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        cr = flecs_component_new(world, id);
    }
    return cr;
}

/* Look up a component record by id, returning NULL if not found. */
ecs_component_record_t* flecs_components_get(
    const ecs_world_t *world,
    ecs_id_t id)
{
    flecs_poly_assert(world, ecs_world_t);
    if (id == ecs_pair(EcsIsA, EcsWildcard)) {
        return world->cr_isa_wildcard;
    } else if (id == ecs_pair(EcsChildOf, EcsWildcard)) {
        return world->cr_childof_wildcard;
    } else if (id == ecs_pair_t(EcsIdentifier, EcsName)) {
        return world->cr_identifier_name;
    }

    ecs_id_t hash = flecs_component_hash(id);
    ecs_component_record_t *cr = NULL;
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        cr = ecs_map_get_deref(&world->id_index_hi, ecs_component_record_t, hash);
    } else {
        cr = world->id_index_lo[hash];
    }

    return cr;
}

/* Ensure a component record exists, but only create if not in threaded mode. */
ecs_component_record_t* flecs_components_try_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        if (!(world->flags & EcsWorldMultiThreaded)) {
            cr = flecs_component_new(world, id);
        }
    }
    return cr;
}

/* Increase refcount of a component record. */
void flecs_component_claim(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    (void)world;
    cr->refcount ++;
}

/* Decrease refcount of a component record, freeing it when it reaches zero. */
int32_t flecs_component_release(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    int32_t rc = -- cr->refcount;

    ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, 
        flecs_errstr(ecs_id_str(world, cr->id)));

    if (!rc) {
        flecs_component_free(world, cr);
    }

    return rc;
}

/* Delete empty, non-kept tables from a component record's cache.
 * Returns true if any tables remain (non-empty or kept). */
bool flecs_component_release_tables(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    bool remaining = false;

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_all_iter(&cr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;

            if (table->keep) {
                remaining = true;
                continue;
            }

            if (ecs_table_count(table)) {
                remaining = true;
                continue;
            }

            flecs_table_fini(world, tr->hdr.table);
        }
    }

    return remaining;
}

/* Set type info for a component record, updating world counters. */
bool flecs_component_set_type_info(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_type_info_t *ti)
{
    bool is_wildcard = ecs_id_is_wildcard(cr->id);
    if (!is_wildcard) {
        if (ti) {
            if (!cr->type_info) {
                world->info.tag_id_count --;
                world->info.component_id_count ++;
            }
        } else {
            if (cr->type_info) {
                world->info.tag_id_count ++;
                world->info.component_id_count --;
            }
        }
    }

    bool changed = cr->type_info != ti;
    cr->type_info = ti;

    return changed;
}

/* Return the type info associated with a component record. */
const ecs_type_info_t* flecs_component_get_type_info(
    const ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->type_info;
}

/* Ensure a name index exists for a component record, creating one if needed. */
ecs_hashmap_t* flecs_component_name_index_ensure(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_hashmap_t *map = cr->pair->name_index;
    if (!map) {
        map = cr->pair->name_index = flecs_name_index_new(&world->allocator);
    }

    return map;
}

/* Return the name index for a component record, or NULL if none exists. */
ecs_hashmap_t* flecs_component_name_index_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_poly_assert(world, ecs_world_t);
    (void)world;
    return cr->pair->name_index;
}

/* Return the table record for a given table in a component record's cache. */
const ecs_table_record_t* flecs_component_get_table(
    const ecs_component_record_t *cr,
    const ecs_table_t *table)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_table_cache_get(&cr->cache, table);
}

/* Bootstrap cached component records for commonly used ids. */
void flecs_components_init(
    ecs_world_t *world)
{
    world->cr_wildcard = flecs_components_ensure(world, EcsWildcard);
    world->cr_wildcard_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsWildcard, EcsWildcard));
    world->cr_any = flecs_components_ensure(world, EcsAny);
    world->cr_isa_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsIsA, EcsWildcard));
}

/* Clean up all component records in the world. */
void flecs_components_fini(
    ecs_world_t *world)
{
    /* Delete one at a time since records can recursively delete each other */
    while (ecs_map_count(&world->id_index_hi) > 0) {
        ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
        ecs_map_next(&it);
        flecs_component_release(world, ecs_map_ptr(&it));
    }

    int32_t i;
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i ++) {
        ecs_component_record_t *cr = world->id_index_lo[i];
        if (cr) {
            flecs_component_release(world, cr);
        }
    }

    ecs_assert(ecs_map_count(&world->id_index_hi) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_map_fini(&world->id_index_hi);
    ecs_os_free(world->id_index_lo);
}

/* Get flags for an id, adding synthetic OnAdd/OnRemove flags if inheritable. */
static
ecs_flags32_t flecs_id_flags(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_flags32_t cr_flags = flecs_component_get_flags(world, id);
    ecs_flags32_t extra_flags = 0;
    if (cr_flags & EcsIdOnInstantiateInherit) {
        extra_flags |= EcsIdHasOnAdd|EcsIdHasOnRemove;
    }
    return cr_flags|extra_flags;
}

/* Return combined flags for an id, its wildcard variants, and the Any record. */
ecs_flags32_t flecs_id_flags_get(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_flags32_t result = flecs_id_flags(world, id);

    if (id != EcsAny) {
        result |= flecs_id_flags(world, EcsAny);
    }

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t first = ECS_PAIR_FIRST(id);
        ecs_entity_t second = ECS_PAIR_SECOND(id);

        if (id != ecs_pair(first, EcsWildcard)) {
            result |= flecs_id_flags(world, ecs_pair(first, EcsWildcard));
        }
        if (id != ecs_pair(EcsWildcard, second)) {
            result |= flecs_id_flags(world, ecs_pair(EcsWildcard, second));
        }
        if (id != ecs_pair(EcsWildcard, EcsWildcard)) {
            result |= flecs_id_flags(world, ecs_pair(EcsWildcard, EcsWildcard));
        }

        if (first == EcsIsA) {
            result |= EcsIdHasOnAdd|EcsIdHasOnRemove;
        }
    } else if (id != EcsWildcard) {
        result |= flecs_id_flags(world, EcsWildcard);
    }

    return result;
}

/* Reclaim unused memory in a component record's table cache and name index. */
void flecs_component_shrink(
    ecs_component_record_t *cr)
{
    ecs_map_reclaim(&cr->cache.index);

    ecs_pair_record_t *pr = cr->pair;
    if (pr) {
        if (pr->name_index) {
            ecs_map_reclaim(&pr->name_index->impl);
        }
    }
}

/* Delete all entities that have a sparse component (deletes the entities, not just the component). */
void flecs_component_delete_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
    int32_t i, count = flecs_sparse_count(cr->sparse);
    if (!count) {
        return;
    }

    const uint64_t *entities = flecs_sparse_ids(cr->sparse);
    ecs_entity_t *to_delete = ecs_os_malloc_n(ecs_entity_t, count);
    ecs_os_memcpy_n(to_delete, entities, ecs_entity_t, count);

    for (i = 0; i < count; i ++) {
        ecs_delete(world, to_delete[i]);
    }

    ecs_os_free(to_delete);
}

/* Return the next record in the (R, *) linked list (same relationship). */
ecs_component_record_t* flecs_component_first_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->first.next;
}

/* Return the next record in the (*, T) linked list (same target). */
ecs_component_record_t* flecs_component_second_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->second.next;
}

/* Return the next record in the traversable (*, T) linked list. */
ecs_component_record_t* flecs_component_trav_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->trav.next;
}

/* Initialize an iterator over all tables in a component record's cache. */
bool flecs_component_iter(
    const ecs_component_record_t *cr,
    ecs_table_cache_iter_t *iter_out)
{
    return flecs_table_cache_all_iter(&cr->cache, iter_out);
}

/* Advance a component record table cache iterator and return the next record. */
const ecs_table_record_t* flecs_component_next(
    ecs_table_cache_iter_t *iter)
{
    return flecs_table_cache_next(iter, ecs_table_record_t);
}

/* Return the id associated with a component record. */
ecs_id_t flecs_component_get_id(
    const ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INVALID_PARAMETER, NULL);
    return cr->id;
}

/* Return the parent record for a given table in a component record. */
ecs_parent_record_t* flecs_component_get_parent_record(
    const ecs_component_record_t *cr,
    const ecs_table_t *table)
{
    ecs_assert(cr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_pair_record_t *pair = cr->pair;

    if (!pair) {
        return NULL;
    }

    if (!ecs_map_is_init(&pair->children_tables)) {
        return NULL;
    }

    return (ecs_parent_record_t*)ecs_map_get(&pair->children_tables, table->id);
}

/* Return the cached hierarchy depth for a ChildOf component record. */
int32_t flecs_component_get_childof_depth(
    const ecs_component_record_t *cr)
{
    ecs_assert(ECS_IS_PAIR(cr->id), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ECS_PAIR_FIRST(cr->id) == EcsChildOf, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(cr->pair != NULL, ECS_INVALID_PARAMETER, NULL);
    return cr->pair->depth;
}

/* Recursively propagate updated hierarchy depth to child entities. */
static
void flecs_entities_update_childof_depth(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (cr->flags & EcsIdOrderedChildren) {
        ecs_entity_t *entities = ecs_vec_first(&cr->pair->ordered_children);
        int32_t i, count = ecs_vec_count(&cr->pair->ordered_children);
        for (i = 0; i < count; i ++) {
            ecs_entity_t tgt = entities[i];
            ecs_record_t *r = flecs_entities_get(world, tgt);
            ecs_table_t *table = r->table;

            if (table->flags & EcsTableHasParent) {
                ecs_add_id(world, tgt, 
                    ecs_value_pair(EcsParentDepth, cr->pair->depth));
            }

            if (!(r->row & EcsEntityIsTraversable)) {
                continue;
            }

            ecs_component_record_t *tgt_cr = flecs_components_get(
                world, ecs_childof(tgt));
            if (!tgt_cr) {
                continue;
            }

            flecs_component_update_childof_depth(world, tgt_cr, tgt, r);
        }
        return;
    }

    /* Fallback: no ordered children vector, iterate table cache instead */
    ecs_table_cache_iter_t it; flecs_component_iter(cr, &it);
    const ecs_table_record_t *tr;
    while ((tr = flecs_component_next(&it))) {
        ecs_table_t *table = tr->hdr.table;
        if (!(table->flags & EcsTableHasTraversable)) {
            continue;
        }

        const ecs_entity_t *entities = ecs_table_entities(table);
        int32_t i, count = ecs_table_count(table);
        for (i = 0; i < count; i ++) {
            ecs_entity_t tgt = entities[i];
            ecs_component_record_t *tgt_cr = flecs_components_get(
                world, ecs_childof(tgt));
            if (!tgt_cr) {
                continue;
            }

            ecs_record_t *r = flecs_entities_get(world, tgt);
            flecs_component_update_childof_depth(world, tgt_cr, tgt, r);
        }
    }
}

/* Update the ChildOf hierarchy depth for a component record and propagate changes. */
void flecs_component_update_childof_w_depth(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    int32_t depth)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_pair_record_t *pair = cr->pair;
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(depth < (FLECS_DAG_DEPTH_MAX + 1), ECS_INVALID_OPERATION, 
        "possible cycle detected in ChildOf/Parent hierarchy");

    if (cr->flags & EcsIdMarkedForDelete) {
        return;
    }

    /* If depth changed, propagate downwards */
    if (depth != pair->depth) {
        pair->depth = depth;
        flecs_entities_update_childof_depth(world, cr);
    }
}

/* Compute and update the ChildOf hierarchy depth based on the target entity's depth. */
void flecs_component_update_childof_depth(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t tgt,
    const ecs_record_t *tgt_r)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_SECOND(cr->id) == (uint32_t)tgt, 
        ECS_INTERNAL_ERROR, NULL);

    if (cr->flags & EcsIdMarkedForDelete) {
        return;
    }

    int32_t new_depth;
    if (tgt) {
        ecs_table_t *tgt_table = tgt_r->table;
        if (tgt_table->flags & EcsTableHasChildOf) {
            ecs_pair_record_t *tgt_childof_pr = flecs_table_get_childof_pr(
                world, tgt_table);
            new_depth = tgt_childof_pr->depth + 1;
        } else if (tgt_table->flags & EcsTableHasParent) {
            int32_t column = tgt_table->component_map[ecs_id(EcsParent)];
            ecs_assert(column > 0, ECS_INTERNAL_ERROR, NULL);

            EcsParent *data = tgt_table->data.columns[column - 1].data;
            ecs_entity_t parent = data[ECS_RECORD_TO_ROW(tgt_r->row)].value;
            if (!parent) {
                new_depth = 0;
            } else {
                ecs_component_record_t *cr_parent = flecs_components_get(world,
                    ecs_childof(parent));
                ecs_assert(cr_parent != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(cr_parent->pair != NULL, ECS_INTERNAL_ERROR, NULL);

                new_depth = cr_parent->pair->depth + 1;
            }
        } else {
            new_depth = 1;
        }
    } else {
        new_depth = 0;
    }

    flecs_component_update_childof_w_depth(world, cr, new_depth);
}
