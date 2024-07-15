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

struct ecs_script_visit_t {
    ecs_script_impl_t *script;
    ecs_visit_action_t visit;
    ecs_script_node_t* nodes[256];
    ecs_script_node_t *prev, *next;
    int32_t depth;
};

int ecs_script_visit_(
    ecs_script_visit_t *visitor,
    ecs_visit_action_t visit,
    ecs_script_impl_t *script);

#define ecs_script_visit(script, visitor, visit) \
    ecs_script_visit_((ecs_script_visit_t*)visitor,\
        (ecs_visit_action_t)visit,\
        script)

int ecs_script_visit_node_(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

#define ecs_script_visit_node(visitor, node) \
    ecs_script_visit_node_((ecs_script_visit_t*)visitor, \
        (ecs_script_node_t*)node)

int ecs_script_visit_scope_(
    ecs_script_visit_t *v,
    ecs_script_scope_t *node);

#define ecs_script_visit_scope(visitor, node) \
    ecs_script_visit_scope_((ecs_script_visit_t*)visitor, node)

ecs_script_node_t* ecs_script_parent_node_(
    ecs_script_visit_t *v);

#define ecs_script_parent_node(visitor) \
    ecs_script_parent_node_((ecs_script_visit_t*)visitor)

ecs_script_scope_t* ecs_script_current_scope_(
    ecs_script_visit_t *v);

#define ecs_script_current_scope(visitor) \
    ecs_script_current_scope_((ecs_script_visit_t*)visitor)

ecs_script_node_t* ecs_script_parent_(
    ecs_script_visit_t *v,
    ecs_script_node_t *node);

#define ecs_script_parent(visitor, node) \
    ecs_script_parent_((ecs_script_visit_t*)visitor, (ecs_script_node_t*)node)

ecs_script_node_t* ecs_script_next_node_(
    ecs_script_visit_t *v);

#define ecs_script_next_node(visitor) \
    ecs_script_next_node_((ecs_script_visit_t*)visitor)

int32_t ecs_script_node_line_number_(
    ecs_script_impl_t *script,
    ecs_script_node_t *node);

#define ecs_script_node_line_number(script, node) \
    ecs_script_node_line_number_(script, (ecs_script_node_t*)node)

#endif
