/**
 * @file addons/meta/meta.h
 * @brief Type support for meta addon.
 */

#ifndef FLECS_META_TYPE_SUPPORT_H
#define FLECS_META_TYPE_SUPPORT_H

#include "../meta.h"

#ifdef FLECS_META

int flecs_init_type(
    ecs_world_t *world,
    ecs_entity_t type,
    ecs_type_kind_t kind,
    ecs_size_t size,
    ecs_size_t alignment);

#define flecs_init_type_t(world, type, kind, T) \
    flecs_init_type(world, type, kind, ECS_SIZEOF(T), ECS_ALIGNOF(T))

int flecs_compare_string(
    const void *str_a,
    const void *str_b,
    const ecs_type_info_t *ti);

bool flecs_equals_string(
    const void *str_a,
    const void *str_b,
    const ecs_type_info_t *ti);

void flecs_meta_primitives_init(
    ecs_world_t *world);

void flecs_meta_enum_init(
    ecs_world_t *world);

void flecs_meta_struct_init(
    ecs_world_t *world);

void flecs_meta_array_init(
    ecs_world_t *world);

void flecs_meta_opaque_init(
    ecs_world_t *world);

void flecs_meta_units_init(
    ecs_world_t *world);

#endif

#endif
