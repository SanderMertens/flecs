/**
 * @file addons/script/eval/visit_collection.c
 * @brief Visitor that implements the vector Points(type: f32) and
 *        array Points(type: f32, count: 3) syntax.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static int flecs_script_collection_visit(
    const ecs_script_visitor_ctx_t *ctx,
    ecs_entity_t *elem_type,
    int32_t *elem_count,
    const char *kind_str)
{
    ecs_world_t *world = ctx->world;
    ecs_script_eval_visitor_t *v = ctx->eval;
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_initializer_t *node = ctx->initializer;
    const char *name = ecs_get_name(world, ctx->entity);

    if (!node || node->node.kind == EcsExprEmptyInitializer ||
        !ecs_vec_count(&node->elements))
    {
        flecs_script_eval_error(v, NULL,
            "missing element type for %s '%s'", kind_str, name);
        return -1;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    bool has_type = false, has_count = false;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (elem->operator) {
            flecs_expr_visit_error(script, elem->value,
                "invalid operator for %s '%s'", kind_str, name);
            return -1;
        }

        if (!elem->value) {
            flecs_expr_visit_error(script, NULL,
                "missing value for %s '%s'", kind_str, name);
            return -1;
        }

        bool is_type, is_count;
        if (elem->member) {
            is_type = !ecs_os_strcmp(elem->member, "type");
            is_count = elem_count && !ecs_os_strcmp(elem->member, "count");
        } else {
            is_type = i == 0;
            is_count = elem_count && i == 1;
        }

        if (!is_type && !is_count) {
            if (elem->member) {
                flecs_expr_visit_error(script, elem->value,
                    "unknown member '%s' for %s '%s'",
                        elem->member, kind_str, name);
            } else {
                flecs_expr_visit_error(script, elem->value,
                    "too many elements for %s '%s'", kind_str, name);
            }
            return -1;
        }

        if ((is_type && has_type) || (is_count && has_count)) {
            flecs_expr_visit_error(script, elem->value,
                "%s specified more than once for %s '%s'",
                    is_type ? "type" : "count", kind_str, name);
            return -1;
        }

        ecs_value_t value = is_type
            ? (ecs_value_t){ .ptr = elem_type, .type = ecs_id(ecs_entity_t) }
            : (ecs_value_t){ .ptr = elem_count, .type = ecs_id(ecs_i32_t) };
        if (flecs_script_eval_expr(v, &elem->value, &value)) {
            return -1;
        }

        has_type |= is_type;
        has_count |= is_count;
    }

    if (!*elem_type) {
        flecs_script_eval_error(v, NULL,
            "missing element type for %s '%s'", kind_str, name);
        return -1;
    }

    if (*elem_type == ctx->entity) {
        flecs_script_eval_error(v, NULL,
            "%s '%s' cannot have itself as element type", kind_str, name);
        return -1;
    }

    if (elem_count && *elem_count <= 0) {
        flecs_script_eval_error(v, NULL,
            "invalid count %d for %s '%s'", *elem_count, kind_str, name);
        return -1;
    }

    return 0;
}

static int flecs_script_collection_layout_check(
    const ecs_script_visitor_ctx_t *ctx,
    const char *kind_str)
{
    const EcsComponent *comp = ecs_get(ctx->world, ctx->entity, EcsComponent);
    if (!comp || comp->size <= 0) {
        flecs_script_eval_error(ctx->eval, NULL,
            "invalid layout for %s '%s'",
                kind_str, ecs_get_name(ctx->world, ctx->entity));
        return -1;
    }

    return 0;
}

int flecs_script_vector_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    EcsVector vector = {0};
    if (flecs_script_collection_visit(ctx, &vector.type, NULL, "vector")) {
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsVector, &vector);

    return flecs_script_collection_layout_check(ctx, "vector");
}

int flecs_script_array_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    EcsArray array = {0};
    if (flecs_script_collection_visit(
        ctx, &array.type, &array.count, "array"))
    {
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsArray, &array);

    return flecs_script_collection_layout_check(ctx, "array");
}

#endif
