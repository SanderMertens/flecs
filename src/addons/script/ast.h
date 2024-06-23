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
    EcsAstEntity,
    EcsAstPairScope,
    EcsAstIf,
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
};

typedef struct ecs_script_id_t {
    const char *first;
    const char *second;
    ecs_id_t flag;
    ecs_id_t eval;
} ecs_script_id_t;

typedef struct ecs_script_tag_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
} ecs_script_tag_t;

typedef struct ecs_script_component_t {
    ecs_script_node_t node;
    ecs_script_id_t id;
    const char *expr;
    ecs_value_t eval;
    bool is_collection;
} ecs_script_component_t;

typedef struct ecs_script_default_component_t {
    ecs_script_node_t node;
    const char *expr;
    ecs_value_t eval;
} ecs_script_default_component_t;

typedef struct ecs_script_var_component_t {
    ecs_script_node_t node;
    const char *name;
} ecs_script_var_component_t;

struct ecs_script_entity_t {
    ecs_script_node_t node;
    const char *kind;
    const char *name;
    bool name_is_var;
    bool kind_w_expr;
    ecs_script_scope_t *scope;

    // Populated during eval
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
    const char *expr;
} ecs_script_var_node_t;

typedef struct ecs_script_if_t {
    ecs_script_node_t node;
    ecs_script_scope_t *if_true;
    ecs_script_scope_t *if_false;
    const char *expr;
} ecs_script_if_t;

#define ecs_script_node(kind, node)\
    ((ecs_script_##kind##_t*)node)

bool flecs_scope_is_empty(
    ecs_script_scope_t *scope);

ecs_script_scope_t* flecs_script_insert_scope(
    ecs_script_parser_t *parser);

ecs_script_entity_t* flecs_script_insert_entity(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_pair_scope_t* flecs_script_insert_pair_scope(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_with_t* flecs_script_insert_with(
    ecs_script_parser_t *parser);

ecs_script_using_t* flecs_script_insert_using(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_module_t* flecs_script_insert_module(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_template_node_t* flecs_script_insert_template(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_annot_t* flecs_script_insert_annot(
    ecs_script_parser_t *parser,
    const char *name,
    const char *expr);

ecs_script_var_node_t* flecs_script_insert_var(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_tag_t* flecs_script_insert_tag(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_tag_t* flecs_script_insert_pair_tag(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_component_t* flecs_script_insert_component(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_component_t* flecs_script_insert_pair_component(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second);

ecs_script_default_component_t* flecs_script_insert_default_component(
    ecs_script_parser_t *parser);

ecs_script_var_component_t* flecs_script_insert_var_component(
    ecs_script_parser_t *parser,
    const char *name);

ecs_script_if_t* flecs_script_insert_if(
    ecs_script_parser_t *parser);

#endif
