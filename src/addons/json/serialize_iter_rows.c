/**
 * @file json/serialize_iter_rows.c
 * @brief Serialize (component) values to JSON strings.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_serialize_table_type_info(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_column_t *columns = table->data.columns;
    int32_t i, column_count = table->column_count;
    
    if (!column_count) {
        return false;
    }

    flecs_json_memberl(buf, "type_info");
    flecs_json_object_push(buf);

    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_type_info_t *ti = column->ti;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        flecs_json_next(buf);
        ecs_strbuf_appendlit(buf, "\"");
        flecs_json_id_member(buf, world, column->id, desc->serialize_full_paths);
        ecs_strbuf_appendlit(buf, "\":");

        ecs_type_info_to_json_buf(world, ti->component, buf);
    }

    flecs_json_object_pop(buf);

    return true;
}

static
bool flecs_json_serialize_row_tags(
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
bool flecs_json_serialize_row_pairs(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)  
{
    int32_t f, field_count = it->field_count;
    ecs_id_t *ids = it->ids;
    
    int32_t pair_count = 0;
    for (f = 0; f < field_count; f ++) {
        if (!ecs_field_is_set(it, f)) {
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
bool flecs_json_serialize_row_vars(
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
bool flecs_json_serialize_tags_pairs_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_row_tags(world, it, buf, desc);
    result |= flecs_json_serialize_row_pairs(world, it, buf, desc);
    result |= flecs_json_serialize_row_vars(world, it, buf, desc);
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
    ecs_entity_t base,
    const ecs_table_t *src_table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int32_t f, type_count = table->type.count;
    ecs_id_t *ids = table->type.array;
    int32_t *column_map = table->column_map;
    
    int32_t tag_count = 0;
    ecs_table_record_t *trs = table->_->records;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (ECS_IS_PAIR(id)) {
            continue;
        }

        if (column_map[f] != -1) {
            continue; /* Ignore components */
        }

        const ecs_table_record_t *tr = &trs[f];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;

        if (src_table) {
            if (!(idr->flags & EcsIdOnInstantiateInherit)) {
                continue;
            }
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
        flecs_json_path_or_label(buf, world, id, desc->serialize_full_paths);

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
    ecs_entity_t base,
    const ecs_table_t *src_table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int32_t f, type_count = table->type.count;
    ecs_id_t *ids = table->type.array;
    
    int32_t pair_count = 0;
    bool same_first = false;

    ecs_table_record_t *trs = table->_->records;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        const ecs_table_record_t *tr = &trs[f];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;

        if (src_table) {
            if (!(idr->flags & EcsIdOnInstantiateInherit)) {
                continue;
            }
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
            flecs_json_path_or_label(buf, world, first, desc->serialize_full_paths);
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

        flecs_json_path_or_label(buf, world, second, desc->serialize_full_paths);

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
bool flecs_json_serialize_table_inherited_type_components(
    const ecs_world_t *world,
    ecs_table_t *src_table,
    ecs_entity_t base,
    ecs_record_t *r,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_table_t *table = r->table;
    int32_t row = ECS_RECORD_TO_ROW(r->row);

    bool result = false;
    int32_t i, count = table->column_count, component_count = 0;
    const ecs_table_record_t *trs = table->_->records;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &table->data.columns[i];
        int32_t type_index = table->column_map[table->type.count + i];
        ecs_assert(type_index != -1, ECS_INTERNAL_ERROR, NULL);

        const ecs_table_record_t *tr = &trs[type_index];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        if (!(idr->flags & EcsIdOnInstantiateInherit)) {
            continue;
        }

        const ecs_type_info_t *ti = column->ti;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        const EcsTypeSerializer *ts = ecs_get(
            world, ti->component, EcsTypeSerializer);

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        ecs_strbuf_appendlit(buf, "\"");
        flecs_json_id_member(buf, world, column->id,
            desc ? desc->serialize_full_paths : false);
        ecs_strbuf_appendlit(buf, "\":");

        if (ts) {
            void *ptr = ecs_vec_get(&column->data, column->size, row);
            if (flecs_json_ser_type(world, &ts->ops, ptr, buf) != 0) {
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

    return result;
}

static
bool flecs_json_serialize_table_inherited_type(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return false;
    }

    const ecs_table_record_t *tr = flecs_id_record_get_table(
        world->idr_isa_wildcard, table);
    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL); /* Table has IsA flag */

    int32_t i, start = tr->index, end = start + tr->count;
    for (i = start; i < end; i ++) {
        ecs_entity_t base = ecs_pair_second(world, table->type.array[i]);
        ecs_record_t *base_record = ecs_record_find(world, base);
        if (!base_record || !base_record->table) {
            continue;
        }

        ecs_table_t *base_table = base_record->table;
        flecs_json_serialize_table_inherited_type(world, base_table, buf, desc);

        char *base_name = ecs_get_fullpath(world, base);
        flecs_json_member(buf, base_name);
        flecs_json_object_push(buf);
        ecs_os_free(base_name);

        flecs_json_serialize_table_row_tags(
            world, base_table, base, table, buf, desc);

        flecs_json_serialize_table_row_pairs(
            world, base_table, base, table, buf, desc);

        flecs_json_serialize_table_inherited_type_components(
            world, table, base, base_record, buf, desc);

        if (desc->serialize_type_info) {
            flecs_json_serialize_table_type_info(
                world, base_table, buf, desc);
        }

        flecs_json_object_pop(buf);
    }

    return true;
}

static
bool flecs_json_serialize_table_inherited(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return false;
    }

    flecs_json_memberl(buf, "inherited");
    flecs_json_object_push(buf);
    flecs_json_serialize_table_inherited_type(world, table, buf, desc);
    flecs_json_object_pop(buf);
    return true;
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
    result |= flecs_json_serialize_table_row_tags(world, table, 0, NULL, buf, desc);
    result |= flecs_json_serialize_table_row_pairs(world, table, 0, NULL, buf, desc);
    if (desc->serialize_inherited) {
        result |= flecs_json_serialize_table_inherited(world, table, buf, desc);
    }
    if (desc->serialize_type_info) {
        /* If we're serializing tables and are requesting type info, it must be
         * added to each result. */
        result |= flecs_json_serialize_table_type_info(world, table, buf, desc);
    }
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
    ecs_json_value_ser_ctx_t *ctx,
    const ecs_iter_to_json_desc_t *desc)
{
    if (!id) {
        return false;
    }

    if (!ctx->initialized) {
        ctx->initialized = true;

        ecs_strbuf_t idlbl = ECS_STRBUF_INIT;
        flecs_json_id_member(&idlbl, world, id, 
            desc ? desc->serialize_full_paths : false);
        ctx->id_label = ecs_strbuf_get(&idlbl);

        ecs_entity_t type = ecs_get_typeid(world, id);
        if (!type) {
            return false;
        }

        ctx->type = type;
        ctx->ser = ecs_get(world, type, EcsTypeSerializer);
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
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
    if (it->query) {
        return flecs_json_serialize_get_value_ctx(
            world, it->query->ids[f], value_ctx, desc);
    } else if (it->ids[f]) {
        return flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc);
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

        ecs_json_value_ser_ctx_t *value_ctx = &ser_ctx->value_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(
            world, it->ids[f], value_ctx, desc)) 
        {
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
    const ecs_iter_to_json_desc_t *desc,
    int32_t row,
    int32_t column_count)
{
    int32_t f, component_count = 0;
    for (f = 0; f < column_count; f ++) {
        ecs_column_t *column = &table->data.columns[f];
        ecs_json_value_ser_ctx_t *value_ctx = &values_ctx[f];
        if (!flecs_json_serialize_get_value_ctx(
            world, column->id, value_ctx, desc)) 
        {
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

typedef struct ecs_json_this_data_t {
    const EcsIdentifier *names;
    const EcsDocDescription *labels;
} ecs_json_this_data_t;

static
void flecs_json_serialize_iter_this_row(
    const ecs_iter_t *it,
    const char *parent_path,
    const ecs_json_this_data_t *this_data,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    if (parent_path) {
        flecs_json_memberl(buf, "parent");
        flecs_json_string(buf, parent_path);
    }

    flecs_json_memberl(buf, "name");
    if (this_data->names) {
        flecs_json_string(buf, this_data->names[row].value);
    } else {
        ecs_strbuf_appendlit(buf, "\"");
        ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
            (uint32_t)it->entities[row]));
        ecs_strbuf_appendlit(buf, "\"");
    }

#ifdef FLECS_DOC
    if (desc->serialize_entity_labels) {
        flecs_json_memberl(buf, "label");
        if (this_data->labels) {
            flecs_json_string(buf, this_data->labels[row].value);
        } else {
            if (this_data->names) {
                flecs_json_string(buf, this_data->names[row].value);
            } else {
                ecs_strbuf_appendlit(buf, "\"");
                ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
                    (uint32_t)it->entities[row]));
                ecs_strbuf_appendlit(buf, "\"");
            }
        }
    }
#endif
}

static
int flecs_json_serialize_iter_result_query_rows(
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
            flecs_json_serialize_iter_this_row(
                it, parent_path, this_data, i, buf, desc);
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (flecs_json_serialize_row_components(
            world, it, buf, ser_ctx, desc, i)) 
        {
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
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t count,
    bool has_this,
    const char *parent_path,
    const ecs_json_this_data_t *this_data)
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
            ecs_json_this_data_t this_data_cpy = *this_data;
            
            /* Correct for offset applied by iterator */
            if (this_data->names) {
                this_data_cpy.names = &this_data->names[-it->offset];
            }

#ifdef FLECS_DOC
            if (this_data->labels) {
                this_data_cpy.labels = &this_data->labels[-it->offset];
            }
#endif

            flecs_json_serialize_iter_this_row(
                it, parent_path, &this_data_cpy, i, buf, desc);
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (flecs_json_serialize_row_table_components(
            world, table, buf, values_ctx, desc, i, column_count)) 
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
    ecs_json_this_data_t this_data = {NULL};

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
                this_data.names = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsIdentifier, EcsName), it->offset);
            }

            /* Get entity labels */
#ifdef FLECS_DOC
            if (desc->serialize_entity_labels) {
                this_data.labels = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsName), it->offset);
            }
#endif
        } else {
            /* Very rare case, but could happen if someone's using an iterator
             * to return empty entities. */
        }
    }

    if (!desc->serialize_table) {
        flecs_json_serialize_iter_result_query_rows(world, it, buf, ser_ctx, 
            desc, count, has_this, parent_path, &this_data);
    } else {
        flecs_json_serialize_iter_result_table_rows(world, it, buf, ser_ctx, 
            desc, count, has_this, parent_path, &this_data);
    }

    ecs_os_free(parent_path);

    return 0;
}

#endif
