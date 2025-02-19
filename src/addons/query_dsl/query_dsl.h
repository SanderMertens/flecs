/**
 * @file addons/query_dsl/query_dsl.h
 * @brief Query DSL parser addon.
 */

#ifndef FLECS_QUERY_DSL_H
#define FLECS_QUERY_DSL_H

#include "../../private_api.h"

int flecs_terms_parse(
    ecs_world_t *world,
    const char *name,
    const char *code,
    char *token_buffer,
    ecs_term_t *terms,
    int32_t *term_count_out);

const char* flecs_term_parse(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    char *token_buffer,
    ecs_term_t *term);

const char* flecs_id_parse(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_id_t *id);

#endif
