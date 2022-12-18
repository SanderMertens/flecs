/**
 * @file value.c
 * @brief Utility functions to work with non-trivial pointers of user types.
 */

#include "private_api.h"

int ecs_value_init_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    ecs_xtor_t ctor;
    if ((ctor = ti->hooks.ctor)) {
        ctor(ptr, 1, ti);
    } else {
        ecs_os_memset(ptr, 0, ti->size);
    }

    return 0;
error:
    return -1;
}

int ecs_value_init(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_init_w_type_info(world, ti, ptr);
error:
    return -1;
}

void* ecs_value_new(
    ecs_world_t *world,
    ecs_entity_t type)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");

    void *result = flecs_alloc(&world->allocator, ti->size);
    if (ecs_value_init_w_type_info(world, ti, result) != 0) {
        flecs_free(&world->allocator, ti->size, result);
        goto error;
    }

    return result;
error:
    return NULL;
}

int ecs_value_fini_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    ecs_xtor_t dtor;
    if ((dtor = ti->hooks.dtor)) {
        dtor(ptr, 1, ti);
    }

    return 0;
error:
    return -1;
}

int ecs_value_fini(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* ptr)
{
    ecs_poly_assert(world, ecs_world_t);
    (void)world;
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_fini_w_type_info(world, ti, ptr);
error:
    return -1;
}

int ecs_value_free(
    ecs_world_t *world,
    ecs_entity_t type,
    void* ptr)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    if (ecs_value_fini_w_type_info(world, ti, ptr) != 0) {
        goto error;
    }

    flecs_free(&world->allocator, ti->size, ptr);

    return 0;
error:
    return -1;
}

int ecs_value_copy_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    const void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    ecs_copy_t copy;
    if ((copy = ti->hooks.copy)) {
        copy(dst, src, 1, ti);
    } else {
        ecs_os_memcpy(dst, src, ti->size);
    }

    return 0;
error:
    return -1;
}

int ecs_value_copy(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    const void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_copy_w_type_info(world, ti, dst, src);
error:
    return -1;
}

int ecs_value_move_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    ecs_move_t move;
    if ((move = ti->hooks.move)) {
        move(dst, src, 1, ti);
    } else {
        ecs_os_memcpy(dst, src, ti->size);
    }

    return 0;
error:
    return -1;
}

int ecs_value_move(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_move_w_type_info(world, ti, dst, src);
error:
    return -1;
}

int ecs_value_move_ctor_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    ecs_move_t move;
    if ((move = ti->hooks.move_ctor)) {
        move(dst, src, 1, ti);
    } else {
        ecs_os_memcpy(dst, src, ti->size);
    }

    return 0;
error:
    return -1;
}

int ecs_value_move_ctor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src)
{
    ecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_move_w_type_info(world, ti, dst, src);
error:
    return -1;
}
