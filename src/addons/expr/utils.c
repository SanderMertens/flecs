/**
 * @file expr/strutil.c
 * @brief String parsing utilities.
 */

#include "../../private_api.h"

#ifdef FLECS_EXPR

#include <ctype.h>

char* ecs_chresc(
    char *out, 
    char in, 
    char delimiter) 
{
    char *bptr = out;
    switch(in) {
    case '\a':
        *bptr++ = '\\';
        *bptr = 'a';
        break;
    case '\b':
        *bptr++ = '\\';
        *bptr = 'b';
        break;
    case '\f':
        *bptr++ = '\\';
        *bptr = 'f';
        break;
    case '\n':
        *bptr++ = '\\';
        *bptr = 'n';
        break;
    case '\r':
        *bptr++ = '\\';
        *bptr = 'r';
        break;
    case '\t':
        *bptr++ = '\\';
        *bptr = 't';
        break;
    case '\v':
        *bptr++ = '\\';
        *bptr = 'v';
        break;
    case '\\':
        *bptr++ = '\\';
        *bptr = '\\';
        break;
    default:
        if (in == delimiter) {
            *bptr++ = '\\';
            *bptr = delimiter;
        } else {
            *bptr = in;
        }
        break;
    }

    *(++bptr) = '\0';

    return bptr;
}

const char* ecs_chrparse(
    const char *in, 
    char *out) 
{
    const char *result = in + 1;
    char ch;

    if (in[0] == '\\') {
        result ++;

        switch(in[1]) {
        case 'a':
            ch = '\a';
            break;
        case 'b':
            ch = '\b';
            break;
        case 'f':
            ch = '\f';
            break;
        case 'n':
            ch = '\n';
            break;
        case 'r':
            ch = '\r';
            break;
        case 't':
            ch = '\t';
            break;
        case 'v':
            ch = '\v';
            break;
        case '\\':
            ch = '\\';
            break;
        case '"':
            ch = '"';
            break;
        case '0':
            ch = '\0';
            break;
        case ' ':
            ch = ' ';
            break;
        case '$':
            ch = '$';
            break;
        default:
            goto error;
        }
    } else {
        ch = in[0];
    }

    if (out) {
        *out = ch;
    }

    return result;
error:
    return NULL;
}

ecs_size_t ecs_stresc(
    char *out, 
    ecs_size_t n, 
    char delimiter, 
    const char *in) 
{
    const char *ptr = in;
    char ch, *bptr = out, buff[3];
    ecs_size_t written = 0;
    while ((ch = *ptr++)) {
        if ((written += (ecs_size_t)(ecs_chresc(
            buff, ch, delimiter) - buff)) <= n) 
        {
            /* If size != 0, an out buffer must be provided. */
            ecs_check(out != NULL, ECS_INVALID_PARAMETER, NULL);
            *bptr++ = buff[0];
            if ((ch = buff[1])) {
                *bptr = ch;
                bptr++;
            }
        }
    }

    if (bptr) {
        while (written < n) {
            *bptr = '\0';
            bptr++;
            written++;
        }
    }
    return written;
error:
    return 0;
}

char* ecs_astresc(
    char delimiter, 
    const char *in)
{
    if (!in) {
        return NULL;
    }

    ecs_size_t len = ecs_stresc(NULL, 0, delimiter, in);
    char *out = ecs_os_malloc_n(char, len + 1);
    ecs_stresc(out, len, delimiter, in);
    out[len] = '\0';
    return out;
}

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
    const ecs_vars_t *vars)
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

            ecs_expr_var_t *var = ecs_vars_lookup(vars, token);
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

            ecs_parse_expr_desc_t expr_desc = { .vars = (ecs_vars_t*)vars };
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

void ecs_iter_to_vars(
    const ecs_iter_t *it,
    ecs_vars_t *vars,
    int offset)
{
    ecs_check(vars != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!offset || offset < it->count, ECS_INVALID_PARAMETER, NULL);

    /* Set variable for $this */
    if (it->count) {
        ecs_expr_var_t *var = ecs_vars_lookup(vars, "this");
        if (!var) {
            ecs_value_t v = { 
                .ptr = &it->entities[offset], 
                .type = ecs_id(ecs_entity_t) 
            };
            var = ecs_vars_declare_w_value(vars, "this", &v);
            var->owned = false;
        } else {
            var->value.ptr = &it->entities[offset];
        }
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

            char name[16];
            ecs_os_sprintf(name, "%d", i + 1);
            ecs_expr_var_t *var = ecs_vars_lookup(vars, name);
            if (!var) {
                ecs_value_t v = { .ptr = ptr, .type = it->ids[i] };
                var = ecs_vars_declare_w_value(vars, name, &v);
                var->owned = false;
            } else {
                ecs_check(var->value.type == it->ids[i], 
                    ECS_INVALID_PARAMETER, NULL);
                var->value.ptr = ptr;
            }
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

            ecs_expr_var_t *var = ecs_vars_lookup(vars, it->variable_names[i]);
            if (!var) {
                ecs_value_t v = { .ptr = e_ptr, .type = ecs_id(ecs_entity_t) };
                var = ecs_vars_declare_w_value(vars, it->variable_names[i], &v);
                var->owned = false;
            } else {
                ecs_check(var->value.type == ecs_id(ecs_entity_t), 
                    ECS_INVALID_PARAMETER, NULL);
                var->value.ptr = e_ptr;
            }
        }
    }

error:
    return;
}

#endif
