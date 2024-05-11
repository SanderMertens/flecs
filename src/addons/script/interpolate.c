/**
 * @file addons/script/interpolate.c
 * @brief String interpolation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include <ctype.h>
#include "script.h"

static
const char* flecs_parse_var_name(
    const char *ptr,
    char *token_out)
{
    char ch, *bptr = token_out;

    while ((ch = *ptr)) {
        if (bptr - token_out > ECS_MAX_TOKEN_SIZE) {
            goto error;
        }

        if (isalpha(ch) || isdigit(ch) || ch == '_') {
            *bptr = ch;
            bptr ++;
            ptr ++;
        } else {
            break;
        }
    }

    if (bptr == token_out) {
        goto error;
    }

    *bptr = '\0';

    return ptr;
error:
    return NULL;
}

static
const char* flecs_parse_interpolated_str(
    const char *ptr,
    char *token_out)
{
    char ch, *bptr = token_out;

    while ((ch = *ptr)) {
        if (bptr - token_out > ECS_MAX_TOKEN_SIZE) {
            goto error;
        }

        if (ch == '\\') {
            if (ptr[1] == '}') {
                *bptr = '}';
                bptr ++;
                ptr += 2;
                continue;
            }
        }

        if (ch != '}') {
            *bptr = ch;
            bptr ++;
            ptr ++;
        } else {
            ptr ++;
            break;
        }
    }

    if (bptr == token_out) {
        goto error;
    }

    *bptr = '\0';

    return ptr;
error:
    return NULL;
}

char* ecs_script_string_interpolate(
    ecs_world_t *world,
    const char *str,
    const ecs_script_vars_t *vars)
{
    char token[ECS_MAX_TOKEN_SIZE];
    ecs_strbuf_t result = ECS_STRBUF_INIT;
    const char *ptr;
    char ch;

    for(ptr = str; (ch = *ptr); ptr++) {
        if (ch == '\\') {
            ptr ++;
            if (ptr[0] == '$') {
                ecs_strbuf_appendch(&result, '$');
                continue;
            }
            if (ptr[0] == '\\') {
                ecs_strbuf_appendch(&result, '\\');
                continue;
            }
            if (ptr[0] == '{') {
                ecs_strbuf_appendch(&result, '{');
                continue;
            }
            if (ptr[0] == '}') {
                ecs_strbuf_appendch(&result, '}');
                continue;
            }
            ptr --;
        }

        if (ch == '$') {
            ptr = flecs_parse_var_name(ptr + 1, token);
            if (!ptr) {
                ecs_parser_error(NULL, str, ptr - str, 
                    "invalid variable name '%s'", ptr);
                goto error;
            }

            ecs_script_var_t *var = ecs_script_vars_lookup(vars, token);
            if (!var) {
                ecs_parser_error(NULL, str, ptr - str, 
                    "unresolved variable '%s'", token);
                goto error;
            }

            if (ecs_ptr_to_str_buf(
                world, var->value.type, var->value.ptr, &result)) 
            {
                goto error;
            }

            ptr --;
        } else if (ch == '{') {
            ptr = flecs_parse_interpolated_str(ptr + 1, token);
            if (!ptr) {
                ecs_parser_error(NULL, str, ptr - str, 
                    "invalid interpolated expression");
                goto error;
            }

            ecs_script_expr_run_desc_t expr_desc = { 
                .vars = ECS_CONST_CAST(ecs_script_vars_t*, vars) 
            };

            ecs_value_t expr_result = {0};
            if (!ecs_script_expr_run(world, token, &expr_result, &expr_desc)) {
                goto error;
            }

            if (ecs_ptr_to_str_buf(
                world, expr_result.type, expr_result.ptr, &result)) 
            {
                goto error;
            }

            ecs_value_free(world, expr_result.type, expr_result.ptr);

            ptr --;
        } else {
            ecs_strbuf_appendch(&result, ch);
        }
    }

    return ecs_strbuf_get(&result);
error:
    return NULL;
}

#endif
