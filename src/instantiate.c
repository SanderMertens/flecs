/**
 * @file instantiate.c
 * @brief Functions for instantiating prefabs (IsA relationship).
 */

#include "private_api.h"

/* Resolve a slot relationship: find the ancestor that inherits from slot_of
 * and add the (slot, child) pair to it. */
static
void flecs_instantiate_slot(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    ecs_entity_t slot_of,
    ecs_entity_t slot,
    ecs_entity_t child)
{
    if (base == slot_of) {
        /* Instance inherits from slot_of, add slot to instance */
        ecs_component_record_t *cr = flecs_components_ensure(
            world, ecs_pair(slot, child));
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_record_t *r = flecs_entities_get(world, instance);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        flecs_sparse_on_add_cr(world, 
            r->table, ECS_RECORD_TO_ROW(r->row), cr, true, NULL);
    } else {
        /* Walk hierarchy upward to find instance that inherits from slot_of */
        ecs_entity_t parent = instance;
        int32_t depth = 0;
        do {
            if (ecs_has_pair(world, parent, EcsIsA, slot_of)) {
                const char *name = ecs_get_name(world, slot);
                if (name == NULL) {
                    char *slot_of_str = ecs_get_path(world, slot_of);
                    ecs_throw(ECS_INVALID_OPERATION, "prefab '%s' has unnamed "
                        "slot (slots must be named)", slot_of_str);
                    ecs_os_free(slot_of_str);
                    return;
                }

                /* Resolve the original slot entity under slot_of. */
                if (depth == 0) {
                    /* Direct child: simple name lookup */
                    slot = ecs_lookup_child(world, slot_of, name);
                } else {
                    /* Deeper nesting: use relative path */
                    char *path = ecs_get_path_w_sep(world, parent, child, ".",
                        NULL);
                    slot = ecs_lookup_path_w_sep(world, slot_of, path, ".", 
                        NULL, false);
                    ecs_os_free(path);
                }

                if (slot == 0) {
                    char *slot_of_str = ecs_get_path(world, slot_of);
                    char *slot_str = ecs_get_path(world, slot);
                    ecs_throw(ECS_INVALID_OPERATION,
                        "'%s' is not in hierarchy for slot '%s'",
                            slot_of_str, slot_str);
                    ecs_os_free(slot_of_str);
                    ecs_os_free(slot_str);
                }

                ecs_add_pair(world, parent, slot, child);
                break;
            }

            depth ++;
        } while ((parent = ecs_get_target(world, parent, EcsChildOf, 0)));
        
        if (parent == 0) {
            char *slot_of_str = ecs_get_path(world, slot_of);
            char *slot_str = ecs_get_path(world, slot);
            ecs_throw(ECS_INVALID_OPERATION,
                "'%s' is not in hierarchy for slot '%s'",
                    slot_of_str, slot_str);
            ecs_os_free(slot_of_str);
            ecs_os_free(slot_str);
        }
    }

error:
    return;
}

/* Insert an id into a type array in sorted order, shifting existing elements. */
static
int32_t flecs_child_type_insert(
    ecs_type_t *type,
    void **component_data,
    ecs_id_t id)
{
    int32_t i, count = type->count;
    for (i = 0; i < count; i ++) {
        ecs_id_t cur = type->array[i];
        if (cur == id) {
            /* Id is already part of type */
            return -1;
        }

        if (cur > id) {
            /* Array is sorted; id goes at this position */
            break;
        }
    }

    /* Assumes that the array has enough memory to store the new element. */
    int32_t to_move = type->count - i;
    if (to_move) {
        ecs_os_memmove(&type->array[i + 1],
            &type->array[i], to_move * ECS_SIZEOF(ecs_id_t));
        ecs_os_memmove(&component_data[i + 1],
            &component_data[i], to_move * ECS_SIZEOF(void*));
    }

    component_data[i] = NULL;
    type->array[i] = id;
    type->count ++;

    return i;
}

/* Copy sparse component data from base children to instance children. */
void flecs_instantiate_sparse(
    ecs_world_t *world,
    const ecs_table_range_t *base_child_range,
    const ecs_entity_t *base_children,
    ecs_table_t *instance_table,
    const ecs_entity_t *instance_children,
    int32_t row_offset)
{
    ecs_table_t *base_child_table = base_child_range->table;
    if (!(base_child_table->flags & EcsTableHasSparse)) {
        return;
    }

    ecs_table_record_t *trs = base_child_table->_->records;
    int32_t i, count = base_child_table->type.count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &trs[i];
        ecs_component_record_t *cr = tr->hdr.cr;

        if (!(cr->flags & EcsIdSparse)) {
            continue;
        }

        const ecs_type_info_t *ti = cr->type_info;
        if (!ti) {
            continue;
        }

        ecs_id_t id = base_child_table->type.array[i];

        for (int32_t j = 0; j < base_child_range->count; j ++) {
            ecs_entity_t child = base_children[j + base_child_range->offset];
            ecs_entity_t instance_child = instance_children[j];

            void *src_ptr = flecs_sparse_get(cr->sparse, ti->size, child);
            ecs_assert(src_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            void *dst_ptr = flecs_sparse_get(cr->sparse, ti->size, instance_child);
            ecs_assert(dst_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            flecs_type_info_copy(dst_ptr, src_ptr, 1, ti);

            flecs_notify_on_set(
                world, instance_table, row_offset + j, id, true);
        }
    }
}

/* Create instance children by cloning a range of prefab children. */
static
void flecs_instantiate_children(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    ecs_table_range_t child_range,
    const ecs_instantiate_ctx_t *ctx,
    int32_t depth)
{
    if (!child_range.count) {
        return;
    }

    ecs_table_t *child_table = child_range.table;
    ecs_type_t type = child_table->type;

    ecs_entity_t slot_of = 0;
    ecs_entity_t *ids = type.array;
    int32_t type_count = type.count;

    ecs_record_t *r = flecs_entities_get(world, instance);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Build the component type for the instance child table */
    ecs_table_diff_t diff = { .added = {0}};
    diff.added.array = ecs_os_alloca_n(ecs_entity_t, type_count + 1);
    void **component_data = ecs_os_alloca_n(void*, type_count + 1);

    /* Copy component ids, tracking ChildOf(base) position so we can
     * replace it with ChildOf(instance) below. */
    int j, i, childof_base_index = -1;
    for (i = 0; i < type_count; i ++) {
        ecs_id_t id = ids[i];

        /* Skip DontInherit ids, except name (preserved for child lookup)
         * and ChildOf (replaced with instance parent below). */
        if ((id != ecs_pair(ecs_id(EcsIdentifier), EcsName)) &&
            ECS_PAIR_FIRST(id) != EcsChildOf) 
        {
            ecs_table_record_t *tr = &child_table->_->records[i];
            ecs_component_record_t *cr = tr->hdr.cr;
            if (cr->flags & EcsIdOnInstantiateDontInherit) {
                continue;
            }
        }

        /* Track SlotOf target but don't add it to instance children.
         * Exception: keep SlotOf intact if the instance is itself a prefab. */
        if (!(table->flags & EcsTableIsPrefab)) {
            if (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsSlotOf) {
                ecs_assert(slot_of == 0, ECS_INTERNAL_ERROR, NULL);
                slot_of = ecs_pair_second(world, id);
                continue;
            }
        }

        /* Record the ChildOf(base) position; will be replaced with
         * ChildOf(instance) to reparent children under the instance. */
        if (ECS_HAS_RELATION(id, EcsChildOf) && 
           (ECS_PAIR_SECOND(id) == (uint32_t)base)) {
            childof_base_index = diff.added.count;
        }

        /* AUTO_OVERRIDE: insert the concrete id. Overrides appear after
         * concrete components in the type, so duplicates are handled. */
        if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
            ecs_id_t concreteId = id & ~ECS_AUTO_OVERRIDE;
            flecs_child_type_insert(&diff.added, component_data, concreteId);
            continue;
        }

        int32_t column = ecs_table_type_to_column_index(child_table, i);
        if (column != -1) {
            component_data[diff.added.count] = ecs_table_get_column(
                child_table, column, child_range.offset);
        } else {
            component_data[diff.added.count] = NULL;
        }

        diff.added.array[diff.added.count] = id;
        diff.added.count ++;
        diff.added_flags |= flecs_id_flags_get(world, id);
    }

    /* The child table must have a ChildOf(base) pair */
    ecs_assert(childof_base_index != -1, ECS_INTERNAL_ERROR, NULL);

    /* If children are added to a prefab, make sure they are prefabs too */
    if (table->flags & EcsTableIsPrefab) {
        if (flecs_child_type_insert(
            &diff.added, component_data, EcsPrefab) != -1) 
        {
            childof_base_index ++;
        }
    }

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *child_ids = flecs_walloc_n(
        world, ecs_entity_t, child_range.count);
    ecs_table_t *i_table = NULL;

    /* Replace ChildOf element in the component array with instance id */
    diff.added.array[childof_base_index] = ecs_pair(EcsChildOf, instance);

    /* Find or create table */
    i_table = flecs_table_find_or_create(world, &diff.added);

    ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(i_table->type.count == diff.added.count,
        ECS_INTERNAL_ERROR, NULL);

    /* Debug check: detect cycles where a prefab child is the same as the instance. */
    const ecs_entity_t *children = ecs_table_entities(child_table);

#ifdef FLECS_DEBUG
    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t child = children[j + child_range.offset];        
        ecs_check(child != instance, ECS_INVALID_PARAMETER, 
            "cycle detected in IsA relationship");
    }
#else
    /* Suppress unused-label warning for error: label */
    ecs_check(true, ECS_INVALID_OPERATION, NULL);
#endif

    /* Assign deterministic entity ids to instance children by mirroring
     * each child's offset from root_prefab onto root_instance. This gives
     * stable ids across networked applications. */
    ecs_instantiate_ctx_t ctx_cur = {base, instance};
    if (ctx) {
        ctx_cur = *ctx;
    }

    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t prefab_child = children[j + child_range.offset];
        if ((uint32_t)prefab_child < (uint32_t)ctx_cur.root_prefab) {
            /* Child id is smaller than root prefab id, can't use offset */
            child_ids[j] = flecs_new_id(world);
            continue;
        }

        /* Get prefab offset, ignore lifecycle generation count */
        ecs_entity_t prefab_offset =
            (uint32_t)prefab_child - (uint32_t)ctx_cur.root_prefab;
        ecs_assert(prefab_offset != 0, ECS_INTERNAL_ERROR, NULL);

        /* First check if any entity with the desired id exists */
        ecs_entity_t instance_child = (uint32_t)ctx_cur.root_instance + prefab_offset;
        ecs_entity_t alive_id = flecs_entities_get_alive(world, instance_child);
        if (alive_id && flecs_entities_is_alive(world, alive_id)) {
            /* Alive entity with requested id exists, can't use offset id */
            child_ids[j] = flecs_new_id(world);
            continue;
        }

        /* Id is not in use. Make it alive, preserving the generation from
         * root_instance (since prefab_offset is uint32_t, generation is
         * inherited from root_instance via addition). */
        instance_child = ctx_cur.root_instance + prefab_offset;
        flecs_entities_make_alive(world, instance_child);
        flecs_entities_ensure(world, instance_child);
        ecs_assert(ecs_is_alive(world, instance_child), ECS_INTERNAL_ERROR, NULL);

        child_ids[j] = instance_child;
    }

    /* Create children */
    int32_t child_row;
    diff.added_flags |= EcsTableEdgeReparent;
    const ecs_entity_t *i_children = flecs_bulk_new(world, i_table, child_ids,
        &diff.added, child_range.count, component_data, false, &child_row, &diff);

    flecs_instantiate_sparse(
        world, &child_range, children, i_table, i_children, child_row);

    /* If children are slots, add slot relationships to parent */
    if (slot_of) {
        for (j = 0; j < child_range.count; j ++) {
            ecs_entity_t child = children[j + child_range.offset];
            ecs_entity_t i_child = i_children[j];
            flecs_instantiate_slot(
                world, base, instance, slot_of, child, i_child);
        }
    }

    /* If prefab child table has children itself, recursively instantiate */
    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t child = children[j + child_range.offset];
        flecs_instantiate(world, child, i_children[j], &ctx_cur, depth + 1);
    }

    flecs_wfree_n(world, ecs_entity_t, child_range.count, child_ids);
error:
    return;    
}

/* Copy all non-fragmenting (sparse/DontFragment) components from base to instance. */
void flecs_instantiate_dont_fragment(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance)
{
    ecs_component_record_t *cur = world->cr_non_fragmenting_head;

    while (cur) {
        ecs_assert(cur->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
        if (cur->sparse && !(cur->flags & EcsIdOnInstantiateInherit) && 
            !ecs_id_is_wildcard(cur->id)) 
        {
            if (flecs_component_sparse_has(cur, base)) {
                void *base_ptr = flecs_component_sparse_get(
                    world, cur, NULL, base);
                const ecs_type_info_t *ti = cur->type_info;

                ecs_record_t *r = flecs_entities_get(world, instance);

                void *ptr = NULL;
                flecs_sparse_on_add_cr(world, 
                    r->table, ECS_RECORD_TO_ROW(r->row), cur, true, &ptr);

                if (ti) {
                    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                    flecs_type_info_copy(ptr, base_ptr, 1, ti);
                }

                if (ti) {
                    flecs_notify_on_set(
                        world, r->table, ECS_RECORD_TO_ROW(r->row), 
                        cur->id, true);
                }
            }
        }

        cur = cur->non_fragmenting.next;
    }
}

/* Add overridden non-fragmenting components from base table to instance. */
static
void flecs_instantiate_override_dont_fragment(
    ecs_world_t *world,
    ecs_table_t *base_table,
    ecs_entity_t instance)
{
    int32_t i, type_count = base_table->type.count;
    for (i = 0; i < type_count; i ++) {
        ecs_id_t id = base_table->type.array[i];
        if (!(id & ECS_AUTO_OVERRIDE)) {
            continue;
        }

        id &= ~ECS_AUTO_OVERRIDE;

        ecs_flags32_t flags = flecs_component_get_flags(world, id);
        if (!(flags & EcsIdDontFragment)) {
            continue;
        }

        ecs_add_id(world, instance, id);
    }
}

/* Recursively instantiate a prefab's children (and their children) for an instance. */
void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx,
    int32_t depth)
{
    ecs_record_t *record = flecs_entities_get_any(world, base);
    ecs_table_t *base_table = record->table;
    ecs_assert(base_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(depth < FLECS_DAG_DEPTH_MAX, ECS_CYCLE_DETECTED, 
        "likely cycle detected during instantiation of base %s",
        flecs_errstr(ecs_get_path(world, base)));

    if (base_table->flags & EcsTableOverrideDontFragment) {
        flecs_instantiate_override_dont_fragment(
            world, base_table, instance);
    }

    /* Copy non-fragmenting (sparse) components from base to instance */
    if (record->row & EcsEntityHasDontFragment) {
        flecs_instantiate_dont_fragment(world, base, instance);
    }

    if (!(base_table->flags & EcsTableIsPrefab)) {
        /* Don't instantiate children from base entities that aren't prefabs */
        return;
    }

    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(base));
    if (cr) {
        ecs_os_perf_trace_push("flecs.instantiate");

        if (cr->flags & EcsIdOrderedChildren) {
            if (flecs_component_has_non_fragmenting_childof(cr)) {
                EcsTreeSpawner *ts = flecs_get_mut(
                    world, base, ecs_id(EcsTreeSpawner), record, 
                    sizeof(EcsTreeSpawner)).ptr;
                if (!ts) {
                    ts = flecs_prefab_spawner_build(world, base);
                }

                if (ts) {
                    flecs_spawner_instantiate(world, ts, instance);
                }

                ecs_os_perf_trace_pop("flecs.instantiate");
                return;
            }

            ecs_vec_t *children_vec = &cr->pair->ordered_children;
            int32_t i, count = ecs_vec_count(children_vec);
            ecs_entity_t *children = ecs_vec_first(children_vec);
            for (i = 0; i < count; i ++) {
                ecs_entity_t child = children[i];
                ecs_table_range_t range = flecs_range_from_entity(world, child);

                if (!(range.table->flags & EcsTableHasChildOf)) {
                    continue;
                }

                flecs_instantiate_children(
                    world, base, instance, range, ctx, depth);
            }
        } else {
            ecs_table_cache_iter_t it;
            if (flecs_table_cache_all_iter((ecs_table_cache_t*)cr, &it)) {
                const ecs_table_record_t *tr;
                while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
                    ecs_table_range_t range = {
                        tr->hdr.table,
                        0,
                        ecs_table_count(tr->hdr.table)
                    };

                    flecs_instantiate_children(
                        world, base, instance, range, ctx, depth);
                }
            }
        }

        ecs_os_perf_trace_pop("flecs.instantiate");
    }
error:
    return;
}
