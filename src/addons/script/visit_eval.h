/**
 * @file addons/script/visit_eval.h
 * @brief Script evaluation visitor.
 */

#ifndef FLECS_SCRIPT_VISIT_EVAL_H
#define FLECS_SCRIPT_VISIT_EVAL_H

typedef struct ecs_script_eval_visitor_t {
    ecs_script_visit_t base;
    ecs_world_t *world;
    ecs_script_runtime_t *r;
    ecs_script_template_t *template; /* Set when creating template */
    ecs_entity_t template_entity; /* Set when creating template instance */
    ecs_entity_t module;
    ecs_entity_t parent;
    ecs_script_entity_t *entity;
    ecs_entity_t with_relationship;
    int32_t with_relationship_sp;
    bool is_with_scope;
    bool dynamic_variable_binding;
    ecs_script_vars_t *vars;
} ecs_script_eval_visitor_t;

void flecs_script_eval_error_(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node,
    const char *fmt,
    ...);

#define flecs_script_eval_error(v, node, ...)\
    flecs_script_eval_error_(v, (ecs_script_node_t*)node, __VA_ARGS__)

int flecs_script_find_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t from,
    const char *path,
    ecs_expr_node_t **name_expr,
    int32_t *frame_offset,
    ecs_entity_t *out,
    bool *is_var);

ecs_script_var_t* flecs_script_find_var(
    const ecs_script_vars_t *vars,
    const char *name,
    int32_t *frame_offset);

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name);

const ecs_type_info_t* flecs_script_get_type_info(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_id_t id);

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_value_t *value);

void flecs_script_eval_visit_init(
    const ecs_script_impl_t *script,
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc);

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc);

int flecs_script_eval_node(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

int flecs_script_check_node(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

int flecs_script_check_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node);

/* Functions shared between check and eval visitor */

int flecs_script_eval_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node);

int flecs_script_eval_id(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_script_id_t *id);

int flecs_script_eval_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_using_t *node);

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export);

ecs_entity_t flecs_script_find_entity_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx);

#endif
