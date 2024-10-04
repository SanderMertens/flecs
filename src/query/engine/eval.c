/**
 * @file query/engine/eval.c
 * @brief Query engine implementation.
 */

#include "../../private_api.h"

// #define FLECS_QUERY_TRACE

#ifdef FLECS_QUERY_TRACE
static int flecs_query_trace_indent = 0;
#endif

static
bool flecs_query_dispatch(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

bool flecs_query_select_w_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_id_t id,
    ecs_flags32_t filter_mask)
{
    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_id_record_t *idr = op_ctx->idr;
    ecs_table_record_t *tr;
    ecs_table_t *table;

    if (!redo) {
        if (!idr || idr->id != id) {
            idr = op_ctx->idr = flecs_id_record_get(ctx->world, id);
            if (!idr) {
                return false;
            }
        }

        if (ctx->query->pub.flags & EcsQueryMatchEmptyTables) {
            if (!flecs_table_cache_all_iter(&idr->cache, &op_ctx->it)) {
                return false;
            }
        } else {
            if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
                return false;
            }
        }
    }

repeat:
    if (!redo || !op_ctx->remaining) {
        tr = flecs_table_cache_next(&op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        op_ctx->column = flecs_ito(int16_t, tr->index);
        op_ctx->remaining = flecs_ito(int16_t, tr->count - 1);
        table = tr->hdr.table;
        flecs_query_var_set_range(op, op->src.var, table, 0, 0, ctx);
    } else {
        tr = (ecs_table_record_t*)op_ctx->it.cur;
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        table = tr->hdr.table;
        op_ctx->column = flecs_query_next_column(table, idr->id, op_ctx->column);
        op_ctx->remaining --;
    }

    if (flecs_query_table_filter(table, op->other, filter_mask)) {
        goto repeat;
    }

    flecs_query_set_match(op, table, op_ctx->column, ctx);
    return true;
}

bool flecs_query_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_id_t id = 0;
    if (!redo) {
        id = flecs_query_op_get_id(op, ctx);
    }
    return flecs_query_select_w_id(op, redo, ctx, id, 
        (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
}

bool flecs_query_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_id_record_t *idr = op_ctx->idr;
    ecs_table_record_t *tr;

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsQuerySrc, ctx);
    if (!table) {
        return false;
    }

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        if (!idr || idr->id != id) {
            idr = op_ctx->idr = flecs_id_record_get(ctx->world, id);
            if (!idr) {
                return false;
            }
        }

        tr = flecs_id_record_get_table(idr, table);
        if (!tr) {
            return false;
        }

        op_ctx->column = flecs_ito(int16_t, tr->index);
        op_ctx->remaining = flecs_ito(int16_t, tr->count);
        op_ctx->it.cur = &tr->hdr;
    } else {
        ecs_assert((op_ctx->remaining + op_ctx->column - 1) < table->type.count, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(op_ctx->remaining >= 0, ECS_INTERNAL_ERROR, NULL);
        if (--op_ctx->remaining <= 0) {
            return false;
        }

        op_ctx->column = flecs_query_next_column(table, idr->id, op_ctx->column);
        ecs_assert(op_ctx->column != -1, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_query_set_match(op, table, op_ctx->column, ctx);
    return true;
}

static
bool flecs_query_and(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_with(op, redo, ctx);
    } else {
        return flecs_query_select(op, redo, ctx);
    }
}

bool flecs_query_select_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_filter)
{
    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_iter_t *it = ctx->it;
    int8_t field = op->field_index;
    ecs_assert(field != -1, ECS_INTERNAL_ERROR, NULL);

    if (!redo) {
        ecs_id_t id = it->ids[field];
        ecs_id_record_t *idr = op_ctx->idr;
        if (!idr || idr->id != id) {
            idr = op_ctx->idr = flecs_id_record_get(ctx->world, id);
            if (!idr) {
                return false;
            }
        }

        if (ctx->query->pub.flags & EcsQueryMatchEmptyTables) {
            if (!flecs_table_cache_all_iter(&idr->cache, &op_ctx->it)) {
                return false;
            }
        } else {
            if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
                return false;
            }
        }
    }

repeat: {}
    const ecs_table_record_t *tr = flecs_table_cache_next(
        &op_ctx->it, ecs_table_record_t);
    if (!tr) {
        return false;
    }

    ecs_table_t *table = tr->hdr.table;
    if (flecs_query_table_filter(table, op->other, table_filter)) {
        goto repeat;
    }

    flecs_query_var_set_range(op, op->src.var, table, 0, 0, ctx);
    flecs_query_it_set_tr(it, field, tr);
    return true;
}

bool flecs_query_with_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_iter_t *it = ctx->it;
    int8_t field = op->field_index;
    ecs_assert(field != -1, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsQuerySrc, ctx);
    if (!table) {
        return false;
    }

    ecs_id_t id = it->ids[field];
    ecs_id_record_t *idr = op_ctx->idr;
    if (!idr || idr->id != id) {
        idr = op_ctx->idr = flecs_id_record_get(ctx->world, id);
        if (!idr) {
            return false;
        }
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        return false;
    }

    flecs_query_it_set_tr(it, field, tr);
    return true;
}

static
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

static
bool flecs_query_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        return flecs_query_self_up_with(op, redo, ctx, false);
    } else {
        return flecs_query_up_select(op, redo, ctx, 
            FlecsQueryUpSelectSelfUp, FlecsQueryUpSelectDefault);
    }
}

static
bool flecs_query_and_any(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t match_flags = op->match_flags;
    if (redo) {
        if (match_flags & EcsTermMatchAnySrc) {
            return false;
        }
    }

    uint64_t written = ctx->written[ctx->op_index];
    int32_t remaining = 1;
    bool result;
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        result = flecs_query_with(op, redo, ctx);
    } else {
        result = flecs_query_select(op, redo, ctx);
        remaining = 0;
    }

    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);

    if (match_flags & EcsTermMatchAny && op_ctx->remaining) {
        op_ctx->remaining = flecs_ito(int16_t, remaining);
    }

    int32_t field = op->field_index;
    if (field != -1) {
        ctx->it->ids[field] = flecs_query_op_get_id(op, ctx);
    }

    ctx->it->trs[field] = (ecs_table_record_t*)op_ctx->it.cur;

    return result;
}

static
bool flecs_query_only_any(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        return flecs_query_and_any(op, redo, ctx);
    } else {
        return flecs_query_select_w_id(op, redo, ctx, EcsAny, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
    }
}

static
bool flecs_query_triv(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_trivial_ctx_t *op_ctx = flecs_op_ctx(ctx, trivial);
    ecs_flags64_t termset = op->src.entity;
    uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        flecs_query_set_iter_this(ctx->it, ctx);
        return flecs_query_trivial_test(ctx, redo, termset);
    } else {
        return flecs_query_trivial_search(ctx, op_ctx, redo, termset);
    }
}

static
bool flecs_query_cache(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    (void)redo;

    uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        return flecs_query_cache_test(ctx, redo);
    } else {
        return flecs_query_cache_search(ctx);
    }
}

static
bool flecs_query_is_cache(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;

    uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        return flecs_query_is_cache_test(ctx, redo);
    } else {
        return flecs_query_is_cache_search(ctx);
    }
}

static
int32_t flecs_query_next_inheritable_id(
    ecs_world_t *world,
    ecs_type_t *type,
    int32_t index)
{
    int32_t i;
    for (i = index; i < type->count; i ++) {
        ecs_id_record_t *idr = flecs_id_record_get(world, type->array[i]);
        if (!(idr->flags & EcsIdOnInstantiateDontInherit)) {
            return i;
        }
    }
    return -1;
}

static
bool flecs_query_x_from(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_oper_kind_t oper)
{
    ecs_query_xfrom_ctx_t *op_ctx = flecs_op_ctx(ctx, xfrom);
    ecs_world_t *world = ctx->world;
    ecs_type_t *type;
    int32_t i;

    if (!redo) {
        /* Find entity that acts as the template from which we match the ids */
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        ecs_assert(ecs_is_alive(world, id), ECS_INTERNAL_ERROR, NULL);
        ecs_record_t *r = flecs_entities_get(world, id);
        ecs_table_t *table;
        if (!r || !(table = r->table)) {
            /* Nothing to match */
            return false;
        }

        /* Find first id to test against. Skip ids with DontInherit flag. */
        type = op_ctx->type = &table->type;
        op_ctx->first_id_index = flecs_query_next_inheritable_id(
            world, type, 0);
        op_ctx->cur_id_index = op_ctx->first_id_index;

        if (op_ctx->cur_id_index == -1) {
            return false; /* No ids to filter on */
        }
    } else {
        type = op_ctx->type;
    }

    ecs_id_t *ids = type->array;

    /* Check if source is variable, and if it's already written */
    bool src_written = true;
    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        uint64_t written = ctx->written[ctx->op_index];
        src_written = written & (1ull << op->src.var);
    }

    do {
        int32_t id_index = op_ctx->cur_id_index;

        /* If source is not yet written, find tables with first id */
        if (!src_written) {
            ecs_entity_t first_id = ids[id_index];

            if (!flecs_query_select_w_id(op, redo, ctx, 
                first_id, (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
            {
                if (oper == EcsOrFrom) {
                    id_index = flecs_query_next_inheritable_id(
                        world, type, id_index + 1);
                    if (id_index != -1) {
                        op_ctx->cur_id_index = id_index;
                        redo = false;
                        continue;
                    }
                }

                return false;
            }

            id_index ++; /* First id got matched */
        } else if (redo && src_written) {
            return false;
        }

        ecs_table_t *src_table = flecs_query_get_table(
            op, &op->src, EcsQuerySrc, ctx);
        if (!src_table) {
            continue;
        }
        
        redo = true;

        if (!src_written && oper == EcsOrFrom) {
            /* Eliminate duplicate matches from tables that have multiple 
             * components from the type list */
            if (op_ctx->cur_id_index != op_ctx->first_id_index) {
                for (i = op_ctx->first_id_index; i < op_ctx->cur_id_index; i ++) {
                    ecs_id_record_t *idr = flecs_id_record_get(world, ids[i]);
                    if (!idr) {
                        continue;
                    }

                    if (idr->flags & EcsIdOnInstantiateDontInherit) {
                        continue;
                    }
                    
                    if (flecs_id_record_get_table(idr, src_table) != NULL) {
                        /* Already matched */
                        break;
                    }
                }
                if (i != op_ctx->cur_id_index) {
                    continue;
                }
            }
            return true;
        }

        if (oper == EcsAndFrom || oper == EcsNotFrom || src_written) {
            for (i = id_index; i < type->count; i ++) {
                ecs_id_record_t *idr = flecs_id_record_get(world, ids[i]);
                if (!idr) {
                    if (oper == EcsAndFrom) {
                        return false;
                    } else {
                        continue;
                    }
                }

                if (idr->flags & EcsIdOnInstantiateDontInherit) {
                    continue;
                }

                if (flecs_id_record_get_table(idr, src_table) == NULL) {
                    if (oper == EcsAndFrom) {
                        break; /* Must have all ids */
                    }
                } else {
                    if (oper == EcsNotFrom) {
                        break; /* Must have none of the ids */
                    } else if (oper == EcsOrFrom) {
                        return true; /* Single match is enough */
                    }
                }
            }

            if (i == type->count) {
                if (oper == EcsAndFrom || oper == EcsNotFrom) {
                    break; /* All ids matched */
                }
            }
        }
    } while (true);

    return true;
}

static
bool flecs_query_and_from(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_x_from(op, redo, ctx, EcsAndFrom);
}

static
bool flecs_query_not_from(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_x_from(op, redo, ctx, EcsNotFrom);
}

static
bool flecs_query_or_from(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_x_from(op, redo, ctx, EcsOrFrom);
}

static
bool flecs_query_ids(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_id_record_t *cur;
    ecs_id_t id = flecs_query_op_get_id(op, ctx);

    {
        cur = flecs_id_record_get(ctx->world, id);
        if (!cur || !cur->cache.tables.count) {
            return false;
        }
    }

    flecs_query_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        ecs_iter_t *it = ctx->it;
        it->ids[op->field_index] = id;
        it->sources[op->field_index] = EcsWildcard;
        it->trs[op->field_index] = NULL; /* Mark field as set */
    }

    return true;
}

static
bool flecs_query_idsright(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_ids_ctx_t *op_ctx = flecs_op_ctx(ctx, ids);
    ecs_id_record_t *cur;

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        if (!ecs_id_is_wildcard(id)) {
            /* If id is not a wildcard, we can directly return it. This can 
             * happen if a variable was constrained by an iterator. */
            op_ctx->cur = NULL;
            flecs_query_set_vars(op, id, ctx);
            return true;
        }

        cur = op_ctx->cur = flecs_id_record_get(ctx->world, id);
        if (!cur) {
            return false;
        }
    } else {
        if (!op_ctx->cur) {
            return false;
        }
    }

    do {
        cur = op_ctx->cur = op_ctx->cur->first.next;
    } while (cur && !cur->cache.tables.count); /* Skip empty ids */

    if (!cur) {
        return false;
    }

    flecs_query_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        ecs_iter_t *it = ctx->it;
        ecs_id_t id = flecs_query_op_get_id_w_written(op, op->written, ctx);
        it->ids[op->field_index] = id;
        it->sources[op->field_index] = EcsWildcard;
        ECS_TERMSET_SET(it->set_fields, 1u << op->field_index);
    }

    return true;
}

static
bool flecs_query_idsleft(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_ids_ctx_t *op_ctx = flecs_op_ctx(ctx, ids);
    ecs_id_record_t *cur;

    if (!redo) {
        ecs_id_t id = flecs_query_op_get_id(op, ctx);
        if (!ecs_id_is_wildcard(id)) {
            /* If id is not a wildcard, we can directly return it. This can 
             * happen if a variable was constrained by an iterator. */
            op_ctx->cur = NULL;
            flecs_query_set_vars(op, id, ctx);
            return true;
        }

        cur = op_ctx->cur = flecs_id_record_get(ctx->world, id);
        if (!cur) {
            return false;
        }
    } else {
        if (!op_ctx->cur) {
            return false;
        }
    }

    do {
        cur = op_ctx->cur = op_ctx->cur->second.next;
    } while (cur && !cur->cache.tables.count); /* Skip empty ids */ 

    if (!cur) {
        return false;
    }

    flecs_query_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        ecs_iter_t *it = ctx->it;
        ecs_id_t id = flecs_query_op_get_id_w_written(op, op->written, ctx);
        it->ids[op->field_index] = id;
        it->sources[op->field_index] = EcsWildcard;
        ECS_TERMSET_SET(it->set_fields, 1u << op->field_index);
    }

    return true;
}

static
bool flecs_query_each(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_each_ctx_t *op_ctx = flecs_op_ctx(ctx, each);
    int32_t row;

    ecs_table_range_t range = flecs_query_var_get_range(op->first.var, ctx);
    ecs_table_t *table = range.table;
    if (!table) {
        return false;
    }

    if (!redo) {
        if (!ecs_table_count(table)) {
            return false;
        }
        row = op_ctx->row = range.offset;
    } else {
        int32_t end = range.count;
        if (end) {
            end += range.offset;
        } else {
            end = ecs_table_count(table);
        }
        row = ++ op_ctx->row;
        if (op_ctx->row >= end) {
            return false;
        }
    }

    ecs_assert(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);
    const ecs_entity_t *entities = ecs_table_entities(table);
    flecs_query_var_set_entity(op, op->src.var, entities[row], ctx);

    return true;
}

static
bool flecs_query_store(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (!redo) {
        flecs_query_var_set_entity(op, op->src.var, op->first.entity, ctx);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_query_reset(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (!redo) {
        return true;
    } else {
        flecs_query_var_reset(op->src.var, ctx);
        return false;
    }
}

static
bool flecs_query_lookup(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    const ecs_query_impl_t *query = ctx->query;
    ecs_entity_t first = flecs_query_var_get_entity(op->first.var, ctx);
    ecs_query_var_t *var = &query->vars[op->src.var];

    ecs_entity_t result = ecs_lookup_path_w_sep(ctx->world, first, var->lookup,
        NULL, NULL, false);
    if (!result) {
        flecs_query_var_set_entity(op, op->src.var, EcsWildcard, ctx);
        return false;
    }

    flecs_query_var_set_entity(op, op->src.var, result, ctx);

    return true;
}

static
bool flecs_query_setvars(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;

    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_var_id_t *src_vars = query->src_vars;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    ecs_flags32_t up_fields = it->up_fields;
    for (i = 0; i < q->field_count; i ++) {
        ecs_var_id_t var_id = src_vars[i];
        if (!var_id) {
            continue;
        }

        if (up_fields & (1u << i)) {
            continue;
        }

        it->sources[i] = flecs_query_var_get_entity(var_id, ctx);
    }

    return true;
}

static
bool flecs_query_setthis(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_query_setthis_ctx_t *op_ctx = flecs_op_ctx(ctx, setthis);
    ecs_var_t *vars = ctx->vars;
    ecs_var_t *this_var = &vars[op->first.var];

    if (!redo) {
        /* Save values so we can restore them later */
        op_ctx->range = vars[0].range;

        /* Constrain This table variable to a single entity from the table */
        vars[0].range = flecs_range_from_entity(this_var->entity, ctx);
        vars[0].entity = this_var->entity;
        return true;
    } else {
        /* Restore previous values, so that instructions that are operating on
         * the table variable use all the entities in the table. */
        vars[0].range = op_ctx->range;
        vars[0].entity = 0;
        return false;
    }
}

static
bool flecs_query_setfixed(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    for (i = 0; i < q->term_count; i ++) {
        const ecs_term_t *term = &q->terms[i];
        const ecs_term_ref_t *src = &term->src;
        if (src->id & EcsIsEntity) {
            it->sources[term->field_index] = ECS_TERM_REF_ID(src);
        }
    }

    return true;
}

bool flecs_query_setids(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    const ecs_query_impl_t *query = ctx->query;
    const ecs_query_t *q = &query->pub;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    for (i = 0; i < q->term_count; i ++) {
        const ecs_term_t *term = &q->terms[i];
        it->ids[term->field_index] = term->id;
    }

    return true;
}

static
bool flecs_query_setid(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_assert(op->field_index != -1, ECS_INTERNAL_ERROR, NULL);
    ctx->it->ids[op->field_index] = op->first.entity;
    return true;
}

/* Check if entity is stored in table */
static
bool flecs_query_contain(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_var_id_t src_id = op->src.var;
    ecs_var_id_t first_id = op->first.var;

    ecs_table_t *table = flecs_query_var_get_table(src_id, ctx);

    ecs_entity_t e = flecs_query_var_get_entity(first_id, ctx);
    return table == ecs_get_table(ctx->world, e);
}

/* Check if first and second id of pair from last operation are the same */
static
bool flecs_query_pair_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t id = it->ids[op->field_index];
    return ECS_PAIR_FIRST(id) == ECS_PAIR_SECOND(id);
}

static
void flecs_query_reset_after_block(
    const ecs_query_op_t *start_op,
    ecs_query_run_ctx_t *ctx,
    ecs_query_ctrl_ctx_t *op_ctx,
    bool result)
{
    ecs_query_lbl_t op_index = start_op->next;
    const ecs_query_op_t *op = &ctx->qit->ops[op_index];

    int32_t field = op->field_index;
    if (field == -1) {
        goto done;
    }

    /* Set/unset field */
    ecs_iter_t *it = ctx->it;
    if (result) {
        ECS_TERMSET_SET(it->set_fields, 1u << field);
        return;
    }

    /* Reset state after a field was not matched */
    ctx->written[op_index] = ctx->written[ctx->op_index];
    ctx->op_index = op_index;
    ECS_TERMSET_CLEAR(it->set_fields, 1u << field);

    /* Ignore variables written by Not operation */
    uint64_t *written = ctx->written;
    uint64_t written_cur = written[op->prev + 1];
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);

    /* Overwrite id with cleared out variables */
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    if (id) {
        it->ids[field] = id;
    }

    it->trs[field] = NULL;

    /* Reset variables */
    if (flags_1st & EcsQueryIsVar) {
        if (!flecs_ref_is_written(op, &op->first, EcsQueryFirst, written_cur)){
            flecs_query_var_reset(op->first.var, ctx);
        }
    }
    if (flags_2nd & EcsQueryIsVar) {
        if (!flecs_ref_is_written(op, &op->second, EcsQuerySecond, written_cur)){
            flecs_query_var_reset(op->second.var, ctx);
        }
    }

    /* If term has entity src, set it because no other instruction might */
    if (op->flags & (EcsQueryIsEntity << EcsQuerySrc)) {
        it->sources[field] = op->src.entity;
    }

done:
    op_ctx->op_index = op_index;
}

static
bool flecs_query_run_block(
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_query_ctrl_ctx_t *op_ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;

    if (!redo) {
        op_ctx->op_index = flecs_itolbl(ctx->op_index + 1);
    } else if (ctx->qit->ops[op_ctx->op_index].kind == EcsQueryEnd) {
        return false;
    }

    ctx->written[ctx->op_index + 1] = ctx->written[ctx->op_index];

    const ecs_query_op_t *op = &ctx->qit->ops[ctx->op_index];
    bool result = flecs_query_run_until(
        redo, ctx, qit->ops, ctx->op_index, op_ctx->op_index, op->next);

    op_ctx->op_index = flecs_itolbl(ctx->op_index - 1);
    return result;
}

static
ecs_query_lbl_t flecs_query_last_op_for_or_cond(
    const ecs_query_op_t *ops,
    ecs_query_lbl_t cur,
    ecs_query_lbl_t last)
{
    const ecs_query_op_t *cur_op, *last_op = &ops[last];

    do {
        cur_op = &ops[cur];
        cur ++;
    } while (cur_op->next != last && cur_op != last_op);

    return cur;
}

static
bool flecs_query_run_until_for_select_or(
    bool redo,
    ecs_query_run_ctx_t *ctx,
    const ecs_query_op_t *ops,
    ecs_query_lbl_t first,
    ecs_query_lbl_t cur,
    int32_t last)
{
    ecs_query_lbl_t last_for_cur = flecs_query_last_op_for_or_cond(
        ops, cur, flecs_itolbl(last));
    if (redo) {
        /* If redoing, start from the last instruction of the last executed 
         * sequence */
        cur = flecs_itolbl(last_for_cur - 1);
    }

    flecs_query_run_until(redo, ctx, ops, first, cur, last_for_cur);
#ifdef FLECS_QUERY_TRACE
    printf("%*s%s (or)\n", (flecs_query_trace_indent + 1)*2, "",
        ctx->op_index == last ? "true" : "false");
#endif
    return ctx->op_index == last;
}

static
bool flecs_query_select_or(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);

    ecs_query_lbl_t first = flecs_itolbl(ctx->op_index + 1);
    if (!redo) {
        op_ctx->op_index = first;
    }

    const ecs_query_op_t *ops = qit->ops;
    const ecs_query_op_t *first_op = &ops[first - 1];
    ecs_query_lbl_t last = first_op->next;
    const ecs_query_op_t *last_op = &ops[last];
    const ecs_query_op_t *cur_op = &ops[op_ctx->op_index];
    bool result = false;

    do {
        ecs_query_lbl_t cur = op_ctx->op_index;
        ctx->op_index = cur;
        ctx->written[cur] = op->written;

        result = flecs_query_run_until_for_select_or(
            redo, ctx, ops, flecs_itolbl(first - 1), cur, last);

        if (result) {
            if (first == cur) {
                break;
            }

            /* If a previous operation in the OR chain returned a result for the
             * same matched source, skip it so we don't yield for each matching
             * element in the chain. */

            /* Copy written status so that the variables we just wrote will show
             * up as written for the test. This ensures that the instructions
             * match against the result we already found, vs. starting a new
             * search (the difference between select & with). */
            ecs_query_lbl_t prev = first;
            bool dup_found = false;

            /* While terms of an OR chain always operate on the same source, it
             * is possible that a table variable is resolved to an entity 
             * variable. When checking for duplicates, copy the entity variable
             * to the table, to ensure we're only testing the found entity. */
            const ecs_query_op_t *prev_op = &ops[cur - 1];
            ecs_var_t old_table_var;
            ecs_os_memset_t(&old_table_var, 0, ecs_var_t);
            bool restore_table_var = false;

            if (prev_op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
                if (first_op->src.var != prev_op->src.var) {
                    restore_table_var = true;
                    old_table_var = ctx->vars[first_op->src.var];
                    ctx->vars[first_op->src.var] = 
                        ctx->vars[prev_op->src.var];
                }
            }

            int16_t field_index = op->field_index;
            ecs_id_t prev_id = it->ids[field_index];
            const ecs_table_record_t *prev_tr = it->trs[field_index];

            do {
                ctx->written[prev] = ctx->written[last];

                flecs_query_run_until(false, ctx, ops, flecs_itolbl(first - 1), 
                    prev, cur);

                if (ctx->op_index == last) {
                    /* Duplicate match was found, find next result */
                    redo = true;
                    dup_found = true;
                    break;
                }

                break;
            } while (true);

            /* Restore table variable to full range for next result */
            if (restore_table_var) {
                ctx->vars[first_op->src.var] = old_table_var;
            }

            if (dup_found) {
                continue;
            }

            /* Restore id in case op set it */
            it->ids[field_index] = prev_id;
            it->trs[field_index] = prev_tr;
            break;
        }

        /* No result was found, go to next operation in chain */
        op_ctx->op_index = flecs_query_last_op_for_or_cond(
            ops, op_ctx->op_index, last);
        cur_op = &qit->ops[op_ctx->op_index];

        redo = false;
    } while (cur_op != last_op);

    return result;
}

static
bool flecs_query_with_or(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);

    bool result = flecs_query_run_block(redo, ctx, op_ctx);
    if (result) {
        /* If a match was found, no need to keep searching for this source */
        op_ctx->op_index = op->next;
    }

    return result;
}

static
bool flecs_query_or(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        uint64_t written = ctx->written[ctx->op_index];
        if (!(written & (1ull << op->src.var))) {
            return flecs_query_select_or(op, redo, ctx);
        }
    }

    return flecs_query_with_or(op, redo, ctx);
}

static
bool flecs_query_run_block_w_reset(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);

    bool result = flecs_query_run_block(redo, ctx, op_ctx);
    flecs_query_reset_after_block(op, ctx, op_ctx, result);
    return result;
}

static
bool flecs_query_not(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    return !flecs_query_run_block_w_reset(op, redo, ctx);
}

static
bool flecs_query_optional(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{   
    bool result = flecs_query_run_block_w_reset(op, redo, ctx);
    if (!redo) {
        return true; /* Return at least once */
    } else {
        return result;
    }
}

static
bool flecs_query_eval_if(
    const ecs_query_op_t *op,
    ecs_query_run_ctx_t *ctx,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind)
{
    bool result = true;
    if (flecs_query_ref_flags(op->flags, ref_kind) == EcsQueryIsVar) {
        result = ctx->vars[ref->var].entity != EcsWildcard;
        ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);
        flecs_query_reset_after_block(op, ctx, op_ctx, result);
        return result;
    }
    return true;
}

static
bool flecs_query_if_var(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (!redo) {
        if (!flecs_query_eval_if(op, ctx, &op->src, EcsQuerySrc) ||
            !flecs_query_eval_if(op, ctx, &op->first, EcsQueryFirst) ||
            !flecs_query_eval_if(op, ctx, &op->second, EcsQuerySecond))
        {
            return true;
        }
    }

    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);
    return flecs_query_run_block(redo, ctx, op_ctx);
}

static
bool flecs_query_if_set(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    int8_t field_index = flecs_ito(int8_t, op->other);

    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);
    if (!redo) {
        op_ctx->is_set = ecs_field_is_set(it, field_index);
    }

    if (!op_ctx->is_set) {
        return !redo;
    }

    return flecs_query_run_block(redo, ctx, op_ctx);
}

static
bool flecs_query_end(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op; (void)ctx;
    return !redo;
}

static
bool flecs_query_dispatch(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    switch(op->kind) {
    case EcsQueryAnd: return flecs_query_and(op, redo, ctx);
    case EcsQueryAndAny: return flecs_query_and_any(op, redo, ctx);
    case EcsQueryTriv: return flecs_query_triv(op, redo, ctx);
    case EcsQueryCache: return flecs_query_cache(op, redo, ctx);
    case EcsQueryIsCache: return flecs_query_is_cache(op, redo, ctx);
    case EcsQueryOnlyAny: return flecs_query_only_any(op, redo, ctx);
    case EcsQueryUp: return flecs_query_up(op, redo, ctx);
    case EcsQuerySelfUp: return flecs_query_self_up(op, redo, ctx);
    case EcsQueryWith: return flecs_query_with(op, redo, ctx);
    case EcsQueryTrav: return flecs_query_trav(op, redo, ctx);
    case EcsQueryAndFrom: return flecs_query_and_from(op, redo, ctx);
    case EcsQueryNotFrom: return flecs_query_not_from(op, redo, ctx);
    case EcsQueryOrFrom: return flecs_query_or_from(op, redo, ctx);
    case EcsQueryIds: return flecs_query_ids(op, redo, ctx);
    case EcsQueryIdsRight: return flecs_query_idsright(op, redo, ctx);
    case EcsQueryIdsLeft: return flecs_query_idsleft(op, redo, ctx);
    case EcsQueryEach: return flecs_query_each(op, redo, ctx);
    case EcsQueryStore: return flecs_query_store(op, redo, ctx);
    case EcsQueryReset: return flecs_query_reset(op, redo, ctx);
    case EcsQueryOr: return flecs_query_or(op, redo, ctx);
    case EcsQueryOptional: return flecs_query_optional(op, redo, ctx);
    case EcsQueryIfVar: return flecs_query_if_var(op, redo, ctx);
    case EcsQueryIfSet: return flecs_query_if_set(op, redo, ctx);
    case EcsQueryEnd: return flecs_query_end(op, redo, ctx);
    case EcsQueryNot: return flecs_query_not(op, redo, ctx);
    case EcsQueryPredEq: return flecs_query_pred_eq(op, redo, ctx);
    case EcsQueryPredNeq: return flecs_query_pred_neq(op, redo, ctx);
    case EcsQueryPredEqName: return flecs_query_pred_eq_name(op, redo, ctx);
    case EcsQueryPredNeqName: return flecs_query_pred_neq_name(op, redo, ctx);
    case EcsQueryPredEqMatch: return flecs_query_pred_eq_match(op, redo, ctx);
    case EcsQueryPredNeqMatch: return flecs_query_pred_neq_match(op, redo, ctx);
    case EcsQueryMemberEq: return flecs_query_member_eq(op, redo, ctx);
    case EcsQueryMemberNeq: return flecs_query_member_neq(op, redo, ctx);
    case EcsQueryToggle: return flecs_query_toggle(op, redo, ctx);
    case EcsQueryToggleOption: return flecs_query_toggle_option(op, redo, ctx);
    case EcsQueryUnionEq: return flecs_query_union(op, redo, ctx);
    case EcsQueryUnionEqWith: return flecs_query_union_with(op, redo, ctx, false);
    case EcsQueryUnionNeq: return flecs_query_union_neq(op, redo, ctx);
    case EcsQueryUnionEqUp: return flecs_query_union_up(op, redo, ctx);
    case EcsQueryUnionEqSelfUp: return flecs_query_union_self_up(op, redo, ctx);
    case EcsQueryLookup: return flecs_query_lookup(op, redo, ctx);
    case EcsQuerySetVars: return flecs_query_setvars(op, redo, ctx);
    case EcsQuerySetThis: return flecs_query_setthis(op, redo, ctx);
    case EcsQuerySetFixed: return flecs_query_setfixed(op, redo, ctx);
    case EcsQuerySetIds: return flecs_query_setids(op, redo, ctx);
    case EcsQuerySetId: return flecs_query_setid(op, redo, ctx);
    case EcsQueryContain: return flecs_query_contain(op, redo, ctx);
    case EcsQueryPairEq: return flecs_query_pair_eq(op, redo, ctx);
    case EcsQueryYield: return false;
    case EcsQueryNothing: return false;
    }
    return false;
}

bool flecs_query_run_until(
    bool redo,
    ecs_query_run_ctx_t *ctx,
    const ecs_query_op_t *ops,
    ecs_query_lbl_t first,
    ecs_query_lbl_t cur,
    int32_t last)
{
    ecs_assert(first >= -1, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur > first, ECS_INTERNAL_ERROR, NULL);

    ctx->op_index = cur;
    const ecs_query_op_t *op = &ops[ctx->op_index];
    const ecs_query_op_t *last_op = &ops[last];
    ecs_assert(last > first, ECS_INTERNAL_ERROR, NULL);

#ifdef FLECS_QUERY_TRACE
    printf("%*sblock:\n", flecs_query_trace_indent*2, "");
    flecs_query_trace_indent ++;
#endif

    do {
        #ifdef FLECS_DEBUG
        ctx->qit->profile[ctx->op_index].count[redo] ++;
        #endif

#ifdef FLECS_QUERY_TRACE
        printf("%*s%d: %s\n", flecs_query_trace_indent*2, "", 
            ctx->op_index, flecs_query_op_str(op->kind));
#endif

        bool result = flecs_query_dispatch(op, redo, ctx);
        cur = (&op->prev)[result];
        redo = cur < ctx->op_index;

        if (!redo) {
            ctx->written[cur] |= ctx->written[ctx->op_index] | op->written;
        }

        ctx->op_index = cur;
        op = &ops[ctx->op_index];

        if (cur <= first) {
#ifdef FLECS_QUERY_TRACE
            printf("%*sfalse\n", flecs_query_trace_indent*2, "");
            flecs_query_trace_indent --;
#endif
            return false;
        }
    } while (op < last_op);
    
#ifdef FLECS_QUERY_TRACE
        printf("%*strue\n", flecs_query_trace_indent*2, "");
        flecs_query_trace_indent --;
#endif

    return true;
}
