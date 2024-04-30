/**
 * @file addons/script/vars.c
 * @brief Script variables.
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "script.h"

ecs_script_vars_t* flecs_script_vars_push(
    ecs_script_vars_t *parent,
    ecs_stack_t *stack,
    ecs_allocator_t *allocator)
{
    ecs_check(stack || parent, ECS_INVALID_PARAMETER, 
        "must provide either parent scope or stack allocator");
    ecs_check(allocator || parent, ECS_INVALID_PARAMETER, 
        "must provide either parent scope or allocator");

    if (!stack) {
        stack = parent->stack;
    } else if (parent) {
        ecs_check(stack == parent->stack, ECS_INVALID_PARAMETER, 
            "provided stack allocator is different from parent scope");
    }
    if (!allocator) {
        allocator = parent->allocator;
    } else if (parent) {
        ecs_check(allocator == parent->allocator, ECS_INVALID_PARAMETER, 
            "provided allocator is different from parent scope");        
    }

    ecs_stack_cursor_t *cursor = flecs_stack_get_cursor(stack);
    ecs_script_vars_t *result = flecs_stack_calloc_t(stack, ecs_script_vars_t);
    ecs_vec_init_t(allocator, &result->vars, ecs_script_var_t, 0);
    result->parent = parent;
    if (parent) {
        result->world = parent->world;
    }
    result->stack = stack;
    result->allocator = allocator;
    result->cursor = cursor;
    return result;
error:
    return NULL;
}

ecs_script_vars_t* ecs_script_vars_init(
    ecs_world_t *world)
{
    ecs_script_vars_t *result = flecs_script_vars_push(NULL, 
        flecs_stage_get_stack_allocator(world),
        flecs_stage_get_allocator(world));
    result->world = ecs_get_world(world); /* Provided world can be stage */
    return result;
}

void ecs_script_vars_fini(
    ecs_script_vars_t *vars)
{
    ecs_check(vars->parent == NULL, ECS_INVALID_PARAMETER,
        "ecs_script_vars_fini can only be called on the roots cope");
    ecs_script_vars_pop(vars);
error:
    return;
}

ecs_script_vars_t* ecs_script_vars_push(
    ecs_script_vars_t *parent)
{
    ecs_check(parent != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stack_t *stack = parent->stack;
    ecs_allocator_t *allocator = parent->allocator;
    ecs_check(stack != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(allocator != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_script_vars_push(parent, stack, allocator);
error:
    return NULL;
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

ecs_script_var_t* ecs_script_vars_define_id(
    ecs_script_vars_t *vars,
    const char *name,
    ecs_entity_t type)
{
    ecs_check(vars->world != NULL, ECS_INVALID_OPERATION, "variable scope is "
        "not associated with world, create scope with ecs_script_vars_init");

    const ecs_type_info_t *ti = ecs_get_type_info(vars->world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "the entity provided for the type parameter is not a type");

    ecs_script_var_t *result = ecs_script_vars_declare(vars, name);
    if (!result) {
        return NULL;
    }

    result->value.type = type;
    result->value.ptr = flecs_stack_alloc(vars->stack, ti->size, ti->alignment);
    result->type_info = ti;

    if (ti->hooks.ctor) {
        ti->hooks.ctor(result->value.ptr, 1, ti);
    }

    return result;
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

/* Static names for iterator fields */
static const char* flecs_script_iter_field_names[] = {
    "0", "1",  "2",  "3",  "4",  "5",  "6",  "7",
    "8", "9", "10", "11", "12", "13", "14", "15"
};

void ecs_script_vars_from_iter(
    const ecs_iter_t *it,
    ecs_script_vars_t *vars,
    int offset)
{
    ecs_check(vars != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!offset || offset < it->count, ECS_INVALID_PARAMETER, NULL);

    /* Set variable for $this */
    if (it->count) {
        ecs_script_var_t *var = ecs_script_vars_lookup(vars, "this");
        if (!var) {
            var = ecs_script_vars_declare(vars, "this");
            var->value.type = ecs_id(ecs_entity_t);
        }
        var->value.ptr = &it->entities[offset];
    }

    /* Set variables for fields */
    {
        int32_t i, field_count = it->field_count;
        for (i = 0; i < field_count; i ++) {
            ecs_size_t size = it->sizes[i];
            if (!size) {
                continue;
            }

            void *ptr = it->ptrs[i];
            if (!ptr) {
                continue;
            }

            ptr = ECS_OFFSET(ptr, offset * size);

            const char *name = flecs_script_iter_field_names[i];
            ecs_script_var_t *var = ecs_script_vars_lookup(vars, name);
            if (!var) {
                var = ecs_script_vars_declare(vars, name);
                ecs_assert(ecs_script_vars_lookup(vars, name) != NULL,  
                    ECS_INTERNAL_ERROR, NULL);
                var->value.type = it->ids[i];
            } else {
                ecs_check(var->value.type == it->ids[i], 
                    ECS_INVALID_PARAMETER, NULL);
            }
            var->value.ptr = ptr;
        }
    }

    /* Set variables for query variables */
    {
        int32_t i, var_count = it->variable_count;
        for (i = 1 /* skip this variable */ ; i < var_count; i ++) {
            ecs_entity_t *e_ptr = NULL;
            ecs_var_t *query_var = &it->variables[i];
            if (query_var->entity) {
                e_ptr = &query_var->entity;
            } else {
                ecs_table_range_t *range = &query_var->range;
                if (range->count == 1) {
                    ecs_entity_t *entities = range->table->data.entities.array;
                    e_ptr = &entities[range->offset];
                }
            }
            if (!e_ptr) {
                continue;
            }

            ecs_script_var_t *var = ecs_script_vars_lookup(
                vars, it->variable_names[i]);
            if (!var) {
                var = ecs_script_vars_declare(vars, it->variable_names[i]);
                var->value.type = ecs_id(ecs_entity_t);
            } else {
                ecs_check(var->value.type == ecs_id(ecs_entity_t), 
                    ECS_INVALID_PARAMETER, NULL);
            }
            var->value.ptr = e_ptr;
        }
    }

error:
    return;
}

#endif
