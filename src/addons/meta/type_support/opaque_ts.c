/**
 * @file addons/meta/type_support/opaque_ts.c
 * @brief Opaque type support.
 */

#include "type_support.h"

#ifdef FLECS_META

static
void flecs_set_opaque_type(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsOpaque *serialize = ecs_field(it, EcsOpaque, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t elem_type = serialize[i].as_type;

        if (!elem_type) {
            ecs_err(
                "opaque type '%s' has no mapping type", ecs_get_name(world, e));
            continue;
        }

        const EcsComponent *comp = ecs_get(world, e, EcsComponent);
        if (!comp || !comp->size || !comp->alignment) {
            ecs_err(
                "opaque type '%s' has no size/alignment, register as component first",
                ecs_get_name(world, e));
            continue;
        }

        if (flecs_init_type(world, e, EcsOpaqueType, comp->size, comp->alignment)) {
            continue;
        }
    }
}

ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc)
{
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc->type.as_type != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set_ptr(world, t, EcsOpaque, &desc->type);

    flecs_resume_readonly(world, &rs);

    return t;
}

void flecs_meta_opaque_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsOpaque),
            .name = "opaque", .symbol = "EcsOpaque",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsOpaque),
        .type.alignment = ECS_ALIGNOF(EcsOpaque)
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .parent = EcsFlecsInternals }),
        .query.terms[0] = { .id = ecs_id(EcsOpaque) },
        .events = {EcsOnSet},
        .callback = flecs_set_opaque_type
    });

}

#endif
