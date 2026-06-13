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

    /* Incremental update state */
    int32_t slot_count;
    int32_t dyn_slot_count;
    int32_t control_count;
    int32_t dyn_id_count;

    /* Template contains constructs that can't be updated incrementally (ids
     * with per-iteration values in loops, entity expressions). Instances are
     * fully rebuilt when properties change. */
    bool force_full;
};

typedef struct ecs_script_template_member_t {
    ecs_size_t offset;
    ecs_entity_t type;
} ecs_script_template_member_t;

typedef struct ecs_script_template_instance_t {
    ecs_entity_t template_entity;
    bool dirty;
    void *prev_values;
    ecs_script_template_member_t *members;
    int32_t member_count;
    ecs_vec_t entities;          /* vec<int64_t> */
    ecs_vec_t dynamic_entities;  /* vec<ecs_vec_t> of vec<int64_t> */
    ecs_vec_t control;           /* vec<int8_t> */
    ecs_vec_t dynamic_ids;       /* vec<ecs_id_t> */
} ecs_script_template_instance_t;

typedef struct EcsScriptTemplateData {
    ecs_vec_t instances;         /* vec<ecs_script_template_instance_t*> */
} EcsScriptTemplateData;

struct ecs_script_template_eval_ctx_t {
    ecs_script_template_t *template_;
    ecs_script_template_instance_t *state;
    uint64_t changed;
    int32_t *cursors;
    ecs_id_t *prev_ids;
    int32_t loop_depth;
    bool create;
    bool force;
    bool with_changed;

    /* Entities created during an incremental update. Used to restore template
     * declaration order for parents with ordered children. vec<ecs_entity_t> */
    ecs_vec_t *created;
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

int flecs_script_template_check(
    ecs_parser_t *parser,
    ecs_script_template_node_t *node);

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template);

void flecs_script_template_import(
    ecs_world_t *world);

bool flecs_script_template_skip_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node);

int flecs_script_template_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    bool *reused);

ecs_entity_t flecs_script_template_slot_entity(
    ecs_script_eval_visitor_t *v,
    int32_t slot);

void flecs_script_template_reset_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    bool remove_components);

void flecs_script_template_consume_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

void flecs_script_template_remove_ids(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    ecs_entity_t src);

void flecs_script_template_loop_begin(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

void flecs_script_template_loop_end(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

void flecs_script_template_sync_id(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t src,
    ecs_script_id_t *id);

void flecs_script_template_sync_scope_id(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    int32_t id_slot,
    ecs_id_t new_id);

void flecs_script_template_sync_with_ids(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node);

#endif
