/**
 * @file addons/script/expr/stack.c
 * @brief Script expression stack implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static
void flecs_expr_value_alloc(
    ecs_expr_stack_t *stack,
    ecs_expr_value_t *v,
    const ecs_type_info_t *ti)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(v->type_info == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    v->type_info = ti;
    v->value.type = ti->component;
    v->value.ptr = flecs_stack_alloc(&stack->stack, ti->size, ti->alignment);

    if (ti->hooks.ctor) {
        ti->hooks.ctor(v->value.ptr, 1, ti);
    }
}

static
void flecs_expr_value_free(
    ecs_expr_value_t *v)
{
    const ecs_type_info_t *ti = v->type_info;
    // printf("stack free %p (%s) owned = %d\n", v->value.ptr,
    //     v->type_info->name, v->owned);
    v->type_info = NULL;

    if (!v->owned) {
        return; /* Runtime doesn't own value, don't destruct */
    }

    if (ti && ti->hooks.dtor) {
        ecs_assert(v->value.ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        ti->hooks.dtor(v->value.ptr, 1, ti);
        flecs_stack_free(v->value.ptr, ti->size);
    }

    v->value.ptr = NULL;
}

void flecs_expr_stack_init(
    ecs_expr_stack_t *stack)
{
    ecs_os_zeromem(stack);
    flecs_stack_init(&stack->stack);
}

void flecs_expr_stack_fini(
    ecs_expr_stack_t *stack)
{
    ecs_assert(stack->frame == 0, ECS_INTERNAL_ERROR, NULL);
    flecs_stack_fini(&stack->stack);
}

ecs_expr_value_t* flecs_expr_stack_result(
    ecs_expr_stack_t *stack,
    ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(node->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_expr_stack_alloc(stack, node->type_info);
}

ecs_expr_value_t* flecs_expr_stack_alloc(
    ecs_expr_stack_t *stack,
    const ecs_type_info_t *ti)
{
    ecs_assert(stack->frame > 0, ECS_INTERNAL_ERROR, NULL);

    int32_t sp = stack->frames[stack->frame - 1].sp ++;
    ecs_assert(sp < FLECS_EXPR_STACK_MAX, ECS_OUT_OF_RANGE, 
        "expression nesting is too deep");
    ecs_expr_value_t *v = &stack->values[sp];

    if (ti) {
        flecs_expr_value_alloc(stack, v, ti);
    }

    return v;
}

void flecs_expr_stack_push(
    ecs_expr_stack_t *stack)
{
    int32_t frame = stack->frame ++;
    ecs_assert(frame < FLECS_EXPR_STACK_MAX, ECS_OUT_OF_RANGE, 
        "expression nesting is too deep");
    stack->frames[frame].cur = flecs_stack_get_cursor(&stack->stack);
    if (frame) {
        stack->frames[frame].sp = stack->frames[frame - 1].sp;
    } else {
        stack->frames[frame].sp = 0;
    }
}

void flecs_expr_stack_pop(
    ecs_expr_stack_t *stack)
{
    int32_t frame = -- stack->frame;
    ecs_assert(frame >= 0, ECS_INTERNAL_ERROR, NULL);
    int32_t sp, start = 0, end = stack->frames[frame].sp;
    if (frame) {
        start = stack->frames[frame - 1].sp;
    }

    for (sp = end - 1; sp >= start; sp --) {
        flecs_expr_value_free(&stack->values[sp]);
    }

    flecs_stack_restore_cursor(&stack->stack, stack->frames[frame].cur);
}

#endif
