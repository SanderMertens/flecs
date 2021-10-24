
#include "../../private_api.h"

#ifdef FLECS_JSON

const char* ecs_parse_json(
    const ecs_world_t *world,
    const char *ptr,
    ecs_entity_t type,
    void *data_out,
    const ecs_parse_json_desc_t *desc)
{
    char token[ECS_MAX_TOKEN_SIZE];
    int depth = 0;

    const char *name = NULL;
    const char *expr = NULL;

    ptr = ecs_parse_fluff(ptr, NULL);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, data_out);
    if (cur.valid == false) {
        return NULL;
    }

    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }

    while ((ptr = ecs_parse_expr_token(name, expr, ptr, token))) {

        ptr = ecs_parse_fluff(ptr, NULL);

        if (!ecs_os_strcmp(token, "{")) {
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, ptr - expr, "expected '['");
                return NULL;
            }
        }

        else if (!ecs_os_strcmp(token, "}")) {
            depth --;

            if (ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, ptr - expr, "expected ']'");
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
                ecs_parser_error(name, expr, ptr - expr, "expected '{'");
                return NULL;
            }
        }

        else if (!ecs_os_strcmp(token, "]")) {
            depth --;

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, ptr - expr, "expected '}'");
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
            if (ptr[0] == ':') {
                /* Member assignment */
                ptr ++;

                /* Strip trailing " */
                ecs_size_t len = ecs_os_strlen(token);
                if (token[len - 1] != '"') {
                    ecs_parser_error(name, expr, ptr - expr, "expected \"");
                    return NULL;
                } else {
                    token[len - 1] = '\0';
                }

                if (ecs_meta_member(&cur, token + 1) != 0) {
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

    return ptr;
error:
    return NULL;
}

#endif
