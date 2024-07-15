/**
 * @file query/compiler/compile.c
 * @brief Compile query program from query.
 */

#include "../../private_api.h"

static
bool flecs_query_var_is_anonymous(
    const ecs_query_impl_t *query,
    ecs_var_id_t var_id)
{
    ecs_query_var_t *var = &query->vars[var_id];
    return var->anonymous;
}

ecs_var_id_t flecs_query_add_var(
    ecs_query_impl_t *query,
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
            var_id = flecs_query_find_var_id(query, name, EcsVarEntity);
            if (var_id != EcsVarNone) {
                return var_id;
            }

            var_id = flecs_query_find_var_id(query, name, EcsVarTable);
            if (var_id != EcsVarNone) {
                return var_id;
            }

            kind = EcsVarTable;
        } else {
            var_id = flecs_query_find_var_id(query, name, kind);
            if (var_id != EcsVarNone) {
                return var_id;
            }
        }

        if (kind == EcsVarTable) {
            var_index = &query->tvar_index;
        } else {
            var_index = &query->evar_index;
        }

        /* If we're creating an entity var, check if it has a table variant */
        if (kind == EcsVarEntity && var_id == EcsVarNone) {
            var_id = flecs_query_find_var_id(query, name, EcsVarTable);
        }
    }

    ecs_query_var_t *var;
    ecs_var_id_t result;
    if (vars) {
        var = ecs_vec_append_t(NULL, vars, ecs_query_var_t);
        result = var->id = flecs_itovar(ecs_vec_count(vars));
    } else {
        ecs_dbg_assert(query->var_count < query->var_size, 
            ECS_INTERNAL_ERROR, NULL);
        var = &query->vars[query->var_count];
        result = var->id = flecs_itovar(query->var_count);
        query->var_count ++;
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
ecs_var_id_t flecs_query_add_var_for_term_id(
    ecs_query_impl_t *query,
    ecs_term_ref_t *term_id,
    ecs_vec_t *vars,
    ecs_var_kind_t kind)
{
    const char *name = flecs_term_ref_var_name(term_id);
    if (!name) {
        return EcsVarNone;
    }

    return flecs_query_add_var(query, name, vars, kind);
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
int flecs_query_discover_vars(
    ecs_stage_t *stage,
    ecs_query_impl_t *query)
{
    ecs_vec_t *vars = &stage->variables; /* Buffer to reduce allocs */
    ecs_vec_reset_t(NULL, vars, ecs_query_var_t);

    ecs_term_t *terms = query->pub.terms;
    int32_t a, i, anonymous_count = 0, count = query->pub.term_count;
    int32_t anonymous_table_count = 0, scope = 0, scoped_var_index = 0;
    bool table_this = false, entity_before_table_this = false;

    /* For This table lookups during discovery. This will be overwritten after
     * discovery with whether the query actually has a This table variable. */
    query->pub.flags |= EcsQueryHasTableThisVar;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_ref_t *first = &term->first;
        ecs_term_ref_t *second = &term->second;
        ecs_term_ref_t *src = &term->src;

        if (ECS_TERM_REF_ID(first) == EcsScopeOpen) {
            /* Keep track of which variables are first used in scope, so that we
             * can mark them as anonymous. Terms inside a scope are collapsed 
             * into a single result, which means that outside of the scope the
             * value of those variables is undefined. */
            if (!scope) {
                scoped_var_index = ecs_vec_count(vars);
            }
            scope ++;
            continue;
        } else if (ECS_TERM_REF_ID(first) == EcsScopeClose) {
            if (!--scope) {
                /* Any new variables declared after entering a scope should be
                 * marked as anonymous. */
                int32_t v;
                for (v = scoped_var_index; v < ecs_vec_count(vars); v ++) {
                    ecs_vec_get_t(vars, ecs_query_var_t, v)->anonymous = true;
                }
            }
            continue;
        }

        ecs_var_id_t first_var_id = flecs_query_add_var_for_term_id(
            query, first, vars, EcsVarEntity);
        if (first_var_id == EcsVarNone) {
            /* If first is not a variable, check if we need to insert anonymous
             * variable for resolving component inheritance */
            if (term->flags_ & EcsTermIdInherited) {
                anonymous_count += 2; /* table & entity variable */
            }

            /* If first is a wildcard, insert anonymous variable */
            if (flecs_term_ref_is_wildcard(first)) {
                anonymous_count ++;
            }
        }

        if ((src->id & EcsIsVariable) && (ECS_TERM_REF_ID(src) != EcsThis)) {
            const char *var_name = flecs_term_ref_var_name(src);
            if (var_name) {
                ecs_var_id_t var_id = flecs_query_find_var_id(
                    query, var_name, EcsVarEntity);
                if (var_id == EcsVarNone || var_id == first_var_id) {
                    var_id = flecs_query_add_var(
                        query, var_name, vars, EcsVarEntity);
                }

                if (var_id != EcsVarNone) {
                    /* Mark variable as one for which we need to create a table
                     * variable. Don't create table variable now, so that we can
                     * store it in the non-public part of the variable array. */
                    ecs_query_var_t *var = ecs_vec_get_t(
                        vars, ecs_query_var_t, (int32_t)var_id - 1);
                    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
                    if (!var->lookup) {
                        var->kind = EcsVarAny;
                        anonymous_table_count ++;
                    }

                    if (!(term->flags_ & EcsTermNoData)) {
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
                    if (!query->src_vars) {
                        query->src_vars = flecs_calloc_n(&stage->allocator,
                            ecs_var_id_t, query->pub.field_count);
                    }

                    query->src_vars[term->field_index] = var_id;
                }
            } else {
                if (flecs_term_ref_is_wildcard(src)) {
                    anonymous_count ++;
                }
            }
        } else if ((src->id & EcsIsVariable) && (ECS_TERM_REF_ID(src) == EcsThis)) {
            if (flecs_term_is_builtin_pred(term) && term->oper == EcsOr) {
                flecs_query_add_var(query, EcsThisName, vars, EcsVarEntity);
            }

            if (term->flags_ & EcsTermIsSparse) {
                /* If this is a sparse $this term entities have to be returned
                 * one by one. */
                flecs_query_add_var(query, EcsThisName, vars, EcsVarEntity);

                /* Track if query contains $this sparse terms. Queries with 
                 * sparse $this fields need to return results one by one. */
                if ((ECS_TERM_REF_ID(&term->src) == EcsThis) && 
                    ((term->src.id & (EcsSelf|EcsIsVariable)) == (EcsSelf|EcsIsVariable)))
                {
                    query->pub.flags |= EcsQueryHasSparseThis;
                }
            }
        }

        if (flecs_query_add_var_for_term_id(
            query, second, vars, EcsVarEntity) == EcsVarNone)
        {
            /* If second is a wildcard, insert anonymous variable */
            if (flecs_term_ref_is_wildcard(second)) {
                anonymous_count ++;
            }
        }

        if (src->id & EcsIsVariable && second->id & EcsIsVariable) {
            if (term->flags_ & EcsTermTransitive) {
                /* Anonymous variable to store temporary id for finding 
                 * targets for transitive relationship, see compile_term. */
                anonymous_count ++;
            }
        }

        /* If member term, make sure source is available as entity */
        if (term->flags_ & EcsTermIsMember) {
            flecs_query_add_var_for_term_id(query, src, vars, EcsVarEntity);
        }

        /* Track if a This entity variable is used before a potential This table 
         * variable. If this happens, the query has no This table variable */
        if (ECS_TERM_REF_ID(src) == EcsThis) {
            table_this = true;
        }

        if (ECS_TERM_REF_ID(first) == EcsThis || ECS_TERM_REF_ID(second) == EcsThis) {
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
        ecs_query_var_t *var = ecs_vec_get_t(vars, ecs_query_var_t, i);
        if (var->lookup) {
            char *var_name = ecs_os_strdup(var->name);
            var_name[var->lookup - var->name - 1] = '\0';

            ecs_var_id_t base_table_id = flecs_query_find_var_id(
                query, var_name, EcsVarTable);
            if (base_table_id != EcsVarNone) {
                var->table_id = base_table_id;
            } else if (anonymous_table_count) {
                /* Scan for implicit anonymous table variables that haven't been
                 * inserted yet (happens after this step). Doing this here vs.
                 * ensures that anonymous variables are appended at the end of
                 * the variable array, while also ensuring that variable ids are
                 * stable (no swapping of table var ids that are in use). */
                for (a = 0; a < var_count; a ++) {
                    ecs_query_var_t *avar = ecs_vec_get_t(
                        vars, ecs_query_var_t, a);
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

            ecs_var_id_t base_entity_id = flecs_query_find_var_id(
                query, var_name, EcsVarEntity);
            if (base_entity_id == EcsVarNone) {
                /* Get name from table var (must exist). We can't use allocated
                 * name since variables don't own names. */
                const char *base_name = NULL;
                if (base_table_id != EcsVarNone && base_table_id) {
                    ecs_query_var_t *base_table_var = ecs_vec_get_t(
                        vars, ecs_query_var_t, (int32_t)base_table_id - 1);
                    base_name = base_table_var->name;
                } else {
                    base_name = EcsThisName;
                }

                base_entity_id = flecs_query_add_var(
                    query, base_name, vars, EcsVarEntity);
                var = ecs_vec_get_t(vars, ecs_query_var_t, i);
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
            ecs_query_var_t *var = ecs_vec_get_t(vars, ecs_query_var_t, i);
            if (var->kind == EcsVarAny) {
                var->kind = EcsVarEntity;

                ecs_var_id_t var_id = flecs_query_add_var(
                    query, var->name, vars, EcsVarTable);
                ecs_vec_get_t(vars, ecs_query_var_t, i)->table_id = var_id;
                anonymous_table_count ++;
            }
        }

        var_count = ecs_vec_count(vars);
    }

    /* If any forward references to newly added anonymous tables exist, replace
     * them with the actual table variable ids. */
    if (replace_placeholders) {
        for (i = 0; i < var_count; i ++) {
            ecs_query_var_t *var = ecs_vec_get_t(vars, ecs_query_var_t, i);
            if (var->table_id == placeholder) {
                char *var_name = ecs_os_strdup(var->name);
                var_name[var->lookup - var->name - 1] = '\0';

                var->table_id = flecs_query_find_var_id(
                    query, var_name, EcsVarTable);
                ecs_assert(var->table_id != EcsVarNone, 
                    ECS_INTERNAL_ERROR, NULL);

                ecs_os_free(var_name);
            }
        }
    }

    /* Always include spot for This variable, even if query doesn't use it */
    var_count ++;

    ecs_query_var_t *query_vars = &query->vars_cache.var;
    if ((var_count + anonymous_count) > 1) {
        query_vars = flecs_alloc(&stage->allocator,
            (ECS_SIZEOF(ecs_query_var_t) + ECS_SIZEOF(char*)) * 
                (var_count + anonymous_count));
    }

    query->vars = query_vars;
    query->var_count = var_count;
    query->pub.var_count = flecs_ito(int16_t, var_count);
    ECS_BIT_COND(query->pub.flags, EcsQueryHasTableThisVar, 
        !entity_before_table_this);
    query->var_size = var_count + anonymous_count;

    char **var_names = ECS_ELEM(query_vars, ECS_SIZEOF(ecs_query_var_t), 
        var_count + anonymous_count);
    query->pub.vars = (char**)var_names;

    query_vars[0].kind = EcsVarTable;
    query_vars[0].name = NULL;
    flecs_set_var_label(&query_vars[0], NULL);
    query_vars[0].id = 0;
    query_vars[0].table_id = EcsVarNone;
    query_vars[0].lookup = NULL;
    var_names[0] = ECS_CONST_CAST(char*, query_vars[0].name);
    query_vars ++;
    var_names ++;
    var_count --;

    if (var_count) {
        ecs_query_var_t *user_vars = ecs_vec_first_t(vars, ecs_query_var_t);
        ecs_os_memcpy_n(query_vars, user_vars, ecs_query_var_t, var_count);
        for (i = 0; i < var_count; i ++) {
            var_names[i] = ECS_CONST_CAST(char*, query_vars[i].name);
        }
    }

    /* Hide anonymous table variables from application */
    query->pub.var_count = 
        flecs_ito(int16_t, query->pub.var_count - anonymous_table_count);

    /* Sanity check to make sure that the public part of the variable array only
     * contains entity variables. */
#ifdef FLECS_DEBUG
    for (i = 1 /* first element = $this */; i < query->pub.var_count; i ++) {
        ecs_assert(query->vars[i].kind == EcsVarEntity, ECS_INTERNAL_ERROR, NULL);
    }
#endif

    return 0;
error:
    return -1;
}

static
bool flecs_query_var_is_unknown(
    ecs_query_impl_t *query,
    ecs_var_id_t var_id,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_var_t *vars = query->vars;
    if (ctx->written & (1ull << var_id)) {
        return false;
    } else {
        ecs_var_id_t table_var = vars[var_id].table_id;
        if (table_var != EcsVarNone) {
            return flecs_query_var_is_unknown(query, table_var, ctx);
        }
    }
    return true;
}

/* Returns whether term is unknown. A term is unknown when it has variable
 * elements (first, second, src) that are all unknown. */
static
bool flecs_query_term_is_unknown(
    ecs_query_impl_t *query, 
    ecs_term_t *term, 
    ecs_query_compile_ctx_t *ctx) 
{
    ecs_query_op_t dummy = {0};
    flecs_query_compile_term_ref(NULL, query, &dummy, &term->first, 
        &dummy.first, EcsQueryFirst, EcsVarEntity, ctx, false);
    flecs_query_compile_term_ref(NULL, query, &dummy, &term->second, 
        &dummy.second, EcsQuerySecond, EcsVarEntity, ctx, false);
    flecs_query_compile_term_ref(NULL, query, &dummy, &term->src, 
        &dummy.src, EcsQuerySrc, EcsVarAny, ctx, false);

    bool has_vars = dummy.flags & 
        ((EcsQueryIsVar << EcsQueryFirst) |
         (EcsQueryIsVar << EcsQuerySecond) |
         (EcsQueryIsVar << EcsQuerySrc));
    if (!has_vars) {
        /* If term has no variables (typically terms with a static src) there
         * can't be anything that's unknown. */
        return false;
    }

    if (dummy.flags & (EcsQueryIsVar << EcsQueryFirst)) {
        if (!flecs_query_var_is_unknown(query, dummy.first.var, ctx)) {
            return false;
        }
    }
    if (dummy.flags & (EcsQueryIsVar << EcsQuerySecond)) {
        if (!flecs_query_var_is_unknown(query, dummy.second.var, ctx)) {
            return false;
        }
    }
    if (dummy.flags & (EcsQueryIsVar << EcsQuerySrc)) {
        if (!flecs_query_var_is_unknown(query, dummy.src.var, ctx)) {
            return false;
        }
    }

    return true;
}

/* Find the next known term from specified offset. This function is used to find
 * a term that can be evaluated before a term that is unknown. Evaluating known
 * before unknown terms can significantly decrease the search space. */
static
int32_t flecs_query_term_next_known(
    ecs_query_impl_t *query, 
    ecs_query_compile_ctx_t *ctx,
    int32_t offset,
    ecs_flags64_t compiled) 
{
    ecs_query_t *q = &query->pub;
    ecs_term_t *terms = q->terms;
    int32_t i, count = q->term_count;

    for (i = offset; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        if (compiled & (1ull << i)) {
            continue;
        }

        /* Only evaluate And terms */
        if (term->oper != EcsAnd || flecs_term_is_or(q, term)){
            continue;
        }

        /* Don't reorder terms in scopes */
        if (term->flags_ & EcsTermIsScope) {
            continue;
        }

        if (flecs_query_term_is_unknown(query, term, ctx)) {
            continue;
        }

        return i;
    }

    return -1;
}

/* If the first part of a query contains more than one trivial term, insert a
 * special instruction which batch-evaluates multiple terms. */
static
void flecs_query_insert_trivial_search(
    ecs_query_impl_t *query,
    ecs_flags64_t *compiled,
    ecs_flags64_t *populated,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_t *q = &query->pub;
    ecs_term_t *terms = q->terms;
    int32_t i, term_count = q->term_count;
    ecs_flags64_t trivial_set = 0;

    /* Trivial search always ignores prefabs and disabled entities */
    if (query->pub.flags & (EcsQueryMatchPrefab|EcsQueryMatchDisabled)) {
        return;
    }

    /* Find trivial terms, which can be handled in single instruction */
    int32_t trivial_wildcard_terms = 0;
    int32_t trivial_data_terms = 0;
    int32_t trivial_terms = 0;
    bool populate = true;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->flags_ & (EcsTermIsToggle|EcsTermIsMember|EcsTermIsSparse)) {
            /* If query returns individual entities, let dedicated populate
             * instruction handle populating data fields */
            populate = false;
            break;
        }
    }

    for (i = 0; i < term_count; i ++) {
        /* Term is already compiled */
        if (*compiled & (1ull << i)) {
            continue;
        }

        ecs_term_t *term = &terms[i];
        if (!(term->flags_ & EcsTermIsTrivial)) {
            continue;
        }

        /* We can only add trivial terms to plan if they no up traversal */
        if ((term->src.id & EcsTraverseFlags) != EcsSelf) {
            continue;
        }

        trivial_set |= (1llu << i);

        if (ecs_id_is_wildcard(term->id)) {
            trivial_wildcard_terms ++;
        }

        if (populate) {
            if (q->data_fields & (1llu << term->field_index)) {
                trivial_data_terms ++;
            }
        }

        trivial_terms ++;
    }

    if (trivial_terms >= 2) {
        /* Mark terms as compiled & populated */
        for (i = 0; i < q->term_count; i ++) {
            if (trivial_set & (1llu << i)) {
                *compiled |= (1ull << i);
                if (populate) {
                    *populated |= (1ull << terms[i].field_index);
                }
            }
        }

        /* If there's more than 1 trivial term, batch them in trivial search */
        ecs_query_op_t trivial = {0};
        if (trivial_wildcard_terms) {
            trivial.kind = EcsQueryTrivWildcard;
        } else {
            if (trivial_data_terms) {
                trivial.kind = EcsQueryTrivData;
            }
            if (!trivial.kind) {
                trivial.kind = EcsQueryTriv;
            }
        }

        /* Store the bitset with trivial terms on the instruction */
        trivial.src.entity = trivial_set;
        flecs_query_op_insert(&trivial, ctx);

        /* Mark $this as written */
        ctx->written |= (1llu << 0);
    }
}

static
void flecs_query_insert_cache_search(
    ecs_query_impl_t *query,
    ecs_flags64_t *compiled,
    ecs_flags64_t *populated,
    ecs_query_compile_ctx_t *ctx)
{
    if (!query->cache) {
        return;
    }

    ecs_query_t *q = &query->pub;
    bool populate = true;

    int32_t populate_count = 0;
    if (q->cache_kind == EcsQueryCacheAll) {
        /* If all terms are cacheable, make sure no other terms are compiled */
        *compiled = 0xFFFFFFFFFFFFFFFF;

        /* If query has a sparse $this field, fields can't be populated by the cache
        * instruction. The reason for this is that sparse $this fields have to be
        * returned one at a time. This means that the same needs to happen for
        * cached fields, and the cache instruction only returns entire arrays. */
        if (!(query->pub.flags & EcsQueryHasSparseThis)) {
            *populated = 0xFFFFFFFFFFFFFFFF;
            populate_count = query->pub.field_count;
        }
    } else if (q->cache_kind == EcsQueryCacheAuto) {
        /* The query is partially cacheable */
        ecs_term_t *terms = q->terms;
        int32_t i, count = q->term_count;

        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            if (term->flags_ & (EcsTermIsToggle|EcsTermIsMember|EcsTermIsSparse)) {
                /* If query returns individual entities, let dedicated populate
                 * instruction handle populating data fields */
                populate = false;
                break;
            }
        }

        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            int16_t field = term->field_index;
            if ((*compiled) & (1ull << i)) {
                continue;
            }

            if (!(term->flags_ & EcsTermIsCacheable)) {
                continue;
            }

            *compiled |= (1ull << i);

            if (populate) {
                *populated |= (1ull << field);
                populate_count ++; 
            }
        }
    }

    /* Insert the operation for cache traversal */
    ecs_query_op_t op = {0};
    if (!populate_count || (q->flags & EcsQueryNoData)) {
        if (q->flags & EcsQueryIsCacheable) {
            op.kind = EcsQueryIsCache;
        } else {
            op.kind = EcsQueryCache;
        }
    } else {
        if (q->flags & EcsQueryIsCacheable) {
            op.kind = EcsQueryIsCacheData;
        } else {
            op.kind = EcsQueryCacheData;
        }
    }

    flecs_query_write(0, &op.written);
    flecs_query_write_ctx(0, ctx, false);
    flecs_query_op_insert(&op, ctx);
}

static
bool flecs_term_ref_match_multiple(
    ecs_term_ref_t *ref)
{
    return (ref->id & EcsIsVariable) && (ECS_TERM_REF_ID(ref) != EcsAny);
}

static
bool flecs_term_match_multiple(
    ecs_term_t *term)
{
    return flecs_term_ref_match_multiple(&term->first) ||
        flecs_term_ref_match_multiple(&term->second);
}

/* Insert instruction to populate data fields. */
void flecs_query_insert_populate(
    ecs_query_impl_t *query,
    ecs_query_compile_ctx_t *ctx,
    ecs_flags64_t populated)
{
    ecs_query_t *q = &query->pub;
    int32_t i, term_count = q->term_count;

    if (populated && !(q->flags & EcsQueryNoData)) {
        ecs_flags64_t sparse_fields = 0;
        int32_t populate_count = 0;
        int32_t self_count = 0;

        /* Figure out which populate instruction to use */
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &q->terms[i];
            int16_t field = term->field_index;
            ecs_flags64_t field_bit = (1ull << field);

            if (!(populated & field_bit)) {
                /* Only check fields that need to be populated */
                continue;
            }

            populate_count ++;

            /* Callee is asking us to populate a term without data */
            if (ecs_term_match_this(term) && !(term->src.id & EcsUp)) {
                self_count ++;
            }

            /* Track sparse fields as they require a separate instruction */
            if (term->flags_ & EcsTermIsSparse) {
                sparse_fields |= field_bit;
            }
        }

        ecs_assert(populate_count != 0, ECS_INTERNAL_ERROR, NULL);

        /* Mask out the sparse fields as they are populated separately. */
        populated &= ~sparse_fields;

        if (populated) {
            ecs_query_op_kind_t kind = EcsQueryPopulate;
            if (populate_count == self_count) {
                kind = EcsQueryPopulateSelf;
            }

            ecs_query_op_t op = {0};
            op.kind = flecs_ito(uint8_t, kind);
            op.src.entity = populated; /* Abuse for bitset w/fields to populate */
            flecs_query_op_insert(&op, ctx);
        }

        if (sparse_fields) {
            ecs_query_op_t op = {0};
            op.kind = EcsQueryPopulateSparse;
            op.src.entity = sparse_fields; /* Abuse for bitset w/fields to populate */
            flecs_query_op_insert(&op, ctx);
        }
    }
}

static
int flecs_query_insert_toggle(
    ecs_query_impl_t *impl,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_t *q = &impl->pub;
    int32_t i, j, term_count = q->term_count;
    ecs_term_t *terms = q->terms;
    ecs_flags64_t fields_done = 0;

    for (i = 0; i < term_count; i ++) {
        if (fields_done & (1llu << i)) {
            continue;
        }

        ecs_term_t *term = &terms[i];
        if (term->flags_ & EcsTermIsToggle) {
            ecs_query_op_t cur = {0};
            flecs_query_compile_term_ref(NULL, impl, &cur, &term->src, 
                &cur.src, EcsQuerySrc, EcsVarAny, ctx, false);

            ecs_flags64_t and_toggles = 0;
            ecs_flags64_t not_toggles = 0;
            ecs_flags64_t optional_toggles = 0;

            for (j = i; j < term_count; j ++) {
                if (fields_done & (1llu << j)) {
                    continue;
                }

                /* Also includes term[i], so flags get set correctly */
                term = &terms[j];

                /* If term is not for the same src, skip */
                ecs_query_op_t next = {0};
                flecs_query_compile_term_ref(NULL, impl, &next, &term->src,
                    &next.src, EcsQuerySrc, EcsVarAny, ctx, false);
                if (next.src.entity != cur.src.entity || 
                    next.flags != cur.flags) 
                {
                    continue;
                }

                /* Source matches, set flag */
                if (term->oper == EcsNot) {
                    not_toggles |= (1llu << j);
                } else if (term->oper == EcsOptional) {
                    optional_toggles |= (1llu << j);
                } else {
                    and_toggles |= (1llu << j);
                }

                fields_done |= (1llu << j);
            }

            if (and_toggles || not_toggles) {
                ecs_query_op_t op = {0};
                op.kind = EcsQueryToggle;
                op.src = cur.src;
                op.flags = cur.flags;

                if (op.flags & (EcsQueryIsVar << EcsQuerySrc)) {
                    flecs_query_write(op.src.var, &op.written);
                }

                /* Encode fields:
                 * - first.entity is the fields that match enabled bits
                 * - second.entity is the fields that match disabled bits
                 */
                op.first.entity = and_toggles;
                op.second.entity = not_toggles;
                flecs_query_op_insert(&op, ctx);
            }

            /* Insert separate instructions for optional terms. To make sure
             * entities are returned in batches where fields are never partially 
             * set or unset, the result must be split up into batches that have 
             * the exact same toggle masks. Instead of complicating the toggle 
             * instruction with code to scan for blocks that have the same bits 
             * set, separate instructions let the query engine backtrack to get 
             * the right results. */
            if (optional_toggles) {
                for (j = i; j < term_count; j ++) {
                    uint64_t field_bit = 1ull << j;
                    if (!(optional_toggles & field_bit)) {
                        continue;
                    }

                    ecs_query_op_t op = {0};
                    op.kind = EcsQueryToggleOption;
                    op.src = cur.src;
                    op.first.entity = field_bit;
                    op.flags = cur.flags;
                    flecs_query_op_insert(&op, ctx);
                }
            }
        }
    }

    return 0;
}

static
int flecs_query_insert_fixed_src_terms(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_flags64_t *compiled,
    ecs_flags64_t *populated_out,
    ecs_query_compile_ctx_t *ctx)
{
    ecs_query_t *q = &impl->pub;
    int32_t i, term_count = q->term_count;
    ecs_term_t *terms = q->terms;
    ecs_flags64_t populated = 0;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (term->oper ==  EcsNot) {
            /* If term has not operator and variables for first/second, we can't
             * put the term first as this could prevent us from getting back
             * valid results. For example:
             *   !$var(e), Tag($var)
             * 
             * Here, the first term would evaluate to false (and cause the 
             * entire query not to match) if 'e' has any components.
             * 
             * However, when reordering we get results:
             *   Tag($var), !$var(e)
             * 
             * Now the query returns all entities with Tag, that 'e' does not
             * have as component. For this reason, queries should never use
             * unwritten variables in not terms- and we should also not reorder
             * terms in a way that results in doing this. */
            if (flecs_term_match_multiple(term)) {
                continue;
            }
        }

        /* Don't reorder terms in scopes */
        if (term->flags_ & EcsTermIsScope) {
            continue;
        }

        if (term->src.id & EcsIsEntity && ECS_TERM_REF_ID(&term->src)) {
            if (flecs_query_compile_term(world, impl, term, populated_out, ctx)) {
                return -1;
            }

            *compiled |= (1llu << i);

            /* If this is a data field, track it. This will let us insert an
             * instruction specifically for populating data fields of terms with
             * fixed source (see below). */
            if (q->data_fields & (1llu << term->field_index)) {
                populated |= (1llu << term->field_index);
            }
        }
    }

    if (populated) {
        /* If data fields with a fixed source were evaluated, insert a populate
         * instruction that just populates those fields. The advantage of doing
         * this before the rest of the query is evaluated, is that we only 
         * populate data for static fields once vs. for each returned result of
         * the query, which would be wasteful since this data doesn't change. */
        flecs_query_insert_populate(impl, ctx, populated);
    }

    *populated_out |= populated;

    return 0;
}

int flecs_query_compile(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_impl_t *query)
{
    ecs_query_t *q = &query->pub;
    ecs_term_t *terms = q->terms;
    ecs_query_compile_ctx_t ctx = {0};
    ecs_vec_reset_t(NULL, &stage->operations, ecs_query_op_t);
    ctx.ops = &stage->operations;
    ctx.cur = ctx.ctrlflow;
    ctx.cur->lbl_begin = -1;
    ctx.cur->lbl_begin = -1;
    ecs_vec_clear(ctx.ops);

    /* Find all variables defined in query */
    if (flecs_query_discover_vars(stage, query)) {
        return -1;
    }

    /* If query contains fixed source terms, insert operation to set sources */
    int32_t i, term_count = q->term_count;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->src.id & EcsIsEntity) {
            ecs_query_op_t set_fixed = {0};
            set_fixed.kind = EcsQuerySetFixed;
            flecs_query_op_insert(&set_fixed, &ctx);
            break;
        }
    }

    /* If the query contains terms with fixed ids (no wildcards, variables), 
     * insert instruction that initializes ecs_iter_t::ids. This allows for the
     * insertion of simpler instructions later on. 
     * If the query is entirely cacheable, ids are populated by the cache. */
    if (q->cache_kind != EcsQueryCacheAll) {
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];
            if (flecs_term_is_fixed_id(q, term) || 
                (term->src.id & EcsIsEntity && 
                    !(term->src.id & ~EcsTermRefFlags))) 
            {
                ecs_query_op_t set_ids = {0};
                set_ids.kind = EcsQuerySetIds;
                flecs_query_op_insert(&set_ids, &ctx);
                break;
            }
        }
    }

    ecs_flags64_t compiled = 0;
    ecs_flags64_t populated = 0;

    /* Always evaluate terms with fixed source before other terms */
    flecs_query_insert_fixed_src_terms(
        world, query, &compiled, &populated, &ctx);

    /* Compile cacheable terms */
    flecs_query_insert_cache_search(
        query, &compiled, &populated, &ctx);

    /* Insert trivial term search if query allows for it */
    flecs_query_insert_trivial_search(
        query, &compiled, &populated, &ctx);

    /* If a query starts with one or more optional terms, first compile the non
     * optional terms. This prevents having to insert an instruction that 
     * matches the query against every entity in the storage. 
     * Only skip optional terms at the start of the query so that any 
     * short-circuiting behavior isn't affected (a non-optional term can become
     * optional if it uses a variable set in an optional term). */
    int32_t start_term = 0;
    for (; start_term < term_count; start_term ++) {
        if (terms[start_term].oper != EcsOptional) {
            break;
        }
    }

    do {
        /* Compile remaining query terms to instructions */
        for (i = start_term; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];
            int32_t compile = i;

            if (compiled & (1ull << i)) {
                continue; /* Already compiled */
            }

            if (term->oper == EcsOptional && start_term) {
                /* Don't reorder past the first optional term that's not in the
                 * initial list of optional terms. This protects short
                 * circuiting branching in the query. 
                 * A future algorithm could look at which variables are 
                 * accessed by optional terms, and continue reordering terms 
                 * that don't access those variables. */
                break;
            }

            bool can_reorder = true;
            if (term->oper != EcsAnd || flecs_term_is_or(q, term)){
                can_reorder = false;
            }

            /* If variables have been written, but this term has no known variables,
            * first try to resolve terms that have known variables. This can 
            * significantly reduce the search space. 
            * Only perform this optimization after at least one variable has been
            * written to, as all terms are unknown otherwise. */
            if (can_reorder && ctx.written && 
                flecs_query_term_is_unknown(query, term, &ctx)) 
            {
                int32_t term_index = flecs_query_term_next_known(
                    query, &ctx, i + 1, compiled);
                if (term_index != -1) {
                    term = &q->terms[term_index];
                    compile = term_index;
                    i --; /* Repeat current term */
                }
            }

            if (flecs_query_compile_term(world, query, term, &populated, &ctx)) {
                return -1;
            }

            compiled |= (1ull << compile);
        }

        if (start_term) {
            start_term = 0; /* Repeat, now also insert optional terms */
        } else {
            break;
        }
    } while (true);

    ecs_var_id_t this_id = flecs_query_find_var_id(query, "this", EcsVarEntity);
    if (this_id != EcsVarNone) {
        /* If query has a $this term with a sparse component value and so far
         * we've only accessed $this as table, we need to insert an each 
         * instruction. This is because we can't return multiple sparse 
         * component instances in a single query result. */
        if ((query->pub.flags & EcsQueryHasSparseThis) && 
            !(ctx.written & (1ull << this_id))) 
        {
            flecs_query_insert_each(0, this_id, &ctx, false);
        }

        /* If This variable has been written as entity, insert an operation to 
         * assign it to it.entities for consistency. */
        if (ctx.written & (1ull << this_id)) {
            ecs_query_op_t set_this = {0};
            set_this.kind = EcsQuerySetThis;
            set_this.flags |= (EcsQueryIsVar << EcsQueryFirst);
            set_this.first.var = this_id;
            flecs_query_op_insert(&set_this, &ctx);
        }
    }

    /* Make sure non-This variables are written as entities */
    if (query->vars) {
        for (i = 0; i < query->var_count; i ++) {
            ecs_query_var_t *var = &query->vars[i];
            if (var->id && var->kind == EcsVarTable && var->name) {
                ecs_var_id_t var_id = flecs_query_find_var_id(query, var->name,
                    EcsVarEntity);
                if (!flecs_query_is_written(var_id, ctx.written)) {
                    /* Skip anonymous variables */
                    if (!flecs_query_var_is_anonymous(query, var_id)) {
                        flecs_query_insert_each(var->id, var_id, &ctx, false);
                    }
                }
            }
        }
    }

    /* If query contains non-This variables as term source, build lookup array */
    if (query->src_vars) {
        ecs_assert(query->vars != NULL, ECS_INTERNAL_ERROR, NULL);
        bool only_anonymous = true;

        for (i = 0; i < q->field_count; i ++) {
            ecs_var_id_t var_id = query->src_vars[i];
            if (!var_id) {
                continue;
            }

            if (!flecs_query_var_is_anonymous(query, var_id)) {
                only_anonymous = false;
                break;
            } else {
                /* Don't fetch component data for anonymous variables. Because
                 * not all metadata (such as it.sources) is initialized for
                 * anonymous variables, and because they may only be available
                 * as table variables (each is not guaranteed to be inserted for
                 * anonymous variables) the iterator may not have sufficient
                 * information to resolve component data. */
                for (int32_t t = 0; t < q->term_count; t ++) {
                    ecs_term_t *term = &q->terms[t];
                    if (term->field_index == i) {
                        term->inout = EcsInOutNone;
                    }
                }
            }
        }

        /* Don't insert setvar instruction if all vars are anonymous */
        if (!only_anonymous) {
            ecs_query_op_t set_vars = {0};
            set_vars.kind = EcsQuerySetVars;
            flecs_query_op_insert(&set_vars, &ctx);
        }

        for (i = 0; i < q->field_count; i ++) {
            ecs_var_id_t var_id = query->src_vars[i];
            if (!var_id) {
                continue;
            }

            if (query->vars[var_id].kind == EcsVarTable) {
                var_id = flecs_query_find_var_id(query, query->vars[var_id].name,
                    EcsVarEntity);

                /* Variables used as source that aren't This must be entities */
                ecs_assert(var_id != EcsVarNone, ECS_INTERNAL_ERROR, NULL);
            }

            query->src_vars[i] = var_id;
        }
    }

    ecs_assert((term_count - ctx.skipped) >= 0, ECS_INTERNAL_ERROR, NULL);

    /* If query is empty, insert Nothing instruction */
    if (!(term_count - ctx.skipped)) {
        ecs_vec_clear(ctx.ops);
        ecs_query_op_t nothing = {0};
        nothing.kind = EcsQueryNothing;
        flecs_query_op_insert(&nothing, &ctx);
    } else {
        /* If query contains terms for toggleable components, insert toggle */
        if (!(q->flags & EcsQueryTableOnly)) {
            flecs_query_insert_toggle(query, &ctx);
        }

        /* Insert instruction to populate remaining data fields */
        ecs_flags64_t remaining = q->data_fields & ~populated;
        flecs_query_insert_populate(query, &ctx, remaining);

        /* Insert yield. If program reaches this operation, a result was found */
        ecs_query_op_t yield = {0};
        yield.kind = EcsQueryYield;
        flecs_query_op_insert(&yield, &ctx);
    }

    int32_t op_count = ecs_vec_count(ctx.ops);
    if (op_count) {
        query->op_count = op_count;
        query->ops = flecs_alloc_n(&stage->allocator, ecs_query_op_t, op_count);
        ecs_query_op_t *query_ops = ecs_vec_first_t(ctx.ops, ecs_query_op_t);
        ecs_os_memcpy_n(query->ops, query_ops, ecs_query_op_t, op_count);
    }

    return 0;
}
