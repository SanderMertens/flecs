/**
 * @file addons/script/reactivity/template.c
 * @brief Script template implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"

ECS_COMPONENT_DECLARE(EcsScriptTemplateRoot);
ECS_DECLARE(EcsScriptTemplate);
ECS_DECLARE(EcsScriptTemplateManual);
ECS_DECLARE(EcsScriptTemplatePending);
ECS_COMPONENT_DECLARE(ecs_script_template_ref_t);

static void flecs_script_template_ref_clear(
    ecs_world_t *world,
    ecs_script_template_ref_t *ref)
{
    if (ref->value) {
        ecs_ptr_free(world, ref->type, ref->value);
        ref->value = NULL;
    }
}

void flecs_script_template_ref_set(
    ecs_world_t *world,
    ecs_script_template_ref_t *ref,
    ecs_entity_t type,
    void *value)
{
    flecs_script_template_ref_clear(world, ref);
    ref->type = type;
    ref->value = value;
}

int flecs_script_template_ref_apply(
    ecs_world_t *world,
    const ecs_script_template_ref_t *ref,
    const ecs_type_info_t *ti,
    void *dst)
{
    if (!ref || !ref->value) {
        return 0;
    }

    if (ref->type != ti->component) {
        return -1;
    }

    ecs_ptr_copy_w_type_info(world, ti, dst, ref->value);
    return 0;
}

static void flecs_script_template_ref_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_script_template_ref_t *dst = dst_ptr;
    const ecs_script_template_ref_t *src = src_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        void *value = NULL;
        if (src[i].value) {
            value = ecs_ptr_new(world, src[i].type);
            ecs_ptr_copy(world, src[i].type, value, src[i].value);
        }

        flecs_script_template_ref_set(world, &dst[i], src[i].type, value);
    }
}

static void flecs_script_template_ref_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_script_template_ref_t *dst = dst_ptr;
    ecs_script_template_ref_t *src = src_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_template_ref_set(
            world, &dst[i], src[i].type, src[i].value);
        src[i].value = NULL;
    }
}

static void flecs_script_template_ref_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_script_template_ref_t *refs = ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_template_ref_clear(world, &refs[i]);
    }
}

static int flecs_script_template_ref_value_cmp(
    const ecs_world_t *world,
    ecs_entity_t type,
    const void *a,
    const void *b)
{
    if (a == b) {
        return 0;
    }

    if (!a) {
        return -1;
    }

    if (!b) {
        return 1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    if (!ti) {
        return 0;
    }

    if (ti->hooks.cmp && !(ti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL)) {
        return flecs_type_info_cmp(a, b, ti);
    }

    if (ti->hooks.equals && !(ti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL)) {
        return flecs_type_info_equals(a, b, ti) ? 0 : 1;
    }

    return ecs_os_memcmp(a, b, ti->size);
}

static int flecs_script_template_ref_cmp(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti)
{
    const ecs_script_template_ref_t *a = a_ptr;
    const ecs_script_template_ref_t *b = b_ptr;
    if (a->type != b->type) {
        return a->type < b->type ? -1 : 1;
    }

    return flecs_script_template_ref_value_cmp(
        ti->hooks.ctx, a->type, a->value, b->value);
}

static bool flecs_script_template_ref_equals(
    const void *a_ptr,
    const void *b_ptr,
    const ecs_type_info_t *ti)
{
    return flecs_script_template_ref_cmp(a_ptr, b_ptr, ti) == 0;
}

static int flecs_script_template_ref_serialize(
    const ecs_serializer_t *ser,
    const void *ptr)
{
    const ecs_script_template_ref_t *ref = ptr;
    return ser->value(ser, ecs_id(ecs_entity_t), &ref->type);
}

static void flecs_script_template_ref_assign_entity(
    void *ptr,
    ecs_world_t *world,
    ecs_entity_t entity)
{
    flecs_script_template_ref_set(world, ptr, entity, NULL);
}

static ecs_script_template_t* flecs_script_template_get(
    const ecs_world_t *world,
    ecs_entity_t template_entity)
{
    if (!template_entity || !ecs_is_alive(world, template_entity)) {
        return NULL;
    }

    const EcsScript *script = ecs_get(world, template_entity, EcsScript);
    return script ? script->template_ : NULL;
}

static const ecs_script_template_member_t* flecs_script_template_member(
    const ecs_script_template_t *template,
    int32_t index,
    bool mut)
{
    if (!template) {
        return NULL;
    }

    const ecs_script_template_member_t *members = ecs_vec_first(
        &template->members);
    int32_t i, count = ecs_vec_count(&template->members);
    for (i = 0; i < count; i ++) {
        if (members[i].is_mut == mut && members[i].index == index) {
            return &members[i];
        }
    }

    return NULL;
}

static const ecs_script_template_member_t* flecs_script_template_member_by_sp(
    const ecs_script_template_t *template,
    int32_t sp)
{
    if (!template) {
        return NULL;
    }

    const ecs_script_template_member_t *members = ecs_vec_first(
        &template->members);
    int32_t i, count = ecs_vec_count(&template->members);
    for (i = 0; i < count; i ++) {
        if (members[i].sp == sp) {
            return &members[i];
        }
    }

    return NULL;
}

static void flecs_script_template_root_tasks_free(
    EcsScriptTemplateRoot *root)
{
    (void)root;
#ifdef FLECS_SCRIPT_ASYNC
    flecs_script_async_tasks_free(&root->tasks);
    ecs_vec_clear(&root->async_blocks);
#endif
}

static void flecs_script_template_root_fini(
    EcsScriptTemplateRoot *root)
{
    flecs_script_template_root_tasks_free(root);
    ecs_vec_fini_t(NULL, &root->tasks, void*);
    ecs_vec_fini_t(NULL, &root->async_blocks, ecs_script_async_block_t);
    flecs_script_state_fini(&root->state);
    ecs_vec_fini_t(NULL, &root->observers, ecs_script_ref_t);
}

static void flecs_script_template_root_reset(
    EcsScriptTemplateRoot *root)
{
    ecs_os_zeromem(root);
    ecs_vec_init_t(NULL, &root->observers, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &root->tasks, void*, 0);
    ecs_vec_init_t(NULL, &root->async_blocks, ecs_script_async_block_t, 0);
    flecs_script_state_init(&root->state);
}

static ECS_CTOR(EcsScriptTemplateRoot, ptr, {
    flecs_script_template_root_reset(ptr);
})

static ECS_MOVE(EcsScriptTemplateRoot, dst, src, {
    flecs_script_template_root_fini(dst);
    *dst = *src;
    flecs_script_template_root_reset(src);
})

static ECS_DTOR(EcsScriptTemplateRoot, ptr, {
    flecs_script_template_root_fini(ptr);
})

static void flecs_script_template_root_init(
    EcsScriptTemplateRoot *root,
    ecs_script_template_t *template)
{
    if (root->generation == template->generation) {
        return;
    }

    root->generation = template->generation;
    flecs_script_slots_reset_t(&root->state.symbol_slots,
        ecs_script_symbol_slot_t, template->symbol_count);

    ecs_script_symbol_slot_t *dst = ecs_vec_first(&root->state.symbol_slots);
    ecs_script_symbol_slot_t *src = ecs_vec_first(
        &template->script->state.symbol_slots);
    int32_t i;
    for (i = 0; i < template->symbol_count; i ++) {
        dst[i].scope_slot = src[template->symbol_offset + i].scope_slot;
    }

    flecs_script_state_resize(&root->state, template->scope_count,
        template->component_count, template->for_count,
        template->computed_count);
}

static bool flecs_script_template_chain_has(
    const ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t needle)
{
    while (template_entity) {
        if (template_entity == needle) {
            return true;
        }

        ecs_script_template_t *t = flecs_script_template_get(
            world, template_entity);
        if (!t) {
            break;
        }

        template_entity = t->base;
    }

    return false;
}

static bool flecs_script_template_root_next(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t *col,
    ecs_entity_t *template_entity)
{
    ecs_id_t found = 0;
    if (!table) {
        return false;
    }

    *col = ecs_search_offset(world, table, *col + 1,
        ecs_pair(ecs_id(EcsScriptTemplateRoot), EcsWildcard), &found);
    if (*col == -1) {
        return false;
    }

    *template_entity = ecs_pair_second(world, found);
    return true;
}

static bool flecs_script_template_base_in_use(
    const ecs_world_t *world,
    ecs_entity_t instance,
    ecs_entity_t removed,
    ecs_entity_t base)
{
    ecs_table_t *table = ecs_get_table(world, instance);
    ecs_entity_t other;
    int32_t col = -1;
    while (flecs_script_template_root_next(world, table, &col, &other)) {
        if (!flecs_script_template_chain_has(world, removed, other) &&
            flecs_script_template_chain_has(world, other, base))
        {
            return true;
        }
    }

    return false;
}

typedef struct ecs_script_template_claims_t {
    ecs_map_t entities;
    ecs_map_t components;
    ecs_vec_t states;
} ecs_script_template_claims_t;

static void flecs_script_template_claims_add(
    const ecs_world_t *world,
    ecs_script_template_claims_t *claims,
    const ecs_script_state_t *state)
{
    const ecs_script_symbol_slot_t *syms = ecs_vec_first(&state->symbol_slots);
    int32_t i, count = ecs_vec_count(&state->symbol_slots);
    for (i = 0; i < count; i ++) {
        if (syms[i].entity) {
            ecs_map_ensure(&claims->entities, syms[i].entity);
        }
    }

    const ecs_script_component_slot_t *comps = ecs_vec_first(
        &state->component_slots);
    count = ecs_vec_count(&state->component_slots);
    for (i = 0; i < count; i ++) {
        if (!comps[i].component) {
            continue;
        }

        ecs_entity_t src = flecs_script_state_component_src(
            world, state, &comps[i]);
        if (!src) {
            continue;
        }

        ecs_vec_t *ids = ecs_map_ensure_alloc_t(
            &claims->components, ecs_vec_t, src);
        ecs_vec_init_if_t(ids, ecs_id_t);
        ecs_vec_append_t(NULL, ids, ecs_id_t)[0] = comps[i].component;
    }

    ecs_vec_append_t(NULL, &claims->states,
        const ecs_script_state_t*)[0] = state;
}

static void flecs_script_template_claims_fini(
    ecs_script_template_claims_t *claims)
{
    ecs_map_iter_t it = ecs_map_iter(&claims->components);
    while (ecs_map_next(&it)) {
        ecs_vec_t *ids = ecs_map_ptr(&it);
        ecs_vec_fini_t(NULL, ids, ecs_id_t);
        ecs_os_free(ids);
    }

    ecs_map_fini(&claims->components);
    ecs_map_fini(&claims->entities);
    ecs_vec_fini_t(NULL, &claims->states, const ecs_script_state_t*);
}

static bool flecs_script_template_claims_for(
    const ecs_script_template_claims_t *claims,
    ecs_entity_t entity,
    ecs_id_t component)
{
    const ecs_script_state_t **states = ecs_vec_first(&claims->states);
    int32_t s, state_count = ecs_vec_count(&claims->states);
    for (s = 0; s < state_count; s ++) {
        const ecs_script_for_slot_t *slots = ecs_vec_first(
            &states[s]->for_slots);
        int32_t f, for_count = ecs_vec_count(&states[s]->for_slots);
        for (f = 0; f < for_count; f ++) {
            ecs_script_for_entry_t *entry = ecs_map_get_deref(
                &slots[f].named, ecs_script_for_entry_t, entity);
            if (!entry) {
                continue;
            }

            if (!component) {
                return true;
            }

            const ecs_script_for_component_t *comps = ecs_vec_first(
                &entry->components);
            int32_t c, count = ecs_vec_count(&entry->components);
            for (c = 0; c < count; c ++) {
                if (comps[c].component == component) {
                    return true;
                }
            }
        }
    }

    return false;
}

static bool flecs_script_template_claims_entity(
    const ecs_script_template_claims_t *claims,
    ecs_entity_t entity)
{
    if (ecs_map_get(&claims->entities, entity)) {
        return true;
    }

    return flecs_script_template_claims_for(claims, entity, 0);
}

static bool flecs_script_template_claims_component(
    const ecs_script_template_claims_t *claims,
    ecs_entity_t src,
    ecs_id_t component)
{
    ecs_vec_t *ids = ecs_map_get_deref(
        &claims->components, ecs_vec_t, src);
    if (ids) {
        const ecs_id_t *array = ecs_vec_first(ids);
        int32_t i, count = ecs_vec_count(ids);
        for (i = 0; i < count; i ++) {
            if (array[i] == component) {
                return true;
            }
        }
    }

    return flecs_script_template_claims_for(claims, src, component);
}

static void flecs_script_template_for_slot_release(
    ecs_world_t *stage,
    ecs_script_for_slot_t *slot,
    const ecs_script_template_claims_t *claims)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->named);
    while (ecs_map_next(&it)) {
        ecs_entity_t entity = ecs_map_key(&it);
        if (!flecs_script_template_claims_entity(claims, entity)) {
            continue;
        }

        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        const ecs_script_for_component_t *comps = ecs_vec_first(
            &entry->components);
        int32_t c, count = ecs_vec_count(&entry->components);
        for (c = 0; c < count; c ++) {
            if (!flecs_script_template_claims_component(
                claims, entity, comps[c].component))
            {
                ecs_remove_id(stage, entity, comps[c].component);
            }
        }

        ecs_vec_fini_t(NULL, &entry->components, ecs_script_for_component_t);
        ecs_map_remove_free(&slot->named, entity);
        slot->cache_entity = 0;
        slot->cache_entry = NULL;
    }
}

static void flecs_script_template_root_release_shared(
    ecs_world_t *stage,
    ecs_entity_t instance,
    ecs_entity_t template_entity,
    EcsScriptTemplateRoot *root)
{
    const ecs_world_t *world = ecs_get_world(stage);
    if (!instance || !ecs_is_alive(world, instance)) {
        return;
    }

    ecs_table_t *table = ecs_get_table(world, instance);
    ecs_entity_t other;
    int32_t col = -1;
    bool has_claims = false;
    ecs_script_template_claims_t claims;

    while (flecs_script_template_root_next(world, table, &col, &other)) {
        if (!other) {
            continue;
        }

        if (flecs_script_template_chain_has(world, other, template_entity) ||
            flecs_script_template_chain_has(world, template_entity, other))
        {
            continue;
        }

        const EcsScriptTemplateRoot *sib = ecs_get_pair(world, instance,
            EcsScriptTemplateRoot, other);
        if (!sib) {
            continue;
        }

        if (!has_claims) {
            ecs_map_init(&claims.entities, NULL);
            ecs_map_init(&claims.components, NULL);
            ecs_vec_init_t(NULL, &claims.states, const ecs_script_state_t*, 0);
            has_claims = true;
        }

        flecs_script_template_claims_add(world, &claims, &sib->state);
    }

    if (!has_claims) {
        return;
    }

    ecs_script_symbol_slot_t *syms = ecs_vec_first(&root->state.symbol_slots);
    int32_t i, count = ecs_vec_count(&root->state.symbol_slots);
    for (i = 0; i < count; i ++) {
        if (syms[i].entity &&
            flecs_script_template_claims_entity(&claims, syms[i].entity))
        {
            syms[i].scope_slot = -1;
        }
    }

    ecs_script_component_slot_t *comps = ecs_vec_first(
        &root->state.component_slots);
    count = ecs_vec_count(&root->state.component_slots);
    for (i = 0; i < count; i ++) {
        if (!comps[i].component) {
            continue;
        }

        ecs_entity_t src = flecs_script_state_component_src(
            world, &root->state, &comps[i]);
        if (src && flecs_script_template_claims_component(
            &claims, src, comps[i].component))
        {
            comps[i].scope_slot = -1;
        }
    }

    ecs_script_for_slot_t *for_slots = ecs_vec_first(&root->state.for_slots);
    count = ecs_vec_count(&root->state.for_slots);
    for (i = 0; i < count; i ++) {
        flecs_script_template_for_slot_release(stage, &for_slots[i], &claims);
    }

    flecs_script_template_claims_fini(&claims);
}

static void flecs_script_template_root_clear(
    ecs_world_t *world,
    ecs_script_template_t *template,
    EcsScriptTemplateRoot *root,
    ecs_entity_t instance,
    ecs_entity_t template_entity)
{
    flecs_script_template_root_release_shared(
        world, instance, template_entity, root);
    ecs_script_state_t state = root->state;

    flecs_script_template_root_tasks_free(root);
    flecs_script_state_clear_computed(&root->state);

    ecs_script_for_slot_t *for_slot_array = ecs_vec_first(&state.for_slots);
    int32_t i, count = ecs_vec_count(&state.for_slots);
    for (i = 0; i < count; i ++) {
        flecs_script_for_slot_clear(world, &for_slot_array[i], true);
    }

    int32_t root_symbol = template->root_symbol - template->symbol_offset;
    if (root_symbol >= 0 && root_symbol < ecs_vec_count(&state.symbol_slots)) {
        ecs_vec_get_t(&state.symbol_slots,
            ecs_script_symbol_slot_t, root_symbol)->scope_slot = -1;
    }

    flecs_script_slots_reset_t(&state.scope_slots, int32_t,
        ecs_vec_count(&state.scope_slots));

    ecs_script_eval_visitor_t v = {
        .base.script = template->script,
        .world = world,
        .state = &state,
        .instance_template = template,
        .visit = 1
    };

    flecs_script_eval_cleanup(&v, true);
}

static void flecs_script_template_root_on_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->stage;
    EcsScriptTemplateRoot *roots = ecs_field_w_size(
        it, sizeof(EcsScriptTemplateRoot), 0);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        flecs_script_ref_observers_clear(world, &roots[i].observers);
    }
}

static void flecs_script_template_root_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->stage;
    ecs_entity_t template_entity = ecs_pair_second(
        it->world, ecs_field_id(it, 0));
    ecs_script_template_t *template = flecs_script_template_get(
        world, template_entity);
    if (!template) {
        return;
    }

    EcsScriptTemplateRoot *roots = ecs_field_w_size(
        it, sizeof(EcsScriptTemplateRoot), 0);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        flecs_script_template_root_clear(world, template, &roots[i],
            it->entities[i], template_entity);
    }
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

    ecs_script_template_t *template = flecs_script_template_get(
        world, template_entity);
    if (!template) {
        ecs_err("template component '%s' has no template, cannot construct",
            ti->name);
        return;
    }

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

static ecs_entity_t flecs_script_template_member_type(
    ecs_world_t *world,
    const ecs_member_t *member)
{
    if (member->count >= 1) {
        return flecs_script_array_type(world, member->type, member->count);
    }

    return member->type;
}

const ecs_member_t* flecs_script_template_parent_member(
    ecs_world_t *world,
    const ecs_script_template_t *template,
    const char *name,
    ecs_entity_t *component)
{
    const ecs_script_template_t *parent = template
        ? flecs_script_template_get(world, template->parent_type)
        : NULL;
    if (!parent) {
        return NULL;
    }

    ecs_entity_t types[2] = {parent->props.type, parent->muts.type};
    const char *dot = strchr(name, '.');
    ecs_size_t len = dot ? flecs_ito(ecs_size_t, dot - name) : ecs_os_strlen(name);

    for (int32_t i = 0; i < 2; i ++) {
        const EcsStruct *st = types[i] ? ecs_get(world, types[i], EcsStruct) : NULL;
        if (!st) {
            continue;
        }

        const ecs_member_t *members = ecs_vec_first(&st->members);
        for (int32_t m = 0; m < ecs_vec_count(&st->members); m ++) {
            if (ecs_os_strlen(members[m].name) == len &&
                !ecs_os_strncmp(members[m].name, name, len))
            {
                *component = types[i];
                return &members[m];
            }
        }
    }

    return NULL;
}

static void flecs_script_template_instantiate_vars(
    ecs_world_t *world,
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
        ecs_script_var_t *var = ecs_script_vars_declare(vars, NULL);
        ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
        var->value.type = flecs_script_template_member_type(world, member);
        var->value.ptr = ECS_OFFSET(data, member->offset);
        var->owned = false;
    }
}

static int flecs_script_template_init_muts(
    ecs_script_eval_visitor_t *v,
    const ecs_script_template_t *template)
{
    ecs_script_scope_t *scope = template->node->scope;
    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = ecs_vec_get_t(
            &scope->stmts, ecs_script_node_t*, i)[0];
        if (stmt->kind != EcsAstMut || stmt->skip) {
            continue;
        }

        ecs_script_var_node_t *node = (ecs_script_var_node_t*)stmt;
        ecs_script_var_t *var = ecs_script_vars_from_sp(v->vars, node->sp);
        ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
        if (flecs_script_eval_expr(v, &node->expr, &var->value)) {
            return -1;
        }
    }

    return 0;
}

static int flecs_script_template_validate_interfaces(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    const ecs_script_template_t *template,
    const EcsStruct *props_st,
    const void *props_data,
    const EcsStruct *muts_st,
    const void *muts_data)
{
    const ecs_script_template_member_t *members = ecs_vec_first(
        &template->members);
    int32_t m, count = ecs_vec_count(&template->members);
    for (m = 0; m < count; m ++) {
        const ecs_script_template_member_t *tm = &members[m];
        if (!tm->interface) {
            continue;
        }

        const EcsStruct *st = tm->is_mut ? muts_st : props_st;
        const void *data = tm->is_mut ? muts_data : props_data;
        if (!st || !data) {
            continue;
        }

        const ecs_member_t *member = ecs_vec_get_t(
            &st->members, ecs_member_t, tm->index);
        const void *ptr = ECS_OFFSET(data, member->offset);
        const ecs_script_template_ref_t *elems = ptr;
        int32_t e, elem_count = 1;
        if (tm->is_vector) {
            elems = ecs_vec_first(ptr);
            elem_count = ecs_vec_count(ptr);
        }

        for (e = 0; e < elem_count; e ++) {
            ecs_entity_t value = elems[e].type;
            const char *expected = flecs_script_template_interface_error(
                world, value, tm->interface);
            if (!expected) {
                continue;
            }

            const char *name = ecs_get_name(world, template_entity);
            if (!tm->is_vector && (!value || !ecs_is_alive(world, value))) {
                ecs_err("missing value for prop '%s' of template '%s': "
                    "expected %s", member->name, name, expected);
            } else {
                const char *value_str = value
                    ? flecs_errstr(ecs_get_path(world, value)) : "0";
                if (tm->is_vector) {
                    ecs_err("invalid value '%s' at index %d of prop '%s' of "
                        "template '%s': expected %s",
                        value_str, e, member->name, name, expected);
                } else {
                    ecs_err("invalid value '%s' for prop '%s' of template "
                        "'%s': expected %s",
                        value_str, member->name, name, expected);
                }
            }

            return -1;
        }
    }

    return 0;
}

static int flecs_script_template_run(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_script_template_t *template,
    ecs_entity_t instance,
    ecs_entity_t parent,
    EcsScriptTemplateRoot *root,
    uint64_t input)
{
    ecs_script_runtime_t *pool = flecs_script_runtime_get(world);
    ecs_script_eval_desc_t desc = {
        .runtime = flecs_script_runtime_acquire_call(pool)
    };

    ecs_script_runner_t runner;
    ecs_script_eval_visitor_t *v = &runner.v;
    flecs_script_eval_visit_init(template->script, v, &desc);
    flecs_script_runner_init(&runner, v);

    ecs_vec_t prev_using = v->r->using;
    v->r->using = template->using_;

    ecs_vec_t run_refs;
    ecs_vec_init_t(NULL, &run_refs, ecs_script_ref_t, 0);

    ecs_script_scope_t *scope = template->node->scope;
    int32_t root_symbol = template->root_symbol - template->symbol_offset;
    ecs_script_entity_t instance_node = {
        .node = {
            .kind = EcsAstEntity,
            .pos = template->node->node.pos
        },
        .scope = scope,
        .symbol = template->root_symbol,
        .non_fragmenting_parent = template->non_fragmenting_parent
    };

    flecs_script_entity_state_t instance_state = {
        .node = &instance_node,
        .eval = instance,
        .symbol = root_symbol
    };

    int32_t visit = flecs_script_state_next(&root->state);
    ecs_script_state_t state = root->state;
    ecs_script_symbol_slot_t *root_slot = ecs_vec_get_t(
        &state.symbol_slots, ecs_script_symbol_slot_t, root_symbol);
    root_slot->entity = instance;
    root_slot->scope_slot = scope->scope_slot;

    v->template_entity = template_entity;
    v->body_template = template_entity;
    v->template_instance = instance;
    v->instance_template = template;
    v->symbol_offset = template->symbol_offset;
    v->entity = &instance_state;
    v->parent = instance;
    v->state = &state;
    v->run_refs = &run_refs;
    flecs_script_eval_begin(v, input, visit);

    int result = -1;
    for (int32_t a = 0; a < ecs_vec_count(&template->annot); a ++) {
        ecs_script_annot_t *annot = ecs_vec_get_t(
            &template->annot, ecs_script_annot_t*, a)[0];
        if (flecs_script_apply_annot(
            v, &instance_node, instance, annot))
        {
            goto done;
        }
    }

    ecs_script_vars_t *vars = flecs_script_vars_push(
        NULL, &v->r->stack, v->r->allocator);
    vars->parent = template->vars;
    vars->sp = ecs_vec_count(&template->vars->vars);

    ecs_script_vars_set_size(vars,
        ecs_vec_count(&template->members) + 1 + (template->parent_type != 0));

    ecs_script_var_t *this_var = ecs_script_vars_declare(vars, NULL);
    this_var->value.type = ecs_id(ecs_entity_t);
    this_var->value.ptr = &instance;
    this_var->owned = false;

    if (template->parent_type) {
        ecs_script_var_t *parent_var = ecs_script_vars_declare(vars, "#parent");
        parent_var->value.type = ecs_id(ecs_entity_t);
        parent_var->value.ptr = &parent;
        parent_var->owned = false;
    }

    const ecs_type_info_t *ti = template->type_info;
    const EcsStruct *props_st = ecs_get(world, template_entity, EcsStruct);
    const EcsStruct *muts_st = template->muts.type
        ? ecs_get(world, template->muts.type, EcsStruct)
        : NULL;
    void *props_data = ECS_CONST_CAST(void*,
        ecs_get_id(world, instance, template->props.type));
    void *muts_data = template->muts.type
        ? ECS_CONST_CAST(void*,
            ecs_get_id(world, instance, template->muts.type))
        : NULL;

    void *props_copy = NULL;
    if (props_data && ti && ti->size) {
        props_copy = flecs_stack_alloc(&v->r->stack, ti->size, ti->alignment);
        ecs_os_memcpy(props_copy, props_data, ti->size);
        props_data = props_copy;
    }

    flecs_script_template_instantiate_vars(world, vars, template,
        props_st, props_data, muts_st, muts_data);

    v->vars = vars;

    if (!state.initialized && template->muts.type) {
        if (flecs_script_template_init_muts(v, template)) {
            goto done_vars;
        }
    }

    if (template->has_interface_members) {
        if (flecs_script_template_validate_interfaces(world,
            template_entity, template, props_st, props_data,
            muts_st, muts_data))
        {
            goto done_vars;
        }
    }

    if (flecs_script_eval_run(&runner, &template->node->node)) {
        goto done_vars;
    }

    root = ecs_ensure_pair(world, instance, EcsScriptTemplateRoot, template_entity);
    root->state.initialized = true;
    root->changed = 0;

    flecs_script_update_dyn_observers(world, template_entity, instance,
        &run_refs, &root->observers, input);

    result = 0;

done_vars:
    if (props_copy) {
        flecs_stack_free(props_copy, ti->size);
    }

    ecs_script_vars_pop(vars);

done:
    v->r->using = prev_using;
    ecs_vec_fini_t(NULL, &run_refs, ecs_script_ref_t);
    flecs_script_runner_fini(&runner, &desc);
    flecs_script_runtime_release_call(pool, desc.runtime);
    return result;
}

static int flecs_script_template_instantiate_body(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_script_template_t *template,
    ecs_entity_t instance,
    uint64_t input,
    bool force,
    bool *ran)
{
    ecs_assert(!ecs_is_stage(world), ECS_INTERNAL_ERROR, NULL);

    int result = 0;
    if (!ecs_is_alive(world, instance) ||
        !ecs_has_id(world, instance, template_entity))
    {
        goto observe;
    }

    ecs_script_runtime_t *rt = flecs_script_runtime_get(world);
    EcsScriptTemplateRoot *root = ecs_get_mut_pair(
        world, instance, EcsScriptTemplateRoot, template_entity);
    bool propagate = (rt->eval_depth || rt->template_depth) &&
        (!root || !root->state.initialized);
    bool prev_error = rt->error, body_ran = false;
    if (!propagate) {
        rt->error = false;
        flecs_log_capture_push(true);
    }

    if (rt->template_depth >= FLECS_SCRIPT_TEMPLATE_DEPTH_MAX) {
        ecs_err("too many nested instantiations of template '%s', "
            "is the template instantiating itself?",
                ecs_get_name(world, template_entity));
        result = -1;
        goto done;
    }

    if (instance == template_entity) {
        ecs_err("cannot instantiate template '%s' on itself",
            ecs_get_name(world, template_entity));
        result = -1;
        goto done;
    }

    ecs_entity_t parent = template->parent_type
        ? ecs_get_parent(world, instance) : 0;
    if (template->parent_type &&
        (!parent || !ecs_has_id(world, parent, template->parent_type)))
    {
        ecs_err("template '%s' requires a parent with template '%s'",
            ecs_get_name(world, template_entity),
            ecs_get_name(world, template->parent_type));
        result = -1;
        goto done;
    }

    root = ecs_ensure_pair(
        world, instance, EcsScriptTemplateRoot, template_entity);
    flecs_script_template_root_init(root, template);
    uint64_t run_input = root->state.initialized ? input : UINT64_MAX;
    if (root->evaluating || (!run_input && !force)) {
        goto done;
    }

    if ((run_input == UINT64_MAX || (run_input & template->async_input)) &&
        ecs_vec_count(&root->async_blocks))
    {
        flecs_script_template_root_tasks_free(root);
    }

    body_ran = true;
    root->evaluating = true;
    rt->template_depth ++;
    result = flecs_script_template_run(world, template_entity, template,
        instance, parent, root, force ? UINT64_MAX : run_input);
    rt->template_depth --;
    root = ecs_is_alive(world, instance) ? ecs_get_mut_pair(world, instance,
        EcsScriptTemplateRoot, template_entity) : NULL;
    if (root) {
        root->evaluating = false;
    }

done:
    *ran |= body_ran;
    if (propagate) {
        if (result) {
            if (!rt->error_name && template->script->pub.name) {
                rt->error_name = ecs_os_strdup(template->script->pub.name);
            }

            rt->error = true;
        }
    } else {
        result = rt->error ? -1 : result;
        rt->error = prev_error;
        char *error = flecs_log_capture_pop();
        if (body_ran || result) {
            flecs_script_report_error(world, template_entity,
                template->script->pub.name, result != 0, error);
        } else {
            ecs_os_free(error);
        }
    }

observe:
    if (ecs_vec_count(&template->refs)) {
        EcsScript *s = ecs_get_mut(world, template_entity, EcsScript);
        if (!ecs_vec_count(&s->observers)) {
            flecs_script_observe_refs(world, template_entity,
                &template->refs, &s->observers);
        }
    }

    return result;
}

static uint64_t flecs_script_template_base_input(
    const ecs_script_template_t *template,
    const ecs_script_template_t *base_template,
    uint64_t input)
{
    if (input == UINT64_MAX) {
        return input;
    }

    uint64_t result = 0;
    const ecs_script_template_member_t *members = ecs_vec_first(
        &template->members);
    int32_t i, count = template->inherited_count;
    for (i = 0; i < count; i ++) {
        if (!(members[i].input & input)) {
            continue;
        }

        const ecs_script_template_member_t *base_member =
            flecs_script_template_member(
                base_template, members[i].index, members[i].is_mut);
        if (base_member) {
            result |= base_member->input;
        }
    }

    return result;
}

static int flecs_script_template_instantiate_chain(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t input_template,
    ecs_entity_t instance,
    uint64_t input,
    bool *ran)
{
    ecs_script_template_t *template = flecs_script_template_get(
        world, template_entity);
    if (!template) {
        ecs_err("template is missing script component");
        return -1;
    }

    bool has_input = template_entity == input_template;
    bool base_ran = false;
    ecs_script_template_t *base_template = flecs_script_template_get(
        world, template->base);
    if (base_template) {
        int result = flecs_script_template_instantiate_chain(world,
            template->base, has_input ? template->base : input_template,
            instance, has_input
                ? flecs_script_template_base_input(
                    template, base_template, input)
                : input,
            &base_ran);
        if (result) {
            return result;
        }
    }

    if (base_ran) {
        *ran = true;
    }

    return flecs_script_template_instantiate_body(world, template_entity,
        template, instance, has_input ? input : 0, base_ran, ran);
}

/* Returns the most derived template stored on the instance that inherits from
 * the specified template. */
static ecs_entity_t flecs_script_template_instance_template(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t instance)
{
    ecs_id_t stored = 0;
    if (ecs_search(world, ecs_get_table(world, instance),
        template_entity, &stored) != -1 && stored != template_entity &&
        flecs_script_template_get(world, stored))
    {
        return stored;
    }

    return template_entity;
}

static int flecs_script_template_instantiate(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t instance,
    uint64_t input)
{
    if (!flecs_script_template_get(world, template_entity) ||
        !ecs_is_alive(world, instance) ||
        !ecs_has_id(world, instance, template_entity))
    {
        return 0;
    }

    ecs_entity_t instance_template = flecs_script_template_instance_template(
        world, template_entity, instance);
    if (ecs_has_id(world, instance_template, EcsScriptTemplateManual)) {
        ecs_add_pair(world, instance, EcsScriptTemplatePending,
            instance_template);
        return 0;
    }

    bool ran = false;
    return flecs_script_template_instantiate_chain(world, instance_template,
        template_entity, instance, input, &ran);
}

static int flecs_script_template_instantiate_all(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    uint64_t input)
{
    ecs_vec_t instances;
    ecs_vec_init_t(NULL, &instances, ecs_entity_t, 0);

    ecs_iter_t it = ecs_each_id(world, template_entity);
    while (ecs_each_next(&it)) {
        ecs_os_memcpy_n(ecs_vec_grow_t(NULL, &instances, ecs_entity_t,
            it.count), it.entities, ecs_entity_t, it.count);
    }

    int result = 0;
    ecs_entity_t *entities = ecs_vec_first(&instances);
    int32_t i, count = ecs_vec_count(&instances);
    for (i = 0; i < count; i ++) {
        if (flecs_script_template_instantiate(
            world, template_entity, entities[i], input))
        {
            result = -1;
        }
    }

    ecs_vec_fini_t(NULL, &instances, ecs_entity_t);
    return result;
}

int ecs_script_template_update(
    ecs_world_t *world,
    ecs_entity_t instance,
    ecs_entity_t template_entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!ecs_is_stage(world) && !ecs_stage_is_readonly(world),
        ECS_INVALID_OPERATION, NULL);
    if (!flecs_script_template_get(world, template_entity) ||
        !ecs_is_alive(world, instance) ||
        !ecs_has_id(world, instance, template_entity))
    {
        return -1;
    }

    ecs_remove_pair(world, instance, EcsScriptTemplatePending, template_entity);
    bool ran = false;
    return flecs_script_template_instantiate_chain(world,
        flecs_script_template_instance_template(
            world, template_entity, instance),
        template_entity, instance, UINT64_MAX, &ran);
error:
    return -1;
}

void flecs_script_template_reeval(
    ecs_world_t *world,
    ecs_entity_t template_entity,
    ecs_entity_t instance,
    uint64_t input)
{
    if (instance) {
        flecs_script_template_instantiate(
            world, template_entity, instance, input);
    } else {
        flecs_script_template_instantiate_all(world, template_entity, input);
    }
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

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        const EcsScriptTemplateRoot *root = ecs_get_pair(it->world,
            it->entities[i], EcsScriptTemplateRoot, template_entity);
        uint64_t input = root && root->state.initialized
            ? root->changed
            : UINT64_MAX;
        if (input) {
            flecs_script_ref_enqueue(it->world, it->stage, template_entity,
                it->entities[i], input);
        }
    }
}

static void flecs_script_template_on_add(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t template_entity = ecs_field_id(it, 0);
    ecs_script_template_t *template = flecs_script_template_get(
        world, template_entity);
    if (!template) {
        return;
    }

    for (; template; template = flecs_script_template_get(
        world, template->base))
    {
        template->refcount += it->count;
    }

    if (it->count == 1 && flecs_stage_is_ensure_add(
        it->world, it->entities[0], template_entity))
    {
        return;
    }

    flecs_script_template_on_set(it, template_entity);
}

static void flecs_script_template_on_replace(
    ecs_iter_t *it,
    ecs_entity_t template_entity,
    bool mut)
{
    ecs_world_t *world = it->world;
    ecs_script_template_t *template = flecs_script_template_get(
        world, template_entity);
    if (!template) {
        return;
    }

    ecs_entity_t component = ecs_field_id(it, 0);
    const ecs_type_info_t *ti = ecs_get_type_info(world, component);
    const EcsStruct *st = ecs_get(world, component, EcsStruct);
    if (!ti || !st) {
        return;
    }

    void *old_data = ecs_field_w_size(it, flecs_itosize(ti->size), 0);
    void *new_data = ecs_field_w_size(it, flecs_itosize(ti->size), 1);
    ecs_script_template_member_t *members = ecs_vec_first(&template->members);
    int32_t i, m, member_count = ecs_vec_count(&template->members);
    for (i = 0; i < it->count; i ++) {
        void *old_ptr = ECS_OFFSET(old_data, ti->size * i);
        void *new_ptr = ECS_OFFSET(new_data, ti->size * i);
        if (!ecs_os_memcmp(old_ptr, new_ptr, ti->size)) {
            continue;
        }

        EcsScriptTemplateRoot *root = ECS_CONST_CAST(EcsScriptTemplateRoot*,
            ecs_get_pair(world, it->entities[i],
                EcsScriptTemplateRoot, template_entity));
        if (!root || !root->state.initialized) {
            continue;
        }

        for (m = 0; m < member_count; m ++) {
            ecs_script_template_member_t *template_member = &members[m];
            if (template_member->is_mut != mut) {
                continue;
            }

            const ecs_member_t *member = ecs_vec_get_t(
                &st->members, ecs_member_t, template_member->index);
            void *old_member = ECS_OFFSET(old_ptr, member->offset);
            void *new_member = ECS_OFFSET(new_ptr, member->offset);
            if (!ecs_os_memcmp(old_member, new_member, member->size)) {
                continue;
            }

            const ecs_type_info_t *member_ti = ecs_get_type_info(
                world, member->type);
            ecs_assert(member_ti != NULL, ECS_INTERNAL_ERROR, NULL);
            const ecs_type_hooks_t *h = &member_ti->hooks;
            if ((!h->ctor && !h->copy && !h->move && !h->dtor) ||
                !h->equals || (h->flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL))
            {
                root->changed |= template_member->input;
                continue;
            }

            int32_t e, elem_count = member->count ? member->count : 1;
            for (e = 0; e < elem_count; e ++) {
                ecs_size_t elem_offset = member_ti->size * e;
                if (!flecs_type_info_equals(
                    ECS_OFFSET(old_member, elem_offset),
                    ECS_OFFSET(new_member, elem_offset), member_ti))
                {
                    root->changed |= template_member->input;
                    break;
                }
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
    ecs_entity_t template_entity = ecs_get_parent(it->stage, muts_entity);
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
    ecs_entity_t template_entity = ecs_get_parent(it->stage, muts_entity);
    flecs_script_template_on_set(it, template_entity);
}

static void flecs_script_template_on_remove(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->stage;
    ecs_entity_t removed = ecs_field_id(it, 0);
    ecs_script_template_t *template = flecs_script_template_get(
        world, removed);
    if (!template) {
        return;
    }

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_pair(world, it->entities[i],
            EcsScriptTemplatePending, removed);
    }

    ecs_entity_t template_entity = removed;
    do {
        for (i = 0; i < it->count; i ++) {
            if (template_entity == removed || !flecs_script_template_base_in_use(
                ecs_get_world(world), it->entities[i], removed,
                template_entity))
            {
                ecs_remove_pair(world, it->entities[i],
                    ecs_id(EcsScriptTemplateRoot), template_entity);
            }
        }

        template->refcount -= it->count;
        if (template->refcount <= 0) {
            template->refcount = 0;
            flecs_script_ref_observers_clear(world,
                &ecs_get_mut(world, template_entity, EcsScript)->observers);
        }

        template_entity = template->base;
        template = flecs_script_template_get(world, template_entity);
    } while (template);
}

static int flecs_script_template_override_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_var_node_t *node)
{
    ecs_script_template_t *template = v->template;
    const EcsStruct *st = ecs_get(v->world, template->props.type, EcsStruct);
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
    int32_t index;
    int32_t count = template->inherited_count - template->inherited_mut_count;
    for (index = 0; index < count; index ++) {
        if (!ecs_os_strcmp(members[index].name, node->name)) {
            break;
        }
    }

    ecs_assert(index < count, ECS_INTERNAL_ERROR, NULL);

    if (!node->expr) {
        flecs_script_eval_error(v, node,
            "prop '%s' of template '%s' overrides a base prop and must "
            "specify a default value", node->name,
            ecs_get_name(v->world, template->props.type));
        return -1;
    }

    ecs_entity_t member_type = flecs_script_template_member_type(
        v->world, &members[index]);
    const ecs_script_template_member_t *member = 
        flecs_script_template_member(template, index, false);
    ecs_assert(member != NULL, ECS_INTERNAL_ERROR, NULL);

    if (node->eval_type != member_type || 
        node->eval_interface != member->interface)
    {
        flecs_script_eval_error(v, node,
            "type of prop '%s' of template '%s' does not match the type "
            "of the base prop it overrides", node->name,
            ecs_get_name(v->world, template->props.type));
        return -1;
    }

    ecs_script_var_t *var = ecs_script_vars_lookup(v->vars, node->name);
    ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_script_eval_expr(v, &node->expr, &var->value)) {
        return -1;
    }

    node->sp = var->sp;
    node->base_member = index + 1;

    return 0;
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
        ecs_vec_count(&template->members) - template->inherited_count)
    {
        flecs_script_eval_error(v, node,
            "const variables declared before %s '%s' (props and muts must "
            "come first)", mut ? "mut" : "prop", node->name);
        return -1;
    }

    if (flecs_script_struct_member_is_inherited(
        v->world, template->props.type, node->name))
    {
        if (mut) {
            flecs_script_eval_error(v, node,
                "mut '%s' of template '%s' is already defined by base type",
                node->name, ecs_get_name(v->world, template->props.type));
            return -1;
        }

        return flecs_script_template_override_var(v, node);
    }

    if (template->inherited_mut_count &&
        flecs_script_struct_member_is_inherited(
            v->world, template->muts.type, node->name))
    {
        flecs_script_eval_error(v, node,
            "%s '%s' of template '%s' is already defined as mut by base type",
            mut ? "mut" : "prop", node->name,
            ecs_get_name(v->world, template->props.type));
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

    if (node->expr) {
        if (flecs_script_eval_expr(v, &node->expr, &var->value)) {
            return -1;
        }
    } else if (node->eval_interface && !node->type_is_vector &&
        node->eval_interface != ecs_id(EcsStruct))
    {
        ((ecs_script_template_ref_t*)var->value.ptr)->type =
            node->eval_interface;
    }

    if (node->type_is_vector && node->eval_interface) {
        const ecs_vec_t *vec = var->value.ptr;
        const ecs_script_template_ref_t *elems = ecs_vec_first(vec);
        int32_t e, elem_count = ecs_vec_count(vec);
        for (e = 0; e < elem_count; e ++) {
            const char *expected = flecs_script_template_interface_error(
                v->world, elems[e].type, node->eval_interface);
            if (expected) {
                flecs_script_eval_error(v, node,
                    "'%s' at index %d of prop '%s' is not %s",
                    elems[e].type ? flecs_errstr(
                        ecs_get_path(v->world, elems[e].type)) : "0",
                    e, node->name, expected);
                return -1;
            }
        }
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

    if (ecs_struct_add_member(v->world, vars->type, &(ecs_member_t){
        .name = node->name,
        .type = var->value.type
    }))
    {
        flecs_script_eval_error(v, node,
            "failed to add %s '%s' to template type",
            mut ? "mut" : "prop", node->name);
        return -1;
    }

    ecs_script_template_member_t *member = ecs_vec_append_t(
        &v->base.script->allocator, &template->members,
        ecs_script_template_member_t);
    member->index = ecs_vec_count(&vars->defaults) - 1;
    member->sp = var->sp;
    member->input = 0;
    member->is_mut = mut;
    member->is_vector = node->type_is_vector;
    member->interface = node->eval_interface;

    if (member->interface) {
        template->has_interface_members = true;
    }

    return 0;
}

static void flecs_script_template_declare_inherited_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_vars_t *vars,
    int32_t count)
{
    if (!count) {
        return;
    }

    const EcsStruct *st = ecs_get(v->world, vars->type, EcsStruct);
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vec_count(&st->members) >= count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vec_count(&vars->defaults) >= count,
        ECS_INTERNAL_ERROR, NULL);

    const ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
    ecs_script_var_t *defaults = ecs_vec_first(&vars->defaults);
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *var = ecs_script_vars_declare(
            v->vars, members[i].name);
        ecs_assert(var != NULL, ECS_INTERNAL_ERROR, NULL);
        var->value.type = flecs_script_template_member_type(
            v->world, &members[i]);
        var->value.ptr = defaults[i].value.ptr;
        var->type_info = defaults[i].type_info;
        var->owned = false;
    }
}

static int flecs_script_template_inherit_members(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template,
    ecs_script_template_node_t *node,
    const ecs_script_template_t *base_template,
    bool mut)
{
    ecs_world_t *world = v->world;
    ecs_allocator_t *a = &v->base.script->allocator;
    ecs_script_template_vars_t *vars = mut ? &template->muts : &template->props;

    if (!ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = vars->type
    })) {
        flecs_script_eval_error(v, node,
            "failed to inherit members of base '%s' for template '%s'",
            node->base, node->name);
        return -1;
    }

    const EcsStruct *st = ecs_get(world, vars->type, EcsStruct);
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t i, count = flecs_struct_inherited_count(world, vars->type, st);
    if (!count) {
        return 0;
    }

    const ecs_script_var_t *base_defaults = NULL;
    if (base_template) {
        const ecs_script_template_vars_t *base_vars = mut
            ? &base_template->muts : &base_template->props;
        if (ecs_vec_count(&base_vars->defaults) != count) {
            flecs_script_eval_error(v, node,
                "base template '%s' has %d %s defaults, expected %d",
                node->base, ecs_vec_count(&base_vars->defaults),
                mut ? "mut" : "prop", count);
            return -1;
        }

        base_defaults = ecs_vec_first(&base_vars->defaults);
    }

    int32_t sp = ecs_vec_count(&template->vars->vars) + 1 +
        (template->parent_type != 0) + template->inherited_count;

    const ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t member_type = flecs_script_template_member_type(
            world, &members[i]);
        const ecs_type_info_t *ti = ecs_get_type_info(world, member_type);
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_set_rtt_equals(world, member_type);

        ecs_script_var_t *value = ecs_vec_append_t(
            a, &vars->defaults, ecs_script_var_t);
        ecs_os_zeromem(value);
        value->name = members[i].name;
        value->value.ptr = flecs_calloc_w_dbg_info(a, ti->size, ti->name);
        value->value.type = member_type;
        value->type_info = ti;
        value->owned = false;
        if (base_defaults) {
            ecs_ptr_copy_w_type_info(world, ti,
                value->value.ptr, base_defaults[i].value.ptr);
        } else {
            flecs_type_info_ctor(value->value.ptr, 1, ti);
        }

        ecs_script_template_member_t *member = ecs_vec_append_t(
            a, &template->members, ecs_script_template_member_t);
        member->index = i;
        member->sp = sp + i;
        member->input = 0;
        member->is_mut = mut;
        const ecs_script_template_member_t *base_member =
            flecs_script_template_member(base_template, i, mut);
        member->is_vector = base_member && base_member->is_vector;
        member->interface = base_member ? base_member->interface : 0;
        if (member->interface) {
            template->has_interface_members = true;
        }
    }

    template->inherited_count += count;
    if (mut) {
        template->inherited_mut_count = count;
    }

    return 0;
}

static int flecs_script_template_inherit(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template,
    ecs_script_template_node_t *node)
{
    ecs_world_t *world = v->world;
    ecs_entity_t base = node->eval_base;
    ecs_entity_t template_entity = template->props.type;

    ecs_script_template_t *base_template = flecs_script_template_get(
        world, base);
    bool has_members = ecs_has(world, base, EcsStruct);
    if (!has_members && !base_template) {
        flecs_script_eval_error(v, node,
            "base '%s' of template '%s' is not a struct type",
            node->base, node->name);
        return -1;
    }

    if (base == template_entity) {
        flecs_script_eval_error(v, node,
            "template '%s' cannot inherit from itself", node->name);
        return -1;
    }

    ecs_add_pair(world, template_entity, EcsIsA, base);

    if (base_template) {
        template->base = base;
    }

    if (has_members && flecs_script_template_inherit_members(
        v, template, node, base_template, false))
    {
        return -1;
    }

    if (!base_template || !base_template->muts.type) {
        return 0;
    }

    const ecs_script_template_t *cur = base_template;
    while (cur) {
        if (cur->muts.type) {
            ecs_remove_pair(world, template_entity, EcsWith, cur->muts.type);
        }

        cur = flecs_script_template_get(world, cur->base);
    }

    template->muts.type = ecs_entity(world, {
        .name = "mut", .parent = template_entity
    });

    ecs_add_pair(world, template->muts.type, EcsIsA, base_template->muts.type);

    return flecs_script_template_inherit_members(
        v, template, node, base_template, true);
}

bool flecs_script_template_is_mut_type(
    const ecs_world_t *world,
    ecs_id_t component)
{
    if (!component || ECS_IS_PAIR(component) ||
        !ecs_is_alive(world, component))
    {
        return false;
    }

    const ecs_script_template_t *template = flecs_script_template_get(
        world, ecs_get_parent(world, component));
    return template && template->muts.type == component;
}

bool flecs_script_template_interface_accepts(
    const ecs_world_t *world,
    ecs_entity_t value,
    ecs_entity_t interface)
{
    if (interface == ecs_id(EcsStruct)) {
        return ecs_has(world, value, EcsStruct);
    }

    if (value == interface) {
        return true;
    }

    return flecs_script_template_get(world, value) &&
        flecs_struct_is_derived_from(world, value, interface);
}

char* flecs_script_template_expected_str(
    const ecs_world_t *world,
    ecs_entity_t interface)
{
    if (interface == ecs_id(EcsStruct)) {
        return ecs_os_strdup("a struct type");
    }

    char *path = ecs_get_path(world, interface);
    char *result = flecs_asprintf("a template derived from '%s'", path);
    ecs_os_free(path);
    return result;
}

const char* flecs_script_template_interface_error(
    const ecs_world_t *world,
    ecs_entity_t value,
    ecs_entity_t interface)
{
    if (value && ecs_is_alive(world, value) &&
        flecs_script_template_interface_accepts(world, value, interface))
    {
        return NULL;
    }

    return flecs_errstr_5(flecs_script_template_expected_str(world, interface));
}

ecs_entity_t flecs_script_template_prop_interface(
    const ecs_world_t *world,
    ecs_entity_t type,
    const char *member)
{
    const ecs_script_template_t *template = flecs_script_template_get(
        world, type);
    const EcsStruct *st = template ? ecs_get(world, type, EcsStruct) : NULL;
    if (!st || !member) {
        return 0;
    }

    const ecs_member_t *members = ecs_vec_first(&st->members);
    int32_t i, count = ecs_vec_count(&st->members);
    for (i = 0; i < count; i ++) {
        if (!ecs_os_strcmp(members[i].name, member)) {
            const ecs_script_template_member_t *m =
                flecs_script_template_member(template, i, false);
            return m ? m->interface : 0;
        }
    }

    return 0;
}

ecs_entity_t flecs_script_template_member_interface(
    const ecs_script_template_t *template,
    int32_t sp)
{
    const ecs_script_template_member_t *m =
        flecs_script_template_member_by_sp(template, sp);
    return m ? m->interface : 0;
}

bool flecs_script_template_member_is_vector(
    const ecs_script_template_t *template,
    int32_t sp)
{
    const ecs_script_template_member_t *m =
        flecs_script_template_member_by_sp(template, sp);
    return m && m->is_vector;
}

static int flecs_script_template_preprocess(
    ecs_script_eval_visitor_t *v,
    ecs_script_template_t *template)
{
    ecs_script_type_visitor_t *t = v->type_visitor;
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_template_node_t *tnode = template->node;
    if (tnode->symbol_offset == -1) {
        tnode->symbol_offset = ecs_vec_count(&v->base.script->state.symbol_slots);
    }

    template->symbol_offset = tnode->symbol_offset;
    if (tnode->root_symbol == -1) {
        tnode->root_symbol = flecs_script_type_slot_new(t, -1);
    }

    template->root_symbol = tnode->root_symbol;

    int32_t old_table = t->table;
    bool old_template_scope = t->template_scope;
    int32_t old_for_depth = t->for_depth;
    t->for_depth = 0;
    flecs_script_entity_state_t *old_entity = v->entity;
    ecs_script_template_t *old_template = v->template;
    ecs_script_template_t *old_instance_template = v->instance_template;
    ecs_entity_t old_script_entity = v->script_entity;
    v->template = template;
    v->instance_template = template;
    v->script_entity = 0;
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
        NULL, &v->r->stack, v->r->allocator);
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

    if (template->parent_type) {
        ecs_script_var_t *parent_var = ecs_script_vars_declare(v->vars, "#parent");
        parent_var->value.type = ecs_id(ecs_entity_t);
        template->parent_sp = parent_var->sp;
    }

    flecs_script_template_declare_inherited_vars(v, &template->props,
        template->inherited_count - template->inherited_mut_count);
    flecs_script_template_declare_inherited_vars(v, &template->muts,
        template->inherited_mut_count);

    int32_t table = flecs_script_type_table_new(
        t, old_table, NULL);
    int result = flecs_script_type_scope(
        t, template->node->scope, table, true, false);

    v->vars = ecs_script_vars_pop(v->vars);
    ecs_script_vars_pop(v->vars);
    v->vars = outer_vars;
    t->template_scope = old_template_scope;
    t->for_depth = old_for_depth;
    t->table = old_table;
    v->entity = old_entity;
    v->script_entity = old_script_entity;
    v->instance_template = old_instance_template;
    v->template = old_template;
    if (tnode->symbol_count == -1) {
        tnode->symbol_count = ecs_vec_count(
            &v->base.script->state.symbol_slots) - template->symbol_offset;
    }

    template->symbol_count = tnode->symbol_count;
    return result;
}

static void flecs_script_template_hoist_using(
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
}

static void flecs_script_template_hoist_vars(
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
}

static ecs_script_template_t* flecs_script_template_init(
    ecs_script_impl_t *script)
{
    ecs_allocator_t *a = &script->allocator;
    ecs_script_template_t *result = flecs_calloc_t(a, ecs_script_template_t);
    result->script = script;
    ecs_vec_init_t(NULL, &result->props.defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->muts.defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->members, ecs_script_template_member_t, 0);
    ecs_vec_init_t(NULL, &result->using_, ecs_entity_t, 0);
    ecs_vec_init_t(NULL, &result->annot, ecs_script_annot_t*, 0);
    ecs_vec_init_t(NULL, &result->refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->capture_sp, int32_t, 0);
    ecs_vec_init_t(NULL, &result->capture_input,
        ecs_script_template_capture_t, 0);
    result->async_input = UINT64_MAX;
    result->root_symbol = -1;
    result->parent_sp = -1;
    result->generation = 1;
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

    flecs_script_template_hoist_using(v, template);
    flecs_script_template_hoist_vars(v, template, v->vars);

    template->parent_type = node->eval_parent;
    if (node->base) {
        const ecs_script_template_t *base_template = flecs_script_template_get(
            v->world, node->eval_base);
        if (base_template) {
            ecs_entity_t inherited_parent = base_template->parent_type;
            if (inherited_parent && template->parent_type &&
                inherited_parent != template->parent_type)
            {
                flecs_script_eval_error(v, node,
                    "parent constraint conflicts with base template '%s'",
                    node->base);
                goto error;
            }

            if (!template->parent_type) {
                template->parent_type = inherited_parent;
            }
        }
    }

    if (template->parent_type) {
        if (!flecs_script_template_get(v->world, template->parent_type)) {
            flecs_script_eval_error(v, node,
                "parent constraint must name a template");
            goto error;
        }
    }

    if (node->base && flecs_script_template_inherit(v, template, node)) {
        goto error;
    }

    ecs_vec_t outer_annot = v->r->annot;
    ecs_vec_init_t(v->r->allocator,
        &v->r->annot, ecs_script_annot_t*, 0);
    int preprocess_result = flecs_script_template_preprocess(v, template);
    ecs_vec_fini_t(v->r->allocator,
        &v->r->annot, ecs_script_annot_t*);
    v->r->annot = outer_annot;
    if (preprocess_result) {
        goto error;
    }

    if (!ecs_has(v->world, template_entity, EcsStruct)) {
        if (!ecs_struct_init(v->world, &(ecs_struct_desc_t){
            .entity = template_entity }))
        {
            goto error;
        }
    }

    /* If template has mut properties, add those when the template is added */
    if (template->muts.type) {
        ecs_add_id(v->world, template->muts.type, EcsSparse);
        ecs_add_pair(v->world, template_entity, EcsWith, template->muts.type);
    }

    /* Consume annotations, if any */
    int32_t i, count = ecs_vec_count(&v->r->annot);
    if (count) {
        ecs_script_annot_t **annots = ecs_vec_first(&v->r->annot);
        for (i = 0; i < count ; i ++) {
            ecs_script_annot_t *annot = annots[i];
            if (!ecs_os_strcmp(annot->name, "tree")) {
                if (ecs_os_strcmp(annot->expr, "Parent") &&
                    ecs_os_strcmp(annot->expr, "ChildOf"))
                {
                    flecs_script_eval_error(v, annot,
                        "invalid value for tree annotation: '%s' (expected "
                        "'Parent' or 'ChildOf')", annot->expr);
                    goto error;
                }

                ecs_script_entity_t tree_node = {
                    .node = { .kind = EcsAstEntity, .pos = node->node.pos },
                    .scope = node->scope
                };
                flecs_script_apply_tree_annot(annot, &tree_node);
                template->non_fragmenting_parent =
                    tree_node.non_fragmenting_parent;

                continue;
            }

            ecs_vec_append_t(&v->base.script->allocator, 
                &template->annot, ecs_script_annot_t*)[0] = annot;
        }

        ecs_vec_clear(&v->r->annot);
    }

    template->type_info = ecs_get_type_info(v->world, template_entity);

    EcsScript *script = ecs_ensure(v->world, template_entity, EcsScript);
    if (script->script) {
        if (script->template_) {
            template->refcount = script->template_->refcount;
            template->generation = script->template_->generation + 1;
            flecs_script_ref_observers_clear(v->world, &script->observers);
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
    ecs_script_template_t *template = flecs_script_template_get(
        v->world, entity);
    ecs_assert(template != NULL, ECS_INTERNAL_ERROR, NULL);
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
            if ((captures[i].outer_input & v->input) ||
                (captures[i].outer_internal & v->internal))
            {
                input |= captures[i].input;
            }
        }
    }

    return flecs_script_template_instantiate_all(v->world, entity, input);
}

void flecs_script_template_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateRoot);
    ECS_COMPONENT_DEFINE(world, ecs_script_template_ref_t);

    ecs_set_hooks(world, ecs_script_template_ref_t, {
        .ctor = flecs_default_ctor,
        .copy = flecs_script_template_ref_copy,
        .move = flecs_script_template_ref_move,
        .dtor = flecs_script_template_ref_dtor,
        .cmp = flecs_script_template_ref_cmp,
        .equals = flecs_script_template_ref_equals,
        .ctx = world
    });

    ecs_opaque(world, {
        .entity = ecs_id(ecs_script_template_ref_t),
        .type.as_type = ecs_id(ecs_entity_t),
        .type.serialize = flecs_script_template_ref_serialize,
        .type.assign_entity = flecs_script_template_ref_assign_entity
    });
    ECS_TAG_DEFINE(world, EcsScriptTemplate);
    ECS_TAG_DEFINE(world, EcsScriptTemplateManual);
    ECS_TAG_DEFINE(world, EcsScriptTemplatePending);

    ecs_add_id(world, EcsScriptTemplatePending, EcsPairIsTag);
    ecs_add_pair(world, EcsScriptTemplatePending, EcsOnInstantiate, EcsDontInherit);

    ecs_add_id(world, EcsScriptTemplate, EcsPairIsTag);
#ifdef FLECS_CONSTRAINT_TRAITS
    ecs_add_id(world, EcsScriptTemplate, EcsRelationship);
#endif

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
        .entity = ecs_entity(world, { .name = "TemplateRootRemoveObserver" }),
        .query.terms = {{
            .id = ecs_pair(ecs_id(EcsScriptTemplateRoot), EcsWildcard)
        }},
        .events = { EcsOnRemove },
        .callback = flecs_script_template_root_remove
    });
}

#endif
