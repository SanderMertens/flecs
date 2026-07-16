/**
 * @file instantiate.c
 * @brief Functions for instantiating prefabs (IsA relationship).
 */

#include "../../private_api.h"

#ifdef FLECS_PREFAB

static int32_t flecs_child_type_insert(
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
            /* A larger id was found so id can't be part of the type. */
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

ecs_entity_t flecs_instantiate_alloc_child_id(
    ecs_world_t *world,
    ecs_entity_t prefab_child,
    ecs_entity_t root_prefab,
    ecs_entity_t root_instance)
{
    if ((uint32_t)prefab_child < (uint32_t)root_prefab) {
        return flecs_new_id(world);
    }

    ecs_entity_t prefab_offset =
        (uint32_t)prefab_child - (uint32_t)root_prefab;
    ecs_assert(prefab_offset != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t instance_child = (uint32_t)root_instance + prefab_offset;
    ecs_entity_t alive_id = flecs_entities_get_alive(world, instance_child);
    if (alive_id && flecs_entities_is_alive(world, alive_id)) {
        return flecs_new_id(world);
    }

    instance_child = root_instance + prefab_offset;
    flecs_entities_make_alive(world, instance_child);
    flecs_entities_ensure(world, instance_child);
    ecs_assert(ecs_is_alive(world, instance_child), ECS_INTERNAL_ERROR, NULL);

    return instance_child;
}

void flecs_instantiate_sparse(
    ecs_world_t *world,
    const ecs_table_range_t *base_child_range,
    const ecs_entity_t *base_children,
    ecs_table_t *instance_table,
    const ecs_entity_t *instance_children,
    int32_t row_offset,
    bool emit_non_sparse)
{
    ecs_table_t *base_child_table = base_child_range->table;
    if (!emit_non_sparse && !(base_child_table->flags & EcsTableHasSparse)) {
        return;
    }

    ecs_table_record_t *trs = base_child_table->_->records;
    int32_t i, count = base_child_table->type.count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &trs[i];
        ecs_component_record_t *cr = tr->hdr.cr;

        bool sparse = cr->flags & EcsIdSparse;
        if (!sparse && !emit_non_sparse) {
            continue;
        }

        const ecs_type_info_t *ti = cr->type_info;
        if (!ti) {
            continue;
        }

        if (cr->flags & EcsIdOnInstantiateDontInherit) {
            continue;
        }

        ecs_id_t id = base_child_table->type.array[i];

        for (int32_t j = 0; j < base_child_range->count; j ++) {
            ecs_entity_t instance_child = instance_children[j];
            void *dst_ptr = NULL;

            /* Sparse component values live outside the instance table, so they
             * are copied here. Non-sparse override values are already in place
             * (copied when the instance entered its table). */
            if (sparse) {
                ecs_entity_t child = base_children[j + base_child_range->offset];

                void *src_ptr = flecs_sparse_get(cr->sparse, ti->size, child);
                ecs_assert(src_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                dst_ptr = flecs_sparse_get(
                    cr->sparse, ti->size, instance_child);
                ecs_assert(dst_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                flecs_type_info_copy(dst_ptr, src_ptr, 1, ti);
            }

            flecs_notify_on_set(
                world, instance_table, row_offset + j, id, true, dst_ptr);
        }
    }
}

static void flecs_instantiate_children(
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

    ecs_entity_t *ids = type.array;
    int32_t type_count = type.count;

    ecs_record_t *r = flecs_entities_get(world, instance);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_table_diff_t diff = { .added = {0}};
    diff.added.array = ecs_os_alloca_n(ecs_entity_t, type_count + 1);
    void **component_data = ecs_os_alloca_n(void*, type_count + 1);

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int j, i, childof_base_index = -1;
    for (i = 0; i < type_count; i ++) {
        ecs_id_t id = ids[i];

        /* If id has DontInherit flag don't inherit it, except for the name
         * and ChildOf pairs. The name is preserved so applications can lookup
         * the instantiated children by name. The ChildOf pair is replaced later
         * with the instance parent. */
        if ((id != ecs_pair(ecs_id(EcsIdentifier), EcsName)) &&
            ECS_PAIR_FIRST(id) != EcsChildOf) 
        {
            ecs_table_record_t *tr = &child_table->_->records[i];
            ecs_component_record_t *cr = tr->hdr.cr;
            if (cr->flags & EcsIdOnInstantiateDontInherit) {
                continue;
            }
        }

        /* Keep track of the element that creates the ChildOf relationship with
         * the prefab parent. We need to replace this element to make sure the
         * created children point to the instance and not the prefab */ 
        if (ECS_HAS_RELATION(id, EcsChildOf) && 
           (ECS_PAIR_SECOND(id) == (uint32_t)base)) {
            childof_base_index = diff.added.count;
        }

        /* If this is a pure override, make sure we have a concrete version of the
         * component. This relies on the fact that overrides always come after
         * concrete components in the table type so we can check the components
         * that have already been added to the child table type. */
        if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
            ecs_id_t concreteId = id & ~ECS_AUTO_OVERRIDE;
            int32_t insert_index = flecs_child_type_insert(
                &diff.added, component_data, concreteId);
            if (childof_base_index != -1 && insert_index != -1 &&
                insert_index <= childof_base_index)
            {
                childof_base_index ++;
            }
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

    /* Table must contain children of base */
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

    /* The instance is trying to instantiate from a base that is also
     * its parent. This would cause the hierarchy to instantiate itself,
     * which would cause infinite recursion. */
    const ecs_entity_t *children = ecs_table_entities(child_table);

#ifdef FLECS_DEBUG
    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t child = children[j + child_range.offset];        
        ecs_check(child != instance, ECS_INVALID_PARAMETER, 
            "cycle detected in IsA relationship");
    }
#else
    /* Bit of boilerplate to ensure that we don't get warnings about the
     * error label not being used. */
    ecs_check(true, ECS_INVALID_OPERATION, NULL);
#endif

    /* Attempt to reserve ids for children that have the same offset from
     * the instance as from the base prefab. This ensures stable ids for
     * instance children, even across networked applications. */
    ecs_instantiate_ctx_t ctx_cur = {base, instance};
    if (ctx) {
        ctx_cur = *ctx;
    }

    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t prefab_child = children[j + child_range.offset];
        child_ids[j] = flecs_instantiate_alloc_child_id(
            world, prefab_child, ctx_cur.root_prefab, ctx_cur.root_instance);
    }

    /* Create children */
    int32_t child_row;
    diff.added_flags |= EcsTableEdgeReparent;
    flecs_defer_begin(world, world->stages[0]);
    const ecs_entity_t *i_children = flecs_bulk_new(world, i_table, child_ids,
        child_range.count, true, EcsEventNoOnSet, &child_row, &diff);

    for (i = 0; i < diff.added.count; i ++) {
        void *src_ptr = component_data[i];
        int32_t column_index = ecs_table_type_to_column_index(i_table, i);
        if (!src_ptr || column_index == -1) {
            continue;
        }

        ecs_column_t *column = &i_table->data.columns[column_index];
        void *dst_ptr = ECS_ELEM(column->data, column->ti->size, child_row);
        flecs_type_info_copy(
            dst_ptr, src_ptr, child_range.count, column->ti);
    }

    for (i = 0; i < i_table->column_count; i ++) {
        ecs_id_t component = flecs_column_id(i_table, i);
        ecs_type_t set_type = { .array = &component, .count = 1 };
        flecs_notify_on_set_ids(
            world, i_table, child_row, child_range.count, &set_type, NULL);
    }
    flecs_defer_end(world, world->stages[0]);

    flecs_instantiate_sparse(
        world, &child_range, children, i_table, i_children, child_row, false);

    /* If prefab child table has children itself, recursively instantiate */
    for (j = 0; j < child_range.count; j ++) {
        ecs_entity_t child = children[j + child_range.offset];
        flecs_instantiate(world, child, i_children[j], &ctx_cur, depth + 1);
    }

    flecs_wfree_n(world, ecs_entity_t, child_range.count, child_ids);
error:
    return;    
}

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
                bool is_new = flecs_sparse_on_add_cr(world,
                    r->table, ECS_RECORD_TO_ROW(r->row), cur, true, &ptr);

                if (ti) {
                    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                    flecs_type_info_copy(ptr, base_ptr, 1, ti);
                }

                if (is_new) {
                    ecs_type_t added = { .array = &cur->id, .count = 1 };
                    flecs_emit(world, world, &(ecs_event_desc_t){
                        .event = EcsOnAdd,
                        .ids = &added,
                        .table = r->table,
                        .offset = ECS_RECORD_TO_ROW(r->row),
                        .count = 1,
                        .observable = world,
                        .flags = EcsEventNoOnSet
                    });
                }

                if (ti) {
                    flecs_notify_on_set(
                        world, r->table, ECS_RECORD_TO_ROW(r->row),
                        cur->id, true, ptr);
                }
            }
        }

        cur = cur->non_fragmenting.next;
    }
}

static void flecs_instantiate_override_dont_fragment(
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

    /* If base has non-fragmenting components, add to instance */
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
                    flecs_spawner_instantiate(world, ts, base, instance, ctx);
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
            if (flecs_table_cache_iter((ecs_table_cache_t*)cr, &it, EcsTableEmpty|EcsTableNotEmpty)) {
                const ecs_table_cache_elem_t *elem;
                while ((elem = flecs_table_cache_next(&it))) {
                    ecs_table_range_t range = {
                        elem->table,
                        0,
                        ecs_table_count(elem->table)
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

void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component,
    ecs_component_record_t *cr,
    int32_t recur_depth)
{
    ecs_check(recur_depth < ECS_MAX_RECURSION, ECS_INVALID_OPERATION,
        "cycle detected in IsA relationship");

    /* Table (and thus entity) does not have component, look for base */
    if (!(table->flags & EcsTableHasIsA)) {
        return NULL;
    }

    if (!(cr->flags & EcsIdOnInstantiateInherit)) {
        return NULL;
    }

    /* Exclude Name */
    if (component == ecs_pair(ecs_id(EcsIdentifier), EcsName)) {
        return NULL;
    }

    /* Table should always be in the table index for (IsA, *), otherwise the
     * HasBase flag should not have been set */
    const ecs_table_record_t *tr_isa = flecs_component_get_table(
        world->cr_isa_wildcard, table);
    ecs_check(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t i = tr_isa->index, end = tr_isa->count + tr_isa->index;
    void *ptr = NULL;

    do {
        ecs_id_t pair = ids[i ++];
        ecs_entity_t base = ecs_pair_second(world, pair);

        ecs_record_t *r = flecs_entities_get(world, base);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        table = r->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
        if (!tr) {
            if (cr->flags & EcsIdDontFragment) {
                ptr = flecs_component_sparse_get(world, cr, table, base);
            }

            if (!ptr) {
                ptr = flecs_get_base_component(world, table, component, cr, 
                    recur_depth + 1);
            }
        } else {
            if (cr->flags & EcsIdSparse) {
                return flecs_component_sparse_get(world, cr, table, base);
            } else if (tr->column != -1) {
                int32_t row = ECS_RECORD_TO_ROW(r->row);
                return flecs_table_get_component(table, tr->column, row).ptr;
            }
        }
    } while (!ptr && (i < end));

    return ptr;
error:
    return NULL;
}

#else

void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component,
    ecs_component_record_t *cr,
    int32_t recur_depth)
{
    (void)world;
    (void)table;
    (void)component;
    (void)cr;
    (void)recur_depth;
    return NULL;
}

void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx,
    int32_t depth)
{
    (void)world;
    (void)base;
    (void)instance;
    (void)ctx;
    (void)depth;
}

#endif
