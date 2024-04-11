/**
 * @file addons/script/script.h
 * @brief Flecs script implementation.
 */

#ifndef FLECS_SCRIPT_PRIVATE_H
#define FLECS_SCRIPT_PRIVATE_H

#ifdef FLECS_SCRIPT

#include "../../private_api.h"
#include <ctype.h>

typedef struct ecs_script_scope_t ecs_script_scope_t;
typedef struct ecs_script_entity_t ecs_script_entity_t;

struct ecs_script_t {
    ecs_world_t *world;
    const char *name;
    const char *code;
    ecs_allocator_t allocator;
    ecs_script_scope_t *root;
    char *token_buffer;
};

typedef struct ecs_script_parser_t {
    ecs_script_t *script;
    ecs_script_scope_t *scope;
    const char *pos;
    char *token_cur;
} ecs_script_parser_t;

#include "tokenizer.h"
#include "ast.h"
#include "visit.h"
#include "vars.h"

#endif // FLECS_SCRIPT
#endif // FLECS_SCRIPT_PRIVATE_H
