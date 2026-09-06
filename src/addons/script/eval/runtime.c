/**
 * @file addons/script/eval/runtime.c
 * @brief Script runtime.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

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
    ecs_vec_init_t(&r->allocator, &r->pending_resolves, ecs_entity_t, 0);
    ecs_vec_init_t(NULL, &r->ir_vms, ecs_script_ir_vm_t*, 0);
    ecs_vec_init_t(NULL, &r->call_runtimes, ecs_script_runtime_t*, 0);
    ecs_vec_init_t(NULL, &r->template_pending,
        ecs_script_template_pending_t, 0);
    return r;
}

void ecs_script_runtime_free(
    ecs_script_runtime_t *r)
{
    int32_t i, count = ecs_vec_count(&r->call_runtimes);
    ecs_script_runtime_t **calls = ecs_vec_first(&r->call_runtimes);
    for (i = 0; i < count; i ++) {
        ecs_script_runtime_free(calls[i]);
    }
    ecs_vec_fini_t(NULL, &r->call_runtimes, ecs_script_runtime_t*);
    flecs_expr_stack_fini(&r->expr_stack);
    flecs_script_ir_vm_pool_fini(r);
    flecs_script_template_pending_fini(&r->template_pending);
    ecs_vec_fini_t(&r->allocator, &r->pending_resolves, ecs_entity_t);
    ecs_vec_fini_t(&r->allocator, &r->annot, ecs_script_annot_t*);
    ecs_vec_fini_t(&r->allocator, &r->with, ecs_value_t);
    ecs_vec_fini_t(&r->allocator, &r->with_type_info, ecs_type_info_t*);
    ecs_vec_fini_t(&r->allocator, &r->using, ecs_entity_t);
    flecs_allocator_fini(&r->allocator);
    flecs_stack_fini(&r->stack);
    ecs_os_free(r->error_name);
    ecs_os_free(r->unresolved_errors);
    ecs_os_free(r);
}

ecs_script_runtime_t* flecs_script_runtime_acquire_call(
    ecs_script_runtime_t *r)
{
    if (r->call_runtimes.count) {
        return ((ecs_script_runtime_t**)r->call_runtimes.array)
            [-- r->call_runtimes.count];
    }
    return ecs_script_runtime_new();
}

void flecs_script_runtime_release_call(
    ecs_script_runtime_t *r,
    ecs_script_runtime_t *call)
{
    ecs_script_runtime_clear(call);
    flecs_script_runtime_error_reset(call);
    ecs_vec_append_t(NULL, &r->call_runtimes, ecs_script_runtime_t*)[0] = call;
}

void flecs_script_runtime_error_reset(
    ecs_script_runtime_t *r)
{
    ecs_os_free(r->error_name);
    r->error_name = NULL;
    ecs_os_free(r->unresolved_errors);
    r->unresolved_errors = NULL;
    r->error = false;
}

void ecs_script_runtime_clear(
    ecs_script_runtime_t *r)
{
    ecs_vec_clear(&r->annot);
    ecs_vec_clear(&r->with);
    ecs_vec_clear(&r->with_type_info);
    ecs_vec_clear(&r->using);
    ecs_os_free(r->unresolved_errors);
    r->unresolved_errors = NULL;
    r->error = false;
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

#endif
