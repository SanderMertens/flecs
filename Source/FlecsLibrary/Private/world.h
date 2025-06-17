/**
 * @file world.h
 * @brief World functions.
 */

#ifndef FLECS_WORLD_H
#define FLECS_WORLD_H

/* The bitmask used when determining the table version array index */
#define ECS_TABLE_VERSION_ARRAY_BITMASK (0xff)

/* The number of table versions to split tables across */
#define ECS_TABLE_VERSION_ARRAY_SIZE (ECS_TABLE_VERSION_ARRAY_BITMASK + 1)

/* World level allocators are for operations that are not multithreaded */
typedef struct ecs_world_allocators_t {
    ecs_map_params_t ptr;
    ecs_map_params_t query_table_list;
    ecs_block_allocator_t query_table;
    ecs_block_allocator_t graph_edge_lo;
    ecs_block_allocator_t graph_edge;
    ecs_block_allocator_t id_record;
    ecs_block_allocator_t pair_id_record;
    ecs_block_allocator_t id_record_chunk;
    ecs_block_allocator_t table_diff;
    ecs_block_allocator_t sparse_chunk;
    ecs_block_allocator_t hashmap;

    /* Temporary vectors used for creating table diff id sequences */
    ecs_table_diff_builder_t diff_builder;
} ecs_world_allocators_t;

/* Component monitor */
typedef struct ecs_monitor_t {
    ecs_vec_t queries;               /* vector<ecs_query_cache_t*> */
    bool is_dirty;                   /* Should queries be rematched? */
} ecs_monitor_t;

/* Component monitors */
typedef struct ecs_monitor_set_t {
    ecs_map_t monitors;              /* map<id, ecs_monitor_t> */
    bool is_dirty;                   /* Should monitors be evaluated? */
} ecs_monitor_set_t;

/* Data stored for id marked for deletion */
typedef struct ecs_marked_id_t {
    ecs_component_record_t *cr;
    ecs_id_t id;
    ecs_entity_t action;             /* Set explicitly for delete_with, remove_all */
    bool delete_id;
} ecs_marked_id_t;

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_entity_index_t entity_index;

    /* Tables */
    ecs_sparse_t tables;             /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map;         /* hashmap<ecs_type_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;

    /* Records cache */
    ecs_vec_t records;

    /* Stack of ids being deleted during cleanup action. */
    ecs_vec_t marked_ids;            /* vector<ecs_marked_id_t> */

    /* Components deleted during cleanup action. Used to delay cleaning up of
     * type info so it's guaranteed that this data is available while the 
     * storage is cleaning up tables. */
    ecs_vec_t deleted_components;    /* vector<ecs_entity_t> */
} ecs_store_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

typedef struct ecs_pipeline_state_t ecs_pipeline_state_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    ecs_header_t hdr;

    /* --  Type metadata -- */
    ecs_component_record_t **id_index_lo;
    ecs_map_t id_index_hi;           /* map<id, ecs_component_record_t*> */
    ecs_map_t type_info;             /* map<type_id, type_info_t> */

    /* -- Cached handle to id records -- */
    ecs_component_record_t *cr_wildcard;
    ecs_component_record_t *cr_wildcard_wildcard;
    ecs_component_record_t *cr_any;
    ecs_component_record_t *cr_isa_wildcard;
    ecs_component_record_t *cr_childof_0;
    ecs_component_record_t *cr_childof_wildcard;
    ecs_component_record_t *cr_identifier_name;

    /* Head of list that points to all non-fragmenting component ids */
    ecs_component_record_t *cr_non_fragmenting_head;

    /* -- Mixins -- */
    ecs_world_t *self;
    ecs_observable_t observable;

    /* Unique id per generated event used to prevent duplicate notifications */
    int32_t event_id;

    /* Array of table versions used with component refs to determine if the 
     * cached pointer is still valid. */
    uint32_t table_version[ECS_TABLE_VERSION_ARRAY_SIZE];

    /* Same as table_version, but only increases after the column pointers of
     * a table change. */
    uint32_t table_column_version[ECS_TABLE_VERSION_ARRAY_SIZE];

    /* Array for checking if components can be looked up trivially */
    ecs_flags8_t non_trivial[FLECS_HI_COMPONENT_ID];

    /* Is entity range checking enabled? */
    bool range_check_enabled;

    /* --  Data storage -- */
    ecs_store_t store;

    /* Used to track when cache needs to be updated */
    ecs_monitor_set_t monitors;      /* map<id, ecs_monitor_t> */

    /* -- Systems -- */
    ecs_entity_t pipeline;           /* Current pipeline */

    /* -- Identifiers -- */
    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;

    /* -- Staging -- */
    ecs_stage_t **stages;            /* Stages */
    int32_t stage_count;             /* Number of stages */

    /* -- Component ids -- */
    ecs_vec_t component_ids;         /* World local component ids */

    /* Internal callback for command inspection. Only one callback can be set at
     * a time. After assignment the action will become active at the start of 
     * the next frame, set by ecs_frame_begin, and will be reset by 
     * ecs_frame_end. */
    ecs_on_commands_action_t on_commands;
    ecs_on_commands_action_t on_commands_active;
    void *on_commands_ctx;
    void *on_commands_ctx_active;

    /* -- Multithreading -- */
    ecs_os_cond_t worker_cond;       /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;         /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;       /* Mutex for job_cond */
    int32_t workers_running;         /* Number of threads running */
    int32_t workers_waiting;         /* Number of workers waiting on sync */
    ecs_pipeline_state_t* pq;        /* Pointer to the pipeline for the workers to execute */
    bool workers_use_task_api;       /* Workers are short-lived tasks, not long-running threads */

    /* -- Exclusive access */
    ecs_os_thread_id_t exclusive_access; /* If set, world can only be mutated by thread */
    const char *exclusive_thread_name;   /* Name of thread with exclusive access (used for debugging) */

    /* -- Time management -- */
    ecs_time_t world_start_time;     /* Timestamp of simulation start */
    ecs_time_t frame_start_time;     /* Timestamp of frame start */
    ecs_ftime_t fps_sleep;           /* Sleep time to prevent fps overshoot */

    /* -- Metrics -- */
    ecs_world_info_t info;

    /* -- World flags -- */
    ecs_flags32_t flags;

    /* -- Default query flags -- */
    ecs_flags32_t default_query_flags;

    /* Count that increases when component monitors change */
    int32_t monitor_generation;

    /* -- Allocators -- */
    ecs_world_allocators_t allocators; /* Static allocation sizes */
    ecs_allocator_t allocator;       /* Dynamic allocation sizes */

    void *ctx;                       /* Application context */
    void *binding_ctx;               /* Binding-specific context */

    ecs_ctx_free_t ctx_free;         /**< Callback to free ctx */
    ecs_ctx_free_t binding_ctx_free; /**< Callback to free binding_ctx */

    ecs_vec_t fini_actions;          /* Callbacks to execute when world exits */
};

/* Get current stage. */
ecs_stage_t* flecs_stage_from_world(
    ecs_world_t **world_ptr);

/* Get current thread-specific stage from readonly world. */
ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world);

/* Get component callbacks. */
const ecs_type_info_t *flecs_type_info_get(
    const ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks. */
ecs_type_info_t* flecs_type_info_ensure(
    ecs_world_t *world,
    ecs_entity_t component);

/* Initialize type info for builtin components. */
bool flecs_type_info_init_id(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_size_t size,
    ecs_size_t alignment,
    const ecs_type_hooks_t *li);

#define flecs_type_info_init(world, T, ...)\
    flecs_type_info_init_id(world, ecs_id(T), ECS_SIZEOF(T), ECS_ALIGNOF(T),\
        &(ecs_type_hooks_t)__VA_ARGS__)

/* Free type info for component id. */
void flecs_type_info_free(
    ecs_world_t *world,
    ecs_entity_t component);

/* Check component monitors (triggers query cache revalidation, not related to
 * EcsMonitor). */
void flecs_eval_component_monitors(
    ecs_world_t *world);

/* Register component monitor. */
void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query);

/* Unregister component monitor. */
void flecs_monitor_unregister(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_query_t *query);

/* Update component monitors for added/removed components. */
void flecs_update_component_monitors(
    ecs_world_t *world,
    ecs_type_t *added,
    ecs_type_t *removed);

/* Notify tables with component of event (or all tables if id is 0). */
void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event);

/* Increase table version (used for invalidating ecs_ref_t's). */
void flecs_increment_table_version(
    ecs_world_t *world,
    ecs_table_t *table);

/* Same as flecs_increment_table_version, but for column version. */
void flecs_increment_table_column_version(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get table version. */
uint32_t flecs_get_table_version_fast(
    const ecs_world_t *world,
    const uint64_t table_id);

/* Get table version for column pointer validation. */
uint32_t flecs_get_table_column_version(
    const ecs_world_t *world,
    const uint64_t table_id);

/* Throws error when (OnDelete*, Panic) constraint is violated. */
void flecs_throw_invalid_delete(
    ecs_world_t *world,
    ecs_id_t id);

/* Convenience macro's for world allocator */
#define flecs_walloc(world, size)\
    flecs_alloc(&world->allocator, size)
#define flecs_walloc_t(world, T)\
    flecs_alloc_t(&world->allocator, T)
#define flecs_walloc_n(world, T, count)\
    flecs_alloc_n(&world->allocator, T, count)
#define flecs_wcalloc(world, size)\
    flecs_calloc(&world->allocator, size)
#define flecs_wfree_t(world, T, ptr)\
    flecs_free_t(&world->allocator, T, ptr)
#define flecs_wcalloc_n(world, T, count)\
    flecs_calloc_n(&world->allocator, T, count)
#define flecs_wfree(world, size, ptr)\
    flecs_free(&world->allocator, size, ptr)
#define flecs_wfree_n(world, T, count, ptr)\
    flecs_free_n(&world->allocator, T, count, ptr)
#define flecs_wrealloc(world, size_dst, size_src, ptr)\
    flecs_realloc(&world->allocator, size_dst, size_src, ptr)
#define flecs_wrealloc_n(world, T, count_dst, count_src, ptr)\
    flecs_realloc_n(&world->allocator, T, count_dst, count_src, ptr)
#define flecs_wdup(world, size, ptr)\
    flecs_dup(&world->allocator, size, ptr)
#define flecs_wdup_n(world, T, count, ptr)\
    flecs_dup_n(&world->allocator, T, count, ptr)

#endif
