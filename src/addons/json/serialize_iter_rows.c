/**
 * @file json/serialize_iter_rows.c
 * @brief Serialize (component) values to JSON strings.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_serialize_row_tags(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)  
{
    int32_t f, field_count = it->field_count;
    ecs_id_t *ids = it->ids;
    void **ptrs = it->ptrs;
    
    int32_t tag_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f + 1)) {
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
        flecs_json_label(buf, world, id);

        tag_count ++;
    }

    if (tag_count) {
        flecs_json_array_pop(buf);
    }

    return tag_count != 0;
}

static
bool flecs_json_serialize_row_pairs(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)  
{
    int32_t f, field_count = it->field_count;
    ecs_id_t *ids = it->ids;
    
    int32_t pair_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f + 1)) {
            continue;
        }

        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        if (!pair_count) {
            flecs_json_memberl(buf, "pairs");
            flecs_json_object_push(buf);
        }

        flecs_json_next(buf);
        ecs_entity_t first = ecs_pair_first(world, id);
        ecs_entity_t second = ecs_pair_second(world, id);

        flecs_json_label(buf, world, first);
        ecs_strbuf_appendlit(buf, ":");
        flecs_json_label(buf, world, second);

        pair_count ++;
    }

    if (pair_count) {
        flecs_json_object_pop(buf);
    }

    return pair_count != 0;
}

static
bool flecs_json_serialize_row_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
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
        flecs_json_label(buf, world, var);
    }

    if (actual_count) {
        flecs_json_object_pop(buf);
    }

    return actual_count != 0;
}

static
bool flecs_json_serialize_tags_pairs_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_row_tags(world, it, buf);
    result |= flecs_json_serialize_row_pairs(world, it, buf);
    result |= flecs_json_serialize_row_vars(world, it, buf);
    result |= flecs_json_serialize_iter_result_is_set(it, buf);
    ecs_strbuf_list_pop(buf, "");
    if (!result) {
        ecs_strbuf_reset(buf);
    }
    return result;
}

static
bool flecs_json_serialize_table_row_tags(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int32_t f, type_count = table->type.count;
    ecs_id_t *ids = table->type.array;
    int32_t *column_map = table->column_map;
    
    int32_t tag_count = 0;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (ECS_IS_PAIR(id)) {
            continue;
        }

        if (column_map[f] != -1) {
            continue; /* Ignore components */
        }

        if (!desc || !desc->serialize_private) {
            if (ecs_has_id(world, id & ECS_COMPONENT_MASK, EcsPrivate)) {
                continue;
            }
        }

        if (!tag_count) {
            flecs_json_memberl(buf, "tags");
            flecs_json_array_push(buf);
        }

        flecs_json_next(buf);
        flecs_json_label(buf, world, id);

        tag_count ++;
    }

    if (tag_count) {
        flecs_json_array_pop(buf);
    }

    return tag_count != 0;
}

static
bool flecs_json_serialize_table_row_pairs(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int32_t f, type_count = table->type.count;
    ecs_id_t *ids = table->type.array;
    
    int32_t pair_count = 0;
    bool same_first = false;

    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        ecs_entity_t first = flecs_entities_get_alive(
            world, ECS_PAIR_FIRST(id));
        if (!desc || !desc->serialize_private) {
            if (ecs_has_id(world, first, EcsPrivate)) {
                continue;
            }
        }

        if (!pair_count) {
            flecs_json_memberl(buf, "pairs");
            flecs_json_object_push(buf);            
        }

        ecs_entity_t second = flecs_entities_get_alive(
            world, ECS_PAIR_SECOND(id));

        bool is_last = f == (type_count - 1);
        bool is_same = !is_last && 
            (ECS_PAIR_FIRST(ids[f + 1]) == ECS_PAIR_FIRST(id));

        if (same_first && f && ECS_PAIR_FIRST(ids[f - 1]) != ECS_PAIR_FIRST(id)) {
            /* New pair has different first elem, so close array */
            flecs_json_array_pop(buf);
            same_first = false;
        }

        if (!same_first) {
            /* Only append pair label if we're not appending to array */
            flecs_json_next(buf);
            flecs_json_label(buf, world, first);
            ecs_strbuf_appendlit(buf, ":");

            /* Open array scope if this is a pair with multiple targets */
            if (is_same) {
                flecs_json_array_push(buf);
                same_first = true;
            }
        } 
        if (same_first) {
            flecs_json_next(buf);
        }

        flecs_json_label(buf, world, second);

        pair_count ++;
    }

    if (same_first) {
        flecs_json_array_pop(buf);
    }

    if (pair_count) {
        flecs_json_object_pop(buf);
    }

    return pair_count != 0;
}

static
bool flecs_json_serialize_table_tags_pairs_vars(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_table_row_tags(world, table, buf, desc);
    result |= flecs_json_serialize_table_row_pairs(world, table, buf, desc);
    ecs_strbuf_list_pop(buf, "");
    if (!result) {
        ecs_strbuf_reset(buf);
    }
    return result;
}

static
bool flecs_json_serialize_get_value_ctx(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_json_value_ser_ctx_t *ctx)
{
    if (!id) {
        return false;
    }
    if (!ctx->initialized) {
        ctx->initialized = true;

        ecs_strbuf_t idlbl = ECS_STRBUF_INIT;
        flecs_json_id_member(&idlbl, world, id);
        ctx->id_label = ecs_strbuf_get(&idlbl);

        ecs_entity_t type = ecs_get_typeid(world, id);
        if (!type) {
            return false;
        }

        ctx->type = type;
        ctx->ser = ecs_get(world, type, EcsMetaTypeSerialized);
        if (!ctx->ser) {
            return false;
        }

        return true;
    } else {
        return ctx->ser != NULL;
    }
}

bool flecs_json_serialize_get_field_ctx(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    int32_t f,
    ecs_json_ser_ctx_t *ser_ctx)
{
    ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
    if (it->query) {
        return flecs_json_serialize_get_value_ctx(
            world, it->query->ids[f], value_ctx);
    } else if (it->ids[f]) {
        return flecs_json_serialize_get_value_ctx(world, it->ids[f], value_ctx);
    } else {
        return false;
    }
}

static
int flecs_json_serialize_row_components(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    int32_t row)
{
    void **ptrs = it->ptrs;
    if (!ptrs) {
        return 0;
    }

    int32_t f, field_count = it->field_count, component_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f + 1)) {
            continue;
        }

        void *ptr = ptrs[f];
        if (!ptr) {
            continue;
        }

        if (ecs_field_is_self(it, f + 1)) {
            ptr = ECS_ELEM(ptr, it->sizes[f], row);
        }

        ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(world, it->ids[f], value_ctx)) {
            continue;
        }

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        flecs_json_member(buf, value_ctx->id_label);
        if (flecs_json_ser_type(world, &value_ctx->ser->ops, ptr, buf) != 0) {
            return -1;
        }

        component_count ++;
    }

    if (component_count) {
        flecs_json_object_pop(buf);
    }

    return 0;
}

static
int flecs_json_serialize_row_table_components(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    ecs_json_value_ser_ctx_t *values_ctx,
    int32_t row,
    int32_t column_count)
{
    int32_t f, component_count = 0;
    for (f = 0; f < column_count; f ++) {
        ecs_column_t *column = &table->data.columns[f];
        ecs_json_value_ser_ctx_t *value_ctx = &values_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(world, column->id, value_ctx)) {
            continue;
        }

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        void *ptr = ecs_vec_get(&column->data, column->size, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(value_ctx->id_label != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_json_member(buf, value_ctx->id_label);
        if (flecs_json_ser_type(world, &value_ctx->ser->ops, ptr, buf) != 0) {
            return -1;
        }

        component_count ++;
    }

    if (component_count) {
        flecs_json_object_pop(buf);
    }

    return 0;
}

static
void flecs_json_serialize_iter_this_row(
    const ecs_iter_t *it,
    const char *parent_path,
    const EcsIdentifier *names,
    int32_t row,
    ecs_strbuf_t *buf)
{
    if (parent_path) {
        flecs_json_memberl(buf, "parent");
        flecs_json_string(buf, parent_path);
    }

    flecs_json_memberl(buf, "name");
    if (names) {
        flecs_json_string(buf, names[row].value);
    } else {
        ecs_strbuf_appendlit(buf, "\"");
        ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
            (uint32_t)it->entities[row]));
        ecs_strbuf_appendlit(buf, "\"");
    }
}

static
int flecs_json_serialize_iter_result_query_rows(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    int32_t count,
    bool has_this,
    const char *parent_path,
    const EcsIdentifier *names)
{
    /* Serialize tags, pairs, vars once, since they're the same for each row */
    ecs_strbuf_t tags_pairs_vars_buf = ECS_STRBUF_INIT;
    int32_t tags_pairs_vars_len = 0;
    char *tags_pairs_vars = NULL;
    if (flecs_json_serialize_tags_pairs_vars(world, it, &tags_pairs_vars_buf)) {
        tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
        tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
    }

    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_json_next(buf);
        flecs_json_object_push(buf);

        if (has_this) {
            flecs_json_serialize_iter_this_row(
                it, parent_path, names, i, buf);
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (flecs_json_serialize_row_components(world, it, buf, ser_ctx, i)) {
            return -1;
        }

        flecs_json_object_pop(buf);
    }

    ecs_os_free(tags_pairs_vars);

    return 0;
}

static
int flecs_json_serialize_iter_result_table_rows(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    int32_t count,
    bool has_this,
    const char *parent_path,
    const EcsIdentifier *names)
{
    ecs_table_t *table = it->table;
    if (!table || !count) {
        return 0;
    }

    /* Serialize tags, pairs, vars once, since they're the same for each row */
    ecs_strbuf_t tags_pairs_vars_buf = ECS_STRBUF_INIT;
    int32_t tags_pairs_vars_len = 0;
    char *tags_pairs_vars = NULL;
    if (flecs_json_serialize_table_tags_pairs_vars(
        world, table, &tags_pairs_vars_buf, desc)) 
    {
        tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
        tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
    }

    /* If one entity has more than 256 components (oof), bad luck */
    ecs_json_value_ser_ctx_t values_ctx[256] = {{0}};
    int32_t column_count = table->column_count;
    if (column_count > 256) {
        column_count = 256;
    }

    int32_t i, end = it->offset + count;
    int result = 0;
    for (i = it->offset; i < end; i ++) {
        flecs_json_next(buf);
        flecs_json_object_push(buf);

        if (has_this) {
            const EcsIdentifier *name_ptr = NULL;
            if (names) {
                /* Correct for offset applied by iterator */
                name_ptr = &names[-it->offset];
            }
            flecs_json_serialize_iter_this_row(
                it, parent_path, name_ptr, i, buf);
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (flecs_json_serialize_row_table_components(
            world, table, buf, values_ctx, i, column_count)) 
        {
            result = -1;
            break;
        }

        flecs_json_object_pop(buf);
    }

    for (i = 0; i < column_count; i ++) {
        ecs_os_free(values_ctx[i].id_label);
    }

    ecs_os_free(tags_pairs_vars);

    return result;
}

int flecs_json_serialize_iter_result_rows(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    char *parent_path = NULL;
    EcsIdentifier *names = NULL;

    int32_t count = it->count;
    bool has_this = true;
    if (!count) {
        count = 1; /* Query without this variable */
        has_this = false;
    } else {
        ecs_table_t *table = it->table;
        if (table) {
            /* Get path to parent once for entire table */
            if (table->flags & EcsTableHasChildOf) {
                const ecs_table_record_t *tr = flecs_table_record_get(
                    world, table, ecs_pair(EcsChildOf, EcsWildcard));
                ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_entity_t parent = ecs_pair_second(
                    world, table->type.array[tr->index]);
                parent_path = ecs_get_path_w_sep(world, 0, parent, ".", "");
            }

            /* Fetch name column once vs. calling ecs_get_name for each row */
            if (table->flags & EcsTableHasName) {
                names = ecs_table_get_id(it->world, it->table, 
                    ecs_pair(ecs_id(EcsIdentifier), EcsName), it->offset);
            }
        } else {
            /* Very rare case, but could happen if someone's using an iterator
             * to return empty entities. */
        }
    }

    if (!desc->serialize_table) {
        flecs_json_serialize_iter_result_query_rows(
            world, it, buf, ser_ctx, count, has_this, parent_path, names);
    } else {
        flecs_json_serialize_iter_result_table_rows(
            world, it, buf, desc, count, has_this, parent_path, names);
    }

    ecs_os_free(parent_path);

    return 0;
}

#endif
