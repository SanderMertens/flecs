#include "../private_api.h"

void flecs_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_vec_init_t(
        &world->allocator, &cr->pair->ordered_children, ecs_entity_t, 0);
}

void flecs_ordered_children_fini(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{
    ecs_vec_fini_t(
        &world->allocator, &cr->pair->ordered_children, ecs_entity_t);
    ecs_map_fini(&cr->pair->children_tables);
}

void flecs_ordered_children_populate(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{    
    ecs_assert(ecs_vec_count(&cr->pair->ordered_children) == 0, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_IS_PAIR(cr->id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(cr->id) ==  EcsChildOf, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_iter_t it = ecs_each_id(world, cr->id);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            flecs_ordered_entities_append(world, cr, it.entities[i]);
        }
    }
}

void flecs_ordered_children_clear(
    ecs_component_record_t *cr)
{    
    ecs_vec_t *v = &cr->pair->ordered_children;
    ecs_assert(ECS_IS_PAIR(cr->id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(cr->id) ==  EcsChildOf, 
        ECS_INTERNAL_ERROR, NULL);

    if (!(cr->flags & EcsIdMarkedForDelete)) {
        ecs_assert(!ecs_map_count(&cr->pair->children_tables),
            ECS_UNSUPPORTED,
            "cannot remove OrderedChildren trait from parent that has "
            "children which use the Parent component");

        ecs_vec_clear(v);
    }
}

void flecs_ordered_entities_append(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_pair_record_t *pr = cr->pair;
    ecs_assert(pr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_append_t(
        &world->allocator, &pr->ordered_children, ecs_entity_t)[0] = e;

    if (cr->flags & EcsIdPrefabChildren) {
        /* Register index of prefab child so that it can be used to lookup 
         * corresponding instance child. */
        ecs_map_ensure(&world->prefab_child_indices, e)[0] = 
            flecs_ito(uint64_t, ecs_vec_count(&pr->ordered_children) - 1);
    } else {
        ecs_assert(
            !ecs_owns_id(world, ecs_pair_second(world, cr->id), EcsPrefab),
            ECS_INTERNAL_ERROR, NULL);
    }
}

void flecs_ordered_entities_remove(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t e)
{
    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_t *vec = &cr->pair->ordered_children;
    int32_t i, count = ecs_vec_count(vec);
    ecs_entity_t *entities = ecs_vec_first_t(vec, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        if (entities[i] == e) {
            ecs_vec_remove_ordered_t(vec, ecs_entity_t, i);
            if (cr->flags & EcsIdPrefabChildren) {
                ecs_map_remove(&world->prefab_child_indices, e);
            }
            break;
        }
    }
}

static
void flecs_ordered_entities_unparent_internal(
    ecs_world_t *world,
    const ecs_table_t *entities_table,
    const ecs_table_t *table,
    int32_t row,
    int32_t count)
{
    if (table && (table->flags & EcsTableHasOrderedChildren)) {
        ecs_component_record_t *cr = flecs_table_get_childof_cr(world, table);
        const ecs_entity_t *entities = ecs_table_entities(entities_table);
        int32_t i = row, end = row + count;
        for (; i < end; i ++) {
            ecs_entity_t e = entities[i];
            flecs_ordered_entities_remove(world, cr, e);
        }
    }
}

void flecs_ordered_children_reparent(
    ecs_world_t *world,
    const ecs_table_t *dst,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    flecs_ordered_entities_unparent_internal(world, dst, src, row, count);

    if (dst->flags & EcsTableHasOrderedChildren) {
        ecs_component_record_t *cr = flecs_table_get_childof_cr(world, dst);
        const ecs_entity_t *entities = ecs_table_entities(dst);
        int32_t i = row, end = row + count;
        for (; i < end; i ++) {
            ecs_entity_t e = entities[i];
            flecs_ordered_entities_append(world, cr, e);
        }
    }
}

void flecs_ordered_children_unparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    int32_t row,
    int32_t count)
{
    (void)world;
    flecs_ordered_entities_unparent_internal(world, src, src, row, count);
}

void flecs_ordered_children_reorder(
    ecs_world_t *world,
    ecs_entity_t parent,
    const ecs_entity_t *children,
    int32_t child_count)
{
    (void)world;

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));

    ecs_check(cr != NULL, ECS_INVALID_PARAMETER, 
        "ecs_set_child_order() is called for parent '%s' which does not have "
        "the OrderedChildren trait", 
            flecs_errstr(ecs_get_path(world, parent)));

    ecs_check(cr->flags & EcsIdOrderedChildren, ECS_INVALID_PARAMETER,
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
