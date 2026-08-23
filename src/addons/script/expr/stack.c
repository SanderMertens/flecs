/**
 * @file addons/script/expr/stack.c
 * @brief Script expression stack implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

ecs_expr_value_t* flecs_expr_stack_alloc(
    ecs_expr_stack_t *stack,
    const ecs_type_info_t *ti);

static void flecs_expr_value_alloc(
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

    /* The runtime allocated and constructed the buffer, so it owns it until an
     * evaluated node says otherwise. Without this the flag would be whatever
     * the previous occupant of this stack slot left behind. */
    v->owned = true;

    flecs_type_info_ctor(v->value.ptr, 1, ti);
}

static void flecs_expr_value_free(
    ecs_expr_value_t *v)
{
    const ecs_type_info_t *ti = v->type_info;
    v->type_info = NULL;

    if (!v->owned || !v->value.ptr) {
        return; /* Runtime doesn't own value, don't destruct */
    }

    if (ti) {
        if (ti->hooks.dtor) {
            flecs_type_info_dtor(v->value.ptr, 1, ti);
        }
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

/* Nodes that replace the result pointer with storage the runtime does not own.
 * Allocating and constructing a buffer for these results is wasted work, and
 * because the pointer is replaced the buffer is never reachable again: for a
 * large component that is a stack allocation plus a constructor over the whole
 * component per evaluation, and an allocation that is never freed. A node in
 * this list that does need storage on some path must obtain it with
 * flecs_expr_stack_storage. */
static bool flecs_expr_node_borrows_result(
    const ecs_expr_node_t *node)
{
    switch(node->kind) {
    case EcsExprValue:
    case EcsExprVariable:
    case EcsExprGlobalVariable:
    case EcsExprMember:
    case EcsExprElement:
    case EcsExprComponent:
        return true;
    default:
        return false;
    }
}

ecs_expr_value_t* flecs_expr_stack_result(
    ecs_expr_stack_t *stack,
    ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(node->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_expr_node_borrows_result(node)) {
        ecs_expr_value_t *v = flecs_expr_stack_alloc(stack, NULL);
        v->type_info = node->type_info;
        v->value.type = node->type_info->component;
        v->value.ptr = NULL;
        v->owned = false;
        return v;
    }

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

void flecs_expr_stack_storage(
    ecs_expr_stack_t *stack,
    ecs_expr_value_t *v)
{
    if (v->value.ptr) {
        return; /* Value already has storage */
    }

    ecs_assert(v->type_info != NULL, ECS_INTERNAL_ERROR, NULL);
    v->value.ptr = flecs_stack_alloc(
        &stack->stack, v->type_info->size, v->type_info->alignment);
    v->owned = true;
    flecs_type_info_ctor(v->value.ptr, 1, v->type_info);
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
