/**
 * @file json.h
 * @brief JSON parser addon.
 *
 * Simple API for parsing JSON string expressions into (component) values.
 */

#ifdef FLECS_JSON

#ifndef FLECS_EXPR
#define FLECS_EXPR
#endif

#include "expr.h"

#ifndef FLECS_JSON_H
#define FLECS_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
const char* ecs_parse_json(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t type,
    void *data_out);

FLECS_API
char* ecs_ptr_to_json(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

FLECS_API
int ecs_ptr_to_json_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf_out);

#ifdef __cplusplus
}
#endif

#endif

#endif
