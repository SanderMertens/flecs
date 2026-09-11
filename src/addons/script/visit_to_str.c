/**
 * @file addons/script/visit_to_str.c
 * @brief Script AST to string visitor.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

typedef struct ecs_script_str_visitor_t {
    ecs_script_impl_t *script;
    ecs_strbuf_t *buf;
    int32_t depth;
    bool newline;
    bool colors;
} ecs_script_str_visitor_t;

static void flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope);

static void flecs_script_stmt_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node);

static void flecs_script_color_to_str(
    ecs_script_str_visitor_t *v,
    const char *color)
{
    if (v->colors) ecs_strbuf_appendstr(v->buf, color);
}

static void flecs_scriptbuf_append(
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

static void flecs_scriptbuf_appendstr(
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

static void flecs_script_id_to_str(
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

static void flecs_expr_to_str(
    ecs_script_str_visitor_t *v,
    const ecs_expr_node_t *expr)
{
    if (expr) {
        flecs_expr_to_str_buf(
            &v->script->pub, expr, v->buf, v->colors);
    } else {
        flecs_scriptbuf_appendstr(v, "{}");
    }
}

const char* flecs_script_node_kind_str(
    const ecs_script_node_t *node)
{
    switch(node->kind) {
    case EcsAstScope:              return "scope";
    case EcsAstWithTag:
    case EcsAstTag:                return "tag";
    case EcsAstWithComponent:
    case EcsAstComponent:          return "component";
    case EcsAstWith:               return "with";
    case EcsAstUsing:              return "using";
    case EcsAstModule:             return "module";
    case EcsAstAnnotation:         return "annot";
    case EcsAstTemplate:           return "template";
    case EcsAstProp:               return "prop";
    case EcsAstMut:                return "mut";
    case EcsAstConst:              return "const";
    case EcsAstExportConst:        return "export const";
    case EcsAstExportMut:          return "export mut";
    case EcsAstEntity:             return "entity";
    case EcsAstPairScope:          return "pair_scope";
    case EcsAstIf:                 return "if";
    case EcsAstFor:                return "for";
    case EcsAstInclude:            return "include";
    case EcsAstFunction:           return "fn";
    case EcsAstAwait:              return "await";
    case EcsAstTry:                return "try";
    case EcsAstContinue:           return "continue";
    case EcsAstAsync:              return "async";
    case EcsAstWhile:              return "while";
    case EcsAstAssign:             return "assign";
    }
    return "???";
}

static void flecs_script_named_scope_to_str(
    ecs_script_str_visitor_t *v,
    const char *name,
    ecs_script_scope_t *scope)
{
    flecs_script_color_to_str(v, ECS_CYAN);
    flecs_scriptbuf_append(v, "%s: ", name);
    flecs_script_color_to_str(v, ECS_NORMAL);
    flecs_script_scope_to_str(v, scope);
}

static void flecs_script_scope_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_scope_t *scope)
{
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_vec_count(&scope->stmts)) {
        flecs_scriptbuf_appendstr(v, "{}\n");
        return;
    }

    flecs_scriptbuf_appendstr(v, "{\n");

    v->depth ++;

    int32_t i, count = ecs_vec_count(&scope->stmts);
    ecs_script_node_t **nodes = ecs_vec_first(&scope->stmts);
    for (i = 0; i < count; i ++) {
        flecs_script_stmt_to_str(v, nodes[i]);
    }

    v->depth --;

    flecs_scriptbuf_appendstr(v, "}\n");
}

static void flecs_script_stmt_to_str(
    ecs_script_str_visitor_t *v,
    ecs_script_node_t *node)
{
    if (node->kind != EcsAstScope) {
        flecs_script_color_to_str(v, ECS_BLUE);
        flecs_scriptbuf_append(v, "%s: ", flecs_script_node_kind_str(node));
        flecs_script_color_to_str(v, ECS_NORMAL);
    }
    ecs_script_scope_t *scope = NULL;
    bool wrap = false;
    switch(node->kind) {
    case EcsAstScope:
        flecs_script_scope_to_str(v, (ecs_script_scope_t*)node);
        return;
    case EcsAstTag:
    case EcsAstWithTag: {
        ecs_script_tag_t *stmt = (ecs_script_tag_t*)node;
        flecs_script_id_to_str(v, &stmt->id);
        break;
    }
    case EcsAstComponent:
    case EcsAstWithComponent: {
        ecs_script_component_t *stmt = (ecs_script_component_t*)node;
        flecs_script_id_to_str(v, &stmt->id);
        if (stmt->expr) {
            flecs_scriptbuf_appendstr(v, ": ");
            flecs_expr_to_str(v, stmt->expr);
        }
        break;
    }
    case EcsAstWith: {
        ecs_script_with_t *stmt = (ecs_script_with_t*)node;
        flecs_scriptbuf_appendstr(v, "{\n");
        v->depth ++;
        flecs_script_named_scope_to_str(v, "expressions", stmt->expressions);
        flecs_script_named_scope_to_str(v, "scope", stmt->scope);
        v->depth --;
        flecs_scriptbuf_appendstr(v, "}\n");
        return;
    }
    case EcsAstUsing: {
        ecs_script_using_t *stmt = (ecs_script_using_t*)node;
        flecs_scriptbuf_append(v, "%s", stmt->name);
        break;
    }
    case EcsAstModule: {
        ecs_script_module_t *stmt = (ecs_script_module_t*)node;
        flecs_scriptbuf_append(v, "%s", stmt->name);
        break;
    }
    case EcsAstAnnotation: {
        ecs_script_annot_t *stmt = (ecs_script_annot_t*)node;
        flecs_script_color_to_str(v, ECS_GREEN);
        flecs_scriptbuf_append(v, "%s = \"%s\"", stmt->name, stmt->expr);
        flecs_script_color_to_str(v, ECS_NORMAL);
        break;
    }
    case EcsAstTemplate: {
        ecs_script_template_node_t *stmt = (ecs_script_template_node_t*)node;
        flecs_scriptbuf_append(v, "%s ", stmt->name);
        if (stmt->base) {
            flecs_scriptbuf_append(v, ": %s ", stmt->base);
        }
        scope = stmt->scope;
        break;
    }
    case EcsAstConst:
    case EcsAstExportConst:
    case EcsAstExportMut:
    case EcsAstProp:
    case EcsAstMut: {
        ecs_script_var_node_t *stmt = (ecs_script_var_node_t*)node;
        if (stmt->type) {
            flecs_scriptbuf_append(v, "%s : %s%s = ",
                stmt->name,
                stmt->type_is_template ? "template " : "",
                stmt->type);
        } else {
            flecs_scriptbuf_append(v, "%s = ",
                stmt->name);
        }
        if (stmt->is_await) {
            flecs_scriptbuf_appendstr(v, "await ");
        }
        flecs_expr_to_str(v, stmt->expr);
        break;
    }
    case EcsAstEntity: {
        ecs_script_entity_t *stmt = (ecs_script_entity_t*)node;
        if (stmt->kind) {
            flecs_scriptbuf_append(v, "%s ", stmt->kind);
        }
        if (stmt->name) {
            flecs_scriptbuf_append(v, "%s ", stmt->name);
        } else {
            flecs_scriptbuf_appendstr(v, "<anon> ");
        }

        if (!flecs_scope_is_empty(stmt->scope)) {
            scope = stmt->scope;
        }
        break;
    }
    case EcsAstPairScope: {
        ecs_script_pair_scope_t *stmt = (ecs_script_pair_scope_t*)node;
        flecs_script_id_to_str(v, &stmt->id);
        flecs_scriptbuf_appendstr(v, " ");
        scope = stmt->scope;
        break;
    }
    case EcsAstIf: {
        ecs_script_if_t *stmt = (ecs_script_if_t*)node;
        flecs_expr_to_str(v, stmt->expr);
        flecs_scriptbuf_appendstr(v, " {\n");
        v->depth ++;
        flecs_script_named_scope_to_str(v, "true", stmt->if_true);
        flecs_script_named_scope_to_str(v, "false", stmt->if_false);
        v->depth --;
        flecs_scriptbuf_appendstr(v, "}\n");
        return;
    }
    case EcsAstFor: {
        ecs_script_for_t *stmt = (ecs_script_for_t*)node;

        int32_t i;
        if (stmt->loop_var_count > 1) {
            flecs_scriptbuf_appendstr(v, "(");
        }
        for (i = 0; i < stmt->loop_var_count; i ++) {
            if (i) {
                flecs_scriptbuf_appendstr(v, ", ");
            }
            flecs_scriptbuf_appendstr(v, stmt->loop_vars[i]);
        }
        if (stmt->loop_var_count > 1) {
            flecs_scriptbuf_appendstr(v, ")");
        }

        flecs_script_color_to_str(v, ECS_BLUE);
        flecs_scriptbuf_appendstr(v, " in ");
        flecs_script_color_to_str(v, ECS_NORMAL);
        if (stmt->expr) {
            flecs_expr_to_str(v, stmt->expr);
        } else {
            flecs_expr_to_str(v, stmt->from);
            flecs_scriptbuf_appendstr(v, " .. ");
            flecs_expr_to_str(v, stmt->to);
        }

        scope = stmt->scope;
        wrap = true;
        break;
    }
    case EcsAstInclude: {
        ecs_script_include_t *stmt = (ecs_script_include_t*)node;
        flecs_scriptbuf_append(v, "%s", stmt->filename);
        break;
    }
    case EcsAstAwait: {
        ecs_script_await_t *await = (ecs_script_await_t*)node;
        flecs_expr_to_str(v, await->expr);
        break;
    }
    case EcsAstContinue:
        break;
    case EcsAstAsync: {
        ecs_script_async_t *stmt = (ecs_script_async_t*)node;
        scope = stmt->scope;
        break;
    }
    case EcsAstWhile: {
        ecs_script_while_t *stmt = (ecs_script_while_t*)node;
        flecs_expr_to_str(v, stmt->expr);
        scope = stmt->scope;
        wrap = true;
        break;
    }
    case EcsAstAssign: {
        ecs_script_assign_t *stmt = (ecs_script_assign_t*)node;
        flecs_scriptbuf_append(v, "%s = ", stmt->name);
        flecs_expr_to_str(v, stmt->expr);
        break;
    }
    case EcsAstTry: {
        ecs_script_try_t *try_stmt = (ecs_script_try_t*)node;
        flecs_scriptbuf_appendstr(v, "{\n");
        v->depth ++;
        flecs_script_scope_to_str(v, try_stmt->try_scope);
        int32_t i, count = ecs_vec_count(&try_stmt->catches);
        ecs_script_catch_t *catches = ecs_vec_first(&try_stmt->catches);
        for (i = 0; i < count; i ++) {
            flecs_script_color_to_str(v, ECS_CYAN);
            if (catches[i].error) {
                flecs_scriptbuf_append(v, "catch(%s): ", catches[i].error);
            } else {
                flecs_scriptbuf_appendstr(v, "catch: ");
            }
            flecs_script_color_to_str(v, ECS_NORMAL);
            flecs_script_scope_to_str(v, catches[i].scope);
        }
        v->depth --;
        flecs_scriptbuf_appendstr(v, "}\n");
        return;
    }
    case EcsAstFunction: {
        ecs_script_function_node_t *fn = (ecs_script_function_node_t*)node;
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
        break;
    }
    }

    if (scope) {
        if (wrap) {
            flecs_scriptbuf_appendstr(v, " {\n");
            v->depth ++;
        }
        flecs_script_scope_to_str(v, scope);
        if (wrap) {
            v->depth --;
            flecs_scriptbuf_appendstr(v, "}\n");
        }
    } else {
        flecs_scriptbuf_appendstr(v, "\n");
    }
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
    v.script = (ecs_script_impl_t*)ECS_CONST_CAST(ecs_script_t*, script);

    flecs_script_stmt_to_str(&v, node);
    return 0;
}

int ecs_script_ast_to_buf(
    ecs_script_t *script,
    ecs_strbuf_t *buf,
    bool colors)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(buf != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_script_ast_node_to_buf(script,
        (ecs_script_node_t*)flecs_script_impl(script)->root, buf, colors, 0);
error:
    ecs_strbuf_reset(buf);
    return -1;
}

char* ecs_script_ast_to_str(
    ecs_script_t *script,
    bool colors)
{
    ecs_check(script != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_script_impl_t *impl = flecs_script_impl(script);

    if (!impl->expr && !impl->root) {
        return NULL;
    }

    if (flecs_script_impl(script)->expr) {
        flecs_expr_to_str_buf(
            script, flecs_script_impl(script)->expr, &buf, colors);
    } else {
        if (ecs_script_ast_to_buf(script, &buf, colors)) {
            goto error;
        }
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

#endif
