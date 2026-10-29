/**
 * @file addons/script/modules/command.h
 * @brief Script command addon internals.
 */

#ifndef FLECS_SCRIPT_SHELL_PRIVATE_H
#define FLECS_SCRIPT_SHELL_PRIVATE_H

#include "../script.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

extern ECS_COMPONENT_DECLARE(ecs_command_param_info_t);
extern ECS_COMPONENT_DECLARE(ecs_command_info_t);
extern ECS_COMPONENT_DECLARE(ecs_command_tree_node_t);
extern ECS_COMPONENT_DECLARE(ecs_command_component_value_t);
extern ECS_COMPONENT_DECLARE(ecs_command_entity_info_t);
extern ECS_COMPONENT_DECLARE(ecs_command_doc_info_t);
extern ECS_COMPONENT_DECLARE(ecs_command_query_row_t);
extern ECS_COMPONENT_DECLARE(ecs_command_grep_match_t);
extern ECS_COMPONENT_DECLARE(ecs_command_script_info_t);
extern ECS_COMPONENT_DECLARE(ecs_command_script_line_t);
extern ECS_COMPONENT_DECLARE(ecs_command_script_match_t);
extern ECS_COMPONENT_DECLARE(ecs_command_script_location_t);
extern ECS_COMPONENT_DECLARE(ecs_command_script_error_t);
#ifdef FLECS_SCRIPT_INPUT
extern ECS_COMPONENT_DECLARE(ecs_command_pointer_state_t);
extern ECS_COMPONENT_DECLARE(ecs_script_event_result_t);
#endif

#define flecs_command_arg(argv, i, T) (*(T*)argv[i].ptr)

typedef struct flecs_command_desc_t {
    const char *name;
    ecs_function_callback_t callback;
    ecs_entity_t return_type;
    const char *brief;
    ecs_script_parameter_t params[4];
    void *ctx;
} flecs_command_desc_t;

void flecs_command_register(
    ecs_world_t *world,
    const flecs_command_desc_t *commands,
    int32_t count);

void flecs_command_fail(
    const ecs_function_ctx_t *ctx);

#define flecs_command_err(ctx, ...)\
    (ecs_err(__VA_ARGS__), flecs_command_fail(ctx))

ecs_entity_t flecs_command_scope(
    const ecs_world_t *world);

void flecs_command_history_clear(
    EcsCommandSession *s);

ecs_entity_t flecs_command_lookup(
    const ecs_world_t *world,
    const char *name);

int flecs_command_parse_value(
    ecs_world_t *world,
    const char *expr,
    ecs_value_t *value,
    bool partial);

bool flecs_command_check_alive(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t entity);

ecs_entity_t flecs_command_value_type(
    const ecs_world_t *world,
    ecs_id_t id);

int flecs_command_parse_component(
    const ecs_function_ctx_t *ctx,
    ecs_entity_t e,
    ecs_id_t id,
    const char *expr,
    ecs_value_t *value);

typedef void (*flecs_command_visit_t)(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t depth,
    void *ctx);

void flecs_command_descendants(
    ecs_world_t *world,
    ecs_entity_t scope,
    int32_t depth,
    flecs_command_visit_t visit,
    void *ctx);

void flecs_command_children(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_vec_t *children);

void flecs_command_lines_append(
    ecs_vec_t *lines,
    const char *code,
    int32_t first,
    int32_t last);

int32_t flecs_command_line_count(
    const char *code);

char* flecs_command_diff(
    const char *from,
    const char *to);

char* flecs_command_patch(
    const char *code,
    const char *patch,
    ecs_vec_t *changed_lines);

void flecs_command_core_import(
    ecs_world_t *world);

void flecs_command_script_import(
    ecs_world_t *world);

#ifdef FLECS_SCRIPT_INPUT
void flecs_command_input_import(
    ecs_world_t *world);
#endif

#endif
