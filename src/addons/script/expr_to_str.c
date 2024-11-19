/**
 * @file addons/script/expr_to_str.c
 * @brief Script expression AST to string visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

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
        v->world, node->value.type, node->value.ptr, v->buf);
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

    switch(node->operator) {
    case EcsTokAdd: ecs_strbuf_appendlit(v->buf, "+"); break;
    case EcsTokSub: ecs_strbuf_appendlit(v->buf, "-"); break;
    case EcsTokMul: ecs_strbuf_appendlit(v->buf, "*"); break;
    case EcsTokDiv: ecs_strbuf_appendlit(v->buf, "/"); break;
    case EcsTokMod: ecs_strbuf_appendlit(v->buf, "%%"); break;
    case EcsTokBitwiseOr: ecs_strbuf_appendlit(v->buf, "|"); break;
    case EcsTokBitwiseAnd: ecs_strbuf_appendlit(v->buf, "&"); break;
    case EcsTokEq: ecs_strbuf_appendlit(v->buf, "=="); break;
    case EcsTokNeq: ecs_strbuf_appendlit(v->buf, "!="); break;
    case EcsTokGt: ecs_strbuf_appendlit(v->buf, ">"); break;
    case EcsTokGtEq: ecs_strbuf_appendlit(v->buf, ">="); break;
    case EcsTokLt: ecs_strbuf_appendlit(v->buf, "<"); break;
    case EcsTokLtEq: ecs_strbuf_appendlit(v->buf, "<="); break;
    case EcsTokAnd: ecs_strbuf_appendlit(v->buf, "&&"); break;
    case EcsTokOr: ecs_strbuf_appendlit(v->buf, "||"); break;
    case EcsTokShiftLeft: ecs_strbuf_appendlit(v->buf, "<<"); break;
    case EcsTokShiftRight: ecs_strbuf_appendlit(v->buf, ">>"); break;
    default:
        ecs_err("invalid operator in expression");
        return -1;
    };

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

int flecs_expr_node_to_str(
    ecs_expr_str_visitor_t *v,
    const ecs_expr_node_t *node)
{
    ecs_assert(node != NULL, ECS_INVALID_PARAMETER, NULL);

    switch(node->kind) {
    case EcsExprValue:
        if (flecs_expr_value_to_str(v, (ecs_expr_val_t*)node)) {
            goto error;
        }
        break;
    case EcsExprUnary:
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
        break;
    }

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
