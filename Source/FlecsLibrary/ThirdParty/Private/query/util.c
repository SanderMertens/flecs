 /**
 * @file query/util.c
 * @brief Query utilities.
 */

#include "../private_api.h"

ecs_query_lbl_t flecs_itolbl(int64_t val) {
    return flecs_ito(int16_t, val);
}

ecs_var_id_t flecs_itovar(int64_t val) {
    return flecs_ito(uint8_t, val);
}

ecs_var_id_t flecs_utovar(uint64_t val) {
    return flecs_uto(uint8_t, val);
}

bool flecs_term_is_builtin_pred(
    ecs_term_t *term)
{
    if (term->first.id & EcsIsEntity) {
        ecs_entity_t id = ECS_TERM_REF_ID(&term->first);
        if (id == EcsPredEq || id == EcsPredMatch || id == EcsPredLookup) {
            return true;
        }
    }
    return false;
}

const char* flecs_term_ref_var_name(
    ecs_term_ref_t *ref)
{
    if (!(ref->id & EcsIsVariable)) {
        return NULL;
    }

    if (ECS_TERM_REF_ID(ref) == EcsThis) {
        return EcsThisName;
    }

    return ref->name;
}

bool flecs_term_ref_is_wildcard(
    ecs_term_ref_t *ref)
{
    if ((ref->id & EcsIsVariable) && 
        ((ECS_TERM_REF_ID(ref) == EcsWildcard) || (ECS_TERM_REF_ID(ref) == EcsAny))) 
    {
        return true;
    }
    return false;
}

bool flecs_term_is_fixed_id(
    ecs_query_t *q,
    ecs_term_t *term)
{
    /* Transitive/inherited terms have variable ids */
    if (term->flags_ & (EcsTermTransitive|EcsTermIdInherited)) {
        return false;
    }

    /* Or terms can match different ids */
    if (term->oper == EcsOr) {
        return false;
    }
    if ((term != q->terms) && term[-1].oper == EcsOr) {
        return false;
    }

    /* Wildcards can assume different ids */
    if (ecs_id_is_wildcard(term->id)) {
        return false;
    }

    /* Any terms can have fixed ids, but they require special handling */
    if (term->flags_ & (EcsTermMatchAny|EcsTermMatchAnySrc)) {
        return false;
    }

    /* First terms that are Not or Optional require special handling */
    if (term->oper == EcsNot || term->oper == EcsOptional) {
        if (term == q->terms) {
            return false;
        }
    }

    return true;
}

bool flecs_term_is_or(
    const ecs_query_t *q,
    const ecs_term_t *term)
{
    bool first_term = term == q->terms;
    return (term->oper == EcsOr) || (!first_term && term[-1].oper == EcsOr);
}

ecs_flags16_t flecs_query_ref_flags(
    ecs_flags16_t flags,
    ecs_flags16_t kind)
{
    return (flags >> kind) & (EcsQueryIsVar | EcsQueryIsEntity);
}

bool flecs_query_is_written(
    ecs_var_id_t var_id,
    uint64_t written)
{
    if (var_id == EcsVarNone) {
        return true;
    }

    ecs_assert(var_id < EcsQueryMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    return (written & (1ull << var_id)) != 0;
}

void flecs_query_write(
    ecs_var_id_t var_id,
    uint64_t *written)
{
    ecs_assert(var_id < EcsQueryMaxVarCount, ECS_INTERNAL_ERROR, NULL);
    *written |= (1ull << var_id);
}

void flecs_query_write_ctx(
    ecs_var_id_t var_id,
    ecs_query_compile_ctx_t *ctx,
    bool cond_write)
{
    bool is_written = flecs_query_is_written(var_id, ctx->written);
    flecs_query_write(var_id, &ctx->written);
    if (!is_written) {
        if (cond_write) {
            flecs_query_write(var_id, &ctx->cond_written);
        }
    }
}

bool flecs_ref_is_written(
    const ecs_query_op_t *op,
    const ecs_query_ref_t *ref,
    ecs_flags16_t kind,
    uint64_t written)
{
    ecs_flags16_t flags = flecs_query_ref_flags(op->flags, kind);
    if (flags & EcsQueryIsEntity) {
        ecs_assert(!(flags & EcsQueryIsVar), ECS_INTERNAL_ERROR, NULL);
        if (ref->entity) {
            return true;
        }
    } else if (flags & EcsQueryIsVar) {
        return flecs_query_is_written(ref->var, written);
    }
    return false;
}

ecs_allocator_t* flecs_query_get_allocator(
    const ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    if (flecs_poly_is(world, ecs_world_t)) {
        return &world->allocator;
    } else {
        ecs_assert(flecs_poly_is(world, ecs_stage_t), ECS_INTERNAL_ERROR, NULL);
        return &((ecs_stage_t*)world)->allocator;
    }
}

const char* flecs_query_op_str(
    uint16_t kind)
{
    switch(kind) {
    case EcsQueryAnd:            return "and       ";
    case EcsQueryAndId:          return "andid     ";
    case EcsQueryAndAny:         return "andany    ";
    case EcsQueryTriv:           return "triv      ";
    case EcsQueryTrivData:       return "trivpop   ";
    case EcsQueryTrivWildcard:   return "trivwc    ";
    case EcsQueryCache:          return "cache     ";
    case EcsQueryCacheData:      return "cachepop  ";
    case EcsQueryIsCache:        return "xcache    ";
    case EcsQueryIsCacheData:    return "xcachepop ";
    case EcsQueryOnlyAny:        return "any       ";
    case EcsQueryUp:             return "up        ";
    case EcsQueryUpId:           return "upid      ";
    case EcsQuerySelfUp:         return "selfup    ";
    case EcsQuerySelfUpId:       return "selfupid  ";
    case EcsQueryWith:           return "with      ";
    case EcsQueryTrav:           return "trav      ";
    case EcsQueryAndFrom:        return "andfrom   ";
    case EcsQueryOrFrom:         return "orfrom    ";
    case EcsQueryNotFrom:        return "notfrom   ";
    case EcsQueryIds:            return "ids       ";
    case EcsQueryIdsRight:       return "idsr      ";
    case EcsQueryIdsLeft:        return "idsl      ";
    case EcsQueryEach:           return "each      ";
    case EcsQueryStore:          return "store     ";
    case EcsQueryReset:          return "reset     ";
    case EcsQueryOr:             return "or        ";
    case EcsQueryOptional:       return "option    ";
    case EcsQueryIfVar:          return "ifvar     ";
    case EcsQueryIfSet:          return "ifset     ";
    case EcsQueryEnd:            return "end       ";
    case EcsQueryNot:            return "not       ";
    case EcsQueryPredEq:         return "eq        ";
    case EcsQueryPredNeq:        return "neq       ";
    case EcsQueryPredEqName:     return "eq_nm     ";
    case EcsQueryPredNeqName:    return "neq_nm    ";
    case EcsQueryPredEqMatch:    return "eq_m      ";
    case EcsQueryPredNeqMatch:   return "neq_m     ";
    case EcsQueryMemberEq:       return "membereq  ";
    case EcsQueryMemberNeq:      return "memberneq ";
    case EcsQueryToggle:         return "toggle    ";
    case EcsQueryToggleOption:   return "togglopt  ";
    case EcsQueryUnionEq:        return "union     ";
    case EcsQueryUnionEqWith:    return "union_w   ";
    case EcsQueryUnionNeq:       return "unionneq  ";
    case EcsQueryUnionEqUp:      return "union_up  ";
    case EcsQueryUnionEqSelfUp:  return "union_sup ";
    case EcsQueryLookup:         return "lookup    ";
    case EcsQuerySetVars:        return "setvars   ";
    case EcsQuerySetThis:        return "setthis   ";
    case EcsQuerySetFixed:       return "setfix    ";
    case EcsQuerySetIds:         return "setids    ";
    case EcsQuerySetId:          return "setid     ";
    case EcsQueryContain:        return "contain   ";
    case EcsQueryPairEq:         return "pair_eq   ";
    case EcsQueryPopulate:       return "pop       ";
    case EcsQueryPopulateSelf:   return "popself   ";
    case EcsQueryPopulateSparse: return "popsparse ";
    case EcsQueryYield:          return "yield     ";
    case EcsQueryNothing:        return "nothing   ";
    default:                     return "!invalid  ";
    }
}

static
int32_t flecs_query_op_ref_str(
    const ecs_query_impl_t *query,
    ecs_query_ref_t *ref,
    ecs_flags16_t flags,
    ecs_strbuf_t *buf)
{
    int32_t color_chars = 0;
    if (flags & EcsQueryIsVar) {
        ecs_assert(ref->var < query->var_count, ECS_INTERNAL_ERROR, NULL);
        ecs_query_var_t *var = &query->vars[ref->var];
        ecs_strbuf_appendlit(buf, "#[green]$#[reset]");
        if (var->kind == EcsVarTable) {
            ecs_strbuf_appendch(buf, '[');
        }
        ecs_strbuf_appendlit(buf, "#[green]");
        if (var->name) {
            ecs_strbuf_appendstr(buf, var->name);
        } else {
            if (var->id) {
#ifdef FLECS_DEBUG
                if (var->label) {
                    ecs_strbuf_appendstr(buf, var->label);
                    ecs_strbuf_appendch(buf, '\'');
                }
#endif
                ecs_strbuf_append(buf, "%d", var->id);
            } else {
                ecs_strbuf_appendlit(buf, "this");
            }
        }
        ecs_strbuf_appendlit(buf, "#[reset]");
        if (var->kind == EcsVarTable) {
            ecs_strbuf_appendch(buf, ']');
        }
        color_chars = ecs_os_strlen("#[green]#[reset]#[green]#[reset]");
    } else if (flags & EcsQueryIsEntity) {
        char *path = ecs_get_path(query->pub.world, ref->entity);
        ecs_strbuf_appendlit(buf, "#[blue]");
        ecs_strbuf_appendstr(buf, path);
        ecs_strbuf_appendlit(buf, "#[reset]");
        ecs_os_free(path);
        color_chars = ecs_os_strlen("#[blue]#[reset]");
    }
    return color_chars;
}

static
void flecs_query_str_append_bitset(
    ecs_strbuf_t *buf,
    ecs_flags64_t bitset)
{
    ecs_strbuf_list_push(buf, "{", ",");
    int8_t b;
    for (b = 0; b < 64; b ++) {
        if (bitset & (1llu << b)) {
            ecs_strbuf_list_append(buf, "%d", b);
        }
    }
    ecs_strbuf_list_pop(buf, "}");
}

static
void flecs_query_plan_w_profile(
    const ecs_query_t *q,
    const ecs_iter_t *it,
    ecs_strbuf_t *buf)
{
    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_op_t *ops = impl->ops;
    int32_t i, count = impl->op_count, indent = 0;
    for (i = 0; i < count; i ++) {
        ecs_query_op_t *op = &ops[i];
        ecs_flags16_t flags = op->flags;
        ecs_flags16_t src_flags = flecs_query_ref_flags(flags, EcsQuerySrc);
        ecs_flags16_t first_flags = flecs_query_ref_flags(flags, EcsQueryFirst);
        ecs_flags16_t second_flags = flecs_query_ref_flags(flags, EcsQuerySecond);

        if (it) {
#ifdef FLECS_DEBUG
            const ecs_query_iter_t *rit = &it->priv_.iter.query;
            ecs_strbuf_append(buf, 
                "#[green]%4d -> #[red]%4d <- #[grey]  |   ",
                rit->profile[i].count[0],
                rit->profile[i].count[1]);
#endif
        }

        ecs_strbuf_append(buf, 
            "#[normal]%2d. [#[grey]%2d#[reset], #[green]%2d#[reset]]  ", 
                i, op->prev, op->next);
        int32_t hidden_chars, start = ecs_strbuf_written(buf);
        if (op->kind == EcsQueryEnd) {
            indent --;
        }

        ecs_strbuf_append(buf, "%*s", indent, "");
        ecs_strbuf_appendstr(buf, flecs_query_op_str(op->kind));
        ecs_strbuf_appendstr(buf, " ");

        int32_t written = ecs_strbuf_written(buf);
        for (int32_t j = 0; j < (12 - (written - start)); j ++) {
            ecs_strbuf_appendch(buf, ' ');
        }
    
        hidden_chars = flecs_query_op_ref_str(impl, &op->src, src_flags, buf);

        if (op->kind == EcsQueryNot || 
            op->kind == EcsQueryOr || 
            op->kind == EcsQueryOptional || 
            op->kind == EcsQueryIfVar ||
            op->kind == EcsQueryIfSet) 
        {
            indent ++;
        }

        if (op->kind == EcsQueryPopulate || 
            op->kind == EcsQueryPopulateSelf ||
            op->kind == EcsQueryPopulateSparse ||
            op->kind == EcsQueryTriv ||
            op->kind == EcsQueryTrivData ||
            op->kind == EcsQueryTrivWildcard)
        {
            flecs_query_str_append_bitset(buf, op->src.entity);
        }

        if (op->kind == EcsQueryIfSet) {
            ecs_strbuf_append(buf, "[%d]\n", op->other);
            continue;
        }

        bool is_toggle = op->kind == EcsQueryToggle || 
            op->kind == EcsQueryToggleOption;

        if (!first_flags && !second_flags && !is_toggle) {
            ecs_strbuf_appendstr(buf, "\n");
            continue;
        }

        written = ecs_strbuf_written(buf) - hidden_chars;
        for (int32_t j = 0; j < (30 - (written - start)); j ++) {
            ecs_strbuf_appendch(buf, ' ');
        }

        if (is_toggle) {
            if (op->first.entity) {
                flecs_query_str_append_bitset(buf, op->first.entity);
            }
            if (op->second.entity) {
                if (op->first.entity) {
                    ecs_strbuf_appendlit(buf, ", !");
                }
                flecs_query_str_append_bitset(buf, op->second.entity);
            }
            ecs_strbuf_appendstr(buf, "\n");
            continue;
        }

        ecs_strbuf_appendstr(buf, "(");
        if (op->kind == EcsQueryMemberEq || op->kind == EcsQueryMemberNeq) {
            uint32_t offset = (uint32_t)op->first.entity;
            uint32_t size = (uint32_t)(op->first.entity >> 32);
            ecs_strbuf_append(buf, "#[yellow]elem#[reset]([%d], 0x%x, 0x%x)", 
                op->field_index, size, offset);
        } else {
            flecs_query_op_ref_str(impl, &op->first, first_flags, buf);
        }

        if (second_flags) {
            ecs_strbuf_appendstr(buf, ", ");
            flecs_query_op_ref_str(impl, &op->second, second_flags, buf);
        } else {
            switch (op->kind) {
            case EcsQueryPredEqName:
            case EcsQueryPredNeqName:
            case EcsQueryPredEqMatch:
            case EcsQueryPredNeqMatch: {
                int8_t term_index = op->term_index;
                ecs_strbuf_appendstr(buf, ", #[yellow]\"");
                ecs_strbuf_appendstr(buf, q->terms[term_index].second.name);
                ecs_strbuf_appendstr(buf, "\"#[reset]");
                break;
            }
            case EcsQueryLookup: {
                ecs_var_id_t src_id = op->src.var;
                ecs_strbuf_appendstr(buf, ", #[yellow]\"");
                ecs_strbuf_appendstr(buf, impl->vars[src_id].lookup);
                ecs_strbuf_appendstr(buf, "\"#[reset]");
                break;
            }
            default:
                break;
            }
        }

        ecs_strbuf_appendch(buf, ')');

        ecs_strbuf_appendch(buf, '\n');
    }
}

char* ecs_query_plan_w_profile(
    const ecs_query_t *q,
    const ecs_iter_t *it)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    flecs_query_plan_w_profile(q, it, &buf);
    // ecs_query_impl_t *impl = flecs_query_impl(q);
    // if (impl->cache) {
    //     ecs_strbuf_appendch(&buf, '\n');
    //     flecs_query_plan_w_profile(impl->cache->query, it, &buf);
    // }

#ifdef FLECS_LOG
    char *str = ecs_strbuf_get(&buf);
    flecs_colorize_buf(str, ecs_os_api.flags_ & EcsOsApiLogWithColors, &buf);
    ecs_os_free(str);
#endif

    return ecs_strbuf_get(&buf);
}

char* ecs_query_plan(
    const ecs_query_t *q)
{
    return ecs_query_plan_w_profile(q, NULL);
}

static
void flecs_query_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_term_t *term,
    const ecs_term_ref_t *ref,
    bool is_src)
{
    bool is_added = false;
    ecs_entity_t ref_id = ECS_TERM_REF_ID(ref);
    if (ref->id & EcsIsVariable && !ecs_id_is_wildcard(ref_id)){
        ecs_strbuf_appendlit(buf, "$");
    }

    if (ref_id) {
        char *path = ecs_get_path(world, ref_id);
        ecs_strbuf_appendstr(buf, path);
        ecs_os_free(path);
    } else if (ref->name) {
        ecs_strbuf_appendstr(buf, ref->name);
    } else {
        ecs_strbuf_appendlit(buf, "0");
    }
    is_added = true;

    ecs_flags64_t flags = ECS_TERM_REF_FLAGS(ref);
    if (!(flags & EcsTraverseFlags)) {
        /* If flags haven't been set yet, initialize with defaults. This can
         * happen if an error is thrown while the term is being finalized */
        flags |= EcsSelf;
    }

    if ((flags & EcsTraverseFlags) != EcsSelf) {
        if (is_added) {
            ecs_strbuf_list_push(buf, "|", "|");
        } else {
            ecs_strbuf_list_push(buf, "", "|");
        }
        if (is_src) {
            if (flags & EcsSelf) {
                ecs_strbuf_list_appendstr(buf, "self");
            }

            if (flags & EcsCascade) {
                ecs_strbuf_list_appendstr(buf, "cascade");
            } else if (flags & EcsUp) {
                ecs_strbuf_list_appendstr(buf, "up");
            }

            if (flags & EcsDesc) {
                ecs_strbuf_list_appendstr(buf, "desc");
            }

            if (term->trav) {
                char *rel_path = ecs_get_path(world, term->trav);
                ecs_strbuf_appendlit(buf, " ");
                ecs_strbuf_appendstr(buf, rel_path);
                ecs_os_free(rel_path);
            }
        }

        ecs_strbuf_list_pop(buf, "");
    }
}

void flecs_term_to_buf(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf,
    int32_t t)
{
    const ecs_term_ref_t *src = &term->src;
    const ecs_term_ref_t *first = &term->first;
    const ecs_term_ref_t *second = &term->second;

    ecs_entity_t src_id = ECS_TERM_REF_ID(src);
    ecs_entity_t first_id = ECS_TERM_REF_ID(first);

    bool src_set = !ecs_term_match_0(term);
    bool second_set = ecs_term_ref_is_set(second);

    if (first_id == EcsScopeOpen) {
        ecs_strbuf_appendlit(buf, "{");
        return;
    } else if (first_id == EcsScopeClose) {
        ecs_strbuf_appendlit(buf, "}");
        return;
    }

    if (((ECS_TERM_REF_ID(&term->first) == EcsPredEq) || 
         (ECS_TERM_REF_ID(&term->first) == EcsPredMatch)) && 
        (term->first.id & EcsIsEntity)) 
    {
        ecs_strbuf_appendlit(buf, "$");
        if (ECS_TERM_REF_ID(&term->src) == EcsThis && 
           (term->src.id & EcsIsVariable))
        {
            ecs_strbuf_appendlit(buf, "this");
        } else if (term->src.id & EcsIsVariable) {
            ecs_strbuf_appendstr(buf, term->src.name);
        } else {
            /* Shouldn't happen */
        }

        if (ECS_TERM_REF_ID(&term->first) == EcsPredEq) {
            if (term->oper == EcsNot) {
                ecs_strbuf_appendlit(buf, " != ");
            } else {
                ecs_strbuf_appendlit(buf, " == ");
            }
        } else if (ECS_TERM_REF_ID(&term->first) == EcsPredMatch) {
            ecs_strbuf_appendlit(buf, " ~= ");
        }

        if (term->second.id & EcsIsEntity) {
            if (term->second.id != 0) {
                ecs_get_path_w_sep_buf(
                    world, 0, ECS_TERM_REF_ID(&term->second), ".", NULL, buf);
            }
        } else {
            if (term->second.id & EcsIsVariable) {
                ecs_strbuf_appendlit(buf, "$");
                if (term->second.name) {
                    ecs_strbuf_appendstr(buf, term->second.name);
                } else if (ECS_TERM_REF_ID(&term->second) == EcsThis) {
                    ecs_strbuf_appendlit(buf, "this");
                }
            } else if (term->second.id & EcsIsName) {
                ecs_strbuf_appendlit(buf, "\"");
                if ((ECS_TERM_REF_ID(&term->first) == EcsPredMatch) && 
                    (term->oper == EcsNot)) 
                {
                    ecs_strbuf_appendlit(buf, "!");
                }
                ecs_strbuf_appendstr(buf, term->second.name);
                ecs_strbuf_appendlit(buf, "\"");
            }
        }

        return;
    }

    if (!t || !(term[-1].oper == EcsOr)) {
        if (term->inout == EcsIn) {
            ecs_strbuf_appendlit(buf, "[in] ");
        } else if (term->inout == EcsInOut) {
            ecs_strbuf_appendlit(buf, "[inout] ");
        } else if (term->inout == EcsOut) {
            ecs_strbuf_appendlit(buf, "[out] ");
        } else if (term->inout == EcsInOutNone && term->oper != EcsNot) {
            ecs_strbuf_appendlit(buf, "[none] ");
        }
    }

    if (term->oper == EcsNot) {
        ecs_strbuf_appendlit(buf, "!");
    } else if (term->oper == EcsOptional) {
        ecs_strbuf_appendlit(buf, "?");
    }

    if (!src_set) {
        flecs_query_str_add_id(world, buf, term, &term->first, false);
        if (!second_set) {
            ecs_strbuf_appendlit(buf, "()");
        } else {
            ecs_strbuf_appendlit(buf, "(#0,");
            flecs_query_str_add_id(world, buf, term, &term->second, false);
            ecs_strbuf_appendlit(buf, ")");
        }
    } else {
        ecs_id_t flags = term->id & ECS_ID_FLAGS_MASK;
        if (flags && !ECS_HAS_ID_FLAG(flags, PAIR)) {
            ecs_strbuf_appendstr(buf, ecs_id_flag_str(flags));
            ecs_strbuf_appendch(buf, '|');
        }

        flecs_query_str_add_id(world, buf, term, &term->first, false);
        ecs_strbuf_appendlit(buf, "(");
        if (term->src.id & EcsIsEntity && src_id == first_id) {
            ecs_strbuf_appendlit(buf, "$");
        } else {
            flecs_query_str_add_id(world, buf, term, &term->src, true);
        }
        if (second_set) {
            ecs_strbuf_appendlit(buf, ",");
            flecs_query_str_add_id(world, buf, term, &term->second, false);
        }
        ecs_strbuf_appendlit(buf, ")");
    }
}

char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_term_to_buf(world, term, &buf, 0);
    return ecs_strbuf_get(&buf);
}

char* ecs_query_str(
    const ecs_query_t *q)
{
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_world_t *world = q->world;

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    const ecs_term_t *terms = q->terms;
    int32_t i, count = q->term_count;

    for (i = 0; i < count; i ++) {
        const ecs_term_t *term = &terms[i];

        flecs_term_to_buf(world, term, &buf, i);

        if (i != (count - 1)) {
            if (term->oper == EcsOr) {
                ecs_strbuf_appendlit(&buf, " || ");
            } else {
                if (ECS_TERM_REF_ID(&term->first) != EcsScopeOpen) {
                    if (ECS_TERM_REF_ID(&term[1].first) != EcsScopeClose) {
                        ecs_strbuf_appendlit(&buf, ", ");
                    }
                }
            }
        }
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

int32_t flecs_query_pivot_term(
    const ecs_world_t *world,
    const ecs_query_t *query)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_term_t *terms = query->terms;
    int32_t i, term_count = query->term_count;
    int32_t pivot_term = -1, min_count = -1, self_pivot_term = -1;

    for (i = 0; i < term_count; i ++) {
        const ecs_term_t *term = &terms[i];
        ecs_id_t id = term->id;

        if ((term->oper != EcsAnd) || (i && (term[-1].oper == EcsOr))) {
            continue;
        }

        if (!ecs_term_match_this(term)) {
            continue;
        }

        ecs_id_record_t *idr = flecs_id_record_get(world, id);
        if (!idr) {
            /* If one of the terms does not match with any data, iterator 
             * should not return anything */
            return -2; /* -2 indicates query doesn't match anything */
        }

        int32_t table_count = flecs_table_cache_count(&idr->cache);
        if (min_count == -1 || table_count < min_count) {
            min_count = table_count;
            pivot_term = i;
            if ((term->src.id & EcsTraverseFlags) == EcsSelf) {
                self_pivot_term = i;
            }
        }
    }

    if (self_pivot_term != -1) {
        pivot_term = self_pivot_term;
    }

    return pivot_term;
error:
    return -2;
}

void flecs_query_apply_iter_flags(
    ecs_iter_t *it,
    const ecs_query_t *query)
{
    ECS_BIT_COND(it->flags, EcsIterIsInstanced, 
        ECS_BIT_IS_SET(query->flags, EcsQueryIsInstanced));
    ECS_BIT_COND(it->flags, EcsIterNoData,
        ECS_BIT_IS_SET(query->flags, EcsQueryNoData));
    ECS_BIT_COND(it->flags, EcsIterHasCondSet, 
        ECS_BIT_IS_SET(query->flags, EcsQueryHasCondSet));
}
