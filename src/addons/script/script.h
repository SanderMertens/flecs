/**
 * @file addons/script/script.h
 * @brief Flecs script implementation.
 */

#ifndef FLECS_SCRIPT_PRIVATE_H
#define FLECS_SCRIPT_PRIVATE_H

#include "../../private_api.h"
#include "../parser/parser.h"

#ifdef FLECS_SCRIPT

typedef struct ecs_script_entity_t ecs_script_entity_t;

#define flecs_script_impl(script) ((ecs_script_impl_t*)script)

typedef struct ecs_script_ref_t {
    ecs_entity_t entity;
    const char *name;
    ecs_id_t component;
    ecs_entity_t observer;
} ecs_script_ref_t;

typedef struct ecs_script_ref_ctx_t {
    ecs_entity_t script;
    ecs_entity_t instance;
} ecs_script_ref_ctx_t;

typedef struct EcsScriptUpdateEvent {
    ecs_entity_t script;
} EcsScriptUpdateEvent;

extern ECS_COMPONENT_DECLARE(EcsScriptUpdateEvent);

/* Context passed to script visitor callbacks. */
typedef struct ecs_script_visitor_ctx_t {
    ecs_world_t *world;
    ecs_entity_t entity;                         /* Entity being defined */
    ecs_entity_t kind;                           /* Kind with the visitor */
    struct ecs_expr_initializer_t *initializer;  /* Initializer AST */
    struct ecs_script_eval_visitor_t *eval;      /* Script evaluation context */
    void *ctx;                                   /* User context */
} ecs_script_visitor_ctx_t;

/* Script visitor callback. */
typedef int(*ecs_script_visitor_action_t)(
    const ecs_script_visitor_ctx_t *ctx);

/* Component that customizes how initializer syntax is interpreted. When an
 * entity kind has this component, the initializer AST of a "Kind entity(...)"
 * statement is passed to the visitor callback instead of being assigned as a
 * component value. This is used to implement the type definition syntax
 * ("struct Position(x: f32, y: f32)", "enum Color(Red, Green, Blue)").
 * Private for now; can become a public customization point once the
 * expression AST types are public. */
typedef struct EcsScriptVisitor {
    ecs_script_visitor_action_t visit;
    void *ctx;
} EcsScriptVisitor;

FLECS_API extern ECS_COMPONENT_DECLARE(EcsScriptVisitor);

struct ecs_script_impl_t {
    ecs_script_t pub;
    ecs_entity_t entity; /* Set if script is managed (has EcsScript) */
    ecs_allocator_t allocator;
    ecs_script_scope_t *root;
    ecs_expr_node_t *expr; /* Only set if script is just an expression */
    char *token_buffer;
    char *token_remaining; /* Remaining space in token buffer */
    const char *next_token; /* First character after expression */
    int32_t token_buffer_size;
    int32_t refcount;
    ecs_vec_t refs;
    bool evaluating;
};

typedef struct ecs_function_calldata_t {
    ecs_entity_t function;
    struct {
        ecs_function_callback_t callback;
        ecs_vector_function_callback_t vector_callback;
    } is;
    int32_t vector_elem_count;
#ifdef FLECS_SCRIPT_ASYNC
    ecs_async_function_callback_t async_callback;
    ecs_async_function_cancel_t async_cancel;
#endif
    void *ctx;
} ecs_function_calldata_t;

#include "ast.h"
#include "expr/expr.h"
#include "visit.h"
#include "visit_eval.h"
#include "async.h"
#include "template.h"

struct ecs_script_runtime_t {
    ecs_allocator_t allocator;
    ecs_expr_stack_t expr_stack;
    ecs_stack_t stack;
    ecs_vec_t using;
    ecs_vec_t with;
    ecs_vec_t with_type_info;
    ecs_vec_t annot;

    /* Tag added to entities created by the currently evaluating managed
     * script. Carried on the world runtime so evaluation triggered from hooks
     * (such as template instantiation) inherits it. */
    ecs_id_t current_tag;

    char *error_name;
    int32_t include_depth;
    bool error;
};

ecs_script_t* flecs_script_new(
    ecs_world_t *world);

ecs_entity_t flecs_script_create_ref_observer(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_entity_t entity,
    ecs_id_t component,
    ecs_iter_action_t callback);

void flecs_script_update_ref_observers(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance,
    ecs_vec_t *refs,
    ecs_vec_t *observers,
    ecs_iter_action_t callback);

ecs_script_scope_t* flecs_script_scope_new(
    ecs_parser_t *parser);

int flecs_script_visit_free(
    ecs_script_t *script);

int flecs_script_visit_free_node(
    ecs_script_t *script,
    ecs_script_node_t *node);

ecs_script_vars_t* flecs_script_vars_push(
    ecs_script_vars_t *parent,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator);

ecs_script_runtime_t* flecs_script_runtime_get(
    ecs_world_t *world);

void flecs_script_runtime_error_reset(
    ecs_script_runtime_t *r);

void flecs_script_register_builtin_functions(
    ecs_world_t *world);

void flecs_function_import(
    ecs_world_t *world);

const char* flecs_script_stmt(
    ecs_parser_t *parser,
    const char *pos);

ecs_script_t* flecs_script_parse_nested(
    ecs_world_t *world,
    const char *name,
    const char *using_code,
    const char *code,
    const char **next);

int ecs_script_ast_node_to_buf(
    const ecs_script_t *script,
    ecs_script_node_t *node,
    ecs_strbuf_t *buf,
    bool colors,
    int32_t depth);

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r);

int flecs_script_apply_annot(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    ecs_entity_t entity,
    ecs_script_annot_t *annot);

/* Type visitors (implement struct/enum/bitmask initializer syntax) */

int flecs_script_struct_visit(
    const ecs_script_visitor_ctx_t *ctx);

int flecs_script_enum_visit(
    const ecs_script_visitor_ctx_t *ctx);

int flecs_script_bitmask_visit(
    const ecs_script_visitor_ctx_t *ctx);

/* Script functions */
double flecs_lerp(
    double a,
    double b,
    double t);

typedef struct ecs_script_user_function_t {
    ecs_script_t *script;
    ecs_script_function_node_t *node;
    ecs_vec_t refs;
    ecs_vec_t using;
} ecs_script_user_function_t;

void flecs_script_user_function_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result);

void flecs_script_user_function_ctx_free(
    void *ctx);

int flecs_script_eval_function(
    ecs_script_eval_visitor_t *v,
    ecs_script_function_node_t *node);

void FlecsScriptMathPerlinImport(
    ecs_world_t *world);

#endif // FLECS_SCRIPT
#endif // FLECS_SCRIPT_PRIVATE_H
