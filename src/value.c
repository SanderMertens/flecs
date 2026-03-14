/**
 * @file value.c
 * @brief Type-safe value operations (init, copy, move, free) using type hooks.
 */

#include "private_api.h"

/* Initialize a value using its type info. Falls back to zero-init if no ctor. */
int ecs_value_init_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    if (!flecs_type_info_ctor(ptr, 1, ti)) {
        ecs_os_memset(ptr, 0, ti->size);
    }

    return 0;
error:
    return -1;
}

/* Initialize a value by looking up type info from entity. */
int ecs_value_init(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity '%s' is not a type", 
        flecs_errstr(ecs_get_path(world, type)));
    return ecs_value_init_w_type_info(world, ti, ptr);
error:
    return -1;
}

/* Allocate and initialize a new value. Caller must free with ecs_value_free. */
void* ecs_value_new_w_type_info(
    ecs_world_t *world,
    const ecs_type_info_t *ti)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    void *result = flecs_alloc_w_dbg_info(
        &world->allocator, ti->size, ti->name);
    if (ecs_value_init_w_type_info(world, ti, result) != 0) {
        flecs_free(&world->allocator, ti->size, result);
        goto error;
    }

    return result;
error:
    return NULL;
}

/* Allocate and initialize a new value by entity type. */
void* ecs_value_new(
    ecs_world_t *world,
    ecs_entity_t type)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");

    return ecs_value_new_w_type_info(world, ti);
error:
    return NULL;
}

/* Destruct a value without freeing memory. */
int ecs_value_fini_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void *ptr)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    flecs_type_info_dtor(ptr, 1, ti);

    return 0;
error:
    return -1;
}

/* Destruct a value by entity type without freeing memory. */
int ecs_value_fini(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* ptr)
{
    flecs_poly_assert(world, ecs_world_t);
    (void)world;
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_fini_w_type_info(world, ti, ptr);
error:
    return -1;
}

/* Destruct and free a value allocated with ecs_value_new. */
int ecs_value_free(
    ecs_world_t *world,
    ecs_entity_t type,
    void* ptr)
{
    flecs_poly_assert(world, ecs_world_t);
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

/* Copy a value using type info hooks. */
int ecs_value_copy_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    const void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    flecs_type_info_copy(dst, src, 1, ti);

    return 0;
error:
    return -1;
}

/* Copy a value by entity type. */
int ecs_value_copy(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    const void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_copy_w_type_info(world, ti, dst, src);
error:
    return -1;
}

/* Move a value using type info hooks. */
int ecs_value_move_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    flecs_type_info_move(dst, src, 1, ti);

    return 0;
error:
    return -1;
}

/* Move a value by entity type. */
int ecs_value_move(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_move_w_type_info(world, ti, dst, src);
error:
    return -1;
}

/* Move-construct a value using type info hooks. */
int ecs_value_move_ctor_w_type_info(
    const ecs_world_t *world,
    const ecs_type_info_t *ti,
    void* dst,
    void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    flecs_type_info_move_ctor(dst, src, 1, ti);

    return 0;
error:
    return -1;
}

/* Move a value by entity type (delegates to ecs_value_move_w_type_info). */
int ecs_value_move_ctor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void* dst,
    void *src)
{
    flecs_poly_assert(world, ecs_world_t);
    const ecs_type_info_t *ti = ecs_get_type_info(world, type);
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, "entity is not a type");
    return ecs_value_move_w_type_info(world, ti, dst, src);
error:
    return -1;
}
