/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

#define flecs_expr_visit_error(script, node, ...) \
    ecs_parser_error( \
        script->name, script->code, ((ecs_expr_node_t*)node)->pos - script->code, \
        __VA_ARGS__);

static
bool flecs_expr_operator_is_equality(
    ecs_script_token_kind_t op)
{
    switch(op) {
    case EcsTokEq:
    case EcsTokNeq:
    case EcsTokGt:
    case EcsTokGtEq:
    case EcsTokLt:
    case EcsTokLtEq:
        return true;
    case EcsTokAnd:
    case EcsTokOr:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
        return false;
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }
error:
    return false;
}

static
bool flecs_expr_is_type_number(
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
ecs_entity_t flecs_expr_largest_type(
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

/** Promote type to most expressive (f64 > i64 > u64) */
static
ecs_entity_t flecs_expr_promote_type(
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
bool flecs_expr_oper_valid_for_type(
    ecs_entity_t type,
    ecs_script_token_kind_t op)
{
    switch(op) {
    case EcsTokAdd:
    case EcsTokSub:
    case EcsTokMul:
    case EcsTokDiv:
        return flecs_expr_is_type_number(type);
    case EcsTokBitwiseAnd:
    case EcsTokBitwiseOr:
    case EcsTokShiftLeft:
    case EcsTokShiftRight:
        return type == ecs_id(ecs_u64_t) || 
               type == ecs_id(ecs_u32_t) || 
               type == ecs_id(ecs_u16_t) || 
               type == ecs_id(ecs_u8_t);
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
    default: 
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static
int flecs_expr_type_for_oper(
    ecs_script_t *script,
    ecs_expr_binary_t *node,
    ecs_entity_t *operand_type,
    ecs_entity_t *result_type)
{
    ecs_world_t *world = script->world;
    ecs_expr_node_t *left = node->left, *right = node->right;

    switch(node->operator) {
    case EcsTokDiv: 
        /* Result type of a division is always a float */
        *operand_type = ecs_id(ecs_f64_t);
        *result_type = ecs_id(ecs_f64_t);
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
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }

    const EcsPrimitive *ltype_ptr = ecs_get(world, left->type, EcsPrimitive);
    const EcsPrimitive *rtype_ptr = ecs_get(world, right->type, EcsPrimitive);
    if (!ltype_ptr || !rtype_ptr) {
        char *lname = ecs_get_path(world, left->type);
        char *rname = ecs_get_path(world, right->type);
        flecs_expr_visit_error(script, node, 
            "invalid non-primitive type in binary expression (%s, %s)", 
            lname, rname);
        ecs_os_free(lname);
        ecs_os_free(rname);
        return 0;
    }

    ecs_entity_t ltype = flecs_expr_largest_type(ltype_ptr);
    ecs_entity_t rtype = flecs_expr_largest_type(rtype_ptr);
    if (ltype == rtype) {
        *operand_type = ltype;
        goto done;
    }

    if (flecs_expr_operator_is_equality(node->operator)) {
        char *lname = ecs_id_str(world, ltype);
        char *rname = ecs_id_str(world, rtype);
        flecs_expr_visit_error(script, node, 
            "mismatching types in equality expression (%s vs %s)", 
            lname, rname);
        ecs_os_free(rname);
        ecs_os_free(lname);
        return 0;
    }

    if (!flecs_expr_is_type_number(ltype) || !flecs_expr_is_type_number(rtype)) {
        flecs_expr_visit_error(script, node,
            "incompatible types in binary expression");
        return 0;
    }

    *operand_type = flecs_expr_promote_type(ltype, rtype);

done:
    if (node->operator == EcsTokSub && *operand_type == ecs_id(ecs_u64_t)) {
        /* Result of subtracting two unsigned ints can be negative */
        *operand_type = ecs_id(ecs_i64_t);
    }

    if (!*result_type) {
        *result_type = *operand_type;
    }

    return 0;
error:
    return -1;
}

static
int flecs_expr_binary_visit_type(
    ecs_script_t *script,
    ecs_expr_binary_t *node)
{    
    /* Operands must be of this type or casted to it */
    ecs_entity_t operand_type = 0;

    /* Resulting type of binary expression */
    ecs_entity_t result_type = 0;

    if (flecs_script_expr_visit_type(script, node->left)) {
        goto error;
    }

    if (flecs_script_expr_visit_type(script, node->right)) {
        goto error;
    }

    if (flecs_expr_type_for_oper(script, node, &operand_type, &result_type)) {
        goto error;
    }

    if (!flecs_expr_oper_valid_for_type(result_type, node->operator)) {
        flecs_expr_visit_error(script, node, "invalid operator for type");
        goto error;
    }

    node->node.type = result_type;

    return 0;
error:
    return -1;
}

int flecs_script_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        break;
    case EcsExprUnary:
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_visit_type(script, (ecs_expr_binary_t*)node)) {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        break;
    case EcsExprVariable:
        break;
    case EcsExprFunction:
        break;
    case EcsExprMember:
        break;
    case EcsExprElement:
        break;
    case EcsExprCast:
        break;
    }

    return 0;
error:
    return -1;
}

#endif
