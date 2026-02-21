/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../../meta/meta.h"
#include "../script.h"

static
int flecs_expr_visit_type_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc);

bool flecs_expr_is_type_integer(
    ecs_entity_t type)
{
         if (type == ecs_id(ecs_u8_t))     return true;
    else if (type == ecs_id(ecs_u16_t))    return true;
    else if (type == ecs_id(ecs_u32_t))    return true;
    else if (type == ecs_id(ecs_u64_t))    return true;
    else if (type == ecs_id(ecs_uptr_t))   return true;
    else if (type == ecs_id(ecs_i8_t))     return true;
    else if (type == ecs_id(ecs_i16_t))    return true;
    else if (type == ecs_id(ecs_i32_t))    return true;
    else if (type == ecs_id(ecs_i64_t))    return true;
    else if (type == ecs_id(ecs_iptr_t))   return true;
    else return false;
}

bool flecs_expr_is_type_number(
    ecs_entity_t type)
{
         if (flecs_expr_is_type_integer(type)) return true;
    else if (type == ecs_id(ecs_f32_t))        return true;
    else if (type == ecs_id(ecs_f64_t))        return true;
    else return false;
}

/* Returns how expressive a type is. This is used to determine whether an 
 * implicit cast is allowed, where only casts from less to more expressive types
 * are valid. */
static
int32_t flecs_expr_expressiveness_score(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t))   return 1;
    else if (type == ecs_id(ecs_char_t))   return 2;

    else if (type == ecs_id(ecs_u8_t))     return 2;
    else if (type == ecs_id(ecs_u16_t))    return 3;
    else if (type == ecs_id(ecs_u32_t))    return 4;
    else if (type == ecs_id(ecs_uptr_t))   return 5;
    else if (type == ecs_id(ecs_u64_t))    return 6;

    else if (type == ecs_id(ecs_i8_t))     return 7;
    else if (type == ecs_id(ecs_i16_t))    return 8;
    else if (type == ecs_id(ecs_i32_t))    return 9;
    else if (type == ecs_id(ecs_iptr_t))   return 10;
    else if (type == ecs_id(ecs_i64_t))    return 11;

    else if (type == ecs_id(ecs_f32_t))    return 12;
    else if (type == ecs_id(ecs_f64_t))    return 13;

    else if (type == ecs_id(ecs_string_t)) return -1;
    else if (type == ecs_id(ecs_entity_t)) return -1;
    else return false;
}

/* Returns a score based on the storage size of a type. This is used in 
 * combination with expressiveness to determine whether a type can be implicitly
 * casted. An implicit cast is only valid if the destination type is both more
 * expressive and has a larger storage size. */
static
ecs_size_t flecs_expr_storage_score(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t))   return 1;
    else if (type == ecs_id(ecs_char_t))   return 1;

    /* Unsigned integers have a larger storage size than signed integers, since
     * the unsigned range of a signed integer is smaller. */
    else if (type == ecs_id(ecs_u8_t))     return 2;
    else if (type == ecs_id(ecs_u16_t))    return 3;
    else if (type == ecs_id(ecs_u32_t))    return 4;
    else if (type == ecs_id(ecs_uptr_t))   return 6;
    else if (type == ecs_id(ecs_u64_t))    return 7;

    else if (type == ecs_id(ecs_i8_t))     return 1;
    else if (type == ecs_id(ecs_i16_t))    return 2;
    else if (type == ecs_id(ecs_i32_t))    return 3;
    else if (type == ecs_id(ecs_iptr_t))   return 5;
    else if (type == ecs_id(ecs_i64_t))    return 6;

    /* Floating points have a smaller storage score, since the largest integer 
     * that can be represented exactly is lower than the actual storage size. */
    else if (type == ecs_id(ecs_f32_t))    return 3;
    else if (type == ecs_id(ecs_f64_t))    return 4;

    else if (type == ecs_id(ecs_string_t)) return -1;
    else if (type == ecs_id(ecs_entity_t)) return -1;
    else return false;
}

/** Returns the storage size for primitive type */
ecs_size_t flecs_expr_storage_size(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t))   return ECS_SIZEOF(ecs_bool_t);
    else if (type == ecs_id(ecs_char_t))   return ECS_SIZEOF(ecs_char_t);

    /* Unsigned integers have a larger storage size than signed integers, since
     * the unsigned range of a signed integer is smaller. */
    else if (type == ecs_id(ecs_u8_t))     return ECS_SIZEOF(ecs_u8_t);
    else if (type == ecs_id(ecs_u16_t))    return ECS_SIZEOF(ecs_u16_t);
    else if (type == ecs_id(ecs_u32_t))    return ECS_SIZEOF(ecs_u32_t);
    else if (type == ecs_id(ecs_uptr_t))   return ECS_SIZEOF(ecs_uptr_t);
    else if (type == ecs_id(ecs_u64_t))    return ECS_SIZEOF(ecs_u64_t);

    else if (type == ecs_id(ecs_i8_t))     return ECS_SIZEOF(ecs_i8_t);
    else if (type == ecs_id(ecs_i16_t))    return ECS_SIZEOF(ecs_i16_t);
    else if (type == ecs_id(ecs_i32_t))    return ECS_SIZEOF(ecs_i32_t);
    else if (type == ecs_id(ecs_iptr_t))   return ECS_SIZEOF(ecs_iptr_t);
    else if (type == ecs_id(ecs_i64_t))    return ECS_SIZEOF(ecs_i64_t);

    /* Floating points have a smaller storage score, since the largest integer 
     * that can be represented exactly is lower than the actual storage size. */
    else if (type == ecs_id(ecs_f32_t))    return ECS_SIZEOF(ecs_f32_t);
    else if (type == ecs_id(ecs_f64_t))    return ECS_SIZEOF(ecs_f64_t);

    else if (type == ecs_id(ecs_string_t)) return ECS_SIZEOF(ecs_string_t);
    else if (type == ecs_id(ecs_entity_t)) return ECS_SIZEOF(ecs_entity_t);
    else return false;
}

/* This function returns true if an type can be casted without changing the 
 * precision of the value. It is used to determine a type for operands in a 
 * binary expression in case they are of different types. */
static
bool flecs_expr_implicit_cast_allowed(
    ecs_entity_t from,
    ecs_entity_t to)
{
    int32_t from_e = flecs_expr_expressiveness_score(from);
    int32_t to_e = flecs_expr_expressiveness_score(to);
    if (from_e == -1 || to_e == -1) {
        return false;
    }

    if (to_e >= from_e) {
        return flecs_expr_storage_score(to) >= flecs_expr_storage_score(from);
    }

    return false;
}

static
ecs_entity_t flecs_expr_cast_to_lvalue(
    ecs_entity_t lvalue,
    ecs_entity_t operand)
{
    if (flecs_expr_implicit_cast_allowed(operand, lvalue)) {
        return lvalue;
    }

    return operand;
}

static
ecs_entity_t flecs_expr_narrow_type(
    ecs_entity_t lvalue,
    ecs_expr_node_t *node)
{
    ecs_entity_t type = node->type;

    if (node->kind != EcsExprValue) {
        return type;
    }

    if (!flecs_expr_is_type_number(type)) {
        return type;
    }

    void *ptr = ((ecs_expr_value_node_t*)node)->ptr;

    uint64_t uval;

    if (type == ecs_id(ecs_u8_t)) {
        uval = *(ecs_u8_t*)ptr;
    } else if (type == ecs_id(ecs_u16_t)) {
        uval = *(ecs_u16_t*)ptr;
    } else if (type == ecs_id(ecs_u32_t)) {
        uval = *(ecs_u32_t*)ptr;
    } else if (type == ecs_id(ecs_u64_t)) {
        uval = *(ecs_u32_t*)ptr;
    } else {
        int64_t ival;

        if (type == ecs_id(ecs_i8_t)) {
            ival = *(ecs_i8_t*)ptr;
        } else if (type == ecs_id(ecs_i16_t)) {
            ival = *(ecs_i16_t*)ptr;
        } else if (type == ecs_id(ecs_i32_t)) {
            ival = *(ecs_i32_t*)ptr;
        } else if (type == ecs_id(ecs_i64_t)) {
            ival = *(ecs_i64_t*)ptr;
        } else {
            /* If the lvalue type is a floating point type we can narrow the
             * literal to that since we'll lose double precision anyway. */
            if (lvalue == ecs_id(ecs_f32_t)) {
                return ecs_id(ecs_f32_t);
            }
            return type;
        }

        if (ival <= INT8_MAX && ival >= INT8_MIN) {
            return ecs_id(ecs_i8_t);
        } else if (ival <= INT16_MAX && ival >= INT16_MIN) {
            return ecs_id(ecs_i16_t);
        } else if (ival <= INT32_MAX && ival >= INT32_MIN) {
            return ecs_id(ecs_i32_t);
        } else {
            return ecs_id(ecs_i64_t);
        }
    }

    if (uval <= UINT8_MAX) {
        return ecs_id(ecs_u8_t);
    } else if (uval <= UINT16_MAX) {
        return ecs_id(ecs_u16_t);
    } else if (uval <= UINT32_MAX) {
        return ecs_id(ecs_u32_t);
    } else {
        return ecs_id(ecs_u64_t);
    }
}

static
bool flecs_expr_oper_valid_for_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_token_kind_t op)
{
    switch(op) {
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokMod:
    case EcsTokAddAssign:
    case EcsTokMulAssign:
        return flecs_expr_is_type_number(type);
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
        if (ecs_get(world, type, EcsBitmask) != NULL) {
            return true;
        }

        /* fall through */
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
        return flecs_expr_is_type_integer(type);
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokAnd:
    case EcsTokOr:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        return flecs_expr_is_type_number(type) ||
            (type == ecs_id(ecs_bool_t)) ||
            (type == ecs_id(ecs_char_t)) ||
            (type == ecs_id(ecs_entity_t));
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
    case EcsTokFunction:
    case EcsTokChar:
    case EcsTokString:
    case EcsTokNumber:
    case EcsTokKeywordModule:
    case EcsTokKeywordUsing:
    case EcsTokKeywordWith:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordFor:
    case EcsTokKeywordIn:
    case EcsTokKeywordMatch:
    case EcsTokKeywordNew:
    case EcsTokKeywordExport:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokEnd:
    default: 
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static
int32_t flecs_script_get_vector_type_data(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_entity_t *vector_type_out)
{
    const EcsStruct *stype = ecs_get(world, type, EcsStruct);
    if (!stype) {
        return 0;
    }

    ecs_entity_t vector_type = 0;
    ecs_member_t *members = ecs_vec_first(&stype->members);
    int32_t i, count = ecs_vec_count(&stype->members);
    for (i = 0; i < count; i ++) {
        ecs_member_t *member = &members[i];
        if (!i) {
            if (!ecs_owns(world, member->type, EcsPrimitive)) {
                /* Only primitive types can be used in vector ops */
                return 0;
            }

            vector_type = member->type;
        } else if (member->type != vector_type) {
            /* Only structs with members of the same type can be used as vector */
            return 0;
        }
    }

    *vector_type_out = vector_type;

    return count;
}

static
int flecs_expr_type_for_operator(
    ecs_script_t *script,
    ecs_expr_node_t *node, /* Only used for error reporting */
    ecs_entity_t node_type,
    ecs_expr_node_t *left,
    ecs_expr_node_t *right,
    ecs_token_kind_t operator,
    ecs_entity_t *operand_type,
    ecs_entity_t *result_type,
    int32_t *vector_elem_count)
{
    ecs_world_t *world = script->world;

    if (operator == EcsTokDiv || operator == EcsTokMod) {
        if (right->kind == EcsExprValue) {
            ecs_expr_value_node_t *val = (ecs_expr_value_node_t*)right;
            ecs_value_t v = { .type = val->node.type, .ptr = val->ptr };
            if (flecs_value_is_0(&v)) {
                flecs_expr_visit_error(script, node, 
                    "invalid division by zero");
                return -1;
            }
        }
    }

    switch(operator) {
    case EcsTokDiv: 
        /* Result type of a division is always a float */
        if (left->type != ecs_id(ecs_f32_t) && left->type != ecs_id(ecs_f64_t)){
            *operand_type = ecs_id(ecs_f64_t);
            *result_type = ecs_id(ecs_f64_t);
        } else {
            *operand_type = left->type;
            *result_type = left->type;
        }

        return 0;
    case EcsTokMod:
        /* Mod only accepts integer operands, and results in an integer. We 
         * could disallow doing mod on floating point types, but in practice
         * that would likely just result in code having to do a manual 
         * conversion to an integer. */
        *operand_type = ecs_id(ecs_i64_t);
        *result_type = ecs_id(ecs_i64_t);
        return 0;
    case EcsTokAnd:
    case EcsTokOr:
        /* Result type of a condition operator is always a bool */
        *operand_type = ecs_id(ecs_bool_t);
        *result_type = ecs_id(ecs_bool_t);
        return 0;
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        /* Result type of equality operator is always bool, but operand types
         * should not be casted to bool */
        *result_type = ecs_id(ecs_bool_t);
        break;
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
        break;
    case EcsTokAddAssign:
    case EcsTokMulAssign:
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
    case EcsTokFunction:
    case EcsTokChar:
    case EcsTokString:
    case EcsTokNumber:
    case EcsTokKeywordModule:
    case EcsTokKeywordUsing:
    case EcsTokKeywordWith:
    case EcsTokKeywordIf:
    case EcsTokKeywordElse:
    case EcsTokKeywordFor:
    case EcsTokKeywordIn:
    case EcsTokKeywordMatch:
    case EcsTokKeywordNew:
    case EcsTokKeywordExport:
    case EcsTokKeywordTemplate:
    case EcsTokKeywordProp:
    case EcsTokKeywordConst:
    case EcsTokEnd:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }

    /* If one of the types is an entity or id, the other one should be also */
    if (left->type == ecs_id(ecs_entity_t) || 
        right->type == ecs_id(ecs_entity_t)) 
    {
        *operand_type = ecs_id(ecs_entity_t);
        goto done;
    }

    ecs_entity_t left_type = left->type;
    const EcsPrimitive *ltype_ptr = ecs_get(world, left_type, EcsPrimitive);
    const EcsPrimitive *rtype_ptr = ecs_get(world, right->type, EcsPrimitive);
    if (!ltype_ptr || !rtype_ptr) {
        if (ecs_get(world, left_type, EcsBitmask) != NULL) {
            *operand_type = ecs_id(ecs_u32_t);
            goto done;
        }

        if (ecs_get(world, right->type, EcsBitmask) != NULL) {
            *operand_type = ecs_id(ecs_u32_t);
            goto done;
        }

        {
            const EcsEnum *ptr = ecs_get(script->world, left_type, EcsEnum);
            if (ptr) {
                *operand_type = ptr->underlying_type;
                goto done;
            }
        }

        {
            const EcsEnum *ptr = ecs_get(script->world, right->type, EcsEnum);
            if (ptr) {
                *operand_type = ptr->underlying_type;
                goto done;
            }
        }

        if (!ltype_ptr) {
            ecs_entity_t vector_type = 0;
            int32_t elem_count = flecs_script_get_vector_type_data(
                world, left_type, &vector_type);
            if (elem_count) {
                /* For vector operations the right operand type must be either a 
                 * primitive type or the same type as the left operand. */
                if (!rtype_ptr && (left_type != right->type)) {
                    flecs_expr_visit_error(script, node, 
                        "invalid types for binary expression (%s, %s)", 
                        flecs_errstr(ecs_get_path(world, left->type)),
                        flecs_errstr_1(ecs_get_path(world, right->type)));
                    goto error;
                }

                /* This is a binary vector operation, check if it's allowed */
                if (!vector_elem_count) {
                    flecs_expr_visit_error(script, node, 
                        "binary vector operation is not allowed here (%s, %s)", 
                        flecs_errstr(ecs_get_path(world, left->type)),
                        flecs_errstr_1(ecs_get_path(world, right->type)));
                    goto error;
                }

                *result_type = left_type;
                *operand_type = vector_type;
                *vector_elem_count = elem_count;
                goto done;
            }
        }

        flecs_expr_visit_error(script, node, 
            "invalid types for binary expression (%s, %s)", 
            flecs_errstr(ecs_get_path(world, left->type)),
            flecs_errstr_1(ecs_get_path(world, right->type)))
        goto error;
    }

    /* If left and right type are the same, do nothing */
    if (left_type == right->type) {
        *operand_type = left->type;
        goto done;
    }

    /* If types are not the same, find the smallest type for literals that can
     * represent the value without losing precision. */
    ecs_entity_t ltype;
    if (left_type == left->type) { /* If this is not a vector type */
        ltype = flecs_expr_narrow_type(node_type, left);
    } else {
        ltype = left_type;
    }

    ecs_entity_t rtype = flecs_expr_narrow_type(node_type, right);

    /* If types are not the same, try to implicitly cast to expression type */
    ltype = flecs_expr_cast_to_lvalue(node_type, ltype);
    rtype = flecs_expr_cast_to_lvalue(node_type, rtype);

    if (ltype == rtype) {
        *operand_type = ltype;
        goto done;
    }

    if (operator == EcsTokEq || operator == EcsTokNeq) {
        /* If this is an equality comparison and one of the operands is a bool, 
         * cast the other operand to a bool as well. This ensures that 
         * expressions such as true == 2 evaluate to true. */
        if (ltype == ecs_id(ecs_bool_t) || rtype == ecs_id(ecs_bool_t)) {
            *operand_type = ecs_id(ecs_bool_t);
            goto done;
        }

        /* Equality comparisons between floating point types are invalid */
        if (ltype == ecs_id(ecs_f32_t) || ltype == ecs_id(ecs_f64_t)) {
            flecs_expr_visit_error(script, node,
                "floating point value is invalid in equality comparison");
            goto error;
        }

        if (rtype == ecs_id(ecs_f32_t) || rtype == ecs_id(ecs_f64_t)) {
            flecs_expr_visit_error(script, node,
                "floating point value is invalid in equality comparison");
            goto error;
        }
    }

    /* If after the implicit cast types are not the same, try to implicitly cast
     * to the most expressive type. */
    if (flecs_expr_expressiveness_score(ltype) >= 
        flecs_expr_expressiveness_score(rtype)) 
    {
        if (flecs_expr_implicit_cast_allowed(rtype, ltype)) {
            *operand_type = ltype;
            goto done;
        }
    } else {
        if (flecs_expr_implicit_cast_allowed(ltype, rtype)) {
            *operand_type = rtype;
            goto done;
        }
    }

    /* If we get here one or both operands cannot be coerced to the same type
     * while guaranteeing no loss of precision. Pick the type that's least 
     * likely to cause trouble. */

    if (flecs_expr_is_type_number(ltype) && flecs_expr_is_type_number(rtype)) {

        /* If one of the types is a floating point, use f64 */
        if (ltype == ecs_id(ecs_f32_t) || ltype == ecs_id(ecs_f64_t) ||
            rtype == ecs_id(ecs_f32_t) || rtype == ecs_id(ecs_f64_t))
        {
            *operand_type = ecs_id(ecs_f64_t);
            goto done;
        }

        /* If one of the types is an integer, use i64 */
        if (ltype == ecs_id(ecs_i8_t) || ltype == ecs_id(ecs_i16_t) ||
            ltype == ecs_id(ecs_i32_t) || ltype == ecs_id(ecs_i64_t))
        {
            *operand_type = ecs_id(ecs_i64_t);
            goto done;
        }
        if (rtype == ecs_id(ecs_i8_t) || rtype == ecs_id(ecs_i16_t) ||
            rtype == ecs_id(ecs_i32_t) || rtype == ecs_id(ecs_i64_t))
        {
            *operand_type = ecs_id(ecs_i64_t);
            goto done;
        }
    }

    /* If all of that didn't work, give up */

    char *ltype_str = ecs_id_str(world, ltype);
    char *rtype_str = ecs_id_str(world, rtype);
    flecs_expr_visit_error(script, node,
        "incompatible types in expression (%s vs %s)", 
            ltype_str, rtype_str);
    ecs_os_free(ltype_str);
    ecs_os_free(rtype_str);
error:
    return -1;

done:
    if (operator == EcsTokSub && *operand_type == ecs_id(ecs_u64_t)) {
        /* Result of subtracting two unsigned ints can be negative */
        *operand_type = ecs_id(ecs_i64_t);
    }

    if (!*result_type) {
        *result_type = *operand_type;
    }

    if (ecs_get(script->world, *result_type, EcsBitmask) != NULL) {
        *operand_type = ecs_id(ecs_u64_t);
    }

    return 0;
}

static
int flecs_expr_type_for_binary_expr(
    ecs_script_t *script,
    ecs_expr_binary_t *node,
    ecs_entity_t *operand_type,
    ecs_entity_t *result_type,
    int32_t *vector_elem_count)
{
    return flecs_expr_type_for_operator(script, (ecs_expr_node_t*)node, 
        node->node.type, node->left, node->right, node->operator, 
        operand_type, result_type, vector_elem_count);
}

static
int flecs_expr_interpolated_string_visit_type(
    ecs_script_t *script,
    ecs_expr_interpolated_string_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    char *ptr, *frag = NULL;
    char ch;

    for (ptr = node->value; (ch = ptr[0]); ptr ++) {
        if (ch == '\\') {
            ptr ++;
            /* Next character is escaped, ignore */
            continue;
        }

        if ((ch == '$') && (isspace(ptr[1]) || !ptr[1])) {
            /* $ by itself */
            continue;
        }

        if (ch == '$' || ch == '{') {
            if (!frag) {
                frag = node->value;
            }

            char *frag_end = ptr;

            ecs_expr_node_t *result = NULL;

            if (ch == '$') {
                char *var_name = ++ ptr;
                ptr = ECS_CONST_CAST(char*, flecs_tokenizer_identifier(
                    NULL, ptr, NULL));
                if (!ptr) {
                    goto error;
                }

                /* Fiddly, but reduces need for allocations */
                ecs_size_t var_name_pos = flecs_ito(int32_t, var_name - node->value);
                var_name = &node->buffer[var_name_pos];
                ecs_size_t var_name_end = flecs_ito(int32_t, ptr - node->value);
                node->buffer[var_name_end] = '\0';

                ecs_expr_variable_t *var = flecs_expr_variable_from(
                    script, (ecs_expr_node_t*)node, var_name);
                if (!var) {
                    goto error;
                }

                result = (ecs_expr_node_t*)var;
            } else {
                ecs_script_impl_t *impl = flecs_script_impl(script);

                ecs_parser_t parser = {
                    .script = impl,
                    .scope = impl->root,
                    .significant_newline = false,
                    .token_cur = impl->token_remaining
                };

                ptr = ECS_CONST_CAST(char*, flecs_script_parse_expr(
                    &parser, ptr + 1, 0, &result));
                if (!ptr) {
                    goto error;
                }

                impl->token_remaining = parser.token_cur;

                if (ptr[0] != '}') {
                    flecs_expr_visit_error(script, node,
                        "expected '}' at end of interpolated expression");
                    goto error;
                }

                ptr ++;
            }

            ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_expr_eval_desc_t priv_desc = *desc;
            priv_desc.type = ecs_id(ecs_string_t); /* String output */

            if (flecs_expr_visit_type_priv(script, result, cur, &priv_desc)) {
                flecs_expr_visit_free(script, result);
                goto error;
            }

            if (result->type != ecs_id(ecs_string_t)) {
                result = (ecs_expr_node_t*)flecs_expr_cast(script, 
                    (ecs_expr_node_t*)result, ecs_id(ecs_string_t));
                if (!result) {
                    /* Cast failed */
                    goto error;
                }
            }

            ecs_vec_append_t(&((ecs_script_impl_t*)script)->allocator, 
                &node->expressions, ecs_expr_node_t*)[0] = result;

            frag_end[0] = '\0';

            if (frag != frag_end) {
                ecs_vec_append_t(&((ecs_script_impl_t*)script)->allocator, 
                    &node->fragments, char*)[0] = frag;
            }

            ecs_vec_append_t(&((ecs_script_impl_t*)script)->allocator, 
                &node->fragments, char*)[0] = NULL;

            frag = ptr; /* Point to next fragment */
            if (!ptr[0]) {
                break; /* We already parsed the end of the string */
            }
        }
    }

    /* This would mean it's not an interpolated string, which means the parser
     * messed up when creating the node. */
    ecs_assert(frag != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Add remaining fragment */
    if (frag != ptr) {
        ecs_vec_append_t(&((ecs_script_impl_t*)script)->allocator, 
            &node->fragments, char*)[0] = frag;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_initializer_collection_check(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    ecs_meta_cursor_t *cur)
{
    if (cur) {
        if (ecs_meta_is_collection(cur) != node->is_collection) {
            char *type_str = ecs_get_path(script->world, node->node.type);
            if (node->is_collection) {
                flecs_expr_visit_error(script, node, 
                    "invalid collection literal for non-collection type '%s'", 
                        type_str);
            } else {
                flecs_expr_visit_error(script, node, 
                    "invalid object literal for collection type '%s'",
                        type_str);
            }

            ecs_os_free(type_str);
            goto error;
        }
    }

    ecs_entity_t type = node->node.type;
    if (type) {
        const EcsOpaque *op = ecs_get(script->world, type, EcsOpaque);
        if (op) {
            type = op->as_type;
        }

        const EcsType *ptr = ecs_get(script->world, type, EcsType);
        if (ptr) {
            ecs_type_kind_t kind = ptr->kind;
            if (node->is_collection) {
                /* Only do this check if no cursor is provided. Cursors also 
                 * handle inline arrays. */
                if (!cur) {
                    if (kind != EcsArrayType && kind != EcsVectorType) {
                        char *type_str = ecs_get_path(
                            script->world, node->node.type);
                        flecs_expr_visit_error(script, node, 
                            "invalid collection literal for type '%s'",
                                type_str);
                        ecs_os_free(type_str);
                        goto error;
                    }
                }
            } else {
                if (kind != EcsStructType) {
                    char *type_str = ecs_get_path(
                        script->world, node->node.type);
                    flecs_expr_visit_error(script, node, 
                        "invalid object literal for type '%s'", type_str);
                    ecs_os_free(type_str);
                    goto error;
                }
            }
        }
    }

    return 0;
error:
    return -1;
}

/* Dynamic initializers use the cursor API to assign values, and are used for 
 * any type where a simple list of offsets into fields doesn't work. */
static
bool flecs_expr_initializer_is_dynamic(
    ecs_world_t *world,
    ecs_entity_t type)
{
    const EcsType *t = ecs_get(world, type, EcsType);
    if (t) {
        return t->kind == EcsOpaqueType || t->kind == EcsVectorType;
    }
    return false;
}

static
int flecs_expr_empty_initializer_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    (void)desc;

    node->node.type = ecs_meta_get_type(cur);
    if (!node->node.type) {
        flecs_expr_visit_error(script, node, 
            "unknown type for initializer");
        goto error;
    }

    node->is_dynamic = flecs_expr_initializer_is_dynamic(
        script->world, node->node.type);

    if (ecs_meta_push(cur)) {
        goto error;
    }

    if (flecs_expr_initializer_collection_check(script, node, cur)) {
        goto error;
    }

    if (ecs_meta_pop(cur)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_initializer_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    if (!cur || !cur->valid) {
        flecs_expr_visit_error(script, node, "missing type for initializer");
        goto error;
    }

    ecs_entity_t type = ecs_meta_get_type(cur);
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);

    /* Opaque types do not have deterministic offsets */
    bool is_dynamic = flecs_expr_initializer_is_dynamic(script->world, type);
    node->is_dynamic = is_dynamic;

    if (ecs_meta_push(cur)) {
        goto error;
    }

    if (flecs_expr_initializer_collection_check(script, node, cur)) {
        goto error;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        if (i) {
            if (ecs_meta_next(cur)) { /* , */
                goto error;
            }
        }

        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member) {
            if (ecs_meta_dotmember(cur, elem->member)) { /* x: */
                flecs_expr_visit_error(script, node, "cannot resolve member");
                goto error;
            }
        }

        /* Check for "member: $" syntax */
        if (elem->value->kind == EcsExprVariable) {
            ecs_expr_variable_t *var = (ecs_expr_variable_t*)elem->value;
            if (var->name && !var->name[0]) {
                var->name = ecs_meta_get_member(cur);
                if (!var->name) {
                    flecs_expr_visit_error(script, node, 
                        "cannot deduce variable name: not a member");
                    goto error;
                }
            }
        }

        ecs_entity_t elem_type = ecs_meta_get_type(cur);
        ecs_meta_cursor_t elem_cur = *cur;
        if (flecs_expr_visit_type_priv(
            script, elem->value, &elem_cur, desc)) 
        {
            goto error;
        }

        if (elem->value->type != elem_type) {
            ecs_expr_node_t *cast = (ecs_expr_node_t*)flecs_expr_cast(
                script, elem->value, elem_type);
            if (!cast) {
                goto error;
            }
            elem->value = cast;
        }

        if (elem->operator) {
            if (!flecs_expr_oper_valid_for_type(
                script->world, elem_type, elem->operator))
            {
                char *type_str = ecs_get_path(script->world, elem_type);
                flecs_expr_visit_error(script, node, 
                    "invalid operator for type '%s'", type_str);
                ecs_os_free(type_str);
                goto error;
            }
        }

        if (!is_dynamic) {
            elem->offset = (uintptr_t)ecs_meta_get_ptr(cur);
        } else {
            if (elem->value->kind == EcsExprInitializer) {
                /* If initializer is dynamic, make sure nested initializer is
                 * marked as dynamic too. This is necessary because a push for
                 * a nested initializer may have to allocate elements in the 
                 * parent collection value. */
                ((ecs_expr_initializer_t*)elem->value)->is_dynamic = true;
            }
        }
    }

    node->node.type = type;

    if (ecs_meta_pop(cur)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_unary_visit_type(
    ecs_script_t *script,
    ecs_expr_unary_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    if (flecs_expr_visit_type_priv(script, node->expr, cur, desc)) {
        goto error;
    }

    /* The only supported unary expression is not (!) which returns a bool */
    node->node.type = ecs_id(ecs_bool_t);

    if (node->expr->type != ecs_id(ecs_bool_t)) {
        node->expr = (ecs_expr_node_t*)flecs_expr_cast(
            script, node->expr, ecs_id(ecs_bool_t));
        if (!node->expr) {
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_binary_visit_type(
    ecs_script_t *script,
    ecs_expr_binary_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{    
    /* Operands must be of this type or casted to it */
    ecs_entity_t operand_type = 0;

    /* Resulting type of binary expression */
    ecs_entity_t result_type = 0;

    /* Number of elements in vector, if vector operation */
    int32_t vector_elem_count = 0;

    if (cur->valid) {
        /* Provides a hint to the type visitor. The lvalue type will be used to
         * reduce the number of casts where possible. */
        node->node.type = ecs_meta_get_type(cur);

        /* If the result of the binary expression is a boolean it's likely a 
         * conditional expression. We don't want to hint that the operands 
         * of conditional expressions should be casted to booleans. */
        if (node->node.type == ecs_id(ecs_bool_t)) {
            ecs_os_zeromem(cur);
        }
    }

    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    ecs_meta_cursor_t right_cur = *cur;
    if (node->right->kind == EcsExprIdentifier) {
        if (ecs_get(script->world, node->left->type, EcsEnum) != NULL) {
            /* If the left hand side is an enum, interpret untyped identifiers
             * on the right hand side as enum constants of the same type. */
            right_cur = ecs_meta_cursor(script->world, node->left->type, NULL);
        }
    }

    if (flecs_expr_visit_type_priv(script, node->right, &right_cur, desc)) {
        goto error;
    }

    if (flecs_expr_type_for_binary_expr(
        script, node, &operand_type, &result_type, &vector_elem_count)) 
    {
        goto error;
    }

    if (!vector_elem_count) {
        if (!flecs_expr_oper_valid_for_type(
            script->world, result_type, node->operator)) 
        {
            char *type_str = ecs_get_path(script->world, result_type);
            flecs_expr_visit_error(script, node, "invalid operator %s for type '%s'",
                flecs_token_str(node->operator), type_str);
            ecs_os_free(type_str);
            goto error;
        }
        
        if (operand_type != node->left->type) {
            node->left = (ecs_expr_node_t*)flecs_expr_cast(
                script, node->left, operand_type);
            if (!node->left) {
                goto error;
            }
        }

        node->vector_type = 0;
        node->vector_count = 0;
    } else {
        node->vector_type = operand_type;
        node->vector_count = vector_elem_count;
    }

    if (operand_type != node->right->type) {
        if (!vector_elem_count || (node->right->type != node->left->type)) {
            /* If this is a vector operation between the same types, don't try
             * to cast the right hand to the vector type. */
            node->right = (ecs_expr_node_t*)flecs_expr_cast(
                script, node->right, operand_type);
            if (!node->right) {
                goto error;
            }
        }
    }

    node->node.type = result_type;

    return 0;
error:
    return -1;
}

static
int flecs_expr_constant_identifier_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node)
{
    ecs_expr_value_node_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, node->node.type);

    ecs_meta_cursor_t expr_cur = ecs_meta_cursor(
        script->world, node->node.type, &result->storage.u64);
    if (ecs_meta_set_string(&expr_cur, node->value)) {
        flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
        goto error;
    }

    result->ptr = &result->storage.u64;
    node->expr = (ecs_expr_node_t*)result;

    return 0;
error:
    return -1;
}

static
bool flecs_expr_is_entity_type(
    ecs_world_t *world,
    ecs_entity_t type,
    bool *is_opaque)
{
    *is_opaque = false;
    if (type == ecs_id(ecs_entity_t) || type == ecs_id(ecs_id_t)) {
        return true;
    }

    const EcsPrimitive *p = ecs_get(world, type, EcsPrimitive);
    if (p) {
        return p->kind == EcsEntity || p->kind == EcsId;
    }

    const EcsOpaque *o = ecs_get(world, type, EcsOpaque);
    if (o) {
        *is_opaque = true;
        bool dummy;
        return flecs_expr_is_entity_type(world, o->as_type, &dummy);
    }

    return false;
}

static
int flecs_expr_identifier_variable_member_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    char *member_sep = strchr(node->value, '.');
    while (member_sep) {
        if (member_sep != node->value && member_sep[-1] == '\\') {
            member_sep = strchr(member_sep + 1, '.');
            continue;
        }
        break;
    }

    if (!member_sep) {
        return 1;
    }

    member_sep[0] = '\0';

    if (!flecs_script_find_var(desc->vars, node->value, NULL)) {
        member_sep[0] = '.';
        return 1;
    }

    ecs_expr_variable_t *var_node = flecs_expr_variable_from(
        script, (ecs_expr_node_t*)node, node->value);
    ecs_expr_member_t *member_node = flecs_expr_member_from(
        script, (ecs_expr_node_t*)var_node, &member_sep[1]);

    node->expr = (ecs_expr_node_t*)member_node;

    ecs_meta_cursor_t tmp_cur; ecs_os_zeromem(&tmp_cur);
    if (flecs_expr_visit_type_priv(script, node->expr, &tmp_cur, desc)) {
        goto error;
    }

    node->node.type = node->expr->type;
    return 0;
error:
    flecs_expr_visit_free(script, node->expr);
    node->expr = NULL;
    return -1;
}

static
int flecs_expr_identifier_visit_type(
    ecs_script_t *script,
    ecs_expr_identifier_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    (void)desc;

    ecs_entity_t type = node->node.type;
    if (cur->valid) {
        type = ecs_meta_get_type(cur);
    }

    const EcsType *type_ptr = NULL;
    if (type) {
        type_ptr = ecs_get(script->world, type, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (type_ptr && 
       (type_ptr->kind == EcsEnumType || type_ptr->kind == EcsBitmaskType)) 
    {
        /* If the requested type is an enum or bitmask, use cursor to resolve 
         * identifier to correct type constant. This lets us type 'Red' in places
         * where we expect a value of type Color, instead of Color.Red. */
        node->node.type = type;
        if (flecs_expr_constant_identifier_visit_type(script, node)) {
            goto error;
        }

        return 0;
    } else {
        /* If not, try to resolve the identifier as entity */
        ecs_entity_t e = desc->lookup_action(
            script->world, node->value, desc->lookup_ctx);
        if (e || !ecs_os_strcmp(node->value, "#0")) {
            const EcsScriptConstVar *global = NULL;
            if (e) {
                global = ecs_get(script->world, e, EcsScriptConstVar);
            }
            if (!global) {
                bool is_opaque = false;
                if (!type) {
                    type = ecs_id(ecs_entity_t);
                } else if (!flecs_expr_is_entity_type(script->world, type, &is_opaque)) {
                    char *type_str = ecs_get_path(script->world, type);
                    flecs_expr_visit_error(script, node,
                        "cannot cast identifier '%s' to %s",
                        node->value, type_str);
                    ecs_os_free(type_str);
                    goto error;
                }

                ecs_expr_value_node_t *result = flecs_expr_value_from(
                    script, (ecs_expr_node_t*)node, type);
                
                if (!is_opaque) {
                    result->storage.entity = e;
                    result->ptr = &result->storage.entity;
                } else {
                    ecs_size_t size = flecs_type_size(script->world, type);
                    ecs_assert(size > 0, ECS_INTERNAL_ERROR, NULL);
                    result->ptr = flecs_walloc(script->world, size);

                    ecs_meta_cursor_t expr_cur = ecs_meta_cursor(
                        script->world, type, result->ptr);
                    if (ecs_meta_set_entity(&expr_cur, e)) {
                        flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
                        goto error;
                    }
                }

                node->expr = (ecs_expr_node_t*)result;
                node->node.type = type;
            } else {
                ecs_expr_variable_t *var_node = flecs_expr_variable_from(
                    script, (ecs_expr_node_t*)node, node->value);
                node->expr = (ecs_expr_node_t*)var_node;
                node->node.type = global->value.type;

                ecs_meta_cursor_t tmp_cur; ecs_os_zeromem(&tmp_cur);
                if (flecs_expr_visit_type_priv(
                    script, (ecs_expr_node_t*)var_node, &tmp_cur, desc))
                {
                    goto error;
                }
            }

            return 0;
        }

        /* If identifier could not be resolved as entity, try as variable */
        int32_t var_sp = -1;
        ecs_script_var_t *var = flecs_script_find_var(
            desc->vars, node->value, &var_sp);
        if (var) {
            ecs_expr_variable_t *var_node = flecs_expr_variable_from(
                script, (ecs_expr_node_t*)node, node->value);
            node->expr = (ecs_expr_node_t*)var_node;
            node->node.type = var->value.type;

            ecs_meta_cursor_t tmp_cur; ecs_os_zeromem(&tmp_cur);
            if (flecs_expr_visit_type_priv(
                script, (ecs_expr_node_t*)var_node, &tmp_cur, desc))
            {
                goto error;
            }

            return 0;
        }

        int var_member_result = flecs_expr_identifier_variable_member_visit_type(
            script, node, desc);
        if (var_member_result == 0) {
            return 0;
        } else if (var_member_result == -1) {
            goto error;
        }

        /* If unresolved identifiers aren't allowed here, throw error */
        if (!desc->allow_unresolved_identifiers) {
            flecs_expr_visit_error(script, node, 
                "unresolved identifier '%s'", node->value);
            goto error;
        }

        /* Identifier will be resolved at eval time, default to entity */
        node->node.type = ecs_id(ecs_entity_t);
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_global_variable_resolve(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_world_t *world = script->world;
    ecs_entity_t global = desc->lookup_action(
        world, node->name, desc->lookup_ctx);
    if (!global) {
        flecs_expr_visit_error(script, node, "unresolved variable '%s'",
            node->name);
        goto error;
    }

    const EcsScriptConstVar *v = ecs_get(world, global, EcsScriptConstVar);
    if (!v) {
        char *str = ecs_get_path(world, global);
        flecs_expr_visit_error(script, node, 
            "entity '%s' is not a variable", node->name);
        ecs_os_free(str);
        goto error;
    }

    node->node.kind = EcsExprGlobalVariable;
    node->node.type = v->value.type;
    node->global_value = v->value;

    return 0;
error:
    return -1;
}

static
int flecs_expr_variable_visit_type(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_script_var_t *var = flecs_script_find_var(
        desc->vars, node->name, &node->sp);
    if (var) {
        node->node.type = var->value.type;
        if (!node->node.type) {
            flecs_expr_visit_error(script, node, 
                "variable '%s' is not initialized", node->name);
            goto error;
        }
    } else {
        if (flecs_expr_global_variable_resolve(script, node, desc)) {
            goto error;
        }
    }

    *cur = ecs_meta_cursor(script->world, node->node.type, NULL);

    return 0;
error:
    return -1;
}

static
int flecs_expr_global_variable_visit_type(
    ecs_script_t *script,
    ecs_expr_variable_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    (void)cur;

    if (flecs_expr_global_variable_resolve(script, node, desc)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_arguments_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_expr_eval_desc_t *desc,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t *vector_type_out)
{
    ecs_script_parameter_t *params = ecs_vec_first(&func_data->params);
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_entity_t vector_type = 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        ecs_entity_t argtype = params[i].type;

        if (argtype != EcsScriptVectorType) {
            ecs_meta_cursor_t cur = ecs_meta_cursor(
                script->world, argtype, NULL);

            if (flecs_expr_visit_type_priv(script, elem->value, &cur, desc)){
                goto error;
            }
        } else {
            ecs_meta_cursor_t cur;
            ecs_os_zeromem(&cur);

            if (flecs_expr_visit_type_priv(script, elem->value, &cur, desc)){
                goto error;
            }

            if (!vector_type) {
                vector_type = elem->value->type;
            }

            argtype = vector_type;

            /* Check if function provides implementation for type if argument
             * is of a primitive type. If it doesn't, try to cast. */
            const EcsPrimitive *p = ecs_get(
                script->world, argtype, EcsPrimitive);
            if (p) {
                if (!func_data->vector_callbacks[p->kind]) {
                    /* Fallback to types with max expressiveness */
                    if (func_data->vector_callbacks[EcsF64]) {
                        vector_type = argtype = ecs_id(ecs_f64_t);
                    } else if (func_data->vector_callbacks[EcsI64]) {
                        vector_type = argtype = ecs_id(ecs_i64_t);
                    } else if (func_data->vector_callbacks[EcsU64]) {
                        vector_type = argtype = ecs_id(ecs_u64_t);
                    } else {
                        /* No matching implementation. Error will be caught 
                         * later in the code. */
                    }
                }
            }
        }

        if (elem->value->type != argtype) {
            elem->value = (ecs_expr_node_t*)flecs_expr_cast(
                script, elem->value, argtype);
            if (!elem->value) {
                goto error;
            }
        }
    }

    *vector_type_out = vector_type;

    return 0;
error:
    return -1;
}

static
int flecs_expr_populate_primitive_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type,
    ecs_entity_t elem_type,
    int32_t count)
{
    ecs_world_t *world = script->world;
    const EcsPrimitive *ptype = ecs_get(world, elem_type, EcsPrimitive);
    if (!ptype) {
        flecs_expr_visit_error(script, node, 
            "type '%s' cannot be passed to vector argument of "
            "function '%s': (member) type '%s' is not a primitive type",
            flecs_errstr(ecs_get_path(world, vector_type)),
            node->function_name,
            flecs_errstr(ecs_get_path(world, elem_type)));
        goto error;
    }

    ecs_vector_function_callback_t cb = func_data->vector_callbacks[ptype->kind];
    if (!cb) {
        flecs_expr_visit_error(script, node, "function '%s' does not implement "
            "matching vector operation for type '%s'",
            node->function_name,
            flecs_errstr(ecs_get_path(script->world, elem_type)));
        goto error;
    }

    node->calldata.is.vector_callback = cb;
    node->calldata.vector_elem_count = count;

    return 0;
error:
    return -1;
}

static
int flecs_expr_populate_struct_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type)
{
    ecs_world_t *world = script->world;
    const EcsStruct *type = ecs_get(world, vector_type, EcsStruct);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t elem_type = 0;
    ecs_member_t *members = ecs_vec_first(&type->members);
    int32_t i, count = ecs_vec_count(&type->members);
    for (i = 0; i < count; i ++) {
        ecs_member_t *m = &members[i];
        if (!i) {
            elem_type = m->type;
        } else {
            if (m->type != elem_type) {
                flecs_expr_visit_error(script, node, 
                    "type '%s' cannot be passed to vector argument of "
                    "function '%s': all its members must be of the same type",
                    flecs_errstr(ecs_get_path(world, vector_type)),
                    node->function_name);
                goto error;
            }
        }
    }

    return flecs_expr_populate_primitive_vector_calldata(
        script, node, func_data, vector_type, elem_type, count);
error:
    return -1;
}

static
int flecs_expr_populate_array_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type)
{
    const EcsArray *type = ecs_get(script->world, vector_type, EcsArray);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_expr_populate_primitive_vector_calldata(
        script, node, func_data, vector_type, type->type, type->count);
}

static
int flecs_expr_populate_vector_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type)
{
    const EcsVector *type = ecs_get(script->world, vector_type, EcsVector);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    return flecs_expr_populate_primitive_vector_calldata(
        script, node, func_data, vector_type, type->type, -1);
}

static
int flecs_expr_populate_vector_calldata(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    const struct ecs_script_function_t *func_data,
    ecs_entity_t vector_type)
{
    ecs_world_t *world = script->world;
    const EcsType *type = ecs_get(world, vector_type, EcsType);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type->kind == EcsPrimitiveType) {
        return flecs_expr_populate_primitive_vector_calldata(
            script, node, func_data, vector_type, vector_type, 1);
    } else if (type->kind == EcsStructType) {
        return flecs_expr_populate_struct_vector_calldata(
            script, node, func_data, vector_type);
    } else if (type->kind == EcsArrayType) {
        return flecs_expr_populate_array_vector_calldata(
            script, node, func_data, vector_type);
    } else if (type->kind == EcsVectorType) {
        return flecs_expr_populate_vector_vector_calldata(
            script, node, func_data, vector_type);
    }

    flecs_expr_visit_error(script, node, 
        "type '%s' cannot be passed to vector argument of function '%s'",
        flecs_errstr(ecs_get_path(world, vector_type)),
        node->function_name);

    return -1;
}

static
int flecs_expr_function_visit_type(
    ecs_script_t *script,
    ecs_expr_function_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    bool is_method = false;
    char *last_elem = NULL;
    const char *func_identifier = NULL;

    if (node->left->kind == EcsExprIdentifier) {
        /* If identifier contains '.' separator(s), this is a method call, 
         * otherwise it's a regular function. */
        ecs_expr_identifier_t *ident = (ecs_expr_identifier_t*)node->left;
        func_identifier = ident->value;

        last_elem = strrchr(func_identifier, '.');
        if (last_elem && last_elem != ident->value && last_elem[-1] != '\\') {
            node->function_name = last_elem + 1;
            last_elem[0] = '\0';
            is_method = true;
        } else {
            node->function_name = ident->value;   
        }

    } else if (node->left->kind == EcsExprMember) {
        /* This is a method. Just like identifiers, method strings can contain
         * separators. Split off last separator to get the method. */
        ecs_expr_member_t *member = (ecs_expr_member_t*)node->left;
        last_elem = strrchr(member->member_name, '.');
        if (!last_elem) {
            node->left = member->left;
            node->function_name = member->member_name;

            member->left = NULL; /* Prevent cleanup */
            flecs_expr_visit_free(script, (ecs_expr_node_t*)member);
        } else {
            node->function_name = last_elem + 1;
            last_elem[0] = '\0';
        }
        is_method = true;
    }

    /* Left of function expression should not inherit lvalue type, since the
     * function return type is what's going to be assigned. */
    ecs_os_zeromem(cur);

    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    ecs_world_t *world = script->world;
    const ecs_vec_t *params = NULL;
    const struct ecs_script_function_t *func_data = NULL;

    /* If this is a method, lookup function entity in scope of type */
    if (is_method) {
        ecs_entity_t func = ecs_lookup_from(
            world, node->left->type, node->function_name);
        if (!func) {
            /* If identifier could be a function (not a method) try that */
            if (func_identifier) {
                is_method = false;
                last_elem[0] = '.';
                node->function_name = func_identifier;
                goto try_function;
            }

            char *type_str = ecs_get_path(world, node->left->type);
            flecs_expr_visit_error(script, node, 
                "unresolved method identifier '%s' for type '%s'", 
                node->function_name, type_str);
            ecs_os_free(type_str);
            goto error;
        }

        func_data = ecs_get(world, func, EcsScriptMethod);
        if (!func_data) {
            char *path = ecs_get_path(world, func);
            flecs_expr_visit_error(script, node, 
                "entity '%s' is not a valid method", path);
            ecs_os_free(path);
            goto error;
        }

        node->node.kind = EcsExprMethod;
        node->node.type = func_data->return_type;
        node->calldata.function = func;
        node->calldata.is.callback = func_data->callback;
        node->calldata.ctx = func_data->ctx;
        params = &func_data->params;
    }

try_function:
    if (!is_method) {
        ecs_entity_t func = desc->lookup_action(
            world, node->function_name, desc->lookup_ctx);
        if (!func) {
            flecs_expr_visit_error(script, node, 
                "unresolved function identifier '%s'", 
                node->function_name);
            goto error;
        }

        func_data = ecs_get(world, func, EcsScriptFunction);
        if (!func_data) {
            char *path = ecs_get_path(world, func);
            flecs_expr_visit_error(script, node, 
                "entity '%s' is not a valid method", path);
            ecs_os_free(path);
            goto error;
        }

        node->node.type = func_data->return_type;
        node->calldata.function = func;
        node->calldata.is.callback = func_data->callback;
        node->calldata.ctx = func_data->ctx;
        node->calldata.vector_elem_count = 0;
        params = &func_data->params;
    }

    int32_t count = ecs_vec_count(&node->args->elements);
    if (count != ecs_vec_count(params)) {
        flecs_expr_visit_error(script, node, "expected %d arguments, got %d",
            ecs_vec_count(params), count);
        goto error;
    }

    ecs_entity_t vector_type = 0;
    if (flecs_expr_arguments_visit_type(
        script, node->args, desc, func_data, &vector_type)) 
    {
        goto error;
    }

    if (vector_type) {
        if (node->node.type == EcsScriptVectorType) {
            node->node.type = vector_type;
        }

        if (flecs_expr_populate_vector_calldata(
            script, node, func_data, vector_type))
        {
            goto error;
        }
    } else {
        ecs_assert(node->node.type != EcsScriptVectorType, 
            ECS_INTERNAL_ERROR, NULL);
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_member_visit_type(
    ecs_script_t *script,
    ecs_expr_member_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    ecs_world_t *world = script->world;
    ecs_entity_t left_type = node->left->type;

    const EcsType *type = ecs_get(world, left_type, EcsType);
    if (!type) {
        char *type_str = ecs_get_path(world, left_type);
        flecs_expr_visit_error(script, node, 
            "cannot resolve member on value of type '%s' "
                "(missing reflection data)", type_str);
        ecs_os_free(type_str);
        goto error;
    }

    if (type->kind != EcsStructType) {
        char *type_str = ecs_get_path(world, left_type);
        flecs_expr_visit_error(script, node, 
            "cannot resolve member on non-struct type '%s'", type_str);
        ecs_os_free(type_str);
        goto error;
    }

    if (ecs_meta_push(cur)) {
        goto error;
    }

    int prev_log = ecs_log_set_level(-4);
    if (ecs_meta_dotmember(cur, node->member_name)) {
        ecs_log_set_level(prev_log);
        char *type_str = ecs_get_path(world, left_type);
        flecs_expr_visit_error(script, node, 
            "unresolved member '%s' for type '%s'", 
                node->member_name, type_str);
        ecs_os_free(type_str);
        goto error;
    }
    ecs_log_set_level(prev_log);

    node->node.type = ecs_meta_get_type(cur);
    node->offset = (uintptr_t)ecs_meta_get_ptr(cur);

    return 0;
error:
    return -1;
}

static
int flecs_expr_element_visit_type(
    ecs_script_t *script,
    ecs_expr_element_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_world_t *world = script->world;

    ecs_meta_cursor_t index_cur = {0};
    if (flecs_expr_visit_type_priv(
        script, node->index, &index_cur, desc)) 
    {
        goto error;
    }

    /* Check if this is a component expression */
    if (node->index->kind == EcsExprIdentifier) {
        /* Fetch type of left side of expression to check if it's of an entity
         * type. Pass in an empty cursor object so we don't fail type checks in
         * case it's a regular element expression. */
        ecs_meta_cursor_t tmp_cur = {0};
        if (flecs_expr_visit_type_priv(script, node->left, &tmp_cur, desc)) {
            goto error;
        }

        ecs_entity_t left_type = node->left->type;

        const EcsType *type = ecs_get(world, left_type, EcsType);
        if (!type) {
            char *type_str = ecs_get_path(world, left_type);
            flecs_expr_visit_error(script, node, 
                "cannot use [] on value of type '%s' (missing reflection data)",
                    type_str);
            ecs_os_free(type_str);
            goto error;
        }

        bool is_entity_type = false;

        if (type->kind == EcsPrimitiveType) {
            const EcsPrimitive *ptype = ecs_get(world, left_type, EcsPrimitive);
            if (ptype->kind == EcsEntity) {
                is_entity_type = true;
            }
        }

        if (is_entity_type) {
            ecs_expr_identifier_t *ident = (ecs_expr_identifier_t*)node->index;
            node->node.type = desc->lookup_action(
                script->world, ident->value, desc->lookup_ctx);
            if (!node->node.type) {
                flecs_expr_visit_error(script, node, 
                    "unresolved component identifier '%s'",
                        ident->value);
                goto error;
            }

            node->node.kind = EcsExprComponent;

            *cur = ecs_meta_cursor(script->world, node->node.type, NULL);

            return 0;
        }
    }

    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    if (ecs_meta_push(cur)) {
        goto not_a_collection;
    }

    if (!ecs_meta_is_collection(cur)) {
        goto not_a_collection;
    }

    node->node.type = ecs_meta_get_type(cur);

    const ecs_type_info_t *elem_ti = ecs_get_type_info(
        script->world, node->node.type);
    node->elem_size = elem_ti->size;

    return 0;

not_a_collection: {
    char *type_str = ecs_get_path(script->world, node->left->type);
    flecs_expr_visit_error(script, node, 
        "invalid usage of [] on non collection/entity type '%s'", type_str);
    ecs_os_free(type_str);
}
error:
    return -1;
}

static
bool flecs_expr_identifier_is_any(
    ecs_expr_node_t *node)
{
    if (node->kind == EcsExprIdentifier) {
        ecs_expr_identifier_t *id = (ecs_expr_identifier_t*)node;
        if (id->value && !ecs_os_strcmp(id->value, "_")) {
            return true;
        }
    }
    return false;
}

static
int flecs_expr_match_visit_type(
    ecs_script_t *script,
    ecs_expr_match_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_meta_cursor_t expr_cur;
    ecs_os_zeromem(&expr_cur);
    if (flecs_expr_visit_type_priv(script, node->expr, &expr_cur, desc)) {
        goto error;
    }

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    if (!count) {
        flecs_expr_visit_error(script, node, 
            "match statement must have at least one case");
        goto error;
    }

    /* Determine most expressive type of all elements */
    node->node.type = ecs_meta_get_type(cur);

    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];

        if (node->node.type) {
            expr_cur = ecs_meta_cursor(script->world, node->node.type, NULL);
        } else {
            ecs_os_zeromem(&expr_cur);
        }

        if (flecs_expr_visit_type_priv(script, elem->expr, &expr_cur, desc)) {
            goto error;
        }

        if (!node->node.type) {
            node->node.type = elem->expr->type;
            continue;
        }

        if (flecs_expr_is_type_number(node->node.type)) {
            ecs_entity_t result_type = 0, operand_type = 0;
            if (flecs_expr_type_for_operator(script, (ecs_expr_node_t*)node, 0, 
                (ecs_expr_node_t*)node, elem->expr, 
                EcsTokAdd, /* Use operator that doesn't change types */
                &operand_type, &result_type, NULL))
            {
                goto error;
            }

            /* "Accumulate" most expressive type in result node */
            node->node.type = result_type;
        } else {
            /* If type is not a number it must match exactly */
            if (elem->expr->type != node->node.type) {
                char *got = ecs_get_path(script->world, elem->expr->type);
                char *expect = ecs_get_path(script->world, node->node.type);
                flecs_expr_visit_error(script, node, 
                    "invalid type for case %d in match (got %s, expected %s)",
                        i + 1, got, expect);
                ecs_os_free(got);
                ecs_os_free(expect);
                goto error;
            }
        }
    }

    /* Loop over elements again, cast values to result type */
    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];
        if (elem->expr->type != node->node.type) {
            elem->expr = (ecs_expr_node_t*)
                flecs_expr_cast(script, elem->expr, node->node.type);
            if (!elem->expr) {
                goto error;
            }
        }
    }

    /* If this is an enum type, cast to the underlying type. This is necessary
     * because the compare operation executed by the match evaluation code isn't
     * implemented for enums. */
    ecs_entity_t expr_type = node->expr->type;
    {
        const EcsEnum *ptr = ecs_get(script->world, expr_type, EcsEnum);
        if (ptr) {
            node->expr = (ecs_expr_node_t*)
                flecs_expr_cast(script, node->expr, ptr->underlying_type);
        }
    }

    /* Do the same for bitmasks */
    {
        const EcsBitmask *ptr = ecs_get(script->world, expr_type, EcsBitmask);
        if (ptr) {
            /* For now bitmasks are always u32s */
            node->expr = (ecs_expr_node_t*)
                flecs_expr_cast(script, node->expr, ecs_id(ecs_u32_t));
        }
    }

    /* Make sure that case values match the input type */
    for (i = 0; i < count; i ++) {
        ecs_expr_match_element_t *elem = &elems[i];

        if (flecs_expr_identifier_is_any(elem->compare)) {
            if (i != count - 1) {
                flecs_expr_visit_error(script, node, 
                    "any (_) must be the last case in match");
                goto error;
            }

            node->any.compare = elem->compare;
            node->any.expr = elem->expr;
            ecs_vec_remove_last(&node->elements);
        } else {
            expr_cur = ecs_meta_cursor(script->world, expr_type, NULL);
            if (flecs_expr_visit_type_priv(
                script, elem->compare, &expr_cur, desc)) 
            {
                goto error;
            }

            ecs_expr_node_t *compare = elem->compare;
            if (compare->type != node->expr->type) {
                elem->compare = (ecs_expr_node_t*)
                    flecs_expr_cast(script, compare, node->expr->type);
                if (!elem->compare) {
                    goto error;
                }
            }
        }
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_new_visit_type(
    ecs_script_t *script,
    ecs_expr_new_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    (void)script;
    (void)cur;
    (void)desc;
    node->node.type = ecs_id(ecs_entity_t);
    return 0;
}

static
int flecs_expr_visit_type_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprInterpolatedString:
        if (flecs_expr_interpolated_string_visit_type(
            script, (ecs_expr_interpolated_string_t*)node, cur, desc))
        {
            goto error;
        }
        break;
    case EcsExprEmptyInitializer:
        if (flecs_expr_empty_initializer_visit_type(
            script, (ecs_expr_initializer_t*)node, cur, desc))
        {
            goto error;
        }
        break;
    case EcsExprInitializer:
        if (flecs_expr_initializer_visit_type(
            script, (ecs_expr_initializer_t*)node, cur, desc))
        {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_visit_type(
            script, (ecs_expr_unary_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_type(
            script, (ecs_expr_binary_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        if (flecs_expr_identifier_visit_type(
            script, (ecs_expr_identifier_t*)node, cur, desc))
        {
            goto error;
        }
        break;
    case EcsExprVariable:
        if (flecs_expr_variable_visit_type(
            script, (ecs_expr_variable_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprGlobalVariable:
        if (flecs_expr_global_variable_visit_type(
            script, (ecs_expr_variable_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprFunction:
        if (flecs_expr_function_visit_type(
            script, (ecs_expr_function_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprMember:
        if (flecs_expr_member_visit_type(
            script, (ecs_expr_member_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprElement:
        if (flecs_expr_element_visit_type(
            script, (ecs_expr_element_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprMatch:
        if (flecs_expr_match_visit_type(
            script, (ecs_expr_match_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprNew:
        if (flecs_expr_new_visit_type(
            script, (ecs_expr_new_t*)node, cur, desc)) 
        {
            goto error;
        }
        break;
    case EcsExprCast:
    case EcsExprCastNumber:
        break;
    case EcsExprMethod:
    case EcsExprComponent:
        /* Expressions are derived by type visitor */
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(node->type != 0, ECS_INTERNAL_ERROR, NULL);
    node->type_info = ecs_get_type_info(script->world, node->type);
    ecs_assert(node->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    return 0;
error:
    return -1;
}

int flecs_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    // ecs_strbuf_t buf = ECS_STRBUF_INIT;
    // flecs_expr_to_str_buf(script, node, &buf, true);
    // char *str = ecs_strbuf_get(&buf);
    // printf("%s\n", str);
    // ecs_os_free(str);

    if (node->kind == EcsExprEmptyInitializer) {
        node->type = desc->type;
        
        if (node->type) {
            ecs_expr_initializer_t* init_node = (ecs_expr_initializer_t*)node;
            init_node->is_dynamic = flecs_expr_initializer_is_dynamic(
                script->world, node->type);

            if (flecs_expr_initializer_collection_check(
                script, init_node, NULL))
            {
                return -1;
            }

            node->type_info = ecs_get_type_info(script->world, node->type);
            return 0;
        }
    }

    if (desc->type) {
        ecs_meta_cursor_t cur = ecs_meta_cursor(
            script->world, desc->type, NULL);
        return flecs_expr_visit_type_priv(script, node, &cur, desc);
    } else {
        ecs_meta_cursor_t cur;
        ecs_os_zeromem(&cur);
        return flecs_expr_visit_type_priv(script, node, &cur, desc);
    }
}

#endif
