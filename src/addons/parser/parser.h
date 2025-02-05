
/**
 * @file addons/script/parser.h
 * @brief Script parser.
 */

#ifndef FLECS_SCRIPT_PARSER_H
#define FLECS_SCRIPT_PARSER_H

#include "../../private_api.h"
#include <ctype.h>

typedef struct ecs_script_impl_t ecs_script_impl_t;
typedef struct ecs_script_scope_t ecs_script_scope_t;

typedef struct ecs_script_parser_t {
    ecs_script_impl_t *script;
    ecs_script_scope_t *scope;

    const char *name;
    const char *code;

    const char *pos;
    char *token_cur;
    char *token_keep;
    bool significant_newline;
    bool merge_variable_members;

    /* For term parser */
    ecs_term_t *term;
    ecs_oper_kind_t extra_oper;
    ecs_term_ref_t *extra_args;
} ecs_script_parser_t;

#include "tokenizer.h"

#endif
