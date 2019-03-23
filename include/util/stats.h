#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#include "../flecs.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemStats {
    EcsEntity handle;
    const char *id;
    const char *signature;
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

typedef struct EcsTableStats {
    const char *id;
    char *columns;
    uint32_t row_count;
    uint32_t memory_used;
    uint32_t memory_allocd;
} EcsTableStats;

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
    uint32_t entity_count;
    uint32_t thread_count;
    uint32_t tick_count;
    float system_time;
    float frame_time;
    float merge_time;
    EcsMemoryStats memory;
    EcsArray *features;
    EcsArray *on_load_systems;
    EcsArray *pre_frame_systems;
    EcsArray *on_frame_systems;
    EcsArray *post_frame_systems;
    EcsArray *on_store_systems;
    EcsArray *task_systems;
    EcsArray *inactive_systems;
    EcsArray *on_demand_systems;
    EcsArray *on_add_systems;
    EcsArray *on_remove_systems;
    EcsArray *on_set_systems;
    EcsArray *tables;
    bool frame_profiling;
    bool system_profiling;
} EcsWorldStats;

FLECS_EXPORT
void ecs_get_stats(
    EcsWorld *world,
    EcsWorldStats *stats);

FLECS_EXPORT
void ecs_free_stats(
    EcsWorld *world,
    EcsWorldStats *stats);

FLECS_EXPORT
void ecs_measure_frame_time(
    EcsWorld *world,
    bool enable);

FLECS_EXPORT
void ecs_measure_system_time(
    EcsWorld *world,
    bool enable);

#ifdef __cplusplus
}
#endif

#endif
