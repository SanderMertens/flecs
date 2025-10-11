/**
 * @file storage/component_index.c
 * @brief Index for looking up tables by component id.
 * 
 * An component record stores the administration for an in use (component) id, that is
 * an id that has been used in tables.
 * 
 * An component record contains a table cache, which stores the list of tables that
 * have the id. Each entry in the cache (a table record) stores the first 
 * occurrence of the id in the table and the number of occurrences of the id in
 * the table (in the case of wildcard ids).
 * 
 * Id records are used in lots of scenarios, like uncached queries, or for 
 * getting a component array/component for an entity.
 */

#include "../private_api.h"

static
ecs_id_record_elem_t* flecs_component_elem(
    ecs_component_record_t *head,
    ecs_id_record_elem_t *list,
    ecs_component_record_t *cr)
{
    return ECS_OFFSET(cr->pair, (uintptr_t)list - (uintptr_t)head->pair);
}

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
        flecs_component_elem_remove(cr, &pair->second);

        if (cr->flags & EcsIdTraversable) {
            flecs_component_elem_remove(cr, &pair->trav);
        }
    }
}

static
ecs_id_t flecs_component_hash(
    ecs_id_t id)
{
    id = ecs_strip_generation(id);
    if (ECS_IS_PAIR(id)) {
        ecs_entity_t r = ECS_PAIR_FIRST(id);
        ecs_entity_t o = ECS_PAIR_SECOND(id);
        if (r == EcsAny) {
            r = EcsWildcard;
        }
        if (o == EcsAny) {
            o = EcsWildcard;
        }
        id = ecs_pair(r, o);
    }
    return id;
}

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

void flecs_component_record_init_exclusive(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_component_init_sparse(world, cr);
}

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

static
ecs_flags32_t flecs_component_event_flags(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_observable_t *o = &world->observable;
    ecs_flags32_t result = 0;
    result |= flecs_observers_exist(o, id, EcsOnAdd) * EcsIdHasOnAdd;
    result |= flecs_observers_exist(o, id, EcsOnRemove) * EcsIdHasOnRemove;
    result |= flecs_observers_exist(o, id, EcsOnSet) * EcsIdHasOnSet;
    result |= flecs_observers_exist(o, id, EcsOnTableCreate) * EcsIdHasOnTableCreate;
    result |= flecs_observers_exist(o, id, EcsOnTableDelete) * EcsIdHasOnTableDelete;
    return result;
}

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

    ecs_entity_t rel = 0, tgt = 0, role = id & ECS_ID_FLAGS_MASK;
    if (is_pair) {
        cr->pair = flecs_bcalloc_w_dbg_info(
            &world->allocators.pair_record, "ecs_pair_record_t");
        cr->pair->reachable.current = -1;

        flecs_ordered_children_init(world, cr);

        rel = ECS_PAIR_FIRST(id);
        rel = flecs_entities_get_alive(world, rel);
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);

        /* Relationship object can be 0, as tables without a ChildOf 
         * relationship are added to the (ChildOf, 0) component record */
        tgt = ECS_PAIR_SECOND(id);
        if (tgt) {
            ecs_entity_t alive_tgt = flecs_entities_get_alive(world, tgt);
            ecs_assert(alive_tgt != 0, ECS_INVALID_PARAMETER,
                "target '%s' of pair '%s' is not alive",
                    flecs_errstr(ecs_id_str(world, tgt)), 
                    flecs_errstr_1(ecs_id_str(world, cr->id)));
            tgt = alive_tgt;
        }

#ifdef FLECS_DEBUG
        /* Check constraints */
        if (tgt) {
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

            /* Can't use relationship as target */
            if (ecs_has_id(world, tgt, EcsRelationship)) {
                if (!ecs_id_is_wildcard(rel) && 
                    !ecs_has_id(world, rel, EcsTrait)) 
                {
                    ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as target"
                        " in pair '%s': '%s' has the Relationship trait",
                            flecs_errstr(ecs_get_path(world, tgt)),
                            flecs_errstr_1(ecs_id_str(world, id)),
                            flecs_errstr_2(ecs_get_path(world, tgt)));
                }
            }
        }

        if (ecs_has_id(world, rel, EcsTarget)) {
            ecs_throw(ECS_CONSTRAINT_VIOLATED, "cannot use '%s' as relationship "
                "in pair '%s': '%s' has the Target trait",
                    flecs_errstr(ecs_get_path(world, rel)),
                    flecs_errstr_1(ecs_id_str(world, id)),
                    flecs_errstr_2(ecs_get_path(world, rel)));
        }

        if (tgt && !ecs_id_is_wildcard(tgt)) {
            /* Check if target of relationship satisfies OneOf property */
            ecs_entity_t oneof = flecs_get_oneof(world, rel);
            if (oneof) {
                if (!ecs_has_pair(world, tgt, EcsChildOf, oneof)) {
                    if (oneof == rel) {
                        ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                            "cannot use '%s' as target in pair '%s': "
                            "relationship '%s' has the OneOf trait and '%s' "
                            "is not a child of '%s'",
                                flecs_errstr(ecs_get_path(world, tgt)),
                                flecs_errstr_1(ecs_id_str(world, id)),
                                flecs_errstr_2(ecs_get_path(world, rel)),
                                flecs_errstr_3(ecs_get_path(world, tgt)),
                                flecs_errstr_4(ecs_get_path(world, rel)));
                        } else {
                            ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                                "cannot use '%s' as target in pair '%s': "
                                "relationship '%s' has (OneOf, %s) and '%s' "
                                "is not a child of '%s'",
                                    flecs_errstr(ecs_get_path(world, tgt)),
                                    flecs_errstr_1(ecs_id_str(world, id)),
                                    flecs_errstr_2(ecs_get_path(world, rel)),
                                    flecs_errstr_3(ecs_get_path(world, oneof)),
                                    flecs_errstr_4(ecs_get_path(world, tgt)),
                                    flecs_errstr_5(ecs_get_path(world, oneof)));
                        }
                }
            }

            /* Check if we're not trying to inherit from a final target */
            if (rel == EcsIsA) {
                if (ecs_has_id(world, tgt, EcsFinal)) {
                    ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                        "cannot add '(IsA, %s)': '%s' has the Final trait",
                            flecs_errstr(ecs_get_path(world, tgt)),
                            flecs_errstr_1(ecs_get_path(world, tgt)));
                }

                ecs_component_record_t *cr_tgt = flecs_components_get(world, tgt);
                if (cr_tgt && cr_tgt->keep_alive) {
                    if (!ecs_has_id(world, tgt, EcsInheritable)) {
                        ecs_throw(ECS_INVALID_OPERATION, 
                            "cannot add '(IsA, %s)': '%s' is already queried for",
                                    flecs_errstr(ecs_get_path(world, tgt)),
                                    flecs_errstr_1(ecs_get_path(world, tgt)));
                    }
                }
            }
        }
#endif

        if (!is_wildcard && (rel != EcsFlag)) {
            /* Inherit flags from (relationship, *) record */
            ecs_component_record_t *cr_r = flecs_components_ensure(
                world, ecs_pair(rel, EcsWildcard));
            cr->pair->parent = cr_r;
            cr->flags = cr_r->flags;

            /* If pair is not a wildcard, append it to wildcard lists. These 
             * allow for quickly enumerating all relationships for an object, 
             * or all objects for a relationship. */
            flecs_insert_id_elem(world, cr, ecs_pair(rel, EcsWildcard), cr_r);

            cr_t = flecs_components_ensure(world, ecs_pair(EcsWildcard, tgt));
            flecs_insert_id_elem(world, cr, ecs_pair(EcsWildcard, tgt), cr_t);
        }
    } else {
        rel = id & ECS_COMPONENT_MASK;
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);

        /* Can't use relationship outside of a pair */
#ifdef FLECS_DEBUG
        rel = flecs_entities_get_alive(world, rel);
        bool is_tgt = false;
        if (ecs_has_id(world, rel, EcsRelationship) ||
            (is_tgt = ecs_has_id(world, rel, EcsTarget)))
        {
            if (is_tgt) {
                ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                    "cannot use '%s' by itself: it has the Target trait and must be used in pair with relationship",
                        flecs_errstr(ecs_get_path(world, rel)));
            } else {
                ecs_throw(ECS_CONSTRAINT_VIOLATED, 
                    "cannot use '%s' by itself: it has the Relationhip trait and must be used in pair with target",
                        flecs_errstr(ecs_get_path(world, rel)));
            }
        }
#endif
    }

    /* Initialize type info if id is not a tag */
    if (!is_wildcard && (!role || is_pair)) {
        if (!(cr->flags & EcsIdPairIsTag)) {
            const ecs_type_info_t *ti = flecs_type_info_get(world, rel);
            if (!ti && tgt) {
                ti = flecs_type_info_get(world, tgt);
            }
            cr->type_info = ti;
        }
    }

    /* Mark entities that are used as component/pair ids. When a tracked
     * entity is deleted, cleanup policies are applied so that the store
     * won't contain any tables with deleted ids. */

    /* Flag for OnDelete policies */
    flecs_add_flag(world, rel, EcsEntityIsId);
    if (tgt) {
        /* Flag for OnDeleteTarget policies */
        ecs_record_t *tgt_r = flecs_entities_get_any(world, tgt);
        ecs_assert(tgt_r != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_record_add_flag(tgt_r, EcsEntityIsTarget);
        if (cr->flags & EcsIdTraversable) {
            /* Flag used to determine if object should be traversed when
             * propagating events or with super/subset queries */
            flecs_record_add_flag(tgt_r, EcsEntityIsTraversable);

            /* Add reference to (*, tgt) component record to entity record */
            tgt_r->cr = cr_t;
        }

        /* If second element of pair determines the type, check if the pair 
         * should be stored as a sparse component. */
        if (cr->type_info && cr->type_info->component == tgt) {
            if (ecs_has_id(world, tgt, EcsSparse)) {
                cr->flags |= EcsIdSparse;
            }
            if (ecs_has_id(world, tgt, EcsDontFragment)) {
                cr->flags |= EcsIdDontFragment;
            }
        }

        /* Check if we should keep a list of ordered children for parent */
        if (rel == EcsChildOf) {
            if (ecs_has_id(world, tgt, EcsOrderedChildren)) {
                cr->flags |= EcsIdOrderedChildren;
            }
        }

        /* Mark (*, tgt) record with HasDontFragment so that queries can quickly
         * detect if there are any non-fragmenting records to consider for a
         * (*, tgt) query. */
        if (cr->flags & EcsIdDontFragment && ECS_PAIR_FIRST(id) != EcsFlag) {
            ecs_assert(cr_t != NULL, ECS_INTERNAL_ERROR, NULL);
            cr_t->flags |= EcsIdMatchDontFragment;
        }
    }

    cr->flags |= flecs_component_event_flags(world, id);

    if (cr->flags & EcsIdSparse) {
        flecs_component_init_sparse(world, cr);
    }

    if (cr->flags & EcsIdDontFragment) {
        flecs_component_record_init_dont_fragment(world, cr);
    }

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]id#[normal] %s #[green]created", id_str);
        ecs_os_free(id_str);
    }

#ifdef FLECS_DEBUG_INFO
    cr->str = ecs_id_str(world, cr->id);
#endif

    /* Update counters */
    world->info.id_create_total ++;
    world->info.component_id_count += cr->type_info != NULL;
    world->info.tag_id_count += cr->type_info == NULL;
    world->info.pair_id_count += is_pair;

    return cr;
#ifdef FLECS_DEBUG
error:
    return NULL;
#endif
}

static
void flecs_component_assert_empty(
    ecs_component_record_t *cr)
{
    (void)cr;
    ecs_assert(flecs_table_cache_count(&cr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_component_free(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t id = cr->id;

    flecs_component_assert_empty(cr);

    /* Id is still in use by a query */
    ecs_assert((world->flags & EcsWorldQuit) || (cr->keep_alive == 0), 
        ECS_INVALID_OPERATION, 
        "cannot delete component '%s' as it is still in use by queries",
            flecs_errstr(ecs_id_str(world, id)));

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        if (!ecs_id_is_wildcard(id)) {
            if (ECS_PAIR_FIRST(id) != EcsFlag) {
                /* If id is not a wildcard, remove it from the wildcard lists */
                flecs_remove_id_elem(cr, ecs_pair(rel, EcsWildcard));
                flecs_remove_id_elem(cr, ecs_pair(EcsWildcard, tgt));
            }
        } else {
            ecs_log_push_2();

            /* If id is a wildcard, it means that all id records that match the
             * wildcard are also empty, so release them */
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

    /* Cleanup sparse storage */
    flecs_component_fini_sparse(world, cr);

    if (cr->flags & EcsIdDontFragment) {
        flecs_component_record_fini_dont_fragment(world, cr);
    }

    /* Update counters */
    world->info.id_delete_total ++;
    world->info.pair_id_count -= ECS_IS_PAIR(id);
    world->info.component_id_count -= cr->type_info != NULL;
    world->info.tag_id_count -= cr->type_info == NULL;

    /* Unregister the component record from the world & free resources */
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
        ecs_dbg_1("#[green]id#[normal] %s #[red]deleted", id_str);
        ecs_os_free(id_str);
    }
}

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

void flecs_component_claim(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    (void)world;
    cr->refcount ++;
}

int32_t flecs_component_release(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    int32_t rc = -- cr->refcount;
    ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!rc) {
        flecs_component_free(world, cr);
    }

    return rc;
}

void flecs_component_release_tables(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_all_iter(&cr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            /* Release current table */
            flecs_table_fini(world, tr->hdr.table);
        }
    }
}

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

ecs_hashmap_t* flecs_component_name_index_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr)
{
    flecs_poly_assert(world, ecs_world_t);
    (void)world;
    return cr->pair->name_index;
}

const ecs_table_record_t* flecs_component_get_table(
    const ecs_component_record_t *cr,
    const ecs_table_t *table)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_table_cache_get(&cr->cache, table);
}

void flecs_components_init(
    ecs_world_t *world)
{
    /* Cache often used id records on world */
    world->cr_wildcard = flecs_components_ensure(world, EcsWildcard);
    world->cr_wildcard_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsWildcard, EcsWildcard));
    world->cr_any = flecs_components_ensure(world, EcsAny);
    world->cr_isa_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsIsA, EcsWildcard));
}

void flecs_components_fini(
    ecs_world_t *world)
{
    /* Loop & delete first element until there are no elements left. Id records
     * can recursively delete each other, this ensures we always have a
     * valid iterator. */
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

static
ecs_flags32_t flecs_id_flags(
    ecs_world_t *world,
    ecs_id_t id)
{
    const ecs_component_record_t *cr = flecs_components_get(world, id);
    if (cr) {
        ecs_flags32_t extra_flags = 0;
        if (cr->flags & EcsIdOnInstantiateInherit) {
            extra_flags |= EcsIdHasOnAdd|EcsIdHasOnRemove;
        }
        return cr->flags|extra_flags;
    }
    return flecs_component_event_flags(world, id);
}

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

void flecs_component_delete_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cr->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
    int32_t i, count = flecs_sparse_count(cr->sparse);
    const uint64_t *entities = flecs_sparse_ids(cr->sparse);
    for (i = 0; i < count; i ++) {
        ecs_delete(world, entities[i]);
    }
}

ecs_component_record_t* flecs_component_first_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->first.next;
}

ecs_component_record_t* flecs_component_second_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->second.next;
}

ecs_component_record_t* flecs_component_trav_next(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cr->pair->trav.next;
}

bool flecs_component_iter(
    const ecs_component_record_t *cr,
    ecs_table_cache_iter_t *iter_out)
{
    return flecs_table_cache_all_iter(&cr->cache, iter_out);
}

const ecs_table_record_t* flecs_component_next(
    ecs_table_cache_iter_t *iter)
{
    return flecs_table_cache_next(iter, ecs_table_record_t);
}

ecs_id_t flecs_component_get_id(
    const ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INVALID_PARAMETER, NULL);
    return cr->id;
}
