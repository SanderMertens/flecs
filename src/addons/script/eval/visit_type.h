/**
 * @file addons/script/eval/visit_type.h
 * @brief Script type visitor.
 */

#ifndef FLECS_SCRIPT_VISIT_TYPE_H
#define FLECS_SCRIPT_VISIT_TYPE_H

typedef struct ecs_script_type_table_t {
    int32_t parent;
    ecs_script_entity_t *owner;
} ecs_script_type_table_t;

typedef struct ecs_script_type_entity_t {
    const char *name;
    ecs_script_entity_t *node;
    ecs_script_entity_t *parent_node;
    ecs_entity_t parent;
    int32_t table;
    int32_t child_table;
    int32_t slot;
} ecs_script_type_entity_t;

typedef struct ecs_script_type_visitor_t {
    ecs_script_eval_visitor_t *v;
    ecs_vec_t tables;
    ecs_vec_t entities;
    int32_t table;
    int32_t control_depth;
    int32_t for_depth;
    bool template_scope;
    bool function_scope;
} ecs_script_type_visitor_t;

int flecs_script_type_scope(
    ecs_script_type_visitor_t *t,
    ecs_script_scope_t *scope,
    int32_t table,
    bool push_vars,
    bool allow_type);

int32_t flecs_script_type_table_new(
    ecs_script_type_visitor_t *t,
    int32_t parent,
    ecs_script_entity_t *owner);

int32_t flecs_script_type_slot_new(
    ecs_script_type_visitor_t *t,
    int32_t expected);

#endif
