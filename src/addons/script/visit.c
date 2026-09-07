/**
 * @file addons/script/visit.c
 * @brief Script AST visitor utilities.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

static int flecs_script_visit_push_checked(
    ecs_script_visit_t *v,
    ecs_script_node_t *node)
{
    if (v->depth >= ECS_SCRIPT_VISIT_MAX_DEPTH) {
        ecs_err("maximum script nesting depth exceeded");
        return -1;
    }

    v->nodes[v->depth ++] = node;
    return 0;
}

int ecs_script_visit_scope_(
    ecs_script_visit_t *v,
    ecs_script_scope_t *scope)
{
    int32_t depth = v->depth;
    ecs_script_node_t **nodes = ecs_vec_first_t(
        &scope->stmts, ecs_script_node_t*);

    if (flecs_script_visit_push_checked(v, (ecs_script_node_t*)scope)) {
        return -1;
    }

    int32_t i, count = ecs_vec_count(&scope->stmts);
    for (i = 0; i < count; i ++) {

        if (!i) {
            v->prev = NULL;
        } else {
            v->prev = nodes[i - 1];
        }

        if (i != (count - 1)) {
            v->next = nodes[i + 1];
        } else {
            v->next = NULL;
        }

        if (flecs_script_visit_push_checked(v, nodes[i])) {
            v->depth = depth;
            return -1;
        }

        ecs_assert(v->visit != NULL, ECS_INTERNAL_ERROR, NULL);

        if (v->visit(v, nodes[i])) {
            v->depth = depth;
            return -1;
        }

        v->depth --;
        ecs_assert(v->nodes[v->depth] == nodes[i], ECS_INTERNAL_ERROR, NULL);
    }

    v->depth = depth;

    return 0;
}

#endif
