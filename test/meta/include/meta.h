#ifndef META_H
#define META_H

/* This generated file contains includes for project dependencies */
#include <meta/bake_config.h>

#ifdef __cplusplus
extern "C" {
#endif

void _meta_test_struct(
    ecs_world_t *world, 
    ecs_entity_t t, 
    ecs_size_t size, 
    ecs_size_t alignment);

void _meta_test_member(
    ecs_world_t *world, 
    ecs_entity_t t, 
    const char *name,
    ecs_entity_t type,
    ecs_size_t offset);

#define meta_test_struct(world, t, T)\
    _meta_test_struct(world, t, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define meta_test_member(world, t, T, name, type)\
    _meta_test_member(world, t, #name, type, offsetof(T, name))

#ifdef __cplusplus
}
#endif

#endif

