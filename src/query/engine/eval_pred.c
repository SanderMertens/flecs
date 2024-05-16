/**
 * @file query/engine/eval_pred.c
 * @brief Equality predicate evaluation.
 */

#include "../../private_api.h"

static
const char* flecs_query_name_arg(
    const ecs_query_op_t *op,
    ecs_query_run_ctx_t *ctx)
{
    int8_t term_index = op->term_index;
    const ecs_term_t *term = &ctx->query->pub.terms[term_index];
    return term->second.name;
}

static
bool flecs_query_compare_range(
    const ecs_table_range_t *l,
    const ecs_table_range_t *r)
{
    if (l->table != r->table) {
        return false;
    }

    if (l->count) {
        int32_t l_end = l->offset + l->count;
        int32_t r_end = r->offset + r->count;
        if (r->offset < l->offset) {
            return false;
        }
        if (r_end > l_end) {
            return false;
        }
    } else {
        /* Entire table is matched */
    }

    return true;
}

static
bool flecs_query_pred_eq_w_range(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_table_range_t r)
{
    if (redo) {
        return false;
    }

    uint64_t written = ctx->written[ctx->op_index];
    ecs_var_id_t src_var = op->src.var;
    if (!(written & (1ull << src_var))) {
        /* left = unknown, right = known. Assign right-hand value to left */
        ecs_var_id_t l = src_var;
        ctx->vars[l].range = r;
        if (r.count == 1) {
            ctx->vars[l].entity = ecs_vec_get_t(&r.table->data.entities, 
                ecs_entity_t, r.offset)[0];
        }
        return true;
    } else {
        ecs_table_range_t l = flecs_query_get_range(
            op, &op->src, EcsQuerySrc, ctx);

        if (!flecs_query_compare_range(&l, &r)) {
            return false;
        }

        ctx->vars[src_var].range.offset = r.offset;
        ctx->vars[src_var].range.count = r.count;
        return true;
    }
}

bool flecs_query_pred_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsQuerySecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized eq operand");

    ecs_table_range_t r = flecs_query_get_range(
        op, &op->second, EcsQuerySecond, ctx);
    return flecs_query_pred_eq_w_range(op, redo, ctx, r);
}

bool flecs_query_pred_eq_name(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    const char *name = flecs_query_name_arg(op, ctx);
    ecs_entity_t e = ecs_lookup(ctx->world, name);
    if (!e) {
        /* Entity doesn't exist */
        return false;
    }

    ecs_table_range_t r = flecs_range_from_entity(e, ctx);
    return flecs_query_pred_eq_w_range(op, redo, ctx, r);
}

bool flecs_query_pred_neq_w_range(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_table_range_t r)
{
    ecs_query_eq_ctx_t *op_ctx = flecs_op_ctx(ctx, eq);
    ecs_var_id_t src_var = op->src.var;
    ecs_table_range_t l = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);

    /* If tables don't match, neq always returns once */
    if (l.table != r.table) {
        return true && !redo;
    }

    int32_t l_offset;
    int32_t l_count;
    if (!redo) {
        /* Make sure we're working with the correct table count */
        if (!l.count && l.table) {
            l.count = ecs_table_count(l.table);
        }

        l_offset = l.offset;
        l_count = l.count;

        /* Cache old value */
        op_ctx->range = l;
    } else {
        l_offset = op_ctx->range.offset;
        l_count = op_ctx->range.count;
    }

    /* If the table matches, a Neq returns twice: once for the slice before the
     * excluded slice, once for the slice after the excluded slice. If the right
     * hand range starts & overlaps with the left hand range, there is only
     * one slice. */
    ecs_var_t *var = &ctx->vars[src_var];
    if (!redo && r.offset > l_offset) {
        int32_t end = r.offset;
        if (end > l_count) {
            end = l_count;
        }

        /* Return first slice */
        var->range.table = l.table;
        var->range.offset = l_offset;
        var->range.count = end - l_offset;
        op_ctx->redo = false;
        return true;
    } else if (!op_ctx->redo) {
        int32_t l_end = op_ctx->range.offset + l_count;
        int32_t r_end = r.offset + r.count;

        if (l_end <= r_end) {
            /* If end of existing range falls inside the excluded range, there's
             * nothing more to return */
            var->range = l;
            return false;
        }

        /* Return second slice */
        var->range.table = l.table;
        var->range.offset = r_end;
        var->range.count = l_end - r_end;

        /* Flag so we know we're done the next redo */
        op_ctx->redo = true;
        return true;
    } else {
        /* Restore previous value */
        var->range = l;
        return false;
    }
}

static
bool flecs_query_pred_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    bool is_neq)
{
    ecs_query_eq_ctx_t *op_ctx = flecs_op_ctx(ctx, eq);
    uint64_t written = ctx->written[ctx->op_index];
    ecs_assert(flecs_ref_is_written(op, &op->src, EcsQuerySrc, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized match operand");
    (void)written;

    ecs_var_id_t src_var = op->src.var;
    const char *match = flecs_query_name_arg(op, ctx);
    ecs_table_range_t l;
    if (!redo) {
        l = flecs_query_get_range(op, &op->src, EcsQuerySrc, ctx);
        if (!l.table) {
            return false;
        }

        if (!l.count) {
            l.count = ecs_table_count(l.table);
        }

        op_ctx->range = l;
        op_ctx->index = l.offset;
        op_ctx->name_col = flecs_ito(int16_t,   
            ecs_table_get_type_index(ctx->world, l.table, 
                ecs_pair(ecs_id(EcsIdentifier), EcsName)));
        if (op_ctx->name_col == -1) {
            return is_neq;
        }
        op_ctx->name_col = flecs_ito(int16_t, 
            l.table->column_map[op_ctx->name_col]);
        ecs_assert(op_ctx->name_col != -1, ECS_INTERNAL_ERROR, NULL);
    } else {
        if (op_ctx->name_col == -1) {
            /* Table has no name */
            return false;
        }

        l = op_ctx->range;
    }

    const EcsIdentifier *names = l.table->data.columns[op_ctx->name_col].data.array;
    int32_t count = l.offset + l.count, offset = -1;
    for (; op_ctx->index < count; op_ctx->index ++) {
        const char *name = names[op_ctx->index].value;
        bool result = strstr(name, match);
        if (is_neq) {
            result = !result;
        }

        if (!result) {
            if (offset != -1) {
                break;
            }
        } else {
            if (offset == -1) {
                offset = op_ctx->index;
            }
        }
    }

    if (offset == -1) {
        ctx->vars[src_var].range = op_ctx->range;
        return false;
    }

    ctx->vars[src_var].range.offset = offset;
    ctx->vars[src_var].range.count = (op_ctx->index - offset);
    return true;
}

bool flecs_query_pred_eq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_pred_match(op, redo, ctx, false);
}

bool flecs_query_pred_neq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_pred_match(op, redo, ctx, true);
}

bool flecs_query_pred_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsQuerySecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized neq operand");

    ecs_table_range_t r = flecs_query_get_range(
        op, &op->second, EcsQuerySecond, ctx);
    return flecs_query_pred_neq_w_range(op, redo, ctx, r);
}

bool flecs_query_pred_neq_name(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    const char *name = flecs_query_name_arg(op, ctx);
    ecs_entity_t e = ecs_lookup(ctx->world, name);
    if (!e) {
        /* Entity doesn't exist */
        return true && !redo;
    }

    ecs_table_range_t r = flecs_range_from_entity(e, ctx);
    return flecs_query_pred_neq_w_range(op, redo, ctx, r);
}
