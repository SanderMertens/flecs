/**
 * @file addons/script/expr/visit_to_str.c
 * @brief Script expression AST to string visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

typedef struct ecs_expr_str_visitor_t {
    const ecs_script_t *script;
    const ecs_world_t *world;
    ecs_strbuf_t *buf;
    int32_t depth;
    bool failed;
    bool colors;
} ecs_expr_str_visitor_t;

static void flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node);

static void flecs_expr_color_to_str(
    ecs_expr_str_visitor_t *v,
    const char *color)
{
    if (v->colors) ecs_strbuf_appendstr(v->buf, color);
}

static void flecs_expr_value_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_value_node_t *node)
{
    flecs_expr_color_to_str(v, ECS_YELLOW);
    
    int ret = 0;
    ecs_entity_t type = node->node.type;
    if (ecs_has(v->world, type, EcsTypeSerializer)) {
        ret = ecs_ptr_to_str_buf(v->world, type, node->ptr, v->buf);
    } else {
        ecs_strbuf_appendstr(v->buf, "{}");
    }

    flecs_expr_color_to_str(v, ECS_NORMAL);
    v->failed |= ret != 0;
}

static void flecs_expr_interpolated_string_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_interpolated_string_t *node)
{
    int32_t i, count = ecs_vec_count(&node->fragments);
    ecs_expr_fragment_t *fragments = ecs_vec_first(&node->fragments);

    ecs_strbuf_appendlit(v->buf, "interpolated(");

    for (i = 0; i < count; i ++) {
        char *fragment = fragments[i].text;

        if (i) {
            ecs_strbuf_appendlit(v->buf, ", ");
        }

        if (fragment) {
            flecs_expr_color_to_str(v, ECS_YELLOW);
            ecs_strbuf_appendlit(v->buf, "\"");
            ecs_strbuf_appendstr(v->buf, fragment);
            ecs_strbuf_appendlit(v->buf, "\"");
            flecs_expr_color_to_str(v, ECS_NORMAL);
        }
        if (fragments[i].expr) {
            if (fragment) {
                ecs_strbuf_appendlit(v->buf, ", ");
            }
            flecs_expr_node_to_str(v, fragments[i].expr);
        }
    }

    ecs_strbuf_appendlit(v->buf, ")");
}

static void flecs_expr_initializer_to_str(
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

        if (elem->key) {
            flecs_expr_node_to_str(v, elem->key);
            ecs_strbuf_appendlit(v->buf, ":");
        }

        flecs_expr_node_to_str(v, elem->value);
    }

    ecs_strbuf_appendlit(v->buf, "}");
}

static void flecs_expr_identifier_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_identifier_t *node)
{
    ecs_strbuf_appendlit(v->buf, "@");
    ecs_strbuf_appendstr(v->buf, node->value);
}

static void flecs_expr_variable_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_variable_t *node)
{
    flecs_expr_color_to_str(v, ECS_GREEN);
    ecs_strbuf_appendlit(v->buf, "$");
    ecs_strbuf_appendstr(v->buf, node->name);
    flecs_expr_color_to_str(v, ECS_NORMAL);
}

static void flecs_expr_function_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_function_t *node)
{
    if (node->left) {
        flecs_expr_node_to_str(v, node->left);
        ecs_strbuf_appendlit(v->buf, ".");
    }

    ecs_strbuf_append(v->buf, "%s(", node->function_name);

    if (node->args) {
        flecs_expr_node_to_str(v, (ecs_expr_node_t*)node->args);
    }

    ecs_strbuf_append(v->buf, ")");
}

static void flecs_expr_has_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_has_t *node)
{
    flecs_expr_node_to_str(v, node->left);

    ecs_strbuf_appendlit(v->buf, "?[");

    if (node->second) {
        ecs_strbuf_appendlit(v->buf, "(");
        flecs_expr_node_to_str(v, node->first);
        ecs_strbuf_appendlit(v->buf, ", ");
        flecs_expr_node_to_str(v, node->second);
        ecs_strbuf_appendlit(v->buf, ")");
    } else {
        flecs_expr_node_to_str(v, node->first);
    }

    ecs_strbuf_appendlit(v->buf, "]");
}

static void flecs_expr_type_to_str(
    ecs_expr_str_visitor_t *v,
    ecs_entity_t type)
{
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
}

static void flecs_expr_match_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_match_t *node)
{
    if (node->node.type) {
        flecs_expr_type_to_str(v, node->node.type);
    }

    flecs_expr_color_to_str(v, ECS_BLUE);
    ecs_strbuf_appendlit(v->buf, "match ");
    flecs_expr_color_to_str(v, ECS_GREEN);
    flecs_expr_node_to_str(v, node->expr);

    ecs_strbuf_appendlit(v->buf, " {\n");

    int32_t i, count = ecs_vec_count(&node->elements);
    ecs_expr_match_element_t *elems = ecs_vec_first(&node->elements);

    for (i = 0; i < count; i ++) {
        ecs_strbuf_appendlit(v->buf, "  ");

        ecs_expr_match_element_t *elem = &elems[i];
        flecs_expr_node_to_str(v, elem->compare);

        ecs_strbuf_appendlit(v->buf, ": ");

        flecs_expr_node_to_str(v, elem->expr);

        ecs_strbuf_appendlit(v->buf, "\n");
    }

    ecs_strbuf_appendlit(v->buf, "}");

    if (node->node.type) {
        ecs_strbuf_appendlit(v->buf, ")");
    }

    ecs_strbuf_appendlit(v->buf, "\n");
}

static void flecs_expr_new_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_new_t *node)
{
    flecs_expr_color_to_str(v, ECS_BLUE);
    ecs_strbuf_appendlit(v->buf, "new ");
    flecs_expr_color_to_str(v, ECS_NORMAL);

    ecs_assert(node->entity != NULL, ECS_INTERNAL_ERROR, NULL);

    v->failed |= ecs_script_ast_node_to_buf(v->script,
        (ecs_script_node_t*)node->entity, v->buf, v->colors, v->depth + 1) != 0;
}

static void flecs_expr_script_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_script_t *node)
{
    flecs_expr_color_to_str(v, ECS_BLUE);
    ecs_strbuf_appendlit(v->buf, "script ");
    flecs_expr_color_to_str(v, ECS_NORMAL);
    ecs_strbuf_appendlit(v->buf, "{");
    ecs_strbuf_appendstr(v->buf, node->script->code);
    ecs_strbuf_appendlit(v->buf, "}");
}

static void flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);
    if (v->failed) {
        return;
    }
    const char *suffix = NULL;

    switch(node->kind) {
    case EcsExprValue:
        flecs_expr_value_to_str(v,
            (const ecs_expr_value_node_t*)node);
        break;
    case EcsExprInterpolatedString:
        flecs_expr_interpolated_string_to_str(v,
            (const ecs_expr_interpolated_string_t*)node);
        break;
    case EcsExprInitializer:
    case EcsExprEmptyInitializer:
        flecs_expr_initializer_to_str(v,
            (const ecs_expr_initializer_t*)node);
        break;
    case EcsExprUnary:
        ecs_strbuf_appendstr(v->buf, flecs_token_str(
            ((const ecs_expr_unary_t*)node)->operator));
        flecs_expr_node_to_str(v, ((const ecs_expr_unary_t*)node)->expr);
        break;
    case EcsExprBinary:
        ecs_strbuf_appendlit(v->buf, "(");
        flecs_expr_node_to_str(v, ((const ecs_expr_binary_t*)node)->left);
        ecs_strbuf_appendlit(v->buf, " ");
        ecs_strbuf_appendstr(v->buf, flecs_token_str(
            ((const ecs_expr_binary_t*)node)->operator));
        ecs_strbuf_appendlit(v->buf, " ");
        flecs_expr_node_to_str(v, ((const ecs_expr_binary_t*)node)->right);
        suffix = ")";
        break;
    case EcsExprIdentifier:
        flecs_expr_identifier_to_str(v,
            (const ecs_expr_identifier_t*)node);
        break;
    case EcsExprVariable:
    case EcsExprGlobalVariable:
        flecs_expr_variable_to_str(v,
            (const ecs_expr_variable_t*)node);
        break;
    case EcsExprFunction:
    case EcsExprMethod:
        flecs_expr_function_to_str(v,
            (const ecs_expr_function_t*)node);
        break;
    case EcsExprMember:
        flecs_expr_node_to_str(v, ((const ecs_expr_member_t*)node)->left);
        ecs_strbuf_appendlit(v->buf, ".");
        ecs_strbuf_appendstr(v->buf,
            ((const ecs_expr_member_t*)node)->member_name);
        break;
    case EcsExprSwizzle:
        flecs_expr_node_to_str(v, ((const ecs_expr_swizzle_t*)node)->left);
        ecs_strbuf_appendlit(v->buf, ".");
        ecs_strbuf_appendstr(v->buf,
            ((const ecs_expr_swizzle_t*)node)->name);
        break;
    case EcsExprElement:
    case EcsExprComponent:
        flecs_expr_node_to_str(v, ((const ecs_expr_element_t*)node)->left);
        ecs_strbuf_appendlit(v->buf, "[");
        flecs_expr_node_to_str(v, ((const ecs_expr_element_t*)node)->index);
        suffix = "]";
        break;
    case EcsExprHas:
        flecs_expr_has_to_str(v,
            (const ecs_expr_has_t*)node);
        break;
    case EcsExprMatch:
        flecs_expr_match_to_str(v,
            (const ecs_expr_match_t*)node);
        break;
    case EcsExprRange:
        ecs_strbuf_appendlit(v->buf, "[");
        flecs_expr_node_to_str(v, ((const ecs_expr_range_t*)node)->from);
        ecs_strbuf_appendlit(v->buf, " .. ");
        flecs_expr_node_to_str(v, ((const ecs_expr_range_t*)node)->to);
        suffix = "]";
        break;
    case EcsExprNew:
        flecs_expr_new_to_str(v,
            (const ecs_expr_new_t*)node);
        break;
    case EcsExprScript:
        flecs_expr_script_to_str(v,
            (const ecs_expr_script_t*)node);
        break;
    case EcsExprCast:
    case EcsExprCastNumber:
        flecs_expr_type_to_str(v, node->type);
        flecs_expr_node_to_str(v, ((const ecs_expr_cast_t*)node)->expr);
        suffix = ")";
        break;
    default:
        ecs_abort(ECS_INTERNAL_ERROR, "invalid node kind");
    }
    if (suffix) {
        ecs_strbuf_appendstr(v->buf, suffix);
    }
}

void flecs_expr_to_str_buf(
    const ecs_script_t *script,
    const ecs_expr_node_t *expr,
    ecs_strbuf_t *buf,
    bool colors)
{
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_world_t *world = script->world;
    ecs_expr_str_visitor_t v = { 
        .script = script, 
        .world = world, 
        .buf = buf, 
        .colors = colors 
    };

    flecs_expr_node_to_str(&v, expr);
    if (v.failed) {
        ecs_strbuf_reset(buf);
    }
}

#endif
