/**
 * @file json/serialize_iter_result_query.c
 * @brief Serialize matched query data of result.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_skip_variable(
    const char *name)
{
    if (!name || name[0] == '_' || !ecs_os_strcmp(name, "this")) {
        return true;
    } else {
        return false;
    }
}

static
bool flecs_json_serialize_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    char **variable_names = it->variable_names;
    int32_t var_count = it->variable_count;
    int32_t actual_count = 0;

    for (int i = 1; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        ecs_entity_t var = it->variables[i].entity;
        if (!var) {
            /* Can't happen, but not the place of the serializer to complain */
            continue;
        }

        if (!actual_count) {
            flecs_json_memberl(buf, "vars");
            flecs_json_object_push(buf);
            actual_count ++;
        }

        flecs_json_member(buf, var_name);
        flecs_json_path_or_label(buf, world, var, desc->serialize_full_paths);
    }

    if (actual_count) {
        flecs_json_object_pop(buf);
    }

    return actual_count != 0;
}

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

    for (int i = 0; i < it->field_count; i ++) {
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
bool flecs_json_serialize_common_for_table(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_vars(world, it, buf, desc);
    result |= flecs_json_serialize_iter_result_is_set(it, buf);
    ecs_strbuf_list_pop(buf, "");
    if (!result) {
        ecs_strbuf_reset(buf);
    }
    return result;
}

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
    ecs_strbuf_t tags_pairs_vars_buf = ECS_STRBUF_INIT;
    int32_t tags_pairs_vars_len = 0;
    char *tags_pairs_vars = NULL;
    if (flecs_json_serialize_common_for_table(
        world, it, &tags_pairs_vars_buf, desc)) 
    {
        tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
        tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
    }

    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_json_next(buf);
        flecs_json_object_push(buf);

        if (has_this) {
            flecs_json_serialize_iter_this(
                it, parent_path, this_data, i, buf, desc);
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (flecs_json_serialize_iter_result_fields(
            world, it, i, buf, desc, ser_ctx))
        {
            return -1;
        }

        flecs_json_object_pop(buf);
    }

    ecs_os_free(tags_pairs_vars);

    return 0;
}

#endif
