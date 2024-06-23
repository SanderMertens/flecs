/**
 * @file query/util.h
 * @brief Utility functions
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

/* Convert integer to label */
ecs_query_lbl_t flecs_itolbl(
    int64_t val);

/* Convert integer to variable id */
ecs_var_id_t flecs_itovar(
    int64_t val);

/* Convert unsigned integer to variable id */
ecs_var_id_t flecs_utovar(
    uint64_t val);

/* Get name for term ref */
const char* flecs_term_ref_var_name(
    ecs_term_ref_t *ref);

/* Is term ref wildcard */
bool flecs_term_ref_is_wildcard(
    ecs_term_ref_t *ref);

/* Does term use builtin predicates (eq, neq, ...)*/
bool flecs_term_is_builtin_pred(
    ecs_term_t *term);

/* Does term have fixed id */
bool flecs_term_is_fixed_id(
    ecs_query_t *q,
    ecs_term_t *term);

/* Is term part of OR chain */
bool flecs_term_is_or(
    const ecs_query_t *q,
    const ecs_term_t *term);

/* Get ref flags (IsEntity) or IsVar) for ref (Src, First, Second) */
ecs_flags16_t flecs_query_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind);

/* Check if variable is written */
bool flecs_query_is_written(
    ecs_var_id_t var_id,
    uint64_t written);

/* Check if ref is written (calls flecs_query_is_written)*/
bool flecs_ref_is_written(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written);

/* Get allocator from iterator */
ecs_allocator_t* flecs_query_get_allocator(
    const ecs_iter_t *it);

/* Convert instruction kind to string */
const char* flecs_query_op_str(
    uint16_t kind);

/* Convert term to string */
void flecs_term_to_buf(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf,
    int32_t t);
