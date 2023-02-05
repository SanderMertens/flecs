/**
 * @file json/deserialize.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"

#ifdef FLECS_JSON

const char* ecs_ptr_from_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr,
    const char *json,
    const ecs_from_json_desc_t *desc)
{
    char token[ECS_MAX_TOKEN_SIZE];
    int depth = 0;

    const char *name = NULL;
    const char *expr = NULL;

    json = ecs_parse_ws_eol(json);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, ptr);
    if (cur.valid == false) {
        return NULL;
    }

    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    while ((json = ecs_parse_expr_token(name, expr, json, token))) {

        json = ecs_parse_ws_eol(json);

        if (!ecs_os_strcmp(token, "{")) {
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '['");
                return NULL;
            }
        }

        else if (!ecs_os_strcmp(token, "}")) {
            depth --;

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected ']'");
                return NULL;
            }

            if (ecs_meta_pop(&cur) != 0) {
                goto error;
            }
        }

        else if (!ecs_os_strcmp(token, "[")) {
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '{'");
                return NULL;
            }
        }

        else if (!ecs_os_strcmp(token, "]")) {
            depth --;

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, json - expr, "expected '}'");
                return NULL;
            }

            if (ecs_meta_pop(&cur) != 0) {
                goto error;
            }
        }

        else if (!ecs_os_strcmp(token, ",")) {
            if (ecs_meta_next(&cur) != 0) {
                goto error;
            }
        }

        else if (!ecs_os_strcmp(token, "null")) {
            if (ecs_meta_set_null(&cur) != 0) {
                goto error;
            }
        }

        else if (token[0] == '\"') {
            if (json[0] == ':') {
                /* Member assignment */
                json ++;

                /* Strip trailing " */
                ecs_size_t len = ecs_os_strlen(token);
                if (token[len - 1] != '"') {
                    ecs_parser_error(name, expr, json - expr, "expected \"");
                    return NULL;
                } else {
                    token[len - 1] = '\0';
                }

                if (ecs_meta_dotmember(&cur, token + 1) != 0) {
                    goto error;
                }
            } else {
                if (ecs_meta_set_string_literal(&cur, token) != 0) {
                    goto error;
                }
            }
        }

        else {
            if (ecs_meta_set_string(&cur, token) != 0) {
                goto error;
            }
        }

        if (!depth) {
            break;
        }
    }

    return json;
error:
    return NULL;
}

static
const char* flecs_parse_json_path_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token)
{
    /* Start of id. Ids can have two elements, in case of pairs. */
    if (ptr[0] != '"') {
        ecs_parser_error(name, expr, ptr - expr, "expected starting '\"'");
        goto error;
    }

    const char *token_start = ptr;
    ptr = ecs_parse_expr_token(name, expr, ptr, token);
    if (!ptr) {
        goto error;
    }

    ecs_size_t len = ecs_os_strlen(token);
    if (token_start[len - 1] != '"') {
        ecs_parser_error(name, expr, ptr - expr, "missing closing '\"'");
        goto error;
    }

    token[len - 1] = '\0';

    return ptr;
error:
    return NULL;
}

static
const char* flecs_parse_json_path(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    char *token,
    ecs_entity_t *out)
{
    ptr = flecs_parse_json_path_token(name, expr, ptr, token);
    if (!ptr) {
        goto error;
    }

    ecs_entity_t result = ecs_lookup_fullpath(world, token + 1);
    if (!result) {
        ecs_parser_error(name, expr, ptr - expr, 
            "unresolved identifier '%s'", token);
        goto error;
    }

    *out = result;

    return ecs_parse_ws_eol(ptr);
error:
    return NULL;
}

const char* ecs_entity_from_json(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *json,
    const ecs_from_json_desc_t *desc)
{
    char token[ECS_MAX_TOKEN_SIZE];
    const char *name = NULL;
    const char *expr = json;
    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    const char *ids = NULL, *values = NULL;

    json = ecs_parse_ws_eol(json);
    if (json[0] != '{') {
        ecs_parser_error(name, expr, json - expr, "expected '{'");
        goto error;
    }

    /* Find start of ids array */
    json = ecs_parse_ws_eol(json + 1);
    if (json[0] == '}') {
        /* End of expression */
        return &json[1];
    }

    if (!ecs_os_strncmp(json, "\"path\"", 6)) {
        json = ecs_parse_ws_eol(json + 6);
        if (json[0] != ':') {
            ecs_parser_error(name, expr, json - expr, "expected ':'");
            goto error;
        }

        json = flecs_parse_json_path_token(name, expr, json + 1, token);
        if (!json) {
            goto error;
        }

        ecs_add_fullpath(world, e, token + 1);

        if (json[0] == '}') {
            /* End of expression */
            return &json[1];
        } else if (json[0] == ',') {
            json ++;
        } else {
            ecs_parser_error(name, expr, json - expr, "unexpected character");
            goto error;
        }

        json = ecs_parse_ws_eol(json + 1);
    }

    if (ecs_os_strncmp(json, "\"ids\"", 5)) {
        ecs_parser_error(name, expr, json - expr, "expected '\"ids\"'");
        goto error;
    }

    json = ecs_parse_ws_eol(json + 5);
    if (json[0] != ':') {
        ecs_parser_error(name, expr, json - expr, "expected ':'");
        goto error;
    }

    json = ecs_parse_ws_eol(json + 1);
    if (json[0] != '[') {
        ecs_parser_error(name, expr, json - expr, "expected '['");
        goto error;
    }

    ids = ecs_parse_ws_eol(json + 1);

    /* Find start of values array */
    const char *vptr = json;
    int32_t bracket_depth = 0;
    while (vptr[0]) {
        if (vptr[0] == '[') {
            bracket_depth ++;
        }
        if (vptr[0] == ']') {
            bracket_depth --;
            if (!bracket_depth) {
                break;
            }
        }
        vptr ++;
    }

    if (!vptr[0]) {
        ecs_parser_error(name, expr, json - expr, 
            "found end of string while looking for values");
        goto error;
    }

    json = ecs_parse_ws_eol(vptr + 1);
    if (json[0] == '}') {
        values = NULL;
    } else {
        if (json[0] != ',') {
            ecs_parser_error(name, expr, json - expr, "expected ','");
            goto error;
        }

        json = ecs_parse_ws_eol(json + 1);
        if (ecs_os_strncmp(json, "\"values\"", 8)) {
            ecs_parser_error(name, expr, json - expr, "expected '\"values\"'");
            goto error;
        }

        json = ecs_parse_ws_eol(json + 8);
        if (json[0] != ':') {
            ecs_parser_error(name, expr, json - expr, "expected ':'");
            goto error;
        }

        json = ecs_parse_ws_eol(json + 1);
        if (json[0] != '[') {
            ecs_parser_error(name, expr, json - expr, "expected '['");
            goto error;
        }

        values = json;
    }

    /* Parse ids & values */
    while (ids[0]) {
        ecs_entity_t first = 0, second = 0, type_id = 0;
        ecs_id_t id;

        if (ids[0] == ']') {
            if (values) {
                /* Last id found */
                if (values[0] != ']') {
                    ecs_parser_error(name, expr, values - expr, "expected ']'");
                }
                json = values;
            } else {
                json = ids;
            }
            break;
        } else if (ids[0] == '[') {
            ids = ecs_parse_ws_eol(ids + 1);
            ids = flecs_parse_json_path(world, name, expr, ids, token, &first);
            if (!ids) {
                goto error;
            }

            if (ids[0] == ',') {
                /* Id is a pair*/
                ids = ecs_parse_ws_eol(ids + 1);
                ids = flecs_parse_json_path(world, name, expr, ids, token, 
                    &second);
                if (!ids) {
                    goto error;
                }
            }
            
            if (ids[0] == ']') {
                /* End of id array */
            } else {
                ecs_parser_error(name, expr, ids - expr, "expected ',' or ']'");
                goto error;
            }

            ids = ecs_parse_ws_eol(ids + 1);
            if (ids[0] == ',') {
                /* Next id */
                ids = ecs_parse_ws_eol(ids + 1);
            } else if (ids[0] != ']') {
                /* End of ids array */
                ecs_parser_error(name, expr, ids - expr, "expected ',' or ']'");
                goto error;
            }
        } else {
            ecs_parser_error(name, expr, ids - expr, "expected '[' or ']'");
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
            values = ecs_ptr_from_json(world, type_id, comp_ptr, values + 1, &parse_desc);
            if (!values) {
                goto error;
            }

            if (values[0] != ']' && values[0] != ',') {
                ecs_parser_error(name, expr, json - expr, "expected ',' or ']'");
                goto error;
            }

            ecs_modified_id(world, e, id);
        }
    }

    json = ecs_parse_ws_eol(json + 1);
    if (json[0] != '}') {
        ecs_parser_error(name, expr, json - expr, "expected '}'");
    }

    return json + 1;
error:
    return NULL;
}

#endif
