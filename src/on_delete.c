/**
 * @file on_delete.c
 * @brief Implementation of OnDelete/OnDeleteTarget traits.
 */

#include "private_api.h"

/* Push a component record onto the marked-for-delete stack. */
static
void flecs_marked_id_push(
    ecs_world_t *world,
    ecs_component_record_t* cr,
    ecs_entity_t action,
    bool delete_id)
{
    ecs_marked_id_t *m = ecs_vec_append_t(&world->allocator,
        &world->store.marked_ids, ecs_marked_id_t);

    m->cr = cr;
    m->id = cr->id;
    m->action = action;
    m->delete_id = delete_id;

    cr->flags |= EcsIdMarkedForDelete;

    flecs_component_claim(world, cr);
}

static
void flecs_component_mark_for_delete(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t action,
    bool delete_id);

/* Mark all component records that reference an entity (as id or target). */
static
void flecs_target_mark_for_delete(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_component_record_t *cr;
    ecs_record_t *r = flecs_entities_get(world, e);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Only entities used as ids or relationship targets have referencing tables */
    ecs_flags32_t flags = r->row & ECS_ROW_FLAGS_MASK;
    if (!(flags & (EcsEntityIsId|EcsEntityIsTarget))) {
        return;
    }

    if (flags & EcsEntityIsId) {
        if ((cr = flecs_components_get(world, e))) {
            flecs_component_mark_for_delete(world, cr, 
                ECS_ID_ON_DELETE(cr->flags), true);
        }
        if ((cr = flecs_components_get(world, ecs_pair(e, EcsWildcard)))) {
            flecs_component_mark_for_delete(world, cr, 
                ECS_ID_ON_DELETE(cr->flags), true);
        }
    }
    if (flags & EcsEntityIsTarget) {
        if ((cr = flecs_components_get(world, ecs_pair(EcsWildcard, e)))) {
            flecs_component_mark_for_delete(world, cr, 
                ECS_ID_ON_DELETE_TARGET(cr->flags), true);
        }
        if ((cr = flecs_components_get(world, ecs_pair(EcsFlag, e)))) {
            flecs_component_mark_for_delete(world, cr, 
                ECS_ID_ON_DELETE_TARGET(cr->flags), true);
        }
    }
}

/* Mark all entities in a table for cascading deletion. */
static
void flecs_targets_mark_for_delete(
    ecs_world_t *world,
    ecs_table_t *table)
{
    const ecs_entity_t *entities = ecs_table_entities(table);
    int32_t i, count = ecs_table_count(table);
    for (i = 0; i < count; i ++) {
        flecs_target_mark_for_delete(world, entities[i]);
    }
}

/* Check if id has the form (*, Target) with no explicit action, meaning
 * OnDeleteTarget policy should be used. */
static
bool flecs_id_is_delete_target(
    ecs_id_t id,
    ecs_entity_t action)
{
    if (!action && ecs_id_is_pair(id) && ECS_PAIR_FIRST(id) == EcsWildcard) {
        return true;
    }
    return false;
}

/* Resolve the effective delete action for a table record, considering
 * per-relationship OnDeleteTarget policies when deleting a target. */
static
ecs_entity_t flecs_get_delete_action(
    ecs_table_t *table,
    const ecs_table_record_t *tr,
    ecs_entity_t action,
    bool delete_target)
{
    ecs_entity_t result = action;
    if (!result && delete_target) {
        ecs_component_record_t *cr = tr->hdr.cr;
        ecs_id_t id = cr->id;

        /* Scan matching pairs in the table for the strongest policy */
        int32_t i = tr->index, count = tr->count;
        do {
            ecs_type_t *type = &table->type;
            ecs_table_record_t *trr = &table->_->records[i];
            ecs_component_record_t *crr = trr->hdr.cr;
            result = ECS_ID_ON_DELETE_TARGET(crr->flags);
            if (result == EcsDelete) {
                break; /* Delete is the strongest policy */
            }

            if (count > 1) {
                /* Pairs may not be contiguous; linear scan for next match */
                for (++ i; i < type->count; i ++) {
                    if (ecs_id_match(type->array[i], id)) {
                        break;
                    }
                }

                /* Must find as many matches as tr->count */
                ecs_assert(i < type->count, ECS_INTERNAL_ERROR, NULL);
            }
        } while (--count);
    }

    return result;
}

/* Delete an entity without recursive id cleanup (simple fast path). */
static
void flecs_simple_delete(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *r)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!ecs_has_pair(world, entity, EcsOnDelete, EcsPanic),
        ECS_CONSTRAINT_VIOLATED,
            "cannot delete entity '%s' with (OnDelete, Panic) trait",
                flecs_errstr(ecs_get_path(world, entity)));

    flecs_journal_begin(world, EcsJournalDelete, entity, NULL, NULL);

    /* Fail if entity is still referenced by active queries */
    ecs_assert((world->flags & EcsWorldQuit) || 
            !flecs_component_is_delete_locked(world, entity), 
        ECS_INVALID_OPERATION, 
        "cannot delete '%s' as it is still in use by queries",
            flecs_errstr(ecs_id_str(world, entity)));

    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_diff_t diff = {
        .removed = table->type,
        .removed_flags = table->flags & EcsTableRemoveEdgeFlags
    };

    int32_t row = ECS_RECORD_TO_ROW(r->row);
    flecs_actions_delete_tree(world, table, row, 1, &diff);
    flecs_entity_remove_non_fragmenting(world, entity, r);
    flecs_table_delete(world, table, row, true);

    flecs_entities_remove(world, entity);

    flecs_journal_end();
error:
    return;
}

/* Check if a component record is only used as a ChildOf target (no other
 * relationships reference this entity). */
static
bool flecs_is_childof_tgt_only(
    const ecs_component_record_t *cr)
{
    ecs_pair_record_t *pr = cr->pair;
    if (pr->second.next) {
        return false;
    }

    if (ECS_PAIR_FIRST(pr->second.prev->id) != EcsWildcard) {
        return false;
    }

    return true;
}

/* Fast-path deletion of children via non-fragmenting ChildOf ordered list. */
static
void flecs_component_delete_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    cr->flags |= EcsIdMarkedForDelete;

    ecs_pair_record_t *pr = cr->pair;
    int32_t i, count = ecs_vec_count(&pr->ordered_children);
    ecs_entity_t *children = ecs_vec_first(&pr->ordered_children);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = children[i];
        ecs_assert(ecs_is_alive(world, e), ECS_INTERNAL_ERROR, NULL);

        ecs_record_t *r = flecs_entities_get_any(world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        if ((r->row & EcsEntityIsTarget)) {
            ecs_component_record_t *child_cr = flecs_components_get(
                world, ecs_childof(e));
            if (child_cr &&
                !(child_cr->flags & EcsIdMarkedForDelete) &&
                flecs_component_has_non_fragmenting_childof(child_cr))
            {
                if (!flecs_is_childof_tgt_only(child_cr)) {
                    /* Has other relationships; use regular cleanup */
                    flecs_target_mark_for_delete(world, e);
                } else {
                    flecs_component_delete_non_fragmenting_childof(
                        world, child_cr);
                }
            } else {
                /* Not a non-fragmenting ChildOf target; use regular cleanup */
                flecs_target_mark_for_delete(world, e);
            }
        }

        flecs_simple_delete(world, e, r);
    }

    ecs_component_record_t *tgt_wc = pr->second.prev;
    ecs_assert(ECS_PAIR_FIRST(tgt_wc->id) == EcsWildcard, ECS_INTERNAL_ERROR, NULL);
    flecs_component_release(world, tgt_wc);
}

/* Try the non-fragmenting ChildOf fast path for cascading deletion.
 * Returns true if the fast path handled everything. */
static
bool flecs_component_mark_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_entity_t tgt = ECS_PAIR_SECOND(cr->id);

    ecs_component_record_t *childof_cr = flecs_components_get(
        world, ecs_childof(tgt));
    if (!childof_cr) {
        return false;
    }

    ecs_flags32_t flags = childof_cr->flags;

    if (flags & EcsIdMarkedForDelete) {
        return false;
    }

    childof_cr->flags |= EcsIdMarkedForDelete;

    if (!flecs_component_has_non_fragmenting_childof(childof_cr)) {
        return false;
    }

    ecs_pair_record_t *pr = childof_cr->pair;

    if (!pr->second.next) {
        if (ECS_PAIR_FIRST(pr->second.prev->id) == EcsWildcard) {
            /* Only used as ChildOf target; use fast path */
            flecs_component_delete_non_fragmenting_childof(world, childof_cr);
            return true;
        }
    }

    flecs_marked_id_push(world, childof_cr, EcsDelete, true);

    int32_t i, count = ecs_vec_count(&pr->ordered_children);
    ecs_entity_t *children = ecs_vec_first(&pr->ordered_children);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = children[i];

        ecs_component_record_t *tgt_cr = flecs_components_get(
            world, ecs_pair(EcsWildcard, e));
        if (!tgt_cr) {
            continue;
        }

        flecs_component_mark_for_delete(world, tgt_cr, 0, true);
    }

    return false;
}

/* Recursively mark a component record and its tables for deletion. */
static
void flecs_component_mark_for_delete(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t action,
    bool delete_id)
{
    if (cr->flags & EcsIdMarkedForDelete) {
        return;
    }

    flecs_marked_id_push(world, cr, action, delete_id);

    ecs_id_t id = cr->id;

    bool delete_target = flecs_id_is_delete_target(id, action);
    if (delete_target) {
        if (flecs_component_mark_non_fragmenting_childof(world, cr)) {
            return;
        }
    }

    /* Mark all tables containing this id */
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&cr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            if (table->flags & EcsTableMarkedForDelete) {
                continue;
            }

            ecs_entity_t cur_action = flecs_get_delete_action(table, tr, action,
                delete_target);

            /* Delete action: recursively mark child ids and tables */
            if (cur_action == EcsDelete) {
                table->flags |= EcsTableMarkedForDelete;
                ecs_log_push_2();
                flecs_targets_mark_for_delete(world, table);
                ecs_log_pop_2();
            } else if (cur_action == EcsPanic) {
                flecs_throw_invalid_delete(world, id);
            }
        }
    }

    /* Also mark empty tables */
    if (flecs_table_cache_empty_iter(&cr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            tr->hdr.table->flags |= EcsTableMarkedForDelete;
        }
    }

    /* For wildcard ids, propagate the marked flag to matching concrete records */
    if (ecs_id_is_wildcard(id)) {
        ecs_assert(ECS_HAS_ID_FLAG(id, PAIR), ECS_INTERNAL_ERROR, NULL);
        ecs_component_record_t *cur = cr;
        if (ECS_PAIR_SECOND(id) == EcsWildcard) {
            while ((cur = flecs_component_first_next(cur))) {
                cur->flags |= EcsIdMarkedForDelete;
            }
        } else {
            /* Iterate all pairs for this target (*, X) */
            ecs_assert(ECS_PAIR_FIRST(id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            while ((cur = flecs_component_second_next(cur))) {
                cur->flags |= EcsIdMarkedForDelete;

                /* Traversable relationships affect query matching through
                 * inheritance. Trigger rematch if tables survive deletion. */
                if (cur->flags & EcsIdTraversable) {
                    /* Skip if tables with this pair will be deleted anyway */
                    if (!(cur->flags & EcsIdOnDeleteTargetDelete)) {
                        if (ecs_map_count(&cur->cache.index)) {
                            flecs_update_component_monitors(world, NULL, 
                                &(ecs_type_t){
                                    .array = (ecs_id_t[]){cur->id},
                                    .count = 1
                                });
                        }
                    }
                }
            }
        }
    }
}

/* Mark an id and its component records for deletion. Returns false if
 * the id does not exist or is protected by a Panic policy. */
static
bool flecs_on_delete_mark(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return false;
    }

    if (!action) {
        /* Derive action from the component or relationship */
        if (!ecs_id_is_pair(id) || ECS_PAIR_SECOND(id) == EcsWildcard) {
            action = ECS_ID_ON_DELETE(cr->flags);
        }
    }

    if (action == EcsPanic) {
        flecs_throw_invalid_delete(world, id);
        return false;
    }

    flecs_component_mark_for_delete(world, cr, action, delete_id);

    return true;
}

/* Remove all marked ids from a table by computing a destination table
 * with those ids stripped, then merging entities into it. */
static
void flecs_remove_from_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_table_diff_t temp_diff = { .added = {0} };
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    ecs_table_t *dst_table = table; 

    /* Compute destination table by removing all marked ids */
    int32_t i, t, count = ecs_vec_count(&world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    const ecs_table_record_t *tr;
    for (i = 0; i < count; i ++) {
        const ecs_component_record_t *cr = ids[i].cr;

        if (!(tr = flecs_component_get_table(cr, dst_table))) {
            continue;
        }

        t = tr->index;

        do {
            ecs_id_t id = dst_table->type.array[t];
            ecs_table_t *tgt_table = flecs_table_traverse_remove(
                world, dst_table, &id, &temp_diff);
            ecs_assert(tgt_table != dst_table, ECS_INTERNAL_ERROR, NULL);
            dst_table = tgt_table;
            flecs_table_diff_build_append_table(world, &diff, &temp_diff);
        } while (dst_table->type.count && (t = ecs_search_offset(
            world, dst_table, t, cr->id, NULL)) != -1);
    }

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (dst_table != table) {
        int32_t table_count = ecs_table_count(table);
        if (diff.removed.count && table_count) {
            ecs_log_push_3();
            ecs_table_diff_t td;
            flecs_table_diff_build_noalloc(&diff, &td);

            if (table->flags & EcsTableHasTraversable) {
                for (i = 0; i < diff.removed.count; i ++) {
                    flecs_update_component_monitors(world, NULL, &(ecs_type_t){
                        .array = (ecs_id_t[]){td.removed.array[i]},
                        .count = 1
                    });
                }
            }

            flecs_actions_move_remove(world, table, dst_table, 0, table_count, &td);
            ecs_log_pop_3();
        }

        flecs_table_merge(world, dst_table, table);
    }

    flecs_table_diff_builder_fini(world, &diff);
}

/* Process all marked ids: delete or move entities from affected tables. */
static
bool flecs_on_delete_clear_entities(
    ecs_world_t *world,
    bool force_delete)
{
    /* Reverse iteration ensures DAG leaves are deleted before parents */
    int32_t i, last = ecs_vec_count(&world->store.marked_ids), first = 0;
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    
    do {
        for (i = last - 1; i >= first; i --) {
            ecs_component_record_t *cr = ids[i].cr;
            ecs_entity_t action = ids[i].action;

            /* Process all tables containing this id */
            ecs_table_cache_iter_t it;
            if (flecs_table_cache_iter(&cr->cache, &it)) {
                const ecs_table_record_t *tr;
                while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
                    ecs_table_t *table = tr->hdr.table;

                    /* Preserve prefab tables unless force_delete is set
                     * (i.e., the owning entity is being deleted). */
                    if ((table->flags & EcsTableIsPrefab) && !force_delete) {
                        table->flags &= ~EcsTableMarkedForDelete;
                        continue;
                    }

                    if ((action == EcsRemove) || 
                        !(table->flags & EcsTableMarkedForDelete))
                    {
                        flecs_remove_from_table(world, table);
                    } else {
                        ecs_dbg_3(
                            "#[red]delete#[reset] entities from table %u", 
                            (uint32_t)table->id);
                        ecs_table_clear_entities(world, table);
                    }
                }
            }

            /* Delete non-fragmenting ChildOf children */
            if (flecs_component_has_non_fragmenting_childof(cr)) {
                int32_t c, count = ecs_vec_count(&cr->pair->ordered_children);
                ecs_entity_t *children = ecs_vec_first(&cr->pair->ordered_children);
                
                ecs_defer_suspend(world);
                for (c = count - 1; c >= 0; c --) {
                    ecs_delete(world, children[c]);
                }
                ecs_defer_resume(world);
            }

            /* Observer callbacks may have marked new ids; re-fetch array */
            ids = ecs_vec_first(&world->store.marked_ids);
        }

        /* Check for newly marked ids from this pass */
        int32_t new_last = ecs_vec_count(&world->store.marked_ids);
        if (new_last != last) {
            ecs_assert(new_last > last, ECS_INTERNAL_ERROR, NULL);
            first = last;
            last = new_last;
        } else {
            break;
        }
    } while (true);

    return true;
}

/* Clear sparse storage for non-fragmenting components when a target is deleted. */
static
void flecs_on_delete_clear_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_component_record_t *cur = cr;
    while ((cur = flecs_component_second_next(cur))) {
        if (!cur->sparse || (!(cur->flags & EcsIdDontFragment))) {
            continue;
        }

        if (cur->flags & EcsIdOnDeleteTargetDelete) {
            flecs_component_delete_sparse(world, cur);
        } else if (cur->flags & EcsIdOnDeleteTargetPanic) {
            flecs_throw_invalid_delete(world, cr->id);
        }
    }
}

/* Release component records for all marked ids. Two passes: non-wildcard ids
 * first, then wildcard ids, to ensure correct cleanup ordering. */
static
bool flecs_on_delete_clear_ids(
    ecs_world_t *world,
    bool force_delete)
{
    int32_t i, count = ecs_vec_count(&world->store.marked_ids);
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    int twice = 2;

    (void)force_delete;

    do {
        for (i = 0; i < count; i ++) {
            /* Pass 1: concrete ids only. Pass 2: wildcard ids only. */
            if (ecs_id_is_wildcard(ids[i].id)) {
                if (twice == 2) {
                    continue;
                }
            } else {
                if (twice == 1) {
                    continue;
                }
            }

            ecs_component_record_t *cr = ids[i].cr;
            bool delete_id = ids[i].delete_id;

            /* Apply OnDeleteTarget for non-fragmenting relationships */
            ecs_id_t component_id = cr->id;
            if (ECS_IS_PAIR(component_id) && 
               (ECS_PAIR_FIRST(component_id) == EcsWildcard) &&
               (cr->flags & EcsIdMatchDontFragment)) 
            {
                flecs_on_delete_clear_sparse(world, cr);
            }

            /* Apply OnDelete for non-fragmenting components */
            if (ids[i].action == EcsDelete) {
                if (cr->flags & EcsIdDontFragment) {
                    flecs_component_delete_sparse(world, cr);
                }
            }

            if (flecs_component_release_tables(world, cr)) {
                ecs_assert(!force_delete, ECS_INVALID_OPERATION,
                    "cannot delete component '%s': tables are keeping it alive",
                    flecs_errstr(ecs_id_str(world, cr->id)));

                /* Tables still alive (kept via flecs_table_keep) */
                cr->flags &= ~EcsIdMarkedForDelete;
                flecs_component_release(world, cr);
            } else {
                /* Release the claim taken by flecs_marked_id_push */
                int32_t rc = flecs_component_release(world, cr);
                ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);
                (void)rc;

                /* rc==0 means a nested delete_with already freed the record */
                if (rc) {
                    if (delete_id) {
                        /* Component/tag/pair part deleted: release initial claim */
                        flecs_component_release(world, cr);
                    } else {
                        /* Only instances removed (delete_with/remove_all): keep id */
                        cr->flags &= ~EcsIdMarkedForDelete;
                    }
                }
            }
        }
    } while (-- twice);

    return true;
}

/* Throw an error for violating an (OnDelete, Panic) constraint. */
void flecs_throw_invalid_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    (void)id;

    if (!(world->flags & EcsWorldQuit)) {
        ecs_throw(ECS_CONSTRAINT_VIOLATED, 
            "(OnDelete, Panic) constraint violated while deleting entities with %s", 
            flecs_errstr(ecs_id_str(world, id)));
    }
error:
    return;
}

/* Execute the full OnDelete cleanup sequence: mark, clear entities, release
 * component records, free type info. Recursive calls defer to the outermost frame. */
void flecs_on_delete(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id,
    bool force_delete)
{
    int32_t i, count = ecs_vec_count(&world->store.marked_ids);

    flecs_on_delete_mark(world, id, action, delete_id);

    /* Only the outermost call performs the actual cleanup */
    if (!count && ecs_vec_count(&world->store.marked_ids)) {
        ecs_dbg_2("#[red]delete#[reset]");
        ecs_log_push_2();

        flecs_on_delete_clear_entities(world, force_delete);
        flecs_on_delete_clear_ids(world, force_delete);
        ecs_vec_clear(&world->store.marked_ids);

        /* Free type info last so it remains available during cleanup */
        count = ecs_vec_count(&world->store.deleted_components);
        ecs_entity_t *comps = ecs_vec_first(&world->store.deleted_components);
        for (i = 0; i < count; i ++) {
            flecs_type_info_free(world, comps[i]);
        }

        ecs_vec_clear(&world->store.deleted_components);

        ecs_log_pop_2();
    }
}

void ecs_delete_with(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_journal_begin(world, EcsJournalDeleteWith, id, NULL, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(stage, id, EcsDelete)) {
        return;
    }

    flecs_on_delete(world, id, EcsDelete, false, false);
    flecs_defer_end(world, stage);

    flecs_journal_end();
}

void ecs_remove_all(
    ecs_world_t *world,
    ecs_id_t id)
{
    flecs_journal_begin(world, EcsJournalRemoveAll, id, NULL, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_on_delete_action(stage, id, EcsRemove)) {
        return;
    }

    flecs_on_delete(world, id, EcsRemove, false, false);
    flecs_defer_end(world, stage);

    flecs_journal_end();
}
