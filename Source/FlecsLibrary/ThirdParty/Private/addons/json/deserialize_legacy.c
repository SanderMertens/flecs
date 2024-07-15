/**
 * @file addons/json/deserialize_legacy.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"
#include "json.h"
#include <ctype.h>

#ifdef FLECS_JSON

typedef struct {
    ecs_allocator_t *a;
    ecs_vec_t records;
    ecs_vec_t result_ids;
    ecs_vec_t columns_set;
    ecs_map_t anonymous_ids;
    ecs_map_t missing_reflection;
} ecs_from_json_ctx_legacy_t;

static
void flecs_from_json_ctx_init_legacy(
    ecs_allocator_t *a,
    ecs_from_json_ctx_legacy_t *ctx)
{
    ctx->a = a;
    ecs_vec_init_t(a, &ctx->records, ecs_record_t*, 0);
    ecs_vec_init_t(a, &ctx->result_ids, ecs_id_t, 0);
    ecs_vec_init_t(a, &ctx->columns_set, ecs_id_t, 0);
    ecs_map_init(&ctx->anonymous_ids, a);
    ecs_map_init(&ctx->missing_reflection, a);
}

static
void flecs_from_json_ctx_fini_legacy(
    ecs_from_json_ctx_legacy_t *ctx)
{
    ecs_vec_fini_t(ctx->a, &ctx->records, ecs_record_t*);
    ecs_vec_fini_t(ctx->a, &ctx->result_ids, ecs_record_t*);
    ecs_vec_fini_t(ctx->a, &ctx->columns_set, ecs_id_t);
    ecs_map_fini(&ctx->anonymous_ids);
    ecs_map_fini(&ctx->missing_reflection);
}

static
const char* flecs_json_parse_path_legacy(
    const ecs_world_t *world,
    const char *json,
    char *token,
    ecs_entity_t *out,
    const ecs_from_json_desc_t *desc)
{
    char *path = NULL;
    json = flecs_json_expect_string(json, token, &path, desc);
    if (!json) {
        goto error;
    }

    ecs_entity_t result = ecs_lookup(world, path);
    if (!result) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "unresolved identifier '%s'", path);
        goto error;
    }

    *out = result;

    if (path != token) {
        ecs_os_free(path);
    }

    return json;
error:
    if (path != token) {
        ecs_os_free(path);
    }

    return NULL;
}

const char* ecs_entity_from_json_legacy(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc_param)
{
    ecs_json_token_t token_kind = 0;
    char token[ECS_MAX_TOKEN_SIZE];

    ecs_from_json_desc_t desc = {0};

    const char *name = NULL, *expr = json, *ids = NULL, *values = NULL, *lah;
    if (desc_param) {
        desc = *desc_param;
    }

    json = flecs_json_expect(json, JsonObjectOpen, token, &desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (!lah) {
        goto error;
    }

    if (token_kind == JsonObjectClose) {
        return lah;
    }

    json = flecs_json_expect_member(json, token, &desc);
    if (!json) {
        return NULL;
    }

    if (!ecs_os_strcmp(token, "path")) {
        char *path = NULL;
        json = flecs_json_expect_string(json, token, &path, &desc);
        if (!json) {
            goto error;
        }

        ecs_add_fullpath(world, e, path);

        if (path != token) {
            ecs_os_free(path);
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (!json) {
            goto error;
        }

        if (token_kind == JsonObjectClose) {
            return json;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr, "unexpected character");
            goto error;
        }

        json = flecs_json_expect_member_name(json, token, "ids", &desc);
        if (!json) {
            goto error;
        }
    } else if (ecs_os_strcmp(token, "ids")) {
        ecs_parser_error(name, expr, json - expr, "expected member 'ids'");
        goto error;
    }

    json = flecs_json_expect(json, JsonArrayOpen, token, &desc);
    if (!json) {
        goto error;
    }

    ids = json;

    json = flecs_json_skip_array(json, token, &desc);
    if (!json) {
        return NULL;
    }

    json = flecs_json_parse(json, &token_kind, token);
    if (token_kind != JsonObjectClose) {
        if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr, "expected ','");
            goto error;
        }

        json = flecs_json_expect_member_name(json, token, "values", &desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(json, JsonArrayOpen, token, &desc);
        if (!json) {
            goto error;
        }

        values = json;
    }

    do {
        ecs_entity_t first = 0, second = 0, type_id = 0;
        ecs_id_t id;

        ids = flecs_json_parse(ids, &token_kind, token);
        if (!ids) {
            goto error;
        }

        if (token_kind == JsonArrayClose) {
            if (values) {
                if (values[0] != ']') {
                    ecs_parser_error(name, expr, values - expr, "expected ']'");
                    goto error;
                }
                json = flecs_parse_ws_eol(values + 1);
            } else {
                json = ids;
            }

            break;
        } else if (token_kind == JsonArrayOpen) {
            ids = flecs_json_parse_path_legacy(world, ids, token, &first, &desc);
            if (!ids) {
                goto error;
            }

            ids = flecs_json_parse(ids, &token_kind, token);
            if (!ids) {
                goto error;
            }

            if (token_kind == JsonComma) {
                /* Id is a pair*/
                ids = flecs_json_parse_path_legacy(world, ids, token, &second, &desc);
                if (!ids) {
                    goto error;
                }

                ids = flecs_json_expect(ids, JsonArrayClose, token, &desc);
                if (!ids) {
                    goto error;
                }
            } else if (token_kind != JsonArrayClose) {
                ecs_parser_error(name, expr, ids - expr, "expected ',' or ']'");
                goto error;
            }

            lah = flecs_json_parse(ids, &token_kind, token);
            if (!lah) {
                goto error;
            }

            if (token_kind == JsonComma) {
                ids = lah;
            } else if (token_kind != JsonArrayClose) {
                ecs_parser_error(name, expr, lah - expr, "expected ',' or ']'");
                goto error;
            }
        } else {
            ecs_parser_error(name, expr, lah - expr, "expected '[' or ']'");
            goto error;
        }

        if (second) {
            id = ecs_pair(first, second);
            type_id = ecs_get_typeid(world, id);
            if (!type_id) {
                ecs_parser_error(name, expr, ids - expr, "id is not a type");
                goto error;
            }
        } else {
            id = first;
            type_id = first;
        }

        /* Get mutable pointer */
        void *comp_ptr = ecs_ensure_id(world, e, id);
        if (!comp_ptr) {
            char *idstr = ecs_id_str(world, id);
            ecs_parser_error(name, expr, json - expr, 
                "id '%s' is not a valid component", idstr);
            ecs_os_free(idstr);
            goto error;
        }

        if (values) {
            ecs_from_json_desc_t parse_desc = {
                .name = name,
                .expr = expr,
            };

            values = ecs_ptr_from_json(
                world, type_id, comp_ptr, values, &parse_desc);
            if (!values) {
                goto error;
            }

            lah = flecs_json_parse(values, &token_kind, token);
            if (!lah) {
                goto error;
            }

            if (token_kind == JsonComma) {
                values = lah;
            } else if (token_kind != JsonArrayClose) {
                ecs_parser_error(name, expr, json - expr, 
                    "expected ',' or ']'");
                goto error;
            } else {
                values = flecs_parse_ws_eol(values);
            }

            ecs_modified_id(world, e, id);
        }
    } while(ids[0]);

    return flecs_json_expect(json, JsonObjectClose, token, &desc);
error:
    return NULL;
}

static
ecs_entity_t flecs_json_new_id_legacy(
    ecs_world_t *world,
    ecs_entity_t ser_id)
{
    /* Try to honor low id requirements */
    if (ser_id < FLECS_HI_COMPONENT_ID) {
        return ecs_new_low_id(world);
    } else {
        return ecs_new(world);
    }
}

static
ecs_entity_t flecs_json_lookup_legacy(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name,
    const ecs_from_json_desc_t *desc)
{
    ecs_entity_t scope = 0;
    if (parent) {
        scope = ecs_set_scope(world, parent);
    }
    ecs_entity_t result = desc->lookup_action(world, name, desc->lookup_ctx);
    if (parent) {
        ecs_set_scope(world, scope);
    }
    return result;
}

static
void flecs_json_mark_reserved_legacy(
    ecs_map_t *anonymous_ids,
    ecs_entity_t e)
{
    ecs_entity_t *reserved = ecs_map_ensure(anonymous_ids, e);
    ecs_assert(reserved[0] == 0, ECS_INTERNAL_ERROR, NULL);
    reserved[0] = 0;
}

static
bool flecs_json_name_is_anonymous_legacy(
    const char *name)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    if (isdigit(name[0])) {
        const char *ptr;
        for (ptr = name + 1; *ptr; ptr ++) {
            if (!isdigit(*ptr)) {
                break;
            }
        }
        if (!(*ptr)) {
            return true;
        }
    }
    return false;
}

static
ecs_entity_t flecs_json_ensure_entity_legacy(
    ecs_world_t *world,
    const char *name,
    ecs_map_t *anonymous_ids)
{
    ecs_entity_t e = 0;

    if (flecs_json_name_is_anonymous_legacy(name)) {
        /* Anonymous entity, find or create mapping to new id */
        ecs_entity_t ser_id = flecs_ito(ecs_entity_t, atoll(name));
        ecs_entity_t *deser_id = ecs_map_get(anonymous_ids, ser_id);
        if (deser_id) {
            if (!deser_id[0]) {
                /* Id is already issued by deserializer, create new id */
                deser_id[0] = flecs_json_new_id_legacy(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved_legacy(anonymous_ids, deser_id[0]);
            } else {
                /* Id mapping exists */
            }
        } else {
            /* Id has not yet been issued by deserializer, which means it's safe
             * to use. This allows the deserializer to bind to existing 
             * anonymous ids, as they will never be reissued. */
            deser_id = ecs_map_ensure(anonymous_ids, ser_id);
            if (!ecs_exists(world, ser_id) || 
               (ecs_is_alive(world, ser_id) && !ecs_get_name(world, ser_id))) 
            {
                /* Only use existing id if it's alive or doesn't exist yet. The 
                 * id could have been recycled for another entity 
                 * Also don't use existing id if the existing entity is not
                 * anonymous. */
                deser_id[0] = ser_id;
                ecs_make_alive(world, ser_id);
            } else {
                /* If id exists and is not alive, create a new id */
                deser_id[0] = flecs_json_new_id_legacy(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved_legacy(anonymous_ids, deser_id[0]);
            }
        }

        e = deser_id[0];
    } else {
        e = ecs_lookup_path_w_sep(world, 0, name, ".", NULL, false);
        if (!e) {
            e = ecs_entity(world, { .name = name });
            flecs_json_mark_reserved_legacy(anonymous_ids, e);
        }
    }

    return e;
}

static
ecs_table_t* flecs_json_parse_table_legacy(
    ecs_world_t *world,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;

    ecs_vec_clear(&ctx->result_ids);

    do {
        ecs_id_t id = 0;
        json = flecs_json_expect(json, JsonArrayOpen, token, desc);
        if (!json) {
            goto error;
        }

        char *first_name = NULL;
        json = flecs_json_expect_string(json, token, &first_name, desc);
        if (!json) {
            goto error;
        }

        ecs_entity_t first = flecs_json_lookup_legacy(world, 0, first_name, desc);
        if (first_name != token) {
            ecs_os_free(first_name);
        }

        if (!first) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonComma) {
            char *second_name = NULL;
            json = flecs_json_expect_string(json, token, &second_name, desc);
            if (!json) {
                goto error;
            }

            ecs_entity_t second = flecs_json_lookup_legacy(world, 0, second_name, desc);
            if (second_name != token) {
                ecs_os_free(second_name);
            }

            if (!second) {
                goto error;
            }

            id = ecs_pair(first, second);

            json = flecs_json_expect(json, JsonArrayClose, token, desc);
            if (!json) {
                goto error;
            }
        } else if (token_kind == JsonArrayClose) {
            id = first;
        } else {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ',' or ']");
            goto error;
        }

        ecs_vec_append_t(ctx->a, &ctx->result_ids, ecs_id_t)[0] = id;

        const char *lah = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonComma) {
            json = lah;
        } else if (token_kind == JsonArrayClose) {
            break;
        } else {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ',' or ']'");
            goto error;
        }
    } while (json[0]);

    /* Make a copy of the ids array because we need the original order for 
     * deserializing the component values, and the sorted order for finding or
     * creating the table. */
    ecs_vec_t id_copy = ecs_vec_copy_t(ctx->a, &ctx->result_ids, ecs_id_t);
    ecs_type_t type = { 
        .array = ecs_vec_first(&id_copy),
        .count = ecs_vec_count(&id_copy)
    };

    qsort(type.array, flecs_itosize(type.count), sizeof(ecs_id_t), 
        flecs_id_qsort_cmp);

    ecs_table_t *table = flecs_table_find_or_create(world, &type);
    if (!table) {
        goto error;
    }

    ecs_vec_fini_t(ctx->a, &id_copy, ecs_id_t);

    return table;
error:
    return NULL;
}

static
void flecs_json_zeromem_table_legacy(
    ecs_table_t *table)
{
    int32_t count = ecs_vec_count(&table->data.entities);
    int32_t i, column_count = table->column_count;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &table->data.columns[i];
        ecs_type_info_t *ti = column->ti;
        if (!ti->hooks.ctor) {
            void *ptr = ecs_vec_first(&column->data);
            ecs_os_memset(ptr, 0, ti->size * count);
        }
    }
}

static
int flecs_json_parse_entities_legacy(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t parent,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    char name_token[ECS_MAX_TOKEN_SIZE];
    ecs_json_token_t token_kind = 0;
    ecs_vec_clear(&ctx->records);

    do {
        char *name = NULL;
        json = flecs_json_parse(json, &token_kind, name_token);
        if (!json) {
            goto error;
        }

        if (token_kind == JsonLargeString) {
            ecs_strbuf_t large_token = ECS_STRBUF_INIT;
            json = flecs_json_parse_large_string(json, &large_token);
            if (!json) {
                break;
            }

            name = ecs_strbuf_get(&large_token);
            token_kind = JsonString;
        } else {
            name = name_token;
        }

        if ((token_kind != JsonNumber) && (token_kind != JsonString)) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected number or string");
            goto error;
        }

        ecs_entity_t e = flecs_json_lookup_legacy(world, parent, name, desc);
        ecs_record_t *r = flecs_entities_try(world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

        if (r->table != table) {
            bool cleared = false;
            if (r->table) {
                ecs_commit(world, e, r, r->table, NULL, &r->table->type);
                cleared = true;
            }

            ecs_commit(world, e, r, table, &table->type, NULL);
            if (cleared) {
                char *entity_name = strrchr(name, '.');
                if (entity_name) {
                    entity_name ++;
                } else {
                    entity_name = name;
                }
                if (!flecs_json_name_is_anonymous_legacy(entity_name)) {
                    ecs_set_name(world, e, entity_name);
                }
            }
        }

        if (name != name_token) {
            ecs_os_free(name);
        }

        if (table != r->table) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "invalid entity identifier");
            goto error;
        } else {
            ecs_assert(table == r->table, ECS_INTERNAL_ERROR, NULL);
            ecs_record_t** elem = ecs_vec_append_t(
                ctx->a, &ctx->records, ecs_record_t*);
            *elem = r;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ',' or ']'");
            goto error;
        }
    } while(json[0]);

    return 0;
error:
    return -1;
}

static
const char* flecs_json_missing_reflection_legacy(
    ecs_world_t *world,
    ecs_id_t id,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    if (!desc->strict && ecs_map_get(&ctx->missing_reflection, id)) {
        json = flecs_json_skip_array(json, token, desc);
        goto done;
    }

    /* Don't spam log when multiple values of a type can't be deserialized */
    ecs_map_ensure(&ctx->missing_reflection, id);

    char *id_str = ecs_id_str(world, id);
    ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
        "missing reflection for '%s'", id_str);
    ecs_os_free(id_str);

done:
    if (desc->strict) {
        return NULL;
    } else {
        return flecs_json_skip_array(json, token, desc);
    }
}

static
const char* flecs_json_parse_column_legacy(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    /* If deserializing id caused trouble before, don't bother trying again */
    if (!desc->strict && ecs_map_get(&ctx->missing_reflection, id)) {
        return flecs_json_skip_array(json, token, desc);
    }

    ecs_table_record_t *tr = flecs_table_record_get(world, table, id);
    /* Table was created with this id, it should exist */
    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);

    /* If id is not a component, reflection data is missing */
    int32_t column_index = tr->column;
    if (column_index == -1) {
        return flecs_json_missing_reflection_legacy(world, id, json, token, ctx, desc);
    }

    ecs_json_token_t token_kind = 0;
    ecs_column_t *column = &table->data.columns[column_index];
    ecs_type_info_t *ti = column->ti;
    ecs_size_t size = ti->size;
    ecs_entity_t type = ti->component;

    ecs_record_t **record_array = ecs_vec_first_t(&ctx->records, ecs_record_t*);
    int32_t entity = 0;
    bool values_set = false;
    const char *values_start = json;

    do {
        ecs_record_t *r = record_array[entity];
        int32_t row = ECS_RECORD_TO_ROW(r->row);

        void *ptr = ecs_vec_get(&column->data, size, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        json = ecs_ptr_from_json(world, type, ptr, json, desc);
        if (!json) {
            if (desc->strict) {
                break;
            } else {
                return flecs_json_missing_reflection_legacy(
                    world, id, values_start, token, ctx, desc);
            }
        } else {
            values_set = true;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected ',' or ']'");
        }

        entity ++;
    } while (json[0]);

    if (values_set) {
        ecs_id_t *id_set = ecs_vec_append_t(
            ctx->a, &ctx->columns_set, ecs_id_t);
        *id_set = id;
    }

    return json;
}

static
const char* flecs_json_parse_values_legacy(
    ecs_world_t *world,
    ecs_table_t *table,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    int32_t value = 0;

    ecs_vec_clear(&ctx->columns_set);

    do {
        if (value >= table->type.count) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "more elements in values array than expected for table");
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (!json) {
            goto error;
        }

        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind == JsonArrayOpen) {
            ecs_id_t *idptr = ecs_vec_get_t(&ctx->result_ids, ecs_id_t, value);
            ecs_assert(idptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_id_t id = idptr[0];
            
            json = flecs_json_parse_column_legacy(world, table, id,
                json, token, ctx, desc);
            if (!json) {
                goto error;
            }
        } else if (token_kind == JsonNumber) {
            if (!ecs_os_strcmp(token, "0")) {
                /* no data */
            } else {
                ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                    "unexpected number");
                goto error;
            }
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr,
                    "expected ',' or ']'");
            goto error;
        }

        value ++;
    } while (json[0]);

    /* Send OnSet notifications */
    ecs_defer_begin(world);
    ecs_type_t type = { 
        .array = ctx->columns_set.array, 
        .count = ctx->columns_set.count };

    int32_t table_count = ecs_table_count(table);
    int32_t i, record_count = ecs_vec_count(&ctx->records);

    /* If the entire table was inserted, send bulk notification */
    if (table_count == ecs_vec_count(&ctx->records)) {
        flecs_notify_on_set(
            world, table, 0, ecs_table_count(table), &type, true);
    } else {
        ecs_record_t **rvec = ecs_vec_first_t(&ctx->records, ecs_record_t*);
        for (i = 0; i < record_count; i ++) {
            ecs_record_t *r = rvec[i];
            int32_t row = ECS_RECORD_TO_ROW(r->row);
            flecs_notify_on_set(world, table, row, 1, &type, true);
        }
    }

    ecs_defer_end(world);

    return json;
error:
    return NULL;
}

static
const char* flecs_json_parse_result_legacy(
    ecs_world_t *world,
    const char *json,
    char *token,
    ecs_from_json_ctx_legacy_t *ctx,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    const char *ids = NULL, *values = NULL, *entities = NULL;

    json = flecs_json_expect(json, JsonObjectOpen, token, desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(json, token, "ids", desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(json, JsonArrayOpen, token, desc);
    if (!json) {
        goto error;
    }

    ids = json; /* store start of ids array */

    json = flecs_json_skip_array(json, token, desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(json, JsonComma, token, desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member(json, token, desc);
    if (!json) {
        goto error;
    }

    ecs_entity_t parent = 0;
    if (!ecs_os_strcmp(token, "parent")) {
        char *parent_name = NULL;
        json = flecs_json_expect_string(json, token, &parent_name, desc);
        if (!json) {
            goto error;
        }

        parent = flecs_json_lookup_legacy(world, 0, parent_name, desc);
        if (parent_name != token) {
            ecs_os_free(parent_name);
        }

        json = flecs_json_expect(json, JsonComma, token, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect_member(json, token, desc);
        if (!json) {
            goto error;
        }
    }

    if (ecs_os_strcmp(token, "entities")) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expected 'entities'");
        goto error;
    }

    json = flecs_json_expect(json, JsonArrayOpen, token, desc);
    if (!json) {
        goto error;
    }

    entities = json; /* store start of entity id array */

    json = flecs_json_skip_array(json, token, desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonComma) {
        json = flecs_json_expect_member_name(json, token, "values", desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(json, JsonArrayOpen, token, desc);
        if (!json) {
            goto error;
        }

        values = json; /* store start of entities array */
    } else if (token_kind != JsonObjectClose) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "expected ',' or '}'");
        goto error;
    }

    /* Find table from ids */
    ecs_table_t *table = flecs_json_parse_table_legacy(world, ids, token, ctx, desc);
    if (!table) {
        goto error;
    }

    /* Add entities to table */
    if (flecs_json_parse_entities_legacy(world, table, parent,
        entities, token, ctx, desc)) 
    {
        goto error;
    }

    /* If not parsing in strict mode, initialize component arrays to 0. This
     * ensures that even if components can't be deserialized (because of 
     * incomplete reflection data) component values aren't left uninitialized */
    if (!desc->strict) {
        flecs_json_zeromem_table_legacy(table);
    }

    /* Parse values */
    if (values) {
        json = flecs_json_parse_values_legacy(world, table, values, token, ctx, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonComma) {
            json = flecs_json_expect_member_name(json, token, "alerts", desc);
            if (!json) {
                goto error;
            }

            json = flecs_json_expect(json, JsonBoolean, token, desc);
            if (!json) {
                goto error;
            }

            json = flecs_json_expect(json, JsonObjectClose, token, desc);
            if (!json) {
                goto error;
            }
        } else if (token_kind != JsonObjectClose) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected '}'");
            goto error;
        }
    }

    return json;
error:
    return NULL;
}

const char* ecs_world_from_json_legacy(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc_arg)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    ecs_from_json_desc_t desc = {0};
    ecs_allocator_t *a = &world->allocator;
    ecs_from_json_ctx_legacy_t ctx;
    flecs_from_json_ctx_init_legacy(a, &ctx);

    const char *name = NULL, *expr = json, *lah;
    if (desc_arg) {
        desc = *desc_arg;
    }

    if (!desc.lookup_action) {
        desc.lookup_action = (ecs_entity_t(*)(
            const ecs_world_t*, const char*, void*))flecs_json_ensure_entity_legacy;
        desc.lookup_ctx = &ctx.anonymous_ids;
    }

    json = flecs_json_expect(json, JsonObjectOpen, token, &desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(json, token, "results", &desc);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(json, JsonArrayOpen, token, &desc);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonArrayClose) {
        json = lah;
        goto end;
    }

    do {
        json = flecs_json_parse_result_legacy(world, json, token, &ctx, &desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr,
                    "expected ',' or ']'");
            goto error;
        }
    } while(json && json[0]);

end:
    flecs_from_json_ctx_fini_legacy(&ctx);

    json = flecs_json_expect(json, JsonObjectClose, token, &desc);
    if (!json) {
        goto error;
    }

    return json;
error:
    flecs_from_json_ctx_fini_legacy(&ctx);
    return NULL;
}

const char* ecs_world_from_json_file_legacy(
    ecs_world_t *world,
    const char *filename,
    const ecs_from_json_desc_t *desc)
{
    char *json = flecs_load_from_file(filename);
    if (!json) {
        ecs_err("file not found: %s", filename);
        return NULL;
    }

    const char *result = ecs_world_from_json_legacy(world, json, desc);
    ecs_os_free(json);
    return result;
}

#endif
