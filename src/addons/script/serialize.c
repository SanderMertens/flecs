/**
 * @file addons/script/serialize.c
 * @brief Serialize values to string.
 */

#include "flecs.h"
#include "../meta/meta.h"
#include <inttypes.h>

#ifdef FLECS_SCRIPT
#include "script.h"

static int flecs_ptr_to_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out,
    bool is_expr,
    bool precise)
{
    const EcsTypeSerializer *ser = ecs_get(
        world, type, EcsTypeSerializer);
    if (ser == NULL) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize value for type '%s'", path);
        ecs_os_free(path);
        return -1;
    }

    return flecs_meta_serialize(world, &ser->ops, ptr, buf_out,
        precise ? EcsMetaExprPrecise : is_expr ? EcsMetaExpr : EcsMetaStr);
}

int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    ecs_strbuf_t *buf_out)
{
    return flecs_ptr_to_buf(world, type, ptr, buf_out, true, false);
}

char* flecs_script_ptr_to_expr_precise(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    if (flecs_ptr_to_buf(world, type, ptr, &str, true, true)) {
        ecs_strbuf_reset(&str);
        return NULL;
    }

    return ecs_strbuf_get(&str);
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
    return flecs_ptr_to_buf(world, type, ptr, buf_out, false, false);
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
