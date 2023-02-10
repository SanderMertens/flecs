/**
 * @file addons/rules/engine.c
 * @brief Rules engine implementation.
 */

#include "rules.h"

#ifdef FLECS_RULES

static
ecs_table_range_t flecs_range_from_entity(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_record_t *r = flecs_entities_get(world, e);
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
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    int32_t r)
{
    ecs_assert(r < rule->var_count, ECS_INTERNAL_ERROR, NULL);
    ecs_rule_var_t *var = &rule->vars[r];
    if (var->kind == EcsVarEntity) {
        return flecs_range_from_entity(rule->world, vars[r].entity);
    }
    return vars[r].range;
}

static
ecs_table_t* flecs_rule_var_get_table(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    int32_t r)
{
    return flecs_rule_var_get_range(rule, vars, r).table;
}

static
ecs_table_t* flecs_rule_get_table(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    const ecs_rule_op_t *op,
    ecs_rule_ref_t *ref,
    ecs_flags16_t ref_kind)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, ref_kind);
    if (flags & EcsRuleIsEntity) {
        return ecs_get_table(rule->world, ref->entity);
    } else {
        return flecs_rule_var_get_table(rule, vars, ref->var);
    }
}

static
ecs_entity_t flecs_rule_var_get_entity(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    ecs_var_id_t var_id)
{
    ecs_assert(var_id < (ecs_var_id_t)rule->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_rule_var_t *var = &rule->vars[var_id];
    if (var->kind == EcsVarTable) {
        const ecs_table_range_t *range = &vars[var_id].range;
        ecs_assert(range->count == 1, ECS_INTERNAL_ERROR, NULL);
        ecs_table_t *table = range->table;
        ecs_entity_t *entities = table->data.entities.array;
        return entities[range->offset];
    }
    return vars[var_id].entity;
}

static
void flecs_rule_var_set_table(
    const ecs_rule_t *rule,
    const ecs_rule_op_t *op,
    ecs_var_t *vars,
    ecs_var_id_t var_id,
    ecs_table_t *table)
{
    ecs_assert(rule->vars[var_id].kind == EcsVarTable, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_rule_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    vars[var_id].range = (ecs_table_range_t){ 
        .table = table,
        .offset = 0,
        .count = ecs_table_count(table)
    };
}

static
void flecs_rule_var_set_entity(
    const ecs_rule_t *rule,
    const ecs_rule_op_t *op,
    ecs_var_t *vars,
    ecs_var_id_t var_id,
    ecs_entity_t entity)
{
    ecs_assert(var_id < (ecs_var_id_t)rule->var_count, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(flecs_rule_is_written(var_id, op->written), 
        ECS_INTERNAL_ERROR, NULL);
    if (rule->vars[var_id].kind == EcsVarEntity) {
        vars[var_id].entity = entity;
    } else {
        vars[var_id].range = flecs_range_from_entity(rule->world, entity);
    }
}

static
void flecs_rule_set_vars(
    const ecs_rule_t *rule,
    const ecs_rule_op_t *op, 
    ecs_var_t *vars, 
    ecs_id_t id)
{
    ecs_flags16_t flags_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);
    ecs_entity_t first = 0, second = 0;

    if (ECS_IS_PAIR(id)) {
        first = ecs_pair_first(rule->world, id);
        second = ecs_pair_second(rule->world, id);
    } else {
        first = id;
    }

    if (flags_1st & EcsRuleIsVar) {
        vars[op->first.var].entity = first;
    }
    if (flags_2nd & EcsRuleIsVar) {
        vars[op->second.var].entity = second;
    }
}

static
ecs_table_range_t flecs_get_ref_range(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t flag)
{
    if (flag & EcsRuleIsEntity) {
        return flecs_range_from_entity(rule->world, ref->entity);
    } else if (flag & EcsRuleIsVar) {
        return flecs_rule_var_get_range(rule, vars, ref->var);
    }
    return (ecs_table_range_t){0};
}

static
ecs_entity_t flecs_get_ref_entity(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t flag)
{
    if (flag & EcsRuleIsEntity) {
        return ref->entity;
    } else if (flag & EcsRuleIsVar) {
        return flecs_rule_var_get_entity(rule, vars, ref->var);
    }
    return 0;
}

static
ecs_id_t flecs_rule_op_get_id(
    const ecs_rule_t *rule,
    const ecs_var_t *vars,
    const ecs_rule_op_t *op,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_flags16_t flags_1st = flecs_rule_ref_flags(op->flags, EcsRuleFirst);
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);
    ecs_entity_t first = 0, second = 0;
    uint64_t written = ctx->rit->written[ctx->op_index];

    if (flags_1st) {
        if (flecs_ref_is_written(rule, op, &op->first, EcsRuleFirst, written)) {
            first = flecs_get_ref_entity(rule, vars, &op->first, flags_1st);
        } else {
            first = EcsWildcard;
        }
    }
    if (flags_2nd) {
        if (flecs_ref_is_written(rule, op, &op->second, EcsRuleSecond, written)) {
            second = flecs_get_ref_entity(rule, vars, &op->second, flags_2nd);
        } else {
            second = EcsWildcard;
        }
    }

    if (second) {
        return ecs_pair(first, second);
    } else {
        return ecs_get_alive(rule->world, first);
    }
}

static
int32_t flecs_rule_next_column(
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
    return column;
}

static
void flecs_rule_it_set_column(
    ecs_iter_t *it,
    int32_t field_index,
    int32_t column)
{
    it->columns[field_index] = column + 1;
    if (it->sources[field_index] != 0) {
        it->columns[field_index] *= -1;
    }
}

static
int32_t flecs_rule_it_get_column(
    ecs_iter_t *it,
    int32_t field_index)
{
    int32_t column = it->columns[field_index];
    if (it->sources[field_index] != 0) {
        column *= -1;
    }
    return column - 1;
}

static
void flecs_rule_set_match(
    ecs_rule_op_t *op,
    ecs_table_t *table,
    int32_t column,
    const ecs_rule_run_ctx_t *ctx)
{
    ecs_iter_t *it = ctx->it;
    const ecs_rule_t *rule = ctx->rule;
    int32_t field_index = op->field_index;
    ecs_var_t *vars = ctx->rit->vars;

    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, EcsRuleSrc);
    if (flags & EcsRuleIsEntity) {
        it->sources[field_index] = op->src.entity;
    }

    flecs_rule_it_set_column(it, field_index, column);
    
    if (!(op->match_flags & EcsRuleMatchAny)) {
        if (column >= 0) {
            ecs_id_t matched = table->type.array[column];
            it->ids[field_index] = matched;
            flecs_rule_set_vars(rule, op, vars, matched);
        }
    } else {
        it->ids[field_index] = flecs_rule_op_get_id(
            rule, ctx->rit->vars, op, ctx);
    }
}

typedef struct {
    ecs_entity_t root;
    ecs_table_t *table;
} ecs_trav_elem_t;

static
ecs_vector_t* flecs_rule_traverse_down_table(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t root,
    ecs_entity_t trav,
    ecs_vector_t *tables,
    ecs_id_record_t *idr_not);

static
ecs_vector_t* flecs_rule_traverse_down(
    const ecs_world_t *world,
    ecs_vector_t *tables,
    ecs_entity_t root,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_not)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, ecs_pair(trav, entity));
    if (!idr) {
        return tables;
    }

    int32_t t, last, first = ecs_vector_count(tables);

    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&idr->cache, &it)) {
        ecs_table_record_t *tr; 
        while ((tr = flecs_table_cache_next(&it, ecs_table_record_t))) {
            ecs_assert(tr->count == 1, ECS_INTERNAL_ERROR, NULL);
            ecs_table_t *table = tr->hdr.table;
            if (idr_not) {
                if (flecs_id_record_get_table(idr_not, table) != NULL) {
                    continue;
                }
            }

            ecs_trav_elem_t *elem = ecs_vector_add(&tables, ecs_trav_elem_t);
            elem->root = root;
            elem->table = table;
        }
    }

    /* Breadth first walk */
    last = ecs_vector_count(tables);
    for (t = first; t < last; t ++) {
        ecs_trav_elem_t *elem = ecs_vector_get(tables, ecs_trav_elem_t, t);
        tables = flecs_rule_traverse_down_table(
            world, elem->table, root, trav, tables, idr_not);
    }

    return tables;
}

static
ecs_vector_t* flecs_rule_traverse_down_table(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t root,
    ecs_entity_t trav,
    ecs_vector_t *tables,
    ecs_id_record_t *idr_not)
{
    if (!table->observed_count) {
        return tables;
    }

    bool is_root = root == 0;
    ecs_entity_t *entities = ecs_vec_first(&table->data.entities);
    ecs_record_t **records = ecs_vec_first(&table->data.records);
    int32_t i, count = ecs_table_count(table);
    for (i = 0; i < count; i ++) {
        ecs_record_t *record = records[i];
        if (!record) {
            continue;
        }

        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        if (flags & EcsEntityIsTraversable) {
            ecs_entity_t entity = entities[i];
            if (is_root) {
                root = entity;
            }

            tables = flecs_rule_traverse_down(world, tables,
                root, trav, entity, idr_not);
        }
    }
    
    return tables;
}

static
bool flecs_rule_select(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    const ecs_world_t *world = rule->world;
    ecs_iter_t *it = ctx->it;
    ecs_id_t id = flecs_rule_op_get_id(rule, ctx->rit->vars, op, ctx);
    ecs_rule_and_ctx_t *op_ctx = &ctx->ctx->is.and;
    ecs_id_record_t *idr = op_ctx->idr;
    ecs_flags8_t match_flags = op->match_flags;
    ecs_table_record_t *tr;
    ecs_table_t *table;
    int32_t column = -1;

    if (!idr || idr->id != id) {
        idr = flecs_id_record_get(world, id);
    }

    if (!idr) {
        return false;
    }

    if (!redo) {
        if (!flecs_table_cache_iter(&idr->cache, &op_ctx->it)) {
            return false;
        }
        op_ctx->index = 0;
        op_ctx->table = NULL;
    } else {
        if (match_flags & EcsRuleMatchAnySrc) {
            return false;
        }
    }

    if (!redo || !op_ctx->remaining) {

repeat:
        do {
            tr = flecs_table_cache_next(&op_ctx->it, ecs_table_record_t);
            if (!tr) {
                return false;
            }

            column = tr->column;
            table = tr->hdr.table;
            op_ctx->remaining = tr->count - 1;
            op_ctx->table = table;

            if (op->match_flags & EcsRuleMatchAny) {
                op_ctx->remaining = 0;
            }

            if (!(op->flags & EcsRuleIsSelf)) {
                redo = true;
                goto repeat;
            }

            it->sources[op->field_index] = 0;
        } while(false);
    } else {
        tr = (ecs_table_record_t*)op_ctx->it.cur;
        ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
        table = tr->hdr.table;
        column = flecs_rule_next_column(table, id, 
            flecs_rule_it_get_column(it, op->field_index));
        op_ctx->remaining --;
    }

    ecs_var_id_t var_id = op->src.var;
    flecs_rule_var_set_table(rule, op, ctx->rit->vars, var_id, table);
    if (!var_id) {
        it->table = table;
        it->entities = table->data.entities.array;
        it->count = table->data.entities.count;
    }

    flecs_rule_set_match(op, table, column, ctx);
    return true;
}

static
bool flecs_rule_with(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    const ecs_world_t *world = rule->world;
    ecs_iter_t *it = ctx->it;
    ecs_rule_iter_t *rit = ctx->rit;
    ecs_id_t id = flecs_rule_op_get_id(rule, rit->vars, op, ctx);
    ecs_rule_and_ctx_t *op_ctx = &ctx->ctx->is.and;
    ecs_id_record_t *idr = op_ctx->idr;
    const ecs_table_record_t *tr;
    int32_t column;

    ecs_table_t *table = flecs_rule_get_table(
        rule, rit->vars, op, &op->src, EcsRuleSrc);
    if (!table) {
        return false;
    }

    if (!redo) {
        if (!idr || idr->id != id) {
            idr = flecs_id_record_get(world, id);
        }

        if (!idr) {
            return false;
        }

        tr = flecs_id_record_get_table(idr, table);
        if (!tr) {
            return false;
        }

        op_ctx->idr = idr;
        op_ctx->remaining = tr->count;
        column = tr->column;

        if (op->match_flags & EcsRuleMatchAny && op_ctx->remaining) {
            op_ctx->remaining = 1;
        }
    } else {
        if (--op_ctx->remaining <= 0) {
            return false;
        }

        column = flecs_rule_next_column(table, id, 
            flecs_rule_it_get_column(it, op->field_index));
        ecs_assert(column != -1, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_rule_set_match(op, table, column, ctx);

    return true;
}

static
bool flecs_rule_and(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    uint64_t written = ctx->rit->written[ctx->op_index];

    if (!flecs_ref_is_written(rule, op, &op->src, EcsRuleSrc, written)) {
        return flecs_rule_select(op, redo, ctx);
    } else {
        return flecs_rule_with(op, redo, ctx);
    }
}

static
bool flecs_rule_each(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    ecs_rule_iter_t *rit = ctx->rit;
    ecs_rule_each_ctx_t *op_ctx = &ctx->ctx->is.each;
    int32_t row;

    ecs_table_range_t range = flecs_rule_var_get_range(
        rule, rit->vars, op->first.var);
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

    ecs_entity_t *entities = table->data.entities.array;
    flecs_rule_var_set_entity(rule, op, rit->vars, op->src.var, entities[row]);
    return true;
}

static
bool flecs_rule_down(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    ecs_rule_down_ctx_t *op_ctx = &ctx->ctx->is.down;
    ecs_id_t id = flecs_rule_op_get_id(rule, ctx->rit->vars, op, ctx);

    int32_t index;
    if (!redo) {
        if (id != op_ctx->id) {
            ecs_vector_clear(op_ctx->tables);
            op_ctx->tables = flecs_rule_traverse_down(
                rule->world, op_ctx->tables, 0, ECS_PAIR_FIRST(id),
                ecs_pair_second(rule->world, id), NULL);
        }
        index = op_ctx->index = 0;
    } else {
        index = ++ op_ctx->index;
    }

    int32_t count = ecs_vector_count(op_ctx->tables);
    if (index >= count) {
        return false;
    }

    ecs_trav_elem_t *elem = ecs_vector_get(op_ctx->tables, ecs_trav_elem_t, index);
    flecs_rule_var_set_table(rule, op, ctx->rit->vars, op->src.var, elem->table);

    return true;
}

static
bool flecs_rule_store(
    ecs_rule_op_t *op,
    bool redo,
    const ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    ecs_rule_iter_t *rit = ctx->rit;

    if (!redo) {
        flecs_rule_var_set_entity(rule, op, rit->vars, op->src.var, 
            op->first.entity);
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_union(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ctx->jump = ctx->op_index + 1;
        return true;
    } else {
        int32_t next = ctx->prev_index + 1;
        if (next == op->next) {
            return false;
        }
        ctx->jump = next;
        return true;
    }
}

static
bool flecs_rule_end(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    ecs_rule_ctrlflow_ctx_t *op_ctx = &ctx->ctx->is.ctrlflow;

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
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    int32_t field = op->field_index;
    if (field) {
        ecs_iter_t *it = ctx->it;
        const ecs_rule_t *rule = ctx->rule;
        ecs_rule_iter_t *rit = ctx->rit;
        ecs_var_t *vars = rit->vars;

        /* Not terms return no data */
        it->columns[field] = 0;

        /* Ignore variables written by Not operation */
        uint64_t *written = rit->written;
        uint64_t written_cur = written[ctx->op_index] = written[op->prev + 1];

        /* Overwrite id with cleared out variables */
        ecs_id_t id = flecs_rule_op_get_id(ctx->rule, ctx->rit->vars, op, ctx);
        if (id) {
            it->ids[field] = id;
        }

        /* Reset variables */
        if (!flecs_ref_is_written(rule, op, &op->first, EcsRuleFirst, written_cur)){
            vars[op->first.var].entity =  EcsWildcard;
        }
        if (!flecs_ref_is_written(rule, op, &op->second, EcsRuleSecond, written_cur)){
            vars[op->second.var].entity =  EcsWildcard;
        }
    }
    return true; /* Flip result */
}

static
bool flecs_rule_setvars(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    const ecs_rule_t *rule = ctx->rule;
    const ecs_filter_t *filter = &rule->filter;
    int32_t i, *src_vars = rule->src_vars;
    ecs_iter_t *it = ctx->it;
    ecs_var_t *vars = ctx->rit->vars;

    if (redo) {
        return false;
    }

    for (i = 0; i < filter->field_count; i ++) {
        int32_t var_id = src_vars[i];
        if (!var_id) {
            continue;
        }

        it->sources[i] = flecs_rule_var_get_entity(rule, vars, var_id);

        int32_t column = it->columns[i];
        if (column > 0) {
            it->columns[i] = -column;
        }
    }

    return true;
}

static
bool flecs_rule_setthis(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    const ecs_rule_t *rule = ctx->rule;
    ecs_var_t *vars = ctx->rit->vars;
    ecs_var_t *this_var = &vars[op->first.var];

    ecs_record_t *r = flecs_entities_get(rule->world, this_var->entity);
    vars[0].range.table = r->table;
    vars[0].range.offset = ECS_RECORD_TO_ROW(r->row);
    vars[0].range.count = 1;

    return true;
}

/* Check if entity is stored in table */
static
bool flecs_rule_contain(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (redo) {
        return false;
    }

    const ecs_rule_t *rule = ctx->rule;
    ecs_var_t *vars = ctx->rit->vars;
    ecs_var_id_t src_id = op->src.var;
    ecs_var_id_t first_id = op->first.var;

    ecs_table_t *table = flecs_rule_var_get_table(rule, vars, src_id);
    ecs_entity_t e = flecs_rule_var_get_entity(rule, vars, first_id);

    return table == ecs_get_table(rule->world, e);
}

/* Check if first and second id of pair from last operation are the same */
static
bool flecs_rule_pair_eq(
    ecs_rule_op_t *op,
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
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ctx->ctx->is.cond.cond = false;
        return true;
    } else {
        if (!ctx->ctx->is.cond.cond) {
            ctx->jump = op->other;
        }
        return false;
    }
}

static
bool flecs_rule_jmp_set_cond(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ctx->rit->op_ctx[op->other].is.cond.cond = true;
        return true;
    } else {
        return false;
    }
}

static
bool flecs_rule_jmp_not_set(
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    if (!redo) {
        ecs_var_t *vars = ctx->rit->vars;
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
    ecs_rule_op_t *op,
    bool redo,
    ecs_rule_run_ctx_t *ctx)
{
    switch(op->kind) {
    case EcsRuleAnd: return flecs_rule_and(op, redo, ctx);
    case EcsRuleEach: return flecs_rule_each(op, redo, ctx);
    case EcsRuleDown: return flecs_rule_down(op, redo, ctx);
    case EcsRuleStore: return flecs_rule_store(op, redo, ctx);
    case EcsRuleUnion: return flecs_rule_union(op, redo, ctx);
    case EcsRuleEnd: return flecs_rule_end(op, redo, ctx);
    case EcsRuleNot: return flecs_rule_not(op, redo, ctx);
    case EcsRuleSetVars: return flecs_rule_setvars(op, redo, ctx);
    case EcsRuleSetThis: return flecs_rule_setthis(op, redo, ctx);
    case EcsRuleContain: return flecs_rule_contain(op, redo, ctx);
    case EcsRulePairEq: return flecs_rule_pair_eq(op, redo, ctx);
    case EcsRuleJmpCondFalse: return flecs_rule_jmp_if_not(op, redo, ctx);
    case EcsRuleSetCond: return flecs_rule_jmp_set_cond(op, redo, ctx);
    case EcsRuleJmpNotSet: return flecs_rule_jmp_not_set(op, redo, ctx);
    case EcsRuleYield: return false;
    }
    return false;
}

ecs_iter_t ecs_rule_iter(
    const ecs_world_t *world,
    const ecs_rule_t *rule)
{
    ecs_iter_t it = {0};
    ecs_rule_iter_t *rit = &it.priv.iter.rule;
    
    ecs_run_aperiodic(rule->world, EcsAperiodicEmptyTables);

    int32_t i, var_count = rule->var_count, op_count = rule->op_count;
    it.world = (ecs_world_t*)world;
    it.real_world = (ecs_world_t*)ecs_get_world(world);
    it.terms = rule->filter.terms;
    it.next = ecs_rule_next;
    rit->rule = rule;
    if (var_count) {
        rit->vars = ecs_os_calloc_n(ecs_var_t, var_count);
    }
    if (op_count) {
        rit->written = ecs_os_calloc_n(ecs_write_flags_t, op_count);
        rit->op_ctx = ecs_os_calloc_n(ecs_rule_op_ctx_t, op_count);
    }
    for (i = 0; i < var_count; i ++) {
        rit->vars[i].entity = EcsWildcard;
    }

    it.field_count = rule->filter.field_count;

    flecs_iter_init(world, &it, 
        flecs_iter_cache_ids |
        flecs_iter_cache_columns |
        flecs_iter_cache_sources |
        flecs_iter_cache_sizes |
        flecs_iter_cache_ptrs);

    it.variables = rit->vars;
    it.variable_count = rule->var_pub_count;
    it.variable_names = rule->var_names;

    return it;
}

static
void flecs_rule_iter_init(
    const ecs_rule_t *rule,
    ecs_iter_t *it,
    ecs_rule_iter_t *rit)
{
    if (rit->written) {
        ecs_world_t *world = it->real_world;
        ecs_flags64_t it_written = it->constrained_vars;
        rit->written[0] = it_written;
        if (it_written && rule->src_vars) {
            /* If variables were constrained, check if there are any table
             * variables that have a constrained entity variable. */
            ecs_var_t *vars = rit->vars;
            int32_t i, count = rule->filter.field_count;
            for (i = 0; i < count; i ++) {
                ecs_var_id_t var_id = rule->src_vars[i];
                ecs_rule_var_t *var = &rule->vars[var_id];

                if (!(it_written & (1 << var_id)) || 
                    (var->kind == EcsVarTable) || (var->table_id == EcsVarNone)) 
                {
                    continue;
                }

                /* Initialize table variable with constrained entity variable */
                ecs_var_t *tvar = &vars[var->table_id];
                tvar->range = flecs_range_from_entity(
                    world, vars[var_id].entity);
                rit->written[0] |= (1 << var->table_id); /* Mark as written */
            }
        }
    }
}

bool ecs_rule_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_rule_next, ECS_INVALID_PARAMETER, NULL);

    ecs_rule_iter_t *rit = &it->priv.iter.rule;
    const ecs_rule_t *rule = rit->rule;
    bool redo = it->flags & EcsIterIsValid;
    int32_t op_index, prev_index = -1;

    ecs_world_t *world = it->world;
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (!(it->flags & EcsIterIsValid)) {
        flecs_rule_iter_init(rule, it, rit);
    }

    flecs_iter_validate(it);

    do {
        op_index = rit->op;
        ecs_rule_op_t *op = &rule->ops[op_index];
        ecs_rule_run_ctx_t ctx;
        ctx.rule = rule;
        ctx.it = it;
        ctx.rit = rit;
        ctx.op_index = op_index;
        ctx.prev_index = prev_index;
        ctx.jump = -1;
        ctx.stage = stage;
        ctx.ctx = &rit->op_ctx[op_index];

        if (!redo) {
            ctx.ctx->sp = ecs_vector_count(stage->stack);
        } else {
            ecs_vector_set_count(&stage->stack, ecs_entity_t, ctx.ctx->sp);
        }

        bool result = flecs_rule_run(op, redo, &ctx);

        prev_index = op_index;
        if (result) {
            rit->op = op->next;
        } else {
            rit->op = op->prev;
        }

        if (ctx.jump != -1) {
            rit->op = ctx.jump;
        }

        if (rit->op > op_index) {
            rit->written[rit->op] |= rit->written[op_index] | op->written;
        }

        redo = rit->op < prev_index;

        if (op->kind == EcsRuleYield) {
            ecs_table_range_t *range = &rit->vars[0].range;
            flecs_iter_populate_data(world, it, range->table, range->offset,
                range->count, it->ptrs, it->sizes);
            return true;
        }
    } while (rit->op >= 0);

    ecs_iter_fini(it);
    return false;
}

#endif
