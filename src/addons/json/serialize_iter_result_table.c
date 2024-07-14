/**
 * @file addons/json/serialize_iter_result_table.c
 * @brief Serialize all components of matched entity.
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
    
    flecs_json_memberl(buf, "type_info");
    flecs_json_object_push(buf);

    if (!column_count) {
        flecs_json_object_pop(buf);
        return false;
    }

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
bool flecs_json_serialize_table_tags(
    const ecs_world_t *world,
    const ecs_table_t *table,
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

        if (column_map && column_map[f] != -1) {
            continue; /* Ignore components */
        }

        const ecs_table_record_t *tr = &trs[f];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;

        if (src_table) {
            if (!(idr->flags & EcsIdOnInstantiateInherit)) {
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
bool flecs_json_serialize_table_pairs(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_table_t *src_table,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int32_t f, type_count = table->type.count;
    ecs_id_t *ids = table->type.array;
    int32_t *column_map = table->column_map;
    
    int32_t pair_count = 0;
    bool same_first = false;

    ecs_table_record_t *trs = table->_->records;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        if (column_map && column_map[f] != -1) {
            continue; /* Ignore components */
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
        
        if (second == EcsUnion) {
            ecs_assert(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);
            ecs_entity_t e = flecs_table_entities_array(table)[row];
            second = ecs_get_target(world, e, first, 0);
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

        const EcsTypeSerializer *ts = NULL;
        if (!desc || desc->serialize_values) {
            ts = ecs_get(world, ti->component, EcsTypeSerializer);
        }

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        ecs_strbuf_list_next(buf);
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

        char *base_name = ecs_get_path(world, base);
        flecs_json_member(buf, base_name);
        flecs_json_object_push(buf);
        ecs_os_free(base_name);

        flecs_json_serialize_table_tags(
            world, base_table, table, buf, desc);

        flecs_json_serialize_table_pairs(
            world, base_table, table, ECS_RECORD_TO_ROW(base_record->row), 
            buf, desc);

        flecs_json_serialize_table_inherited_type_components(
            world, base_record, buf, desc);

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
    const ecs_iter_t *it,
    ecs_table_t *table,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    bool result = false;
    ecs_strbuf_list_push(buf, "", ",");
    result |= flecs_json_serialize_table_tags(world, table, NULL, buf, desc);
    result |= flecs_json_serialize_table_pairs(world, table, NULL, row, buf, desc);
    result |= flecs_json_serialize_vars(world, it, buf, desc);
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
int flecs_json_serialize_table_components(
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
        bool has_reflection = flecs_json_serialize_get_value_ctx(
            world, column->id, value_ctx, desc);

        if (!component_count) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        flecs_json_member(buf, value_ctx->id_label);

        if (has_reflection && (!desc || desc->serialize_values)) {
            void *ptr = ecs_vec_get(&column->data, column->size, row);
            ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(value_ctx->id_label != NULL, ECS_INTERNAL_ERROR, NULL);
            if (flecs_json_ser_type(world, &value_ctx->ser->ops, ptr, buf) != 0) {
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

int flecs_json_serialize_iter_result_table(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
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

    /* Serialize tags, pairs, vars once, since they're the same for each row, 
     * except when table has union pairs, which can be different for each 
     * entity. */
    ecs_strbuf_t tags_pairs_vars_buf = ECS_STRBUF_INIT;
    int32_t tags_pairs_vars_len = 0;
    char *tags_pairs_vars = NULL;
    bool has_union = table->flags & EcsTableHasUnion;

    if (!has_union) {
        if (flecs_json_serialize_table_tags_pairs_vars(
            world, it, table, 0, &tags_pairs_vars_buf, desc)) 
        {
            tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
            tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
        }
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
            flecs_json_serialize_iter_this(
                it, parent_path, &this_data_cpy, i - it->offset, buf, desc);
        }

        if (has_union) {
            if (flecs_json_serialize_table_tags_pairs_vars(
                world, it, table, i, &tags_pairs_vars_buf, desc)) 
            {
                tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
                tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
            }
        }

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        if (has_union) {
            ecs_os_free(tags_pairs_vars);
            tags_pairs_vars = NULL;
        }

        if (flecs_json_serialize_table_components(
            world, table, buf, values_ctx, desc, i, column_count)) 
        {
            result = -1;
            break;
        }

        if (desc->serialize_matches) {
            flecs_json_serialize_matches(
                world, buf, it->entities[i - it->offset]);
        }

        if (desc->serialize_refs) {
            flecs_json_serialize_refs(world, buf, it->entities[i - it->offset], 
                desc->serialize_refs);
        }

        if (desc->serialize_alerts) {
            flecs_json_serialize_alerts(world, buf, 
                it->entities[i - it->offset]);
        }

        flecs_json_object_pop(buf);
    }

    for (i = 0; i < column_count; i ++) {
        ecs_os_free(values_ctx[i].id_label);
    }

    ecs_os_free(tags_pairs_vars);

    return result;
}

#endif
