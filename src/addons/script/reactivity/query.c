#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../query_dsl/query_dsl.h"

ECS_COMPONENT_DECLARE(EcsScriptQuery);

static ecs_entity_t* flecs_script_query_lookup_push(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t **prev_lookup,
    ecs_entity_t *prev_scope)
{
    ecs_world_t *world = v->world;
    int32_t i, count = ecs_vec_count(&v->r->using);
    ecs_entity_t *using = ecs_vec_first(&v->r->using);
    ecs_entity_t *lookup = ecs_os_malloc_n(ecs_entity_t, count + 1);
    for (i = 0; i < count; i ++) {
        lookup[i] = using[count - i - 1];
    }

    lookup[count] = 0;
    *prev_lookup = ecs_set_lookup_path(world, lookup);
    *prev_scope = ecs_set_scope(world, v->parent);
    return lookup;
}

static void flecs_script_query_lookup_pop(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t *lookup,
    ecs_entity_t *prev_lookup,
    ecs_entity_t prev_scope)
{
    ecs_set_scope(v->world, prev_scope);
    ecs_set_lookup_path(v->world, prev_lookup);
    ecs_os_free(lookup);
}

static bool flecs_script_query_name_is_entity(
    const char *name)
{
    if (!name || !name[0] || name[0] == '$' || name[0] == '#') {
        return false;
    }

    if (!ecs_os_strcmp(name, "*") || !ecs_os_strcmp(name, "_") ||
        !ecs_os_strcmp(name, "0") || !ecs_os_strcmp(name, "this"))
    {
        return false;
    }

    return !isdigit(name[0]);
}

static void flecs_script_query_unresolved(
    ecs_script_eval_visitor_t *v,
    ecs_script_query_node_t *node)
{
    ecs_world_t *world = v->world;
    ecs_size_t len = ecs_os_strlen(node->expr);
    ecs_os_free(node->unresolved);
    char *token_buffer = node->unresolved = ecs_os_malloc(len * 2 + 1);
    ecs_term_t *terms = ecs_os_calloc_n(ecs_term_t, FLECS_TERM_COUNT_MAX);
    int32_t i, term_count = 0;

    int result = flecs_terms_parse(world, NULL, node->expr, token_buffer,
        terms, &term_count);

    if (!result) {
        for (i = 0; i < term_count; i ++) {
            ecs_term_ref_t *refs[] = {
                &terms[i].src, &terms[i].first, &terms[i].second };
            for (int32_t r = 0; r < 3; r ++) {
                const char *name = refs[r]->name;
                if (!flecs_script_query_name_is_entity(name) ||
                    (refs[r]->id & (EcsIsVariable | EcsIsName)))
                {
                    continue;
                }

                if (!ecs_lookup(world, name)) {
                    flecs_script_unresolved_ref_add(v->base.script,
                        node->expr_pos, name, FlecsScriptUnresolvedEntity);
                }
            }
        }
    }

    ecs_os_free(terms);
}

int flecs_script_query_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_query_node_t *node)
{
    ecs_world_t *world = v->world;
    if (node->query) {
        ecs_query_fini(node->query);
        node->query = NULL;
    }

    ecs_entity_t *prev_lookup, prev_scope;
    ecs_entity_t *lookup = flecs_script_query_lookup_push(
        v, &prev_lookup, &prev_scope);
    node->query = ecs_query_init(world, &(ecs_query_desc_t){
        .expr = node->expr
    });

    if (!node->query) {
        flecs_script_query_unresolved(v, node);
    }

    flecs_script_query_lookup_pop(v, lookup, prev_lookup, prev_scope);

    if (!node->query) {
        flecs_script_eval_error(v, node,
            "invalid expression for query '%s'", node->name);
        return -1;
    }

    return 0;
}

int32_t flecs_script_query_data_fields(
    const ecs_query_t *query,
    int8_t *fields)
{
    int32_t f, count = 0;
    for (f = 0; f < query->field_count; f ++) {
        if (query->data_fields & ((ecs_termset_t)1 << f)) {
            if (fields) {
                fields[count] = flecs_ito(int8_t, f);
            }
            count ++;
        }
    }

    return count;
}

bool flecs_script_query_iter(
    const EcsScriptQuery *value,
    ecs_iter_t *it)
{
    if (!value || !value->node || !value->node->query) {
        return false;
    }

    const ecs_query_t *q = value->node->query;
    *it = ecs_query_iter(q->world, q);
    return true;
}

void flecs_script_query_to_bool(
    void *dst,
    const void *src)
{
    ecs_iter_t it;
    *(bool*)dst = flecs_script_query_iter(src, &it) && ecs_iter_is_true(&it);
}

static void flecs_script_query_observe(
    ecs_world_t *world,
    const ecs_script_query_node_t *node,
    ecs_script_query_slot_t *slot,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    const ecs_query_t *q = node->query;
    ecs_query_desc_t desc = {0};
    int32_t i;
    for (i = 0; i < q->term_count; i ++) {
        desc.terms[i] = q->terms[i];
    }

    slot->observers[0] = flecs_script_query_observer_init(
        world, script, instance, &desc, EcsMonitor, node->input);

    if (q->data_fields) {
        slot->observers[1] = flecs_script_query_observer_init(
            world, script, instance, &desc, EcsOnSet, node->input);
    }
}

int flecs_script_eval_query(
    ecs_script_eval_visitor_t *v,
    ecs_script_query_node_t *node)
{
    ecs_world_t *world = v->world;
    if (!node->query) {
        flecs_script_eval_error(v, node,
            "query '%s' is not initialized", node->name);
        return -1;
    }

    ecs_vec_t *slots = &v->state->query_slots;
    if (node->query_slot < 0 || node->query_slot >= ecs_vec_count(slots)) {
        flecs_script_eval_error(v, node,
            "query '%s' cannot be declared here", node->name);
        return -1;
    }

    ecs_script_query_slot_t *slot = ecs_vec_get_t(
        slots, ecs_script_query_slot_t, node->query_slot);
    slot->world = world;
    slot->value.node = node;

    if (!slot->observers[0] && node->input) {
        ecs_entity_t script = 0, instance = 0;
        if (v->template_instance) {
            script = v->body_template;
            instance = v->template_instance;
        } else {
            script = v->base.script->entity;
        }

        if (script) {
            flecs_script_query_observe(world, node, slot, script, instance);
        }
    }

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars,
        (v->template_entity || v->base.script->ir_enabled)
            ? NULL : node->name);
    if (!var) {
        flecs_script_eval_error(v, node,
            "variable '%s' redeclared", node->name);
        return -1;
    }

    var->value.type = ecs_id(EcsScriptQuery);
    var->value.ptr = &slot->value;
    var->type_info = ecs_get_type_info(world, ecs_id(EcsScriptQuery));
    var->is_const = true;
    var->owned = false;

    if (node->computed) {
        ecs_vec_t *computed = &v->state->computed;
        ecs_script_computed_t *cslot = node->computed <= ecs_vec_count(computed)
            ? ecs_vec_get_t(computed, ecs_script_computed_t, node->computed - 1)
            : NULL;
        if (cslot && (!cslot->valid ||
            flecs_script_eval_force(v, &node->node, v->force)))
        {
            bool any = false;
            flecs_script_query_to_bool(&any, &slot->value);
            flecs_script_computed_store(v, cslot, node->computed - 1,
                ecs_get_type_info(world, ecs_id(ecs_bool_t)), &any);
        }
    }

    return 0;
}

void flecs_script_query_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptQuery);
}

#endif
