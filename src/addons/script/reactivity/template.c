/**
 * @file addons/script/reactivity/template.c
 * @brief Script template implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

ECS_COMPONENT_DECLARE(EcsScriptTemplateSetEvent);
ECS_COMPONENT_DECLARE(EcsScriptTemplateInstanceUpdateEvent);
ECS_COMPONENT_DECLARE(EcsScriptTemplateRoot);
ECS_DECLARE(EcsScriptTemplate);

static void flecs_script_template_root_fini(
    EcsScriptTemplateRoot *root)
{
    ecs_vec_fini_t(NULL, &root->observers, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &root->symbol_slots, ecs_script_symbol_slot_t);
    ecs_vec_fini_t(NULL, &root->component_slots,
        ecs_script_component_slot_t);
    ecs_vec_fini_t(NULL, &root->scope_slots, int32_t);
    flecs_script_for_slots_fini(&root->for_slots);
}

static ECS_CTOR(EcsScriptTemplateRoot, ptr, {
    ecs_vec_init_t(NULL, &ptr->observers, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &ptr->symbol_slots, ecs_script_symbol_slot_t, 0);
    ecs_vec_init_t(NULL, &ptr->component_slots,
        ecs_script_component_slot_t, 0);
    ecs_vec_init_t(NULL, &ptr->scope_slots, int32_t, 0);
    ecs_vec_init_t(NULL, &ptr->for_slots, ecs_script_for_slot_t, 0);
    ptr->changed = 0;
    ptr->visit = 0;
    ptr->initialized = false;
})

static ECS_MOVE(EcsScriptTemplateRoot, dst, src, {
    flecs_script_template_root_fini(dst);
    *dst = *src;
    ecs_vec_init_t(NULL, &src->observers, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &src->symbol_slots, ecs_script_symbol_slot_t, 0);
    ecs_vec_init_t(NULL, &src->component_slots,
        ecs_script_component_slot_t, 0);
    ecs_vec_init_t(NULL, &src->scope_slots, int32_t, 0);
    ecs_vec_init_t(NULL, &src->for_slots, ecs_script_for_slot_t, 0);
    src->changed = 0;
    src->visit = 0;
    src->initialized = false;
})

static ECS_DTOR(EcsScriptTemplateRoot, ptr, {
    flecs_script_template_root_fini(ptr);
})

static void flecs_script_template_root_on_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    EcsScriptTemplateRoot *ptr = ecs_field_w_size(
        it, sizeof(EcsScriptTemplateRoot), 0);

    int32_t i, j;
    for (i = 0; i < it->count; i ++) {
        ecs_vec_t *observers = &ptr[i].observers;
        ecs_script_ref_t *obs = ecs_vec_first(observers);
        int32_t count = ecs_vec_count(observers);
        for (j = 0; j < count; j ++) {
            if (obs[j].observer && ecs_is_alive(world, obs[j].observer)) {
                ecs_delete(world, obs[j].observer);
            }
        }
    }
}

static void flecs_script_template_root_init(
    EcsScriptTemplateRoot *root,
    ecs_script_template_t *template,
    ecs_script_impl_t *impl)
{
    if (ecs_vec_count(&root->symbol_slots) == template->symbol_count) {
        return;
    }
    ecs_vec_set_count_t(NULL, &root->symbol_slots,
        ecs_script_symbol_slot_t, template->symbol_count);
    if (template->symbol_count) {
        ecs_os_memset(ecs_vec_first(&root->symbol_slots), 0,
            template->symbol_count * ECS_SIZEOF(ecs_script_symbol_slot_t));
        ecs_script_symbol_slot_t *dst = ecs_vec_first(&root->symbol_slots);
        ecs_script_symbol_slot_t *src = ecs_vec_first(&impl->symbol_slots);
        int32_t i;
        for (i = 0; i < template->symbol_count; i ++) {
            dst[i].scope_slot = src[template->symbol_offset + i].scope_slot;
        }
    }
    ecs_vec_set_count_t(NULL, &root->component_slots,
        ecs_script_component_slot_t, template->component_count);
    if (template->component_count) {
        ecs_os_memset(ecs_vec_first(&root->component_slots), 0,
            template->component_count *
                ECS_SIZEOF(ecs_script_component_slot_t));
    }
    ecs_vec_set_count_t(NULL, &root->scope_slots,
        int32_t, template->scope_count);
    if (template->scope_count) {
        ecs_os_memset(ecs_vec_first(&root->scope_slots), 0,
            template->scope_count * ECS_SIZEOF(int32_t));
    }
    flecs_script_for_slots_init(&root->for_slots, template->for_count);
}

static void flecs_script_template_root_clear(
    ecs_world_t *world,
    ecs_script_template_t *template,
    ecs_script_impl_t *impl,
    EcsScriptTemplateRoot *root)
{
    ecs_vec_t symbol_slots = root->symbol_slots;
    ecs_vec_t component_slots = root->component_slots;
    ecs_vec_t scope_slots = root->scope_slots;
    ecs_vec_t for_slots = root->for_slots;

    ecs_script_for_slot_t *for_slot_array = ecs_vec_first(&for_slots);
    int32_t i, count = ecs_vec_count(&for_slots);
    for (i = 0; i < count; i ++) {
        flecs_script_for_slot_clear(world, &for_slot_array[i], true);
    }

    int32_t root_symbol = template->root_symbol - template->symbol_offset;
    if (root_symbol >= 0 && root_symbol < ecs_vec_count(&symbol_slots)) {
        ecs_vec_get_t(&symbol_slots,
            ecs_script_symbol_slot_t, root_symbol)->scope_slot = -1;
    }
    if (ecs_vec_count(&scope_slots)) {
        ecs_os_memset(ecs_vec_first(&scope_slots), 0,
            ecs_vec_count(&scope_slots) * ECS_SIZEOF(int32_t));
    }

    ecs_script_eval_visitor_t v = {
        .base.script = impl,
        .world = world,
        .symbol_slots = &symbol_slots,
        .component_slots = &component_slots,
        .scope_slots = &scope_slots,
        .for_slots = &for_slots,
        .instance_template = template,
        .visit = 1
    };
    flecs_script_eval_cleanup(&v);
}

static void flecs_template_set_event_free(EcsScriptTemplateSetEvent *ptr) {
    if (ptr->entities != &ptr->entity_storage) {
        ecs_os_free(ptr->entities);
    }
    if (ptr->inputs != &ptr->input_storage) {
        ecs_os_free(ptr->inputs);
    }
    if (ptr->data != ptr->data_storage) {
        ecs_os_free(ptr->data);
    }
}

static ECS_MOVE(EcsScriptTemplateSetEvent, dst, src, {
    flecs_template_set_event_free(dst);

    *dst = *src;

    if (src->entities == &src->entity_storage) {
        dst->entities = &dst->entity_storage;
    }

    if (src->inputs == &src->input_storage) {
        dst->inputs = &dst->input_storage;
    }

    if (src->data == src->data_storage) {
        dst->data = &dst->data_storage;
    }

    src->entities = NULL;
    src->inputs = NULL;
    src->data = NULL;
})

static ECS_DTOR(EcsScriptTemplateSetEvent, ptr, {
    flecs_template_set_event_free(ptr);
})

static void flecs_script_template_on_add(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t template_entity = ecs_field_id(it, 0);

    if (!ecs_is_alive(world, template_entity)) {
        return;
    }

    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    if (!script || !script->template_) {
        return;
    }

    script->template_->refcount += it->count;
}

/* Template component ctor to initialize prop or mut default values */
static void flecs_script_template_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti,
    bool mut)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_entity_t component = ti->component;
    ecs_entity_t template_entity = mut
        ? ecs_get_parent(world, component)
        : component;

    /* Zero-initialize memory so that hooks can safely run destructors. */
    flecs_default_ctor(ptr, count, ti);

    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    if (!script || !script->template_) {
        ecs_err("template component '%s' has no template, cannot construct",
            ti->name);
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_script_template_vars_t *vars = mut ? &template->muts : &template->props;
    if (vars->type != component) {
        ecs_err("component '%s' does not belong to template '%s'",
            ti->name, ecs_get_name(world, template_entity));
        return;
    }

    const EcsStruct *st = ecs_get(world, component, EcsStruct);
    if (!st) {
        return;
    }

    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
    if (st->members.count != ecs_vec_count(&vars->defaults)) {
        ecs_err("number of defaults (%d) for template component '%s' does not "
            "match members (%d), cannot construct",
                ecs_vec_count(&vars->defaults), ti->name, st->members.count);
        return;
    }

    const ecs_member_t *members = st->members.array;
    int32_t i, m, member_count = st->members.count;
    ecs_script_var_t *values = ecs_vec_first(&vars->defaults);
    for (m = 0; m < member_count; m ++) {
        const ecs_member_t *member = &members[m];
        ecs_script_var_t *value = &values[m];
        const ecs_type_info_t *mti = value->type_info;
        ecs_assert(mti != NULL, ECS_INTERNAL_ERROR, NULL);

        for (i = 0; i < count; i ++) {
            void *el = ECS_ELEM(ptr, ti->size, i);
            ecs_ptr_copy_w_type_info(world, mti, 
                ECS_OFFSET(el, member->offset), value->value.ptr);
        }
    }
}

static void flecs_script_template_props_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_script_template_ctor(ptr, count, ti, false);
}

static void flecs_script_template_muts_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    flecs_script_template_ctor(ptr, count, ti, true);
}

/* Defer template instantiation if we're in deferred mode. */
static void flecs_script_template_defer_on_set(
    ecs_iter_t *it,
    ecs_entity_t template_entity,
    ecs_entity_t component,
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

    if (it->count == 1) {
        evt.inputs = &evt.input_storage;
    } else {
        evt.inputs = ecs_os_malloc_n(uint64_t, it->count);
    }

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        const EcsScriptTemplateRoot *root = ecs_get_pair(it->real_world,
            it->entities[i], EcsScriptTemplateRoot, template_entity);
        evt.inputs[i] = root && root->initialized
            ? root->changed
            : UINT64_MAX;
    }

    evt.count = it->count;
    evt.template_entity = template_entity;
    evt.component = component;

    ecs_enqueue(it->world, &(ecs_event_desc_t){
        .event = ecs_id(EcsScriptTemplateSetEvent),
        .entity = EcsAny,
        .param = &evt
    });
}

static void flecs_script_template_ref_on_set(
    ecs_iter_t *it);

static void flecs_script_template_instance_ref_on_set(
    ecs_iter_t *it);

static void flecs_script_template_update_instance_observers(
    ecs_world_t *world,
    ecs_script_template_t *template,
    ecs_entity_t template_entity,
    ecs_entity_t instance,
    const ecs_script_vars_t *vars)
{
    int32_t i, j, count = ecs_vec_count(&template->dynamic_refs);
    ecs_script_ref_t *dynamic_refs = ecs_vec_first(&template->dynamic_refs);

    ecs_vec_t refs;
    ecs_vec_init_t(NULL, &refs, ecs_script_ref_t, count);
    for (i = 0; i < count; i ++) {
        const ecs_script_var_t *var = ecs_script_vars_lookup(
            vars, dynamic_refs[i].name);
        if (!var || var->value.type != ecs_id(ecs_entity_t) ||
            !var->value.ptr)
        {
            continue;
        }

        ecs_entity_t entity = *(ecs_entity_t*)var->value.ptr;
        if (!entity) {
            continue;
        }

        ecs_id_t component = dynamic_refs[i].component;
        bool is_has = dynamic_refs[i].is_has;
        ecs_script_ref_t *elems = ecs_vec_first(&refs);
        int32_t resolved_count = ecs_vec_count(&refs);
        for (j = 0; j < resolved_count; j ++) {
            if (elems[j].entity == entity &&
                elems[j].component == component &&
                elems[j].is_has == is_has)
            {
                break;
            }
        }
        if (j != resolved_count) {
            continue;
        }

        ecs_script_ref_t *ref = ecs_vec_append_t(NULL, &refs, ecs_script_ref_t);
        ref->entity = entity;
        ref->name = NULL;
        ref->component = component;
        ref->observer = 0;
        ref->input = dynamic_refs[i].input;
        ref->is_has = is_has;
        ref->is_resolve = false;
    }

    EcsScriptTemplateRoot *root = ecs_ensure_pair(
        world, instance, EcsScriptTemplateRoot, template_entity);
    ecs_assert(root != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_script_update_ref_observers(world, template_entity, instance, &refs,
        &root->observers, flecs_script_template_instance_ref_on_set);

    ecs_vec_fini_t(NULL, &refs, ecs_script_ref_t);
}

/* Returns a stack allocated copy of the instance data, or NULL if no copy was
 * made. A copy must be released with flecs_script_template_free_data. */
static void* flecs_script_template_copy_data(
    ecs_script_eval_visitor_t *v,
    const ecs_type_info_t *ti,
    void *data)
{
    if (!data || !ti || !ti->size) {
        return NULL;
    }

    void *result = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
    ecs_os_memcpy(result, data, ti->size);
    return result;
}

static void flecs_script_template_free_data(
    const ecs_type_info_t *ti,
    void *data)
{
    if (!data) {
        return;
    }

    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_stack_free(data, ti->size);
}

static void flecs_script_template_instantiate_vars(
    ecs_script_vars_t *vars,
    const ecs_script_template_t *template,
    const EcsStruct *props_st,
    void *props_data,
    const EcsStruct *muts_st,
    void *muts_data)
{
    ecs_script_template_member_t *template_members =
        ecs_vec_first(&template->members);
    int32_t m, member_count = ecs_vec_count(&template->members);
    for (m = 0; m < member_count; m ++) {
        ecs_script_template_member_t *template_member = &template_members[m];
        const EcsStruct *st = template_member->is_mut ? muts_st : props_st;
        void *data = template_member->is_mut ? muts_data : props_data;
        ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

        const ecs_member_t *member = ecs_vec_get_t(
            &st->members, ecs_member_t, template_member->index);
        ecs_script_var_t *var = ecs_script_vars_declare(vars, member->name);
        ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
        var->value.type = member->type;
        var->value.ptr = ECS_OFFSET(data, member->offset);
        var->owned = false;
    }
}

static int flecs_script_template_instantiate(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *data,
    int32_t count,
    uint64_t input,
    bool allow_stale_entities)
{
    (void)allow_stale_entities;

    ecs_assert(!ecs_is_deferred(world), ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *r = ecs_record_find(world, template_entity);
    if (!r) {
        ecs_err("template entity is empty (should never happen)");
        return -1;
    }

    const EcsScript *script = ecs_record_get(world, r, EcsScript);
    if (!script) {
        ecs_err("template is missing script component");
        return -1;
    }

    ecs_script_template_t *template = script->template_;
    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = component
        ? ecs_get_type_info(world, component)
        : NULL;
    ecs_assert(!component || ti != NULL, ECS_INTERNAL_ERROR, NULL);
    const EcsStruct *props_st = ecs_record_get(world, r, EcsStruct);
    const EcsStruct *muts_st = template->muts.type
        ? ecs_get(world, template->muts.type, EcsStruct)
        : NULL;
    const ecs_type_info_t *muts_ti = template->muts.type
        ? ecs_get_type_info(world, template->muts.type)
        : NULL;

    ecs_script_runner_t runner;
    ecs_script_eval_visitor_t *v = &runner.v;
    ecs_script_eval_desc_t desc = {
        .runtime = flecs_script_runtime_get(world)
    };

    flecs_script_runner_init(&runner, flecs_script_impl(script->script), &desc);
    ecs_vec_t prev_using = v->r->using;
    ecs_vec_t prev_with = desc.runtime->with;
    ecs_vec_t prev_with_type_info = desc.runtime->with_type_info;

    v->r->using = template->using_;
    v->template_entity = template_entity;
    v->instance_template = template;
    v->symbol_offset = template->symbol_offset;
    ecs_vec_init_t(NULL, &desc.runtime->with, ecs_value_t, 0);
    ecs_vec_init_t(NULL, &desc.runtime->with_type_info, ecs_type_info_t*, 0);

    ecs_script_scope_t *scope = template->node->scope;
    ecs_vec_t symbol_slots = {0};
    ecs_vec_t component_slots = {0};
    ecs_vec_t scope_slots = {0};
    ecs_vec_t for_slots = {0};

    /* Dummy entity node for instance */
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = template->node->node.pos
        },
        .scope = scope,
        .symbol = template->root_symbol
    };

    flecs_script_entity_state_t instance_state = {
        .node = &instance_node
    };

    v->entity = &instance_state;

    int result = 0;
    int32_t i, a;
    for (i = 0; i < count; i ++) {
        v->parent = entities[i];
        if (!ecs_is_alive(world, v->parent)) {
            ecs_assert(allow_stale_entities, ECS_INTERNAL_ERROR, NULL);
            if (data) {
                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                data = ECS_OFFSET(data, ti->size);
            }
            continue;
        }
        if (!ecs_has_id(world, v->parent, template_entity)) {
            if (data) {
                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                data = ECS_OFFSET(data, ti->size);
            }
            continue;
        }
        if (v->parent == template_entity) {
            ecs_err("cannot instantiate template '%s' on itself",
                ecs_get_name(world, template_entity));
            result = -1;
            break;
        }

        EcsScriptTemplateRoot *root = ecs_ensure_pair(
            world, entities[i], EcsScriptTemplateRoot, template_entity);
        flecs_script_template_root_init(
            root, template, flecs_script_impl(script->script));
        symbol_slots = root->symbol_slots;
        component_slots = root->component_slots;
        scope_slots = root->scope_slots;
        for_slots = root->for_slots;
        v->symbol_slots = &symbol_slots;
        v->component_slots = &component_slots;
        v->scope_slots = &scope_slots;
        v->for_slots = &for_slots;
        uint64_t run_input = root->initialized ? input : UINT64_MAX;
        if (!run_input) {
            if (data) {
                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                data = ECS_OFFSET(data, ti->size);
            }
            continue;
        }
        if (root->visit == INT32_MAX) {
            if (ecs_vec_count(&root->scope_slots)) {
                ecs_os_memset(ecs_vec_first(&root->scope_slots), 0,
                    ecs_vec_count(&root->scope_slots) * ECS_SIZEOF(int32_t));
            }
            root->visit = 1;
        } else {
            root->visit ++;
        }
        flecs_script_eval_begin(v, run_input, root->visit);
        int32_t root_symbol = template->root_symbol - template->symbol_offset;
        ecs_script_symbol_slot_t *root_slot = ecs_vec_get_t(
            &root->symbol_slots, ecs_script_symbol_slot_t, root_symbol);
        root_slot->entity = entities[i];
        root_slot->scope_slot = scope->scope_slot;

        instance_state.eval = entities[i];
        instance_state.symbol = root_symbol;

        /* Apply annotations, if any */
        bool annot_failed = false;
        for (a = 0; a < ecs_vec_count(&template->annot); a ++) {
            ecs_script_annot_t *annot = ecs_vec_get_t(
                &template->annot, ecs_script_annot_t*, a)[0];
            if (flecs_script_apply_annot(
                v, &instance_node, entities[i], annot))
            {
                annot_failed = true;
                break;
            }
        }
        if (annot_failed) {
            result = -1;
            break;
        }

        /* Create variables to hold template properties and mutable state */
        ecs_script_vars_t *vars = flecs_script_vars_push(
            NULL, &v->r->stack, &v->r->allocator);
        vars->parent = template->vars; /* Include hoisted variables */
        vars->sp = ecs_vec_count(&template->vars->vars);

        /* Allocate enough space for variables */
        ecs_script_vars_set_size(vars,
            ecs_vec_count(&template->members) + 1);

        /* Populate $this variable with instance entity */
        ecs_entity_t instance = entities[i];
        ecs_script_var_t *this_var = ecs_script_vars_declare(
            vars, "this");
        this_var->value.type = ecs_id(ecs_entity_t);
        this_var->value.ptr = &instance;
        this_var->owned = false;

        void *props_data = component == template->props.type
            ? data
            : ECS_CONST_CAST(void*,
                ecs_get_id(world, instance, template->props.type));
        void *muts_data = component == template->muts.type
            ? data
            : (template->muts.type
                ? ECS_CONST_CAST(void*,
                    ecs_get_id(world, instance, template->muts.type))
                : NULL);

        void *props_copy = flecs_script_template_copy_data(
            v, template->type_info, props_data);
        if (props_copy) {
            props_data = props_copy;
        }

        void *muts_copy = flecs_script_template_copy_data(
            v, muts_ti, muts_data);
        if (muts_copy) {
            muts_data = muts_copy;
        }

        flecs_script_template_instantiate_vars(vars, template,
            props_st, props_data, muts_st, muts_data);

        v->vars = vars;

        /* Run template code */
        if (flecs_script_runner_run_scope(&runner, scope) !=
            FlecsScriptRunDone)
        {
            result = -1;
            flecs_script_template_free_data(template->type_info, props_copy);
            flecs_script_template_free_data(muts_ti, muts_copy);
            ecs_script_vars_pop(vars);
            break;
        }

        flecs_script_eval_cleanup(v);
        root = ecs_ensure_pair(
            world, entities[i], EcsScriptTemplateRoot, template_entity);
        root->initialized = true;
        root->changed = 0;

        if (ecs_vec_count(&template->dynamic_refs)) {
            flecs_script_template_update_instance_observers(
                world, template, template_entity, instance, vars);
        }

        flecs_script_template_free_data(template->type_info, props_copy);
        flecs_script_template_free_data(muts_ti, muts_copy);

        /* Pop variable scope */
        ecs_script_vars_pop(vars);

        if (data) {
            ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
            data = ECS_OFFSET(data, ti->size);
        }
    }

    ecs_vec_fini_t(&desc.runtime->allocator, 
        &desc.runtime->with, ecs_value_t);
    ecs_vec_fini_t(&desc.runtime->allocator, 
        &desc.runtime->with_type_info, ecs_type_info_t*);

    v->r->with = prev_with;
    v->r->with_type_info = prev_with_type_info;
    v->r->using = prev_using;
    v->symbol_slots = NULL;
    v->component_slots = &v->base.script->component_slots;
    v->scope_slots = &v->base.script->scope_slots;
    v->for_slots = &v->base.script->for_slots;
    flecs_script_runner_fini(&runner, &desc);

    if (count && ecs_vec_count(&template->refs) &&
        !ecs_vec_count(&template->observers))
    {
        flecs_script_update_ref_observers(world, template_entity, 0,
            &template->refs, &template->observers,
            flecs_script_template_ref_on_set);
    }

    if (result) {
        if (!desc.runtime->error_name && script->script->name) {
            desc.runtime->error_name = ecs_os_strdup(script->script->name);
        }
        desc.runtime->error = true;
    }

    return result;
}

static void flecs_script_template_ref_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    ecs_entity_t template_entity = ctx->script;
    ecs_world_t *world = it->real_world;

    if (!ecs_is_alive(world, template_entity)) {
        return;
    }

    const EcsScript *s = ecs_get(world, template_entity, EcsScript);
    if (!s || !s->template_) {
        return;
    }

    const ecs_type_info_t *ti = s->template_->type_info;
    if (!ti) {
        return;
    }

    bool is_deferred = ecs_is_deferred(world);
    if (is_deferred) {
        ecs_defer_suspend(world);
    }

    /* Gather all instances first so re-instantiation doesn't invalidate the
     * iterator by modifying the tables it iterates. */
    ecs_vec_t instances;
    ecs_vec_init_t(NULL, &instances, ecs_entity_t, 0);

    ecs_iter_t each_it = ecs_each_id(world, template_entity);
    while (ecs_each_next(&each_it)) {
        int32_t i;
        for (i = 0; i < each_it.count; i ++) {
            ecs_vec_append_t(NULL, &instances, ecs_entity_t)[0] =
                each_it.entities[i];
        }
    }

    ecs_entity_t *entities = ecs_vec_first(&instances);
    int32_t i, count = ecs_vec_count(&instances);
    for (i = 0; i < count; i ++) {
        ecs_entity_t instance = entities[i];
        void *data = ECS_CONST_CAST(void*,
            ecs_get_id(world, instance, template_entity));
        if (data) {
            flecs_script_template_instantiate(
                world, template_entity, template_entity,
                &instance, data, 1, ctx->input, false);
        }
    }

    ecs_vec_fini_t(NULL, &instances, ecs_entity_t);

    if (is_deferred) {
        ecs_defer_resume(world);
    }
}

static void flecs_script_template_instance_update(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t instance,
    uint64_t input)
{
    if (!ecs_is_alive(world, template_entity)) {
        return;
    }

    const EcsScript *s = ecs_get(world, template_entity, EcsScript);
    if (!s || !s->template_) {
        return;
    }

    if (!ecs_is_alive(world, instance)) {
        return;
    }

    void *data = ECS_CONST_CAST(void*,
        ecs_get_id(world, instance, template_entity));
    if (!data) {
        return;
    }

    bool is_deferred = ecs_is_deferred(world);
    if (is_deferred) {
        ecs_defer_suspend(world);
    }

    flecs_script_template_instantiate(
        world, template_entity, template_entity,
        &instance, data, 1, input, false);

    if (is_deferred) {
        ecs_defer_resume(world);
    }
}

static void flecs_script_template_instance_ref_on_set(
    ecs_iter_t *it)
{
    ecs_script_ref_ctx_t *ctx = it->ctx;
    ecs_entity_t template_entity = ctx->script;
    ecs_entity_t instance = ctx->instance;

    if (it->event == EcsOnRemove && ecs_is_deferred(it->world)) {
        EcsScriptTemplateInstanceUpdateEvent evt = {
            .template_entity = template_entity,
            .instance = instance,
            .input = ctx->input
        };
        ecs_enqueue(it->world, &(ecs_event_desc_t){
            .event = ecs_id(EcsScriptTemplateInstanceUpdateEvent),
            .entity = EcsAny,
            .param = &evt
        });
        return;
    }

    flecs_script_template_instance_update(
        it->real_world, template_entity, instance, ctx->input);
}

static void flecs_on_template_instance_update_event(
    ecs_iter_t *it)
{
    EcsScriptTemplateInstanceUpdateEvent *evt = it->param;
    ecs_world_t *world = it->real_world;
    ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);

    flecs_script_template_instance_update(
        world, evt->template_entity, evt->instance, evt->input);
}

static void flecs_on_template_set_event(
    ecs_iter_t *it)
{
    ecs_assert(ecs_is_deferred(it->world), ECS_INTERNAL_ERROR, NULL);

    EcsScriptTemplateSetEvent *evt = it->param;
    ecs_world_t *world = it->real_world;
    ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);

    ecs_defer_suspend(world);

    const ecs_type_info_t *ti = ecs_get_type_info(world, evt->component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t i;
    for (i = 0; i < evt->count; i ++) {
        void *data = ECS_OFFSET(evt->data, ti->size * i);
        flecs_script_template_instantiate(
            world, evt->template_entity, evt->component,
            &evt->entities[i], data, 1, evt->inputs[i], true);
    }

    ecs_defer_resume(world);
}

/* Template on_set handler to update contents for new property values */
static void flecs_script_template_on_set(
    ecs_iter_t *it,
    ecs_entity_t template_entity)
{
    if (it->table->flags & EcsTableIsPrefab) {
        /* Don't instantiate templates for prefabs */
        return;
    }

    ecs_world_t *world = it->world;
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

    ecs_assert(script->template_ != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t component = ecs_field_id(it, 0);
    ecs_assert(component == script->template_->props.type ||
        component == script->template_->muts.type,
        ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    void *data = ecs_field_w_size(it, flecs_ito(size_t, ti->size), 0);

    if (ecs_is_deferred(it->world)) {
        flecs_script_template_defer_on_set(
            it, template_entity, component, ti, data);
        return;
    }

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        const EcsScriptTemplateRoot *root = ecs_get_pair(world,
            it->entities[i], EcsScriptTemplateRoot, template_entity);
        uint64_t input = root && root->initialized
            ? root->changed
            : UINT64_MAX;
        flecs_script_template_instantiate(
            world, template_entity, component,
            &it->entities[i], ECS_OFFSET(data, ti->size * i), 1,
            input, false);
    }
}

static void flecs_script_template_on_replace(
    ecs_iter_t *it,
    ecs_entity_t template_entity,
    bool mut)
{
    ecs_world_t *world = it->world;
    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    if (!script || !script->template_) {
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_entity_t component = ecs_field_id(it, 0);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    const EcsStruct *st = ecs_get(world, component, EcsStruct);
    if (!ti || !st) {
        return;
    }

    void *old_data = ecs_field_w_size(it, ti->size, 0);
    void *new_data = ecs_field_w_size(it, ti->size, 1);
    ecs_script_template_member_t *members = ecs_vec_first(&template->members);
    int32_t i, m, member_count = ecs_vec_count(&template->members);
    for (i = 0; i < it->count; i ++) {
        EcsScriptTemplateRoot *root = ECS_CONST_CAST(EcsScriptTemplateRoot*,
            ecs_get_pair(world, it->entities[i],
                EcsScriptTemplateRoot, template_entity));
        if (!root || !root->initialized) {
            continue;
        }
        void *old_ptr = ECS_OFFSET(old_data, ti->size * i);
        void *new_ptr = ECS_OFFSET(new_data, ti->size * i);
        for (m = 0; m < member_count; m ++) {
            ecs_script_template_member_t *template_member = &members[m];
            if (template_member->is_mut != mut) {
                continue;
            }
            const ecs_member_t *member = ecs_vec_get_t(
                &st->members, ecs_member_t, template_member->index);
            const ecs_type_info_t *member_ti = ecs_get_type_info(
                world, member->type);
            ecs_assert(member_ti != NULL, ECS_INTERNAL_ERROR, NULL);
            if (!flecs_type_info_equals(
                ECS_OFFSET(old_ptr, member->offset),
                ECS_OFFSET(new_ptr, member->offset), member_ti))
            {
                root->changed |= template_member->input;
            }
        }
    }
}

static void flecs_script_template_on_props_replace(
    ecs_iter_t *it)
{
    flecs_script_template_on_replace(it, ecs_field_id(it, 0), false);
}

static void flecs_script_template_on_muts_replace(
    ecs_iter_t *it)
{
    ecs_entity_t muts_entity = ecs_field_id(it, 0);
    ecs_entity_t template_entity = ecs_get_parent(it->world, muts_entity);
    flecs_script_template_on_replace(it, template_entity, true);
}

static void flecs_script_template_on_props_set(
    ecs_iter_t *it)
{
    flecs_script_template_on_set(it, ecs_field_id(it, 0));
}

static void flecs_script_template_on_muts_set(
    ecs_iter_t *it)
{
    ecs_entity_t muts_entity = ecs_field_id(it, 0);
    ecs_entity_t template_entity = ecs_get_parent(it->world, muts_entity);
    flecs_script_template_on_set(it, template_entity);
}

static void flecs_script_template_delete_observers(
    ecs_world_t *world,
    ecs_script_template_t *template)
{
    ecs_script_ref_t *obs = ecs_vec_first(&template->observers);
    int32_t i, count = ecs_vec_count(&template->observers);
    for (i = 0; i < count; i ++) {
        if (obs[i].observer && ecs_is_alive(world, obs[i].observer)) {
            ecs_delete(world, obs[i].observer);
        }
    }
    ecs_vec_clear(&template->observers);
}

static void flecs_script_template_on_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t template_entity = ecs_field_id(it, 0);

    if (!ecs_is_alive(world, template_entity)) {
        return;
    }

    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    if (!script || !script->template_) {
        return;
    }

    ecs_script_template_t *template = script->template_;
    ecs_script_impl_t *impl = flecs_script_impl(script->script);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        EcsScriptTemplateRoot *root = ECS_CONST_CAST(EcsScriptTemplateRoot*,
            ecs_get_pair(world, it->entities[i],
                EcsScriptTemplateRoot, template_entity));
        if (root) {
            flecs_script_template_root_clear(
                world, template, impl, root);
        }
        ecs_remove_pair(world, it->entities[i],
            ecs_id(EcsScriptTemplateRoot), template_entity);
    }

    template->refcount -= it->count;
    if (template->refcount <= 0) {
        template->refcount = 0;
        flecs_script_template_delete_observers(world, template);
    }
}

int flecs_script_template_eval_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node,
    bool mut)
{
    ecs_script_template_t *template = v->template;
    ecs_script_template_vars_t *vars = mut ?
        &template->muts : &template->props;

    if (ecs_vec_count(&v->vars->vars) >
        ecs_vec_count(&template->members))
    {
        flecs_script_eval_error(v, node,
            "const variables declared before %s '%s' (props and muts must "
            "come first)", mut ? "mut" : "prop", node->name);
        return -1;
    }

    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, node->name);
    if (!var) {
        flecs_script_eval_error(v, node, 
            "variable '%s' redeclared", node->name);
        return -1;
    }

    ecs_entity_t type = node->eval_type;
    ecs_assert(type != 0, ECS_INTERNAL_ERROR, NULL);
    const ecs_type_info_t *ti = flecs_script_get_type_info(v, node, type);
    if (!ti) {
        return -1;
    }
    if (ecs_set_rtt_equals(v->world, type)) {
        flecs_script_eval_error(v, node,
            "type '%s' cannot be compared", ti->name);
        return -1;
    }
    ti = ecs_get_type_info(v->world, type);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    var->value.type = type;
    var->value.ptr = flecs_stack_calloc(
        &v->r->stack, ti->size, ti->alignment);
    var->type_info = ti;
    var->owned = true;
    node->sp = var->sp;
    flecs_type_info_ctor(var->value.ptr, 1, ti);

    if (flecs_script_eval_expr(v, &node->expr, &var->value)) {
        return -1;
    }

    ecs_script_var_t *value = ecs_vec_append_t(&v->base.script->allocator,
        &vars->defaults, ecs_script_var_t);
    value->value.ptr = flecs_calloc_w_dbg_info(
        &v->base.script->allocator, ti->size, ti->name);
    value->value.type = type;
    value->type_info = ti;
    value->owned = false; /* Not stack storage, freed with the template */
    ecs_ptr_copy_w_type_info(
        v->world, ti, value->value.ptr, var->value.ptr);

    if (mut && !vars->type) {
        vars->type = ecs_entity(v->world, {
            .name = "mut", .parent = template->props.type
        });
    }

    ecs_entity_t mbr = ecs_entity(v->world, {
        .name = node->name,
        .parent = vars->type
    });

    ecs_set(v->world, mbr, EcsMember, { .type = var->value.type });

    ecs_script_template_member_t *member = ecs_vec_append_t(
        &v->base.script->allocator, &template->members,
        ecs_script_template_member_t);
    member->index = ecs_vec_count(&vars->defaults) - 1;
    member->input = 0;
    member->is_mut = mut;

    return 0;
}

static int flecs_script_visit_type_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template)
{
    ecs_script_type_visitor_t *t = v->type_visitor;
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_template_node_t *tnode = template->node;
    if (tnode->symbol_offset == -1) {
        tnode->symbol_offset = ecs_vec_count(&v->base.script->symbol_slots);
    }
    template->symbol_offset = tnode->symbol_offset;
    if (tnode->root_symbol == -1) {
        tnode->root_symbol = flecs_script_type_slot_new(t, -1);
    }
    template->root_symbol = tnode->root_symbol;

    int32_t old_table = t->table;
    bool old_template_scope = t->template_scope;
    flecs_script_entity_state_t *old_entity = v->entity;
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = template->node->node.pos
        },
        .symbol = -1,
        .kind_symbol = -1,
        .kind_sp = -1
    };
    flecs_script_entity_state_t instance_state = {
        .node = &instance_node
    };
    v->entity = &instance_state;
    t->template_scope = true;

    ecs_script_vars_t *outer_vars = v->vars;
    ecs_script_vars_t *type_vars = flecs_script_vars_push(
        NULL, &v->r->stack, &v->r->allocator);
    type_vars->world = v->world;

    int32_t i, var_count = ecs_vec_count(&template->vars->vars);
    ecs_script_var_t *src_vars = ecs_vec_first(&template->vars->vars);
    for (i = 0; i < var_count; i ++) {
        ecs_script_var_t *src = &src_vars[i];
        ecs_script_var_t *dst = ecs_script_vars_define_id(
            type_vars, src->name, src->value.type);
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_ptr_copy(v->world,
            src->value.type, dst->value.ptr, src->value.ptr);
        dst->is_const = false;
    }

    v->vars = ecs_script_vars_push(type_vars);
    ecs_script_var_t *this_var = ecs_script_vars_declare(v->vars, "this");
    this_var->value.type = ecs_id(ecs_entity_t);

    int32_t table = flecs_script_type_table_new(
        t, old_table, NULL);
    int result = flecs_script_type_scope(
        t, template->node->scope, table, true, false);

    v->vars = ecs_script_vars_pop(v->vars);
    ecs_script_vars_pop(v->vars);
    v->vars = outer_vars;
    t->template_scope = old_template_scope;
    t->table = old_table;
    v->entity = old_entity;
    if (tnode->symbol_count == -1) {
        tnode->symbol_count = ecs_vec_count(
            &v->base.script->symbol_slots) - template->symbol_offset;
    }
    template->symbol_count = tnode->symbol_count;
    return result;
}


static int flecs_script_template_preprocess(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template)
{
    ecs_script_template_t *old_template = v->template;
    ecs_script_template_t *old_instance_template = v->instance_template;
    ecs_entity_t old_script_entity = v->script_entity;
    v->template = template;
    v->instance_template = template;
    v->script_entity = 0;
    int result = flecs_script_visit_type_template(v, template);
    v->script_entity = old_script_entity;
    v->instance_template = old_instance_template;
    v->template = old_template;
    return result;
}

static int flecs_script_template_hoist_using(
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

static int flecs_script_template_hoist_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template,
    ecs_script_vars_t *vars)
{
    int32_t i, count = ecs_vec_count(&vars->vars);
    ecs_script_var_t *src_vars = ecs_vec_first(&vars->vars);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *src = &src_vars[i];
        if (!src->is_const) {
            continue;
        }
        if (ecs_script_vars_lookup(template->vars, src->name)) {
            /* If variable is masked, don't declare it twice */
            continue;
        }
        ecs_script_var_t *dst = ecs_script_vars_define_id(
            template->vars, src->name, src->value.type);
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        if (src->value.ptr) {
            ecs_ptr_copy(v->world,
                src->value.type, dst->value.ptr, src->value.ptr);
        }
        dst->is_const = true;
        ecs_vec_append_t(NULL, &template->capture_sp, int32_t)[0] = src->sp;
    }

    if (vars->parent) {
        flecs_script_template_hoist_vars(v, template, vars->parent);
    }

    return 0;
}

static ecs_script_template_t* flecs_script_template_init(
    ecs_script_impl_t *script)
{
    ecs_allocator_t *a = &script->allocator;
    ecs_script_template_t *result = flecs_alloc_t(a, ecs_script_template_t);
    result->props.type = 0;
    ecs_vec_init_t(NULL, &result->props.defaults, ecs_script_var_t, 0);
    result->muts.type = 0;
    ecs_vec_init_t(NULL, &result->muts.defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->members, ecs_script_template_member_t, 0);
    result->type_info = NULL;
    ecs_vec_init_t(NULL, &result->using_, ecs_entity_t, 0);
    ecs_vec_init_t(NULL, &result->annot, ecs_script_annot_t*, 0);
    ecs_vec_init_t(NULL, &result->refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->observers, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->dynamic_refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->capture_sp, int32_t, 0);
    ecs_vec_init_t(NULL, &result->capture_input,
        ecs_script_template_capture_t, 0);
    result->symbol_offset = 0;
    result->symbol_count = 0;
    result->root_symbol = -1;
    result->input_count = 0;
    result->scope_count = 0;
    result->component_count = 0;
    result->for_count = 0;
    result->refcount = 0;

    result->vars = ecs_script_vars_init(script->pub.world);
    return result;
}

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &script->allocator;

    ecs_vec_t *defaults[2] = {
        &template->props.defaults, &template->muts.defaults
    };

    int32_t d, i;
    for (d = 0; d < 2; d ++) {
        int32_t count = ecs_vec_count(defaults[d]);
        ecs_script_var_t *values = ecs_vec_first(defaults[d]);
        for (i = 0; i < count; i ++) {
            ecs_script_var_t *value = &values[i];
            const ecs_type_info_t *ti = value->type_info;
            if (ti->hooks.dtor) {
                flecs_type_info_dtor(value->value.ptr, 1, ti);
            }
            flecs_free(a, ti->size, value->value.ptr);
        }
        ecs_vec_fini_t(a, defaults[d], ecs_script_var_t);
    }

    ecs_vec_fini_t(a, &template->members, ecs_script_template_member_t);

    ecs_vec_fini_t(a, &template->using_, ecs_entity_t);
    ecs_vec_fini_t(a, &template->annot, ecs_script_annot_t*);
    ecs_vec_fini_t(NULL, &template->refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &template->observers, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &template->dynamic_refs, ecs_script_ref_t);
    ecs_vec_fini_t(NULL, &template->capture_sp, int32_t);
    ecs_vec_fini_t(NULL, &template->capture_input,
        ecs_script_template_capture_t);
    ecs_script_vars_fini(template->vars);
    flecs_free_t(a, ecs_script_template_t, template);
}

/* Create new template */
int flecs_script_eval_template(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *node)
{
    ecs_entity_t template_entity = node->symbol != -1
        ? flecs_script_symbol_entity(v, node->symbol) : 0;
    if (!template_entity) {
        template_entity = flecs_script_create_entity(v, node->name);
        if (node->symbol != -1 && template_entity) {
            flecs_script_symbol_set(v, node->symbol, template_entity);
        }
    }
    if (!template_entity) {
        return -1;
    }

    ecs_script_template_t *template = flecs_script_template_init(v->base.script);
    template->props.type = template_entity;
    template->node = node;

    if (flecs_script_template_hoist_using(v, template)) {
        goto error;
    }

    if (flecs_script_template_hoist_vars(v, template, v->vars)) {
        goto error;
    }

    ecs_vec_t outer_annot = v->r->annot;
    ecs_vec_init_t(&v->r->allocator,
        &v->r->annot, ecs_script_annot_t*, 0);
    int preprocess_result = flecs_script_template_preprocess(v, template);
    ecs_vec_fini_t(&v->r->allocator,
        &v->r->annot, ecs_script_annot_t*);
    v->r->annot = outer_annot;
    if (preprocess_result) {
        goto error;
    }

    /* If template has no props, give template dummy size so we can register
     * hooks for it. */
    const EcsComponent *template_component = ecs_get(
        v->world, template_entity, EcsComponent);
    if (!template_component || !template_component->size) {
        ecs_set(v->world, template_entity, EcsComponent, {1, 1});
    }

    /* If template has mut properties, add those when the template is added */
    if (template->muts.type) {
        ecs_add_pair(v->world, template_entity, EcsWith, template->muts.type);
    }

    /* Consume annotations, if any */
    int32_t i, count = ecs_vec_count(&v->r->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = 0; i < count ; i ++) {
            ecs_vec_append_t(&v->base.script->allocator, 
                &template->annot, ecs_script_annot_t*)[0] = annots[i];
        }
        ecs_vec_clear(&v->r->annot);
    }

    template->type_info = ecs_get_type_info(v->world, template_entity);

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
        .ctor = flecs_script_template_props_ctor,
        .on_add = flecs_script_template_on_add,
        .on_replace = flecs_script_template_on_props_replace,
        .on_set = flecs_script_template_on_props_set,
        .on_remove = flecs_script_template_on_remove,
        .ctx = v->world
    });

    if (template->muts.type) {
        ecs_set_hooks_id(v->world, template->muts.type, &(ecs_type_hooks_t) {
            .ctor = flecs_script_template_muts_ctor,
            .on_replace = flecs_script_template_on_muts_replace,
            .on_set = flecs_script_template_on_muts_set,
            .ctx = v->world
        });
    }

    /* Keep script alive for as long as template is alive */
    v->base.script->refcount ++;

    return 0;
error:
    flecs_script_template_fini(v->base.script, template);
    return -1;
}

int flecs_script_template_update_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_node_t *node)
{
    ecs_entity_t entity = flecs_script_symbol_entity(v, node->symbol);
    const EcsScript *script = ecs_get(v->world, entity, EcsScript);
    ecs_assert(script != NULL && script->template_ != NULL,
        ECS_INTERNAL_ERROR, NULL);

    ecs_script_template_t *template = script->template_;
    int32_t count = ecs_vec_count(&template->capture_sp);
    ecs_assert(count == ecs_vec_count(&template->vars->vars),
        ECS_INTERNAL_ERROR, NULL);
    int32_t *capture_sp = ecs_vec_first(&template->capture_sp);
    ecs_script_var_t *dst = ecs_vec_first(&template->vars->vars);
    for (int32_t i = 0; i < count; i ++) {
        ecs_script_var_t *src = ecs_script_vars_from_sp(
            v->vars, capture_sp[i]);
        if (!src || !src->value.ptr) {
            flecs_script_eval_error(v, node,
                "variable '%s' captured by template has no value",
                dst[i].name);
            return -1;
        }
        if (src->value.type != dst[i].value.type) {
            flecs_script_eval_error(v, node,
                "variable '%s' captured by template has type %s, expected %s",
                dst[i].name,
                flecs_errstr(ecs_get_path(v->world, src->value.type)),
                flecs_errstr_1(ecs_get_path(v->world, dst[i].value.type)));
            return -1;
        }
        ecs_ptr_copy(v->world, src->value.type,
            dst[i].value.ptr, src->value.ptr);
    }

    /* The inputs of a template are numbered independently from the inputs of
     * the script that declares it. Translate the inputs of the enclosing scope
     * to the inputs the template body uses for its captured variables. */
    uint64_t input = UINT64_MAX;
    if (!v->force) {
        input = 0;
        ecs_script_template_capture_t *captures = ecs_vec_first(
            &template->capture_input);
        int32_t capture_count = ecs_vec_count(&template->capture_input);
        for (int32_t i = 0; i < capture_count; i ++) {
            if (captures[i].outer_input & v->input) {
                input |= captures[i].input;
            }
        }
    }

    ecs_vec_t instances;
    ecs_vec_init_t(NULL, &instances, ecs_entity_t, 0);
    ecs_iter_t it = ecs_each_id(v->world, entity);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_vec_append_t(NULL, &instances, ecs_entity_t)[0] =
                it.entities[i];
        }
    }

    ecs_entity_t *array = ecs_vec_first(&instances);
    count = ecs_vec_count(&instances);
    for (int32_t i = 0; i < count; i ++) {
        void *data = ECS_CONST_CAST(void*,
            ecs_get_id(v->world, array[i], entity));
        if (data && flecs_script_template_instantiate(
            v->world, entity, entity, &array[i], data, 1,
            input, false))
        {
            ecs_vec_fini_t(NULL, &instances, ecs_entity_t);
            return -1;
        }
    }
    ecs_vec_fini_t(NULL, &instances, ecs_entity_t);
    return 0;
}

void flecs_script_template_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateSetEvent);
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateInstanceUpdateEvent);
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateRoot);
    ECS_TAG_DEFINE(world, EcsScriptTemplate);

    ecs_add_id(world, EcsScriptTemplate, EcsPairIsTag);
#ifdef FLECS_CONSTRAINT_TRAITS
    ecs_add_id(world, EcsScriptTemplate, EcsRelationship);
#endif

    ecs_set_hooks(world, EcsScriptTemplateSetEvent, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScriptTemplateSetEvent),
        .dtor = ecs_dtor(EcsScriptTemplateSetEvent),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_set_hooks(world, EcsScriptTemplateRoot, {
        .ctor = ecs_ctor(EcsScriptTemplateRoot),
        .move = ecs_move(EcsScriptTemplateRoot),
        .dtor = ecs_dtor(EcsScriptTemplateRoot),
        .on_remove = flecs_script_template_root_on_remove,
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ecs_add_pair(world, ecs_id(EcsScriptTemplateRoot),
        EcsOnInstantiate, EcsDontInherit);

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "TemplateSetObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { ecs_id(EcsScriptTemplateSetEvent) },
        .callback = flecs_on_template_set_event
    });

    ecs_observer(world, {
        .entity = ecs_entity(world, { .name = "TemplateInstanceUpdateObserver" }),
        .query.terms = {{ .id = EcsAny }},
        .events = { ecs_id(EcsScriptTemplateInstanceUpdateEvent) },
        .callback = flecs_on_template_instance_update_event
    });
}

#endif
