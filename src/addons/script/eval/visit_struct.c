/**
 * @file addons/script/eval/visit_struct.c
 * @brief Visitor that implements the struct Position(x: f32, y: f32) syntax.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"
#include "../../meta/meta.h"

bool flecs_script_struct_member_is_inherited(
    ecs_world_t *world,
    ecs_entity_t struct_type,
    const char *name)
{
    const EcsStruct *st = ecs_get(world, struct_type, EcsStruct);
    if (!st) {
        return false;
    }

    int32_t i, inherited = flecs_struct_inherited_count(world, struct_type, st);
    const ecs_member_t *members = ecs_vec_first_t(&st->members, ecs_member_t);
    for (i = 0; i < inherited; i ++) {
        if (!ecs_os_strcmp(members[i].name, name)) {
            return true;
        }
    }

    return false;
}

int flecs_script_struct_visit(
    const ecs_script_visitor_ctx_t *ctx)
{
    ecs_world_t *world = ctx->world;
    ecs_script_eval_visitor_t *v = ctx->eval;
    ecs_script_t *script = &v->base.script->pub;
    ecs_expr_initializer_t *node = ctx->initializer;

    if (!node || node->node.kind == EcsExprEmptyInitializer ||
        !ecs_vec_count(&node->elements))
    {
        const EcsStruct *st = ecs_get(world, ctx->entity, EcsStruct);
        if (st && flecs_struct_inherited_count(world, ctx->entity, st) > 0) {
            return 0;
        }

        flecs_script_eval_error(v, NULL,
            "struct '%s' must have at least one member "
            "('struct %s(name: type)')",
                ecs_get_name(world, ctx->entity),
                ecs_get_name(world, ctx->entity));
        return -1;
    }

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(EcsMember));
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);

    for (i = 0; i < count; i ++) {
        ecs_expr_initializer_element_t *elem = &elems[i];

        if (!elem->member) {
            flecs_expr_visit_error(script, elem->value,
                "missing name for struct member (expected 'name: type')");
            return -1;
        }

        if (elem->operator) {
            flecs_expr_visit_error(script, elem->value,
                "invalid operator for struct member '%s'", elem->member);
            return -1;
        }

        if (!elem->value) {
            flecs_expr_visit_error(script, NULL,
                "missing type for struct member '%s'", elem->member);
            return -1;
        }

        if (flecs_script_struct_member_is_inherited(
            world, ctx->entity, elem->member))
        {
            flecs_expr_visit_error(script, elem->value,
                "member '%s' of struct '%s' is already defined by base type",
                elem->member, ecs_get_name(world, ctx->entity));
            return -1;
        }

        ecs_entity_t m = flecs_script_create_entity(v, elem->member);
        if (!m) {
            return -1;
        }

        if (elem->value->kind == EcsExprInitializer ||
            elem->value->kind == EcsExprEmptyInitializer)
        {
            /* Evaluate into temporary storage. Evaluating the expression can
             * move the member entity, which would invalidate a pointer into
             * the component storage. */
            ecs_value_t value = {
                .ptr = ecs_os_alloca(ti->size),
                .type = ecs_id(EcsMember)
            };

            ecs_os_memset(value.ptr, 0, ti->size);

            if (flecs_script_eval_expr(v, &elem->value, &value)) {
                return -1;
            }

            void *dst = ecs_ensure_id(world, m, ecs_id(EcsMember),
                flecs_ito(size_t, ti->size));
            ecs_os_memcpy(dst, value.ptr, ti->size);

            ecs_modified(world, m, EcsMember);
        } else {
            ecs_entity_t type = 0;
            ecs_value_t value = {
                .ptr = &type,
                .type = ecs_id(ecs_entity_t)
            };

            if (flecs_script_eval_expr(v, &elem->value, &value)) {
                return -1;
            }

            if (!type) {
                flecs_expr_visit_error(script, elem->value,
                    "invalid type for struct member '%s'", elem->member);
                return -1;
            }

            ecs_set(world, m, EcsMember, { .type = type });
        }
    }

    return 0;
}

#endif
