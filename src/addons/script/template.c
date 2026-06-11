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

static
const ecs_type_info_t* flecs_script_template_member_ti(
    ecs_world_t *world,
    ecs_script_template_member_t *member)
{
    if (!ecs_is_alive(world, member->type)) {
        return NULL;
    }

    return ecs_get_type_info(world, member->type);
}

static
void flecs_script_template_free_prev_values(
    ecs_world_t *world,
    ecs_script_template_instance_t *state)
{
    if (!state->prev_values) {
        return;
    }

    int32_t i;
    for (i = 0; i < state->member_count; i ++) {
        const ecs_type_info_t *mti = flecs_script_template_member_ti(
            world, &state->members[i]);
        if (mti && mti->hooks.dtor) {
            mti->hooks.dtor(ECS_OFFSET(
                state->prev_values, state->members[i].offset), 1, mti);
        }
    }

    ecs_os_free(state->prev_values);
    state->prev_values = NULL;
}

static
void flecs_script_template_instance_free(
    ecs_world_t *world,
    ecs_script_template_instance_t *state)
{
    int32_t i;

    flecs_script_template_free_prev_values(world, state);

    ecs_os_free(state->members);

    int32_t dyn_count = ecs_vec_count(&state->dynamic_entities);
    for (i = 0; i < dyn_count; i ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, i);
        ecs_vec_fini_t(NULL, dvec, int64_t);
    }

    ecs_vec_fini_t(NULL, &state->dynamic_entities, ecs_vec_t);
    ecs_vec_fini_t(NULL, &state->entities, int64_t);
    ecs_vec_fini_t(NULL, &state->control, int8_t);
    ecs_vec_fini_t(NULL, &state->dynamic_ids, ecs_id_t);

    ecs_os_free(state);
}

static
void flecs_script_template_cache_fini(
    ecs_world_t *world,
    EcsScriptTemplateData *ptr)
{
    int32_t i, count = ecs_vec_count(&ptr->instances);
    ecs_script_template_instance_t **states = ecs_vec_first(&ptr->instances);
    for (i = 0; i < count; i ++) {
        flecs_script_template_instance_free(world, states[i]);
    }

    ecs_vec_fini_t(NULL, &ptr->instances, ecs_script_template_instance_t*);
}

static
void flecs_script_template_cache_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)ti;
    EcsScriptTemplateData *data = ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_vec_init_t(NULL, &data[i].instances,
            ecs_script_template_instance_t*, 0);
    }
}

static
void flecs_script_template_cache_move(
    void *dst_ptr,
    void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    EcsScriptTemplateData *dst = dst_ptr;
    EcsScriptTemplateData *src = src_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_template_cache_fini(world, &dst[i]);
        dst[i].instances = src[i].instances;
        ecs_vec_init_t(NULL, &src[i].instances,
            ecs_script_template_instance_t*, 0);
    }
}

static
void flecs_script_template_cache_copy(
    void *dst_ptr,
    const void *src_ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    (void)src_ptr;
    ecs_world_t *world = ti->hooks.ctx;
    EcsScriptTemplateData *dst = dst_ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_template_cache_fini(world, &dst[i]);
        ecs_vec_init_t(NULL, &dst[i].instances,
            ecs_script_template_instance_t*, 0);
    }
}

static
void flecs_script_template_cache_dtor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    EcsScriptTemplateData *data = ptr;
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_template_cache_fini(world, &data[i]);
    }
}

static
ecs_script_template_instance_t* flecs_script_template_instance_ensure(
    ecs_world_t *world,
    ecs_entity_t instance,
    ecs_script_template_t *template)
{
    EcsScriptTemplateData *st = ecs_ensure_id(world, instance,
        EcsScriptTemplate, sizeof(EcsScriptTemplateData));
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vec_count(&st->instances);
    ecs_script_template_instance_t **states = ecs_vec_first(&st->instances);
    for (i = 0; i < count; i ++) {
        if (states[i]->template_entity == template->entity) {
            return states[i];
        }
    }

    ecs_script_template_instance_t *result = ecs_os_calloc_t(
        ecs_script_template_instance_t);
    result->template_entity = template->entity;
    ecs_vec_init_t(NULL, &result->entities, int64_t, 0);
    ecs_vec_init_t(NULL, &result->dynamic_entities, ecs_vec_t, 0);
    ecs_vec_init_t(NULL, &result->control, int8_t, 0);
    ecs_vec_init_t(NULL, &result->dynamic_ids, ecs_id_t, 0);

    ecs_vec_append_t(NULL, &st->instances,
        ecs_script_template_instance_t*)[0] = result;

    return result;
}

static
void flecs_script_template_instance_prepare(
    ecs_world_t *world,
    ecs_script_template_t *template,
    const EcsStruct *st,
    ecs_script_template_instance_t *state)
{
    int32_t i;

    flecs_script_template_free_prev_values(world, state);

    ecs_os_free(state->members);
    state->members = NULL;
    state->member_count = 0;

    int32_t member_count = st ? st->members.count : 0;
    if (member_count && (member_count == template->prop_defaults.count)) {
        state->members = ecs_os_calloc_n(
            ecs_script_template_member_t, member_count);
        const ecs_member_t *members = st->members.array;
        for (i = 0; i < member_count; i ++) {
            state->members[i].offset = members[i].offset;
            state->members[i].type = members[i].type;
        }
        state->member_count = member_count;
    }

    ecs_vec_set_count_t(NULL, &state->entities, int64_t,
        template->slot_count);
    if (template->slot_count) {
        ecs_os_memset_n(ecs_vec_first(&state->entities), 0, int64_t,
            template->slot_count);
    }

    int32_t old_dyn = ecs_vec_count(&state->dynamic_entities);
    for (i = 0; i < old_dyn; i ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, i);
        ecs_vec_fini_t(NULL, dvec, int64_t);
    }

    ecs_vec_set_count_t(NULL, &state->dynamic_entities, ecs_vec_t,
        template->dyn_slot_count);
    for (i = 0; i < template->dyn_slot_count; i ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, i);
        ecs_vec_init_t(NULL, dvec, int64_t, 0);
    }

    ecs_vec_set_count_t(NULL, &state->control, int8_t,
        template->control_count);
    if (template->control_count) {
        ecs_os_memset(ecs_vec_first(&state->control), -1,
            template->control_count);
    }

    ecs_vec_set_count_t(NULL, &state->dynamic_ids, ecs_id_t,
        template->dyn_id_count);
    if (template->dyn_id_count) {
        ecs_os_memset_n(ecs_vec_first(&state->dynamic_ids), 0, ecs_id_t,
            template->dyn_id_count);
    }
}

static
uint64_t flecs_script_template_diff(
    ecs_world_t *world,
    ecs_script_template_instance_t *state,
    void *data)
{
    if (!state->member_count) {
        return UINT64_MAX;
    }

    uint64_t changed = 0;
    int32_t m;
    for (m = 0; m < state->member_count; m ++) {
        const ecs_type_info_t *mti = flecs_script_template_member_ti(
            world, &state->members[m]);
        if (!mti) {
            continue;
        }

        void *a = ECS_OFFSET(state->prev_values, state->members[m].offset);
        void *b = ECS_OFFSET(data, state->members[m].offset);
        bool equal;
        if (mti->hooks.equals &&
            !(mti->hooks.flags & ECS_TYPE_HOOK_EQUALS_ILLEGAL))
        {
            equal = mti->hooks.equals(a, b, mti);
        } else if (mti->hooks.cmp &&
            !(mti->hooks.flags & ECS_TYPE_HOOK_CMP_ILLEGAL))
        {
            equal = mti->hooks.cmp(a, b, mti) == 0;
        } else {
            equal = ecs_os_memcmp(a, b, mti->size) == 0;
        }

        if (!equal) {
            changed |= 1llu << m;
        }
    }

    return changed;
}

static
void flecs_script_template_store_prev(
    ecs_world_t *world,
    const ecs_type_info_t *ti,
    ecs_script_template_instance_t *state,
    void *data)
{
    int32_t m;

    if (!state->prev_values) {
        state->prev_values = ecs_os_calloc(ti->size);
        for (m = 0; m < state->member_count; m ++) {
            const ecs_type_info_t *mti = flecs_script_template_member_ti(
                world, &state->members[m]);
            if (!mti) {
                continue;
            }

            flecs_type_info_ctor(ECS_OFFSET(
                state->prev_values, state->members[m].offset), 1, mti);
        }
    }

    for (m = 0; m < state->member_count; m ++) {
        const ecs_type_info_t *mti = flecs_script_template_member_ti(
            world, &state->members[m]);
        if (!mti) {
            continue;
        }

        ecs_value_copy_w_type_info(world, mti,
            ECS_OFFSET(state->prev_values, state->members[m].offset),
            ECS_OFFSET(data, state->members[m].offset));
    }
}

bool flecs_script_template_skip_node(
    ecs_script_eval_visitor_t *v,
    ecs_script_node_t *node)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    if (ctx->create || ctx->force) {
        return false;
    }

    switch(node->kind) {
    case EcsAstTag:
    case EcsAstComponent:
    case EcsAstVarComponent:
    case EcsAstDefaultComponent:
    case EcsAstEntity:
    case EcsAstIf:
    case EcsAstFor:
    case EcsAstWith:
    case EcsAstPairScope:
        break;
    case EcsAstScope:
    case EcsAstWithVar:
    case EcsAstWithTag:
    case EcsAstWithComponent:
    case EcsAstUsing:
    case EcsAstModule:
    case EcsAstAnnotation:
    case EcsAstTemplate:
    case EcsAstProp:
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstInclude:
    case EcsAstFunction:
        return false;
    }

    if (node->depends_on & ctx->changed) {
        return false;
    }

    if (node->contains_entities && (ctx->loop_depth || ctx->with_changed)) {
        return false;
    }

    if (node->kind == EcsAstEntity) {
        ecs_vec_clear(&v->r->annot);
    }

    return true;
}

ecs_entity_t flecs_script_template_slot_entity(
    ecs_script_eval_visitor_t *v,
    int32_t slot)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    if (!ctx || slot <= 0) {
        return 0;
    }

    if (slot > ecs_vec_count(&ctx->state->entities)) {
        return 0;
    }

    int64_t e = ecs_vec_get_t(&ctx->state->entities, int64_t, slot - 1)[0];
    if (e <= 0) {
        return 0;
    }

    return (ecs_entity_t)e;
}

int flecs_script_template_entity(
    ecs_script_eval_visitor_t *v,
    ecs_script_entity_t *node,
    bool *reused)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_script_template_instance_t *state = ctx->state;
    ecs_world_t *world = v->world;

    if (!node->slot && !node->dyn_slot) {
        if (node->name_expr) {
            node->eval = flecs_script_eval_name_expr(
                v, node, &node->name_expr, false);
        } else {
            node->eval = flecs_script_create_entity(v, node->name);
        }

        if (!node->eval) {
            return -1;
        }

        reused[0] = false;
        return 0;
    }

    int64_t *slot_ptr;
    if (node->dyn_slot) {
        int32_t d = node->dyn_slot - 1;
        if (node->slot) {
            ecs_vec_get_t(&state->entities, int64_t, node->slot - 1)[0] =
                -(int64_t)node->dyn_slot;
        }

        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, d);
        int32_t k = ctx->cursors[d] ++;
        if (k >= ecs_vec_count(dvec)) {
            ecs_vec_append_t(NULL, dvec, int64_t)[0] = 0;
        }

        slot_ptr = ecs_vec_get_t(dvec, int64_t, k);
    } else {
        ecs_assert(node->slot != 0, ECS_INTERNAL_ERROR, NULL);
        slot_ptr = ecs_vec_get_t(&state->entities, int64_t, node->slot - 1);
    }

    ecs_entity_t e = 0;
    if (slot_ptr[0] > 0) {
        e = (ecs_entity_t)slot_ptr[0];
    }

    if (e) {
        node->eval = e;
        reused[0] = true;

        if (node->name_expr &&
            (ctx->force || (node->name_depends_on & ctx->changed)))
        {
            char *name = flecs_script_name_expr_dup(v, &node->name_expr);
            if (!name) {
                return -1;
            }

            const char *cur = ecs_get_name(world, e);
            if (!cur || ecs_os_strcmp(cur, name)) {
                ecs_set_name(world, e, name);
            }

            ecs_os_free(name);
        }

        if (ctx->with_changed || ctx->force) {
            flecs_script_template_apply_with(v, e);
        }

        return 0;
    }

    if (node->name_expr) {
        node->eval = flecs_script_eval_name_expr(
            v, node, &node->name_expr, false);
    } else {
        node->eval = flecs_script_create_entity(v, node->name);
    }

    if (!node->eval) {
        return -1;
    }

    slot_ptr[0] = (int64_t)node->eval;
    reused[0] = false;

    if (ctx->created && !ctx->create) {
        ecs_vec_append_t(NULL, ctx->created, ecs_entity_t)[0] = node->eval;
    }

    return 0;
}

void flecs_script_template_remove_ids(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    ecs_entity_t src)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_world_t *world = v->world;
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        switch(stmt->kind) {
        case EcsAstTag:
        case EcsAstComponent: {
            ecs_script_id_t *id = &((ecs_script_tag_t*)stmt)->id;
            if (!src || src == EcsVariable) {
                break;
            }

            if (id->id_slot) {
                ecs_id_t prev = ecs_vec_get_t(&ctx->state->dynamic_ids,
                    ecs_id_t, id->id_slot - 1)[0];
                if (prev && ecs_id_is_valid(world, prev)) {
                    ecs_remove_id(world, src, prev);
                }
            } else if (id->eval && !id->dynamic) {
                ecs_remove_id(world, src, id->eval);
            } else if (id->dynamic && id->target_slot && id->eval) {
                ecs_entity_t tgt = flecs_script_template_slot_entity(
                    v, id->target_slot);
                ecs_entity_t rel = ecs_pair_first(world, id->eval);
                if (tgt && rel) {
                    ecs_remove_id(world, src, id->flag | ecs_pair(rel, tgt));
                }
            }
            break;
        }
        case EcsAstVarComponent: {
            ecs_script_var_component_t *node =
                (ecs_script_var_component_t*)stmt;
            if (!src || src == EcsVariable) {
                break;
            }

            ecs_script_var_t *var = flecs_script_find_var(v->vars, node->name,
                v->dynamic_variable_binding ? NULL : &node->sp);
            if (var && var->value.type) {
                ecs_remove_id(world, src, var->value.type);
            }
            break;
        }
        case EcsAstDefaultComponent: {
            if (!src || src == EcsVariable) {
                break;
            }

            ecs_id_t default_type = scope->parent ?
                scope->parent->default_component_eval : 0;
            if (default_type && ecs_id_is_valid(world, default_type)) {
                ecs_remove_id(world, src, default_type);
            }
            break;
        }
        case EcsAstIf:
            flecs_script_template_remove_ids(
                v, ((ecs_script_if_t*)stmt)->if_true, src);
            flecs_script_template_remove_ids(
                v, ((ecs_script_if_t*)stmt)->if_false, src);
            break;
        case EcsAstFor:
            flecs_script_template_remove_ids(
                v, ((ecs_script_for_range_t*)stmt)->scope, src);
            break;
        case EcsAstWith:
            flecs_script_template_remove_ids(
                v, ((ecs_script_with_t*)stmt)->scope, src);
            break;
        case EcsAstScope:
        case EcsAstEntity:
        case EcsAstWithVar:
        case EcsAstWithTag:
        case EcsAstWithComponent:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstProp:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstPairScope:
        case EcsAstInclude:
        case EcsAstFunction:
            break;
        }
    }
}

void flecs_script_template_reset_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    bool remove_components)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_script_template_instance_t *state = ctx->state;
    ecs_world_t *world = v->world;
    int32_t i;

    if (remove_components) {
        ecs_entity_t src = 0;
        if (v->entity) {
            src = v->entity->eval;
        }
        flecs_script_template_remove_ids(v, scope, src);
    }

    for (i = scope->slot_start; i < scope->slot_end; i ++) {
        int64_t *e = ecs_vec_get_t(&state->entities, int64_t, i);
        if (e[0] > 0) {
            ecs_delete(world, (ecs_entity_t)e[0]);
            e[0] = 0;
        }
    }

    for (i = scope->dyn_start; i < scope->dyn_end; i ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, i);
        int32_t k, count = ecs_vec_count(dvec);
        for (k = 0; k < count; k ++) {
            int64_t e = ecs_vec_get_t(dvec, int64_t, k)[0];
            if (e > 0) {
                ecs_delete(world, (ecs_entity_t)e);
            }
        }
        ecs_vec_clear(dvec);
    }

    for (i = scope->control_start; i < scope->control_end; i ++) {
        ecs_vec_get_t(&state->control, int8_t, i)[0] = -1;
    }
}

void flecs_script_template_consume_scope(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_script_template_instance_t *state = ctx->state;
    ecs_world_t *world = v->world;
    int32_t d;

    for (d = scope->dyn_start; d < scope->dyn_end; d ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, d);
        int32_t k = ctx->cursors[d] ++;
        if (k < ecs_vec_count(dvec)) {
            int64_t *e = ecs_vec_get_t(dvec, int64_t, k);
            if (e[0] > 0) {
                ecs_delete(world, (ecs_entity_t)e[0]);
                e[0] = 0;
            }
        } else {
            ecs_vec_append_t(NULL, dvec, int64_t)[0] = 0;
        }
    }
}

void flecs_script_template_sync_id(
    ecs_script_eval_visitor_t *v,
    ecs_entity_t src,
    ecs_script_id_t *id)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_id_t *slot = ecs_vec_get_t(
        &ctx->state->dynamic_ids, ecs_id_t, id->id_slot - 1);
    ecs_id_t prev = ctx->prev_ids ? ctx->prev_ids[id->id_slot - 1] : 0;
    slot[0] = id->eval;

    if (!prev || prev == id->eval || ctx->create) {
        return;
    }

    if (!src || src == EcsVariable) {
        return;
    }

    if (!ecs_id_is_valid(v->world, prev)) {
        return;
    }

    ecs_remove_id(v->world, src, prev);
}

void flecs_script_template_sync_scope_id(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope,
    int32_t id_slot,
    ecs_id_t new_id)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_script_template_instance_t *state = ctx->state;
    ecs_world_t *world = v->world;
    int32_t i;

    ecs_id_t *slot = ecs_vec_get_t(
        &state->dynamic_ids, ecs_id_t, id_slot - 1);
    ecs_id_t prev = ctx->prev_ids ? ctx->prev_ids[id_slot - 1] : 0;
    slot[0] = new_id;

    if (!prev || prev == new_id || ctx->create) {
        return;
    }

    if (!ecs_id_is_valid(world, prev)) {
        return;
    }

    for (i = scope->slot_start; i < scope->slot_end; i ++) {
        int64_t e = ecs_vec_get_t(&state->entities, int64_t, i)[0];
        if (e > 0) {
            ecs_remove_id(world, (ecs_entity_t)e, prev);
        }
    }

    for (i = scope->dyn_start; i < scope->dyn_end; i ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, i);
        int32_t k, count = ecs_vec_count(dvec);
        for (k = 0; k < count; k ++) {
            int64_t e = ecs_vec_get_t(dvec, int64_t, k)[0];
            if (e > 0) {
                ecs_remove_id(world, (ecs_entity_t)e, prev);
            }
        }
    }
}

void flecs_script_template_sync_with_ids(
    ecs_script_eval_visitor_t *v,
    ecs_script_with_t *node)
{
    int32_t i, count = ecs_vec_count(&node->expressions->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&node->expressions->stmts);

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        if (stmt->kind != EcsAstWithTag && stmt->kind != EcsAstWithComponent) {
            continue;
        }

        ecs_script_id_t *id = &((ecs_script_tag_t*)stmt)->id;
        if (!id->id_slot || !id->eval) {
            continue;
        }

        flecs_script_template_sync_scope_id(
            v, node->scope, id->id_slot, id->eval);
    }
}

void flecs_script_template_loop_begin(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    int32_t d;
    for (d = scope->dyn_start; d < scope->dyn_end; d ++) {
        ctx->cursors[d] = 0;
    }
}

void flecs_script_template_loop_end(
    ecs_script_eval_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_template_eval_ctx_t *ctx = v->tmpl_ctx;
    ecs_script_template_instance_t *state = ctx->state;
    ecs_world_t *world = v->world;
    int32_t d;

    for (d = scope->dyn_start; d < scope->dyn_end; d ++) {
        ecs_vec_t *dvec = ecs_vec_get_t(
            &state->dynamic_entities, ecs_vec_t, d);
        int32_t cur = ctx->cursors[d];
        int32_t k, r, count = ecs_vec_count(dvec);
        for (k = cur; k < count; k ++) {
            int64_t e = ecs_vec_get_t(dvec, int64_t, k)[0];
            if (e > 0) {
                bool keep = false;
                for (r = 0; r < cur; r ++) {
                    if (ecs_vec_get_t(dvec, int64_t, r)[0] == e) {
                        keep = true;
                        break;
                    }
                }
                if (!keep) {
                    ecs_delete(world, (ecs_entity_t)e);
                }
            }
        }

        if (cur < count) {
            ecs_vec_set_count_t(NULL, dvec, int64_t, cur);
        }
    }
}

static
void flecs_script_template_order_append(
    ecs_world_t *world,
    ecs_vec_t *desired,
    ecs_entity_t e,
    ecs_entity_t parent)
{
    if (ecs_get_parent(world, e) != parent) {
        return;
    }

    int32_t i, count = ecs_vec_count(desired);
    ecs_entity_t *ids = ecs_vec_first(desired);
    for (i = 0; i < count; i ++) {
        if (ids[i] == e) {
            return;
        }
    }

    ecs_vec_append_t(NULL, desired, ecs_entity_t)[0] = e;
}

static
void flecs_script_template_reorder_parent(
    ecs_world_t *world,
    ecs_script_template_instance_t *state,
    ecs_entity_t parent)
{
    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_childof(parent));
    if (!cr || !(cr->flags & EcsIdOrderedChildren)) {
        return;
    }

    ecs_entities_t cur = ecs_get_ordered_children(world, parent);
    if (cur.count <= 1) {
        return;
    }

    ecs_vec_t desired;
    ecs_vec_init_t(NULL, &desired, ecs_entity_t, 0);

    /* Slots are assigned in template declaration order */
    int32_t i, count = ecs_vec_count(&state->entities);
    int64_t *slots = ecs_vec_first(&state->entities);
    for (i = 0; i < count; i ++) {
        int64_t s = slots[i];
        if (s > 0) {
            flecs_script_template_order_append(
                world, &desired, (ecs_entity_t)s, parent);
        } else if (s < 0) {
            ecs_vec_t *dvec = ecs_vec_get_t(
                &state->dynamic_entities, ecs_vec_t, (int32_t)(-s) - 1);
            int32_t k, dcount = ecs_vec_count(dvec);
            for (k = 0; k < dcount; k ++) {
                int64_t de = ecs_vec_get_t(dvec, int64_t, k)[0];
                if (de > 0) {
                    flecs_script_template_order_append(
                        world, &desired, (ecs_entity_t)de, parent);
                }
            }
        }
    }

    int32_t j = 0, dcount = ecs_vec_count(&desired);
    if (dcount > 1) {
        ecs_entity_t *dids = ecs_vec_first(&desired);
        ecs_entity_t *new_order = ecs_os_memdup_n(
            cur.ids, ecs_entity_t, cur.count);

        /* Replace the positions occupied by entities of this template with
         * the same entities in declaration order. Children not created by
         * this template keep their current position. */
        for (i = 0; i < cur.count && j < dcount; i ++) {
            int32_t k;
            for (k = 0; k < dcount; k ++) {
                if (dids[k] == new_order[i]) {
                    break;
                }
            }
            if (k != dcount) {
                new_order[i] = dids[j ++];
            }
        }

        ecs_set_child_order(world, parent, new_order, cur.count);
        ecs_os_free(new_order);
    }

    ecs_vec_fini_t(NULL, &desired, ecs_entity_t);
}

static
void flecs_script_template_restore_child_order(
    ecs_world_t *world,
    ecs_script_template_instance_t *state,
    ecs_vec_t *created)
{
    int32_t i, count = ecs_vec_count(created);
    ecs_entity_t *ids = ecs_vec_first(created);
    for (i = 0; i < count; i ++) {
        ecs_entity_t parent = ecs_get_parent(world, ids[i]);
        if (!parent) {
            continue;
        }

        int32_t j;
        for (j = 0; j < i; j ++) {
            if (ecs_get_parent(world, ids[j]) == parent) {
                break;
            }
        }
        if (j != i) {
            continue;
        }

        flecs_script_template_reorder_parent(world, state, parent);
    }
}

typedef struct ecs_script_template_dep_var_t {
    const char *name;
    uint64_t mask;
} ecs_script_template_dep_var_t;

typedef struct ecs_script_template_analyzer_t {
    ecs_script_template_t *template_;
    ecs_vec_t vars;
    int32_t prop_count;
    bool in_loop;
    ecs_script_for_range_t *cur_loop;
} ecs_script_template_analyzer_t;

static
uint64_t flecs_script_template_analyze_scope(
    ecs_script_template_analyzer_t *an,
    ecs_script_scope_t *scope);

static
uint64_t flecs_script_template_var_mask(
    ecs_script_template_analyzer_t *an,
    const char *name)
{
    if (!name) {
        return 0;
    }

    int32_t i, count = ecs_vec_count(&an->vars);
    ecs_script_template_dep_var_t *vars = ecs_vec_first(&an->vars);
    for (i = count - 1; i >= 0; i --) {
        if (vars[i].name && !ecs_os_strcmp(vars[i].name, name)) {
            return vars[i].mask;
        }
    }

    return 0;
}

static
void flecs_script_template_var_push(
    ecs_script_template_analyzer_t *an,
    const char *name,
    uint64_t mask)
{
    ecs_script_template_dep_var_t *var = ecs_vec_append_t(
        NULL, &an->vars, ecs_script_template_dep_var_t);
    var->name = name;
    var->mask = mask;
}

static
bool flecs_script_template_is_ident_char(
    char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || (ch == '_');
}

static
uint64_t flecs_script_template_str_deps(
    ecs_script_template_analyzer_t *an,
    const char *str)
{
    if (!str) {
        return UINT64_MAX;
    }

    uint64_t mask = 0;
    const char *p = str;
    char token[ECS_MAX_TOKEN_SIZE];

    for (; *p; p ++) {
        if (*p == '\\') {
            p ++;
            if (!*p) {
                break;
            }
            continue;
        }

        if (*p == '{') {
            return UINT64_MAX;
        }

        if (*p == '$') {
            int32_t len = 0;
            p ++;
            while (flecs_script_template_is_ident_char(*p) &&
                (len < (ECS_MAX_TOKEN_SIZE - 1)))
            {
                token[len ++] = *p;
                p ++;
            }
            token[len] = '\0';
            p --;

            if (len == (ECS_MAX_TOKEN_SIZE - 1)) {
                return UINT64_MAX;
            }

            mask |= flecs_script_template_var_mask(an, token);
        }
    }

    return mask;
}

static
uint64_t flecs_script_template_expr_deps(
    ecs_script_template_analyzer_t *an,
    ecs_expr_node_t *node)
{
    uint64_t mask = 0;
    int32_t i, count;

    if (!node) {
        return 0;
    }

    switch(node->kind) {
    case EcsExprValue:
    case EcsExprEmptyInitializer:
        break;
    case EcsExprInterpolatedString: {
        ecs_expr_interpolated_string_t *expr =
            (ecs_expr_interpolated_string_t*)node;
        count = ecs_vec_count(&expr->expressions);
        if (!count) {
            mask = flecs_script_template_str_deps(an, expr->value);
        } else {
            ecs_expr_node_t **exprs = ecs_vec_first(&expr->expressions);
            for (i = 0; i < count; i ++) {
                mask |= flecs_script_template_expr_deps(an, exprs[i]);
            }
        }
        break;
    }
    case EcsExprInitializer: {
        ecs_expr_initializer_t *expr = (ecs_expr_initializer_t*)node;
        count = ecs_vec_count(&expr->elements);
        ecs_expr_initializer_element_t *elems = ecs_vec_first(&expr->elements);
        for (i = 0; i < count; i ++) {
            mask |= flecs_script_template_expr_deps(an, elems[i].value);
        }
        break;
    }
    case EcsExprUnary:
        mask = flecs_script_template_expr_deps(
            an, ((ecs_expr_unary_t*)node)->expr);
        break;
    case EcsExprBinary: {
        ecs_expr_binary_t *expr = (ecs_expr_binary_t*)node;
        mask = flecs_script_template_expr_deps(an, expr->left);
        mask |= flecs_script_template_expr_deps(an, expr->right);
        break;
    }
    case EcsExprIdentifier:
        mask = flecs_script_template_expr_deps(
            an, ((ecs_expr_identifier_t*)node)->expr);
        break;
    case EcsExprVariable: {
        ecs_expr_variable_t *expr = (ecs_expr_variable_t*)node;
        mask = flecs_script_template_var_mask(an, expr->name);
        break;
    }
    case EcsExprGlobalVariable:
        break;
    case EcsExprFunction:
    case EcsExprMethod: {
        ecs_expr_function_t *expr = (ecs_expr_function_t*)node;
        mask = flecs_script_template_expr_deps(an, expr->left);
        mask |= flecs_script_template_expr_deps(
            an, (ecs_expr_node_t*)expr->args);
        break;
    }
    case EcsExprMember:
        mask = flecs_script_template_expr_deps(
            an, ((ecs_expr_member_t*)node)->left);
        break;
    case EcsExprElement: {
        ecs_expr_element_t *expr = (ecs_expr_element_t*)node;
        mask = flecs_script_template_expr_deps(an, expr->left);
        mask |= flecs_script_template_expr_deps(an, expr->index);
        break;
    }
    case EcsExprComponent:
        mask = flecs_script_template_expr_deps(
            an, ((ecs_expr_component_t*)node)->expr);
        break;
    case EcsExprCast:
    case EcsExprCastNumber:
        mask = flecs_script_template_expr_deps(
            an, ((ecs_expr_cast_t*)node)->expr);
        break;
    case EcsExprMatch: {
        ecs_expr_match_t *expr = (ecs_expr_match_t*)node;
        mask = flecs_script_template_expr_deps(an, expr->expr);
        count = ecs_vec_count(&expr->elements);
        ecs_expr_match_element_t *elems = ecs_vec_first(&expr->elements);
        for (i = 0; i < count; i ++) {
            mask |= flecs_script_template_expr_deps(an, elems[i].compare);
            mask |= flecs_script_template_expr_deps(an, elems[i].expr);
        }
        mask |= flecs_script_template_expr_deps(an, expr->any.compare);
        mask |= flecs_script_template_expr_deps(an, expr->any.expr);
        break;
    }
    case EcsExprNew: {
        ecs_expr_new_t *expr = (ecs_expr_new_t*)node;
        ecs_script_entity_t *entity = expr->entity;
        uint64_t name_mask = 0;
        if (entity->name_expr) {
            name_mask = flecs_script_template_expr_deps(
                an, entity->name_expr);
        } else if (entity->name_is_var) {
            name_mask = flecs_script_template_var_mask(an, entity->name);
        }
        entity->name_depends_on = name_mask;

        /* Entities created by expressions have no slots; instances must be
         * fully rebuilt when properties change. */
        an->template_->force_full = true;

        mask = name_mask;
        mask |= flecs_script_template_analyze_scope(an, entity->scope);
        entity->node.depends_on = mask;
        break;
    }
    }

    return mask;
}

static
int32_t flecs_script_template_find_root_slot(
    ecs_script_template_t *template,
    const char *name)
{
    ecs_script_scope_t *scope = template->node->scope;
    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        if (stmts[i]->kind == EcsAstEntity) {
            ecs_script_entity_t *node = (ecs_script_entity_t*)stmts[i];
            if (node->name && !ecs_os_strcmp(node->name, name)) {
                return node->slot;
            }
        }
    }

    return 0;
}

static
uint64_t flecs_script_template_id_deps(
    ecs_script_template_analyzer_t *an,
    ecs_script_id_t *id)
{
    uint64_t mask = 0;

    if (id->first_expr) {
        mask |= flecs_script_template_expr_deps(an, id->first_expr);
    } else if (id->first && id->first[0] == '$') {
        mask |= flecs_script_template_var_mask(an, &id->first[1]);
    } else if (id->first && strchr(id->first, '$')) {
        mask |= flecs_script_template_str_deps(an, id->first);
    }

    if (id->second_expr) {
        mask |= flecs_script_template_expr_deps(an, id->second_expr);
    } else if (id->second && id->second[0] == '$') {
        mask |= flecs_script_template_var_mask(an, &id->second[1]);
    } else if (id->second && strchr(id->second, '$')) {
        mask |= flecs_script_template_str_deps(an, id->second);
    }

    if (id->second && id->second[0] != '$' && !id->second_expr) {
        id->target_slot = flecs_script_template_find_root_slot(
            an->template_, id->second);
    }

    if (an->in_loop) {
        /* Ids with per-iteration values inside loops can't be tracked per
         * slot. For pairs with a dynamic element that are added to a
         * persistent entity, the matching wildcard is cleared before the loop
         * is re-evaluated. Other dynamic ids fall back to a full rebuild. */
        bool first_dyn = (id->first_expr != NULL) ||
            (id->first && strchr(id->first, '$'));
        bool second_dyn = (id->second_expr != NULL) ||
            (id->second && strchr(id->second, '$') &&
                ecs_os_strcmp(id->second, "$this"));
        if (first_dyn || second_dyn) {
            if (id->second && an->cur_loop) {
                an->cur_loop->has_clear_ids = true;
                an->cur_loop->expr_depends_on |= mask;
            } else {
                an->template_->force_full = true;
            }
        }
        id->id_slot = 0;
    } else if (mask) {
        id->id_slot = ++ an->template_->dyn_id_count;
    } else {
        id->id_slot = 0;
    }

    return mask;
}

static
uint64_t flecs_script_template_analyze_scope(
    ecs_script_template_analyzer_t *an,
    ecs_script_scope_t *scope)
{
    uint64_t scope_mask = 0;
    int32_t var_start = ecs_vec_count(&an->vars);

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        uint64_t mask = 0;

        switch(stmt->kind) {
        case EcsAstProp: {
            ecs_script_var_node_t *node = (ecs_script_var_node_t*)stmt;
            ecs_assert(an->prop_count < 64, ECS_INTERNAL_ERROR, NULL);
            flecs_script_template_var_push(
                an, node->name, 1llu << an->prop_count);
            an->prop_count ++;
            break;
        }
        case EcsAstConst:
        case EcsAstExportConst: {
            ecs_script_var_node_t *node = (ecs_script_var_node_t*)stmt;
            mask = flecs_script_template_expr_deps(an, node->expr);
            flecs_script_template_var_push(an, node->name, mask);
            break;
        }
        case EcsAstTag:
        case EcsAstWithTag: {
            ecs_script_tag_t *node = (ecs_script_tag_t*)stmt;
            mask = flecs_script_template_id_deps(an, &node->id);
            break;
        }
        case EcsAstComponent:
        case EcsAstWithComponent: {
            ecs_script_component_t *node = (ecs_script_component_t*)stmt;
            mask = flecs_script_template_id_deps(an, &node->id);
            mask |= flecs_script_template_expr_deps(an, node->expr);
            break;
        }
        case EcsAstVarComponent:
        case EcsAstWithVar: {
            ecs_script_var_component_t *node =
                (ecs_script_var_component_t*)stmt;
            mask = flecs_script_template_var_mask(an, node->name);
            break;
        }
        case EcsAstDefaultComponent: {
            ecs_script_default_component_t *node =
                (ecs_script_default_component_t*)stmt;
            mask = flecs_script_template_expr_deps(an, node->expr);
            break;
        }
        case EcsAstEntity: {
            ecs_script_entity_t *node = (ecs_script_entity_t*)stmt;
            uint64_t name_mask = 0;
            if (node->name_expr) {
                name_mask = flecs_script_template_expr_deps(
                    an, node->name_expr);
            } else if (node->name_is_var) {
                name_mask = flecs_script_template_var_mask(an, node->name);
            }
            node->name_depends_on = name_mask;

            uint64_t kind_mask = 0;
            if (node->kind && node->kind[0] == '$') {
                kind_mask = flecs_script_template_var_mask(
                    an, &node->kind[1]);
            }

            mask = name_mask | kind_mask;
            ecs_script_for_range_t *old_loop = an->cur_loop;
            an->cur_loop = NULL;
            mask |= flecs_script_template_analyze_scope(an, node->scope);
            an->cur_loop = old_loop;
            break;
        }
        case EcsAstIf: {
            ecs_script_if_t *node = (ecs_script_if_t*)stmt;
            uint64_t expr_mask = flecs_script_template_expr_deps(
                an, node->expr);
            node->expr_depends_on = expr_mask;
            mask = expr_mask;
            mask |= flecs_script_template_analyze_scope(an, node->if_true);
            mask |= flecs_script_template_analyze_scope(an, node->if_false);
            break;
        }
        case EcsAstFor: {
            ecs_script_for_range_t *node = (ecs_script_for_range_t*)stmt;
            uint64_t expr_mask = flecs_script_template_expr_deps(
                an, node->from);
            expr_mask |= flecs_script_template_expr_deps(an, node->to);
            node->expr_depends_on = expr_mask;

            int32_t loop_var_start = ecs_vec_count(&an->vars);
            flecs_script_template_var_push(an, node->loop_var, expr_mask);
            mask = expr_mask;
            bool old_in_loop = an->in_loop;
            ecs_script_for_range_t *old_loop = an->cur_loop;
            an->in_loop = true;
            if (!an->cur_loop) {
                an->cur_loop = node;
            }
            mask |= flecs_script_template_analyze_scope(an, node->scope);
            an->in_loop = old_in_loop;
            an->cur_loop = old_loop;
            ecs_vec_set_count_t(NULL, &an->vars,
                ecs_script_template_dep_var_t, loop_var_start);
            break;
        }
        case EcsAstWith: {
            ecs_script_with_t *node = (ecs_script_with_t*)stmt;
            /* With-clause ids apply to entities in the with scope, not to the
             * loop owner, so they can't use the wildcard-clear path. */
            ecs_script_for_range_t *old_loop = an->cur_loop;
            an->cur_loop = NULL;
            uint64_t expr_mask = flecs_script_template_analyze_scope(
                an, node->expressions);
            node->expr_depends_on = expr_mask;
            mask = expr_mask;
            mask |= flecs_script_template_analyze_scope(an, node->scope);
            an->cur_loop = old_loop;
            break;
        }
        case EcsAstPairScope: {
            ecs_script_pair_scope_t *node = (ecs_script_pair_scope_t*)stmt;
            uint64_t expr_mask = flecs_script_template_id_deps(
                an, &node->id);
            node->expr_depends_on = expr_mask;
            mask = expr_mask;
            mask |= flecs_script_template_analyze_scope(an, node->scope);
            break;
        }
        case EcsAstScope:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstInclude:
        case EcsAstFunction:
            break;
        }

        stmt->depends_on = mask;
        scope_mask |= mask;
    }

    ecs_vec_set_count_t(NULL, &an->vars,
        ecs_script_template_dep_var_t, var_start);

    scope->node.depends_on = scope_mask;

    return scope_mask;
}

static
void flecs_script_template_analyze(
    ecs_script_template_t *template)
{
    ecs_script_template_analyzer_t an = { .template_ = template };
    ecs_vec_init_t(NULL, &an.vars, ecs_script_template_dep_var_t, 0);
    flecs_script_template_analyze_scope(&an, template->node->scope);
    ecs_vec_fini_t(NULL, &an.vars, ecs_script_template_dep_var_t);
}

static
bool flecs_script_template_assign_slots(
    ecs_script_template_t *template,
    ecs_script_scope_t *scope,
    bool in_loop)
{
    bool contains = false;

    scope->slot_start = template->slot_count;
    scope->dyn_start = template->dyn_slot_count;
    scope->control_start = template->control_count;

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **stmts = ecs_vec_first(&scope->stmts);

    for (i = 0; i < count; i ++) {
        ecs_script_node_t *stmt = stmts[i];
        bool c = false;

        switch(stmt->kind) {
        case EcsAstEntity: {
            ecs_script_entity_t *node = (ecs_script_entity_t*)stmt;
            node->slot = ++ template->slot_count;
            if (in_loop) {
                node->dyn_slot = ++ template->dyn_slot_count;
            } else {
                node->dyn_slot = 0;
            }
            flecs_script_template_assign_slots(
                template, node->scope, in_loop);
            c = true;
            break;
        }
        case EcsAstIf: {
            ecs_script_if_t *node = (ecs_script_if_t*)stmt;
            if (!in_loop) {
                node->control_slot = ++ template->control_count;
            } else {
                node->control_slot = 0;
            }
            c = flecs_script_template_assign_slots(
                template, node->if_true, in_loop);
            c |= flecs_script_template_assign_slots(
                template, node->if_false, in_loop);
            break;
        }
        case EcsAstFor: {
            ecs_script_for_range_t *node = (ecs_script_for_range_t*)stmt;
            c = flecs_script_template_assign_slots(
                template, node->scope, true);
            break;
        }
        case EcsAstWith: {
            ecs_script_with_t *node = (ecs_script_with_t*)stmt;
            c = flecs_script_template_assign_slots(
                template, node->expressions, in_loop);
            c |= flecs_script_template_assign_slots(
                template, node->scope, in_loop);
            break;
        }
        case EcsAstPairScope: {
            ecs_script_pair_scope_t *node = (ecs_script_pair_scope_t*)stmt;
            c = flecs_script_template_assign_slots(
                template, node->scope, in_loop);
            break;
        }
        case EcsAstScope:
        case EcsAstTag:
        case EcsAstWithTag:
        case EcsAstComponent:
        case EcsAstWithComponent:
        case EcsAstDefaultComponent:
        case EcsAstConst:
        case EcsAstExportConst:
        case EcsAstVarComponent:
        case EcsAstWithVar:
        case EcsAstUsing:
        case EcsAstModule:
        case EcsAstAnnotation:
        case EcsAstTemplate:
        case EcsAstProp:
        case EcsAstInclude:
        case EcsAstFunction:
            break;
        }

        stmt->contains_entities = c;
        contains |= c;
    }

    scope->slot_end = template->slot_count;
    scope->dyn_end = template->dyn_slot_count;
    scope->control_end = template->control_count;
    scope->node.contains_entities = contains;

    return contains;
}

/* Template ctor to initialize with default property values */
static
void flecs_script_template_ctor(
    void *ptr,
    int32_t count,
    const ecs_type_info_t *ti)
{
    ecs_world_t *world = ti->hooks.ctx;
    ecs_entity_t template_entity = ti->component;

    /* Zero-initialize memory so that hooks can safely run destructors. */
    flecs_default_ctor(ptr, count, ti);

    const EcsStruct *st = ecs_get(world, template_entity, EcsStruct);
    if (!st) {
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
    int32_t count,
    bool allow_stale_entities)
{
    (void)allow_stale_entities;

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

    int32_t *cursors = NULL;
    ecs_id_t *prev_ids = NULL;
    ecs_vec_t created;
    ecs_vec_init_t(NULL, &created, ecs_entity_t, 0);

    int32_t i, m, a;
    for (i = 0; i < count; i ++) {
        v.parent = entities[i];
        if (!ecs_is_alive(world, v.parent)) {
            ecs_assert(allow_stale_entities, ECS_INTERNAL_ERROR, NULL);
            data = ECS_OFFSET(data, ti->size);
            continue;
        }

        ecs_entity_t instance = entities[i];
        instance_node.eval = instance;

        uint64_t changed = UINT64_MAX;
        bool full = true;

        ecs_script_template_instance_t *state =
            flecs_script_template_instance_ensure(world, instance, template);
        if (state->prev_values) {
            changed = flecs_script_template_diff(world, state, data);
            if (state->dirty) {
                changed = UINT64_MAX;
            }
            full = false;
        }

        if (!full && !changed) {
            data = ECS_OFFSET(data, ti->size);
            continue;
        }

        if (!full && template->force_full) {
            /* Template contains constructs that can't be updated
             * incrementally; rebuild the instance. */
            full = true;
            changed = UINT64_MAX;
        }

        /* Apply annotations, if any */
        bool annot_failed = false;
        for (a = 0; a < ecs_vec_count(&template->annot); a ++) {
            ecs_script_annot_t *annot = ecs_vec_get_t(
                &template->annot, ecs_script_annot_t*, a)[0];
            if (flecs_script_apply_annot(&v, &instance_node, annot)) {
                annot_failed = true;
                break;
            }
        }
        if (annot_failed) {
            break;
        }

        /* Create variables to hold template properties */
        ecs_script_vars_t *vars = flecs_script_vars_push(
            NULL, &v.r->stack, &v.r->allocator);
        vars->parent = template->vars; /* Include hoisted variables */
        vars->sp = ecs_vec_count(&template->vars->vars);

        /* Allocate enough space for variables */
        ecs_script_vars_set_size(vars, (st ? st->members.count : 0) + 1);

        /* Populate $this variable with instance entity */
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
                 * set the name, as variables will be resolved by frame offset. */
                ecs_script_var_t *var = ecs_script_vars_declare(
                    vars, NULL /* member->name */);
                var->value.type = member->type;
                var->value.ptr = ECS_OFFSET(data, member->offset);
            }
        }

        if (full) {
            ecs_script_clear(world, template_entity, instance);
            flecs_script_template_instance_prepare(
                world, template, st, state);
        }

        if (template->dyn_slot_count && !cursors) {
            cursors = ecs_os_calloc_n(int32_t, template->dyn_slot_count);
        }

        if (template->dyn_id_count && !prev_ids) {
            prev_ids = ecs_os_calloc_n(ecs_id_t, template->dyn_id_count);
        }

        if (prev_ids) {
            ecs_os_memcpy_n(prev_ids,
                ecs_vec_first(&state->dynamic_ids), ecs_id_t,
                template->dyn_id_count);
        }

        ecs_vec_clear(&created);

        ecs_script_template_eval_ctx_t ctx = {
            .template_ = template,
            .state = state,
            .changed = changed,
            .create = full,
            .cursors = cursors,
            .prev_ids = prev_ids,
            .created = full ? NULL : &created
        };

        v.tmpl_ctx = &ctx;

        /* Run template code */
        v.vars = vars;

        int result = flecs_script_eval_scope(&v, scope);

        v.tmpl_ctx = NULL;

        if (!result) {
            flecs_script_template_store_prev(world, ti, state, data);
            state->dirty = false;

            if (!full && ecs_vec_count(&created)) {
                flecs_script_template_restore_child_order(
                    world, state, &created);
            }
        } else {
            state->dirty = true;
            int32_t control_count = ecs_vec_count(&state->control);
            if (control_count) {
                ecs_os_memset(ecs_vec_first(&state->control), -1,
                    control_count);
            }
        }

        /* Pop variable scope */
        ecs_script_vars_pop(vars);

        data = ECS_OFFSET(data, ti->size);
    }

    ecs_vec_fini_t(NULL, &created, ecs_entity_t);
    ecs_os_free(cursors);
    ecs_os_free(prev_ids);

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
        world, evt->template_entity, evt->entities, evt->data, evt->count,
        true);

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

    ecs_entity_t *entities = ecs_os_memdup_n(
        it->entities, ecs_entity_t, it->count);
    void *data_copy = ecs_os_memdup(data, ti->size * it->count);

    flecs_script_template_instantiate(
        world, template_entity, entities, data_copy, it->count, false);

    ecs_os_free(data_copy);
    ecs_os_free(entities);
    return;
}

static
int flecs_script_template_eval_prop(
    ecs_script_visit_t *_v,
    ecs_script_var_node_t *node)
{
    ecs_script_eval_visitor_t *v = (ecs_script_eval_visitor_t*)_v;
    ecs_script_template_t *template = v->template;
    if (ecs_vec_count(&v->vars->vars) >
        ecs_vec_count(&template->prop_defaults))
    {
        flecs_script_eval_error(v, node,
            "const variables declared before prop '%s' (props must come first)",
                node->name);
        return -1;
    }

    if (ecs_vec_count(&template->prop_defaults) >= 64) {
        flecs_script_eval_error(v, node,
            "templates support at most 64 props (exceeded by prop '%s')",
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
        if (flecs_script_find_entity(v, 0, node->type, NULL, NULL,
            &type, NULL) || !type)
        {
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
        var->value.ptr = flecs_stack_calloc(
            &v->r->stack, ti->size, ti->alignment);
        var->type_info = ti;

        flecs_type_info_ctor(var->value.ptr, 1, ti);

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

        flecs_type_info_ctor(var->value.ptr, 1, ti);

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
    value->sp = ti->size;
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
    ecs_script_visit_t *v,
    ecs_script_node_t *node)
{
    if (node->kind == EcsAstTemplate) {
        flecs_script_eval_error((ecs_script_eval_visitor_t*)v, node,
            "nested templates are not allowed");
        return -1;
    } else if (node->kind == EcsAstProp) {
        return flecs_script_template_eval_prop(
            v, (ecs_script_var_node_t*)node);
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

    v->base.visit = flecs_script_template_eval;
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

static
ecs_script_template_t* flecs_script_template_init(
    ecs_script_impl_t *script)
{
    ecs_allocator_t *a = &script->allocator;
    ecs_script_template_t *result = flecs_alloc_t(a, ecs_script_template_t);
    ecs_os_memset_t(result, 0, ecs_script_template_t);
    ecs_vec_init_t(NULL, &result->prop_defaults, ecs_script_var_t, 0);
    ecs_vec_init_t(NULL, &result->using_, ecs_entity_t, 0);
    ecs_vec_init_t(NULL, &result->annot, ecs_script_annot_t*, 0);

    result->vars = ecs_script_vars_init(script->pub.world);
    return result;
}

void flecs_script_template_fini(
    ecs_script_impl_t *script,
    ecs_script_template_t *template)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &script->allocator;

    ecs_world_t *world = script->pub.world;
    int32_t i, count = ecs_vec_count(&template->prop_defaults);
    ecs_script_var_t *values = ecs_vec_first(&template->prop_defaults);
    for (i = 0; i < count; i ++) {
        ecs_script_var_t *value = &values[i];
        const ecs_type_info_t *ti = ecs_get_type_info(
            world, value->value.type);
        if (ti) {
            ecs_assert(ti->size == value->sp, ECS_INTERNAL_ERROR, NULL);
            if (ti->hooks.dtor) {
                flecs_type_info_dtor(value->value.ptr, 1, ti);
            }
            flecs_free(a, ti->size, value->value.ptr);
        } else {
            flecs_free(a, value->sp, value->value.ptr);
        }
    }

    ecs_vec_fini_t(a, &template->prop_defaults, ecs_script_var_t);

    ecs_vec_fini_t(a, &template->using_, ecs_entity_t);
    ecs_vec_fini_t(a, &template->annot, ecs_script_annot_t*);
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

    flecs_script_template_assign_slots(template, node->scope, false);

    /* Variables are always presented to a template in a well-defined order, so
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

    flecs_script_template_analyze(template);

    v->dynamic_variable_binding = old_dynamic_variable_binding;

    /* If template has no props, give template dummy size so we can register
     * hooks for it. */
    if (!ecs_has(v->world, template_entity, EcsComponent)) {
        ecs_set(v->world, template_entity, EcsComponent, {1, 1});
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
    v->dynamic_variable_binding = old_dynamic_variable_binding;
    flecs_script_template_fini(v->base.script, template);
    return -1;
}

void flecs_script_template_import(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsScriptTemplateSetEvent);
    ECS_TAG_DEFINE(world, EcsScriptTemplate);

    ecs_add_id(world, EcsScriptTemplate, EcsPairIsTag);

    ecs_set(world, EcsScriptTemplate, EcsComponent, {
        .size = ECS_SIZEOF(EcsScriptTemplateData),
        .alignment = ECS_ALIGNOF(EcsScriptTemplateData)
    });

    ecs_set_hooks_id(world, EcsScriptTemplate, &(ecs_type_hooks_t){
        .ctor = flecs_script_template_cache_ctor,
        .move = flecs_script_template_cache_move,
        .copy = flecs_script_template_cache_copy,
        .dtor = flecs_script_template_cache_dtor,
        .ctx = world
    });

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
