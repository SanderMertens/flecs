#include "flecs.h"
#include "script.h"

typedef struct ecs_script_str_visitor_t {
    ecs_script_t *script;
    ecs_strbuf_t *buf;
    int32_t depth;
    bool newline;
} ecs_script_str_visitor_t;

static
void flecs_script_scope_to_str(
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
    flecs_scriptbuf_append(v, "{%s%s%s}", ECS_MAGENTA, expr, ECS_NORMAL);
}

static
char* flecs_script_node_to_str(
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:              return "scope";
    case EcsAstTag:                return "tag";
    case EcsAstComponent:          return "component";
    case EcsAstDefaultComponent:   return "default_component";
    case EcsAstVariableComponent:  return "var_component";
    case EcsAstWith:               return "with";
    case EcsAstUsing:              return "using";
    case EcsAstAnnotation:         return "annot";
    case EcsAstAssembly:           return "assembly";
    case EcsAstProp:               return "prop";
    case EcsAstConst:              return "const";
    case EcsAstEntity:             return "entity";
    case EcsAstPairScope:          return "pair_scope";
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
        flecs_scriptbuf_appendstr(v, " = ");
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
void flecs_script_var_component_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_var_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->var_name);
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
void flecs_script_annot_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_annot_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s = ", node->name);
    flecs_script_expr_to_str(v, node->expr);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_assembly_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_assembly_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->name);
    flecs_script_scope_to_str(v, node->scope);
}

static
void flecs_script_prop_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_prop_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s : %s = ", 
        node->name,
        node->type,
        node->expr);
    flecs_script_expr_to_str(v, node->expr);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_const_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_const_t *node)
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
    if (node->name) {
        flecs_scriptbuf_append(v, "%s ", node->name);
    } else {
        flecs_scriptbuf_appendstr(v, "<anon> ");
    }
    flecs_script_scope_to_str(v, node->scope);
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
void flecs_script_stmt_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_scope_to_str(v, (ecs_script_scope_t*)node);
        break;
    case EcsAstTag:
        flecs_script_tag_to_str(v, (ecs_script_tag_t*)node);
        break;
    case EcsAstComponent:
        flecs_script_component_to_str(v, (ecs_script_component_t*)node);
        break;
    case EcsAstDefaultComponent:
        flecs_script_default_component_to_str(v, 
            (ecs_script_default_component_t*)node);
        break;
    case EcsAstVariableComponent:
        flecs_script_var_component_to_str(v, 
            (ecs_script_var_component_t*)node);
        break;
    case EcsAstWith:
        flecs_script_with_to_str(v, (ecs_script_with_t*)node);
        break;
    case EcsAstUsing:
        flecs_script_using_to_str(v, (ecs_script_using_t*)node);
        break;
    case EcsAstAnnotation:
        flecs_script_annot_to_str(v, (ecs_script_annot_t*)node);
        break;
    case EcsAstAssembly:
        flecs_script_assembly_to_str(v, (ecs_script_assembly_t*)node);
        break;
    case EcsAstProp:
        flecs_script_prop_to_str(v, (ecs_script_prop_t*)node);
        break;
    case EcsAstConst:
        flecs_script_const_to_str(v, (ecs_script_const_t*)node);
        break;
    case EcsAstEntity:
        flecs_script_entity_to_str(v, (ecs_script_entity_t*)node);
        break;
    case EcsAstPairScope:
        flecs_script_pair_scope_to_str(v, (ecs_script_pair_scope_t*)node);
        break;
    }
}

static
void flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_script_node_t **nodes = ecs_vec_first_t(
        &scope->stmts, ecs_script_node_t*);

    int32_t i, count = ecs_vec_count(&scope->stmts);

    if (!count) {
        flecs_scriptbuf_appendstr(v, "{}\n");
        return;
    }

    flecs_scriptbuf_appendstr(v, "{\n");

    v->depth ++;

    for (i = 0; i < count; i ++) {
        flecs_script_stmt_to_str(v, nodes[i]);
    }

    v->depth --;

    flecs_scriptbuf_appendstr(v, "}\n");
}

void ecs_script_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(buf != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_str_visitor_t v = { .script = script, .buf = buf };
    flecs_script_scope_to_str(&v, script->root);
error:
    return;
}

char* ecs_script_to_str(
    ecs_script_t *script)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_script_to_buf(script, &buf);
    return ecs_strbuf_get(&buf);
error:
    return NULL;
}
