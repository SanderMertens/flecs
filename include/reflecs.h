
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

#ifdef __cplusplus
extern "C" {
#endif


/* -- Types -- */

typedef enum EcsResult {
    EcsOk,
    EcsError
} EcsResult;

typedef struct EcsWorld EcsWorld;
typedef struct EcsEntity EcsEntity;


/* -- Builtin component types -- */

typedef struct EcsType {
    uint32_t size;
} EcsType;


/* -- Builtin component entities -- */


/* -- Start / stop reflecs -- */

void ecs_init(void);

void ecs_fini(void);


/* -- Entity API -- */

EcsWorld* ecs_world_new(void);

EcsWorld* ecs_world_delete(
    uint32_t initial_size);

EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id);

void ecs_delete(
    EcsEntity *entity);

void ecs_add(
    EcsEntity *entity,
    EcsEntity *component);

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


/* -- Utilities -- */

void ecs_hash(
    const void *key,
    size_t length,
    uint64_t *result);

#ifdef __cplusplus
}
#endif

#endif
