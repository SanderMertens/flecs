/**
 * @file addons/rules/compile.c
 * @brief Compile rule program from filter.
 */

#include "rules.h"

#ifdef FLECS_RULES

#define FlecsRuleOrMarker ((int16_t)-2) /* Marks instruction in OR chain */

ecs_rule_lbl_t flecs_itolbl(int64_t val) {
    return flecs_ito(int16_t, val);
}

static
ecs_var_id_t flecs_itovar(int64_t val) {
    return flecs_ito(uint8_t, val);
}

static
ecs_var_id_t flecs_utovar(uint64_t val) {
    return flecs_uto(uint8_t, val);
}

#ifdef FLECS_DEBUG
#define flecs_set_var_label(var, lbl) (var)->label = lbl
#else
#define flecs_set_var_label(var, lbl)
#endif

static
bool flecs_rule_is_builtin_pred(
    ecs_term_t *term)
{
    if (term->first.flags & EcsIsEntity) {
        ecs_entity_t id = term->first.id;
        if (id == EcsPredEq || id == EcsPredMatch || id == EcsPredLookup) {
            return true;
        }
    }
    return false;
}

bool flecs_rule_is_written(
    ecs_var_id_t var_id,
    uint64_t written)
{
    if (var_id == EcsVarNone) {
        return true;
    }

    ecs_assert(var_id < EcsRuleMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    return (written & (1ull << var_id)) != 0;
}

static
void flecs_rule_write(
    ecs_var_id_t var_id,
    uint64_t *written)
{
    ecs_assert(var_id < EcsRuleMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    *written |= (1ull << var_id);
}

static
void flecs_rule_write_ctx(
    ecs_var_id_t var_id,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    bool is_written = flecs_rule_is_written(var_id, ctx->written);
    flecs_rule_write(var_id, &ctx->written);
    if (!is_written) {
        if (cond_write) {
            flecs_rule_write(var_id, &ctx->cond_written);
        }
    }
}

ecs_flags16_t flecs_rule_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind)
{
    return (flags >> kind) & (EcsRuleIsVar | EcsRuleIsEntity);
}

bool flecs_ref_is_written(
    const ecs_rule_op_t *op,
    const ecs_rule_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, kind);
    if (flags & EcsRuleIsEntity) {
        ecs_assert(!(flags & EcsRuleIsVar), ECS_INTERNAL_ERROR, NULL);
        if (ref->entity) {
            return true;
        }
    } else if (flags & EcsRuleIsVar) {
        return flecs_rule_is_written(ref->var, written);
    }
    return false;
}

static
bool flecs_rule_var_is_anonymous(
    const ecs_rule_t *rule,
    ecs_var_id_t var_id)
{
    ecs_rule_var_t *var = &rule->vars[var_id];
    return var->anonymous;
}

static
ecs_var_id_t flecs_rule_find_var_id(
    const ecs_rule_t *rule,
    const char *name,
    ecs_var_kind_t kind)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Backwards compatibility */
    if (!ecs_os_strcmp(name, "This")) {
        name = "this";
    }

    if (kind == EcsVarTable) {
        if (!ecs_os_strcmp(name, EcsThisName)) {
            if (rule->has_table_this) {
                return 0;
            } else {
                return EcsVarNone;
            }
        }

        if (!flecs_name_index_is_init(&rule->tvar_index)) {
            return EcsVarNone;
        }

        uint64_t index = flecs_name_index_find(
            &rule->tvar_index, name, 0, 0);
        if (index == 0) {
            return EcsVarNone;
        }
        return flecs_utovar(index);
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
        return flecs_utovar(index);
    }

    ecs_assert(kind == EcsVarAny, ECS_INTERNAL_ERROR, NULL);

    /* If searching for any kind of variable, start with most specific */
    ecs_var_id_t index = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if (index != EcsVarNone) {
        return index;
    }

    return flecs_rule_find_var_id(rule, name, EcsVarTable);
}

int32_t ecs_rule_var_count(
    const ecs_rule_t *rule)
{
    return rule->var_pub_count;
}

int32_t ecs_rule_find_var(
    const ecs_rule_t *rule,
    const char *name)
{
    ecs_var_id_t var_id = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if (var_id == EcsVarNone) {
        if (rule->filter.flags & EcsFilterMatchThis) {
            if (!ecs_os_strcmp(name, "This")) {
                name = "this";
            }
            if (!ecs_os_strcmp(name, EcsThisName)) {
                var_id = 0;
            }
        }
        if (var_id == EcsVarNone) {
            return -1;
        }
    }
    return (int32_t)var_id;
}

const char* ecs_rule_var_name(
    const ecs_rule_t *rule,
    int32_t var_id)
{
    if (var_id) {
        return rule->vars[var_id].name;
    } else {
        return EcsThisName;
    }
}

bool ecs_rule_var_is_entity(
    const ecs_rule_t *rule,
    int32_t var_id)
{
    return rule->vars[var_id].kind == EcsVarEntity;
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
    ecs_vec_t *vars,
    ecs_var_kind_t kind)
{
    const char *dot = NULL;
    if (name) {
        dot = strchr(name, '.');
        if (dot) {
            kind = EcsVarEntity; /* lookup variables are always entities */
        }
    }

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
    ecs_var_id_t result;
    if (vars) {
        var = ecs_vec_append_t(NULL, vars, ecs_rule_var_t);
        result = var->id = flecs_itovar(ecs_vec_count(vars));
    } else {
        ecs_dbg_assert(rule->var_count < rule->var_size, 
            ECS_INTERNAL_ERROR, NULL);
        var = &rule->vars[rule->var_count];
        result = var->id = flecs_itovar(rule->var_count);
        rule->var_count ++;
    }

    var->kind = flecs_ito(int8_t, kind);
    var->name = name;
    var->table_id = var_id;
    var->base_id = 0;
    var->lookup = NULL;
    flecs_set_var_label(var, NULL);

    if (name) {
        flecs_name_index_init_if(var_index, NULL);
        flecs_name_index_ensure(var_index, var->id, name, 0, 0);
        var->anonymous = name[0] == '_';

        /* Handle variables that require a by-name lookup, e.g. $this.wheel */
        if (dot != NULL) {
            ecs_assert(var->table_id == EcsVarNone, ECS_INTERNAL_ERROR, NULL);
            var->lookup = dot + 1;
        }
    }

    return result;
}

static
ecs_var_id_t flecs_rule_add_var_for_term_id(
    ecs_rule_t *rule,
    ecs_term_id_t *term_id,
    ecs_vec_t *vars,
    ecs_var_kind_t kind)
{
    const char *name = flecs_term_id_var_name(term_id);
    if (!name) {
        return EcsVarNone;
    }

    return flecs_rule_add_var(rule, name, vars, kind);
}

/* This function walks over terms to discover which variables are used in the
 * query. It needs to provide the following functionality:
 * - create table vars for all variables used as source
 * - create entity vars for all variables not used as source
 * - create entity vars for all non-$this vars
 * - create anonymous vars to store the content of wildcards
 * - create anonymous vars to store result of lookups (for $var.child_name)
 * - create anonymous vars for resolving component inheritance
 * - create array that stores the source variable for each field
 * - ensure table vars for non-$this variables are anonymous
 * - ensure variables created inside scopes are anonymous
 * - place anonymous variables after public variables in vars array
 */
static
int flecs_rule_discover_vars(
    ecs_stage_t *stage,
    ecs_rule_t *rule)
{
    ecs_vec_t *vars = &stage->variables; /* Buffer to reduce allocs */
    ecs_vec_reset_t(NULL, vars, ecs_rule_var_t);

    ecs_term_t *terms = rule->filter.terms;
    int32_t a, i, anonymous_count = 0, count = rule->filter.term_count;
    int32_t anonymous_table_count = 0, scope = 0, scoped_var_index = 0;
    bool table_this = false, entity_before_table_this = false;

    /* For This table lookups during discovery. This will be overwritten after
     * discovery with whether the rule actually has a This table variable. */
    rule->has_table_this = true;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *first = &term->first;
        ecs_term_id_t *second = &term->second;
        ecs_term_id_t *src = &term->src;

        if (first->id == EcsScopeOpen) {
            /* Keep track of which variables are first used in scope, so that we
             * can mark them as anonymous. Terms inside a scope are collapsed 
             * into a single result, which means that outside of the scope the
             * value of those variables is undefined. */
            if (!scope) {
                scoped_var_index = ecs_vec_count(vars);
            }
            scope ++;
            continue;
        } else if (first->id == EcsScopeClose) {
            if (!--scope) {
                /* Any new variables declared after entering a scope should be
                 * marked as anonymous. */
                int32_t v;
                for (v = scoped_var_index; v < ecs_vec_count(vars); v ++) {
                    ecs_vec_get_t(vars, ecs_rule_var_t, v)->anonymous = true;
                }
            }
            continue;
        }

        ecs_var_id_t first_var_id = flecs_rule_add_var_for_term_id(
            rule, first, vars, EcsVarEntity);
        if (first_var_id == EcsVarNone) {
            /* If first is not a variable, check if we need to insert anonymous
             * variable for resolving component inheritance */
            if (term->flags & EcsTermIdInherited) {
                anonymous_count += 2; /* table & entity variable */
            }

            /* If first is a wildcard, insert anonymous variable */
            if (flecs_term_id_is_wildcard(first)) {
                anonymous_count ++;
            }
        }

        if ((src->flags & EcsIsVariable) && (src->id != EcsThis)) {
            const char *var_name = flecs_term_id_var_name(src);
            if (var_name) {
                ecs_var_id_t var_id = flecs_rule_find_var_id(
                    rule, var_name, EcsVarEntity);
                if (var_id == EcsVarNone || var_id == first_var_id) {
                    var_id = flecs_rule_add_var(
                        rule, var_name, vars, EcsVarEntity);
                }

                if (var_id != EcsVarNone) {
                    /* Mark variable as one for which we need to create a table
                     * variable. Don't create table variable now, so that we can
                     * store it in the non-public part of the variable array. */
                    ecs_rule_var_t *var = ecs_vec_get_t(
                        vars, ecs_rule_var_t, (int32_t)var_id - 1);
                    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
                    if (!var->lookup) {
                        var->kind = EcsVarAny;
                        anonymous_table_count ++;
                    }

                    if (!(term->flags & EcsTermNoData)) {
                        /* Can't have an anonymous variable as source of a term
                         * that returns a component. We need to return each
                         * instance of the component, whereas anonymous 
                         * variables are not guaranteed to be resolved to 
                         * individual entities. */
                        if (var->anonymous) {
                            ecs_err(
                                "can't use anonymous variable '%s' as source of "
                                "data term", var->name);
                            goto error;
                        }
                    }

                    /* Track which variable ids are used as field source */
                    if (!rule->src_vars) {
                        rule->src_vars = ecs_os_calloc_n(ecs_var_id_t,
                            rule->filter.field_count);
                    }

                    rule->src_vars[term->field_index] = var_id;
                }
            } else {
                if (flecs_term_id_is_wildcard(src)) {
                    anonymous_count ++;
                }
            }
        } else if ((src->flags & EcsIsVariable) && (src->id == EcsThis)) {
            if (flecs_rule_is_builtin_pred(term) && term->oper == EcsOr) {
                flecs_rule_add_var(rule, EcsThisName, vars, EcsVarEntity);
            }
        }

        if (flecs_rule_add_var_for_term_id(
            rule, second, vars, EcsVarEntity) == EcsVarNone)
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

        /* Track if a This entity variable is used before a potential This table 
         * variable. If this happens, the rule has no This table variable */
        if (src->id == EcsThis) {
            table_this = true;
        }
        if (first->id == EcsThis || second->id == EcsThis) {
            if (!table_this) {
                entity_before_table_this = true;
            }
        }
    }

    int32_t var_count = ecs_vec_count(vars);
    ecs_var_id_t placeholder = EcsVarNone - 1;
    bool replace_placeholders = false;

    /* Ensure lookup variables have table and/or entity variables */
    for (i = 0; i < var_count; i ++) {
        ecs_rule_var_t *var = ecs_vec_get_t(vars, ecs_rule_var_t, i);
        if (var->lookup) {
            char *var_name = ecs_os_strdup(var->name);
            var_name[var->lookup - var->name - 1] = '\0';

            ecs_var_id_t base_table_id = flecs_rule_find_var_id(
                rule, var_name, EcsVarTable);
            if (base_table_id != EcsVarNone) {
                var->table_id = base_table_id;
            } else if (anonymous_table_count) {
                /* Scan for implicit anonymous table variables that haven't been
                 * inserted yet (happens after this step). Doing this here vs.
                 * ensures that anonymous variables are appended at the end of
                 * the variable array, while also ensuring that variable ids are
                 * stable (no swapping of table var ids that are in use). */
                for (a = 0; a < var_count; a ++) {
                    ecs_rule_var_t *avar = ecs_vec_get_t(
                        vars, ecs_rule_var_t, a);
                    if (avar->kind == EcsVarAny) {
                        if (!ecs_os_strcmp(avar->name, var_name)) {
                            base_table_id = (ecs_var_id_t)(a + 1);
                            break;
                        }
                    }
                }
                if (base_table_id != EcsVarNone) {
                    /* Set marker so we can set the new table id afterwards */
                    var->table_id = placeholder;
                    replace_placeholders = true;
                }
            }

            ecs_var_id_t base_entity_id = flecs_rule_find_var_id(
                rule, var_name, EcsVarEntity);
            if (base_entity_id == EcsVarNone) {
                /* Get name from table var (must exist). We can't use allocated
                 * name since variables don't own names. */
                const char *base_name = NULL;
                if (base_table_id != EcsVarNone && base_table_id) {
                    ecs_rule_var_t *base_table_var = ecs_vec_get_t(
                        vars, ecs_rule_var_t, (int32_t)base_table_id - 1);
                    base_name = base_table_var->name;
                } else {
                    base_name = EcsThisName;
                }

                base_entity_id = flecs_rule_add_var(
                    rule, base_name, vars, EcsVarEntity);
                var = ecs_vec_get_t(vars, ecs_rule_var_t, i);
            }

            var->base_id = base_entity_id;

            ecs_os_free(var_name);
        }
    }
    var_count = ecs_vec_count(vars);

    /* Add non-This table variables */
    if (anonymous_table_count) {
        anonymous_table_count = 0;
        for (i = 0; i < var_count; i ++) {
            ecs_rule_var_t *var = ecs_vec_get_t(vars, ecs_rule_var_t, i);
            if (var->kind == EcsVarAny) {
                var->kind = EcsVarEntity;

                ecs_var_id_t var_id = flecs_rule_add_var(
                    rule, var->name, vars, EcsVarTable);
                ecs_vec_get_t(vars, ecs_rule_var_t, i)->table_id = var_id;
                anonymous_table_count ++;
            }
        }

        var_count = ecs_vec_count(vars);
    }

    /* If any forward references to newly added anonymous tables exist, replace
     * them with the actual table variable ids. */
    if (replace_placeholders) {
        for (i = 0; i < var_count; i ++) {
            ecs_rule_var_t *var = ecs_vec_get_t(vars, ecs_rule_var_t, i);
            if (var->table_id == placeholder) {
                char *var_name = ecs_os_strdup(var->name);
                var_name[var->lookup - var->name - 1] = '\0';

                var->table_id = flecs_rule_find_var_id(
                    rule, var_name, EcsVarTable);
                ecs_assert(var->table_id != EcsVarNone, 
                    ECS_INTERNAL_ERROR, NULL);

                ecs_os_free(var_name);
            }
        }
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
    rule->has_table_this = !entity_before_table_this;

#ifdef FLECS_DEBUG
    rule->var_size = var_count + anonymous_count;
#endif

    char **var_names = ECS_ELEM(rule_vars, ECS_SIZEOF(ecs_rule_var_t), 
        var_count + anonymous_count);
    rule->var_names = (char**)var_names;

    rule_vars[0].kind = EcsVarTable;
    rule_vars[0].name = NULL;
    flecs_set_var_label(&rule_vars[0], NULL);
    rule_vars[0].id = 0;
    rule_vars[0].table_id = EcsVarNone;
    rule_vars[0].lookup = NULL;
    var_names[0] = ECS_CONST_CAST(char*, rule_vars[0].name);
    rule_vars ++;
    var_names ++;
    var_count --;

    if (var_count) {
        ecs_rule_var_t *user_vars = ecs_vec_first_t(vars, ecs_rule_var_t);
        ecs_os_memcpy_n(rule_vars, user_vars, ecs_rule_var_t, var_count);
        for (i = 0; i < var_count; i ++) {
            var_names[i] = ECS_CONST_CAST(char*, rule_vars[i].name);
        }
    }

    /* Hide anonymous table variables from application */
    rule->var_pub_count -= anonymous_table_count;

    /* Sanity check to make sure that the public part of the variable array only
     * contains entity variables. */
#ifdef FLECS_DEBUG
    for (i = 1 /* first element = $this */; i < rule->var_pub_count; i ++) {
        ecs_assert(rule->vars[i].kind == EcsVarEntity, ECS_INTERNAL_ERROR, NULL);
    }
#endif

    return 0;
error:
    return -1;
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

    ecs_var_id_t evar = flecs_rule_find_var_id(rule, name, EcsVarEntity);
    if ((evar != EcsVarNone) && flecs_rule_is_written(evar, ctx->written)) {
        /* If entity variable is available and written to, it contains the most
         * specific result and should be used. */
        return evar;
    }

    ecs_var_id_t tvar = flecs_rule_find_var_id(rule, name, EcsVarTable);
    if ((tvar != EcsVarNone) && !flecs_rule_is_written(tvar, ctx->written)) {
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

static
ecs_rule_lbl_t flecs_rule_op_insert(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t *elem = ecs_vec_append_t(NULL, ctx->ops, ecs_rule_op_t);
    int32_t count = ecs_vec_count(ctx->ops);
    *elem = *op;
    if (count > 1) {
        if (ctx->cur->lbl_begin == -1) {
            /* Variables written by previous instruction can't be written by
             * this instruction, except when this is a union. */
            elem->written &= ~elem[-1].written;
        }
    }

    elem->next = flecs_itolbl(count);
    elem->prev = flecs_itolbl(count - 2);
    return flecs_itolbl(count - 1);
}

static
ecs_rule_op_t* flecs_rule_begin_block(
    ecs_rule_op_kind_t kind,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t op = {0};
    op.kind = flecs_ito(uint8_t, kind);
    ctx->cur->lbl_begin = flecs_rule_op_insert(&op, ctx);
    return ecs_vec_get_t(ctx->ops, ecs_rule_op_t, ctx->cur->lbl_begin);
}

static
void flecs_rule_end_block(
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t new_op = {0};
    new_op.kind = EcsRuleEnd;
    ecs_rule_lbl_t end = flecs_rule_op_insert(&new_op, ctx);
    
    ecs_rule_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_rule_op_t);
    ops[ctx->cur->lbl_begin].next = end;

    ecs_rule_op_t *end_op = &ops[end];
    if (ctx->cur->lbl_query != -1) {
        ecs_rule_op_t *query_op = &ops[ctx->cur->lbl_query];
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
void flecs_rule_begin_block_cond_eval(
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx,
    ecs_write_flags_t cond_write_state)
{
    ecs_var_id_t first_var = EcsVarNone, second_var = EcsVarNone, src_var = EcsVarNone;
    ecs_write_flags_t cond_mask = 0;

    if (flecs_rule_ref_flags(op->flags, EcsRuleFirst) == EcsRuleIsVar) {
        first_var = op->first.var;
        ecs_assert(first_var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        cond_mask |= (1ull << first_var);
    }
    if (flecs_rule_ref_flags(op->flags, EcsRuleSecond) == EcsRuleIsVar) {
        second_var = op->second.var;
        ecs_assert(second_var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        cond_mask |= (1ull << second_var);
    }
    if (flecs_rule_ref_flags(op->flags, EcsRuleSrc) == EcsRuleIsVar) {
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

        ecs_rule_op_t jmp_op = {0};
        jmp_op.kind = EcsRuleIf;

        if ((first_var != EcsVarNone) && cond_write_state & (1ull << first_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleFirst);
            jmp_op.first.var = first_var;
        }
        if ((second_var != EcsVarNone) && cond_write_state & (1ull << second_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleSecond);
            jmp_op.second.var = second_var;
        }
        if ((src_var != EcsVarNone) && cond_write_state & (1ull << src_var)) {
            jmp_op.flags |= (EcsRuleIsVar << EcsRuleSrc);
            jmp_op.src.var = src_var;
        }

        flecs_rule_op_insert(&jmp_op, ctx);
    } else {
        ctx->cur->lbl_cond_eval = -1;
    }
}

static
void flecs_rule_end_block_cond_eval(
    ecs_rule_compile_ctx_t *ctx)
{
    if (ctx->cur->lbl_cond_eval == -1) {
        return;
    }

    ecs_assert(ctx->cur->lbl_query >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_rule_op_t end_op = {0};
    end_op.kind = EcsRuleEnd;
    ecs_rule_lbl_t end = flecs_rule_op_insert(&end_op, ctx);

    ecs_rule_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_rule_op_t);
    ops[ctx->cur->lbl_cond_eval].next = end;

    ecs_rule_op_t *end_op_ptr = &ops[end];
    ecs_rule_op_t *query_op = &ops[ctx->cur->lbl_query];
    end_op_ptr->prev = ctx->cur->lbl_cond_eval;
    end_op_ptr->src = query_op->src;
    end_op_ptr->first = query_op->first;
    end_op_ptr->second = query_op->second;
    end_op_ptr->flags = query_op->flags;
    end_op_ptr->field_index = query_op->field_index;
}

static
void flecs_rule_begin_block_or(
    ecs_rule_op_t *op,
    ecs_term_t *term,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t *or_op = flecs_rule_begin_block(EcsRuleNot, ctx);
    or_op->kind = EcsRuleOr;

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
    if (term->first.id == EcsPredEq && term->second.flags & EcsIsVariable) {
        if (!(flecs_rule_is_written(op->src.var, ctx->written))) {
            add_src = false;
        }
    }

    if (add_src) {
        if (op->flags & (EcsRuleIsVar << EcsRuleSrc)) {
            or_op->flags = (EcsRuleIsVar << EcsRuleSrc);
            or_op->src = op->src;
        }
    }
}

static
void flecs_rule_end_block_or(
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t op = {0};
    op.kind = EcsRuleEnd;
    ecs_rule_lbl_t end = flecs_rule_op_insert(&op, ctx);
    
    ecs_rule_op_t *ops = ecs_vec_first_t(ctx->ops, ecs_rule_op_t);
    int32_t i, j, prev_or = -2;
    for (i = ctx->cur->lbl_begin + 1; i < end; i ++) {
        if (ops[i].next == FlecsRuleOrMarker) {
            if (prev_or != -2) {
                ops[prev_or].prev = flecs_itolbl(i);
            }
            ops[i].next = flecs_itolbl(end);
            prev_or = i;
        } else {
            /* Combine operation with next OR marker. This supports OR chains 
             * with terms that require multiple operations to test. */
            for (j = i + 1; j < end; j ++) {
                if (ops[j].next == FlecsRuleOrMarker) {
                    if (j == (end - 1)) {
                        ops[i].prev = ctx->cur->lbl_begin;
                    } else {
                        ops[i].prev = flecs_itolbl(j + 1);
                    }
                    break;
                }
            }
        }
    }

    ops[ctx->cur->lbl_begin].next = flecs_itolbl(end);
    ops[end].prev = ctx->cur->lbl_begin;
    ops[end - 1].prev = ctx->cur->lbl_begin;

    /* Scan which variables were conditionally written in the OR chain and 
     * reset instructions after the OR chain. If a variable is set in part one
     * of a chain but not part two, there would be nothing writing to the
     * variable in part two, leaving it to the previous value. To address this
     * a reset is inserted that resets the variable value on redo. */
    for (i = 1; i < (8 * ECS_SIZEOF(ecs_write_flags_t)); i ++) {
        ecs_write_flags_t prev = 1 & (ctx->ctrlflow->cond_written_or >> i);
        ecs_write_flags_t cur = 1 & (ctx->cond_written >> i);

        if (!prev && cur) {
            ecs_rule_op_t reset_op = {0};
            reset_op.kind = EcsRuleReset;
            reset_op.flags |= (EcsRuleIsVar << EcsRuleSrc);
            reset_op.src.var = flecs_itovar(i);
            flecs_rule_op_insert(&reset_op, ctx);
        }
    }

    ctx->ctrlflow->in_or = false;
    ctx->cur->lbl_begin = -1;
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
void flecs_rule_insert_lookup(
    ecs_var_id_t base_var,
    ecs_var_id_t evar,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    ecs_rule_op_t lookup = {0};
    lookup.kind = EcsRuleLookup;
    lookup.src.var = evar;
    lookup.first.var = base_var;
    lookup.flags = (EcsRuleIsVar << EcsRuleSrc) | 
                 (EcsRuleIsVar << EcsRuleFirst);
    flecs_rule_write_ctx(evar, ctx, cond_write);
    flecs_rule_write(evar, &lookup.written);
    flecs_rule_op_insert(&lookup, ctx);
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
    flecs_set_var_label(&rule->vars[tgt], rule->vars[op->second.var].name);

    /* First, find ids to start traversal from. This fixes op.second. */
    ecs_rule_op_t find_ids = {0};
    find_ids.kind = EcsRuleIdsRight;
    find_ids.field_index = -1;
    find_ids.first = op->first;
    find_ids.second = op->second;
    find_ids.flags = op->flags;
    find_ids.flags &= (ecs_flags8_t)~((EcsRuleIsVar|EcsRuleIsEntity) << EcsRuleSrc);
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
void flecs_rule_insert_inheritance(
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_op_t *op,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write)
{
    /* Anonymous variable to store the resolved component ids */
    ecs_var_id_t tvar = flecs_rule_add_var(rule, NULL, NULL, EcsVarTable);
    ecs_var_id_t evar = flecs_rule_add_var(rule, NULL, NULL, EcsVarEntity);
    flecs_set_var_label(&rule->vars[tvar], ecs_get_name(rule->filter.world, term->first.id));
    flecs_set_var_label(&rule->vars[evar], ecs_get_name(rule->filter.world, term->first.id));

    ecs_rule_op_t trav_op = {0};
    trav_op.kind = EcsRuleTrav;
    trav_op.field_index = -1;
    trav_op.first.entity = term->first.trav;
    trav_op.second.entity = term->first.id;
    trav_op.src.var = tvar;
    trav_op.flags = EcsRuleIsSelf;
    trav_op.flags |= (EcsRuleIsEntity << EcsRuleFirst);
    trav_op.flags |= (EcsRuleIsEntity << EcsRuleSecond);
    trav_op.flags |= (EcsRuleIsVar << EcsRuleSrc);
    trav_op.written |= (1ull << tvar);
    if (term->first.flags & EcsSelf) {
        trav_op.match_flags |= EcsTermReflexive;
    }
    flecs_rule_op_insert(&trav_op, ctx);
    flecs_rule_insert_each(tvar, evar, ctx, cond_write);

    ecs_rule_ref_t r = { .var = evar };
    op->first = r;
    op->flags &= (ecs_flags8_t)~(EcsRuleIsEntity << EcsRuleFirst);
    op->flags |= (EcsRuleIsVar << EcsRuleFirst);
}

static
void flecs_rule_compile_term_id(
    ecs_world_t *world,
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_term_id_t *term_id,
    ecs_rule_ref_t *ref,
    ecs_flags8_t ref_kind,
    ecs_var_kind_t kind,
    ecs_rule_compile_ctx_t *ctx,
    bool create_wildcard_vars)
{
    (void)world;

    if (!ecs_term_id_is_set(term_id)) {
        return;
    }

    if (term_id->flags & EcsIsVariable) {
        op->flags |= (ecs_flags8_t)(EcsRuleIsVar << ref_kind);
        const char *name = flecs_term_id_var_name(term_id);
        if (name) {
            ref->var = flecs_rule_most_specific_var(rule, name, kind, ctx);
            ecs_assert(ref->var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        } else if (create_wildcard_vars) {
            bool is_wildcard = flecs_term_id_is_wildcard(term_id);
            if (is_wildcard && (kind == EcsVarAny)) {
                ref->var = flecs_rule_add_var(rule, NULL, NULL, EcsVarTable);
            } else {
                ref->var = flecs_rule_add_var(rule, NULL, NULL, EcsVarEntity);
            }
            if (is_wildcard) {
                flecs_set_var_label(&rule->vars[ref->var], 
                    ecs_get_name(world, term_id->id));
            }
            ecs_assert(ref->var != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (term_id->flags & EcsIsEntity) {
        op->flags |= (ecs_flags8_t)(EcsRuleIsEntity << ref_kind);
        ref->entity = term_id->id;
    }
}

static
int flecs_rule_compile_ensure_vars(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_rule_ref_t *ref,
    ecs_flags16_t ref_kind,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write,
    bool *written_out)
{
    ecs_flags16_t flags = flecs_rule_ref_flags(op->flags, ref_kind);
    bool written = false;

    if (flags & EcsRuleIsVar) {
        ecs_var_id_t var_id = ref->var;
        ecs_rule_var_t *var = &rule->vars[var_id];

        if (var->kind == EcsVarEntity && 
            !flecs_rule_is_written(var_id, ctx->written)) 
        {
            /* If entity variable is not yet written but a table variant exists
             * that has been written, insert each operation to translate from
             * entity variable to table */
            ecs_var_id_t tvar = var->table_id;
            if ((tvar != EcsVarNone) && 
                flecs_rule_is_written(tvar, ctx->written)) 
            {
                if (var->lookup) {
                    if (!flecs_rule_is_written(tvar, ctx->written)) {
                        ecs_err("dependent variable of '$%s' is not written", 
                            var->name);
                        return -1;
                    }

                    if (!flecs_rule_is_written(var->base_id, ctx->written)) {
                        flecs_rule_insert_each(
                            tvar, var->base_id, ctx, cond_write);
                    }
                } else {
                    flecs_rule_insert_each(tvar, var_id, ctx, cond_write);
                }

                /* Variable was written, just not as entity */
                written = true;
            } else if (var->lookup) {
                if (!flecs_rule_is_written(var->base_id, ctx->written)) {
                    ecs_err("dependent variable of '$%s' is not written", 
                        var->name);
                    return -1;
                }
            }
        }

        written |= flecs_rule_is_written(var_id, ctx->written);
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
bool flecs_rule_compile_lookup(
    ecs_rule_t *rule,
    ecs_var_id_t var_id,
    ecs_rule_compile_ctx_t *ctx,
    bool cond_write) 
{
    ecs_rule_var_t *var = &rule->vars[var_id];
    if (var->lookup) {
        flecs_rule_insert_lookup(var->base_id, var_id, ctx, cond_write);
        return true;
    } else {
        return false;
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
        contains.flags |= (EcsRuleIsVar << EcsRuleSrc) | 
                          (EcsRuleIsVar << EcsRuleFirst);
        flecs_rule_op_insert(&contains, ctx);
    }
}

static
void flecs_rule_insert_pair_eq(
    int32_t field_index,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_op_t contains = {0};
    contains.kind = EcsRulePairEq;
    contains.field_index = flecs_ito(int8_t, field_index);
    flecs_rule_op_insert(&contains, ctx);
}

static
bool flecs_rule_term_fixed_id(
    ecs_filter_t *filter,
    ecs_term_t *term)
{
    /* Transitive/inherited terms have variable ids */
    if (term->flags & (EcsTermTransitive|EcsTermIdInherited)) {
        return false;
    }

    /* Or terms can match different ids */
    if (term->oper == EcsOr) {
        return false;
    }
    if ((term != filter->terms) && term[-1].oper == EcsOr) {
        return false;
    }

    /* Wildcards can assume different ids */
    if (ecs_id_is_wildcard(term->id)) {
        return false;
    }

    /* Any terms can have fixed ids, but they require special handling */
    if (term->flags & (EcsTermMatchAny|EcsTermMatchAnySrc)) {
        return false;
    }

    /* First terms that are Not or Optional require special handling */
    if (term->oper == EcsNot || term->oper == EcsOptional) {
        if (term == filter->terms) {
            return false;
        }
    }

    return true;
}

static
int flecs_rule_compile_builtin_pred(
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_op_t *op,
    ecs_write_flags_t write_state)
{
    ecs_entity_t id = term->first.id;

    ecs_rule_op_kind_t eq[] = {EcsRulePredEq, EcsRulePredNeq};
    ecs_rule_op_kind_t eq_name[] = {EcsRulePredEqName, EcsRulePredNeqName};
    ecs_rule_op_kind_t eq_match[] = {EcsRulePredEqMatch, EcsRulePredNeqMatch};
    
    ecs_flags16_t flags_2nd = flecs_rule_ref_flags(op->flags, EcsRuleSecond);

    if (id == EcsPredEq) {
        if (term->second.flags & EcsIsName) {
            op->kind = flecs_ito(uint8_t, eq_name[term->oper == EcsNot]);
        } else {
            op->kind = flecs_ito(uint8_t, eq[term->oper == EcsNot]);
        }
    } else if (id == EcsPredMatch) {
        op->kind = flecs_ito(uint8_t, eq_match[term->oper == EcsNot]);
    }

    op->first = op->src;
    op->src = (ecs_rule_ref_t){0};
    op->flags &= (ecs_flags8_t)~((EcsRuleIsEntity|EcsRuleIsVar) << EcsRuleSrc);
    op->flags &= (ecs_flags8_t)~((EcsRuleIsEntity|EcsRuleIsVar) << EcsRuleFirst);
    op->flags |= EcsRuleIsVar << EcsRuleFirst;

    if (flags_2nd & EcsRuleIsVar) {
        if (!(write_state & (1ull << op->second.var))) {
            ecs_err("uninitialized variable '%s' on right-hand side of "
                "equality operator", 
                    ecs_rule_var_name(rule, op->second.var));
            return -1;
        }
    }

    if (!(write_state & (1ull << op->first.var))) {
        /* If this is an == operator with a right-hand side that resolves to a
         * single entity, the left-hand side is allowed to be undefined, as the
         * instruction will be evaluated as an assignment. */
        if (op->kind != EcsRulePredEq && op->kind != EcsRulePredEqName) {
            ecs_err("uninitialized variable '%s' on left-hand side of "
                "equality operator", 
                    ecs_rule_var_name(rule, op->first.var));
            return -1;
        }
    }

    return 0;
}

static
int flecs_rule_ensure_scope_var(
    ecs_rule_t *rule,
    ecs_rule_op_t *op,
    ecs_rule_ref_t *ref,
    ecs_flags16_t ref_kind,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_var_id_t var = ref->var;

    if (rule->vars[var].kind == EcsVarEntity && 
        !flecs_rule_is_written(var, ctx->written)) 
    {
        ecs_var_id_t table_var = rule->vars[var].table_id;
        if (table_var != EcsVarNone && 
            flecs_rule_is_written(table_var, ctx->written)) 
        {
            if (flecs_rule_compile_ensure_vars(
                rule, op, ref, ref_kind, ctx, false, NULL))
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
int flecs_rule_ensure_scope_vars(
    ecs_world_t *world,
    ecs_rule_t *rule,
    ecs_rule_compile_ctx_t *ctx,
    ecs_term_t *term)
{
    /* If the scope uses variables as entity that have only been written as
     * table, resolve them as entities before entering the scope. */
    ecs_term_t *cur = term;
    while(cur->first.id != EcsScopeClose) {
        /* Dummy operation to obtain variable information for term */
        ecs_rule_op_t op = {0};
        flecs_rule_compile_term_id(world, rule, &op, &cur->first, 
            &op.first, EcsRuleFirst, EcsVarEntity, ctx, false);
        flecs_rule_compile_term_id(world, rule, &op, &cur->second, 
            &op.second, EcsRuleSecond, EcsVarEntity, ctx, false);

        if (op.flags & (EcsRuleIsVar << EcsRuleFirst)) {
            if (flecs_rule_ensure_scope_var(
                rule, &op, &op.first, EcsRuleFirst, ctx))
            {
                goto error;
            }
        }
        if (op.flags & (EcsRuleIsVar << EcsRuleSecond)) {
            if (flecs_rule_ensure_scope_var(
                rule, &op, &op.second, EcsRuleSecond, ctx))
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
void flecs_rule_compile_push(
    ecs_rule_compile_ctx_t *ctx)
{
    ctx->cur = &ctx->ctrlflow[++ ctx->scope];
    ctx->cur->lbl_begin = -1;
    ctx->cur->lbl_begin = -1;
}

static
void flecs_rule_compile_pop(
    ecs_rule_compile_ctx_t *ctx)
{
    ctx->cur = &ctx->ctrlflow[-- ctx->scope];
}

static
bool flecs_rule_term_is_or(
    const ecs_filter_t *filter,
    const ecs_term_t *term)
{
    bool first_term = term == filter->terms;
    return (term->oper == EcsOr) || (!first_term && term[-1].oper == EcsOr);
}

static
int flecs_rule_compile_term(
    ecs_world_t *world,
    ecs_rule_t *rule,
    ecs_term_t *term,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_filter_t *filter = &rule->filter;
    bool first_term = term == filter->terms;
    bool first_is_var = term->first.flags & EcsIsVariable;
    bool second_is_var = term->second.flags & EcsIsVariable;
    bool src_is_var = term->src.flags & EcsIsVariable;
    bool builtin_pred = flecs_rule_is_builtin_pred(term);
    bool is_not = (term->oper == EcsNot) && !builtin_pred;
    bool is_or = flecs_rule_term_is_or(filter, term);
    bool first_or = false, last_or = false;
    bool cond_write = term->oper == EcsOptional || is_or;
    ecs_rule_op_t op = {0};

    if (is_or) {
        first_or = first_term || (term[-1].oper != EcsOr);
        last_or = term->oper != EcsOr;
    }

    /* !_ (don't match anything) terms always return nothing. */
    if (is_not && term->id == EcsAny) {
        op.kind = EcsRuleNothing;
        flecs_rule_op_insert(&op, ctx);
        return 0;
    }

    if (is_or && (first_term || term[-1].oper != EcsOr)) {
        ctx->ctrlflow->cond_written_or = ctx->cond_written;
        ctx->ctrlflow->in_or = true;
    }

    if (!term->src.id && term->src.flags & EcsIsEntity) {
        /* If the term has a 0 source, check if it's a scope open/close */
        if (term->first.id == EcsScopeOpen) {
            if (flecs_rule_ensure_scope_vars(world, rule, ctx, term)) {
                goto error;
            }
            if (term->oper == EcsNot) {
                ctx->scope_is_not |= (ecs_flags32_t)(1ull << ctx->scope);
                flecs_rule_begin_block(EcsRuleNot, ctx);
            } else {
                ctx->scope_is_not &= (ecs_flags32_t)~(1ull << ctx->scope);
            }
            flecs_rule_compile_push(ctx);
        } else if (term->first.id == EcsScopeClose) {
            flecs_rule_compile_pop(ctx);
            if (ctx->scope_is_not & (ecs_flags32_t)(1ull << (ctx->scope))) {
                ctx->cur->lbl_query = -1;
                flecs_rule_end_block(ctx);
            }
        } else {
            /* Noop */
        }
        return 0;
    }

    if (builtin_pred) {
        if (term->second.id == EcsWildcard || term->second.id == EcsAny) {
            /* Noop */
            return 0;
        }
    }

    /* Default instruction for And operators. If the source is fixed (like for
     * singletons or terms with an entity source), use With, which like And but
     * just matches against a source (vs. finding a source). */
    op.kind = src_is_var ? EcsRuleAnd : EcsRuleWith;
    op.field_index = flecs_ito(int8_t, term->field_index);
    op.term_index = flecs_ito(int8_t, term - filter->terms);

    /* If rule is transitive, use Trav(ersal) instruction */
    if (term->flags & EcsTermTransitive) {
        ecs_assert(ecs_term_id_is_set(&term->second), ECS_INTERNAL_ERROR, NULL);
        op.kind = EcsRuleTrav;
    } else {
        /* Ignore cascade & parent flags */
        ecs_flags32_t trav_flags = EcsTraverseFlags & ~(EcsCascade|EcsParent);
        if (term->flags & (EcsTermMatchAny|EcsTermMatchAnySrc)) {
            op.kind = EcsRuleAndAny;
        } else if ((term->src.flags & trav_flags) == EcsUp) {
            op.kind = EcsRuleUp;
        } else if ((term->src.flags & trav_flags) == (EcsSelf|EcsUp)) {
            op.kind = EcsRuleSelfUp;
        }
    }

    /* If term has fixed id, insert simpler instruction that skips dealing with
     * wildcard terms and variables */
    if (flecs_rule_term_fixed_id(filter, term)) {
        if (op.kind == EcsRuleAnd) {
            op.kind = EcsRuleAndId;
        } else if (op.kind == EcsRuleSelfUp) {
            op.kind = EcsRuleSelfUpId;
        } else if (op.kind == EcsRuleUp) {
            op.kind = EcsRuleUpId;
        }
    }

    /* Save write state at start of term so we can use it to reliably track
     * variables got written by this term. */
    ecs_write_flags_t cond_write_state = ctx->cond_written;

    /* Resolve component inheritance if necessary */
    ecs_var_id_t first_var = EcsVarNone, second_var = EcsVarNone, src_var = EcsVarNone;

    /* Resolve variables and entities for operation arguments */
    flecs_rule_compile_term_id(world, rule, &op, &term->first, 
        &op.first, EcsRuleFirst, EcsVarEntity, ctx, true);
    flecs_rule_compile_term_id(world, rule, &op, &term->second, 
        &op.second, EcsRuleSecond, EcsVarEntity, ctx, true);

    if (first_is_var) first_var = op.first.var;
    if (second_is_var) second_var = op.second.var;

    flecs_rule_compile_term_id(world, rule, &op, &term->src, 
        &op.src, EcsRuleSrc, EcsVarAny, ctx, true);
    if (src_is_var) src_var = op.src.var;
    bool src_written = flecs_rule_is_written(src_var, ctx->written);

    /* Insert each instructions for table -> entity variable if needed */
    bool first_written, second_written;
    if (flecs_rule_compile_ensure_vars(
        rule, &op, &op.first, EcsRuleFirst, ctx, cond_write, &first_written)) 
    {
        goto error;    
    }
    if (flecs_rule_compile_ensure_vars(
        rule, &op, &op.second, EcsRuleSecond, ctx, cond_write, &second_written))
    {
        goto error;
    }

    /* If the query starts with a Not or Optional term, insert an operation that
     * matches all entities. */
    if (first_term && src_is_var && !src_written && term->src.id != EcsAny) {
        bool pred_match = builtin_pred && term->first.id == EcsPredMatch;
        if (term->oper == EcsNot || term->oper == EcsOptional || pred_match) {
            ecs_rule_op_t match_any = {0};
            match_any.kind = EcsAnd;
            match_any.flags = EcsRuleIsSelf | (EcsRuleIsEntity << EcsRuleFirst);
            match_any.flags |= (EcsRuleIsVar << EcsRuleSrc);
            match_any.src = op.src;
            match_any.field_index = -1;
            if (!pred_match) {
                match_any.first.entity = EcsAny;
            } else {
                /* If matching by name, instead of finding all tables, just find
                 * the ones with a name. */
                match_any.first.entity = ecs_id(EcsIdentifier);
                match_any.second.entity = EcsName;
                match_any.flags |= (EcsRuleIsEntity << EcsRuleSecond);
            }
            match_any.written = (1ull << src_var);
            flecs_rule_op_insert(&match_any, ctx);
            flecs_rule_write_ctx(op.src.var, ctx, false);

            /* Update write administration */
            src_written = true;
        }
    }

    /* A bit of special logic for OR expressions and equality predicates. If the
     * left-hand of an equality operator is a table, and there are multiple
     * operators in an Or expression, the Or chain should match all entities in
     * the table that match the right hand sides of the operator expressions. 
     * For this to work, the src variable needs to be resolved as entity, as an
     * Or chain would otherwise only yield the first match from a table. */
    if (src_is_var && src_written && builtin_pred && term->oper == EcsOr) {
        /* Or terms are required to have the same source, so we don't have to
         * worry about the last term in the chain. */
        if (rule->vars[src_var].kind == EcsVarTable) {
            flecs_rule_compile_term_id(world, rule, &op, &term->src, 
                    &op.src, EcsRuleSrc, EcsVarEntity, ctx, true);
            src_var = op.src.var;
        }
    }

    if (flecs_rule_compile_ensure_vars(
        rule, &op, &op.src, EcsRuleSrc, ctx, cond_write, NULL)) 
    {
        goto error;
    }

    /* If source is Any (_) and first and/or second are unconstrained, insert an
     * ids instruction instead of an And */
    if (term->flags & EcsTermMatchAnySrc) {
        op.kind = EcsRuleIds;
        /* Use up-to-date written values after potentially inserting each */
        if (!first_written || !second_written) {
            if (!first_written) {
                /* If first is unknown, traverse left: <- (*, t) */
                if (term->first.id != EcsAny) {
                    op.kind = EcsRuleIdsLeft;
                }
            } else {
                /* If second is wildcard, traverse right: (r, *) -> */
                if (term->second.id != EcsAny) {
                    op.kind = EcsRuleIdsRight;
                }
            }
            op.src.entity = 0;
            op.flags &= (ecs_flags8_t)~(EcsRuleIsVar << EcsRuleSrc); /* ids has no src */
            op.flags &= (ecs_flags8_t)~(EcsRuleIsEntity << EcsRuleSrc);
        }

    /* If source variable is not written and we're querying just for Any, insert
     * a dedicated instruction that uses the Any record in the id index. Any 
     * queries that are evaluated against written sources can use Wildcard 
     * records, which is what the AndAny instruction does. */
    } else if (!src_written && term->id == EcsAny && op.kind == EcsRuleAndAny) {
        /* Lookup variables ($var.child_name) are always written */
        if (!rule->vars[src_var].lookup) {
            op.kind = EcsRuleSelectAny; /* Uses Any (_) id record */
        }
    }

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

    /* Check if this term has variables that have been conditionally written,
     * like variables written by an optional term. */
    if (ctx->cond_written) {
        if (!is_or || first_or) {
            flecs_rule_begin_block_cond_eval(&op, ctx, cond_write_state);
        }
    }

    /* Handle Not, Optional, Or operators */
    if (is_not) {
        flecs_rule_begin_block(EcsRuleNot, ctx);
    } else if (term->oper == EcsOptional) {
        flecs_rule_begin_block(EcsRuleOptional, ctx);
    } else if (first_or) {
        flecs_rule_begin_block_or(&op, term, ctx);
    }

    /* If term has component inheritance enabled, insert instruction to walk
     * down the relationship tree of the id. */
    if (term->flags & EcsTermIdInherited) {
        flecs_rule_insert_inheritance(rule, term, &op, ctx, cond_write);
    }

    op.match_flags = term->flags;

    if (first_is_var) {
        op.first.var = first_var;
        op.flags &= (ecs_flags8_t)~(EcsRuleIsEntity << EcsRuleFirst);
        op.flags |= (EcsRuleIsVar << EcsRuleFirst);
    }

    if (term->src.flags & EcsSelf) {
        op.flags |= EcsRuleIsSelf;
    }

    /* Insert instructions for lookup variables */
    ecs_write_flags_t write_state = ctx->written;
    if (first_is_var) {
        if (flecs_rule_compile_lookup(rule, first_var, ctx, cond_write)) {
            write_state |= (1ull << first_var); // lookups are resolved inline
        }
    }
    if (src_is_var) {
        if (flecs_rule_compile_lookup(rule, src_var, ctx, cond_write)) {
            write_state |= (1ull << src_var); // lookups are resolved inline
        }
    }
    if (second_is_var) {
        if (flecs_rule_compile_lookup(rule, second_var, ctx, cond_write)) {
            write_state |= (1ull << second_var); // lookups are resolved inline
        }
    }

    if (builtin_pred) {
        if (flecs_rule_compile_builtin_pred(rule, term, &op, write_state)) {
            goto error;
        }
    }

    /* If we're writing the $this variable, filter out disabled/prefab entities
     * unless the filter explicitly matches them.
     * This could've been done with regular With instructions, but since 
     * filtering out disabled/prefab entities is the default and this check is
     * cheap to perform on table flags, it's worth special casing. */
    if (!src_written && src_var == 0) {
        ecs_flags32_t filter_flags = filter->flags;
        if (!(filter_flags & EcsFilterMatchDisabled) || 
            !(filter_flags & EcsFilterMatchPrefab)) 
        {
            ecs_flags32_t table_flags = 0;
            if (!(filter_flags & EcsFilterMatchDisabled)) {
                table_flags |= EcsTableIsDisabled;
            }
            if (!(filter_flags & EcsFilterMatchPrefab)) {
                table_flags |= EcsTableIsPrefab;
            }

            op.other = flecs_itolbl(table_flags);
        }
    }

    /* After evaluating a term, a used variable is always written */
    if (src_is_var) {
        flecs_rule_write(src_var, &op.written);
        flecs_rule_write_ctx(op.src.var, ctx, cond_write);
    }
    if (first_is_var) {
        flecs_rule_write(first_var, &op.written);
        flecs_rule_write_ctx(first_var, ctx, cond_write);
    }
    if (second_is_var) {
        flecs_rule_write(second_var, &op.written);
        flecs_rule_write_ctx(second_var, ctx, cond_write);
    }
    flecs_rule_op_insert(&op, ctx);

    ctx->cur->lbl_query = flecs_itolbl(ecs_vec_count(ctx->ops) - 1);
    if (is_or) {
        ecs_rule_op_t *op_ptr = ecs_vec_get_t(ctx->ops, ecs_rule_op_t, 
            ctx->cur->lbl_query);
        op_ptr->next = FlecsRuleOrMarker;
    }

    /* Handle self-references between src and first/second variables */
    if (src_is_var) {
        if (first_is_var) {
            flecs_rule_insert_contains(rule, src_var, first_var, ctx);
        }
        if (second_is_var && first_var != second_var) {
            flecs_rule_insert_contains(rule, src_var, second_var, ctx);
        }
    }

    /* Handle self references between first and second variables */
    if (first_is_var && !first_written && (first_var == second_var)) {
        flecs_rule_insert_pair_eq(term->field_index, ctx);
    }

    /* Handle closing of Not, Optional and Or operators */
    if (is_not) {
        flecs_rule_end_block(ctx);
    } else if (term->oper == EcsOptional) {
        flecs_rule_end_block(ctx);
    } else if (last_or) {
        flecs_rule_end_block_or(ctx);
    }

    /* Handle closing of conditional evaluation */
    if (ctx->cur->lbl_cond_eval && (first_is_var || second_is_var || src_is_var)) {
        if (!is_or || last_or) {
            flecs_rule_end_block_cond_eval(ctx);
        }
    }

    /* Ensure that term id is set after evaluating Not */
    if (term->flags & EcsTermIdInherited) {
        if (is_not) {
            ecs_rule_op_t set_id = {0};
            set_id.kind = EcsRuleSetId;
            set_id.first.entity = term->id;
            set_id.flags = (EcsRuleIsEntity << EcsRuleFirst);
            set_id.field_index = flecs_ito(int8_t, term->field_index);
            flecs_rule_op_insert(&set_id, ctx);
        }
    }

    return 0;
error:
    return -1;
}

static
bool flecs_rule_var_is_unknown(
    ecs_rule_t *rule,
    ecs_var_id_t var_id,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_rule_var_t *vars = rule->vars;
    if (ctx->written & (1ull << var_id)) {
        return false;
    } else {
        ecs_var_id_t table_var = vars[var_id].table_id;
        if (table_var != EcsVarNone) {
            return flecs_rule_var_is_unknown(rule, table_var, ctx);
        }
    }
    return true;
}

/* Returns whether term is unkown. A term is unknown when it has variable 
 * elements (first, second, src) that are all unknown. */
static
bool flecs_rule_term_is_unknown(
    ecs_rule_t *rule, 
    ecs_term_t *term, 
    ecs_rule_compile_ctx_t *ctx) 
{
    ecs_rule_op_t dummy = {0};
    flecs_rule_compile_term_id(NULL, rule, &dummy, &term->first, 
        &dummy.first, EcsRuleFirst, EcsVarEntity, ctx, false);
    flecs_rule_compile_term_id(NULL, rule, &dummy, &term->second, 
        &dummy.second, EcsRuleSecond, EcsVarEntity, ctx, false);
    flecs_rule_compile_term_id(NULL, rule, &dummy, &term->src, 
        &dummy.src, EcsRuleSrc, EcsVarAny, ctx, false);

    bool has_vars = dummy.flags & 
        ((EcsRuleIsVar << EcsRuleFirst) |
         (EcsRuleIsVar << EcsRuleSecond) |
         (EcsRuleIsVar << EcsRuleSrc));
    if (!has_vars) {
        /* If term has no variables (typically terms with a static src) there
         * can't be anything that's unknown. */
        return false;
    }

    if (dummy.flags & (EcsRuleIsVar << EcsRuleFirst)) {
        if (!flecs_rule_var_is_unknown(rule, dummy.first.var, ctx)) {
            return false;
        }
    }
    if (dummy.flags & (EcsRuleIsVar << EcsRuleSecond)) {
        if (!flecs_rule_var_is_unknown(rule, dummy.second.var, ctx)) {
            return false;
        }
    }
    if (dummy.flags & (EcsRuleIsVar << EcsRuleSrc)) {
        if (!flecs_rule_var_is_unknown(rule, dummy.src.var, ctx)) {
            return false;
        }
    }

    return true;
}

/* Find the next known term from specified offset. This function is used to find
 * a term that can be evaluated before a term that is unknown. Evaluating known
 * before unknown terms can significantly decrease the search space. */
static
int32_t flecs_rule_term_next_known(
    ecs_rule_t *rule, 
    ecs_rule_compile_ctx_t *ctx,
    int32_t offset,
    ecs_flags64_t compiled) 
{
    ecs_filter_t *filter = &rule->filter;
    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    for (i = offset; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (compiled & (1ull << i)) {
            continue;
        }

        /* Only evaluate And terms */
        if (term->oper != EcsAnd || flecs_rule_term_is_or(&rule->filter, term)){
            continue;
        }

        /* Don't reorder terms before/after scopes */
        if (term->first.id == EcsScopeOpen || term->first.id == EcsScopeClose) {
            return -1;
        }

        if (flecs_rule_term_is_unknown(rule, term, ctx)) {
            continue;
        }

        return i;
    }

    return -1;
}

/* If the first part of a query contains more than one trivial term, insert a
 * special instruction which batch-evaluates multiple terms. */
static
int32_t flecs_rule_insert_trivial_search(
    ecs_rule_t *rule,
    ecs_rule_compile_ctx_t *ctx)
{
    ecs_filter_t *filter = &rule->filter;
    ecs_term_t *terms = filter->terms;
    int32_t i, term_count = filter->term_count;

    /* Find trivial terms, which can be handled in single instruction */
    int32_t trivial_wildcard_terms = 0;
    int32_t trivial_data_terms = 0;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (!(term->flags & EcsTermIsTrivial)) {
            break;
        }

        /* We can only add trivial terms to plan if they no up traversal */
        if ((term->src.flags & EcsTraverseFlags) != EcsSelf) {
            break;
        }

        if (ecs_id_is_wildcard(term->id)) {
            trivial_wildcard_terms ++;
        }

        if (!(term->flags & EcsTermNoData)) {
            trivial_data_terms ++;
        }
    }

    int32_t trivial_terms = i;
    if (trivial_terms >= 2) {
        /* If there's more than 1 trivial term, batch them in trivial search */
        ecs_rule_op_t trivial = {0};
        if (trivial_wildcard_terms) {
            trivial.kind = EcsRuleTrivWildcard;
        } else {
            if (trivial_data_terms) {
                /* Check to see if there are remaining data terms. If there are,
                 * we'll have to insert an instruction later that populates all
                 * fields, so don't do double work here. */
                for (i = trivial_terms; i < term_count; i ++) {
                    ecs_term_t *term = &terms[i];
                    if (!(term->flags & EcsTermIsTrivial)) {
                        break;
                    }
                }
                if (trivial_terms == term_count || i != term_count) {
                    /* Nobody else is going to set the data fields, so we should
                     * do it here. */
                    trivial.kind = EcsRuleTrivData;
                }
            }
            if (!trivial.kind) {
                trivial.kind = EcsRuleTriv;
            }
        }

        /* Store on the operation how many trivial terms should be evaluated */
        trivial.other = (ecs_rule_lbl_t)trivial_terms;
        flecs_rule_op_insert(&trivial, ctx);
    } else {
        /* If fewer than 1 trivial term, there's no point in batching them */
        trivial_terms = 0;
    }

    return trivial_terms;
}

/* Insert instruction to populate data fields. */
static
void flecs_rule_insert_populate(
    ecs_rule_t *rule,
    ecs_rule_compile_ctx_t *ctx,
    int32_t trivial_terms)
{
    ecs_filter_t *filter = &rule->filter;
    int32_t i, term_count = filter->term_count;

    /* Insert instruction that populates data. This instruction does not
     * have to be inserted if the filter provides no data, or if all terms
     * of the filter are trivial, in which case the trivial search operation
     * also sets the data. */
    if (!(filter->flags & EcsFilterNoData) && (trivial_terms != term_count)) {
        int32_t data_fields = 0;
        bool only_self = true;

        /* There are two instructions for setting data fields, a fast one 
         * that only supports owned fields, and one that supports any kind
         * of field. Loop through (remaining) terms to check which one we
         * need to use. */
        for (i = trivial_terms; i < term_count; i ++) {
            ecs_term_t *term = &filter->terms[i];
            if (term->flags & EcsTermNoData) {
                /* Don't care about terms that have no data */
                continue;
            }

            data_fields ++;

            if (!ecs_term_match_this(term)) {
                break;
            }

            if (term->src.flags & EcsUp) {
                break;
            }
        }

        if (i != filter->term_count) {
            only_self = false; /* Needs the more complex operation */
        }

        if (data_fields) {
            if (only_self) {
                ecs_rule_op_t nothing = {0};
                nothing.kind = EcsRulePopulateSelf;
                flecs_rule_op_insert(&nothing, ctx);
            } else {
                ecs_rule_op_t nothing = {0};
                nothing.kind = EcsRulePopulate;
                flecs_rule_op_insert(&nothing, ctx);
            }
        }
    }
}

int flecs_rule_compile(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_rule_t *rule)
{
    ecs_filter_t *filter = &rule->filter;
    ecs_term_t *terms = filter->terms;
    ecs_rule_compile_ctx_t ctx = {0};
    ecs_vec_reset_t(NULL, &stage->operations, ecs_rule_op_t);
    ctx.ops = &stage->operations;
    ctx.cur = ctx.ctrlflow;
    ctx.cur->lbl_begin = -1;
    ctx.cur->lbl_begin = -1;
    ecs_vec_clear(ctx.ops);

    /* Find all variables defined in query */
    if (flecs_rule_discover_vars(stage, rule)) {
        return -1;
    }

    /* If rule contains fixed source terms, insert operation to set sources */
    int32_t i, term_count = filter->term_count;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->src.flags & EcsIsEntity) {
            ecs_rule_op_t set_fixed = {0};
            set_fixed.kind = EcsRuleSetFixed;
            flecs_rule_op_insert(&set_fixed, &ctx);
            break;
        }
    }

    /* If the rule contains terms with fixed ids (no wildcards, variables), 
     * insert instruction that initializes ecs_iter_t::ids. This allows for the
     * insertion of simpler instructions later on. */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (flecs_rule_term_fixed_id(filter, term) || 
           (term->src.flags & EcsIsEntity && !term->src.id)) 
        {
            ecs_rule_op_t set_ids = {0};
            set_ids.kind = EcsRuleSetIds;
            flecs_rule_op_insert(&set_ids, &ctx);
            break;
        }
    }

    /* Insert trivial term search if query allows for it */
    int32_t trivial_terms = flecs_rule_insert_trivial_search(rule, &ctx);

    /* Compile remaining query terms to instructions */
    ecs_flags64_t compiled = 0;
    for (i = trivial_terms; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        int32_t compile = i;

        if (compiled & (1ull << i)) {
            continue; /* Already compiled */
        }

        bool can_reorder = true;
        if (term->oper != EcsAnd || flecs_rule_term_is_or(&rule->filter, term)){
            can_reorder = false;
        }

        /* If variables have been written, but this term has no known variables,
         * first try to resolve terms that have known variables. This can 
         * significantly reduce the search space. 
         * Only perform this optimization after at least one variable has been
         * written to, as all terms are unknown otherwise. */
        if (can_reorder && ctx.written && 
            flecs_rule_term_is_unknown(rule, term, &ctx)) 
        {
            int32_t term_index = flecs_rule_term_next_known(
                rule, &ctx, i + 1, compiled);
            if (term_index != -1) {
                term = &rule->filter.terms[term_index];
                compile = term_index;
                i --; /* Repeat current term */
            }
        }

        if (flecs_rule_compile_term(world, rule, term, &ctx)) {
            return -1;
        }

        compiled |= (1ull << compile);
    }

    ecs_var_id_t this_id = flecs_rule_find_var_id(rule, "This", EcsVarEntity);

    /* If This variable has been written as entity, insert an operation to 
     * assign it to it.entities for consistency. */
    if (this_id != EcsVarNone && (ctx.written & (1ull << this_id))) {
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
                    /* Skip anonymous variables */
                    if (!flecs_rule_var_is_anonymous(rule, var_id)) {
                        flecs_rule_insert_each(var->id, var_id, &ctx, false);
                    }
                }
            }
        }
    }

    /* If rule contains non-This variables as term source, build lookup array */
    if (rule->src_vars) {
        ecs_assert(rule->vars != NULL, ECS_INTERNAL_ERROR, NULL);
        bool only_anonymous = true;

        for (i = 0; i < filter->field_count; i ++) {
            ecs_var_id_t var_id = rule->src_vars[i];
            if (!var_id) {
                continue;
            }

            if (!flecs_rule_var_is_anonymous(rule, var_id)) {
                only_anonymous = false;
                break;
            } else {
                /* Don't fetch component data for anonymous variables. Because
                 * not all metadata (such as it.sources) is initialized for
                 * anonymous variables, and because they may only be available
                 * as table variables (each is not guaranteed to be inserted for
                 * anonymous variables) the iterator may not have sufficient
                 * information to resolve component data. */
                for (int32_t t = 0; t < filter->term_count; t ++) {
                    ecs_term_t *term = &filter->terms[t];
                    if (term->field_index == i) {
                        term->inout = EcsInOutNone;
                    }
                }
            }
        }

        /* Don't insert setvar instruction if all vars are anonymous */
        if (!only_anonymous) {
            ecs_rule_op_t set_vars = {0};
            set_vars.kind = EcsRuleSetVars;
            flecs_rule_op_insert(&set_vars, &ctx);
        }

        for (i = 0; i < filter->field_count; i ++) {
            ecs_var_id_t var_id = rule->src_vars[i];
            if (!var_id) {
                continue;
            }

            if (rule->vars[var_id].kind == EcsVarTable) {
                var_id = flecs_rule_find_var_id(rule, rule->vars[var_id].name,
                    EcsVarEntity);

                /* Variables used as source that aren't This must be entities */
                ecs_assert(var_id != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
            }

            rule->src_vars[i] = var_id;
        }
    }

    /* If filter is empty, insert Nothing instruction */
    if (!term_count) {
        ecs_rule_op_t nothing = {0};
        nothing.kind = EcsRuleNothing;
        flecs_rule_op_insert(&nothing, &ctx);
    } else {
        /* Insert instruction to populate data fields */
        flecs_rule_insert_populate(rule, &ctx, trivial_terms);

        /* Insert yield. If program reaches this operation, a result was found */
        ecs_rule_op_t yield = {0};
        yield.kind = EcsRuleYield;
        flecs_rule_op_insert(&yield, &ctx);
    }

    int32_t op_count = ecs_vec_count(ctx.ops);
    if (op_count) {
        rule->op_count = op_count;
        rule->ops = ecs_os_malloc_n(ecs_rule_op_t, op_count);
        ecs_rule_op_t *rule_ops = ecs_vec_first_t(ctx.ops, ecs_rule_op_t);
        ecs_os_memcpy_n(rule->ops, rule_ops, ecs_rule_op_t, op_count);
    }

    return 0;
}

#endif
