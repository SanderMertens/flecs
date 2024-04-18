#include "flecs.h"
#include "script.h"

/* Assembly ctor to initialize with default property values */
static
void flecs_script_assembly_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_entity_t assembly_entity = ti->component;
    
    const EcsStruct *st = ecs_get(world, assembly_entity, EcsStruct);
    if (!st) {
        ecs_os_memset(ptr, 0, count * ti->size);
        return;
    }

    const EcsScript *script = ecs_get(world, assembly_entity, EcsScript);
    if (!script) {
        ecs_err("assembly '%s' is not a script, cannot construct", ti->name);
        return;
    }

    ecs_script_assembly_t *assembly = script->assembly;
    ecs_assert(assembly != NULL, ECS_INTERNAL_ERROR, NULL);
    if (st->members.count != assembly->prop_defaults.count) {
        ecs_err("number of props (%d) of assembly '%s' does not match members"
            " (%d), cannot construct", assembly->prop_defaults.count, 
                ti->name, st->members.count);
        return;
    }

    const ecs_member_t *members = st->members.array;
    int32_t i, m, member_count = st->members.count;
    ecs_script_var_t *values = assembly->prop_defaults.array;
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

/* Assembly on_set handler to update contents for new property values */
static
void flecs_script_assembly_on_set(
    ecs_iter_t *it)
{
    if (it->table->flags & EcsTableIsPrefab) {
        /* Don't instantiate assemblies for prefabs */
        return;
    }

    ecs_world_t *world = it->world;
    ecs_entity_t assembly_entity = ecs_field_id(it, 0);
    ecs_record_t *r = ecs_record_find(world, assembly_entity);
    if (!r) {
        ecs_err("assembly entity is empty (should never happen)");
        return;
    }

    const EcsScript *script = ecs_record_get(world, r, EcsScript);
    if (!script) {
        ecs_err("assembly is missing script component");
        return;
    }

    ecs_script_assembly_t *assembly = script->assembly;
    ecs_assert(assembly != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = assembly->type_info;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    const EcsStruct *st = ecs_record_get(world, r, EcsStruct);

    void *data = ecs_field_w_size(it, flecs_ito(size_t, ti->size), 0);

    ecs_script_eval_visitor_t v;
    flecs_script_eval_visit_init(script->script, &v);
    ecs_vec_t prev_using = v.using;
    v.using = assembly->using_;

    ecs_script_scope_t *scope = assembly->node->scope;

    /* Dummy entity node for instance */
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = assembly->node->node.pos
        },
        .scope = scope
    };

    v.entity = &instance_node;

    int32_t i, m;
    for (i = 0; i < it->count; i ++) {
        v.parent = it->entities[i];
        instance_node.eval = it->entities[i];

        /* Create variables to hold assembly properties */
        ecs_script_vars_t *vars = ecs_script_vars_push(
            NULL, &v.stack, v.allocator);

        /* Populate properties from assembly members */
        if (st) {
            const ecs_member_t *members = st->members.array;
            for (m = 0; m < st->members.count; m ++) {
                const ecs_member_t *member = &members[m];

                /* Assign assembly property from assembly instance */
                ecs_script_var_t *var = ecs_script_vars_declare(vars, member->name);
                var->value.type = member->type;
                var->value.ptr = ECS_OFFSET(data, member->offset);
            }
        }

        /* Populate $this variable with instance entity */
        ecs_entity_t instance = it->entities[i];
        ecs_script_var_t *var = ecs_script_vars_declare(vars, "this");
        var->value.type = ecs_id(ecs_entity_t);
        var->value.ptr = &instance;

        bool is_defer = ecs_is_deferred(world);
        ecs_suspend_readonly_state_t srs;
        ecs_world_t *real_world = NULL;
        if (is_defer) {
            ecs_assert(ecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
            real_world = flecs_suspend_readonly(world, &srs);
            ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        ecs_script_clear(world, assembly_entity, instance);

        /* Run assembly code */
        v.vars = vars;
        ecs_script_visit_scope(&v, scope);

        if (is_defer) {
            flecs_resume_readonly(real_world, &srs);
        }

        /* Pop variable scope */
        ecs_script_vars_pop(vars);

        data = ECS_OFFSET(data, ti->size);
    }

    v.using = prev_using;
    flecs_script_eval_visit_fini(&v);
}

static
int flecs_script_eval_prop(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_script_assembly_t *assembly = v->assembly;
    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "variable '%s' redeclared", node->name);
        return -1;
    }

    if (node->type) {
        ecs_entity_t type = flecs_script_find_entity(v, 0, node->type);
        if (!type) {
            flecs_script_eval_error(v, node,
                "unresolved type '%s' for const variable '%s'", 
                    node->type, node->name);
            return -1;
        }

        const ecs_type_info_t *ti = flecs_script_get_type_info(v, node, type);
        if (!ti) {
            return -1;
        }

        var->value.type = type;
        var->value.ptr = flecs_stack_alloc(&v->stack, ti->size, ti->alignment);
        var->type_info = ti;

        if (flecs_script_eval_expr(v, node->expr, &var->value)) {
            return -1;
        }

        ecs_script_var_t *value = ecs_vec_append_t(&v->base.script->allocator, 
            &assembly->prop_defaults, ecs_script_var_t);
        value->value.ptr = flecs_calloc(&v->base.script->allocator, ti->size);
        value->value.type = type;
        value->type_info = ti;
        ecs_value_copy_w_type_info(
            v->world, ti, value->value.ptr, var->value.ptr);

        ecs_entity_t mbr = ecs_entity(v->world, {
            .name = node->name,
            .parent = assembly->entity
        });

        ecs_set(v->world, mbr, EcsMember, { .type = var->value.type });
    }

    return 0;
}

static
int flecs_script_assembly_eval(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstProp:
        return flecs_script_eval_prop(v, (ecs_script_var_node_t*)node);
    case EcsAstScope:
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstVarComponent:
    case EcsAstDefaultComponent:
    case EcsAstWithVar:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstWith:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstAssembly:
    case EcsAstConst:
    case EcsAstEntity:
    case EcsAstPairScope:
    case EcsAstIf:
        break;
    }

    return flecs_script_eval_node(v, node);
}

static
int flecs_script_assembly_preprocess(
    ecs_script_eval_visitor_t *v,
    ecs_script_assembly_t *assembly)
{
    v->assembly = assembly;
    v->base.visit = (ecs_visit_action_t)flecs_script_assembly_eval;
    v->vars = ecs_script_vars_push(v->vars, &v->stack, v->allocator);
    int result = ecs_script_visit_scope(v, assembly->node->scope);
    v->vars = ecs_script_vars_pop(v->vars);
    v->assembly = NULL;
    return result;
}

static
int flecs_script_assembly_hoist_using(
    ecs_script_eval_visitor_t *v,
    ecs_script_assembly_t *assembly)
{
    if (v->module) {
        ecs_vec_append_t(
            v->allocator, &assembly->using_, ecs_entity_t)[0] = v->module;
    }

    int i, count = ecs_vec_count(&v->using);
    for (i = 0; i < count; i ++) {
        ecs_vec_append_t(v->allocator, &assembly->using_, ecs_entity_t)[0] = 
            ecs_vec_get_t(&v->using, ecs_entity_t, i)[0];
    }

    return 0;
}

ecs_script_assembly_t* flecs_script_assembly_init(
    ecs_script_t *script)
{
    ecs_allocator_t *a = &script->allocator;
    ecs_script_assembly_t *result = flecs_alloc_t(a, ecs_script_assembly_t);
    ecs_vec_init_t(NULL, &result->prop_defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->using_, ecs_entity_t, 0);
    return result;
}

void flecs_script_assembly_fini(
    ecs_script_t *script,
    ecs_script_assembly_t *assembly)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &script->allocator;

    int32_t i, count = ecs_vec_count(&assembly->prop_defaults);
    ecs_script_var_t *values = ecs_vec_first(&assembly->prop_defaults);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *value = &values[i];
        const ecs_type_info_t *ti = value->type_info;
        if (ti->hooks.dtor) {
            ti->hooks.dtor(value->value.ptr, 1, ti);
        }
        flecs_free(a, ti->size, value->value.ptr);
    }

    ecs_vec_fini_t(a, &assembly->prop_defaults, ecs_script_var_t);
    ecs_vec_fini_t(a, &assembly->using_, ecs_entity_t);
    flecs_free_t(a, ecs_script_assembly_t, assembly);
}

/* Create new assembly */
int flecs_script_eval_assembly(
    ecs_script_eval_visitor_t *v,
    ecs_script_assembly_node_t *node)
{
    ecs_entity_t assembly_entity = flecs_script_create_entity(v, node->name);
    if (!assembly_entity) {
        return -1;
    }

    ecs_script_assembly_t *assembly = flecs_script_assembly_init(v->base.script);
    assembly->entity = assembly_entity;
    assembly->node = node;

    if (flecs_script_assembly_preprocess(v, assembly)) {
        goto error;
    }

    if (flecs_script_assembly_hoist_using(v, assembly)) {
        goto error;
    }

    /* If assembly has no props, give assembly dummy size so we can register
     * hooks for it. */
    if (!ecs_has(v->world, assembly_entity, EcsComponent)) {
        ecs_set(v->world, assembly_entity, EcsComponent, {1, 1});
    }

    assembly->type_info = ecs_get_type_info(v->world, assembly_entity);

    ecs_add_id(v->world, assembly_entity, EcsAlwaysOverride);

    EcsScript *script = ecs_ensure(v->world, assembly_entity, EcsScript);
    script->script = v->base.script;
    script->assembly = assembly;
    ecs_modified(v->world, assembly_entity, EcsScript);

    ecs_set_hooks_id(v->world, assembly_entity, &(ecs_type_hooks_t) {
        .ctor = flecs_script_assembly_ctor,
        .on_set = flecs_script_assembly_on_set,
        // .on_remove = flecs_assembly_on_remove,
        .ctx = v->world
    });

    /* Keep script alive for as long as assembly is alive */
    v->base.script->refcount ++;

    return 0;
error:
    flecs_script_assembly_fini(v->base.script, assembly);
    return -1;
}
