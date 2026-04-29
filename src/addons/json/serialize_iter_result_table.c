/**
 * @file addons/json/serialize_iter_result_table.c
 * @brief Serialize all components of matched entity.
 */

#include "json.h"

#ifdef FLECS_JSON

#define FLECS_JSON_MAX_TABLE_COMPONENTS (256)

static
bool flecs_json_is_builtin(
    ecs_id_t id)
{
    return ECS_IS_PAIR(id) && (ECS_PAIR_FIRST(id) == EcsChildOf ||
        id == ecs_pair_t(EcsIdentifier, EcsName));
}

#define FLECS_JSON_DF_TAG       0
#define FLECS_JSON_DF_PAIR_TAG  1
#define FLECS_JSON_DF_COMPONENT 2

static
ecs_component_record_t* flecs_json_dont_fragment_first(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    if (!entity) return NULL;
    ecs_record_t *r = ecs_record_find(world, entity);
    if (!r || !(r->row & EcsEntityHasDontFragment)) {
        return NULL;
    }
    return world->cr_non_fragmenting_head;
}

static
bool flecs_json_dont_fragment_match(
    const ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t entity,
    const ecs_table_t *src_table,
    const ecs_iter_to_json_desc_t *desc,
    int kind)
{
    ecs_id_t id = cr->id;
    bool is_pair = ECS_IS_PAIR(id);
    bool has_ti = cr->type_info != NULL;

    if (kind == FLECS_JSON_DF_COMPONENT) {
        if (!has_ti) return false;
    } else {
        if (has_ti) return false;
        if (is_pair != (kind == FLECS_JSON_DF_PAIR_TAG)) return false;
    }
    if (!cr->sparse || ecs_id_is_wildcard(id)) return false;
    if (!flecs_sparse_has(cr->sparse, entity)) return false;
    if (src_table && !(cr->flags & EcsIdOnInstantiateInherit)) return false;

    bool skip_builtin = !desc || !desc->serialize_builtin;
    if (skip_builtin && flecs_json_is_builtin(id)) return false;
    if (kind == FLECS_JSON_DF_COMPONENT && desc->component_filter &&
        !desc->component_filter(world, id))
    {
        return false;
    }

    return true;
}

static
void flecs_json_serialize_table_tags(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
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

    ecs_component_record_t *cur = flecs_json_dont_fragment_first(world, entity);
    for (; cur; cur = cur->non_fragmenting.next) {
        if (!flecs_json_dont_fragment_match(
            world, cur, entity, src_table, desc, FLECS_JSON_DF_TAG))
        {
            continue;
        }
        if (!tag_count) {
            flecs_json_memberl(buf, "tags");
            flecs_json_array_push(buf);
        }
        flecs_json_next(buf);
        ecs_strbuf_appendlit(buf, "\"");
        flecs_json_id_member(buf, world, cur->id,
            desc ? desc->serialize_full_paths : true);
        ecs_strbuf_appendlit(buf, "\"");
        tag_count ++;
    }

    if (tag_count) {
        flecs_json_array_pop(buf);
    }
}

static
void flecs_json_emit_pair_entry(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_id_t id,
    ecs_id_t prev_id,
    ecs_id_t next_id,
    int32_t *pair_count,
    bool *same_first)
{
    bool full_paths = desc ? desc->serialize_full_paths : true;

    if (!*pair_count) {
        flecs_json_memberl(buf, "pairs");
        flecs_json_object_push(buf);
    }

    bool is_same = ECS_PAIR_FIRST(next_id) == ECS_PAIR_FIRST(id);

    if (*same_first && ECS_PAIR_FIRST(prev_id) != ECS_PAIR_FIRST(id)) {
        flecs_json_array_pop(buf);
        *same_first = false;
    }

    if (!*same_first) {
        ecs_entity_t first = flecs_entities_get_alive(
            world, ECS_PAIR_FIRST(id));
        flecs_json_next(buf);
        flecs_json_path_or_label(buf, world, first, full_paths);
        ecs_strbuf_appendlit(buf, ":");
        if (is_same) {
            flecs_json_array_push(buf);
            *same_first = true;
        }
    }
    if (*same_first) {
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
        flecs_json_path_or_label(buf, world, second, full_paths);
    }

    (*pair_count) ++;
}

static
void flecs_json_serialize_table_pairs(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    int16_t f, type_count = flecs_ito(int16_t, table->type.count);
    ecs_id_t *ids = table->type.array;
    int16_t *column_map = table->column_map;

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

        flecs_json_emit_pair_entry(world, buf, desc, id,
            f > 0 ? ids[f - 1] : 0,
            f < (type_count - 1) ? ids[f + 1] : 0,
            &pair_count, &same_first);
    }

    if (same_first) {
        flecs_json_array_pop(buf);
    }

    ecs_component_record_t *cur = flecs_json_dont_fragment_first(world, entity);
    ecs_id_t pending_id = 0, prev_id = 0;
    bool has_pending = false;
    same_first = false;
    for (; cur; cur = cur->non_fragmenting.next) {
        if (!flecs_json_dont_fragment_match(
            world, cur, entity, src_table, desc, FLECS_JSON_DF_PAIR_TAG))
        {
            continue;
        }
        if (has_pending) {
            flecs_json_emit_pair_entry(world, buf, desc, pending_id,
                prev_id, cur->id, &pair_count, &same_first);
            prev_id = pending_id;
        }
        pending_id = cur->id;
        has_pending = true;
    }
    if (has_pending) {
        flecs_json_emit_pair_entry(world, buf, desc, pending_id,
            prev_id, 0, &pair_count, &same_first);
    }
    if (same_first) {
        flecs_json_array_pop(buf);
    }

    if (pair_count) {
        flecs_json_object_pop(buf);
    }
}

static
int flecs_json_serialize_component_value(
    const ecs_world_t *world,
    ecs_id_t id,
    void *ptr,
    const ecs_type_info_t *ti,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    ecs_json_value_ser_ctx_t *values_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t *component_count)
{
    if (ti) {
        flecs_json_accum_type_info(world, ti->component, ser_ctx);
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
            return -1;
        }
    } else {
        ecs_strbuf_appendlit(buf, "null");
    }

    return 0;
}

static
int flecs_json_serialize_table_components(
    const ecs_world_t *world,
    ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
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

        if (flecs_json_serialize_component_value(world, id, ptr, ti, buf,
            ser_ctx, values_ctx, desc, component_count))
        {
            goto error;
        }
    }

    {
        ecs_component_record_t *cur = flecs_json_dont_fragment_first(
            world, entity);
        for (; cur; cur = cur->non_fragmenting.next) {
            if (component_count[0] == FLECS_JSON_MAX_TABLE_COMPONENTS) {
                break;
            }
            if (!flecs_json_dont_fragment_match(
                world, cur, entity, src_table, desc, FLECS_JSON_DF_COMPONENT))
            {
                continue;
            }
            void *ptr = flecs_sparse_get(cur->sparse, 0, entity);
            if (!ptr) continue;

            if (flecs_json_serialize_component_value(world, cur->id, ptr,
                cur->type_info, buf, ser_ctx, values_ctx, desc,
                component_count))
            {
                goto error;
            }
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
    ecs_json_ser_ctx_t *ser_ctx,
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
            world, base_table, buf, ser_ctx, desc, has_inherited))
        {
            return -1;
        }

        char *base_name = ecs_get_path(world, base);
        flecs_json_member(buf, base_name);
        flecs_json_object_push(buf);
        ecs_os_free(base_name);

        flecs_json_serialize_table_tags(
            world, base_table, table, base, buf, desc);

        flecs_json_serialize_table_pairs(
            world, base_table, table, base, buf, desc);

        int32_t component_count = 0;
        if (flecs_json_serialize_table_components(
            world, base_table, table, base, buf, ser_ctx, NULL, desc,
                ECS_RECORD_TO_ROW(base_record->row), &component_count))
        {
            return -1;
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
    ecs_json_ser_ctx_t *ser_ctx,
    const ecs_iter_to_json_desc_t *desc,
    bool *has_inherited)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return 0;
    }

    flecs_json_memberl(buf, "inherited");
    flecs_json_object_push(buf);
    if (flecs_json_serialize_table_inherited_type(
        world, table, buf, ser_ctx, desc, has_inherited))
    {
        return -1;
    }
    flecs_json_object_pop(buf);
    return 0;
}

int flecs_json_serialize_iter_result_table(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx,
    int32_t count,
    bool has_this,
    const char *parent_path,
    const ecs_json_this_data_t *this_data)
{
    ecs_table_t *table = it->table;
    if (!table || !count) {
        return 0;
    }

    /* If one entity has more than 256 components (oof), bad luck */
    ecs_json_value_ser_ctx_t values_ctx[FLECS_JSON_MAX_TABLE_COMPONENTS] = {{0}};
    int32_t component_count = 0;

    const ecs_entity_t *entities = ecs_table_entities(table);

    int32_t i, end = it->offset + count;
    int result = 0;
    for (i = it->offset; i < end; i ++) {
        ecs_entity_t e = entities[i];

        if (has_this) {
            ecs_json_this_data_t this_data_cpy = *this_data;
            if (!flecs_json_serialize_iter_this(it, parent_path,
                &this_data_cpy, i - it->offset, buf, desc, ser_ctx))
            {
                continue;
            }
        } else {
            flecs_json_next(buf);
            flecs_json_object_push(buf);
        }

        flecs_json_serialize_table_tags(world, table, NULL, e, buf, desc);
        flecs_json_serialize_table_pairs(world, table, NULL, e, buf, desc);
        flecs_json_serialize_vars(world, it, buf, desc);

        if (desc->serialize_inherited) {
            bool has_inherited = false;
            if (flecs_json_serialize_table_inherited(
                world, table, buf, ser_ctx, desc, &has_inherited))
            {
                result = -1;
                break;
            }
        }

        component_count = 0;
        if (flecs_json_serialize_table_components(
            world, table, NULL, e, buf, ser_ctx, values_ctx, desc, i,
            &component_count))
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

    return result;
}

#endif
