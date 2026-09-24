/**
 * @file query/engine/eval_up.c
 * @brief Up traversal evaluation.
 */

#include "../../private_api.h"

#ifdef FLECS_QUERY_PLANS

/* Find tables with requested component that have traversable entities. */
static bool flecs_query_up_select_table(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_select_trav_kind_t trav_kind,
    ecs_query_up_select_kind_t kind)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_query_up_impl_t *impl = op_ctx->impl;
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_iter_t *it = ctx->it;
    bool self = trav_kind == FlecsQueryUpSelectSelfUp;
    ecs_table_range_t range;

    do {
        bool result;
        if (ECS_PAIR_FIRST(op_ctx->with) == EcsChildOf) {
            if (op_ctx->with == ecs_childof(EcsWildcard)) {
                result = flecs_query_tree_and_wildcard(op, redo, ctx, false);
            } else {
                result = flecs_query_tree_and(op, redo, ctx);
            }
        } else if (kind == FlecsQueryUpSelectDefault) {
            result = flecs_query_select_w_id(op, redo, ctx, 
                op_ctx->with, 0);
        } else if (kind == FlecsQueryUpSelectSparse) {
            result = flecs_query_sparse_select(op, redo, ctx, 0);
        } else {
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }

        if (!result) {
            /* No remaining tables with component found. */
            return false;
        }

        redo = true;

        range = flecs_query_get_range(op, &op->src, EcsQuerySrc, ctx);
        ecs_assert(range.table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Keep searching until we find a table that has the requested component,
         * with traversable entities */
    } while (!self && range.table->_->traversable_count == 0);

    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    impl->table = range.table;
    impl->row = range.offset;
    impl->end = range.offset + range.count;
    impl->matched = it->ids[op->field_index];
    impl->start_down_walk = true;

    return true;
}

/* Find next traversable entity in table. */
static ecs_trav_down_t* flecs_query_up_find_next_traversable(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_select_trav_kind_t trav_kind)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_query_up_impl_t *impl = op_ctx->impl;
    ecs_assert(impl != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_world_t *world = ctx->world;
    ecs_iter_t *it = ctx->it;
    const ecs_query_t *q = &ctx->query->pub;
    ecs_table_t *table = impl->table;
    bool self = trav_kind == FlecsQueryUpSelectSelfUp;

    if (table->_->traversable_count == 0) {
        /* No traversable entities in table */
        impl->table = NULL;
        return NULL;
    } else {
        int32_t row;
        ecs_entity_t entity = 0;
        const ecs_entity_t *entities = ecs_table_entities(table);

        for (row = impl->row; row < impl->end; row ++) {
            entity = entities[row];
            ecs_record_t *record = flecs_entities_get(world, entity);
            if (record->row & EcsEntityIsTraversable) {
                /* Found traversable entity */
                it->sources[op->field_index] = entity;
                ECS_CONST_CAST(int16_t*, it->columns)[op->field_index] = -1;
                break;
            }
        }

        if (row == impl->end) {
            /* No traversable entities remaining in table */
            impl->table = NULL;
            return NULL;
        }

        impl->row = row;

        /* Get down cache entry for traversable entity */
        bool match_empty = (q->flags & EcsQueryMatchEmptyTables) != 0;
        impl->down = flecs_query_get_down_cache(ctx, &impl->cache, 
            op_ctx->trav, entity, op_ctx->cr_with, self, match_empty);
        impl->cache_elem = -1;
    }

    return impl->down;
}

/* Select all tables that can reach the target component through the traversal
 * relationship. */
bool flecs_query_up_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_select_trav_kind_t trav_kind,
    ecs_query_up_select_kind_t kind)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_query_up_impl_t *impl = op_ctx->impl;
    
    if (!impl) {
        ecs_iter_t *it = ctx->it;
        ecs_allocator_t *a = flecs_query_get_allocator(it);
        impl = op_ctx->impl = flecs_calloc_t(a, ecs_query_up_impl_t);
        it->flags |= EcsIterOpCtxFini;
    }

    ecs_iter_t *it = ctx->it;
    bool redo_select = redo;
    const ecs_query_t *q = &ctx->query->pub;
    bool self = trav_kind == FlecsQueryUpSelectSelfUp;

    op_ctx->trav = q->terms[op->term_index].trav;

    /* Reuse component record from previous iteration if possible */
    if (!op_ctx->cr_trav) {
        op_ctx->cr_trav = flecs_components_get(ctx->world, 
            ecs_pair(op_ctx->trav, EcsWildcard));
    }

    /* If component record is not found, or if it doesn't have any tables, revert to
     * iterating owned components (no traversal) */
    if (!op_ctx->cr_trav || 
        !flecs_table_cache_count(&op_ctx->cr_trav->cache))
    {
        if (!self) {
            /* If operation does not match owned components, return false */
            return false;
        } else if (kind == FlecsQueryUpSelectDefault) {
            return flecs_query_select(op, redo, ctx);
        } else if (kind == FlecsQueryUpSelectSparse) {
            return flecs_query_sparse_select(op, redo, ctx, 0);
        } else {
            /* Invalid select kind */
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (!redo) {
        /* Get component id to match */
        op_ctx->with = flecs_query_op_get_id(op, ctx);

        /* Get component record for component to match */
        op_ctx->cr_with = flecs_components_get(ctx->world, op_ctx->with);
        if (!op_ctx->cr_with) {
            /* If component record does not exist, there can't be any results */
            return false;
        }

        impl->down = NULL;
        impl->cache_elem = 0;
        impl->last_down_table = NULL;
        impl->start_down_walk = false;
    }

    /* Get last used entry from down traversal cache. Cache entries in the down
     * traversal cache contain a list of tables that can reach the requested 
     * component through the traversal relationship, for a traversable entity
     * which acts as the key for the cache. */
    ecs_trav_down_t *down = impl->down;

next_down_entry:
    /* Get (next) entry in down traversal cache */
    while (!down) {
        ecs_table_t *table = impl->table;

        /* Get (next) table with traversable entities that have the 
         * requested component. We'll traverse downwards from the 
         * traversable entities in the table to find all entities that can
         * reach the component through the traversal relationship. */
        if (!table) {
            /* Reset source, in case we have to return a component matched
             * by the entity in the found table. */
            it->sources[op->field_index] = 0;

            if (!flecs_query_up_select_table(
                op, redo_select, ctx, trav_kind, kind))
            {
                return false;
            }

            table = impl->table;

            /* If 'self' is true, we're evaluating a term with self|up. This
             * means that before traversing downwards, we should also return 
             * the current table as result. */
            if (self) {
                if (!flecs_query_table_filter(table, op->other,
                    (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
                {
                    flecs_reset_source_set_flag(it, op->field_index);
                    const ecs_table_record_t *tr =
                        it->trs[op->field_index];
                    ECS_CONST_CAST(int16_t*, it->columns)[op->field_index] =
                        tr ? tr->column : -1;
                    impl->row --;
                    return true;
                }
            }

            redo_select = true;
        } else {
            /* Evaluate next entity in table */
            impl->row ++;
        }

        if (impl->start_down_walk) {
            impl->start_down_walk = false;

            if (table == impl->last_down_table) {
                impl->table = NULL;
                continue;
            }

            impl->last_down_table = table;
        }

        /* Get down cache entry for next traversable entity in table */
        down = flecs_query_up_find_next_traversable(op, ctx, trav_kind);
        if (!down) {
            goto next_down_entry;
        }
    }

next_down_elem:
    /* Get next element (table) in cache entry */
    if ((++ impl->cache_elem) >= ecs_vec_count(&down->elems)) {
        /* No more elements in cache entry, find next. */
        down = NULL;
        goto next_down_entry;
    }

    ecs_trav_down_elem_t *elem = ecs_vec_get_t(
        &down->elems, ecs_trav_down_elem_t, impl->cache_elem);
    flecs_query_var_set_range(op, op->src.var, elem->range.table, 
        elem->range.offset, elem->range.count, ctx);
    flecs_query_set_vars(op, impl->matched, ctx);

    if (flecs_query_table_filter(elem->range.table, op->other, 
        (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
    {
        /* Go to next table if table contains prefabs, disabled entities or
         * entities that are not queryable. */
        goto next_down_elem;
    }

    flecs_set_source_set_flag(it, op->field_index);

    return true;
}

void flecs_query_up_set_fields(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    const ecs_query_up_ctx_t *op_ctx,
    const ecs_trav_up_t *up)
{
    ecs_iter_t *it = ctx->it;
    it->sources[op->field_index] = up->src;
    flecs_query_it_set_tr(it, op->field_index, up->tr);
    it->ids[op->field_index] = up->id;
    if (op->match_flags & EcsTermMatchAny) {
        it->ids[op->field_index] = ecs_pair(op_ctx->trav, EcsWildcard);
    }

    flecs_query_set_vars(op, up->id, ctx);
    flecs_set_source_set_flag(it, op->field_index);
}

static bool flecs_query_up_resolve(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_ctx_t *op_ctx,
    ecs_table_t *table,
    int32_t row,
    ecs_trav_up_t *out)
{
    (void)op;

    int result = flecs_query_up_reachable(ctx->world, table, row,
        op_ctx->with, op_ctx->trav, op_ctx->cr_with, op_ctx->match_inherited,
        out);
    if (result != -1) {
        return result == 1;
    }

    ecs_query_up_impl_t *impl = op_ctx->impl;
    if (!impl) {
        ecs_allocator_t *a = flecs_query_get_allocator(ctx->it);
        impl = op_ctx->impl = flecs_calloc_t(a, ecs_query_up_impl_t);
        ctx->it->flags |= EcsIterOpCtxFini;
    }

    ecs_trav_up_t *up = flecs_query_get_up_cache(ctx, &impl->cache,
        table, row, op_ctx->with, op_ctx->trav, op_ctx->cr_with,
        op_ctx->cr_trav);
    if (!up) {
        return false;
    }

    out->src = flecs_entities_get_alive(ctx->world, up->src);
    out->tr = up->tr;
    out->id = up->id;
    return true;
}

bool flecs_query_up_init(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_ctx_t *op_ctx)
{
    const ecs_query_t *q = &ctx->query->pub;
    const ecs_term_t *term = &q->terms[op->term_index];

    op_ctx->trav = term->trav;
    if (!op_ctx->cr_trav) {
        if (term->trav == EcsChildOf) {
            op_ctx->cr_trav = ctx->world->cr_childof_wildcard;
        } else {
            op_ctx->cr_trav = flecs_components_get(ctx->world,
                ecs_pair(op_ctx->trav, EcsWildcard));
        }
    }

    if (!op_ctx->cr_trav ||
        !flecs_table_cache_count(&op_ctx->cr_trav->cache))
    {
        return false;
    }

    ecs_id_t with = op_ctx->with = flecs_query_op_get_id(op, ctx);
    ecs_component_record_t *cr_with = NULL;
    int8_t field_index = op->field_index;
    if (field_index != -1) {
        ecs_component_record_t **cr_cache = ctx->query->cr_cache;
        cr_with = cr_cache[field_index];
        if (!cr_with || cr_with->id != with) {
            cr_with = cr_cache[field_index] =
                flecs_components_get(ctx->world, with);
        }
    } else {
        cr_with = flecs_components_get(ctx->world, with);
    }

    op_ctx->cr_with = cr_with;
    if (!cr_with) {
        return false;
    }

    op_ctx->match_inherited = (term->flags_ & EcsTermIdInherited) != 0 ||
        ecs_id_is_wildcard(op_ctx->with);

    return true;
}

void flecs_query_up_parent_begin(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_ctx_t *op_ctx,
    ecs_table_range_t range)
{
    (void)op;
    (void)ctx;

    if (!range.count) {
        range.count = ecs_table_count(range.table);
    }

    op_ctx->range = range;
    op_ctx->parents = flecs_query_tree_get_parents(range);
    op_ctx->cur = 0;
    op_ctx->cur_count = 0;
}

bool flecs_query_up_parent_next(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_up_ctx_t *op_ctx,
    ecs_trav_up_t *up,
    bool *found)
{
    int32_t cur = op_ctx->cur + op_ctx->cur_count;
    int32_t count = op_ctx->range.count;
    if (cur >= count) {
        op_ctx->cur = cur;
        op_ctx->cur_count = 0;
        return false;
    }

    const EcsParent *parents = op_ctx->parents;
    ecs_entity_t parent = parents[cur].value;
    int32_t run = 1;

    if (op->kind != EcsQuerySparseUp && op->kind != EcsQuerySparseSelfUp) {
        while ((cur + run) < count && parents[cur + run].value == parent) {
            run ++;
        }
    }

    op_ctx->cur = cur;
    op_ctx->cur_count = run;

    ecs_table_range_t range = {
        .table = op_ctx->range.table,
        .offset = op_ctx->range.offset + cur,
        .count = run
    };

    flecs_query_src_set_range(op, &range, ctx);

    *found = flecs_query_up_resolve(op, ctx, op_ctx, range.table,
        range.offset, up);

    return true;
}

/* Check if a table can reach the target component through the traversal
 * relationship. */
bool flecs_query_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_query_t *q = &ctx->query->pub;
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_iter_t *it = ctx->it;
    ecs_trav_up_t up;
    bool found;

    if (!redo) {
        /* Get the range (table) that is currently being evaluated. In most 
         * cases the range will cover the entire table, but in some cases it
         * can only cover a subset of the entities in the table. */
        ecs_table_range_t range = flecs_query_get_range(
            op, &op->src, EcsQuerySrc, ctx);
        if (!range.table) {
            return false;
        }

        const ecs_term_t *term = &q->terms[op->term_index];
        bool parent_table = term->trav == EcsChildOf &&
            (range.table->flags & EcsTableHasParent) != 0;

        if (parent_table && (q->flags & EcsQueryNested)) {
            /* If this is a nested query (used to populate a cache),
             * don't store entries for individual entities in the cache.
             * Instead, match the entire table, and figure out from
             * which parent the entity gets the component in an uncached
             * operation. */

            if (term->oper == EcsNot) {
                return false;
            }

            /* Signal that the uncached instruction needs to search.
             * This helps distinguish between tables with a Parent
             * component that own the component vs. those that don't. */
            op_ctx->trav = term->trav;
            op_ctx->cur = -1;
            it->sources[op->field_index] = EcsWildcard;
            ECS_CONST_CAST(int16_t*, it->columns)[op->field_index] = -1;
            return true;
        }

        if (!flecs_query_up_init(op, ctx, op_ctx)) {
            return false;
        }

        op_ctx->range = range;
        if (!op_ctx->range.count) {
            op_ctx->range.count = ecs_table_count(op_ctx->range.table);
        }

        op_ctx->cur = -1;

        /* Handle tables with non-fragmenting ChildOf */
        if (parent_table) {
            if (!op_ctx->range.count) {
                return false;
            }

            flecs_query_up_parent_begin(op, ctx, op_ctx, op_ctx->range);
            goto next_row;
        }

        if (!flecs_query_up_resolve(op, ctx, op_ctx, op_ctx->range.table, 
            -1, &up))
        {
            return false;
        }

        flecs_query_up_set_fields(op, ctx, op_ctx, &up);
        return true;
    }

    if (op_ctx->cur == -1) {
        /* The table either can or can't reach the component, nothing to do 
         * for a second evaluation of this operation. */
        return false;
    }

next_row:
    while (flecs_query_up_parent_next(op, ctx, op_ctx, &up, &found)) {
        if (found) {
            flecs_query_up_set_fields(op, ctx, op_ctx, &up);
            return true;
        }
    }

    return false;
}

/* Check if a table can reach the target component through the traversal
 * relationship, or if the table has the target component itself. */
bool flecs_query_self_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_iter_t *it = ctx->it;

    if (!redo) {
        flecs_reset_source_set_flag(ctx->it, op->field_index);
        op_ctx->cur = -1;
        op_ctx->trav = 0;
        op_ctx->with = flecs_query_op_get_id(op, ctx);
    }

    /* First check if table has the component */
    if (op_ctx->trav == 0) {
        bool result;
        ecs_id_t with = op_ctx->with;
        if (ECS_PAIR_FIRST(with) == EcsChildOf) {
            if (with == ecs_childof(EcsWildcard)) {
                result = flecs_query_tree_and_wildcard(op, redo, ctx, false);
            } else {
                result = flecs_query_tree_and(op, redo, ctx);
            }
        } else {
            result = flecs_query_with(op, redo, ctx);
        }

        if (result) {
            /* Table has component, no need to traverse */
            if (flecs_query_ref_flags(op->flags, EcsQuerySrc) & EcsQueryIsVar) {
                /* Matching self, so set sources to 0 */
                it->sources[op->field_index] = 0;
                flecs_reset_source_set_flag(it, op->field_index);
                const ecs_table_record_t *tr = it->trs[op->field_index];
                ECS_CONST_CAST(int16_t*, it->columns)[op->field_index] =
                    tr ? tr->column : -1;
            }

            return true;
        }
    }

    /* Table doesn't have component, traverse relationship */
    return flecs_query_up_with(op, redo, ctx);
}

bool flecs_query_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        return flecs_query_up_with(op, redo, ctx);
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectUp, FlecsQueryUpSelectDefault);
    }
}

bool flecs_query_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        return flecs_query_self_up_with(op, redo, ctx);
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectSelfUp, FlecsQueryUpSelectDefault);
    }
}

#endif // FLECS_QUERY_PLANS
