/**
 * @file expr/vars.c
 * @brief Utilities for variable substitution in flecs string expressions.
 */

#include "../../private_api.h"

#ifdef FLECS_EXPR

static
void flecs_expr_var_scope_init(
    ecs_world_t *world,
    ecs_expr_var_scope_t *scope,
    ecs_expr_var_scope_t *parent)
{
    flecs_name_index_init(&scope->var_index, &world->allocator);
    ecs_vec_init_t(&world->allocator, &scope->vars, ecs_expr_var_t, 0);
    scope->parent = parent;
}

static
void flecs_expr_var_scope_fini(
    ecs_world_t *world,
    ecs_expr_var_scope_t *scope)
{
    ecs_vec_t *vars = &scope->vars;
    int32_t i, count = vars->count;
    for (i = 0; i < count; i++) {
        ecs_expr_var_t *var = ecs_vec_get_t(vars, ecs_expr_var_t, i);
        ecs_value_free(world, var->value.type, var->value.ptr);
        flecs_strfree(&world->allocator, var->name);
    }

    ecs_vec_fini_t(&world->allocator, &scope->vars, ecs_expr_var_t);
    flecs_name_index_fini(&scope->var_index);
}

void ecs_vars_init(
    ecs_world_t *world,
    ecs_vars_t *vars)
{
    flecs_expr_var_scope_init(world, &vars->root, NULL);
    vars->world = world;
    vars->cur = &vars->root;
}

void ecs_vars_fini(
    ecs_vars_t *vars)
{
    ecs_expr_var_scope_t *cur = vars->cur, *next;
    do {
        next = cur->parent;
        flecs_expr_var_scope_fini(vars->world, cur);
        if (cur != &vars->root) {
            flecs_free_t(&vars->world->allocator, ecs_expr_var_scope_t, cur);
        }
    } while ((cur = next));
}

void ecs_vars_push(
    ecs_vars_t *vars)
{
    ecs_expr_var_scope_t *scope = flecs_calloc_t(&vars->world->allocator, 
        ecs_expr_var_scope_t);
    flecs_expr_var_scope_init(vars->world, scope, vars->cur);
    vars->cur = scope;
}

int ecs_vars_pop(
    ecs_vars_t *vars)
{
    ecs_expr_var_scope_t *scope = vars->cur;
    ecs_check(scope != &vars->root, ECS_INVALID_OPERATION, NULL);
    vars->cur = scope->parent;
    flecs_expr_var_scope_fini(vars->world, scope);
    flecs_free_t(&vars->world->allocator, ecs_expr_var_scope_t, scope);
    return 0;
error:
    return 1;
}

ecs_expr_var_t* ecs_vars_declare(
    ecs_vars_t *vars,
    const char *name,
    ecs_entity_t type)
{
    ecs_assert(vars != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(type != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_expr_var_scope_t *scope = vars->cur;
    ecs_hashmap_t *var_index = &scope->var_index;

    if (flecs_name_index_find(var_index, name, 0, 0) != 0) {
        ecs_err("variable %s redeclared", name);
        goto error;
    }

    ecs_expr_var_t *var = ecs_vec_append_t(&vars->world->allocator, 
        &scope->vars, ecs_expr_var_t);
    
    var->value.ptr = ecs_value_new(vars->world, type);
    if (!var->value.ptr) {
        goto error;
    }
    var->value.type = type;
    var->name = flecs_strdup(&vars->world->allocator, name);

    flecs_name_index_ensure(var_index, 
        flecs_ito(uint64_t, ecs_vec_count(&scope->vars)), var->name, 0, 0);
    return var;
error:
    return NULL;
}

ecs_expr_var_t* ecs_vars_declare_w_value(
    ecs_vars_t *vars,
    const char *name,
    ecs_value_t *value)
{
    ecs_assert(vars != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_expr_var_scope_t *scope = vars->cur;
    ecs_hashmap_t *var_index = &scope->var_index;

    if (flecs_name_index_find(var_index, name, 0, 0) != 0) {
        ecs_err("variable %s redeclared", name);
        ecs_value_free(vars->world, value->type, value->ptr);
        goto error;
    }

    ecs_expr_var_t *var = ecs_vec_append_t(&vars->world->allocator, 
        &scope->vars, ecs_expr_var_t);
    var->value = *value;
    var->name = flecs_strdup(&vars->world->allocator, name);
    value->ptr = NULL; /* Take ownership, prevent double free */

    flecs_name_index_ensure(var_index, 
        flecs_ito(uint64_t, ecs_vec_count(&scope->vars)), var->name, 0, 0);
    return var;
error:
    return NULL;
}

static
ecs_expr_var_t* flecs_vars_scope_lookup(
    ecs_expr_var_scope_t *scope,
    const char *name)
{
    uint64_t var_id = flecs_name_index_find(&scope->var_index, name, 0, 0);
    if (var_id == 0) {
        if (scope->parent) {
            return flecs_vars_scope_lookup(scope->parent, name);
        }
        return NULL;
    }

    return ecs_vec_get_t(&scope->vars, ecs_expr_var_t, 
        flecs_uto(int32_t, var_id - 1));
}

ecs_expr_var_t* ecs_vars_lookup(
    ecs_vars_t *vars,
    const char *name)
{
    return flecs_vars_scope_lookup(vars->cur, name);
}

#endif
