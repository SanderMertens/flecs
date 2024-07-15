/**
 * @file addons/script/script.h
 * @brief Flecs script implementation.
 */

#ifndef FLECS_SCRIPT_PRIVATE_H
#define FLECS_SCRIPT_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_SCRIPT

#include <ctype.h>

typedef struct ecs_script_scope_t ecs_script_scope_t;
typedef struct ecs_script_entity_t ecs_script_entity_t;

typedef struct ecs_script_impl_t {
    ecs_script_t pub;
    ecs_allocator_t allocator;
    ecs_script_scope_t *root;
    char *token_buffer;
    int32_t token_buffer_size;
    int32_t refcount;
} ecs_script_impl_t;

typedef struct ecs_script_parser_t ecs_script_parser_t;

#define flecs_script_impl(script) ((ecs_script_impl_t*)script)

#include "tokenizer.h"

struct ecs_script_parser_t {
    ecs_script_impl_t *script;
    ecs_script_scope_t *scope;
    const char *pos;
    char *token_cur;
    char *token_keep;
    bool significant_newline;

    /* For term parser */
    ecs_term_t *term;
    ecs_oper_kind_t extra_oper;
    ecs_term_ref_t *extra_args;
};

#include "ast.h"
#include "visit.h"
#include "visit_eval.h"

struct ecs_script_template_t {
    /* Template handle */
    ecs_entity_t entity;

    /* Template AST node */
    ecs_script_template_node_t *node;

    /* Hoisted using statements */
    ecs_vec_t using_;

    /* Hoisted variables */
    ecs_script_vars_t *vars;

    /* Default values for props */
    ecs_vec_t prop_defaults;

    /* Type info for template component */
    const ecs_type_info_t *type_info;
};

ecs_script_t* flecs_script_new(
    ecs_world_t *world);

ecs_script_scope_t* flecs_script_scope_new(
    ecs_script_parser_t *parser);

int flecs_script_visit_free(
    ecs_script_t *script);

ecs_script_vars_t* flecs_script_vars_push(
    ecs_script_vars_t *parent,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator);

int flecs_terms_parse(
    ecs_script_t *script,
    ecs_term_t *terms,
    int32_t *term_count_out);

const char* flecs_id_parse(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_id_t *id);

const char* flecs_term_parse(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term,
    char *token_buffer);

#endif // FLECS_SCRIPT
#endif // FLECS_SCRIPT_PRIVATE_H
