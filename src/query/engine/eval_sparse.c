/**
 * @file query/engine/eval_sparse.c
 * @brief Sparse (non-fragmenting) component evaluation: stored per-entity, not in tables.
 */

#include "../../private_api.h"

/* Initialize sparse storage context from a component record. */
static
bool flecs_query_sparse_init_sparse(
    ecs_query_sparse_ctx_t *op_ctx,
    ecs_component_record_t *cr)
{
    if (!cr) {
        return false;
    }

    ecs_assert(cr->flags & EcsIdDontFragment, ECS_INTERNAL_ERROR, NULL);

    op_ctx->sparse = cr->sparse;
    if (!op_ctx->sparse) {
        return false;
    }

    int32_t count = flecs_sparse_count(op_ctx->sparse);
    if (!count) {
        return false;
    }

    op_ctx->cur = count;

    return true;
}

/* Initialize table range for sparse entity iteration. */
static
void flecs_query_sparse_init_range(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_sparse_ctx_t *op_ctx)
{
    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    op_ctx->range = range;
    op_ctx->cur = range.offset - 1; /* Pre-decremented; next_entity increments before use */
}

/* Advance to the next entity in the range that has the sparse component. */
static
bool flecs_query_sparse_next_entity(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_sparse_ctx_t *op_ctx,
    bool not,
    void **ptr_out)
{
    int32_t end = op_ctx->range.count + op_ctx->range.offset;

next:
    op_ctx->cur ++;

    if (op_ctx->cur == end) {
        flecs_query_src_set_range(op, &op_ctx->range, ctx);
        return false;
    }

    ecs_entity_t e = ecs_table_entities(op_ctx->range.table)[op_ctx->cur];
    bool result;

    if (ptr_out) {
        void *ptr = flecs_sparse_get(op_ctx->sparse, 0, e);
        result = ptr != NULL;
        *ptr_out = ptr;
    } else {
        result = flecs_sparse_has(op_ctx->sparse, e);
    }

    if (not) {
        result = !result;
    }

    if (!result) {
        goto next;
    }

    flecs_query_src_set_single(op, op_ctx->cur, ctx);

    return true;
}

/* Select entities with a specific sparse component id. */
static
bool flecs_query_sparse_select_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_mask,
    ecs_id_t id)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!redo) {
        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!flecs_query_sparse_init_sparse(op_ctx, cr)) {
            return false;
        }
    }

next:
    if (!(op_ctx->cur--)) {
        return false;
    }

    ecs_assert(op_ctx->cur >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(op_ctx->cur < flecs_sparse_count(op_ctx->sparse), 
        ECS_INVALID_OPERATION, "sparse iterator invalidated while iterating");

    ecs_entity_t e = flecs_sparse_ids(op_ctx->sparse)[op_ctx->cur];
    ecs_table_range_t range = flecs_range_from_entity(ctx->world, e);

    if (flecs_query_table_filter(range.table, op->other, table_mask)) {
        goto next;
    }

    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);
    it->ids[field_index] = id;
    flecs_query_set_vars(op, it->ids[field_index], ctx);

    return true;
}

/* Select entities matching a wildcard sparse component id. */
static
bool flecs_query_sparse_select_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_mask,
    ecs_id_t id)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

    if (!redo) {
        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!cr) {
            return false;
        }

        if (ECS_PAIR_FIRST(id) == EcsWildcard) {
            op_ctx->cr = cr->pair->second.next;
        } else {
            ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            op_ctx->cr = cr->pair->first.next;
        }
    } else {
        goto next_select;
    }

next:
    if (!flecs_query_sparse_init_sparse(op_ctx, op_ctx->cr)) {
        return false;
    }

next_select:
    if (flecs_query_sparse_select_id(op, true, ctx, table_mask, 0)) {
        ecs_id_t actual_id = op_ctx->cr->id;
        ctx->it->ids[op->field_index] = actual_id;
        flecs_query_set_vars(op, actual_id, ctx);

        if (op->match_flags & EcsTermMatchAny) {
            ctx->it->ids[op->field_index] = id;
        }

        return true;
    }
    
next_component: {
        ecs_component_record_t *cr = op_ctx->cr;
        if (ECS_PAIR_FIRST(id) == EcsWildcard) {
            cr = op_ctx->cr = cr->pair->second.next;
        } else {
            ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                ECS_INTERNAL_ERROR, NULL);
            cr = op_ctx->cr = cr->pair->first.next;
        }

        if (!cr) {
            return false;
        }

        if (!(cr->flags & EcsIdDontFragment)) {
            goto next_component;
        }
    }

    goto next;
}

/* Advance to the next non-wildcard pair in the non-fragmenting list. */
static
bool flecs_query_sparse_next_wildcard_pair(
    ecs_query_sparse_ctx_t *op_ctx)
{
    ecs_component_record_t *cr = op_ctx->cr;
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);

    do {
        cr = cr->non_fragmenting.next;
    } while (cr && (!ECS_IS_PAIR(cr->id) || ecs_id_is_wildcard(cr->id)));

    if (!cr) {
        return false;
    }

    op_ctx->cr = cr;

    ecs_assert(cr->flags & EcsIdDontFragment, ECS_INTERNAL_ERROR, NULL);

    return true;
}

/* Select entities across all sparse wildcard pair components. */
static
bool flecs_query_sparse_select_all_wildcard_pairs(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_mask)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

    if (!redo) {
        ecs_component_record_t *cr = op_ctx->cr = 
            ctx->world->cr_non_fragmenting_head;
        if (!cr) {
            return false;
        }

        if (!ECS_IS_PAIR(cr->id)|| ecs_id_is_wildcard(cr->id)) {
            goto next_component;
        }
    } else {
        goto next_select;
    }

next:
    if (!flecs_query_sparse_init_sparse(op_ctx, op_ctx->cr)) {
        return false;
    }

next_select:
    if (flecs_query_sparse_select_id(op, true, ctx, table_mask, 0)) {
        ecs_id_t actual_id = op_ctx->cr->id;
        ctx->it->ids[op->field_index] = actual_id;
        flecs_query_set_vars(op, actual_id, ctx);
        return true;
    }

next_component:
    if (!flecs_query_sparse_next_wildcard_pair(op_ctx)) {
        return false;
    }

    goto next;
}

/* Dispatch sparse select operation based on id type. */
bool flecs_query_sparse_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_mask)
{
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    if (ecs_id_is_wildcard(id)) {
        if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
            return flecs_query_sparse_select_all_wildcard_pairs(
                op, redo, ctx, table_mask);
        } else {
            return flecs_query_sparse_select_wildcard(
                op, redo, ctx, table_mask, id);
        }
    } else {
        return flecs_query_sparse_select_id(op, redo, ctx, table_mask, id);
    }
}

/* Test entities in a range for presence of a specific sparse component. */
static
bool flecs_query_sparse_with_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not,
    ecs_id_t id,
    void **ptr_out)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

    if (!redo) {
        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!cr) {
            goto no_sparse;
        }

        ecs_sparse_t *sparse = cr->sparse;
        if (!sparse || !flecs_sparse_count(sparse)) {
            goto no_sparse;
        }

        op_ctx->sparse = sparse;
        flecs_query_sparse_init_range(op, ctx, op_ctx);
    } else {
        if (!op_ctx->range.table) {
            return false;
        }
    }

    return flecs_query_sparse_next_entity(op, ctx, op_ctx, not, ptr_out);
no_sparse:
    op_ctx->sparse = NULL;
    op_ctx->range.table = NULL;
    op_ctx->range.offset = 0;
    op_ctx->range.count = 0;
    return not;
}

/* Test entities for an exclusive sparse relationship. */
static
bool flecs_query_sparse_with_exclusive(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not,
    ecs_id_t id)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

    if (!redo) {
        if (!flecs_query_sparse_init_sparse(op_ctx, op_ctx->cr)) {
            return false;
        }
    }

    ecs_id_t actual_id = op_ctx->cr->id;
    void *tgt_ptr = NULL;
    if (flecs_query_sparse_with_id(op, redo, ctx, not, actual_id, &tgt_ptr)) {
        if (!not) {
            ecs_entity_t tgt = *(ecs_entity_t*)tgt_ptr;
            actual_id = ctx->it->ids[op->field_index] = 
                ecs_pair(ECS_PAIR_FIRST(actual_id), tgt);
        }

        if (not || (op->match_flags & EcsTermMatchAny)) {
            ctx->it->ids[op->field_index] = id;
        }

        flecs_query_set_vars(op, actual_id, ctx);
        return true;
    }

    return false;
}

/* Test entities for a wildcard sparse component match. */
static
bool flecs_query_sparse_with_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not,
    ecs_id_t id)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
    bool with_redo = false;

    if (!redo) {
        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!cr) {
            return false;
        }

        if (cr->flags & EcsIdExclusive) {
            op_ctx->cr = cr;
            op_ctx->exclusive = true;
            return flecs_query_sparse_with_exclusive(op, false, ctx, not, id);
        }

        if (!not) {
            if (ECS_PAIR_FIRST(id) == EcsWildcard) {
                op_ctx->cr = cr->pair->second.next;
            } else {
                ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                    ECS_INTERNAL_ERROR, NULL);
                op_ctx->cr = cr->pair->first.next;
            }
        } else {
            op_ctx->cr = cr;
        }

        if (!op_ctx->cr) {
            return not;
        }
    } else {
        if (op_ctx->exclusive) {
            return flecs_query_sparse_with_exclusive(op, true, ctx, not, id);
        }
        with_redo = true;
        goto next_select;
    }

next:
    if (!flecs_query_sparse_init_sparse(op_ctx, op_ctx->cr)) {
        return not;
    }

next_select: {
        ecs_id_t actual_id = op_ctx->cr->id;

        if (flecs_query_sparse_with_id(op, with_redo, ctx, not, actual_id, NULL)) {
            ctx->it->ids[op->field_index] = actual_id;
            flecs_query_set_vars(op, actual_id, ctx);

            if (op->match_flags & EcsTermMatchAny) {
                ctx->it->ids[op->field_index] = id;
            }

            return true;
        }
    }

next_component: {
        ecs_component_record_t *cr = op_ctx->cr;
        if (!not) {
            if (ECS_PAIR_FIRST(id) == EcsWildcard) {
                cr = op_ctx->cr = cr->pair->second.next;
            } else {
                ecs_assert(ECS_PAIR_SECOND(id) == EcsWildcard, 
                    ECS_INTERNAL_ERROR, NULL);
                cr = op_ctx->cr = cr->pair->first.next;
            }
        } else {
            cr = NULL;
        }

        if (!cr) {
            return false;
        }

        if (!(cr->flags & EcsIdDontFragment)) {
            goto next_component;
        }
    }

    with_redo = false;
    goto next;
}

/* Test entities against all sparse wildcard pair components. */
static
bool flecs_query_sparse_with_all_wildcard_pairs(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not)
{
    ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);
    bool with_redo = false;

    if (!redo) {
        ecs_component_record_t *cr = op_ctx->cr = 
            ctx->world->cr_non_fragmenting_head;
        if (!cr) {
            return false;
        }

        if (!ECS_IS_PAIR(cr->id) || ecs_id_is_wildcard(cr->id)) {
            goto next_component;
        }
    } else {
        with_redo = true;
        goto next_select;
    }

next:
    if (!flecs_query_sparse_init_sparse(op_ctx, op_ctx->cr)) {
        return false;
    }

next_select: {
        ecs_id_t actual_id = op_ctx->cr->id;
        if (flecs_query_sparse_with_id(op, with_redo, ctx, not, actual_id, NULL)) {
            ctx->it->ids[op->field_index] = actual_id;
            flecs_query_set_vars(op, actual_id, ctx);
            return true;
        }
    }

next_component:
    if (!flecs_query_sparse_next_wildcard_pair(op_ctx)) {
        return false;
    }

    with_redo = false;
    goto next;
}

/* Dispatch sparse with operation based on id type. */
bool flecs_query_sparse_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool not)
{
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    if (ecs_id_is_wildcard(id)) {
        if (id == ecs_pair(EcsWildcard, EcsWildcard)) {
            return flecs_query_sparse_with_all_wildcard_pairs(op, redo, ctx, not);
        } else {
            return flecs_query_sparse_with_wildcard(op, redo, ctx, not, id);
        }
    } else {
        return flecs_query_sparse_with_id(op, redo, ctx, not, id, NULL);
    }
}

/* Evaluate sparse component with upward hierarchy traversal. */
bool flecs_query_sparse_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (!redo) {
            /* Sparse components are resolved by the traversal cache */
            if (!flecs_query_up_with(op, redo, ctx)) {
                return false;
            }

            return true;
        } else {
            return false;
        }
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectUp, FlecsQueryUpSelectSparse);
    }
}

/* Evaluate sparse component on self first, then traverse upward. */
bool flecs_query_sparse_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

        if (!redo) {
            op_ctx->self = true;
        }

        if (op_ctx->self) {
            bool result = flecs_query_sparse_with(op, redo, ctx, false);
            if (result) {
                return true;
            }

            op_ctx->self = false;

            if (!flecs_query_up_with(op, false, ctx)) {
                return false;
            }

            flecs_query_sparse_init_range(op, ctx, op_ctx);
        }

        return flecs_query_sparse_next_entity(op, ctx, op_ctx, true, NULL);
    } else {
        ecs_query_sparse_ctx_t *op_ctx = flecs_op_ctx(ctx, sparse);

        if (!redo) {
            op_ctx->self = true;
        }

        bool up_redo = true;

        if (op_ctx->self) {
            ecs_id_t id = flecs_query_op_get_id(op, ctx);
            if (flecs_query_sparse_select_id(op, redo, ctx, 
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled), id))
            {
                return true;
            }

            op_ctx->self = false;
            up_redo = false;

            goto next_table;
        }

next_entity: {
            bool result = flecs_query_sparse_next_entity(
                op, ctx, op_ctx, true, NULL);
            if (!result) {
                op_ctx->cur = op_ctx->prev_cur;
                op_ctx->range = op_ctx->prev_range;
                goto next_table;
            }

            return true;
        }

next_table: {
            if (!flecs_query_up_select(op, up_redo, ctx, 
                FlecsQueryUpSelectUp, FlecsQueryUpSelectSparse))
            {
                return false;
            }

            op_ctx->prev_cur = op_ctx->cur;
            op_ctx->prev_range = op_ctx->range;

            flecs_query_sparse_init_range(op, ctx, op_ctx);

            goto next_entity;
        }
    }
}

/* Evaluate sparse component query operation. */
bool flecs_query_sparse(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_sparse_with(op, redo, ctx, false);
    } else {
        return flecs_query_sparse_select(op, redo, ctx, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
    }
}
