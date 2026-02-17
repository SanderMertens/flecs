#include "../private_api.h"

static
void flecs_add_non_fragmenting_child_to_table(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t entity,
    const ecs_table_t *table)
{
    ecs_map_init_if(&cr->pair->children_tables, &world->allocator);
    ecs_parent_record_t *elem = (ecs_parent_record_t*)
        ecs_map_ensure(&cr->pair->children_tables, table->id);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Encode id of first entity in table + the total number of entities in the
     * table for this parent in a single uint64 so everything fits in a map
     * element without having to allocate. */
    if (!elem->entity) {
        elem->entity = (uint32_t)entity;
        elem->count = 1;

        if (table->flags & EcsTableIsDisabled) {
            cr->pair->disabled_tables ++;
        }
        if (table->flags & EcsTableIsPrefab) {
            cr->pair->prefab_tables ++;
        }
    } else {
        elem->count ++;
        elem->entity = 0;
    }
}

static
void flecs_remove_non_fragmenting_child_from_table(
    ecs_component_record_t *cr,
    const ecs_table_t *table)
{
    ecs_parent_record_t *elem = flecs_component_get_parent_record(cr, table);
    if (!elem) {
        return;
    }

    elem->count --;

    if (!elem->count) {
        ecs_map_remove(&cr->pair->children_tables, table->id);
        if (table->flags & EcsTableIsDisabled) {
            cr->pair->disabled_tables --;
            ecs_assert(cr->pair->disabled_tables >= 0, 
                ECS_INTERNAL_ERROR, NULL);
        }
        if (table->flags & EcsTableIsPrefab) {
            cr->pair->prefab_tables --;
            ecs_assert(cr->pair->prefab_tables >= 0, 
                ECS_INTERNAL_ERROR, NULL);
        }
    }
}

int flecs_add_non_fragmenting_child_w_records(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity,
    ecs_component_record_t *cr,
    const ecs_record_t *r)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

    (void)parent;
    
    if (!(cr->flags & EcsIdOrderedChildren)) {
        flecs_component_ordered_children_init(world, cr);
        flecs_ordered_children_populate(world, cr);
    }

    ecs_check(parent != 0, ECS_INVALID_OPERATION, 
        "cannot set Parent component with 0 entity");
    ecs_check(ecs_is_alive(world, parent), ECS_INVALID_OPERATION, 
        "cannot set Parent component to entity that is not alive");
    
    flecs_ordered_entities_append(world, cr, entity);

    flecs_add_non_fragmenting_child_to_table(world, cr, entity, r->table);

    ecs_record_t *r_parent = flecs_entities_get(world, parent);
    if (r_parent->table->flags & EcsTableIsPrefab) {
        ecs_add_id(world, entity, EcsPrefab);
    }

    return 0;
error:
    return -1;
}

static
ecs_component_record_t* flecs_add_non_fragmenting_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity)
{
    ecs_component_record_t *cr = flecs_components_ensure(world, 
        ecs_pair(EcsChildOf, parent));
    ecs_record_t *r = flecs_entities_get(world, entity);
    
    if (flecs_add_non_fragmenting_child_w_records(world, parent, entity, cr, r)) {
        return NULL;
    }

    return cr;
}

static
void flecs_remove_non_fragmenting_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity)
{
    if (!parent) {
        return;
    }

    ecs_component_record_t *cr = flecs_components_get(world, 
        ecs_pair(EcsChildOf, parent));
    if (!cr || (cr->flags & EcsIdMarkedForDelete)) {
        return;
    }

    flecs_ordered_entities_remove(world, cr, entity);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_remove_non_fragmenting_child_from_table(cr, table);
}

static
void flecs_on_reparent_update_name(
    ecs_world_t *world,
    ecs_entity_t e,
    EcsIdentifier *name,
    ecs_entity_t parent_old,
    ecs_component_record_t *cr_parent_new) 
{
    if (name->value && name->index_hash) {
        /* Remove entity from name index of old parent */
        ecs_component_record_t *old_cr = flecs_components_get(
            world, ecs_childof(parent_old));
        if (old_cr) {
            ecs_hashmap_t *old_index = 
                flecs_component_name_index_get(world, old_cr);
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
}

static
void flecs_on_replace_parent(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsParent *old = ecs_field(it, EcsParent, 0);
    EcsParent *new = ecs_field(it, EcsParent, 1);
    
    ecs_assert(old != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new != NULL, ECS_INTERNAL_ERROR, NULL);

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

        flecs_journal_begin(world, EcsJournalSetParent, e, &(ecs_type_t){
            .count = 1, .array = &new_parent
        }, NULL);
        
        flecs_remove_non_fragmenting_child(world, old_parent, e);

        ecs_component_record_t *cr_parent = 
            flecs_add_non_fragmenting_child(world, new_parent, e);
        if (!cr_parent) {
            continue;
        }

        if (names) {
            flecs_on_reparent_update_name(
                world, e, &names[i], old_parent, cr_parent);
        }

        int32_t depth = cr_parent->pair->depth;
        ecs_add_id(world, e, ecs_value_pair(EcsParentDepth, depth));

        ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(e));
        if (cr) {
            flecs_component_update_childof_w_depth(world, cr, depth + 1);
        }

        ecs_record_t *r = flecs_entities_get(world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        if (r->row & EcsEntityIsTraversable) {
            ecs_id_t added = ecs_childof(new_parent);
            ecs_id_t removed = ecs_childof(old_parent);

            flecs_update_component_monitors(world, 
                &(ecs_type_t){ .count = 1, .array = &added },
                &(ecs_type_t) { .count = 1, .array = &removed });
        }

        flecs_journal_end();
    }
}

void flecs_on_non_fragmenting_child_move_add(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    ecs_assert(dst->flags & EcsTableHasParent, ECS_INTERNAL_ERROR, NULL);

    EcsParent *parents = ecs_table_get(world, dst, EcsParent, 0);
    int32_t i = row, end = i + count;
    for (; i < end; i ++) {
        ecs_entity_t e = ecs_table_entities(dst)[i];
        ecs_entity_t p = parents[i].value;

        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(p));
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (src && (src->flags & EcsTableHasParent)) {
            flecs_remove_non_fragmenting_child_from_table(cr, src);
        }

        flecs_add_non_fragmenting_child_to_table(world, cr, e, dst);
    }
}

void flecs_on_non_fragmenting_child_move_remove(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count,
    bool update_parent_records)
{
    EcsIdentifier *names = NULL;
    if (src->flags & EcsTableHasName) {
        names = ecs_table_get_pair(world, src, EcsIdentifier, EcsName, 0);
    }

    EcsParent *parents = ecs_table_get(world, src, EcsParent, 0);
    int32_t i = row, end = i + count;
    for (; i < end; i ++) {
        ecs_entity_t e = ecs_table_entities(src)[i];
        ecs_entity_t p = parents[i].value;
        if (!ecs_is_alive(world, p)) {
            continue;
        }

        ecs_component_record_t *cr = flecs_components_ensure(
            world, ecs_childof(p));
        
        if (update_parent_records) {
            flecs_remove_non_fragmenting_child_from_table(cr, src);
        }

        if (dst && (dst->flags & EcsTableHasParent)) {
            if (update_parent_records) {
                flecs_add_non_fragmenting_child_to_table(world, cr, e, dst);
            }
        } else {
            flecs_ordered_entities_remove(world, cr, e);

            if (names) {
                flecs_on_reparent_update_name(world, e, &names[i], p, NULL);
            }

            ecs_component_record_t *cr_e = flecs_components_get(
                world, ecs_childof(e));
            if (cr_e) {
                flecs_component_update_childof_w_depth(world, cr_e, 1);
            }
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
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!src) {
        return;
    }

    ecs_pair_record_t *dst_pair = flecs_table_get_childof_pr(world, dst);
    ecs_assert(dst_pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_pair_record_t *src_pair = flecs_table_get_childof_pr(world, src);

    int32_t dst_depth = dst_pair->depth;
    int32_t src_depth = 0;
    if (src_pair) {
        src_depth = src_pair->depth;
    }

    if (dst_depth == src_depth) {
        /* If src depth is dst depth, no need to update cached depth values */
        return;
    }

    if (!ecs_table_has_traversable(src)) {
        /* If table doesn't contain any traversable entities (meaning there 
         * can't be any parents in the table) there can't be any cached depth
         * values to update. */
        return;
    }

    const ecs_entity_t *entities = ecs_table_entities(dst);
    int32_t i = row, end = i + count;
    for (i = 0; i < end; i ++) {
        ecs_entity_t e = entities[i];
        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(e));
        if (!cr) {
            continue;
        }

        ecs_record_t *r = flecs_entities_get(world, e);
        flecs_component_update_childof_depth(world, cr, e, r);
    }
}

void flecs_non_fragmenting_childof_unparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_pair_record_t *dst_pair = flecs_table_get_childof_pr(world, dst);
    ecs_pair_record_t *src_pair = flecs_table_get_childof_pr(world, src);

    ecs_assert(src_pair != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t dst_depth = dst_pair ? dst_pair->depth : 0;
    int32_t src_depth = src_pair->depth;

    /* If unparent is called there has to be a difference in depth, since the
     * parent is removed from an entity. */
    ecs_assert(src_depth != dst_depth, ECS_INTERNAL_ERROR, NULL);
    (void)src_depth;
    (void)dst_depth;

    if (!ecs_table_has_traversable(src)) {
        /* If table doesn't contain any traversable entities (meaning there 
         * aren't any parents in the table) there can't be any cached depth
         * values to update. */
        return;
    }

    const ecs_entity_t *entities = ecs_table_entities(src);
    int32_t i = row, end = i + count;
    for (i = 0; i < end; i ++) {
        ecs_entity_t e = entities[i];
        ecs_component_record_t *cr = flecs_components_get(
            world, ecs_childof(e));
        if (!cr || (cr->flags & EcsIdMarkedForDelete)) {
            continue;
        } else {
            /* Entity is a parent */
        }

        /* Update depth to 1 if parent is removed */
        flecs_component_update_childof_w_depth(world, cr, 1);
    }
}

bool flecs_component_has_non_fragmenting_childof(
    ecs_component_record_t *cr)
{
    if (cr->flags & EcsIdOrderedChildren) {
        return ecs_map_count(&cr->pair->children_tables) != 0;
    }
    return false;
}

void flecs_bootstrap_parent_component(
    ecs_world_t *world)
{
    flecs_type_info_init(world, EcsParent, { 
        .ctor = flecs_default_ctor,
        .on_replace = flecs_on_replace_parent
    });
}
