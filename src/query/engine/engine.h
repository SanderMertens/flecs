/**
 * @file query/engine/engine.h
 * @brief Query engine functions.
 */

#include "cache.h"
#include "cache_iter.h"
#include "change_detection.h"
#include "trav_cache.h"
#include "trivial_iter.h"

/* Query evaluation utilities */

void flecs_query_set_iter_this(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx);

ecs_query_op_ctx_t* flecs_op_ctx_(
    const ecs_query_run_ctx_t *ctx);

#define flecs_op_ctx(ctx, op_kind) (&flecs_op_ctx_(ctx)->is.op_kind)

void flecs_reset_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index);

void flecs_set_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index);

ecs_table_range_t flecs_range_from_entity(
    ecs_entity_t e,
    const ecs_query_run_ctx_t *ctx);

ecs_table_range_t flecs_query_var_get_range(
    int32_t var_id,
    const ecs_query_run_ctx_t *ctx);

ecs_table_t* flecs_query_var_get_table(
    int32_t var_id,
    const ecs_query_run_ctx_t *ctx);

ecs_table_t* flecs_query_get_table(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx);

ecs_table_range_t flecs_query_get_range(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx);

ecs_entity_t flecs_query_var_get_entity(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_var_reset(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_var_set_range(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_var_narrow_range(
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_var_set_entity(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_entity_t entity,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_set_vars(
    const ecs_query_op_t *op,
    ecs_id_t id,
    const ecs_query_run_ctx_t *ctx);

ecs_table_range_t flecs_get_ref_range(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx);

ecs_entity_t flecs_get_ref_entity(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx);

ecs_id_t flecs_query_op_get_id_w_written(
    const ecs_query_op_t *op,
    uint64_t written,
    const ecs_query_run_ctx_t *ctx);

ecs_id_t flecs_query_op_get_id(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx);

int16_t flecs_query_next_column(
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column);

void flecs_query_it_set_column(
    ecs_iter_t *it,
    int32_t field_index,
    int32_t column);

ecs_id_t flecs_query_it_set_id(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t field_index,
    int32_t column);

void flecs_query_set_match(
    const ecs_query_op_t *op,
    ecs_table_t *table,
    int32_t column,
    const ecs_query_run_ctx_t *ctx);

void flecs_query_set_trav_match(
    const ecs_query_op_t *op,
    int32_t column,
    ecs_entity_t trav,
    ecs_entity_t second,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_table_filter(
    ecs_table_t *table,
    ecs_query_lbl_t other,
    ecs_flags32_t filter_mask);

void flecs_query_populate_field_from_range(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    int32_t index);

bool flecs_query_setids(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_run_until(
    bool redo,
    ecs_query_run_ctx_t *ctx,
    const ecs_query_op_t *ops,
    ecs_query_lbl_t first,
    ecs_query_lbl_t cur,
    int32_t last);


/* Select evaluation */

bool flecs_query_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_select_w_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_id_t id,
    ecs_flags32_t filter_mask);

typedef enum ecs_query_up_select_trav_kind_t {
    FlecsQueryUpSelectUp,
    FlecsQueryUpSelectSelfUp
} ecs_query_up_select_trav_kind_t;

typedef enum ecs_query_up_select_kind_t {
    FlecsQueryUpSelectDefault,
    FlecsQueryUpSelectId,
    FlecsQueryUpSelectUnion
} ecs_query_up_select_kind_t;

bool flecs_query_up_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_select_trav_kind_t trav_kind,
    ecs_query_up_select_kind_t kind);

bool flecs_query_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_self_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool id_only);


/* Populate data fields */

bool flecs_query_populate(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_populate_self(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_populate_sparse(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);


/* Union evaluation */

bool flecs_query_union_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_union(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_union_neq(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_union_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool neq);

bool flecs_query_union_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_union_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);


/* Toggle evaluation*/

bool flecs_query_toggle(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_toggle_option(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);


/* Equality predicate evaluation */

bool flecs_query_pred_eq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_pred_neq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_pred_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_pred_eq_name(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_pred_neq_w_range(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_table_range_t r);

bool flecs_query_pred_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_pred_neq_name(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);


/* Component member evaluation */

bool flecs_query_member_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_member_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);


/* Transitive relationship traversal */

bool flecs_query_trav(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx);

