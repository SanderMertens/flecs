/**
 * @file addons/script/modules/typecache.c
 * @brief Cache with anonymous types used by expressions (e.g. vector<i32>).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static ecs_entity_t flecs_script_typecache_get(
    ecs_world_t *world)
{
    ecs_entity_t result = ecs_lookup(world, "flecs.script.typecache");
    if (!result) {
        ecs_entity_t prev_scope = ecs_set_scope(world, 0);
        result = ecs_entity(world, { .name = "flecs.script.typecache" });
        ecs_add_id(world, result, EcsModule);
        ecs_set_scope(world, prev_scope);
    }
    return result;
}

ecs_entity_t flecs_script_array_type(
    ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count)
{
    ecs_assert(elem_type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t typecache = flecs_script_typecache_get(world);

    char *name;
    const char *elem_name = ecs_get_name(world, elem_type);
    if (elem_name && !strchr(elem_name, '.')) {
        name = flecs_asprintf("array<%s,%d>", elem_name, count);
    } else {
        name = flecs_asprintf("array<#%u,%d>", (uint32_t)elem_type, count);
    }

    ecs_entity_t existing = ecs_lookup_child(world, typecache, name);
    if (existing) {
        const EcsArray *a = ecs_get(world, existing, EcsArray);
        if (a && a->type == elem_type && a->count == count) {
            ecs_os_free(name);
            return existing;
        }
        ecs_os_free(name);
        name = NULL;
    }

    ecs_entity_t result = ecs_array(world, {
        .entity = ecs_entity(world, { .name = name, .parent = typecache }),
        .type = elem_type,
        .count = count
    });

    ecs_os_free(name);
    return result;
}

ecs_entity_t flecs_script_vector_type(
    ecs_world_t *world,
    ecs_entity_t elem_type)
{
    ecs_assert(elem_type != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t typecache = flecs_script_typecache_get(world);

    char *name;
    const char *elem_name = ecs_get_name(world, elem_type);
    if (elem_name && !strchr(elem_name, '.')) {
        name = flecs_asprintf("vector<%s>", elem_name);
    } else {
        name = flecs_asprintf("vector<#%u>", (uint32_t)elem_type);
    }

    ecs_entity_t existing = ecs_lookup_child(world, typecache, name);
    if (existing) {
        const EcsVector *v = ecs_get(world, existing, EcsVector);
        if (v && v->type == elem_type) {
            ecs_os_free(name);
            return existing;
        }
        ecs_os_free(name);
        name = NULL;
    }

    ecs_entity_t result = ecs_vector(world, {
        .entity = ecs_entity(world, { .name = name, .parent = typecache }),
        .type = elem_type
    });

    ecs_os_free(name);
    return result;
}

#endif
