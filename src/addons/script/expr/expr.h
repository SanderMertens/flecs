/**
 * @file addons/script/expr/expr.h
 * @brief Script expression support.
 */

#ifndef FLECS_EXPR_SCRIPT_H
#define FLECS_EXPR_SCRIPT_H

#include "stack.h"
#include "ast.h"
#include "visit.h"

int flecs_value_copy_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    const ecs_value_t *src);

int flecs_value_move_to(
    ecs_world_t *world,
    ecs_value_t *dst,
    ecs_value_t *src);

int flecs_value_binary(
    const ecs_script_t *script,
    const ecs_value_t *left,
    const ecs_value_t *right,
    ecs_value_t *out,
    ecs_script_token_kind_t operator);

int flecs_value_unary(
    const ecs_script_t *script,
    const ecs_value_t *expr,
    ecs_value_t *out,
    ecs_script_token_kind_t operator);

const char* flecs_script_parse_expr(
    ecs_script_parser_t *parser,
    const char *pos,
    ecs_script_token_kind_t left_oper,
    ecs_expr_node_t **out);

const char* flecs_script_parse_initializer(
    ecs_script_parser_t *parser,
    const char *pos,
    char until,
    ecs_expr_initializer_t **node_out);

void flecs_expr_to_str_buf(
    const ecs_world_t *world,
    const ecs_expr_node_t *expr,
    ecs_strbuf_t *buf,
    bool colors);

#define ECS_VALUE_GET(value, T) (*(T*)(value)->ptr)

#define ECS_BOP(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, R) = ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T)

#define ECS_BOP_COND(left, right, result, op, R, T)\
    ECS_VALUE_GET(result, ecs_bool_t) = ECS_VALUE_GET(left, T) op ECS_VALUE_GET(right, T)

/* Unsigned operations */
#define ECS_BINARY_UINT_OPS(left, right, result, op, OP)\
    if ((left)->type == ecs_id(ecs_u64_t)) { \
        OP(left, right, result, op, ecs_u64_t, ecs_u64_t);\
    } else if ((left)->type == ecs_id(ecs_u32_t)) { \
        OP(left, right, result, op, ecs_u32_t, ecs_u32_t);\
    } else if ((left)->type == ecs_id(ecs_u16_t)) { \
        OP(left, right, result, op, ecs_u16_t, ecs_u16_t);\
    } else if ((left)->type == ecs_id(ecs_u8_t)) { \
        OP(left, right, result, op, ecs_u8_t, ecs_u8_t);\
    }

/* Unsigned + signed operations */
#define ECS_BINARY_INT_OPS(left, right, result, op, OP)\
    ECS_BINARY_UINT_OPS(left, right, result, op, OP)\
     else if ((left)->type == ecs_id(ecs_i64_t)) { \
        OP(left, right, result, op, ecs_i64_t, ecs_i64_t);\
    } else if ((left)->type == ecs_id(ecs_i32_t)) { \
        OP(left, right, result, op, ecs_i32_t, ecs_i32_t);\
    } else if ((left)->type == ecs_id(ecs_i16_t)) { \
        OP(left, right, result, op, ecs_i16_t, ecs_i16_t);\
    } else if ((left)->type == ecs_id(ecs_i8_t)) { \
        OP(left, right, result, op, ecs_i8_t, ecs_i8_t);\
    }

/* Unsigned + signed + floating point operations */
#define ECS_BINARY_NUMBER_OPS(left, right, result, op, OP)\
    ECS_BINARY_INT_OPS(left, right, result, op, OP)\
      else if ((left)->type == ecs_id(ecs_f64_t)) { \
        OP(left, right, result, op, ecs_f64_t, ecs_f64_t);\
    } else if ((left)->type == ecs_id(ecs_f32_t)) { \
        OP(left, right, result, op, ecs_f32_t, ecs_f32_t);\
    }


/* Combinations + error checking */

#define ECS_BINARY_INT_OP(left, right, result, op)\
    ECS_BINARY_INT_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_UINT_OP(left, right, result, op)\
    ECS_BINARY_UINT_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_OP(left, right, result, op)\
    ECS_BINARY_NUMBER_OPS(left, right, result, op, ECS_BOP) else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_EQ_OP(left, right, result, op)\
    ECS_BINARY_INT_OPS(left, right, result, op, ECS_BOP_COND)\
      else if ((left)->type == ecs_id(ecs_char_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if ((left)->type == ecs_id(ecs_u8_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if ((left)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_COND_OP(left, right, result, op)\
    ECS_BINARY_NUMBER_OPS(left, right, result, op, ECS_BOP_COND)\
      else if ((left)->type == ecs_id(ecs_char_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_char_t);\
    } else if ((left)->type == ecs_id(ecs_u8_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_u8_t);\
    } else if ((left)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#define ECS_BINARY_BOOL_OP(left, right, result, op)\
    if ((left)->type == ecs_id(ecs_bool_t)) { \
        ECS_BOP_COND(left, right, result, op, ecs_bool_t, ecs_bool_t);\
    } else {\
        ecs_abort(ECS_INTERNAL_ERROR, "unexpected type in binary expression");\
    }

#endif
