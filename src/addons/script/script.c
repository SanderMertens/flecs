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

static void flecs_script_component_fini(
    EcsScript *ptr)
{
    if (ptr->template_) {
        flecs_script_template_fini(
            flecs_script_impl(ptr->script), ptr->template_);
    }

    if (ptr->script) {
#ifdef FLECS_SCRIPT_ASYNC
        if (!ptr->template_) {
            flecs_script_async_tasks_free(
                &flecs_script_impl(ptr->script)->async_tasks);
        }
#endif
        ecs_script_free(ptr->script);
    }

    ecs_os_free(ptr->filename);
    ecs_os_free(ptr->code);
    ecs_os_free(ptr->error);
    flecs_script_ref_observers_fini(&ptr->observers);
    flecs_script_ref_observers_fini(&ptr->dyn_observers);
}

static ECS_MOVE(EcsScript, dst, src, {
    flecs_script_component_fini(dst);
    *dst = *src;
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsScript, ptr, {
    flecs_script_component_fini(ptr);
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
    ecs_vec_init_t(NULL, &result->async_tasks, void*, 0);
    ecs_vec_init_t(NULL, &result->refs, ecs_script_ref_t, 0);
    ecs_vec_init_t(NULL, &result->run_refs, ecs_script_ref_t, 0);
    flecs_script_state_init(&result->state);
    ecs_vec_init_t(NULL, &result->regions, ecs_script_region_t, 0);
    ecs_vec_init_t(NULL, &result->unresolved_refs,
        ecs_script_unresolved_ref_t, 0);
    ecs_vec_init_t(NULL, &result->unresolved_component_refs,
        ecs_script_unresolved_component_ref_t, 0);
    ecs_vec_init_t(NULL, &result->skip_unknown_warned, char*, 0);
    return &result->pub;
}

char* ecs_script_ir_to_str(
    const ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(
        ECS_CONST_CAST(ecs_script_t*, script));
    int r = 0;
    if (!impl->compiled) {
        ecs_entity_t prev_scope = ecs_set_scope(script->world, 0);
        r = flecs_script_eval(script, NULL, 0, 0, NULL);
        ecs_set_scope(script->world, prev_scope);
    }

    ecs_script_ir_t *ir = r ? NULL : flecs_script_ir_ensure(impl);
    if (!ir) {
        goto error;
    }

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_script_ir_to_buf(impl, ir, &buf);
    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

void flecs_script_skip_unknown_warn(
    ecs_script_t *script,
    const char *name,
    const char *msg)
{
    ecs_script_impl_t *impl = flecs_script_impl(script);
    char **names = ecs_vec_first(&impl->skip_unknown_warned);
    int32_t i, count = ecs_vec_count(&impl->skip_unknown_warned);
    for (i = 0; i < count; i ++) {
        if (!ecs_os_strcmp(names[i], name)) {
            return;
        }
    }

    ecs_vec_append_t(NULL, &impl->skip_unknown_warned, char*)[0] =
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

void flecs_script_unresolved_ref_add(
    ecs_script_impl_t *impl,
    const char *pos,
    const char *name,
    flecs_script_unresolved_kind_t kind)
{
    ecs_script_unresolved_ref_t *ref = ecs_vec_append_t(
        NULL, &impl->unresolved_refs, ecs_script_unresolved_ref_t);
    ref->name = name;
    ref->kind = kind;
    flecs_script_pos_to_line_col(impl->pub.code, pos, &ref->line, &ref->column);
    ref->offset = ref->line ? (int32_t)(pos - impl->pub.code) : -1;
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

int ecs_script_run_w_desc(
    ecs_world_t *world,
    const char *name,
    const char *code,
    const ecs_script_eval_desc_t *desc,
    ecs_script_eval_result_t *result)
{
    ecs_script_t *script = ecs_script_parse(world, name, code, desc, result);
    if (!script) {
        return -1;
    }

    int r = ecs_script_eval(script, desc, result);
    ecs_script_free(script);
    return r;
}

int ecs_script_run(
    ecs_world_t *world,
    const char *name,
    const char *code,
    ecs_script_eval_result_t *result)
{
    return ecs_script_run_w_desc(world, name, code, NULL, result);
}

int ecs_script_run_file_w_desc(
    ecs_world_t *world,
    const char *filename,
    const ecs_script_eval_desc_t *desc)
{
    char *script = flecs_load_from_file(filename);
    if (!script) {
        return -1;
    }

    int result = ecs_script_run_w_desc(world, filename, script, desc, NULL);
    ecs_os_free(script);
    return result;
}

int ecs_script_run_file(
    ecs_world_t *world,
    const char *filename) 
{
    return ecs_script_run_file_w_desc(world, filename, NULL);
}

void ecs_script_free(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_impl_t *impl = flecs_script_impl(script);
    ecs_check(impl->refcount > 0, ECS_INVALID_OPERATION, NULL);
    if (!--impl->refcount) {
#ifdef FLECS_SCRIPT_ASYNC
        flecs_script_async_tasks_free(&impl->async_tasks);
#endif
        ecs_vec_fini_t(NULL, &impl->async_tasks, void*);
        ecs_assert(impl->task_refcount == 0, ECS_INVALID_OPERATION,
            "script freed while tasks are still alive");
        flecs_script_ir_free(impl->ir);
        impl->ir = NULL;
        flecs_script_edit_cache_fini(impl);
        flecs_script_visit_free(script);
        flecs_expr_visit_free(script, impl->expr);
        ecs_vec_fini_t(NULL, &impl->refs, ecs_script_ref_t);
        ecs_vec_fini_t(NULL, &impl->run_refs, ecs_script_ref_t);
        flecs_script_state_fini(&impl->state);
        ecs_vec_fini_t(NULL, &impl->regions, ecs_script_region_t);
        ecs_vec_fini_t(NULL, &impl->unresolved_refs,
            ecs_script_unresolved_ref_t);
        ecs_vec_fini_t(NULL, &impl->unresolved_component_refs,
            ecs_script_unresolved_component_ref_t);
        {
            char **warned = ecs_vec_first(&impl->skip_unknown_warned);
            int32_t wi, wcount = ecs_vec_count(&impl->skip_unknown_warned);
            for (wi = 0; wi < wcount; wi ++) {
                ecs_os_free(warned[wi]);
            }

            ecs_vec_fini_t(NULL, &impl->skip_unknown_warned, char*);
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

static EcsScript* flecs_script_set_error(
    ecs_world_t *world,
    ecs_entity_t e,
    char *error,
    const char *fallback)
{
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    ecs_os_free(s->error);
    s->error = error ? error : ecs_os_strdup(fallback);
    return s;
}

static ecs_script_t* flecs_script_update_parse_code(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *code)
{
    const char *name = ecs_get_name(world, e);
    const EcsScript *s = ecs_get(world, e, EcsScript);
    ecs_assert(s != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_eval_result_t parse_result = {0};
    ecs_script_eval_desc_t parse_desc = {
        .skip_unknown = s->skip_unknown,
        .ir = s->ir
    };

    ecs_script_t *parsed = ecs_script_parse(
        world, name, code, &parse_desc, &parse_result);
    if (!parsed) {
        s = flecs_script_set_error(
            world, e, parse_result.error, "failed to parse script");
        ecs_log_(-3, NULL, 0, "%s: %s", name ? name : "script", s->error);
    }

    return parsed;
}

static int flecs_script_update_parse(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_script_t *parsed)
{
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    if (s->template_) {
        char *template_name = ecs_get_path(world, s->template_->props.type);
        flecs_script_set_error(world, e, flecs_asprintf(
            "cannot update scripts for individual templates, "
            "update parent script instead (tried to update '%s')",
                template_name), NULL);
        ecs_err("%s", s->error);
        ecs_os_free(template_name);
        if (parsed) {
            ecs_script_free(parsed);
        }
        return -1;
    }

    ecs_os_free(s->code);
    s->code = ecs_os_strdup(code);
    ecs_os_free(s->error);
    s->error = NULL;

    if (s->script) {
#ifdef FLECS_SCRIPT_ASYNC
        flecs_script_async_tasks_free(
            &flecs_script_impl(s->script)->async_tasks);
#endif
        ecs_script_free(s->script);
    }

    flecs_script_runtime_error_reset(flecs_script_runtime_get(world));
    if (!parsed) {
        parsed = flecs_script_update_parse_code(world, e, code);
    }

    s->script = parsed;
    if (!parsed) {
        if (!instance) {
            flecs_script_ref_observers_clear(world, &s->observers);
            flecs_script_ref_observers_clear(world, &s->dyn_observers);
        }

        return -1;
    }

    flecs_script_impl(parsed)->entity = e;
    return 0;
}

static void flecs_script_update_error(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    ecs_script_t *parsed,
    const char *name,
    char *error)
{
    ecs_script_runtime_t *runtime = flecs_script_runtime_get(world);
    EcsScript *s = flecs_script_set_error(
        world, e, error, "failed to evaluate script");

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
}

static void flecs_script_update_observers(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_script_t *parsed)
{
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    ecs_vec_t *script_refs = &flecs_script_impl(s->script)->refs;
    flecs_script_observe_refs(world, e, script_refs, &s->observers);
    ecs_vec_clear(script_refs);
    s = ecs_ensure(world, e, EcsScript);
    flecs_script_update_dyn_observers(world, e, 0,
        &flecs_script_impl(parsed)->run_refs, &s->dyn_observers, UINT64_MAX);
    ecs_vec_clear(&flecs_script_impl(parsed)->run_refs);
}

static int flecs_script_update_impl(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_script_runtime_t *eval_runtime,
    bool preparse)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(code != NULL, ECS_INTERNAL_ERROR, NULL);

    int result = 0;
    bool is_defer = ecs_is_stage(world) || ecs_stage_is_readonly(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
        world = real_world;
    }

    /* Parse the new version before anything in the world is deleted, so a
     * script that does not parse leaves the previous version in place. */
    ecs_script_t *preparsed = NULL;
    if (preparse) {
        preparsed = flecs_script_update_parse_code(world, e, code);
        if (!preparsed) {
            result = -1;
            goto done;
        }
    }

    const char *name = ecs_get_name(world, e);
    if (flecs_script_update_parse(world, e, instance, code, preparsed)) {
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

    EcsScript *s = ecs_ensure(world, e, EcsScript);

    ecs_script_t *parsed = s->script;
    ecs_script_impl_t *parsed_impl = flecs_script_impl(parsed);
    parsed_impl->evaluating = true;
    parsed_impl->refcount ++;

    ecs_script_eval_result_t eval_result = {0};
    ecs_script_eval_desc_t eval_desc = { .runtime = eval_runtime, .ir = s->ir };
    int eval_result_code = flecs_script_eval(parsed, &eval_desc,
        flecs_script_tag(e, instance), UINT64_MAX, &eval_result);

    if (!ecs_is_alive(world, e)) {
        parsed_impl->evaluating = false;
        flecs_script_runtime_error_reset(flecs_script_runtime_get(world));
        ecs_os_free(eval_result.error);
        result = -1;
        goto done_free;
    }

    if (eval_result_code) {
        flecs_script_update_error(world, e, instance, parsed, name,
            eval_result.error);
        result = -1;
    } else {
        parsed_impl->evaluating = false;
        if (!instance) {
            flecs_script_pending_resolve_remove(world, e);
            flecs_script_update_observers(world, e, parsed);
        }
    }

done_free:
    ecs_script_free(parsed);

done:
    if (is_defer) {
        flecs_resume_readonly(real_world, &srs);
    }

    return result;
}

int flecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code,
    ecs_script_runtime_t *eval_runtime)
{
    return flecs_script_update_impl(
        world, e, instance, code, eval_runtime, false);
}

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code)
{
    return flecs_script_update(world, e, instance, code, NULL);
}

int ecs_script_reload(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *code)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(e != 0, ECS_INVALID_PARAMETER, NULL);

    char *file_code = NULL;
    const EcsScript *s = ecs_get(world, e, EcsScript);
    if (!s) {
        ecs_err("entity is not a script, cannot reload");
        goto error;
    }

    if (!code) {
        if (!s->filename) {
            ecs_err("script was not loaded from a file, cannot reload");
            goto error;
        }

        file_code = flecs_load_from_file(s->filename);
        if (!file_code) {
            goto error;
        }

        code = file_code;
    }

    int result = flecs_script_update_impl(world, e, 0, code, NULL, true);
    ecs_os_free(file_code);
    return result;
error:
    return -1;
}

ecs_entity_t ecs_script_init(
    ecs_world_t *world,
    const ecs_script_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t e = desc->entity;
    if (!e) {
        if (desc->filename) {
            e = ecs_new_from_path_w_sep(world, 0, desc->filename, "/", "/");
        } else {
            e = ecs_new(world);
        }
    }

    if (desc->skip_unknown || desc->ir) {
        EcsScript *comp = ecs_ensure(world, e, EcsScript);
        comp->skip_unknown = desc->skip_unknown;
        comp->ir = desc->ir;
    }

    const char *script = desc->code;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            if (!desc->entity) {
                ecs_delete(world, e);
            }

            return 0;
        }

        EcsScript *comp = ecs_ensure(world, e, EcsScript);
        ecs_os_strset(&comp->filename, desc->filename);
    }

    ecs_script_update(world, e, 0, script);

    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }

    return e;
error:
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

    ecs_set(world, ecs_id(EcsVector), EcsScriptVisitor, {
        .visit = flecs_script_vector_visit
    });

    ecs_set(world, ecs_id(EcsArray), EcsScriptVisitor, {
        .visit = flecs_script_array_visit
    });

    ecs_set(world, ecs_id(EcsMap), EcsScriptVisitor, {
        .visit = flecs_script_map_visit
    });

    ecs_set(world, ecs_id(EcsPrimitive), EcsScriptVisitor, {
        .visit = flecs_script_primitive_visit
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
