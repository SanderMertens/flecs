/**
 * @file query/engine/eval_utils.c
 * @brief Query engine evaluation utilities.
 */

#include "../../private_api.h"

void flecs_query_set_iter_this(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx)
{
    const ecs_var_t *var = &ctx->vars[0];
    const ecs_table_range_t *range = &var->range;
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

ecs_query_op_ctx_t* flecs_op_ctx_(
    const ecs_query_run_ctx_t *ctx)
{
    return &ctx->op_ctx[ctx->op_index];
}

#define flecs_op_ctx(ctx, op_kind) (&flecs_op_ctx_(ctx)->is.op_kind)

void flecs_reset_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    ECS_TERMSET_CLEAR(it->up_fields, 1u << field_index);
}

void flecs_set_source_set_flag(
    ecs_iter_t *it,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    ECS_TERMSET_SET(it->up_fields, 1u << field_index);
}

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

ecs_table_range_t flecs_query_var_get_range(
    int32_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(var_id < ctx->query->var_count, ECS_INTERNAL_ERROR, NULL);
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

ecs_table_t* flecs_query_get_table(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, ref_kind);
    if (flags & EcsQueryIsEntity) {
        return ecs_get_table(ctx->world, ref->entity);
    } else {
        return flecs_query_var_get_table(ref->var, ctx);
    }
}

ecs_table_range_t flecs_query_get_range(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, ref_kind);
    if (flags & EcsQueryIsEntity) {
        ecs_assert(!(flags & EcsQueryIsVar), ECS_INTERNAL_ERROR, NULL);
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

ecs_entity_t flecs_query_var_get_entity(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_assert(var_id < (ecs_var_id_t)ctx->query->var_count, 
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

void flecs_query_var_reset(
    ecs_var_id_t var_id,
    const ecs_query_run_ctx_t *ctx)
{
    ctx->vars[var_id].entity = EcsWildcard;
    ctx->vars[var_id].range.table = NULL;
}

void flecs_query_var_set_range(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(ctx->query_vars[var_id].kind == EcsVarTable, 
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

    ecs_assert(var_id < ctx->query->var_count, ECS_INTERNAL_ERROR, NULL);
    if (ctx->query_vars[var_id].kind != EcsVarTable) {    
        ecs_assert(count == 1, ECS_INTERNAL_ERROR, NULL);
        var->entity = flecs_table_entities_array(table)[offset];
    }
}

void flecs_query_var_set_entity(
    const ecs_query_op_t *op,
    ecs_var_id_t var_id,
    ecs_entity_t entity,
    const ecs_query_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(var_id < (ecs_var_id_t)ctx->query->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_query_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    var->range.table = NULL;
    var->entity = entity;
}

void flecs_query_set_vars(
    const ecs_query_op_t *op,
    ecs_id_t id,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);

    if (flags_1st & EcsQueryIsVar) {
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

    if (flags_2nd & EcsQueryIsVar) {
        ecs_var_id_t var = op->second.var;
        if (op->written & (1ull << var)) {
            flecs_query_var_set_entity(
                op, var, ecs_get_alive(ctx->world, ECS_PAIR_SECOND(id)), ctx);
        }
    }
}

ecs_table_range_t flecs_get_ref_range(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx)
{
    if (flag & EcsQueryIsEntity) {
        return flecs_range_from_entity(ref->entity, ctx);
    } else if (flag & EcsQueryIsVar) {
        return flecs_query_var_get_range(ref->var, ctx);
    }
    return (ecs_table_range_t){0};
}

ecs_entity_t flecs_get_ref_entity(
    const ecs_query_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_query_run_ctx_t *ctx)
{
    if (flag & EcsQueryIsEntity) {
        return ref->entity;
    } else if (flag & EcsQueryIsVar) {
        return flecs_query_var_get_entity(ref->var, ctx);
    }
    return 0;
}

ecs_id_t flecs_query_op_get_id_w_written(
    const ecs_query_op_t *op,
    uint64_t written,
    const ecs_query_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_query_ref_flags(op->flags, EcsQueryFirst);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);
    ecs_entity_t first = 0, second = 0;

    if (flags_1st) {
        if (flecs_ref_is_written(op, &op->first, EcsQueryFirst, written)) {
            first = flecs_get_ref_entity(&op->first, flags_1st, ctx);
        } else if (flags_1st & EcsQueryIsVar) {
            first = EcsWildcard;
        }
    }
    if (flags_2nd) {
        if (flecs_ref_is_written(op, &op->second, EcsQuerySecond, written)) {
            second = flecs_get_ref_entity(&op->second, flags_2nd, ctx);
        } else if (flags_2nd & EcsQueryIsVar) {
            second = EcsWildcard;
        }
    }

    if (flags_2nd & (EcsQueryIsVar | EcsQueryIsEntity)) {
        return ecs_pair(first, second);
    } else {
        return flecs_entities_get_alive(ctx->world, first);
    }
}

ecs_id_t flecs_query_op_get_id(
    const ecs_query_op_t *op,
    const ecs_query_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    return flecs_query_op_get_id_w_written(op, written, ctx);
}

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

void flecs_query_it_set_column(
    ecs_iter_t *it,
    int32_t field_index,
    int32_t column)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);
    it->columns[field_index] = column;
}

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
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(column < table->type.count, ECS_INTERNAL_ERROR, NULL);
    flecs_query_it_set_column(it, field_index, column);
    ecs_id_t matched = flecs_query_it_set_id(it, table, field_index, column);
    flecs_query_set_vars(op, matched, ctx);
}

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

bool flecs_query_table_filter(
    ecs_table_t *table,
    ecs_query_lbl_t other,
    ecs_flags32_t filter_mask)
{
    uint32_t filter = flecs_ito(uint32_t, other);
    return (table->flags & filter_mask & filter) != 0;
}
