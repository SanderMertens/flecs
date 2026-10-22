/**
 * @file storage/non_fragmenting_childof.c
 * @brief Non-fragmenting ChildOf storage.
 */

#include "../private_api.h"

void flecs_non_fragmenting_childof_count(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_table_t *table,
    int32_t count)
{
    ecs_pair_record_t *pr = cr->pair;
    pr->non_fragmenting_child_count += count;
    world->non_fragmenting_child_count += count;
    if (table->flags & EcsTableIsDisabled) {
        pr->disabled_children += count;
    }
    if (table->flags & EcsTableIsPrefab) {
        pr->prefab_children += count;
    }
    ecs_assert(pr->non_fragmenting_child_count >= 0,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->non_fragmenting_child_count >= 0,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pr->disabled_children >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pr->prefab_children >= 0, ECS_INTERNAL_ERROR, NULL);
}

void flecs_add_non_fragmenting_child_w_records(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity,
    ecs_component_record_t *cr,
    const ecs_record_t *r)
{
    (void)parent;
    ecs_assert(flecs_entities_is_alive(world, parent),
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!(cr->flags & EcsIdOrderedChildren)) {
        flecs_ordered_children_init(world, cr);
    }

    flecs_ordered_entities_append(world, cr, entity);
    flecs_non_fragmenting_childof_count(world, cr, r->table, 1);
    flecs_tree_spawner_assert_not_instantiated(world, parent);

#ifdef FLECS_PREFAB
    ecs_record_t *r_parent = flecs_entities_get(world, parent);
    if (r_parent->table->flags & EcsTableIsPrefab) {
        ecs_add_id(ecs_get_stage(world, 0), entity, EcsPrefab);
    }
#endif
}

static ecs_component_record_t* flecs_remove_non_fragmenting_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_record_t *r,
    ecs_entity_t entity)
{
    if (!parent) {
        return NULL;
    }

    ecs_component_record_t *cr = flecs_components_get(world,
        ecs_pair(EcsChildOf, parent));
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_ordered_entities_remove(world, cr, entity);
    flecs_non_fragmenting_childof_count(world, cr, r->table, -1);
    flecs_tree_spawner_assert_not_instantiated(world, parent);

    return cr;
}

static void flecs_on_reparent_update_name(
    ecs_world_t *world,
    ecs_entity_t e,
    EcsIdentifier *name,
    ecs_entity_t parent_old,
    ecs_component_record_t *cr_parent_new)
{
    if (!name->value || !name->index_hash) {
        return;
    }

    /* Remove entity from name index of old parent */
    ecs_component_record_t *old_cr = flecs_components_get(
        world, ecs_childof(parent_old));
    if (old_cr) {
        ecs_hashmap_t *old_index = flecs_component_name_index_get(world, old_cr);
        if (old_index) {
            flecs_name_index_remove(old_index, e, name->index_hash);
        }
    }

    if (cr_parent_new) {
        /* Add entity to name index of new parent */
        ecs_hashmap_t *new_index =
            flecs_component_name_index_ensure(world, cr_parent_new);
        flecs_name_index_ensure(
            new_index, e, name->value, name->length, name->hash);
        name->index = new_index;
    } else {
        name->index = NULL;
    }
}

static void flecs_on_replace_parent(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsParent *old = ecs_field(it, EcsParent, 0);
    EcsParent *new = ecs_field(it, EcsParent, 1);

    EcsIdentifier *names = NULL;
    if (it->table->flags & EcsTableHasName) {
        names = ecs_table_get_pair(
            world, it->table, EcsIdentifier, EcsName, it->offset);
        ecs_assert(names != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t old_parent = old[i].value;
        ecs_entity_t new_parent = new[i].value;
        ecs_record_t *r = flecs_entities_get(world, e);

        ecs_check(new_parent != 0, ECS_INVALID_OPERATION,
            "cannot set Parent component with 0 entity");
        ecs_check(flecs_entities_is_alive(world, new_parent),
            ECS_INVALID_OPERATION,
            "cannot set Parent component to entity that is not alive");

        /* Setting the parent a child already has is a no-op. Without this
         * check the child would be removed from and appended to the ordered
         * children vector, which would move it to the back. */
        if (old_parent == new_parent) {
            continue;
        }

    #ifdef FLECS_DEBUG
        ecs_entity_t cur = new_parent;
        while (cur) {
            ecs_assert(cur != e, ECS_CYCLE_DETECTED,
                "cycle detected in Parent hierarchy");
            cur = ecs_get_parent(world, cur);
        }
    #endif

        flecs_journal_begin(world, EcsJournalSetParent, e, &(ecs_type_t){
            .count = 1, .array = &new_parent
        }, NULL);

        ecs_component_record_t *cr_old =
            flecs_remove_non_fragmenting_child(world, old_parent, r, e);

        ecs_component_record_t *cr_parent = flecs_components_ensure(
            world, ecs_childof(new_parent));
        flecs_add_non_fragmenting_child_w_records(
            world, new_parent, e, cr_parent, r);

        if (names) {
            flecs_on_reparent_update_name(
                world, e, &names[i], old_parent, cr_parent);
        }

        /* Write new parent value to component storage before ecs_add_id, as
         * it can trigger a table move that reads the parent value. */
        old[i].value = new_parent;

        if (r->row & EcsEntityIsTraversable) {
            flecs_emit_propagate_invalidate(world, r->table,
                ECS_RECORD_TO_ROW(r->row), 1);
        }

        int32_t depth = cr_parent->pair->depth;

        /* If the entity had a parent before, it has a ParentDepth pair that
         * matches the depth of the old parent. If the depth of the new parent
         * is the same, the pair doesn't have to be updated and neither do the
         * cached depths for the entity's children. */
        if (!cr_old || cr_old->pair->depth != depth) {
            ecs_add_id(it->stage, e, ecs_value_pair(EcsParentDepth, depth));

            ecs_component_record_t *cr = flecs_components_get(
                world, ecs_childof(e));
            if (cr) {
                flecs_component_update_childof_w_depth(world, cr, depth + 1);
            }
        }

        flecs_journal_end();
    }
error:
    return;
}

void flecs_non_fragmenting_childof_move(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    bool keep_parent = dst->flags & EcsTableHasParent;
    if (keep_parent &&
        !((src->flags ^ dst->flags) & (EcsTableIsDisabled|EcsTableIsPrefab)))
    {
        return;
    }

    bool dst_childof = dst->flags & EcsTableHasChildOf;

    EcsIdentifier *names = NULL;
    ecs_component_record_t *cr_name_dst = NULL;
    if (!keep_parent && (src->flags & EcsTableHasName)) {
        names = ecs_table_get_pair(world, src, EcsIdentifier, EcsName, 0);
        if ((dst->flags & EcsTableHasName) && !dst_childof) {
            cr_name_dst = world->cr_childof_0;
        }
    }

    const ecs_entity_t *entities = ecs_table_entities(src);
    EcsParent *parents = ecs_table_get(world, src, EcsParent, 0);
    int32_t i = row, end = i + count;
    for (; i < end; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t p = parents[i].value;
        ecs_assert(flecs_entities_is_alive(world, p),
            ECS_INTERNAL_ERROR, NULL);

        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(p));
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

        flecs_non_fragmenting_childof_count(world, cr, src, -1);

        if (keep_parent) {
            flecs_non_fragmenting_childof_count(world, cr, dst, 1);
            continue;
        }

        flecs_ordered_entities_remove(world, cr, e);

        flecs_tree_spawner_assert_not_instantiated(world, p);

        if (names) {
            flecs_on_reparent_update_name(world, e, &names[i], p, cr_name_dst);
        }

        if (!dst_childof) {
            ecs_component_record_t *cr_e = flecs_components_get(
                world, ecs_childof(e));
            if (cr_e) {
                flecs_component_update_childof_w_depth(world, cr_e, 1);
            }
        }
    }
}

static void flecs_non_fragmenting_childof_update_depth(
    ecs_world_t *world,
    const ecs_table_t *table,
    int32_t row,
    int32_t count,
    int32_t depth)
{
    if (!ecs_table_has_traversable(table)) {
        return;
    }

    const ecs_entity_t *entities = ecs_table_entities(table);
    int32_t i = row, end = row + count;
    for (; i < end; i ++) {
        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(entities[i]));
        if (cr) {
            flecs_component_update_childof_w_depth(world, cr, depth);
        }
    }
}

void flecs_non_fragmenting_childof_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    if (!src) {
        return;
    }

    int32_t dst_depth = flecs_table_get_childof_pr(world, dst)->depth;
    int32_t src_depth = flecs_table_get_childof_pr(world, src)->depth;
    if (dst_depth != src_depth) {
        flecs_non_fragmenting_childof_update_depth(
            world, dst, row, count, dst_depth + 1);
    }
}

void flecs_non_fragmenting_childof_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    flecs_non_fragmenting_childof_update_depth(world, src, row, count, 1);
}

void flecs_bootstrap_parent_component(
    ecs_world_t *world)
{
    flecs_type_info_init(world, EcsParent, {
        .ctor = flecs_default_ctor,
        .on_replace = flecs_on_replace_parent
    });

    ecs_add_pair(world, ecs_id(EcsParent), EcsOnInstantiate, EcsDontInherit);
}
