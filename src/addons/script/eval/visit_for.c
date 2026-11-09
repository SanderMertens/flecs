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

static int flecs_script_for_query_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state)
{
    const ecs_query_t *q = node->query->query;
    if (!q) {
        flecs_script_eval_error(v, node,
            "query '%s' is not initialized", node->query->name);
        return -1;
    }

    state->kind = FlecsScriptForQuery;
    state->elem_var = -1;
    state->key_var = -1;
    state->index_var = -1;
    state->entity_var = -1;

    int32_t field_count = flecs_script_query_data_fields(q, NULL);
    int32_t var_count = node->loop_var_count;
    if (var_count != field_count && var_count != field_count + 1) {
        if (field_count) {
            flecs_script_eval_error(v, node,
                "query '%s' has %d data field%s: expected %d or %d loop "
                "variables (the first one being the matched entity), got %d",
                node->query->name, field_count, field_count == 1 ? "" : "s",
                field_count, field_count + 1, var_count);
        } else {
            flecs_script_eval_error(v, node,
                "query '%s' has no data fields: expected 1 loop variable "
                "for the matched entity, got %d", node->query->name,
                var_count);
        }
        return -1;
    }

    int8_t fields[FLECS_TERM_COUNT_MAX];
    flecs_script_query_data_fields(q, fields);

    int32_t i = 0, f;
    if (var_count == field_count + 1) {
        state->entity_var = 0;
        state->var_types[i ++] = ecs_id(ecs_entity_t);
    }

    for (f = 0; f < field_count; f ++) {
        ecs_id_t id = q->ids[fields[f]];
        ecs_entity_t type = ecs_get_typeid(v->world, id);
        if (!type || !ecs_get_type_info(v->world, type)) {
            flecs_script_eval_error(v, node,
                "cannot bind field %d of query '%s' to loop variable '%s': "
                "id '%s' has no type", fields[f] + 1, node->query->name,
                node->loop_vars[i], flecs_errstr(ecs_id_str(v->world, id)));
            return -1;
        }

        state->var_types[i ++] = type;
    }

    return 0;
}

int flecs_script_for_query_iter(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state)
{
    ecs_world_t *world = v->world;
    const ecs_query_t *q = node->query->query;
    ecs_script_var_t *var = ecs_script_vars_from_sp(v->vars, node->query_sp);
    if (!var || var->value.type != ecs_id(EcsScriptQuery)) {
        flecs_script_eval_error(v, node,
            "query '%s' is not declared", node->query->name);
        return -1;
    }

    flecs_script_for_query_t *qs = ecs_os_calloc_t(flecs_script_for_query_t);
    state->query = qs;
    qs->field_count = flecs_script_query_data_fields(q, qs->fields);

    int32_t f;
    for (f = 0; f < qs->field_count; f ++) {
        int32_t var_index = f + (state->entity_var == 0);
        qs->ti[f] = ecs_get_type_info(world, state->var_types[var_index]);
        ecs_assert(qs->ti[f] != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    qs->active = flecs_script_query_iter(var->value.ptr, &qs->it);
    return 0;
}

void flecs_script_for_fini(
    ecs_script_eval_visitor_t *v,
    flecs_script_for_state_t *state)
{
    flecs_script_for_query_t *qs = state->query;
    if (qs) {
        if (qs->active) {
            ecs_iter_fini(&qs->it);
        }

        int32_t f;
        for (f = 0; f < qs->field_count; f ++) {
            if (qs->defaults[f]) {
                flecs_type_info_dtor(qs->defaults[f], 1, qs->ti[f]);
                ecs_os_free(qs->defaults[f]);
            }
        }

        ecs_os_free(qs->entities);
        ecs_os_free(qs);
        state->query = NULL;
    }

    if (state->collection.type) {
        ecs_ptr_free(v->world, state->collection.type, state->collection.ptr);
        state->collection.type = 0;
    }
}

int flecs_script_for_init(
    ecs_script_eval_visitor_t *v,
    ecs_script_for_t *node,
    flecs_script_for_state_t *state,
    const ecs_value_t *collection)
{
    if (node->query) {
        return flecs_script_for_query_init(v, node, state);
    }

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
        if (state->kind == FlecsScriptForQuery) {
            var->type_info = ecs_get_type_info(v->world, var->value.type);
            continue;
        }

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

    if (node->query) {
    } else if (!node->expr) {
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
            node->expr && !node->query ? &state->collection : NULL) ||
        (node->query && flecs_script_for_query_iter(v, node, state)) ||
        flecs_script_for_declare_vars(v, node, state,
            state->kind == FlecsScriptForRange))
    {
        flecs_script_for_fini(v, state);
        return -1;
    }

    v->for_slot = node->for_slot;
    v->force = true;
    return 0;
}

static void* flecs_script_for_query_field(
    flecs_script_for_query_t *qs,
    int32_t f)
{
    ecs_iter_t *it = &qs->it;
    int8_t field = qs->fields[f];
    const ecs_type_info_t *ti = qs->ti[f];
    const void *ptr = NULL;
    if (ecs_field_is_set(it, field) &&
        ecs_field_size(it, field) == (size_t)ti->size)
    {
        ecs_entity_t src = ecs_field_src(it, field);
        if (!src) {
            src = qs->entity;
        }

        if (src && ecs_is_alive(it->world, src)) {
            ptr = ecs_get_id(it->world, src, ecs_field_id(it, field));
        }
    }

    if (!ptr) {
        if (!qs->defaults[f]) {
            qs->defaults[f] = ecs_os_malloc(ti->size);
            flecs_type_info_ctor(qs->defaults[f], 1, ti);
        }

        ptr = qs->defaults[f];
    }

    return ECS_CONST_CAST(void*, ptr);
}

static bool flecs_script_for_query_next(
    flecs_script_for_state_t *state)
{
    flecs_script_for_query_t *qs = state->query;
    if (!qs) {
        return false;
    }

    do {
        while (qs->row >= qs->count) {
            if (!qs->active || !ecs_query_next(&qs->it)) {
                qs->active = false;
                return false;
            }

            qs->row = 0;
            qs->count = qs->it.count ? qs->it.count : 1;
            if (qs->it.count) {
                if (qs->entities_size < qs->it.count) {
                    qs->entities = ecs_os_realloc_n(
                        qs->entities, ecs_entity_t, qs->it.count);
                    qs->entities_size = qs->it.count;
                }

                ecs_os_memcpy_n(qs->entities, qs->it.entities,
                    ecs_entity_t, qs->it.count);
            }
        }

        qs->entity = qs->it.count ? qs->entities[qs->row] : 0;
        qs->row ++;
    } while (qs->entity && !ecs_is_alive(qs->it.world, qs->entity));

    ecs_script_var_t *vars = &((ecs_script_var_t*)
        state->vars->vars.array)[state->var_base];
    int32_t f, var = 0;
    if (state->entity_var == 0) {
        vars[var ++].value.ptr = &qs->entity;
    }

    for (f = 0; f < qs->field_count; f ++) {
        vars[var ++].value.ptr = flecs_script_for_query_field(qs, f);
    }

    return true;
}

bool flecs_script_for_next(
    flecs_script_for_state_t *state)
{
    if (state->kind == FlecsScriptForQuery) {
        return flecs_script_for_query_next(state);
    }

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
    case FlecsScriptForQuery:
        return false;
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
    flecs_script_for_fini(v, state);
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
