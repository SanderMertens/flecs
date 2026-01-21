/**
 * @file addons/script/template.h
 * @brief Script template implementation.
 */

#ifndef FLECS_SCRIPT_TEMPLATE_H
#define FLECS_SCRIPT_TEMPLATE_H

extern ECS_COMPONENT_DECLARE(EcsScriptTemplateSetEvent);

struct ecs_script_template_t {
    /* Template handle */
    ecs_entity_t entity;

    /* Template AST node */
    ecs_script_template_node_t *node;

    /* Hoisted using statements */
    ecs_vec_t using_;

    /* Hoisted variables */
    ecs_script_vars_t *vars;

    /* Default values for props */
    ecs_vec_t prop_defaults;

    /* Type info for template component */
    const ecs_type_info_t *type_info;

    /* Annotations to apply to template instance */
    ecs_vec_t annot;

    /* Use non-fragmenting hierarchy */
    bool non_fragmenting_parent;
};

#define ECS_TEMPLATE_SMALL_SIZE (36)

/* Event used for deferring template instantiation */
typedef struct EcsScriptTemplateSetEvent {
    ecs_entity_t template_entity;
    ecs_entity_t *entities;
    void *data;
    int32_t count;

    /* Storage for small template types */
    int64_t _align; /* Align data storage to 8 bytes */
    char data_storage[ECS_TEMPLATE_SMALL_SIZE];
    ecs_entity_t entity_storage;
} EcsScriptTemplateSetEvent;

int flecs_script_eval_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *template);

ecs_script_template_t* flecs_script_template_init(
    ecs_script_impl_t *script);

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template);

void flecs_script_template_import(
    ecs_world_t *world);

#endif
