/**
 * @file on_delete.c
 * @brief Implementation of OnDelete/OnDeleteTarget traits.
 */

#include "private_api.h"

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

static
void flecs_target_mark_for_delete(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_component_record_t *cr;
    ecs_record_t *r = flecs_entities_get(world, e);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    /* If entity is not used as id or as relationship target, there won't
     * be any tables with a reference to it. */
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

static
bool flecs_id_is_delete_target(
    ecs_id_t id,
    ecs_entity_t action)
{
    if (!action && ecs_id_is_pair(id) && ECS_PAIR_FIRST(id) == EcsWildcard) {
        /* If no explicit delete action is provided, and the id we're deleting
         * has the form (*, Target), use OnDeleteTarget action */
        return true;
    }
    return false;
}

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

        /* If action is not specified and we're deleting a relationship target,
         * derive the action from the current record */
        int32_t i = tr->index, count = tr->count;
        do {
            ecs_type_t *type = &table->type;
            ecs_table_record_t *trr = &table->_->records[i];
            ecs_component_record_t *crr = trr->hdr.cr;
            result = ECS_ID_ON_DELETE_TARGET(crr->flags);
            if (result == EcsDelete) {
                /* Delete takes precedence over Remove */
                break;
            }

            if (count > 1) {
                /* If table contains multiple pairs for target they are not
                 * guaranteed to occupy consecutive elements in the table's type
                 * vector, so a linear search is needed to find matches. */
                for (++ i; i < type->count; i ++) {
                    if (ecs_id_match(type->array[i], id)) {
                        break;
                    }
                }

                /* We should always have as many matching ids as tr->count */
                ecs_assert(i < type->count, ECS_INTERNAL_ERROR, NULL);
            }
        } while (--count);
    }

    return result;
}

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

    /* Entity is still in use by a query */
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

static
void flecs_component_delete_non_fragmenting_childof(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_pair_record_t *pr = cr->pair;
    int32_t i, count = ecs_vec_count(&pr->ordered_children);
    ecs_entity_t *children = ecs_vec_first(&pr->ordered_children);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = children[i];

        ecs_record_t *r = flecs_entities_get_any(world, e);
        if ((r->row & EcsEntityIsTarget)) {
            ecs_component_record_t *child_cr = flecs_components_get(
                world, ecs_childof(e));
            if (child_cr) {
                if (!flecs_is_childof_tgt_only(child_cr)) {
                    /* Entity is used as target with other relationships, go
                     * through regular cleanup path. */
                    flecs_target_mark_for_delete(world, e);
                } else {
                    flecs_component_delete_non_fragmenting_childof(world, child_cr);
                }
            } else {
                /* Entity is a target but is not a ChildOf target. Go through
                 * regular cleanup path. */
                flecs_target_mark_for_delete(world, e);
            }
        }

        flecs_simple_delete(world, e, r);
    }

    ecs_component_record_t *tgt_wc = pr->second.prev;
    ecs_assert(ECS_PAIR_FIRST(tgt_wc->id) == EcsWildcard, ECS_INTERNAL_ERROR, NULL);

    flecs_component_release(world, tgt_wc);
}

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
            /* Entity is only used as ChildOf target */
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

    /* Mark all tables with the id for delete */
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

            /* If this is a Delete action, recursively mark ids & tables */
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

    /* Same for empty tables */
    if (flecs_table_cache_empty_iter(&cr->cache, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            tr->hdr.table->flags |= EcsTableMarkedForDelete;
        }
    }

    /* Flag component records for deletion */
    if (ecs_id_is_wildcard(id)) {
        ecs_assert(ECS_HAS_ID_FLAG(id, PAIR), ECS_INTERNAL_ERROR, NULL);
        ecs_component_record_t *cur = cr;
        if (ECS_PAIR_SECOND(id) == EcsWildcard) {
            while ((cur = flecs_component_first_next(cur))) {
                cur->flags |= EcsIdMarkedForDelete;
            }
        } else {
            /* Iterating all pairs for relationship target */
            ecs_assert(ECS_PAIR_FIRST(id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            while ((cur = flecs_component_second_next(cur))) {
                cur->flags |= EcsIdMarkedForDelete;

                /* If relationship is traversable and is removed upon deletion
                 * of a target, we may have to rematch queries. If a query 
                 * matched for example (IsA, A) -> (IsA, B) -> Position, and 
                 * B is deleted, Position would no longer be reachable from 
                 * tables that have (IsA, B). */
                if (cur->flags & EcsIdTraversable) {
                    /* If tables with (R, target) are deleted anyway we don't
                     * need to rematch. Since this will happen recursively it is
                     * guaranteed that queries cannot have tables that reached a
                     * component through the deleted entity. */
                    if (!(cur->flags & EcsIdOnDeleteTargetDelete)) {
                        /* Only bother if tables have relationship. */
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

static
bool flecs_on_delete_mark(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id)
{
    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        /* If there's no component record, there's nothing to delete */
        return false;
    }

    if (!action) {
        /* If no explicit action is provided, derive it */
        if (!ecs_id_is_pair(id) || ECS_PAIR_SECOND(id) == EcsWildcard) {
            /* Delete actions are determined by the component, or in the case
             * of a pair by the relationship. */
            action = ECS_ID_ON_DELETE(cr->flags);
        }
    }

    if (action == EcsPanic) {
        /* This id is protected from deletion */
        flecs_throw_invalid_delete(world, id);
        return false;
    }

    flecs_component_mark_for_delete(world, cr, action, delete_id);

    return true;
}

static
void flecs_remove_from_table(
    ecs_world_t *world, 
    ecs_table_t *table) 
{
    ecs_table_diff_t temp_diff = { .added = {0} };
    ecs_table_diff_builder_t diff = ECS_TABLE_DIFF_INIT;
    flecs_table_diff_builder_init(world, &diff);
    ecs_table_t *dst_table = table; 

    /* To find the dst table, remove all ids that are marked for deletion */
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

static
bool flecs_on_delete_clear_entities(
    ecs_world_t *world,
    bool force_delete)
{
    /* Iterate in reverse order so that DAGs get deleted bottom to top */
    int32_t i, last = ecs_vec_count(&world->store.marked_ids), first = 0;
    ecs_marked_id_t *ids = ecs_vec_first(&world->store.marked_ids);
    
    do {
        for (i = last - 1; i >= first; i --) {
            ecs_component_record_t *cr = ids[i].cr;
            ecs_entity_t action = ids[i].action;

            /* Empty all tables for id */
            ecs_table_cache_iter_t it;
            if (flecs_table_cache_iter(&cr->cache, &it)) {
                const ecs_table_record_t *tr;
                while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
                    ecs_table_t *table = tr->hdr.table;

                    /* If table contains prefabs and we're not deleting the 
                     * prefab entity itself (!force_delete), don't delete table.
                     * This means that delete_with/remove_all can be used safely
                     * for game entities without risking modifying prefabs. 
                     * If force_delete is true, it means that one of the 
                     * components, relationships or relationship targets is 
                     * being deleted in which case the table must go too. */
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

            /* If component record contains children with Parent components, 
             * delete them. */
            if (flecs_component_has_non_fragmenting_childof(cr)) {
                int32_t c, count = ecs_vec_count(&cr->pair->ordered_children);
                ecs_entity_t *children = ecs_vec_first(&cr->pair->ordered_children);
                
                ecs_defer_suspend(world);
                for (c = count - 1; c >= 0; c --) {
                    ecs_delete(world, children[c]);
                }
                ecs_defer_resume(world);
            }

            /* User code (from triggers) could have enqueued more ids to delete,
             * reobtain the array in case it got reallocated */
            ids = ecs_vec_first(&world->store.marked_ids);
        }

        /* Check if new ids were marked since we started */
        int32_t new_last = ecs_vec_count(&world->store.marked_ids);
        if (new_last != last) {
            /* Iterate remaining ids */
            ecs_assert(new_last > last, ECS_INTERNAL_ERROR, NULL);
            first = last;
            last = new_last;
        } else {
            break;
        }
    } while (true);

    return true;
}

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
            /* Release normal ids before wildcard ids */
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

            /* Run OnDeleteTarget traits for non-fragmenting relationships */
            ecs_id_t component_id = cr->id;
            if (ECS_IS_PAIR(component_id) && 
               (ECS_PAIR_FIRST(component_id) == EcsWildcard) &&
               (cr->flags & EcsIdMatchDontFragment)) 
            {
                flecs_on_delete_clear_sparse(world, cr);
            }

            /* Run OnDelete traits for non-fragmenting components */
            if (ids[i].action == EcsDelete) {
                if (cr->flags & EcsIdDontFragment) {
                    flecs_component_delete_sparse(world, cr);
                }
            }

            if (flecs_component_release_tables(world, cr)) {
                ecs_assert(!force_delete, ECS_INVALID_OPERATION, 
                    "cannot delete component '%s': tables are keeping it alive",
                    flecs_errstr(ecs_id_str(world, cr->id)));

                /* There are still tables remaining. This can happen when 
                 * flecs_table_keep has been called for a table, which is used
                 * whenever code doesn't want a table to get deleted. */
                cr->flags &= ~EcsIdMarkedForDelete;
                flecs_component_release(world, cr);
            } else {
                /* Release the claim taken by flecs_marked_id_push. This may delete the
                * component record as all other claims may have been released. */
                int32_t rc = flecs_component_release(world, cr);
                ecs_assert(rc >= 0, ECS_INTERNAL_ERROR, NULL);
                (void)rc;

                /* If rc is 0, the id was likely deleted by a nested delete_with call
                * made by an on_remove handler/OnRemove observer */
                if (rc) {
                    if (delete_id) {
                        /* If id should be deleted, release initial claim. This happens when
                         * a component, tag, or part of a pair is deleted. */
                        flecs_component_release(world, cr);
                    } else {
                        /* If id should not be deleted, unmark component record for deletion. This
                        * happens when all instances *of* an id are deleted, for example
                        * when calling ecs_remove_all or ecs_delete_with. */
                        cr->flags &= ~EcsIdMarkedForDelete;
                    }
                }
            }
        }
    } while (-- twice);

    return true;
}

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

void flecs_on_delete(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t action,
    bool delete_id,
    bool force_delete)
{
    /* Cleanup can happen recursively. If a cleanup action is already in 
     * progress, only append ids to the marked_ids. The topmost cleanup
     * frame will handle the actual cleanup. */
    int32_t i, count = ecs_vec_count(&world->store.marked_ids);

    /* Collect all ids that need to be deleted */
    flecs_on_delete_mark(world, id, action, delete_id);

    /* Only perform cleanup if we're the first stack frame doing it */
    if (!count && ecs_vec_count(&world->store.marked_ids)) {
        ecs_dbg_2("#[red]delete#[reset]");
        ecs_log_push_2();

        /* Delete all the entities from the to be deleted tables/components */
        flecs_on_delete_clear_entities(world, force_delete);

        /* Release remaining references to the ids */
        flecs_on_delete_clear_ids(world, force_delete);

        /* Ids are deleted, clear stack */
        ecs_vec_clear(&world->store.marked_ids);

        /* If any components got deleted, cleanup type info. Delaying this 
         * ensures that type info remains available during cleanup. */
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
