/**
 * @file addons/script/visit.h
 * @brief Script AST visitor utilities.
 */

#ifndef FLECS_SCRIPT_VISIT_H
#define FLECS_SCRIPT_VISIT_H

typedef struct ecs_script_visit_t ecs_script_visit_t;

typedef int (*ecs_visit_action_t)(
    ecs_script_visit_t *visitor, 
    ecs_script_node_t *node);

/* Visitors track both scope nodes and statement nodes on the traversal stack.
 * For deeply nested scopes this requires roughly 2x the parser nesting depth,
 * plus the root scope node. */
#define ECS_SCRIPT_VISIT_MAX_DEPTH ((ECS_PARSER_MAX_RECURSION_DEPTH * 2) + 4)

struct ecs_script_visit_t {
    ecs_script_impl_t *script;
    ecs_visit_action_t visit;
    ecs_script_node_t* nodes[ECS_SCRIPT_VISIT_MAX_DEPTH];
    ecs_script_node_t *prev, *next;
    int32_t depth;
};

int ecs_script_visit_scope_(
    ecs_script_visit_t *v,
    ecs_script_scope_t *node);

#define ecs_script_visit_scope(visitor, node) \
    ecs_script_visit_scope_((ecs_script_visit_t*)visitor, node)

#endif
