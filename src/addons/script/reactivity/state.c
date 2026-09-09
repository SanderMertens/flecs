#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

void flecs_script_state_init(
    ecs_script_state_t *state)
{
    ecs_vec_init_t(NULL, &state->symbol_slots, ecs_script_symbol_slot_t, 0);
    ecs_vec_init_t(NULL, &state->component_slots, ecs_script_component_slot_t, 0);
    ecs_vec_init_t(NULL, &state->scope_slots, int32_t, 0);
    ecs_vec_init_t(NULL, &state->for_slots, ecs_script_for_slot_t, 0);
    ecs_vec_init_t(NULL, &state->computed, ecs_script_computed_t, 0);
    state->visit = 0;
    state->initialized = false;
}

void flecs_script_state_clear_computed(
    ecs_script_state_t *state)
{
    ecs_script_computed_t *slots = ecs_vec_first(&state->computed);
    for (int32_t i = 0; i < ecs_vec_count(&state->computed); i ++) {
        if (slots[i].ptr) {
            if (slots[i].ti && slots[i].ti->hooks.dtor) {
                flecs_type_info_dtor(slots[i].ptr, 1, slots[i].ti);
            }
            ecs_os_free(slots[i].ptr);
        }
        slots[i] = (ecs_script_computed_t){0};
    }
}

void flecs_script_state_fini(
    ecs_script_state_t *state)
{
    flecs_script_state_clear_computed(state);
    ecs_vec_fini_t(NULL, &state->computed, ecs_script_computed_t);
    ecs_vec_fini_t(NULL, &state->symbol_slots, ecs_script_symbol_slot_t);
    ecs_vec_fini_t(NULL, &state->component_slots, ecs_script_component_slot_t);
    ecs_vec_fini_t(NULL, &state->scope_slots, int32_t);
    flecs_script_for_slots_fini(&state->for_slots);
}

void flecs_script_state_resize_computed(
    ecs_script_state_t *state,
    int32_t count)
{
    flecs_script_state_clear_computed(state);
    ecs_vec_set_count_t(NULL, &state->computed, ecs_script_computed_t, count);
    if (count) {
        ecs_os_memset(ecs_vec_first(&state->computed), 0,
            count * ECS_SIZEOF(ecs_script_computed_t));
    }
}

void flecs_script_state_resize(
    ecs_script_state_t *state,
    int32_t scope_count,
    int32_t component_count,
    int32_t for_count)
{
    ecs_vec_set_count_t(NULL, &state->scope_slots, int32_t, scope_count);
    if (scope_count) {
        ecs_os_memset(ecs_vec_first(&state->scope_slots), 0,
            scope_count * ECS_SIZEOF(int32_t));
    }
    ecs_vec_set_count_t(NULL, &state->component_slots,
        ecs_script_component_slot_t, component_count);
    if (component_count) {
        ecs_os_memset(ecs_vec_first(&state->component_slots), 0,
            component_count * ECS_SIZEOF(ecs_script_component_slot_t));
    }
    flecs_script_for_slots_init(&state->for_slots, for_count);
}

int32_t flecs_script_state_next(
    ecs_script_state_t *state)
{
    if (state->visit == INT32_MAX) {
        if (ecs_vec_count(&state->scope_slots)) {
            ecs_os_memset(ecs_vec_first(&state->scope_slots), 0,
                ecs_vec_count(&state->scope_slots) * ECS_SIZEOF(int32_t));
        }
        state->visit = 0;
    }
    return ++ state->visit;
}

void flecs_script_state_mark(
    ecs_script_state_t *state,
    const ecs_script_region_t *region,
    int32_t visit)
{
    int32_t scope_count = ecs_vec_count(&state->scope_slots);
    int32_t *scopes = ecs_vec_first(&state->scope_slots);
    for (int32_t i = 0; i < region->scope_count; i ++) {
        int32_t slot = region->scope_first + i;
        if (slot >= 0 && slot < scope_count) {
            scopes[slot] = visit;
        }
    }
    int32_t for_count = ecs_vec_count(&state->for_slots);
    for (int32_t i = 0; i < region->for_count; i ++) {
        int32_t slot = region->for_first + i;
        if (slot >= 0 && slot < for_count) {
            flecs_script_for_slot_mark(ecs_vec_get_t(
                &state->for_slots, ecs_script_for_slot_t, slot), visit);
        }
    }
}

#endif
