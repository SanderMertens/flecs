/**
 * @file addons/script/expr/stack.h
 * @brief Script expression stack.
 */

#ifndef FLECS_SCRIPT_EXPR_STACK_H
#define FLECS_SCRIPT_EXPR_STACK_H

#define FLECS_EXPR_STACK_MAX (256)
#define FLECS_EXPR_SMALL_DATA_SIZE (24)

#include "flecs.h"

typedef union ecs_expr_small_value_t {
    bool bool_;
    char char_;
    int64_t i64;
    uint64_t u64;
    double f64;
    char *string;
    ecs_entity_t entity;

    /* Avoid allocations for small trivial types */
    char small_data[FLECS_EXPR_SMALL_DATA_SIZE];
} ecs_expr_small_value_t;

typedef struct ecs_expr_value_t {
    ecs_value_t value;
    const ecs_type_info_t *type_info;
    bool owned; /* Is value owned by the runtime */
} ecs_expr_value_t;

typedef struct ecs_expr_stack_frame_t {
    ecs_stack_cursor_t *cur;
    int32_t sp;
} ecs_expr_stack_frame_t;

typedef struct ecs_expr_stack_t {
    ecs_expr_value_t values[FLECS_EXPR_STACK_MAX];
    ecs_expr_stack_frame_t frames[FLECS_EXPR_STACK_MAX];
    ecs_stack_t stack;
    int32_t frame;
} ecs_expr_stack_t;

void flecs_expr_stack_init(
    ecs_expr_stack_t *stack);

void flecs_expr_stack_fini(
    ecs_expr_stack_t *stack);

ecs_expr_value_t* flecs_expr_stack_result(
    ecs_expr_stack_t *stack,
    ecs_expr_node_t *node);

void flecs_expr_stack_push(
    ecs_expr_stack_t *stack);

void flecs_expr_stack_pop(
    ecs_expr_stack_t *stack);

#endif
