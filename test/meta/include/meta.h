#ifndef META_H
#define META_H

/* This generated file contains includes for project dependencies */
#include <meta/bake_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Multiline strings */
#define HEAD
#define LINE "\n"

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
    int32_t count,
    ecs_size_t offset);

#define test_ok(expr)\
    test_int((expr), 0)

#define test_fail(expr)\
    test_int((expr), -1)

#define meta_test_struct(world, t, T)\
    _meta_test_struct(world, t, ECS_SIZEOF(T), ECS_ALIGNOF(T))

#define meta_test_member(world, t, T, name, type, count)\
    _meta_test_member(world, t, #name, type, count, offsetof(T, name))

typedef struct {
    int32_t x, y;
} Position, Velocity;

typedef struct {
    int32_t value;
} Mass;

#ifdef __cplusplus
}
#endif

#endif

