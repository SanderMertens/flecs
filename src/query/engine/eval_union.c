/**
 * @file query/engine/eval_union.c
 * @brief Union relationship evaluation.
 */

#include "../../private_api.h"

static
bool flecs_query_union_with_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t rel,
    bool neq)
{
    ecs_query_union_ctx_t *op_ctx = flecs_op_ctx(ctx, union_);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    ecs_table_range_t range;
    ecs_table_t *table;
    if (!redo) {
        range = flecs_query_get_range(op, &op->src, EcsQuerySrc, ctx);
        table = range.table;
        if (!range.count) {
            range.count = ecs_table_count(table);
        }

        op_ctx->range = range;
        op_ctx->idr = flecs_id_record_get(ctx->world, ecs_pair(rel, EcsUnion));
        if (!op_ctx->idr) {
            return neq;
        }

        if (neq) {
            if (flecs_id_record_get_table(op_ctx->idr, table) != NULL) {
                /* If table has (R, Union) none match !(R, _) */
                return false;
            } else {
                /* If table doesn't have (R, Union) all match !(R, _) */
                return true;
            }
        }

        op_ctx->row = 0;
    } else {
        if (neq) {
            /* !(R, _) terms only can have a single result */
            return false;
        }

        range = op_ctx->range;
        table = range.table;
        op_ctx->row ++;
    }

next_row:
    if (op_ctx->row >= range.count) {
        /* Restore range */
        if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
            flecs_query_var_narrow_range(op->src.var, table, 
                op_ctx->range.offset, op_ctx->range.count, ctx);
        }
        return false;
    }

    ecs_entity_t e = flecs_table_entities_array(range.table)
        [range.offset + op_ctx->row];
    ecs_entity_t tgt = flecs_switch_get(op_ctx->idr->sparse, (uint32_t)e);
    if (!tgt) {
        op_ctx->row ++;
        goto next_row;
    }

    it->ids[field_index] = ecs_pair(rel, tgt);

    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        flecs_query_var_narrow_range(op->src.var, table, 
            range.offset + op_ctx->row, 1, ctx);
    }
    flecs_query_set_vars(op, it->ids[field_index], ctx);

    return true;
}

static
bool flecs_query_union_with_tgt(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t rel,
    ecs_entity_t tgt,
    bool neq)
{
    ecs_query_union_ctx_t *op_ctx = flecs_op_ctx(ctx, union_);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    ecs_table_range_t range;
    ecs_table_t *table;
    if (!redo) {
        range = flecs_query_get_range(op, &op->src, EcsQuerySrc, ctx);
        table = range.table;
        if (!range.count) {
            range.count = ecs_table_count(table);
        }

        op_ctx->range = range;
        op_ctx->idr = flecs_id_record_get(ctx->world, ecs_pair(rel, EcsUnion));
        if (!op_ctx->idr) {
            return false;
        }

        op_ctx->row = 0;
    } else {
        range = op_ctx->range;
        table = range.table;
        op_ctx->row ++;
    }

next_row:
    if (op_ctx->row >= range.count) {
        /* Restore range */
        if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
            flecs_query_var_narrow_range(op->src.var, table, 
                op_ctx->range.offset, op_ctx->range.count, ctx);
        }
        return false;
    }

    ecs_entity_t e = flecs_table_entities_array(range.table)
        [range.offset + op_ctx->row];
    ecs_entity_t e_tgt = flecs_switch_get(op_ctx->idr->sparse, (uint32_t)e);
    bool match = e_tgt == tgt;
    if (neq) {
        match = !match;
    }

    if (!match) {
        op_ctx->row ++;
        goto next_row;
    }

    it->ids[field_index] = ecs_pair(rel, tgt);
    
    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        flecs_query_var_narrow_range(op->src.var, table, 
            range.offset + op_ctx->row, 1, ctx);
    }

    flecs_query_set_vars(op, it->ids[field_index], ctx);

    return true;
}

bool flecs_query_union_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool neq)
{
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_entity_t rel = ECS_PAIR_FIRST(id);
    ecs_entity_t tgt = ecs_pair_second(ctx->world, id);

    if (tgt == EcsWildcard) {
        return flecs_query_union_with_wildcard(op, redo, ctx, rel, neq);
    } else {
        return flecs_query_union_with_tgt(op, redo, ctx, rel, tgt, neq);
    }
}

static
bool flecs_query_union_select_tgt(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t rel,
    ecs_entity_t tgt)
{
    ecs_query_union_ctx_t *op_ctx = flecs_op_ctx(ctx, union_);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!redo) {
        op_ctx->idr = flecs_id_record_get(ctx->world, ecs_pair(rel, EcsUnion));
        if (!op_ctx->idr) {
            return false;
        }

        op_ctx->cur = flecs_switch_first(op_ctx->idr->sparse, tgt);
    } else {
        op_ctx->cur = flecs_switch_next(op_ctx->idr->sparse, (uint32_t)op_ctx->cur);
    }

    if (!op_ctx->cur) {
        return false;
    }

    ecs_table_range_t range = flecs_range_from_entity(op_ctx->cur, ctx);
    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);
    flecs_query_set_vars(op, it->ids[field_index], ctx);

    it->ids[field_index] = ecs_pair(rel, tgt);

    return true;
}

static
bool flecs_query_union_select_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t rel)
{
    ecs_query_union_ctx_t *op_ctx = flecs_op_ctx(ctx, union_);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!redo) {
        op_ctx->idr = flecs_id_record_get(ctx->world, ecs_pair(rel, EcsUnion));
        if (!op_ctx->idr) {
            return false;
        }

        op_ctx->tgt_iter = flecs_switch_targets(op_ctx->idr->sparse);
        op_ctx->tgt = 0;
    }

next_tgt:
    if (!op_ctx->tgt) {
        if (!ecs_map_next(&op_ctx->tgt_iter)) {
            return false;
        }

        op_ctx->tgt = ecs_map_key(&op_ctx->tgt_iter);
        op_ctx->cur = 0;
        it->ids[field_index] = ecs_pair(rel, op_ctx->tgt);
    }

    if (!op_ctx->cur) {
        op_ctx->cur = flecs_switch_first(op_ctx->idr->sparse, op_ctx->tgt);
    } else {
        op_ctx->cur = flecs_switch_next(op_ctx->idr->sparse, (uint32_t)op_ctx->cur);
    }

    if (!op_ctx->cur) {
        op_ctx->tgt = 0;
        goto next_tgt;
    }

    ecs_table_range_t range = flecs_range_from_entity(op_ctx->cur, ctx);
    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);
    flecs_query_set_vars(op, it->ids[field_index], ctx);

    return true;
}

bool flecs_query_union_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_entity_t rel = ECS_PAIR_FIRST(id);
    ecs_entity_t tgt = ecs_pair_second(ctx->world, id);

    if (tgt == EcsWildcard) {
        return flecs_query_union_select_wildcard(op, redo, ctx, rel);
    } else {
        return flecs_query_union_select_tgt(op, redo, ctx, rel, tgt);
    }
}

bool flecs_query_union(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_union_with(op, redo, ctx, false);
    } else {
        return flecs_query_union_select(op, redo, ctx);
    }
}

bool flecs_query_union_neq(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_union_with(op, redo, ctx, true);
    } else {
        return false;
    }
}

static
void flecs_query_union_set_shared(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_id_record_t *idr = op_ctx->idr_with;
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t rel = ECS_PAIR_FIRST(idr->id);
    idr = flecs_id_record_get(ctx->world, ecs_pair(rel, EcsUnion));
    ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(idr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    int8_t field_index = op->field_index;
    ecs_iter_t *it = ctx->it;
    ecs_entity_t src = it->sources[field_index];
    ecs_entity_t tgt = flecs_switch_get(idr->sparse, (uint32_t)src);
    
    it->ids[field_index] = ecs_pair(rel, tgt);
}

bool flecs_query_union_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (!redo) {
            if (!flecs_query_up_with(op, redo, ctx)) {
                return false;
            }

            flecs_query_union_set_shared(op, ctx);
            return true;
        } else {
            return false;
        }
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectUp, FlecsQueryUpSelectUnion);
    }
}

bool flecs_query_union_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (redo) {
            goto next_for_union;
        }

next_for_self_up_with:
        if (!flecs_query_self_up_with(op, redo, ctx, false)) {
            return false;
        }

        int8_t field_index = op->field_index;
        ecs_iter_t *it = ctx->it;
        if (it->sources[field_index]) {
            flecs_query_union_set_shared(op, ctx);
            return true;
        }

next_for_union:
        if (!flecs_query_union_with(op, redo, ctx, false)) {
            goto next_for_self_up_with;
        }

        return true;
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectSelfUp, FlecsQueryUpSelectUnion);
    }
}
