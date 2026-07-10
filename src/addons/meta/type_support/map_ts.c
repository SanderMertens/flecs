/**
 * @file addons/meta/type_support/map_ts.c
 * @brief Map type support.
 */

#include "type_support.h"

#ifdef FLECS_META

static
bool flecs_map_valid_key_type(
    ecs_world_t *world,
    ecs_entity_t key_type)
{
    const EcsType *kt = ecs_get(world, key_type, EcsType);
    if (kt && (kt->kind == EcsEnumType || kt->kind == EcsBitmaskType)) {
        return true;
    }

    const EcsPrimitive *p = ecs_get(world, key_type, EcsPrimitive);
    if (!p) {
        return false;
    }

    switch(p->kind) {
    case EcsBool:
    case EcsChar:
    case EcsByte:
    case EcsU8:
    case EcsU16:
    case EcsU32:
    case EcsU64:
    case EcsI8:
    case EcsI16:
    case EcsI32:
    case EcsI64:
    case EcsUPtr:
    case EcsIPtr:
    case EcsEntity:
    case EcsId:
        return true;
    case EcsF32:
    case EcsF64:
    case EcsString:
        return false;
    default:
        return false;
    }
}

static
void flecs_set_map(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    EcsMap *map = ecs_field(it, EcsMap, 0);

    int i, count = it->count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_entity_t key_type = map[i].key_type;
        ecs_entity_t value_type = map[i].type;

        if (!key_type) {
            ecs_err("map '%s' has no key type", ecs_get_name(world, e));
            continue;
        }

        if (!value_type) {
            ecs_err("map '%s' has no value type", ecs_get_name(world, e));
            continue;
        }

        if (!ecs_is_alive(world, key_type)) {
            ecs_err("map '%s' has invalid key type", ecs_get_name(world, e));
            continue;
        }

        if (!flecs_map_valid_key_type(world, key_type)) {
            ecs_err("map '%s' key type must be a primitive (other than "
                "string, f32 or f64), enum or bitmask type",
                ecs_get_name(world, e));
            continue;
        }

        if (!ecs_is_alive(world, value_type)) {
            ecs_err("map '%s' has invalid value type", ecs_get_name(world, e));
            continue;
        }

        if (!ecs_get_type_info(world, value_type)) {
            ecs_err("map '%s' value type has no type info",
                ecs_get_name(world, e));
            continue;
        }

        if (flecs_init_type_t(world, e, EcsMapType, ecs_map_t)) {
            continue;
        }
    }
}

ecs_entity_t ecs_map_type_init(
    ecs_world_t *world,
    const ecs_map_desc_t *desc)
{
    ecs_suspend_readonly_state_t rs;
    world = flecs_suspend_readonly(world, &rs);

    ecs_entity_t t = desc->entity;
    if (!t) {
        t = ecs_new_low_id(world);
    }

    ecs_set(world, t, EcsMap, {
        .key_type = desc->key_type,
        .type = desc->type
    });

    flecs_resume_readonly(world, &rs);

    return t;
}

void flecs_meta_map_init(
    ecs_world_t *world)
{
    ecs_component(world, {
        .entity = ecs_entity(world, { .id = ecs_id(EcsMap),
            .name = "map", .symbol = "EcsMap",
            .add = ecs_ids(ecs_pair(EcsOnInstantiate, EcsDontInherit))
        }),
        .type.size = sizeof(EcsMap),
        .type.alignment = ECS_ALIGNOF(EcsMap)
    });

    ecs_observer(world, {
        .query.terms[0] = { .id = ecs_id(EcsMap) },
        .events = {EcsOnSet},
        .callback = flecs_set_map,
        .global_observer = true
    });
}

#endif
