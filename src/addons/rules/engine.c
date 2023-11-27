/**
 * @file addons/rules/engine.c
 * @brief Rules engine implementation.
 */

#include "rules.h"

#ifdef FLECS_RULES

ecs_allocator_t* flecs_rule_get_allocator(
    const ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    if (ecs_poly_is(world, ecs_world_t)) {
        return &world->allocator;
    } else {
        ecs_assert(ecs_poly_is(world, ecs_stage_t), ECS_INTERNAL_ERROR, NULL);
        return &((ecs_stage_t*)world)->allocator;
    }
}

static
ecs_rule_op_ctx_t* flecs_op_ctx_(
    const ecs_rule_run_ctx_t *ctx)
{
    return &ctx->op_ctx[ctx->op_index];
}

#define flecs_op_ctx(ctx, op_kind) (&flecs_op_ctx_(ctx)->is.op_kind)

static
void flecs_reset_source_set_flag(
    const ecs_rule_run_ctx_t *ctx,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    (*ctx->source_set) &= ~(1u << field_index);
}

static
void flecs_set_source_set_flag(
    const ecs_rule_run_ctx_t *ctx,
    int32_t field_index)
{
    ecs_assert(field_index != -1, ECS_INTERNAL_ERROR, NULL);
    (*ctx->source_set) |= (1u << field_index);
}

static
ecs_table_range_t flecs_range_from_entity(
    ecs_entity_t e,
    const ecs_rule_run_ctx_t *ctx)
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
ecs_table_range_t flecs_rule_var_get_range(
    int32_t var_id,
    const ecs_rule_run_ctx_t *ctx)
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
ecs_table_t* flecs_rule_var_get_table(
    int32_t var_id,
    const ecs_rule_run_ctx_t *ctx)
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
ecs_table_t* flecs_rule_get_table(
    const ecs_rule_op_t *op,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, ref_kind);
    if (flags & EcsRuleIsEntity) {
        return ecs_get_table(ctx->world, ref->entity);
    } else {
        return flecs_rule_var_get_table(ref->var, ctx);
    }
}

static
ecs_table_range_t flecs_rule_get_range(
    const ecs_rule_op_t *op,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t ref_kind,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, ref_kind);
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
ecs_entity_t flecs_rule_var_get_entity(
    ecs_var_id_t var_id,
    const ecs_rule_run_ctx_t *ctx)
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
void flecs_rule_var_reset(
    ecs_var_id_t var_id,
    const ecs_rule_run_ctx_t *ctx)
{
    ctx->vars[var_id].entity = EcsWildcard;
    ctx->vars[var_id].range.table = NULL;
}

static
void flecs_rule_var_set_table(
    const ecs_rule_op_t *op,
    ecs_var_id_t var_id,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    const ecs_rule_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(ctx->rule_vars[var_id].kind == EcsVarTable, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_rule_is_written(var_id, op->written), 
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
void flecs_rule_var_set_entity(
    const ecs_rule_op_t *op,
    ecs_var_id_t var_id,
    ecs_entity_t entity,
    const ecs_rule_run_ctx_t *ctx)
{
    (void)op;
    ecs_assert(var_id < (ecs_var_id_t)ctx->rule->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_rule_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_t *var = &ctx->vars[var_id];
    var->range.table = NULL;
    var->entity = entity;
}

static
void flecs_rule_set_vars(
    const ecs_rule_op_t *op,
    ecs_id_t id,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);

    if (flags_1st & EcsRuleIsVar) {
        ecs_var_id_t var = op->first.var;
        if (op->written & (1ull << var)) {
            if (ECS_IS_PAIR(id)) {
                flecs_rule_var_set_entity(
                    op, var, ecs_get_alive(ctx->world, ECS_PAIR_FIRST(id)), ctx);
            } else {
                flecs_rule_var_set_entity(op, var, id, ctx);
            }
        }
    }
    if (flags_2nd & EcsRuleIsVar) {
        ecs_var_id_t var = op->second.var;
        if (op->written & (1ull << var)) {
            flecs_rule_var_set_entity(
                op, var, ecs_get_alive(ctx->world, ECS_PAIR_SECOND(id)), ctx);
        }
    }
}

static
ecs_table_range_t flecs_get_ref_range(
    const ecs_rule_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_rule_run_ctx_t *ctx)
{
    if (flag & EcsRuleIsEntity) {
        return flecs_range_from_entity(ref->entity, ctx);
    } else if (flag & EcsRuleIsVar) {
        return flecs_rule_var_get_range(ref->var, ctx);
    }
    return (ecs_table_range_t){0};
}

static
ecs_entity_t flecs_get_ref_entity(
    const ecs_rule_ref_t *ref,
    ecs_flags16_t flag,
    const ecs_rule_run_ctx_t *ctx)
{
    if (flag & EcsRuleIsEntity) {
        return ref->entity;
    } else if (flag & EcsRuleIsVar) {
        return flecs_rule_var_get_entity(ref->var, ctx);
    }
    return 0;
}

static
ecs_id_t flecs_rule_op_get_id_w_written(
    const ecs_rule_op_t *op,
    uint64_t written,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);
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
        return ecs_get_alive(ctx->world, first);
    }
}

static
ecs_id_t flecs_rule_op_get_id(
    const ecs_rule_op_t *op,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    return flecs_rule_op_get_id_w_written(op, written, ctx);
}

static
int16_t flecs_rule_next_column(
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
void flecs_rule_it_set_column(
    ecs_iter_t *it,
    int32_t field_index,
    int32_t column)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(field_index >= 0, ECS_INTERNAL_ERROR, NULL);
    it->columns[field_index] = column + 1;
    if (it->sources[field_index] != 0) {
        it->columns[field_index] *= -1;
    }
}

static
ecs_id_t flecs_rule_it_set_id(
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
void flecs_rule_set_match(
    const ecs_rule_op_t *op,
    ecs_table_t *table,
    int32_t column,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_assert(column >= 0, ECS_INTERNAL_ERROR, NULL);
    int32_t field_index = op->field_index;
    if (field_index == -1) {
        return;
    }

    ecs_iter_t *it = ctx->it;
    flecs_rule_it_set_column(it, field_index, column);
    ecs_id_t matched = flecs_rule_it_set_id(it, table, field_index, column);
    flecs_rule_set_vars(op, matched, ctx);
}

static
void flecs_rule_set_trav_match(
    const ecs_rule_op_t *op,
    int32_t column,
    ecs_entity_t trav,
    ecs_entity_t second,
    const ecs_rule_run_ctx_t *ctx)
{
    int32_t field_index = op->field_index;
    if (field_index == -1) {
        return;
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t matched = ecs_pair(trav, second);
    it->ids[op->field_index] = matched;
    if (column != -1) {
        flecs_rule_it_set_column(it, op->field_index, column);
    }
    flecs_rule_set_vars(op, matched, ctx);
}

static
bool flecs_rule_select_w_id(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx,
    ecs_id_t id)
{
    ecs_rule_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
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

        if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
            return false;
        }
    }

    if (!redo || !op_ctx->remaining) {
        tr = flecs_table_cache_next(&op_ctx->it, ecs_table_record_t);
        if (!tr) {
            return false;
        }

        op_ctx->column = flecs_ito(int16_t, tr->index);
        op_ctx->remaining = flecs_ito(int16_t, tr->count - 1);
        table = tr->hdr.table;
        flecs_rule_var_set_table(op, op->src.var, table, 0, 0, ctx);
    } else {
        tr = (ecs_table_record_t*)op_ctx->it.cur;
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        table = tr->hdr.table;
        op_ctx->column = flecs_rule_next_column(table, idr->id, op_ctx->column);
        op_ctx->remaining --;
    }

    flecs_rule_set_match(op, table, op_ctx->column, ctx);
    return true;
}

static
bool flecs_rule_select(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_id_t id = 0;
    if (!redo) {
        id = flecs_rule_op_get_id(op, ctx);
    }
    return flecs_rule_select_w_id(op, redo, ctx, id);
}

static
bool flecs_rule_with(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_id_record_t *idr = op_ctx->idr;
    const ecs_table_record_t *tr;

    ecs_table_t *table = flecs_rule_get_table(op, &op->src, EcsRuleSrc, ctx);
    if (!table) {
        return false;
    }

    if (!redo) {
        ecs_id_t id = flecs_rule_op_get_id(op, ctx);
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

        op_ctx->column = flecs_rule_next_column(table, idr->id, op_ctx->column);
        ecs_assert(op_ctx->column != -1, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_rule_set_match(op, table, op_ctx->column, ctx);
    return true;
}

static
bool flecs_rule_and(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_rule_with(op, redo, ctx);
    } else {
        return flecs_rule_select(op, redo, ctx);
    }
}

static
bool flecs_rule_select_id(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
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

        if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
            return false;
        }
    }

    const ecs_table_record_t *tr = flecs_table_cache_next(
        &op_ctx->it, ecs_table_record_t);
    if (!tr) {
        return false;
    }

    ecs_table_t *table = tr->hdr.table;
    flecs_rule_var_set_table(op, op->src.var, table, 0, 0, ctx);
    flecs_rule_it_set_column(it, field, tr->index);
    return true;
}

static
bool flecs_rule_with_id(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_rule_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
    ecs_iter_t *it = ctx->it;
    int8_t field = op->field_index;
    ecs_assert(field != -1, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = flecs_rule_get_table(op, &op->src, EcsRuleSrc, ctx);
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

    flecs_rule_it_set_column(it, field, tr->index);
    return true;
}

static
bool flecs_rule_and_id(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (written & (1ull << op->src.var)) {
        return flecs_rule_with_id(op, redo, ctx);
    } else {
        return flecs_rule_select_id(op, redo, ctx);
    }
}

static
bool flecs_rule_up_select(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx,
    bool self)
{
    ecs_rule_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_world_t *world = ctx->world;
    ecs_iter_t *it = ctx->it;
    bool redo_select = redo;

    if (!redo) {
        const ecs_filter_t *filter = &ctx->rule->filter;
        op_ctx->trav = filter->terms[op->term_index].src.trav;
        op_ctx->with = flecs_rule_op_get_id(op, ctx);
        op_ctx->idr_with = flecs_id_record_get(ctx->world, op_ctx->with);
        if (!op_ctx->idr_with) {
            return false;
        }

        if (!self) {
            op_ctx->idr_trav = flecs_id_record_get(ctx->world, op_ctx->trav);
            if (!op_ctx->idr_trav) {
                return false;
            }
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
                    if (!flecs_rule_select(op, redo_select, ctx)) {
                        return false;
                    }

                    redo_select = true;

                    range = flecs_rule_get_range(
                        op, &op->src, EcsRuleSrc, ctx);
                } while (!self && range.table->_->traversable_count == 0);

                if (!range.count) {
                    range.count = ecs_table_count(range.table);
                }

                table = op_ctx->table = range.table;
                op_ctx->row = range.offset;
                op_ctx->end = range.offset + range.count;
                op_ctx->matched = it->ids[op->field_index];

                if (self) {
                    flecs_reset_source_set_flag(ctx, op->field_index);
                    op_ctx->row --;
                    return true;
                }

                int32_t column = it->columns[op->field_index];
                it->columns[op->field_index] = column * -1;

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

                down = op_ctx->down = flecs_rule_get_down_cache(ctx, &op_ctx->cache, 
                    op_ctx->trav, entity, op_ctx->idr_with, self);
                op_ctx->cache_elem = -1;
            }
        }

        if ((++ op_ctx->cache_elem) >= ecs_vec_count(&down->elems)) {
            down = NULL;
            continue;
        }

        ecs_trav_down_elem_t *elem = ecs_vec_get_t(
            &down->elems, ecs_trav_down_elem_t, op_ctx->cache_elem);
        flecs_rule_var_set_table(op, op->src.var, elem->table, 0, 0, ctx);
        flecs_rule_set_vars(op, op_ctx->matched, ctx);

        if (it->columns[op->field_index] > 0) {
            it->columns[op->field_index] = -it->columns[op->field_index];
        }

        break;
    } while (true);

    flecs_set_source_set_flag(ctx, op->field_index);

    return true;
}

static
bool flecs_rule_up_with(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
    ecs_iter_t *it = ctx->it;

    if (!redo) {
        const ecs_filter_t *filter = &ctx->rule->filter;
        op_ctx->trav = filter->terms[op->term_index].src.trav;
        op_ctx->with = flecs_rule_op_get_id(op, ctx);
        op_ctx->idr_with = flecs_id_record_get(ctx->world, op_ctx->with);

        if (!op_ctx->idr_with) {
            return false;
        }

        op_ctx->idr_trav = flecs_id_record_get(ctx->world, 
            ecs_pair(op_ctx->trav, EcsWildcard));
        if (!op_ctx->idr_trav) {
            return false;
        }

        ecs_table_range_t range = flecs_rule_get_range(
            op, &op->src, EcsRuleSrc, ctx);
        if (!range.table) {
            return false;
        }

        ecs_trav_up_t *up = flecs_rule_get_up_cache(ctx, &op_ctx->cache, 
            range.table, op_ctx->with, op_ctx->trav, op_ctx->idr_with,
            op_ctx->idr_trav);

        if (!up) {
            return false;
        }

        it->sources[op->field_index] = flecs_entities_get_generation(
            ctx->world, up->src);
        it->columns[op->field_index] = -(up->column + 1);
        it->ids[op->field_index] = up->id;
        flecs_rule_set_vars(op, up->id, ctx);
        flecs_set_source_set_flag(ctx, op->field_index);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_self_up_with(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        if (flecs_rule_with(op, redo, ctx)) {
            ecs_rule_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
            op_ctx->trav = 0;
            if (flecs_rule_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar) {
                ecs_iter_t *it = ctx->it;
                it->sources[op->field_index] = 0;
            }
            return true;
        }

        flecs_reset_source_set_flag(ctx, op->field_index);

        return flecs_rule_up_with(op, redo, ctx);
    } else {
        ecs_rule_up_ctx_t *op_ctx = flecs_op_ctx(ctx, up);
        if (op_ctx->trav == 0) {
            return flecs_rule_with(op, redo, ctx);
        }
    }

    return false;
}

static
bool flecs_rule_up(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_rule_up_with(op, redo, ctx);
    } else {
        return flecs_rule_up_select(op, redo, ctx, false);
    }
}

static
bool flecs_rule_self_up(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];
    if (flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        return flecs_rule_self_up_with(op, redo, ctx);
    } else {
        return flecs_rule_up_select(op, redo, ctx, true);
    }
}

static
bool flecs_rule_and_any(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
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
        result = flecs_rule_with(op, redo, ctx);
    } else {
        result = flecs_rule_select(op, redo, ctx);
        remaining = 0;
    }

    if (!redo) {
        ecs_rule_and_ctx_t *op_ctx = flecs_op_ctx(ctx, and);
        if (match_flags & EcsTermMatchAny && op_ctx->remaining) {
            op_ctx->remaining = flecs_ito(int16_t, remaining);
        }
    }

    int32_t field = op->field_index;
    if (field != -1) {
        ctx->it->ids[field] = flecs_rule_op_get_id(op, ctx);
    }

    return result;
}

static
bool flecs_rule_trav_fixed_src_reflexive(
    const ecs_rule_op_t *op,
    const ecs_rule_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_table_t *table = range->table;
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
        ecs_assert(flecs_rule_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_var_t *var = &ctx->vars[op->src.var];
        ecs_table_range_t *var_range = &var->range;
        var_range->offset = i;
        var_range->count = 1;
        var->entity = entities[i];
    }

    flecs_rule_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

static
bool flecs_rule_trav_unknown_src_reflexive(
    const ecs_rule_op_t *op,
    const ecs_rule_run_ctx_t *ctx,
    ecs_entity_t trav,
    ecs_entity_t second)
{
    ecs_assert(flecs_rule_ref_flags(op->flags, EcsRuleSrc) & EcsRuleIsVar,
        ECS_INTERNAL_ERROR, NULL);
    ecs_var_id_t src_var = op->src.var;
    flecs_rule_var_set_entity(op, src_var, second, ctx);
    flecs_rule_var_get_table(src_var, ctx);
    flecs_rule_set_trav_match(op, -1, trav, second, ctx);
    return true;
}

static
bool flecs_rule_trav_fixed_src_up_fixed_second(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false; /* If everything's fixed, can only have a single result */
    }

    ecs_flags16_t f_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);
    ecs_flags16_t f_src = flecs_rule_ref_flags(op->flags, EcsRuleSrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;

    /* Check if table has transitive relationship by traversing upwards */
    int32_t column = ecs_search_relation(ctx->world, table, 0, 
        ecs_pair(trav, second), trav, EcsSelf|EcsUp, NULL, NULL, NULL);
    if (column == -1) {
        if (op->match_flags & EcsTermReflexive) {
            return flecs_rule_trav_fixed_src_reflexive(op, ctx,
                &range, trav, second);
        } else {
            return false;
        }
    }

    flecs_rule_set_trav_match(op, column, trav, second, ctx);
    return true;
}

static
bool flecs_rule_trav_unknown_src_up_fixed_second(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_entity_t second = flecs_get_ref_entity(&op->second, f_2nd, ctx);
    ecs_rule_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        ecs_record_t *r_second = flecs_entities_get(ctx->world, second);
        bool traversable = r_second && r_second->row & EcsEntityIsTraversable;
        bool reflexive = op->match_flags & EcsTermReflexive;
        if (!traversable && !reflexive) {
            trav_ctx->cache.id = 0;

            /* If there's no record for the entity, it can't have a subtree so
             * forward operation to a regular select. */
            return flecs_rule_select(op, redo, ctx);
        }

        /* Entity is traversable, which means it could have a subtree */
        flecs_rule_get_trav_down_cache(ctx, &trav_ctx->cache, trav, second);
        trav_ctx->index = 0;

        if (op->match_flags & EcsTermReflexive) {
            trav_ctx->index = -1;
            return flecs_rule_trav_unknown_src_reflexive(
                op, ctx, trav, second);
        }
    } else {
        if (!trav_ctx->cache.id) {
            /* No traversal cache, which means this is a regular select */
            return flecs_rule_select(op, redo, ctx);
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
        if (flecs_rule_select_w_id(op, redo, ctx, ecs_pair(trav, el->entity))) {
            return true;
        }
        
        redo = false;
    }

    return false;
}

static
bool flecs_rule_trav_yield_reflexive_src(
    const ecs_rule_op_t *op,
    const ecs_rule_run_ctx_t *ctx,
    ecs_table_range_t *range,
    ecs_entity_t trav)
{
    ecs_var_t *vars = ctx->vars;
    ecs_rule_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);
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
    flecs_rule_set_trav_match(op, -1, trav, entity, ctx);

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
bool flecs_rule_trav_fixed_src_up_unknown_second(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t f_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t f_src = flecs_rule_ref_flags(op->flags, EcsRuleSrc);
    ecs_entity_t trav = flecs_get_ref_entity(&op->first, f_1st, ctx);
    ecs_table_range_t range = flecs_get_ref_range(&op->src, f_src, ctx);
    ecs_table_t *table = range.table;
    ecs_rule_trav_ctx_t *trav_ctx = flecs_op_ctx(ctx, trav);

    if (!redo) {
        flecs_rule_get_trav_up_cache(ctx, &trav_ctx->cache, trav, table);
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
        if (flecs_rule_trav_yield_reflexive_src(op, ctx, &range, trav)) {
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
    flecs_rule_set_trav_match(op, el->column, trav, el->entity, ctx);
    return true;
}

static
bool flecs_rule_trav(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index];

    if (!flecs_ref_is_written(op, &op->src, EcsRuleSrc, written)) {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written)) {
            /* This can't happen, src or second should have been resolved */
            ecs_abort(ECS_INTERNAL_ERROR, 
                "invalid instruction sequence: unconstrained traversal");
        } else {
            return flecs_rule_trav_unknown_src_up_fixed_second(op, redo, ctx);
        }
    } else {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written)) {
            return flecs_rule_trav_fixed_src_up_unknown_second(op, redo, ctx);
        } else {
            return flecs_rule_trav_fixed_src_up_fixed_second(op, redo, ctx);
        }
    }
}

static
bool flecs_rule_idsright(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_ids_ctx_t *op_ctx = flecs_op_ctx(ctx, ids);
    ecs_id_record_t *cur;

    if (!redo) {
        ecs_id_t id = flecs_rule_op_get_id(op, ctx);
        if (!ecs_id_is_wildcard(id)) {
            /* If id is not a wildcard, we can directly return it. This can 
             * happen if a variable was constrained by an iterator. */
            op_ctx->cur = NULL;
            flecs_rule_set_vars(op, id, ctx);
            return true;
        }

        cur = op_ctx->cur = flecs_id_record_get(ctx->world, id);
        if (!cur) {
            return false;
        }

        cur = op_ctx->cur = cur->first.next;
    } else {
        if (!op_ctx->cur) {
            return false;
        }

        cur = op_ctx->cur = op_ctx->cur->first.next;
    }

    if (!cur) {
        return false;
    }

    flecs_rule_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        ecs_iter_t *it = ctx->it;
        ecs_id_t id = flecs_rule_op_get_id_w_written(op, op->written, ctx);
        it->ids[op->field_index] = id;
    }

    return true;
}

static
bool flecs_rule_idsleft(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_ids_ctx_t *op_ctx = flecs_op_ctx(ctx, ids);
    ecs_id_record_t *cur;

    if (!redo) {
        ecs_id_t id = flecs_rule_op_get_id(op, ctx);
        if (!ecs_id_is_wildcard(id)) {
            /* If id is not a wildcard, we can directly return it. This can 
             * happen if a variable was constrained by an iterator. */
            op_ctx->cur = NULL;
            flecs_rule_set_vars(op, id, ctx);
            return true;
        }

        cur = op_ctx->cur = flecs_id_record_get(ctx->world, id);
        if (!cur) {
            return false;
        }

        cur = op_ctx->cur = cur->second.next;
    } else {
        if (!op_ctx->cur) {
            return false;
        }

        cur = op_ctx->cur = op_ctx->cur->second.next;
    }

    if (!cur) {
        return false;
    }

    flecs_rule_set_vars(op, cur->id, ctx);

    if (op->field_index != -1) {
        ecs_iter_t *it = ctx->it;
        ecs_id_t id = flecs_rule_op_get_id_w_written(op, op->written, ctx);
        it->ids[op->field_index] = id;
    }

    return true;
}

static
bool flecs_rule_each(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_each_ctx_t *op_ctx = flecs_op_ctx(ctx, each);
    int32_t row;

    ecs_table_range_t range = flecs_rule_var_get_range(op->first.var, ctx);
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
    ecs_entity_t e;
    do {
        e = entities[row ++];
    
        /* Exclude entities that are used as markers by rule engine */
    } while ((e == EcsWildcard) || (e == EcsAny) || 
        (e == EcsThis) || (e == EcsVariable));

    flecs_rule_var_set_entity(op, op->src.var, e, ctx);

    return true;
}

static
bool flecs_rule_store(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        flecs_rule_var_set_entity(op, op->src.var, op->first.entity, ctx);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_reset(
    const ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        return true;
    } else {
        flecs_rule_var_reset(op->src.var, ctx);
        return false;
    }
}

static
bool flecs_rule_union(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ctx->jump = flecs_itolbl(ctx->op_index + 1);
        return true;
    } else {
        ecs_rule_lbl_t next = flecs_itolbl(ctx->prev_index + 1);
        if (next == op->next) {
            return false;
        }
        ctx->jump = next;
        return true;
    }
}

static
bool flecs_rule_end(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    (void)op;

    ecs_rule_ctrlflow_ctx_t *op_ctx = flecs_op_ctx(ctx, ctrlflow);
    if (!redo) {
        op_ctx->lbl = ctx->prev_index;
        return true;
    } else {
        ctx->jump = op_ctx->lbl;
        return true;
    }
}

static
bool flecs_rule_not(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    int32_t field = op->field_index;
    if (field == -1) {
        return true;
    }

    ecs_iter_t *it = ctx->it;

    /* Not terms return no data */
    it->columns[field] = 0;

    /* Ignore variables written by Not operation */
    uint64_t *written = ctx->written;
    uint64_t written_cur = written[ctx->op_index] = written[op->prev + 1];
    ecs_flags16_t flags_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);

    /* Overwrite id with cleared out variables */
    ecs_id_t id = flecs_rule_op_get_id(op, ctx);
    if (id) {
        it->ids[field] = id;
    }

    /* Reset variables */
    if (flags_1st & EcsRuleIsVar) {
        if (!flecs_ref_is_written(op, &op->first, EcsRuleFirst, written_cur)){
            flecs_rule_var_reset(op->first.var, ctx);
        }
    }
    if (flags_2nd & EcsRuleIsVar) {
        if (!flecs_ref_is_written(op, &op->second, EcsRuleSecond, written_cur)){
            flecs_rule_var_reset(op->second.var, ctx);
        }
    }

    /* If term has entity src, set it because no other instruction might */
    if (op->flags & (EcsRuleIsEntity << EcsRuleSrc)) {
        it->sources[field] = op->src.entity;
    }

    return true; /* Flip result */
}

static
const char* flecs_rule_name_arg(
    const ecs_rule_op_t *op,
    ecs_rule_run_ctx_t *ctx)
{
    int8_t term_index = op->term_index;
    ecs_term_t *term = &ctx->rule->filter.terms[term_index];
    return term->second.name;
}

static
bool flecs_rule_compare_range(
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
bool flecs_rule_pred_eq_w_range(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx,
    ecs_table_range_t r)
{
    if (redo) {
        return false;
    }

    uint64_t written = ctx->written[ctx->op_index];
    ecs_var_id_t first_var = op->first.var;
    if (!(written & (1ull << first_var))) {
        /* left = unknown, right = known. Assign right-hand value to left */
        ecs_var_id_t l = first_var;
        ctx->vars[l].range = r;
        if (r.count == 1) {
            ctx->vars[l].entity = ecs_vec_get_t(&r.table->data.entities, 
                ecs_entity_t, r.offset)[0];
        }
        return true;
    } else {
        ecs_table_range_t l = flecs_rule_get_range(
            op, &op->first, EcsRuleFirst, ctx);

        if (!flecs_rule_compare_range(&l, &r)) {
            return false;
        }

        ctx->vars[first_var].range.offset = r.offset;
        ctx->vars[first_var].range.count = r.count;
        return true;
    }
}

static
bool flecs_rule_pred_eq(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsRuleSecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized eq operand");

    ecs_table_range_t r = flecs_rule_get_range(
        op, &op->second, EcsRuleSecond, ctx);
    return flecs_rule_pred_eq_w_range(op, redo, ctx, r);
}

static
bool flecs_rule_pred_eq_name(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    const char *name = flecs_rule_name_arg(op, ctx);
    ecs_entity_t e = ecs_lookup_fullpath(ctx->world, name);
    if (!e) {
        /* Entity doesn't exist */
        return false;
    }

    ecs_table_range_t r = flecs_range_from_entity(e, ctx);
    return flecs_rule_pred_eq_w_range(op, redo, ctx, r);
}

static
bool flecs_rule_pred_neq_w_range(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx,
    ecs_table_range_t r)
{
    ecs_rule_eq_ctx_t *op_ctx = flecs_op_ctx(ctx, eq);
    ecs_var_id_t first_var = op->first.var;
    ecs_table_range_t l = flecs_rule_get_range(
        op, &op->first, EcsRuleFirst, ctx);

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
    ecs_var_t *var = &ctx->vars[first_var];
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
bool flecs_rule_pred_match(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx,
    bool is_neq)
{
    ecs_rule_eq_ctx_t *op_ctx = flecs_op_ctx(ctx, eq);
    uint64_t written = ctx->written[ctx->op_index];
    ecs_assert(flecs_ref_is_written(op, &op->first, EcsRuleFirst, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized match operand");
    (void)written;

    ecs_var_id_t first_var = op->first.var;
    const char *match = flecs_rule_name_arg(op, ctx);
    ecs_table_range_t l;
    if (!redo) {
        l = flecs_rule_get_range(op, &op->first, EcsRuleFirst, ctx);
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
        ctx->vars[first_var].range = op_ctx->range;
        return false;
    }

    ctx->vars[first_var].range.offset = offset;
    ctx->vars[first_var].range.count = (op_ctx->index - offset);
    return true;
}

static
bool flecs_rule_pred_eq_match(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    return flecs_rule_pred_match(op, redo, ctx, false);
}

static
bool flecs_rule_pred_neq_match(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    return flecs_rule_pred_match(op, redo, ctx, true);
}

static
bool flecs_rule_pred_neq(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    uint64_t written = ctx->written[ctx->op_index]; (void)written;
    ecs_assert(flecs_ref_is_written(op, &op->second, EcsRuleSecond, written),
        ECS_INTERNAL_ERROR, 
            "invalid instruction sequence: uninitialized neq operand");

    ecs_table_range_t r = flecs_rule_get_range(
        op, &op->second, EcsRuleSecond, ctx);
    return flecs_rule_pred_neq_w_range(op, redo, ctx, r);
}

static
bool flecs_rule_pred_neq_name(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    const char *name = flecs_rule_name_arg(op, ctx);
    ecs_entity_t e = ecs_lookup_fullpath(ctx->world, name);
    if (!e) {
        /* Entity doesn't exist */
        return true && !redo;
    }

    ecs_table_range_t r = flecs_range_from_entity(e, ctx);
    return flecs_rule_pred_neq_w_range(op, redo, ctx, r);
}

static
bool flecs_rule_lookup(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    const ecs_rule_t *rule = ctx->rule;
    ecs_entity_t first = flecs_rule_var_get_entity(op->first.var, ctx);
    ecs_rule_var_t *var = &rule->vars[op->src.var];

    ecs_entity_t result = ecs_lookup_path_w_sep(ctx->world, first, var->lookup,
        NULL, NULL, false);
    if (!result) {
        flecs_rule_var_set_entity(op, op->src.var, EcsWildcard, ctx);
        return false;
    }

    flecs_rule_var_set_entity(op, op->src.var, result, ctx);

    return true;
}

static
bool flecs_rule_setvars(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    (void)op;

    const ecs_rule_t *rule = ctx->rule;
    const ecs_filter_t *filter = &rule->filter;
    ecs_var_id_t *src_vars = rule->src_vars;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    ecs_flags32_t source_set = *ctx->source_set;
    for (i = 0; i < filter->field_count; i ++) {
        ecs_var_id_t var_id = src_vars[i];
        if (!var_id) {
            continue;
        }

        if (source_set & (1u << i)) {
            continue;
        }

        it->sources[i] = flecs_rule_var_get_entity(var_id, ctx);
        int32_t column = it->columns[i];
        if (column > 0) {
            it->columns[i] = -column;
        }
    }

    return true;
}

static
bool flecs_rule_setthis(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_setthis_ctx_t *op_ctx = flecs_op_ctx(ctx, setthis);
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
bool flecs_rule_setfixed(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    (void)op;
    const ecs_rule_t *rule = ctx->rule;
    const ecs_filter_t *filter = &rule->filter;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    for (i = 0; i < filter->term_count; i ++) {
        ecs_term_t *term = &filter->terms[i];
        ecs_term_id_t *src = &term->src;
        if (src->flags & EcsIsEntity) {
            it->sources[term->field_index] = src->id;
        }
    }

    return true;
}

static
bool flecs_rule_setids(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    (void)op;
    const ecs_rule_t *rule = ctx->rule;
    const ecs_filter_t *filter = &rule->filter;
    ecs_iter_t *it = ctx->it;

    if (redo) {
        return false;
    }

    int32_t i;
    for (i = 0; i < filter->term_count; i ++) {
        ecs_term_t *term = &filter->terms[i];
        it->ids[term->field_index] = term->id;
    }

    return true;
}

/* Check if entity is stored in table */
static
bool flecs_rule_contain(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_var_id_t src_id = op->src.var;
    ecs_var_id_t first_id = op->first.var;

    ecs_table_t *table = flecs_rule_var_get_table(src_id, ctx);

    ecs_entity_t e = flecs_rule_var_get_entity(first_id, ctx);
    return table == ecs_get_table(ctx->world, e);
}

/* Check if first and second id of pair from last operation are the same */
static
bool flecs_rule_pair_eq(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    ecs_iter_t *it = ctx->it;
    ecs_id_t id = it->ids[op->field_index];
    return ECS_PAIR_FIRST(id) == ECS_PAIR_SECOND(id);
}

static
bool flecs_rule_jmp_if_not(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        flecs_op_ctx(ctx, cond)->cond = false;
        return true;
    } else {
        if (!flecs_op_ctx(ctx, cond)->cond) {
            ctx->jump = op->other;
        }
        return false;
    }
}

static
bool flecs_rule_jmp_set_cond(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ctx->op_ctx[op->other].is.cond.cond = true;
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_jmp_not_set(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ecs_var_t *vars = ctx->vars;
        if (flecs_rule_ref_flags(op->flags, EcsRuleFirst) == EcsRuleIsVar) {
            if (vars[op->first.var].entity == EcsWildcard) {
                ctx->jump = op->other;
                return true;
            }
        }
        if (flecs_rule_ref_flags(op->flags, EcsRuleSecond) == EcsRuleIsVar) {
            if (vars[op->second.var].entity == EcsWildcard) {
                ctx->jump = op->other;
                return true;
            }
        }
        if (flecs_rule_ref_flags(op->flags, EcsRuleSrc) == EcsRuleIsVar) {
            if (vars[op->src.var].entity == EcsWildcard) {
                ctx->jump = op->other;
                return true;
            }
        }

        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_run(
    const ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    switch(op->kind) {
    case EcsRuleAnd: return flecs_rule_and(op, redo, ctx);
    case EcsRuleAndId: return flecs_rule_and_id(op, redo, ctx);
    case EcsRuleAndAny: return flecs_rule_and_any(op, redo, ctx);
    case EcsRuleUp: return flecs_rule_up(op, redo, ctx);
    case EcsRuleSelfUp: return flecs_rule_self_up(op, redo, ctx);
    case EcsRuleWith: return flecs_rule_with(op, redo, ctx);
    case EcsRuleTrav: return flecs_rule_trav(op, redo, ctx);
    case EcsRuleIdsRight: return flecs_rule_idsright(op, redo, ctx);
    case EcsRuleIdsLeft: return flecs_rule_idsleft(op, redo, ctx);
    case EcsRuleEach: return flecs_rule_each(op, redo, ctx);
    case EcsRuleStore: return flecs_rule_store(op, redo, ctx);
    case EcsRuleReset: return flecs_rule_reset(op, redo, ctx);
    case EcsRuleUnion: return flecs_rule_union(op, redo, ctx);
    case EcsRuleEnd: return flecs_rule_end(op, redo, ctx);
    case EcsRuleNot: return flecs_rule_not(op, redo, ctx);
    case EcsRulePredEq: return flecs_rule_pred_eq(op, redo, ctx);
    case EcsRulePredNeq: return flecs_rule_pred_neq(op, redo, ctx);
    case EcsRulePredEqName: return flecs_rule_pred_eq_name(op, redo, ctx);
    case EcsRulePredNeqName: return flecs_rule_pred_neq_name(op, redo, ctx);
    case EcsRulePredEqMatch: return flecs_rule_pred_eq_match(op, redo, ctx);
    case EcsRulePredNeqMatch: return flecs_rule_pred_neq_match(op, redo, ctx);
    case EcsRuleLookup: return flecs_rule_lookup(op, redo, ctx);
    case EcsRuleSetVars: return flecs_rule_setvars(op, redo, ctx);
    case EcsRuleSetThis: return flecs_rule_setthis(op, redo, ctx);
    case EcsRuleSetFixed: return flecs_rule_setfixed(op, redo, ctx);
    case EcsRuleSetIds: return flecs_rule_setids(op, redo, ctx);
    case EcsRuleContain: return flecs_rule_contain(op, redo, ctx);
    case EcsRulePairEq: return flecs_rule_pair_eq(op, redo, ctx);
    case EcsRuleJmpCondFalse: return flecs_rule_jmp_if_not(op, redo, ctx);
    case EcsRuleSetCond: return flecs_rule_jmp_set_cond(op, redo, ctx);
    case EcsRuleJmpNotSet: return flecs_rule_jmp_not_set(op, redo, ctx);
    case EcsRuleYield: return false;
    case EcsRuleNothing: return false;
    }
    return false;
}

static
void flecs_rule_iter_init(
    ecs_rule_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    if (ctx->written) {
        const ecs_rule_t *rule = ctx->rule;
        ecs_flags64_t it_written = it->constrained_vars;
        ctx->written[0] = it_written;
        if (it_written && ctx->rule->src_vars) {
            /* If variables were constrained, check if there are any table
             * variables that have a constrained entity variable. */
            ecs_var_t *vars = ctx->vars;
            int32_t i, count = rule->filter.field_count;
            for (i = 0; i < count; i ++) {
                ecs_var_id_t var_id = rule->src_vars[i];
                ecs_rule_var_t *var = &rule->vars[var_id];

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
    }

    flecs_iter_validate(it);
}

bool ecs_rule_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_rule_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_rule_next_instanced(it));
error:
    return false;
}

bool ecs_rule_next_instanced(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_rule_next, ECS_INVALID_PARAMETER, NULL);

    ecs_rule_iter_t *rit = &it->priv.iter.rule;
    bool redo = it->flags & EcsIterIsValid;
    ecs_rule_lbl_t next;

    ecs_rule_run_ctx_t ctx;
    ctx.world = it->real_world;
    ctx.rule = rit->rule;
    ctx.it = it;
    ctx.vars = rit->vars;
    ctx.rule_vars = rit->rule_vars;
    ctx.written = rit->written;
    ctx.prev_index = -1;
    ctx.jump = -1;
    ctx.op_ctx = rit->op_ctx;
    ctx.source_set = &rit->source_set;
    const ecs_rule_op_t *ops = rit->ops;

    if (!(it->flags & EcsIterIsValid)) {
        if (!ctx.rule) {
            goto done;
        }
        flecs_rule_iter_init(&ctx);
    }

    do {
        ctx.op_index = rit->op;
        const ecs_rule_op_t *op = &ops[ctx.op_index];

#ifdef FLECS_DEBUG
        rit->profile[ctx.op_index].count[redo] ++;
#endif

        bool result = flecs_rule_run(op, redo, &ctx);
        ctx.prev_index = ctx.op_index;

        next = (&op->prev)[result];
        if (ctx.jump != -1) {
            next = ctx.jump;
            ctx.jump = -1;
        }

        if ((next > ctx.op_index)) {
            ctx.written[next] |= ctx.written[ctx.op_index] | op->written;
        }

        redo = next < ctx.prev_index;
        rit->op = next;

        if (op->kind == EcsRuleYield) {
            ecs_table_range_t *range = &rit->vars[0].range;
            ecs_table_t *table = range->table;
            if (table && !range->count) {
                range->count = ecs_table_count(table);
            }
            flecs_iter_populate_data(ctx.world, it, range->table, range->offset,
                range->count, it->ptrs);
            if (!table && range->count == 1) {
                it->count = 1;
                it->entities = &rit->vars[0].entity;
            }
            return true;
        }
    } while (next >= 0);

done:
    ecs_iter_fini(it);
    return false;
}

static
void flecs_rule_iter_fini_ctx(
    ecs_iter_t *it,
    ecs_rule_iter_t *rit)
{
    const ecs_rule_t *rule = rit->rule;
    int32_t i, count = rule->op_count;
    ecs_rule_op_t *ops = rule->ops;
    ecs_rule_op_ctx_t *ctx = rit->op_ctx;
    ecs_allocator_t *a = flecs_rule_get_allocator(it);

    for (i = 0; i < count; i ++) {
        ecs_rule_op_t *op = &ops[i];
        switch(op->kind) {
        case EcsRuleTrav:
            flecs_rule_trav_cache_fini(a, &ctx[i].is.trav.cache);
            break;
        case EcsRuleUp:
        case EcsRuleSelfUp: {
            ecs_trav_up_cache_t *cache = &ctx[i].is.up.cache;
            if (cache->dir == EcsDown) {
                flecs_rule_down_cache_fini(a, cache);
            } else {
                flecs_rule_up_cache_fini(cache);
            }
            break;
        }
        default:
            break;
        }
    }
}

static
void flecs_rule_iter_fini(
    ecs_iter_t *it)
{
    ecs_rule_iter_t *rit = &it->priv.iter.rule;
    ecs_assert(rit->rule != NULL, ECS_INVALID_OPERATION, NULL);
    ecs_poly_assert(rit->rule, ecs_rule_t);
    int32_t op_count = rit->rule->op_count;
    int32_t var_count = rit->rule->var_count;

#ifdef FLECS_DEBUG
    if (it->flags & EcsIterProfile) {
        char *str = ecs_rule_str_w_profile(rit->rule, it);
        printf("%s\n", str);
        ecs_os_free(str);
    }

    flecs_iter_free_n(rit->profile, ecs_rule_op_profile_t, op_count);
#endif

    flecs_rule_iter_fini_ctx(it, rit);
    flecs_iter_free_n(rit->vars, ecs_var_t, var_count);
    flecs_iter_free_n(rit->written, ecs_write_flags_t, op_count);
    flecs_iter_free_n(rit->op_ctx, ecs_rule_op_ctx_t, op_count);
    rit->vars = NULL;
    rit->written = NULL;
    rit->op_ctx = NULL;
    rit->rule = NULL;
}

ecs_iter_t ecs_rule_iter(
    const ecs_world_t *world,
    const ecs_rule_t *rule)
{
    ecs_iter_t it = {0};
    ecs_rule_iter_t *rit = &it.priv.iter.rule;
    ecs_check(rule != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_run_aperiodic(rule->filter.world, EcsAperiodicEmptyTables);

    int32_t i, var_count = rule->var_count, op_count = rule->op_count;
    it.world = ECS_CONST_CAST(ecs_world_t*, world);
    it.real_world = rule->filter.world;
    it.terms = rule->filter.terms;
    it.next = ecs_rule_next;
    it.fini = flecs_rule_iter_fini;
    it.field_count = rule->filter.field_count;
    it.sizes = rule->filter.sizes;
    flecs_filter_apply_iter_flags(&it, &rule->filter);

    flecs_iter_init(world, &it, 
        flecs_iter_cache_ids |
        flecs_iter_cache_columns |
        flecs_iter_cache_sources |
        flecs_iter_cache_ptrs);

    rit->rule = rule;
    rit->rule_vars = rule->vars;
    rit->ops = rule->ops;
    rit->source_set = 0;
    if (var_count) {
        rit->vars = flecs_iter_calloc_n(&it, ecs_var_t, var_count);
    }
    if (op_count) {
        rit->written = flecs_iter_calloc_n(&it, ecs_write_flags_t, op_count);
        rit->op_ctx = flecs_iter_calloc_n(&it, ecs_rule_op_ctx_t, op_count);
    }

#ifdef FLECS_DEBUG
    rit->profile = flecs_iter_calloc_n(&it, ecs_rule_op_profile_t, op_count);
#endif

    for (i = 0; i < var_count; i ++) {
        rit->vars[i].entity = EcsWildcard;
    }

    it.variables = rit->vars;
    it.variable_count = rule->var_pub_count;
    it.variable_names = rule->var_names;

error:
    return it;
}

#endif
