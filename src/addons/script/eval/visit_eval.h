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
    ecs_script_vars_t *vars;
    void *type_visitor;
    ecs_vec_t *symbol_slots;
    ecs_vec_t *component_slots;
    ecs_vec_t *scope_slots;
    ecs_vec_t *for_slots;
    uint64_t input;
    int32_t symbol_offset;
    int32_t visit;
    int32_t scope_slot;
    int32_t for_slot;
    bool force;
} ecs_script_eval_visitor_t;

int flecs_script_eval(
    const ecs_script_t *script,
    const ecs_script_eval_desc_t *desc,
    ecs_id_t tag,
    uint64_t input,
    ecs_script_eval_result_t *result);

/* Statement evaluation state. Owned by the frame stack of the runner (or the
 * C stack for the check visitor), never by the AST, so that concurrent
 * evaluations of the same script cannot interfere. */

typedef struct flecs_script_scope_state_t {
    ecs_entity_t parent;
    int32_t scope_slot;
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
    bool prev_force;
    bool prev_is_with_scope;
    int32_t symbol;
    int32_t for_slot;
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
    bool force;
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
    bool force;
} flecs_script_pair_scope_state_t;

int flecs_script_eval_pair_scope_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_pair_scope_t *node,
    flecs_script_pair_scope_state_t *state);

void flecs_script_eval_pair_scope_leave(
    ecs_script_eval_visitor_t *v,
    const flecs_script_pair_scope_state_t *state);

typedef enum flecs_script_for_kind_t {
    FlecsScriptForRange,
    FlecsScriptForArray,
    FlecsScriptForVector,
    FlecsScriptForMap
} flecs_script_for_kind_t;

typedef struct flecs_script_for_state_t {
    flecs_script_for_kind_t kind;
    ecs_script_var_t *elem_var;
    ecs_script_var_t *key_var;
    ecs_script_var_t *index_var;
    int32_t index;
    int32_t from;
    int32_t count;
    ecs_size_t elem_size;
    ecs_size_t key_size;
    void *elems;
    ecs_value_t collection;
    ecs_map_iter_t map_it;
    int32_t for_slot;
    bool force;
} flecs_script_for_state_t;

typedef struct flecs_script_frame_t {
    ecs_script_node_t *node;
    int32_t pc;
    union {
        flecs_script_scope_state_t scope;
        flecs_script_entity_state_t entity;
        flecs_script_for_state_t for_;
        flecs_script_with_state_t with;
        flecs_script_pair_scope_state_t pair_scope;
        struct {
            int32_t catch_index;
        } try_;
        struct {
            bool force;
        } if_;
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

bool flecs_script_is_builtin(
    const ecs_world_t *world,
    ecs_entity_t e);

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

int flecs_script_eval_id_elem(
    ecs_script_eval_visitor_t *v,
    void *node,
    ecs_expr_node_t **name_expr,
    ecs_entity_t eval,
    int32_t slot,
    int32_t sp,
    ecs_entity_t *elem);

void flecs_script_eval_visit_init(
    const ecs_script_impl_t *script,
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc);

/* Push/pop a variable scope that exposes external variables to evaluation.
 * Used when evaluating entity statements embedded in expressions, which can
 * reference variables of the expression they're embedded in. */
void flecs_script_eval_push_vars(
    ecs_script_eval_visitor_t *v,
    const ecs_script_vars_t *vars);

void flecs_script_eval_pop_vars(
    ecs_script_eval_visitor_t *v);

void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc);

void flecs_script_eval_begin(
    ecs_script_eval_visitor_t *v,
    uint64_t input,
    int32_t visit);

void flecs_script_eval_cleanup(
    ecs_script_eval_visitor_t *v);

int flecs_script_eval_node(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

int flecs_script_symbol_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t from,
    const char *name,
    flecs_script_lookup_kind_t lookup_kind,
    flecs_script_symbol_t *symbol);

int flecs_script_id_elem_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t first,
    const char *name,
    flecs_script_lookup_kind_t lookup_kind,
    ecs_entity_t *from_out,
    flecs_script_symbol_t *symbol);

int flecs_script_id_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    const char *first_name,
    const char *second_name,
    flecs_script_lookup_kind_t lookup_kind,
    ecs_entity_t *first_out,
    ecs_id_t *id_out,
    const char **unresolved);

ecs_entity_t flecs_script_symbol_entity(
    const ecs_script_eval_visitor_t *v,
    int32_t slot);

void flecs_script_symbol_set(
    ecs_script_eval_visitor_t *v,
    int32_t slot,
    ecs_entity_t entity);

int flecs_script_type_symbol_lookup(
    void *ctx,
    const char *name,
    flecs_script_symbol_t *symbol);

int flecs_script_visit_type(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope);

int flecs_script_visit_type_entity_expr(
    ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *entity);

/* For statement (see visit_for.c) */

int flecs_script_step_for(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame);

void flecs_script_eval_for_leave(
    ecs_script_eval_visitor_t *v,
    flecs_script_for_state_t *state);

int flecs_script_for_collection_kind(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    ecs_entity_t type,
    flecs_script_for_kind_t *kind,
    ecs_entity_t *key_type,
    ecs_entity_t *elem_type);

int flecs_script_for_check_var_count(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_kind_t kind);

bool flecs_script_for_has_index_var(
    flecs_script_for_kind_t kind,
    int32_t loop_var_count);

ecs_script_var_t* flecs_script_for_declare_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    const char *name,
    ecs_entity_t type,
    bool alloc);

/* Functions shared between type and eval visitor */

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export);

#endif
