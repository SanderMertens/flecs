
#ifndef REFLECS_H
#define REFLECS_H

#if REFLECS_IMPL && defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllexport)
#elif REFLECS_IMPL
#define REFLECS_EXPORT __attribute__((__visibility__("default")))
#elif defined _MSC_VER
#define REFLECS_EXPORT __declspec(dllimport)
#else
#define REFLECS_EXPORT
#endif

#include <reflecs/platform.h>
#include <reflecs/vector.h>
#include <reflecs/array.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -- Types -- */

typedef struct EcsWorld EcsWorld;
typedef struct EcsEntity EcsEntity;


/* -- Builtin component types -- */

typedef void (*EcsSystemAction)(
    EcsEntity *system,
    EcsEntity *entity,
    void *data[]);

typedef struct EcsComponent {
    uint32_t size;
} EcsComponent;

typedef struct EcsSystem {
    EcsSystemAction action;
    EcsArray *components;
    EcsVector *tables;
    EcsVectorParams tables_params;
    bool enabled;
} EcsSystem;


/* -- Start / stop reflecs -- */

void ecs_init(void);

void ecs_fini(void);


/* -- World API -- */

EcsWorld* ecs_world_new(void);

void ecs_world_delete(
    EcsWorld *world);

void ecs_world_progress(
    EcsWorld *world);

/* -- Entity API -- */

EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id);

void ecs_delete(
    EcsEntity *entity);

void _ecs_stage(
    EcsEntity *entity,
    EcsEntity *component);

#define ecs_stage(entity, component) _ecs_stage(entity, component##_e)

void* _ecs_add(
    EcsEntity *entity,
    EcsEntity *component);

#define ecs_add(entity, component) _ecs_add(entity, component##_e)

void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component);

void ecs_remove(
    EcsEntity *entity,
    EcsEntity *component);

EcsResult ecs_commit(
    EcsEntity *entity);

EcsEntity* ecs_lookup(
    EcsWorld *world,
    const char *id);

const char* ecs_idof(
    EcsEntity *entity);

/* -- Component API -- */

#define ECS_COMPONENT(world, id) EcsEntity *id##_e = ecs_component_new(world, #id, sizeof(id)); (void)id##_e;

EcsEntity* ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size);


/* -- System API -- */

#define ECS_SYSTEM(world, id, ...) EcsEntity *id##_e = ecs_system_new(world, #id, #__VA_ARGS__, id); (void)id##_e;

EcsEntity *ecs_system_new(
    EcsWorld *world,
    const char *id,
    const char *sig,
    EcsSystemAction action);

#define ecs_system_enable(e, enabled) _ecs_system_enable(e##_e, enabled)

EcsResult _ecs_system_enable(
    EcsEntity *entity,
    bool enabled);


/* -- Utilities -- */

void ecs_hash(
    const void *key,
    size_t length,
    uint64_t *result);

#ifdef __cplusplus
}
#endif

#endif
