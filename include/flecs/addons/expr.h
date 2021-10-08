/**
 * @file expr.h
 * @brief Expression parser addon.
 *
 * Simple API for parsing string expressions into (component) values.
 */

#ifdef FLECS_EXPR

#ifndef FLECS_META
#define FLECS_META
#endif

#ifndef FLECS_PARSER
#define FLECS_PARSER
#endif

#include "meta.h"
#include "parser.h"

#ifndef FLECS_EXPR_H
#define FLECS_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
char* ecs_chresc(
    char *out, 
    char in, 
    char delimiter);

const char* ecs_chrparse(
    const char *in, 
    char *out);

FLECS_API
ecs_size_t ecs_stresc(
    char *out, 
    ecs_size_t n, 
    char delimiter, 
    const char *in);

FLECS_API
const char* ecs_parse_expr(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_entity_t type,
    void *data_out);

FLECS_API
char* ecs_ptr_to_expr(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data);

FLECS_API
int ecs_ptr_to_expr_buf(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *data,
    ecs_strbuf_t *buf_out);

FLECS_API
int ecs_primitive_to_expr_buf(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base, 
    ecs_strbuf_t *str);

#ifdef __cplusplus
}
#endif

#endif

#endif
