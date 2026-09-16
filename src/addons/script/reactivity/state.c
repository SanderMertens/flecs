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

static void flecs_script_for_slot_init(
    ecs_script_for_slot_t *slot)
{
    slot->scope_slot = -1;
    ecs_vec_init_t(NULL, &slot->entities, ecs_entity_t, 0);
    ecs_map_init(&slot->named, NULL);
}

static void flecs_script_for_slot_fini(
    ecs_script_for_slot_t *slot)
{
    ecs_vec_fini_t(NULL, &slot->entities, ecs_entity_t);
    ecs_map_iter_t it = ecs_map_iter(&slot->named);
    while (ecs_map_next(&it)) {
        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        ecs_vec_fini_t(NULL, &entry->components, ecs_script_for_component_t);
        ecs_os_free(entry);
    }
    ecs_map_fini(&slot->named);
}

void flecs_script_for_slots_init(
    ecs_vec_t *for_slots,
    int32_t count)
{
    flecs_script_for_slots_fini(for_slots);
    ecs_vec_init_t(NULL, for_slots, ecs_script_for_slot_t, count);
    ecs_vec_set_count_t(NULL, for_slots, ecs_script_for_slot_t, count);
    ecs_script_for_slot_t *slots = ecs_vec_first(for_slots);
    int32_t i;
    for (i = 0; i < count; i ++) {
        flecs_script_for_slot_init(&slots[i]);
    }
}

void flecs_script_for_slots_fini(
    ecs_vec_t *for_slots)
{
    ecs_script_for_slot_t *slots = ecs_vec_first(for_slots);
    int32_t i, count = ecs_vec_count(for_slots);
    for (i = 0; i < count; i ++) {
        flecs_script_for_slot_fini(&slots[i]);
    }
    ecs_vec_fini_t(NULL, for_slots, ecs_script_for_slot_t);
}

static void flecs_script_for_slot_delete_named(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    bool all,
    int32_t visit)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->named);
    while (ecs_map_next(&it)) {
        ecs_entity_t entity = ecs_map_key(&it);
        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        bool alive = ecs_is_alive(world, entity);
        if (!all && alive && entry->visit == visit) {
            ecs_script_for_component_t *components =
                ecs_vec_first(&entry->components);
            int32_t c, component_count = ecs_vec_count(&entry->components);
            for (c = component_count - 1; c >= 0; c --) {
                if (components[c].visit != visit) {
                    ecs_remove_id(world, entity, components[c].component);
                    ecs_vec_remove_t(&entry->components,
                        ecs_script_for_component_t, c);
                }
            }
            continue;
        }
        if (alive) {
            ecs_delete(world, entity);
        }
        ecs_vec_fini_t(NULL, &entry->components, ecs_script_for_component_t);
        ecs_map_remove_free(&slot->named, entity);
        slot->cache_entity = 0;
        slot->cache_entry = NULL;
    }
}

void flecs_script_for_slot_clear(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    bool delete_named)
{
    ecs_entity_t *array = ecs_vec_first(&slot->entities);
    int32_t i, count = ecs_vec_count(&slot->entities);
    for (i = 0; i < count; i ++) {
        if (ecs_is_alive(world, array[i])) {
            ecs_delete(world, array[i]);
        }
    }
    ecs_vec_clear(&slot->entities);

    if (delete_named) {
        flecs_script_for_slot_delete_named(world, slot, true, 0);
    }
}

void flecs_script_for_slot_purge(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    int32_t visit)
{
    flecs_script_for_slot_delete_named(world, slot, false, visit);
}

void flecs_script_for_slot_mark(
    ecs_script_for_slot_t *slot,
    int32_t visit)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->named);
    while (ecs_map_next(&it)) {
        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        entry->visit = visit;
        ecs_script_for_component_t *components =
            ecs_vec_first(&entry->components);
        int32_t c, component_count = ecs_vec_count(&entry->components);
        for (c = 0; c < component_count; c ++) {
            components[c].visit = visit;
        }
    }
}

void flecs_script_for_slot_track(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    int32_t visit,
    bool *named)
{
    if (ecs_get_name(world, entity)) {
        ecs_script_for_entry_t *entry = ecs_map_ensure_alloc_t(
            &slot->named, ecs_script_for_entry_t, entity);
        ecs_vec_init_if_t(&entry->components, ecs_script_for_component_t);
        entry->visit = visit;
        slot->cache_entity = entity;
        slot->cache_entry = entry;
        *named = true;
        return;
    }

    *named = false;
    ecs_entity_t *array = ecs_vec_first(&slot->entities);
    int32_t i, count = ecs_vec_count(&slot->entities);
    for (i = 0; i < count; i ++) {
        if (array[i] == entity) {
            return;
        }
    }
    ecs_vec_append_t(NULL, &slot->entities, ecs_entity_t)[0] = entity;
}

void flecs_script_for_slot_track_component(
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    ecs_id_t component,
    int32_t visit)
{
    ecs_script_for_entry_t *entry;
    if (slot->cache_entity == entity) {
        entry = slot->cache_entry;
    } else {
        entry = ecs_map_get_deref(&slot->named, ecs_script_for_entry_t, entity);
        if (!entry) {
            return;
        }
        slot->cache_entity = entity;
        slot->cache_entry = entry;
    }

    ecs_script_for_component_t *components = ecs_vec_first(&entry->components);
    int32_t i, count = ecs_vec_count(&entry->components);
    for (i = 0; i < count; i ++) {
        if (components[i].component == component) {
            components[i].visit = visit;
            return;
        }
    }

    ecs_script_for_component_t *elem = ecs_vec_append_t(
        NULL, &entry->components, ecs_script_for_component_t);
    elem->component = component;
    elem->visit = visit;
}

#endif
