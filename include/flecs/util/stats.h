#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Type to keep track of memory that is in use vs. allocated */
typedef struct ecs_memory_stat_t {
    uint32_t allocd;
    uint32_t used;
} ecs_memory_stat_t;

/* Global statistics on memory allocations */
typedef struct EcsAllocStats {
    uint64_t malloc_count;
    uint64_t realloc_count;
    uint64_t calloc_count;
    uint64_t free_count;
} EcsAllocStats;

/* Memory statistics on row (reactive) systems */
typedef struct EcsRowSystemMemoryStats {
    uint32_t base;
    ecs_memory_stat_t columns;
    ecs_memory_stat_t components;
} EcsRowSystemMemoryStats;

/* Memory statistics on column (periodic) systems */
typedef struct EcsColSystemMemoryStats {
    uint32_t base;
    ecs_memory_stat_t columns;
    ecs_memory_stat_t active_tables;
    ecs_memory_stat_t inactive_tables;
    ecs_memory_stat_t jobs;
    uint32_t other;
} EcsColSystemMemoryStats;

/* Memory statistics for a world */
typedef struct EcsMemoryStats {
    ecs_memory_stat_t total;
    ecs_memory_stat_t entities;
    ecs_memory_stat_t components;
    ecs_memory_stat_t systems;
    ecs_memory_stat_t types;
    ecs_memory_stat_t tables;
    ecs_memory_stat_t stage;
    ecs_memory_stat_t world;
} EcsMemoryStats;

/* Component statistics */
typedef struct EcsComponentStats {
    ecs_entity_t handle;
    const char *id;
    uint16_t size;
    uint32_t memory_used;
    uint32_t memory_allocd;
    uint32_t entities;
    uint32_t tables;
} EcsComponentStats;

/* System statistics */
typedef struct EcsSystemStats {
    ecs_entity_t handle;
    const char *id;
    const char *signature;
    EcsSystemKind kind;
    uint32_t tables_matched;
    uint32_t entities_matched;
    uint64_t invoke_count;
    float period;
    float time_spent;
    bool enabled;
    bool active;
    bool is_hidden;
} EcsSystemStats;

/* Type statistics (only for named types, created with ECS_TYPE) */
typedef struct EcsTypeStats {
    const char *id;
    ecs_type_t type;
    ecs_type_t normalized_type;
    uint32_t entity_count;
    uint32_t entity_childof_count;
    uint32_t entity_instanceof_count;
    uint32_t component_count;
    uint32_t col_system_count;
    uint32_t row_system_count;
    uint32_t enabled_system_count;
    uint32_t active_system_count;
    bool is_hidden;
} EcsTypeStats; 

/* Table statistics */
typedef struct EcsTableStats {
    ecs_type_t type;
    uint32_t column_count;
    uint32_t entity_count;
    uint32_t systems_matched;
    ecs_memory_stat_t data_memory;
    uint32_t other_memory;
} EcsTableStats;

/* World statistics */
typedef struct EcsWorldStats {
    uint32_t table_count;
    uint32_t component_count;
    uint32_t entity_count;
    uint32_t thread_count;
    uint32_t tick_count;
    double system_time;
    double frame_time;
    double merge_time;
    double world_time;
    double target_fps;
} EcsWorldStats;

/* Stats module component */
typedef struct FlecsStats {
    ECS_DECLARE_COMPONENT(EcsAllocStats);
    ECS_DECLARE_COMPONENT(EcsWorldStats);
    ECS_DECLARE_COMPONENT(EcsMemoryStats);
    ECS_DECLARE_COMPONENT(EcsSystemStats);
    ECS_DECLARE_COMPONENT(EcsColSystemMemoryStats);
    ECS_DECLARE_COMPONENT(EcsRowSystemMemoryStats);
    ECS_DECLARE_COMPONENT(EcsComponentStats);
    ECS_DECLARE_COMPONENT(EcsTableStats);
    ECS_DECLARE_COMPONENT(EcsTablePtr);
    ECS_DECLARE_COMPONENT(EcsTypeStats);
} FlecsStats;

void FlecsStatsImport(
    ecs_world_t *world,
    int flags);

#define FlecsStatsImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, EcsAllocStats);\
    ECS_IMPORT_COMPONENT(handles, EcsWorldStats);\
    ECS_IMPORT_COMPONENT(handles, EcsMemoryStats);\
    ECS_IMPORT_COMPONENT(handles, EcsSystemStats);\
    ECS_IMPORT_COMPONENT(handles, EcsColSystemMemoryStats);\
    ECS_IMPORT_COMPONENT(handles, EcsRowSystemMemoryStats);\
    ECS_IMPORT_COMPONENT(handles, EcsComponentStats);\
    ECS_IMPORT_COMPONENT(handles, EcsTableStats);\
    ECS_IMPORT_COMPONENT(handles, EcsTablePtr);\
    ECS_IMPORT_COMPONENT(handles, EcsTypeStats);

#ifdef __cplusplus
}
#endif

#endif
