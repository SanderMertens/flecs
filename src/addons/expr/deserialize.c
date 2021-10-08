
#include "../../private_api.h"

#ifdef FLECS_EXPR

const char* ecs_parse_expr(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t type,
    void *data_out)
{
    char token[ECS_MAX_TOKEN_SIZE];
    int depth = 0;

    if (!ptr) {
        ptr = expr;
    }

    ptr = ecs_parse_fluff(ptr);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, data_out);
    if (cur.valid == false) {
        return NULL;
    }

    while ((ptr = ecs_parse_token(name, expr, ptr, token))) {

        ptr = ecs_parse_fluff(ptr);

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
            if (ecs_meta_set_string_literal(&cur, token) != 0) {
                goto error;
            }
        }

        else {
            if (ptr[0] == '=') {
                /* Member assignment */
                ptr ++;
                if (ecs_meta_member(&cur, token) != 0) {
                    goto error;
                }
            } else {
                if (ecs_meta_set_string(&cur, token) != 0) {
                    goto error;
                }
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
