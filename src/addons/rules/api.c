 /**
 * @file addons/rules/api.c
 * @brief User facing API for rules.
 */

#include "rules.h"
#include <ctype.h>

#ifdef FLECS_RULES

static ecs_mixins_t ecs_rule_t_mixins = {
    .type_name = "ecs_rule_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_rule_t, filter.world),
        [EcsMixinEntity] = offsetof(ecs_rule_t, filter.entity),
        [EcsMixinIterable] = offsetof(ecs_rule_t, iterable),
        [EcsMixinDtor] = offsetof(ecs_rule_t, dtor)
    }
};

static
const char* flecs_rule_op_str(
    uint16_t kind)
{
    switch(kind) {
    case EcsRuleAnd:          return "and     ";
    case EcsRuleAndId:        return "and_id  ";
    case EcsRuleAndAny:       return "and_any ";
    case EcsRuleUp:           return "up      ";
    case EcsRuleSelfUp:       return "selfup  ";
    case EcsRuleWith:         return "with    ";
    case EcsRuleTrav:         return "trav    ";
    case EcsRuleIdsRight:     return "idsr    ";
    case EcsRuleIdsLeft:      return "idsl    ";
    case EcsRuleEach:         return "each    ";
    case EcsRuleStore:        return "store   ";
    case EcsRuleReset:        return "reset   ";
    case EcsRuleUnion:        return "union   ";
    case EcsRuleEnd:          return "end     ";
    case EcsRuleNot:          return "not     ";
    case EcsRulePredEq:       return "eq      ";
    case EcsRulePredNeq:      return "neq     ";
    case EcsRulePredEqName:   return "eq_nm   ";
    case EcsRulePredNeqName:  return "neq_nm  ";
    case EcsRulePredEqMatch:  return "eq_m    ";
    case EcsRulePredNeqMatch: return "neq_m   ";
    case EcsRuleLookup:       return "lookup  ";
    case EcsRuleSetVars:      return "setvars ";
    case EcsRuleSetThis:      return "setthis ";
    case EcsRuleSetFixed:     return "setfix  ";
    case EcsRuleSetIds:       return "setids  ";
    case EcsRuleContain:      return "contain ";
    case EcsRulePairEq:       return "pair_eq ";
    case EcsRuleSetCond:      return "setcond ";
    case EcsRuleJmpCondFalse: return "jfalse  ";
    case EcsRuleJmpNotSet:    return "jnotset ";
    case EcsRuleYield:        return "yield   ";
    case EcsRuleNothing:      return "nothing ";
    default: return "!invalid";
    }
}

/* Implementation for iterable mixin */
static
void flecs_rule_iter_mixin_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter)
{
    ecs_poly_assert(poly, ecs_rule_t);

    if (filter) {
        iter[1] = ecs_rule_iter(world, ECS_CONST_CAST(ecs_rule_t*, poly));
        iter[0] = ecs_term_chain_iter(&iter[1], filter);
    } else {
        iter[0] = ecs_rule_iter(world, ECS_CONST_CAST(ecs_rule_t*, poly));
    }
}

static
void flecs_rule_fini(
    ecs_rule_t *rule)
{
    if (rule->vars != &rule->vars_cache.var) {
        ecs_os_free(rule->vars);
    }

    ecs_os_free(rule->ops);
    ecs_os_free(rule->src_vars);
    flecs_name_index_fini(&rule->tvar_index);
    flecs_name_index_fini(&rule->evar_index);
    ecs_filter_fini(&rule->filter);

    ecs_poly_free(rule, ecs_rule_t);
}

void ecs_rule_fini(
    ecs_rule_t *rule)
{
    if (rule->filter.entity) {
        /* If filter is associated with entity, use poly dtor path */
        ecs_delete(rule->filter.world, rule->filter.entity);
    } else {
        flecs_rule_fini(rule);
    }
}

ecs_rule_t* ecs_rule_init(
    ecs_world_t *world, 
    const ecs_filter_desc_t *const_desc)
{
    ecs_rule_t *result = ecs_poly_new(ecs_rule_t);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    /* Initialize the query */
    ecs_filter_desc_t desc = *const_desc;
    desc.storage = &result->filter; /* Use storage of rule */
    result->filter = ECS_FILTER_INIT;
    if (ecs_filter_init(world, &desc) == NULL) {
        goto error;
    }

    result->iterable.init = flecs_rule_iter_mixin_init;

    /* Compile filter to operations */
    if (flecs_rule_compile(world, stage, result)) {
        goto error;
    }

    ecs_entity_t entity = const_desc->entity;
    result->dtor = (ecs_poly_dtor_t)flecs_rule_fini;

    if (entity) {
        EcsPoly *poly = ecs_poly_bind(world, entity, ecs_rule_t);
        poly->poly = result;
        ecs_poly_modified(world, entity, ecs_rule_t);
    }

    return result;
error:
    ecs_rule_fini(result);
    return NULL;
}

static
int32_t flecs_rule_op_ref_str(
    const ecs_rule_t *rule,
    ecs_rule_ref_t *ref,
    ecs_flags16_t flags,
    ecs_strbuf_t *buf)
{
    int32_t color_chars = 0;
    if (flags & EcsRuleIsVar) {
        ecs_assert(ref->var < rule->var_count, ECS_INTERNAL_ERROR, NULL);
        ecs_rule_var_t *var = &rule->vars[ref->var];
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
    } else if (flags & EcsRuleIsEntity) {
        char *path = ecs_get_fullpath(rule->filter.world, ref->entity);
        ecs_strbuf_appendlit(buf, "#[blue]");
        ecs_strbuf_appendstr(buf, path);
        ecs_strbuf_appendlit(buf, "#[reset]");
        ecs_os_free(path);
        color_chars = ecs_os_strlen("#[blue]#[reset]");
    }
    return color_chars;
}

char* ecs_rule_str_w_profile(
    const ecs_rule_t *rule,
    const ecs_iter_t *it)
{
    ecs_poly_assert(rule, ecs_rule_t);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_rule_op_t *ops = rule->ops;
    int32_t i, count = rule->op_count, indent = 0;
    for (i = 0; i < count; i ++) {
        ecs_rule_op_t *op = &ops[i];
        ecs_flags16_t flags = op->flags;
        ecs_flags16_t src_flags = flecs_rule_ref_flags(flags, EcsRuleSrc);
        ecs_flags16_t first_flags = flecs_rule_ref_flags(flags, EcsRuleFirst);
        ecs_flags16_t second_flags = flecs_rule_ref_flags(flags, EcsRuleSecond);

        if (it) {
#ifdef FLECS_DEBUG
            const ecs_rule_iter_t *rit = &it->priv.iter.rule;
            ecs_strbuf_append(&buf, 
                "#[green]%4d -> #[red]%4d <- #[grey]  |   ",
                rit->profile[i].count[0],
                rit->profile[i].count[1]);
#endif
        }

        ecs_strbuf_append(&buf, 
            "#[normal]%2d. [#[grey]%2d#[reset], #[green]%2d#[reset]]  ", 
                i, op->prev, op->next);
        int32_t hidden_chars, start = ecs_strbuf_written(&buf);
        if (op->kind == EcsRuleEnd) {
            indent --;
        }

        ecs_strbuf_append(&buf, "%*s", indent, "");
        ecs_strbuf_appendstr(&buf, flecs_rule_op_str(op->kind));
        ecs_strbuf_appendstr(&buf, " ");

        int32_t written = ecs_strbuf_written(&buf);
        for (int32_t j = 0; j < (10 - (written - start)); j ++) {
            ecs_strbuf_appendch(&buf, ' ');
        }

        if (op->kind == EcsRuleJmpCondFalse || op->kind == EcsRuleSetCond ||
            op->kind == EcsRuleJmpNotSet) 
        {
            ecs_strbuf_appendint(&buf, op->other);
            ecs_strbuf_appendch(&buf, ' ');
        }
    
        hidden_chars = flecs_rule_op_ref_str(rule, &op->src, src_flags, &buf);

        if (op->kind == EcsRuleUnion) {
            indent ++;
        }

        if (!first_flags && !second_flags) {
            ecs_strbuf_appendstr(&buf, "\n");
            continue;
        }

        written = ecs_strbuf_written(&buf) - hidden_chars;
        for (int32_t j = 0; j < (30 - (written - start)); j ++) {
            ecs_strbuf_appendch(&buf, ' ');
        }

        ecs_strbuf_appendstr(&buf, "(");
        flecs_rule_op_ref_str(rule, &op->first, first_flags, &buf);

        if (second_flags) {
            ecs_strbuf_appendstr(&buf, ", ");
            flecs_rule_op_ref_str(rule, &op->second, second_flags, &buf);
        } else {
            switch (op->kind) {
            case EcsRulePredEqName:
            case EcsRulePredNeqName:
            case EcsRulePredEqMatch:
            case EcsRulePredNeqMatch: {
                int8_t term_index = op->term_index;
                ecs_strbuf_appendstr(&buf, ", #[yellow]\"");
                ecs_strbuf_appendstr(&buf, rule->filter.terms[term_index].second.name);
                ecs_strbuf_appendstr(&buf, "\"#[reset]");
                break;
            }
            case EcsRuleLookup: {
                ecs_var_id_t src_id = op->src.var;
                ecs_strbuf_appendstr(&buf, ", #[yellow]\"");
                ecs_strbuf_appendstr(&buf, rule->vars[src_id].lookup);
                ecs_strbuf_appendstr(&buf, "\"#[reset]");
                break;
            }
            default:
                break;
            }
        }

        ecs_strbuf_appendch(&buf, ')');

        ecs_strbuf_appendch(&buf, '\n');
    }

#ifdef FLECS_LOG    
    char *str = ecs_strbuf_get(&buf);
    flecs_colorize_buf(str, true, &buf);
    ecs_os_free(str);
#endif
    return ecs_strbuf_get(&buf);
}

char* ecs_rule_str(
    const ecs_rule_t *rule)
{
    return ecs_rule_str_w_profile(rule, NULL);
}

const ecs_filter_t* ecs_rule_get_filter(
    const ecs_rule_t *rule)
{
    return &rule->filter;
}

const char* ecs_rule_parse_vars(
    ecs_rule_t *rule,
    ecs_iter_t *it,
    const char *expr)
{
    ecs_poly_assert(rule, ecs_rule_t);
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(expr != NULL, ECS_INVALID_PARAMETER, NULL)
    char token[ECS_MAX_TOKEN_SIZE];
    const char *ptr = expr;
    bool paren = false;

    const char *name = NULL;
    if (rule->filter.entity) {
        name = ecs_get_name(rule->filter.world, rule->filter.entity);
    }

    ptr = ecs_parse_ws_eol(ptr);
    if (!ptr[0]) {
        return ptr;
    }

    if (ptr[0] == '(') {
        paren = true;
        ptr = ecs_parse_ws_eol(ptr + 1);
        if (ptr[0] == ')') {
            return ptr + 1;
        }
    }

    do {
        ptr = ecs_parse_ws_eol(ptr);
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            return NULL;
        }

        int var = ecs_rule_find_var(rule, token);
        if (var == -1) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unknown variable '%s'", token);
            return NULL;
        }

        ptr = ecs_parse_ws_eol(ptr);
        if (ptr[0] != ':') {
            ecs_parser_error(name, expr, (ptr - expr), 
                "missing ':'");
            return NULL;
        }

        ptr = ecs_parse_ws_eol(ptr + 1);
        ptr = ecs_parse_identifier(name, expr, ptr, token);
        if (!ptr) {
            return NULL;
        }

        ecs_entity_t val = ecs_lookup_fullpath(rule->filter.world, token);
        if (!val) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unresolved entity '%s'", token);
            return NULL;
        }

        ecs_iter_set_var(it, var, val);

        ptr = ecs_parse_ws_eol(ptr);
        if (ptr[0] == ')') {
            if (!paren) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "unexpected closing parenthesis");
                return NULL;
            }

            ptr ++;
            break;
        } else if (ptr[0] == ',') {
            ptr ++;
        } else if (!ptr[0]) {
            if (paren) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "missing closing parenthesis");
                return NULL;
            }
            break;
        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected , or end of string");
            return NULL;
        }
    } while (true);

    return ptr;
error:
    return NULL;
}

#endif
