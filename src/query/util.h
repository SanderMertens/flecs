/**
 * @file query/util.h
 * @brief Query utility functions: variable state, flag helpers, term introspection.
 */

#include "types.h"

/* Helper type for passing around context required for error messages */
typedef struct {
    const ecs_world_t *world;
    const ecs_query_desc_t *desc;
    ecs_query_t *query;
    ecs_term_t *term;
    int32_t term_index;
} ecs_query_validator_ctx_t;

int flecs_term_finalize(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx);

ecs_query_lbl_t flecs_itolbl(
    int64_t val);

ecs_var_id_t flecs_itovar(
    int64_t val);

ecs_var_id_t flecs_utovar(
    uint64_t val);

const char* flecs_term_ref_var_name(
    ecs_term_ref_t *ref);

bool flecs_term_ref_is_wildcard(
    ecs_term_ref_t *ref);

bool flecs_term_is_builtin_pred(
    ecs_term_t *term);

bool flecs_term_is_fixed_id(
    ecs_query_t *q,
    ecs_term_t *term);

bool flecs_term_is_or(
    const ecs_query_t *q,
    const ecs_term_t *term);

ecs_flags16_t flecs_query_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind);

bool flecs_query_is_written(
    ecs_var_id_t var_id,
    uint64_t written);

bool flecs_ref_is_written(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written);

ecs_allocator_t* flecs_query_get_allocator(
    const ecs_iter_t *it);

const char* flecs_query_op_str(
    uint16_t kind);

void flecs_term_to_buf(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf,
    int32_t t);

void flecs_query_apply_iter_flags(
    ecs_iter_t *it,
    const ecs_query_t *query);

ecs_id_t flecs_query_iter_set_id(
    ecs_iter_t *it,
    int8_t field,
    ecs_id_t id);

