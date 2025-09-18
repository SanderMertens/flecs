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

int flecs_value_move_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    ecs_value_t *src);

int flecs_value_binary(
    const ecs_script_t *script,
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
    ecs_expr_initializer_t **node_out);

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

#endif
