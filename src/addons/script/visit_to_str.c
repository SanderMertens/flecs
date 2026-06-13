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
    bool colors;
    bool annotations;
    bool in_template;
    int32_t prop_count;
} ecs_script_str_visitor_t;

static
int flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope);

static
void flecs_script_color_to_str(
    ecs_script_str_visitor_t *v,
    const char *color)
{
    if (v->colors) ecs_strbuf_appendstr(v->buf, color);
}

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
void flecs_script_annot_begin(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    if (!v->annotations) {
        return;
    }

    flecs_script_color_to_str(v, ECS_GREY);

    int32_t pos = -1;
    const char *code = v->base.script->pub.code;
    if (node->pos && code && (node->pos >= code)) {
        pos = flecs_ito(int32_t, node->pos - code);
    }

    flecs_scriptbuf_append(v, " @{pos=%d", pos);

    if (node->depends_on) {
        flecs_scriptbuf_append(v, " deps=0x%llx",
            (unsigned long long)node->depends_on);
    }

    if (node->contains_entities) {
        flecs_scriptbuf_appendstr(v, " entities");
    }
}

static
void flecs_script_annot_end(
    ecs_script_str_visitor_t *v)
{
    if (!v->annotations) {
        return;
    }

    flecs_scriptbuf_appendstr(v, "}");
    flecs_script_color_to_str(v, ECS_NORMAL);
}

static
void flecs_script_annot_id(
    ecs_script_str_visitor_t *v,
    ecs_script_id_t *id)
{
    if (!v->annotations) {
        return;
    }

    if (id->id_slot) {
        flecs_scriptbuf_append(v, " id_slot=%d", id->id_slot);
    }
    if (id->target_slot) {
        flecs_scriptbuf_append(v, " target_slot=%d", id->target_slot);
    }
    if (id->dynamic) {
        flecs_scriptbuf_appendstr(v, " dynamic");
    }
}

static
void flecs_script_annot_scope(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope)
{
    if (!v->annotations) {
        return;
    }

    if ((scope->slot_start == scope->slot_end) &&
        (scope->dyn_start == scope->dyn_end) &&
        (scope->control_start == scope->control_end))
    {
        return;
    }

    flecs_script_color_to_str(v, ECS_GREY);
    flecs_scriptbuf_appendstr(v, " @{");

    bool sep = false;
    if (scope->slot_start != scope->slot_end) {
        flecs_scriptbuf_append(v, "slots=%d..%d",
            scope->slot_start, scope->slot_end);
        sep = true;
    }
    if (scope->dyn_start != scope->dyn_end) {
        flecs_scriptbuf_append(v, "%sdyn=%d..%d", sep ? " " : "",
            scope->dyn_start, scope->dyn_end);
        sep = true;
    }
    if (scope->control_start != scope->control_end) {
        flecs_scriptbuf_append(v, "%sctrl=%d..%d", sep ? " " : "",
            scope->control_start, scope->control_end);
    }

    flecs_scriptbuf_appendstr(v, "}");
    flecs_script_color_to_str(v, ECS_NORMAL);
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
void flecs_expr_to_str(
    ecs_script_str_visitor_t *v,
    const ecs_expr_node_t *expr)
{
    if (expr) {
        flecs_expr_to_str_buf(
            &v->base.script->pub, expr, v->buf, v->colors);
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
    case EcsAstExportConst:        return "export const";
    case EcsAstEntity:             return "entity";
    case EcsAstPairScope:          return "pair_scope";
    case EcsAstIf:                 return "if";
    case EcsAstFor:                return "for";
    case EcsAstInclude:            return "include";
    case EcsAstFunction:           return "fn";
    }
    return "???";
}

static
void flecs_scriptbuf_node(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    flecs_script_color_to_str(v, ECS_BLUE);
    flecs_scriptbuf_append(v, "%s: ", flecs_script_node_to_str(node));
    flecs_script_color_to_str(v, ECS_NORMAL);
}

static
void flecs_script_tag_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_tag_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_script_id_to_str(v, &node->id);
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_id(v, &node->id);
    flecs_script_annot_end(v);
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
        flecs_expr_to_str(v, node->expr);
    }
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_id(v, &node->id);
    flecs_script_annot_end(v);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_default_component_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_default_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    if (node->expr) {
        flecs_expr_to_str(v, node->expr);
    }
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_end(v);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_with_var_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_var_component_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->name);
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_end(v);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_with_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_with_t *node)
{
    flecs_scriptbuf_node(v, &node->node);

    flecs_script_annot_begin(v, &node->node);
    if (v->annotations && node->expr_depends_on) {
        flecs_scriptbuf_append(v, " expr_deps=0x%llx",
            (unsigned long long)node->expr_depends_on);
    }
    flecs_script_annot_end(v);

    flecs_scriptbuf_appendstr(v, "{\n");
    v->depth ++;
    flecs_script_color_to_str(v, ECS_CYAN);
    flecs_scriptbuf_appendstr(v, "expressions: ");
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->expressions);
    flecs_script_color_to_str(v, ECS_CYAN);
    flecs_scriptbuf_append(v, "scope: ");
    flecs_script_color_to_str(v, ECS_NORMAL);
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
    flecs_script_color_to_str(v, ECS_GREEN);
    flecs_scriptbuf_append(v, "%s = \"%s\"", node->name, node->expr);
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_scriptbuf_appendstr(v, "\n");
}

static
void flecs_script_template_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_template_node_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s ", node->name);
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_end(v);
    if (v->annotations) {
        flecs_scriptbuf_appendstr(v, " ");
    }
    bool old_in_template = v->in_template;
    int32_t old_prop_count = v->prop_count;
    v->in_template = true;
    v->prop_count = 0;
    flecs_script_scope_to_str(v, node->scope);
    v->in_template = old_in_template;
    v->prop_count = old_prop_count;
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
    flecs_expr_to_str(v, node->expr);
    flecs_script_annot_begin(v, &node->node);
    if (v->annotations && v->in_template && node->node.kind == EcsAstProp) {
        flecs_scriptbuf_append(v, " bit=%d", v->prop_count ++);
    }
    flecs_script_annot_end(v);
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

    flecs_script_annot_begin(v, &node->node);
    if (v->annotations) {
        if (node->name_depends_on) {
            flecs_scriptbuf_append(v, " name_deps=0x%llx",
                (unsigned long long)node->name_depends_on);
        }
        if (node->slot) {
            flecs_scriptbuf_append(v, " slot=%d", node->slot);
        }
        if (node->dyn_slot) {
            flecs_scriptbuf_append(v, " dyn_slot=%d", node->dyn_slot);
        }
    }
    flecs_script_annot_end(v);

    if (!flecs_scope_is_empty(node->scope)) {
        if (v->annotations) {
            flecs_scriptbuf_appendstr(v, " ");
        }
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
    flecs_script_annot_begin(v, &node->node);
    flecs_script_annot_id(v, &node->id);
    if (v->annotations && node->expr_depends_on) {
        flecs_scriptbuf_append(v, " expr_deps=0x%llx",
            (unsigned long long)node->expr_depends_on);
    }
    flecs_script_annot_end(v);
    flecs_scriptbuf_appendstr(v, " ");
    flecs_script_scope_to_str(v, node->scope);
}

static
void flecs_script_if_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_if_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_expr_to_str(v, node->expr);

    flecs_script_annot_begin(v, &node->node);
    if (v->annotations) {
        if (node->expr_depends_on) {
            flecs_scriptbuf_append(v, " expr_deps=0x%llx",
                (unsigned long long)node->expr_depends_on);
        }
        if (node->control_slot) {
            flecs_scriptbuf_append(v, " control=%d", node->control_slot);
        }
    }
    flecs_script_annot_end(v);

    flecs_scriptbuf_appendstr(v, " {\n");
    v->depth ++;
    flecs_script_color_to_str(v, ECS_CYAN);
    flecs_scriptbuf_appendstr(v, "true: ");
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->if_true);
    flecs_script_color_to_str(v, ECS_CYAN);
    flecs_scriptbuf_appendstr(v, "false: ");
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_script_scope_to_str(v, node->if_false);
    v->depth --;
    flecs_scriptbuf_appendstr(v, "}\n");
}

static
void flecs_script_include_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_include_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_append(v, "%s\n", node->filename);
}

static
void flecs_script_for_range_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_for_range_t *node)
{
    flecs_scriptbuf_node(v, &node->node);
    flecs_scriptbuf_appendstr(v, node->loop_var);
    flecs_script_color_to_str(v, ECS_BLUE);
    flecs_scriptbuf_appendstr(v, " in ");
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_expr_to_str(v, node->from);
    flecs_scriptbuf_appendstr(v, " .. ");
    flecs_expr_to_str(v, node->to);

    flecs_script_annot_begin(v, &node->node);
    if (v->annotations && node->expr_depends_on) {
        flecs_scriptbuf_append(v, " expr_deps=0x%llx",
            (unsigned long long)node->expr_depends_on);
    }
    flecs_script_annot_end(v);

    flecs_scriptbuf_appendstr(v, " {\n");
    v->depth ++;
    flecs_script_scope_to_str(v, node->scope);
    v->depth --;
    flecs_scriptbuf_appendstr(v, "}\n");
}

static
int flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_vec_count(&scope->stmts)) {
        flecs_scriptbuf_appendstr(v, "{}\n");
        return 0;
    }

    flecs_scriptbuf_appendstr(v, "{");
    flecs_script_annot_scope(v, scope);
    flecs_scriptbuf_appendstr(v, "\n");

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
    ecs_script_visit_t *_v,
    ecs_script_node_t *node)
{
    ecs_script_str_visitor_t *v = (ecs_script_str_visitor_t*)_v;
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
    case EcsAstExportConst:
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
    case EcsAstFor:
        flecs_script_for_range_to_str(v, (ecs_script_for_range_t*)node);
        break;
    case EcsAstInclude:
        flecs_script_include_to_str(v, (ecs_script_include_t*)node);
        break;
    case EcsAstFunction: {
        ecs_script_function_node_t *fn = (ecs_script_function_node_t*)node;
        flecs_scriptbuf_node(v, &fn->node);
        flecs_scriptbuf_append(v, "%s(", fn->name);
        int32_t i, count = ecs_vec_count(&fn->params);
        ecs_script_fn_param_t *params = ecs_vec_first(&fn->params);
        for (i = 0; i < count; i ++) {
            if (i) {
                flecs_scriptbuf_appendstr(v, ", ");
            }
            flecs_scriptbuf_append(v, "%s: %s",
                params[i].name, params[i].type);
        }
        flecs_scriptbuf_append(v, ") -> %s ", fn->return_type);
        flecs_script_scope_to_str(v, fn->body);
        if (fn->return_expr) {
            flecs_expr_to_str(v, fn->return_expr);
        }
        flecs_scriptbuf_appendstr(v, "\n");
        break;
    }
    }

    return 0;
}

int ecs_script_ast_node_to_buf(
    const ecs_script_t *script,
    ecs_script_node_t *node,
    ecs_strbuf_t *buf,
    bool colors,
    int32_t depth)
{
    ecs_script_str_visitor_t v = { 
        .buf = buf, .colors = colors, .depth = depth 
    };

    /* Safe, string visitor doesn't modify script */
    if (ecs_script_visit_from(
        (ecs_script_impl_t*)ECS_CONST_CAST(ecs_script_t*, script), 
        &v, flecs_script_stmt_to_str, node, depth)) 
    {
        goto error;
    }

    return 0;
error:
    ecs_strbuf_reset(buf);
    return -1;
}

int ecs_script_ast_to_buf_w_desc(
    ecs_script_t *script,
    ecs_strbuf_t *buf,
    const ecs_script_ast_to_str_desc_t *desc)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(buf != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_script_str_visitor_t v = { .buf = buf };
    if (desc) {
        v.colors = desc->colors;
        v.annotations = desc->annotations;
    }

    if (ecs_script_visit(flecs_script_impl(script), &v, flecs_script_stmt_to_str)) {
        goto error;
    }

    return 0;
error:
    ecs_strbuf_reset(buf);
    return -1;
}

char* ecs_script_ast_to_str_w_desc(
    ecs_script_t *script,
    const ecs_script_ast_to_str_desc_t *desc)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_script_impl_t *impl = flecs_script_impl(script);

    if (!impl->expr && !impl->root) {
        return NULL;
    }

    if (impl->expr) {
        flecs_expr_to_str_buf(
            script, impl->expr, &buf, desc ? desc->colors : false);
    } else {
        if (ecs_script_ast_to_buf_w_desc(script, &buf, desc)) {
            goto error;
        }
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

int ecs_script_ast_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf,
    bool colors)
{
    return ecs_script_ast_to_buf_w_desc(script, buf,
        &(ecs_script_ast_to_str_desc_t){ .colors = colors });
}

char* ecs_script_ast_to_str(
    ecs_script_t *script,
    bool colors)
{
    return ecs_script_ast_to_str_w_desc(script,
        &(ecs_script_ast_to_str_desc_t){ .colors = colors });
}

#endif
