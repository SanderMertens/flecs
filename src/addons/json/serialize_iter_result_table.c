/**
 * @file addons/json/serialize_iter_result_table.c
 * @brief Serialize all components of matched entity.
 */

#include "json.h"

#ifdef FLECS_JSON

typedef struct flecs_json_type_iter_t {
    const ecs_world_t *world;
    const ecs_table_t *table;
    const ecs_iter_to_json_desc_t *desc;
    ecs_entity_t entity;
    bool inherited;
    int32_t index;
    int32_t column;
    ecs_component_record_t *cr;
} flecs_json_type_iter_t;

static ecs_component_record_t* flecs_json_type_next(
    flecs_json_type_iter_t *it,
    int kind)
{
    const ecs_table_t *table = it->table;
    for (;;) {
        ecs_component_record_t *cr;
        if (it->index < table->type.count) {
            int32_t index = it->index ++;
            cr = table->_->records[index].hdr.cr;
            it->column = table->column_map ? table->column_map[index] : -1;
        } else {
            if (it->index == table->type.count) {
                it->index ++;
                ecs_record_t *r = it->entity ?
                    ecs_record_find(it->world, it->entity) : NULL;
                cr = r && (r->row & EcsEntityHasDontFragment) ?
                    it->world->cr_non_fragmenting_head : NULL;
            } else {
                cr = it->cr->non_fragmenting.next;
            }
            it->column = -1;
            if (!cr) {
                return NULL;
            }
            it->cr = cr;
            if (!cr->sparse || ecs_id_is_wildcard(cr->id) ||
                !flecs_sparse_has(cr->sparse, it->entity))
            {
                continue;
            }
        }
        it->cr = cr;
        ecs_id_t id = cr->id;
        bool data = it->column != -1 ||
            ((cr->flags & EcsIdSparse) && cr->type_info);
        if ((data ? 2 : ECS_IS_PAIR(id) ? 1 : 0) != kind ||
            (it->inherited && !(cr->flags & EcsIdOnInstantiateInherit)))
        {
            continue;
        }
        if ((!it->desc || !it->desc->serialize_builtin) && ECS_IS_PAIR(id) &&
            (ECS_PAIR_FIRST(id) == EcsChildOf ||
             id == ecs_pair_t(EcsIdentifier, EcsName)))
        {
            continue;
        }
        if (kind == 2 && it->desc && it->desc->component_filter &&
            !it->desc->component_filter(it->world, id))
        {
            continue;
        }
        return cr;
    }
}

static void flecs_json_serialize_table_tags(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    flecs_json_type_iter_t it = {world, table, desc, entity, src_table != NULL};
    ecs_component_record_t *cr = flecs_json_type_next(&it, 0);
    if (!cr) {
        return;
    }
    flecs_json_memberl(buf, "tags");
    flecs_json_array_push(buf);
    do {
        flecs_json_next(buf);
        ecs_strbuf_appendch(buf, '"');
        flecs_json_id_member(buf, world, cr->id,
            desc ? desc->serialize_full_paths : true);
        ecs_strbuf_appendch(buf, '"');
    } while ((cr = flecs_json_type_next(&it, 0)));
    flecs_json_array_pop(buf);
}

static void flecs_json_emit_pair_entry(
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

static void flecs_json_serialize_table_pairs(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    flecs_json_type_iter_t it = {world, table, desc, entity, src_table != NULL};
    ecs_component_record_t *cr = flecs_json_type_next(&it, 1);
    int32_t count = 0;
    ecs_id_t prev = 0;
    bool same_first = false;
    while (cr) {
        ecs_id_t id = cr->id;
        int32_t index = it.index - 1;
        cr = flecs_json_type_next(&it, 1);
        ecs_id_t next = cr ? cr->id : 0;
        if (index < table->type.count) {
            prev = index ? table->type.array[index - 1] : 0;
            next = index + 1 < table->type.count ?
                table->type.array[index + 1] : 0;
        }
        flecs_json_emit_pair_entry(world, buf, desc, id, prev,
            next, &count, &same_first);
        if (index < table->type.count && it.index > table->type.count &&
            same_first)
        {
            flecs_json_array_pop(buf);
            same_first = false;
        }
        prev = id;
    }
    if (same_first) {
        flecs_json_array_pop(buf);
    }
    if (count) {
        flecs_json_object_pop(buf);
    }
}

static int flecs_json_serialize_component_value(
    const ecs_world_t *world,
    ecs_id_t id,
    void *ptr,
    const ecs_type_info_t *ti,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    ecs_json_value_ser_ctx_t *value_ctx,
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
    if (value_ctx) {
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

static int flecs_json_serialize_table_components(
    const ecs_world_t *world,
    ecs_table_t *table,
    const ecs_table_t *src_table,
    ecs_entity_t entity,
    ecs_strbuf_t *buf,
    ecs_json_ser_ctx_t *ser_ctx,
    ecs_vec_t *values_ctx,
    const ecs_iter_to_json_desc_t *desc,
    int32_t row,
    int32_t *component_count)
{
    flecs_json_type_iter_t it = {world, table, desc, entity, src_table != NULL};
    ecs_component_record_t *cr;
    while ((cr = flecs_json_type_next(&it, 2))) {
        const ecs_type_info_t *ti = cr->type_info;
        void *ptr;
        if (it.column != -1) {
            ptr = ECS_ELEM(table->data.columns[it.column].data, ti->size, row);
        } else {
            ecs_entity_t e = it.index <= table->type.count ?
                ecs_table_entities(table)[row] : entity;
            ptr = flecs_sparse_get(cr->sparse, ti->size, e);
        }
        if (!ptr) {
            continue;
        }
        ecs_json_value_ser_ctx_t *value_ctx = NULL;
        if (values_ctx) {
            int32_t index = component_count[0];
            if (index == ecs_vec_count(values_ctx)) {
                value_ctx = ecs_vec_append_t(
                    NULL, values_ctx, ecs_json_value_ser_ctx_t);
                *value_ctx = (ecs_json_value_ser_ctx_t){0};
            } else {
                value_ctx = ecs_vec_get_t(
                    values_ctx, ecs_json_value_ser_ctx_t, index);
            }
        }
        if (flecs_json_serialize_component_value(world, cr->id, ptr, ti, buf,
            ser_ctx, value_ctx, desc, component_count))
        {
            return -1;
        }
    }
    if (component_count[0]) {
        flecs_json_object_pop(buf);
    }
    return 0;
}

static int flecs_json_serialize_table_inherited_type(
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

static int flecs_json_serialize_table_inherited(
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

    ecs_vec_t values_ctx;
    ecs_vec_init_t(NULL, &values_ctx, ecs_json_value_ser_ctx_t, 0);
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
            world, table, NULL, e, buf, ser_ctx, &values_ctx, desc, i,
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

    ecs_json_value_ser_ctx_t *contexts = ecs_vec_first(&values_ctx);
    for (i = 0; i < ecs_vec_count(&values_ctx); i ++) {
        ecs_os_free(contexts[i].id_label);
    }
    ecs_vec_fini_t(NULL, &values_ctx, ecs_json_value_ser_ctx_t);
    return result;
}

#endif
