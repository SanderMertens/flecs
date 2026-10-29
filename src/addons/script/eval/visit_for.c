/**
 * @file addons/script/eval/visit_for.c
 * @brief For statement (range and array/vector/map iteration).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

void flecs_script_for_prepare(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state)
{
    ecs_os_zeromem(state);
    state->for_slot = v->for_slot;
    state->force = v->force;

    bool visited = node->scope->scope_slot >= 0 &&
        node->scope->scope_slot < ecs_vec_count(&v->state->scope_slots) &&
        ecs_vec_get_t(&v->state->scope_slots,
            int32_t, node->scope->scope_slot)[0] == v->visit;
    if (node->for_slot >= 0 &&
        node->for_slot < ecs_vec_count(&v->state->for_slots))
    {
        ecs_script_for_slot_t *slot = ecs_vec_get_t(
            &v->state->for_slots, ecs_script_for_slot_t, node->for_slot);
        slot->scope_slot = node->scope->scope_slot;
        if (!visited) {
            flecs_script_for_slot_clear(v->world, slot, false);
        }
    }
}

int flecs_script_for_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state,
    const ecs_value_t *collection)
{
    ecs_entity_t key_type = 0, elem_type = ecs_id(ecs_i32_t);
    if (collection) {
        ecs_entity_t type = collection->type;
        const EcsType *type_ptr = ecs_get(v->world, type, EcsType);
        switch(type_ptr ? type_ptr->kind : EcsPrimitiveType) {
        case EcsArrayType: {
            const EcsArray *array = ecs_get(v->world, type, EcsArray);
            ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);
            state->kind = FlecsScriptForArray;
            state->elems = collection->ptr;
            state->count = array->count;
            elem_type = array->type;
            break;
        }
        case EcsVectorType: {
            const EcsVector *vector = ecs_get(v->world, type, EcsVector);
            ecs_assert(vector != NULL, ECS_INTERNAL_ERROR, NULL);
            state->kind = FlecsScriptForVector;
            if (collection->ptr) {
                state->elems = ecs_vec_first(collection->ptr);
                state->count = ecs_vec_count(collection->ptr);
            }

            elem_type = vector->type;
            break;
        }
        case EcsMapType: {
            const EcsMap *map = ecs_get(v->world, type, EcsMap);
            ecs_assert(map != NULL, ECS_INTERNAL_ERROR, NULL);
            state->kind = FlecsScriptForMap;
            if (collection->ptr) {
                state->map_it = ecs_map_iter(collection->ptr);
            }

            key_type = map->key_type;
            elem_type = map->type;
            break;
        }
        default:
            flecs_script_eval_error(v, node,
                "for loop cannot iterate value of type '%s': "
                "not an array, vector or map",
                flecs_errstr(ecs_get_path(v->world, type)));
            return -1;
        }
    }

    bool map = state->kind == FlecsScriptForMap;
    int32_t var_count = node->loop_var_count;
    if (var_count > (map ? 3 : 2)) {
        if (state->kind == FlecsScriptForRange) {
            flecs_script_eval_error(v, node,
                "too many loop variables for range "
                "(expected 'value' or '(index, value)')");
        } else if (map) {
            flecs_script_eval_error(v, node,
                "too many loop variables for map (expected 'elem', "
                "'(key, elem)' or '(key, index, elem)')");
        } else {
            flecs_script_eval_error(v, node,
                "too many loop variables for %s "
                "(expected 'elem' or '(index, elem)')",
                state->kind == FlecsScriptForArray ? "array" : "vector");
        }

        return -1;
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(v->world, elem_type);
    if (!elem_ti) {
        flecs_script_eval_error(v, node,
            "for loop element type is not a valid type");
        return -1;
    }

    state->elem_size = elem_ti->size;

    int32_t i = 0;
    state->key_var = state->index_var = -1;
    if (map && var_count >= 2) {
        const ecs_type_info_t *key_ti = ecs_get_type_info(v->world, key_type);
        ecs_assert(key_ti != NULL, ECS_INTERNAL_ERROR, NULL);
        state->key_size = key_ti->size;
        state->var_types[i] = key_type;
        state->key_var = i ++;
    }

    if (var_count == (map ? 3 : 2)) {
        state->var_types[i] = ecs_id(ecs_i32_t);
        state->index_var = i ++;
    }

    state->var_types[i] = elem_type;
    state->elem_var = i;
    return 0;
}

int flecs_script_for_declare_vars(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state,
    bool alloc_elem)
{
    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, v->r->allocator);
    ecs_script_vars_set_size(v->vars, node->loop_var_count);
    state->vars = v->vars;
    state->var_base = 0;

    int32_t i;
    for (i = 0; i < node->loop_var_count; i ++) {
        const char *name = node->loop_vars[i];
        ecs_script_var_t *var = ecs_script_vars_declare(v->vars, name);
        if (!var) {
            flecs_script_eval_error(v, node,
                "duplicate loop variable '%s'", name);
            goto error;
        }

        var->value.type = state->var_types[i];
        if (!alloc_elem && i == state->elem_var) {
            continue;
        }

        const ecs_type_info_t *ti = ecs_get_type_info(
            v->world, var->value.type);
        if (!ti) {
            flecs_script_eval_error(v, node,
                "invalid type for loop variable '%s'", name);
            goto error;
        }

        var->value.ptr = flecs_stack_calloc(
            &v->r->stack, ti->size, ti->alignment);
        var->type_info = ti;
        var->owned = true;
    }

    return 0;
error:
    v->vars = ecs_script_vars_pop(v->vars);
    return -1;
}

static int flecs_script_for_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state)
{
    flecs_script_for_prepare(v, node, state);

    if (!node->expr) {
        ecs_value_t from = { .type = ecs_id(ecs_i32_t), .ptr = &state->from };
        ecs_value_t to = { .type = ecs_id(ecs_i32_t), .ptr = &state->count };
        if (flecs_script_eval_expr(v, &node->from, &from) ||
            flecs_script_eval_expr(v, &node->to, &to))
        {
            return -1;
        }
    } else if (flecs_script_eval_expr(v, &node->expr, &state->collection)) {
        return -1;
    }

    if (flecs_script_for_init(v, node, state,
            node->expr ? &state->collection : NULL) ||
        flecs_script_for_declare_vars(v, node, state,
            state->kind == FlecsScriptForRange))
    {
        if (state->collection.type) {
            ecs_ptr_free(v->world, state->collection.type,
                state->collection.ptr);
        }

        return -1;
    }

    v->for_slot = node->for_slot;
    v->force = true;
    return 0;
}

bool flecs_script_for_next(
    flecs_script_for_state_t *state)
{
    ecs_script_var_t *vars = &((ecs_script_var_t*)
        state->vars->vars.array)[state->var_base];
    ecs_script_var_t *elem = &vars[state->elem_var];
    int32_t i = state->index;

    switch(state->kind) {
    case FlecsScriptForRange: {
        int32_t value = state->from + i;
        if (value >= state->count) {
            return false;
        }

        *(int32_t*)elem->value.ptr = value;
        break;
    }
    case FlecsScriptForArray:
    case FlecsScriptForVector:
        if (i >= state->count) {
            return false;
        }
        elem->value.ptr = ECS_OFFSET(state->elems, state->elem_size * i);
        elem->owned = false;
        break;
    case FlecsScriptForMap:
        if (!ecs_map_next(&state->map_it)) {
            return false;
        }
        if (state->key_var != -1) {
            ecs_map_key_t key = ecs_map_key(&state->map_it);
            ecs_os_memcpy(vars[state->key_var].value.ptr, &key,
                state->key_size);
        }

        if (state->elem_size > ECS_SIZEOF(ecs_map_val_t)) {
            elem->value.ptr = ecs_map_ptr(&state->map_it);
        } else {
            elem->value.ptr = &state->map_it.res[1];
        }

        elem->owned = false;
        break;
    }

    if (state->index_var != -1) {
        *(int32_t*)vars[state->index_var].value.ptr = i;
    }

    state->index = i + 1;
    return true;
}

void flecs_script_eval_for_leave(
    ecs_script_eval_visitor_t *v,
    flecs_script_for_state_t *state)
{
    v->vars = ecs_script_vars_pop(v->vars);
    v->for_slot = state->for_slot;
    v->force = state->force;

    if (state->collection.type) {
        ecs_ptr_free(v->world, state->collection.type, state->collection.ptr);
    }
}

int flecs_script_step_for(
    ecs_script_runner_t *r,
    flecs_script_frame_t *frame)
{
    ecs_script_eval_visitor_t *v = &r->v;
    ecs_script_for_t *node = (ecs_script_for_t*)frame->node;
    flecs_script_for_state_t *state = &frame->state.for_;

    if (frame->pc == 0) {
        if (flecs_script_for_enter(v, node, state)) {
            return -1;
        }

        frame->pc = 1;
    }

    if (flecs_script_for_next(state)) {
        flecs_script_scope_push(r, node->scope);
        return 0;
    }

    flecs_script_eval_for_leave(v, state);
    flecs_script_frame_pop(r);
    return 0;
}

#endif
