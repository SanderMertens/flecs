/**
 * @file addons/script/expr/expr.h
 * @brief Script expression support.
 */

#ifndef FLECS_EXPR_SCRIPT_H
#define FLECS_EXPR_SCRIPT_H

#include "stack.h"
#include "ast.h"
#include "visit.h"

int flecs_value_copy_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_expr_value_t *src);

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_expr_node_t *node,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_token_kind_t operator);

int flecs_value_unary(
    const ecs_script_t *script,
    const ecs_value_t *expr,
    ecs_value_t *out,
    ecs_token_kind_t operator);

const char* flecs_script_parse_expr(
    ecs_parser_t *parser,
    const char *pos,
    ecs_token_kind_t left_oper,
    ecs_expr_node_t **out);

const char* flecs_script_parse_initializer(
    ecs_parser_t *parser,
    const char *pos,
    char until,
    ecs_expr_node_t **node_out);

const char* flecs_expr_format_parse(
    ecs_parser_t *parser,
    const char *pos,
    ecs_expr_format_t *format,
    const ecs_expr_eval_desc_t *desc);

int flecs_expr_format_value(
    const ecs_script_t *script,
    const ecs_expr_node_t *node,
    const ecs_value_t *value,
    const ecs_expr_format_t *format,
    int32_t width,
    int32_t precision,
    ecs_strbuf_t *buf);

void flecs_expr_to_str_buf(
    const ecs_script_t *world,
    const ecs_expr_node_t *expr,
    ecs_strbuf_t *buf,
    bool colors);

bool flecs_string_is_interpolated(
    const char *str);

char* flecs_string_escape(
    char *str);

bool flecs_value_is_0(
    const ecs_value_t *value);

bool flecs_expr_is_type_integer(
    ecs_entity_t type);

bool flecs_expr_is_type_number(
    ecs_entity_t type);

bool flecs_expr_is_type_signed_integer(
    ecs_entity_t type);

bool flecs_expr_is_type_unsigned_integer(
    ecs_entity_t type);

bool flecs_expr_is_type_float(
    ecs_entity_t type);

bool flecs_expr_is_type_string(
    ecs_entity_t type);

ecs_size_t flecs_expr_storage_size(
    ecs_entity_t type);

typedef enum flecs_expr_num_class_t {
    EcsExprNumNone,
    EcsExprNumSigned,
    EcsExprNumUnsigned,
    EcsExprNumFloat,
    EcsExprNumChar
} flecs_expr_num_class_t;

flecs_expr_num_class_t flecs_expr_num_class(
    ecs_entity_t type,
    ecs_size_t *size);

flecs_expr_cast_t flecs_expr_cast_handler(
    ecs_entity_t from,
    ecs_entity_t to);

static inline FLECS_ALWAYS_INLINE
int flecs_expr_binary_eval(
    const ecs_script_t *script,
    const ecs_expr_binary_t *node,
    const ecs_value_t *left,
    const ecs_value_t *right,
    void *out,
    ecs_token_kind_t operator)
{
    if (!node->vector_count) {
        ecs_value_t out_value = { .type = node->node.type, .ptr = out };
        return flecs_value_binary(script, &node->node, left, right,
            &out_value, operator);
    }

    ecs_entity_t vector_type = node->vector_type;
    ecs_value_t left_value = { .ptr = left->ptr, .type = vector_type };
    ecs_value_t right_value = { .ptr = right->ptr, .type = vector_type };
    ecs_value_t out_value = { .ptr = out, .type = vector_type };
    ecs_size_t size = flecs_expr_storage_size(vector_type);
    ecs_size_t right_offset = 0;
    if (left->type == right->type) {
        right_offset = size;
    }

    int32_t i;
    for (i = 0; i < node->vector_count; i ++) {
        if (flecs_value_binary(script, &node->node, &left_value,
            &right_value, &out_value, operator))
        {
            return -1;
        }

        left_value.ptr = ECS_OFFSET(left_value.ptr, size);
        right_value.ptr = ECS_OFFSET(right_value.ptr, right_offset);
        out_value.ptr = ECS_OFFSET(out_value.ptr, size);
    }

    return 0;
}

int flecs_expr_identifier_set(
    const ecs_script_t *script,
    const ecs_expr_identifier_t *node,
    ecs_entity_t entity,
    const ecs_value_t *out);

const ecs_script_var_t* flecs_expr_var_check(
    const ecs_script_t *script,
    const ecs_expr_variable_t *node,
    const ecs_script_var_t *var);

int flecs_expr_element_get(
    const ecs_script_t *script,
    const ecs_expr_element_t *node,
    ecs_type_kind_t kind,
    const ecs_value_t *expr,
    const ecs_value_t *index,
    void **out);

const void* flecs_expr_component_get(
    const ecs_script_t *script,
    struct ecs_script_eval_visitor_t *v,
    const ecs_expr_element_t *node,
    ecs_entity_t entity,
    ecs_entity_t component);

bool flecs_expr_has_eval(
    const ecs_script_t *script,
    struct ecs_script_eval_visitor_t *v,
    const ecs_expr_has_t *node,
    ecs_entity_t entity);

void* flecs_expr_template_ref_value(
    const ecs_script_t *script,
    const ecs_expr_function_t *node,
    const ecs_script_template_ref_t *ref);

int flecs_expr_range_eval(
    ecs_world_t *world,
    ecs_entity_t type,
    int32_t from,
    int32_t to,
    void *out);

ecs_entity_t flecs_expr_script_new(
    ecs_world_t *world,
    ecs_script_t *script);

static inline
int flecs_expr_initializer_check(
    const ecs_script_t *script,
    const void *node,
    ecs_entity_t type,
    uintptr_t offset,
    ecs_size_t size,
    uintptr_t dst_size)
{
    if (offset > dst_size || flecs_uto(uintptr_t, size) > (dst_size - offset)) {
        flecs_expr_visit_error(script, node,
            "initializer of type '%s' writes past end of target value",
            flecs_errstr(ecs_get_path(script->world, type)));
        return -1;
    }

    return 0;
}

int flecs_expr_swizzle_set(
    ecs_meta_cursor_t *cur,
    const ecs_expr_swizzle_t *swizzle,
    const void *src);

ecs_expr_swizzle_t* flecs_expr_expand_swizzle_get(
    ecs_expr_node_t *node);

const ecs_expr_variable_t* flecs_expr_template_ref_var(
    const ecs_expr_node_t *left);

#endif
