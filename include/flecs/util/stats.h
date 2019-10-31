#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

/* The naming convention of a metrics is:
 * <name>_<unit>
 *
 * If the metric is a monotonically increasing count, the metric will have the
 * suffix 'total'.
 *
 * If a member is of a composite type, it only has the metric name. A metric
 * name will be plural if it is measured from multiple sources/objects.
 */

/* Type to keep track of memory that is in use vs. allocated */
typedef struct ecs_memory_stat_t {
    uint32_t allocd_bytes;            /* Memory allocated */
    uint32_t used_bytes;              /* Memory in use */
} ecs_memory_stat_t;

/* Global statistics on memory allocations */
typedef struct EcsAllocStats {
    uint64_t malloc_count_total;      /* Total number of times malloc was invoked */
    uint64_t realloc_count_total;     /* Total number of times realloc was invoked */
    uint64_t calloc_count_total;      /* Total number of times calloc was invoked */
    uint64_t free_count_total;        /* Total number of times free was invoked */
} EcsAllocStats;

/* Memory statistics on row (reactive) systems */
typedef struct EcsRowSystemMemoryStats {
    uint32_t base_memory_bytes;             /* Size of the component datatype */
    ecs_memory_stat_t columns_memory;       /* Memory in use for columns */
    ecs_memory_stat_t components_memory;    /* Memory in use for components */
} EcsRowSystemMemoryStats;

/* Memory statistics on column (periodic) systems */
typedef struct EcsColSystemMemoryStats {
    uint32_t base_memory_bytes;             /* Size of the component datatype */
    ecs_memory_stat_t columns_memory;       /* Memory in use for columns */
    ecs_memory_stat_t active_tables_memory; /* Memory in use for active tables */
    ecs_memory_stat_t inactive_tables_memory; /* Memory in use for inactive tables */
    ecs_memory_stat_t jobs_memory;          /* Memory in use for jobs */
    uint32_t other_memory_bytes;            /* Remaining memory in use */
} EcsColSystemMemoryStats;

/* Memory statistics for a world */
typedef struct EcsMemoryStats {
    uint32_t __dummy;                       /* Allow for {0} initialization */
    ecs_memory_stat_t total_memory;         /* Total amount of memory in use */
    ecs_memory_stat_t entities_memory;      /* Memory in use for entities */
    ecs_memory_stat_t components_memory;    /* Memory in use for components */
    ecs_memory_stat_t systems_memory;       /* Memory in use for systems */
    ecs_memory_stat_t types_memory;         /* Memory in use for types */
    ecs_memory_stat_t tables_memory;        /* Memory in use for tables */
    ecs_memory_stat_t stages_memory;        /* Memory in use for stages */
    ecs_memory_stat_t world_memory;         /* Memory in use for world */
} EcsMemoryStats;

/* Component statistics */
typedef struct EcsComponentStats {
    ecs_entity_t entity;                    /* Entity handle of component */
    const char *name;                       /* Entity name */
    uint16_t size_bytes;                    /* Size of the component */
    ecs_memory_stat_t memory;               /* Memory in use for component */
    uint32_t entities_count;                /* Number of entities for component */
    uint32_t tables_count;                  /* Number of tables for component */
} EcsComponentStats;

/* System statistics */
typedef struct EcsSystemStats {
    ecs_entity_t entity;                    /* Entity handle of component */
    const char *name;                       /* Entity name */
    const char *signature;                  /* System signature */
    EcsSystemKind kind;                     /* System kind */
    float period_seconds;                   /* Period at which system runs */                       
    uint32_t tables_matched_count;          /* Number of tables matched */
    uint32_t entities_matched_count;        /* Number of entities matched */
    uint64_t invoke_count_total;            /* Number of times system got invoked */
    float seconds_total;                    /* Total time spent in system */
    bool is_enabled;                        /* Is system enabled */
    bool is_active;                         /* Is system active */
    bool is_hidden;                         /* Is system hidden */
} EcsSystemStats;

/* Type statistics (only for named types, created with ECS_TYPE) */
typedef struct EcsTypeStats {
    ecs_entity_t entity;                    /* Entity handle of type */
    const char *name;                         /* Type name */
    ecs_type_t type;                        /* Reference to type with nesting intact */
    ecs_type_t normalized_type;             /* Reference to normalized type */
    uint32_t entities_count;                /* Number of plain entities in type */  
    uint32_t entities_childof_count;        /* Number of CHILDOF entities in type */
    uint32_t entities_instanceof_count;     /* Number of INSTANCEOF entities in type */
    uint32_t components_count;              /* Number of components in type */
    uint32_t col_systems_count;             /* Number of column (periodic) systems in type */
    uint32_t row_systems_count;             /* Number of row (reactive) systems in type */
    uint32_t enabled_systems_count;         /* Number of enabled systems in type */
    uint32_t active_systems_count;          /* Number of active systems in type */
    uint32_t instance_count;                /* Number of instances of this type */
    bool is_hidden;                         /* Is type hidden */
} EcsTypeStats; 

/* Table statistics */
typedef struct EcsTableStats {
    ecs_type_t type;                        /* Reference to table type */
    uint32_t columns_count;                 /* Number of columns in table */
    uint32_t rows_count;                    /* Number of rows (entities) in table */                
    uint32_t systems_matched_count;         /* Number of systems matched */               
    ecs_memory_stat_t entity_memory;        /* Memory in use for entity data */
    ecs_memory_stat_t component_memory;     /* Memory in use for table data */
    uint32_t other_memory_bytes;            /* Memory in use for other */
} EcsTableStats;

/* World statistics */
typedef struct EcsWorldStats {
    double target_fps_hz;                   /* Target FPS */
    uint32_t tables_count;                  /* Number of tables in world */
    uint32_t components_count;              /* Number of components in world */
    uint32_t col_systems_count;             /* Number of column (periodic) systems in world */
    uint32_t row_systems_count;             /* Nunber of row (reactive) systems in world */
    uint32_t inactive_systems_count;        /* Number of inactive systems in world */
    uint32_t entities_count;                /* Number if entities in world */
    uint32_t threads_count;                 /* Number of threads in world */
    uint32_t frame_count_total;             /* Total number of frames processed */
    double frame_seconds_total;        /* Total time spent processing frames */
    double system_seconds_total;       /* Total time spent in systems */
    double merge_seconds_total;        /* Total time spent merging */
    double world_seconds_total;        /* Total time passed since simulation start */
    double fps_hz;                          /* Frames per second (current) */
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

FLECS_EXPORT
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
