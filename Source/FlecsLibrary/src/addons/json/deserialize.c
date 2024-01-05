/**
 * @file json/deserialize.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"
#include "json.h"
#include <ctype.h>

#ifdef FLECS_JSON

static
const char* flecs_json_parse_path(
    const ecs_world_t *world,
    const char *json,
    char *token,
    ecs_entity_t *out,
    const ecs_from_json_desc_t *desc)
{
    json = flecs_json_expect(json, JsonString, token, desc);
    if (!json) {
        goto error;
    }

    ecs_entity_t result = ecs_lookup_fullpath(world, token);
    if (!result) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "unresolved identifier '%s'", token);
        goto error;
    }

    *out = result;

    return json;
error:
    return NULL;
}

const char* ecs_ptr_from_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr,
    const char *json,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    char token_buffer[ECS_MAX_TOKEN_SIZE], t_lah[ECS_MAX_TOKEN_SIZE];
    char *token = token_buffer;
    int depth = 0;

    const char *name = NULL;
    const char *expr = NULL;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, ptr);
    if (cur.valid == false) {
        return NULL;
    }

    if (desc) {
        name = desc->name;
        expr = desc->expr;
        cur.lookup_action = desc->lookup_action;
        cur.lookup_ctx = desc->lookup_ctx;
    }

    while ((json = flecs_json_parse(json, &token_kind, token))) {
        if (token_kind == JsonLargeString) {
            ecs_strbuf_t large_token = ECS_STRBUF_INIT;
            json = flecs_json_parse_large_string(json, &large_token);
            if (!json) {
                break;
            }

            token = ecs_strbuf_get(&large_token);
            token_kind = JsonString;
        }

        if (token_kind == JsonObjectOpen) {
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '['");
                return NULL;
            }
        } else if (token_kind == JsonObjectClose) {
            depth --;

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected ']'");
                return NULL;
            }

            if (ecs_meta_pop(&cur) != 0) {
                goto error;
            }
        } else if (token_kind == JsonArrayOpen) {
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '{'");
                return NULL;
            }
        } else if (token_kind == JsonArrayClose) {
            depth --;

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '}'");
                return NULL;
            }

            if (ecs_meta_pop(&cur) != 0) {
                goto error;
            }
        } else if (token_kind == JsonComma) {
            if (ecs_meta_next(&cur) != 0) {
                goto error;
            }
        } else if (token_kind == JsonNull) {
            if (ecs_meta_set_null(&cur) != 0) {
                goto error;
            }
        } else if (token_kind == JsonString) {
            const char *lah = flecs_json_parse(
                json, &token_kind, t_lah);
            if (token_kind == JsonColon) {
                /* Member assignment */
                json = lah;
                if (ecs_meta_dotmember(&cur, token) != 0) {
                    goto error;
                }
            } else {
                if (ecs_meta_set_string(&cur, token) != 0) {
                    goto error;
                }
            }
        } else if (token_kind == JsonNumber) {
            double number = atof(token);
            if (ecs_meta_set_float(&cur, number) != 0) {
                goto error;
            }
        } else if (token_kind == JsonLargeInt) {
            int64_t number = flecs_ito(int64_t, atoll(token));
            if (ecs_meta_set_int(&cur, number) != 0) {
                goto error;
            }
        } else if (token_kind == JsonNull) {
            if (ecs_meta_set_null(&cur) != 0) {
                goto error;
            }
        } else if (token_kind == JsonTrue) {
            if (ecs_meta_set_bool(&cur, true) != 0) {
                goto error;
            }
        } else if (token_kind == JsonFalse) {
            if (ecs_meta_set_bool(&cur, false) != 0) {
                goto error;
            }
        } else {
            goto error;
        }

        if (token != token_buffer) {
            ecs_os_free(token);
            token = token_buffer;
        }

        if (!depth) {
            break;
        }
    }

    return json;
error:
    return NULL;
}

const char* ecs_entity_from_json(
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
        json = flecs_json_expect(json, JsonString, token, &desc);
        if (!json) {
            goto error;
        }

        ecs_add_fullpath(world, e, token);

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
                json = ecs_parse_ws_eol(values + 1);
            } else {
                json = ids;
            }

            break;
        } else if (token_kind == JsonArrayOpen) {
            ids = flecs_json_parse_path(world, ids, token, &first, &desc);
            if (!ids) {
                goto error;
            }

            ids = flecs_json_parse(ids, &token_kind, token);
            if (!ids) {
                goto error;
            }

            if (token_kind == JsonComma) {
                /* Id is a pair*/
                ids = flecs_json_parse_path(world, ids, token, &second, &desc);
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
        void *comp_ptr = ecs_get_mut_id(world, e, id);
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
                values = ecs_parse_ws_eol(values);
            }

            ecs_modified_id(world, e, id);
        }
    } while(ids[0]);

    return flecs_json_expect(json, JsonObjectClose, token, &desc);
error:
    return NULL;
}

static
ecs_entity_t flecs_json_new_id(
    ecs_world_t *world,
    ecs_entity_t ser_id)
{
    /* Try to honor low id requirements */
    if (ser_id < FLECS_HI_COMPONENT_ID) {
        return ecs_new_low_id(world);
    } else {
        return ecs_new_id(world);
    }
}

static
ecs_entity_t flecs_json_lookup(
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
void flecs_json_mark_reserved(
    ecs_map_t *anonymous_ids,
    ecs_entity_t e)
{
    ecs_entity_t *reserved = ecs_map_ensure(anonymous_ids, e);
    ecs_assert(reserved[0] == 0, ECS_INTERNAL_ERROR, NULL);
    reserved[0] = 0;
}

static
bool flecs_json_name_is_anonymous(
    const char *name)
{
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
ecs_entity_t flecs_json_ensure_entity(
    ecs_world_t *world,
    const char *name,
    ecs_map_t *anonymous_ids)
{
    ecs_entity_t e = 0;

    if (flecs_json_name_is_anonymous(name)) {
        /* Anonymous entity, find or create mapping to new id */
        ecs_entity_t ser_id = flecs_ito(ecs_entity_t, atoll(name));
        ecs_entity_t *deser_id = ecs_map_get(anonymous_ids, ser_id);
        if (deser_id) {
            if (!deser_id[0]) {
                /* Id is already issued by deserializer, create new id */
                deser_id[0] = flecs_json_new_id(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved(anonymous_ids, deser_id[0]);
            } else {
                /* Id mapping exists */
            }
        } else {
            /* Id has not yet been issued by deserializer, which means it's safe
             * to use. This allows the deserializer to bind to existing 
             * anonymous ids, as they will never be reissued. */
            deser_id = ecs_map_ensure(anonymous_ids, ser_id);
            if (!ecs_exists(world, ser_id) || ecs_is_alive(world, ser_id)) {
                /* Only use existing id if it's alive or doesn't exist yet. The 
                 * id could have been recycled for another entity */
                deser_id[0] = ser_id;
                ecs_ensure(world, ser_id);
            } else {
                /* If id exists and is not alive, create a new id */
                deser_id[0] = flecs_json_new_id(world, ser_id);

                /* Mark new id as reserved */
                flecs_json_mark_reserved(anonymous_ids, deser_id[0]);
            }
        }

        e = deser_id[0];
    } else {
        e = ecs_lookup_path_w_sep(world, 0, name, ".", NULL, false);
        if (!e) {
            e = ecs_entity(world, { .name = name });
            flecs_json_mark_reserved(anonymous_ids, e);
        }
    }

    return e;
}

static
ecs_table_t* flecs_json_parse_table(
    ecs_world_t *world,
    const char *json,
    char *token,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    ecs_table_t *table = NULL;

    do {
        ecs_id_t id = 0;
        json = flecs_json_expect(json, JsonArrayOpen, token, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(json, JsonString, token, desc);
        if (!json) {
            goto error;
        }

        ecs_entity_t first = flecs_json_lookup(world, 0, token, desc);
        if (!first) {
            goto error;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonComma) {
            json = flecs_json_expect(json, JsonString, token, desc);
            if (!json) {
                goto error;
            }

            ecs_entity_t second = flecs_json_lookup(world, 0, token, desc);
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

        table = ecs_table_add_id(world, table, id);
        if (!table) {
            goto error;
        }

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

    return table;
error:
    return NULL;
}

static
int flecs_json_parse_entities(
    ecs_world_t *world,
    ecs_allocator_t *a,
    ecs_table_t *table,
    ecs_entity_t parent,
    const char *json,
    char *token,
    ecs_vec_t *records,
    const ecs_from_json_desc_t *desc)
{
    char name_token[ECS_MAX_TOKEN_SIZE];
    ecs_json_token_t token_kind = 0;
    ecs_vec_clear(records);

    do {
        json = flecs_json_parse(json, &token_kind, name_token);
        if (!json) {
            goto error;
        }
        if ((token_kind != JsonNumber) && (token_kind != JsonString)) {
            ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
                "expected number or string");
            goto error;
        }

        ecs_entity_t e = flecs_json_lookup(world, parent, name_token, desc);
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
                char *entity_name = strrchr(name_token, '.');
                if (entity_name) {
                    entity_name ++;
                } else {
                    entity_name = name_token;
                }
                if (!flecs_json_name_is_anonymous(entity_name)) {
                    ecs_set_name(world, e, entity_name);
                }
            }
        }

        ecs_assert(table == r->table, ECS_INTERNAL_ERROR, NULL);
        ecs_record_t** elem = ecs_vec_append_t(a, records, ecs_record_t*);
        *elem = r;

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
const char* flecs_json_parse_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    const char *json,
    char *token,
    ecs_vec_t *records,
    const ecs_from_json_desc_t *desc)
{
    if (!table->column_count) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "table has no components");
        goto error;
    }

    if (index >= table->type.count) {
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "more value arrays than component columns in table");
        goto error;
    }

    int32_t data_column = table->column_map[index];
    if (data_column == -1) {
        char *table_str = ecs_table_str(world, table);
        ecs_parser_error(desc->name, desc->expr, json - desc->expr, 
            "values provided for tag at column %d of table [%s]",   
                index, table_str);

        ecs_os_free(table_str);
        goto error;
    }

    ecs_json_token_t token_kind = 0;
    ecs_column_t *column = &table->data.columns[data_column];
    ecs_type_info_t *ti = column->ti;
    ecs_size_t size = ti->size;
    ecs_entity_t type = ti->component;
    ecs_record_t **record_array = ecs_vec_first_t(records, ecs_record_t*);
    int32_t entity = 0;

    do {
        ecs_record_t *r = record_array[entity];
        int32_t row = ECS_RECORD_TO_ROW(r->row);

        void *ptr = ecs_vec_get(&column->data, size, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        json = ecs_ptr_from_json(world, type, ptr, json, desc);
        if (!json) {
            break;
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

    return json;
error:
    return NULL;
}

static
const char* flecs_json_parse_values(
    ecs_world_t *world,
    ecs_table_t *table,
    const char *json,
    char *token,
    ecs_vec_t *records,
    ecs_vec_t *columns_set,
    const ecs_from_json_desc_t *desc)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_json_token_t token_kind = 0;
    int32_t column = 0;

    ecs_vec_clear(columns_set);

    do {
        json = flecs_json_parse(json, &token_kind, token);
        if (!json) {
            goto error;
        }

        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_parse_column(world, table, column,
                json, token, records, desc);
            if (!json) {
                goto error;
            }

            ecs_id_t *id_set = ecs_vec_append_t(a, columns_set, ecs_id_t);
            *id_set = table->type.array[column];

            column ++;
        } else if (token_kind == JsonNumber) {
            if (!ecs_os_strcmp(token, "0")) {
                column ++; /* no data */
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
    } while (json[0]);

    /* Send OnSet notifications */
    ecs_defer_begin(world);
    ecs_type_t type = { 
        .array = columns_set->array, 
        .count = columns_set->count };

    int32_t table_count = ecs_table_count(table);
    int32_t i, record_count = ecs_vec_count(records);

    /* If the entire table was inserted, send bulk notification */
    if (table_count == ecs_vec_count(records)) {
        flecs_notify_on_set(world, table, 0, ecs_table_count(table), &type, true);
    } else {
        ecs_record_t **rvec = ecs_vec_first_t(records, ecs_record_t*);
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
const char* flecs_json_parse_result(
    ecs_world_t *world,
    ecs_allocator_t *a,
    const char *json,
    char *token,
    ecs_vec_t *records,
    ecs_vec_t *columns_set,
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
        json = flecs_json_expect(json, JsonString, token, desc);
        if (!json) {
            goto error;
        }
        parent = ecs_lookup_fullpath(world, token);

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
    ecs_table_t *table = flecs_json_parse_table(world, ids, token, desc);
    if (!table) {
        goto error;
    }

    /* Add entities to table */
    if (flecs_json_parse_entities(world, a, table, parent,
        entities, token, records, desc)) 
    {
        goto error;
    }

    /* Parse values */
    if (values) {
        json = flecs_json_parse_values(world, table, values, token, 
            records, columns_set, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(json, JsonObjectClose, token, desc);
        if (!json) {
            goto error;
        }
    }

    return json;
error:
    return NULL;
}

const char* ecs_world_from_json(
    ecs_world_t *world,
    const char *json,
    const ecs_from_json_desc_t *desc_arg)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    ecs_from_json_desc_t desc = {0};
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_t records;
    ecs_vec_t columns_set;
    ecs_map_t anonymous_ids;
    ecs_vec_init_t(a, &records, ecs_record_t*, 0);
    ecs_vec_init_t(a, &columns_set, ecs_id_t, 0);
    ecs_map_init(&anonymous_ids, a);

    const char *name = NULL, *expr = json, *lah;
    if (desc_arg) {
        desc = *desc_arg;
    }

    if (!desc.lookup_action) {
        desc.lookup_action = (ecs_entity_t(*)(
            const ecs_world_t*, const char*, void*))flecs_json_ensure_entity;
        desc.lookup_ctx = &anonymous_ids;
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
        json = flecs_json_parse_result(world, a, json, token, 
            &records, &columns_set, &desc);
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
    ecs_vec_fini_t(a, &records, ecs_record_t*);
    ecs_vec_fini_t(a, &columns_set, ecs_id_t);
    ecs_map_fini(&anonymous_ids);

    json = flecs_json_expect(json, JsonObjectClose, token, &desc);
    if (!json) {
        goto error;
    }

    return json;
error:
    ecs_vec_fini_t(a, &records, ecs_record_t*);
    ecs_vec_fini_t(a, &columns_set, ecs_id_t);
    ecs_map_fini(&anonymous_ids);

    return NULL;
}

#endif
