/**
 * @file addons/json/serialize_iter_result_table.c
 * @brief Serialize all components of matched entity.
 */

#include "json.h"

#ifdef FLECS_JSON

#define FLECS_JSON_MAX_TABLE_COMPONENTS (256)

bool flecs_json_is_builtin(
    ecs_id_t id)
{
    if (ECS_IS_PAIR(id)) {
        if (ECS_PAIR_FIRST(id) == EcsChildOf) {
            return true;
        }
        if (id == ecs_pair_t(EcsIdentifier, EcsName)) {
            return true;
        }
    }
    return false;
}

static
bool flecs_json_serialize_table_type_info(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{    
    flecs_json_memberl(buf, "type_info");
    flecs_json_object_push(buf);

    int32_t i, type_count = table->type.count;
    for (i = 0; i < type_count; i ++) {
        const ecs_table_record_t *tr = &table->_->records[i];
        ecs_component_record_t *cr = tr->hdr.cr;
        ecs_id_t id = table->type.array[i];
        if (!(cr->flags & EcsIdSparse) && 
             (!table->column_map || (table->column_map[i] == -1))) 
        {
            continue;
        }

        if (!desc || !desc->serialize_builtin) {
            if (flecs_json_is_builtin(id)) {
                continue;
            }
        }

        const ecs_type_info_t *ti = cr->type_info;
        if (!ti) {
            continue;
        }

        flecs_json_next(buf);
        ecs_strbuf_appendlit(buf, "\"");
        flecs_json_id_member(buf, world, id, desc->serialize_full_paths);
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
    int16_t f, type_count = flecs_ito(int16_t, table->type.count);
    ecs_id_t *ids = table->type.array;
    int16_t *column_map = table->column_map;
    
    int32_t tag_count = 0;
    ecs_table_record_t *trs = table->_->records;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (ECS_IS_PAIR(id)) {
            continue;
        }

        if (!desc || !desc->serialize_builtin) {
            if (flecs_json_is_builtin(id)) {
                continue;
            }
        }

        if (column_map && column_map[f] != -1) {
            continue; /* Ignore components */
        }

        const ecs_table_record_t *tr = &trs[f];
        ecs_component_record_t *cr = tr->hdr.cr;

        if (src_table) {
            if (!(cr->flags & EcsIdOnInstantiateInherit)) {
                continue;
            }
        }
        if (cr->flags & EcsIdSparse) {
            continue;
        }

        if (!tag_count) {
            flecs_json_memberl(buf, "tags");
            flecs_json_array_push(buf);
        }

        flecs_json_next(buf);

        ecs_strbuf_appendlit(buf, "\"");
        flecs_json_id_member(buf, world, id, 
            desc ? desc->serialize_full_paths : true);
        ecs_strbuf_appendlit(buf, "\"");

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
    int16_t f, type_count = flecs_ito(int16_t, table->type.count);
    ecs_id_t *ids = table->type.array;
    int16_t *column_map = table->column_map;

    (void)row;
    
    int32_t pair_count = 0;
    bool same_first = false;

    ecs_table_record_t *trs = table->_->records;
    for (f = 0; f < type_count; f ++) {
        ecs_id_t id = ids[f];
        if (!ECS_IS_PAIR(id)) {
            continue;
        }

        if (!desc || !desc->serialize_builtin) {
            if (flecs_json_is_builtin(id)) {
                continue;
            }
        }

        if (column_map && column_map[f] != -1) {
            continue; /* Ignore components */
        }

        const ecs_table_record_t *tr = &trs[f];
        ecs_component_record_t *cr = tr->hdr.cr;

        if (src_table) {
            if (!(cr->flags & EcsIdOnInstantiateInherit)) {
                continue;
            }
        }
        if (cr->flags & EcsIdSparse) {
            continue;
        }

        ecs_entity_t first = flecs_entities_get_alive(
            world, ECS_PAIR_FIRST(id));

        if (!pair_count) {
            flecs_json_memberl(buf, "pairs");
            flecs_json_object_push(buf);            
        }

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
            flecs_json_path_or_label(buf, world, first, 
                desc ? desc->serialize_full_paths : true);
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

        if (ECS_IS_VALUE_PAIR(id)) {
            ecs_strbuf_appendch(buf, '\"');
            ecs_strbuf_appendch(buf, '@');
            ecs_strbuf_appendint(buf, ECS_PAIR_SECOND(id));
            ecs_strbuf_appendch(buf, '\"');
        } else {
            ecs_entity_t second = flecs_entities_get_alive(
                world, ECS_PAIR_SECOND(id));

            flecs_json_path_or_label(buf, world, second, 
                desc ? desc->serialize_full_paths : true);
        }

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
int flecs_json_serialize_table_components(
    const ecs_world_t *world,
    ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_strbuf_t *buf,
    ecs_json_value_ser_ctx_t *values_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t row,
    int32_t *component_count)
{
    int32_t i, count = table->type.count;
    for (i = 0; i < count; i ++) {
        if (component_count[0] == FLECS_JSON_MAX_TABLE_COMPONENTS) {
            break;
        }

        ecs_id_t id = table->type.array[i];
        if(desc->component_filter && !desc->component_filter(world, id)) {
            continue;
        }
        if (!desc || !desc->serialize_builtin) {
            if (flecs_json_is_builtin(id)) {
                continue;
            }
        }

        void *ptr;
        const ecs_table_record_t *tr = &table->_->records[i];
        ecs_component_record_t *cr = tr->hdr.cr;

        if (src_table) {
            if (!(cr->flags & EcsIdOnInstantiateInherit)) {
                continue;
            }
        }

        const ecs_type_info_t *ti;
        int32_t column_index = table->column_map ? table->column_map[i] : -1;
        if (column_index != -1) {
            ecs_column_t *column = &table->data.columns[column_index];
            ti = column->ti;
            ptr = ECS_ELEM(column->data, ti->size, row);
        } else {
            if (!(cr->flags & EcsIdSparse)) {
                continue;
            }
            ecs_entity_t e = ecs_table_entities(table)[row];
            ptr = flecs_sparse_get(cr->sparse, 0, e);
            ti = cr->type_info;
        }

        if (!ptr) {
            continue;
        }

        if (!component_count[0]) {
            flecs_json_memberl(buf, "components");
            flecs_json_object_push(buf);
        }

        bool has_reflection;
        const EcsTypeSerializer *type_ser;
        if (values_ctx) {
            ecs_json_value_ser_ctx_t *value_ctx = 
                &values_ctx[component_count[0]];
            has_reflection = flecs_json_serialize_get_value_ctx(
                world, id, value_ctx, desc);
            flecs_json_member(buf, value_ctx->id_label);
            type_ser = value_ctx->ser;
        } else {
            ecs_strbuf_list_next(buf);
            ecs_strbuf_appendlit(buf, "\"");
            flecs_json_id_member(buf, world, id,
                desc ? desc->serialize_full_paths : true);
            ecs_strbuf_appendlit(buf, "\":");
            type_ser = NULL;
            if (!desc || desc->serialize_values) {
                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                type_ser = ecs_get(world, ti->component, EcsTypeSerializer);
            }
            has_reflection = type_ser != NULL;
        }

        component_count[0] ++;

        if (has_reflection && (!desc || desc->serialize_values)) {
            ecs_assert(type_ser != NULL, ECS_INTERNAL_ERROR, NULL);
            if (flecs_json_ser_type(
                world, &type_ser->ops, ptr, buf) != 0) 
            {
                goto error;
            }
        } else {
            ecs_strbuf_appendlit(buf, "null");
        }
    }

    if (component_count[0]) {
        flecs_json_object_pop(buf);
    }

    return 0;
error:
    return -1;
}

static
int flecs_json_serialize_table_inherited_type(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    bool *has_inherited)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return 0;
    }

    const ecs_table_record_t *tr = flecs_component_get_table(
        world->cr_isa_wildcard, table);
    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL); /* Table has IsA flag */

    int32_t i, start = tr->index, end = start + tr->count;
    for (i = start; i < end; i ++) {
        ecs_entity_t base = ecs_pair_second(world, table->type.array[i]);
        ecs_record_t *base_record = ecs_record_find(world, base);
        if (!base_record || !base_record->table) {
            continue;
        }

        ecs_table_t *base_table = base_record->table;
        if (flecs_json_serialize_table_inherited_type(
            world, base_table, buf, desc, has_inherited)) 
        {
            return -1;      
        }

        char *base_name = ecs_get_path(world, base);
        flecs_json_member(buf, base_name);
        flecs_json_object_push(buf);
        ecs_os_free(base_name);

        flecs_json_serialize_table_tags(
            world, base_table, table, buf, desc);

        flecs_json_serialize_table_pairs(
            world, base_table, table, ECS_RECORD_TO_ROW(base_record->row), 
            buf, desc);

        int32_t component_count = 0;
        if (flecs_json_serialize_table_components(
            world, base_table, table, buf, NULL, desc, 
                ECS_RECORD_TO_ROW(base_record->row), &component_count)) 
        {
            return -1;
        }

        if (desc->serialize_type_info) {
            flecs_json_serialize_table_type_info(
                world, base_table, buf, desc);
        }

        flecs_json_object_pop(buf);
    }

    *has_inherited = true;
    return 0;
}

static
int flecs_json_serialize_table_inherited(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    bool *has_inherited)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return 0;
    }

    flecs_json_memberl(buf, "inherited");
    flecs_json_object_push(buf);
    if (flecs_json_serialize_table_inherited_type(
        world, table, buf, desc, has_inherited)) 
    {
        return -1;
    }
    flecs_json_object_pop(buf);
    return 0;
}

static
int flecs_json_serialize_table_tags_pairs_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_table_t *table,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    bool *result_out)
{
    *result_out = false;
    ecs_strbuf_list_push(buf, "", ",");
    *result_out |= flecs_json_serialize_table_tags(world, table, NULL, buf, desc);
    *result_out |= flecs_json_serialize_table_pairs(world, table, NULL, row, buf, desc);
    *result_out |= flecs_json_serialize_vars(world, it, buf, desc);

    if (desc->serialize_inherited) {
        if (flecs_json_serialize_table_inherited(
            world, table, buf, desc, result_out)) 
        {
            return -1;
        }
    }

    if (desc->serialize_type_info) {
        /* If we're serializing tables and are requesting type info, it must be
         * added to each result. */
         *result_out |= flecs_json_serialize_table_type_info(world, table, buf, desc);
    }

    ecs_strbuf_list_pop(buf, "");
    if (!*result_out) {
        ecs_strbuf_reset(buf);
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

    /* Serialize tags, pairs, vars once, since they're the same for each row */
    ecs_strbuf_t tags_pairs_vars_buf = ECS_STRBUF_INIT;
    int32_t tags_pairs_vars_len = 0;
    char *tags_pairs_vars = NULL;

    bool has_tags_pairs_vars = false;
    if (flecs_json_serialize_table_tags_pairs_vars(
        world, it, table, 0, &tags_pairs_vars_buf, desc, &has_tags_pairs_vars)) 
    {
        return -1;
    }

    if (has_tags_pairs_vars) {
        tags_pairs_vars_len = ecs_strbuf_written(&tags_pairs_vars_buf);
        tags_pairs_vars = ecs_strbuf_get(&tags_pairs_vars_buf);
    }

    /* If one entity has more than 256 components (oof), bad luck */
    ecs_json_value_ser_ctx_t values_ctx[FLECS_JSON_MAX_TABLE_COMPONENTS] = {{0}};
    int32_t component_count = 0;

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

        if (tags_pairs_vars) {
            ecs_strbuf_list_appendstrn(buf, 
                tags_pairs_vars, tags_pairs_vars_len);
        }

        component_count = 0; /* Each row has the same number of components */
        if (flecs_json_serialize_table_components(
            world, table, NULL, buf, values_ctx, desc, i, &component_count)) 
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

    for (i = 0; i < component_count; i ++) {
        ecs_os_free(values_ctx[i].id_label);
    }

    ecs_os_free(tags_pairs_vars);

    return result;
}

#endif
