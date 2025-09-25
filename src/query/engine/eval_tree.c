/**
 * @file query/engine/eval_tree.c
 * @brief Hierarchy evaluation (ChildOf pairs/Parent components).
 */

#include "../../private_api.h"

static
bool flecs_query_tree_select_tgt(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    if (redo) {
        op_ctx->cur ++;
        if (op_ctx->cur >= op_ctx->count) {
            return false;
        }
    }

    ecs_entity_t child = op_ctx->entities[op_ctx->cur];
    ecs_assert(child != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_table_range_t range = flecs_range_from_entity(child, ctx);
    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);

    return true;
}

static
bool flecs_query_tree_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, 
            ECS_INTERNAL_ERROR, NULL);
        op_ctx->tgt = ECS_PAIR_SECOND(id);
        op_ctx->cur = 0;

        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!cr) {
            return false;
        }

        if (!(cr->flags & EcsIdOrderedChildren)) {
            op_ctx->tgt = 0;
            return flecs_query_and(op, redo, ctx);
        }

        ecs_assert(op_ctx->tgt != EcsWildcard, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_t *v_children = &cr->pair->ordered_children;
        op_ctx->entities = ecs_vec_first_t(v_children, ecs_entity_t);
        op_ctx->count = ecs_vec_count(v_children);
        if (!op_ctx->count) {
            return false;
        }
    } else {
        if (!op_ctx->tgt) {
            return flecs_query_and(op, redo, ctx);
        }
    }

    return flecs_query_tree_select_tgt(op, redo, ctx);
}

bool flecs_query_tree_select_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool bulk_return)
{
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    if (tgt != EcsWildcard) {
        it->ids[field_index] = id;

        /* Happens when variable has been constrained */
        if (bulk_return) {
            return flecs_query_children(op, redo, ctx);
        } else {
            return flecs_query_tree_select(op, redo, ctx);
        }
    }

    ecs_query_tree_wildcard_ctx_t *op_ctx = flecs_op_ctx(ctx, tree_wildcard);

    if (!redo) {
        op_ctx->cr = ctx->world->cr_childof_wildcard;
        ecs_assert(op_ctx->cr != NULL, ECS_INTERNAL_ERROR, NULL);
        op_ctx->state = EcsQueryTreeIterNext;
    }

next:
    switch(op_ctx->state) {

    /* Select next (ChildOf, parent) pair */
    case EcsQueryTreeIterNext: {
        ecs_component_record_t *cr = op_ctx->cr = 
            flecs_component_first_next(op_ctx->cr);
        if (!cr) {
            return false;
        }

        flecs_query_var_reset(0, ctx);
        if (op->match_flags & EcsTermMatchAny) {
            it->ids[field_index] = ecs_childof(EcsWildcard);
        } else {
            it->ids[field_index] = op_ctx->cr->id;
        }

        if (cr->flags & EcsIdOrderedChildren) {
            ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_vec_t *v_children = &cr->pair->ordered_children;
            if (bulk_return) {
                op_ctx->state = EcsQueryTreeIterNext;
                it->entities = ecs_vec_first_t(v_children, ecs_entity_t);
                it->count = ecs_vec_count(v_children);
                goto done;
            } else {
                op_ctx->entities = ecs_vec_first_t(v_children, ecs_entity_t);
                op_ctx->count = ecs_vec_count(v_children);
                op_ctx->cur = -1;
                op_ctx->state = EcsQueryTreeIterEntities;
                goto next;
            }
        } else {
            if (!flecs_component_iter(cr, &op_ctx->it)) {
                op_ctx->state = EcsQueryTreeIterNext;
            } else {
                op_ctx->state = EcsQueryTreeIterTables;
            }
        }
        goto next;
    }

    /* Iterate tables for (ChildOf, parent) */
    case EcsQueryTreeIterTables: {
        const ecs_table_record_t *tr = flecs_component_next(&op_ctx->it);
        if (!tr) {
            op_ctx->state = EcsQueryTreeIterNext;
            goto next;
        } else {
            ecs_table_t *table = tr->hdr.table;
            if (!ecs_table_count(table) || (table->flags & EcsTableNotQueryable)) {
                goto next;
            }

            it->trs[field_index] = tr;
            flecs_query_var_set_range(op, op->src.var, table, 0, 0, ctx);
            goto done;
        }
    }

    /* Return ordered entities for (ChildOf, parent) one by one */
    case EcsQueryTreeIterEntities: {
        op_ctx->cur ++;
        if (op_ctx->cur >= op_ctx->count) {
            op_ctx->state = EcsQueryTreeIterNext;
            goto next;
        } else {
            ecs_entity_t child = op_ctx->entities[op_ctx->cur];
            ecs_assert(child != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_table_range_t range = flecs_range_from_entity(child, ctx);
            flecs_query_var_set_range(op, op->src.var, 
                range.table, range.offset, range.count, ctx);
            goto done;
        }
    }
    }

done:
    flecs_query_set_vars(op, it->ids[field_index], ctx);
    return true;
}

bool flecs_query_tree_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    ecs_table_t *table = range.table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->flags & EcsTableHasChildOf) {
        if (op->match_flags & EcsTermMatchAny) {
            return flecs_query_and_any(op, redo, ctx);
        } else {
            return flecs_query_with(op, redo, ctx);
        }
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_assert(ECS_IS_PAIR(id), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t tgt = ECS_PAIR_SECOND(id);

    if (!(table->flags & EcsTableHasParent)) {
        if (!tgt) {
            int8_t field_index = op->field_index;
            it->set_fields &= ~(1u << field_index);
            it->ids[op->field_index] = ecs_childof(EcsWildcard);
            return true;
        }
        return false;
    }

    int32_t parent_column = range.table->component_map[ecs_id(EcsParent)];
    ecs_assert(parent_column != -1, ECS_INTERNAL_ERROR, NULL);
    EcsParent *parents = ecs_table_get_column(
        range.table, parent_column - 1, range.offset);

    if (range.count == 1) {
        ecs_entity_t parent = parents[0].value;
        if (tgt == EcsWildcard) {
            if (op->match_flags & EcsTermMatchAny) {
                it->ids[op->field_index] = ecs_childof(EcsWildcard);
            } else {
                ecs_id_t actual_id = it->ids[op->field_index] = 
                    ecs_childof(parent);
                flecs_query_set_vars(op, actual_id, ctx);
            }
        } else {
            if ((uint32_t)parent != tgt) {
                return false;
            }

            ecs_id_t actual_id = it->ids[op->field_index] = 
                ecs_childof(parent);
            flecs_query_set_vars(op, actual_id, ctx);
        }     
    } else {
        ecs_abort(ECS_UNSUPPORTED, NULL);
    }

    return true;
}

bool flecs_query_children(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    if (redo) {
        if (!op_ctx->tgt) {
            bool result = flecs_query_and(op, redo, ctx);
            return result;
        }
        return false;
    }

    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    ecs_assert(tgt != EcsWildcard, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(tgt != EcsAny, ECS_INTERNAL_ERROR, NULL);

    ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
    if (!cr) {
        return false;
    }

    if (!(cr->flags & EcsIdOrderedChildren)) {
        op_ctx->tgt = 0;
        return flecs_query_and(op, redo, ctx);
    }

    ecs_iter_t *it = ctx->it;

    ecs_assert(cr->pair != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_vec_t *v_children = &cr->pair->ordered_children;
    it->entities = ecs_vec_first_t(v_children, ecs_entity_t);
    it->count = ecs_vec_count(v_children);
    return true;
}

bool flecs_query_tree_and(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{    
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_tree_with(op, redo, ctx);
    } else {
        return flecs_query_tree_select(op, redo, ctx);
    }
}

bool flecs_query_tree_and_wildcard(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool bulk_return)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_tree_with(op, redo, ctx);
    } else {
        return flecs_query_tree_select_wildcard(op, redo, ctx, bulk_return);
    }
}
