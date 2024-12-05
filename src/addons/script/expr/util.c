/**
 * @file addons/script/expr/parser.c * brief Scriptexpoutsion parser.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

int flecs_value_copy_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_value_t *src)
{
    ecs_assert(dst->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->type != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->ptr != 0, ECS_INTERNAL_ERROR, NULL);

    if (src->type == dst->type) {
        ecs_value_copy(world, src->type, dst->ptr, src->ptr);
    } else {
        /* Cast value to desired output type */
        ecs_meta_cursor_t cur = ecs_meta_cursor(world, dst->type, dst->ptr);
        if (ecs_meta_set_value(&cur, src)) {
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
    ecs_script_token_kind_t operator)
{
    (void)script;
    switch(operator) {
    case EcsTokNot:
        ecs_assert(expr->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        ecs_assert(out->type == ecs_id(ecs_bool_t), ECS_INTERNAL_ERROR, NULL);
        *(bool*)out->ptr = !*(bool*)expr->ptr;
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
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokMod:
    case EcsTokBitwiseOr:
    case EcsTokBitwiseAnd:
    case EcsTokOptional:
    case EcsTokAnnotation:
    case EcsTokNewline:
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
    case EcsTokAnd:
    case EcsTokOr:
    case EcsTokMatch:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokIdentifier:
    case EcsTokString:
    case EcsTokNumber:
    case EcsTokKeywordModule:
    case EcsTokKeywordUsing:
    case EcsTokKeywordWith:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for binary expression");
    }

    return 0;
}

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_script_token_kind_t operator)
{
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
    case EcsTokIdentifier:
    case EcsTokString:
    case EcsTokNumber:
    case EcsTokKeywordModule:
    case EcsTokKeywordUsing:
    case EcsTokKeywordWith:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for binary expression");
    }

    return 0;
}

#endif
