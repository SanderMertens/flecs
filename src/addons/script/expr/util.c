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
    if (src->type == dst->type) {
        /* Outputvalue issame asexpoutsion, copy value */
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

int flecs_value_binary(
    ecs_script_t *script,
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
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid operator for binary expression");
        goto error;
    }

    return 0;
error:
    return -1;
}

#endif
