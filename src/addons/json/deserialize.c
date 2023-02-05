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
    ecs_json_token_t token_kind;
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
    ecs_json_token_t token_kind;
    char token[ECS_MAX_TOKEN_SIZE];

    const char *name = NULL;
    const char *expr = json;
    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    const char *ids = NULL, *values = NULL, *lah;

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
                ecs_parser_error(name, expr, json - lah, 
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

#endif
