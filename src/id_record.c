#include "private_api.h"

static
ecs_id_record_elem_t* id_record_elem(
    ecs_id_record_t *head,
    ecs_id_record_elem_t *list,
    ecs_id_record_t *idr)
{
    return ECS_OFFSET(idr, (uintptr_t)list - (uintptr_t)head);
}

static
void id_record_elem_insert(
    ecs_id_record_t *head,
    ecs_id_record_t *idr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_elem_t *head_elem = id_record_elem(idr, elem, head);
    ecs_id_record_t *cur = head_elem->next;
    elem->next = cur;
    elem->prev = head;
    if (cur) {
        ecs_id_record_elem_t *cur_elem = id_record_elem(idr, elem, cur);
        cur_elem->prev = idr;
    }
    head_elem->next = idr;
}

static
void id_record_elem_remove(
    ecs_id_record_t *idr,
    ecs_id_record_elem_t *elem)
{
    ecs_id_record_t *prev = elem->prev;
    ecs_id_record_t *next = elem->next;
    ecs_assert(prev != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_record_elem_t *prev_elem = id_record_elem(idr, elem, prev);
    prev_elem->next = next;
    if (next) {
        ecs_id_record_elem_t *next_elem = id_record_elem(idr, elem, next);
        next_elem->prev = prev;
    }
}

static
void insert_id_elem(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_id_t wildcard,
    ecs_id_record_t *widr)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);
    if (!widr) {
        widr = flecs_ensure_id_record(world, wildcard);
    }
    ecs_assert(widr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        id_record_elem_insert(widr, idr, &idr->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        id_record_elem_insert(widr, idr, &idr->second);

        if (idr->flags & EcsIdAcyclic) {
            id_record_elem_insert(widr, idr, &idr->acyclic);
        }
    }
}

static
void remove_id_elem(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    ecs_id_t wildcard)
{
    ecs_assert(ecs_id_is_wildcard(wildcard), ECS_INTERNAL_ERROR, NULL);
    ecs_id_record_t *widr = flecs_get_id_record(world, wildcard);
    if (!widr) {
        return;
    }

    if (ECS_PAIR_SECOND(wildcard) == EcsWildcard) {
        ecs_assert(ECS_PAIR_FIRST(wildcard) != EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        id_record_elem_remove(idr, &idr->first);
    } else {
        ecs_assert(ECS_PAIR_FIRST(wildcard) == EcsWildcard, 
            ECS_INTERNAL_ERROR, NULL);
        id_record_elem_remove(idr, &idr->second);

        if (idr->flags & EcsIdAcyclic) {
            id_record_elem_remove(idr, &idr->acyclic);
        }
    }
}

static
ecs_id_record_t* new_id_record(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = ecs_os_calloc_t(ecs_id_record_t);
    ecs_table_cache_init(&idr->cache);

    idr->id = id;

    bool is_wildcard = ecs_id_is_wildcard(id);

    ecs_entity_t rel = 0, obj = 0, role = id & ECS_ROLE_MASK;
    if (role == ECS_PAIR) {
        rel = ecs_pair_first(world, id);
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);

        /* Relation object can be 0, as tables without a ChildOf relation are
         * added to the (ChildOf, 0) id record */
        obj = ECS_PAIR_SECOND(id);
        if (obj) {
            obj = ecs_get_alive(world, obj);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);
        }

        /* Check constraints */
        if (obj && !ecs_id_is_wildcard(obj)) {
            ecs_entity_t oneof = flecs_get_oneof(world, rel);
            ecs_check( !oneof || ecs_has_pair(world, obj, EcsChildOf, oneof),
                ECS_CONSTRAINT_VIOLATED, NULL);
            (void)oneof;
        }

        if (!is_wildcard) {
            /* Inherit flags from (relation, *) record */
            ecs_id_record_t *idr_r = flecs_ensure_id_record(
                world, ecs_pair(rel, EcsWildcard));
            idr->parent = idr_r;
            idr->flags = idr_r->flags;

            /* If pair is not a wildcard, append it to wildcard lists. These 
             * allow for quickly enumerating all relations for an object, or all 
             * objecs for a relation. */
            insert_id_elem(world, idr, ecs_pair(rel, EcsWildcard), idr_r);
            insert_id_elem(world, idr, ecs_pair(EcsWildcard, obj), NULL);

            if (rel == EcsUnion) {
                idr->flags |= EcsIdUnion;
            }
        }
    } else {
        rel = id & ECS_COMPONENT_MASK;
        rel = ecs_get_alive(world, rel);
        ecs_assert(rel != 0, ECS_INTERNAL_ERROR, NULL);
    }

    /* Initialize type info if id is not a tag */
    if (!is_wildcard && (!role || (role == ECS_PAIR))) {
        if (!(idr->flags & EcsIdTag)) {
            const ecs_type_info_t *ti = flecs_get_type_info(world, rel);
            if (!ti && obj) {
                ti = flecs_get_type_info(world, obj);
            }
            idr->type_info = ti;
        }
    }

    /* Mark entities that are used as component/pair ids. When a tracked
     * entity is deleted, cleanup policies are applied so that the store
     * won't contain any tables with deleted ids. */

    /* Flag for OnDelete policies */
    flecs_add_flag(world, rel, EcsEntityObservedId);
    if (obj) {
        /* Flag for OnDeleteObject policies */
        flecs_add_flag(world, obj, EcsEntityObservedObject);
        if (ecs_has_id(world, rel, EcsAcyclic)) {
            /* Flag used to determine if object should be traversed when
             * propagating events or with super/subset queries */
            flecs_add_flag(world, obj, EcsEntityObservedAcyclic);
        }
    }

    /* Flags for events */
    if (flecs_check_triggers_for_event(world, id, EcsOnAdd)) {
        idr->flags |= EcsIdHasOnAdd;
    }
    if (flecs_check_triggers_for_event(world, id, EcsOnRemove)) {
        idr->flags |= EcsIdHasOnRemove;
    }
    if (flecs_check_triggers_for_event(world, id, EcsOnSet)) {
        idr->flags |= EcsIdHasOnSet;
    }
    if (flecs_check_triggers_for_event(world, id, EcsUnSet)) {
        idr->flags |= EcsIdHasUnSet;
    }

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

        if (ECS_HAS_ROLE(id, PAIR)) {
            world->info.pair_id_count ++;
        }
    } else {
        world->info.wildcard_id_count ++;
    }

    return idr;
error:
    return NULL;
}

/* Cleanup id index */
static
bool free_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_poly_assert(world, ecs_world_t);
    (void)id;

    /* Force the empty table administration to be consistent if the non-empty
     * list of the id record has elements */
    if (ecs_table_cache_count(&idr->cache)) {
        ecs_run_aperiodic(world, EcsAperiodicEmptyTableEvents);
    }

    /* If there are still tables in the non-empty list they're really not empty.
     * We can't free the record yet. */
    if (ecs_table_cache_count(&idr->cache)) {
        return false;
    }

    /* If id record contains no more empty tables, free it */
    if (ecs_table_cache_empty_count(&idr->cache) == 0) {
        if (ecs_should_log_1()) {
            char *id_str = ecs_id_str(world, id);
            ecs_dbg_1("#[green]id#[normal] %s #[red]deleted", id_str);
            ecs_os_free(id_str);
        }

        if (ECS_HAS_ROLE(id, PAIR)) {
            if (!ecs_id_is_wildcard(id)) {
                ecs_entity_t rel = ecs_pair_first(world, id);
                ecs_entity_t obj = ECS_PAIR_SECOND(id);
                remove_id_elem(world, idr, ecs_pair(rel, EcsWildcard));
                remove_id_elem(world, idr, ecs_pair(EcsWildcard, obj));
            }
        }

        /* Update counters */
        world->info.id_delete_total ++;

        if (!ecs_id_is_wildcard(id)) {
            world->info.id_count --;

            if (ECS_HAS_ROLE(id, PAIR)) {
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

        ecs_table_cache_fini(&idr->cache);
        flecs_name_index_free(idr->name_index);
        ecs_os_free(idr);
        return true;
    }

    /* Delete empty tables */
    ecs_table_cache_iter_t cache_it;
    flecs_table_cache_empty_iter(&idr->cache, &cache_it);

    const ecs_table_record_t *tr;
    int32_t unreleased_count = 0;
    while ((tr = flecs_table_cache_next(&cache_it, ecs_table_record_t))) {
        if (!flecs_table_release(world, tr->hdr.table)) {
            /* Releasing the table did not free it, which means that something
             * is keeping the table alive. Cleanup of the id record will happen
             * when the last reference(s) to the table are released */
            unreleased_count ++;
        }
    }
    if (unreleased_count) {
        return false;
    }

    /* If all tables were deleted for this id record, the last deleted table
     * should have removed the record from the world. */
    ecs_assert(flecs_get_id_record(world, id) == NULL, 
        ECS_INTERNAL_ERROR, NULL);

    return true;
}

static
ecs_id_t flecs_id_record_id(
    ecs_id_t id)
{
    id = ecs_strip_generation(id);
    if (ECS_HAS_ROLE(id, PAIR)) {
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

ecs_id_record_t* flecs_ensure_id_record(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t **idr_ptr = ecs_map_ensure(&world->id_index, 
        ecs_id_record_t*, ecs_strip_generation(id));
    ecs_id_record_t *idr = idr_ptr[0];
    if (!idr) {
        idr = new_id_record(world, id);
        idr_ptr = ecs_map_get(&world->id_index, 
            ecs_id_record_t*, flecs_id_record_id(id));
        ecs_assert(idr_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        idr_ptr[0] = idr;
    }

    return idr;
}

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);
    return ecs_map_get_ptr(&world->id_index, ecs_id_record_t*,
        flecs_id_record_id(id));
}

ecs_id_record_t* flecs_get_query_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        if (ECS_HAS_ROLE(id, PAIR)) {
            idr = flecs_get_id_record(world,
                ecs_pair(EcsUnion, ECS_PAIR_FIRST(id)));
        }
        return idr;
    }
    if (ECS_HAS_ROLE(id, PAIR) && 
        ECS_PAIR_SECOND(id) == EcsWildcard && 
        (idr->flags & EcsIdUnion)) 
    {
        idr = flecs_get_id_record(world, 
            ecs_pair(EcsUnion, ECS_PAIR_FIRST(id)));
    }

    return idr;
}

void flecs_remove_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr)
{
    ecs_poly_assert(world, ecs_world_t);

    /* Free id record resources */
    if (free_id_record(world, id, idr)) {
        /* Remove record from world index */
        ecs_map_remove(&world->id_index, flecs_id_record_id(id));
    }
}

void flecs_clear_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr)
{
    ecs_poly_assert(world, ecs_world_t);
    
    if (world->is_fini) {
        return;
    }

    ecs_table_cache_fini_delete_all(world, &idr->cache);

    flecs_remove_id_record(world, id, idr);
}

bool flecs_set_type_info_for_id_record(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    const ecs_type_info_t *ti)
{
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

    bool changed = idr->type_info != ti;
    idr->type_info = ti;
    return changed;
}

ecs_hashmap_t* flecs_ensure_id_name_index(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_hashmap_t *map = idr->name_index;
    if (!map) {
        map = idr->name_index = flecs_name_index_new();
    }

    return map;
}

ecs_hashmap_t* flecs_get_id_name_index(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    return idr->name_index;
}

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t* idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    return (ecs_table_record_t*)ecs_table_cache_get(&idr->cache, table);
}

const ecs_table_record_t* flecs_id_record_table(
    ecs_id_record_t *idr,
    ecs_table_t *table)
{
    if (!idr) {
        return NULL;
    }
    return (ecs_table_record_t*)ecs_table_cache_get(&idr->cache, table);
}

void flecs_register_for_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    const ecs_table_t *table,
    ecs_table_record_t *tr)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_id_record_t *idr = flecs_ensure_id_record(world, id);
    if (!idr) {
        return;
    }

    ecs_table_cache_insert(&idr->cache, table, &tr->hdr);
}

void flecs_fini_id_records(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(&world->id_index);
    ecs_id_record_t *idr;
    ecs_map_key_t key;
    while ((idr = ecs_map_next_ptr(&it, ecs_id_record_t*, &key))) {
        free_id_record(world, key, idr);
    }

    ecs_map_fini(&world->id_index);
    flecs_sparse_free(world->pending_tables);
    flecs_sparse_free(world->pending_buffer);
}

ecs_id_record_t* flecs_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_cache_iter_t *out)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    flecs_process_pending_tables(world);
    ecs_assert( flecs_sparse_count(world->pending_tables) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    flecs_table_cache_iter(&idr->cache, out);
    return idr;
}

ecs_id_record_t* flecs_empty_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_cache_iter_t *out)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    flecs_process_pending_tables(world);
    ecs_assert( flecs_sparse_count(world->pending_tables) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    flecs_table_cache_empty_iter(&idr->cache, out);
    return idr;
}
