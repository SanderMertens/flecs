/**
 * @file addons/json/serialize_iter_result.c
 * @brief Serialize iterator result to JSON.
 */

#include "json.h"

#ifdef FLECS_JSON

static
bool flecs_json_skip_variable(
    const char *name)
{
    return !name || name[0] == '_' || !ecs_os_strcmp(name, "this");
}

bool flecs_json_serialize_vars(
    const ecs_world_t *world,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    if (!it->query) {
        return 0;
    }

    char **variable_names = it->query->vars;
    int32_t var_count = it->query->var_count;
    int32_t actual_count = 0;

    for (int i = 1; i < var_count; i ++) {
        const char *var_name = variable_names[i];
        if (flecs_json_skip_variable(var_name)) continue;

        ecs_entity_t var = ecs_iter_get_vars(it)[i].entity;
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
        flecs_json_path_or_label(buf, world, var, 
            desc ? desc->serialize_full_paths : true);
    }

    if (actual_count) {
        flecs_json_object_pop(buf);
    }

    return actual_count != 0;
}

int flecs_json_serialize_matches(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_entity_t entity)
{
    flecs_json_memberl(buf, "matches");
    flecs_json_array_push(buf);

    ecs_component_record_t *cr = flecs_components_get(world, 
        ecs_pair_t(EcsPoly, EcsQuery));

    if (cr) {
        ecs_table_cache_iter_t it;
        if (cr && flecs_table_cache_iter((ecs_table_cache_t*)cr, &it, EcsTableNotEmpty)) {
            const ecs_table_cache_elem_t *elem;
            while ((elem = flecs_table_cache_next(&it))) {
                ecs_table_t *table = elem->table;
                EcsPoly *queries = ecs_table_get_column(table, elem->column, 0);

                const ecs_entity_t *entities = ecs_table_entities(table);
                int32_t i, count = ecs_table_count(table);
                for (i = 0; i < count; i ++) {
                    ecs_query_t *q = queries[i].poly;
                    if (!q) {
                        continue;
                    }

                    ecs_assert(flecs_poly_is(q, ecs_query_t), 
                        ECS_INTERNAL_ERROR, NULL);

                    if (!(q->flags & EcsQueryMatchThis)) {
                        continue;
                    }

                    ecs_iter_t qit = ecs_query_iter(world, q);
                    if (!ecs_iter_get_vars(&qit)) {
                        ecs_iter_fini(&qit);
                        continue;
                    }

                    ecs_iter_set_var(&qit, 0, entity);
                    if (ecs_iter_is_true(&qit)) {
                        flecs_json_next(buf);
                        flecs_json_path(buf, world, entities[i]);
                    }
                }
            }
        }
    }

    flecs_json_array_pop(buf);
    
    return 0;
}

static
int flecs_json_serialize_refs_cr(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_component_record_t *cr)
{
    char *id_str = ecs_id_str(world, ecs_pair_first(world, cr->id));

    flecs_json_member(buf, id_str);
    ecs_os_free(id_str);

    flecs_json_array_push(buf);

    ecs_table_cache_iter_t it;
    if (cr && flecs_table_cache_iter((ecs_table_cache_t*)cr, &it, EcsTableEmpty|EcsTableNotEmpty)) {
        const ecs_table_cache_elem_t *elem;
        while ((elem = flecs_table_cache_next(&it))) {
            ecs_table_t *table = elem->table;
            const ecs_entity_t *entities = ecs_table_entities(table);
            int32_t i, count = ecs_table_count(table);
            for (i = 0; i < count; i ++) {
                ecs_entity_t e = entities[i];
                flecs_json_next(buf);
                flecs_json_path(buf, world, e);
            }
        }
    }

    flecs_json_array_pop(buf);

    return 0;
}

int flecs_json_serialize_refs(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_entity_t entity,
    ecs_entity_t relationship)
{
    flecs_json_memberl(buf, "refs");
    flecs_json_object_push(buf);

    ecs_component_record_t *cr = flecs_components_get(world, 
        ecs_pair(relationship, entity));

    if (cr) {
        if (relationship == EcsWildcard) {
            ecs_component_record_t *cur = cr;
            while ((cur = flecs_component_second_next(cur))) {
                flecs_json_serialize_refs_cr(world, buf, cur);
            }
        } else {
            flecs_json_serialize_refs_cr(world, buf, cr);
        }
    }

    flecs_json_object_pop(buf);
    
    return 0;
}

#ifdef FLECS_ALERTS
static
int flecs_json_serialize_entity_alerts(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_entity_t entity,
    const EcsAlertsActive *alerts,
    bool self)
{
    ecs_assert(alerts != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t it = ecs_map_iter(&alerts->alerts);
    while (ecs_map_next(&it)) {
        flecs_json_next(buf);
        flecs_json_object_push(buf);
        ecs_entity_t ai = ecs_map_value(&it);
        char *alert_name = ecs_get_path(world, ai);
        flecs_json_memberl(buf, "alert");
        flecs_json_string(buf, alert_name);
        ecs_os_free(alert_name);

        ecs_entity_t severity_id = ecs_get_target(
            world, ai, ecs_id(EcsAlert), 0);
        const char *severity = ecs_get_name(world, severity_id);

        const EcsAlertInstance *alert = ecs_get(
            world, ai, EcsAlertInstance);
        if (alert) {
            if (alert->message) {
                flecs_json_memberl(buf, "message");
                flecs_json_string(buf, alert->message);
            }
            flecs_json_memberl(buf, "severity");
            flecs_json_string(buf, severity);
            
            if (!self) {
                char *path = ecs_get_path(world, entity);
                flecs_json_memberl(buf, "path");
                flecs_json_string(buf, path);
                ecs_os_free(path);
            }
        }
        flecs_json_object_pop(buf);
    }

    return 0;
}

static
int flecs_json_serialize_children_alerts(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_entity_t entity)
{
    ecs_query_t *q = ecs_query(ECS_CONST_CAST(ecs_world_t*, world), {
        .terms = {{ .id = ecs_pair(EcsChildOf, entity) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        EcsAlertsActive *alerts = ecs_table_get_id(
            world, it.table, ecs_id(EcsAlertsActive), it.offset);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_entity_t child = it.entities[i];
            if (alerts) {
                if (flecs_json_serialize_entity_alerts(
                    world, buf, child, &alerts[i], false))
                {
                    goto error;
                }
            }

            ecs_record_t *r = flecs_entities_get(world, it.entities[i]);
            if (r->row & EcsEntityIsTraversable) {
                if (flecs_json_serialize_children_alerts(
                    world, buf, child))
                {
                    goto error;
                }
            }
        }
    }

    ecs_query_fini(q);

    return 0;
error:
    return -1;
}
#endif

int flecs_json_serialize_alerts(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_entity_t entity)
{
    (void)world;
    (void)buf;
    (void)entity;

#ifdef FLECS_ALERTS
    if (!ecs_id(EcsAlertsActive)) {
        return 0; /* Alert module not imported */
    }

    flecs_json_memberl(buf, "alerts");
    flecs_json_array_push(buf);
    const EcsAlertsActive *alerts = ecs_get(world, entity, EcsAlertsActive);
    if (alerts) {
        flecs_json_serialize_entity_alerts(world, buf, entity, alerts, true);
    }
    flecs_json_serialize_children_alerts(world, buf, entity);
    flecs_json_array_pop(buf);
#endif
    return 0;
}

/* Check if type is a struct that only contains primitive members that don't
 * require special serialization logic. Serialization of such types can skip
 * the serializer instruction walk and directly emit precomputed member
 * prefixes and values. */
static
void flecs_json_init_flat_struct(
    ecs_json_value_ser_ctx_t *ctx)
{
    const ecs_vec_t *v_ops = &ctx->ser->ops;
    ecs_meta_op_t *ops = ecs_vec_first_t(v_ops, ecs_meta_op_t);
    int32_t i, count = ecs_vec_count(v_ops);

    if (count < 3) {
        return;
    }
    if (ops[0].kind != EcsOpPushStruct || ops[0].op_count != count) {
        return;
    }
    if (ops[count - 1].kind != EcsOpPop) {
        return;
    }

    int32_t member_count = count - 2;
    for (i = 1; i <= member_count; i ++) {
        ecs_meta_op_t *op = &ops[i];
        if (!op->name || op->op_count != 1) {
            return;
        }
        switch(op->kind) {
        case EcsOpBool:
        case EcsOpByte:
        case EcsOpU8:
        case EcsOpU16:
        case EcsOpU32:
        case EcsOpI8:
        case EcsOpI16:
        case EcsOpI32:
        case EcsOpF32:
        case EcsOpF64:
            break;
        default:
            return;
        }
        if (op->name_len > 18) {
            return;
        }
    }

    ecs_json_flat_member_t *members = ecs_os_malloc_n(
        ecs_json_flat_member_t, member_count);

    for (i = 0; i < member_count; i ++) {
        ecs_meta_op_t *op = &ops[i + 1];
        ecs_json_flat_member_t *m = &members[i];
        m->kind = op->kind;
        m->offset = op->offset;

        char *prefix = m->prefix;
        if (!i) {
            *prefix++ = '{';
        } else {
            *prefix++ = ',';
            *prefix++ = ' ';
        }
        *prefix++ = '"';
        ecs_os_memcpy(prefix, op->name, op->name_len);
        prefix += op->name_len;
        *prefix++ = '"';
        *prefix++ = ':';
        m->prefix_len = flecs_ito(int32_t, prefix - m->prefix);
    }

    ctx->flat_members = members;
    ctx->flat_count = member_count;
}

int flecs_json_ser_value_ctx(
    const ecs_world_t *world,
    const ecs_json_value_ser_ctx_t *ctx,
    const void *ptr,
    ecs_strbuf_t *buf)
{
    int32_t i, j, count = ctx->flat_count;
    if (!count) {
        return flecs_json_ser_type(world, &ctx->ser->ops, ptr, buf);
    }

    char *dst = flecs_strbuf_reserve(buf, count * 88 + 1);
    char *p = dst;

    const ecs_json_flat_member_t *members = ctx->flat_members;
    for (i = 0; i < count; i ++) {
        const ecs_json_flat_member_t *m = &members[i];
        for (j = 0; j < m->prefix_len; j ++) {
            p[j] = m->prefix[j];
        }
        p += m->prefix_len;

        const void *mptr = ECS_OFFSET(ptr, m->offset);
        switch(m->kind) {
        case EcsOpBool:
            if (*(const bool*)mptr) {
                p[0] = 't'; p[1] = 'r'; p[2] = 'u'; p[3] = 'e';
                p += 4;
            } else {
                p[0] = 'f'; p[1] = 'a'; p[2] = 'l'; p[3] = 's'; p[4] = 'e';
                p += 5;
            }
            break;
        case EcsOpByte:
        case EcsOpU8:
            p = flecs_itoa(p, flecs_uto(int64_t, *(const uint8_t*)mptr));
            break;
        case EcsOpU16:
            p = flecs_itoa(p, flecs_uto(int64_t, *(const uint16_t*)mptr));
            break;
        case EcsOpU32:
            p = flecs_itoa(p, flecs_uto(int64_t, *(const uint32_t*)mptr));
            break;
        case EcsOpI8:
            p = flecs_itoa(p, flecs_ito(int64_t, *(const int8_t*)mptr));
            break;
        case EcsOpI16:
            p = flecs_itoa(p, flecs_ito(int64_t, *(const int16_t*)mptr));
            break;
        case EcsOpI32:
            p = flecs_itoa(p, flecs_ito(int64_t, *(const int32_t*)mptr));
            break;
        case EcsOpF32:
            p = flecs_ftoa(p, (ecs_f64_t)*(const ecs_f32_t*)mptr, 10, '"');
            break;
        case EcsOpF64:
            p = flecs_ftoa(p, *(const ecs_f64_t*)mptr, 10, '"');
            break;
        default:
            ecs_throw(ECS_INTERNAL_ERROR, "invalid flat member kind");
        }
    }

    *p++ = '}';
    buf->length += flecs_ito(int32_t, p - dst);
    return 0;
error:
    return -1;
}

void flecs_json_value_ser_ctx_fini(
    ecs_json_value_ser_ctx_t *ctx)
{
    ecs_os_free(ctx->id_label);
    ecs_os_free(ctx->flat_members);
}

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
            desc ? desc->serialize_full_paths : true);
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

        flecs_json_init_flat_struct(ctx);

        return true;
    } else {
        return ctx->ser != NULL;
    }
}

bool flecs_json_serialize_iter_this(
    const ecs_iter_t *it,
    const char *parent_path,
    const ecs_json_this_data_t *this_data,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    ecs_assert(row < it->count, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = it->real_world;

    ecs_assert(this_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(this_data->ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t e = this_data->ids[row];
    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

    /* Skip entity if it already has been serialized */
    if (!flecs_json_should_serialize(e, it, ser_ctx)) {
        return false;
    }

    flecs_json_mark_serialized(e, ser_ctx);

    /* Serialize parent first. Only necessary if this is a table with Parent
     * component, as this will have been done at the table level for fragmenting
     * relationships. */
    ecs_entity_t parent = 0;
    if (this_data->parents) {
        parent = this_data->parents[row].value;
        ecs_assert(parent != 0, ECS_INTERNAL_ERROR, NULL);
        if (ecs_map_is_init(&ser_ctx->serialized)) {
            flecs_entity_to_json_buf(world, parent, buf, desc, ser_ctx);
        }
    }

    flecs_json_next(buf);
    flecs_json_object_push(buf);

    if (parent_path) {
        flecs_json_memberl(buf, "parent");
        flecs_json_string(buf, parent_path);
    } else if (parent) {        
        flecs_json_memberl(buf, "parent");
        char *path = ecs_get_path_w_sep(world, 0, parent, ".", "");
        flecs_json_string(buf, path);
        ecs_os_free(path);
    }

    flecs_json_memberl(buf, "name");
    if (this_data->names) {
        if (this_data->table) {
            ecs_assert(this_data->names[row].value != NULL, 
                ECS_INTERNAL_ERROR, NULL);
            flecs_json_string(buf, this_data->names[row].value);
        }
    } else {
        bool name_set = false;
        if (!this_data->table) {
            const char *name = ecs_get_name(world, e);
            if (name) {
                flecs_json_string(buf, name);
                name_set = true;
            }
        }

        if (!name_set) {
            ecs_strbuf_appendlit(buf, "\"#");
            ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
                (uint32_t)it->entities[row]));
            ecs_strbuf_appendlit(buf, "\"");
        }
    }

    if (desc && desc->serialize_entity_ids) {
        flecs_json_memberl(buf, "id");
        flecs_json_u32(buf, (uint32_t)e);
        if (e != (uint32_t)e) {
            flecs_json_memberl(buf, "version");
            flecs_json_u32(buf, (uint32_t)(e >> 32));
        }
    }

#ifdef FLECS_DOC
    if (desc && desc->serialize_doc) {
        flecs_json_memberl(buf, "doc");
        flecs_json_object_push(buf);
        if (this_data->label) {
            flecs_json_memberl(buf, "label");
            flecs_json_string_escape(buf, this_data->label[row].value);
        } else {
            flecs_json_memberl(buf, "label");
            if (this_data->names) {
                ecs_assert(this_data->names[row].value != NULL, 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_json_string(buf, this_data->names[row].value);
            } else {
                ecs_strbuf_appendlit(buf, "\"#");
                ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
                    (uint32_t)it->entities[row]));
                ecs_strbuf_appendlit(buf, "\"");
            }
        }

        if (this_data->brief) {
            flecs_json_memberl(buf, "brief");
            flecs_json_string_escape(buf, this_data->brief[row].value);
        }

        if (this_data->detail) {
            flecs_json_memberl(buf, "detail");
            flecs_json_string_escape(buf, this_data->detail[row].value);
        }

        if (this_data->color) {
            flecs_json_memberl(buf, "color");
            flecs_json_string_escape(buf, this_data->color[row].value);
        }

        if (this_data->link) {
            flecs_json_memberl(buf, "link");
            flecs_json_string_escape(buf, this_data->link[row].value);
        }

        flecs_json_object_pop(buf);
    }
#endif

    if (this_data->has_alerts) {
        flecs_json_memberl(buf, "has_alerts");
        flecs_json_true(buf);
    }

    return true;
}

int flecs_json_serialize_iter_result(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    char *parent_path = NULL;
    ecs_json_this_data_t this_data = { .table = it->table };

    int32_t count = it->count;
    bool has_this = true;
    if (!count) {
        count = 1; /* Query without this variable */
        has_this = false;
    } else {
        ecs_table_t *table = it->table;
        if (table) {
            ecs_assert((it->offset + it->count) <= ecs_table_count(table), 
                ECS_INTERNAL_ERROR, NULL);
            this_data.ids = &ecs_table_entities(table)[it->offset];

            /* Get path to parent once for entire table */
            if (table->flags & EcsTableHasChildOf) {
                const ecs_table_record_t *tr = flecs_component_get_table(
                    world->cr_childof_wildcard, table);
                ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_entity_t parent = ecs_pair_second(
                    world, table->type.array[tr->index]);
                parent_path = ecs_get_path_w_sep(world, 0, parent, ".", "");

                /* If matching a query check if we need to serialize the parent
                 * of the table before its children. Don't do this if query 
                 * isn't set, as that means that we're serializing a single
                 * entity. */
                if (ecs_map_is_init(&ser_ctx->serialized)) {
                    if (flecs_json_should_serialize(parent, it, ser_ctx)) {
                        if (flecs_entity_to_json_buf(
                            world, parent, buf, desc, ser_ctx)) 
                        {
                            return -1;
                        }

                        flecs_json_mark_serialized(parent, ser_ctx);
                    }
                }
            }

            /* Fetch name column once vs. calling ecs_get_name for each row */
            if (table->flags & EcsTableHasName) {
                this_data.names = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsIdentifier, EcsName), it->offset);
            }

            /* Same for Parent column */
            if (table->flags & EcsTableHasParent) {
                this_data.parents = ecs_table_get_id(it->world, it->table,
                    ecs_id(EcsParent), it->offset);
            }

            /* Get entity labels */
#ifdef FLECS_DOC
            if (desc && desc->serialize_doc) {
                this_data.label = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsName), it->offset);
                this_data.brief = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsDocBrief), it->offset);
                this_data.detail = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsDocDetail), it->offset);
                this_data.color = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsDocColor), it->offset);
                this_data.link = ecs_table_get_id(it->world, it->table, 
                    ecs_pair_t(EcsDocDescription, EcsDocLink), it->offset);
            }
#endif

#ifdef FLECS_ALERTS
        if (it->table && (ecs_id(EcsAlertsActive) != 0)) {
            /* Only add field if alerts addon is imported */
            if (ecs_table_has_id(world, table, ecs_id(EcsAlertsActive))) {
                this_data.has_alerts = true;
            }
        }
#endif
        } else {
            this_data.ids = it->entities;
        }
    }

    if (desc && desc->serialize_table) {
        if (flecs_json_serialize_iter_result_table(world, it, buf, 
            desc, ser_ctx, count, has_this, parent_path, &this_data))
        {
            goto error;
        }
    } else {
        if (flecs_json_serialize_iter_result_query(world, it, buf, ser_ctx, 
            desc, count, has_this, parent_path, &this_data))
        {
            goto error;
        }
    }

    ecs_os_free(parent_path);
    return 0;
error:
    ecs_os_free(parent_path);
    return -1;
}

#endif
