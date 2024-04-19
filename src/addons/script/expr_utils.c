/**
 * @file script/expr_utils.c
 * @brief Expression utility functions.
 */

#include "script.h"
#include <ctype.h>

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

char* ecs_interpolate_string(
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

            ecs_parse_expr_desc_t expr_desc = { 
                .vars = ECS_CONST_CAST(ecs_script_vars_t*, vars) 
            };

            ecs_value_t expr_result = {0};
            if (!ecs_parse_expr(world, token, &expr_result, &expr_desc)) {
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

/* Static names for iterator fields */
static const char* flecs_script_iter_field_names[] = {
    "0", "1",  "2",  "3",  "4",  "5",  "6",  "7",
    "8", "9", "10", "11", "12", "13", "14", "15"
};

void ecs_iter_to_vars(
    const ecs_iter_t *it,
    ecs_script_vars_t *vars,
    int offset)
{
    ecs_check(vars != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!offset || offset < it->count, ECS_INVALID_PARAMETER, NULL);

    /* Set variable for $this */
    if (it->count) {
        ecs_script_var_t *var = ecs_script_vars_lookup(vars, "this");
        if (!var) {
            var = ecs_script_vars_declare(vars, "this");
            var->value.type = ecs_id(ecs_entity_t);
        }
        var->value.ptr = &it->entities[offset];
    }

    /* Set variables for fields */
    {
        int32_t i, field_count = it->field_count;
        for (i = 0; i < field_count; i ++) {
            ecs_size_t size = it->sizes[i];
            if (!size) {
                continue;
            }

            void *ptr = it->ptrs[i];
            if (!ptr) {
                continue;
            }

            ptr = ECS_OFFSET(ptr, offset * size);

            const char *name = flecs_script_iter_field_names[i];
            ecs_script_var_t *var = ecs_script_vars_lookup(vars, name);
            if (!var) {
                var = ecs_script_vars_declare(vars, name);
                ecs_assert(ecs_script_vars_lookup(vars, name) != NULL,  
                    ECS_INTERNAL_ERROR, NULL);
                var->value.type = it->ids[i];
            } else {
                ecs_check(var->value.type == it->ids[i], 
                    ECS_INVALID_PARAMETER, NULL);
            }
            var->value.ptr = ptr;
        }
    }

    /* Set variables for query variables */
    {
        int32_t i, var_count = it->variable_count;
        for (i = 1 /* skip this variable */ ; i < var_count; i ++) {
            ecs_entity_t *e_ptr = NULL;
            ecs_var_t *query_var = &it->variables[i];
            if (query_var->entity) {
                e_ptr = &query_var->entity;
            } else {
                ecs_table_range_t *range = &query_var->range;
                if (range->count == 1) {
                    ecs_entity_t *entities = range->table->data.entities.array;
                    e_ptr = &entities[range->offset];
                }
            }
            if (!e_ptr) {
                continue;
            }

            ecs_script_var_t *var = ecs_script_vars_lookup(
                vars, it->variable_names[i]);
            if (!var) {
                var = ecs_script_vars_declare(vars, it->variable_names[i]);
                var->value.type = ecs_id(ecs_entity_t);
            } else {
                ecs_check(var->value.type == ecs_id(ecs_entity_t), 
                    ECS_INVALID_PARAMETER, NULL);
            }
            var->value.ptr = e_ptr;
        }
    }

error:
    return;
}
