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
}

void flecs_ordered_children_populate(
    ecs_world_t *world,
    ecs_component_record_t *cr)
{    
    ecs_vec_t *v = &cr->pair->ordered_children;
    ecs_assert(ecs_vec_count(v) == 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_IS_PAIR(cr->id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(cr->id) ==  EcsChildOf, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_iter_t it = ecs_each_id(world, cr->id);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_vec_append_t(
                &world->allocator, v, ecs_entity_t)[0] = it.entities[i];
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

    ecs_vec_clear(v);
}

static
void flecs_ordered_entities_append(
    ecs_world_t *world,
    ecs_pair_record_t *pair,
    ecs_entity_t e)
{
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_append_t(
        &world->allocator, &pair->ordered_children, ecs_entity_t)[0] = e;
}

static
void flecs_ordered_entities_remove(
    ecs_pair_record_t *pair,
    ecs_entity_t e)
{
    ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vec_t *vec = &pair->ordered_children;
    int32_t i, count = ecs_vec_count(vec);
    ecs_entity_t *entities = ecs_vec_first_t(vec, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        if (entities[i] == e) {
            ecs_vec_remove_ordered_t(vec, ecs_entity_t, i);
        }
    }
}

static
void flecs_ordered_entities_unparent_internal(
    const ecs_table_t *table,
    const ecs_table_t *entities_table,
    int32_t row,
    int32_t count)
{
    if (table && (table->flags & EcsTableHasOrderedChildren)) {
        ecs_pair_record_t *pair = table->_->childof_r;
        const ecs_entity_t *entities = ecs_table_entities(entities_table);
        int32_t i = row, end = row + count;
        for (; i < end; i ++) {
            ecs_entity_t e = entities[i];
            flecs_ordered_entities_remove(pair, e);
        }
    }
}

void flecs_ordered_children_reparent(
    ecs_world_t *world,
    const ecs_table_t *src,
    const ecs_table_t *dst,
    int32_t row,
    int32_t count)
{
    flecs_ordered_entities_unparent_internal(src, dst, row, count);

    if (dst->flags & EcsTableHasOrderedChildren) {
        ecs_pair_record_t *pair = dst->_->childof_r;
        const ecs_entity_t *entities = ecs_table_entities(dst);
        int32_t i = row, end = row + count;
        for (; i < end; i ++) {
            ecs_entity_t e = entities[i];
            flecs_ordered_entities_append(world, pair, e);
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
    flecs_ordered_entities_unparent_internal(src, src, row, count);
}

void flecs_ordered_children_reorder(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_entity_t *children,
    int32_t child_count)
{
    (void)world;

    ecs_check(cr != NULL, ECS_INVALID_PARAMETER, 
        "ecs_set_child_order is only allowed for parents with the "
        "OrderedChildren trait");

    ecs_check(cr->flags & EcsIdOrderedChildren, ECS_INVALID_PARAMETER, 
        "ecs_set_child_order is only allowed for parents with the "
        "OrderedChildren trait");

    ecs_vec_t *vec = &cr->pair->ordered_children;
    ecs_entity_t *parent_children = ecs_vec_first_t(vec, ecs_entity_t);
    int32_t parent_child_count = ecs_vec_count(vec);
    ecs_check(parent_child_count == child_count, ECS_INVALID_PARAMETER,
        "children provided to set_child_order must match existing children");
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
            char *child_path = ecs_get_path(world, child);
            ecs_throw(ECS_INVALID_PARAMETER, 
                "children provided to set_child_order must match existing "
                "children (child '%s' is missing in provided children vector)",
                child_path);
            ecs_os_free(child_path);
        }
    }
    #endif

    /* The actual operation. */
    ecs_os_memcpy_n(parent_children, children, ecs_entity_t, child_count);
error:
    return;
}
