/**
 * @file addons/script/serialize.c
 * @brief Serialize values to string.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../serialize/serialize.h"
#include "script.h"

int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }

    ecs_visitor_desc_t visitor_desc;
    flecs_expr_init_visitor_desc(&visitor_desc, buf_out);
    if (flecs_ser_type(world, &ser->ops, ptr, &visitor_desc)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

char* ecs_ptr_to_expr(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_ptr_to_expr_buf(world, type, ptr, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

int ecs_ptr_to_str_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        goto error;
    }

    ecs_visitor_desc_t visitor_desc;
    flecs_expr_string_init_visitor_desc(&visitor_desc, buf_out);
    if (flecs_ser_type(world, &ser->ops, ptr, &visitor_desc)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

char* ecs_ptr_to_str(
    const ecs_world_t *world, 
    ecs_entity_t type, 
    const void* ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;

    if (ecs_ptr_to_str_buf(world, type, ptr, &str) != 0) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
}

#endif
