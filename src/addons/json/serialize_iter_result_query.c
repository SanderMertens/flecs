/**
 * @file json/serialize_iter_result_query.c
 * @brief Serialize matched query data of result.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_serialize_tags(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)  
{
    int32_t f, field_count = it->field_count;
    ecs_id_t *ids = it->ids;
    void **ptrs = it->ptrs;
    
    int32_t tag_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f)) {
            continue;
        }

        ecs_id_t id = ids[f];
        if (ECS_IS_PAIR(id)) {
            continue;
        }

        if (ptrs[f]) {
            continue; /* Ignore components */
        }

        if (!tag_count) {
            flecs_json_memberl(buf, "tags");
            flecs_json_array_push(buf);
        }

        flecs_json_next(buf);
        flecs_json_path_or_label(buf, world, id, desc->serialize_full_paths);
        tag_count ++;
    }

    if (tag_count) {
        flecs_json_array_pop(buf);
    }

    return tag_count != 0;
}

static
bool flecs_json_serialize_pairs(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)  
{
    int32_t f, field_count = it->field_count;
    ecs_id_t *ids = it->ids;
    void **ptrs = it->ptrs;
    
    int32_t pair_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f)) {
            continue;
        }

        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        if (ptrs[f]) {
            continue; /* Ignore components */
        }

        if (!pair_count) {
            flecs_json_memberl(buf, "pairs");
            flecs_json_object_push(buf);
        }

        flecs_json_next(buf);
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_second(world, id);

        flecs_json_path_or_label(buf, world, first, desc->serialize_full_paths);
        ecs_strbuf_appendlit(buf, ":");
        flecs_json_path_or_label(buf, world, second, desc->serialize_full_paths);

        pair_count ++;
    }

    if (pair_count) {
        flecs_json_object_pop(buf);
    }

    return pair_count != 0;
}

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
bool flecs_json_serialize_tags_pairs_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_tags(world, it, buf, desc);
    result |= flecs_json_serialize_pairs(world, it, buf, desc);
    result |= flecs_json_serialize_vars(world, it, buf, desc);
    result |= flecs_json_serialize_iter_result_is_set(it, buf);
    ecs_strbuf_list_pop(buf, "");
    if (!result) {
        ecs_strbuf_reset(buf);
    }
    return result;
}

static
int flecs_json_serialize_components(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t row)
{
    void **ptrs = it->ptrs;
    if (!ptrs) {
        return 0;
    }

    int32_t f, field_count = it->field_count, component_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f)) {
            continue;
        }

        void *ptr = ptrs[f];
        if (!ptr) {
            continue;
        }

        if (ecs_field_is_self(it, f)) {
            ptr = ECS_ELEM(ptr, it->sizes[f], row);
        }

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
        bool has_reflection = flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc);
        flecs_json_member(buf, value_ctx->id_label);

        if (has_reflection) {
            if (flecs_json_ser_type(
                world, &value_ctx->ser->ops, ptr, buf) != 0) 
            {
                return -1;
            }
        } else {
            ecs_strbuf_appendlit(buf, "null");
        }

        component_count ++;
    }

    if (component_count) {
        flecs_json_object_pop(buf);
    }

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
    if (flecs_json_serialize_tags_pairs_vars(
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

        if (flecs_json_serialize_components(
            world, it, buf, ser_ctx, desc, i)) 
        {
            return -1;
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
