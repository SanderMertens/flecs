/**
 * @file addons/script/script.c
 * @brief Script API.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScript);
ECS_COMPONENT_DECLARE(EcsScriptVisitor);
ECS_COMPONENT_DECLARE(EcsScriptConstVar);
ECS_COMPONENT_DECLARE(EcsScriptMutVar);
ECS_COMPONENT_DECLARE(EcsScriptFunction);
ECS_COMPONENT_DECLARE(EcsScriptMethod);
ECS_DECLARE(EcsScriptVectorType);
ECS_DECLARE(EcsScriptError);

static ECS_MOVE(EcsScript, dst, src, {
    if (dst->script) {
        if (dst->template_ && (dst->template_ != src->template_)) {
            flecs_script_template_fini(
                flecs_script_impl(dst->script), dst->template_);
        }
        ecs_script_free(dst->script);
    }

    if (dst->filename != src->filename) {
        ecs_os_free(dst->filename);
    }
    if (dst->code != src->code) {
        ecs_os_free(dst->code);
    }
    if (dst->error != src->error) {
        ecs_os_free(dst->error);
    }
    flecs_script_ref_observers_fini(&dst->observers);
    flecs_script_ref_observers_fini(&dst->dyn_observers);

    dst->filename = src->filename;
    dst->code = src->code;
    dst->error = src->error;
    dst->script = src->script;
    dst->template_ = src->template_;
    dst->observers = src->observers;
    dst->dyn_observers = src->dyn_observers;

    src->filename = NULL;
    src->code = NULL;
    src->error = NULL;
    src->script = NULL;
    src->template_ = NULL;
    ecs_os_zeromem(&src->observers);
    ecs_os_zeromem(&src->dyn_observers);
})

static ECS_DTOR(EcsScript, ptr, {
    if (ptr->template_) {
        flecs_script_template_fini(
            flecs_script_impl(ptr->script), ptr->template_);
    }

    if (ptr->script) {
        ecs_script_free(ptr->script);
    }

    flecs_script_ref_observers_fini(&ptr->observers);
    flecs_script_ref_observers_fini(&ptr->dyn_observers);

    ecs_os_free(ptr->filename);
    ecs_os_free(ptr->code);
    ecs_os_free(ptr->error);
})

static ecs_id_t flecs_script_tag(
    ecs_entity_t script,
    ecs_entity_t instance)
{
    if (!instance) {
        return ecs_pair_t(EcsScript, script);
    } else {
        return ecs_pair(EcsChildOf, instance);
    }
}

ecs_script_t* flecs_script_new(
    ecs_world_t *world) 
{
    ecs_script_impl_t *result = ecs_os_calloc_t(ecs_script_impl_t);
    flecs_allocator_init(&result->allocator);
    ecs_parser_t parser = { .script = result };
    result->root = flecs_script_scope_new(&parser);
    result->pub.world = world;
    result->refcount = 1;
    ecs_vec_init_t(NULL, &result->refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->run_refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->symbol_slots, ecs_script_symbol_slot_t, 0);
    ecs_vec_init_t(NULL, &result->component_slots,
        ecs_script_component_slot_t, 0);
    ecs_vec_init_t(NULL, &result->scope_slots, int32_t, 0);
    ecs_vec_init_t(NULL, &result->for_slots, ecs_script_for_slot_t, 0);
    ecs_vec_init_t(NULL, &result->unresolved_refs,
        ecs_script_unresolved_ref_t, 0);
    ecs_vec_init_t(NULL, &result->unresolved_component_refs,
        ecs_script_unresolved_component_ref_t, 0);
    ecs_vec_init_t(NULL, &result->lenient_warned, char*, 0);
    result->lenient = ecs_script_get_lenient(world);
    return &result->pub;
}

void ecs_script_set_lenient(
    ecs_world_t *world,
    bool lenient)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_world_t *w = ECS_CONST_CAST(ecs_world_t*, ecs_get_world(world));
    if (lenient) {
        w->flags |= EcsWorldScriptLenient;
    } else {
        w->flags &= ~(ecs_flags32_t)EcsWorldScriptLenient;
    }
error:
    return;
}

bool ecs_script_get_lenient(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_world_t *w = ecs_get_world(world);
    return (w->flags & EcsWorldScriptLenient) != 0;
error:
    return false;
}

void flecs_script_lenient_warn(
    ecs_script_t *script,
    const char *name,
    const char *msg)
{
    ecs_script_impl_t *impl = flecs_script_impl(script);
    char **names = ecs_vec_first(&impl->lenient_warned);
    int32_t i, count = ecs_vec_count(&impl->lenient_warned);
    for (i = 0; i < count; i ++) {
        if (!ecs_os_strcmp(names[i], name)) {
            return;
        }
    }

    ecs_vec_append_t(NULL, &impl->lenient_warned, char*)[0] =
        ecs_os_strdup(name);

    ecs_warn("%s: %s '%s'", impl->pub.name ? impl->pub.name : "script",
        msg, name);
}

void flecs_script_pos_to_line_col(
    const char *code,
    const char *pos,
    int32_t *line,
    int32_t *column)
{
    line[0] = 0;
    column[0] = 0;
    if (!code || !pos || (pos < code) || (pos > &code[ecs_os_strlen(code)])) {
        return;
    }

    const char *ptr, *line_start = code;
    line[0] = 1;
    for (ptr = code; ptr < pos; ptr ++) {
        if (ptr[0] == '\n') {
            line[0] ++;
            line_start = ptr + 1;
        }
    }
    column[0] = flecs_ito(int32_t, pos - line_start) + 1;
}

void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    if (!instance) {
        flecs_delete_with(world, ecs_pair_t(EcsScript, script), true);
    } else {
        ecs_assert(ecs_is_alive(world, instance), ECS_INTERNAL_ERROR, NULL);
        ecs_vec_t to_delete = {0};
        ecs_vec_init_t(&world->allocator, &to_delete, ecs_entity_t, 0);

        ecs_iter_t it = ecs_children(world, instance);
        while (ecs_children_next(&it)) {
            if (it.table) {
                if (ecs_table_has_id(world, it.table, ecs_pair(EcsScriptTemplate, script))) {
                    int32_t i, count = it.count;
                    for (i = 0; i < count; i ++) {
                        ecs_vec_append_t(
                            &world->allocator, &to_delete, ecs_entity_t)[0] = 
                                it.entities[i];
                    }
                }
            } else {
                int32_t i, count = it.count;
                for (i = 0; i < count; i ++) {
                    ecs_entity_t e = it.entities[i];
                    if (ecs_has_pair(world, e, EcsScriptTemplate, script)) {
                        ecs_vec_append_t(
                            &world->allocator, &to_delete, ecs_entity_t)[0] = e;
                    }
                }
            }
        }

        ecs_entity_t *to_delete_ids = ecs_vec_first(&to_delete);
        int32_t i, count = ecs_vec_count(&to_delete);
        for (i = 0; i < count; i ++) {
            ecs_delete(world, to_delete_ids[i]);
        }
        ecs_vec_fini_t(&world->allocator, &to_delete, ecs_entity_t);
    }
}

int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code,
    ecs_script_eval_result_t *result)
{
    ecs_script_t *script = ecs_script_parse(world, name, code, NULL, result);
    if (!script) {
        goto error;
    }

    if (ecs_script_eval(script, NULL, result)) {
        goto error_free;
    }

    ecs_script_free(script);
    return 0;
error_free:
    ecs_script_free(script);
error:
    return -1;
}

int ecs_script_run_file(
    ecs_world_t *world,
    const char *filename) 
{
    char *script = flecs_load_from_file(filename);
    if (!script) {
        return -1;
    }

    int result = ecs_script_run(world, filename, script, NULL);
    ecs_os_free(script);
    return result;
}

void ecs_script_free(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);
    ecs_check(impl->refcount > 0, ECS_INVALID_OPERATION, NULL);
    if (!--impl->refcount) {
        ecs_assert(impl->task_refcount == 0, ECS_INVALID_OPERATION,
            "script freed while tasks are still alive");
        flecs_script_visit_free(script);
        flecs_expr_visit_free(script, impl->expr);
        ecs_vec_fini_t(NULL, &impl->refs, ecs_script_ref_t);
        ecs_vec_fini_t(NULL, &impl->run_refs, ecs_script_ref_t);
        ecs_vec_fini_t(NULL, &impl->symbol_slots, ecs_script_symbol_slot_t);
        ecs_vec_fini_t(NULL, &impl->component_slots,
            ecs_script_component_slot_t);
        ecs_vec_fini_t(NULL, &impl->scope_slots, int32_t);
        flecs_script_for_slots_fini(&impl->for_slots);
        ecs_vec_fini_t(NULL, &impl->unresolved_refs,
            ecs_script_unresolved_ref_t);
        ecs_vec_fini_t(NULL, &impl->unresolved_component_refs,
            ecs_script_unresolved_component_ref_t);
        {
            char **warned = ecs_vec_first(&impl->lenient_warned);
            int32_t wi, wcount = ecs_vec_count(&impl->lenient_warned);
            for (wi = 0; wi < wcount; wi ++) {
                ecs_os_free(warned[wi]);
            }
            ecs_vec_fini_t(NULL, &impl->lenient_warned, char*);
        }
        flecs_free(&impl->allocator,
            impl->token_buffer_size, impl->token_buffer);
        flecs_allocator_fini(&impl->allocator);
        ecs_os_free(ECS_CONST_CAST(char*, impl->pub.name)); /* safe, owned value */
        ecs_os_free(ECS_CONST_CAST(char*, impl->pub.code)); /* safe, owned value */
        ecs_os_free(impl);
    }
error:
    return;
}

int flecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_script_runtime_t *eval_runtime)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(code != NULL, ECS_INTERNAL_ERROR, NULL);

    int result = 0;
    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    const char *name = ecs_get_name(world, e);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    if (s->template_) {
        char *template_name = ecs_get_path(world, s->template_->props.type);
        ecs_err("cannot update scripts for individual templates, "
            "update parent script instead (tried to update '%s')",
                template_name);
        ecs_os_free(s->error);
        s->error = flecs_asprintf(
            "cannot update scripts for individual templates, "
            "update parent script instead (tried to update '%s')",
                template_name);
        ecs_os_free(template_name);
        result = -1;
        goto done;
    }

    if (s->code) {
        ecs_os_free(s->code);
    }

    s->code = ecs_os_strdup(code);

    if (s->error) {
        ecs_os_free(s->error);
        s->error = NULL;
    }

    if (s->script) {
        ecs_script_free(s->script);
    }

    ecs_script_eval_result_t eval_result = {0};
    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    flecs_script_runtime_error_reset(runtime);

    ecs_script_eval_desc_t parse_desc = { .lenient = s->lenient };
    s->script = ecs_script_parse(world, name, code, &parse_desc, &eval_result);
    if (s->script != NULL) {
        flecs_script_impl(s->script)->entity = e;
    } else {
        s->error = eval_result.error;
        ecs_log_(-3, NULL, 0, "%s: %s", name ? name : "script", s->error);
        if (!instance) {
            flecs_script_ref_observers_clear(world, &s->observers);
            flecs_script_ref_observers_clear(world, &s->dyn_observers);
        }
        result = -1;
        goto done;
    }

    ecs_script_clear(world, e, instance);

#ifdef FLECS_DEBUG
    {
        ecs_iter_t it = ecs_each_pair_t(world, EcsScript, e);
        ecs_assert(!ecs_iter_is_true(&it), ECS_INTERNAL_ERROR, NULL);
    }
#endif

    /* Clearing the script can move the script entity, which invalidates the
     * component pointer. */
    s = ecs_ensure(world, e, EcsScript);

    ecs_script_t *parsed = s->script;
    ecs_script_impl_t *parsed_impl = flecs_script_impl(parsed);
    parsed_impl->evaluating = true;

    /* Keep the parsed script alive for the duration of the evaluation, so that
     * code that runs while the script is evaluated (such as a component hook)
     * can't free the AST that is being evaluated by deleting the script. */
    parsed_impl->refcount ++;

    ecs_script_eval_desc_t eval_desc = { .runtime = eval_runtime };
    int eval_result_code = flecs_script_eval(parsed, &eval_desc,
        flecs_script_tag(e, instance), UINT64_MAX, &eval_result);

    if (!ecs_is_alive(world, e)) {
        /* Script entity was deleted while it was being evaluated. */
        parsed_impl->evaluating = false;
        flecs_script_runtime_error_reset(runtime);
        ecs_os_free(eval_result.error);
        ecs_script_free(parsed);
        if (is_defer) {
            flecs_resume_readonly(real_world, &srs);
        }
        return -1;
    }

    if (eval_result_code) {
        s = ecs_ensure(world, e, EcsScript);
        s->error = eval_result.error;
        if (!s->error) {
            /* A failed evaluation must never go unreported. */
            s->error = ecs_os_strdup("failed to evaluate script");
        }
        if (runtime->error_name && runtime->include_depth) {
            ecs_log_(-3, NULL, 0, "%s: %s: %s",
                name ? name : "script", runtime->error_name, s->error);
        } else {
            ecs_log_(-3, NULL, 0, "%s: %s",
                runtime->error_name ? runtime->error_name :
                    (name ? name : "script"),
                s->error);
        }
        flecs_script_runtime_error_reset(runtime);
        if (!instance) {
            flecs_script_update_resolve_observers(world, e,
                flecs_script_impl(parsed), &s->observers);
            s = ecs_ensure(world, e, EcsScript);
            flecs_script_ref_observers_clear(world, &s->dyn_observers);
            s = ecs_ensure(world, e, EcsScript);
        }
        ecs_script_free(parsed);
        s->script = NULL;
        ecs_delete_with(world, ecs_pair_t(EcsScript, e));
        result = -1;
    } else {
        flecs_script_impl(parsed)->evaluating = false;
        if (!instance) {
            s = ecs_ensure(world, e, EcsScript);
            ecs_vec_t *script_refs = &flecs_script_impl(s->script)->refs;
            ecs_script_ref_t *refs = ecs_vec_first(script_refs);
            int32_t i;
            for (i = ecs_vec_count(script_refs) - 1; i >= 0; i --) {
                if (refs[i].component == ecs_id(EcsScriptMutVar)) {
                    continue;
                }
                if (refs[i].entity && ecs_has_pair(
                    world, refs[i].entity, ecs_id(EcsScript), e))
                {
                    ecs_vec_remove_t(script_refs, ecs_script_ref_t, i);
                }
            }
            flecs_script_ref_observers_clear(world, &s->observers);
            flecs_script_update_ref_observers(world, e, 0,
                script_refs, &s->observers, flecs_script_ref_on_set);
            ecs_vec_clear(script_refs);
            s = ecs_ensure(world, e, EcsScript);
            flecs_script_update_dyn_observers(world, e,
                flecs_script_impl(parsed), &s->dyn_observers, UINT64_MAX);
            ecs_vec_clear(&flecs_script_impl(parsed)->run_refs);
        }
    }

    ecs_script_free(parsed);

done:
    if (result) {
        ecs_add_id(world, e, EcsScriptError);
    } else {
        ecs_remove_id(world, e, EcsScriptError);
    }

    if (is_defer) {
        flecs_resume_readonly(real_world, &srs);
    }

    return result;
}

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code)
{
    return flecs_script_update(world, e, instance, code, NULL);
}

ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc)
{
    const char *script = NULL;
    ecs_entity_t e = desc->entity;
    
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!e) {
        if (desc->filename) {
            e = ecs_new_from_path_w_sep(world, 0, desc->filename, "/", NULL);
        } else {
            e = ecs_new(world);
        }
    }

    if (desc->lenient) {
        EcsScript *comp = ecs_ensure(world, e, EcsScript);
        comp->lenient = true;
    }

    script = desc->code;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            goto error;
        }

        EcsScript *comp = ecs_ensure(world, e, EcsScript);
        comp->filename = ecs_os_strdup(desc->filename);
    }

    ecs_script_update(world, e, 0, script);

    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }

    return e;
error:
    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }
    if (!desc->entity) {
        ecs_delete(world, e);
    }
    return 0;
}

static int EcsScript_serialize(
    const ecs_serializer_t *ser, 
    const void *ptr) 
{
    const EcsScript *data = ptr;
    ser->member(ser, "filename");
    ser->value(ser, ecs_id(ecs_string_t), &data->filename);
    ser->member(ser, "code");
    ser->value(ser, ecs_id(ecs_string_t), &data->code);
    ser->member(ser, "error");
    ser->value(ser, ecs_id(ecs_string_t), &data->error);

    if (data->script) {
        char *ast = ecs_script_ast_to_str(data->script, true);
        ser->member(ser, "ast");
        ser->value(ser, ecs_id(ecs_string_t), &ast);
        ecs_os_free(ast);
    } else {
        char *nullString = NULL;
        ser->member(ser, "ast");
        ser->value(ser, ecs_id(ecs_string_t), &nullString);
    }
    return 0;
}

void FlecsScriptImport(
    ecs_world_t *world)
{
    ecs_assert(FLECS_SCRIPT_VECTOR_FUNCTION_COUNT == EcsPrimitiveKindLast,
        ECS_INTERNAL_ERROR, NULL);

    ECS_MODULE(world, FlecsScript);
    ECS_IMPORT(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsScript);
    ECS_COMPONENT_DEFINE(world, EcsScriptVisitor);
    ECS_TAG_DEFINE(world, EcsScriptVectorType);
    ECS_TAG_DEFINE(world, EcsScriptError);

    ecs_add_pair(world, EcsScriptError, EcsOnInstantiate, EcsDontInherit);

    ecs_add_pair(world, ecs_id(EcsScriptVisitor), EcsOnInstantiate,
        EcsDontInherit);

    ecs_set(world, ecs_id(EcsStruct), EcsScriptVisitor, {
        .visit = flecs_script_struct_visit
    });

    ecs_set(world, ecs_id(EcsEnum), EcsScriptVisitor, {
        .visit = flecs_script_enum_visit
    });

    ecs_set(world, ecs_id(EcsBitmask), EcsScriptVisitor, {
        .visit = flecs_script_bitmask_visit
    });

    ecs_set_hooks(world, EcsScript, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScript),
        .dtor = ecs_dtor(EcsScript),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ECS_COMPONENT(world, ecs_script_t);

    ecs_entity_t opaque_view = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "ecs_script_view_t" }),
        .members = {
            { .name = "filename", .type = ecs_id(ecs_string_t) },
            { .name = "code", .type = ecs_id(ecs_string_t) },
            { .name = "error", .type = ecs_id(ecs_string_t) },
            { .name = "ast", .type = ecs_id(ecs_string_t) }
        }
    });

    ecs_opaque(world, {
        .entity = ecs_id(EcsScript),
        .type.as_type = opaque_view,
        .type.serialize = EcsScript_serialize
    });

    ecs_add_id(world, ecs_id(EcsScript), EcsPairIsTag);
    ecs_add_pair(world, ecs_id(EcsScript), EcsOnInstantiate, EcsDontInherit);

    flecs_script_refs_import(world);
    flecs_script_template_import(world);
    flecs_function_import(world);
#ifdef FLECS_SCRIPT_ASYNC
    flecs_script_async_import(world);
#endif
}

#endif
