/**
 * @file addons/script/script.c
 * @brief Script API.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ECS_COMPONENT_DECLARE(EcsScript);
ECS_COMPONENT_DECLARE(EcsScriptConstVar);
ECS_COMPONENT_DECLARE(EcsScriptFunction);
ECS_COMPONENT_DECLARE(EcsScriptMethod);

static
ECS_MOVE(EcsScript, dst, src, {
    if (dst->script && (dst->script != src->script)) {
        if (dst->template_ && (dst->template_ != src->template_)) {
            flecs_script_template_fini(
                flecs_script_impl(dst->script), dst->template_);
        }
        ecs_script_free(dst->script);
    }

    dst->filename = src->filename;
    dst->code = src->code;
    dst->error = src->error;
    dst->script = src->script;
    dst->template_ = src->template_;

    src->filename = NULL;
    src->code = NULL;
    src->error = NULL;
    src->script = NULL;
    src->template_ = NULL;
})

static
ECS_DTOR(EcsScript, ptr, {
    if (ptr->template_) {
        flecs_script_template_fini(
            flecs_script_impl(ptr->script), ptr->template_);
    }

    if (ptr->script) {
        ecs_script_free(ptr->script);
    }

    ecs_os_free(ptr->filename);
    ecs_os_free(ptr->code);
    ecs_os_free(ptr->error);
})

static
ecs_id_t flecs_script_tag(
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
    return &result->pub;
}

void ecs_script_clear(
    ecs_world_t *world,
    ecs_entity_t script,
    ecs_entity_t instance)
{
    if (!instance) {
        ecs_delete_with(world, ecs_pair_t(EcsScript, script));
    } else {
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

    ecs_entity_t prev_scope = ecs_set_scope(world, 0);

    if (ecs_script_eval(script, NULL, result)) {
        goto error_free;
    }

    ecs_set_scope(world, prev_scope);

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
        flecs_script_visit_free(script);
        flecs_expr_visit_free(script, impl->expr);
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

int ecs_script_update(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_entity_t instance,
    const char *code)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(code != NULL, ECS_INTERNAL_ERROR, NULL);

    const char *name = ecs_get_name(world, e);
    EcsScript *s = ecs_ensure(world, e, EcsScript);
    if (s->template_) {
        char *template_name = ecs_get_path(world, s->template_->entity);
        ecs_err("cannot update scripts for individual templates, "
            "update parent script instead (tried to update '%s')",
                template_name);
        ecs_os_free(template_name);
        return -1;
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

    ecs_script_eval_result_t eval_result = {NULL};

    s->script = ecs_script_parse(world, name, code, NULL, &eval_result);
    if (s->script == NULL) {
        s->error = eval_result.error;
        return -1;
    }

    int result = 0;
    bool is_defer = ecs_is_deferred(world);
    ecs_suspend_readonly_state_t srs;
    ecs_world_t *real_world = NULL;
    if (is_defer) {
        ecs_assert(flecs_poly_is(world, ecs_world_t), ECS_INTERNAL_ERROR, NULL);
        real_world = flecs_suspend_readonly(world, &srs);
        ecs_assert(real_world != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_script_clear(world, e, instance);

    ecs_entity_t prev = ecs_set_with(world, flecs_script_tag(e, instance));

    if (ecs_script_eval(s->script, NULL, &eval_result)) {
        s->error = eval_result.error;
        ecs_script_free(s->script);
        s->script = NULL;
        ecs_delete_with(world, ecs_pair_t(EcsScript, e));
        result = -1;
    }

    ecs_set_with(world, prev);

    if (is_defer) {
        flecs_resume_readonly(real_world, &srs);
    }

    return result;
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

    script = desc->code;
    if (!script && desc->filename) {
        script = flecs_load_from_file(desc->filename);
        if (!script) {
            goto error;
        }

        EcsScript *comp = ecs_ensure(world, e, EcsScript);
        comp->filename = ecs_os_strdup(desc->filename);
    }

    if (ecs_script_update(world, e, 0, script)) {
        goto code_error;
    }

    if (script != desc->code) {
        /* Safe cast, only happens when script is loaded from file */
        ecs_os_free(ECS_CONST_CAST(char*, script));
    }

code_error:
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

ecs_script_runtime_t* ecs_script_runtime_new(void) 
{
    ecs_script_runtime_t *r = ecs_os_calloc_t(ecs_script_runtime_t);
    flecs_expr_stack_init(&r->expr_stack);
    flecs_allocator_init(&r->allocator);
    flecs_stack_init(&r->stack);
    ecs_vec_init_t(&r->allocator, &r->using, ecs_entity_t, 0);
    ecs_vec_init_t(&r->allocator, &r->with, ecs_value_t, 0);
    ecs_vec_init_t(&r->allocator, &r->with_type_info, ecs_type_info_t*, 0);
    ecs_vec_init_t(&r->allocator, &r->annot, ecs_script_annot_t*, 0);
    return r;
}

void ecs_script_runtime_free(
    ecs_script_runtime_t *r)
{
    flecs_expr_stack_fini(&r->expr_stack);
    ecs_vec_fini_t(&r->allocator, &r->annot, ecs_script_annot_t*);
    ecs_vec_fini_t(&r->allocator, &r->with, ecs_value_t);
    ecs_vec_fini_t(&r->allocator, &r->with_type_info, ecs_type_info_t*);
    ecs_vec_fini_t(&r->allocator, &r->using, ecs_entity_t);
    flecs_allocator_fini(&r->allocator);
    flecs_stack_fini(&r->stack);
    ecs_os_free(r);
}

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r)
{
    ecs_vec_clear(&r->annot);
    ecs_vec_clear(&r->with);
    ecs_vec_clear(&r->with_type_info);
    ecs_vec_clear(&r->using);
}

ecs_script_runtime_t* flecs_script_runtime_get(
    ecs_world_t *world)
{
    ecs_stage_t *stage;
    if (flecs_poly_is(world, ecs_stage_t)) {
        stage = (ecs_stage_t*)world;
    } else {
        stage = world->stages[0];
    }

    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!stage->runtime) {
        stage->runtime = ecs_script_runtime_new();
    }

    return stage->runtime;
}

static
int EcsScript_serialize(
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
    ECS_MODULE(world, FlecsScript);
    ECS_IMPORT(world, FlecsMeta);
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsScript), 
        "Module with components for managing Flecs scripts");
#endif

    ecs_set_name_prefix(world, "Ecs");
    ECS_COMPONENT_DEFINE(world, EcsScript);

    ecs_set_hooks(world, EcsScript, {
        .ctor = flecs_default_ctor,
        .move = ecs_move(EcsScript),
        .dtor = ecs_dtor(EcsScript),
        .flags = ECS_TYPE_HOOK_COPY_ILLEGAL
    });

    ECS_COMPONENT(world, ecs_script_t);

    ecs_entity_t opaque_view = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "ecs_script_view_t"}),
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
    ecs_add_id(world, ecs_id(EcsScript), EcsPrivate);
    ecs_add_pair(world, ecs_id(EcsScript), EcsOnInstantiate, EcsDontInherit);

    flecs_script_template_import(world);
    flecs_function_import(world);
}

#endif
