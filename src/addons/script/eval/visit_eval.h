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
    ecs_entity_t template_instance;
    ecs_entity_t body_template; /* Template of the body being instantiated. Set
                                 * for the entire body, at any scope depth. */
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
    uint64_t input;
    uint64_t internal;
    ecs_script_state_t *state;
    int32_t symbol_offset;
    int32_t visit;
    int32_t scope_slot;
    int32_t for_slot;
    bool force;
    const ecs_script_template_ref_t *id_ref;
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
    bool created;
};

void flecs_script_scope_add_ids(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity,
    ecs_script_scope_t *scope,
    const ecs_id_t *ids,
    int32_t count);

FLECS_API
int flecs_script_eval_entity_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    flecs_script_entity_state_t *state);

int flecs_script_eval_entity_enter_w_name(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    const ecs_value_t *name,
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
    ecs_script_vars_t *vars;
    int32_t var_base;
    int32_t elem_var;
    int32_t key_var;
    int32_t index_var;
    ecs_entity_t var_types[3];
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
            bool force;
        } if_;
    } state;
} flecs_script_frame_t;

typedef enum flecs_script_run_status_t {
    FlecsScriptRunDone,
    FlecsScriptRunError,
    FlecsScriptRunSuspended
} flecs_script_run_status_t;

#define ECS_SCRIPT_FRAME_CHUNK_SIZE (16)

#ifdef FLECS_SCRIPT_ASYNC
typedef struct flecs_script_async_state_t {
    ecs_script_future_t *future;
    ecs_entity_t entity;
    ecs_script_future_t *thrown;
    const ecs_script_node_t *throw_node;
} flecs_script_async_state_t;
#endif

typedef struct ecs_script_runner_t {
    ecs_script_eval_visitor_t v;
    flecs_script_frame_t *frames[(ECS_SCRIPT_VISIT_MAX_DEPTH +
        ECS_SCRIPT_FRAME_CHUNK_SIZE - 1) / ECS_SCRIPT_FRAME_CHUNK_SIZE];
    flecs_script_frame_t frame_storage[ECS_SCRIPT_FRAME_CHUNK_SIZE];
    int32_t frame_count;
    ecs_entity_t last_entity; /* Result of last completed entity frame */
    bool can_suspend;
#ifdef FLECS_SCRIPT_ASYNC
    flecs_script_async_state_t async;
#endif
} ecs_script_runner_t;

void flecs_script_runner_init(
    ecs_script_runner_t *r,
    const ecs_script_eval_visitor_t *v);

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
    const ecs_script_node_t *node,
    const char *fmt,
    ...);

#define flecs_script_eval_error(v, node, ...)\
    flecs_script_eval_error_(v, (const ecs_script_node_t*)node, __VA_ARGS__)

bool flecs_script_is_builtin(
    const ecs_world_t *world,
    ecs_entity_t e);

bool flecs_script_can_own_entity(
    const ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t e);

ecs_entity_t flecs_script_new_entity(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t parent,
    const char *name,
    bool non_fragmenting_parent);

ecs_entity_t flecs_script_create_entity(
    ecs_script_eval_visitor_t *v,
    const char *name);

const ecs_type_info_t* flecs_script_get_type_info(
    ecs_script_eval_visitor_t *v,
    const void *node,
    ecs_id_t id);

ecs_expr_eval_desc_t flecs_script_expr_desc(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t type);

int flecs_script_eval_expr(
    ecs_script_eval_visitor_t *v,
    ecs_expr_node_t **expr_ptr,
    ecs_value_t *value);

int flecs_script_assign_value(
    ecs_script_eval_visitor_t *v,
    ecs_script_assign_t *node,
    const void *src);

ecs_entity_t flecs_script_lookup_name(
    ecs_script_eval_visitor_t *v,
    const char *name);

int flecs_script_eval_id_elem(
    ecs_script_eval_visitor_t *v,
    const void *node,
    const char *const *name,
    ecs_entity_t eval,
    int32_t slot,
    int32_t sp,
    ecs_entity_t *elem);

int flecs_script_eval_id_resolve(
    ecs_script_eval_visitor_t *v,
    const void *node,
    const ecs_script_id_t *id,
    const char *first_name,
    const char *second_name,
    int32_t index,
    ecs_id_t *out);

int flecs_script_eval_interface_id(
    ecs_script_eval_visitor_t *v,
    const void *node,
    const ecs_script_id_t *id,
    ecs_id_t *out);

int flecs_script_eval_tag_id(
    ecs_script_eval_visitor_t *v,
    const ecs_script_tag_t *node,
    ecs_id_t id,
    bool resolved);

int flecs_script_eval_with_tag_id(
    ecs_script_eval_visitor_t *v,
    const ecs_script_tag_t *node,
    ecs_id_t id);

int flecs_script_eval_component_check(
    ecs_script_eval_visitor_t *v,
    const ecs_script_component_t *node,
    ecs_id_t id,
    bool resolved);

int flecs_script_eval_component_visit(
    ecs_script_eval_visitor_t *v,
    const ecs_script_component_t *node,
    ecs_id_t id,
    const EcsScriptVisitor *visitor);

int flecs_script_eval_component_add(
    ecs_script_eval_visitor_t *v,
    const ecs_script_component_t *node,
    ecs_id_t id);

int flecs_script_value_to_bool(
    ecs_script_eval_visitor_t *v,
    const void *node,
    const ecs_value_t *value,
    bool *out);

bool flecs_script_stmt_support(
    const ecs_script_node_t *node);

static inline bool flecs_script_eval_force(
    const ecs_script_eval_visitor_t *v,
    const ecs_script_node_t *node,
    bool prev)
{
    return prev || (node->direct_input & v->input) ||
        (node->direct_internal & v->internal);
}

static inline void flecs_script_const_var_init(
    ecs_script_var_t *var,
    const ecs_type_info_t *ti,
    ecs_entity_t type,
    void *ptr,
    bool owned)
{
    var->is_const = true;
    var->type_info = ti;
    var->value.type = type;
    var->value.ptr = ptr;
    var->owned = owned;
}

FLECS_API
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

FLECS_API
void flecs_script_eval_visit_fini(
    ecs_script_eval_visitor_t *v,
    const ecs_script_eval_desc_t *desc);

void flecs_script_eval_begin(
    ecs_script_eval_visitor_t *v,
    uint64_t input,
    int32_t visit);

void flecs_script_eval_cleanup(
    ecs_script_eval_visitor_t *v,
    bool dirty);

FLECS_API
int flecs_script_eval_node(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

int flecs_script_symbol_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t from,
    const char *name,
    ecs_flags32_t lookup_kind,
    flecs_script_symbol_t *symbol);

int flecs_script_id_elem_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    ecs_entity_t first,
    const char *name,
    ecs_flags32_t lookup_kind,
    ecs_entity_t *from_out,
    flecs_script_symbol_t *symbol);

int flecs_script_id_lookup(
    const ecs_script_t *script,
    const ecs_expr_eval_desc_t *desc,
    const char *first_name,
    const char *second_name,
    ecs_flags32_t lookup_kind,
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

void flecs_script_for_prepare(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state);

int flecs_script_for_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state,
    const ecs_value_t *collection);

int flecs_script_for_declare_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state,
    bool alloc_elem);

bool flecs_script_for_next(
    flecs_script_for_state_t *state);

/* Runtime bookkeeping helpers shared with the IR runtime */

ecs_value_t* flecs_script_with_append(
    ecs_script_eval_visitor_t *v,
    const ecs_type_info_t *ti);

void flecs_script_with_set_count(
    ecs_script_eval_visitor_t *v,
    int32_t count);

ecs_entity_t flecs_script_get_src(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_script_can_default_ctor(
    ecs_world_t *world,
    ecs_id_t component);

void flecs_script_track_component(
    ecs_script_eval_visitor_t *v,
    int32_t component_slot,
    ecs_id_t component);

bool flecs_script_scope_visited(
    ecs_script_eval_visitor_t *v,
    int32_t slot);

/* Functions shared between type and eval visitor */

ecs_script_computed_t* flecs_script_computed_get(
    ecs_script_eval_visitor_t *v,
    const ecs_script_var_node_t *node);

bool flecs_script_computed_store(
    ecs_script_eval_visitor_t *v,
    ecs_script_computed_t *slot,
    int32_t index,
    const ecs_type_info_t *ti,
    const void *value);

int flecs_script_eval_const_cached(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node);

int flecs_script_eval_const(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool export);

int flecs_script_eval_pair_elem(
    ecs_script_eval_visitor_t *v,
    const void *node,
    const char *const *name,
    const char *str,
    ecs_entity_t eval,
    int32_t symbol,
    int32_t sp,
    ecs_entity_t *out);

static inline FLECS_ALWAYS_INLINE int flecs_script_eval_pair_scope_enter(
    ecs_script_eval_visitor_t *v,
    const ecs_script_pair_scope_t *node,
    const char *first_name,
    const char *second_name,
    flecs_script_pair_scope_state_t *state)
{
    const ecs_script_id_t *id = &node->id;
    ecs_entity_t first, second;
    if (flecs_script_eval_pair_elem(v, node,
        id->first_expr ? &first_name : NULL, id->first, id->first_eval,
        id->first_symbol, id->first_sp, &first))
    {
        return -1;
    }

    if (flecs_script_eval_pair_elem(v, node,
        id->second_expr ? &second_name : NULL, id->second, id->second_eval,
        id->second_symbol, id->second_sp, &second))
    {
        return -1;
    }

    state->with_relationship = v->with_relationship;
    state->second = 0;
    state->with_relationship_sp = v->with_relationship_sp;
    state->force = v->force;

    v->with_relationship = first;

    if (state->with_relationship != first) {
        ecs_value_t *value = flecs_script_with_append(v, NULL);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
        v->with_relationship_sp = ecs_vec_count(&v->r->with) - 1;
    } else {
        ecs_value_t *value = &ecs_vec_get_t(
            &v->r->with, ecs_script_with_value_t,
            v->with_relationship_sp)->value;
        ecs_assert(ECS_PAIR_FIRST(value->type) == (uint32_t)first,
            ECS_INTERNAL_ERROR, NULL);
        state->second = ECS_PAIR_SECOND(value->type);
        value->type = ecs_pair(first, second);
        value->ptr = NULL;
    }

    v->force = flecs_script_eval_force(v, &node->node, state->force);
    return 0;
}

#endif
