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

typedef enum flecs_script_symbol_kind_t {
    FlecsScriptSymbolNone,
    FlecsScriptSymbolEntity,
    FlecsScriptSymbolEntitySlot,
    FlecsScriptSymbolVariable,
    FlecsScriptSymbolGlobalVariable
} flecs_script_symbol_kind_t;

typedef struct flecs_script_symbol_t {
    flecs_script_symbol_kind_t kind;
    ecs_entity_t entity;
    int32_t slot;
    int32_t sp;
} flecs_script_symbol_t;

typedef enum flecs_script_lookup_kind_t {
    FlecsScriptLookupEntity = 1,
    FlecsScriptLookupVariable = 2,
    FlecsScriptLookupAll = 3,
    FlecsScriptLookupDynamic = 4
} flecs_script_lookup_kind_t;

typedef enum flecs_script_unresolved_kind_t {
    FlecsScriptUnresolvedEntity,
    FlecsScriptUnresolvedComponent,
    FlecsScriptUnresolvedVariable
} flecs_script_unresolved_kind_t;

typedef struct ecs_script_unresolved_ref_t {
    const char *name;
    flecs_script_unresolved_kind_t kind;
    int32_t line;
    int32_t column;
} ecs_script_unresolved_ref_t;

typedef struct ecs_script_unresolved_component_ref_t {
    ecs_entity_t entity;
    ecs_id_t component;
    bool is_has;
    int32_t line;
    int32_t column;
} ecs_script_unresolved_component_ref_t;

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
    ecs_vec_t symbols;
    ecs_vec_t unresolved_refs;
    ecs_vec_t unresolved_component_refs;
    bool evaluating;
    bool compiled;
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
#include "eval/eval.h"
#include "reactivity/refs.h"
#include "reactivity/template.h"

ecs_script_t* flecs_script_new(
    ecs_world_t *world);

void flecs_script_pos_to_line_col(
    const char *code,
    const char *pos,
    int32_t *line,
    int32_t *column);

int flecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_script_runtime_t *eval_runtime);

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

void flecs_script_register_builtin_functions(
    ecs_world_t *world);

void flecs_function_import(
    ecs_world_t *world);

/* Returns the value of a global variable, which is either a const or a mut
 * variable. When component is provided it is set to the component that stores
 * the value, which tells const and mut variables apart. */
ecs_value_t flecs_script_global_var_get(
    const ecs_world_t *world,
    ecs_entity_t var,
    ecs_id_t *component);

ecs_entity_t flecs_script_vector_type(
    ecs_world_t *world,
    ecs_entity_t elem_type);

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

/* Script functions */
double flecs_lerp(
    double a,
    double b,
    double t);

void FlecsScriptMathPerlinImport(
    ecs_world_t *world);

#endif // FLECS_SCRIPT
#endif // FLECS_SCRIPT_PRIVATE_H
