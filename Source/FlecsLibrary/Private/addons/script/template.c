/**
 * @file addons/script/template.c
 * @brief Script template implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScriptTemplateSetEvent);
ECS_DECLARE(EcsScriptTemplate);

static
void flecs_template_set_event_free(EcsScriptTemplateSetEvent *ptr) {
    if (ptr->entities != &ptr->entity_storage) {
        ecs_os_free(ptr->entities);
    }
    if (ptr->data != ptr->data_storage) {
        ecs_os_free(ptr->data);
    }
}

static
ECS_MOVE(EcsScriptTemplateSetEvent, dst, src, {
    flecs_template_set_event_free(dst);

    *dst = *src;

    if (src->entities == &src->entity_storage) {
        dst->entities = &dst->entity_storage;
    }

    if (src->data == src->data_storage) {
        dst->data = &dst->data_storage;
    }

    src->entities = NULL;
    src->data = NULL;
})

static
ECS_DTOR(EcsScriptTemplateSetEvent, ptr, {
    flecs_template_set_event_free(ptr);
})

/* Template ctor to initialize with default property values */
static
void flecs_script_template_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_entity_t template_entity = ti->component;
    
    const EcsStruct *st = ecs_get(world, template_entity, EcsStruct);
    if (!st) {
        ecs_os_memset(ptr, 0, count * ti->size);
        return;
    }

    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    if (!script) {
        ecs_err("template '%s' is not a script, cannot construct", ti->name);
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
    if (st->members.count != template->prop_defaults.count) {
        ecs_err("number of props (%d) of template '%s' does not match members"
            " (%d), cannot construct", template->prop_defaults.count, 
                ti->name, st->members.count);
        return;
    }

    const ecs_member_t *members = st->members.array;
    int32_t i, m, member_count = st->members.count;
    ecs_script_var_t *values = template->prop_defaults.array;
    for (m = 0; m < member_count; m ++) {
        const ecs_member_t *member = &members[m];
        ecs_script_var_t *value = &values[m];
        const ecs_type_info_t *mti = value->type_info;
        ecs_assert(mti != NULL, ECS_INTERNAL_ERROR, NULL);

        for (i = 0; i < count; i ++) {
            void *el = ECS_ELEM(ptr, ti->size, i);
            ecs_value_copy_w_type_info(world, mti, 
                ECS_OFFSET(el, member->offset), value->value.ptr);
        }
    }
}

/* Defer template instantiation if we're in deferred mode. */
static
void flecs_script_template_defer_on_set(
    ecs_iter_t *it,
    ecs_entity_t template_entity,
    const ecs_type_info_t *ti,
    void *data)
{
    EcsScriptTemplateSetEvent evt;

    if ((it->count == 1) && ti->size <= ECS_TEMPLATE_SMALL_SIZE && !ti->hooks.dtor) {
        /* This should be true for the vast majority of templates */
        evt.entities = &evt.entity_storage;
        evt.data = evt.data_storage;
        evt.entity_storage = it->entities[0];
        ecs_os_memcpy(evt.data, data, ti->size);
    } else {
        evt.entities = ecs_os_memdup_n(it->entities, ecs_entity_t, it->count);
        evt.data = ecs_os_memdup(data, ti->size * it->count);
    }

    evt.count = it->count;
    evt.template_entity = template_entity;

    ecs_enqueue(it->world, &(ecs_event_desc_t){
        .event = ecs_id(EcsScriptTemplateSetEvent),
        .entity = EcsAny,
        .param = &evt
    });
}

static
void flecs_script_template_instantiate(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    const ecs_entity_t *entities,
    void *data,
    int32_t count)
{
    ecs_assert(!ecs_is_deferred(world), ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = ecs_record_find(world, template_entity);
    if (!r) {
        ecs_err("template entity is empty (should never happen)");
        return;
    }

    const EcsScript *script = ecs_record_get(world, r, EcsScript);
    if (!script) {
        ecs_err("template is missing script component");
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = template->type_info;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    const EcsStruct *st = ecs_record_get(world, r, EcsStruct);

    ecs_script_eval_visitor_t v;
    ecs_script_eval_desc_t desc = {
        .runtime = flecs_script_runtime_get(world)
    };

    flecs_script_eval_visit_init(flecs_script_impl(script->script), &v, &desc);
    ecs_vec_t prev_using = v.r->using;
    ecs_vec_t prev_with = desc.runtime->with;
    ecs_vec_t prev_with_type_info = desc.runtime->with_type_info;
    v.r->using = template->using_;
    v.template_entity = template_entity;
    ecs_vec_init_t(NULL, &desc.runtime->with, ecs_value_t, 0);
    ecs_vec_init_t(NULL, &desc.runtime->with_type_info, ecs_type_info_t*, 0);

    ecs_script_scope_t *scope = template->node->scope;

    /* Dummy entity node for instance */
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = template->node->node.pos
        },
        .scope = scope
    };

    v.entity = &instance_node;

    int32_t i, m;
    for (i = 0; i < count; i ++) {
        v.parent = entities[i];
        ecs_assert(ecs_is_alive(world, v.parent), ECS_INTERNAL_ERROR, NULL);

        instance_node.eval = entities[i];

        /* Create variables to hold template properties */
        ecs_script_vars_t *vars = flecs_script_vars_push(
            NULL, &v.r->stack, &v.r->allocator);
        vars->parent = template->vars; /* Include hoisted variables */
        vars->sp = ecs_vec_count(&template->vars->vars);

        /* Allocate enough space for variables */
        ecs_script_vars_set_size(vars, (st ? st->members.count : 0) + 1);

        /* Populate $this variable with instance entity */
        ecs_entity_t instance = entities[i];
        ecs_script_var_t *this_var = ecs_script_vars_declare(
            vars, NULL /* $this */);
        this_var->value.type = ecs_id(ecs_entity_t);
        this_var->value.ptr = &instance;

        /* Populate properties from template members */
        if (st) {
            const ecs_member_t *members = st->members.array;
            for (m = 0; m < st->members.count; m ++) {
                const ecs_member_t *member = &members[m];

                /* Assign template property from template instance. Don't 
                 * set name as variables will be resolved by frame offset. */
                ecs_script_var_t *var = ecs_script_vars_declare(
                    vars, NULL /* member->name */);
                var->value.type = member->type;
                var->value.ptr = ECS_OFFSET(data, member->offset);
            }
        }

        ecs_script_clear(world, template_entity, instance);

        /* Run template code */
        v.vars = vars;

        flecs_script_eval_scope(&v, scope);

        /* Pop variable scope */
        ecs_script_vars_pop(vars);

        data = ECS_OFFSET(data, ti->size);
    }

    ecs_vec_fini_t(&desc.runtime->allocator, 
        &desc.runtime->with, ecs_value_t);
    ecs_vec_fini_t(&desc.runtime->allocator, 
        &desc.runtime->with_type_info, ecs_type_info_t*);

    v.r->with = prev_with;
    v.r->with_type_info = prev_with_type_info;
    v.r->using = prev_using;
    flecs_script_eval_visit_fini(&v, &desc);
}

static
void flecs_on_template_set_event(
    ecs_iter_t *it)
{
    ecs_assert(ecs_is_deferred(it->world), ECS_INTERNAL_ERROR, NULL);

    EcsScriptTemplateSetEvent *evt = it->param;
    ecs_world_t *world = it->real_world;
    ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);

    ecs_defer_suspend(world);

    flecs_script_template_instantiate(
        world, evt->template_entity, evt->entities, evt->data, evt->count);

    ecs_defer_resume(world);
}

/* Template on_set handler to update contents for new property values */
static
void flecs_script_template_on_set(
    ecs_iter_t *it)
{
    if (it->table->flags & EcsTableIsPrefab) {
        /* Don't instantiate templates for prefabs */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t template_entity = ecs_field_id(it, 0);
    ecs_record_t *r = ecs_record_find(world, template_entity);
    if (!r) {
        ecs_err("template entity is empty (should never happen)");
        return;
    }

    const EcsScript *script = ecs_record_get(world, r, EcsScript);
    if (!script) {
        ecs_err("template is missing script component");
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = template->type_info;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    void *data = ecs_field_w_size(it, flecs_ito(size_t, ti->size), 0);

    if (ecs_is_deferred(it->world)) {
        flecs_script_template_defer_on_set(it, template_entity, ti, data);
        return;
    }

    flecs_script_template_instantiate(
        world, template_entity, it->entities, data, it->count);
    return;
}

static
int flecs_script_template_eval_prop(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_script_template_t *template = v->template;
    if (ecs_vec_count(&v->vars->vars) > 
        ecs_vec_count(&template->prop_defaults)) 
    {
        flecs_script_eval_error(v, node, 
            "const variables declared before prop '%s' (props must come first)", 
                node->name);
        return -1;
    }

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "variable '%s' redeclared", node->name);
        return -1;
    }

    ecs_entity_t type;
    const ecs_type_info_t *ti;

    if (node->type) {
        if (flecs_script_find_entity(v, 0, node->type, NULL, &type) || !type) {
            flecs_script_eval_error(v, node,
                "unresolved type '%s' for prop '%s'", 
                    node->type, node->name);
            return -1;
        }

        ti = flecs_script_get_type_info(v, node, type);
        if (!ti) {
            return -1;
        }

        var->value.type = type;
        var->value.ptr = flecs_stack_alloc(
            &v->r->stack, ti->size, ti->alignment);
        var->type_info = ti;

        if (flecs_script_eval_expr(v, &node->expr, &var->value)) {
            return -1;
        }
    } else {
        /* We don't know the type yet, so we can't create a storage for it yet.
         * Run the expression first to deduce the type. */
        ecs_value_t value = {0};
        if (flecs_script_eval_expr(v, &node->expr, &value)) {
            flecs_script_eval_error(v, node,
                "failed to evaluate expression for const variable '%s'", 
                    node->name);
            return -1;
        }

        ecs_assert(value.type != 0, ECS_INTERNAL_ERROR, NULL);
        ti = ecs_get_type_info(v->world, value.type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        var->value.ptr = flecs_stack_calloc(
            &v->r->stack, ti->size, ti->alignment);
        type = var->value.type = value.type;
        var->type_info = ti;

        if (ti->hooks.ctor) {
            ti->hooks.ctor(var->value.ptr, 1, ti);
        }

        ecs_value_copy_w_type_info(v->world, ti, var->value.ptr, value.ptr);
        ecs_value_fini_w_type_info(v->world, ti, value.ptr);
        flecs_free(&v->world->allocator, ti->size, value.ptr);
    }

    ecs_script_var_t *value = ecs_vec_append_t(&v->base.script->allocator, 
        &template->prop_defaults, ecs_script_var_t);
    value->value.ptr = flecs_calloc_w_dbg_info(
        &v->base.script->allocator, ti->size, ti->name);
    value->value.type = type;
    value->type_info = ti;
    ecs_value_copy_w_type_info(
        v->world, ti, value->value.ptr, var->value.ptr);

    ecs_entity_t mbr = ecs_entity(v->world, {
        .name = node->name,
        .parent = template->entity
    });

    ecs_set(v->world, mbr, EcsMember, { .type = var->value.type });

    return 0;
}

static
int flecs_script_template_eval(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstVarComponent:
    case EcsAstEntity:
    case EcsAstScope:
    case EcsAstDefaultComponent:
    case EcsAstWithVar:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstConst:
    case EcsAstPairScope:
    case EcsAstWith:
    case EcsAstIf:
    case EcsAstFor:
        break;
    case EcsAstTemplate:
        flecs_script_eval_error(v, node, "nested templates are not allowed");
        return -1;
    case EcsAstProp:
        return flecs_script_template_eval_prop(v, (ecs_script_var_node_t*)node);
    }

    return flecs_script_check_node(v, node);
}

static
int flecs_script_template_preprocess(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template)
{
    ecs_visit_action_t prev_visit = v->base.visit;
    v->template = template;

    /* Dummy entity node for instance */
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = template->node->node.pos
        }
    };

    v->entity = &instance_node;

    v->base.visit = (ecs_visit_action_t)flecs_script_template_eval;
    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);
    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, "this");
    var->value.type = ecs_id(ecs_entity_t);
    int result = flecs_script_check_scope(v, template->node->scope);
    v->vars = ecs_script_vars_pop(v->vars);
    v->base.visit = prev_visit;
    v->template = NULL;
    v->entity = NULL;

    return result;
}

static
int flecs_script_template_hoist_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template)
{
    ecs_allocator_t *a = &v->base.script->allocator;
    if (v->module) {
        ecs_vec_append_t(a, &template->using_, ecs_entity_t)[0] = v->module;
    }

    int i, count = ecs_vec_count(&v->r->using);
    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(a, &template->using_, ecs_entity_t)[0] = 
            ecs_vec_get_t(&v->r->using, ecs_entity_t, i)[0];
    }

    return 0;
}

static
int flecs_script_template_hoist_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template,
    ecs_script_vars_t *vars)
{
    int32_t i, count = ecs_vec_count(&vars->vars);
    ecs_script_var_t *src_vars = ecs_vec_first(&vars->vars);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *src = &src_vars[i];
        if (ecs_script_vars_lookup(template->vars, src->name)) {
            /* If variable is masked, don't declare it twice */
            continue;
        }
        ecs_script_var_t *dst = ecs_script_vars_define_id(
            template->vars, src->name, src->value.type);
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_value_copy(v->world,
            src->value.type, dst->value.ptr, src->value.ptr);
    }

    if (vars->parent) {
        flecs_script_template_hoist_vars(v, template, vars->parent);
    }

    return 0;
}

ecs_script_template_t* flecs_script_template_init(
    ecs_script_impl_t *script)
{
    ecs_allocator_t *a = &script->allocator;
    ecs_script_template_t *result = flecs_alloc_t(a, ecs_script_template_t);
    ecs_vec_init_t(NULL, &result->prop_defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->using_, ecs_entity_t, 0);
    result->vars = ecs_script_vars_init(script->pub.world);
    return result;
}

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &script->allocator;

    int32_t i, count = ecs_vec_count(&template->prop_defaults);
    ecs_script_var_t *values = ecs_vec_first(&template->prop_defaults);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *value = &values[i];
        const ecs_type_info_t *ti = value->type_info;
        if (ti->hooks.dtor) {
            ti->hooks.dtor(value->value.ptr, 1, ti);
        }
        flecs_free(a, ti->size, value->value.ptr);
    }

    ecs_vec_fini_t(a, &template->prop_defaults, ecs_script_var_t);

    ecs_vec_fini_t(a, &template->using_, ecs_entity_t);
    ecs_script_vars_fini(template->vars);
    flecs_free_t(a, ecs_script_template_t, template);
}

/* Create new template */
int flecs_script_eval_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *node)
{
    ecs_entity_t template_entity = flecs_script_create_entity(v, node->name);
    if (!template_entity) {
        return -1;
    }

    ecs_script_template_t *template = flecs_script_template_init(v->base.script);
    template->entity = template_entity;
    template->node = node;

    /* Variables are always presented to a template in a well defined order, so
     * we don't need dynamic variable binding. */
    bool old_dynamic_variable_binding = v->dynamic_variable_binding;
    v->dynamic_variable_binding = false;

    if (flecs_script_template_preprocess(v, template)) {
        goto error;
    }

    if (flecs_script_template_hoist_using(v, template)) {
        goto error;
    }

    if (flecs_script_template_hoist_vars(v, template, v->vars)) {
        goto error;
    }

    v->dynamic_variable_binding = old_dynamic_variable_binding;

    /* If template has no props, give template dummy size so we can register
     * hooks for it. */
    if (!ecs_has(v->world, template_entity, EcsComponent)) {
        ecs_set(v->world, template_entity, EcsComponent, {1, 1});
    }

    template->type_info = ecs_get_type_info(v->world, template_entity);

    ecs_add_pair(v->world, template_entity, EcsOnInstantiate, EcsOverride);

    EcsScript *script = ecs_ensure(v->world, template_entity, EcsScript);
    if (script->script) {
        if (script->template_) {
            flecs_script_template_fini(
                flecs_script_impl(script->script), script->template_);
        }
        ecs_script_free(script->script);
    }

    script->script = &v->base.script->pub;
    script->template_ = template;
    ecs_modified(v->world, template_entity, EcsScript);

    ecs_set_hooks_id(v->world, template_entity, &(ecs_type_hooks_t) {
        .ctor = flecs_script_template_ctor,
        .on_set = flecs_script_template_on_set,
        .ctx = v->world
    });

    /* Keep script alive for as long as template is alive */
    v->base.script->refcount ++;

    return 0;
error:
    flecs_script_template_fini(v->base.script, template);
    return -1;
}

void flecs_script_template_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateSetEvent);
    ECS_TAG_DEFINE(world, EcsScriptTemplate);

    ecs_add_id(world, EcsScriptTemplate, EcsPairIsTag);

    ecs_set_hooks(world, EcsScriptTemplateSetEvent, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScriptTemplateSetEvent),
        .dtor = ecs_dtor(EcsScriptTemplateSetEvent),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "TemplateSetObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { ecs_id(EcsScriptTemplateSetEvent) },
        .callback = flecs_on_template_set_event
    });
}

#endif
