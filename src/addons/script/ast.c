/**
 * @file addons/script/ast.c
 * @brief Script AST implementation.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

#define flecs_ast_new(parser, T, kind)\
    (T*)flecs_ast_new_(parser, ECS_SIZEOF(T), kind)
#define flecs_ast_vec(parser, vec, T) \
    ecs_vec_init_t(&parser->script->allocator, &vec, T*, 0)
#define flecs_ast_append(parser, vec, T, node) \
    ecs_vec_append_t(&parser->script->allocator, &vec, T*)[0] = node

static
void* flecs_ast_new_(
    ecs_parser_t *parser,
    ecs_size_t size, 
    ecs_script_node_kind_t kind)
{
    ecs_assert(parser->script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_allocator_t *a = &parser->script->allocator;
    ecs_script_node_t *result = flecs_calloc_w_dbg_info(
        a, size, "ecs_script_node_t");
    result->kind = kind;
    result->pos = parser->pos;
    return result;
}

ecs_script_scope_t* flecs_script_scope_new(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *result = flecs_ast_new(
        parser, ecs_script_scope_t, EcsAstScope);
    flecs_ast_vec(parser, result->stmts, ecs_script_node_t);
    ecs_vec_init_t(NULL, &result->components, ecs_id_t, 0);
    return result;
}

bool flecs_scope_is_empty(
    ecs_script_scope_t *scope)
{
    return ecs_vec_count(&scope->stmts) == 0;
}

ecs_script_scope_t* flecs_script_insert_scope(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_script_scope_t *result = flecs_script_scope_new(parser);
    flecs_ast_append(parser, scope->stmts, ecs_script_scope_t, result);
    ecs_vec_init_t(NULL, &result->components, ecs_id_t, 0);
    return result;
}

ecs_script_entity_t* flecs_script_insert_entity(
    ecs_parser_t *parser,
    const char *name,
    bool name_is_expr)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_entity_t *result = flecs_ast_new(
        parser, ecs_script_entity_t, EcsAstEntity);

    if (name && !ecs_os_strcmp(name, "_")) {
        name = NULL;
    }

    result->name = name;

    if (name_is_expr) {
        parser->significant_newline = false;
        result->name_expr = (ecs_expr_node_t*)
            flecs_expr_interpolated_string(parser, name);
        if (!result->name_expr) {
            goto error;
        }
        parser->significant_newline = true;
    }

    ecs_script_scope_t *entity_scope = flecs_script_scope_new(parser);
    ecs_assert(entity_scope != NULL, ECS_INTERNAL_ERROR, NULL);
    result->scope = entity_scope;

    flecs_ast_append(parser, scope->stmts, ecs_script_entity_t, result);
    return result;
error:
    return NULL;
}

static
void flecs_script_set_id(
    ecs_script_id_t *id,
    const char *first,
    const char *second)
{
    ecs_assert(first != NULL, ECS_INTERNAL_ERROR, NULL);
    id->first = first;
    id->second = second;
    id->first_sp = -1;
    id->second_sp = -1;
}

ecs_script_pair_scope_t* flecs_script_insert_pair_scope(
    ecs_parser_t *parser,
    const char *first,
    const char *second)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_script_pair_scope_t *result = flecs_ast_new(
        parser, ecs_script_pair_scope_t, EcsAstPairScope);
    flecs_script_set_id(&result->id, first, second);
    result->scope = flecs_script_scope_new(parser);

    flecs_ast_append(parser, scope->stmts, ecs_script_pair_scope_t, result);
    return result;
}

ecs_script_tag_t* flecs_script_insert_pair_tag(
    ecs_parser_t *parser,
    const char *first,
    const char *second)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_tag_t *result = flecs_ast_new(
        parser, ecs_script_tag_t, EcsAstTag);
    flecs_script_set_id(&result->id, first, second);

    flecs_ast_append(parser, scope->stmts, ecs_script_tag_t, result);

    return result;
}

ecs_script_tag_t* flecs_script_insert_tag(
    ecs_parser_t *parser,
    const char *name)
{
    return flecs_script_insert_pair_tag(parser, name, NULL);
}

ecs_script_component_t* flecs_script_insert_pair_component(
    ecs_parser_t *parser,
    const char *first,
    const char *second)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_component_t *result = flecs_ast_new(
            parser, ecs_script_component_t, EcsAstComponent);
    flecs_script_set_id(&result->id, first, second);

    flecs_ast_append(parser, scope->stmts, ecs_script_component_t, result);

    return result;
}

ecs_script_component_t* flecs_script_insert_component(
    ecs_parser_t *parser,
    const char *name)
{
    return flecs_script_insert_pair_component(parser, name, NULL);
}

ecs_script_default_component_t* flecs_script_insert_default_component(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_default_component_t *result = flecs_ast_new(
            parser, ecs_script_default_component_t, EcsAstDefaultComponent);

    flecs_ast_append(parser, scope->stmts, 
        ecs_script_default_component_t, result);

    return result;
}

ecs_script_var_component_t* flecs_script_insert_var_component(
    ecs_parser_t *parser,
    const char *var_name)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var_name != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_var_component_t *result = flecs_ast_new(
            parser, ecs_script_var_component_t, EcsAstVarComponent);
    result->name = var_name;
    result->sp = -1;

    flecs_ast_append(parser, scope->stmts, 
        ecs_script_var_component_t, result);
    return result;
}

ecs_script_with_t* flecs_script_insert_with(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_with_t *result = flecs_ast_new(
        parser, ecs_script_with_t, EcsAstWith);

    result->expressions = flecs_script_scope_new(parser);
    result->scope = flecs_script_scope_new(parser);

    flecs_ast_append(parser, scope->stmts, ecs_script_with_t, result);
    return result;
}

ecs_script_using_t* flecs_script_insert_using(
    ecs_parser_t *parser,
    const char *name)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_using_t *result = flecs_ast_new(
        parser, ecs_script_using_t, EcsAstUsing);

    result->name = name;

    flecs_ast_append(parser, scope->stmts, ecs_script_using_t, result);
    return result;
}

ecs_script_module_t* flecs_script_insert_module(
    ecs_parser_t *parser,
    const char *name)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_module_t *result = flecs_ast_new(
        parser, ecs_script_module_t, EcsAstModule);

    result->name = name;

    flecs_ast_append(parser, scope->stmts, ecs_script_module_t, result);
    return result;
}

ecs_script_annot_t* flecs_script_insert_annot(
    ecs_parser_t *parser,
    const char *name,
    const char *expr)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_annot_t *result = flecs_ast_new(
        parser, ecs_script_annot_t, EcsAstAnnotation);

    result->name = name;
    result->expr = expr;

    flecs_ast_append(parser, scope->stmts, ecs_script_annot_t, result);
    return result;
}

ecs_script_template_node_t* flecs_script_insert_template(
    ecs_parser_t *parser,
    const char *name)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_template_node_t *result = flecs_ast_new(
        parser, ecs_script_template_node_t, EcsAstTemplate);
    result->name = name;
    result->scope = flecs_script_scope_new(parser);

    flecs_ast_append(parser, scope->stmts, ecs_script_template_node_t, result);
    return result;
}

ecs_script_var_node_t* flecs_script_insert_var(
    ecs_parser_t *parser,
    const char *name)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_var_node_t *result = flecs_ast_new(
        parser, ecs_script_var_node_t, EcsAstConst);
    result->name = name;

    flecs_ast_append(parser, scope->stmts, ecs_script_var_node_t, result);
    return result;
}

ecs_script_if_t* flecs_script_insert_if(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_if_t *result = flecs_ast_new(
        parser, ecs_script_if_t, EcsAstIf);
    result->if_true = flecs_script_scope_new(parser);
    result->if_false = flecs_script_scope_new(parser);

    flecs_ast_append(parser, scope->stmts, ecs_script_if_t, result);
    return result;
}

ecs_script_for_range_t* flecs_script_insert_for_range(
    ecs_parser_t *parser)
{
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_for_range_t *result = flecs_ast_new(
        parser, ecs_script_for_range_t, EcsAstFor);
    result->scope = flecs_script_scope_new(parser);

    flecs_ast_append(parser, scope->stmts, ecs_script_for_range_t, result);
    return result;
}

#endif
