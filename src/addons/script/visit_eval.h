/**
 * @file addons/script/assembly.h
 * @brief Flecs assembly code.
 */

#ifndef FLECS_SCRIPT_EVAL_H
#define FLECS_SCRIPT_EVAL_H

typedef struct ecs_script_eval_visitor_t {
    ecs_script_visit_t base;
    ecs_world_t *world;
    ecs_allocator_t *allocator;
    ecs_script_assembly_t *assembly;
    ecs_entity_t module;
    ecs_entity_t parent;
    ecs_script_entity_t *entity;
    ecs_vec_t using;
    ecs_vec_t with;
    ecs_vec_t annot;
    ecs_entity_t with_relationship;
    int32_t with_relationship_sp;
    ecs_script_vars_t *vars;
    ecs_stack_t stack;
} ecs_script_eval_visitor_t;

void flecs_script_eval_error_(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node,
    const char *fmt,
    ...);

#define flecs_script_eval_error(v, node, ...)\
    flecs_script_eval_error_(v, (ecs_script_node_t*)node, __VA_ARGS__)

ecs_entity_t flecs_script_find_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t from,
    const char *path);

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name);

const ecs_type_info_t* flecs_script_get_type_info(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_id_t id);

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    const char *expr,
    ecs_value_t *value);

int flecs_script_eval_assembly(
    ecs_script_eval_visitor_t *v,
    ecs_script_assembly_node_t *assembly);

ecs_script_assembly_t* flecs_script_assembly_init(
    ecs_script_t *script);

void flecs_script_assembly_fini(
    ecs_script_t *script,
    ecs_script_assembly_t *assembly);

void flecs_script_eval_visit_init(
    ecs_script_t *script,
    ecs_script_eval_visitor_t *v);

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v);

int flecs_script_eval_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node);

#endif
