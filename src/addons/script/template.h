/**
 * @file addons/script/template.h
 * @brief Script template implementation.
 */

#ifndef FLECS_SCRIPT_TEMPLATE_H
#define FLECS_SCRIPT_TEMPLATE_H

extern ECS_COMPONENT_DECLARE(EcsScriptTemplateSetEvent);

struct ecs_script_template_t {
    ecs_entity_t entity;
    ecs_script_template_node_t *node;
    ecs_vec_t using_;            /* Hoisted using statements */
    ecs_script_vars_t *vars;     /* Hoisted variables from enclosing scopes */
    ecs_vec_t prop_defaults;     /* Default values for template properties */
    const ecs_type_info_t *type_info;
    ecs_vec_t annot;             /* Annotations applied to template instances */
    bool non_fragmenting_parent;
};

#define ECS_TEMPLATE_SMALL_SIZE (36)

/* Deferred template instantiation event */
typedef struct EcsScriptTemplateSetEvent {
    ecs_entity_t template_entity;
    ecs_entity_t *entities;
    void *data;
    int32_t count;

    /* Inline storage for single-entity, small, trivially-destructible templates
     * (avoids heap allocation). _align forces 8-byte alignment of data_storage. */
    int64_t _align;
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
