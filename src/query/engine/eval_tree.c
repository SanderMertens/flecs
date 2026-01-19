/**
 * @file query/engine/eval_tree.c
 * @brief Hierarchy evaluation (ChildOf pairs/Parent components).
 */

#include "../../private_api.h"

const EcsParent* flecs_query_tree_get_parents(
    ecs_table_range_t range)
{
    int32_t parent_column = range.table->component_map[ecs_id(EcsParent)];
    ecs_assert(parent_column != -1, ECS_INTERNAL_ERROR, NULL);
    return ecs_table_get_column(range.table, parent_column - 1, range.offset);
}

static
bool flecs_query_tree_select_tgt(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    if (redo) {
        op_ctx->cur ++;
        if (op_ctx->cur >= op_ctx->range.count) {
            return false;
        }
    }

    ecs_entity_t child = op_ctx->entities[op_ctx->cur];
    ecs_assert(child != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_table_range_t range = flecs_range_from_entity(ctx->world, child);
    flecs_query_var_set_range(op, op->src.var, 
        range.table, range.offset, range.count, ctx);

    return true;
}

static
bool flecs_query_tree_with_parent(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);
    ecs_table_range_t range = op_ctx->range;

repeat:
    if (redo) {
        op_ctx->cur ++;
    }

    if ((op_ctx->cur - range.offset) >= range.count) {
        flecs_query_src_set_range(op, &op_ctx->range, ctx);
        return false;
    }

    ecs_assert(op_ctx->cur >= range.offset, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(op_ctx->cur < (range.offset + range.count), ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t tgt = op_ctx->tgt;
    ecs_entity_t parent = op_ctx->parents[op_ctx->cur - range.offset].value;

    ecs_iter_t *it = ctx->it;
    flecs_query_iter_set_id(it, op->field_index, ecs_childof(parent));

    if (tgt != EcsWildcard) {
        if (parent != op_ctx->tgt) {
            redo = true;
            goto repeat;
        }
    }

    flecs_query_src_set_single(op, op_ctx->cur, ctx);

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
        op_ctx->range.count = ecs_vec_count(v_children);
        if (!op_ctx->range.count) {
            return false;
        }
    } else {
        if (!op_ctx->tgt) {
            return flecs_query_and(op, redo, ctx);
        }
    }

    return flecs_query_tree_select_tgt(op, redo, ctx);
}

static
bool flecs_query_tree_select_any(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_id_t pair)
{
    ecs_query_tree_wildcard_ctx_t *op_ctx = flecs_op_ctx(ctx, tree_wildcard);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);

    if (!redo) {
        op_ctx->cr = ctx->world->cr_childof_wildcard;
        ecs_assert(op_ctx->cr != NULL, ECS_INTERNAL_ERROR, NULL);
        op_ctx->state = EcsQueryTreeIterEntities;
    }

next:
    switch(op_ctx->state) {
    case EcsQueryTreeIterEntities: {
        bool result = flecs_query_select_w_id(
            op, redo, ctx, ecs_id(EcsParent), 
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
        if (!result) {
            op_ctx->state = EcsQueryTreeIterTables;
            redo = false;
            goto next;
        }
        it->ids[field_index] = ecs_pair(EcsChildOf, EcsWildcard);
        return true;
    }
    case EcsQueryTreeIterTables: {
        bool result = flecs_query_select_w_id(
            op, redo, ctx, pair, 
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
        if (!result) {
            return false;
        }
        it->ids[field_index] = pair;
        return true;
    }
    case EcsQueryTreeIterNext:
    default:
        return false;
    }
}

static
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
            ecs_table_range_t range = flecs_range_from_entity(ctx->world, child);
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
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    if (redo) {
        if (op_ctx->tgt) {
            return flecs_query_tree_with_parent(op, redo, ctx);
        }

        flecs_query_src_set_range(op, &op_ctx->range, ctx);
        return false;
    }

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    op_ctx->range = range;

    ecs_table_t *table = range.table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Fragmenting childof */
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

    /* Root */
    if (!(table->flags & EcsTableHasParent)) {
        if (!tgt) {
            int8_t field_index = op->field_index;
            it->set_fields &= ~(1u << field_index);
            flecs_query_iter_set_id(it, op->field_index, ecs_childof(EcsWildcard));
            return true;
        }
        return false;
    }

    /* Non-fragmenting childof */
    const EcsParent *parents = flecs_query_tree_get_parents(range);

    /* Evaluating a single entity */
    if (range.count == 1) {
        ecs_entity_t parent = parents[0].value;

        /* Wildcard query */
        if (tgt == EcsWildcard) {
            if (op->match_flags & EcsTermMatchAny) {
                flecs_query_iter_set_id(it, op->field_index, ecs_childof(EcsWildcard));
            } else {
                ecs_id_t actual_id = flecs_query_iter_set_id(it, op->field_index, 
                    ecs_childof(parent));
                flecs_query_set_vars(op, actual_id, ctx);
            }

        /* Parent query */
        } else {
            if ((uint32_t)parent != tgt) {
                return false;
            }

            ecs_id_t actual_id = flecs_query_iter_set_id(it, op->field_index,
                ecs_childof(parent));
            flecs_query_set_vars(op, actual_id, ctx);
        }

    /* Evaluating an entity range */
    } else {
        /* Wildcard query */
        if (tgt == EcsWildcard) {
            if (op->match_flags & EcsTermMatchAny) {
                flecs_query_iter_set_id(it, op->field_index, ecs_childof(EcsWildcard));
            } else {
                op_ctx->parents = parents;
                op_ctx->tgt = tgt;
                op_ctx->cur = range.offset;
                return flecs_query_tree_with_parent(op, redo, ctx);
            }

        /* Parent query */
        } else {
            ecs_component_record_t *cr = flecs_components_get(
                ctx->world, ecs_pair(EcsChildOf, tgt));
            if (!cr) {
                return false;
            }

            ecs_parent_record_t *pr = flecs_component_get_parent_record(
                cr, range.table);
            if (!pr) {
                /* Table doesn't have entities with parent */
                return false;
            }

            ecs_entity_t child = pr->first_entity;
            if (child) {
                ecs_assert(pr->count == 1, ECS_INTERNAL_ERROR, NULL);
                ecs_record_t *r = flecs_entities_get_any(
                    ctx->world, pr->first_entity);
                ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(r->table == range.table, ECS_INTERNAL_ERROR, NULL);
                int32_t cur = ECS_RECORD_TO_ROW(r->row);

                /* Table contains a single entity for parent */
                if ((range.offset > cur) || ((range.offset + range.count) <= cur)) {
                    /* Entity does not fall within range */
                    return false;
                }
                
                flecs_query_iter_set_id(it, op->field_index, ecs_childof(tgt));
                flecs_query_src_set_single(op, cur, ctx);
                return true;
            }

            /* Table contains multiple entities for same parent, scan */
            op_ctx->parents = parents;
            op_ctx->tgt = tgt;
            op_ctx->cur = range.offset;

            return flecs_query_tree_with_parent(op, redo, ctx);
        }
    }

    return true;
}

static
bool flecs_query_tree_with_pre(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);

    if (range.table->flags & EcsTableHasChildOf) {
        bool result = flecs_query_with(op, redo, ctx);
        if (op->match_flags & EcsTermMatchAny) {
            flecs_query_iter_set_id(
                ctx->it, op->field_index, ecs_childof(EcsWildcard));
        }
        return result;
    }

    if (range.table->flags & EcsTableHasParent) {
        flecs_query_iter_set_id(
            ctx->it, op->field_index, ecs_pair(EcsChildOf, EcsWildcard));
        return true;
    }

    return false;
}

static
bool flecs_query_children_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);
    ecs_assert(op_ctx->tgt == 0, ECS_INTERNAL_ERROR, NULL);
    ecs_iter_t *it = ctx->it;

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, 
            ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t tgt = ECS_PAIR_SECOND(id);
        ecs_assert(tgt != EcsWildcard, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(tgt != EcsAny, ECS_INTERNAL_ERROR, NULL);
        (void)tgt;

        ecs_component_record_t *cr = flecs_components_get(ctx->world, id);
        if (!cr) {
            return false;
        }

        op_ctx->tgt = 0;
        if (!(cr->flags & EcsIdOrderedChildren)) {
            /* No vector with ordered children, forward to regular search. */
            op_ctx->state = EcsQueryTreeIterTables;
            goto next;
        }

        ecs_pair_record_t *pr = cr->pair;
        ecs_assert(pr != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_t *v_children = &pr->ordered_children;
        uint32_t filter = flecs_ito(uint32_t, op->other);

        if ((!pr->disabled_tables || !(filter & EcsTableIsDisabled)) &&
            (!pr->prefab_tables || !(filter & EcsTableIsPrefab))) 
        {
            it->entities = ecs_vec_first_t(v_children, ecs_entity_t);
            it->count = ecs_vec_count(v_children);
            return true;
        } else {
           /* Flags that we're going to iterate each entity separately because we
            * need to filter out disabled entities. */
           op_ctx->state = EcsQueryTreeIterEntities;
           op_ctx->entities = ecs_vec_first_t(v_children, ecs_entity_t);
           op_ctx->cur = -1;
           op_ctx->range.count = ecs_vec_count(v_children);
           op_ctx->cr = cr;
        }
    } else {
        if (!op_ctx->state) {
            return false;
        }
    }

next:
    if (op_ctx->state == EcsQueryTreeIterEntities) {
        int32_t cur = ++ op_ctx->cur;
        if (cur >= op_ctx->range.count) {
            return false;
        }

        ecs_entity_t *e = &op_ctx->entities[cur];
        ecs_record_t *r = flecs_entities_get(ctx->world, *e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (flecs_query_table_filter(r->table, op->other, 
            (EcsTableIsDisabled|EcsTableIsPrefab))) 
        {
            /* Skip disabled/prefab entities */
            goto next;
        }

        it->entities = e;
        it->count = 1;
        return true;
    } else {
        ecs_assert(op_ctx->state == EcsQueryTreeIterTables, 
            ECS_INTERNAL_ERROR, NULL);
        return flecs_query_and(op, redo, ctx);
    }
}

static
bool flecs_query_children_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);
    if (range.table->flags & EcsTableHasChildOf) {
        return flecs_query_and(op, redo, ctx);
    }

    if (!(range.table->flags & EcsTableHasParent)) {
        /* If table doesn't have ChildOf or Parent its entities don't have 
         * parents. */
        return false;
    }

    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    ecs_assert(ECS_PAIR_FIRST(id) == EcsChildOf, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t tgt = ECS_PAIR_SECOND(id);
    if (tgt == EcsWildcard || tgt == EcsAny) {
        /* Entities in table have parents, so wildcard will always match. */
        return true;
    }

    /* TODO: if a ChildOf query is constrained to a table with Parent component,
     * only some entities in the table may match the query. TBD on what the 
     * behavior should be in this case. For now the query engine only supports
     * constraining the query to a single entity or an entire table. */
    ecs_assert(range.count < 2, ECS_UNSUPPORTED, 
        "can only use set_var() to a single entity for ChildOf($this, parent) terms");

    if (range.count == 0) {
        /* If matching the entire table, return true. Even though not all 
         * entities in the table may match, this lets us add tables with the
         * Parent component to query caches. */
        return true;
    }

    if (flecs_query_table_filter(range.table, op->other, 
        EcsTableIsDisabled|EcsTableIsPrefab)) 
    {
        return false;
    }

    const EcsParent *parents = flecs_query_tree_get_parents(range);
    if ((uint32_t)parents->value == tgt) {
        return true;
    }

    return false;
}

bool flecs_query_children(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_children_with(op, redo, ctx);
    } else {
        return flecs_query_children_select(op, redo, ctx);
    }
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
        if (op->match_flags & EcsTermMatchAny) {
            return flecs_query_tree_select_any(
                op, redo, ctx, ecs_pair(EcsChildOf, EcsWildcard));
        } else {
            return flecs_query_tree_select_wildcard(op, redo, ctx, bulk_return);
        }
    }
}

bool flecs_query_tree_pre(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_tree_with_pre(op, redo, ctx);
    } else {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        return flecs_query_tree_select_any(op, redo, ctx, id);
    }
}

bool flecs_query_tree_post(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    /* Source should have been written as this instruction can only be inserted
     * after a cache instruction has been evaluated. */
    uint64_t written = ctx->written[ctx->op_index];
    ecs_assert(written & (1ull << op->src.var), ECS_INTERNAL_ERROR, NULL);
    (void)written;

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);

    if (range.table->flags & (EcsTableHasChildOf)) {
        return !redo;
    }

    /* Shouldn't have gotten here if the table has neither ChildOf or Parent */
    ecs_assert(range.table->flags & EcsTableHasParent, ECS_INTERNAL_ERROR, NULL);
    
    return flecs_query_tree_with(op, redo, ctx);
}

bool flecs_query_tree_up_pre(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool self)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        if (self) {
            return flecs_query_self_up(op, redo, ctx);
        } else {
            return flecs_query_up(op, redo, ctx);
        }
    } else {
        ecs_query_tree_pre_ctx_t *op_ctx = flecs_op_ctx(ctx, tree_pre);
        if (!redo) {
            op_ctx->state = EcsQueryTreeIterTables;
        }

        if (op_ctx->state == EcsQueryTreeIterTables) {
            bool result;
retry:
            if (self) {
                result = flecs_query_self_up(op, redo, ctx);
            } else {
                result = flecs_query_up(op, redo, ctx);
            }

            if (!result) {
                op_ctx->state = EcsQueryTreeIterEntities;
                redo = false;
            } else {
                ecs_table_range_t range = flecs_query_get_range(
                    op, &op->src, EcsQuerySrc, ctx);
                ecs_assert(range.table != NULL, ECS_INTERNAL_ERROR, NULL);
                if (range.table->flags & EcsTableHasParent) {
                    /* Skip tables with Parent, since we'll yield all tables 
                     * with Parent in the second phase. */
                    redo = true;
                    goto retry;
                }

                return true;
            }
        }

        bool result = flecs_query_select_w_id(op, redo, ctx, ecs_id(EcsParent), 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
        if (result) {
            /* Signal this table needs post processing */
            ctx->it->sources[op->field_index] = EcsWildcard;
        }

        return result;
    }
}

bool flecs_query_tree_up_post(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool self)
{
    ecs_query_tree_ctx_t *op_ctx = flecs_op_ctx(ctx, tree);

    /* Source should have been written as this instruction can only be inserted
     * after a cache instruction has been evaluated. */
    uint64_t written = ctx->written[ctx->op_index];
    ecs_assert(written & (1ull << op->src.var), ECS_INTERNAL_ERROR, NULL);
    (void)written;

    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsQuerySrc, ctx);

    /* Passthrough tables with ChildOf pair */
    if (!(range.table->flags & EcsTableHasParent)) {
        return !redo;
    }

    if (!redo) {
        op_ctx->tgt = ctx->it->sources[op->field_index];
    }

    /* Passthrough tables that own the component */
    if (op_ctx->tgt != EcsWildcard) {
        return !redo;
    }

    /* Shouldn't have gotten here if the table has neither ChildOf or Parent */
    ecs_assert(range.table->flags & EcsTableHasParent, ECS_INTERNAL_ERROR, NULL);

    const ecs_term_t *term = &ctx->query->pub.terms[op->term_index];

    if (term->oper == EcsOptional) {
        if (!redo) {
            if (!range.count) {
                range.count = ecs_table_count(range.table);
            }

            op_ctx->range = range;
            op_ctx->cur = range.offset - 1;
        }

        op_ctx->cur ++;

        if (op_ctx->cur >= op_ctx->range.count) {
            flecs_query_src_set_range(op, &op_ctx->range, ctx);
            return false;
        }

        bool result = false;

        flecs_query_src_set_single(op, op_ctx->cur, ctx);

        if (self) {
            result = flecs_query_self_up_with(op, false, ctx);
        } else {
            result = flecs_query_up_with(op, false, ctx);
        }

        uint64_t field_bit = 1llu << op->field_index;
        if (!result) {
            ctx->it->set_fields &= (ecs_termset_t)~field_bit;
        } else {
            ctx->it->set_fields |= (ecs_termset_t)field_bit;
        }

        return true;
    } else {
        if (self) {
            return flecs_query_self_up_with(op, redo, ctx);
        } else {
            return flecs_query_up_with(op, redo, ctx);
        }
    }
}
