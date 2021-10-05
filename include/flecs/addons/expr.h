/**
 * @file expr.h
 * @brief Expression parser addon.
 *
 * Simple API for parsing string expressions into (component) values.
 */

#ifdef FLECS_EXPR

#ifndef FLECS_EXPR_H
#define FLECS_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
const char* ecs_parse_expr(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t type,
    void *data_out);

#ifdef __cplusplus
}
#endif

#endif

#endif
