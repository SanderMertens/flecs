
#ifndef REFLECS_H
#define REFLECS_H

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

typedef enum EcsSystemKind {
    EcsPeriodic,
    EcsOnInit,
    EcsOnDeinit
} EcsSystemKind;

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
    EcsSystemKind kind;
    bool enabled;
} EcsSystem;


/* -- Start / stop reflecs -- */

void ecs_init(void);

void ecs_fini(void);


/* -- World API -- */

REFLECS_EXPORT
EcsWorld* ecs_world_new(void);

REFLECS_EXPORT
void ecs_world_delete(
    EcsWorld *world);

REFLECS_EXPORT
EcsEntity* ecs_world_lookup(
    EcsWorld *world,
    const char *id);

REFLECS_EXPORT
void ecs_world_progress(
    EcsWorld *world);

REFLECS_EXPORT
void ecs_world_set_context(
    EcsWorld *world,
    void *ctx);

REFLECS_EXPORT
void* ecs_world_get_context(
    EcsWorld *world);

#define ecs_world_iter_systems(world)\
    _ecs_world_iter_systems(world, alloca(sizeof(EcsVectorIter)))

REFLECS_EXPORT
EcsIter _ecs_world_iter_systems(
    EcsWorld *world,
    EcsVectorIter *iter_data);

/* -- Entity API -- */

REFLECS_EXPORT
EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id);

REFLECS_EXPORT
void ecs_delete(
    EcsEntity *entity);

REFLECS_EXPORT
void ecs_stage(
    EcsEntity *entity,
    EcsEntity *component);

REFLECS_EXPORT
void* ecs_add(
    EcsEntity *entity,
    EcsEntity *component);

REFLECS_EXPORT
void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component);

REFLECS_EXPORT
void ecs_remove(
    EcsEntity *entity,
    EcsEntity *component);

REFLECS_EXPORT
EcsResult ecs_commit(
    EcsEntity *entity);

REFLECS_EXPORT
const char* ecs_get_id(
    EcsEntity *entity);

REFLECS_EXPORT
EcsWorld* ecs_get_world(
    EcsEntity *entity);


/* -- Component API -- */

#define ECS_COMPONENT(world, id) EcsEntity *id##_e = ecs_component_new(world, #id, sizeof(id)); (void)id##_e;

REFLECS_EXPORT
EcsEntity* ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size);


/* -- System API -- */

#define ECS_SYSTEM(world, id, kind, ...) \
    void id(EcsEntity*, EcsEntity*, void*[]);\
    EcsEntity *id##_e = ecs_system_new(world, #id, kind, #__VA_ARGS__, id);\
    (void)id##_e;

REFLECS_EXPORT
EcsEntity *ecs_system_new(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action);

REFLECS_EXPORT
EcsResult ecs_system_enable(
    EcsEntity *entity,
    bool enabled);

REFLECS_EXPORT
bool ecs_system_is_enabled(
    EcsEntity *entity);

/* -- Utilities -- */

REFLECS_EXPORT
void ecs_hash(
    const void *key,
    size_t length,
    uint64_t *result);

#ifdef __cplusplus
}
#endif

#endif
