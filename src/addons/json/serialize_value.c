/**
 * @file addons/json/serialize_value.c
 * @brief Serialize value to JSON.
 */

#include "json.h"
#include "../meta/meta.h"

#ifdef FLECS_JSON

int flecs_json_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *ops,
    const void *ptr,
    ecs_strbuf_t *str)
{
    return flecs_meta_serialize(world, ops, ptr, str, EcsMetaJson);
}

static int flecs_array_to_json_buf_w_type_data(
    const ecs_world_t *world,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf,
    const EcsComponent *comp,
    const EcsTypeSerializer *ser)
{
    if (count) {
        ecs_size_t size = comp->size;

        flecs_json_array_push(buf);

        do {
            ecs_strbuf_list_next(buf);
            if (flecs_json_ser_type(world, &ser->ops, ptr, buf)) {
                return -1;
            }

            ptr = ECS_OFFSET(ptr, size);
        } while (-- count);

        flecs_json_array_pop(buf);
    } else {
        if (flecs_json_ser_type(world, &ser->ops, ptr, buf)) {
            return -1;
        }
    }

    return 0;
}

int ecs_array_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *ptr,
    int32_t count,
    ecs_strbuf_t *buf)
{
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    
    const EcsComponent *comp = ecs_get(world, type, EcsComponent);
    const EcsTypeSerializer *ser = comp ?
        ecs_get(world, type, EcsTypeSerializer) : NULL;
    if (!comp || !ser) {
        char *path = ecs_get_path(world, type);
        ecs_err("cannot serialize to JSON, '%s' %s", path,
            !comp ? "is not a component" : "has no reflection data");
        ecs_os_free(path);
        return -1;
    }

    return flecs_array_to_json_buf_w_type_data(world, ptr, count, buf, comp, ser);
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
