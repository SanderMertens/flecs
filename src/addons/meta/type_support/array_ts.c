/**
 * @file addons/meta/type_support/array_ts.c
 * @brief Array type support.
 */

#include "type_support.h"

#ifdef FLECS_META

static
void flecs_set_array(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsArray *array = ecs_field(it, EcsArray, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t elem_type = array[i].type;
        int32_t elem_count = array[i].count;

        if (!elem_type) {
            ecs_err("array '%s' has no element type", ecs_get_name(world, e));
            continue;
        }

        if (!elem_count) {
            ecs_err("array '%s' has size 0", ecs_get_name(world, e));
            continue;
        }

        const EcsComponent *elem_ptr = ecs_get(world, elem_type, EcsComponent);
        if (flecs_init_type(world, e, EcsArrayType, 
            elem_ptr->size * elem_count, elem_ptr->alignment)) 
        {
            continue;
        }
    }
}

static
void flecs_set_vector(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsVector *array = ecs_field(it, EcsVector, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t elem_type = array[i].type;

        if (!elem_type) {
            ecs_err("vector '%s' has no element type", ecs_get_name(world, e));
            continue;
        }

        if (flecs_init_type_t(world, e, EcsVectorType, ecs_vec_t)) {
            continue;
        }
    }
}

ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsArray, {
        .type = desc->type,
        .count = desc->count
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsVector, {
        .type = desc->type
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

void flecs_meta_array_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsArray),
            .name = "array", .symbol = "EcsArray",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsArray),
        .type.alignment = ECS_ALIGNOF(EcsArray)
    });

    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsVector),
            .name = "vector", .symbol = "EcsVector",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsVector),
        .type.alignment = ECS_ALIGNOF(EcsVector)
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsArray) },
        .events = {EcsOnSet},
        .callback = flecs_set_array
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsVector) },
        .events = {EcsOnSet},
        .callback = flecs_set_vector
    });
}

#endif
