/**
 * @file query/compiler/compiler_term.c
 * @brief Compile query term.
 */

#include "../../private_api.h"

#define FlecsRuleOrMarker ((int16_t)-2) /* Marks instruction in OR chain */

ecs_var_id_t flecs_query_find_var_id(
    const ecs_query_impl_t *query,
    const char *name,
    ecs_var_kind_t kind)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);

    if (kind == EcsVarTable) {
        if (!ecs_os_strcmp(name, EcsThisName)) {
            if (query->pub.flags & EcsQueryHasTableThisVar) {
                return 0;
            } else {
                return EcsVarNone;
            }
        }

        if (!flecs_name_index_is_init(&query->tvar_index)) {
            return EcsVarNone;
        }

        uint64_t index = flecs_name_index_find(
            &query->tvar_index, name, 0, 0);
        if (index == 0) {
            return EcsVarNone;
        }
        return flecs_utovar(index);
    }

    if (kind == EcsVarEntity) {
        if (!flecs_name_index_is_init(&query->evar_index)) {
            return EcsVarNone;
        }

        uint64_t index = flecs_name_index_find(
            &query->evar_index, name, 0, 0);
        if (index == 0) {
            return EcsVarNone;
        }
        return flecs_utovar(index);
    }

    ecs_assert(kind == EcsVarAny, ECS_INTERNAL_ERROR, NULL);

    /* If searching for any kind of variable, start with most specific */
    ecs_var_id_t index = flecs_query_find_var_id(query, name, EcsVarEntity);
    if (index != EcsVarNone) {
        return index;
    }

    return flecs_query_find_var_id(query, name, EcsVarTable);
}

static
ecs_var_id_t flecs_query_most_specific_var(
    ecs_query_impl_t *query,
    const char *name,
    ecs_var_kind_t kind,
    ecs_query_compile_ctx_t *ctx)
{
    if (kind == EcsVarTable || kind == EcsVarEntity) {
        return flecs_query_find_var_id(query, name, kind);
    }

    ecs_var_id_t evar = flecs_query_find_var_id(query, name, EcsVarEntity);
    if ((evar != EcsVarNone) && flecs_query_is_written(evar, ctx->written)) {
        /* If entity variable is available and written to, it contains the most
         * specific result and should be used. */
        return evar;
    }

    ecs_var_id_t tvar = flecs_query_find_var_id(query, name, EcsVarTable);
    if ((tvar != EcsVarNone) && !flecs_query_is_written(tvar, ctx->written)) {
        /* If variable of any kind is requested and variable hasn't been written
         * yet, write to table variable */
        return tvar;
    }

    /* If table var is written, and entity var doesn't exist or is not written,
     * return table var */
    if (tvar != EcsVarNone) {
        return tvar;
    } else {
        return evar;
    }
}

ecs_query_lbl_t flecs_query_op_insert(
    ecs_query_op_t *op,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t *elem = ecs_vec_append_t(NULL, ctx->ops, ecs_query_op_t);
    int32_t count = ecs_vec_count(ctx->ops);
    *elem = *op;
    if (count > 1) {
        if (ctx->cur->lbl_begin == -1) {
            /* Variables written by previous instruction can't be written by
             * this instruction, except when this is part of an OR chain. */
            elem->written &= ~elem[-1].written;
        }
    }

    elem->next = flecs_itolbl(count);
    elem->prev = flecs_itolbl(count - 2);
    return flecs_itolbl(count - 1);
}

ecs_query_op_t* flecs_query_begin_block(
    ecs_query_op_kind_t kind,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t op = {0};
    op.kind = flecs_ito(uint8_t, kind);
    ctx->cur->lbl_begin = flecs_query_op_insert(&op, ctx);
    return ecs_vec_get_t(ctx->ops, ecs_query_op_t, ctx->cur->lbl_begin);
}

void flecs_query_end_block(
    ecs_query_compile_ctx_t *ctx,
    bool reset)
{
    ecs_query_op_t new_op = {0};
    new_op.kind = EcsQueryEnd;
    ecs_query_lbl_t end = flecs_query_op_insert(&new_op, ctx);
    
    ecs_query_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_query_op_t);
    ops[ctx->cur->lbl_begin].next = end;

    ecs_query_op_t *end_op = &ops[end];
    if (reset && ctx->cur->lbl_query != -1) {
        ecs_query_op_t *query_op = &ops[ctx->cur->lbl_query];
        end_op->prev = ctx->cur->lbl_begin;
        end_op->src = query_op->src;
        end_op->first = query_op->first;
        end_op->second = query_op->second;
        end_op->flags = query_op->flags;
        end_op->field_index = query_op->field_index;
    } else {
        end_op->prev = ctx->cur->lbl_begin;
        end_op->field_index = -1;
    }

    ctx->cur->lbl_begin = -1;
}

static
void flecs_query_begin_block_cond_eval(
    ecs_query_op_t *op,
    ecs_query_compile_ctx_t *ctx,
    ecs_write_flags_t cond_write_state)
{
    ecs_var_id_t first_var = EcsVarNone, second_var = EcsVarNone, src_var = EcsVarNone;
    ecs_write_flags_t cond_mask = 0;

    if (flecs_query_ref_flags(op->flags, EcsQueryFirst) == EcsQueryIsVar) {
        first_var = op->first.var;
        ecs_assert(first_var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        cond_mask |= (1ull << first_var);
    }
    if (flecs_query_ref_flags(op->flags, EcsQuerySecond) == EcsQueryIsVar) {
        second_var = op->second.var;
        ecs_assert(second_var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        cond_mask |= (1ull << second_var);
    }
    if (flecs_query_ref_flags(op->flags, EcsQuerySrc) == EcsQueryIsVar) {
        src_var = op->src.var;
        ecs_assert(src_var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        cond_mask |= (1ull << src_var);
    }

    /* Variables set in an OR chain are marked as conditional writes. However, 
     * writes from previous terms in the current OR chain shouldn't be treated
     * as variables that are conditionally set, so instead use the write mask 
     * from before the chain started. */
    if (ctx->ctrlflow->in_or) {
        cond_write_state = ctx->ctrlflow->cond_written_or;
    }

    /* If this term uses conditionally set variables, insert instruction that
     * jumps over the term if the variables weren't set yet. */
    if (cond_mask & cond_write_state) {
        ctx->cur->lbl_cond_eval = flecs_itolbl(ecs_vec_count(ctx->ops));

        ecs_query_op_t jmp_op = {0};
        jmp_op.kind = EcsQueryIfVar;

        if ((first_var != EcsVarNone) && cond_write_state & (1ull << first_var)) {
            jmp_op.flags |= (EcsQueryIsVar << EcsQueryFirst);
            jmp_op.first.var = first_var;
        }
        if ((second_var != EcsVarNone) && cond_write_state & (1ull << second_var)) {
            jmp_op.flags |= (EcsQueryIsVar << EcsQuerySecond);
            jmp_op.second.var = second_var;
        }
        if ((src_var != EcsVarNone) && cond_write_state & (1ull << src_var)) {
            jmp_op.flags |= (EcsQueryIsVar << EcsQuerySrc);
            jmp_op.src.var = src_var;
        }

        flecs_query_op_insert(&jmp_op, ctx);
    } else {
        ctx->cur->lbl_cond_eval = -1;
    }
}

static
void flecs_query_end_block_cond_eval(
    ecs_query_compile_ctx_t *ctx)
{
    if (ctx->cur->lbl_cond_eval == -1) {
        return;
    }

    ecs_assert(ctx->cur->lbl_query >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_query_op_t end_op = {0};
    end_op.kind = EcsQueryEnd;
    ecs_query_lbl_t end = flecs_query_op_insert(&end_op, ctx);

    ecs_query_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_query_op_t);
    ops[ctx->cur->lbl_cond_eval].next = end;

    ecs_query_op_t *end_op_ptr = &ops[end];
    ecs_query_op_t *query_op = &ops[ctx->cur->lbl_query];
    end_op_ptr->prev = ctx->cur->lbl_cond_eval;
    end_op_ptr->src = query_op->src;
    end_op_ptr->first = query_op->first;
    end_op_ptr->second = query_op->second;
    end_op_ptr->flags = query_op->flags;
    end_op_ptr->field_index = query_op->field_index;
}

static
void flecs_query_begin_block_or(
    ecs_query_op_t *op,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t *or_op = flecs_query_begin_block(EcsQueryNot, ctx);
    or_op->kind = EcsQueryOr;

    /* Set the source of the evaluate terms as source of the Or instruction. 
     * This lets the engine determine whether the variable has already been
     * written. When the source is not yet written, an OR operation needs to
     * take the union of all the terms in the OR chain. When the variable is
     * known, it will return after the first matching term.
     * 
     * In case a term in the OR expression is an equality predicate which 
     * compares the left hand side with a variable, the variable acts as an 
     * alias, so we can always assume that it's written. */
    bool add_src = true;
    if (ECS_TERM_REF_ID(&term->first) == EcsPredEq && term->second.id & EcsIsVariable) {
        if (!(flecs_query_is_written(op->src.var, ctx->written))) {
            add_src = false;
        }
    }

    if (op->flags & (EcsQueryIsVar << EcsQuerySrc)) {
        if (add_src) {
            or_op->flags = (EcsQueryIsVar << EcsQuerySrc);
            or_op->src = op->src;
            ctx->cur->src_or = op->src;
        }

        ctx->cur->src_written_or = flecs_query_is_written(
            op->src.var, ctx->written);
    }
}

static
void flecs_query_end_block_or(
    ecs_query_impl_t *impl,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t op = {0};
    op.kind = EcsQueryEnd;
    ecs_query_lbl_t end = flecs_query_op_insert(&op, ctx);

    ecs_query_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_query_op_t);
    int32_t i, prev_or = ctx->cur->lbl_begin + 1;
    for (i = ctx->cur->lbl_begin + 1; i < end; i ++) {
        if (ops[i].next == FlecsRuleOrMarker) {
            if (i == (end - 1)) {
                ops[prev_or].prev = ctx->cur->lbl_begin;
            } else {
                ops[prev_or].prev = flecs_itolbl(i + 1);
            }

            ops[i].next = flecs_itolbl(end);

            prev_or = i + 1;
        }
    }

    ecs_query_op_t *first = &ops[ctx->cur->lbl_begin];
    bool src_is_var = first->flags & (EcsQueryIsVar << EcsQuerySrc);
    first->next = flecs_itolbl(end);
    ops[end].prev = ctx->cur->lbl_begin;
    ops[end - 1].prev = ctx->cur->lbl_begin;

    ctx->ctrlflow->in_or = false;
    ctx->cur->lbl_begin = -1;
    if (src_is_var) {
        ecs_var_id_t src_var = first->src.var;
        ctx->written |= (1llu << src_var);

        /* If src is a table variable, it is possible that this was resolved to
         * an entity variable in all of the OR terms. If this is the case, mark
         * entity variable as written as well. */
        ecs_query_var_t *var = &impl->vars[src_var];
        if (var->kind == EcsVarTable) {
            const char *name = var->name;
            if (!name) {
                name = "this";
            }

            ecs_var_id_t evar = flecs_query_find_var_id(
                impl, name, EcsVarEntity);
            if (evar != EcsVarNone && (ctx->cond_written & (1llu << evar))) {
                ctx->written |= (1llu << evar);
                ctx->cond_written &= ~(1llu << evar);
            }
        }
    }
    ctx->written |= ctx->cond_written;

    /* Scan which variables were conditionally written in the OR chain and 
     * reset instructions after the OR chain. If a variable is set in part one
     * of a chain but not part two, there would be nothing writing to the
     * variable in part two, leaving it to the previous value. To address this
     * a reset is inserted that resets the variable value on redo. */
    for (i = 1; i < (8 * ECS_SIZEOF(ecs_write_flags_t)); i ++) {
        ecs_write_flags_t prev = 1 & (ctx->ctrlflow->cond_written_or >> i);
        ecs_write_flags_t cur = 1 & (ctx->cond_written >> i);

        /* Skip variable if it's the source for the OR chain */
        if (src_is_var && (i == first->src.var)) {
            continue;
        }

        if (!prev && cur) {
            ecs_query_op_t reset_op = {0};
            reset_op.kind = EcsQueryReset;
            reset_op.flags |= (EcsQueryIsVar << EcsQuerySrc);
            reset_op.src.var = flecs_itovar(i);
            flecs_query_op_insert(&reset_op, ctx);
        }
    }
}

void flecs_query_insert_each(
    ecs_var_id_t tvar,
    ecs_var_id_t evar,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write)
{
    ecs_query_op_t each = {0};
    each.kind = EcsQueryEach;
    each.src.var = evar;
    each.first.var = tvar;
    each.flags = (EcsQueryIsVar << EcsQuerySrc) | 
                 (EcsQueryIsVar << EcsQueryFirst);
    flecs_query_write_ctx(evar, ctx, cond_write);
    flecs_query_write(evar, &each.written);
    flecs_query_op_insert(&each, ctx);
}

static
void flecs_query_insert_lookup(
    ecs_var_id_t base_var,
    ecs_var_id_t evar,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write)
{
    ecs_query_op_t lookup = {0};
    lookup.kind = EcsQueryLookup;
    lookup.src.var = evar;
    lookup.first.var = base_var;
    lookup.flags = (EcsQueryIsVar << EcsQuerySrc) | 
                 (EcsQueryIsVar << EcsQueryFirst);
    flecs_query_write_ctx(evar, ctx, cond_write);
    flecs_query_write(evar, &lookup.written);
    flecs_query_op_insert(&lookup, ctx);
}

static
void flecs_query_insert_unconstrained_transitive(
    ecs_query_impl_t *query,
    ecs_query_op_t *op,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write)
{
    /* Create anonymous variable to store the target ids. This will return the
     * list of targets without constraining the variable of the term, which
     * needs to stay variable to find all transitive relationships for a src. */
    ecs_var_id_t tgt = flecs_query_add_var(query, NULL, NULL, EcsVarEntity);
    flecs_set_var_label(&query->vars[tgt], query->vars[op->second.var].name);

    /* First, find ids to start traversal from. This fixes op.second. */
    ecs_query_op_t find_ids = {0};
    find_ids.kind = EcsQueryIdsRight;
    find_ids.field_index = -1;
    find_ids.first = op->first;
    find_ids.second = op->second;
    find_ids.flags = op->flags;
    find_ids.flags &= (ecs_flags8_t)~((EcsQueryIsVar|EcsQueryIsEntity) << EcsQuerySrc);
    find_ids.second.var = tgt;
    flecs_query_write_ctx(tgt, ctx, cond_write);
    flecs_query_write(tgt, &find_ids.written);
    flecs_query_op_insert(&find_ids, ctx);

    /* Next, iterate all tables for the ids. This fixes op.src */
    ecs_query_op_t and_op = {0};
    and_op.kind = EcsQueryAnd;
    and_op.field_index = op->field_index;
    and_op.first = op->first;
    and_op.second = op->second;
    and_op.src = op->src;
    and_op.flags = op->flags | EcsQueryIsSelf;
    and_op.second.var = tgt;
    flecs_query_write_ctx(and_op.src.var, ctx, cond_write);
    flecs_query_write(and_op.src.var, &and_op.written);
    flecs_query_op_insert(&and_op, ctx);
}

static
void flecs_query_insert_inheritance(
    ecs_query_impl_t *query,
    ecs_term_t *term,
    ecs_query_op_t *op,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write)
{
    /* Anonymous variable to store the resolved component ids */
    ecs_var_id_t tvar = flecs_query_add_var(query, NULL, NULL, EcsVarTable);
    ecs_var_id_t evar = flecs_query_add_var(query, NULL, NULL, EcsVarEntity);

    flecs_set_var_label(&query->vars[tvar], ecs_get_name(query->pub.world, 
        ECS_TERM_REF_ID(&term->first)));
    flecs_set_var_label(&query->vars[evar], ecs_get_name(query->pub.world, 
        ECS_TERM_REF_ID(&term->first)));

    ecs_query_op_t trav_op = {0};
    trav_op.kind = EcsQueryTrav;
    trav_op.field_index = -1;
    trav_op.first.entity = EcsIsA;
    trav_op.second.entity = ECS_TERM_REF_ID(&term->first);
    trav_op.src.var = tvar;
    trav_op.flags = EcsQueryIsSelf;
    trav_op.flags |= (EcsQueryIsEntity << EcsQueryFirst);
    trav_op.flags |= (EcsQueryIsEntity << EcsQuerySecond);
    trav_op.flags |= (EcsQueryIsVar << EcsQuerySrc);
    trav_op.written |= (1ull << tvar);
    if (term->first.id & EcsSelf) {
        trav_op.match_flags |= EcsTermReflexive;
    }
    flecs_query_op_insert(&trav_op, ctx);
    flecs_query_insert_each(tvar, evar, ctx, cond_write);

    ecs_query_ref_t r = { .var = evar };
    op->first = r;
    op->flags &= (ecs_flags8_t)~(EcsQueryIsEntity << EcsQueryFirst);
    op->flags |= (EcsQueryIsVar << EcsQueryFirst);
}

void flecs_query_compile_term_ref(
    ecs_world_t *world,
    ecs_query_impl_t *query,
    ecs_query_op_t *op,
    ecs_term_ref_t *term_ref,
    ecs_query_ref_t *ref,
    ecs_flags8_t ref_kind,
    ecs_var_kind_t kind,
    ecs_query_compile_ctx_t *ctx,
    bool create_wildcard_vars)
{
    (void)world;

    if (!ecs_term_ref_is_set(term_ref)) {
        return;
    }

    if (term_ref->id & EcsIsVariable) {
        op->flags |= (ecs_flags8_t)(EcsQueryIsVar << ref_kind);
        const char *name = flecs_term_ref_var_name(term_ref);
        if (name) {
            ref->var = flecs_query_most_specific_var(query, name, kind, ctx);
            ecs_assert(ref->var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        } else if (create_wildcard_vars) {
            bool is_wildcard = flecs_term_ref_is_wildcard(term_ref);
            if (is_wildcard && (kind == EcsVarAny)) {
                ref->var = flecs_query_add_var(query, NULL, NULL, EcsVarTable);
            } else {
                ref->var = flecs_query_add_var(query, NULL, NULL, EcsVarEntity);
            }
            if (is_wildcard) {
                flecs_set_var_label(&query->vars[ref->var], 
                    ecs_get_name(world, ECS_TERM_REF_ID(term_ref)));
            }
            ecs_assert(ref->var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (term_ref->id & EcsIsEntity) {
        op->flags |= (ecs_flags8_t)(EcsQueryIsEntity << ref_kind);
        ref->entity = ECS_TERM_REF_ID(term_ref);
    }
}

static
int flecs_query_compile_ensure_vars(
    ecs_query_impl_t *query,
    ecs_query_op_t *op,
    ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write,
    bool *written_out)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, ref_kind);
    bool written = false;

    if (flags & EcsQueryIsVar) {
        ecs_var_id_t var_id = ref->var;
        ecs_query_var_t *var = &query->vars[var_id];

        if (var->kind == EcsVarEntity && 
            !flecs_query_is_written(var_id, ctx->written)) 
        {
            /* If entity variable is not yet written but a table variant exists
             * that has been written, insert each operation to translate from
             * entity variable to table */
            ecs_var_id_t tvar = var->table_id;
            if ((tvar != EcsVarNone) && 
                flecs_query_is_written(tvar, ctx->written)) 
            {
                if (var->lookup) {
                    if (!flecs_query_is_written(tvar, ctx->written)) {
                        ecs_err("dependent variable of '$%s' is not written", 
                            var->name);
                        return -1;
                    }

                    if (!flecs_query_is_written(var->base_id, ctx->written)) {
                        flecs_query_insert_each(
                            tvar, var->base_id, ctx, cond_write);
                    }
                } else {
                    flecs_query_insert_each(tvar, var_id, ctx, cond_write);
                }

                /* Variable was written, just not as entity */
                written = true;
            } else if (var->lookup) {
                if (!flecs_query_is_written(var->base_id, ctx->written)) {
                    ecs_err("dependent variable of '$%s' is not written", 
                        var->name);
                    return -1;
                }
            }
        }

        written |= flecs_query_is_written(var_id, ctx->written);
    } else {
        /* If it's not a variable, it's always written */
        written = true;
    }

    if (written_out) {
        *written_out = written;
    }

    return 0;
}

static
bool flecs_query_compile_lookup(
    ecs_query_impl_t *query,
    ecs_var_id_t var_id,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write) 
{
    ecs_query_var_t *var = &query->vars[var_id];
    if (var->lookup) {
        flecs_query_insert_lookup(var->base_id, var_id, ctx, cond_write);
        return true;
    } else {
        return false;
    }
}

static
void flecs_query_insert_contains(
    ecs_query_impl_t *query,
    ecs_var_id_t src_var,
    ecs_var_id_t other_var,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t contains = {0};
    if ((src_var != other_var) && (src_var == query->vars[other_var].table_id)) {
        contains.kind = EcsQueryContain;
        contains.src.var = src_var;
        contains.first.var = other_var;
        contains.flags |= (EcsQueryIsVar << EcsQuerySrc) | 
                          (EcsQueryIsVar << EcsQueryFirst);
        flecs_query_op_insert(&contains, ctx);
    }
}

static
void flecs_query_insert_pair_eq(
    int32_t field_index,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t contains = {0};
    contains.kind = EcsQueryPairEq;
    contains.field_index = flecs_ito(int8_t, field_index);
    flecs_query_op_insert(&contains, ctx);
}

static
int flecs_query_compile_builtin_pred(
    ecs_query_t *q,
    ecs_term_t *term,
    ecs_query_op_t *op,
    ecs_write_flags_t write_state)
{
    ecs_entity_t id = ECS_TERM_REF_ID(&term->first);

    ecs_query_op_kind_t eq[] = {EcsQueryPredEq, EcsQueryPredNeq};
    ecs_query_op_kind_t eq_name[] = {EcsQueryPredEqName, EcsQueryPredNeqName};
    ecs_query_op_kind_t eq_match[] = {EcsQueryPredEqMatch, EcsQueryPredNeqMatch};
    
    ecs_flags16_t flags_src = flecs_query_ref_flags(op->flags, EcsQuerySrc);
    ecs_flags16_t flags_2nd = flecs_query_ref_flags(op->flags, EcsQuerySecond);

    if (id == EcsPredEq) {
        if (term->second.id & EcsIsName) {
            op->kind = flecs_ito(uint8_t, eq_name[term->oper == EcsNot]);
        } else {
            op->kind = flecs_ito(uint8_t, eq[term->oper == EcsNot]);
        }
    } else if (id == EcsPredMatch) {
        op->kind = flecs_ito(uint8_t, eq_match[term->oper == EcsNot]);
    }

    if (flags_2nd & EcsQueryIsVar) {
        if (!(write_state & (1ull << op->second.var))) {
            ecs_err("uninitialized variable '%s' on right-hand side of "
                "equality operator", ecs_query_var_name(q, op->second.var));
            return -1;
        }
    }

    ecs_assert(flags_src & EcsQueryIsVar, ECS_INTERNAL_ERROR, NULL);
    (void)flags_src;

    if (!(write_state & (1ull << op->src.var))) {
        /* If this is an == operator with a right-hand side that resolves to a
         * single entity, the left-hand side is allowed to be undefined, as the
         * instruction will be evaluated as an assignment. */
        if (op->kind != EcsQueryPredEq && op->kind != EcsQueryPredEqName) {
            ecs_err("uninitialized variable '%s' on left-hand side of "
                "equality operator", ecs_query_var_name(q, op->src.var));
            return -1;
        }
    }

    return 0;
}

static
int flecs_query_ensure_scope_var(
    ecs_query_impl_t *query,
    ecs_query_op_t *op,
    ecs_query_ref_t *ref,
    ecs_flags16_t ref_kind,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_var_id_t var = ref->var;

    if (query->vars[var].kind == EcsVarEntity && 
        !flecs_query_is_written(var, ctx->written)) 
    {
        ecs_var_id_t table_var = query->vars[var].table_id;
        if (table_var != EcsVarNone && 
            flecs_query_is_written(table_var, ctx->written)) 
        {
            if (flecs_query_compile_ensure_vars(
                query, op, ref, ref_kind, ctx, false, NULL))
            {
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

static
int flecs_query_ensure_scope_vars(
    ecs_world_t *world,
    ecs_query_impl_t *query,
    ecs_query_compile_ctx_t *ctx,
    ecs_term_t *term)
{
    /* If the scope uses variables as entity that have only been written as
     * table, resolve them as entities before entering the scope. */
    ecs_term_t *cur = term;
    while(ECS_TERM_REF_ID(&cur->first) != EcsScopeClose) {
        /* Dummy operation to obtain variable information for term */
        ecs_query_op_t op = {0};
        flecs_query_compile_term_ref(world, query, &op, &cur->first, 
            &op.first, EcsQueryFirst, EcsVarEntity, ctx, false);
        flecs_query_compile_term_ref(world, query, &op, &cur->second, 
            &op.second, EcsQuerySecond, EcsVarEntity, ctx, false);

        if (op.flags & (EcsQueryIsVar << EcsQueryFirst)) {
            if (flecs_query_ensure_scope_var(
                query, &op, &op.first, EcsQueryFirst, ctx))
            {
                goto error;
            }
        }

        if (op.flags & (EcsQueryIsVar << EcsQuerySecond)) {
            if (flecs_query_ensure_scope_var(
                query, &op, &op.second, EcsQuerySecond, ctx))
            {
                goto error;
            }
        }

        cur ++;
    }

    return 0;
error:
    return -1;
}

static
void flecs_query_compile_push(
    ecs_query_compile_ctx_t *ctx)
{
    ctx->cur = &ctx->ctrlflow[++ ctx->scope];
    ctx->cur->lbl_begin = -1;
    ctx->cur->lbl_begin = -1;
}

static
void flecs_query_compile_pop(
    ecs_query_compile_ctx_t *ctx)
{
    ctx->cur = &ctx->ctrlflow[-- ctx->scope];
}

static
int flecs_query_compile_0_src(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx)
{
    /* If the term has a 0 source, check if it's a scope open/close */
    if (ECS_TERM_REF_ID(&term->first) == EcsScopeOpen) {
        if (flecs_query_ensure_scope_vars(world, impl, ctx, term)) {
            goto error;
        }
        if (term->oper == EcsNot) {
            ctx->scope_is_not |= (ecs_flags32_t)(1ull << ctx->scope);
            flecs_query_begin_block(EcsQueryNot, ctx);
        } else {
            ctx->scope_is_not &= (ecs_flags32_t)~(1ull << ctx->scope);
        }
        flecs_query_compile_push(ctx);
    } else if (ECS_TERM_REF_ID(&term->first) == EcsScopeClose) {
        flecs_query_compile_pop(ctx);
        if (ctx->scope_is_not & (ecs_flags32_t)(1ull << (ctx->scope))) {
            flecs_query_end_block(ctx, false);
        }
    } else {
        /* Noop */
    }

    return 0;
error:
    return -1;
}

static
bool flecs_query_select_all(
    ecs_term_t *term,
    ecs_query_op_t *op,
    ecs_var_id_t src_var,
    ecs_query_compile_ctx_t *ctx)
{
    bool builtin_pred = flecs_term_is_builtin_pred(term);
    bool pred_match = builtin_pred && ECS_TERM_REF_ID(&term->first) == EcsPredMatch;

    if (term->oper == EcsNot || term->oper == EcsOptional || 
        term->oper == EcsNotFrom || pred_match) 
    {
        ecs_query_op_t match_any = {0};
        match_any.kind = EcsAnd;
        match_any.flags = EcsQueryIsSelf | (EcsQueryIsEntity << EcsQueryFirst);
        match_any.flags |= (EcsQueryIsVar << EcsQuerySrc);
        match_any.src = op->src;
        match_any.field_index = -1;
        if (!pred_match) {
            match_any.first.entity = EcsAny;
        } else {
            /* If matching by name, instead of finding all tables, just find
             * the ones with a name. */
            match_any.first.entity = ecs_id(EcsIdentifier);
            match_any.second.entity = EcsName;
            match_any.flags |= (EcsQueryIsEntity << EcsQuerySecond);
        }
        match_any.written = (1ull << src_var);
        flecs_query_op_insert(&match_any, ctx);
        flecs_query_write_ctx(op->src.var, ctx, false);

        /* Update write administration */
        return true;
    }
    return false;
}

#ifdef FLECS_META
static
int flecs_query_compile_begin_member_term(
    ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx,
    ecs_entity_t first_id)
{
    ecs_assert(first_id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(first_id & EcsIsEntity, ECS_INTERNAL_ERROR, NULL);

    first_id = ECS_TERM_REF_ID(&term->first);

    /* First compile as if it's a regular term, to match the component */
    term->flags_ &= (uint16_t)~EcsTermIsMember;

    /* Replace term id with member parent (the component) */
    ecs_entity_t component = ecs_get_parent(world, first_id);
    if (!component) {
        ecs_err("member without parent in query");
        return -1;
    }

    if (!ecs_has(world, component, EcsComponent)) {
        ecs_err("parent of member is not a component");
        return -1;
    }

    bool second_wildcard = 
        (ECS_TERM_REF_ID(&term->second) == EcsWildcard || 
            ECS_TERM_REF_ID(&term->second) == EcsAny) &&
        (term->second.id & EcsIsVariable) && !term->second.name;

    term->first.id = component | ECS_TERM_REF_FLAGS(&term->first);
    term->second.id = 0;
    term->id = component;

    ctx->oper = (ecs_oper_kind_t)term->oper;
    if (term->oper == EcsNot && !second_wildcard) {
        /* When matching a member term with not operator, we need to cover both
         * the case where an entity doesn't have the component, and where it 
         * does have the component, but doesn't match the member. */
        term->oper = EcsOptional;
    }

    return 0;
}

static
int flecs_query_compile_end_member_term(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_op_t *op,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx,
    ecs_id_t term_id,
    ecs_entity_t first_id,
    ecs_entity_t second_id,
    bool cond_write)
{
    ecs_entity_t component = ECS_TERM_REF_ID(&term->first);
    const EcsComponent *comp = ecs_get(world, component, EcsComponent);
    ecs_assert(comp != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Restore term values */
    term->id = term_id;
    term->first.id = first_id;
    term->second.id = second_id;
    term->flags_ |= EcsTermIsMember;
    term->oper = flecs_ito(int16_t, ctx->oper);

    first_id = ECS_TERM_REF_ID(&term->first);
    const EcsMember *member = ecs_get(world, first_id, EcsMember);
    ecs_assert(member != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_query_var_t *var = &impl->vars[op->src.var];
    const char *var_name = flecs_term_ref_var_name(&term->src);
    ecs_var_id_t evar = flecs_query_find_var_id(
        impl, var_name, EcsVarEntity);
    
    bool second_wildcard = 
        (ECS_TERM_REF_ID(&term->second) == EcsWildcard || 
            ECS_TERM_REF_ID(&term->second) == EcsAny) &&
        (term->second.id & EcsIsVariable) && !term->second.name;

    if (term->oper == EcsOptional) {
        second_wildcard = true;
    }

    ecs_query_op_t mbr_op = *op;
    mbr_op.kind = EcsQueryMemberEq;
    mbr_op.first.entity = /* Encode type size and member offset */
        flecs_ito(uint32_t, member->offset) | 
        (flecs_ito(uint64_t, comp->size) << 32);

    /* If this is a term with a Not operator, conditionally evaluate member on
     * whether term was set by previous operation (see begin_member_term). */
    if (ctx->oper == EcsNot || ctx->oper == EcsOptional) {
        if (second_wildcard && ctx->oper == EcsNot) {
            /* A !(T.value, *) term doesn't need special operations */
            return 0;
        }

        /* Resolve to entity variable before entering if block, so that we 
         * don't have different branches of the query working with different
         * versions of the same variable. */
        if (var->kind == EcsVarTable) {
            flecs_query_insert_each(op->src.var, evar, ctx, cond_write);
            var = &impl->vars[evar];
        }

        ecs_query_op_t *if_op = flecs_query_begin_block(EcsQueryIfSet, ctx);
        if_op->other = term->field_index;

        if (ctx->oper == EcsNot) {
            mbr_op.kind = EcsQueryMemberNeq;
        }
    }

    if (var->kind == EcsVarTable) {
        /* If MemberEq is called on table variable, store it on .other member.
         * This causes MemberEq to do double duty as 'each' instruction,
         * which is faster than having to go back & forth between instructions
         * while finding matching values. */
        mbr_op.other = flecs_itolbl(op->src.var + 1);

        /* Mark entity variable as written */
        flecs_query_write_ctx(evar, ctx, cond_write);
        flecs_query_write(evar, &mbr_op.written);
    }

    flecs_query_compile_term_ref(world, impl, &mbr_op, &term->src, 
        &mbr_op.src, EcsQuerySrc, EcsVarEntity, ctx, true);

    if (second_wildcard) {
        mbr_op.flags |= (EcsQueryIsEntity << EcsQuerySecond);
        mbr_op.second.entity = EcsWildcard;
    } else {
        flecs_query_compile_term_ref(world, impl, &mbr_op, &term->second, 
            &mbr_op.second, EcsQuerySecond, EcsVarEntity, ctx, true);
        
        if (term->second.id & EcsIsVariable) {
            if (flecs_query_compile_ensure_vars(impl, &mbr_op, &mbr_op.second, 
                EcsQuerySecond, ctx, cond_write, NULL)) 
            {
                goto error;
            }

            flecs_query_write_ctx(mbr_op.second.var, ctx, cond_write);
            flecs_query_write(mbr_op.second.var, &mbr_op.written);
        }
    }

    flecs_query_op_insert(&mbr_op, ctx);

    if (ctx->oper == EcsNot || ctx->oper == EcsOptional) {
        flecs_query_end_block(ctx, false);
    }

    return 0;
error:
    return -1;
}
#else
static
int flecs_query_compile_begin_member_term(
    ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx,
    ecs_entity_t first_id)
{
    (void)world; (void)term; (void)ctx; (void)first_id;
    return 0;
}

static
int flecs_query_compile_end_member_term(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_op_t *op,
    ecs_term_t *term,
    ecs_query_compile_ctx_t *ctx,
    ecs_id_t term_id,
    ecs_entity_t first_id,
    ecs_entity_t second_id,
    bool cond_write)
{
    (void)world; (void)impl; (void)op; (void)term; (void)ctx; (void)term_id;
    (void)first_id; (void)second_id; (void)cond_write;
    return 0;
}
#endif

static
void flecs_query_mark_last_or_op(
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_op_t *op_ptr = ecs_vec_last_t(ctx->ops, ecs_query_op_t);
    op_ptr->next = FlecsRuleOrMarker;
}

static
void flecs_query_set_op_kind(
    ecs_query_t *q,
    ecs_query_op_t *op,
    ecs_term_t *term,
    bool src_is_var,
    bool member_term)
{
    /* Default instruction for And operators. If the source is fixed (like for
     * singletons or terms with an entity source), use With, which like And but
     * just matches against a source (vs. finding a source). */
    op->kind = src_is_var ? EcsQueryAnd : EcsQueryWith;

    /* Ignore cascade flag */
    ecs_entity_t trav_flags = EcsTraverseFlags & ~(EcsCascade|EcsDesc);

    /* Handle *From operators */
    if (term->oper == EcsAndFrom) {
        op->kind = EcsQueryAndFrom;
    } else if (term->oper == EcsOrFrom) {
        op->kind = EcsQueryOrFrom;
    } else if (term->oper == EcsNotFrom) {
        op->kind = EcsQueryNotFrom;

    /* If query is transitive, use Trav(ersal) instruction */
    } else if (term->flags_ & EcsTermTransitive) {
        ecs_assert(ecs_term_ref_is_set(&term->second), ECS_INTERNAL_ERROR, NULL);
        op->kind = EcsQueryTrav;

    /* If term queries for union pair, use union instruction */
    } else if (term->flags_ & EcsTermIsUnion) {
        if (op->kind == EcsQueryAnd) {
            op->kind = EcsQueryUnionEq;
            if (term->oper == EcsNot) {
                if (!ecs_id_is_wildcard(ECS_TERM_REF_ID(&term->second))) {
                    term->oper = EcsAnd;
                    op->kind = EcsQueryUnionNeq;
                }
            }
        } else {
            op->kind = EcsQueryUnionEqWith;
        }

        if ((term->src.id & trav_flags) == EcsUp) {
            if (op->kind == EcsQueryUnionEq) {
                op->kind = EcsQueryUnionEqUp;
            }
        } else if ((term->src.id & trav_flags) == (EcsSelf|EcsUp)) {
            if (op->kind == EcsQueryUnionEq) {
                op->kind = EcsQueryUnionEqSelfUp;
            }
        }
    } else {
        if (term->flags_ & (EcsTermMatchAny|EcsTermMatchAnySrc)) {
            op->kind = EcsQueryAndAny;
        } else if ((term->src.id & trav_flags) == EcsUp) {
            op->kind = EcsQueryUp;
        } else if ((term->src.id & trav_flags) == (EcsSelf|EcsUp)) {
            op->kind = EcsQuerySelfUp;
        }
    }

    /* If term has fixed id, insert simpler instruction that skips dealing with
     * wildcard terms and variables */
    if (flecs_term_is_fixed_id(q, term) && !member_term) {
        if (op->kind == EcsQueryAnd) {
            op->kind = EcsQueryAndId;
        } else if (op->kind == EcsQuerySelfUp) {
            op->kind = EcsQuerySelfUpId;
        } else if (op->kind == EcsQueryUp) {
            op->kind = EcsQueryUpId;
        }
    }
}

int flecs_query_compile_term(
    ecs_world_t *world,
    ecs_query_impl_t *query,
    ecs_term_t *term,
    ecs_flags64_t *populated,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_id_t term_id = term->id;
    ecs_entity_t first_id = term->first.id;
    ecs_entity_t second_id = term->second.id;
    bool toggle_term = (term->flags_ & EcsTermIsToggle) != 0;
    bool member_term = (term->flags_ & EcsTermIsMember) != 0;
    if (member_term) {
        (*populated) |= (1llu << term->field_index);
        flecs_query_compile_begin_member_term(world, term, ctx, first_id);
    }

    ecs_query_t *q = &query->pub;
    bool first_term = term == q->terms;
    bool first_is_var = term->first.id & EcsIsVariable;
    bool second_is_var = term->second.id & EcsIsVariable;
    bool src_is_var = term->src.id & EcsIsVariable;
    bool src_is_wildcard = src_is_var && 
        (ECS_TERM_REF_ID(&term->src) == EcsWildcard ||
            ECS_TERM_REF_ID(&term->src) == EcsAny);
    bool src_is_lookup = false;
    bool builtin_pred = flecs_term_is_builtin_pred(term);
    bool is_optional = (term->oper == EcsOptional);
    bool is_or = flecs_term_is_or(q, term);
    bool first_or = false, last_or = false;
    bool cond_write = term->oper == EcsOptional || is_or;
    ecs_query_op_t op = {0};

    if (is_or) {
        first_or = first_term || (term[-1].oper != EcsOr);
        last_or = term->oper != EcsOr;
    }

    if (term->oper == EcsAndFrom || term->oper == EcsOrFrom || 
        term->oper == EcsNotFrom) 
    {
        const ecs_type_t *type = ecs_get_type(world, term->id);
        if (!type) {
            /* Empty type for id in *From operation is a noop */
            ctx->skipped ++;
            return 0;
        }

        int32_t i, count = type->count;
        ecs_id_t *ti_ids = type->array;

        for (i = 0; i < count; i ++) {
            ecs_id_t ti_id = ti_ids[i];
            ecs_id_record_t *idr = flecs_id_record_get(world, ti_id);
            if (!(idr->flags & EcsIdOnInstantiateDontInherit)) {
                break;
            }
        }

        if (i == count) {
            /* Type did not contain any ids to perform operation on */
            ctx->skipped ++;
            return 0;
        }
    }

    /* !_ (don't match anything) terms always return nothing. */
    if (term->oper == EcsNot && term->id == EcsAny) {
        op.kind = EcsQueryNothing;
        flecs_query_op_insert(&op, ctx);
        return 0;
    }

    if (first_or) {
        ctx->ctrlflow->cond_written_or = ctx->cond_written;
        ctx->ctrlflow->in_or = true;
    } else if (is_or) {
        ctx->written = ctx->ctrlflow->written_or;
    }

    if (!ECS_TERM_REF_ID(&term->src) && term->src.id & EcsIsEntity) {
        if (flecs_query_compile_0_src(world, query, term, ctx)) {
            goto error;
        }
        return 0;
    }

    if (builtin_pred) {
        ecs_entity_t id_noflags = ECS_TERM_REF_ID(&term->second);
        if (id_noflags == EcsWildcard || id_noflags == EcsAny) {
            /* Noop */
            return 0;
        }
    }

    op.field_index = flecs_ito(int8_t, term->field_index);
    op.term_index = flecs_ito(int8_t, term - q->terms);

    flecs_query_set_op_kind(q, &op, term, src_is_var, member_term);

    bool is_not = (term->oper == EcsNot) && !builtin_pred;

    /* Save write state at start of term so we can use it to reliably track
     * variables got written by this term. */
    ecs_write_flags_t cond_write_state = ctx->cond_written;

    /* Resolve variables and entities for operation arguments */
    flecs_query_compile_term_ref(world, query, &op, &term->first, 
        &op.first, EcsQueryFirst, EcsVarEntity, ctx, true);
    flecs_query_compile_term_ref(world, query, &op, &term->second, 
        &op.second, EcsQuerySecond, EcsVarEntity, ctx, true);
    flecs_query_compile_term_ref(world, query, &op, &term->src, 
        &op.src, EcsQuerySrc, EcsVarAny, ctx, true);

    bool src_written = true;
    if (src_is_var) {
        src_is_lookup = query->vars[op.src.var].lookup != NULL;
        src_written = flecs_query_is_written(op.src.var, ctx->written);
    }

    /* Insert each instructions for table -> entity variable if needed */
    bool first_written, second_written;
    if (flecs_query_compile_ensure_vars(
        query, &op, &op.first, EcsQueryFirst, ctx, cond_write, &first_written)) 
    {
        goto error;    
    }

    if (flecs_query_compile_ensure_vars(
        query, &op, &op.second, EcsQuerySecond, ctx, cond_write, &second_written))
    {
        goto error;
    }

    /* Store write state of variables for first OR term in chain which will get
     * restored for the other terms in the chain, so that all OR terms make the
     * same assumptions about which variables were already written. */
    if (first_or) {
        ctx->ctrlflow->written_or = ctx->written;
    }

    /* If an optional or not term is inserted for a source that's not been 
     * written to yet, insert instruction that selects all entities so we have
     * something to match the optional/not against. */
    if (src_is_var && !src_written && !src_is_wildcard && !src_is_lookup) {
        src_written = flecs_query_select_all(term, &op, op.src.var, ctx);
    }

    /* A bit of special logic for OR expressions and equality predicates. If the
     * left-hand of an equality operator is a table, and there are multiple
     * operators in an Or expression, the Or chain should match all entities in
     * the table that match the right hand sides of the operator expressions. 
     * For this to work, the src variable needs to be resolved as entity, as an
     * Or chain would otherwise only yield the first match from a table. */
    if (src_is_var && src_written && (builtin_pred || member_term) && term->oper == EcsOr) {
        if (query->vars[op.src.var].kind == EcsVarTable) {
            flecs_query_compile_term_ref(world, query, &op, &term->src, 
                    &op.src, EcsQuerySrc, EcsVarEntity, ctx, true);
            ctx->ctrlflow->written_or |= (1llu << op.src.var);
        }
    }

    if (flecs_query_compile_ensure_vars(
        query, &op, &op.src, EcsQuerySrc, ctx, cond_write, NULL)) 
    {
        goto error;
    }

    /* If source is Any (_) and first and/or second are unconstrained, insert an
     * ids instruction instead of an And */
    if (term->flags_ & EcsTermMatchAnySrc) {
        op.kind = EcsQueryIds;
        /* Use up-to-date written values after potentially inserting each */
        if (!first_written || !second_written) {
            if (!first_written) {
                /* If first is unknown, traverse left: <- (*, t) */
                if (ECS_TERM_REF_ID(&term->first) != EcsAny) {
                    op.kind = EcsQueryIdsLeft;
                }
            } else {
                /* If second is wildcard, traverse right: (r, *) -> */
                if (ECS_TERM_REF_ID(&term->second) != EcsAny) {
                    op.kind = EcsQueryIdsRight;
                }
            }
            op.src.entity = 0;
            src_is_var = false;
            op.flags &= (ecs_flags8_t)~(EcsQueryIsVar << EcsQuerySrc); /* ids has no src */
            op.flags &= (ecs_flags8_t)~(EcsQueryIsEntity << EcsQuerySrc);
        }

    /* If source variable is not written and we're querying just for Any, insert
     * a dedicated instruction that uses the Any record in the id index. Any 
     * queries that are evaluated against written sources can use Wildcard 
     * records, which is what the AndAny instruction does. */
    } else if (!src_written && term->id == EcsAny && op.kind == EcsQueryAndAny) {
        /* Lookup variables ($var.child_name) are always written */
        if (!src_is_lookup) {
            op.kind = EcsQueryOnlyAny; /* Uses Any (_) id record */
        }
    }

    /* If this is a transitive term and both the target and source are unknown,
     * find the targets for the relationship first. This clusters together 
     * tables for the same target, which allows for more efficient usage of the
     * traversal caches. */
    if (term->flags_ & EcsTermTransitive && src_is_var && second_is_var) {
        if (!src_written && !second_written) {
            flecs_query_insert_unconstrained_transitive(
                query, &op, ctx, cond_write);
        }
    }

    /* Check if this term has variables that have been conditionally written,
     * like variables written by an optional term. */
    if (ctx->cond_written) {
        if (!is_or || first_or) {
            flecs_query_begin_block_cond_eval(&op, ctx, cond_write_state);
        }
    }

    /* If term can toggle and is Not, change operator to Optional as we
     * have to match entities that have the component but disabled. */
    if (toggle_term && is_not) {
        is_not = false;
        is_optional = true;
    }

    /* Handle Not, Optional, Or operators */
    if (is_not) {
        flecs_query_begin_block(EcsQueryNot, ctx);
    } else if (is_optional) {
        flecs_query_begin_block(EcsQueryOptional, ctx);
    } else if (first_or) {
        flecs_query_begin_block_or(&op, term, ctx);
    }

    /* If term has component inheritance enabled, insert instruction to walk
     * down the relationship tree of the id. */
    if (term->flags_ & EcsTermIdInherited) {
        flecs_query_insert_inheritance(query, term, &op, ctx, cond_write);
    }

    op.match_flags = term->flags_;

    ecs_write_flags_t write_state = ctx->written;
    if (first_is_var) {
        op.flags &= (ecs_flags8_t)~(EcsQueryIsEntity << EcsQueryFirst);
        op.flags |= (EcsQueryIsVar << EcsQueryFirst);
    }

    if (term->src.id & EcsSelf) {
        op.flags |= EcsQueryIsSelf;
    }

    /* Insert instructions for lookup variables */
    if (first_is_var) {
        if (flecs_query_compile_lookup(query, op.first.var, ctx, cond_write)) {
            write_state |= (1ull << op.first.var); // lookups are resolved inline
        }
    }
    if (src_is_var) {
        if (flecs_query_compile_lookup(query, op.src.var, ctx, cond_write)) {
            write_state |= (1ull << op.src.var); // lookups are resolved inline
        }
    }
    if (second_is_var) {
        if (flecs_query_compile_lookup(query, op.second.var, ctx, cond_write)) {
            write_state |= (1ull << op.second.var); // lookups are resolved inline
        }
    }

    if (builtin_pred) {
        if (flecs_query_compile_builtin_pred(q, term, &op, write_state)) {
            goto error;
        }
    }

    /* If we're writing the $this variable, filter out disabled/prefab entities
     * unless the query explicitly matches them.
     * This could've been done with regular With instructions, but since 
     * filtering out disabled/prefab entities is the default and this check is
     * cheap to perform on table flags, it's worth special casing. */
    if (!src_written && op.src.var == 0) {
        ecs_flags32_t query_flags = q->flags;
        if (!(query_flags & EcsQueryMatchDisabled) || 
            !(query_flags & EcsQueryMatchPrefab)) 
        {
            ecs_flags32_t table_flags = EcsTableNotQueryable;
            if (!(query_flags & EcsQueryMatchDisabled)) {
                table_flags |= EcsTableIsDisabled;
            }
            if (!(query_flags & EcsQueryMatchPrefab)) {
                table_flags |= EcsTableIsPrefab;
            }

            op.other = flecs_itolbl(table_flags);
        }
    }

    /* After evaluating a term, a used variable is always written */
    if (src_is_var) {
        flecs_query_write(op.src.var, &op.written);
        flecs_query_write_ctx(op.src.var, ctx, cond_write);
    }
    if (first_is_var) {
        flecs_query_write(op.first.var, &op.written);
        flecs_query_write_ctx(op.first.var, ctx, cond_write);
    }
    if (second_is_var) {
        flecs_query_write(op.second.var, &op.written);
        flecs_query_write_ctx(op.second.var, ctx, cond_write);
    }
    
    flecs_query_op_insert(&op, ctx);

    ctx->cur->lbl_query = flecs_itolbl(ecs_vec_count(ctx->ops) - 1);
    if (is_or && !member_term) {
        flecs_query_mark_last_or_op(ctx);
    }

    /* Handle self-references between src and first/second variables */
    if (src_is_var) {
        if (first_is_var) {
            flecs_query_insert_contains(query, op.src.var, op.first.var, ctx);
        }
        if (second_is_var && op.first.var != op.second.var) {
            flecs_query_insert_contains(query, op.src.var, op.second.var, ctx);
        }
    }

    /* Handle self references between first and second variables */
    if (!ecs_id_is_wildcard(first_id)) {
        if (first_is_var && !first_written && (op.first.var == op.second.var)) {
            flecs_query_insert_pair_eq(term->field_index, ctx);
        }
    }

    /* Handle closing of Not, Optional and Or operators */
    if (is_not) {
        flecs_query_end_block(ctx, true);
    } else if (is_optional) {
        flecs_query_end_block(ctx, true);
    }

    /* Now that the term is resolved, evaluate member of component */
    if (member_term) {
        flecs_query_compile_end_member_term(world, query, &op, term, ctx, 
            term_id, first_id, second_id, cond_write);
        if (is_or) {
            flecs_query_mark_last_or_op(ctx);
        }
    }

    if (last_or) {
        flecs_query_end_block_or(query, ctx);
    }

    /* Handle closing of conditional evaluation */
    if (ctx->cur->lbl_cond_eval && (first_is_var || second_is_var || src_is_var)) {
        if (!is_or || last_or) {
            flecs_query_end_block_cond_eval(ctx);
        }
    }

    /* Ensure that term id is set after evaluating Not */
    if (term->flags_ & EcsTermIdInherited) {
        if (is_not) {
            ecs_query_op_t set_id = {0};
            set_id.kind = EcsQuerySetId;
            set_id.first.entity = term->id;
            set_id.flags = (EcsQueryIsEntity << EcsQueryFirst);
            set_id.field_index = flecs_ito(int8_t, term->field_index);
            flecs_query_op_insert(&set_id, ctx);
        }
    }

    return 0;
error:
    return -1;
}
