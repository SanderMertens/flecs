/**
 * @file id_index.c
 * @brief Index for looking up tables by (component) id.
 * 
 * An id record stores the administration for an in use (component) id, that is
 * an id that has been used in tables.
 * 
 * An id record contains a table cache, which stores the list of tables that
 * have the id. Each entry in the cache (a table record) stores the first 
 * occurrence of the id in the table and the number of occurrences of the id in
 * the table (in the case of wildcard ids).
 * 
 * Id records are used in lots of scenarios, like uncached queries, or for 
 * getting a component array/component for an entity.
 */

#include "../private_api.h"

static
ecs_id_record_elem_t* flecs_id_record_elem(
    ecs_id_record_t *head,
    ecs_id_record_elem_t *list,
    ecs_id_record_t *idr)
{
    return ECS_OFFSET(idr, (uintptr_t)list - (uintptr_t)head);
}

static
void flecs_id_record_elem_insert(
    ecs_id_record_t *head,
    ecs_id_record_t *idr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_elem_t *head_elem = flecs_id_record_elem(idr, elem, head);
    ecs_id_record_t *cur = head_elem->next;
    elem->next = cur;
    elem->prev = head;
    if (cur) {
        ecs_id_record_elem_t *cur_elem = flecs_id_record_elem(idr, elem, cur);
        cur_elem->prev = idr;
    }
    head_elem->next = idr;
}

static
void flecs_id_record_elem_remove(
    ecs_id_record_t *idr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_t *prev = elem->prev;
    ecs_id_record_t *next = elem->next;
    ecs_assert(prev != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_record_elem_t *prev_elem = flecs_id_record_elem(idr, elem, prev);
    prev_elem->next = next;
    if (next) {
        ecs_id_record_elem_t *next_elem = flecs_id_record_elem(idr, elem, next);
        next_elem->prev = prev;
    }
}

static
void flecs_insert_id_elem(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_id_t wildcard,
    ecs_id_record_t *widr)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);
    if (!widr) {
        widr = flecs_id_record_ensure(world, wildcard);
    }
    ecs_assert(widr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_id_record_elem_insert(widr, idr, &idr->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_id_record_elem_insert(widr, idr, &idr->second);

        if (idr->flags & EcsIdTraversable) {
            flecs_id_record_elem_insert(widr, idr, &idr->trav);
        }
    }
}

static
void flecs_remove_id_elem(
    ecs_id_record_t *idr,
    ecs_id_t wildcard)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_id_record_elem_remove(idr, &idr->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_id_record_elem_remove(idr, &idr->second);

        if (idr->flags & EcsIdTraversable) {
            flecs_id_record_elem_remove(idr, &idr->trav);
        }
    }
}

static
ecs_id_t flecs_id_record_hash(
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

static
ecs_id_record_t* flecs_id_record_new(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr, *idr_t = NULL;
    ecs_id_t hash = flecs_id_record_hash(id);
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        idr = flecs_bcalloc(&world->allocators.id_record);
        ecs_map_insert_ptr(&world->id_index_hi, hash, idr);
    } else {
        idr = &world->id_index_lo[hash];
        ecs_os_zeromem(idr);
    }

    ecs_table_cache_init(world, &idr->cache);

    idr->id = id;
    idr->refcount = 1;
    idr->reachable.current = -1;

    bool is_wildcard = ecs_id_is_wildcard(id);
    bool is_pair = ECS_IS_PAIR(id);

    ecs_entity_t rel = 0, tgt = 0, role = id & ECS_ID_FLAGS_MASK;
    if (is_pair) {
        // rel = ecs_pair_first(world, id);
        rel = ECS_PAIR_FIRST(id);
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);

        /* Relationship object can be 0, as tables without a ChildOf 
         * relationship are added to the (ChildOf, 0) id record */
        tgt = ECS_PAIR_SECOND(id);

#ifdef FLECS_DEBUG
        /* Check constraints */
        if (tgt) {
            tgt = ecs_get_alive(world, tgt);
            ecs_assert(tgt != 0, ECS_INTERNAL_ERROR, NULL);
        }
        if (tgt && !ecs_id_is_wildcard(tgt)) {
            /* Check if target of relationship satisfies OneOf property */
            ecs_entity_t oneof = flecs_get_oneof(world, rel);
            ecs_check( !oneof || ecs_has_pair(world, tgt, EcsChildOf, oneof),
                ECS_CONSTRAINT_VIOLATED, NULL);
            (void)oneof;

            /* Check if we're not trying to inherit from a final target */
            if (rel == EcsIsA) {
                bool is_final = ecs_has_id(world, tgt, EcsFinal);
                ecs_check(!is_final, ECS_CONSTRAINT_VIOLATED, 
                    "cannot inherit from final entity");
                (void)is_final;
            }
        }
#endif

        if (!is_wildcard && (rel != EcsFlag)) {
            /* Inherit flags from (relationship, *) record */
            ecs_id_record_t *idr_r = flecs_id_record_ensure(
                world, ecs_pair(rel, EcsWildcard));
            idr->parent = idr_r;
            idr->flags = idr_r->flags;

            /* If pair is not a wildcard, append it to wildcard lists. These 
             * allow for quickly enumerating all relationships for an object, 
             * or all objecs for a relationship. */
            flecs_insert_id_elem(world, idr, ecs_pair(rel, EcsWildcard), idr_r);

            idr_t = flecs_id_record_ensure(world, ecs_pair(EcsWildcard, tgt));
            flecs_insert_id_elem(world, idr, ecs_pair(EcsWildcard, tgt), idr_t);

            if (rel == EcsUnion) {
                idr->flags |= EcsIdUnion;
            }
        }
    } else {
        rel = id & ECS_COMPONENT_MASK;
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);
    }

    /* Initialize type info if id is not a tag */
    if (!is_wildcard && (!role || is_pair)) {
        if (!(idr->flags & EcsIdTag)) {
            const ecs_type_info_t *ti = flecs_type_info_get(world, rel);
            if (!ti && tgt) {
                ti = flecs_type_info_get(world, tgt);
            }
            idr->type_info = ti;
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
        if (idr->flags & EcsIdTraversable) {
            /* Flag used to determine if object should be traversed when
             * propagating events or with super/subset queries */
            flecs_record_add_flag(tgt_r, EcsEntityIsTraversable);

            /* Add reference to (*, tgt) id record to entity record */
            tgt_r->idr = idr_t;
        }
    }

    ecs_observable_t *o = &world->observable;
    idr->flags |= flecs_observers_exist(o, id, EcsOnAdd) * EcsIdHasOnAdd;
    idr->flags |= flecs_observers_exist(o, id, EcsOnRemove) * EcsIdHasOnRemove;
    idr->flags |= flecs_observers_exist(o, id, EcsOnSet) * EcsIdHasOnSet;
    idr->flags |= flecs_observers_exist(o, id, EcsUnSet) * EcsIdHasUnSet;
    idr->flags |= flecs_observers_exist(o, id, EcsOnTableFill) * EcsIdHasOnTableFill;
    idr->flags |= flecs_observers_exist(o, id, EcsOnTableEmpty) * EcsIdHasOnTableEmpty;
    idr->flags |= flecs_observers_exist(o, id, EcsOnTableCreate) * EcsIdHasOnTableCreate;
    idr->flags |= flecs_observers_exist(o, id, EcsOnTableDelete) * EcsIdHasOnTableDelete;

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]id#[normal] %s #[green]created", id_str);
        ecs_os_free(id_str);
    }

    /* Update counters */
    world->info.id_create_total ++;

    if (!is_wildcard) {
        world->info.id_count ++;

        if (idr->type_info) {
            world->info.component_id_count ++;
        } else {
            world->info.tag_id_count ++;
        }

        if (is_pair) {
            world->info.pair_id_count ++;
        }
    } else {
        world->info.wildcard_id_count ++;
    }

    return idr;
#ifdef FLECS_DEBUG
error:
    return NULL;
#endif
}

static
void flecs_id_record_assert_empty(
    ecs_id_record_t *idr)
{
    (void)idr;
    ecs_assert(flecs_table_cache_count(&idr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_table_cache_empty_count(&idr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_id_record_free(
    ecs_world_t *world,
    ecs_id_record_t *idr)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_id_t id = idr->id;

    flecs_id_record_assert_empty(idr);

    /* Id is still in use by a filter, query, rule or observer */
    ecs_assert((world->flags & EcsWorldQuit) || (idr->keep_alive == 0), 
        ECS_ID_IN_USE, "cannot delete id that is queried for");

    if (ECS_IS_PAIR(id)) {
        ecs_entity_t rel = ECS_PAIR_FIRST(id);
        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        if (!ecs_id_is_wildcard(id)) {
            if (ECS_PAIR_FIRST(id) != EcsFlag) {
                /* If id is not a wildcard, remove it from the wildcard lists */
                flecs_remove_id_elem(idr, ecs_pair(rel, EcsWildcard));
                flecs_remove_id_elem(idr, ecs_pair(EcsWildcard, tgt));
            }
        } else {
            ecs_log_push_2();

            /* If id is a wildcard, it means that all id records that match the
             * wildcard are also empty, so release them */
            if (ECS_PAIR_FIRST(id) == EcsWildcard) {
                /* Iterate (*, Target) list */
                ecs_id_record_t *cur, *next = idr->second.next;
                while ((cur = next)) {
                    flecs_id_record_assert_empty(cur);
                    next = cur->second.next;
                    flecs_id_record_release(world, cur);
                }
            } else {
                /* Iterate (Relationship, *) list */
                ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_id_record_t *cur, *next = idr->first.next;
                while ((cur = next)) {
                    flecs_id_record_assert_empty(cur);
                    next = cur->first.next;
                    flecs_id_record_release(world, cur);
                }
            }

            ecs_log_pop_2();
        }
    }

    /* Update counters */
    world->info.id_delete_total ++;

    if (!ecs_id_is_wildcard(id)) {
        world->info.id_count --;

        if (ECS_IS_PAIR(id)) {
            world->info.pair_id_count --;
        }

        if (idr->type_info) {
            world->info.component_id_count --;
        } else {
            world->info.tag_id_count --;
        }
    } else {
        world->info.wildcard_id_count --;
    }

    /* Unregister the id record from the world & free resources */
    ecs_table_cache_fini(&idr->cache);
    flecs_name_index_free(idr->name_index);
    ecs_vec_fini_t(&world->allocator, &idr->reachable.ids, ecs_reachable_elem_t);

    ecs_id_t hash = flecs_id_record_hash(id);
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        ecs_map_remove(&world->id_index_hi, hash);
        flecs_bfree(&world->allocators.id_record, idr);
    } else {
        idr->id = 0; /* Tombstone */
    }

    if (ecs_should_log_1()) {
        char *id_str = ecs_id_str(world, id);
        ecs_dbg_1("#[green]id#[normal] %s #[red]deleted", id_str);
        ecs_os_free(id_str);
    }
}

ecs_id_record_t* flecs_id_record_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        idr = flecs_id_record_new(world, id);
    }
    return idr;
}

ecs_id_record_t* flecs_id_record_get(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);
    if (id == ecs_pair(EcsIsA, EcsWildcard)) {
        return world->idr_isa_wildcard;
    } else if (id == ecs_pair(EcsChildOf, EcsWildcard)) {
        return world->idr_childof_wildcard;
    } else if (id == ecs_pair_t(EcsIdentifier, EcsName)) {
        return world->idr_identifier_name;
    }

    ecs_id_t hash = flecs_id_record_hash(id);
    ecs_id_record_t *idr = NULL;
    if (hash >= FLECS_HI_ID_RECORD_ID) {
        idr = ecs_map_get_deref(&world->id_index_hi, ecs_id_record_t, hash);
    } else {
        idr = &world->id_index_lo[hash];
        if (!idr->id) {
            idr = NULL;
        }
    }

    return idr;
}

ecs_id_record_t* flecs_query_id_record_get(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        ecs_entity_t first = ECS_PAIR_FIRST(id);
        if (ECS_IS_PAIR(id) && (first != EcsWildcard)) {
            idr = flecs_id_record_get(world, ecs_pair(EcsUnion, first));
        }
        return idr;
    }
    if (ECS_IS_PAIR(id) && 
        ECS_PAIR_SECOND(id) == EcsWildcard && 
        (idr->flags & EcsIdUnion)) 
    {
        idr = flecs_id_record_get(world, 
            ecs_pair(EcsUnion, ECS_PAIR_FIRST(id)));
    }

    return idr;
}

void flecs_id_record_claim(
    ecs_world_t *world,
    ecs_id_record_t *idr)
{
    (void)world;
    idr->refcount ++;
}

int32_t flecs_id_record_release(
    ecs_world_t *world,
    ecs_id_record_t *idr)
{
    int32_t rc = -- idr->refcount;
    ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!rc) {
        flecs_id_record_free(world, idr);
    }

    return rc;
}

void flecs_id_record_release_tables(
    ecs_world_t *world,
    ecs_id_record_t *idr)
{
    /* Cache should not contain tables that aren't empty */
    ecs_assert(flecs_table_cache_count(&idr->cache) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_empty_iter(&idr->cache, &it)) {
        ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            /* Release current table */
            flecs_table_free(world, tr->hdr.table);
        }
    }
}

bool flecs_id_record_set_type_info(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    const ecs_type_info_t *ti)
{
    bool is_wildcard = ecs_id_is_wildcard(idr->id);
    if (!is_wildcard) {
        if (ti) {
            if (!idr->type_info) {
                world->info.tag_id_count --;
                world->info.component_id_count ++;
            }
        } else {
            if (idr->type_info) {
                world->info.tag_id_count ++;
                world->info.component_id_count --;
            }
        }
    }

    bool changed = idr->type_info != ti;
    idr->type_info = ti;

    return changed;
}

ecs_hashmap_t* flecs_id_record_name_index_ensure(
    ecs_world_t *world,
    ecs_id_record_t *idr)
{
    ecs_hashmap_t *map = idr->name_index;
    if (!map) {
        map = idr->name_index = flecs_name_index_new(world, &world->allocator);
    }

    return map;
}

ecs_hashmap_t* flecs_id_name_index_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_id_record_name_index_ensure(world, idr);
}

ecs_hashmap_t* flecs_id_name_index_get(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return NULL;
    }

    return idr->name_index;
}

ecs_table_record_t* flecs_table_record_get(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t* idr = flecs_id_record_get(world, id);
    if (!idr) {
        return NULL;
    }

    return (ecs_table_record_t*)ecs_table_cache_get(&idr->cache, table);
}

ecs_table_record_t* flecs_id_record_get_table(
    const ecs_id_record_t *idr,
    const ecs_table_t *table)
{
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    return (ecs_table_record_t*)ecs_table_cache_get(&idr->cache, table);
}

void flecs_init_id_records(
    ecs_world_t *world)
{
    /* Cache often used id records on world */
    world->idr_wildcard = flecs_id_record_ensure(world, EcsWildcard);
    world->idr_wildcard_wildcard = flecs_id_record_ensure(world, 
        ecs_pair(EcsWildcard, EcsWildcard));
    world->idr_any = flecs_id_record_ensure(world, EcsAny);
    world->idr_isa_wildcard = flecs_id_record_ensure(world, 
        ecs_pair(EcsIsA, EcsWildcard));
}

void flecs_fini_id_records(
    ecs_world_t *world)
{
    /* Loop & delete first element until there are no elements left. Id records
     * can recursively delete each other, this ensures we always have a
     * valid iterator. */
    while (ecs_map_count(&world->id_index_hi) > 0) {
        ecs_map_iter_t it = ecs_map_iter(&world->id_index_hi);
        ecs_map_next(&it);
        flecs_id_record_release(world, ecs_map_ptr(&it));
    }

    int32_t i;
    for (i = 0; i < FLECS_HI_ID_RECORD_ID; i ++) {
        ecs_id_record_t *idr = &world->id_index_lo[i];
        if (idr->id) {
            flecs_id_record_release(world, idr);
        }
    }

    ecs_assert(ecs_map_count(&world->id_index_hi) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_map_fini(&world->id_index_hi);
    ecs_os_free(world->id_index_lo);
}
