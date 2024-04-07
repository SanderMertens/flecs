
#include "flecs.h"
#include "script.h"

#define flecs_ast_strdup(script, str)\
    (str ? flecs_strdup(&script->allocator, str) : NULL)
#define flecs_ast_new(script, T, kind)\
    (T*)flecs_ast_new_(&script->allocator, ECS_SIZEOF(T), kind)
#define flecs_ast_vec(script, vec, T) \
    ecs_vec_init_t(&script->allocator, &vec, T*, 0)
#define flecs_ast_append(script, vec, T, node) \
    ecs_vec_append_t(&script->allocator, &vec, T*)[0] = node

static
void* flecs_ast_new_(
    ecs_allocator_t *a, 
    ecs_size_t size, 
    ecs_script_node_kind_t kind)
{
    ecs_script_node_t *result = ecs_os_calloc(size);
    result->kind = kind;
    return result;
}

static
ecs_script_scope_t* flecs_script_scope_new(
    ecs_script_t *script)
{
    ecs_script_scope_t *result = flecs_ast_new(
        script, ecs_script_scope_t, EcsAstScope);
    flecs_ast_vec(script, result->stmts, ecs_script_node_t);
    return result;
}

ecs_script_t* flecs_script_new(void) {
    ecs_script_t *result = ecs_os_calloc_t(ecs_script_t);
    flecs_allocator_init(&result->allocator);
    result->root = flecs_script_scope_new(result);
    return result;
}

ecs_script_entity_t* flecs_script_insert_entity(
    ecs_script_parser_t *parser,
    const char *name)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_entity_t *result = flecs_ast_new(
        script, ecs_script_entity_t, EcsAstEntity);
    result->name = name;

    ecs_script_scope_t *entity_scope = flecs_script_scope_new(script);
    ecs_assert(entity_scope != NULL, ECS_INTERNAL_ERROR, NULL);
    result->parent = scope;
    result->scope = entity_scope;
    entity_scope->parent = result;

    flecs_ast_append(script, scope->stmts, ecs_script_entity_t, result);
    return result;
}

static
void flecs_script_set_id(
    ecs_script_parser_t *parser,
    ecs_script_id_t *id,
    const char *first,
    const char *second)
{
    ecs_assert(first != NULL, ECS_INTERNAL_ERROR, NULL);
    id->first = first;
    id->second = second;
}

ecs_script_tag_t* flecs_script_insert_pair_tag(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_tag_t *result = flecs_ast_new(
        script, ecs_script_tag_t, EcsAstTag);
    flecs_script_set_id(parser, &result->id, first, second);

    flecs_ast_append(script, scope->stmts, ecs_script_tag_t, result);

    return result;
}

ecs_script_tag_t* flecs_script_insert_tag(
    ecs_script_parser_t *parser,
    const char *name)
{
    return flecs_script_insert_pair_tag(parser, name, NULL);
}

ecs_script_component_t* flecs_script_insert_pair_component(
    ecs_script_parser_t *parser,
    const char *first,
    const char *second)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_component_t *result = flecs_ast_new(
            script, ecs_script_component_t, EcsAstComponent);
    flecs_script_set_id(parser, &result->id, first, second);

    flecs_ast_append(script, scope->stmts, ecs_script_component_t, result);

    return result;
}

ecs_script_component_t* flecs_script_insert_component(
    ecs_script_parser_t *parser,
    const char *name)
{
    return flecs_script_insert_pair_component(parser, name, NULL);
}

ecs_script_default_component_t* flecs_script_insert_default_component(
    ecs_script_parser_t *parser)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_default_component_t *result = flecs_ast_new(
            script, ecs_script_default_component_t, EcsAstDefaultComponent);

    flecs_ast_append(script, scope->stmts, 
        ecs_script_default_component_t, result);

    return result;
}

ecs_script_variable_component_t* flecs_script_insert_variable_component(
    ecs_script_parser_t *parser,
    const char *var_name)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(var_name != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_variable_component_t *result = flecs_ast_new(
            script, ecs_script_variable_component_t, EcsAstVariableComponent);
    result->var_name = var_name;

    flecs_ast_append(script, scope->stmts, 
        ecs_script_variable_component_t, result);

    return result;
}

ecs_script_with_t* flecs_script_insert_with(
    ecs_script_parser_t *parser)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_with_t *result = flecs_ast_new(
        script, ecs_script_with_t, EcsAstWith);

    result->expressions = flecs_script_scope_new(script);
    result->scope = flecs_script_scope_new(script);

    return result;
}

ecs_script_using_t* flecs_script_insert_using(
    ecs_script_parser_t *parser,
    const char *identifier)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_using_t *result = flecs_ast_new(
        script, ecs_script_using_t, EcsAstUsing);

    result->identifier = identifier;

    return result;
}

ecs_script_annotation_t* flecs_script_insert_annotation(
    ecs_script_parser_t *parser,
    const char *identifier,
    const char *expr)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_annotation_t *result = flecs_ast_new(
        script, ecs_script_annotation_t, EcsAstAnnotation);

    result->identifier = identifier;
    result->expr = expr;

    return result;
}

ecs_script_assembly_t* flecs_script_insert_assembly(
    ecs_script_parser_t *parser,
    const char *name)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_assembly_t *result = flecs_ast_new(
        script, ecs_script_assembly_t, EcsAstAssembly);
    result->name = name;
    result->scope = flecs_script_scope_new(script);

    return result;
}

ecs_script_var_t* flecs_script_insert_var(
    ecs_script_parser_t *parser,
    const char *name)
{
    ecs_script_t *script = parser->script;
    ecs_script_scope_t *scope = parser->scope;
    ecs_assert(script != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(scope != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_script_var_t *result = flecs_ast_new(
        script, ecs_script_var_t, EcsAstVar);
    result->name = name;

    return result;
}
