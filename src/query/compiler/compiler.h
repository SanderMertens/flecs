/**
 * @file query/compiler/compiler.h
 * @brief Query compiler functions.
 */

#include "../types.h"

/* Compile query to list of operations */
int flecs_query_compile(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_impl_t *query);

/* Compile single term */
int flecs_query_compile_term(
    ecs_world_t *world,
    ecs_query_impl_t *query,
    ecs_term_t *term,
    ecs_flags64_t *populated,
    ecs_query_compile_ctx_t *ctx);

/* Compile term ref (first, second or src) */
void flecs_query_compile_term_ref(
    ecs_world_t *world,
    ecs_query_impl_t *query,
    ecs_query_op_t *op,
    ecs_term_ref_t *term_ref,
    ecs_query_ref_t *ref,
    ecs_flags8_t ref_kind,
    ecs_var_kind_t kind,
    ecs_query_compile_ctx_t *ctx,
    bool create_wildcard_vars);

/* Mark variable as written */
void flecs_query_write(
    ecs_var_id_t var_id,
    uint64_t *written);

/* Mark variable as written in compiler context */
void flecs_query_write_ctx(
    ecs_var_id_t var_id,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write);

/* Add operation to query plan */
ecs_query_lbl_t flecs_query_op_insert(
    ecs_query_op_t *op,
    ecs_query_compile_ctx_t *ctx);

/* Insert each instruction */
void flecs_query_insert_each(
    ecs_var_id_t tvar,
    ecs_var_id_t evar,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write);

/* Insert instruction that populates field */
void flecs_query_insert_populate(
    ecs_query_impl_t *query,
    ecs_query_compile_ctx_t *ctx,
    ecs_flags64_t populated);

/* Add discovered variable */
ecs_var_id_t flecs_query_add_var(
    ecs_query_impl_t *query,
    const char *name,
    ecs_vec_t *vars,
    ecs_var_kind_t kind);

/* Find variable by name/kind */
ecs_var_id_t flecs_query_find_var_id(
    const ecs_query_impl_t *query,
    const char *name,
    ecs_var_kind_t kind);

ecs_query_op_t* flecs_query_begin_block(
    ecs_query_op_kind_t kind,
    ecs_query_compile_ctx_t *ctx);

void flecs_query_end_block(
    ecs_query_compile_ctx_t *ctx,
    bool reset);

