/**
 * @file addons/script/eval/visit_eval.h
 * @brief Script evaluation visitor.
 */

#ifndef FLECS_SCRIPT_VISIT_EVAL_H
#define FLECS_SCRIPT_VISIT_EVAL_H

typedef struct flecs_script_entity_state_t flecs_script_entity_state_t;

typedef struct ecs_script_eval_visitor_t {
    ecs_script_visit_t base;
    ecs_world_t *world;
    ecs_script_runtime_t *r;
    ecs_script_template_t *template; /* Set when creating template */
    ecs_script_template_t *instance_template;
    ecs_entity_t template_entity; /* Set when creating template instance */
    ecs_entity_t script_entity;
    ecs_id_t script_tag; /* Added to entities created by managed scripts */
    ecs_entity_t module;
    ecs_entity_t parent;
    flecs_script_entity_state_t *entity;
    ecs_entity_t with_relationship;
    int32_t with_relationship_sp;
    bool is_with_scope;
    bool dynamic_variable_binding;
    ecs_script_vars_t *vars;
} ecs_script_eval_visitor_t;

int flecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_id_t tag,
    ecs_script_eval_result_t *result);

/* Statement evaluation state. Owned by the frame stack of the runner (or the
 * C stack for the check visitor), never by the AST, so that concurrent
 * evaluations of the same script cannot interfere. */

typedef struct flecs_script_scope_state_t {
    ecs_entity_t parent;
    int32_t using_count;
} flecs_script_scope_state_t;

void flecs_script_eval_scope_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *node,
    flecs_script_scope_state_t *state);

void flecs_script_eval_scope_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_scope_state_t *state);

struct flecs_script_entity_state_t {
    ecs_script_entity_t *node;
    ecs_entity_t eval;
    ecs_entity_t eval_kind;
    flecs_script_entity_state_t *prev_entity;
    ecs_entity_t prev_template_entity;
    bool prev_is_with_scope;
};

int flecs_script_eval_entity_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    flecs_script_entity_state_t *state);

void flecs_script_eval_entity_leave(
    ecs_script_eval_visitor_t *v,
    flecs_script_entity_state_t *state);

typedef struct flecs_script_with_state_t {
    ecs_stack_cursor_t *cursor;
    int32_t with_count;
    bool is_with_scope;
} flecs_script_with_state_t;

int flecs_script_eval_with_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node,
    flecs_script_with_state_t *state);

void flecs_script_eval_with_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_with_state_t *state);

typedef struct flecs_script_pair_scope_state_t {
    ecs_entity_t with_relationship;
    ecs_entity_t second;
    int32_t with_relationship_sp;
} flecs_script_pair_scope_state_t;

int flecs_script_eval_pair_scope_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node,
    flecs_script_pair_scope_state_t *state);

void flecs_script_eval_pair_scope_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_pair_scope_state_t *state);

typedef struct flecs_script_frame_t {
    ecs_script_node_t *node;
    int32_t pc;
    union {
        flecs_script_scope_state_t scope;
        flecs_script_entity_state_t entity;
        struct {
            ecs_script_var_t *var;
            int32_t current;
            int32_t to;
        } for_;
        flecs_script_with_state_t with;
        flecs_script_pair_scope_state_t pair_scope;
        struct {
            /* Index of catch clause being executed, -1 while in try body */
            int32_t catch_index;
        } try_;
    } state;
} flecs_script_frame_t;

typedef enum flecs_script_run_status_t {
    FlecsScriptRunDone,
    FlecsScriptRunError,
    FlecsScriptRunSuspended
} flecs_script_run_status_t;

typedef struct ecs_script_runner_t {
    ecs_script_eval_visitor_t v;
    flecs_script_frame_t frames[ECS_SCRIPT_VISIT_MAX_DEPTH];
    int32_t frame_count;
    ecs_entity_t last_entity; /* Result of last completed entity frame */
    bool can_suspend;
#ifdef FLECS_SCRIPT_ASYNC
    ecs_script_future_t *future; /* Pending awaited future */
    ecs_entity_t async_entity;   /* Owner entity for async function calls */

    /* Rejected future whose error is propagating, catchable with try/catch.
     * Runtime errors don't set this and are never caught. */
    ecs_script_future_t *thrown;
    ecs_script_node_t *throw_node;
#endif
} ecs_script_runner_t;

void flecs_script_runner_init(
    ecs_script_runner_t *r,
    const ecs_script_impl_t *script,
    const ecs_script_eval_desc_t *desc);

void flecs_script_runner_fini(
    ecs_script_runner_t *r,
    const ecs_script_eval_desc_t *desc);

flecs_script_run_status_t flecs_script_runner_run_scope(
    ecs_script_runner_t *r,
    ecs_script_scope_t *scope);

/* Evaluate an entity statement embedded in an expression, either in the
 * context of an in-progress evaluation (v != NULL) or standalone. */
int flecs_script_eval_entity(
    ecs_script_eval_visitor_t *v,
    const ecs_script_t *script,
    const ecs_script_vars_t *vars,
    ecs_script_entity_t *node,
    ecs_entity_t *out);

void flecs_script_runner_abandon(
    ecs_script_runner_t *r);

const char* flecs_script_runner_stmt_pos(
    const ecs_script_runner_t *r);

void flecs_script_scope_push(
    ecs_script_runner_t *r,
    ecs_script_scope_t *scope);

void flecs_script_frame_pop(
    ecs_script_runner_t *r);

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

int flecs_script_prepare_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_entity_t type);

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

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export);

ecs_entity_t flecs_script_find_entity_action(
    const ecs_world_t *world,
    const char *path,
    void *ctx);

#endif
