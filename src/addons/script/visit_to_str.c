/**
 * @file addons/script/visit_to_str.c
 * @brief Script AST to string visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

typedef struct ecs_script_str_visitor_t {
    ecs_script_visit_t base;
    ecs_strbuf_t *buf;
    int32_t depth;
    bool newline;
} ecs_script_str_visitor_t;

static
int flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope);

static
void flecs_scriptbuf_append(
    ecs_script_str_visitor_t *v,
    const char *fmt,
    ...)
{
    if (v->newline) {
        ecs_strbuf_append(v->buf, "%*s", v->depth * 2, "");
        v->newline = false;
    }

    va_list args;
    va_start(args, fmt);
    ecs_strbuf_vappend(v->buf, fmt, args);
    va_end(args);

    if (fmt[strlen(fmt) - 1] == '\n') {
        v->newline = true;
    }
}

static
void flecs_scriptbuf_appendstr(
    ecs_script_str_visitor_t *v,
    const char *str)
{
    if (v->newline) {
        ecs_strbuf_append(v->buf, "%*s", v->depth * 2, "");
        v->newline = false;
    }

    ecs_strbuf_appendstr(v->buf, str);

    if (str[strlen(str) - 1] == '\n') {
        v->newline = true;
    }
}

static
void flecs_script_id_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_id_t *id)
{
    if (id->flag) {
        if (id->flag == ECS_AUTO_OVERRIDE) {
            flecs_scriptbuf_appendstr(v, "auto_override | ");
        } else {
            flecs_scriptbuf_appendstr(v, "??? | ");
        }
    }

    if (id->second) {
        flecs_scriptbuf_append(v, "(%s, %s)", 
            id->first, id->second);
    } else {
        flecs_scriptbuf_appendstr(v, id->first);
    }
}

static
void flecs_script_expr_to_str(
    ecs_script_str_visitor_t *v,
    const char *expr)
{
    if (expr) {
        flecs_scriptbuf_append(v, "%s%s%s", ECS_GREEN, expr, ECS_NORMAL);
    } else {
        flecs_scriptbuf_appendstr(v, "{}");
    }
}

static
const char* flecs_script_node_to_str(
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:              return "scope";
    case EcsAstWithTag:
    case EcsAstTag:                return "tag";
    case EcsAstWithComponent:
    case EcsAstComponent:          return "component";
    case EcsAstWithVar:
    case EcsAstVarComponent:       return "var";
    case EcsAstDefaultComponent:   return "default_component";
    case EcsAstWith:               return "with";
    case EcsAstUsing:              return "using";
    case EcsAstModule:             return "module";
    case EcsAstAnnotation:         return "annot";
    case EcsAstTemplate:           return "template";
    case EcsAstProp:               return "prop";
    case EcsAstConst:              return "const";
    case EcsAstEntity:             return "entity";
    case EcsAstPairScope:          return "pair_scope";
    case EcsAstIf:                 return "if";
    }
    return "???";
}

static
void flecs_scriptbuf_node(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    flecs_scriptbuf_append(v, "%s%s%s: ", 
        ECS_BLUE, flecs_script_node_to_str(node), ECS_NORMAL);
}

static
void flecs_script_tag_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_tag_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_script_id_to_str(v, &node->id);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_component_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_script_id_to_str(v, &node->id);
    if (node->expr) {
        flecs_scriptbuf_appendstr(v, ": ");
        flecs_script_expr_to_str(v, node->expr);
    }
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_default_component_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_default_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    if (node->expr) {
        flecs_script_expr_to_str(v, node->expr);
    }
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_with_var_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_var_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->name);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_with_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_with_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    
    flecs_scriptbuf_appendstr(v, "{\n");
    v->depth ++;
    flecs_scriptbuf_append(v, "%sexpressions%s: ", ECS_CYAN, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->expressions);
    flecs_scriptbuf_append(v, "%sscope%s: ", ECS_CYAN, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->scope);
    v->depth --;
    flecs_scriptbuf_appendstr(v, "}\n");
}

static
void flecs_script_using_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_using_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s\n", node->name);
}

static
void flecs_script_module_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_module_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s\n", node->name);
}

static
void flecs_script_annot_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_annot_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s = %s\"%s\"%s", node->name, 
        ECS_GREEN, node->expr, ECS_NORMAL);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_template_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_template_node_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->name);
    flecs_script_scope_to_str(v, node->scope);
}

static
void flecs_script_var_node_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_var_node_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    if (node->type) {
        flecs_scriptbuf_append(v, "%s : %s = ", 
            node->name,
            node->type);
    } else {
        flecs_scriptbuf_append(v, "%s = ", 
            node->name);
    }
    flecs_script_expr_to_str(v, node->expr);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_entity_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_entity_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    if (node->kind) {
        flecs_scriptbuf_append(v, "%s ", node->kind);
    }
    if (node->name) {
        flecs_scriptbuf_append(v, "%s ", node->name);
    } else {
        flecs_scriptbuf_appendstr(v, "<anon> ");
    }

    if (!flecs_scope_is_empty(node->scope)) {
        flecs_script_scope_to_str(v, node->scope);
    } else {
        flecs_scriptbuf_appendstr(v, "\n");
    }
}

static
void flecs_script_pair_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_pair_scope_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_script_id_to_str(v, &node->id);
    flecs_scriptbuf_appendstr(v, " ");
    flecs_script_scope_to_str(v, node->scope);
}

static
void flecs_script_if_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_if_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_script_expr_to_str(v, node->expr);

    flecs_scriptbuf_appendstr(v, " {\n");
    v->depth ++;
    flecs_scriptbuf_append(v, "%strue%s: ", ECS_CYAN, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->if_true);
    flecs_scriptbuf_append(v, "%sfalse%s: ", ECS_CYAN, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->if_false);
    v->depth --;
    flecs_scriptbuf_appendstr(v, "}\n");
}

static
int flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope)
{
    if (!ecs_vec_count(&scope->stmts)) {
        flecs_scriptbuf_appendstr(v, "{}\n");
        return 0;
    }

    flecs_scriptbuf_appendstr(v, "{\n");

    v->depth ++;

    if (ecs_script_visit_scope(v, scope)) {
        return -1;
    }

    v->depth --;

    flecs_scriptbuf_appendstr(v, "}\n");

    return 0;
}

static
int flecs_script_stmt_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        if (flecs_script_scope_to_str(v, (ecs_script_scope_t*)node)) {
            return -1;
        }
        break;
    case EcsAstTag:
    case EcsAstWithTag:
        flecs_script_tag_to_str(v, (ecs_script_tag_t*)node);
        break;
    case EcsAstComponent:
    case EcsAstWithComponent:
        flecs_script_component_to_str(v, (ecs_script_component_t*)node);
        break;
    case EcsAstVarComponent:
    case EcsAstWithVar:
        flecs_script_with_var_to_str(v, 
            (ecs_script_var_component_t*)node);
        break;
    case EcsAstDefaultComponent:
        flecs_script_default_component_to_str(v, 
            (ecs_script_default_component_t*)node);
        break;
    case EcsAstWith:
        flecs_script_with_to_str(v, (ecs_script_with_t*)node);
        break;
    case EcsAstUsing:
        flecs_script_using_to_str(v, (ecs_script_using_t*)node);
        break;
    case EcsAstModule:
        flecs_script_module_to_str(v, (ecs_script_module_t*)node);
        break;
    case EcsAstAnnotation:
        flecs_script_annot_to_str(v, (ecs_script_annot_t*)node);
        break;
    case EcsAstTemplate:
        flecs_script_template_to_str(v, (ecs_script_template_node_t*)node);
        break;
    case EcsAstConst:
    case EcsAstProp:
        flecs_script_var_node_to_str(v, (ecs_script_var_node_t*)node);
        break;
    case EcsAstEntity:
        flecs_script_entity_to_str(v, (ecs_script_entity_t*)node);
        break;
    case EcsAstPairScope:
        flecs_script_pair_scope_to_str(v, (ecs_script_pair_scope_t*)node);
        break;
    case EcsAstIf:
        flecs_script_if_to_str(v, (ecs_script_if_t*)node);
        break;
    }

    return 0;
}

int ecs_script_ast_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(buf != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_str_visitor_t v = { .buf = buf };
    if (ecs_script_visit(flecs_script_impl(script), &v, flecs_script_stmt_to_str)) {
        goto error;
    }

    return 0;
error:
    ecs_strbuf_reset(buf);
    return - 1;
}

char* ecs_script_ast_to_str(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    if (ecs_script_ast_to_buf(script, &buf)) {
        goto error;
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

#endif
