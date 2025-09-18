/**
 * @file addons/json/deserialize_value.c
 * @brief Deserialize JSON strings into (component) values.
 */

#include "../../private_api.h"
#include "json.h"
#include "../script/script.h"

#ifdef FLECS_JSON

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

    bool strict = desc ? desc->strict : false;
    bool skip = false;
    int skip_depth = 0;

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
        if (skip) {
            /* Skip over tokens in case an unknown member was encountered */
            if (token_kind == JsonObjectOpen || token_kind == JsonArrayOpen) {
                skip_depth ++;
            } else
            if (token_kind == JsonObjectClose || token_kind == JsonArrayClose) {
                skip_depth --;
                if (!skip_depth) {
                    skip = false;
                }
            } else {
                if (!skip_depth) {
                    skip = false;
                }
            }

            continue;
        }

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
                if (strict) {
                    if (ecs_meta_dotmember(&cur, token) != 0) {
                        goto error;
                    }
                } else {
                    if (ecs_meta_try_dotmember(&cur, token) != 0) {
                        skip = true;
                    }
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

#endif
