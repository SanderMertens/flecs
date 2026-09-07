/**
 * @file addons/script/expr/util.c
 * @brief Script expression utilities.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

void flecs_expr_visit_error_(
    const ecs_script_t *script,
    const void *node,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    char *msg = flecs_vasprintf(fmt, args);
    va_end(args);

    const ecs_expr_node_t *expr_node = node;
    const char *code = script->code;
    const char *pos = expr_node ? expr_node->pos : NULL;

    if (code && pos && (pos >= code) && (pos <= &code[ecs_os_strlen(code)])) {
        ecs_parser_error(script->name, code, pos - code, "%s", msg);
    } else {
        ecs_parser_error(script->name, NULL, 0, "%s", msg);
    }

    ecs_os_free(msg);
}

static bool flecs_value_bool_to_number(
    ecs_value_t *dst,
    bool value)
{
    ecs_entity_t type = dst->type;
    void *ptr = dst->ptr;
    if (type == ecs_id(ecs_f32_t)) { *(float*)ptr = value ? 1.0f : 0.0f; return true; }
    if (type == ecs_id(ecs_f64_t)) { *(double*)ptr = value ? 1.0 : 0.0; return true; }
    if (type == ecs_id(ecs_i32_t)) { *(int32_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_i64_t)) { *(int64_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_u32_t)) { *(uint32_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_u64_t)) { *(uint64_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_i8_t))  { *(int8_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_i16_t)) { *(int16_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_u8_t))  { *(uint8_t*)ptr = value; return true; }
    if (type == ecs_id(ecs_u16_t)) { *(uint16_t*)ptr = value; return true; }
    return false;
}

int flecs_value_copy_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_expr_value_t *src)
{
    ecs_assert(dst->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->value.type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->value.ptr != 0, ECS_INTERNAL_ERROR, NULL);

    if (src->value.type == dst->type) {
        ecs_assert(src->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_ptr_copy_w_type_info(
            world, src->type_info, dst->ptr, src->value.ptr);
    } else if (src->value.type == ecs_id(ecs_bool_t) &&
        flecs_value_bool_to_number(dst, *(const bool*)src->value.ptr))
    {
        return 0;
    } else {
        /* Cast value to desired output type */
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst->type, dst->ptr);
        if (ecs_meta_set_value(&cur, &src->value)) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

int flecs_value_move_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    ecs_value_t *src)
{
    ecs_assert(dst->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->ptr != 0, ECS_INTERNAL_ERROR, NULL);

    if (src->type == dst->type) {
        ecs_ptr_move(world, src->type, dst->ptr, src->ptr);
    } else {
        ecs_value_t tmp;
        tmp.type = src->type;
        tmp.ptr = ecs_ptr_new(world, src->type);
        ecs_ptr_move(world, src->type, tmp.ptr, src->ptr);

        /* Cast value to desired output type */
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst->type, dst->ptr);
        if (ecs_meta_set_value(&cur, &tmp)) {
            goto error;
        }

        ecs_ptr_free(world, src->type, tmp.ptr);
    }

    return 0;
error:
    return -1;
}


int flecs_value_unary(
    const ecs_script_t *script,
    const ecs_value_t *expr,
    ecs_value_t *out,
    ecs_token_kind_t operator)
{
    (void)script;

    if (operator == EcsTokNot) {
        ecs_assert(expr->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        ecs_assert(out->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        *(bool*)out->ptr = !*(bool*)expr->ptr;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for unary expression");
    }

    return 0;
}

#define FLECS_VALUE_COMPARE(T)\
    case EcsTokEq: *(bool*)out = l == r; return 0;\
    case EcsTokNeq: *(bool*)out = l != r; return 0;\
    case EcsTokGt: *(bool*)out = l > r; return 0;\
    case EcsTokGtEq: *(bool*)out = l >= r; return 0;\
    case EcsTokLt: *(bool*)out = l < r; return 0;\
    case EcsTokLtEq: *(bool*)out = l <= r; return 0;

#define FLECS_VALUE_INTEGER(T)\
    case EcsTokMod: *(T*)out = (T)(l % r); return 0;\
    case EcsTokBitwiseAnd: *(T*)out = (T)(l & r); return 0;\
    case EcsTokBitwiseOr: *(T*)out = (T)(l | r); return 0;\
    case EcsTokShiftLeft: *(T*)out = (T)(l << r); return 0;\
    case EcsTokShiftRight: *(T*)out = (T)(l >> r); return 0;

#define FLECS_VALUE_FLOAT(T)

#define FLECS_VALUE_ARITHMETIC(T)\
    case EcsTokAdd: *(T*)out = (T)(l + r); return 0;\
    case EcsTokSub: *(T*)out = (T)(l - r); return 0;\
    case EcsTokMul: *(T*)out = (T)(l * r); return 0;\
    case EcsTokDiv: *(T*)out = (T)(l / r); return 0;\
    case EcsTokAddAssign: *(T*)out += r; return 0;\
    case EcsTokMulAssign: *(T*)out *= r; return 0;

#define FLECS_VALUE_BINARY(T, ARITHMETIC, INTEGER)\
static int flecs_value_binary_##T(\
    const void *left, const void *right, void *out, ecs_token_kind_t operator)\
{\
    T l = *(const T*)left, r = *(const T*)right;\
    switch (operator) {\
    FLECS_VALUE_COMPARE(T)\
    ARITHMETIC(T)\
    INTEGER(T)\
    default: ecs_abort(ECS_INTERNAL_ERROR, "invalid binary operator");\
    }\
    return -1;\
}

FLECS_VALUE_BINARY(ecs_i8_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i16_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_i64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u8_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u16_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_u64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_INTEGER)
FLECS_VALUE_BINARY(ecs_f32_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_f64_t, FLECS_VALUE_ARITHMETIC, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_char_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_bool_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)
FLECS_VALUE_BINARY(ecs_entity_t, FLECS_VALUE_FLOAT, FLECS_VALUE_FLOAT)

#undef FLECS_VALUE_BINARY
#undef FLECS_VALUE_ARITHMETIC
#undef FLECS_VALUE_INTEGER
#undef FLECS_VALUE_FLOAT
#undef FLECS_VALUE_COMPARE

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_expr_node_t *node,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_token_kind_t operator)
{
    if (operator == EcsTokAddAssign || operator == EcsTokMulAssign) {
        left = out;
    }
    if ((operator == EcsTokDiv || operator == EcsTokMod) && flecs_value_is_0(right)) {
        flecs_expr_visit_error(script, node, "division by zero");
        return -1;
    }
    if (operator == EcsTokAnd || operator == EcsTokOr) {
        bool l = *(bool*)left->ptr, r = *(bool*)right->ptr;
        *(bool*)out->ptr = operator == EcsTokAnd ? l && r : l || r;
        return 0;
    }
    if (right->type == ecs_id(ecs_string_t)) {
        char *l = *(char**)left->ptr, *r = *(char**)right->ptr;
        ecs_assert(operator == EcsTokEq || operator == EcsTokNeq, ECS_INTERNAL_ERROR, NULL);
        *(bool*)out->ptr = l && r
            ? (operator == EcsTokEq ? !ecs_os_strcmp(l, r) : ecs_os_strcmp(l, r) != 0)
            : l == r;
        return 0;
    }
    if (operator == EcsTokAdd || operator == EcsTokSub || operator == EcsTokMul ||
        operator == EcsTokDiv || operator == EcsTokMod || operator == EcsTokBitwiseAnd ||
        operator == EcsTokBitwiseOr || operator == EcsTokShiftLeft || operator == EcsTokShiftRight ||
        operator == EcsTokAddAssign || operator == EcsTokMulAssign)
    {
        ecs_assert(out->type == right->type, ECS_INTERNAL_ERROR, NULL);
    }
#define FLECS_VALUE_DISPATCH(T)\
    if (right->type == ecs_id(T)) {\
        return flecs_value_binary_##T(left->ptr, right->ptr, out->ptr, operator);\
    }
    FLECS_VALUE_DISPATCH(ecs_i8_t)
    FLECS_VALUE_DISPATCH(ecs_i16_t)
    FLECS_VALUE_DISPATCH(ecs_i32_t)
    FLECS_VALUE_DISPATCH(ecs_i64_t)
    FLECS_VALUE_DISPATCH(ecs_u8_t)
    FLECS_VALUE_DISPATCH(ecs_u16_t)
    FLECS_VALUE_DISPATCH(ecs_u32_t)
    FLECS_VALUE_DISPATCH(ecs_u64_t)
    FLECS_VALUE_DISPATCH(ecs_f32_t)
    FLECS_VALUE_DISPATCH(ecs_f64_t)
    FLECS_VALUE_DISPATCH(ecs_char_t)
    FLECS_VALUE_DISPATCH(ecs_bool_t)
    FLECS_VALUE_DISPATCH(ecs_entity_t)
#undef FLECS_VALUE_DISPATCH
    ecs_abort(ECS_INTERNAL_ERROR, "invalid binary operand type");
    return -1;
}

bool flecs_string_is_interpolated(
    const char *value)
{
    const char *ptr = value;

    for (ptr = strchr(ptr, '$'); ptr; ptr = strchr(ptr + 1, '$')) {
        if (ptr != value) {
            if (ptr[-1] == '\\') {
                continue; /* Escaped */
            }
        }

        if (isspace(ptr[1]) || !ptr[1]) {
            continue; /* $ by itself */
        }

        return true;
    }

    ptr = value;

    for (ptr = strchr(ptr, '{'); ptr; ptr = strchr(ptr + 1, '{')) {
        if (ptr != value) {
            if (ptr[-1] == '\\') {
                continue; /* Escaped */
            }
        }

        return true;
    }

    return false;
}

char* flecs_string_escape(
    char *str)
{
    const char *ptr;
    char *out = str, ch;

    for (ptr = str; ptr[0]; ) {
        if (ptr[0] == '\\') {
            if (ptr[1] == '{') { /* Escape string interpolation delimiter */
                ch = '{';
                ptr += 2;
            } else if (ptr[1] == '$') { /* Escape string interpolation var */
                ch = '$';
                ptr += 2;
            } else {
                ptr = flecs_chrparse(ptr, &ch);
                if (!ptr) {
                    ecs_err("invalid escape sequence in string '%s'", str);
                    return NULL;
                }
            }
        } else {
            ch = ptr[0];
            ptr ++;
        }

        out[0] = ch;
        out ++;
    }

    out[0] = '\0';

    return out + 1;
}

bool flecs_value_is_0(
    const ecs_value_t *value)
{
    ecs_entity_t type = value->type;
    void *ptr = value->ptr;
           if (type == ecs_id(ecs_bool_t)) {
        return *(ecs_bool_t*)ptr == 0;
    } else if (type == ecs_id(ecs_i8_t)) {
        return *(ecs_i8_t*)ptr == 0;
    } else if (type == ecs_id(ecs_i16_t)) {
        return *(ecs_i16_t*)ptr == 0;
    } else if (type == ecs_id(ecs_i32_t)) {
        return *(ecs_i32_t*)ptr == 0;
    } else if (type == ecs_id(ecs_i64_t)) {
        return *(ecs_i64_t*)ptr == 0;
    } else if (type == ecs_id(ecs_iptr_t)) {
        return *(ecs_iptr_t*)ptr == 0;
    } else if (type == ecs_id(ecs_u8_t)) {
        return *(ecs_u8_t*)ptr == 0;
    } else if (type == ecs_id(ecs_u16_t)) {
        return *(ecs_u16_t*)ptr == 0;
    } else if (type == ecs_id(ecs_u32_t)) {
        return *(ecs_u32_t*)ptr == 0;
    } else if (type == ecs_id(ecs_u64_t)) {
        return *(ecs_u64_t*)ptr == 0;
    } else if (type == ecs_id(ecs_uptr_t)) {
        return *(ecs_uptr_t*)ptr == 0;
    } else if (type == ecs_id(ecs_f32_t)) {
        return ECS_EQZERO(*(ecs_f32_t*)ptr);
    } else if (type == ecs_id(ecs_f64_t)) {
        return ECS_EQZERO(*(ecs_f64_t*)ptr);
    } else {
        return true;
    }
}

ecs_expr_swizzle_t* flecs_expr_expand_swizzle_get(
    ecs_expr_node_t *node)
{
    if (node->kind == EcsExprIdentifier) {
        node = ((ecs_expr_identifier_t*)node)->expr;
        if (!node) {
            return NULL;
        }
    }

    if (node->kind != EcsExprSwizzle) {
        return NULL;
    }

    ecs_expr_swizzle_t *swizzle = (ecs_expr_swizzle_t*)node;
    if (!swizzle->expand) {
        return NULL;
    }

    return swizzle;
}

#endif
