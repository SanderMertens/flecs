 /**
 * @file addons/rules/api.c
 * @brief User facing API for rules.
 */

#include "rules.h"

#ifdef FLECS_RULES

ecs_mixins_t ecs_rule_t_mixins = {
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
    case EcsRuleEach:         return "each    ";
    case EcsRuleDown:         return "down    ";
    case EcsRuleStore:        return "store   ";
    case EcsRuleUnion:        return "union   ";
    case EcsRuleEnd:          return "end     ";
    case EcsRuleNot:          return "not     ";
    case EcsRuleOption:       return "option  ";
    case EcsRuleSetVars:      return "setvars ";
    case EcsRuleSetThis:      return "setthis ";
    case EcsRuleContain:      return "contain ";
    case EcsRulePairEq:       return "pair_eq ";
    case EcsRuleSetCond:      return "setcond ";
    case EcsRuleJmpCondFalse: return "jfalse  ";
    case EcsRuleJmpNotSet:    return "jnotset ";
    case EcsRuleYield:        return "yield   ";
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
        iter[1] = ecs_rule_iter(world, (ecs_rule_t*)poly);
        iter[0] = ecs_term_chain_iter(&iter[1], filter);
    } else {
        iter[0] = ecs_rule_iter(world, (ecs_rule_t*)poly);
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

    /* Find all variables defined in query */
    flecs_rule_discover_vars(stage, result);

    /* Compile filter to operations */
    flecs_rule_compile(world, stage, result);

    result->world = world;
    result->iterable.init = flecs_rule_iter_mixin_init;

    return result;
error:
    return NULL;
}

void ecs_rule_fini(
    ecs_rule_t *rule)
{
    if (rule->vars != &rule->vars_cache.var) {
        ecs_os_free(rule->vars);
    }

    ecs_os_free(rule->ops);
    flecs_name_index_fini(&rule->tvar_index);
    flecs_name_index_fini(&rule->evar_index);
    ecs_filter_fini(&rule->filter);

    ecs_poly_free(rule, ecs_rule_t);
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
        ecs_rule_var_t *var = &rule->vars[ref->var];
        ecs_strbuf_appendstr(buf, "#[green]$#[reset]");
        if (var->kind == EcsVarTable) {
            ecs_strbuf_appendch(buf, '[');
        }
        ecs_strbuf_appendstr(buf, "#[green]");
        if (var->name) {
            ecs_strbuf_appendstr(buf, var->name);
        } else {
            if (var->label) {
                ecs_strbuf_appendstr(buf, var->label);
                ecs_strbuf_appendch(buf, '\'');
            }
            ecs_strbuf_append(buf, "%d", var->id);
        }
        ecs_strbuf_appendstr(buf, "#[reset]");
        if (var->kind == EcsVarTable) {
            ecs_strbuf_appendch(buf, ']');
        }
        color_chars = ecs_os_strlen("#[green]#[reset]#[green]#[reset]");
    } else if (flags & EcsRuleIsEntity) {
        char *path = ecs_get_fullpath(rule->world, ref->entity);
        ecs_strbuf_appendstr(buf, "#[blue]");
        ecs_strbuf_appendstr(buf, path);
        ecs_strbuf_appendstr(buf, "#[reset]");
        ecs_os_free(path);
        color_chars = ecs_os_strlen("#[blue]#[reset]");
    }
    return color_chars;
}

char* ecs_rule_str(
    ecs_rule_t *rule)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    ecs_rule_op_t *ops = rule->ops;
    int32_t i, count = rule->op_count, indent = 0;
    for (i = 0; i < count; i ++) {
        ecs_rule_op_t *op = &ops[i];
        ecs_flags16_t flags = op->flags;
        ecs_flags16_t src_flags = flecs_rule_ref_flags(flags, EcsRuleSrc);
        ecs_flags16_t first_flags = flecs_rule_ref_flags(flags, EcsRuleFirst);
        ecs_flags16_t second_flags = flecs_rule_ref_flags(flags, EcsRuleSecond);

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
        for (int32_t i = 0; i < (10 - (written - start)); i ++) {
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
        for (int32_t i = 0; i < (30 - (written - start)); i ++) {
            ecs_strbuf_appendch(&buf, ' ');
        }

        ecs_strbuf_appendstr(&buf, "(");
        flecs_rule_op_ref_str(rule, &op->first, first_flags, &buf);

        if (second_flags) {
            ecs_strbuf_appendstr(&buf, ", ");
            flecs_rule_op_ref_str(rule, &op->second, second_flags, &buf);
        }

        ecs_strbuf_appendch(&buf, ')');

        // if (op->flags & EcsRuleIsSelf) {
        //     ecs_strbuf_appendstr(&buf, ", self");
        // }

        // if (op->trav) {
        //     ecs_strbuf_appendstr(&buf, ", trav: #[blue]");
        //     ecs_strbuf_appendstr(&buf, ecs_get_name(rule->world, op->trav));
        //     ecs_strbuf_appendstr(&buf, "#[reset]");
        // }

        ecs_strbuf_appendch(&buf, '\n');
    }
    
    char *str = ecs_strbuf_get(&buf);
    flecs_colorize_buf(str, true, &buf);
    ecs_os_free(str);
    return ecs_strbuf_get(&buf);
}

#endif
