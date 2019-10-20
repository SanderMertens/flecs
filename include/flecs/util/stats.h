#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemStats {
    ecs_entity_t handle;
    const char *id;
    const char *signature;
    EcsSystemKind kind;
    uint32_t tables_matched;
    uint32_t entities_matched;
    float period;
    float time_spent;
    bool enabled;
    bool active;
    bool is_hidden;
} EcsSystemStats;

typedef struct EcsFeatureStats {
    const char *id;
    char *entities;
    uint32_t system_count;
    uint32_t systems_enabled;
    bool is_hidden;
} EcsFeatureStats;

typedef struct EcsComponentStats {
    ecs_entity_t handle;
    const char *id;
    uint16_t size;
    uint32_t memory_used;
    uint32_t memory_allocd;
    uint32_t entities;
    uint32_t tables;
} EcsComponentStats;

typedef struct EcsMemoryStat {
    uint32_t allocd;
    uint32_t used;
} EcsMemoryStat;

typedef struct EcsMemoryStats {
    EcsMemoryStat total;
    EcsMemoryStat components;
    EcsMemoryStat entities;
    EcsMemoryStat systems;
    EcsMemoryStat families;
    EcsMemoryStat tables;
    EcsMemoryStat stage;
    EcsMemoryStat world;
} EcsMemoryStats;

typedef struct EcsWorldStats {
    uint32_t system_count;
    uint32_t table_count;
    uint32_t component_count;
    uint32_t entity_count;
    uint32_t thread_count;
    uint32_t tick_count;
    double system_time;
    double frame_time;
    double merge_time;
} EcsWorldStats;

typedef struct ecs_world_stats_t {
    uint32_t system_count;
    uint32_t table_count;
    uint32_t component_count;
    uint32_t entity_count;
    uint32_t thread_count;
    uint32_t tick_count;
    float system_time;
    float frame_time;
    float merge_time;
    EcsMemoryStats memory;
    ecs_vector_t *features;
    ecs_vector_t *on_load_systems;
    ecs_vector_t *post_load_systems;
    ecs_vector_t *pre_update_systems;
    ecs_vector_t *on_update_systems;
    ecs_vector_t *on_validate_systems;
    ecs_vector_t *post_update_systems;
    ecs_vector_t *pre_store_systems;
    ecs_vector_t *on_store_systems;
    ecs_vector_t *task_systems;
    ecs_vector_t *inactive_systems;
    ecs_vector_t *manual_systems;
    ecs_vector_t *on_add_systems;
    ecs_vector_t *on_remove_systems;
    ecs_vector_t *on_set_systems;
    ecs_vector_t *components;
    bool frame_profiling;
    bool system_profiling;
} ecs_world_stats_t;

FLECS_EXPORT
void ecs_get_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats);

FLECS_EXPORT
void ecs_free_stats(
    ecs_world_stats_t *stats);

FLECS_EXPORT
void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

FLECS_EXPORT
void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);


/* Stats module component */
typedef struct FlecsStats {
    ECS_DECLARE_COMPONENT(EcsSystemStats);
    ECS_DECLARE_COMPONENT(EcsFeatureStats);
    ECS_DECLARE_COMPONENT(EcsComponentStats);
    ECS_DECLARE_COMPONENT(EcsMemoryStats);
    ECS_DECLARE_COMPONENT(EcsWorldStats);
} FlecsStats;

void FlecsStatsImport(
    ecs_world_t *world,
    int flags);

#define FlecsStatsImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, EcsSystemStats);\
    ECS_IMPORT_COMPONENT(handles, EcsFeatureStats);\
    ECS_IMPORT_COMPONENT(handles, EcsComponentStats);\
    ECS_IMPORT_COMPONENT(handles, EcsMemoryStats);\
    ECS_IMPORT_COMPONENT(handles, EcsWorldStats);

#ifdef __cplusplus
}
#endif

#endif
