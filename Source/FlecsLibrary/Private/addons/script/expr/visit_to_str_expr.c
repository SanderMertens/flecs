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
    bool colors;
} ecs_expr_str_visitor_t;

static
int flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node);

static
void flecs_expr_color_to_str(
    ecs_expr_str_visitor_t *v,
    const char *color)
{
    if (v->colors) ecs_strbuf_appendstr(v->buf, color);
}

static
int flecs_expr_value_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_value_node_t *node)
{
    flecs_expr_color_to_str(v, ECS_YELLOW);
    int ret = ecs_ptr_to_str_buf(
        v->world, node->node.type, node->ptr, v->buf);
    flecs_expr_color_to_str(v, ECS_NORMAL);
    return ret;
}

static
int flecs_expr_interpolated_string_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_interpolated_string_t *node)
{
    int32_t i, e = 0, count = ecs_vec_count(&node->fragments);
    char **fragments = ecs_vec_first(&node->fragments);
    ecs_expr_node_t **expressions = ecs_vec_first(&node->expressions);

    ecs_strbuf_appendlit(v->buf, "interpolated(");

    for (i = 0; i < count; i ++) {
        char *fragment = fragments[i];

        if (i) {
            ecs_strbuf_appendlit(v->buf, ", ");
        }

        if (fragment) {
            flecs_expr_color_to_str(v, ECS_YELLOW);
            ecs_strbuf_appendlit(v->buf, "\"");
            ecs_strbuf_appendstr(v->buf, fragment);
            ecs_strbuf_appendlit(v->buf, "\"");
            flecs_expr_color_to_str(v, ECS_NORMAL);
        } else {
            ecs_expr_node_t *expr = expressions[e ++];
            if (flecs_expr_node_to_str(v, expr)) {
                return -1;
            }
        }
    }

    ecs_strbuf_appendlit(v->buf, ")");
    
    return 0;
}

static
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

static
int flecs_expr_initializer_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_initializer_t *node)
{
    ecs_strbuf_appendlit(v->buf, "{");

    ecs_expr_initializer_element_t *elems = ecs_vec_first(&node->elements);
    int32_t i, count = ecs_vec_count(&node->elements);
    for (i = 0; i < count; i ++) {
        if (i) {
            ecs_strbuf_appendstr(v->buf, ", ");
        }

        ecs_expr_initializer_element_t *elem = &elems[i];
        if (elem->member) {
            ecs_strbuf_appendstr(v->buf, elem->member);
            ecs_strbuf_appendlit(v->buf, ":");
        }

        if (flecs_expr_node_to_str(v, elem->value)) {
            goto error;
        }
    }

    ecs_strbuf_appendlit(v->buf, "}");

    return 0;
error:
    return -1;
}

static
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

static
int flecs_expr_identifier_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_identifier_t *node)
{
    ecs_strbuf_appendlit(v->buf, "@");
    ecs_strbuf_appendstr(v->buf, node->value);
    return 0;
}

static
int flecs_expr_variable_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_variable_t *node)
{
    flecs_expr_color_to_str(v, ECS_GREEN);
    ecs_strbuf_appendlit(v->buf, "$");
    ecs_strbuf_appendstr(v->buf, node->name);
    flecs_expr_color_to_str(v, ECS_NORMAL);
    return 0;
}

static
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

static
int flecs_expr_function_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_function_t *node)
{
    if (node->left) {
        if (flecs_expr_node_to_str(v, node->left)) {
            return -1;
        }
        ecs_strbuf_appendlit(v->buf, ".");
    }

    ecs_strbuf_append(v->buf, "%s(", node->function_name);

    if (node->args) {
        if (flecs_expr_node_to_str(v, (ecs_expr_node_t*)node->args)) {
            return -1;
        }
    }

    ecs_strbuf_append(v->buf, ")");
    return 0;
}

static
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

static
int flecs_expr_match_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_match_t *node)
{
    if (node->node.type) {
        flecs_expr_color_to_str(v, ECS_BLUE);
        const char *name = ecs_get_name(v->world, node->node.type);
        if (name) {
            ecs_strbuf_appendstr(v->buf, name);
        } else {
            char *path = ecs_get_path(v->world, node->node.type);
            ecs_strbuf_appendstr(v->buf, path);
            ecs_os_free(path);
        }
        flecs_expr_color_to_str(v, ECS_NORMAL);
        ecs_strbuf_appendlit(v->buf, "(");
    }

    flecs_expr_color_to_str(v, ECS_BLUE);
    ecs_strbuf_appendlit(v->buf, "match ");
    flecs_expr_color_to_str(v, ECS_GREEN);
    if (flecs_expr_node_to_str(v, node->expr)) {
        return -1;
    }

    ecs_strbuf_appendlit(v->buf, " {\n");

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    for (i = 0; i < count; i ++) {
        ecs_strbuf_appendlit(v->buf, "  ");

        ecs_expr_match_element_t *elem = &elems[i];
        if (flecs_expr_node_to_str(v, elem->compare)) {
            return -1;
        }

        ecs_strbuf_appendlit(v->buf, ": ");

        if (flecs_expr_node_to_str(v, elem->expr)) {
            return -1;
        }

        ecs_strbuf_appendlit(v->buf, "\n");
    }

    ecs_strbuf_appendlit(v->buf, "}");

    if (node->node.type) {
        ecs_strbuf_appendlit(v->buf, ")");
    }

    ecs_strbuf_appendlit(v->buf, "\n");

    return 0;
}

static
int flecs_expr_cast_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_cast_t *node)
{
    ecs_entity_t type = node->node.type;
    
    flecs_expr_color_to_str(v, ECS_BLUE);
    const char *name = ecs_get_name(v->world, type);
    if (name) {
        ecs_strbuf_appendstr(v->buf, name);
    } else {
        char *path = ecs_get_path(v->world, type);
        ecs_strbuf_appendstr(v->buf, path);
        ecs_os_free(path);
    }
    flecs_expr_color_to_str(v, ECS_NORMAL);
    ecs_strbuf_appendlit(v->buf, "(");

    if (flecs_expr_node_to_str(v, node->expr)) {
        return -1;
    }

    ecs_strbuf_append(v->buf, ")");

    return 0;
}

static
int flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        if (flecs_expr_value_to_str(v, 
            (const ecs_expr_value_node_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprInterpolatedString:
        if (flecs_expr_interpolated_string_to_str(v, 
            (const ecs_expr_interpolated_string_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        if (flecs_expr_initializer_to_str(v, 
            (const ecs_expr_initializer_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprUnary:
        if (flecs_expr_unary_to_str(v, 
            (const ecs_expr_unary_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprBinary:
        if (flecs_expr_binary_to_str(v, 
            (const ecs_expr_binary_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprIdentifier:
        if (flecs_expr_identifier_to_str(v, 
            (const ecs_expr_identifier_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprVariable:
    case EcsExprGlobalVariable:
        if (flecs_expr_variable_to_str(v, 
            (const ecs_expr_variable_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprFunction:
    case EcsExprMethod:
        if (flecs_expr_function_to_str(v, 
            (const ecs_expr_function_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprMember:
        if (flecs_expr_member_to_str(v, 
            (const ecs_expr_member_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprElement:
    case EcsExprComponent:
        if (flecs_expr_element_to_str(v, 
            (const ecs_expr_element_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprMatch:
        if (flecs_expr_match_to_str(v, 
            (const ecs_expr_match_t*)node)) 
        {
            goto error;
        }
        break;
    case EcsExprCast:
    case EcsExprCastNumber:
        if (flecs_expr_cast_to_str(v, 
            (const ecs_expr_cast_t*)node)) 
        {
            goto error;
        }
        break;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid node kind");
    }

    return 0;
error:
    return -1;
}

void flecs_expr_to_str_buf(
    const ecs_world_t *world,
    const ecs_expr_node_t *expr,
    ecs_strbuf_t *buf,
    bool colors)
{
    ecs_expr_str_visitor_t v = { .world = world, .buf = buf, .colors = colors };
    if (flecs_expr_node_to_str(&v, expr)) {
        ecs_strbuf_reset(buf);
    }
}

#endif
