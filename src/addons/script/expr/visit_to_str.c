/**
 * @file addons/script/expr_to_str.c
 * @brief Script expression AST to string visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct ecs_expr_str_visitor_t {
    const ecs_world_t *world;
    ecs_strbuf_t *buf;
    int32_t depth;
    bool newline;
} ecs_expr_str_visitor_t;

int flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node);

int flecs_expr_value_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_val_t *node)
{
    return ecs_ptr_to_str_buf(
        v->world, node->node.type, node->ptr, v->buf);
}

int flecs_expr_unary_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_unary_t *node)
{
    ecs_strbuf_appendstr(v->buf, flecs_script_token_str(node->operator));

    if (flecs_expr_node_to_str(v, node->expr)) {
        goto error;
    }

    return 0;
error:
    return -1;
}

int flecs_expr_binary_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_binary_t *node)
{
    ecs_strbuf_appendlit(v->buf, "(");

    if (flecs_expr_node_to_str(v, node->left)) {
        goto error;
    }

    ecs_strbuf_appendlit(v->buf, " ");

    ecs_strbuf_appendstr(v->buf, flecs_script_token_str(node->operator));

    ecs_strbuf_appendlit(v->buf, " ");

    if (flecs_expr_node_to_str(v, node->right)) {
        goto error;
    }

    ecs_strbuf_appendlit(v->buf, ")");

    return 0;
error:
    return -1;
}

int flecs_expr_identifier_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_identifier_t *node)
{
    ecs_strbuf_appendstr(v->buf, node->value);
    return 0;
}

int flecs_expr_variable_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_variable_t *node)
{
    ecs_strbuf_appendlit(v->buf, "$");
    ecs_strbuf_appendstr(v->buf, node->value);
    return 0;
}

int flecs_expr_member_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_member_t *node)
{
    if (flecs_expr_node_to_str(v, node->left)) {
        return -1;
    }

    ecs_strbuf_appendlit(v->buf, ".");
    ecs_strbuf_appendstr(v->buf, node->member_name);
    return 0;
}

int flecs_expr_element_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_element_t *node)
{
    if (flecs_expr_node_to_str(v, node->left)) {
        return -1;
    }

    ecs_strbuf_appendlit(v->buf, "[");
    if (flecs_expr_node_to_str(v, node->index)) {
        return -1;
    }
    ecs_strbuf_appendlit(v->buf, "]");
    return 0;
}

int flecs_expr_cast_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_cast_t *node)
{
    return flecs_expr_node_to_str(v, node->expr);
}

int flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    // if (node->type) {
    //     ecs_strbuf_append(v->buf, "%s", ECS_BLUE);
    //     ecs_strbuf_appendstr(v->buf, ecs_get_name(v->world, node->type));
    //     ecs_strbuf_append(v->buf, "%s(", ECS_NORMAL);
    // }

    switch(node->kind) {
    case EcsExprValue:
        if (flecs_expr_value_to_str(v, (ecs_expr_val_t*)node)) {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_to_str(v, (ecs_expr_unary_t*)node)) {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_to_str(v, (ecs_expr_binary_t*)node)) {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        if (flecs_expr_identifier_to_str(v, (ecs_expr_identifier_t*)node)) {
            goto error;
        }
        break;
    case EcsExprVariable:
        if (flecs_expr_variable_to_str(v, (ecs_expr_variable_t*)node)) {
            goto error;
        }
        break;
    case EcsExprFunction:
        break;
    case EcsExprMember:
        if (flecs_expr_member_to_str(v, (ecs_expr_member_t*)node)) {
            goto error;
        }
        break;
    case EcsExprElement:
        if (flecs_expr_element_to_str(v, (ecs_expr_element_t*)node)) {
            goto error;
        }
        break;
    case EcsExprCast:
        if (flecs_expr_cast_to_str(v, (ecs_expr_cast_t*)node)) {
            goto error;
        }
        break;
    }

    // if (node->type) {
    //     ecs_strbuf_append(v->buf, ")");
    // }

    return 0;
error:
    return -1;
}

FLECS_API
char* ecs_script_expr_to_str(
    const ecs_world_t *world,
    const ecs_expr_node_t *expr)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_expr_str_visitor_t v = { .world = world, .buf = &buf };
    if (flecs_expr_node_to_str(&v, expr)) {
        ecs_strbuf_reset(&buf);
        return NULL;
    }

    return ecs_strbuf_get(&buf);
}

#endif
