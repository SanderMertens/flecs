/**
 * @file addons/script/visit.c
 * @brief Script AST visitor utilities.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ecs_script_node_t* ecs_script_parent_node_(
    ecs_script_visit_t *v)
{
    if (v->depth > 1) {
        return v->nodes[v->depth - 2]; /* Last node is current node */
    } else {
        return NULL;
    }
}

ecs_script_scope_t* ecs_script_current_scope_(
    ecs_script_visit_t *v)
{
    int32_t depth;
    for(depth = v->depth - 1; depth >= 0; depth --) {
        ecs_script_node_t *node = v->nodes[depth];
        if (node->kind == EcsAstScope) {
            return (ecs_script_scope_t*)node;
        }
    }

    return NULL;
}

ecs_script_node_t* ecs_script_parent_(
    ecs_script_visit_t *v,
    ecs_script_node_t *child)
{
    int32_t depth;
    for(depth = v->depth - 1; depth >= 0; depth --) {
        ecs_script_node_t *node = v->nodes[depth];
        if (node == child && depth) {
            return v->nodes[depth - 1];
        }
    }

    return NULL;
}

int32_t ecs_script_node_line_number_(
    ecs_script_impl_t *script,
    ecs_script_node_t *node)
{
    const char *ptr;
    int32_t line_count = 1;
    for (ptr = script->pub.code; ptr < node->pos; ptr ++) {
        ecs_assert(ptr[0] != 0, ECS_INTERNAL_ERROR, NULL);
        if (ptr[0] == '\n') {
            line_count ++;
        }
    }

    return line_count;
}

int ecs_script_visit_scope_(
    ecs_script_visit_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **nodes = ecs_vec_first_t(
        &scope->stmts, ecs_script_node_t*);

    v->nodes[v->depth ++] = (ecs_script_node_t*)scope;

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

        v->nodes[v->depth ++] = nodes[i];

        if (v->visit(v, nodes[i])) {
            return -1;
        }

        v->depth --;
    }

    v->depth --;

    return 0;
}

int ecs_script_visit_node_(
    ecs_script_visit_t *v,
    ecs_script_node_t *node)
{
    v->nodes[v->depth ++] = node;

    if (v->visit(v, node)) {
        return -1;
    }

    v->depth --;

    return 0;
}

int ecs_script_visit_(
    ecs_script_visit_t *visitor,
    ecs_visit_action_t visit,
    ecs_script_impl_t *script)
{
    visitor->script = script;
    visitor->visit = visit;
    visitor->depth = 0;
    int result = ecs_script_visit_node(visitor, script->root);
    if (result) {
        return -1;
    }

    if (visitor->depth) {
        ecs_parser_error(script->pub.name, NULL, 0, "unexpected end of script");
        return -1;
    }

    return 0;
}

#endif
