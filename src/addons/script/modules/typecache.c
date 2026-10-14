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

static bool flecs_script_typecache_match(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t elem_type,
    int32_t count)
{
    if (count) {
        const EcsArray *a = ecs_get(world, type, EcsArray);
        return a && a->type == elem_type && a->count == count;
    }

    const EcsVector *v = ecs_get(world, type, EcsVector);
    return v && v->type == elem_type;
}

static ecs_entity_t flecs_script_typecache_type(
    ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count)
{
    ecs_assert(elem_type != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t typecache = flecs_script_typecache_get(world);

    char *id_str = flecs_asprintf("#%u_%u", 
        (uint32_t)elem_type, (uint32_t)(elem_type >> 32));
    const char *elem_name = ecs_get_name(world, elem_type);
    if (!elem_name || strchr(elem_name, '.')) {
        elem_name = id_str;
    }

    char *name = NULL;
    do {
        ecs_os_free(name);
        name = count
            ? flecs_asprintf("array<%s,%d>", elem_name, count)
            : flecs_asprintf("vector<%s>", elem_name);

        ecs_entity_t existing = ecs_lookup_child(world, typecache, name);
        if (!existing) {
            break;
        }

        if (flecs_script_typecache_match(world, existing, elem_type, count)) {
            ecs_os_free(name);
            ecs_os_free(id_str);
            return existing;
        }

        if (elem_name == id_str) {
            ecs_os_free(name);
            name = NULL;
            break;
        }

        elem_name = id_str;
    } while (true);

    ecs_entity_t entity = ecs_entity(world, { 
        .name = name, .parent = typecache });
    ecs_os_free(name);
    ecs_os_free(id_str);

    if (count) {
        return ecs_array(world, {
            .entity = entity, .type = elem_type, .count = count });
    }

    return ecs_vector(world, { .entity = entity, .type = elem_type });
}

ecs_entity_t flecs_script_array_type(
    ecs_world_t *world,
    ecs_entity_t elem_type,
    int32_t count)
{
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_script_typecache_type(world, elem_type, count);
}

ecs_entity_t flecs_script_vector_type(
    ecs_world_t *world,
    ecs_entity_t elem_type)
{
    return flecs_script_typecache_type(world, elem_type, 0);
}

#endif
