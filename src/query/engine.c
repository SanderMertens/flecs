/**
 * @file addons/rules/engine.c
 * @brief Query engine implementation.
 */

#include "../private_api.h"

// #define FLECS_QUERY_TRACE

#ifdef FLECS_QUERY_TRACE
static int flecs_query_trace_indent = 0;
#endif

static
bool flecs_query_dispatch(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx);

static
bool flecs_query_run_until(
    bool redo,
    ecs_query_run_ctx_t *ctx,
    const ecs_query_op_t *ops,
    ecs_query_lbl_t first,
    ecs_query_lbl_t cur,
    int32_t last);

static
void flecs_query_populate_field(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    ecs_query_run_ctx_t *ctx);

static
void flecs_query_populate_field_from_range(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    int32_t index);

static
void flecs_query_set_iter_this(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_table_range_t *range = &ctx->vars[0].range;
    ecs_table_t *table = range->table;
    int32_t count = range->count;
    if (table) {
        if (!count) {
            count = ecs_table_count(table);
        }
        it->table = table;
        it->offset = range->offset;
        it->count = count;
        it->entities = ECS_ELEM_T(
            table->data.entities.array, ecs_entity_t, it->offset);
    } else if (count == 1) {
        it->count = 1;
        it->entities = &ctx->vars[0].entity;
    }
}

static
ecs_query_op_ctx_t* flecs_op_ctx_(
    const ecs_query_run_ctx_t *ctx)
{
    return &ctx->op_ctx[ctx->op_index];
}

#define flecs_op_ctx(ctx, op_kind) (&flecs_op_ctx_(ctx)->is.op_kind)

static
void flecs_reset_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    it->up_fields &= ~(1u << field_index);
}

static
void flecs_set_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    it->up_fields |= (1u << field_index);
}

static
ecs_table_range_t flecs_range_from_entity(
    ecs_entity_t e,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_record_t *r = flecs_entities_get(ctx->world, e);
    if (!r) {
        return (ecs_table_range_t){ 0 };
    }
    return (ecs_table_range_t){
        .table = r->table,
        .offset = ECS_RECORD_TO_ROW(r->row),
        .count = 1
    };
}

static
ecs_table_range_t flecs_query_var_get_range(
    int32_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(var_id < ctx->rule->var_count, ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    ecs_table_t *table = var->range.table;
    if (table) {
        return var->range;
    }

    ecs_entity_t entity = var->entity;
    if (entity && entity != EcsWildcard) {
        var->range = flecs_range_from_entity(entity, ctx);
        return var->range;
    }

    return (ecs_table_range_t){ 0 };
}

static
ecs_table_t* flecs_query_var_get_table(
    int32_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_var_t *var = &ctx->vars[var_id];
    ecs_table_t *table = var->range.table;
    if (table) {
        return table;
    }

    ecs_entity_t entity = var->entity;
    if (entity && entity != EcsWildcard) {
        var->range = flecs_range_from_entity(entity, ctx);
        return var->range.table;
    }

    return NULL;
}

static
ecs_table_t* flecs_query_get_table(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, ref_kind);
    if (flags & EcsRuleIsEntity) {
        return ecs_get_table(ctx->world, ref->entity);
    } else {
        return flecs_query_var_get_table(ref->var, ctx);
    }
}

static
ecs_table_range_t flecs_query_get_range(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, ref_kind);
    if (flags & EcsRuleIsEntity) {
        ecs_assert(!(flags & EcsRuleIsVar), ECS_INTERNAL_ERROR, NULL);
        return flecs_range_from_entity(ref->entity, ctx);
    } else {
        ecs_var_t *var = &ctx->vars[ref->var];
        if (var->range.table) {
            return ctx->vars[ref->var].range;
        } else if (var->entity) {
            return flecs_range_from_entity(var->entity, ctx);
        }
    }
    return (ecs_table_range_t){0};
}

static
ecs_entity_t flecs_query_var_get_entity(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(var_id < (ecs_var_id_t)ctx->rule->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    ecs_entity_t entity = var->entity;
    if (entity) {
        return entity;
    }

    ecs_assert(var->range.count == 1, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = var->range.table;
    ecs_entity_t *entities = table->data.entities.array;
    var->entity = entities[var->range.offset];
    return var->entity;
}

static
void flecs_query_var_reset(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ctx->vars[var_id].entity = EcsWildcard;
    ctx->vars[var_id].range.table = NULL;
}

static
void flecs_query_var_set_range(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(ctx->rule_vars[var_id].kind == EcsVarTable, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_query_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    var->entity = 0;
    var->range = (ecs_table_range_t){ 
        .table = table,
        .offset = offset,
        .count = count
    };
}

static
void flecs_query_var_narrow_range(
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_query_run_ctx_t *ctx)
{    
    ecs_var_t *var = &ctx->vars[var_id];
    
    var->entity = 0;
    var->range = (ecs_table_range_t){ 
        .table = table,
        .offset = offset,
        .count = count
    };
    if (ctx->rule_vars[var_id].kind != EcsVarTable) {    
        ecs_assert(count == 1, ECS_INTERNAL_ERROR, NULL);
        var->entity = flecs_table_entities_array(table)[offset];
    }
}

static
void flecs_query_var_set_entity(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_entity_t entity,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(var_id < (ecs_var_id_t)ctx->rule->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_query_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    var->range.table = NULL;
    var->entity = entity;
}

static
void flecs_query_set_vars(
    const ecs_query_op_t *op,
    ecs_id_t id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsRuleSecond);

    if (flags_1st & EcsRuleIsVar) {
        ecs_var_id_t var = op->first.var;
        if (op->written & (1ull << var)) {
            if (ECS_IS_PAIR(id)) {
                flecs_query_var_set_entity(
                    op, var, ecs_get_alive(ctx->world, ECS_PAIR_FIRST(id)), ctx);
            } else {
                flecs_query_var_set_entity(op, var, id, ctx);
            }
        }
    }

    if (flags_2nd & EcsRuleIsVar) {
        ecs_var_id_t var = op->second.var;
        if (op->written & (1ull << var)) {
            flecs_query_var_set_entity(
                op, var, ecs_get_alive(ctx->world, ECS_PAIR_SECOND(id)), ctx);
        }
    }
}

static
ecs_table_range_t flecs_get_ref_range(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx)
{
    if (flag & EcsRuleIsEntity) {
        return flecs_range_from_entity(ref->entity, ctx);
    } else if (flag & EcsRuleIsVar) {
        return flecs_query_var_get_range(ref->var, ctx);
    }
    return (ecs_table_range_t){0};
}

static
ecs_entity_t flecs_get_ref_entity(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx)
{
    if (flag & EcsRuleIsEntity) {
        return ref->entity;
    } else if (flag & EcsRuleIsVar) {
        return flecs_query_var_get_entity(ref->var, ctx);
    }
    return 0;
}

static
ecs_id_t flecs_query_op_get_id_w_written(
    const ecs_query_op_t *op,
    uint64_t written,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsRuleSecond);
    ecs_entity_t first = 0, second = 0;

    if (flags_1st) {
        if (flecs_ref_is_written(op, &op->first, EcsRuleFirst, written)) {
            first = flecs_get_ref_entity(&op->first, flags_1st, ctx);
        } else if (flags_1st & EcsRuleIsVar) {
            first = EcsWildcard;
        }
    }
    if (flags_2nd) {
        if (flecs_ref_is_written(op, &op->second, EcsRuleSecond, written)) {
            second = flecs_get_ref_entity(&op->second, flags_2nd, ctx);
        } else if (flags_2nd & EcsRuleIsVar) {
            second = EcsWildcard;
        }
    }

    if (flags_2nd & (EcsRuleIsVar | EcsRuleIsEntity)) {
        return ecs_pair(first, second);
    } else {
        return flecs_entities_get_alive(ctx->world, first);
    }
}

static
ecs_id_t flecs_query_op_get_id(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    return flecs_query_op_get_id_w_written(op, written, ctx);
}

static
int16_t flecs_query_next_column(
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column)
{
    if (!ECS_IS_PAIR(id) || (ECS_PAIR_FIRST(id) != EcsWildcard)) {
        column = column + 1;
    } else {
        ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
        column = ecs_search_offset(NULL, table, column + 1, id, NULL);
        ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);
    }
    return flecs_ito(int16_t, column);
}

static
void flecs_query_it_set_column(
    ecs_iter_t *it,
    int32_t field_index,
    int32_t column)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);
    it->columns[field_index] = column;
}

static
ecs_id_t flecs_query_it_set_id(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t field_index,
    int32_t column)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);
    return it->ids[field_index] = table->type.array[column];
}

static
void flecs_query_set_match(
    const ecs_query_op_t *op,
    ecs_table_t *table,
    int32_t column,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    int32_t field_index = op->field_index;
    if (field_index == -1) {
        return;
    }

    ecs_iter_t *it = ctx->it;
    flecs_query_it_set_column(it, field_index, column);
    ecs_id_t matched = flecs_query_it_set_id(it, table, field_index, column);
    flecs_query_set_vars(op, matched, ctx);
}

static
void flecs_query_set_trav_match(
    const ecs_query_op_t *op,
    int32_t column,
    ecs_entity_t trav,
    ecs_entity_t second,
    const ecs_query_run_ctx_t *ctx)
{
    int32_t field_index = op->field_index;
    if (field_index == -1) {
        return;
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t matched = ecs_pair(trav, second);
    it->ids[op->field_index] = matched;
    if (column != -1) {
        flecs_query_it_set_column(it, op->field_index, column);
    }
    flecs_query_set_vars(op, matched, ctx);
}

static
bool flecs_query_table_filter(
    ecs_table_t *table,
    ecs_query_lbl_t other,
    ecs_flags32_t filter_mask)
{
    uint32_t filter = flecs_ito(uint32_t, other);
    return (table->flags & filter_mask & filter) != 0;
}

static
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

        if (ctx->rule->pub.flags & EcsQueryMatchEmptyTables) {
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

static
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

static
bool flecs_query_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_id_record_t *idr = op_ctx->idr;
    const ecs_table_record_t *tr;

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsRuleSrc, ctx);
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
    } else {
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

static
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

        if (ctx->rule->pub.flags & EcsQueryMatchEmptyTables) {
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
    flecs_query_it_set_column(it, field, tr->index);
    return true;
}

static
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

    ecs_table_t *table = flecs_query_get_table(op, &op->src, EcsRuleSrc, ctx);
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

    flecs_query_it_set_column(it, field, tr->index);
    return true;
}

static
bool flecs_query_and_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_query_with_id(op, redo, ctx);
    } else {
        return flecs_query_select_id(op, redo, ctx, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
    }
}

static
bool flecs_query_up_select(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool self,
    bool id_only)
{
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_world_t *world = ctx->world;
    ecs_iter_t *it = ctx->it;
    bool redo_select = redo;
    const ecs_query_t *q = &ctx->rule->pub;

    /* Early out if traversal relationship doesn't exist */
    op_ctx->trav = q->terms[op->term_index].trav;
    if (!op_ctx->idr_trav) {
        op_ctx->idr_trav = flecs_id_record_get(ctx->world, 
            ecs_pair(op_ctx->trav, EcsWildcard));
    }
    if (!op_ctx->idr_trav || !flecs_table_cache_count(&op_ctx->idr_trav->cache)){
        if (!self) {
            return false;
        } else if (id_only) {
            return flecs_query_select_id(op, redo, ctx,
                (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
        } else {
            return flecs_query_select(op, redo, ctx);
        }
    }

    if (!redo) {
        op_ctx->with = flecs_query_op_get_id(op, ctx);
        op_ctx->idr_with = flecs_id_record_get(ctx->world, op_ctx->with);
        if (!op_ctx->idr_with) {
            return false;
        }

        op_ctx->down = NULL;
        op_ctx->cache_elem = 0;
    }

    ecs_trav_down_t *down = op_ctx->down;

    do {
        while (!down) {
            ecs_table_t *table = op_ctx->table;
            if (!table) {
                ecs_table_range_t range;
                it->sources[op->field_index] = 0;
                do {
                    bool result;
                    if (id_only) {
                        result = flecs_query_select_id(op, redo_select, ctx, 0);
                    } else {
                        result = flecs_query_select_w_id(op, redo_select, ctx, 
                            op_ctx->with, 0);
                    }
                    if (!result) {
                        return false;
                    }

                    redo_select = true;

                    range = flecs_query_get_range(
                        op, &op->src, EcsRuleSrc, ctx);
                    ecs_assert(range.table != NULL, ECS_INTERNAL_ERROR, NULL);
                } while (!self && range.table->_->traversable_count == 0);

                if (!range.count) {
                    range.count = ecs_table_count(range.table);
                }

                table = op_ctx->table = range.table;
                op_ctx->row = range.offset;
                op_ctx->end = range.offset + range.count;
                op_ctx->matched = it->ids[op->field_index];

                if (self) {
                    if (!flecs_query_table_filter(table, op->other, 
                        (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
                    {
                        flecs_reset_source_set_flag(it, op->field_index);
                        op_ctx->row --;
                        return true;
                    }
                }

                redo_select = true;
            } else {
                op_ctx->row ++;
            }

            if (table->_->traversable_count == 0) {
                op_ctx->table = NULL;
                continue;
            } else {
                int32_t row;
                ecs_entity_t entity = 0;
                ecs_entity_t *entities = flecs_table_entities_array(table);

                for (row = op_ctx->row; row < op_ctx->end; row ++) {
                    entity = entities[row];
                    ecs_record_t *record = flecs_entities_get(world, entity);
                    if (record->row & EcsEntityIsTraversable) {
                        it->sources[op->field_index] = entity;
                        break;
                    }
                }

                if (row == op_ctx->end) {
                    op_ctx->table = NULL;
                    continue;
                }

                bool match_empty = (q->flags & EcsQueryMatchEmptyTables) != 0;
                down = op_ctx->down = flecs_query_get_down_cache(ctx, &op_ctx->cache, 
                    op_ctx->trav, entity, op_ctx->idr_with, self, match_empty);
                op_ctx->cache_elem = -1;
            }
        }

next_elem:
        if ((++ op_ctx->cache_elem) >= ecs_vec_count(&down->elems)) {
            down = NULL;
            continue;
        }

        ecs_trav_down_elem_t *elem = ecs_vec_get_t(
            &down->elems, ecs_trav_down_elem_t, op_ctx->cache_elem);

        flecs_query_var_set_range(op, op->src.var, elem->table, 0, 0, ctx);
        flecs_query_set_vars(op, op_ctx->matched, ctx);

        if (flecs_query_table_filter(elem->table, op->other, 
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled)))
        {
            goto next_elem;
        }

        break;
    } while (true);

    flecs_set_source_set_flag(it, op->field_index);

    return true;
}

static
bool flecs_query_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_query_t *q = &ctx->rule->pub;
    ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_iter_t *it = ctx->it;

    /* Early out if traversal relationship doesn't exist */
    op_ctx->trav = q->terms[op->term_index].trav;
    if (!op_ctx->idr_trav) {
        op_ctx->idr_trav = flecs_id_record_get(ctx->world, 
            ecs_pair(op_ctx->trav, EcsWildcard));
    }
    if (!op_ctx->idr_trav || !flecs_table_cache_all_count(&op_ctx->idr_trav->cache)){
        return false;
    }

    if (!redo) {
        op_ctx->trav = q->terms[op->term_index].trav;
        op_ctx->with = flecs_query_op_get_id(op, ctx);
        op_ctx->idr_with = flecs_id_record_get(ctx->world, op_ctx->with);

        if (!op_ctx->idr_with) {
            return false;
        }

        ecs_table_range_t range = flecs_query_get_range(
            op, &op->src, EcsRuleSrc, ctx);
        if (!range.table) {
            return false;
        }

        ecs_trav_up_t *up = flecs_query_get_up_cache(ctx, &op_ctx->cache, 
            range.table, op_ctx->with, op_ctx->trav, op_ctx->idr_with,
            op_ctx->idr_trav);

        if (!up) {
            return false;
        }

        it->sources[op->field_index] = flecs_entities_get_alive(
            ctx->world, up->src);
        it->columns[op->field_index] = up->column;
        it->ids[op->field_index] = up->id;
        flecs_query_set_vars(op, up->id, ctx);
        flecs_set_source_set_flag(it, op->field_index);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_query_self_up_with(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx,
    bool id_only)
{
    if (!redo) {
        bool result;
        if (id_only) {
            result = flecs_query_with_id(op, redo, ctx);
        } else {
            result = flecs_query_with(op, redo, ctx);
        }

        flecs_reset_source_set_flag(ctx->it, op->field_index);

        if (result) {
            ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
            op_ctx->trav = 0;
            if (flecs_query_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar) {
                ecs_iter_t *it = ctx->it;
                it->sources[op->field_index] = 0;
            }
            return true;
        }

        return flecs_query_up_with(op, redo, ctx);
    } else {
        ecs_query_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
        if (op_ctx->trav == 0) {
            return flecs_query_with(op, redo, ctx);
        }
    }

    return false;
}

static
bool flecs_query_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_query_up_with(op, redo, ctx);
    } else {
        return flecs_query_up_select(op, redo, ctx, false, false);
    }
}

static
bool flecs_query_self_up(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_query_self_up_with(op, redo, ctx, false);
    } else {
        return flecs_query_up_select(op, redo, ctx, true, false);
    }
}

static
bool flecs_query_up_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_query_up_with(op, redo, ctx);
    } else {
        return flecs_query_up_select(op, redo, ctx, false, true);
    }
}

static
bool flecs_query_self_up_id(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_query_self_up_with(op, redo, ctx, true);
    } else {
        return flecs_query_up_select(op, redo, ctx, true, true);
    }
}

static
bool flecs_query_select_any(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    return flecs_query_select_w_id(op, redo, ctx, EcsAny, 
        (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled));
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
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        result = flecs_query_with(op, redo, ctx);
    } else {
        result = flecs_query_select(op, redo, ctx);
        remaining = 0;
    }

    if (!redo) {
        ecs_query_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
        if (match_flags & EcsTermMatchAny && op_ctx->remaining) {
            op_ctx->remaining = flecs_ito(int16_t, remaining);
        }
    }

    int32_t field = op->field_index;
    if (field != -1) {
        ctx->it->ids[field] = flecs_query_op_get_id(op, ctx);
    }

    return result;
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
        return flecs_query_trivial_test(
            ctx->rule, ctx, !redo, termset);
    } else {
        return flecs_query_trivial_search_nodata(
            ctx->rule, ctx, op_ctx, !redo, termset);
    }
}

static
bool flecs_query_triv_data(
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
        return flecs_query_trivial_test(
            ctx->rule, ctx, !redo, termset);
    } else {
        return flecs_query_trivial_search(
            ctx->rule, ctx, op_ctx, !redo, termset);
    }
}

static
bool flecs_query_triv_wildcard(
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
        return flecs_query_trivial_test_w_wildcards(
            ctx->rule, ctx, !redo, termset);
    } else {
        return flecs_query_trivial_search_w_wildcards(
            ctx->rule, ctx, op_ctx, !redo, termset);
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
        return flecs_query_cache_test(ctx->rule, ctx, !redo);
    } else {
        return flecs_query_cache_search(ctx->rule, ctx);
    }
}

static
bool flecs_query_cache_data(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    (void)redo;

    uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        return flecs_query_cache_data_test(ctx->rule, ctx, !redo);
    } else {
        return flecs_query_cache_data_search(ctx->rule, ctx);
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
        return flecs_query_is_cache_test(ctx->rule, ctx, !redo);
    } else {
        return flecs_query_is_cache_search(ctx->rule, ctx);
    }
}

static
bool flecs_query_is_cache_data(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;

    uint64_t written = ctx->written[ctx->op_index];
    ctx->written[ctx->op_index + 1] |= 1ull;
    if (written & 1ull) {
        return flecs_query_is_cache_data_test(ctx->rule, ctx, !redo);
    } else {
        return flecs_query_is_cache_data_search(ctx->rule, ctx);
    }
}

static
bool flecs_query_trav_fixed_src_reflexive(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_table_t *table = range->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t *entities = table->data.entities.array;
    int32_t count = range->count;
    if (!count) {
        count = ecs_table_count(table);
    }

    int32_t i = range->offset, end = i + count;
    for (; i < end; i ++) {
        if (entities[i] == second) {
            /* Even though table doesn't have the specific relationship 
             * pair, the relationship is reflexive and the target entity
             * is stored in the table. */
            break;
        }
    }
    if (i == end) {
        /* Table didn't contain target entity */
        return false;
    }
    if (count > 1) {
        /* If the range contains more than one entity, set the range to
         * return only the entity matched by the reflexive property. */
        ecs_assert(flecs_query_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_var_t *var = &ctx->vars[op->src.var];
        ecs_table_range_t *var_range = &var->range;
        var_range->offset = i;
        var_range->count = 1;
        var->entity = entities[i];
    }

    flecs_query_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

static
bool flecs_query_trav_unknown_src_reflexive(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_assert(flecs_query_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar,
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_id_t src_var = op->src.var;
    flecs_query_var_set_entity(op, src_var, second, ctx);
    flecs_query_var_get_table(src_var, ctx);
    flecs_query_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

static
bool flecs_query_trav_fixed_src_up_fixed_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false; /* If everything's fixed, can only have a single result */
    }

    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_2nd = flecs_query_ref_flags(op->flags, EcsRuleSecond);
    ecs_flags16_t f_src = flecs_query_ref_flags(op->flags, EcsRuleSrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;

    /* Check if table has transitive relationship by traversing upwards */
    int32_t column = ecs_search_relation(ctx->world, table, 0, 
        ecs_pair(trav, second), trav, EcsSelf|EcsUp, NULL, NULL, NULL);
    if (column == -1) {
        if (op->match_flags & EcsTermReflexive) {
            return flecs_query_trav_fixed_src_reflexive(op, ctx,
                &range, trav, second);
        } else {
            return false;
        }
    }

    flecs_query_set_trav_match(op, column, trav, second, ctx);
    return true;
}

static
bool flecs_query_trav_unknown_src_up_fixed_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_2nd = flecs_query_ref_flags(op->flags, EcsRuleSecond);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        ecs_record_t *r_second = flecs_entities_get(ctx->world, second);
        bool traversable = r_second && r_second->row & EcsEntityIsTraversable;
        bool reflexive = op->match_flags & EcsTermReflexive;
        if (!traversable && !reflexive) {
            trav_ctx->cache.id = 0;

            /* If there's no record for the entity, it can't have a subtree so
             * forward operation to a regular select. */
            return flecs_query_select(op, redo, ctx);
        }

        /* Entity is traversable, which means it could have a subtree */
        flecs_query_get_trav_down_cache(ctx, &trav_ctx->cache, trav, second);
        trav_ctx->index = 0;

        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->index = -1;
            return flecs_query_trav_unknown_src_reflexive(
                op, ctx, trav, second);
        }
    } else {
        if (!trav_ctx->cache.id) {
            /* No traversal cache, which means this is a regular select */
            return flecs_query_select(op, redo, ctx);
        }
    }

    if (trav_ctx->index == -1) {
        redo = false; /* First result after handling reflexive relationship */
        trav_ctx->index = 0;
    }

    /* Forward to select */
    int32_t count = ecs_vec_count(&trav_ctx->cache.entities);
    ecs_trav_elem_t *elems = ecs_vec_first(&trav_ctx->cache.entities);
    for (; trav_ctx->index < count; trav_ctx->index ++) {
        ecs_trav_elem_t *el = &elems[trav_ctx->index];
        trav_ctx->and.idr = el->idr; /* prevents lookup by select */
        if (flecs_query_select_w_id(op, redo, ctx, ecs_pair(trav, el->entity),
            (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))) 
        {
            return true;
        }
        
        redo = false;
    }

    return false;
}

static
bool flecs_query_trav_yield_reflexive_src(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav)
{
    ecs_var_t *vars = ctx->vars;
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);
    int32_t offset = trav_ctx->offset, count = trav_ctx->count;
    bool src_is_var = op->flags & (EcsRuleIsVar << EcsRuleSrc);

    if (trav_ctx->index >= (offset + count)) {
        /* Restore previous offset, count */
        if (src_is_var) {
            ecs_var_id_t src_var = op->src.var;
            vars[src_var].range.offset = offset;
            vars[src_var].range.count = count;
            vars[src_var].entity = 0;
        }
        return false;
    }

    ecs_entity_t entity = ecs_vec_get_t(
        &range->table->data.entities, ecs_entity_t, trav_ctx->index)[0];
    flecs_query_set_trav_match(op, -1, trav, entity, ctx);

    /* Hijack existing variable to return one result at a time */
    if (src_is_var) {
        ecs_var_id_t src_var = op->src.var;
        ecs_table_t *table = vars[src_var].range.table;
        ecs_assert(!table || table == ecs_get_table(ctx->world, entity),
            ECS_INTERNAL_ERROR, NULL);
        (void)table;
        vars[src_var].entity = entity;
        vars[src_var].range = flecs_range_from_entity(entity, ctx);
    }

    return true;
}

static
bool flecs_query_trav_fixed_src_up_unknown_second(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_src = flecs_query_ref_flags(op->flags, EcsRuleSrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;
    ecs_query_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        flecs_query_get_trav_up_cache(ctx, &trav_ctx->cache, trav, table);
        trav_ctx->index = 0;
        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->yield_reflexive = true;
            trav_ctx->index = range.offset;
            trav_ctx->offset = range.offset;
            trav_ctx->count = range.count ? range.count : ecs_table_count(table);
        }
    } else {
        trav_ctx->index ++;
    }

    if (trav_ctx->yield_reflexive) {
        if (flecs_query_trav_yield_reflexive_src(op, ctx, &range, trav)) {
            return true;
        }
        trav_ctx->yield_reflexive = false;
        trav_ctx->index = 0;
    }

    if (trav_ctx->index >= ecs_vec_count(&trav_ctx->cache.entities)) {
        return false;
    }

    ecs_trav_elem_t *el = ecs_vec_get_t(
        &trav_ctx->cache.entities, ecs_trav_elem_t, trav_ctx->index);
    flecs_query_set_trav_match(op, el->column, trav, el->entity, ctx);
    return true;
}

static
bool flecs_query_trav(
    const ecs_query_op_t *op,
    bool redo,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];

    if (!flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written)) {
            /* This can't happen, src or second should have been resolved */
            ecs_abort(ECS_INTERNAL_ERROR, 
                "invalid instruction sequence: unconstrained traversal");
        } else {
            return flecs_query_trav_unknown_src_up_fixed_second(op, redo, ctx);
        }
    } else {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written)) {
            return flecs_query_trav_fixed_src_up_unknown_second(op, redo, ctx);
        } else {
            return flecs_query_trav_fixed_src_up_fixed_second(op, redo, ctx);
        }
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
        if (!(idr->flags & EcsIdDontInherit)) {
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
    if (op->flags & (EcsRuleIsVar << EcsRuleSrc)) {
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
            op, &op->src, EcsRuleSrc, ctx);
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

                    if (idr->flags & EcsIdDontInherit) {
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

                if (idr->flags & EcsIdDontInherit) {
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
        it->columns[op->field_index] = -1; /* Mark field as set */
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
        it->set_fields |= (1llu << op->field_index);
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
        it->set_fields |= (1llu << op->field_index);
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
        row = op_ctx->row = range.offset;
    } else {
        int32_t end = range.count;
        if (end) {
            end += range.offset;
        } else {
            end = table->data.entities.count;
        }
        row = ++ op_ctx->row;
        if (op_ctx->row >= end) {
            return false;
        }
    }

    ecs_assert(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t *entities = table->data.entities.array;
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
const char* flecs_query_name_arg(
    const ecs_query_op_t *op,
    ecs_query_run_ctx_t *ctx)
{
    int8_t term_index = op->term_index;
    const ecs_term_t *term = &ctx->rule->pub.terms[term_index];
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
            op, &op->src, EcsRuleSrc, ctx);

        if (!flecs_query_compare_range(&l, &r)) {
            return false;
        }

        ctx->vars[src_var].range.offset = r.offset;
        ctx->vars[src_var].range.count = r.count;
        return true;
    }
}

static
bool flecs_query_pred_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsRuleSecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized eq operand");

    ecs_table_range_t r = flecs_query_get_range(
        op, &op->second, EcsRuleSecond, ctx);
    return flecs_query_pred_eq_w_range(op, redo, ctx, r);
}

static
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

static
bool flecs_query_pred_neq_w_range(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_table_range_t r)
{
    ecs_query_eq_ctx_t *op_ctx = flecs_op_ctx(ctx, eq);
    ecs_var_id_t src_var = op->src.var;
    ecs_table_range_t l = flecs_query_get_range(
        op, &op->src, EcsRuleSrc, ctx);

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
    ecs_assert(flecs_ref_is_written(op, &op->src, EcsRuleSrc, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized match operand");
    (void)written;

    ecs_var_id_t src_var = op->src.var;
    const char *match = flecs_query_name_arg(op, ctx);
    ecs_table_range_t l;
    if (!redo) {
        l = flecs_query_get_range(op, &op->src, EcsRuleSrc, ctx);
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

static
bool flecs_query_pred_eq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_pred_match(op, redo, ctx, false);
}

static
bool flecs_query_pred_neq_match(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_pred_match(op, redo, ctx, true);
}

static
bool flecs_query_member_cmp(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    bool neq)
{
    ecs_table_range_t range;
    if (op->other) {
        ecs_var_id_t table_var = op->other - 1;
        range = flecs_query_var_get_range(table_var, ctx);
    } else {
        range = flecs_query_get_range(op, &op->src, EcsRuleSrc, ctx);
    }

    ecs_table_t *table = range.table;
    if (!table) {
        return false;
    }

    ecs_query_membereq_ctx_t *op_ctx = flecs_op_ctx(ctx, membereq);
    ecs_iter_t *it = ctx->it;
    int8_t field_index = op->field_index;

    if (!range.count) {
        range.count = ecs_table_count(range.table); 
    }

    int32_t row, end = range.count;
    if (end) {
        end += range.offset;
    } else {
        end = ecs_table_count(range.table);
    }

    void *data;
    if (!redo) {
        row = op_ctx->each.row = range.offset;

        /* Get data ptr starting from offset 0 so we can use row to index */
        range.offset = 0;

        /* Populate data field so we have the array we can compare the member
         * value against. */
        void *old_data = it->ptrs[field_index];
        flecs_query_populate_field_from_range(
            it, &range, field_index, it->columns[field_index]);
        data = op_ctx->data = it->ptrs[field_index];

        /* Ensure we only write ptrs when we match data */
        it->ptrs[field_index] = old_data;

        /* Member fields are of type ecs_entity_t */
        it->ids[field_index] = ecs_id(ecs_entity_t);
    } else {
        row = ++ op_ctx->each.row;
        if (op_ctx->each.row >= end) {
            return false;
        }

        data = op_ctx->data;
    }

    uint32_t offset = (uint32_t)op->first.entity;
    uint32_t size = (uint32_t)(op->first.entity >> 32);
    ecs_entity_t *entities = table->data.entities.array;
    ecs_entity_t e = 0;
    ecs_entity_t *val;

    ecs_assert(row < ecs_table_count(table), ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL); /* Must be written */
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);

    bool second_written = true;
    if (op->flags & (EcsRuleIsVar << EcsRuleSecond)) {
        uint64_t written = ctx->written[ctx->op_index];
        second_written = written & (1ull << op->second.var);
    }

    if (second_written) {
        ecs_flags16_t second_flags = flecs_query_ref_flags(
            op->flags, EcsRuleSecond);
        ecs_entity_t second = flecs_get_ref_entity(
            &op->second, second_flags, ctx);

        do {
            e = entities[row];

            val = ECS_OFFSET(ECS_ELEM(data, size, row), offset);
            if (val[0] == second || second == EcsWildcard) {
                if (!neq) {
                    goto match;
                }
            } else {
                if (neq) {
                    goto match;
                }
            }

            row ++;
        } while (row < end);

        return false;
    } else {
        e = entities[row];
        val = ECS_OFFSET(ECS_ELEM(data, size, row), offset);
        flecs_query_var_set_entity(op, op->second.var, val[0], ctx);
    }

match:
    if (op->other) {
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
        flecs_query_var_set_entity(op, op->src.var, e, ctx);
    }

    it->ptrs[field_index] = val;
    op_ctx->each.row = row;

    return true;
}

static
bool flecs_query_member_eq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_member_cmp(op, redo, ctx, false);
}

static
bool flecs_query_member_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    return flecs_query_member_cmp(op, redo, ctx, true);
}

typedef struct {
    ecs_flags64_t mask;
    bool has_bitset;
} flecs_query_row_mask_t;

static
flecs_query_row_mask_t flecs_query_get_row_mask(
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t block_index,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields,
    ecs_query_toggle_ctx_t *op_ctx)
{
    ecs_flags64_t mask = UINT64_MAX;
    uint32_t i, field_count = it->field_count;
    ecs_flags64_t fields = and_fields | not_fields;
    bool has_bitset = false;

    for (i = 0; i < field_count; i ++) {
        uint64_t field_bit = 1llu << i;
        if (!(fields & field_bit)) {
            continue;
        }

        if (not_fields & field_bit) {
            it->set_fields &= ~field_bit;
        } else if (and_fields & field_bit) {
            ecs_assert(it->set_fields & field_bit, ECS_INTERNAL_ERROR, NULL);
        } else {
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        }

        ecs_id_t id = it->ids[i];
        ecs_bitset_t *bs = flecs_table_get_toggle(table, id);
        if (!bs) {
            if (not_fields & field_bit) {
                if (op_ctx->prev_set_fields & field_bit) {
                    has_bitset = false;
                    break;
                }
            }
            continue;
        }

        ecs_assert((64 * block_index) < bs->size, ECS_INTERNAL_ERROR, NULL);
        ecs_flags64_t block = bs->data[block_index];

        if (not_fields & field_bit) {
            block = ~block;
        }
        mask &= block;
        has_bitset = true;
    }

    return (flecs_query_row_mask_t){ mask, has_bitset };
}

static
bool flecs_query_toggle_for_up(
    ecs_iter_t *it,
    ecs_query_run_ctx_t *ctx,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields)
{
    uint32_t i, field_count = it->field_count;
    ecs_flags64_t fields = (and_fields | not_fields) & it->up_fields;

    for (i = 0; i < field_count; i ++) {
        uint64_t field_bit = 1llu << i;
        if (!(fields & field_bit)) {
            continue;
        }

        bool match = false;
        if ((it->set_fields & field_bit)) {
            ecs_entity_t src = it->sources[i];
            ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);
            match = ecs_is_enabled_id(it->world, src, it->ids[i]);
        }

        if (field_bit & not_fields) {
            match = !match;
        }

        if (!match) {
            return false;
        }
    }

    return true;
}

static
bool flecs_query_toggle_cmp(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx,
    ecs_flags64_t and_fields,
    ecs_flags64_t not_fields)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    ecs_table_range_t range = flecs_query_get_range(
        op, &op->src, EcsRuleSrc, ctx);
    ecs_table_t *table = range.table;

    if ((and_fields & op_ctx->prev_set_fields) != and_fields) {
        /* If not all fields matching and toggles are set, table can't match */
        return false;
    }

    ecs_flags32_t up_fields = it->up_fields;
    if (!redo) {
        if (up_fields & (and_fields|not_fields)) {
            /* If there are toggle fields that were matched with query 
             * traversal, evaluate those separately. */
            if (!flecs_query_toggle_for_up(
                it, ctx, and_fields, not_fields)) 
            {
                return false;
            }

            it->set_fields &= ~(not_fields & up_fields);
        }
    }

    /* Shared fields are evaluated, can be ignored from now on */
    // and_fields &= ~up_fields;
    not_fields &= ~up_fields;

    if (!(table->flags & EcsTableHasToggle)) {
        if (not_fields) {
            /* If any of the toggle fields with a not operator are for fields
             * that are set, without a bitset those fields can't match. */
            return false;
        } else {
            /* If table doesn't have toggles but query matched toggleable 
             * components, all entities match. */
            if (!redo) {            
                return true;
            } else {
                return false;
            }
        }
    }

    if (table && !range.count) {
        range.count = ecs_table_count(table);
        if (!range.count) {
            return false;
        }
    }

    int32_t i, j;
    int32_t first, last, block_index, cur;
    uint64_t block;
    if (!redo) {
        op_ctx->range = range;
        cur = op_ctx->cur = range.offset;
        block_index = op_ctx->block_index = -1;
        first = range.offset;
        last = range.offset + range.count;
    } else {
        if (!op_ctx->has_bitset) {
            goto done;
        }

        last = op_ctx->range.offset + op_ctx->range.count;
        cur = op_ctx->cur;
        ecs_assert(cur <= last, ECS_INTERNAL_ERROR, NULL);
        if (cur == last) {
            goto done;
        }

        first = cur;
        block_index = op_ctx->block_index;
        block = op_ctx->block;
    }

    /* If end of last iteration is start of new block, compute new block */    
    int32_t new_block_index = cur / 64, row = first;
    if (new_block_index != block_index) {
compute_block:
        block_index = op_ctx->block_index = new_block_index;

        flecs_query_row_mask_t row_mask = flecs_query_get_row_mask(
            it, table, block_index, and_fields, not_fields, op_ctx);

        /* If table doesn't have bitset columns, all columns match */
        if (!(op_ctx->has_bitset = row_mask.has_bitset)) {
            if (!not_fields) {
                return true;
            } else {
                goto done;
            }
        }

        /* No enabled bits */
        block = row_mask.mask;
        if (!block) {
next_block:
            new_block_index ++;
            cur = new_block_index * 64;
            if (cur >= last) {
                /* No more rows */
                goto done;
            }

            op_ctx->cur = cur;
            goto compute_block;
        }

        op_ctx->block = block;
    }

    /* Find first enabled bit (TODO: use faster bitmagic) */
    int32_t first_bit = cur - (block_index * 64);
    int32_t last_bit = ECS_MIN(64, last - (block_index * 64));
    ecs_assert(first_bit >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(first_bit < 64, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit <= 64, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(last_bit >= first_bit, ECS_INTERNAL_ERROR, NULL);

    for (i = first_bit; i < last_bit; i ++) {
        uint64_t bit = (1ull << i);
        bool cond = 0 != (block & bit);
        if (cond) {
            /* Find last enabled bit */
            for (j = i; j < last_bit; j ++) {
                bit = (1ull << j);
                cond = !(block & bit);
                if (cond) {
                    break;
                }
            }

            row = i + (block_index * 64);
            cur = j + (block_index * 64);
            break;
        }
    }

    if (i == last_bit) {
        goto next_block;
    }

    ecs_assert(row >= first, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur <= last, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cur >= first, ECS_INTERNAL_ERROR, NULL);

    if (!(cur - row)) {
        goto done;
    }

    flecs_query_var_narrow_range(op->src.var, table, row, cur - row, ctx);
    op_ctx->cur = cur;

    return true;

done:
    /* Restore range & set fields */
    flecs_query_var_narrow_range(op->src.var, 
        table, op_ctx->range.offset, op_ctx->range.count, ctx);

    it->set_fields = op_ctx->prev_set_fields;
    return false;
}

static
bool flecs_query_toggle(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    if (!redo) {
        op_ctx->prev_set_fields = it->set_fields;
    }

    ecs_flags64_t and_fields = op->first.entity;
    ecs_flags64_t not_fields = op->second.entity & op_ctx->prev_set_fields;

    return flecs_query_toggle_cmp(
        op, redo, ctx, and_fields, not_fields);
}

static
bool flecs_query_toggle_option(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    ecs_query_toggle_ctx_t *op_ctx = flecs_op_ctx(ctx, toggle);
    if (!redo) {
        op_ctx->prev_set_fields = it->set_fields;
        op_ctx->optional_not = false;
        op_ctx->has_bitset = false;
    }

repeat: {}
    ecs_flags64_t and_fields = 0, not_fields = 0;
    if (op_ctx->optional_not) {
        not_fields = op->first.entity & op_ctx->prev_set_fields;
    } else {
        and_fields = op->first.entity;
    }

    bool result = flecs_query_toggle_cmp(
        op, redo, ctx, and_fields, not_fields);
    if (!result) {
        if (!op_ctx->optional_not) {
            /* Run the not-branch of optional fields */
            op_ctx->optional_not = true;
            it->set_fields = op_ctx->prev_set_fields;
            redo = false;
            goto repeat;
        }
    }

    return result;
}

static
bool flecs_query_pred_neq(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsRuleSecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized neq operand");

    ecs_table_range_t r = flecs_query_get_range(
        op, &op->second, EcsRuleSecond, ctx);
    return flecs_query_pred_neq_w_range(op, redo, ctx, r);
}

static
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

static
bool flecs_query_lookup(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    const ecs_query_impl_t *rule = ctx->rule;
    ecs_entity_t first = flecs_query_var_get_entity(op->first.var, ctx);
    ecs_query_var_t *var = &rule->vars[op->src.var];

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

    const ecs_query_impl_t *rule = ctx->rule;
    const ecs_query_t *q = &rule->pub;
    ecs_var_id_t *src_vars = rule->src_vars;
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
    const ecs_query_impl_t *rule = ctx->rule;
    const ecs_query_t *q = &rule->pub;
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

static
bool flecs_query_setids(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    const ecs_query_impl_t *rule = ctx->rule;
    const ecs_query_t *q = &rule->pub;
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
    ecs_flags64_t bit = (1llu << field);
    if (result) {
        it->set_fields |= bit;
        return;
    }

    /* Reset state after a field was not matched */
    ctx->written[op_index] = ctx->written[ctx->op_index];
    ctx->op_index = op_index;
    it->set_fields &= ~bit;

    /* Ignore variables written by Not operation */
    uint64_t *written = ctx->written;
    uint64_t written_cur = written[op->prev + 1];
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsRuleSecond);

    /* Overwrite id with cleared out variables */
    ecs_id_t id = flecs_query_op_get_id(op, ctx);
    if (id) {
        it->ids[field] = id;
    }

    /* Reset variables */
    if (flags_1st & EcsRuleIsVar) {
        if (!flecs_ref_is_written(op, &op->first, EcsRuleFirst, written_cur)){
            flecs_query_var_reset(op->first.var, ctx);
        }
    }
    if (flags_2nd & EcsRuleIsVar) {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written_cur)){
            flecs_query_var_reset(op->second.var, ctx);
        }
    }

    /* If term has entity src, set it because no other instruction might */
    if (op->flags & (EcsRuleIsEntity << EcsRuleSrc)) {
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
    ecs_query_iter_t *qit = &it->priv.iter.rule;

    if (!redo) {
        op_ctx->op_index = flecs_itolbl(ctx->op_index + 1);
    } else if (ctx->qit->ops[op_ctx->op_index].kind == EcsRuleEnd) {
        return false;
    }

    ctx->written[ctx->op_index + 1] = ctx->written[ctx->op_index];

    const ecs_query_op_t *op = &ctx->qit->ops[ctx->op_index];
    bool result = flecs_query_run_until(
        redo, ctx, qit->ops, ctx->op_index, op_ctx->op_index, op->next);

    op_ctx->op_index = ctx->op_index - 1;
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
        ops, cur, last);
    if (redo) {
        /* If redoing, start from the last instruction of the last executed 
         * sequence */
        cur = last_for_cur - 1;
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
    ecs_query_iter_t *qit = &it->priv.iter.rule;
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
            redo, ctx, ops, first - 1, cur, last);

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
            bool restore_table_var = false;
            
            if (prev_op->flags & (EcsRuleIsVar << EcsRuleSrc)) {
                if (first_op->src.var != prev_op->src.var) {
                    restore_table_var = true;
                    old_table_var = ctx->vars[first_op->src.var];
                    ctx->vars[first_op->src.var] = 
                        ctx->vars[prev_op->src.var];
                }
            }

            do {
                ctx->written[prev] = ctx->written[last];

                flecs_query_run_until(false, ctx, ops, first - 1, prev, cur);

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
    if (op->flags & (EcsRuleIsVar << EcsRuleSrc)) {
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
    if (flecs_query_ref_flags(op->flags, ref_kind) == EcsRuleIsVar) {
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
        if (!flecs_query_eval_if(op, ctx, &op->src, EcsRuleSrc) ||
            !flecs_query_eval_if(op, ctx, &op->first, EcsRuleFirst) ||
            !flecs_query_eval_if(op, ctx, &op->second, EcsRuleSecond))
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
    uint8_t field_index = op->other;

    ecs_query_ctrl_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrl);
    if (!redo) {
        op_ctx->is_set = ecs_field_is_set(it, field_index + 1);
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
void flecs_query_populate_field_from_range(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    int32_t index)
{
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(range->table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (range->count && range->table->column_map) {
        int32_t column = range->table->column_map[index];
        if (column != -1) {
            it->ptrs[field_index] = ECS_ELEM(
                range->table->data.columns[column].data.array,
                it->sizes[field_index],
                range->offset);
        }
    }
}

static
void flecs_query_populate_field(
    ecs_iter_t *it,
    ecs_table_range_t *range,
    int8_t field_index,
    ecs_query_run_ctx_t *ctx)
{
    int32_t index = it->columns[field_index];
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_field_is_set(it, field_index + 1), ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t src = it->sources[field_index];
    if (!src) {
        flecs_query_populate_field_from_range(it, range, field_index, index);
        ECS_BIT_CLEARN(it->shared_fields, field_index);
    } else {
        ecs_record_t *r = flecs_entities_get(ctx->world, src);
        ecs_table_t *src_table = r->table;
        if (src_table->column_map) {
            int32_t column = src_table->column_map[index];
            if (column != -1) {
                it->ptrs[field_index] = ecs_vec_get(
                    &src_table->data.columns[column].data,
                    it->sizes[field_index],
                    ECS_RECORD_TO_ROW(r->row));

                ECS_BIT_SETN(it->shared_fields, field_index);
            }
        }
    }
}

static
bool flecs_query_populate(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    if (!redo) {
        ecs_iter_t *it = ctx->it;
        if (it->flags & EcsIterNoData) {
            return true;
        }

        const ecs_query_impl_t *rule = ctx->rule;
        const ecs_query_t *q = &rule->pub;
        int32_t i, field_count = q->field_count;
        ecs_flags64_t data_fields = op->src.entity; /* Bitset with fields to set */
        ecs_table_range_t *range = &ctx->vars[0].range;
        ecs_table_t *table = range->table;
        if (table && !range->count) {
            range->count = ecs_table_count(table);
        }

        /* Only populate fields that are set */
        data_fields &= it->set_fields;
        for (i = 0; i < field_count; i ++) {
            if (!(data_fields & (1llu << i))) {
                continue;
            }

            flecs_query_populate_field(it, range, i, ctx);
        }

        return true;
    } else {
        return false;
    }
}

static
bool flecs_query_populate_self(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    (void)op;
    if (!redo) {
        const ecs_query_impl_t *rule = ctx->rule;
        const ecs_query_t *q = &rule->pub;
        int32_t i, field_count = q->field_count;
        ecs_flags64_t data_fields = op->src.entity; /* Bitset with fields to set */
        ecs_iter_t *it = ctx->it;

        ecs_table_range_t *range = &ctx->vars[0].range;
        ecs_table_t *table = range->table;
        if (!table->column_map) {
            return true;
        }

        if (!ecs_table_count(table)) {
            return true;
        }

        /* Only populate fields that can be set */
        data_fields &= it->set_fields;
        for (i = 0; i < field_count; i ++) {
            if (!(data_fields & (1llu << i))) {
                continue;
            }

            int32_t index = it->columns[i];
            ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

            int32_t column = table->column_map[index];
            if (column != -1) {
                it->ptrs[i] = ECS_ELEM(
                    table->data.columns[column].data.array,
                    it->sizes[i],
                    range->offset);
            }
        }

        return true;
    } else {
        return false;
    }
}

static
bool flecs_query_dispatch(
    const ecs_query_op_t *op,
    bool redo,
    ecs_query_run_ctx_t *ctx)
{
    switch(op->kind) {
    case EcsRuleAnd: return flecs_query_and(op, redo, ctx);
    case EcsRuleAndId: return flecs_query_and_id(op, redo, ctx);
    case EcsRuleAndAny: return flecs_query_and_any(op, redo, ctx);
    case EcsRuleTriv: return flecs_query_triv(op, redo, ctx);
    case EcsRuleTrivData: return flecs_query_triv_data(op, redo, ctx);
    case EcsRuleTrivWildcard: return flecs_query_triv_wildcard(op, redo, ctx);
    case EcsRuleCache: return flecs_query_cache(op, redo, ctx);
    case EcsRuleIsCache: return flecs_query_is_cache(op, redo, ctx);
    case EcsRuleCacheData: return flecs_query_cache_data(op, redo, ctx);
    case EcsRuleIsCacheData: return flecs_query_is_cache_data(op, redo, ctx);
    case EcsRuleSelectAny: return flecs_query_select_any(op, redo, ctx);
    case EcsRuleUp: return flecs_query_up(op, redo, ctx);
    case EcsRuleUpId: return flecs_query_up_id(op, redo, ctx);
    case EcsRuleSelfUp: return flecs_query_self_up(op, redo, ctx);
    case EcsRuleSelfUpId: return flecs_query_self_up_id(op, redo, ctx);
    case EcsRuleWith: return flecs_query_with(op, redo, ctx);
    case EcsRuleTrav: return flecs_query_trav(op, redo, ctx);
    case EcsRuleAndFrom: return flecs_query_and_from(op, redo, ctx);
    case EcsRuleNotFrom: return flecs_query_not_from(op, redo, ctx);
    case EcsRuleOrFrom: return flecs_query_or_from(op, redo, ctx);
    case EcsRuleIds: return flecs_query_ids(op, redo, ctx);
    case EcsRuleIdsRight: return flecs_query_idsright(op, redo, ctx);
    case EcsRuleIdsLeft: return flecs_query_idsleft(op, redo, ctx);
    case EcsRuleEach: return flecs_query_each(op, redo, ctx);
    case EcsRuleStore: return flecs_query_store(op, redo, ctx);
    case EcsRuleReset: return flecs_query_reset(op, redo, ctx);
    case EcsRuleOr: return flecs_query_or(op, redo, ctx);
    case EcsRuleOptional: return flecs_query_optional(op, redo, ctx);
    case EcsRuleIfVar: return flecs_query_if_var(op, redo, ctx);
    case EcsRuleIfSet: return flecs_query_if_set(op, redo, ctx);
    case EcsRuleEnd: return flecs_query_end(op, redo, ctx);
    case EcsRuleNot: return flecs_query_not(op, redo, ctx);
    case EcsRulePredEq: return flecs_query_pred_eq(op, redo, ctx);
    case EcsRulePredNeq: return flecs_query_pred_neq(op, redo, ctx);
    case EcsRulePredEqName: return flecs_query_pred_eq_name(op, redo, ctx);
    case EcsRulePredNeqName: return flecs_query_pred_neq_name(op, redo, ctx);
    case EcsRulePredEqMatch: return flecs_query_pred_eq_match(op, redo, ctx);
    case EcsRulePredNeqMatch: return flecs_query_pred_neq_match(op, redo, ctx);
    case EcsRuleMemberEq: return flecs_query_member_eq(op, redo, ctx);
    case EcsRuleMemberNeq: return flecs_query_member_neq(op, redo, ctx);
    case EcsRuleToggle: return flecs_query_toggle(op, redo, ctx);
    case EcsRuleToggleOption: return flecs_query_toggle_option(op, redo, ctx);
    case EcsRuleLookup: return flecs_query_lookup(op, redo, ctx);
    case EcsRuleSetVars: return flecs_query_setvars(op, redo, ctx);
    case EcsRuleSetThis: return flecs_query_setthis(op, redo, ctx);
    case EcsRuleSetFixed: return flecs_query_setfixed(op, redo, ctx);
    case EcsRuleSetIds: return flecs_query_setids(op, redo, ctx);
    case EcsRuleSetId: return flecs_query_setid(op, redo, ctx);
    case EcsRuleContain: return flecs_query_contain(op, redo, ctx);
    case EcsRulePairEq: return flecs_query_pair_eq(op, redo, ctx);
    case EcsRulePopulate: return flecs_query_populate(op, redo, ctx);
    case EcsRulePopulateSelf: return flecs_query_populate_self(op, redo, ctx);
    case EcsRuleYield: return false;
    case EcsRuleNothing: return false;
    }
    return false;
}

static
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

static
void flecs_query_iter_init(
    ecs_query_run_ctx_t *ctx)
{
    ecs_assert(ctx->written != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_iter_t *it = ctx->it;

    const ecs_query_impl_t *rule = ctx->rule;
    const ecs_query_t *q = &rule->pub;
    ecs_flags64_t it_written = it->constrained_vars;
    ctx->written[0] = it_written;
    if (it_written && ctx->rule->src_vars) {
        /* If variables were constrained, check if there are any table
         * variables that have a constrained entity variable. */
        ecs_var_t *vars = ctx->vars;
        int32_t i, count = q->field_count;
        for (i = 0; i < count; i ++) {
            ecs_var_id_t var_id = rule->src_vars[i];
            ecs_query_var_t *var = &rule->vars[var_id];

            if (!(it_written & (1ull << var_id)) || 
                (var->kind == EcsVarTable) || (var->table_id == EcsVarNone)) 
            {
                continue;
            }

            /* Initialize table variable with constrained entity variable */
            ecs_var_t *tvar = &vars[var->table_id];
            tvar->range = flecs_range_from_entity(vars[var_id].entity, ctx);
            ctx->written[0] |= (1ull << var->table_id); /* Mark as written */
        }
    }

    ecs_flags32_t flags = q->flags;
    ecs_query_cache_t *cache = rule->cache;
    if (flags & EcsQueryIsTrivial && !cache) {
        if ((flags & EcsQueryMatchOnlySelf) || 
            !flecs_table_cache_all_count(&ctx->world->idr_isa_wildcard->cache)) 
        {
            if (it_written) {
                flecs_query_set_iter_this(it, ctx);

                if (flags & EcsQueryMatchWildcards) {
                    it->flags |= EcsIterTrivialTestWildcard;
                    flecs_query_setids(&rule->ops[0], false, ctx);
                } else {
                    it->flags |= EcsIterTrivialTest;
                    flecs_query_setids(&rule->ops[0], false, ctx);
                }
            } else {
                if (flags & EcsQueryMatchWildcards) {
                    it->flags |= EcsIterTrivialSearchWildcard;
                    flecs_query_setids(&rule->ops[0], false, ctx);
                } else if (flags & EcsQueryNoData) {
                    it->flags |= EcsIterTrivialSearchNoData;
                    flecs_query_setids(&rule->ops[0], false, ctx);
                } else {
                    it->flags |= EcsIterTrivialSearch;
                    flecs_query_setids(&rule->ops[0], false, ctx);
                }
            }
        }
    }

    if (cache) {
        cache->prev_match_count = cache->match_count;
    }

    flecs_iter_validate(it);
}

bool ecs_query_next_instanced(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv.iter.rule;
    ecs_query_impl_t *impl = ECS_CONST_CAST(ecs_query_impl_t*, qit->rule);
    ecs_query_run_ctx_t ctx;
    ctx.world = it->real_world;
    ctx.rule = impl;
    ctx.it = it;
    ctx.vars = qit->vars;
    ctx.rule_vars = qit->rule_vars;
    ctx.written = qit->written;
    ctx.op_ctx = qit->op_ctx;
    ctx.qit = qit;
    const ecs_query_op_t *ops = qit->ops;

    bool redo = true;
    if (!(it->flags & EcsIterIsValid)) {
        ecs_assert(impl != NULL, ECS_INVALID_PARAMETER, NULL);
        flecs_query_iter_init(&ctx);
        redo = false;
    } else {
        it->frame_offset += it->count;

        if (!(it->flags & EcsIterSkip)) {
            flecs_query_mark_fields_dirty(impl, it);
            if (qit->prev) {
                if (ctx.rule->pub.flags & EcsQueryHasMonitor) {
                    flecs_query_sync_match_monitor(impl, qit->prev);
                }
            }
        }

        it->flags &= ~EcsIterSkip;
    }

    /* Specialized iterator modes for trivial queries */
    if (it->flags & EcsIterTrivialSearch) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.rule->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search(ctx.rule, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialSearchNoData) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.rule->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search_nodata(ctx.rule, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialSearchWildcard) {
        ecs_query_trivial_ctx_t *op_ctx = &ctx.op_ctx[0].is.trivial;
        int32_t fields = ctx.rule->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_search_w_wildcards(ctx.rule, &ctx, op_ctx, !redo, mask)) {
            goto done;
        }
        it->table = ctx.vars[0].range.table;
        it->count = ecs_table_count(it->table);
        it->entities = flecs_table_entities_array(it->table);
        return true;
    } else if (it->flags & EcsIterTrivialTest) {
        int32_t fields = ctx.rule->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_test(ctx.rule, &ctx, !redo, mask)) {
            goto done;
        }
        return true;
    } else if (it->flags & EcsIterTrivialTestWildcard) {
        int32_t fields = ctx.rule->pub.term_count;
        ecs_flags64_t mask = (2llu << (fields - 1)) - 1;
        if (!flecs_query_trivial_test_w_wildcards(ctx.rule, &ctx, !redo, mask)) {
            goto done;
        }
        return true;
    }

    /* Default iterator mode */
    if (flecs_query_run_until(redo, &ctx, ops, -1, qit->op, impl->op_count - 1)) {
        ecs_assert(ops[ctx.op_index].kind == EcsRuleYield, 
            ECS_INTERNAL_ERROR, NULL);
        flecs_query_set_iter_this(it, &ctx);
        ecs_assert(it->count >= 0, ECS_INTERNAL_ERROR, NULL);
        qit->op = flecs_itolbl(ctx.op_index - 1);
        return true;
    }

done:
    flecs_query_mark_fixed_fields_dirty(impl, it);
    if (ctx.rule->monitor) {
        flecs_query_update_fixed_monitor(
            ECS_CONST_CAST(ecs_query_impl_t*, ctx.rule));
    }

    ecs_iter_fini(it);
    return false;
}

bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_query_next_instanced(it));
error:
    return false;
}

static
void flecs_query_iter_fini_ctx(
    ecs_iter_t *it,
    ecs_query_iter_t *qit)
{
    const ecs_query_impl_t *rule = flecs_query_impl(qit->rule);
    int32_t i, count = rule->op_count;
    ecs_query_op_t *ops = rule->ops;
    ecs_query_op_ctx_t *ctx = qit->op_ctx;
    ecs_allocator_t *a = flecs_query_get_allocator(it);

    for (i = 0; i < count; i ++) {
        ecs_query_op_t *op = &ops[i];
        switch(op->kind) {
        case EcsRuleTrav:
            flecs_query_trav_cache_fini(a, &ctx[i].is.trav.cache);
            break;
        case EcsRuleUp:
        case EcsRuleSelfUp:
        case EcsRuleUpId:
        case EcsRuleSelfUpId: {
            ecs_trav_up_cache_t *cache = &ctx[i].is.up.cache;
            if (cache->dir == EcsTravDown) {
                flecs_query_down_cache_fini(a, cache);
            } else {
                flecs_query_up_cache_fini(cache);
            }
            break;
        }
        default:
            break;
        }
    }
}

static
void flecs_query_iter_fini(
    ecs_iter_t *it)
{
    ecs_query_iter_t *qit = &it->priv.iter.rule;
    ecs_assert(qit->rule != NULL, ECS_INVALID_OPERATION, NULL);
    ecs_poly_assert(qit->rule, ecs_query_t);
    int32_t op_count = flecs_query_impl(qit->rule)->op_count;
    int32_t var_count = flecs_query_impl(qit->rule)->var_count;

#ifdef FLECS_DEBUG
    if (it->flags & EcsIterProfile) {
        char *str = ecs_query_str_w_profile(qit->rule, it);
        printf("%s\n", str);
        ecs_os_free(str);
    }

    flecs_iter_free_n(qit->profile, ecs_query_op_profile_t, op_count);
#endif

    flecs_query_iter_fini_ctx(it, qit);
    flecs_iter_free_n(qit->vars, ecs_var_t, var_count);
    flecs_iter_free_n(qit->written, ecs_write_flags_t, op_count);
    flecs_iter_free_n(qit->op_ctx, ecs_query_op_ctx_t, op_count);
    qit->vars = NULL;
    qit->written = NULL;
    qit->op_ctx = NULL;
    qit->rule = NULL;
}

ecs_iter_t flecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q)
{
    ecs_iter_t it = {0};
    ecs_query_iter_t *qit = &it.priv.iter.rule;
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_poly_assert(q, ecs_query_t);
    ecs_query_impl_t *impl = flecs_query_impl(q);

    int32_t i, var_count = impl->var_count, op_count = impl->op_count;
    it.world = ECS_CONST_CAST(ecs_world_t*, world);
    it.real_world = q->world;
    it.query = q;
    it.system = q->entity;
    it.next = ecs_query_next;
    it.fini = flecs_query_iter_fini;
    it.field_count = q->field_count;
    it.sizes = q->sizes;
    it.set_fields = q->set_fields;
    it.up_fields = 0;
    flecs_query_apply_iter_flags(&it, q);

    flecs_iter_init(world, &it, 
        flecs_iter_cache_ids |
        flecs_iter_cache_columns |
        flecs_iter_cache_sources |
        flecs_iter_cache_ptrs);

    qit->rule = q;
    qit->rule_vars = impl->vars;
    qit->ops = impl->ops;

    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        qit->node = cache->list.first;
        qit->last = cache->list.last;

        if (cache->order_by && cache->list.info.table_count) {
            flecs_query_cache_sort_tables(it.real_world, impl);
            qit->node = ecs_vec_first(&cache->table_slices);
            qit->last = ecs_vec_last_t(
                &cache->table_slices, ecs_query_cache_table_match_t);
        }
    }

    if (var_count) {
        qit->vars = flecs_iter_calloc_n(&it, ecs_var_t, var_count);
    }

    if (op_count) {
        qit->written = flecs_iter_calloc_n(&it, ecs_write_flags_t, op_count);
        qit->op_ctx = flecs_iter_calloc_n(&it, ecs_query_op_ctx_t, op_count);
    }

#ifdef FLECS_DEBUG
    qit->profile = flecs_iter_calloc_n(&it, ecs_query_op_profile_t, op_count);
#endif

    for (i = 1; i < var_count; i ++) {
        qit->vars[i].entity = EcsWildcard;
    }

    it.variables = qit->vars;
    it.variable_count = impl->var_pub_count;
    it.variable_names = impl->var_names;

error:
    return it;
}

ecs_iter_t ecs_query_iter(
    const ecs_world_t *world,
    const ecs_query_t *q)
{
    ecs_run_aperiodic(q->world, EcsAperiodicEmptyTables);

    /* Ok, only for stats */
    ECS_CONST_CAST(ecs_query_t*, q)->eval_count ++;

    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_cache_t *cache = impl->cache;
    if (cache) {
        /* If monitors changed, do query rematching */
        ecs_flags32_t flags = q->flags;
        if (!(world->flags & EcsWorldReadonly) && flags & EcsQueryHasRefs) {
            flecs_eval_component_monitors(q->world);
        }
    }

    return flecs_query_iter(world, q);
}
