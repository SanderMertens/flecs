/**
 * @file addons/script/expr/ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"

#define flecs_expr_ast_new(parser, T, kind)\
    (T*)flecs_expr_ast_new_(parser, ECS_SIZEOF(T), kind)

static void* flecs_expr_ast_new_(
    ecs_parser_t *parser,
    ecs_size_t size, 
    ecs_expr_node_kind_t kind)
{
    ecs_assert(parser->script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &parser->script->allocator;
    ecs_expr_node_t *result = flecs_calloc_w_dbg_info(a, size,
        "ecs_expr_node_t");
    result->kind = kind;
    result->pos = parser->pos;
    return result;
}

ecs_expr_value_node_t* flecs_expr_value_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_entity_t type)
{
    ecs_expr_value_node_t *result = flecs_calloc_t(
        &((ecs_script_impl_t*)script)->allocator, ecs_expr_value_node_t);
    result->ptr = &result->storage.u64;
    result->node.kind = EcsExprValue;
    result->node.pos = node ? node->pos : NULL;
    result->node.end = node ? node->end : NULL;
    result->node.type = type;
    result->node.type_info = ecs_get_type_info(script->world, type);
    return result;
}

ecs_expr_variable_t* flecs_expr_variable_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const char *name)
{
    ecs_expr_variable_t *result = flecs_calloc_t(
        &((ecs_script_impl_t*)script)->allocator, ecs_expr_variable_t);
    result->name = name;
    result->sp = -1;
    result->node.kind = EcsExprVariable;
    result->node.pos = node ? node->pos : NULL;
    result->node.end = node ? node->end : NULL;
    return result;
}

ecs_expr_member_t* flecs_expr_member_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const char *name)
{
    ecs_expr_member_t *result = flecs_calloc_t(
        &flecs_script_impl(script)->allocator, ecs_expr_member_t);
    result->node.kind = EcsExprMember;
    result->node.pos = node->pos;
    result->node.end = node->end;
    result->left = node;
    result->member_name =name;
    return result;
}

ecs_expr_swizzle_t* flecs_expr_swizzle_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_expr_node_t *left,
    const char *name)
{
    ecs_expr_swizzle_t *result = flecs_calloc_t(
        &flecs_script_impl(script)->allocator, ecs_expr_swizzle_t);
    result->node.kind = EcsExprSwizzle;
    result->node.pos = node->pos;
    result->node.end = node->end;
    result->left = left;
    result->name = name;
    return result;
}

ecs_expr_value_node_t* flecs_expr_bool(
    ecs_parser_t *parser,
    bool value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.bool_ = value;
    result->ptr = &result->storage.bool_;
    result->node.type = ecs_id(ecs_bool_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_char(
    ecs_parser_t *parser,
    const char *value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    if(value[0] == '\\' && value[1] == '\'') {
        result->storage.char_ = '\'';
    } else {
        char ch = 0;
        const char *ptr = flecs_chrparse(value, &ch);
        if(!ptr) {
            flecs_free_t(
                &parser->script->allocator, ecs_expr_value_node_t, result);
            return NULL;
        }
        result->storage.char_ = ch;
    }
    result->ptr = &result->storage.char_;
    result->node.type = ecs_id(ecs_char_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_int(
    ecs_parser_t *parser,
    int64_t value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.i64 = value;
    result->ptr = &result->storage.i64;
    result->node.type = ecs_id(ecs_i64_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_uint(
    ecs_parser_t *parser,
    uint64_t value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.u64 = value;
    result->ptr = &result->storage.u64;
    if (value > INT64_MAX) {
        result->node.type = ecs_id(ecs_u64_t);
    } else {
        result->node.type = ecs_id(ecs_i64_t);
    }
    return result;
}

ecs_expr_value_node_t* flecs_expr_float(
    ecs_parser_t *parser,
    double value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.f64 = value;
    result->ptr = &result->storage.f64;
    result->node.type = ecs_id(ecs_f64_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_string(
    ecs_parser_t *parser,
    const char *value)
{
    char *str = ECS_CONST_CAST(char*, value);
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.string = str;
    result->ptr = &result->storage.string;
    result->node.type = ecs_id(ecs_string_t);

    if (!flecs_string_escape(str)) {
        flecs_free_t(&parser->script->allocator, ecs_expr_value_node_t, result);
        return NULL;
    }

    return result;
}

ecs_expr_interpolated_string_t* flecs_expr_interpolated_string(
    ecs_parser_t *parser,
    const char *value)
{
    ecs_expr_interpolated_string_t *result = flecs_expr_ast_new(
        parser, ecs_expr_interpolated_string_t, EcsExprInterpolatedString);
    result->value = ECS_CONST_CAST(char*, value);
    result->buffer = flecs_strdup(&parser->script->allocator, value);
    result->buffer_size = ecs_os_strlen(result->buffer) + 1;
    result->node.type = ecs_id(ecs_string_t);
    ecs_vec_init_t(&parser->script->allocator, &result->fragments, char*, 0);
    ecs_vec_init_t(&parser->script->allocator, &result->expressions, 
        ecs_expr_node_t*, 0);
    ecs_vec_init_t(&parser->script->allocator, &result->formats,
        ecs_expr_format_t, 0);

    return result;
}

ecs_expr_initializer_t* flecs_expr_initializer(
    ecs_parser_t *parser)
{
    ecs_expr_initializer_t *result = flecs_expr_ast_new(
        parser, ecs_expr_initializer_t, EcsExprInitializer);
    ecs_vec_init_t(&parser->script->allocator, &result->elements, 
        ecs_expr_initializer_element_t, 0);
    return result;
}

ecs_expr_identifier_t* flecs_expr_identifier(
    ecs_parser_t *parser,
    const char *value)
{
    ecs_expr_identifier_t *result = flecs_expr_ast_new(
        parser, ecs_expr_identifier_t, EcsExprIdentifier);
    result->value = value;
    result->symbol = -1;
    return result;
}

ecs_expr_variable_t* flecs_expr_variable(
    ecs_parser_t *parser,
    const char *value)
{
    ecs_expr_variable_t *result = flecs_expr_ast_new(
        parser, ecs_expr_variable_t, EcsExprVariable);
    result->name = value;
    result->sp = -1;
    return result;
}

ecs_expr_unary_t* flecs_expr_unary(
    ecs_parser_t *parser)
{
    ecs_expr_unary_t *result = flecs_expr_ast_new(
        parser, ecs_expr_unary_t, EcsExprUnary);
    return result;
}

ecs_expr_binary_t* flecs_expr_binary(
    ecs_parser_t *parser)
{
    ecs_expr_binary_t *result = flecs_expr_ast_new(
        parser, ecs_expr_binary_t, EcsExprBinary);
    return result;
}

ecs_expr_member_t* flecs_expr_member(
    ecs_parser_t *parser)
{
    ecs_expr_member_t *result = flecs_expr_ast_new(
        parser, ecs_expr_member_t, EcsExprMember);
    return result;
}

ecs_expr_function_t* flecs_expr_function(
    ecs_parser_t *parser)
{
    ecs_expr_function_t *result = flecs_expr_ast_new(
        parser, ecs_expr_function_t, EcsExprFunction);
    return result;
}

ecs_expr_element_t* flecs_expr_element(
    ecs_parser_t *parser)
{
    ecs_expr_element_t *result = flecs_expr_ast_new(
        parser, ecs_expr_element_t, EcsExprElement);
    return result;
}

ecs_expr_has_t* flecs_expr_has(
    ecs_parser_t *parser)
{
    ecs_expr_has_t *result = flecs_expr_ast_new(
        parser, ecs_expr_has_t, EcsExprHas);
    return result;
}

ecs_expr_match_t* flecs_expr_match(
    ecs_parser_t *parser)
{
    ecs_expr_match_t *result = flecs_expr_ast_new(
        parser, ecs_expr_match_t, EcsExprMatch);
    return result;
}

ecs_expr_range_t* flecs_expr_range(
    ecs_parser_t *parser)
{
    ecs_expr_range_t *result = flecs_expr_ast_new(
        parser, ecs_expr_range_t, EcsExprRange);
    return result;
}

ecs_expr_new_t* flecs_expr_new(
    ecs_parser_t *parser)
{
    ecs_expr_new_t *result = flecs_expr_ast_new(
        parser, ecs_expr_new_t, EcsExprNew);
    return result;
}

ecs_expr_script_t* flecs_expr_script(
    ecs_parser_t *parser)
{
    ecs_expr_script_t *result = flecs_expr_ast_new(
        parser, ecs_expr_script_t, EcsExprScript);
    return result;
}

bool flecs_expr_explicit_cast_allowed(
    ecs_world_t *world,
    ecs_entity_t from,
    ecs_entity_t to)
{
    if (from == to) {
        return true;
    }

    /* Any type can be cast to and from a value */
    if (to == ecs_id(ecs_value_t) || from == ecs_id(ecs_value_t)) {
        return true;
    }

    const EcsType *from_type = ecs_get(world, from, EcsType);
    const EcsType *to_type = ecs_get(world, to, EcsType);
    ecs_assert(from_type != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(to_type != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Treat opaque types as the types that they're pretending to be */
    if (from_type->kind == EcsOpaqueType) {
        const EcsOpaque *o = ecs_get(world, from, EcsOpaque);
        ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
        from_type = ecs_get(world, o->as_type, EcsType);
        ecs_assert(from_type != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    if (to_type->kind == EcsOpaqueType) {
        const EcsOpaque *o = ecs_get(world, to, EcsOpaque);
        ecs_assert(o != NULL, ECS_INTERNAL_ERROR, NULL);
        to_type = ecs_get(world, o->as_type, EcsType);
        ecs_assert(to_type != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (from_type->kind != EcsPrimitiveType || 
        to_type->kind != EcsPrimitiveType) 
    {
        if (from_type->kind == EcsEnumType || 
            from_type->kind == EcsBitmaskType)
        {
            if (flecs_expr_is_type_integer(to)) {
                /* Can cast enums/bitmasks to integers */
                return true;
            }
        }

        if (flecs_expr_is_type_integer(from)) {
            if (to_type->kind == EcsEnumType || 
                to_type->kind == EcsBitmaskType)
            {
                /* Can cast integers to enums/bitmasks */
                return true;
            }
        }

        if (from_type->kind == EcsStructType &&
            to_type->kind == EcsStructType)
        {
            return flecs_struct_is_derived_from(world, from, to);
        }

        /* Cannot cast complex types that are not the same */
        return false;
    }

    /* Anything can be cast to a number */
    if (flecs_expr_is_type_number(to)) {
        return true;
    }

    /* Anything can be cast to a string */
    if (to == ecs_id(ecs_string_t)) {
        return true;
    }

    return true;
}

ecs_expr_cast_t* flecs_expr_cast(
    ecs_script_t *script,
    ecs_expr_node_t *expr,
    ecs_entity_t type)
{
    if (!flecs_expr_explicit_cast_allowed(script->world, expr->type, type)) {
        char *from = ecs_id_str(script->world, expr->type);
        char *to = ecs_id_str(script->world, type);
        flecs_expr_visit_error(script, expr, "invalid cast from %s to %s", 
            from, to);
        ecs_os_free(from);
        ecs_os_free(to);
        return NULL;
    }

    ecs_allocator_t *a = &((ecs_script_impl_t*)script)->allocator;
    ecs_expr_cast_t *result = flecs_calloc_t(a, ecs_expr_cast_t);
    result->node.kind = EcsExprCast;
    if (flecs_expr_is_type_number(expr->type) && 
        flecs_expr_is_type_number(type)) 
    {
        result->node.kind = EcsExprCastNumber;
    }

    result->node.pos = expr->pos;
    result->node.end = expr->end;
    result->node.type = type;
    result->node.type_info = ecs_get_type_info(script->world, type);
    ecs_assert(result->node.type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    result->expr = expr;
    return result;
}

int flecs_expr_visit_children(
    ecs_expr_node_t *node,
    flecs_expr_visit_action_t action,
    void *ctx)
{
    switch(node->kind) {
    case EcsExprValue:
    case EcsExprGlobalVariable:
        break;
    case EcsExprVariable:
        break;
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *n =
            (ecs_expr_interpolated_string_t*)node;
        ecs_expr_node_t **expressions = ecs_vec_first(&n->expressions);
        int32_t i, count = ecs_vec_count(&n->expressions);
        for (i = 0; i < count; i ++) {
            if (action(&expressions[i], ctx)) {
                return -1;
            }
        }
        ecs_expr_format_t *formats = ecs_vec_first(&n->formats);
        count = ecs_vec_count(&n->formats);
        for (i = 0; i < count; i ++) {
            if (action(&formats[i].width, ctx) ||
                action(&formats[i].precision, ctx))
            {
                return -1;
            }
        }
        break;
    }
    case EcsExprInitializer:
    case EcsExprEmptyInitializer: {
        ecs_expr_initializer_t *n = (ecs_expr_initializer_t*)node;
        ecs_expr_initializer_element_t *elems = ecs_vec_first(&n->elements);
        int32_t i, count = ecs_vec_count(&n->elements);
        for (i = 0; i < count; i ++) {
            if (action(&elems[i].key, ctx) ||
                action(&elems[i].value, ctx))
            {
                return -1;
            }
        }
        break;
    }
    case EcsExprUnary:
        return action(&((ecs_expr_unary_t*)node)->expr, ctx);
    case EcsExprBinary: {
        ecs_expr_binary_t *n = (ecs_expr_binary_t*)node;
        if (action(&n->left, ctx) ||
            action(&n->right, ctx))
        {
            return -1;
        }
        break;
    }
    case EcsExprIdentifier:
        return action(&((ecs_expr_identifier_t*)node)->expr, ctx);
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *n = (ecs_expr_function_t*)node;
        ecs_expr_node_t *args = (ecs_expr_node_t*)n->args;
        if (action(&n->left, ctx) || action(&args, ctx))
        {
            return -1;
        }
        n->args = (ecs_expr_initializer_t*)args;
        break;
    }
    case EcsExprMember:
        return action(&((ecs_expr_member_t*)node)->left, ctx);
    case EcsExprSwizzle:
        return action(&((ecs_expr_swizzle_t*)node)->left, ctx);
    case EcsExprComponent:
    case EcsExprElement: {
        ecs_expr_element_t *n = (ecs_expr_element_t*)node;
        if (action(&n->left, ctx) ||
            action(&n->index, ctx))
        {
            return -1;
        }
        break;
    }
    case EcsExprHas: {
        ecs_expr_has_t *n = (ecs_expr_has_t*)node;
        if (action(&n->left, ctx) ||
            action(&n->first, ctx) ||
            action(&n->second, ctx))
        {
            return -1;
        }
        break;
    }
    case EcsExprCast:
    case EcsExprCastNumber:
        return action(&((ecs_expr_cast_t*)node)->expr, ctx);
    case EcsExprMatch: {
        ecs_expr_match_t *n = (ecs_expr_match_t*)node;
        if (action(&n->expr, ctx)) {
            return -1;
        }
        ecs_expr_match_element_t *elems = ecs_vec_first(&n->elements);
        int32_t i, count = ecs_vec_count(&n->elements);
        for (i = 0; i < count; i ++) {
            if (action(&elems[i].compare, ctx) ||
                action(&elems[i].expr, ctx))
            {
                return -1;
            }
        }
        if (action(&n->any.compare, ctx) ||
            action(&n->any.expr, ctx))
        {
            return -1;
        }
        break;
    }
    case EcsExprRange: {
        ecs_expr_range_t *n = (ecs_expr_range_t*)node;
        if (action(&n->from, ctx) ||
            action(&n->to, ctx))
        {
            return -1;
        }
        break;
    }
    case EcsExprNew:
    case EcsExprScript:
        break;
    }
    return 0;
}

#endif
