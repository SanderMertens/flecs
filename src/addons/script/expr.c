/**
 * @file addons/script/expr.c
 * @brief Evaluate script expressions.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include <ctype.h>
#include <math.h>
#include "script.h"

/* String deserializer for script expressions */

/* Order in enumeration is important, as it is used for precedence */
typedef enum ecs_expr_oper_t {
    EcsExprOperUnknown,
    EcsLeftParen,
    EcsCondAnd,
    EcsCondOr,
    EcsCondEq,
    EcsCondNeq,
    EcsCondGt,
    EcsCondGtEq,
    EcsCondLt,
    EcsCondLtEq,
    EcsShiftLeft,
    EcsShiftRight,
    EcsAdd,
    EcsSub,
    EcsMul,
    EcsDiv,
    EcsMin
} ecs_expr_oper_t;

/* Used to track temporary values */
#define FLECS_EXPR_MAX_STACK_SIZE (256)

typedef struct ecs_expr_value_t {
    const ecs_type_info_t *ti;
    void *ptr;
} ecs_expr_value_t;

typedef struct ecs_value_stack_t {
    ecs_expr_value_t values[FLECS_EXPR_MAX_STACK_SIZE];
    ecs_stack_cursor_t *cursor;
    ecs_stack_t *stack;
    ecs_stage_t *stage;
    int32_t count;
} ecs_value_stack_t;

static
const char* flecs_script_expr_run(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *ptr,
    ecs_value_t *value,
    ecs_expr_oper_t op,
    const ecs_script_expr_run_desc_t *desc);

#define TOK_VARIABLE '$'

/* -- Private functions -- */

static
bool flecs_isident(
    char ch)
{
    return isalpha(ch) || (ch == '_');
}

static
bool flecs_valid_identifier_start_char(
    char ch)
{
    if (ch && (flecs_isident(ch) || (ch == '*') ||
        (ch == '0') || (ch == TOK_VARIABLE) || isdigit(ch))) 
    {
        return true;
    }

    return false;
}

static
bool flecs_valid_token_start_char(
    char ch)
{
    if ((ch == '"') || (ch == '{') || (ch == '}') || (ch == ',') || (ch == '-')
        || (ch == '[') || (ch == ']') || (ch == '`') || 
        flecs_valid_identifier_start_char(ch))
    {
        return true;
    }

    return false;
}

static
bool flecs_valid_token_char(
    char ch)
{
    if (ch && (flecs_isident(ch) || isdigit(ch) || ch == '.' || ch == '"')) {
        return true;
    }

    return false;
}

static
const char* flecs_parse_ws(
    const char *ptr)
{
    while ((*ptr != '\n') && isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

static
const char* flecs_parse_expr_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token_out,
    char delim)
{
    int64_t column = ptr - expr;

    ptr = flecs_parse_ws(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!flecs_valid_token_start_char(ch)) {
        if (ch == '\0' || ch == '\n') {
            ecs_parser_error(name, expr, column, 
                "unexpected end of expression");
        } else {
            ecs_parser_error(name, expr, column, 
                "invalid start of token '%s'", ptr);
        }
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' || ch == '`') {
        tptr[0] = 0;
        return ptr;
    }

    int tmpl_nesting = 0;
    bool in_str = ch == '"';

    for (; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            tmpl_nesting ++;
        } else if (ch == '>') {
            if (!tmpl_nesting) {
                break;
            }
            tmpl_nesting --;
        } else if (ch == '"') {
            in_str = !in_str;
        } else if (ch == '\\') {
            ptr ++;
            tptr[0] = ptr[0];
            tptr ++;
            continue;
        } else if (!flecs_valid_token_char(ch) && !in_str) {
            break;
        }

        if (delim && (ch == delim)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    if (tmpl_nesting != 0) {
        ecs_parser_error(name, expr, column, 
            "identifier '%s' has mismatching < > pairs", ptr);
        return NULL;
    }

    const char *next_ptr = flecs_parse_ws(ptr);
    if (next_ptr[0] == ':' && next_ptr != ptr) {
        /* Whitespace between token and : is significant */
        ptr = next_ptr - 1;
    } else {
        ptr = next_ptr;
    }

    return ptr;
}

static
void* flecs_expr_value_new(
    ecs_value_stack_t *stack,
    ecs_entity_t type)
{
    ecs_stage_t *stage = stack->stage;
    ecs_world_t *world = stage->world;
    ecs_id_record_t *idr = flecs_id_record_get(world, type);
    if (!idr) {
        return NULL;
    }

    const ecs_type_info_t *ti = idr->type_info;
    if (!ti) {
        return NULL;
    }

    ecs_assert(ti->size != 0, ECS_INTERNAL_ERROR, NULL);
    void *result = flecs_stack_alloc(stack->stack, ti->size, ti->alignment);
    if (ti->hooks.ctor) {
        ti->hooks.ctor(result, 1, ti);
    } else {
        ecs_os_memset(result, 0, ti->size);
    }
    if (ti->hooks.dtor) {
        /* Track values that have destructors */
        stack->values[stack->count].ti = ti;
        stack->values[stack->count].ptr = result;
        stack->count ++;
    }

    return result;
}

static
const char* flecs_str_to_expr_oper(
    const char *str,
    ecs_expr_oper_t *op)
{
    if (!ecs_os_strncmp(str, "+", 1)) {
        *op = EcsAdd;
        return str + 1;
    } else if (!ecs_os_strncmp(str, "-", 1)) {
        *op = EcsSub;
        return str + 1;
    } else if (!ecs_os_strncmp(str, "*", 1)) {
        *op = EcsMul;
        return str + 1;
    } else if (!ecs_os_strncmp(str, "/", 1)) {
        *op = EcsDiv;
        return str + 1;
    } else if (!ecs_os_strncmp(str, "&&", 2)) {
        *op = EcsCondAnd;
        return str + 2;
    } else if (!ecs_os_strncmp(str, "||", 2)) {
        *op = EcsCondOr;
        return str + 2;
    } else if (!ecs_os_strncmp(str, "==", 2)) {
        *op = EcsCondEq;
        return str + 2;
    } else if (!ecs_os_strncmp(str, "!=", 2)) {
        *op = EcsCondNeq;
        return str + 2;
    } else if (!ecs_os_strncmp(str, ">=", 2)) {
        *op = EcsCondGtEq;
        return str + 2;
    } else if (!ecs_os_strncmp(str, "<=", 2)) {
        *op = EcsCondLtEq;
        return str + 2;
    } else if (!ecs_os_strncmp(str, ">>", 2)) {
        *op = EcsShiftRight;
        return str + 2;
    } else if (!ecs_os_strncmp(str, "<<", 2)) {
        *op = EcsShiftLeft;
        return str + 2;
    } else if (!ecs_os_strncmp(str, ">", 1)) {
        *op = EcsCondGt;
        return str + 1;
    } else if (!ecs_os_strncmp(str, "<", 1)) {
        *op = EcsCondLt;
        return str + 1;
    }

    *op = EcsExprOperUnknown;
    return NULL;
}

static
const char *flecs_script_expr_parse_token(
    const char *name,
    const char *expr,
    const char *ptr,
    char *token)
{
    char *token_ptr = token;

    if (ptr[0] == '/') {
        char ch;
        if (ptr[1] == '/') {
            // Single line comment
            for (ptr = &ptr[2]; (ch = ptr[0]) && (ch != '\n'); ptr ++) {}
            token[0] = 0;
            return flecs_parse_ws_eol(ptr);
        } else if (ptr[1] == '*') {
            // Multi line comment
            for (ptr = &ptr[2]; (ch = ptr[0]); ptr ++) {
                if (ch == '*' && ptr[1] == '/') {
                    token[0] = 0;
                    return flecs_parse_ws_eol(ptr + 2);
                }
            }

            ecs_parser_error(name, expr, ptr - expr, 
                "missing */ for multiline comment");
            return NULL;
        }
    }

    ecs_expr_oper_t op;
    if (ptr[0] == '(') {
        token[0] = '(';
        token[1] = 0;
        return ptr + 1;
    } else if (ptr[0] != '-') {
        const char *tptr = flecs_str_to_expr_oper(ptr, &op);
        if (tptr) {
            ecs_os_strncpy(token, ptr, tptr - ptr);
            return tptr;
        }
    }

    while ((ptr = flecs_parse_expr_token(name, expr, ptr, token_ptr, 0))) {
        if (ptr[0] == '|' && ptr[1] != '|') {
            token_ptr = &token_ptr[ecs_os_strlen(token_ptr)];
            token_ptr[0] = '|';
            token_ptr[1] = '\0';
            token_ptr ++;
            ptr ++;
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

/* Attempt to resolve variable dotexpression to value (foo.bar) */
static
ecs_value_t flecs_dotresolve_var(
    ecs_world_t *world,
    ecs_script_vars_t *vars,
    char *token)
{
    char *dot = strchr(token, '.');
    if (!dot) {
        return (ecs_value_t){ .type = ecs_id(ecs_entity_t) };
    }

    dot[0] = '\0';

    const ecs_script_var_t *var = ecs_script_vars_lookup(vars, token);
    if (!var) {
        return (ecs_value_t){0};
    }

    ecs_meta_cursor_t cur = ecs_meta_cursor(
        world, var->value.type, var->value.ptr);
    ecs_meta_push(&cur);
    if (ecs_meta_dotmember(&cur, dot + 1) != 0) {
        return (ecs_value_t){0};
    }

    return (ecs_value_t){ 
        .ptr = ecs_meta_get_ptr(&cur),
        .type = ecs_meta_get_type(&cur)
    };
}

static
int flecs_meta_call(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_meta_cursor_t *cur, 
    const char *function) 
{
    ecs_entity_t type = ecs_meta_get_type(cur);
    void *value_ptr = ecs_meta_get_ptr(cur);

    if (!ecs_os_strcmp(function, "parent")) {
        if (type != ecs_id(ecs_entity_t)) {
            ecs_parser_error(name, expr, ptr - expr, 
                "parent() can only be called on entity");
            return -1;
        }

        *(ecs_entity_t*)value_ptr = ecs_get_parent(
            world, *(ecs_entity_t*)value_ptr);
    } else if (!ecs_os_strcmp(function, "name")) {
        if (type != ecs_id(ecs_entity_t)) {
            ecs_parser_error(name, expr, ptr - expr, 
                "name() can only be called on entity");
            return -1;
        }

        char **result = flecs_expr_value_new(stack, ecs_id(ecs_string_t));
        *result = ecs_os_strdup(ecs_get_name(world, *(ecs_entity_t*)value_ptr));
        *cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), result);
    } else if (!ecs_os_strcmp(function, "doc_name")) {
#ifdef FLECS_DOC
        if (type != ecs_id(ecs_entity_t)) {
            ecs_parser_error(name, expr, ptr - expr, 
                "name() can only be called on entity");
            return -1;
        }

        char **result = flecs_expr_value_new(stack, ecs_id(ecs_string_t));
        *result = ecs_os_strdup(ecs_doc_get_name(world, *(ecs_entity_t*)value_ptr));
        *cur = ecs_meta_cursor(world, ecs_id(ecs_string_t), result);
#else
        ecs_parser_error(name, expr, ptr - expr, 
            "doc_name() is not available without FLECS_DOC addon");
        return -1;
#endif
    } else {
        ecs_parser_error(name, expr, ptr - expr, 
            "unknown function '%s'", function);
        return -1;
    }

    return 0;
}

/* Determine the type of an expression from the first character(s). This allows
 * us to initialize a storage for a type if none was provided. */
static
ecs_entity_t flecs_parse_discover_type(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t input_type,
    const ecs_script_expr_run_desc_t *desc)
{
    /* String literal */
    if (ptr[0] == '"' || ptr[0] == '`') {
        if (input_type == ecs_id(ecs_char_t)) {
            return input_type;
        }
        return ecs_id(ecs_string_t);
    }

    /* Negative number literal */
    if (ptr[0] == '-') {
        if (!isdigit(ptr[1])) {
            ecs_parser_error(name, expr, ptr - expr, "invalid literal");
            return 0;
        }
        if (flecs_parse_is_float(ptr + 1)) {
            return ecs_id(ecs_f64_t);
        } else {
            return ecs_id(ecs_i64_t);
        }
    }

    /* Positive number literal */
    if (isdigit(ptr[0])) {
        if (flecs_parse_is_float(ptr)) {
            return ecs_id(ecs_f64_t);
        } else {
            return ecs_id(ecs_u64_t);
        }
    }

    /* Variable */
    if (ptr[0] == '$') {
        if (!desc || !desc->vars) {
            ecs_parser_error(name, expr, ptr - expr, 
                "unresolved variable (no variable scope)");
            return 0;
        }

        char token[ECS_MAX_TOKEN_SIZE];
        if (flecs_script_expr_parse_token(name, expr, &ptr[1], token) == NULL) {
            return 0;
        }

        const ecs_script_var_t *var = ecs_script_vars_lookup(desc->vars, token);
        if (!var) {
            ecs_size_t len = ecs_os_strlen(token);
            if (ptr[len + 1] == '(') {
                while ((len > 0) && (token[len] != '.')) {
                    len --;
                }
                if (token[len] == '.') {
                    token[len] = '\0';
                }

                return ecs_id(ecs_entity_t);
            }

            ecs_value_t v = flecs_dotresolve_var(world, desc->vars, token);
            if (v.type) {
                return v.type;
            }

            ecs_parser_error(name, expr, ptr - expr, 
                "unresolved variable '%s'", token);
            return 0;
        }
        return var->value.type;
    }

    /* Boolean */
    if (ptr[0] == 't' && !ecs_os_strncmp(ptr, "true", 4)) {
        if (!isalpha(ptr[4]) && ptr[4] != '_') {
            return ecs_id(ecs_bool_t);
        }
    }

    if (ptr[0] == 'f' && !ecs_os_strncmp(ptr, "false", 5)) {
        if (!isalpha(ptr[5]) && ptr[5] != '_') {
            return ecs_id(ecs_bool_t);
        }
    }

    /* Entity identifier */
    if (isalpha(ptr[0])) {
        if (!input_type) { /* Identifier could also be enum/bitmask constant */
            char token[ECS_MAX_TOKEN_SIZE];
            const char *tptr = flecs_script_expr_parse_token(
                name, expr, ptr, token);
            if (!tptr) {
                return 0;
            }

            if (tptr[0] != '[') {
                return ecs_id(ecs_entity_t);
            }

            tptr = flecs_script_expr_parse_token(
                name, expr, tptr + 1, token);
            if (!tptr) {
                return 0;
            }

            ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(desc->lookup_action != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_entity_t type = desc->lookup_action(
                world, token, desc->lookup_ctx);
            if (!type) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "unresolved type '%s'", token);
                return 0;
            }

            if (tptr[0] != ']') {
                ecs_parser_error(name, expr, ptr - expr, 
                    "missing ']' after '%s'", token);
                return 0;
            }

            if (tptr[1] != '.') {
                return type;
            }

            ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, NULL);
            ecs_meta_push(&cur);

            tptr = flecs_script_expr_parse_token(
                name, expr, tptr + 2, token);
            if (!tptr) {
                return 0;
            }

            if (ecs_meta_dotmember(&cur, token) != 0) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "failed to assign member '%s'", token);
                return 0;
            }

            return ecs_meta_get_type(&cur);
        }
    }

    /* If no default type was provided we can't automatically deduce the type of
     * composite/collection expressions. */
    if (!input_type) {
        if (ptr[0] == '{') {
            ecs_parser_error(name, expr, ptr - expr,
                "unknown type for composite literal");
            return 0;
        }

        if (ptr[0] == '[') {
            ecs_parser_error(name, expr, ptr - expr,
                "unknown type for collection literal");
            return 0;
        }

        ecs_parser_error(name, expr, ptr - expr, "invalid expression");
    }

    return input_type;
}

/* Normalize types to their largest representation.
 * Rather than taking the original type of a value, use the largest 
 * representation of the type so we don't have to worry about overflowing the
 * original type in the operation. */
static
ecs_entity_t flecs_largest_type(
    const EcsPrimitive *type)
{
    switch(type->kind) {
    case EcsBool:   return ecs_id(ecs_bool_t);
    case EcsChar:   return ecs_id(ecs_char_t);
    case EcsByte:   return ecs_id(ecs_u8_t);
    case EcsU8:     return ecs_id(ecs_u64_t);
    case EcsU16:    return ecs_id(ecs_u64_t);
    case EcsU32:    return ecs_id(ecs_u64_t);
    case EcsU64:    return ecs_id(ecs_u64_t);
    case EcsI8:     return ecs_id(ecs_i64_t);
    case EcsI16:    return ecs_id(ecs_i64_t);
    case EcsI32:    return ecs_id(ecs_i64_t);
    case EcsI64:    return ecs_id(ecs_i64_t);
    case EcsF32:    return ecs_id(ecs_f64_t);
    case EcsF64:    return ecs_id(ecs_f64_t);
    case EcsUPtr:   return ecs_id(ecs_u64_t);
    case EcsIPtr:   return ecs_id(ecs_i64_t);
    case EcsString: return ecs_id(ecs_string_t);
    case EcsEntity: return ecs_id(ecs_entity_t);
    case EcsId:     return ecs_id(ecs_id_t);
    default: ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }
error:
    return 0;
}

/** Test if a normalized type can promote to another type in an expression */
static
bool flecs_is_type_number(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t)) return false;
    else if (type == ecs_id(ecs_char_t)) return false;
    else if (type == ecs_id(ecs_u8_t)) return false;
    else if (type == ecs_id(ecs_u64_t)) return true;
    else if (type == ecs_id(ecs_i64_t)) return true;
    else if (type == ecs_id(ecs_f64_t)) return true;
    else if (type == ecs_id(ecs_string_t)) return false;
    else if (type == ecs_id(ecs_entity_t)) return false;
    else return false;
}

static
bool flecs_oper_valid_for_type(
    ecs_entity_t type,
    ecs_expr_oper_t op)
{
    switch(op) {
    case EcsAdd:
    case EcsSub:
    case EcsMul:
    case EcsDiv:
        return flecs_is_type_number(type);
    case EcsCondEq:
    case EcsCondNeq:
    case EcsCondAnd:
    case EcsCondOr:
    case EcsCondGt:
    case EcsCondGtEq:
    case EcsCondLt:
    case EcsCondLtEq:
        return flecs_is_type_number(type) ||
            (type == ecs_id(ecs_bool_t)) ||
            (type == ecs_id(ecs_char_t)) ||
            (type == ecs_id(ecs_entity_t));
    case EcsShiftLeft:
    case EcsShiftRight:
        return (type == ecs_id(ecs_u64_t));
    case EcsExprOperUnknown:
    case EcsLeftParen:
    case EcsMin:
        return false;
    default: 
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

/** Promote type to most expressive (f64 > i64 > u64) */
static
ecs_entity_t flecs_promote_type(
    ecs_entity_t type,
    ecs_entity_t promote_to)
{
    if (type == ecs_id(ecs_u64_t)) {
        return promote_to;
    }
    if (promote_to == ecs_id(ecs_u64_t)) {
        return type;
    }
    if (type == ecs_id(ecs_f64_t)) {
        return type;
    }
    if (promote_to == ecs_id(ecs_f64_t)) {
        return promote_to;
    }
    return ecs_id(ecs_i64_t);
}

static
int flecs_oper_precedence(
    ecs_expr_oper_t left,
    ecs_expr_oper_t right)
{
    return (left > right) - (left < right);
}

static
void flecs_value_cast(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    ecs_value_t *value,
    ecs_entity_t type)
{
    if (value->type == type) {
        return;
    }

    ecs_value_t result;
    result.type = type;
    result.ptr = flecs_expr_value_new(stack, type);

    if (value->ptr) {
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, type, result.ptr);
        ecs_meta_set_value(&cur, value);
    }

    *value = result;
}

static
bool flecs_expr_op_is_equality(
    ecs_expr_oper_t op)
{
    switch(op) {
    case EcsCondEq:
    case EcsCondNeq:
    case EcsCondGt:
    case EcsCondGtEq:
    case EcsCondLt:
    case EcsCondLtEq:
        return true;
    case EcsCondAnd:
    case EcsCondOr:
    case EcsShiftLeft:
    case EcsShiftRight:
    case EcsAdd:
    case EcsSub:
    case EcsMul:
    case EcsDiv:
    case EcsExprOperUnknown:
    case EcsLeftParen:
    case EcsMin:
        return false;
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }
error:
    return false;
}

static
ecs_entity_t flecs_binary_expr_type(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_value_t *lvalue,
    ecs_value_t *rvalue,
    ecs_expr_oper_t op,
    ecs_entity_t *operand_type_out)
{
    ecs_entity_t result_type = 0, operand_type = 0;

    switch(op) {
    case EcsDiv: 
        /* Result type of a division is always a float */
        *operand_type_out = ecs_id(ecs_f64_t);
        return ecs_id(ecs_f64_t);
    case EcsCondAnd:
    case EcsCondOr:
        /* Result type of a condition operator is always a bool */
        *operand_type_out = ecs_id(ecs_bool_t);
        return ecs_id(ecs_bool_t);
    case EcsCondEq:
    case EcsCondNeq:
    case EcsCondGt:
    case EcsCondGtEq:
    case EcsCondLt:
    case EcsCondLtEq:
        /* Result type of equality operator is always bool, but operand types
         * should not be casted to bool */
        result_type = ecs_id(ecs_bool_t);
        break;
    case EcsShiftLeft:
    case EcsShiftRight:
    case EcsAdd:
    case EcsSub:
    case EcsMul:
    case EcsExprOperUnknown:
    case EcsLeftParen:
    case EcsMin:
        break;
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }

    /* Result type for arithmetic operators is determined by operands */
    const EcsPrimitive *ltype_ptr = ecs_get(world, lvalue->type, EcsPrimitive);
    const EcsPrimitive *rtype_ptr = ecs_get(world, rvalue->type, EcsPrimitive);
    if (!ltype_ptr || !rtype_ptr) {
        char *lname = ecs_get_path(world, lvalue->type);
        char *rname = ecs_get_path(world, rvalue->type);
        ecs_parser_error(name, expr, ptr - expr, 
            "invalid non-primitive type in binary expression (%s, %s)",
                lname, rname);
        ecs_os_free(lname);
        ecs_os_free(rname);
        return 0;
    }

    ecs_entity_t ltype = flecs_largest_type(ltype_ptr);
    ecs_entity_t rtype = flecs_largest_type(rtype_ptr);
    if (ltype == rtype) {
        operand_type = ltype;
        goto done;
    }

    if (flecs_expr_op_is_equality(op)) {
        char *lstr = ecs_id_str(world, ltype);
        char *rstr = ecs_id_str(world, rtype);
        ecs_parser_error(name, expr, ptr - expr, 
            "mismatching types in equality expression (%s vs %s)",
                lstr, rstr);
        ecs_os_free(rstr);
        ecs_os_free(lstr);
        return 0;
    }

    if (!flecs_is_type_number(ltype) || !flecs_is_type_number(rtype)) {
        ecs_parser_error(name, expr, ptr - expr, 
            "incompatible types in binary expression");
        return 0;
    }

    operand_type = flecs_promote_type(ltype, rtype);

done:
    if (op == EcsSub && operand_type == ecs_id(ecs_u64_t)) {
        /* Result of subtracting two unsigned ints can be negative */
        operand_type = ecs_id(ecs_i64_t);
    }

    if (!result_type) {
        result_type = operand_type;
    }

    *operand_type_out = operand_type;
    return result_type;
error:
    return 0;
}

/* Macro's to let the compiler do the operations & conversion work for us */

#define ECS_VALUE_GET(value, T) (*(T*)value->ptr)

#define ECS_BINARY_OP_T(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, R) = ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T)

#define ECS_BINARY_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_i64_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_f64_t, ecs_f64_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_EQ_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        ecs_parser_error(name, expr, ptr - expr, "unsupported operator for floating point");\
        return -1;\
    } else if (left->type == ecs_id(ecs_u8_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if (left->type == ecs_id(ecs_char_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u64_t);\
    } else if (left->type == ecs_id(ecs_i64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_i64_t);\
    } else if (left->type == ecs_id(ecs_f64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_f64_t);\
    } else if (left->type == ecs_id(ecs_u8_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if (left->type == ecs_id(ecs_char_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_BOOL_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_bool_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_UINT_OP(left, right, result, op)\
    if (left->type == ecs_id(ecs_u64_t)) { \
        ECS_BINARY_OP_T(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

static
int flecs_binary_expr_do(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_value_t *lvalue,
    ecs_value_t *rvalue,
    ecs_value_t *result,
    ecs_expr_oper_t op)
{
    /* Find expression type */
    ecs_entity_t operand_type, type = flecs_binary_expr_type(
        world, name, expr, ptr, lvalue, rvalue, op, &operand_type);
    if (!type) {
        goto error;
    }

    if (!flecs_oper_valid_for_type(type, op)) {
        ecs_parser_error(name, expr, ptr - expr, "invalid operator for type");
        goto error;
    }

    flecs_value_cast(world, stack, lvalue, operand_type);
    flecs_value_cast(world, stack, rvalue, operand_type);

    ecs_value_t *storage = result;
    ecs_value_t tmp_storage = {0};
    if (result->type != type) {
        storage = &tmp_storage;
        storage->type = type;
        storage->ptr = flecs_expr_value_new(stack, type);
    }

    switch(op) {
    case EcsAdd:
        ECS_BINARY_OP(lvalue, rvalue, storage, +);
        break;
    case EcsSub:
        ECS_BINARY_OP(lvalue, rvalue, storage, -);
        break;
    case EcsMul:
        ECS_BINARY_OP(lvalue, rvalue, storage, *);
        break;
    case EcsDiv:
        ECS_BINARY_OP(lvalue, rvalue, storage, /);
        break;
    case EcsCondEq:
        ECS_BINARY_COND_EQ_OP(lvalue, rvalue, storage, ==);
        break;
    case EcsCondNeq:
        ECS_BINARY_COND_EQ_OP(lvalue, rvalue, storage, !=);
        break;
    case EcsCondGt:
        ECS_BINARY_COND_OP(lvalue, rvalue, storage, >);
        break;
    case EcsCondGtEq:
        ECS_BINARY_COND_OP(lvalue, rvalue, storage, >=);
        break;
    case EcsCondLt:
        ECS_BINARY_COND_OP(lvalue, rvalue, storage, <);
        break;
    case EcsCondLtEq:
        ECS_BINARY_COND_OP(lvalue, rvalue, storage, <=);
        break;
    case EcsCondAnd:
        ECS_BINARY_BOOL_OP(lvalue, rvalue, storage, &&);
        break;
    case EcsCondOr:
        ECS_BINARY_BOOL_OP(lvalue, rvalue, storage, ||);
        break;
    case EcsShiftLeft:
        ECS_BINARY_UINT_OP(lvalue, rvalue, storage, <<);
        break;
    case EcsShiftRight:
        ECS_BINARY_UINT_OP(lvalue, rvalue, storage, >>);
        break;
    case EcsExprOperUnknown:
    case EcsLeftParen:
    case EcsMin:
        ecs_parser_error(name, expr, ptr - expr, "unsupported operator");
        goto error;
    default:
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }

    if (storage->ptr != result->ptr) {
        if (!result->ptr) {
            *result = *storage;
        } else {
            ecs_meta_cursor_t cur = ecs_meta_cursor(world, 
                result->type, result->ptr);
            ecs_meta_set_value(&cur, storage);
        }
    }

    return 0;
error:
    return -1;
}

static
const char* flecs_binary_expr_parse(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_value_t *lvalue,
    ecs_value_t *result,
    ecs_expr_oper_t left_op,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_entity_t result_type = result->type;
    do {
        ecs_expr_oper_t op;

        ptr = flecs_str_to_expr_oper(ptr, &op);
        if (!ptr) {
            ecs_parser_error(name, expr, ptr - expr, "invalid operator");
            return NULL;
        }

        ptr = flecs_parse_ws_eol(ptr);

        ecs_value_t rvalue = {0};
        const char *rptr = flecs_script_expr_run(world, stack, ptr, &rvalue, op, desc);
        if (!rptr) {
            return NULL;
        }

        if (flecs_binary_expr_do(world, stack, name, expr, ptr, 
            lvalue, &rvalue, result, op)) 
        {
            return NULL;
        }

        ptr = rptr;

        ecs_expr_oper_t right_op;
        flecs_str_to_expr_oper(rptr, &right_op);
        if (right_op > left_op) {
            if (result_type) {
                /* If result was initialized, preserve its value */
                lvalue->type = result->type;
                lvalue->ptr = flecs_expr_value_new(stack, lvalue->type);
                ecs_value_copy(world, lvalue->type, lvalue->ptr, result->ptr);
                continue;
            } else {
                /* Otherwise move result to lvalue */
                *lvalue = *result;
                ecs_os_zeromem(result);
                continue;
            }
        }

        break;
    } while (true);

    return ptr;
}

static
const char* flecs_funccall_parse(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *name,
    const char *expr,
    const char *ptr,
    char *token,
    ecs_meta_cursor_t *cur,
    ecs_value_t *value,
    bool isvar,
    const ecs_script_expr_run_desc_t *desc)
{
    char *sep = strrchr(token, '.');
    if (!sep) {
        ecs_parser_error(name, expr, ptr - expr, 
            "missing object for function call '%s'", token);
        return NULL;
    }

    sep[0] = '\0';
    const char *function = sep + 1;

    if (!isvar) {
        if (ecs_meta_set_string(cur, token) != 0) {
            goto error;
        }
    } else {
        const ecs_script_var_t *var = ecs_script_vars_lookup(desc->vars, token);
        ecs_meta_set_value(cur, &var->value);
    }

    do {
        if (!function[0]) {
            ecs_parser_error(name, expr, ptr - expr, 
                "missing function name for function call '%s'", token);
            return NULL;
        }

        if (flecs_meta_call(world, stack, name, expr, ptr, cur, 
            function) != 0) 
        {
            goto error;
        }

        ecs_entity_t type = ecs_meta_get_type(cur);
        value->ptr = ecs_meta_get_ptr(cur);
        value->type = type;

        ptr += 2;
        if (ptr[0] != '.') {
            break;
        }

        ptr ++;
        char *paren = strchr(ptr, '(');
        if (!paren) {
            break;
        }

        ecs_size_t len = flecs_ito(int32_t, paren - ptr);
        if (len >= ECS_MAX_TOKEN_SIZE) {
            ecs_parser_error(name, expr, ptr - expr, 
                "token exceeds maximum token size");
            goto error;
        }

        ecs_os_strncpy(token, ptr, len);
        token[len] = '\0';
        function = token;

        ptr = paren;
    } while (true);

    return ptr;
error:
    return NULL;
}

static
ecs_entity_t flecs_script_default_lookup(
    const ecs_world_t *world,
    const char *name,
    void *ctx)
{
    (void)ctx;
    return ecs_lookup(world, name);
}

static
const char* flecs_script_expr_run(
    ecs_world_t *world,
    ecs_value_stack_t *stack,
    const char *ptr,
    ecs_value_t *value,
    ecs_expr_oper_t left_op,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
    char token[ECS_MAX_TOKEN_SIZE];
    int depth = 0;
    ecs_value_t result = {0};
    ecs_meta_cursor_t cur = {0};
    const char *name = desc ? desc->name : NULL;
    const char *expr = desc ? desc->expr : NULL;
    token[0] = '\0';
    expr = expr ? expr : ptr;

    ptr = flecs_parse_ws_eol(ptr);

    /* Check for postfix operators */
    ecs_expr_oper_t unary_op = EcsExprOperUnknown;
    if (ptr[0] == '-' && !isdigit(ptr[1])) {
        unary_op = EcsMin;
        ptr = flecs_parse_ws_eol(ptr + 1);
    }

    /* Initialize storage and cursor. If expression starts with a '(' storage
     * will be initialized by a nested expression */
    if (ptr[0] != '(') {
        ecs_entity_t type = flecs_parse_discover_type(
            world, name, expr, ptr, value->type, desc);
        if (!type) {
            return NULL;
        }

        result.type = type;
        if (type != value->type) {
            result.ptr = flecs_expr_value_new(stack, type);
        } else {
            result.ptr = value->ptr;
        }

        cur = ecs_meta_cursor(world, result.type, result.ptr);
        if (!cur.valid) {
            return NULL;
        }

        cur.lookup_action = desc ? desc->lookup_action : NULL;
        cur.lookup_ctx = desc ? desc->lookup_ctx : NULL;
    }

    /* Loop that parses all values in a value scope */
    while ((ptr = flecs_script_expr_parse_token(name, expr, ptr, token))) {
        /* Used to track of the result of the parsed token can be used as the
         * lvalue for a binary expression */
        bool is_lvalue = false;
        bool newline = false;

        if (!token[0]) {
            /* Comment */
            continue;
        }

        if (!ecs_os_strcmp(token, "(")) {
            ecs_value_t temp_result, *out;
            if (!depth) {
                out = &result;
            } else {
                temp_result.type = ecs_meta_get_type(&cur);
                temp_result.ptr = ecs_meta_get_ptr(&cur);
                out = &temp_result;
            }

            /* Parenthesis, parse nested expression */
            ptr = flecs_script_expr_run(
                world, stack, ptr, out, EcsLeftParen, desc);
            if (ptr[0] != ')') {
                ecs_parser_error(name, expr, ptr - expr, 
                    "missing closing parenthesis");
                return NULL;
            }
            ptr = flecs_parse_ws(ptr + 1);
            is_lvalue = true;

        } else if (!ecs_os_strcmp(token, "{")) {
            /* Parse nested value scope */
            ecs_entity_t scope_type = ecs_meta_get_type(&cur);

            depth ++; /* Keep track of depth so we know when parsing is done */
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (ecs_meta_is_collection(&cur)) {
                char *path = ecs_get_path(world, scope_type);
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
            /* Open collection value scope */
            depth ++;
            if (ecs_meta_push(&cur) != 0) {
                goto error;
            }

            if (!ecs_meta_is_collection(&cur)) {
                ecs_parser_error(name, expr, ptr - expr, "unexpected '['");
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

        else if (!ecs_os_strcmp(token, "-")) {
            if (unary_op != EcsExprOperUnknown) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "unexpected unary operator");
                return NULL;
            }
            unary_op = EcsMin;
        }

        else if (!ecs_os_strcmp(token, ",")) {
            /* Move to next field */
            if (ecs_meta_next(&cur) != 0) {
                goto error;
            }
        }

        else if (!ecs_os_strcmp(token, "null")) {
            if (ecs_meta_set_null(&cur) != 0) {
                goto error;
            }

            is_lvalue = true;
        }

        else if (token[0] == '\"') {
            /* Regular string */
            if (ecs_meta_set_string_literal(&cur, token) != 0) {
                goto error;
            }

            is_lvalue = true;
        }

        else if (!ecs_os_strcmp(token, "`")) {
            /* Multiline string */
            if (!(ptr = flecs_parse_multiline_string(&cur, name, expr, ptr))) {
                goto error;
            }

            is_lvalue = true;

        } else if (token[0] == '$') {
            /* Variable */
            if (!desc || !desc->vars) {
                ecs_parser_error(name, expr, ptr - expr, 
                    "unresolved variable '%s' (no variable scope)", token);
                return NULL;
            }

            if (!token[1]) {
                /* Empty name means default to assigned member */
                const char *member = ecs_meta_get_member(&cur);
                if (!member) {
                    ecs_parser_error(name, expr, ptr - expr, 
                        "invalid default variable outside member assignment");
                    return NULL;
                }
                ecs_os_strcpy(&token[1], member);
            }

            const ecs_script_var_t *var = ecs_script_vars_lookup(desc->vars, &token[1]);
            if (!var) {
                if (ptr[0] == '(') {
                    /* Function */
                    ptr = flecs_funccall_parse(world, stack, name, expr, ptr, 
                        &token[1], &cur, &result, true, desc);
                    if (!ptr) {
                        goto error;
                    }
                } else {
                    ecs_value_t v = flecs_dotresolve_var(world, desc->vars, &token[1]);
                    if (!v.ptr) {
                        ecs_parser_error(name, expr, ptr - expr, 
                            "unresolved variable '%s'", token);
                        return NULL;
                    } else {
                        ecs_meta_set_value(&cur, &v);
                    }
                }
            } else {
                ecs_meta_set_value(&cur, &var->value);
            }

            is_lvalue = true;
        } else {
            const char *tptr = flecs_parse_ws(ptr);
            for (; ptr != tptr; ptr ++) {
                if (ptr[0] == '\n') {
                    newline = true;
                }
            }

            if (ptr[0] == ':') {
                /* Member assignment */
                ptr ++;
                if (ecs_meta_dotmember(&cur, token) != 0) {
                    goto error;
                }
            } else if (ptr[0] == '(') {
                /* Function */
                ptr = flecs_funccall_parse(world, stack, name, expr, ptr, 
                    token, &cur, &result, false, desc);
                if (!ptr) {
                    goto error;
                }
            } else {
                if (ptr[0] != '[') {
                    /* Entity id expression */
                    if (ecs_meta_set_string(&cur, token) != 0) {
                        goto error;
                    }
                } else {
                    /* Component expression */
                    ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_assert(desc->lookup_action != NULL, ECS_INTERNAL_ERROR, NULL);
                    ecs_entity_t e = desc->lookup_action(
                        world, token, desc->lookup_ctx);
                    if (!e) {
                        ecs_parser_error(name, expr, ptr - expr, 
                            "entity '%s' not found", token);
                        goto error;
                    }

                    ptr = flecs_script_expr_parse_token(
                        name, expr, ptr + 1, token);
                    if (!ptr) {
                        goto error;
                    }

                    ecs_entity_t component = desc->lookup_action(
                        world, token, desc->lookup_ctx);
                    if (!component) {
                        ecs_parser_error(name, expr, ptr - expr, 
                            "unresolved component '%s'", token);
                        goto error;
                    }

                    ecs_entity_t type = ecs_get_typeid(world, component);
                    if (!type) {
                        ecs_parser_error(name, expr, ptr - expr, 
                            "entity '%s' is not a component", token);
                        goto error;
                    }

                    result.type = type;
                    result.ptr = ECS_CONST_CAST(void*, 
                        ecs_get_id(world, e, component));
                    if (!result.ptr) {
                        char *entitystr = ecs_id_str(world, e);
                        char *idstr = ecs_id_str(world, component);
                        ecs_parser_error(name, expr, ptr - expr, 
                            "entity '%s' does not have component '%s'", 
                            entitystr, idstr);
                        ecs_os_free(idstr);
                        ecs_os_free(entitystr);
                        goto error;
                    }

                    if (ptr[0] != ']') {
                        ecs_parser_error(name, expr, ptr - expr, 
                            "missing ] for component operator");
                        goto error;
                    }

                    ptr ++;

                    if (ptr[0] == '.') {
                        ecs_meta_cursor_t member_cur = ecs_meta_cursor(
                            world, result.type, result.ptr);

                        ptr = flecs_script_expr_parse_token(
                            name, expr, ptr + 1, token);
                        if (!ptr) {
                            goto error;
                        }

                        ecs_meta_push(&member_cur);
                        if (ecs_meta_dotmember(&member_cur, token) != 0) {
                            ecs_parser_error(name, expr, ptr - expr, 
                                "failed to assign member '%s'", token);
                            goto error;
                        }

                        result.type = ecs_meta_get_type(&member_cur);
                        result.ptr = ecs_meta_get_ptr(&member_cur);
                    }
                }
            }

            is_lvalue = true;
        }

        /* If lvalue was parsed, apply operators. Expressions cannot start
         * directly after a newline character. */
        if (is_lvalue && !newline) {
            if (unary_op != EcsExprOperUnknown) {
                if (unary_op == EcsMin) {
                    int64_t v = -1;
                    ecs_value_t lvalue = {.type = ecs_id(ecs_i64_t), .ptr = &v};
                    ecs_value_t *out, rvalue, temp_out = {0};

                    if (!depth) {
                        rvalue = result;
                        ecs_os_zeromem(&result);
                        out = &result;
                    } else {
                        ecs_entity_t cur_type = ecs_meta_get_type(&cur);
                        void *cur_ptr = ecs_meta_get_ptr(&cur);
                        rvalue.type = cur_type;
                        rvalue.ptr = cur_ptr;
                        temp_out.type = cur_type;
                        temp_out.ptr = cur_ptr;
                        out = &temp_out;
                    }

                    flecs_binary_expr_do(world, stack, name, expr, ptr, &lvalue,
                        &rvalue, out, EcsMul);
                }
                unary_op = 0;
            }

            ecs_expr_oper_t right_op;
            flecs_str_to_expr_oper(ptr, &right_op);
            if (right_op) {
                /* This is a binary expression, test precedence to determine if
                 * it should be evaluated here */
                if (flecs_oper_precedence(left_op, right_op) < 0) {
                    ecs_value_t lvalue;
                    ecs_value_t *op_result = &result;
                    ecs_value_t temp_storage;
                    if (!depth) {
                        /* Root level value, move result to lvalue storage */
                        lvalue = result;
                        ecs_os_zeromem(&result);
                    } else {
                        /* Not a root level value. Move the parsed lvalue to a
                         * temporary storage, and initialize the result value
                         * for the binary operation with the current cursor */
                        ecs_entity_t cur_type = ecs_meta_get_type(&cur);
                        void *cur_ptr = ecs_meta_get_ptr(&cur);
                        lvalue.type = cur_type;
                        lvalue.ptr = flecs_expr_value_new(stack, cur_type);
                        ecs_value_copy(world, cur_type, lvalue.ptr, cur_ptr);
                        temp_storage.type = cur_type;
                        temp_storage.ptr = cur_ptr;
                        op_result = &temp_storage;
                    }

                    /* Do the binary expression */
                    ptr = flecs_binary_expr_parse(world, stack, name, expr, ptr, 
                        &lvalue, op_result, left_op, desc);
                    if (!ptr) {
                        return NULL;
                    }
                }
            }
        }

        if (!depth) {
            /* Reached the end of the root scope */
            break;
        }

        ptr = flecs_parse_ws_eol(ptr);
    }

    if (!value->ptr) {
        value->type = result.type;
        value->ptr = flecs_expr_value_new(stack, result.type);
    }

    if (value->ptr != result.ptr) {
        cur = ecs_meta_cursor(world, value->type, value->ptr);
        ecs_meta_set_value(&cur, &result);
    }

    ecs_assert(value->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value->ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    return ptr;
error:
    return NULL;
}

const char* ecs_script_expr_run(
    ecs_world_t *world,
    const char *ptr,
    ecs_value_t *value,
    const ecs_script_expr_run_desc_t *desc)
{
    ecs_script_expr_run_desc_t priv_desc = {0};
    if (desc) {
        priv_desc = *desc;
    }

    if (!priv_desc.lookup_action) {
        priv_desc.lookup_action = flecs_script_default_lookup;
    }

    /* Prepare storage for temporary values */
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_value_stack_t stack;
    stack.count = 0;
    stack.stage = stage;
    stack.stack = &stage->allocators.deser_stack;
    stack.cursor = flecs_stack_get_cursor(stack.stack);

    /* Parse expression */
    bool storage_provided = value->ptr != NULL;
    ptr = flecs_script_expr_run(
        world, &stack, ptr, value, EcsExprOperUnknown, &priv_desc);

    /* If no result value was provided, allocate one as we can't return a 
     * pointer to a temporary storage */
    if (!storage_provided && value->ptr) {
        ecs_assert(value->type != 0, ECS_INTERNAL_ERROR, NULL);
        void *temp_storage = value->ptr;
        value->ptr = ecs_value_new(world, value->type);
        ecs_value_move_ctor(world, value->type, value->ptr, temp_storage);
    }
    
    /* Cleanup temporary values */
    int i;
    for (i = 0; i < stack.count; i ++) {
        const ecs_type_info_t *ti = stack.values[i].ti;
        ecs_assert(ti->hooks.dtor != NULL, ECS_INTERNAL_ERROR, NULL);
        ti->hooks.dtor(stack.values[i].ptr, 1, ti);
    }
    flecs_stack_restore_cursor(stack.stack, stack.cursor);

    return ptr;
}

#endif
