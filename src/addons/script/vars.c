#include "flecs.h"
#include "script.h"

ecs_script_vars_t* ecs_script_vars_push(
    ecs_script_vars_t *parent,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator)
{
    ecs_stack_cursor_t *cursor = flecs_stack_get_cursor(stack);
    ecs_script_vars_t *result = flecs_stack_calloc_t(stack, ecs_script_vars_t);
    ecs_vec_init_t(allocator, &result->vars, ecs_script_var_t, 0);
    result->parent = parent;
    result->stack = stack;
    result->allocator = allocator;
    result->cursor = cursor;
    return result;
}

ecs_script_vars_t* ecs_script_vars_pop(
    ecs_script_vars_t *vars)
{
    ecs_script_vars_t *parent = vars->parent;
    ecs_stack_cursor_t *cursor = vars->cursor;
    int32_t i, count = ecs_vec_count(&vars->vars);
    if (count) {
        ecs_script_var_t *var_array = ecs_vec_first(&vars->vars);
        for (i = 0; i < count; i ++) {
            ecs_script_var_t *var = &var_array[i];
            if (!var->value.ptr) {
                continue;
            }

            if (!var->type_info || !var->type_info->hooks.dtor) {
                continue;
            }

            var->type_info->hooks.dtor(var->value.ptr, 1, var->type_info);
        }

        flecs_name_index_fini(&vars->var_index);
    }

    ecs_vec_fini_t(vars->allocator, &vars->vars, ecs_script_var_t);
    flecs_stack_restore_cursor(vars->stack, cursor);
    return parent;
}

ecs_script_var_t* ecs_script_vars_declare(
    ecs_script_vars_t *vars,
    const char *name)
{
    if (!ecs_vec_count(&vars->vars)) {
        flecs_name_index_init(&vars->var_index, vars->allocator);
    } else {
        if (flecs_name_index_find(&vars->var_index, name, 0, 0) != 0) {
            goto error;
        }
    }

    ecs_script_var_t *var = ecs_vec_append_t(
        vars->allocator, &vars->vars, ecs_script_var_t);
    var->name = name;
    var->value.ptr = NULL;
    var->value.type = 0;
    var->type_info = NULL;

    flecs_name_index_ensure(&vars->var_index,
        flecs_ito(uint64_t, ecs_vec_count(&vars->vars)), name, 0, 0);

    return var;
error:
    return NULL;
}

ecs_script_var_t* ecs_script_vars_lookup(
    const ecs_script_vars_t *vars,
    const char *name)
{
    uint64_t var_id = 0;
    if (ecs_vec_count(&vars->vars)) {
        var_id = flecs_name_index_find(&vars->var_index, name, 0, 0);
    }

    if (!var_id) {
        if (vars->parent) {
            return ecs_script_vars_lookup(vars->parent, name);
        }
        return NULL;
    }

    return ecs_vec_get_t(&vars->vars, ecs_script_var_t, 
        flecs_uto(int32_t, var_id - 1));
}
