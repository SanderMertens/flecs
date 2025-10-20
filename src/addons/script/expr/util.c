/**
 * @file addons/script/expr/parser.c * brief Scriptexpoutsion parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

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
        ecs_value_copy_w_type_info(
            world, src->type_info, dst->ptr, src->value.ptr);
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
        ecs_value_move(world, src->type, dst->ptr, src->ptr);
    } else {
        ecs_value_t tmp;
        tmp.type = src->type;
        tmp.ptr = ecs_value_new(world, src->type);
        ecs_value_move(world, src->type, tmp.ptr, src->ptr);

        /* Cast value to desired output type */
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst->type, dst->ptr);
        if (ecs_meta_set_value(&cur, &tmp)) {
            goto error;
        }

        ecs_value_free(world, src->type, tmp.ptr);
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
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for binary expression");
    }

    return 0;
}

#define ECS_VALUE_GET(value, T) (*(T*)(value)->ptr)

#define ECS_BOP_DO(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, R) = (R)(ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T))

#define ECS_BOP(left, right, result, op, R, T)\
    if ((result)->type == ecs_id(ecs_u64_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_u64_t, T);\
    } else if ((result)->type == ecs_id(ecs_u32_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_u32_t, T);\
    } else if ((result)->type == ecs_id(ecs_u16_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_u16_t, T);\
    } else if ((result)->type == ecs_id(ecs_u8_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_u8_t, T);\
    } else if ((result)->type == ecs_id(ecs_i64_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_i64_t, T);\
    } else if ((result)->type == ecs_id(ecs_i32_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_i32_t, T);\
    } else if ((result)->type == ecs_id(ecs_i16_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_i16_t, T);\
    } else if ((result)->type == ecs_id(ecs_i8_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_i8_t, T);\
    } else if ((result)->type == ecs_id(ecs_f64_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_f64_t, T);\
    } else if ((result)->type == ecs_id(ecs_f32_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_f32_t, T);\
    } else if ((result)->type == ecs_id(ecs_char_t)) { \
        ECS_BOP_DO(left, right, result, op, ecs_char_t, T);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BOP_COND(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, ecs_bool_t) = ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T)

#define ECS_BOP_ASSIGN(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, R) op (R)(ECS_VALUE_GET(right, T))

/* Unsigned operations */
#define ECS_BINARY_UINT_OPS(left, right, result, op, OP)\
    if ((right)->type == ecs_id(ecs_u64_t)) { \
        OP(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else if ((right)->type == ecs_id(ecs_u32_t)) { \
        OP(left, right, result, op, ecs_u32_t, ecs_u32_t);\
    } else if ((right)->type == ecs_id(ecs_u16_t)) { \
        OP(left, right, result, op, ecs_u16_t, ecs_u16_t);\
    } else if ((right)->type == ecs_id(ecs_u8_t)) { \
        OP(left, right, result, op, ecs_u8_t, ecs_u8_t);\
    }

/* Unsigned + signed operations */
#define ECS_BINARY_INT_OPS(left, right, result, op, OP)\
    ECS_BINARY_UINT_OPS(left, right, result, op, OP)\
     else if ((right)->type == ecs_id(ecs_i64_t)) { \
        OP(left, right, result, op, ecs_i64_t, ecs_i64_t);\
    } else if ((right)->type == ecs_id(ecs_i32_t)) { \
        OP(left, right, result, op, ecs_i32_t, ecs_i32_t);\
    } else if ((right)->type == ecs_id(ecs_i16_t)) { \
        OP(left, right, result, op, ecs_i16_t, ecs_i16_t);\
    } else if ((right)->type == ecs_id(ecs_i8_t)) { \
        OP(left, right, result, op, ecs_i8_t, ecs_i8_t);\
    }

/* Unsigned + signed + floating point operations */
#define ECS_BINARY_NUMBER_OPS(left, right, result, op, OP)\
    ECS_BINARY_INT_OPS(left, right, result, op, OP)\
      else if ((right)->type == ecs_id(ecs_f64_t)) { \
        OP(left, right, result, op, ecs_f64_t, ecs_f64_t);\
    } else if ((right)->type == ecs_id(ecs_f32_t)) { \
        OP(left, right, result, op, ecs_f32_t, ecs_f32_t);\
    }


/* Combinations + error checking */

#define ECS_BINARY_INT_OP(left, right, result, op)\
    ECS_BINARY_INT_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_UINT_OP(left, right, result, op)\
    ECS_BINARY_UINT_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_OP(left, right, result, op)\
    ECS_BINARY_NUMBER_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_EQ_OP(left, right, result, op)\
    ECS_BINARY_INT_OPS(left, right, result, op, ECS_BOP_COND)\
      else if ((right)->type == ecs_id(ecs_char_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if ((right)->type == ecs_id(ecs_u8_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if ((right)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else if ((right)->type == ecs_id(ecs_entity_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_entity_t, ecs_entity_t);\
    } else if ((right)->type == ecs_id(ecs_string_t)) { \
        char *lstr = *(char**)(left)->ptr;\
        char *rstr = *(char**)(right)->ptr;\
        if (lstr && rstr) {\
            *(bool*)(result)->ptr = ecs_os_strcmp(lstr, rstr) op 0;\
        } else {\
            *(bool*)(result)->ptr = lstr == rstr;\
        }\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_OP(left, right, result, op)\
    ECS_BINARY_NUMBER_OPS(left, right, result, op, ECS_BOP_COND)\
      else if ((right)->type == ecs_id(ecs_char_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if ((right)->type == ecs_id(ecs_u8_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if ((right)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_ASSIGN_OP(left, right, result, op)\
    ECS_BINARY_NUMBER_OPS(left, right, result, op, ECS_BOP_ASSIGN)\

#define ECS_BINARY_BOOL_OP(left, right, result, op)\
    if ((right)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_token_kind_t operator)
{
    (void)script;

    if (operator == EcsTokDiv || operator == EcsTokMod) {
        if (flecs_value_is_0(right)) {
            ecs_err("%s: division by zero", 
                script->name ? script->name : "anonymous script");
            return -1;
        }
    }

    switch(operator) {
    case EcsTokAdd:
        ECS_BINARY_OP(left, right, out, +);
        break;
    case EcsTokSub:
        ECS_BINARY_OP(left, right, out, -);
        break;
    case EcsTokMul:
        ECS_BINARY_OP(left, right, out, *);
        break;
    case EcsTokDiv:
        ECS_BINARY_OP(left, right, out, /);
        break;
    case EcsTokMod:
        ECS_BINARY_INT_OP(left, right, out, %);
        break;
    case EcsTokEq:
        ECS_BINARY_COND_EQ_OP(left, right, out, ==);
        break;
    case EcsTokNeq:
        ECS_BINARY_COND_EQ_OP(left, right, out, !=);
        break;
    case EcsTokGt:
        ECS_BINARY_COND_OP(left, right, out, >);
        break;
    case EcsTokGtEq:
        ECS_BINARY_COND_OP(left, right, out, >=);
        break;
    case EcsTokLt:
        ECS_BINARY_COND_OP(left, right, out, <);
        break;
    case EcsTokLtEq:
        ECS_BINARY_COND_OP(left, right, out, <=);
        break;
    case EcsTokAnd:
        ECS_BINARY_BOOL_OP(left, right, out, &&);
        break;
    case EcsTokOr:
        ECS_BINARY_BOOL_OP(left, right, out, ||);
        break;
    case EcsTokBitwiseAnd:
        ECS_BINARY_INT_OP(left, right, out, &);
        break;
    case EcsTokBitwiseOr:
        ECS_BINARY_INT_OP(left, right, out, |);
        break;
    case EcsTokShiftLeft:
        ECS_BINARY_INT_OP(left, right, out, <<);
        break;
    case EcsTokShiftRight:
        ECS_BINARY_INT_OP(left, right, out, >>);
        break;
    case EcsTokAddAssign:
        ECS_BINARY_ASSIGN_OP(left, right, out, +=);
        break;
    case EcsTokMulAssign:
        ECS_BINARY_ASSIGN_OP(left, right, out, *=);
        break;
    case EcsTokEnd:
    case EcsTokUnknown:
    case EcsTokScopeOpen:
    case EcsTokScopeClose:
    case EcsTokParenOpen:
    case EcsTokParenClose:
    case EcsTokBracketOpen:
    case EcsTokBracketClose:
    case EcsTokMember:
    case EcsTokComma:
    case EcsTokSemiColon:
    case EcsTokColon:
    case EcsTokAssign:
    case EcsTokNot:
    case EcsTokOptional:
    case EcsTokAnnotation:
    case EcsTokNewline:
    case EcsTokMatch:
    case EcsTokRange:
    case EcsTokIdentifier:
    case EcsTokString:
    case EcsTokChar:
    case EcsTokNumber:
    case EcsTokKeywordModule:
    case EcsTokKeywordUsing:
    case EcsTokKeywordWith:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordFor:
    case EcsTokKeywordIn:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordMatch:
    case EcsTokKeywordNew:
    case EcsTokKeywordExport:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for binary expression");
    }

    return 0;
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
           if (type == ecs_id(ecs_i8_t)) {
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

#endif
