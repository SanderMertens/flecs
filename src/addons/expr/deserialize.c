
#include "../../private_api.h"

#ifdef FLECS_EXPR

const char *ecs_parse_expr_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token)
{
    const char *start = ptr;
    char *token_ptr = token;

    while ((ptr = ecs_parse_token(name, expr, ptr, token_ptr))) {
        if (ptr[0] == '|') {
            token_ptr = &token_ptr[ptr - start];
            token_ptr[0] = '|';
            token_ptr[1] = '\0';
            token_ptr ++;
            ptr ++;
            start = ptr;
        } else {
            break;
        }
    }

    return ptr;
}

const char* ecs_parse_expr(
    const ecs_world_t *world,
    const char *ptr,
    ecs_entity_t type,
    void *data_out,
    const ecs_parse_expr_desc_t *desc)
{
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
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
        cur.lookup_action = desc->lookup_action;
        cur.lookup_ctx = desc->lookup_ctx;
    }

    while ((ptr = ecs_parse_expr_token(name, expr, ptr, token))) {

        if (!ecs_os_strcmp(token, "{")) {
            ecs_entity_t scope_type = ecs_meta_get_type(&cur);
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (ecs_meta_is_collection(&cur)) {
                char *path = ecs_get_fullpath(world, scope_type);
                ecs_parser_error(name, expr, ptr - expr, 
                    "expected '[' for collection type '%s'", path);
                ecs_os_free(path);
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
            ptr = ecs_parse_fluff(ptr, NULL);

            if (ptr[0] == ':') {
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

        ptr = ecs_parse_fluff(ptr, NULL);
    }

    return ptr;
error:
    return NULL;
}

#endif
