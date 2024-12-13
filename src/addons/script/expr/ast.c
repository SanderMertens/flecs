/**
 * @file addons/script/expr_ast.c
 * @brief Script expression AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

#define flecs_expr_ast_new(parser, T, kind)\
    (T*)flecs_expr_ast_new_(parser, ECS_SIZEOF(T), kind)

static
void* flecs_expr_ast_new_(
    ecs_script_parser_t *parser,
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
    result->node.kind = EcsExprVariable;
    result->node.pos = node ? node->pos : NULL;
    return result;
}

ecs_expr_value_node_t* flecs_expr_bool(
    ecs_script_parser_t *parser,
    bool value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.bool_ = value;
    result->ptr = &result->storage.bool_;
    result->node.type = ecs_id(ecs_bool_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_int(
    ecs_script_parser_t *parser,
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
    ecs_script_parser_t *parser,
    uint64_t value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.u64 = value;
    result->ptr = &result->storage.u64;
    result->node.type = ecs_id(ecs_i64_t);
    return result;
}

ecs_expr_value_node_t* flecs_expr_float(
    ecs_script_parser_t *parser,
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
    ecs_script_parser_t *parser,
    const char *value)
{
    char *str = ECS_CONST_CAST(char*, value);
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.string = str;
    result->ptr = &result->storage.string;
    result->node.type = ecs_id(ecs_string_t);

    if (!flecs_string_escape(str)) {
        return NULL;
    }

    return result;
}

ecs_expr_interpolated_string_t* flecs_expr_interpolated_string(
    ecs_script_parser_t *parser,
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

    return result;
}

ecs_expr_value_node_t* flecs_expr_entity(
    ecs_script_parser_t *parser,
    ecs_entity_t value)
{
    ecs_expr_value_node_t *result = flecs_expr_ast_new(
        parser, ecs_expr_value_node_t, EcsExprValue);
    result->storage.entity = value;
    result->ptr = &result->storage.entity;
    result->node.type = ecs_id(ecs_entity_t);
    return result;
}

ecs_expr_initializer_t* flecs_expr_initializer(
    ecs_script_parser_t *parser)
{
    ecs_expr_initializer_t *result = flecs_expr_ast_new(
        parser, ecs_expr_initializer_t, EcsExprInitializer);
    ecs_vec_init_t(&parser->script->allocator, &result->elements, 
        ecs_expr_initializer_element_t, 0);
    return result;
}

ecs_expr_identifier_t* flecs_expr_identifier(
    ecs_script_parser_t *parser,
    const char *value)
{
    ecs_expr_identifier_t *result = flecs_expr_ast_new(
        parser, ecs_expr_identifier_t, EcsExprIdentifier);
    result->value = value;
    return result;
}

ecs_expr_variable_t* flecs_expr_variable(
    ecs_script_parser_t *parser,
    const char *value)
{
    ecs_expr_variable_t *result = flecs_expr_ast_new(
        parser, ecs_expr_variable_t, EcsExprVariable);
    result->name = value;
    return result;
}

ecs_expr_unary_t* flecs_expr_unary(
    ecs_script_parser_t *parser)
{
    ecs_expr_unary_t *result = flecs_expr_ast_new(
        parser, ecs_expr_unary_t, EcsExprUnary);
    return result;
}

ecs_expr_binary_t* flecs_expr_binary(
    ecs_script_parser_t *parser)
{
    ecs_expr_binary_t *result = flecs_expr_ast_new(
        parser, ecs_expr_binary_t, EcsExprBinary);
    return result;
}

ecs_expr_member_t* flecs_expr_member(
    ecs_script_parser_t *parser)
{
    ecs_expr_member_t *result = flecs_expr_ast_new(
        parser, ecs_expr_member_t, EcsExprMember);
    return result;
}

ecs_expr_function_t* flecs_expr_function(
    ecs_script_parser_t *parser)
{
    ecs_expr_function_t *result = flecs_expr_ast_new(
        parser, ecs_expr_function_t, EcsExprFunction);
    return result;
}

ecs_expr_element_t* flecs_expr_element(
    ecs_script_parser_t *parser)
{
    ecs_expr_element_t *result = flecs_expr_ast_new(
        parser, ecs_expr_element_t, EcsExprElement);
    return result;
}

ecs_expr_cast_t* flecs_expr_cast(
    ecs_script_t *script,
    ecs_expr_node_t *expr,
    ecs_entity_t type)
{
    ecs_allocator_t *a = &((ecs_script_impl_t*)script)->allocator;
    ecs_expr_cast_t *result = flecs_calloc_t(a, ecs_expr_cast_t);
    result->node.kind = EcsExprCast;
    result->node.pos = expr->pos;
    result->node.type = type;
    result->node.type_info = ecs_get_type_info(script->world, type);
    ecs_assert(result->node.type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    result->expr = expr;
    return result;
}

#endif
