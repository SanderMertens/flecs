/**
 * @file json/deserialize.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"
#include "json.h"

#ifdef FLECS_JSON

static
const char* flecs_json_parse_path(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *json,
    char *token,
    ecs_entity_t *out)
{
    json = flecs_json_expect(name, expr, json, JsonString, token);
    if (!json) {
        goto error;
    }

    ecs_entity_t result = ecs_lookup_fullpath(world, token);
    if (!result) {
        ecs_parser_error(name, expr, json - expr, 
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
    char token[ECS_MAX_TOKEN_SIZE], t_lah[ECS_MAX_TOKEN_SIZE];
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
    }

    while ((json = flecs_json_parse(json, &token_kind, token))) {
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
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *name = NULL, *expr = json, *ids = NULL, *values = NULL, *lah;
    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    json = flecs_json_expect(name, expr, json, JsonObjectOpen, token);
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

    json = flecs_json_expect_member(name, expr, json, token);
    if (!json) {
        return NULL;
    }

    if (!ecs_os_strcmp(token, "path")) {
        json = flecs_json_expect(name, expr, json, JsonString, token);
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

        json = flecs_json_expect_member_name(name, expr, json, token, "ids");
        if (!json) {
            goto error;
        }
    } else if (ecs_os_strcmp(token, "ids")) {
        ecs_parser_error(name, expr, json - expr, "expected member 'ids'");
        goto error;
    }

    json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
    if (!json) {
        goto error;
    }

    ids = json;

    json = flecs_json_skip_array(name, expr, json, token);
    if (!json) {
        return NULL;
    }

    json = flecs_json_parse(json, &token_kind, token);
    if (token_kind != JsonObjectClose) {
        if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr, "expected ','");
            goto error;
        }

        json = flecs_json_expect_member_name(name, expr, json, token, "values");
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
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
            ids = flecs_json_parse_path(world, name, expr, ids, token, &first);
            if (!ids) {
                goto error;
            }

            ids = flecs_json_parse(ids, &token_kind, token);
            if (!ids) {
                goto error;
            }

            if (token_kind == JsonComma) {
                /* Id is a pair*/
                ids = flecs_json_parse_path(world, name, expr, ids, token, 
                    &second);
                if (!ids) {
                    goto error;
                }

                ids = flecs_json_expect(name, expr, ids, JsonArrayClose, token);
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
                ecs_parser_error(name, expr, lah - expr, 
                    "expected ',' or ']'");
                goto error;
            }
        } else {
            ecs_parser_error(name, expr, lah - expr, 
                "expected '[' or ']'");
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

    return flecs_json_expect(name, expr, json, JsonObjectClose, token);;
error:
    return NULL;
}

static
ecs_entity_t flecs_json_ensure_entity(
    ecs_world_t *world,
    const char *name)
{
    ecs_entity_t e = ecs_lookup_fullpath(world, name);
    if (!e) {
        e = ecs_entity(world, { .name = name });
    }
    return e;
}

static
ecs_table_t* flecs_json_parse_table(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *json,
    char *token)
{
    ecs_json_token_t token_kind = 0;
    ecs_table_t *table = NULL;

    do {
        ecs_id_t id = 0;
        json = flecs_json_expect(name, expr, json, JsonString, token);
        if (!json) {
            goto error;
        }

        if (token[0] == '(') {
            char *sep = strchr(token, ',');
            if (!sep) {
                ecs_parser_error(name, expr, json - expr, 
                    "expected ',' for pair'");
                goto error;
            }

            *sep = '\0';
            ecs_entity_t first = flecs_json_ensure_entity(world, &token[1]);
            if (!first) {
                goto error;
            }

            char *end = strchr(sep + 1, ')');
            if (!end) {
                ecs_parser_error(name, expr, json - expr, 
                    "expected ')' for pair'");
                goto error;
            }

            *end = '\0';
            ecs_entity_t second = flecs_json_ensure_entity(world, &sep[1]);
            if (!second) {
                goto error;
            }

            id = ecs_pair(first, second);
        } else {
            id = flecs_json_ensure_entity(world, token);
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
            ecs_parser_error(name, expr, json - expr, "expected ',' or ']'");
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
    const char *name,
    const char *expr,
    const char *json,
    char *token,
    ecs_vec_t *records)
{
    ecs_json_token_t token_kind = 0;
    ecs_vec_clear(records);

    do {
        json = flecs_json_expect(name, expr, json, JsonString, token);
        if (!json) {
            goto error;
        }

        /* Create new entity or move existing entity to table */
        ecs_entity_t e = ecs_lookup_fullpath(world, token);
        ecs_record_t *r;
        if (e) {
            r = flecs_entities_get(world, e);
            ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
            if (r->table != table) {
                /* Move entity to table */
                ecs_commit(world, e, NULL, table, NULL, NULL);
            }
        } else {
            e = ecs_new_w_table(world, table);
            r = flecs_entities_get(world, e);
            char *name = strrchr(token, '.');
            if (name) {
                name ++;
            } else {
                name = token;
            }
            ecs_set_name(world, e, name);
        }

        ecs_record_t** elem = ecs_vec_append_t(a, records, ecs_record_t*);
        *elem = r;

        const char *lah = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonComma) {
            json = lah;
        } else if (token_kind == JsonArrayClose) {
            break;
        } else {
            ecs_parser_error(name, expr, json - expr, "expected ',' or ']'");
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
    int32_t column,
    const char *name,
    const char *expr,
    const char *json,
    char *token,
    ecs_vec_t *records,
    const ecs_from_json_desc_t *desc)
{
    if (!table->storage_table) {
        ecs_parser_error(name, expr, json - expr, 
            "table has no components");
        goto error;
    }

    if (column >= table->type.count) {
        ecs_parser_error(name, expr, json - expr, 
            "more value arrays than component columns in table");
        goto error;
    }

    int32_t data_column = table->storage_map[column];
    if (data_column == -1) {
        char *table_str = ecs_table_str(world, table);
        ecs_parser_error(name, expr, json - expr, 
            "values provided for tag at column %d of table [%s]",   
                column, table_str);

        ecs_os_free(table_str);
        goto error;
    }

    ecs_json_token_t token_kind = 0;
    ecs_vec_t *data = &table->data.columns[data_column];
    ecs_type_info_t *ti = table->type_info[data_column];
    ecs_size_t size = ti->size;
    ecs_entity_t type = ti->component;
    ecs_record_t **record_array = ecs_vec_first_t(records, ecs_record_t*);
    int32_t entity = 0;

    do {
        ecs_record_t *r = record_array[entity];
        int32_t row = ECS_RECORD_TO_ROW(r->row);
        ecs_assert(ecs_vec_get_t(
            &table->data.records, ecs_record_t*, row)[0] == r,
                ECS_INTERNAL_ERROR, NULL);

        void *ptr = ecs_vec_get(data, size, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        json = ecs_ptr_from_json(world, type, ptr, json, desc);
        if (!json) {
            break;
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr, "expected ',' or ']'");
        }
    } while (json[0]);

    return json;
error:
    return NULL;
}

static
const char* flecs_json_parse_values(
    ecs_world_t *world,
    ecs_table_t *table,
    const char *name,
    const char *expr,
    const char *json,
    char *token,
    ecs_vec_t *records,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    int32_t column = 0;

    do {
        json = flecs_json_parse(json, &token_kind, token);
        if (!json) {
            goto error;
        }

        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind == JsonArrayOpen) {
            json = flecs_json_parse_column(world, table, column, name, expr, 
                json, token, records, desc);
            if (!json) {
                goto error;
            }

            column ++;
        } else if (token_kind == JsonNumber) {
            if (!ecs_os_strcmp(token, "0")) {
                column ++; /* no data */
            } else {
                ecs_parser_error(name, expr, json - expr, "unexpected number");
                goto error;
            }
        }

        json = flecs_json_parse(json, &token_kind, token);
        if (token_kind == JsonArrayClose) {
            break;
        } else if (token_kind != JsonComma) {
            ecs_parser_error(name, expr, json - expr,
                    "expected ',' or ']'");
            goto error;
        }
    } while (json[0]);

    return json;
error:
    return NULL;
}

static
const char* flecs_json_parse_result(
    ecs_world_t *world,
    ecs_allocator_t *a,
    const char *name,
    const char *expr,
    const char *json,
    char *token,
    ecs_vec_t *records,
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind = 0;
    const char *ids = NULL, *values = NULL, *entities = NULL;

    json = flecs_json_expect(name, expr, json, JsonObjectOpen, token);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(name, expr, json, token, "ids");
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
    if (!json) {
        goto error;
    }

    ids = json; /* store start of ids array */

    json = flecs_json_skip_array(name, expr, json, token);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(name, expr, json, JsonComma, token);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(name, expr, json, token, "entities");
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
    if (!json) {
        goto error;
    }

    entities = json; /* store start of entities array */

    json = flecs_json_skip_array(name, expr, json, token);
    if (!json) {
        goto error;
    }

    json = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonComma) {
        json = flecs_json_expect_member_name(name, expr, json, token, "values");
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
        if (!json) {
            goto error;
        }

        values = json; /* store start of entities array */
    } else if (token_kind != JsonObjectClose) {
        ecs_parser_error(name, expr, json - expr, "expected ',' or '}'");
        goto error;
    }

    /* Find table from ids */
    ecs_table_t *table = flecs_json_parse_table(world, name, expr, ids, token);
    if (!table) {
        goto error;
    }

    /* Add entities to table */    
    if (flecs_json_parse_entities(world, a, table, name, expr, entities, token, 
        records)) 
    {
        goto error;
    }

    /* Parse values */
    if (values) {
        json = flecs_json_parse_values(world, table, name, expr, values, token, 
            records, desc);
        if (!json) {
            goto error;
        }

        json = flecs_json_expect(name, expr, json, JsonObjectClose, token);
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
    const ecs_from_json_desc_t *desc)
{
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *name = NULL, *expr = json, *lah;
    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    json = flecs_json_expect(name, expr, json, JsonObjectOpen, token);
    if (!json) {
        goto error;
    }

    json = flecs_json_expect_member_name(name, expr, json, token, "results");
    if (!json) {
        goto error;
    }

    json = flecs_json_expect(name, expr, json, JsonArrayOpen, token);
    if (!json) {
        goto error;
    }

    lah = flecs_json_parse(json, &token_kind, token);
    if (token_kind == JsonArrayClose) {
        json = lah;
        goto end;
    }

    ecs_allocator_t *a = &world->allocator;
    ecs_vec_t records;
    ecs_vec_init_t(a, &records, ecs_record_t*, 0);

    do {
        json = flecs_json_parse_result(world, a, name, expr, json, token, 
            &records, desc);
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

    ecs_vec_fini_t(a, &records, ecs_record_t*);

end:
    json = flecs_json_expect(name, expr, json, JsonObjectClose, token);
    if (!json) {
        goto error;
    }

    return json;
error:
    return NULL;
}

#endif
