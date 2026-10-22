/**
 * @file storage/ordered_children.c
 * @brief Ordered children storage.
 */

#include "../private_api.h"

static void flecs_ordered_children_set_table_flags(
    ecs_component_record_t *cr,
    bool value)
{
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&cr->cache, &it, EcsTableEmpty|EcsTableNotEmpty)) {
        const ecs_table_cache_elem_t *elem;
        while ((elem = flecs_table_cache_next(&it))) {
            ECS_BIT_COND(elem->table->flags, EcsTableHasOrderedChildren, value);
        }
    }
}

void flecs_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_assert(ECS_IS_PAIR(cr->id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(cr->id) == EcsChildOf, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(cr->flags & EcsIdOrderedChildren), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vec_count(&cr->pair->ordered_children) == 0,
        ECS_INTERNAL_ERROR, NULL);

    cr->flags |= EcsIdOrderedChildren;
    flecs_ordered_children_set_table_flags(cr, true);

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&cr->cache, &it, EcsTableNotEmpty)) {
        const ecs_table_cache_elem_t *elem;
        while ((elem = flecs_table_cache_next(&it))) {
            const ecs_entity_t *entities = ecs_table_entities(elem->table);
            int32_t i, count = ecs_table_count(elem->table);
            for (i = 0; i < count; i ++) {
                flecs_ordered_entities_append(world, cr, entities[i]);
            }
        }
    }
}

void flecs_ordered_children_fini(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_vec_fini_t(
        &world->allocator, &cr->pair->ordered_children, ecs_entity_t);
}

void flecs_ordered_children_clear(
    ecs_component_record_t *cr)
{
    ecs_assert(cr->flags & EcsIdOrderedChildren, ECS_INTERNAL_ERROR, NULL);

    if (!(cr->flags & EcsIdMarkedForDelete)) {
        ecs_assert(!cr->pair->non_fragmenting_child_count,
            ECS_UNSUPPORTED,
            "cannot remove OrderedChildren trait from parent that has "
            "children which use the Parent component");

        ecs_vec_clear(&cr->pair->ordered_children);
    }

    cr->flags &= ~EcsIdOrderedChildren;
    flecs_ordered_children_set_table_flags(cr, false);
}

void flecs_ordered_entities_append(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e)
{
    ecs_vec_t *vec = &cr->pair->ordered_children;
    ecs_vec_init_if_t(vec, ecs_entity_t);
    ecs_vec_append_t(&world->allocator, vec, ecs_entity_t)[0] = e;

    if (cr->flags & EcsIdPrefabChildren) {
        /* Register index of prefab child so that it can be used to lookup
         * corresponding instance child. */
        ecs_map_ensure(&world->prefab_child_indices, e)[0] =
            flecs_ito(uint64_t, ecs_vec_count(vec) - 1);
#ifdef FLECS_PREFAB
    } else {
        ecs_assert(
            !ecs_owns_id(world, ecs_pair_second(world, cr->id), EcsPrefab),
            ECS_INTERNAL_ERROR, NULL);
#endif
    }
}

void flecs_ordered_entities_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e)
{
    ecs_vec_t *vec = &cr->pair->ordered_children;
    int32_t i, count = ecs_vec_count(vec);
    ecs_entity_t *entities = ecs_vec_first_t(vec, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        if (entities[i] == e) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_remove_ordered_t(vec, ecs_entity_t, i);
    if (cr->flags & EcsIdPrefabChildren) {
        ecs_map_remove(&world->prefab_child_indices, e);
    }
}

void flecs_ordered_children_set_prefab(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    if (cr->flags & EcsIdPrefabChildren) {
        return;
    }

    cr->flags |= EcsIdPrefabChildren;

    ecs_vec_t *vec = &cr->pair->ordered_children;
    int32_t i, count = ecs_vec_count(vec);
    ecs_entity_t *entities = ecs_vec_first_t(vec, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_map_ensure(&world->prefab_child_indices, entities[i])[0] =
            flecs_ito(uint64_t, i);
    }
}

static void flecs_ordered_entities_remove_range(
    ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_entity_t *entities,
    int32_t count)
{
    ecs_component_record_t *cr = flecs_table_get_childof_cr(world, table);
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_ordered_entities_remove(world, cr, entities[i]);
    }
}

void flecs_ordered_children_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    const ecs_entity_t *entities = &ecs_table_entities(dst)[row];

    if (src && (src->flags & EcsTableHasOrderedChildren)) {
        flecs_ordered_entities_remove_range(world, src, entities, count);
    }

    if (dst->flags & EcsTableHasOrderedChildren) {
        ecs_component_record_t *cr = flecs_table_get_childof_cr(world, dst);
        int32_t i;
        for (i = 0; i < count; i ++) {
            flecs_ordered_entities_append(world, cr, entities[i]);
        }
    }
}

void flecs_ordered_children_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    ecs_assert(src->flags & EcsTableHasOrderedChildren,
        ECS_INTERNAL_ERROR, NULL);

    flecs_ordered_entities_remove_range(world, src,
        &ecs_table_entities(src)[row], count);
}

void flecs_ordered_children_reorder(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count)
{
    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));

    ecs_check(cr != NULL && (cr->flags & EcsIdOrderedChildren),
        ECS_INVALID_PARAMETER,
        "ecs_set_child_order() is called for parent '%s' which does not have "
        "the OrderedChildren trait",
            flecs_errstr(ecs_get_path(world, parent)));

    ecs_vec_t *vec = &cr->pair->ordered_children;
    ecs_entity_t *parent_children = ecs_vec_first_t(vec, ecs_entity_t);
    int32_t parent_child_count = ecs_vec_count(vec);
    ecs_check(parent_child_count == child_count, ECS_INVALID_PARAMETER,
        "children provided to set_child_order() for parent '%s' do not match "
        "existing children",
            flecs_errstr(ecs_get_path(world, parent)));

    (void)parent_child_count;

    if (parent_children == children) {
        return; /* Application is passing the existing children array. */
    }

    ecs_check(children != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(parent_children != NULL, ECS_INTERNAL_ERROR, NULL);

    #ifdef FLECS_DEBUG
    /* Make sure that the provided child ids equal the existing children */
    int i, j;
    for (i = 0; i < child_count; i ++) {
        ecs_entity_t child = parent_children[i];
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child_arg = children[j];
            if (child == child_arg) {
                break;
            }
        }

        if (j == child_count) {
            ecs_throw(ECS_INVALID_PARAMETER,
                "children provided to set_child_order() for parent '%s' do not "
                "match existing children (child '%s' is missing in provided "
                "children vector)",
                    flecs_errstr(ecs_get_path(world, parent)),
                    flecs_errstr_2(ecs_get_path(world, child)));
        }
    }
    #endif

    /* The actual operation. */
    ecs_os_memcpy_n(parent_children, children, ecs_entity_t, child_count);
error:
    return;
}
