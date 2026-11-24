/**
 * @file storage/nested_pairs.c
 * @brief Handles for the targets of nested pairs.
 */

#include "../private_api.h"

void flecs_nested_pairs_init(
    ecs_world_t *world)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_os_zeromem(np);
    ecs_map_init(&np->handles, &world->allocator);
    ecs_map_init(&np->elements, &world->allocator);
    ecs_vec_init_if_t(&np->ids, ecs_id_t);
    ecs_vec_init_if_t(&np->free_handles, uint32_t);
    ecs_vec_init_if_t(&np->pending_free, uint32_t);
}

void flecs_nested_pairs_fini(
    ecs_world_t *world)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_allocator_t *a = &world->allocator;

    ecs_map_iter_t it = ecs_map_iter(&np->elements);
    while (ecs_map_next(&it)) {
        ecs_vec_t *handles = ecs_map_ptr(&it);
        ecs_vec_fini_t(a, handles, uint32_t);
        ecs_os_free(handles);
    }

    ecs_map_fini(&np->elements);
    ecs_map_fini(&np->handles);
    ecs_vec_fini_t(a, &np->ids, ecs_id_t);
    ecs_vec_fini_t(a, &np->free_handles, uint32_t);
    ecs_vec_fini_t(a, &np->pending_free, uint32_t);
}

ecs_id_t flecs_nested_pair_get(
    const ecs_world_t *world,
    uint32_t handle)
{
    world = ecs_get_world(world);
    const ecs_nested_pairs_t *np = &world->nested_pairs;
    if (handle < FLECS_NESTED_PAIR_HANDLE_OFFSET) {
        return 0;
    }

    int32_t index = (int32_t)(handle - FLECS_NESTED_PAIR_HANDLE_OFFSET);
    if (index >= ecs_vec_count(&np->ids)) {
        return 0;
    }

    return ecs_vec_get_t(&np->ids, ecs_id_t, index)[0];
}

static void flecs_nested_pair_elements(
    ecs_id_t id,
    uint64_t *elements)
{
    elements[0] = ECS_PAIR_FIRST(id);
    elements[1] = 0;

    if (ECS_IS_NESTED_PAIR(id)) {
        elements[1] = ECS_NESTED_PAIR | ECS_PAIR_SECOND(id);
    } else if (!ECS_IS_VALUE_PAIR(id)) {
        elements[1] = ECS_PAIR_SECOND(id);
    }

    if (elements[1] == elements[0]) {
        elements[1] = 0;
    }
}

static void flecs_nested_pair_element_add(
    ecs_world_t *world,
    uint64_t element,
    uint32_t handle,
    uint32_t flag)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_vec_t *handles = ecs_map_ensure_alloc_t(
        &np->elements, ecs_vec_t, element);
    ecs_vec_init_if_t(handles, uint32_t);
    ecs_vec_append_t(&world->allocator, handles, uint32_t)[0] = handle;

    if (!(element & ECS_ID_FLAGS_MASK)) {
        flecs_add_flag(world, element, flag);
    }
}

static void flecs_nested_pair_element_remove(
    ecs_world_t *world,
    uint64_t element,
    uint32_t handle)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_vec_t *handles = ecs_map_get_deref(&np->elements, ecs_vec_t, element);
    if (!handles) {
        return;
    }

    int32_t i, count = ecs_vec_count(handles);
    uint32_t *array = ecs_vec_first_t(handles, uint32_t);
    for (i = 0; i < count; i ++) {
        if (array[i] == handle) {
            ecs_vec_remove_t(handles, uint32_t, i);
            break;
        }
    }

    if (!ecs_vec_count(handles)) {
        ecs_vec_fini_t(&world->allocator, handles, uint32_t);
        ecs_map_remove_free(&np->elements, element);
    }
}

uint32_t flecs_nested_pair_ensure(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_map_val_t *existing = ecs_map_get(&np->handles, id);
    if (existing) {
        return (uint32_t)existing[0];
    }

    uint32_t handle;
    if (ecs_vec_count(&np->free_handles)) {
        handle = ecs_vec_last_t(&np->free_handles, uint32_t)[0];
        ecs_vec_remove_last(&np->free_handles);
        ecs_vec_get_t(&np->ids, ecs_id_t,
            (int32_t)(handle - FLECS_NESTED_PAIR_HANDLE_OFFSET))[0] = id;
    } else {
        handle = FLECS_NESTED_PAIR_HANDLE_OFFSET +
            (uint32_t)ecs_vec_count(&np->ids);
        ecs_vec_append_t(&world->allocator, &np->ids, ecs_id_t)[0] = id;
    }

    ecs_map_insert(&np->handles, id, handle);

    uint64_t elements[2];
    flecs_nested_pair_elements(id, elements);
    flecs_nested_pair_element_add(world, elements[0], handle, EcsEntityIsId);
    if (elements[1]) {
        flecs_nested_pair_element_add(
            world, elements[1], handle, EcsEntityIsTarget);
    }

    return handle;
}

bool flecs_nested_pairs_take_element(
    ecs_world_t *world,
    uint64_t element,
    ecs_vec_t *out)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_vec_t *handles = ecs_map_get_deref(&np->elements, ecs_vec_t, element);
    if (!handles) {
        return false;
    }

    *out = *handles;
    ecs_map_remove_free(&np->elements, element);
    return true;
}

bool flecs_nested_pair_release(
    ecs_world_t *world,
    uint32_t handle)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    ecs_id_t id = flecs_nested_pair_get(world, handle);
    if (!id) {
        return false;
    }

    ecs_map_val_t *existing = ecs_map_get(&np->handles, id);
    if (!existing || ((uint32_t)existing[0] != handle)) {
        return false;
    }

    ecs_map_remove(&np->handles, id);

    uint64_t elements[2];
    flecs_nested_pair_elements(id, elements);
    flecs_nested_pair_element_remove(world, elements[0], handle);
    if (elements[1]) {
        flecs_nested_pair_element_remove(world, elements[1], handle);
    }

    ecs_vec_append_t(&world->allocator, &np->pending_free, uint32_t)[0] =
        handle;

    return true;
}

void flecs_nested_pairs_flush(
    ecs_world_t *world)
{
    ecs_nested_pairs_t *np = &world->nested_pairs;
    int32_t i, count = ecs_vec_count(&np->pending_free);
    if (!count) {
        return;
    }

    uint32_t *handles = ecs_vec_first_t(&np->pending_free, uint32_t);
    for (i = 0; i < count; i ++) {
        uint32_t handle = handles[i];
        ecs_vec_get_t(&np->ids, ecs_id_t,
            (int32_t)(handle - FLECS_NESTED_PAIR_HANDLE_OFFSET))[0] = 0;
        ecs_vec_append_t(&world->allocator, &np->free_handles, uint32_t)[0] =
            handle;
    }

    ecs_vec_clear(&np->pending_free);
}
