/**
 * @file addons/json/serialize_iter_result_query.c
 * @brief Serialize matched query data of result.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_serialize_iter_result_is_set(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    if (!(it->flags & EcsIterHasCondSet)) {
        return false;
    }

    flecs_json_memberl(buf, "is_set");
    flecs_json_array_push(buf);

    int32_t i, count = it->field_count;
    for (i = 0; i < count; i ++) {
        ecs_strbuf_list_next(buf);
        if (ecs_field_is_set(it, i)) {
            flecs_json_true(buf);
        } else {
            flecs_json_false(buf);
        }
    }

    flecs_json_array_pop(buf);

    return true;
}

static
bool flecs_json_serialize_iter_result_ids(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    const ecs_query_t *q = it->query;
    if (!q) {
        return false;
    }

    ecs_world_t *world = it->world;
    int16_t f, field_count = flecs_ito(int16_t, it->field_count);
    uint16_t field_mask = flecs_ito(uint16_t, (1 << field_count) - 1);    
    if (q->static_id_fields == field_mask) {
        /* All matched ids are static, nothing to serialize */
        return false;
    }

    flecs_json_memberl(buf, "ids");
    flecs_json_array_push(buf);

    for (f = 0; f < field_count; f ++) {
        ecs_flags16_t field_bit = flecs_ito(uint16_t, 1 << f);

        if (!(it->set_fields & field_bit)) {
            /* Don't serialize ids for fields that aren't set */
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        if (q->static_id_fields & field_bit) {
            /* Only add non-static ids to save bandwidth/performance */
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        flecs_json_next(buf);
        flecs_json_id(buf, world, it->ids[f]);
    }

    flecs_json_array_pop(buf);

    return true;
}

static
bool flecs_json_serialize_iter_result_sources(
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    const ecs_query_t *q = it->query;
    if (!q) {
        return false;
    }

    ecs_world_t *world = it->world;
    int32_t f, field_count = it->field_count;

    for (f = 0; f < field_count; f ++) {
        if (it->sources[f]) {
            break;
        }
    }

    if (f == field_count) {
        /* All fields are matched on $this */
        return false;
    }

    flecs_json_memberl(buf, "sources");
    flecs_json_array_push(buf);

    for (f = 0; f < field_count; f ++) {
        ecs_flags16_t field_bit = flecs_ito(uint16_t, 1 << f);

        if (!(it->set_fields & field_bit)) {
            /* Don't serialize source for fields that aren't set */
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        if (!it->sources[f]) {
            /* Don't serialize source for fields that have $this source */
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        flecs_json_next(buf);
        flecs_json_path(buf, world, it->sources[f]);
    }

    flecs_json_array_pop(buf);

    return true;
}

static
bool flecs_json_serialize_common_for_table(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_strbuf_list_push(buf, "", ",");
    flecs_json_serialize_vars(world, it, buf, desc);

    bool result = false;
    if (!desc || desc->serialize_fields) {
        ecs_strbuf_list_appendlit(buf, "\"fields\":");
        flecs_json_object_push(buf);
        result |= flecs_json_serialize_iter_result_is_set(it, buf);
        result |= flecs_json_serialize_iter_result_ids(it, buf);
        result |= flecs_json_serialize_iter_result_sources(it, buf);
    }

    ecs_strbuf_list_pop(buf, "");
    return result;
}

static
int flecs_json_serialize_iter_result_field_values(
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

    ecs_strbuf_appendlit(buf, "\"values\":");
    flecs_json_array_push(buf);

    for (f = 0; f < field_count; f ++) {
        ecs_flags16_t field_bit = flecs_ito(uint16_t, 1 << f);
        if (!(it->set_fields & field_bit)) {
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc)) 
        {
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        if (!it->ptrs[f]) {
            ecs_strbuf_list_appendlit(buf, "0");
            continue;
        }

        void *ptr = it->ptrs[f];
        if (!it->sources[f]) {
            ptr = ECS_ELEM(ptr, it->sizes[f], i);
        }

        flecs_json_next(buf);
        if (flecs_json_ser_type(world, &value_ctx->ser->ops, ptr, buf) != 0) {
            return -1;
        }
    }

    flecs_json_array_pop(buf);

    return 0;
}

int flecs_json_serialize_iter_result_query(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t count,
    bool has_this,
    const char *parent_path,
    const ecs_json_this_data_t *this_data)
{
    /* Serialize tags, pairs, vars once, since they're the same for each row */
    ecs_strbuf_t common_data_buf = ECS_STRBUF_INIT;
    bool common_field_data = flecs_json_serialize_common_for_table(
        world, it, &common_data_buf, desc);
    int32_t common_data_len = ecs_strbuf_written(&common_data_buf);
    char *common_data = NULL;
    if (!common_data_len) {
        ecs_strbuf_reset(&common_data_buf);
    } else {
        common_data = ecs_strbuf_get(&common_data_buf);
    }

    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_json_next(buf);
        flecs_json_object_push(buf);

        if (has_this) {
            flecs_json_serialize_iter_this(
                it, parent_path, this_data, i, buf, desc);
        }

        if (common_data) {
            ecs_strbuf_list_appendstrn(buf, 
                common_data, common_data_len);
        }

        if (!desc || desc->serialize_fields) {
            bool has_values = !desc || desc->serialize_values;
            if (it->flags & EcsIterNoData || !it->field_count) {
                has_values = false;
            }

            const ecs_query_t *q = it->query;
            if (q && !q->data_fields) {
                has_values = false;
            }

            if (has_values) {
                if (common_field_data) {
                    flecs_json_next(buf);
                }

                if (flecs_json_serialize_iter_result_field_values(
                    world, it, i, buf, desc, ser_ctx))
                {
                    ecs_os_free(common_data);
                    return -1;
                }
            }

            ecs_strbuf_appendstr(buf, "}"); // "fields": {
        }

        flecs_json_object_pop(buf);
    }

    ecs_os_free(common_data);

    return 0;
}

#endif
