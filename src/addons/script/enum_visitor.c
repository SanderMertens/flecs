/**
 * @file addons/script/enum_visitor.c
 * @brief Visitor that implements the enum Color(Red, Green, Blue) and
 *        bitmask Toppings(Bacon, Lettuce) syntax.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static
int flecs_script_constants_visit(
    const ecs_script_visitor_ctx_t *ctx,
    bool is_bitmask)
{
    ecs_world_t *world = ctx->world;
    ecs_script_eval_visitor_t *v = ctx->eval;
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_initializer_t *node = ctx->initializer;
    const char *kind_str = is_bitmask ? "bitmask" : "enum";

    if (!node || node->node.kind == EcsExprEmptyInitializer ||
        !ecs_vec_count(&node->elements))
    {
        flecs_script_eval_error(v, NULL,
            "%s '%s' must have at least one constant",
                kind_str, ecs_get_name(world, ctx->entity));
        return -1;
    }

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);

    ecs_entity_t underlying = is_bitmask ? ecs_id(ecs_u32_t) : 0;

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member || !elem->value) {
            continue;
        }

        if (elem->value->kind != EcsExprInitializer &&
            elem->value->kind != EcsExprEmptyInitializer)
        {
            continue;
        }

        ecs_expr_initializer_t *cfg = (ecs_expr_initializer_t*)elem->value;
        ecs_expr_initializer_element_t *cfg_elems = ecs_vec_first(
            &cfg->elements);
        int32_t c, cfg_count = ecs_vec_count(&cfg->elements);
        for (c = 0; c < cfg_count; c ++) {
            ecs_expr_initializer_element_t *cfg_elem = &cfg_elems[c];
            if (!cfg_elem->member || ecs_os_strcmp(
                cfg_elem->member, "underlying_type"))
            {
                flecs_expr_visit_error(script, elem->value,
                    "invalid element in %s configuration (expected "
                    "'{underlying_type: type}')", kind_str);
                return -1;
            }

            if (is_bitmask) {
                flecs_expr_visit_error(script, elem->value,
                    "bitmask does not support underlying_type");
                return -1;
            }

            if (underlying) {
                flecs_expr_visit_error(script, elem->value,
                    "underlying_type specified more than once");
                return -1;
            }

            ecs_value_t value = {
                .ptr = &underlying,
                .type = ecs_id(ecs_entity_t)
            };

            if (flecs_script_eval_expr(v, &cfg_elem->value, &value)) {
                return -1;
            }

            if (!underlying) {
                flecs_expr_visit_error(script, elem->value,
                    "invalid underlying_type for enum '%s'",
                        ecs_get_name(world, ctx->entity));
                return -1;
            }

            EcsEnum *ptr = ecs_ensure(world, ctx->entity, EcsEnum);
            ptr->underlying_type = underlying;
            ecs_modified(world, ctx->entity, EcsEnum);
        }
    }

    if (!underlying) {
        underlying = ecs_id(ecs_i32_t);
    }

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (!elem->member && elem->value &&
            (elem->value->kind == EcsExprInitializer ||
             elem->value->kind == EcsExprEmptyInitializer))
        {
            continue;
        }

        if (elem->member) {
            if (elem->operator) {
                flecs_expr_visit_error(script, elem->value,
                    "invalid operator for constant '%s'", elem->member);
                return -1;
            }

            ecs_entity_t c = flecs_script_create_entity(v, elem->member);
            if (!c) {
                return -1;
            }

            const ecs_type_info_t *ti = ecs_get_type_info(world, underlying);
            if (!ti) {
                flecs_expr_visit_error(script, elem->value,
                    "underlying type is not a type");
                return -1;
            }

            ecs_value_t value = {
                .ptr = ecs_ensure_id(world, c, ecs_pair(EcsConstant, underlying),
                    flecs_ito(size_t, ti->size)),
                .type = underlying
            };

            ecs_os_memset(value.ptr, 0, ti->size);

            if (flecs_script_eval_expr(v, &elem->value, &value)) {
                return -1;
            }

            ecs_modified_id(world, c, ecs_pair(EcsConstant, underlying));
        } else {
            if (!elem->value || elem->value->kind != EcsExprIdentifier) {
                flecs_expr_visit_error(script, elem->value,
                    "invalid constant for %s '%s' (expected identifier or "
                    "'name: value')", kind_str,
                        ecs_get_name(world, ctx->entity));
                return -1;
            }

            const char *name = ((ecs_expr_identifier_t*)elem->value)->value;
            ecs_entity_t c = flecs_script_create_entity(v, name);
            if (!c) {
                return -1;
            }

            ecs_add_id(world, c, EcsConstant);
        }
    }

    return 0;
}

int flecs_script_enum_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    return flecs_script_constants_visit(ctx, false);
}

int flecs_script_bitmask_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    return flecs_script_constants_visit(ctx, true);
}

#endif
