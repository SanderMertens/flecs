/**
 * @file addons/script/eval/visit_for.c
 * @brief For statement (range and array/vector/map iteration).
 */

#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

int flecs_script_for_collection_kind(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    ecs_entity_t type,
    flecs_script_for_kind_t *kind,
    ecs_entity_t *key_type,
    ecs_entity_t *elem_type)
{
    const EcsType *type_ptr = ecs_get(v->world, type, EcsType);
    if (type_ptr) {
        if (type_ptr->kind == EcsArrayType) {
            const EcsArray *array = ecs_get(v->world, type, EcsArray);
            ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);
            *kind = FlecsScriptForArray;
            *elem_type = array->type;
            return 0;
        } else if (type_ptr->kind == EcsVectorType) {
            const EcsVector *vector = ecs_get(v->world, type, EcsVector);
            ecs_assert(vector != NULL, ECS_INTERNAL_ERROR, NULL);
            *kind = FlecsScriptForVector;
            *elem_type = vector->type;
            return 0;
        } else if (type_ptr->kind == EcsMapType) {
            const EcsMap *map = ecs_get(v->world, type, EcsMap);
            ecs_assert(map != NULL, ECS_INTERNAL_ERROR, NULL);
            *kind = FlecsScriptForMap;
            *key_type = map->key_type;
            *elem_type = map->type;
            return 0;
        }
    }

    char *type_str = ecs_get_path(v->world, type);
    flecs_script_eval_error(v, node,
        "for loop cannot iterate value of type '%s': "
        "not an array, vector or map", type_str);
    ecs_os_free(type_str);
    return -1;
}

int flecs_script_for_check_var_count(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_kind_t kind)
{
    int32_t max_vars = (kind == FlecsScriptForMap) ? 3 : 2;
    if (node->loop_var_count > max_vars) {
        if (kind == FlecsScriptForRange) {
            flecs_script_eval_error(v, node,
                "too many loop variables for range "
                "(expected 'value' or '(index, value)')");
        } else if (kind == FlecsScriptForMap) {
            flecs_script_eval_error(v, node,
                "too many loop variables for map (expected 'elem', "
                "'(key, elem)' or '(key, index, elem)')");
        } else {
            flecs_script_eval_error(v, node,
                "too many loop variables for %s "
                "(expected 'elem' or '(index, elem)')",
                kind == FlecsScriptForArray ? "array" : "vector");
        }
        return -1;
    }
    return 0;
}

bool flecs_script_for_has_index_var(
    flecs_script_for_kind_t kind,
    int32_t loop_var_count)
{
    if (kind == FlecsScriptForMap) {
        return loop_var_count == 3;
    }
    return loop_var_count == 2;
}

ecs_script_var_t* flecs_script_for_declare_var(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    const char *name,
    ecs_entity_t type,
    bool alloc)
{
    ecs_script_var_t *var = ecs_script_vars_declare(v->vars, name);
    if (!var) {
        flecs_script_eval_error(v, node,
            "duplicate loop variable '%s'", name);
        return NULL;
    }

    var->value.type = type;

    if (alloc) {
        const ecs_type_info_t *ti = ecs_get_type_info(v->world, type);
        if (!ti) {
            flecs_script_eval_error(v, node,
                "invalid type for loop variable '%s'", name);
            return NULL;
        }
        var->value.ptr = flecs_stack_calloc(
            &v->r->stack, ti->size, ti->alignment);
    }

    return var;
}

static int flecs_script_for_enter(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state)
{
    ecs_os_zeromem(state);
    state->for_slot = v->for_slot;
    state->force = v->force;

    bool visited = v->scope_slots && node->scope->scope_slot >= 0 &&
        node->scope->scope_slot < ecs_vec_count(v->scope_slots) &&
        ecs_vec_get_t(v->scope_slots,
            int32_t, node->scope->scope_slot)[0] == v->visit;
    if (!visited && v->for_slots && node->for_slot >= 0 &&
        node->for_slot < ecs_vec_count(v->for_slots))
    {
        ecs_vec_t *entities = ecs_vec_get_t(
            v->for_slots, ecs_vec_t, node->for_slot);
        ecs_entity_t *array = ecs_vec_first(entities);
        int32_t i, count = ecs_vec_count(entities);
        for (i = 0; i < count; i ++) {
            if (ecs_is_alive(v->world, array[i])) {
                ecs_delete(v->world, array[i]);
            }
        }
        ecs_vec_clear(entities);
    }

    ecs_entity_t key_type = 0;
    ecs_entity_t elem_type = ecs_id(ecs_i32_t);

    if (!node->expr) {
        int32_t from, to;
        ecs_value_t from_val = { .type = ecs_id(ecs_i32_t), .ptr = &from };
        ecs_value_t to_val = { .type = ecs_id(ecs_i32_t), .ptr = &to };

        if (flecs_script_eval_expr(v, &node->from, &from_val)) {
            return -1;
        }
        if (flecs_script_eval_expr(v, &node->to, &to_val)) {
            return -1;
        }

        state->kind = FlecsScriptForRange;
        state->from = from;
        state->count = to;
    } else {
        if (flecs_script_eval_expr(v, &node->expr, &state->collection)) {
            return -1;
        }

        if (flecs_script_for_collection_kind(v, node, state->collection.type,
            &state->kind, &key_type, &elem_type))
        {
            goto error;
        }

        if (state->kind == FlecsScriptForArray) {
            const EcsArray *array = ecs_get(
                v->world, state->collection.type, EcsArray);
            state->elems = state->collection.ptr;
            state->count = array->count;
        } else if (state->kind == FlecsScriptForVector) {
            ecs_vec_t *vec = state->collection.ptr;
            state->elems = ecs_vec_first(vec);
            state->count = ecs_vec_count(vec);
        } else {
            state->map_it = ecs_map_iter(state->collection.ptr);
        }
    }

    if (flecs_script_for_check_var_count(v, node, state->kind)) {
        goto error;
    }

    const ecs_type_info_t *elem_ti = ecs_get_type_info(v->world, elem_type);
    if (!elem_ti) {
        flecs_script_eval_error(v, node,
            "for loop element type is not a valid type");
        goto error;
    }

    state->elem_size = elem_ti->size;

    v->vars = flecs_script_vars_push(v->vars, &v->r->stack, &v->r->allocator);

    int32_t var_i = 0;

    if (state->kind == FlecsScriptForMap && node->loop_var_count >= 2) {
        const ecs_type_info_t *key_ti = ecs_get_type_info(v->world, key_type);
        ecs_assert(key_ti != NULL, ECS_INTERNAL_ERROR, NULL);
        state->key_size = key_ti->size;
        state->key_var = flecs_script_for_declare_var(
            v, node, node->loop_vars[var_i ++], key_type, true);
        if (!state->key_var) {
            goto error_pop;
        }
    }

    if (flecs_script_for_has_index_var(state->kind, node->loop_var_count)) {
        state->index_var = flecs_script_for_declare_var(
            v, node, node->loop_vars[var_i ++], ecs_id(ecs_i32_t), true);
        if (!state->index_var) {
            goto error_pop;
        }
    }

    state->elem_var = flecs_script_for_declare_var(
        v, node, node->loop_vars[var_i], elem_type,
        state->kind == FlecsScriptForRange);
    if (!state->elem_var) {
        goto error_pop;
    }

    v->for_slot = node->for_slot;
    v->force = true;

    return 0;
error_pop:
    v->vars = ecs_script_vars_pop(v->vars);
error:
    v->for_slot = state->for_slot;
    v->force = state->force;
    if (state->collection.type) {
        ecs_ptr_free(v->world, state->collection.type, state->collection.ptr);
        state->collection = (ecs_value_t){0, NULL};
    }
    return -1;
}

static bool flecs_script_for_next(
    flecs_script_for_state_t *state)
{
    int32_t i = state->index;

    switch(state->kind) {
    case FlecsScriptForRange: {
        int32_t value = state->from + i;
        if (value >= state->count) {
            return false;
        }
        *(int32_t*)state->elem_var->value.ptr = value;
        break;
    }
    case FlecsScriptForArray:
    case FlecsScriptForVector:
        if (i >= state->count) {
            return false;
        }
        state->elem_var->value.ptr = ECS_OFFSET(
            state->elems, state->elem_size * i);
        break;
    case FlecsScriptForMap:
        if (!ecs_map_next(&state->map_it)) {
            return false;
        }
        if (state->key_var) {
            ecs_map_key_t key = ecs_map_key(&state->map_it);
            ecs_os_memcpy(state->key_var->value.ptr, &key, state->key_size);
        }
        if (state->elem_size > ECS_SIZEOF(ecs_map_val_t)) {
            state->elem_var->value.ptr = ecs_map_ptr(&state->map_it);
        } else {
            state->elem_var->value.ptr = &state->map_it.res[1];
        }
        break;
    }

    if (state->index_var) {
        *(int32_t*)state->index_var->value.ptr = i;
    }

    state->index = i + 1;
    return true;
}

static void flecs_script_for_merge_slots(
    ecs_script_eval_visitor_t *v,
    int32_t dst_slot,
    int32_t src_slot)
{
    if (!v->for_slots || dst_slot < 0 || src_slot < 0 ||
        dst_slot >= ecs_vec_count(v->for_slots) ||
        src_slot >= ecs_vec_count(v->for_slots))
    {
        return;
    }

    ecs_vec_t *dst = ecs_vec_get_t(v->for_slots, ecs_vec_t, dst_slot);
    ecs_vec_t *src = ecs_vec_get_t(v->for_slots, ecs_vec_t, src_slot);
    ecs_entity_t *src_array = ecs_vec_first(src);
    int32_t i, src_count = ecs_vec_count(src);
    for (i = 0; i < src_count; i ++) {
        ecs_entity_t entity = src_array[i];
        if (!ecs_is_alive(v->world, entity)) {
            continue;
        }
        ecs_entity_t *dst_array = ecs_vec_first(dst);
        int32_t j, dst_count = ecs_vec_count(dst);
        for (j = 0; j < dst_count; j ++) {
            if (dst_array[j] == entity) {
                break;
            }
        }
        if (j == dst_count) {
            ecs_vec_append_t(NULL, dst, ecs_entity_t)[0] = entity;
        }
    }
}

void flecs_script_eval_for_leave(
    ecs_script_eval_visitor_t *v,
    flecs_script_for_state_t *state)
{
    v->vars = ecs_script_vars_pop(v->vars);
    flecs_script_for_merge_slots(v, state->for_slot, v->for_slot);
    v->for_slot = state->for_slot;
    v->force = state->force;

    if (state->collection.type) {
        ecs_ptr_free(v->world, state->collection.type, state->collection.ptr);
        state->collection = (ecs_value_t){0, NULL};
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
