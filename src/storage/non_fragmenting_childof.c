#include "../private_api.h"

static
void flecs_on_parent(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsParent *p = ecs_field(it, EcsParent, 0);

    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t parent = p[i].value;

        ecs_component_record_t *cr = flecs_components_get(world, 
            ecs_pair(EcsChildOf, parent));

        if (it->event == EcsOnSet) {
            ecs_check(cr != NULL, ECS_INVALID_OPERATION, 
                "entity set in Parent component must have OrderedChildren trait");
            ecs_check(cr->flags & EcsIdOrderedChildren, ECS_INVALID_OPERATION, 
                "entity set in Parent component must have OrderedChildren trait");
            ecs_check(parent != 0, ECS_INVALID_OPERATION, 
                "cannot set Parent component with 0 entity");
            ecs_check(ecs_is_alive(world, parent), ECS_INVALID_OPERATION, 
                "cannot set Parent component to entity that is not alive");
            flecs_ordered_entities_append(world, cr->pair, it->entities[i]);
        } else if (cr) {
            ecs_assert(it->event == EcsOnRemove, ECS_INTERNAL_ERROR, NULL);
            flecs_ordered_entities_remove(cr->pair, it->entities[i]);
        }
    error:
        continue;
    }
}

void flecs_bootstrap_parent_component(
    ecs_world_t *world)
{
    flecs_type_info_init(world, EcsParent, { 
        .ctor = flecs_default_ctor,
        .on_set = flecs_on_parent,
        .on_remove = flecs_on_parent
    });
}
