/**
 * @file addons/parser/parser.h
 * @brief Parser addon.
 */

#ifndef FLECS_PARSER_H
#define FLECS_PARSER_H

#include "../../private_api.h"

typedef struct ecs_script_impl_t ecs_script_impl_t;
typedef struct ecs_script_scope_t ecs_script_scope_t;

typedef struct ecs_parser_t {
    const char *name;
    const char *code;

    const char *pos;
    char *token_cur;
    char *token_keep;
    bool significant_newline;
    bool merge_variable_members;

    ecs_world_t *world;

    /* For script parser */
    ecs_script_impl_t *script;
    ecs_script_scope_t *scope;

    /* For term parser */
    ecs_term_t *term;
    ecs_oper_kind_t extra_oper;
    ecs_term_ref_t *extra_args;
} ecs_parser_t;

#include "tokenizer.h"

#endif
