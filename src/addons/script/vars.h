/**
 * @file addons/script/vars.h
 * @brief Flecs script variables.
 */

#ifndef FLECS_SCRIPT_VARS_H
#define FLECS_SCRIPT_VARS_H

typedef struct ecs_script_var_t {
    const char *name;
    ecs_value_t value;
} ecs_script_var_t;

typedef struct ecs_script_vars_t {
    struct ecs_script_vars_t *parent;
    ecs_hashmap_t var_index;
    ecs_vec_t vars;

    ecs_stack_t *stack;
    ecs_stack_cursor_t *cursor;
    ecs_allocator_t *allocator;
} ecs_script_vars_t;

ecs_script_vars_t* ecs_script_vars_push(
    ecs_script_vars_t *parent,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator);

ecs_script_vars_t* ecs_script_vars_pop(
    ecs_script_vars_t *vars);

ecs_script_var_t* ecs_script_vars_declare(
    ecs_script_vars_t *vars,
    const char *name);

ecs_script_var_t* ecs_script_vars_lookup(
    const ecs_script_vars_t *vars,
    const char *name);

#endif
