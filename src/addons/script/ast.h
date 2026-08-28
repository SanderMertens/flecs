/**
 * @file addons/script/ast.h
 * @brief Script AST.
 */

#ifndef FLECS_SCRIPT_AST_H
#define FLECS_SCRIPT_AST_H

typedef enum ecs_script_node_kind_t {
    EcsAstScope,
    EcsAstTag,
    EcsAstComponent,
    EcsAstWithTag,
    EcsAstWithComponent,
    EcsAstWith,
    EcsAstUsing,
    EcsAstModule,
    EcsAstAnnotation,
    EcsAstTemplate,
    EcsAstProp,
    EcsAstMut,
    EcsAstConst,
    EcsAstExportConst,
    EcsAstExportMut,
    EcsAstEntity,
    EcsAstPairScope,
    EcsAstIf,
    EcsAstFor,
    EcsAstInclude,
    EcsAstFunction,
    EcsAstAwait,
    EcsAstTry,
    EcsAstContinue
} ecs_script_node_kind_t;

typedef struct ecs_script_node_t {
    ecs_script_node_kind_t kind;
    const char *pos;
    uint64_t input;
    uint64_t direct_input;

    bool skip;
} ecs_script_node_t;

struct ecs_script_scope_t {
    ecs_script_node_t node;
    ecs_vec_t stmts;
    ecs_script_scope_t *parent;
    int32_t scope_slot;

    /* Array with component ids that are added in scope. Used to limit
     * archetype moves. */
    ecs_vec_t components; /* vec<ecs_id_t> */
};

typedef struct ecs_script_id_t {
    const char *first;
    const char *second;
    ecs_id_t flag;
    ecs_id_t eval;
    ecs_entity_t first_eval;
    ecs_entity_t second_eval;

    /* If first or second refer to a variable, these are the cached variable 
     * stack pointers so we don't have to lookup variables by name. */
    int32_t first_sp; 
    int32_t second_sp;
    int32_t first_symbol;
    int32_t second_symbol;

    /* In case first/second are specified as interpolated strings. */
    ecs_expr_node_t *first_expr;
    ecs_expr_node_t *second_expr;

    /* If true, the lookup result for this id cannot be cached. This is the case
     * for entities that are defined inside of templates, which have different
     * values for each instantiation. */
    bool dynamic;
} ecs_script_id_t;

typedef struct ecs_script_tag_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    int32_t component_slot;
} ecs_script_tag_t;

typedef struct ecs_script_component_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    ecs_expr_node_t *expr;
    int32_t component_slot;
    bool is_collection;
} ecs_script_component_t;

struct ecs_script_entity_t {
    ecs_script_node_t node;
    const char *kind;
    const char *name;
    bool name_is_var;
    bool kind_w_expr;
    bool non_fragmenting_parent;
    ecs_script_scope_t *scope;
    ecs_expr_node_t *name_expr;

    /* Entities created by "new" expressions are hoisted into the statement list
     * of the scope that contains the statement with the expression. This makes
     * it possible for the scope mark/cleanup logic to find them. When set, this
     * is the statement that owns the expression. */
    ecs_script_node_t *hoisted_by;

    ecs_entity_t eval;
    ecs_entity_t eval_kind;
    int32_t kind_symbol;
    int32_t kind_sp;
    int32_t symbol;
    bool is_type;
};

typedef struct ecs_script_with_t {
    ecs_script_node_t node;
    ecs_script_scope_t *expressions;
    ecs_script_scope_t *scope;
} ecs_script_with_t;

typedef struct ecs_script_inherit_t {
    ecs_script_node_t node;
    ecs_script_scope_t *base_list;
} ecs_script_inherit_t;

typedef struct ecs_script_pair_scope_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    ecs_script_scope_t *scope;
} ecs_script_pair_scope_t;

typedef struct ecs_script_using_t {
    ecs_script_node_t node;
    const char *name;
    ecs_entity_t eval;
} ecs_script_using_t;

typedef struct ecs_script_module_t {
    ecs_script_node_t node;
    const char *name;
    ecs_entity_t eval;
    int32_t symbol;
} ecs_script_module_t;

typedef struct ecs_script_annot_t {
    ecs_script_node_t node;
    const char *name;
    const char *expr;
} ecs_script_annot_t;

typedef struct ecs_script_template_node_t {
    ecs_script_node_t node;
    const char *name;
    ecs_script_scope_t* scope;
    int32_t symbol;
    int32_t symbol_offset;
    int32_t symbol_count;
    int32_t root_symbol;
} ecs_script_template_node_t;

typedef struct ecs_script_var_node_t {
    ecs_script_node_t node;
    const char *name;
    const char *type;
    ecs_expr_node_t *expr;
    ecs_entity_t eval_type;
    int32_t sp;
    int32_t symbol;
    bool is_await;
} ecs_script_var_node_t;

typedef struct ecs_script_await_t {
    ecs_script_node_t node;
    ecs_expr_node_t *expr;
} ecs_script_await_t;

typedef struct ecs_script_continue_t {
    ecs_script_node_t node;
} ecs_script_continue_t;

typedef struct ecs_script_catch_t {
    const char *error; /* Error entity to catch. NULL for catch-all clause. */
    ecs_script_scope_t *scope;
    ecs_entity_t eval_error;
    int32_t error_symbol;
} ecs_script_catch_t;

typedef struct ecs_script_try_t {
    ecs_script_node_t node;
    ecs_script_scope_t *try_scope;
    ecs_vec_t catches; /* vec<ecs_script_catch_t> */
} ecs_script_try_t;

typedef struct ecs_script_if_t {
    ecs_script_node_t node;
    ecs_script_scope_t *if_true;
    ecs_script_scope_t *if_false;
    ecs_expr_node_t *expr;
} ecs_script_if_t;

typedef struct ecs_script_for_t {
    ecs_script_node_t node;
    const char *loop_vars[3];
    int32_t loop_var_sp[3];
    int32_t loop_var_count;
    ecs_expr_node_t *from;
    ecs_expr_node_t *to;
    ecs_expr_node_t *expr;
    ecs_script_scope_t *scope;
    int32_t for_slot;
} ecs_script_for_t;

typedef struct ecs_script_include_t {
    ecs_script_node_t node;
    const char *filename;
} ecs_script_include_t;

typedef struct ecs_script_fn_param_t {
    ecs_script_node_t node;
    const char *name;
    const char *type;
    ecs_entity_t eval_type;
    int32_t sp;
} ecs_script_fn_param_t;

typedef struct ecs_script_function_node_t {
    ecs_script_node_t node;
    ecs_script_node_t return_type_node;
    const char *name;
    const char *return_type;
    ecs_vec_t params;
    ecs_script_scope_t *body;
    ecs_expr_node_t *return_expr;
    ecs_entity_t eval_return_type;
    int32_t symbol;
} ecs_script_function_node_t;

#define ecs_script_node(kind, node)\
    ((ecs_script_##kind##_t*)node)

bool flecs_scope_is_empty(
    ecs_script_scope_t *scope);

/* Returns true for entity nodes that were hoisted into a scope by a "new"
 * expression. Hoisted nodes are evaluated by the expression that created them,
 * not by the statement list they are stored in. */
bool flecs_script_node_is_hoisted(
    const ecs_script_node_t *node);

/* Hoist entity node of a "new" expression into scope. */
void flecs_script_hoist_entity(
    ecs_script_impl_t *script,
    ecs_script_scope_t *scope,
    ecs_script_node_t *owner,
    ecs_script_entity_t *entity);

ecs_script_entity_t* flecs_script_insert_entity(
    ecs_parser_t *parser,
    const char *name);

ecs_script_pair_scope_t* flecs_script_insert_pair_scope(
    ecs_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_with_t* flecs_script_insert_with(
    ecs_parser_t *parser);

ecs_script_using_t* flecs_script_insert_using(
    ecs_parser_t *parser,
    const char *name);

ecs_script_module_t* flecs_script_insert_module(
    ecs_parser_t *parser,
    const char *name);

ecs_script_template_node_t* flecs_script_insert_template(
    ecs_parser_t *parser,
    const char *name);

ecs_script_annot_t* flecs_script_insert_annot(
    ecs_parser_t *parser,
    const char *name,
    const char *expr);

ecs_script_var_node_t* flecs_script_insert_var(
    ecs_parser_t *parser,
    const char *name);

ecs_script_await_t* flecs_script_insert_await(
    ecs_parser_t *parser);

ecs_script_continue_t* flecs_script_insert_continue(
    ecs_parser_t *parser);

ecs_script_try_t* flecs_script_insert_try(
    ecs_parser_t *parser);

ecs_script_catch_t* flecs_script_try_add_catch(
    ecs_parser_t *parser,
    ecs_script_try_t *stmt);

ecs_script_tag_t* flecs_script_insert_tag(
    ecs_parser_t *parser,
    const char *name);

ecs_script_tag_t* flecs_script_insert_pair_tag(
    ecs_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_component_t* flecs_script_insert_component(
    ecs_parser_t *parser,
    const char *name);

ecs_script_component_t* flecs_script_insert_pair_component(
    ecs_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_if_t* flecs_script_insert_if(
    ecs_parser_t *parser);

ecs_script_for_t* flecs_script_insert_for(
    ecs_parser_t *parser);

ecs_script_include_t* flecs_script_insert_include(
    ecs_parser_t *parser,
    const char *filename);

ecs_script_function_node_t* flecs_script_insert_function(
    ecs_parser_t *parser,
    const char *name);

#endif
