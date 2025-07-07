#include "../private_api.h"

static
void flecs_add_non_fragmenting_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t entity)
{
    ecs_component_record_t *cr = flecs_components_get(world, 
        ecs_pair(EcsChildOf, parent));

    ecs_check(cr != NULL, ECS_INVALID_OPERATION, 
        "entity set in Parent component must have OrderedChildren trait");
    ecs_check(cr->flags & EcsIdOrderedChildren, ECS_INVALID_OPERATION, 
        "entity set in Parent component must have OrderedChildren trait");
    ecs_check(parent != 0, ECS_INVALID_OPERATION, 
        "cannot set Parent component with 0 entity");
    ecs_check(ecs_is_alive(world, parent), ECS_INVALID_OPERATION, 
        "cannot set Parent component to entity that is not alive");
    flecs_ordered_entities_append(world, cr->pair, entity);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_init_if(&cr->pair->non_fragmenting_tables, &world->allocator);
    ecs_non_fragmenting_record_t *elem = (ecs_non_fragmenting_record_t*)
        ecs_map_ensure(&cr->pair->non_fragmenting_tables, r->table->id);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Encode id of first entity in table + the total number of entities in the
     * table for this parent in a single uint64 so everything fits in a map
     * element without having to allocate. */
    if (!elem->first_entity) {
        elem->first_entity = (uint32_t)entity;
        elem->count = 1;
    } else {
        elem->count ++;
    }

error:
    return;
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

    flecs_ordered_entities_remove(cr->pair, entity);

    ecs_record_t *r = flecs_entities_get(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = r->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_non_fragmenting_record_t *elem = (ecs_non_fragmenting_record_t*)
        ecs_map_get(&cr->pair->non_fragmenting_tables, r->table->id);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->count --;
    if (!elem->count) {
        ecs_map_remove(&cr->pair->non_fragmenting_tables, r->table->id);
    } else {
        if (elem->first_entity == (uint32_t)entity) {
            EcsParent *parents = ecs_table_get(world, table, EcsParent, 0);
            int32_t i, row = ECS_RECORD_TO_ROW(r->row);
            int32_t count = ecs_table_count(table);

            for (i = 0; i < count; i ++) {
                row --;
                if (row < 0) {
                    row = count - 1;
                }

                if (parents[row].value == parent) {
                    elem->first_entity = 
                        (uint32_t)ecs_table_entities(table)[row];
                    break;
                }
            }

            /* Table should contain other children for parent */
            ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);
        }
    }
}

static
void flecs_on_parent(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsParent *p = ecs_field(it, EcsParent, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t parent = p[i].value;

        if (it->event == EcsOnSet) {
            flecs_add_non_fragmenting_child(world, parent, it->entities[i]);
        } else {
            ecs_assert(it->event == EcsOnRemove, ECS_INTERNAL_ERROR, NULL);
            flecs_remove_non_fragmenting_child(world, parent, it->entities[i]);
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

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        flecs_remove_non_fragmenting_child(
            world, old[i].value, it->entities[i]);
        flecs_add_non_fragmenting_child(
            world, new[i].value, it->entities[i]);
    }
}

void flecs_bootstrap_parent_component(
    ecs_world_t *world)
{
    flecs_type_info_init(world, EcsParent, { 
        .ctor = flecs_default_ctor,
        .on_remove = flecs_on_parent,
        .on_replace = flecs_on_replace_parent
    });
}
