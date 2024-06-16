/**
 * @file json/serialize_fields.c
 * @brief Serialize fields.
 */

#include "json.h"

#ifdef FLECS_JSON

int flecs_json_serialize_iter_result_fields(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    int32_t i,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    int32_t f, field_count = it->field_count;
    if (!field_count) {
        return 0;
    }

    flecs_json_memberl(buf, "fields");
    flecs_json_array_push(buf);

    for (f = 0; f < field_count; f ++) {
        flecs_json_next(buf);

        ecs_flags16_t field_bit = flecs_ito(uint16_t, 1 << f);;
        if (!(it->set_fields & field_bit)) {
            flecs_json_null(buf);
            continue;
        }

        flecs_json_object_push(buf);

        if (it->query) {
            if (ecs_id_is_wildcard(it->query->ids[f])) {
                flecs_json_memberl(buf, "id");
                flecs_json_id(buf, world, it->ids[f]);
            }
        }

        if (it->sources[f]) {
            flecs_json_memberl(buf, "source");
            flecs_json_path(buf, world, it->sources[f]);
        }


        ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc)) 
        {
            goto next;
        }

        if (!it->ptrs[f]) {
            goto next; /* Should not happen, but don't panic on it */
        }

        flecs_json_memberl(buf, "data");

        void *ptr = it->ptrs[f];
        if (!it->sources[f]) {
            ptr = ECS_ELEM(ptr, it->sizes[f], i);
        }
        if (flecs_json_ser_type(world, &value_ctx->ser->ops, ptr, buf) != 0) {
            return -1;
        }

    next:
        flecs_json_object_pop(buf);
    }

    flecs_json_array_pop(buf);

    return 0;
}

#endif
