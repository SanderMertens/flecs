/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static
int flecs_expr_visit_type_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc);

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
    case EcsTokMod:
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
    case EcsTokEnd:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }
error:
    return false;
}

static
bool flecs_expr_is_type_integer(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t))   return false;
    else if (type == ecs_id(ecs_char_t))   return false;
    else if (type == ecs_id(ecs_u8_t))     return false;
    else if (type == ecs_id(ecs_u64_t))    return true;
    else if (type == ecs_id(ecs_i64_t))    return true;
    else if (type == ecs_id(ecs_f64_t))    return false;
    else if (type == ecs_id(ecs_string_t)) return false;
    else if (type == ecs_id(ecs_entity_t)) return false;
    else return false;
}

static
bool flecs_expr_is_type_number(
    ecs_entity_t type)
{
    if      (type == ecs_id(ecs_bool_t))   return false;
    else if (type == ecs_id(ecs_char_t))   return false;
    else if (type == ecs_id(ecs_u8_t))     return false;
    else if (type == ecs_id(ecs_u64_t))    return true;
    else if (type == ecs_id(ecs_i64_t))    return true;
    else if (type == ecs_id(ecs_f64_t))    return true;
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
    ecs_world_t *world,
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
    case EcsTokMod:
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
    case EcsTokEnd:
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
    case EcsTokMod:
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
    case EcsTokEnd:
    default:
        ecs_throw(ECS_INTERNAL_ERROR, "invalid operator");
    }

    const EcsPrimitive *ltype_ptr = ecs_get(world, left->type, EcsPrimitive);
    const EcsPrimitive *rtype_ptr = ecs_get(world, right->type, EcsPrimitive);
    if (!ltype_ptr || !rtype_ptr) {
        /* Only primitives and bitmask constants are allowed */
        if (left->type == right->type) {
            if (ecs_get(world, left->type, EcsBitmask) != NULL) {
                *operand_type = left->type;
                goto done;
            }
        }

        char *lname = ecs_get_path(world, left->type);
        char *rname = ecs_get_path(world, right->type);
        flecs_expr_visit_error(script, node, 
            "invalid non-primitive type in binary expression (%s, %s)", 
            lname, rname);
        ecs_os_free(lname);
        ecs_os_free(rname);
        goto error;
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
        goto error;
    }

    if (!flecs_expr_is_type_number(ltype) || !flecs_expr_is_type_number(rtype)) {
        flecs_expr_visit_error(script, node,
            "incompatible types in binary expression");
        goto error;
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
    bool is_opaque = ecs_get(script->world, type, EcsOpaque) != NULL;
    node->is_dynamic = is_opaque;

    if (ecs_meta_push(cur)) {
        goto error;
    }

    if (ecs_meta_is_collection(cur) != node->is_collection) {
        char *type_str = ecs_get_path(script->world, type);
        if (node->is_collection) {
            flecs_expr_visit_error(script, node, 
                "invalid collection literal for non-collection type '%s'"
                    " (expected '[]')", type_str);
        } else {
            flecs_expr_visit_error(script, node, 
                "invalid object literal for collection type '%s' (expected {})",
                    type_str);
        }

        ecs_os_free(type_str);
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
            elem->value = (ecs_expr_node_t*)flecs_expr_cast(
                script, elem->value, elem_type);
        }

        if (!is_opaque) {
            elem->offset = (uintptr_t)ecs_meta_get_ptr(cur);
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

    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    if (flecs_expr_visit_type_priv(script, node->right, cur, desc)) {
        goto error;
    }

    if (flecs_expr_type_for_oper(script, node, &operand_type, &result_type)) {
        goto error;
    }

    if (!flecs_expr_oper_valid_for_type(
        script->world, result_type, node->operator)) 
    {
        char *type_str = ecs_get_path(script->world, result_type);
        flecs_expr_visit_error(script, node, "invalid operator %s for type '%s'",
            flecs_script_token_str(node->operator), type_str);
        ecs_os_free(type_str);
        goto error;
    }

    if (ecs_get(script->world, result_type, EcsBitmask) != NULL) {
        operand_type = ecs_id(ecs_u64_t);
    }

    if (operand_type != node->left->type) {
        node->left = (ecs_expr_node_t*)flecs_expr_cast(
            script, node->left, operand_type);
    }

    if (operand_type != node->right->type) {
        node->right = (ecs_expr_node_t*)flecs_expr_cast(
            script, node->right, operand_type);
    }

    node->node.type = result_type;

    return 0;
error:
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
    if (cur->valid) {
        node->node.type = ecs_meta_get_type(cur);
    } else {
        node->node.type = ecs_id(ecs_entity_t);
        *cur = ecs_meta_cursor(script->world, ecs_id(ecs_entity_t), NULL);
    }

    ecs_entity_t type = node->node.type;

    ecs_expr_value_node_t *result = flecs_expr_value_from(
        script, (ecs_expr_node_t*)node, type);

    if (type == ecs_id(ecs_entity_t)) {
        result->storage.entity = desc->lookup_action(
            script->world, node->value, desc->lookup_ctx);
        if (!result->storage.entity) {
            flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
            flecs_expr_visit_error(script, node, 
                "unresolved identifier '%s'", node->value);
            goto error;
        }
        result->ptr = &result->storage.entity;
    } else {
        ecs_meta_cursor_t tmp_cur = ecs_meta_cursor(
            script->world, type, &result->storage.u64);
        if (ecs_meta_set_string(&tmp_cur, node->value)) {
            flecs_expr_visit_free(script, (ecs_expr_node_t*)result);
            goto error;
        }
        result->ptr = &result->storage.u64;
    }

    node->expr = (ecs_expr_node_t*)result;

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
    ecs_script_var_t *var = ecs_script_vars_lookup(
        desc->vars, node->name);
    if (!var) {
        flecs_expr_visit_error(script, node, "unresolved variable '%s'",
            node->name);
        goto error;
    }

    node->node.type = var->value.type;

    *cur = ecs_meta_cursor(script->world, var->value.type, NULL);

    return 0;
error:
    return -1;
}

static
int flecs_expr_arguments_visit_type(
    ecs_script_t *script,
    ecs_expr_initializer_t *node,
    const ecs_expr_eval_desc_t *desc,
    const ecs_vec_t *param_vec)
{
    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);

    if (count != ecs_vec_count(param_vec)) {
        flecs_expr_visit_error(script, node, "expected %d arguments, got %d",
            ecs_vec_count(param_vec), count);
        goto error;
    }

    ecs_script_parameter_t *params = ecs_vec_first(param_vec);

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        ecs_meta_cursor_t cur = ecs_meta_cursor(
            script->world, params[i].type, NULL);

        if (flecs_expr_visit_type_priv(script, elem->value, &cur, desc)){
            goto error;
        }

        if (elem->value->type != params[i].type) {
            elem->value = (ecs_expr_node_t*)flecs_expr_cast(
                script, elem->value, params[i].type);
        }
    }

    return 0;
error:
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

        const EcsScriptMethod *func_data = ecs_get(
            world, func, EcsScriptMethod);
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
        node->calldata.callback = func_data->callback;
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

        const EcsScriptFunction *func_data = ecs_get(
            world, func, EcsScriptFunction);
        if (!func_data) {
            char *path = ecs_get_path(world, func);
            flecs_expr_visit_error(script, node, 
                "entity '%s' is not a valid method", path);
            ecs_os_free(path);
            goto error;
        }

        node->node.type = func_data->return_type;
        node->calldata.function = func;
        node->calldata.callback = func_data->callback;
        node->calldata.ctx = func_data->ctx;
        params = &func_data->params;
    }

    if (flecs_expr_arguments_visit_type(script, node->args, desc, params)) {
        goto error;
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
#ifdef FLECS_DEBUG
    const EcsMember *m = ecs_get(world, ecs_meta_get_member_id(cur), EcsMember);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);
#endif
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
    if (flecs_expr_visit_type_priv(script, node->left, cur, desc)) {
        goto error;
    }

    ecs_meta_cursor_t index_cur = {0};
    if (flecs_expr_visit_type_priv(
        script, node->index, &index_cur, desc)) 
    {
        goto error;
    }

    ecs_world_t *world = script->world;
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
        if (node->index->kind == EcsExprIdentifier) {
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
        } else {
            flecs_expr_visit_error(script, node, 
                "invalid component expression");
            goto error;
        }
    } else {
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
    }

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
int flecs_expr_visit_type_priv(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_meta_cursor_t *cur,
    const ecs_expr_eval_desc_t *desc)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        /* Value types are assigned by the AST */
        break;
    case EcsExprEmptyInitializer:
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
    case EcsExprCast:
        break;
    case EcsExprMethod:
    case EcsExprComponent:
        /* Expressions are derived by type visitor */
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(node->type != 0, ECS_INTERNAL_ERROR, NULL);

    if (node->type) {
        node->type_info = ecs_get_type_info(script->world, node->type);
        ecs_assert(node->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return 0;
error:
    return -1;
}

int flecs_expr_visit_type(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const ecs_expr_eval_desc_t *desc)
{
    if (node->kind == EcsExprEmptyInitializer) {
        node->type = desc->type;
        if (node->type) {
            node->type_info = ecs_get_type_info(script->world, node->type);
        }
        return 0;
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
