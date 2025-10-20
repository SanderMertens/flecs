/**
 * @file instantiate.c
 * @brief Functions for instantiating prefabs (IsA relationship).
 */

#include "private_api.h"

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
        /* Slot is registered for other prefab, travel hierarchy
         * upwards to find instance that inherits from slot_of */
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

                /* The 'slot' variable is currently pointing to a child (or 
                 * grandchild) of the current base. Find the original slot by
                 * looking it up under the prefab it was registered. */
                if (depth == 0) {
                    /* If the current instance is an instance of slot_of, just
                     * lookup the slot by name, which is faster than having to
                     * create a relative path. */
                    slot = ecs_lookup_child(world, slot_of, name);
                } else {
                    /* If the slot is more than one level away from the slot_of
                     * parent, use a relative path to find the slot */
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

static
void flecs_instantiate_children(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    ecs_table_t *child_table,
    const ecs_instantiate_ctx_t *ctx)
{
    if (!ecs_table_count(child_table)) {
        return;
    }

    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = &child_table->data;

    ecs_entity_t slot_of = 0;
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

        /* If child is a slot, keep track of which parent to add it to, but
         * don't add slot relationship to child of instance. If this is a child
         * of a prefab, keep the SlotOf relationship intact. */
        if (!(table->flags & EcsTableIsPrefab)) {
            if (ECS_IS_PAIR(id) && ECS_PAIR_FIRST(id) == EcsSlotOf) {
                ecs_assert(slot_of == 0, ECS_INTERNAL_ERROR, NULL);
                slot_of = ecs_pair_second(world, id);
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
            flecs_child_type_insert(&diff.added, component_data, concreteId);
            continue;
        }

        int32_t storage_index = ecs_table_type_to_column_index(child_table, i);
        if (storage_index != -1) {
            component_data[diff.added.count] = 
                child_data->columns[storage_index].data;
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
    int32_t child_count = ecs_table_count(child_table);
    ecs_entity_t *child_ids = flecs_walloc_n(world, ecs_entity_t, child_count);

    ecs_table_t *i_table = NULL;

    /* Replace ChildOf element in the component array with instance id */
    diff.added.array[childof_base_index] = ecs_pair(EcsChildOf, instance);

    /* Find or create table */
    i_table = flecs_table_find_or_create(world, &diff.added);

    ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(i_table->type.count == diff.added.count,
        ECS_INTERNAL_ERROR, NULL);

    /* The instance is trying to instantiate from a base that is also
     * its parent. This would cause the hierarchy to instantiate itself
     * which would cause infinite recursion. */
    const ecs_entity_t *children = ecs_table_entities(child_table);

#ifdef FLECS_DEBUG
    for (j = 0; j < child_count; j ++) {
        ecs_entity_t child = children[j];        
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

    for (j = 0; j < child_count; j ++) {
        if ((uint32_t)children[j] < (uint32_t)ctx_cur.root_prefab) {
            /* Child id is smaller than root prefab id, can't use offset */
            child_ids[j] = flecs_new_id(world);
            continue;
        }

        /* Get prefab offset, ignore lifecycle generation count */
        ecs_entity_t prefab_offset =
            (uint32_t)children[j] - (uint32_t)ctx_cur.root_prefab;
        ecs_assert(prefab_offset != 0, ECS_INTERNAL_ERROR, NULL);

        /* First check if any entity with the desired id exists */
        ecs_entity_t instance_child = (uint32_t)ctx_cur.root_instance + prefab_offset;
        ecs_entity_t alive_id = flecs_entities_get_alive(world, instance_child);
        if (alive_id && flecs_entities_is_alive(world, alive_id)) {
            /* Alive entity with requested id exists, can't use offset id */
            child_ids[j] = flecs_new_id(world);
            continue;
        }

        /* Id is not in use. Make it alive & match the generation of the instance. */
        instance_child = ctx_cur.root_instance + prefab_offset;
        flecs_entities_make_alive(world, instance_child);
        flecs_entities_ensure(world, instance_child);
        ecs_assert(ecs_is_alive(world, instance_child), ECS_INTERNAL_ERROR, NULL);
        child_ids[j] = instance_child;
    }

    /* Create children */
    int32_t child_row;
    const ecs_entity_t *i_children = flecs_bulk_new(world, i_table, child_ids,
        &diff.added, child_count, component_data, false, &child_row, &diff);

    /* If children are slots, add slot relationships to parent */
    if (slot_of) {
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            ecs_entity_t i_child = i_children[j];
            flecs_instantiate_slot(world, base, instance, slot_of,
                child, i_child);
        }
    }

    /* If prefab child table has children itself, recursively instantiate */
    for (j = 0; j < child_count; j ++) {
        ecs_entity_t child = children[j];
        flecs_instantiate(world, child, i_children[j], &ctx_cur);
    }

    flecs_wfree_n(world, ecs_entity_t, child_count, child_ids);
error:
    return;    
}

static
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
                    if (ti->hooks.copy) {
                        ti->hooks.copy(ptr, base_ptr, 1, ti);
                    } else {
                        ecs_os_memcpy(ptr, base_ptr, ti->size);
                    }
                }
            }
        }

        cur = cur->non_fragmenting.next;
    }
}

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

        ecs_component_record_t *cr = flecs_components_get(world, id);
        if (!cr || !(cr->flags & EcsIdDontFragment)) {
            continue;
        }

        ecs_add_id(world, instance, id);
    }
}

void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx)
{
    ecs_record_t *record = flecs_entities_get_any(world, base);
    ecs_table_t *base_table = record->table;
    ecs_assert(base_table != NULL, ECS_INTERNAL_ERROR, NULL);

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
    ecs_table_cache_iter_t it;
    if (cr && flecs_table_cache_all_iter((ecs_table_cache_t*)cr, &it)) {
        ecs_os_perf_trace_push("flecs.instantiate");
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            flecs_instantiate_children(
                world, base, instance, tr->hdr.table, ctx);
        }
        ecs_os_perf_trace_pop("flecs.instantiate");

        if (cr->flags & EcsIdOrderedChildren) {
            ecs_component_record_t *icr = flecs_components_get(world, ecs_childof(instance));
            /* If base has children, instance must now have children */
            ecs_assert(icr != NULL, ECS_INTERNAL_ERROR, NULL);
            flecs_ordered_children_populate(world, icr);
        }
    }
}
