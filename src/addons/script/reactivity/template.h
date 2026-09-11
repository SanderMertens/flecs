/**
 * @file addons/script/reactivity/template.h
 * @brief Script template implementation.
 */

#ifndef FLECS_SCRIPT_TEMPLATE_H
#define FLECS_SCRIPT_TEMPLATE_H

/* Maximum number of template instantiations that can be nested. Guards against
 * templates that (indirectly) instantiate themselves. */
#ifndef FLECS_SCRIPT_TEMPLATE_DEPTH_MAX
#define FLECS_SCRIPT_TEMPLATE_DEPTH_MAX (64)
#endif

extern ECS_COMPONENT_DECLARE(EcsScriptTemplateInstanceUpdateEvent);
extern ECS_COMPONENT_DECLARE(EcsScriptTemplateRoot);

typedef struct ecs_script_template_member_t {
    int32_t index;
    int32_t sp;
    uint64_t input;
    bool is_mut;
    bool is_template;
    ecs_entity_t interface;

    /* Cached member layout for diffing new against old prop values */
    const ecs_type_info_t *diff_ti;
    int32_t diff_offset;
    int32_t diff_size;
    int32_t diff_count;
    bool diff_pod;
} ecs_script_template_member_t;

typedef struct {
    ecs_entity_t type;
    ecs_vec_t defaults;
} ecs_script_template_vars_t;

typedef struct ecs_script_template_capture_t {
    uint64_t outer_input;
    uint64_t outer_internal;
    uint64_t input;
} ecs_script_template_capture_t;

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
    ecs_vec_t capture_input;

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

    uint64_t async_input;
    int32_t symbol_offset;
    int32_t symbol_count;
    int32_t root_symbol;
    int32_t input_count;
    int32_t computed_count;
    int32_t scope_count;
    int32_t component_count;
    int32_t for_count;
    int32_t inherited_count;

    int32_t refcount;

    /* Use non-fragmenting hierarchy */
    bool non_fragmenting_parent;

    /* Template has interface-typed members that need input validation */
    bool has_interface_members;
};

#define ECS_TEMPLATE_SMALL_SIZE (36)

/* Deferred template instance update, queued by the template on_set hook */
typedef struct ecs_script_template_pending_t {
    ecs_entity_t entity;
    ecs_entity_t template_entity;
    ecs_entity_t component;
    uint64_t input;
    int32_t depth;
    void *data;
    bool inline_data;
    int64_t _align;
    char data_storage[ECS_TEMPLATE_SMALL_SIZE];
} ecs_script_template_pending_t;

void flecs_script_template_pending_fini(
    ecs_vec_t *pending);

typedef struct ecs_script_async_block_t {
    ecs_entity_t entity;
    ecs_script_scope_t *scope;
    int32_t scope_slot;
} ecs_script_async_block_t;

typedef struct EcsScriptTemplateRoot {
    ecs_script_state_t state;
    ecs_vec_t observers;
    ecs_vec_t tasks; /* vec<ecs_script_task_t*>, owned. Created by async blocks */
    ecs_vec_t async_blocks;
    uint64_t changed;
} EcsScriptTemplateRoot;

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

ecs_entity_t flecs_script_template_member_interface(
    const ecs_script_template_t *template,
    int32_t sp);

bool flecs_script_template_interface_accepts(
    const ecs_world_t *world,
    ecs_entity_t value,
    ecs_entity_t interface);

bool flecs_script_template_member_is_template(
    const ecs_script_template_t *template,
    int32_t sp);

int flecs_script_template_update_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *node);

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template);

void flecs_script_template_import(
    ecs_world_t *world);

#endif
