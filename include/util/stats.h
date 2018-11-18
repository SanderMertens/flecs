#ifndef REFLECS_STATS_H
#define REFLECS_STATS_H

#include <reflecs/reflecs.h>
#include <reflecs/util/array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsSystemStats {
    EcsHandle system;
    const char *id;
    bool enabled;
    bool active;
} EcsSystemStats;

typedef struct EcsTableStats {
    char *columns;
    uint32_t row_count;
    uint32_t memory_used;
    uint32_t memory_allocd;
} EcsTableStats;

typedef struct EcsWorldStats {
    uint32_t memory_used;
    uint32_t memory_allocd;
    uint32_t system_count;
    uint32_t table_count;
    uint32_t entity_count;
    uint32_t thread_count;
    EcsArray *active_systems;
    EcsArray *inactive_systems;
    EcsArray *on_demand_systems;
    EcsArray *on_add_systems;
    EcsArray *on_remove_systems;
    EcsArray *on_set_systems;
    EcsArray *tables;
} EcsWorldStats;

REFLECS_EXPORT
void ecs_world_get_stats(
    EcsWorld *world,
    EcsWorldStats *stats);

REFLECS_EXPORT
void ecs_world_free_stats(
    EcsWorld *world,
    EcsWorldStats *stats);

#ifdef __cplusplus
}
#endif

#endif
