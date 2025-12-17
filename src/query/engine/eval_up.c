/**
 * @file query/engine/eval.c
 * @brief Query engine implementation.
 */

#include "../../private_api.h"

/* Find tables with requested component that has traversable entities. */
static
bool flecs_query_up_select_table(
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
        if (ECS_PAIR_FIRST(impl->with) == EcsChildOf) {
            if (impl->with == ecs_childof(EcsWildcard)) {
                result = flecs_query_tree_and_wildcard(op, redo, ctx, false);
            } else {
                result = flecs_query_tree_and(op, redo, ctx);
            }
        } else if (kind == FlecsQueryUpSelectId) {
            result = flecs_query_select_id(op, redo, ctx, 0);
        } else if (kind == FlecsQueryUpSelectDefault) {
            result = flecs_query_select_w_id(op, redo, ctx, 
                impl->with, 0);
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

    return true;
}

/* Find next traversable entity in table. */
static
ecs_trav_down_t* flecs_query_up_find_next_traversable(
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
            impl->trav, entity, impl->cr_with, self, match_empty);
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
    }

    ecs_iter_t *it = ctx->it;
    bool redo_select = redo;
    const ecs_query_t *q = &ctx->query->pub;
    bool self = trav_kind == FlecsQueryUpSelectSelfUp;

    impl->trav = q->terms[op->term_index].trav;

    /* Reuse component record from previous iteration if possible*/
    if (!impl->cr_trav) {
        impl->cr_trav = flecs_components_get(ctx->world, 
            ecs_pair(impl->trav, EcsWildcard));
    }

    /* If component record is not found, or if it doesn't have any tables, revert to
     * iterating owned components (no traversal) */
    if (!impl->cr_trav || 
        !flecs_table_cache_count(&impl->cr_trav->cache))
    {
        if (!self) {
            /* If operation does not match owned components, return false */
            return false;
        } else if (kind == FlecsQueryUpSelectId) {
            return flecs_query_select_id(op, redo, ctx,
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
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
        impl->with = flecs_query_op_get_id(op, ctx);

        /* Get component record for component to match */
        impl->cr_with = flecs_components_get(ctx->world, impl->with);
        if (!impl->cr_with) {
            /* If component record does not exist, there can't be any results */
            return false;
        }

        impl->down = NULL;
        impl->cache_elem = 0;
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
                    impl->row --;
                    return true;
                }
            }

            redo_select = true;
        } else {
            /* Evaluate next entity in table */
            impl->row ++;
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
        /* No more elements in cache entry, find next.*/
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

/* Check if a table can reach the target component through the traversal
 * relationship. */
bool flecs_query_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_query_t *q = &ctx->query->pub;
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_query_up_impl_t *impl = op_ctx->impl;
    ecs_iter_t *it = ctx->it;

    if (!impl) {
        ecs_allocator_t *a = flecs_query_get_allocator(it);
        impl = op_ctx->impl = flecs_calloc_t(a, ecs_query_up_impl_t);
    }

    impl->trav = q->terms[op->term_index].trav;
    if (!impl->cr_trav) {
        impl->cr_trav = flecs_components_get(ctx->world, 
            ecs_pair(impl->trav, EcsWildcard));
    }

    if (!impl->cr_trav || 
        !flecs_table_cache_count(&impl->cr_trav->cache))
    {
        /* If there are no tables with traversable relationship, there are no
         * matches. */
        return false;
    }

    if (!redo) {
        impl->trav = q->terms[op->term_index].trav;
        impl->with = flecs_query_op_get_id(op, ctx);
        impl->cr_with = flecs_components_get(ctx->world, impl->with);

        /* If component record for component doesn't exist, there are no matches */
        if (!impl->cr_with) {
            return false;
        }

        /* Get the range (table) that is currently being evaluated. In most 
         * cases the range will cover the entire table, but in some cases it
         * can only cover a subset of the entities in the table. */
        ecs_table_range_t range = flecs_query_get_range(
            op, &op->src, EcsQuerySrc, ctx);
        if (!range.table) {
            return false;
        }

        op_ctx->range = range;
        if (!op_ctx->range.count) {
            op_ctx->range.count = ecs_table_count(op_ctx->range.table);
        }

        op_ctx->cur = -1;

        /* Handle tables with non-fragmenting ChildOf */
        if (impl->trav == EcsChildOf) {
            if (range.table->flags & EcsTableHasParent) {
                if (q->flags & EcsQueryNested) {
                    /* If this is a nested query (used to populate a cache), 
                     * don't store entries for individual entities in the cache.
                     * Instead, match the entire table, and figure out from 
                     * which parent the entity gets the component in an uncached
                     * operation. */

                    /* Signal that the uncached instruction needs to search. 
                     * This helps distinguish between tables with a Parent 
                     * component that own the component vs. those that don't. */
                    it->sources[op->field_index] = EcsWildcard;
                    return true;
                }

                /* Signals that we need to evaluate individual rows. */
                op_ctx->cur = 0;
                flecs_query_src_set_single(op, range.offset, ctx);
            }
        }
    } else {
next_row:
        if (op_ctx->cur == -1) {
            /* The table either can or can't reach the component, nothing to do 
             * for a second evaluation of this operation.*/
            return false;
        }

        /* Evaluate next row. Only necessary for non-fragmenting ChildOf since
         * in that case different table rows can have different parents. */
        op_ctx->cur ++;
        if (op_ctx->cur >= op_ctx->range.count) {
            return false;
        }

        flecs_query_src_set_single(op, op_ctx->range.offset + op_ctx->cur, ctx);
    }

    /* Get entry from up traversal cache. The up traversal cache contains 
     * the entity on which the component was found, with additional metadata
     * on where it is stored. */
    ecs_trav_up_t *up = flecs_query_get_up_cache(ctx, &impl->cache, 
        op_ctx->range.table, op_ctx->range.offset + op_ctx->cur, impl->with, 
        impl->trav, impl->cr_with, impl->cr_trav);

    if (!up) {
        /* Component is not reachable from table */
        goto next_row;
    }

    it->sources[op->field_index] = flecs_entities_get_alive(
        ctx->world, up->src);
    it->trs[op->field_index] = up->tr;
    it->ids[op->field_index] = up->id;
    if (op->match_flags & EcsTermMatchAny) {
        it->ids[op->field_index] = ecs_pair(impl->trav, EcsWildcard);
    }
    
    flecs_query_set_vars(op, up->id, ctx);
    flecs_set_source_set_flag(it, op->field_index);

    return true;
}

/* Check if a table can reach the target component through the traversal
 * relationship, or if the table has the target component itself. */
bool flecs_query_self_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_query_up_impl_t *impl = op_ctx->impl;
    ecs_iter_t *it = ctx->it;

    if (!redo) {        
        if (!op_ctx->impl) {
            ecs_allocator_t *a = flecs_query_get_allocator(it);
            impl = op_ctx->impl = flecs_calloc_t(a, ecs_query_up_impl_t);
        }

        flecs_reset_source_set_flag(ctx->it, op->field_index);
        op_ctx->cur = -1;
        impl->trav = 0;
        impl->with = flecs_query_op_get_id(op, ctx);
    }

    /* First check if table has the component */
    if (op_ctx->impl->trav == 0) {
        bool result;
        ecs_id_t with = impl->with;
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
            /* Table has component, no need to traverse*/            
            if (flecs_query_ref_flags(op->flags, EcsQuerySrc) & EcsQueryIsVar) {
                /* Matching self, so set sources to 0 */
                it->sources[op->field_index] = 0;
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
