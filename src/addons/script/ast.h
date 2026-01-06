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
    EcsAstDefaultComponent,
    EcsAstVarComponent,
    EcsAstWithVar,
    EcsAstWithTag,
    EcsAstWithComponent,
    EcsAstWith,
    EcsAstUsing,
    EcsAstModule,
    EcsAstAnnotation,
    EcsAstTemplate,
    EcsAstProp,
    EcsAstConst,
    EcsAstExportConst,
    EcsAstEntity,
    EcsAstPairScope,
    EcsAstIf,
    EcsAstFor
} ecs_script_node_kind_t;

typedef struct ecs_script_node_t {
    ecs_script_node_kind_t kind;
    const char *pos;
} ecs_script_node_t;

struct ecs_script_scope_t {
    ecs_script_node_t node;
    ecs_vec_t stmts;
    ecs_script_scope_t *parent;
    ecs_id_t default_component_eval;

    /* Array with component ids that are added in scope. Used to limit
     * archetype moves. */
    ecs_vec_t components; /* vec<ecs_id_t> */
};

typedef struct ecs_script_id_t {
    const char *first;
    const char *second;
    ecs_id_t flag;
    ecs_id_t eval;

    /* If first or second refer to a variable, these are the cached variable 
     * stack pointers so we don't have to lookup variables by name. */
    int32_t first_sp; 
    int32_t second_sp;

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
} ecs_script_tag_t;

typedef struct ecs_script_component_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    ecs_expr_node_t *expr;
    ecs_value_t eval;
    bool is_collection;
} ecs_script_component_t;

typedef struct ecs_script_default_component_t {
    ecs_script_node_t node;
    ecs_expr_node_t *expr;
    ecs_value_t eval;
} ecs_script_default_component_t;

typedef struct ecs_script_var_component_t {
    ecs_script_node_t node;
    const char *name;
    int32_t sp;
} ecs_script_var_component_t;

struct ecs_script_entity_t {
    ecs_script_node_t node;
    const char *kind;
    const char *name;
    bool name_is_var;
    bool kind_w_expr;
    bool non_fragmenting_parent;
    ecs_script_scope_t *scope;
    ecs_expr_node_t *name_expr;

    /* Populated during eval */
    ecs_script_entity_t *parent;
    ecs_entity_t eval;
    ecs_entity_t eval_kind;
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
} ecs_script_using_t;

typedef struct ecs_script_module_t {
    ecs_script_node_t node;
    const char *name;
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
} ecs_script_template_node_t;

typedef struct ecs_script_var_node_t {
    ecs_script_node_t node;
    const char *name;
    const char *type;
    ecs_expr_node_t *expr;
} ecs_script_var_node_t;

typedef struct ecs_script_if_t {
    ecs_script_node_t node;
    ecs_script_scope_t *if_true;
    ecs_script_scope_t *if_false;
    ecs_expr_node_t *expr;
} ecs_script_if_t;

typedef struct ecs_script_for_range_t {
    ecs_script_node_t node;
    const char *loop_var;
    ecs_expr_node_t *from;
    ecs_expr_node_t *to;
    ecs_script_scope_t *scope;
} ecs_script_for_range_t;

#define ecs_script_node(kind, node)\
    ((ecs_script_##kind##_t*)node)

bool flecs_scope_is_empty(
    ecs_script_scope_t *scope);

ecs_script_scope_t* flecs_script_insert_scope(
    ecs_parser_t *parser);

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

ecs_script_default_component_t* flecs_script_insert_default_component(
    ecs_parser_t *parser);

ecs_script_var_component_t* flecs_script_insert_var_component(
    ecs_parser_t *parser,
    const char *name);

ecs_script_if_t* flecs_script_insert_if(
    ecs_parser_t *parser);

ecs_script_for_range_t* flecs_script_insert_for_range(
    ecs_parser_t *parser);

#endif
