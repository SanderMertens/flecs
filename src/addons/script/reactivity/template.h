/**
 * @file addons/script/reactivity/template.h
 * @brief Script template implementation.
 */

#ifndef FLECS_SCRIPT_TEMPLATE_H
#define FLECS_SCRIPT_TEMPLATE_H

extern ECS_COMPONENT_DECLARE(EcsScriptTemplateSetEvent);
extern ECS_COMPONENT_DECLARE(EcsScriptTemplateInstanceUpdateEvent);
extern ECS_COMPONENT_DECLARE(EcsScriptTemplateRoot);

typedef struct ecs_script_template_member_t {
    int32_t index;
    uint64_t input;
    bool is_mut;
} ecs_script_template_member_t;

typedef struct {
    ecs_entity_t type;
    ecs_vec_t defaults;
} ecs_script_template_vars_t;

struct ecs_script_template_t {
    ecs_script_template_vars_t props;
    ecs_script_template_vars_t muts;

    /* Template AST node */
    ecs_script_template_node_t *node;

    /* Hoisted using statements */
    ecs_vec_t using_;

    /* Hoisted variables */
    ecs_script_vars_t *vars;
    ecs_vec_t capture_sp;

    /* Prop and mut members in variable declaration order */
    ecs_vec_t members;

    /* Type info for template component */
    const ecs_type_info_t *type_info;

    /* Annotations to apply to template instance */
    ecs_vec_t annot;

    /* Statically known component references used in the template body */
    ecs_vec_t refs;

    /* Observers monitoring the refs, shared by all template instances */
    ecs_vec_t observers;

    ecs_vec_t dynamic_refs;

    int32_t symbol_offset;
    int32_t symbol_count;
    int32_t root_symbol;
    int32_t input_count;
    int32_t scope_count;
    int32_t component_count;
    int32_t for_count;

    int32_t refcount;

    /* Use non-fragmenting hierarchy */
    bool non_fragmenting_parent;
};

#define ECS_TEMPLATE_SMALL_SIZE (36)

typedef struct EcsScriptTemplateRoot {
    ecs_vec_t observers;
    ecs_vec_t symbol_slots;
    ecs_vec_t component_slots;
    ecs_vec_t scope_slots;
    ecs_vec_t for_slots;
    uint64_t changed;
    int32_t visit;
    bool initialized;
} EcsScriptTemplateRoot;

/* Event used for deferring template instantiation */
typedef struct EcsScriptTemplateSetEvent {
    ecs_entity_t template_entity;
    ecs_entity_t component;
    ecs_entity_t *entities;
    uint64_t *inputs;
    void *data;
    int32_t count;

    /* Storage for small template types */
    int64_t _align; /* Align data storage to 8 bytes */
    char data_storage[ECS_TEMPLATE_SMALL_SIZE];
    ecs_entity_t entity_storage;
    uint64_t input_storage;
} EcsScriptTemplateSetEvent;

typedef struct EcsScriptTemplateInstanceUpdateEvent {
    ecs_entity_t template_entity;
    ecs_entity_t instance;
    uint64_t input;
} EcsScriptTemplateInstanceUpdateEvent;

int flecs_script_eval_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *template);

int flecs_script_template_eval_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool mut);

int flecs_script_template_update_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *node);

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template);

void flecs_script_template_import(
    ecs_world_t *world);

#endif
