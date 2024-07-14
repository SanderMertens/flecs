/**
 * @file addons/json/serialize_iter_rows.c
 * @brief Serialize (component) values to JSON strings.
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
        flecs_json_path_or_label(buf, world, var, 
            desc ? desc->serialize_full_paths : false);
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

    ecs_id_record_t *idr = flecs_id_record_get(world, 
        ecs_pair_t(EcsPoly, EcsQuery));

    if (idr) {
        ecs_table_cache_iter_t it;
        if (idr && flecs_table_cache_iter((ecs_table_cache_t*)idr, &it)) {
            const ecs_table_record_t *tr;
            while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
                ecs_table_t *table = tr->hdr.table;
                EcsPoly *queries = ecs_table_get_column(table, tr->column, 0);

                int32_t i, count = ecs_table_count(table);
                ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
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
                    if (!qit.variables) {
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
int flecs_json_serialize_refs_idr(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_id_record_t *idr)
{
    char *id_str = ecs_id_str(world, ecs_pair_first(world, idr->id));

    flecs_json_member(buf, id_str);
    ecs_os_free(id_str);

    flecs_json_array_push(buf);

    ecs_table_cache_iter_t it;
    if (idr && flecs_table_cache_all_iter((ecs_table_cache_t*)idr, &it)) {
        const ecs_table_record_t *tr;
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_table_t *table = tr->hdr.table;
            int32_t i, count = ecs_table_count(table);
            ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
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

    ecs_id_record_t *idr = flecs_id_record_get(world, 
        ecs_pair(relationship, entity));

    if (idr) {
        if (relationship == EcsWildcard) {
            ecs_id_record_t *cur = idr;
            while ((cur = cur->second.next)) {
                flecs_json_serialize_refs_idr(world, buf, cur);
            }
        } else {
            flecs_json_serialize_refs_idr(world, buf, idr);
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

void flecs_json_serialize_iter_this(
    const ecs_iter_t *it,
    const char *parent_path,
    const ecs_json_this_data_t *this_data,
    int32_t row,
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc)
{
    ecs_assert(row < it->count, ECS_INTERNAL_ERROR, NULL);

    if (parent_path) {
        flecs_json_memberl(buf, "parent");
        flecs_json_string(buf, parent_path);
    }

    flecs_json_memberl(buf, "name");
    if (this_data->names) {
        flecs_json_string(buf, this_data->names[row].value);
    } else {
        ecs_strbuf_appendlit(buf, "\"#");
        ecs_strbuf_appendint(buf, flecs_uto(int64_t, 
            (uint32_t)it->entities[row]));
        ecs_strbuf_appendlit(buf, "\"");
    }

    if (desc && desc->serialize_entity_ids) {
        flecs_json_memberl(buf, "id");
        flecs_json_u32(buf, (uint32_t)this_data->ids[row]);
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
        flecs_json_memberl(buf, "alerts");
        flecs_json_true(buf);
    }
}

int flecs_json_serialize_iter_result(
    const ecs_world_t *world, 
    const ecs_iter_t *it, 
    ecs_strbuf_t *buf,
    const ecs_iter_to_json_desc_t *desc,
    ecs_json_ser_ctx_t *ser_ctx)
{
    char *parent_path = NULL;
    ecs_json_this_data_t this_data = {0};

    int32_t count = it->count;
    bool has_this = true;
    if (!count) {
        count = 1; /* Query without this variable */
        has_this = false;
    } else {
        ecs_table_t *table = it->table;
        if (table) {
            this_data.ids = &flecs_table_entities_array(table)[it->offset];

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
            /* Very rare case, but could happen if someone's using an iterator
             * to return empty entities. */
        }
    }

    if (desc && desc->serialize_table) {
        if (flecs_json_serialize_iter_result_table(world, it, buf, 
            desc, count, has_this, parent_path, &this_data))
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
