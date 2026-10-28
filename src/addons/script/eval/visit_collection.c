/**
 * @file addons/script/eval/visit_collection.c
 * @brief Visitor that implements the vector Points(type: f32),
 *        array Points(type: f32, count: 3), map Lookup(key_type: i32,
 *        type: f32) and primitive Float(kind: F32) syntax.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

const ecs_member_t* flecs_script_meta_type_member(
    const ecs_world_t *world,
    ecs_entity_t kind,
    const char *member,
    int32_t index)
{
    const EcsStruct *st = ecs_get(world, kind, EcsStruct);
    if (!st) {
        return NULL;
    }

    ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
    int32_t i, count = ecs_vec_count(&st->members);
    if (!member) {
        return index < count ? &members[index] : NULL;
    }

    for (i = 0; i < count; i ++) {
        if (!ecs_os_strcmp(members[i].name, member)) {
            return &members[i];
        }
    }

    return NULL;
}

static int flecs_script_meta_type_visit(
    const ecs_script_visitor_ctx_t *ctx,
    void *ptr,
    const char *kind_str)
{
    ecs_world_t *world = ctx->world;
    ecs_script_eval_visitor_t *v = ctx->eval;
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_initializer_t *node = ctx->initializer;
    const char *name = ecs_get_name(world, ctx->entity);

    if (!node || node->node.kind == EcsExprEmptyInitializer) {
        return 0;
    }

    const EcsStruct *st = ecs_get(world, ctx->kind, EcsStruct);
    ecs_assert(st != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    uint32_t has_member = 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (!elem->value) {
            flecs_expr_visit_error(script, NULL,
                "missing value for %s '%s'", kind_str, name);
            return -1;
        }

        const ecs_member_t *m = flecs_script_meta_type_member(
            world, ctx->kind, elem->member, i);
        if (!m) {
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

        uint32_t bit = 1u << (m - members);
        if (has_member & bit) {
            flecs_expr_visit_error(script, elem->value,
                "%s specified more than once for %s '%s'",
                    m->name, kind_str, name);
            return -1;
        }

        ecs_value_t value = {
            .ptr = ECS_OFFSET(ptr, m->offset),
            .type = m->type
        };
        if (flecs_script_eval_expr(v, &elem->value, &value)) {
            return -1;
        }

        has_member |= bit;
    }

    return 0;
}

static int flecs_script_meta_type_ref_check(
    const ecs_script_visitor_ctx_t *ctx,
    ecs_entity_t type,
    const char *type_str,
    const char *kind_str)
{
    const char *name = ecs_get_name(ctx->world, ctx->entity);

    if (!type) {
        flecs_script_eval_error(ctx->eval, NULL,
            "missing %s for %s '%s'", type_str, kind_str, name);
        return -1;
    }

    if (type == ctx->entity) {
        flecs_script_eval_error(ctx->eval, NULL,
            "%s '%s' cannot have itself as %s", kind_str, name, type_str);
        return -1;
    }

    return 0;
}

static int flecs_script_meta_type_layout_check(
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
    if (flecs_script_meta_type_visit(ctx, &vector, "vector")) {
        return -1;
    }

    if (flecs_script_meta_type_ref_check(
        ctx, vector.type, "element type", "vector"))
    {
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsVector, &vector);

    return flecs_script_meta_type_layout_check(ctx, "vector");
}

int flecs_script_array_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    EcsArray array = {0};
    if (flecs_script_meta_type_visit(ctx, &array, "array")) {
        return -1;
    }

    if (flecs_script_meta_type_ref_check(
        ctx, array.type, "element type", "array"))
    {
        return -1;
    }

    if (array.count <= 0) {
        flecs_script_eval_error(ctx->eval, NULL,
            "invalid count %d for array '%s'", array.count,
                ecs_get_name(ctx->world, ctx->entity));
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsArray, &array);

    return flecs_script_meta_type_layout_check(ctx, "array");
}

int flecs_script_map_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    EcsMap map = {0};
    if (flecs_script_meta_type_visit(ctx, &map, "map")) {
        return -1;
    }

    if (flecs_script_meta_type_ref_check(
        ctx, map.key_type, "key type", "map"))
    {
        return -1;
    }

    if (flecs_script_meta_type_ref_check(
        ctx, map.type, "value type", "map"))
    {
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsMap, &map);

    return flecs_script_meta_type_layout_check(ctx, "map");
}

int flecs_script_primitive_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    EcsPrimitive primitive;
    ecs_os_zeromem(&primitive);
    if (flecs_script_meta_type_visit(ctx, &primitive, "primitive")) {
        return -1;
    }

    if (primitive.kind < EcsBool || primitive.kind > EcsPrimitiveKindLast) {
        flecs_script_eval_error(ctx->eval, NULL,
            "missing or invalid kind for primitive '%s'",
                ecs_get_name(ctx->world, ctx->entity));
        return -1;
    }

    ecs_set_ptr(ctx->world, ctx->entity, EcsPrimitive, &primitive);

    return flecs_script_meta_type_layout_check(ctx, "primitive");
}

#endif
