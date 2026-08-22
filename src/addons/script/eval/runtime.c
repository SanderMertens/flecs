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
    return r;
}

void ecs_script_runtime_free(
    ecs_script_runtime_t *r)
{
    flecs_expr_stack_fini(&r->expr_stack);
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
