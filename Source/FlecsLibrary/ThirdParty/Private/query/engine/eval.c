/**
 * @file query/engine/eval.c
 * @brief Query engine implementation.
 */

#include "../../private_api.h"

// #define FLECS_QUERY_TRACE

#ifdef FLECS_QUERY_TRACE
static int flecs_query_trace_indent = 0;
#endif

typedef bool (*flecs_query_func_t)(
    const ecs_query_op_t *,
    bool,
    ecs_query_run_ctx_t *);

static inline
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

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsQuerySrc, ctx);
    if (!table) {
        return false;
    }

    if (!redo) {
        const ecs_id_t id = flecs_query_op_get_id(op, ctx);
        if (!idr || idr->id != id) {
            idr = op_ctx->idr = flecs_id_record_get(ctx->world, id);
            if (!idr) {
                return false;
            }
        }

        ecs_table_record_t* tr = flecs_id_record_get_table(idr, table);
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

static inline
bool flecs_query_and(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const uint64_t written = ctx->written[ctx->op_index];
    return written & 1ull << op->src.var ? flecs_query_with(op, redo, ctx) : flecs_query_select(op, redo, ctx);
}

bool flecs_query_select_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    ecs_flags32_t table_filter)
{
    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_iter_t *it = ctx->it;
    const int8_t field = op->field_index;
    ecs_assert(field != -1, ECS_INTERNAL_ERROR, NULL);

    if (!redo) {
        const ecs_id_t id = it->ids[field];
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
    const int8_t field = op->field_index;
    ecs_assert(field != -1, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsQuerySrc, ctx);
    if (!table) {
        return false;
    }

    const ecs_id_t id = it->ids[field];
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

static inline
bool flecs_query_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const uint64_t written = ctx->written[ctx->op_index];
    return flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)
                ? flecs_query_up_with(op, redo, ctx) : flecs_query_up_select(op, redo, ctx, 
                   FlecsQueryUpSelectUp, FlecsQueryUpSelectDefault);
}

static
bool flecs_query_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const uint64_t written = ctx->written[ctx->op_index];
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
    const ecs_flags16_t match_flags = op->match_flags;
    if (redo) {
        if (match_flags & EcsTermMatchAnySrc) {
            return false;
        }
    }

    const uint64_t written = ctx->written[ctx->op_index];
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

    const int32_t field = op->field_index;
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
    const uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsQuerySrc, written)) {
        return flecs_query_and_any(op, redo, ctx);
    } else {
        return flecs_query_select_w_id(op, redo, ctx, EcsAny, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
    }
}

static inline
bool flecs_query_triv(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_trivial_ctx_t *op_ctx = flecs_op_ctx(ctx, trivial);
    const ecs_flags64_t termset = op->src.entity;
    const uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        flecs_query_set_iter_this(ctx->it, ctx);
        return flecs_query_trivial_test(ctx, redo, termset);
    } else {
        return flecs_query_trivial_search(ctx, op_ctx, redo, termset);
    }
}

static inline
bool flecs_query_cache(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    (void)redo;

    const uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    return written & 1ull ? flecs_query_cache_test(ctx, redo) : flecs_query_cache_search(ctx);
}

static inline
bool flecs_query_is_cache(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;

    const uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    return written & 1ull ? flecs_query_is_cache_test(ctx, redo) : flecs_query_is_cache_search(ctx);
}

static inline
int32_t flecs_query_next_inheritable_id(
    ecs_world_t *world,
    ecs_type_t *type,
    int32_t index)
{
    for (int32_t i = index; i < type->count; i ++) {
        const ecs_id_record_t *idr = flecs_id_record_get(world, type->array[i]);
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
        const ecs_id_t id = flecs_query_op_get_id(op, ctx);
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

    const ecs_id_t *ids = type->array;

    /* Check if source is variable, and if it's already written */
    bool src_written = true;
    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        const uint64_t written = ctx->written[ctx->op_index];
        src_written = written & (1ull << op->src.var);
    }

    do {
        int32_t id_index = op_ctx->cur_id_index;

        /* If source is not yet written, find tables with first id */
        if (!src_written) {
            const ecs_entity_t first_id = ids[id_index];

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
    const ecs_id_t id = flecs_query_op_get_id(op, ctx);

    {
        cur = flecs_id_record_get(ctx->world, id);
        if (!cur || !cur->cache.tables.count) {
            return false;
        }
    }

    flecs_query_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        const ecs_iter_t *it = ctx->it;
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
        const ecs_id_t id = flecs_query_op_get_id(op, ctx);
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
        const ecs_id_t id = flecs_query_op_get_id_w_written(op, op->written, ctx);
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
        const ecs_id_t id = flecs_query_op_get_id(op, ctx);
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
        const ecs_id_t id = flecs_query_op_get_id_w_written(op, op->written, ctx);
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

    const ecs_table_range_t range = flecs_query_var_get_range(op->first.var, ctx);
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
    const ecs_entity_t first = flecs_query_var_get_entity(op->first.var, ctx);
    const ecs_query_var_t *var = &query->vars[op->src.var];

    const ecs_entity_t result = ecs_lookup_path_w_sep(ctx->world, first, var->lookup,
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
    const ecs_var_id_t *src_vars = query->src_vars;
    const ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    const ecs_flags32_t up_fields = it->up_fields;
    for (int32_t i = 0; i < q->field_count; i ++) {
        const ecs_var_id_t var_id = src_vars[i];
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
    const ecs_var_t *this_var = &vars[op->first.var];

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
    const ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    for (int32_t i = 0; i < q->term_count; i ++) {
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
    const ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    for (int32_t i = 0; i < q->term_count; i ++) {
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

    const ecs_var_id_t src_id = op->src.var;
    const ecs_var_id_t first_id = op->first.var;

    const ecs_table_t *table = flecs_query_var_get_table(src_id, ctx);

    const ecs_entity_t e = flecs_query_var_get_entity(first_id, ctx);
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

    const ecs_iter_t *it = ctx->it;
    const ecs_id_t id = it->ids[op->field_index];
    return ECS_PAIR_FIRST(id) == ECS_PAIR_SECOND(id);
}

static
void flecs_query_reset_after_block(
    const ecs_query_op_t *start_op,
    ecs_query_run_ctx_t *ctx,
    ecs_query_ctrl_ctx_t *op_ctx,
    bool result)
{
    const ecs_query_lbl_t op_index = start_op->next;
    const ecs_query_op_t *op = &ctx->qit->ops[op_index];

    const int32_t field = op->field_index;
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
    const uint64_t *written = ctx->written;
    const uint64_t written_cur = written[op->prev + 1];
    const ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    const ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);

    /* Overwrite id with cleared out variables */
    const ecs_id_t id = flecs_query_op_get_id(op, ctx);
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
    const ecs_query_iter_t *qit = &it->priv_.iter.query;

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
    const ecs_query_lbl_t last_for_cur = flecs_query_last_op_for_or_cond(
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
    const ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);

    const ecs_query_lbl_t first = flecs_itolbl(ctx->op_index + 1);
    if (!redo) {
        op_ctx->op_index = first;
    }

    const ecs_query_op_t *ops = qit->ops;
    const ecs_query_op_t *first_op = &ops[first - 1];
    const ecs_query_lbl_t last = first_op->next;
    const ecs_query_op_t *last_op = &ops[last];
    const ecs_query_op_t *cur_op = &ops[op_ctx->op_index];
    bool result = false;

    do {
        const ecs_query_lbl_t cur = op_ctx->op_index;
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
            const ecs_query_lbl_t prev = first;
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

            const int16_t field_index = op->field_index;
            const ecs_id_t prev_id = it->ids[field_index];
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
        const uint64_t written = ctx->written[ctx->op_index];
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
    if (flecs_query_ref_flags(op->flags, ref_kind) == EcsQueryIsVar) {
        bool result = ctx->vars[ref->var].entity != EcsWildcard;
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
    const ecs_iter_t *it = ctx->it;
    const int8_t field_index = flecs_ito(int8_t, op->other);

    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);
    if (!redo) {
        op_ctx->is_set = ecs_field_is_set(it, field_index);
    }

    if (!op_ctx->is_set) {
        return !redo;
    }

    return flecs_query_run_block(redo, ctx, op_ctx);
}

static inline
bool flecs_query_end(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op; (void)ctx;
    return !redo;
}

static inline
bool flecs_query_dispatch(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    static const flecs_query_func_t flecs_query_jump_table[] = {
        [EcsQueryAnd] = flecs_query_and,
        [EcsQueryAndAny] = flecs_query_and_any,
        [EcsQueryTriv] = flecs_query_triv,
        [EcsQueryCache] = flecs_query_cache,
        [EcsQueryIsCache] = flecs_query_is_cache,
        [EcsQueryOnlyAny] = flecs_query_only_any,
        [EcsQueryUp] = flecs_query_up,
        [EcsQuerySelfUp] = flecs_query_self_up,
        [EcsQueryWith] = flecs_query_with,
        [EcsQueryTrav] = flecs_query_trav,
        [EcsQueryAndFrom] = flecs_query_and_from,
        [EcsQueryNotFrom] = flecs_query_not_from,
        [EcsQueryOrFrom] = flecs_query_or_from,
        [EcsQueryIds] = flecs_query_ids,
        [EcsQueryIdsRight] = flecs_query_idsright,
        [EcsQueryIdsLeft] = flecs_query_idsleft,
        [EcsQueryEach] = flecs_query_each,
        [EcsQueryStore] = flecs_query_store,
        [EcsQueryReset] = flecs_query_reset,
        [EcsQueryOr] = flecs_query_or,
        [EcsQueryOptional] = flecs_query_optional,
        [EcsQueryIfVar] = flecs_query_if_var,
        [EcsQueryIfSet] = flecs_query_if_set,
        [EcsQueryEnd] = flecs_query_end,
        [EcsQueryNot] = flecs_query_not,
        [EcsQueryPredEq] = flecs_query_pred_eq,
        [EcsQueryPredNeq] = flecs_query_pred_neq,
        [EcsQueryPredEqName] = flecs_query_pred_eq_name,
        [EcsQueryPredNeqName] = flecs_query_pred_neq_name,
        [EcsQueryPredEqMatch] = flecs_query_pred_eq_match,
        [EcsQueryPredNeqMatch] = flecs_query_pred_neq_match,
        [EcsQueryMemberEq] = flecs_query_member_eq,
        [EcsQueryMemberNeq] = flecs_query_member_neq,
        [EcsQueryToggle] = flecs_query_toggle,
        [EcsQueryToggleOption] = flecs_query_toggle_option,
        [EcsQueryUnionEq] = flecs_query_union,
        [EcsQueryUnionEqWith] = (flecs_query_func_t)flecs_query_union_with, // cast due to extra argument
        [EcsQueryUnionNeq] = flecs_query_union_neq,
        [EcsQueryUnionEqUp] = flecs_query_union_up,
        [EcsQueryUnionEqSelfUp] = flecs_query_union_self_up,
        [EcsQueryLookup] = flecs_query_lookup,
        [EcsQuerySetVars] = flecs_query_setvars,
        [EcsQuerySetThis] = flecs_query_setthis,
        [EcsQuerySetFixed] = flecs_query_setfixed,
        [EcsQuerySetIds] = flecs_query_setids,
        [EcsQuerySetId] = flecs_query_setid,
        [EcsQueryContain] = flecs_query_contain,
        [EcsQueryPairEq] = flecs_query_pair_eq,
        [EcsQueryYield] = NULL,   // returns false by definition
        [EcsQueryNothing] = NULL  // returns false by definition
    };

    flecs_query_func_t fn = flecs_query_jump_table[op->kind];
    return fn && fn(op, redo, ctx);
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

    ecs_os_perf_trace_pop("flecs.query.run_until");

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
            ecs_os_perf_trace_pop("flecs.query.run_until");
            return false;
        }
    } while (op < last_op);
    
#ifdef FLECS_QUERY_TRACE
        printf("%*strue\n", flecs_query_trace_indent*2, "");
        flecs_query_trace_indent --;
#endif

    ecs_os_perf_trace_pop("flecs.query.run_until");
    return true;
}
