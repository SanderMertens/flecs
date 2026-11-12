#include "flecs.h"

#ifdef FLECS_SCRIPT
#include "../script.h"

static void flecs_script_for_slot_init(
    ecs_script_for_slot_t *slot)
{
    slot->scope_slot = -1;
    ecs_map_init(&slot->entries, NULL);
    ecs_map_init(&slot->keys, NULL);
}

static void flecs_script_for_slot_fini(
    ecs_script_for_slot_t *slot)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->entries);
    while (ecs_map_next(&it)) {
        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        ecs_vec_fini_t(NULL, &entry->components, ecs_script_for_component_t);
        ecs_os_free(entry);
    }
    ecs_map_fini(&slot->entries);
    ecs_map_fini(&slot->keys);
}

static void flecs_script_for_slots_fini(
    ecs_vec_t *for_slots)
{
    ecs_script_for_slot_t *slots = ecs_vec_first(for_slots);
    int32_t i, count = ecs_vec_count(for_slots);
    for (i = 0; i < count; i ++) {
        flecs_script_for_slot_fini(&slots[i]);
    }

    ecs_vec_fini_t(NULL, for_slots, ecs_script_for_slot_t);
}

static void flecs_script_for_slots_init(
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

void flecs_script_query_slots_clear(
    ecs_vec_t *query_slots)
{
    ecs_script_query_slot_t *slots = ecs_vec_first(query_slots);
    int32_t i, j, count = ecs_vec_count(query_slots);
    for (i = 0; i < count; i ++) {
        for (j = 0; j < 2; j ++) {
            ecs_entity_t observer = slots[i].observers[j];
            if (slots[i].world && observer &&
                ecs_is_alive(slots[i].world, observer))
            {
                ecs_delete(slots[i].world, observer);
            }

            slots[i].observers[j] = 0;
        }
    }
}

static void flecs_script_query_slots_init(
    ecs_vec_t *query_slots,
    int32_t count)
{
    flecs_script_query_slots_clear(query_slots);
    ecs_vec_set_count_t(NULL, query_slots, ecs_script_query_slot_t, count);
    ecs_os_memset_n(ecs_vec_first(query_slots), 0,
        ecs_script_query_slot_t, count);
}

static void flecs_script_for_slot_mark(
    ecs_script_for_slot_t *slot,
    int32_t visit)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->entries);
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

void flecs_script_slots_reset(
    ecs_vec_t *slots,
    ecs_size_t size,
    int32_t count)
{
    ecs_vec_set_count(NULL, slots, size, count);
    if (count) {
        ecs_os_memset(ecs_vec_first(slots), 0, size * count);
    }
}

void flecs_script_state_init(
    ecs_script_state_t *state)
{
    ecs_vec_init_t(NULL, &state->symbol_slots, ecs_script_symbol_slot_t, 0);
    ecs_vec_init_t(NULL, &state->component_slots, ecs_script_component_slot_t, 0);
    ecs_vec_init_t(NULL, &state->scope_slots, int32_t, 0);
    ecs_vec_init_t(NULL, &state->for_slots, ecs_script_for_slot_t, 0);
    ecs_vec_init_t(NULL, &state->query_slots, ecs_script_query_slot_t, 0);
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
    flecs_script_query_slots_clear(&state->query_slots);
    ecs_vec_fini_t(NULL, &state->query_slots, ecs_script_query_slot_t);
}

void flecs_script_state_resize(
    ecs_script_state_t *state,
    int32_t scope_count,
    int32_t component_count,
    int32_t for_count,
    int32_t query_count,
    int32_t computed_count)
{
    flecs_script_state_clear_computed(state);
    flecs_script_slots_reset_t(&state->computed, ecs_script_computed_t,
        computed_count);
    flecs_script_slots_reset_t(&state->scope_slots, int32_t, scope_count);
    flecs_script_slots_reset_t(&state->component_slots,
        ecs_script_component_slot_t, component_count);
    flecs_script_for_slots_init(&state->for_slots, for_count);
    flecs_script_query_slots_init(&state->query_slots, query_count);
}

ecs_entity_t flecs_script_state_component_src(
    const ecs_world_t *world,
    const ecs_script_state_t *state,
    const ecs_script_component_slot_t *slot)
{
    if (slot->entity_slot < 0 ||
        slot->entity_slot >= ecs_vec_count(&state->symbol_slots))
    {
        return 0;
    }

    ecs_entity_t entity = ecs_vec_get_t(&state->symbol_slots,
        ecs_script_symbol_slot_t, slot->entity_slot)->entity;
    return entity
        ? flecs_script_get_src(world, entity, slot->component)
        : 0;
}

int32_t flecs_script_state_next(
    ecs_script_state_t *state)
{
    if (state->visit == INT32_MAX) {
        flecs_script_slots_reset_t(&state->scope_slots, int32_t,
            ecs_vec_count(&state->scope_slots));
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

void flecs_script_for_slot_remove_key(
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    uint64_t key)
{
    if (!key) {
        return;
    }

    ecs_map_val_t *cur = ecs_map_get(&slot->keys, key);
    if (cur && (ecs_entity_t)cur[0] == entity) {
        ecs_map_remove(&slot->keys, key);
    }
}

void flecs_script_for_slot_purge(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    int32_t visit)
{
    ecs_map_iter_t it = ecs_map_iter(&slot->entries);
    while (ecs_map_next(&it)) {
        ecs_entity_t entity = ecs_map_key(&it);
        ecs_script_for_entry_t *entry = ecs_map_ptr(&it);
        bool alive = ecs_is_alive(world, entity);
        if (alive && entry->visit == visit) {
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

        flecs_script_for_slot_remove_key(slot, entity, entry->key);
        ecs_vec_fini_t(NULL, &entry->components, ecs_script_for_component_t);
        ecs_map_remove_free(&slot->entries, entity);
        slot->cache_entity = 0;
        slot->cache_entry = NULL;
    }
}

void flecs_script_for_slot_clear(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot)
{
    flecs_script_for_slot_purge(world, slot, -1);
}

uint64_t flecs_script_for_key(
    uint64_t outer,
    uint64_t value)
{
    uint64_t data[2] = { outer, value };
    uint64_t result = flecs_hash(data, ECS_SIZEOF(data));
    return result ? result : 1;
}

ecs_entity_t flecs_script_for_slot_find(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    uint64_t *key,
    ecs_entity_t parent,
    int32_t visit)
{
    ecs_map_val_t *cur;
    while ((cur = ecs_map_get(&slot->keys, *key))) {
        ecs_entity_t entity = (ecs_entity_t)cur[0];
        ecs_script_for_entry_t *entry = ecs_map_get_deref(
            &slot->entries, ecs_script_for_entry_t, entity);
        if (entry && entry->visit == visit) {
            *key = flecs_script_for_key(*key, 0);
            continue;
        }

        if (ecs_is_alive(world, entity) &&
            ecs_get_target(world, entity, EcsChildOf, 0) == parent)
        {
            return entity;
        }

        return 0;
    }

    return 0;
}

bool flecs_script_for_slot_track(
    ecs_world_t *world,
    ecs_script_for_slot_t *slot,
    ecs_entity_t entity,
    uint64_t key,
    int32_t visit)
{
    if (!key && !ecs_get_name(world, entity)) {
        return false;
    }

    ecs_script_for_entry_t *entry = ecs_map_ensure_alloc_t(
        &slot->entries, ecs_script_for_entry_t, entity);
    ecs_vec_init_if_t(&entry->components, ecs_script_for_component_t);
    entry->visit = visit;
    if (key && entry->key != key) {
        flecs_script_for_slot_remove_key(slot, entity, entry->key);
        entry->key = key;
        ecs_map_ensure(&slot->keys, key)[0] = (ecs_map_val_t)entity;
    }

    slot->cache_entity = entity;
    slot->cache_entry = entry;
    return true;
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
        entry = ecs_map_get_deref(&slot->entries, ecs_script_for_entry_t, entity);
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
