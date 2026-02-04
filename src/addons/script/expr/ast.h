/**
 * @file addons/script/expr_ast.h
 * @brief Script expression AST.
 */

#ifndef FLECS_SCRIPT_EXPR_AST_H
#define FLECS_SCRIPT_EXPR_AST_H

#define FLECS_EXPR_SMALL_DATA_SIZE (24)

typedef enum ecs_expr_node_kind_t {
    EcsExprValue,
    EcsExprInterpolatedString,
    EcsExprInitializer,
    EcsExprEmptyInitializer,
    EcsExprUnary,
    EcsExprBinary,
    EcsExprIdentifier,
    EcsExprVariable,
    EcsExprGlobalVariable,
    EcsExprFunction,
    EcsExprMethod,
    EcsExprMember,
    EcsExprElement,
    EcsExprComponent,
    EcsExprCast,
    EcsExprCastNumber,
    EcsExprMatch,
    EcsExprNew
} ecs_expr_node_kind_t;

struct ecs_expr_node_t {
    ecs_expr_node_kind_t kind;
    ecs_entity_t type;
    const ecs_type_info_t *type_info;
    const char *pos;
};

typedef struct ecs_expr_value_node_t {
    ecs_expr_node_t node;
    void *ptr;
    ecs_expr_small_value_t storage;
} ecs_expr_value_node_t;

typedef struct ecs_expr_interpolated_string_t {
    ecs_expr_node_t node;
    char *value;              /* modified by parser */
    char *buffer;             /* for storing expr tokens */
    ecs_size_t buffer_size;
    ecs_vec_t fragments;      /* vec<char*> */
    ecs_vec_t expressions;    /* vec<ecs_expr_node_t*> */
} ecs_expr_interpolated_string_t;

typedef struct ecs_expr_initializer_element_t {
    const char *member;
    ecs_expr_node_t *value;
    uintptr_t offset;
    ecs_token_kind_t operator;
} ecs_expr_initializer_element_t;

typedef struct ecs_expr_initializer_t {
    ecs_expr_node_t node;
    ecs_vec_t elements;
    const ecs_type_info_t *type_info;
    bool is_collection;
    bool is_dynamic;
} ecs_expr_initializer_t;

typedef struct ecs_expr_variable_t {
    ecs_expr_node_t node;
    const char *name;
    ecs_value_t global_value; /* Only set for global variables */
    int32_t sp; /* For fast variable lookups */
} ecs_expr_variable_t;

typedef struct ecs_expr_identifier_t {
    ecs_expr_node_t node;
    const char *value;
    ecs_expr_node_t *expr;
} ecs_expr_identifier_t;

typedef struct ecs_expr_unary_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *expr;
    ecs_token_kind_t operator;
} ecs_expr_unary_t;

typedef struct ecs_expr_binary_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    ecs_expr_node_t *right;
    ecs_token_kind_t operator;

    /* For vector operations */
    ecs_entity_t vector_type;
    int32_t vector_count;
} ecs_expr_binary_t;

typedef struct ecs_expr_member_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    const char *member_name;
    uintptr_t offset;
} ecs_expr_member_t;

typedef struct ecs_expr_function_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    ecs_expr_initializer_t *args;
    const char *function_name;
    ecs_function_calldata_t calldata;
} ecs_expr_function_t;

typedef struct ecs_expr_element_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    ecs_expr_node_t *index;
    ecs_size_t elem_size;
} ecs_expr_element_t;

typedef struct ecs_expr_component_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *expr;
    ecs_id_t component;
} ecs_expr_component_t;

typedef struct ecs_expr_cast_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *expr;
} ecs_expr_cast_t;

typedef struct ecs_expr_match_element_t {
    ecs_expr_node_t *compare;
    ecs_expr_node_t *expr;
} ecs_expr_match_element_t;

typedef struct ecs_expr_match_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *expr;
    ecs_vec_t elements;
    ecs_expr_match_element_t any;
} ecs_expr_match_t;

typedef struct ecs_expr_new_t {
    ecs_expr_node_t node;
    ecs_script_entity_t *entity;
} ecs_expr_new_t;

ecs_expr_value_node_t* flecs_expr_value_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_entity_t type);

ecs_expr_variable_t* flecs_expr_variable_from(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    const char *name);

ecs_expr_value_node_t* flecs_expr_bool(
    ecs_parser_t *parser,
    bool value);

ecs_expr_value_node_t *flecs_expr_char(
    ecs_parser_t *parser,
    const char *value);

ecs_expr_value_node_t* flecs_expr_int(
    ecs_parser_t *parser,
    int64_t value);

ecs_expr_value_node_t* flecs_expr_uint(
    ecs_parser_t *parser,
    uint64_t value);

ecs_expr_value_node_t* flecs_expr_float(
    ecs_parser_t *parser,
    double value);

ecs_expr_value_node_t* flecs_expr_string(
    ecs_parser_t *parser,
    const char *value);

ecs_expr_interpolated_string_t* flecs_expr_interpolated_string(
    ecs_parser_t *parser,
    const char *value);

ecs_expr_value_node_t* flecs_expr_entity(
    ecs_parser_t *parser,
    ecs_entity_t value);

ecs_expr_initializer_t* flecs_expr_initializer(
    ecs_parser_t *parser);

ecs_expr_identifier_t* flecs_expr_identifier(
    ecs_parser_t *parser,
    const char *value);

ecs_expr_variable_t* flecs_expr_variable(
    ecs_parser_t *parser,
    const char *value);

ecs_expr_unary_t* flecs_expr_unary(
    ecs_parser_t *parser);

ecs_expr_binary_t* flecs_expr_binary(
    ecs_parser_t *parser);

ecs_expr_member_t* flecs_expr_member(
    ecs_parser_t *parser);

ecs_expr_function_t* flecs_expr_function(
    ecs_parser_t *parser);

ecs_expr_element_t* flecs_expr_element(
    ecs_parser_t *parser);

ecs_expr_match_t* flecs_expr_match(
    ecs_parser_t *parser);

ecs_expr_new_t* flecs_expr_new(
    ecs_parser_t *parser);

ecs_expr_cast_t* flecs_expr_cast(
    ecs_script_t *script,
    ecs_expr_node_t *node,
    ecs_entity_t type);

#endif
