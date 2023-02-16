 /**
 * @file addons/rules/compile.c
 * @brief Compile rule program from filter.
 */

#include "rules.h"

#ifdef FLECS_RULES

#define FlecsRuleOrMarker ((int16_t)-2) /* Marks instruction in OR chain */

static bool flecs_rule_op_is_test[] = {
    [EcsRuleAnd] = true,
    [EcsRuleContain] = true,
    [EcsRulePairEq] = true,
    [EcsRuleYield] = false
};

bool flecs_rule_is_written(
    uint32_t var_id,
    uint64_t written)
{
    ecs_assert(var_id < EcsRuleMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    return (written & (1 << var_id)) != 0;
}

static
void flecs_rule_write(
    uint32_t var_id,
    uint64_t *written)
{
    ecs_assert(var_id < EcsRuleMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    *written |= (1 << var_id);
}

static
void flecs_rule_write_ctx(
    uint32_t var_id,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    bool is_written = flecs_rule_is_written(var_id, ctx->written);
    flecs_rule_write(var_id, &ctx->written);
    if (!is_written && cond_write) {
        flecs_rule_write(var_id, &ctx->cond_written);
    }
}

ecs_flags16_t flecs_rule_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind)
{
    return (flags >> kind) & (EcsRuleIsVar | EcsRuleIsEntity);
}

bool flecs_ref_is_written(
    const ecs_rule_t *rule,
    const ecs_rule_op_t *op,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, kind);
    if (flags & EcsRuleIsEntity) {
        if (ref->entity) {
            return true;
        }
    } else if (flags & EcsRuleIsVar) {
        return flecs_rule_is_written(ref->var, written);
    }
    return false;
}

static
ecs_var_id_t flecs_rule_find_var_id(
    const ecs_rule_t *rule,
    const char *name,
    ecs_var_kind_t kind)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (kind == EcsVarTable) {
        if (!ecs_os_strcmp(name, EcsThisName)) {
            return 0;
        }

        if (!flecs_name_index_is_init(&rule->tvar_index)) {
            return EcsVarNone;
        }

        uint64_t index = flecs_name_index_find(
            &rule->tvar_index, name, 0, 0);
        if (index == 0) {
            return EcsVarNone;
        }
        return flecs_uto(uint32_t, index);
    }

    if (kind == EcsVarEntity) {
        if (!flecs_name_index_is_init(&rule->evar_index)) {
            return EcsVarNone;
        }

        uint64_t index = flecs_name_index_find(
            &rule->evar_index, name, 0, 0);
        if (index == 0) {
            return EcsVarNone;
        }
        return flecs_uto(uint32_t, index);
    }

    ecs_assert(kind == EcsVarAny, ECS_INTERNAL_ERROR, NULL);

    /* If searching for any kind of variable, start with most specific */
    uint64_t index = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if (index != EcsVarNone) {
        return index;
    }

    return flecs_rule_find_var_id(rule, name, EcsVarTable);
}

int32_t ecs_rule_find_var(
    const ecs_rule_t *rule,
    const char *name)
{
    int32_t var_id = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if (var_id == -1) {
        if (rule->filter.flags & EcsFilterMatchThis) {
            if (!ecs_os_strcmp(name, "This")) {
                var_id = 0;
            }
        }
    }
    return var_id;
}

static
const char* flecs_term_id_var_name(
    ecs_term_id_t *term_id)
{
    if (!(term_id->flags & EcsIsVariable)) {
        return NULL;
    }

    if (term_id->id == EcsThis) {
        return EcsThisName;
    }

    return term_id->name;
}

static
bool flecs_term_id_is_wildcard(
    ecs_term_id_t *term_id)
{
    if ((term_id->flags & EcsIsVariable) && 
        ((term_id->id == EcsWildcard) || (term_id->id == EcsAny))) 
    {
        return true;
    }
    return false;
}

static
ecs_var_id_t flecs_rule_add_var(
    ecs_rule_t *rule,
    const char *name,
    ecs_vector_t **vars,
    ecs_var_kind_t kind)
{
    ecs_hashmap_t *var_index = NULL;
    ecs_var_id_t var_id = EcsVarNone;
    if (name) {
        if (kind == EcsVarAny) {
            var_id = flecs_rule_find_var_id(rule, name, EcsVarEntity);
            if (var_id != EcsVarNone) {
                return var_id;
            }

            var_id = flecs_rule_find_var_id(rule, name, EcsVarTable);
            if (var_id != EcsVarNone) {
                return var_id;
            }

            kind = EcsVarTable;
        } else {
            var_id = flecs_rule_find_var_id(rule, name, kind);
            if (var_id != EcsVarNone) {
                return var_id;
            }
        }

        if (kind == EcsVarTable) {
            var_index = &rule->tvar_index;
        } else {
            var_index = &rule->evar_index;
        }

        /* If we're creating an entity var, check if it has a table variant */
        if (kind == EcsVarEntity && var_id == EcsVarNone) {
            var_id = flecs_rule_find_var_id(rule, name, EcsVarTable);
        }
    }

    ecs_rule_var_t *var;
    if (vars) {
        var = ecs_vector_add(vars, ecs_rule_var_t);
        var->id = ecs_vector_count(*vars);
    } else {
        ecs_assert(rule->var_count < rule->var_size, ECS_INTERNAL_ERROR, NULL);
        var = &rule->vars[rule->var_count];
        var->id = rule->var_count;
        rule->var_count ++;
    }

    var->kind = kind;
    var->name = name;
    var->label = NULL;
    var->table_id = var_id;

    if (name) {
        flecs_name_index_init_if(var_index, NULL);
        flecs_name_index_ensure(var_index, var->id, name, 0, 0);
    }
    return var->id;
}

static
int32_t flecs_rule_add_var_for_term_id(
    ecs_rule_t *rule,
    ecs_term_id_t *term_id,
    ecs_vector_t **vars,
    ecs_var_kind_t kind)
{
    const char *name = flecs_term_id_var_name(term_id);
    if (!name) {
        return -1;
    }

    return flecs_rule_add_var(rule, name, vars, kind);
}

void flecs_rule_discover_vars(
    ecs_stage_t *stage,
    ecs_rule_t *rule)
{
    ecs_vector_t *vars = stage->variables; /* Buffer to reduce allocs */
    ecs_vector_clear(vars);

    ecs_term_t *terms = rule->filter.terms;
    int32_t i, anonymous_count = 0, count = rule->filter.term_count;
    int32_t anonymous_table_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *first = &term->first;
        ecs_term_id_t *second = &term->second;
        ecs_term_id_t *src = &term->src;

        int32_t first_var_id = flecs_rule_add_var_for_term_id(
            rule, first, &vars, EcsVarEntity);
        if (first_var_id == -1) {
            /* If first is not a variable, check if we need to insert anonymous
             * variable for resolving component inheritance */
            if (first->flags & EcsIsEntity) {
                if (first->flags & EcsDown) {
                    anonymous_count += 2; /* table & entity variable */
                }
            }

            /* If first is a wildcard, insert anonymous variable */
            if (flecs_term_id_is_wildcard(first)) {
                anonymous_count ++;
            }
        }

        if ((src->flags & EcsIsVariable) && (src->id != EcsThis)) {
            const char *var_name = flecs_term_id_var_name(src);
            if (var_name) {
                int32_t var_id = flecs_rule_find_var_id(
                    rule, var_name, EcsVarEntity);
                if (var_id == -1 || var_id == first_var_id) {
                    var_id = flecs_rule_add_var(
                        rule, var_name, &vars, EcsVarEntity);

                    /* Mark variable as one for which we need to create a table
                     * variable. Don't create table variable now, so that we can
                     * store it in the non-public part of the variable array. */
                    ecs_rule_var_t *var = ecs_vector_get(
                        vars, ecs_rule_var_t, var_id - 1);
                    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
                    var->kind = EcsVarAny;

                    anonymous_table_count ++;
                }

                if (var_id != -1) {
                    /* Track of which variable ids are used as field source */
                    if (!rule->src_vars) {
                        rule->src_vars = ecs_os_calloc_n(int32_t, 
                            rule->filter.field_count);
                    }

                    rule->src_vars[term->field_index] = var_id;
                }
            } else {
                if (flecs_term_id_is_wildcard(src)) {
                    anonymous_count ++;
                }
            }
        }

        if (flecs_rule_add_var_for_term_id(
            rule, second, &vars, EcsVarEntity) == -1)
        {
            /* If second is a wildcard, insert anonymous variable */
            if (flecs_term_id_is_wildcard(second)) {
                anonymous_count ++;
            }
        }

        if (src->flags & EcsIsVariable && second->flags & EcsIsVariable) {
            if (term->flags & EcsTermTransitive) {
                /* Anonymous variable to store temporary id for finding 
                 * targets for transitive relationship, see compile_term. */
                anonymous_count ++;
            }
        }
    }

    int32_t var_count = ecs_vector_count(vars);

    /* Add non-This table variables */
    if (anonymous_table_count) {
        anonymous_table_count = 0;
        for (i = 0; i < var_count; i ++) {
            ecs_rule_var_t *var = ecs_vector_get(vars, ecs_rule_var_t, i);
            if (var->kind == EcsVarAny) {
                var->kind = EcsVarEntity;

                int32_t var_id = flecs_rule_add_var(
                    rule, var->name, &vars, EcsVarTable);
                ecs_vector_get(vars, ecs_rule_var_t, i)->table_id = var_id;
                anonymous_table_count ++;
            }
        }

        var_count = ecs_vector_count(vars);
    }

    /* Always include spot for This variable, even if rule doesn't use it */
    var_count ++;

    ecs_rule_var_t *rule_vars = &rule->vars_cache.var;
    if ((var_count + anonymous_count) > 1) {
        rule_vars = ecs_os_malloc(
            (ECS_SIZEOF(ecs_rule_var_t) + ECS_SIZEOF(char*)) * 
                (var_count + anonymous_count));
    }

    rule->vars = rule_vars;
    rule->var_count = var_count;
    rule->var_pub_count = var_count;
    rule->var_size = var_count + anonymous_count;

    char **var_names = ECS_ELEM(rule_vars, ECS_SIZEOF(ecs_rule_var_t), 
        var_count + anonymous_count);
    rule->var_names = (char**)var_names;

    rule_vars[0].kind = EcsVarTable;
    rule_vars[0].name = (char*)EcsThisName;
    rule_vars[0].id = 0;
    rule_vars[0].table_id = EcsVarNone;
    var_names[0] = (char*)rule_vars[0].name;
    rule_vars ++;
    var_names ++;
    var_count --;

    if (var_count) {
        ecs_rule_var_t *user_vars = ecs_vector_first(vars, ecs_rule_var_t);
        ecs_os_memcpy_n(rule_vars, user_vars, ecs_rule_var_t, var_count);
        for (i = 0; i < var_count; i ++) {
            var_names[i] = (char*)rule_vars[i].name;
        }
    }

    /* Hide anonymous table variables from application */
    rule->var_pub_count -= anonymous_table_count;

    stage->variables = vars;
}

static
ecs_var_id_t flecs_rule_most_specific_var(
    ecs_rule_t *rule,
    const char *name,
    ecs_var_kind_t kind,
    ecs_rule_compile_ctx_t *ctx)
{
    if (kind == EcsVarTable || kind == EcsVarEntity) {
        return flecs_rule_find_var_id(rule, name, kind);
    }

    ecs_var_id_t tvar = flecs_rule_find_var_id(rule, name, EcsVarTable);
    if ((tvar != EcsVarNone) && !flecs_rule_is_written(tvar, ctx->written)) {
        /* If variable of any kind is requested and variable hasn't been written
         * yet, write to table variable */
        return tvar;
    }

    ecs_var_id_t evar = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if ((evar != EcsVarNone) && flecs_rule_is_written(evar, ctx->written)) {
        /* If entity variable is available and written to, it contains the most
         * specific result and should be used. */
        return evar;
    }

    /* If table var is written, and entity var doesn't exist or is not written,
     * return table var */
    ecs_assert(tvar != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
    return tvar;
}

static
int32_t flecs_rule_op_insert(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t *elem = ecs_vector_add(&ctx->ops, ecs_rule_op_t);
    int32_t count = ecs_vector_count(ctx->ops);
    *elem = *op;
    if (count > 1) {
        if (ctx->lbl_union == -1) {
            /* Variables written by previous instruction can't be written by
             * this instruction, except when this is a union. */
            elem->written &= ~elem[-1].written;
        }
    }

    if (ctx->lbl_union != -1) {
        elem->prev = ctx->lbl_union;
    } else if (ctx->lbl_prev != -1) {
        elem->prev = ctx->lbl_prev;
        ctx->lbl_prev = -1;
    } else {
        elem->prev = count - 2;
    }

    elem->next = count;
    return count - 1;
}

static
int32_t flecs_rule_not_insert(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t *op_last = ecs_vector_last(ctx->ops, ecs_rule_op_t);
    if (op_last && op_last->kind == EcsRuleNot) {
        /* There can be multiple reasons for inserting a Not operation, ensure
         * that only one is created. */
        ecs_assert(op_last->field_index == op->field_index, 
            ECS_INTERNAL_ERROR, NULL);
        return ecs_vector_count(ctx->ops) - 1;
    }

    ecs_rule_op_t not_op = {0};
    not_op.kind = EcsRuleNot;
    not_op.field_index = op->field_index;
    not_op.first = op->first;
    not_op.second = op->second;
    not_op.flags = op->flags;
    return flecs_rule_op_insert(&not_op, ctx);
}

static
void flecs_rule_begin_not(
    ecs_rule_compile_ctx_t *ctx)
{
    ctx->lbl_not = ecs_vector_count(ctx->ops);
}

static
void flecs_rule_end_not(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx)
{
    flecs_rule_not_insert(op, ctx);

    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t i, count = ecs_vector_count(ctx->ops);
    for (i = ctx->lbl_not; i < count; i ++) {
        ecs_rule_op_t *op = &ops[i];
        if (flecs_rule_op_is_test[op->kind]) {
            op->prev = count - 1;
            op->next = ctx->lbl_not - 1;
        }
    }

    /* After a match was found, return to op before Not operation */
    ecs_rule_op_t *not_ptr = ecs_vector_last(ctx->ops, ecs_rule_op_t);
    not_ptr->prev = ctx->lbl_not - 1;

    ctx->lbl_not = -1;
}

static
void flecs_rule_begin_option(
    ecs_rule_compile_ctx_t *ctx)
{
    ctx->lbl_option = ecs_vector_count(ctx->ops);

    {
        ecs_rule_op_t new_op = {0};
        new_op.kind = EcsRuleJmpCondFalse;
        flecs_rule_op_insert(&new_op, ctx);
    }
}

static
void flecs_rule_end_option(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx)
{
    flecs_rule_not_insert(op, ctx);
    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t count = ecs_vector_count(ctx->ops);

    ops[ctx->lbl_option].other = count - 1;
    ops[count - 2].next = count;

    ecs_rule_op_t new_op = {0};
    new_op.kind = EcsRuleSetCond;
    new_op.other = ctx->lbl_option;
    flecs_rule_op_insert(&new_op, ctx);

    ctx->lbl_option = -1;
}

static
void flecs_rule_begin_cond_eval(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx,
    ecs_flags8_t cond_write_state)
{
    ecs_var_id_t first_var = 0, second_var = 0, src_var = 0;
    ecs_flags8_t cond_mask = 0;

    if (flecs_rule_ref_flags(op->flags, EcsRuleFirst) == EcsRuleIsVar) {
        first_var = op->first.var;
        cond_mask |= (1 << first_var);
    }
    if (flecs_rule_ref_flags(op->flags, EcsRuleSecond) == EcsRuleIsVar) {
        second_var = op->second.var;
        cond_mask |= (1 << second_var);
    }
    if (flecs_rule_ref_flags(op->flags, EcsRuleSrc) == EcsRuleIsVar) {
        src_var = op->src.var;
        cond_mask |= (1 << src_var);
    }

    /* If this term uses conditionally set variables, insert instruction that
     * jumps over the term if the variables weren't set yet. */
    if (cond_mask & cond_write_state) {
        ctx->lbl_cond_eval = ecs_vector_count(ctx->ops);

        ecs_rule_op_t jmp_op = {0};
        jmp_op.kind = EcsRuleJmpNotSet;

        if (cond_write_state & (1 << first_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleFirst);
            jmp_op.first.var = first_var;
        }
        if (cond_write_state & (1 << second_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleSecond);
            jmp_op.second.var = second_var;
        }
        if (cond_write_state & (1 << src_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleSrc);
            jmp_op.src.var = src_var;
        }

        flecs_rule_op_insert(&jmp_op, ctx);
    } else {
        ctx->lbl_cond_eval = -1;
    }
}

static
void flecs_rule_end_cond_eval(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx,
    ecs_flags8_t write_state)
{
    if (ctx->lbl_cond_eval == -1) {
        return;
    }

    flecs_rule_not_insert(op, ctx);

    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t count = ecs_vector_count(ctx->ops);
    ops[ctx->lbl_cond_eval].other = count - 1;
    ops[count - 2].next = count;
}

static
void flecs_rule_next_or(
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t count = ecs_vector_count(ctx->ops);
    ops[count - 1].next = FlecsRuleOrMarker;
}

static
void flecs_rule_begin_or(
    ecs_rule_compile_ctx_t *ctx)
{
    ctx->lbl_or = ecs_vector_count(ctx->ops) - 1;
    flecs_rule_next_or(ctx);
}

static
void flecs_rule_end_or(
    ecs_rule_compile_ctx_t *ctx)
{
    flecs_rule_next_or(ctx);

    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t i = ctx->lbl_or, count = ecs_vector_count(ctx->ops);
    int32_t prev_or = -2;
    for (i = 0; i < count; i ++) {
        if (ops[i].next == FlecsRuleOrMarker) {
            if (prev_or != -2) {
                ops[prev_or].prev = i;
            }
            ops[i].next = count;
            prev_or = i;
        }
    }

    ops[count - 1].prev = ctx->lbl_or - 1;

    /* Set prev of next instruction to before the start of the OR chain */
    ctx->lbl_prev = ctx->lbl_or - 1;
    ctx->lbl_or = -1;
}

static
void flecs_rule_begin_union(
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t op = {0};
    op.kind = EcsRuleUnion;
    ctx->lbl_union = flecs_rule_op_insert(&op, ctx);
}

static
void flecs_rule_end_union(
    ecs_rule_compile_ctx_t *ctx)
{
    flecs_rule_next_or(ctx);

    ecs_rule_op_t op = {0};
    op.kind = EcsRuleEnd;
    ctx->lbl_union = -1;
    int32_t next = flecs_rule_op_insert(&op, ctx);
    
    ecs_rule_op_t *ops = ecs_vector_first(ctx->ops, ecs_rule_op_t);
    int32_t i = ecs_vector_count(ctx->ops) - 2;
    for (; i >= 0 && (ops[i].kind != EcsRuleUnion); i --) {
        if (ops[i].next == FlecsRuleOrMarker) {
            ops[i].next = next;
        }
    }

    ops[next].prev = i;
    ops[i].next = next;
}

static
void flecs_rule_insert_store(
    ecs_entity_t entity,
    ecs_var_id_t var,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t op = {0};
    op.kind = EcsRuleStore;
    op.src.var = var;
    op.first.entity = entity;
    op.flags = (EcsRuleIsVar << EcsRuleSrc) |
               (EcsRuleIsEntity << EcsRuleFirst);
    flecs_rule_write(var, &op.written);
    flecs_rule_op_insert(&op, ctx);
}

static
void flecs_rule_insert_each(
    ecs_var_id_t tvar,
    ecs_var_id_t evar,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    ecs_rule_op_t each = {0};
    each.kind = EcsRuleEach;
    each.src.var = evar;
    each.first.var = tvar;
    each.flags = (EcsRuleIsVar << EcsRuleSrc) | 
                 (EcsRuleIsVar << EcsRuleFirst);
    flecs_rule_write_ctx(evar, ctx, cond_write);
    flecs_rule_write(evar, &each.written);
    flecs_rule_op_insert(&each, ctx);
}

static
void flecs_rule_insert_unconstrained_transitive(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    /* Create anonymous variable to store the target ids. This will return the
     * list of targets without constraining the variable of the term, which
     * needs to stay variable to find all transitive relationships for a src. */
    ecs_var_id_t tgt = flecs_rule_add_var(rule, NULL, NULL, EcsVarEntity);
    rule->vars[tgt].label = rule->vars[op->second.var].name;

    /* First, find ids to start traversal from. This fixes op.second. */
    ecs_rule_op_t find_ids = {0};
    find_ids.kind = EcsRuleFindIds;
    find_ids.field_index = op->field_index;
    find_ids.first = op->first;
    find_ids.second = op->second;
    find_ids.flags = op->flags;
    find_ids.flags &= ~(EcsRuleIsVar << EcsRuleSrc);
    find_ids.flags &= ~(EcsRuleIsEntity << EcsRuleSrc);
    find_ids.second.var = tgt;
    flecs_rule_write_ctx(tgt, ctx, cond_write);
    flecs_rule_write(tgt, &find_ids.written);
    flecs_rule_op_insert(&find_ids, ctx);

    /* Next, iterate all tables for the ids. This fixes op.src */
    ecs_rule_op_t and_op = {0};
    and_op.kind = EcsRuleAnd;
    and_op.field_index = op->field_index;
    and_op.first = op->first;
    and_op.second = op->second;
    and_op.src = op->src;
    and_op.flags = op->flags | EcsRuleIsSelf;
    and_op.second.var = tgt;
    flecs_rule_write_ctx(and_op.src.var, ctx, cond_write);
    flecs_rule_write(and_op.src.var, &and_op.written);
    flecs_rule_op_insert(&and_op, ctx);
}

static
void flecs_rule_compile_term_id(
    ecs_world_t *world,
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_term_id_t *term_id,
    ecs_rule_ref_t *ref,
    ecs_flags32_t ref_kind,
    ecs_var_kind_t kind,
    ecs_rule_compile_ctx_t *ctx)
{
    if (!ecs_term_id_is_set(term_id)) {
        return;
    }

    if (term_id->flags & EcsIsVariable) {
        op->flags |= (EcsRuleIsVar << ref_kind);
        const char *name = flecs_term_id_var_name(term_id);
        if (name) {
            ref->var = flecs_rule_most_specific_var(rule, name, kind, ctx);
        } else {
            bool is_wildcard = flecs_term_id_is_wildcard(term_id);
            if (is_wildcard && (kind == EcsVarAny)) {
                ref->var = flecs_rule_add_var(rule, NULL, NULL, EcsVarTable);
            } else {
                ref->var = flecs_rule_add_var(rule, NULL, NULL, EcsVarEntity);
            }
            if (is_wildcard) {
                rule->vars[ref->var].label = ecs_get_name(world, term_id->id);
            }
        }
        ecs_assert(ref->var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
    }

    if (term_id->flags & EcsIsEntity) {
        op->flags |= (EcsRuleIsEntity << ref_kind);
        ref->entity = term_id->id;
    }
}

static
void flecs_rule_compile_ensure_vars(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_rule_ref_t *ref,
    ecs_flags32_t ref_kind,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, ref_kind);
    if (flags & EcsRuleIsVar) {
        ecs_var_id_t var_id = ref->var;
        ecs_rule_var_t *var = &rule->vars[var_id];
        if (var->kind == EcsVarEntity && !flecs_rule_is_written(var_id, ctx->written)) {
            /* If entity variable is not yet written but a table variant exists
             * that has been written, insert each operation to translate from
             * entity variable to table */
            ecs_var_id_t tvar = var->table_id;
            if ((tvar != EcsVarNone) && flecs_rule_is_written(tvar, ctx->written)) {
                flecs_rule_insert_each(tvar, var_id, ctx, cond_write);
            }
        }

        /* After evaluating a term, a used variable is always written */
        flecs_rule_write(var_id, &op->written);
        flecs_rule_write_ctx(var_id, ctx, cond_write);
    }
}

static
void flecs_rule_insert_contains(
    ecs_rule_t *rule,
    ecs_var_id_t src_var,
    ecs_var_id_t other_var,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t contains = {0};
    if ((src_var != other_var) && (src_var == rule->vars[other_var].table_id)) {
        contains.kind = EcsRuleContain;
        contains.src.var = src_var;
        contains.first.var = other_var;
        contains.flags |=(EcsRuleIsVar << EcsRuleSrc) | 
                            (EcsRuleIsVar << EcsRuleFirst);
        flecs_rule_op_insert(&contains, ctx);
    }
}

static
void flecs_rule_insert_pair_eq(
    ecs_rule_t *rule,
    int32_t field_index,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t contains = {0};
    contains.kind = EcsRulePairEq;
    contains.field_index = field_index;
    flecs_rule_op_insert(&contains, ctx);
}

static
void flecs_rule_compile_term(
    ecs_world_t *world,
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t op = {0};
    op.kind = EcsRuleAnd;
    op.field_index = term->field_index;

    if (term->flags & EcsTermTransitive) {
        ecs_assert(ecs_term_id_is_set(&term->second), ECS_INTERNAL_ERROR, NULL);
        op.kind = EcsRuleTrav;
    }

    bool first_term = term == rule->filter.terms;
    bool first_is_var = term->first.flags & EcsIsVariable;
    bool second_is_var = term->second.flags & EcsIsVariable;
    bool src_is_var = term->src.flags & EcsIsVariable;
    bool cond_write = term->oper == EcsOptional;

    /* Save write state at start of term so we can use it to reliably track
     * variables got written by this term. */
    ecs_flags8_t write_state = ctx->written;
    ecs_flags8_t cond_write_state = ctx->cond_written;

    /* Resolve component inheritance if necessary */
    ecs_var_id_t first_var = 0, second_var = 0, src_var = 0;

    /* Resolve variables and entities for operation arguments */
    flecs_rule_compile_term_id(world, rule, &op, &term->first, 
        &op.first, EcsRuleFirst, EcsVarEntity, ctx);
    flecs_rule_compile_term_id(world, rule, &op, &term->second, 
        &op.second, EcsRuleSecond, EcsVarEntity, ctx);

    if (first_is_var) first_var = op.first.var;
    if (second_is_var) second_var = op.second.var;
    if (src_is_var) src_var = op.src.var;
    bool first_written = flecs_rule_is_written(first_var, ctx->written);
    bool second_written = flecs_rule_is_written(src_var, ctx->written);

    /* Insert each instructions for table -> entity variable translation */
    flecs_rule_compile_ensure_vars(rule, &op, &op.first, EcsRuleFirst, ctx, cond_write);
    flecs_rule_compile_ensure_vars(rule, &op, &op.second, EcsRuleSecond, ctx, cond_write);

    /* Do src last, in case it uses the same variable as first/second */
    flecs_rule_compile_term_id(world, rule, &op, &term->src, 
        &op.src, EcsRuleSrc, EcsVarAny, ctx);
    bool src_written = flecs_rule_is_written(src_var, ctx->written);
    flecs_rule_compile_ensure_vars(rule, &op, &op.src, EcsRuleSrc, ctx, cond_write);

    /* If this is a transitive term and both the target and source are unknown,
     * find the targets for the relationship first. This clusters together 
     * tables for the same target, which allows for more efficient usage of the
     * traversal caches. */
    if (term->flags & EcsTermTransitive && src_is_var && second_is_var) {
        if (!src_written && !second_written) {
            flecs_rule_insert_unconstrained_transitive(
                rule, &op, ctx, cond_write);
        }
    }

    if (term->oper == EcsNot) {
        flecs_rule_begin_not(ctx);
    } else if (term->oper == EcsOptional) {
        flecs_rule_begin_option(ctx);
    } else if (term->oper == EcsOr) {
        if (first_term || term[-1].oper != EcsOr) {
            if (!src_written) {
                flecs_rule_begin_union(ctx);
            }
        }
    }

    /* Check if this term has variables that have been conditionally written,
     * like variables written by an optional term. */
    if (ctx->cond_written) {
        flecs_rule_begin_cond_eval(&op, ctx, cond_write_state);
    }

    op.match_flags = term->flags;

    if (first_var) {
        op.first.var = first_var;
        op.flags &= ~(EcsRuleIsEntity << EcsRuleFirst);
        op.flags |= (EcsRuleIsVar << EcsRuleFirst);
    }

    if (term->src.flags & EcsSelf) {
        op.flags |= EcsRuleIsSelf;
    }

    if (term->src.flags & EcsUp) {
        op.trav = term->src.trav;
    }

    flecs_rule_op_insert(&op, ctx);

    /* Handle self-references between src and first/second variables */
    if (src_var) {
        if (first_var) {
            flecs_rule_insert_contains(rule, src_var, first_var, ctx);
        }
        if (second_var && first_var != second_var) {
            flecs_rule_insert_contains(rule, src_var, second_var, ctx);
        }
    }

    /* Handle self references between first and second variables */
    if (first_var && !first_written && (first_var == second_var)) {
        flecs_rule_insert_pair_eq(rule, term->field_index, ctx);
    }

    if (ctx->cond_written && (first_is_var || second_is_var || src_is_var)) {
        flecs_rule_end_cond_eval(&op, ctx, write_state);
    }

    if (term->oper == EcsNot) {
        flecs_rule_end_not(&op, ctx);
    } else if (term->oper == EcsOptional) {
        flecs_rule_end_option(&op, ctx);
    } else if (term->oper == EcsOr) {
        if (ctx->lbl_union != -1) {
            flecs_rule_next_or(ctx);
        } else {
            if (first_term || term[-1].oper != EcsOr) {
                if (ctx->lbl_union == -1) {
                    flecs_rule_begin_or(ctx);
                }
            } else if (term->oper == EcsOr) {
                flecs_rule_next_or(ctx);
            }
        }
    } else if (term->oper == EcsAnd) {
        if (!first_term && term[-1].oper == EcsOr) {
            if (ctx->lbl_union != -1) {
                flecs_rule_end_union(ctx);
            } else {
                flecs_rule_end_or(ctx);
            }
        }
    }
}

void flecs_rule_compile(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_rule_t *rule)
{
    ecs_rule_compile_ctx_t ctx = {0};
    ctx.ops = stage->operations;
    ctx.lbl_union = -1;
    ctx.lbl_prev = -1;
    ecs_vector_clear(ctx.ops);

    /* Compile query terms to instructions */
    ecs_filter_t *filter = &rule->filter;
    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;
    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        flecs_rule_compile_term(world, rule, term, &ctx);
    }

    /* If This variable has been written as entity, insert an operation to 
     * assign it to it.entities for consistency. */
    ecs_var_id_t this_id = flecs_rule_find_var_id(rule, "This", EcsVarEntity);
    if (this_id != EcsVarNone) {
        ecs_rule_op_t set_this = {0};
        set_this.kind = EcsRuleSetThis;
        set_this.flags |= (EcsRuleIsVar << EcsRuleFirst);
        set_this.first.var = this_id;
        flecs_rule_op_insert(&set_this, &ctx);
    }

    /* Make sure non-This variables are written as entities */
    if (rule->vars) {
        for (i = 0; i < rule->var_count; i ++) {
            ecs_rule_var_t *var = &rule->vars[i];
            if (var->id && var->kind == EcsVarTable && var->name) {
                ecs_var_id_t var_id = flecs_rule_find_var_id(rule, var->name,
                    EcsVarEntity);
                if (!flecs_rule_is_written(var_id, ctx.written)) {
                    flecs_rule_insert_each(var->id, var_id, &ctx, false);
                }
            }
        }
    }

    /* If rule contains non-This variables as term source, build lookup array */
    if (rule->src_vars) {
        ecs_rule_op_t set_vars = {0};
        set_vars.kind = EcsRuleSetVars;
        flecs_rule_op_insert(&set_vars, &ctx);

        for (i = 0; i < filter->field_count; i ++) {
            int32_t var_id = rule->src_vars[i];
            if (!var_id) {
                continue;
            }

            if (rule->vars[var_id].kind == EcsVarTable) {
                var_id = flecs_rule_find_var_id(rule, rule->vars[var_id].name,
                    EcsVarEntity);

                /* Variables used as source that aren't This must be entities */
                ecs_assert(var_id != -1, ECS_INTERNAL_ERROR, NULL);
            }

            rule->src_vars[i] = var_id;
        }
    }

    /* Insert yield. If program reaches this operation, a result was found */
    ecs_rule_op_t yield = {0};
    yield.kind = EcsRuleYield;
    flecs_rule_op_insert(&yield, &ctx);

    int32_t op_count = ecs_vector_count(ctx.ops);
    if (op_count) {
        rule->op_count = op_count;
        rule->ops = ecs_os_malloc_n(ecs_rule_op_t, op_count);
        ecs_rule_op_t *rule_ops = ecs_vector_first(ctx.ops, ecs_rule_op_t);
        ecs_os_memcpy_n(rule->ops, rule_ops, ecs_rule_op_t, op_count);
    }

    stage->operations = ctx.ops;
}

#endif
