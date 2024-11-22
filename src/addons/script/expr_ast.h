/**
 * @file addons/script/expr_ast.h
 * @brief Script expression AST.
 */

#ifndef FLECS_SCRIPT_EXPR_AST_H
#define FLECS_SCRIPT_EXPR_AST_H

typedef enum ecs_expr_node_kind_t {
    EcsExprValue,
    EcsExprUnary,
    EcsExprBinary,
    EcsExprIdentifier,
    EcsExprVariable,
    EcsExprFunction,
    EcsExprMember,
    EcsExprElement,
    EcsExprCast
} ecs_expr_node_kind_t;

struct ecs_expr_node_t {
    ecs_expr_node_kind_t kind;
    ecs_entity_t type;
    const char *pos;
};

typedef struct ecs_expr_val_t {
    ecs_expr_node_t node;
    void *ptr;
    union {
        bool bool_;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        double f32;
        double f64;
        const char *string;
    } storage;
} ecs_expr_val_t;

typedef struct ecs_expr_identifier_t {
    ecs_expr_node_t node;
    const char *value;
    ecs_entity_t id;
} ecs_expr_identifier_t;

typedef struct ecs_expr_variable_t {
    ecs_expr_node_t node;
    const char *value;
} ecs_expr_variable_t;

typedef struct ecs_expr_binary_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    ecs_expr_node_t *right;
    ecs_script_token_kind_t operator;
} ecs_expr_binary_t;

typedef struct ecs_expr_member_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    const char *member_name;
} ecs_expr_member_t;

typedef struct ecs_expr_element_t {
    ecs_expr_node_t node;
    ecs_expr_node_t *left;
    ecs_expr_node_t *index;
} ecs_expr_element_t;

ecs_expr_val_t* flecs_expr_bool(
    ecs_script_parser_t *parser,
    bool value);

ecs_expr_val_t* flecs_expr_int(
    ecs_script_parser_t *parser,
    int64_t value);

ecs_expr_val_t* flecs_expr_uint(
    ecs_script_parser_t *parser,
    uint64_t value);

ecs_expr_val_t* flecs_expr_float(
    ecs_script_parser_t *parser,
    double value);

ecs_expr_val_t* flecs_expr_string(
    ecs_script_parser_t *parser,
    const char *value);

ecs_expr_identifier_t* flecs_expr_identifier(
    ecs_script_parser_t *parser,
    const char *value);

ecs_expr_variable_t* flecs_expr_variable(
    ecs_script_parser_t *parser,
    const char *value);

ecs_expr_binary_t* flecs_expr_binary(
    ecs_script_parser_t *parser);

ecs_expr_member_t* flecs_expr_member(
    ecs_script_parser_t *parser);

ecs_expr_element_t* flecs_expr_element(
    ecs_script_parser_t *parser);

#endif
