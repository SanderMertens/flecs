
#include "../../private_api.h"
#include <ctype.h>

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

static
const char* flecs_parse_multiline_string(
    ecs_meta_cursor_t *cur,
    const char *name,
    const char *expr,
    const char *ptr)
{
    /* Multiline string */
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    char ch;
    while ((ch = ptr[0]) && (ch != '`')) {
        if (ch == '\\' && ptr[1] == '`') {
            ch = '`';
            ptr ++;
        }
        ecs_strbuf_appendch(&str, ch);
        ptr ++;
    }
    
    if (ch != '`') {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing '`' to close multiline string");
        goto error;
    }
    char *strval = ecs_strbuf_get(&str);
    if (ecs_meta_set_string(cur, strval) != 0) {
        goto error;
    }
    ecs_os_free(strval);
    
    return ptr + 1;
error:
    return NULL;
}

static
bool flecs_parse_is_float(
    const char *ptr)
{
    ecs_assert(isdigit(ptr[0]), ECS_INTERNAL_ERROR, NULL);
    char ch;
    while ((ch = (++ptr)[0])) {
        if (ch == '.' || ch == 'e') {
            return true;
        }
        if (!isdigit(ch)) {
            return false;
        }
    }
    return false;
}

static
int flecs_parse_discover_type(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_value_t *value,
    const ecs_parse_expr_desc_t *desc)
{
    /* String literal */
    if (ptr[0] == '"' || ptr[0] == '`') {
        value->type = ecs_id(ecs_string_t);
        value->ptr = ecs_value_new_t(world, ecs_string_t);
        return 0;
    }
    
    /* Negative number literal */
    if (ptr[0] == '-') {
        if (!isdigit(ptr[1])) {
            ecs_parser_error(name, expr, ptr - expr, "invalid literal");
            return -1;
        }
        if (flecs_parse_is_float(ptr + 1)) {
            value->type = ecs_id(ecs_f64_t);
            value->ptr = ecs_value_new_t(world, ecs_f64_t);
            return 0;
        } else {
            value->type = ecs_id(ecs_i64_t);
            value->ptr = ecs_value_new_t(world, ecs_i64_t);
            return 0;
        }
    }

    /* Positive number literal */
    if (isdigit(ptr[0])) {
        if (flecs_parse_is_float(ptr)) {
            value->type = ecs_id(ecs_f64_t);
            value->ptr = ecs_value_new_t(world, ecs_f64_t);
            return 0;
        } else {
            value->type = ecs_id(ecs_u64_t);
            value->ptr = ecs_value_new_t(world, ecs_u64_t);
            return 0;
        }
    }

    /* Variable */
    if (ptr[0] == '$') {
        const ecs_expr_var_t *var = ecs_vars_lookup(desc->vars, &ptr[1]);
        if (!var) {
            ecs_parser_error(name, expr, ptr - expr, "unresolved variable");
            return -1;
        }
        value->type = var->value.type;
        value->ptr = ecs_value_new(world, value->type);
        return 0;
    }

    /* Boolean */
    if (ptr[0] == 't' && !ecs_os_strncmp(ptr, "true", 4)) {
        if (!isalpha(ptr[4]) && ptr[4] != '_') {
            value->type = ecs_id(ecs_bool_t);
            value->ptr = ecs_value_new_t(world, ecs_bool_t);
            return 0;
        }
    }
    if (ptr[0] == 'f' && !ecs_os_strncmp(ptr, "false", 5)) {
        if (!isalpha(ptr[5]) && ptr[5] != '_') {
            value->type = ecs_id(ecs_bool_t);
            value->ptr = ecs_value_new_t(world, ecs_bool_t);
            return 0;
        }
    }

    /* Entity identifier */
    if (isalpha(ptr[0])) {
        value->type = ecs_id(ecs_entity_t);
        value->ptr = ecs_value_new_t(world, ecs_entity_t);
        return 0;
    }

    if (ptr[0] == '{') {
        ecs_parser_error(name, expr, ptr - expr,
            "unknown type for composite literal");
        return -1;
    }

    if (ptr[0] == '[') {
        ecs_parser_error(name, expr, ptr - expr,
            "unknown type for collection literal");
        return -1;
    }

    ecs_parser_error(name, expr, ptr - expr, "invalid literal");
    return -1;
}

const char* ecs_parse_expr(
    ecs_world_t *world,
    const char *ptr,
    ecs_value_t *value,
    const ecs_parse_expr_desc_t *desc)
{
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    char token[ECS_MAX_TOKEN_SIZE];
    int depth = 0;

    const char *name = NULL;
    const char *expr = NULL;

    if (desc) {
        name = desc->name;
        expr = desc->expr;
    }
    if (!expr) {
        expr = ptr;
    }

    ptr = ecs_parse_fluff(ptr, NULL);

    ecs_entity_t type = value->type;
    if (!type) {
        ecs_assert(value->ptr == NULL, ECS_INVALID_PARAMETER, NULL);
        if (flecs_parse_discover_type(world, name, expr, ptr, value, desc)) {
            return NULL;
        }
        type = value->type;
    }
    void *data_out = value->ptr;

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, data_out);
    if (cur.valid == false) {
        return NULL;
    }

    if (desc) {
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

        else if (token[0] == '$') {
            if (!desc || !desc->vars) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "unresolved variable '%s' (no variables provided)", token);
                return NULL;
            }

            const ecs_expr_var_t *var = ecs_vars_lookup(desc->vars, &token[1]);
            if (!var) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "unresolved variable '%s'", token);
                return NULL;
            }

            ecs_meta_set_value(&cur, &var->value);
        }

        else if (!ecs_os_strcmp(token, "`")) {
            if (!(ptr = flecs_parse_multiline_string(&cur, name, expr, ptr))) {
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
