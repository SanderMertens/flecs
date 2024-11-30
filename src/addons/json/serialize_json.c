/**
 * @file addons/json/serialize_json.c
 * @brief Serialize to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf)
{
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    if (!comp) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize to JSON, '%s' is not a component", path);
        ecs_os_free(path);
        return -1;
    }

    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (!ser) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
        ecs_os_free(path);
        return -1;
    }

    ecs_visitor_desc_t visitor_desc;
    flecs_json_init_visitor_desc(&visitor_desc, buf);

    return flecs_ser_array_w_type_data(world, ptr, count, &visitor_desc, comp, ser);
}

char* ecs_array_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr,
    int32_t count)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_array_to_json_buf(world, type, ptr, count, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf)
{
    return ecs_array_to_json_buf(world, type, ptr, 0, buf);
}

char* ecs_ptr_to_json(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    return ecs_array_to_json(world, type, ptr, 0);
}

#endif
