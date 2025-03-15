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

<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
static inline
ecs_id_record_elem_t* flecs_id_record_elem(
    ecs_id_record_t *head,
========
static
ecs_id_record_elem_t* flecs_component_elem(
    ecs_component_record_t *head,
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
    ecs_id_record_elem_t *list,
    ecs_component_record_t *cdr)
{
    return ECS_OFFSET(cdr->pair, (uintptr_t)list - (uintptr_t)head->pair);
}

static
void flecs_component_elem_insert(
    ecs_component_record_t *head,
    ecs_component_record_t *cdr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_elem_t *head_elem = flecs_component_elem(cdr, elem, head);
    ecs_component_record_t *cur = head_elem->next;
    elem->next = cur;
    elem->prev = head;
    if (cur) {
        ecs_id_record_elem_t *cur_elem = flecs_component_elem(cdr, elem, cur);
        cur_elem->prev = cdr;
    }
    head_elem->next = cdr;
}

static
void flecs_component_elem_remove(
    ecs_component_record_t *cdr,
    ecs_id_record_elem_t *elem)
{
    ecs_component_record_t *prev = elem->prev;
    ecs_component_record_t *next = elem->next;
    ecs_assert(prev != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_record_elem_t *prev_elem = flecs_component_elem(cdr, elem, prev);
    prev_elem->next = next;
    if (next) {
        ecs_id_record_elem_t *next_elem = flecs_component_elem(cdr, elem, next);
        next_elem->prev = prev;
    }
}

static
void flecs_insert_id_elem(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_id_t wildcard,
    ecs_component_record_t *widr)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);
    if (!widr) {
        widr = flecs_components_ensure(world, wildcard);
    }
    ecs_assert(widr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_pair_id_record_t *pair = cdr->pair;
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_insert(widr, cdr, &pair->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_insert(widr, cdr, &pair->second);

        if (cdr->flags & EcsIdTraversable) {
            flecs_component_elem_insert(widr, cdr, &pair->trav);
        }
    }
}

static
void flecs_remove_id_elem(
    ecs_component_record_t *cdr,
    ecs_id_t wildcard)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);

    ecs_pair_id_record_t *pair = cdr->pair;
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_remove(cdr, &pair->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_component_elem_remove(cdr, &pair->second);

        if (cdr->flags & EcsIdTraversable) {
            flecs_component_elem_remove(cdr, &pair->trav);
        }
    }
}

<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
static inline
ecs_id_t flecs_id_record_hash(
========
static
ecs_id_t flecs_component_hash(
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
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
    ecs_component_record_t *cdr)
{
    if (!cdr->sparse) {
        if (cdr->flags & EcsIdIsSparse) {
            ecs_assert(!(cdr->flags & EcsIdIsUnion), ECS_CONSTRAINT_VIOLATED,
                "cannot mix union and sparse traits");
            ecs_assert(cdr->type_info != NULL, ECS_INVALID_OPERATION, 
                "only components can be marked as sparse");
            cdr->sparse = flecs_walloc_t(world, ecs_sparse_t);
            flecs_sparse_init(cdr->sparse, NULL, NULL, cdr->type_info->size);
        } else
        if (cdr->flags & EcsIdIsUnion) {
            cdr->sparse = flecs_walloc_t(world, ecs_switch_t);
            flecs_switch_init(cdr->sparse, &world->allocator);
        }
    }
}

static
void flecs_component_fini_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    if (cdr->sparse) {
        if (cdr->flags & EcsIdIsSparse) {
            ecs_assert(flecs_sparse_count(cdr->sparse) == 0, 
                ECS_INTERNAL_ERROR, NULL);
            flecs_sparse_fini(cdr->sparse);
            flecs_wfree_t(world, ecs_sparse_t, cdr->sparse);
        } else
        if (cdr->flags & EcsIdIsUnion) {
            flecs_switch_fini(cdr->sparse);
            flecs_wfree_t(world, ecs_switch_t, cdr->sparse);
        } else {
            ecs_abort(ECS_INTERNAL_ERROR, "unknown sparse storage");
        }
    }
}

<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
static inline
ecs_flags32_t flecs_id_record_event_flags(
========
static
ecs_flags32_t flecs_component_event_flags(
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
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
    ecs_component_record_t *cdr, *idr_t = NULL;
    ecs_id_t hash = flecs_component_hash(id);
    cdr = flecs_bcalloc_w_dbg_info(
        &world->allocators.id_record, "ecs_component_record_t");

    if (hash >= FLECS_HI_ID_RECORD_ID) {
        ecs_map_insert_ptr(&world->id_index_hi, hash, cdr);
    } else {
        world->id_index_lo[hash] = cdr;
    }

    ecs_table_cache_init(world, &cdr->cache);

    cdr->id = id;
    cdr->refcount = 1;

    bool is_wildcard = ecs_id_is_wildcard(id);
    bool is_pair = ECS_IS_PAIR(id);

    ecs_entity_t rel = 0, tgt = 0, role = id & ECS_ID_FLAGS_MASK;
    if (is_pair) {
        cdr->pair = flecs_bcalloc_w_dbg_info(
            &world->allocators.pair_id_record, "ecs_pair_id_record_t");
        cdr->pair->reachable.current = -1;

        rel = ECS_PAIR_FIRST(id);
        rel = flecs_entities_get_alive(world, rel);
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);

        /* Relationship object can be 0, as tables without a ChildOf 
         * relationship are added to the (ChildOf, 0) component record */
        tgt = ECS_PAIR_SECOND(id);

#ifdef FLECS_DEBUG
        /* Check constraints */
        if (tgt) {
            tgt = flecs_entities_get_alive(world, tgt);
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);

            /* Can't use relationship as target */
            if (ecs_has_id(world, tgt, EcsRelationship)) {
                if (!ecs_id_is_wildcard(rel) && 
                    !ecs_has_id(world, rel, EcsTrait)) 
                {
                    char *idstr = ecs_id_str(world, id);
                    char *tgtstr = ecs_id_str(world, tgt);
                    ecs_err("constraint violated: relationship '%s' cannot be used"
                        " as target in pair '%s'", tgtstr, idstr);
                    ecs_os_free(tgtstr);
                    ecs_os_free(idstr);
                    #ifndef FLECS_SOFT_ASSERT
                    ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
                    #endif
                }
            }
        }

        if (ecs_has_id(world, rel, EcsTarget)) {
            char *idstr = ecs_id_str(world, id);
            char *relstr = ecs_id_str(world, rel);
            ecs_err("constraint violated: "
                "%s: target '%s' cannot be used as relationship",
                    idstr, relstr);
            ecs_os_free(relstr);
            ecs_os_free(idstr);
            #ifndef FLECS_SOFT_ASSERT
            ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
            #endif
        }

        if (tgt && !ecs_id_is_wildcard(tgt) && tgt != EcsUnion) {
            /* Check if target of relationship satisfies OneOf property */
            const ecs_entity_t oneof = flecs_get_oneof(world, rel);
            if (oneof) {
                if (!ecs_has_pair(world, tgt, EcsChildOf, oneof)) {
                    char *idstr = ecs_id_str(world, id);
                    char *tgtstr = ecs_get_path(world, tgt);
                    char *oneofstr = ecs_get_path(world, oneof);
                    ecs_err("OneOf constraint violated: "
                        "%s: '%s' is not a child of '%s'",
                        idstr, tgtstr, oneofstr);
                    ecs_os_free(oneofstr);
                    ecs_os_free(tgtstr);
                    ecs_os_free(idstr);
                    #ifndef FLECS_SOFT_ASSERT
                    ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
                    #endif
                }
            }

            /* Check if we're not trying to inherit from a final target */
            if (rel == EcsIsA) {
                if (ecs_has_id(world, tgt, EcsFinal)) {
                    char *idstr = ecs_id_str(world, id);
                    char *tgtstr = ecs_get_path(world, tgt);
                    ecs_err("Final constraint violated: "
                            "%s: cannot inherit from final entity '%s'",
                        idstr, tgtstr);
                    ecs_os_free(tgtstr);
                    ecs_os_free(idstr);
                    #ifndef FLECS_SOFT_ASSERT
                    ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
                    #endif
                }
            }
        }
#endif

        if (!is_wildcard && (rel != EcsFlag)) {
            /* Inherit flags from (relationship, *) record */
            ecs_component_record_t *idr_r = flecs_components_ensure(
                world, ecs_pair(rel, EcsWildcard));
            cdr->pair->parent = idr_r;
            cdr->flags = idr_r->flags;

            /* If pair is not a wildcard, append it to wildcard lists. These 
             * allow for quickly enumerating all relationships for an object, 
             * or all objects for a relationship. */
            flecs_insert_id_elem(world, cdr, ecs_pair(rel, EcsWildcard), idr_r);

            idr_t = flecs_components_ensure(world, ecs_pair(EcsWildcard, tgt));
            flecs_insert_id_elem(world, cdr, ecs_pair(EcsWildcard, tgt), idr_t);
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
            char *idstr = ecs_id_str(world, id);
            char *relstr = ecs_id_str(world, rel);
            ecs_err("constraint violated: "
                "%s: relationship%s '%s' cannot be used as component",
                    idstr, is_tgt ? " target" : "", relstr);
            ecs_os_free(relstr);
            ecs_os_free(idstr);
            #ifndef FLECS_SOFT_ASSERT
            ecs_abort(ECS_CONSTRAINT_VIOLATED, NULL);
            #endif
        }
#endif
    }

    /* Initialize type info if id is not a tag */
    if (!is_wildcard && (!role || is_pair)) {
        if (!(cdr->flags & EcsIdTag)) {
            const ecs_type_info_t *ti = flecs_type_info_get(world, rel);
            if (!ti && tgt) {
                ti = flecs_type_info_get(world, tgt);
            }
            cdr->type_info = ti;
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
        if (cdr->flags & EcsIdTraversable) {
            /* Flag used to determine if object should be traversed when
             * propagating events or with super/subset queries */
            flecs_record_add_flag(tgt_r, EcsEntityIsTraversable);

            /* Add reference to (*, tgt) component record to entity record */
            tgt_r->cdr = idr_t;
        }

        /* If second element of pair determines the type, check if the pair 
         * should be stored as a sparse component. */
        if (cdr->type_info && cdr->type_info->component == tgt) {
            if (ecs_has_id(world, tgt, EcsSparse)) {
                cdr->flags |= EcsIdIsSparse;
            }
        }
    }

    cdr->flags |= flecs_component_event_flags(world, id);

    if (cdr->flags & EcsIdIsSparse) {
        flecs_component_init_sparse(world, cdr);
    } else if (cdr->flags & EcsIdIsUnion) {
        if (ECS_IS_PAIR(id) && ECS_PAIR_SECOND(id) == EcsUnion) {
            flecs_component_init_sparse(world, cdr);
        }
    }

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]id#[normal] %s #[green]created", id_str);
        ecs_os_free(id_str);
    }

#ifdef FLECS_DEBUG_INFO
    cdr->str = ecs_id_str(world, cdr->id);
#endif

    /* Update counters */
    world->info.id_create_total ++;
    world->info.component_id_count += cdr->type_info != NULL;
    world->info.tag_id_count += cdr->type_info == NULL;
    world->info.pair_id_count += is_pair;

    return cdr;
}

static
void flecs_component_assert_empty(
    ecs_component_record_t *cdr)
{
    (void)cdr;
    ecs_assert(flecs_table_cache_count(&cdr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_component_free(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    flecs_poly_assert(world, ecs_world_t);
<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_id_t id = idr->id;
========
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t id = cdr->id;
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c

    flecs_component_assert_empty(cdr);

    /* Id is still in use by a query */
    ecs_assert((world->flags & EcsWorldQuit) || (cdr->keep_alive == 0), 
        ECS_ID_IN_USE, "cannot delete id that is queried for");

    if (ECS_IS_PAIR(id)) {
        const ecs_entity_t rel = ECS_PAIR_FIRST(id);
        const ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        if (!ecs_id_is_wildcard(id)) {
            if (ECS_PAIR_FIRST(id) != EcsFlag) {
                /* If id is not a wildcard, remove it from the wildcard lists */
                flecs_remove_id_elem(cdr, ecs_pair(rel, EcsWildcard));
                flecs_remove_id_elem(cdr, ecs_pair(EcsWildcard, tgt));
            }
        } else {
            ecs_log_push_2();

            /* If id is a wildcard, it means that all id records that match the
             * wildcard are also empty, so release them */
            if (ECS_PAIR_FIRST(id) == EcsWildcard) {
                /* Iterate (*, Target) list */
                ecs_component_record_t *cur, *next = cdr->pair->second.next;
                while ((cur = next)) {
                    flecs_component_assert_empty(cur);
                    next = cur->pair->second.next;
                    flecs_component_release(world, cur);
                }
            } else {
                /* Iterate (Relationship, *) list */
                ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_component_record_t *cur, *next = cdr->pair->first.next;
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
    flecs_component_fini_sparse(world, cdr);

    /* Update counters */
    world->info.id_delete_total ++;
    world->info.pair_id_count -= ECS_IS_PAIR(id);
    world->info.component_id_count -= cdr->type_info != NULL;
    world->info.tag_id_count -= cdr->type_info == NULL;

    /* Unregister the component record from the world & free resources */
    ecs_table_cache_fini(&cdr->cache);

    if (cdr->pair) {
        flecs_name_index_free(cdr->pair->name_index);
        ecs_vec_fini_t(&world->allocator, &cdr->pair->reachable.ids, 
            ecs_reachable_elem_t);
        flecs_bfree_w_dbg_info(&world->allocators.pair_id_record, 
                cdr->pair, "ecs_pair_id_record_t");
    }

<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
    const ecs_id_t hash = flecs_id_record_hash(id);
========
    ecs_id_t hash = flecs_component_hash(id);
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        ecs_map_remove(&world->id_index_hi, hash);
    } else {
        world->id_index_lo[hash] = NULL;
    }

#ifdef FLECS_DEBUG_INFO
    ecs_os_free(cdr->str);
#endif

    flecs_bfree_w_dbg_info(&world->allocators.id_record, 
        cdr, "ecs_component_record_t");

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
    ecs_component_record_t *cdr = flecs_components_get(world, id);
    if (!cdr) {
        cdr = flecs_component_new(world, id);
    }
    return cdr;
}

ecs_component_record_t* flecs_components_get(
    const ecs_world_t *world,
    ecs_id_t id)
{
    flecs_poly_assert(world, ecs_world_t);
    if (id == ecs_pair(EcsIsA, EcsWildcard)) {
        return world->idr_isa_wildcard;
    } else if (id == ecs_pair(EcsChildOf, EcsWildcard)) {
        return world->idr_childof_wildcard;
    } else if (id == ecs_pair_t(EcsIdentifier, EcsName)) {
        return world->idr_identifier_name;
    }

<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
    const ecs_id_t hash = flecs_id_record_hash(id);
    ecs_id_record_t *idr = NULL;
========
    ecs_id_t hash = flecs_component_hash(id);
    ecs_component_record_t *cdr = NULL;
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        cdr = ecs_map_get_deref(&world->id_index_hi, ecs_component_record_t, hash);
    } else {
        cdr = world->id_index_lo[hash];
    }

    return cdr;
}

void flecs_component_claim(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    (void)world;
    cdr->refcount ++;
}

int32_t flecs_component_release(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
<<<<<<<< HEAD:Source/FlecsLibrary/Private/storage/id_index.c
    const int32_t rc = -- idr->refcount;
========
    int32_t rc = -- cdr->refcount;
>>>>>>>> upstream/master:Source/FlecsLibrary/Private/storage/component_index.c
    ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!rc) {
        flecs_component_free(world, cdr);
    }

    return rc;
}

void flecs_component_release_tables(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_all_iter(&cdr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            /* Release current table */
            flecs_table_fini(world, tr->hdr.table);
        }
    }
}

bool flecs_component_set_type_info(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    const ecs_type_info_t *ti)
{
    bool is_wildcard = ecs_id_is_wildcard(cdr->id);
    if (!is_wildcard) {
        if (ti) {
            if (!cdr->type_info) {
                world->info.tag_id_count --;
                world->info.component_id_count ++;
            }
        } else {
            if (cdr->type_info) {
                world->info.tag_id_count ++;
                world->info.component_id_count --;
            }
        }
    }

    bool changed = cdr->type_info != ti;
    cdr->type_info = ti;

    return changed;
}

ecs_hashmap_t* flecs_component_name_index_ensure(
    ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    ecs_assert(cdr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_hashmap_t *map = cdr->pair->name_index;
    if (!map) {
        map = cdr->pair->name_index = 
            flecs_name_index_new(world, &world->allocator);
    }

    return map;
}

ecs_hashmap_t* flecs_component_name_index_get(
    const ecs_world_t *world,
    ecs_component_record_t *cdr)
{
    flecs_poly_assert(world, ecs_world_t);
    (void)world;
    return cdr->pair->name_index;
}

const ecs_table_record_t* flecs_component_get_table(
    const ecs_component_record_t *cdr,
    const ecs_table_t *table)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_table_cache_get(&cdr->cache, table);
}

void flecs_components_init(
    ecs_world_t *world)
{
    /* Cache often used id records on world */
    world->idr_wildcard = flecs_components_ensure(world, EcsWildcard);
    world->idr_wildcard_wildcard = flecs_components_ensure(world, 
        ecs_pair(EcsWildcard, EcsWildcard));
    world->idr_any = flecs_components_ensure(world, EcsAny);
    world->idr_isa_wildcard = flecs_components_ensure(world, 
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
        ecs_component_record_t *cdr = world->id_index_lo[i];
        if (cdr) {
            flecs_component_release(world, cdr);
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
    const ecs_component_record_t *cdr = flecs_components_get(world, id);
    if (cdr) {
        ecs_flags32_t extra_flags = 0;
        if (cdr->flags & EcsIdOnInstantiateInherit) {
            extra_flags |= EcsIdHasOnAdd|EcsIdHasOnRemove;
        }
        return cdr->flags|extra_flags;
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
        const ecs_entity_t first = ECS_PAIR_FIRST(id);
        const ecs_entity_t second = ECS_PAIR_SECOND(id);

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

ecs_component_record_t* flecs_component_first_next(
    ecs_component_record_t *cdr)
{
    ecs_assert(cdr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cdr->pair->first.next;
}

ecs_component_record_t* flecs_component_second_next(
    ecs_component_record_t *cdr)
{
    ecs_assert(cdr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cdr->pair->second.next;
}

ecs_component_record_t* flecs_component_trav_next(
    ecs_component_record_t *cdr)
{
    ecs_assert(cdr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    return cdr->pair->trav.next;
}

bool flecs_component_iter(
    const ecs_component_record_t *cdr,
    ecs_table_cache_iter_t *iter_out)
{
    return flecs_table_cache_all_iter(&cdr->cache, iter_out);
}

const ecs_table_record_t* flecs_component_next(
    ecs_table_cache_iter_t *iter)
{
    return flecs_table_cache_next(iter, ecs_table_record_t);
}
