#include "flecs.h"
#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#ifndef FLECS_TYPES_PRIVATE_H
#define FLECS_TYPES_PRIVATE_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#ifdef _MSC_VER
//FIXME
#else
#include <sys/param.h>  /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_ei_t ecs_ei_t;

struct ecs_record_t {
    ecs_table_t *table;            /* Identifies a type (and table) in world */
    int32_t row;                  /* Table row of the entity */
};

typedef struct ecs_ei_iter_t {
    int32_t index;
    const int32_t *sparse_indices;
    int32_t sparse_count;
    ecs_map_iter_t map_iter;
    ecs_sparse_t *lo;
} ecs_ei_iter_t;

/* Get entity record */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Set entity */
ecs_record_t* ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record);

/* Get or set entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Delete entity from stage */
void ecs_ei_clear_entity(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    bool is_watched);

/* Delete entity from stage */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity);

/* Recycle deleted entity id (returns 0 if no available) */
ecs_entity_t ecs_ei_recycle(
    ecs_ei_t *entity_index);

/* Grow entity index */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    int32_t count);

/* Grow entity index to specific size */
void ecs_ei_set_size(
    ecs_ei_t *entity_index,
    int32_t size);    

/* Count entities in stage */
int32_t ecs_ei_count(
    ecs_ei_t *entity_index);      

/* Initialize entity index for stage */
void ecs_ei_new(
    ecs_ei_t *entity_index);

/* Clear all entities from a stage */
void ecs_ei_clear(
    ecs_ei_t *entity_index);

/* Clear all entities from a stage */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index);    

/* Free entity index for stage */
void ecs_ei_free(
    ecs_ei_t *entity_index);

void ecs_ei_memory(
    ecs_ei_t *entity_index,
    int32_t *allocd,
    int32_t *used);

/* Create iterator for entity index */
ecs_ei_iter_t ecs_ei_iter(
    ecs_ei_t *entity_index);

/* Return next record for iterator (return NULL when end is reached) */
ecs_record_t *ecs_ei_next(
    ecs_ei_iter_t *iter,
    ecs_entity_t *entity_out);

// Convenience macro's for directly calling operations for stage
#define ecs_eis_get(stage, entity) ecs_ei_get(&(stage)->entity_index, entity)
#define ecs_eis_set(stage, entity, ...) ecs_ei_set(&(stage)->entity_index, entity, __VA_ARGS__)
#define ecs_eis_get_or_create(stage, entity) ecs_ei_get_or_create(&(stage)->entity_index, entity)
#define ecs_eis_delete(stage, entity) ecs_ei_delete(&(stage)->entity_index, entity)
#define ecs_eis_recycle(stage) ecs_ei_recycle(&(stage)->entity_index)
#define ecs_eis_clear_entity(stage, entity, is_watched) ecs_ei_clear_entity(&(stage)->entity_index, entity, is_watched)
#define ecs_eis_grow(stage, count) ecs_ei_grow(&(stage)->entity_index, count)
#define ecs_eis_set_size(stage, size) ecs_ei_set_size(&(stage)->entity_index, size)
#define ecs_eis_count(stage) ecs_ei_count(&(stage)->entity_index)
#define ecs_eis_new(stage) ecs_ei_new(&(stage)->entity_index)
#define ecs_eis_clear(stage) ecs_ei_clear(&(stage)->entity_index)
#define ecs_eis_copy(stage) ecs_ei_copy(&(stage)->entity_index)
#define ecs_eis_free(stage) ecs_ei_free(&(stage)->entity_index)
#define ecs_eis_memory(stage, allocd, used) ecs_ei_memory(&(stage)->entity_index, allocd, used)

#ifdef __cplusplus
}
#endif

#endif

#ifndef FLECS_TABLE_H_
#define FLECS_TABLE_H_


#ifdef __cplusplus
extern "C" {
#endif

/** Find or create table for a set of components */
ecs_table_t* ecs_table_find_or_create(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *type);

/** Find or create table for a type */
ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type);    

/* Get table data from main stage */
ecs_data_t *ecs_table_get_data(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get table data for specific stage */
ecs_data_t *ecs_table_get_staged_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table);

/* Get or create data for specific stage */
ecs_data_t *ecs_table_get_or_create_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table); 

/* Activates / deactivates table for systems. A deactivated table will not be
 * evaluated when the system is invoked. Tables automatically get activated /
 * deactivated when they become non-empty / empty. 
 *
 * If a query is provided, the table will only be activated / deactivated for
 * that query. */
void ecs_table_activate(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    bool activate);

/* Clear all entities from a table. */
void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table);

/* Reset a table to its initial state */
void ecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear all entities from the table. Do not invoke OnRemove systems */
void ecs_table_clear_silent(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear table data. Don't call OnRemove handlers. */
void ecs_table_clear_data(
    ecs_table_t *table,
    ecs_data_t *data);    

/* Return number of entities in table in main stage. */
int32_t ecs_table_count(
    ecs_table_t *table);

/* Return number of entities in stage-specific data */
int32_t ecs_table_data_count(
    ecs_data_t *data);

/* Add a new entry to the table for the specified entity */
int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record);

/* Delete an entity from the table. */
void ecs_table_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index);

/* Move a row from one table to another */
void ecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool is_copy);

/* Grow table with specified number of records. Populate table with entities,
 * starting from specified entity id. */
int32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count,
    ecs_entity_t first_entity);

/* Set table to a fixed size. Useful for preallocating memory in advance. */
int16_t ecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count); 

/* Set table to a fixed count. Useful for copying data in bulk. */
int16_t ecs_table_set_count(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count); 

/* Match table with filter */
bool ecs_table_match_filter(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_filter_t *filter);

/* Get dirty state for table columns */
int32_t* ecs_table_get_dirty_state(
    ecs_table_t *table);

/* Get monitor for monitoring table changes */
int32_t* ecs_table_get_monitor(
    ecs_table_t *table);

#ifdef __cplusplus
}
#endif

#endif

#define ECS_MAX_JOBS_PER_WORKER (16)

/** Entity id's higher than this number will be stored in a map instead of a
 * sparse set. Increasing this value can improve performance at the cost of
 * (significantly) higher memory usage. */
#define ECS_HI_ENTITY_ID (1000000)

/** These values are used to verify validity of the pointers passed into the API
 * and to allow for passing a thread as a world to some API calls (this allows
 * for transparently passing thread context to API functions) */
#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_THREAD_MAGIC (0x65637374)

/* Maximum number of entities that can be added in a single operation. 
 * Increasing this value will increase consumption of stack space. */
#define ECS_MAX_ADD_REMOVE (32)

/* Maximum length of an entity name, including 0 terminator */
#define ECS_MAX_NAME_LENGTH (64)

/* Simple bitmask structure to store a set of components. This is used amongst
 * others to keep track of which components have been overridden from a base. */
typedef ecs_entity_t ecs_comp_mask_t[ECS_HI_COMPONENT_ID / 64];

/** Callback used by the system signature expression parser. */
typedef int (*ecs_parse_action_t)(
    ecs_world_t *world,                 
    const char *id,
    const char *expr,
    int64_t column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *component,
    const char *source,
    void *ctx);

/** A component array in a table. */
struct ecs_column_t {
    ecs_vector_t *data;         /**< Column data */
    int16_t size;              /**< Column element size */
    int16_t alignment;         /**< Column element alignment */
};

/** Stage-specific component data */
struct ecs_data_t {
    ecs_vector_t *entities;     /**< Entity identifiers */
    ecs_vector_t *record_ptrs;  /**< Pointers to records in main entity index */
    ecs_column_t *columns;      /**< Component data */
    bool marked_dirty;          /**< Was table marked dirty by stage? */  
};

/** Small footprint data structure for storing data associated with a table. */
typedef struct ecs_table_leaf_t {
    ecs_table_t *table;
    ecs_type_t type;
    ecs_data_t *data;
} ecs_table_leaf_t;

/** Flags for quickly checking for special properties of a table. */
#define EcsTableHasBuiltins         1u    /**< Does table have builtin components */
#define EcsTableIsPrefab            2u    /**< Does the table store prefabs */
#define EcsTableHasBase             4u    /**< Does the table type has INSTANCEOF */
#define EcsTableHasParent           8u    /**< Does the table type has CHILDOF */
#define EcsTableHasComponentData    16u   /**< Does the table have component data */
#define EcsTableHasXor              32u   /**< Does the table type has XOR */
#define EcsTableIsDisabled          64u   /**< Does the table type has EcsDisabled */
#define EcsTableHasCtors            128u
#define EcsTableHasDtors            256u
#define EcsTableHasOnAdd            512u
#define EcsTableHasOnRemove         1024u
#define EcsTableHasOnSet            2048u
#define EcsTableHasUnSet            4096u
#define EcsTableHasMonitors         8192u

/* Composite constants */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableHasAddActions       (EcsTableHasBase | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet | EcsTableHasMonitors)
#define EcsTableHasRemoveActions    (EcsTableHasBase | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet | EcsTableHasMonitors)

/** Edge used for traversing the table graph. */
typedef struct ecs_edge_t {
    ecs_table_t *add;               /**< Edges traversed when adding */
    ecs_table_t *remove;            /**< Edges traversed when removing */
} ecs_edge_t;

/** Quey matched with table with backref to query table administration.
 * This type is used to store a matched query together with the array index of
 * where the table is stored in the query administration. This type is used when
 * an action that originates on a table needs to invoke a query (system) and a
 * fast lookup is required for the query administration, as is the case with
 * OnSet and Monitor systems. */
typedef struct ecs_matched_query_t {
    ecs_query_t *query;             /**< The query matched with the table */
    int32_t matched_table_index;    /**< Table index in the query type */
} ecs_matched_query_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing column systems */
struct ecs_table_t {
    ecs_type_t type;                 /**< Identifies table type in type_index */

    ecs_edge_t *lo_edges;            /**< Edges to low entity ids */
    ecs_map_t *hi_edges;             /**< Edges to high entity ids */

    ecs_vector_t *data;              /**< Data per stage */

    ecs_vector_t *queries;           /**< Queries matched with table */
    ecs_vector_t *monitors;          /**< Monitor systems matched with table */
    ecs_vector_t **on_set;           /**< OnSet systems, broken up by column */
    ecs_vector_t *on_set_all;        /**< All OnSet systems */
    ecs_vector_t *on_set_override;   /**< All OnSet systems with overrides */
    ecs_vector_t *un_set_all;        /**< All OnSet systems */

    int32_t *dirty_state;            /**< Keep track of changes in columns */
    int32_t alloc_count;             /**< Increases when columns are reallocd */

    ecs_flags32_t flags;             /**< Flags for testing table properties */
    int32_t column_count;            /**< Number of data columns in table */
};

/** Type containing data for a table matched with a query. */
typedef struct ecs_matched_table_t {
    ecs_table_t *table;            /**< Reference to the table */
    int32_t *columns;              /**< Mapping of system columns to table */
    ecs_entity_t *components;      /**< Actual components of system columns */
    ecs_vector_t *references;      /**< Reference columns and cached pointers */
    int32_t rank;                  /**< Rank used to sort tables */
    int32_t *monitor;              /**< Used to monitor table for changes */
} ecs_matched_table_t;

/** Type storing an entity range within a table.
 * This type is used for iterating in orer across archetypes. A sorting function
 * constructs a list of the ranges across archetypes that are in order so that
 * when the query iterates over the archetypes, it only needs to iterate the
 * list of ranges. */
typedef struct ecs_table_range_t {
    ecs_matched_table_t *table;     /**< Reference to the matched table */
    int32_t start_row;              /**< Start of range  */
    int32_t count;                  /**< Number of entities in range */
} ecs_table_range_t;

#define EcsQueryNeedsTables (1)      /* Query needs matching with tables */ 
#define EcsQueryMonitor (2)          /* Query needs to be registered as a monitor */
#define EcsQueryOnSet (4)            /* Query needs to be registered as on_set system */
#define EcsQueryUnSet (8)            /* Query needs to be registered as un_set system */
#define EcsQueryMatchDisabled (16)   /* Does query match disabled */
#define EcsQueryMatchPrefab (32)     /* Does query match prefabs */
#define EcsQueryHasRefs (64)         /* Does query have references */
#define EcsQueryHasTraits (128)      /* Does query have traits */

#define EcsQueryNoActivation (EcsQueryMonitor | EcsQueryOnSet | EcsQueryUnSet)

/** Query that is automatically matched against active tables */
struct ecs_query_t {
    /* Signature of query */
    ecs_sig_t sig;

    /* Reference to world */
    ecs_world_t *world;

    /* Tables matched with query */
    ecs_vector_t *tables;
    ecs_vector_t *empty_tables;

    /* Handle to system (optional) */
    ecs_entity_t system;   

    /* Used for sorting */
    ecs_entity_t sort_on_component;
    ecs_compare_action_t compare;   
    ecs_vector_t *table_ranges;     

    /* Used for table sorting */
    ecs_entity_t rank_on_component;
    ecs_rank_type_action_t group_table;

    /* The query kind determines how it is registered with tables */
    ecs_flags32_t flags;

    int32_t cascade_by;         /* Identify CASCADE column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Used to track if sorting is needed */
};

/** Keep track of how many [in] columns are active for [out] columns of OnDemand
 * systems. */
typedef struct ecs_on_demand_out_t {
    ecs_entity_t system;    /* Handle to system */
    int32_t count;         /* Total number of times [out] columns are used */
} ecs_on_demand_out_t;

/** Keep track of which OnDemand systems are matched with which [in] columns */
typedef struct ecs_on_demand_in_t {
    int32_t count;         /* Number of active systems with [in] column */
    ecs_vector_t *systems;  /* Systems that have this column as [out] column */
} ecs_on_demand_in_t;

/** Entity index */
struct ecs_ei_t {
    ecs_sparse_t *lo;       /* Low entity ids are stored in a sparse set */
    ecs_map_t *hi;          /* To save memory high ids are stored in a map */
};

/** Types for deferred operations */
typedef enum ecs_op_kind_t {
    EcsOpNone,
    EcsOpAdd,
    EcsOpRemove,   
    EcsOpSet,
} ecs_op_kind_t;

typedef struct ecs_op_t {
    ecs_op_kind_t kind;
    ecs_entity_t entity;
    ecs_entities_t components;
    ecs_entity_t component;
    void *value;
    ecs_size_t size;
} ecs_op_t;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
struct ecs_stage_t {
    /* This points to the world pointer associated with the stage. Even though
     * stages belong to the same world, when multithreaded, an application will
     * receive a pointer not to the world, but to a thread. This allows for
     * transparently passing the thread context without having to fallback on
     * more expensive methods such as thread local storage. This world pointer
     * is stored in the stage, so that it can be easily passed around when for
     * example invoking callbacks, and prevents the API from passing around two
     * world pointers (or constantly obtaining the real world when needed). */
    ecs_world_t *world;

    /* If this is not main stage, 
     * changes to the entity index 
     * are buffered here */
    ecs_ei_t entity_index; /* Entity lookup table for (table, row) */

    /* If this is not a thread
     * stage, these are the same
     * as the main stage */
    ecs_sparse_t *tables;          /* Tables created while >1 threads running */
    ecs_table_t root;              /* Root table */
    ecs_vector_t *dirty_tables;    /* Tables that need merging */

    /* Namespacing */
    ecs_table_t *scope_table;      /* Table for current scope */
    ecs_entity_t scope;            /* Entity of current scope */

    int32_t id;                    /* Unique id that identifies the stage */

    /* Are operations deferred? */
    int32_t defer;
    ecs_vector_t *defer_queue;

    /* One-shot actions to be executed after the merge */
    ecs_vector_t *post_frame_actions;

    /* Is entity range checking enabled? */
    bool range_check_enabled;
};

/** Supporting type to store looked up or derived entity data */
typedef struct ecs_entity_info_t {
    ecs_record_t *record;       /* Main stage record in entity index */
    ecs_table_t *table;         /* Table. Not set if entity is empty */
    ecs_data_t *data;           /* Stage-specific table columns */
    int32_t row;                /* Actual row (stripped from is_watched bit) */
    bool is_watched;            /* Is entity being watched */
} ecs_entity_info_t;

/** A type desribing a worker thread. When a system is invoked by a worker
 * thread, it receives a pointer to an ecs_thread_t instead of a pointer to an 
 * ecs_world_t (provided by the ecs_iter_t type). When this ecs_thread_t is passed down
 * into the flecs API, the API functions are able to tell whether this is an
 * ecs_thread_t or an ecs_world_t by looking at the 'magic' number. This allows the
 * API to transparently resolve the stage to which updates should be written,
 * without requiring different API calls when working in multi threaded mode. */
typedef struct ecs_thread_t {
    int32_t magic;                           /* Magic number to verify thread pointer */
    ecs_world_t *world;                       /* Reference to world */
    ecs_stage_t *stage;                       /* Stage for thread */
    ecs_os_thread_t thread;                   /* Thread handle */
    int32_t index;                           /* Index of thread */
} ecs_thread_t;

/** Component-specific data */
typedef struct ecs_c_info_t {
    ecs_vector_t *on_add;       /* Systems ran after adding this component */
    ecs_vector_t *on_remove;    /* Systems ran after removing this component */
    EcsComponentLifecycle lifecycle; /* Component lifecycle callbacks */
} ecs_c_info_t;

/** Supporting type to store looked up component data in specific table */
typedef struct ecs_column_info_t {
    ecs_entity_t id;
    ecs_c_info_t *ci;
    int32_t column;
} ecs_column_info_t;

/* Component monitors */
typedef struct ecs_component_monitor_t {
    bool dirty_flags[ECS_HI_COMPONENT_ID];
    ecs_vector_t *monitors[ECS_HI_COMPONENT_ID];
    bool rematch;
} ecs_component_monitor_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    int32_t magic;               /* Magic number to verify world pointer */
    void *context;               /* Application context */
    ecs_vector_t *fini_actions;  /* Callbacks to execute when world exits */

    ecs_c_info_t c_info[ECS_HI_COMPONENT_ID]; /* Component callbacks & triggers */
    ecs_map_t *t_info;                        /* Tag triggers */


    /* --  Queries -- */

    /* Persistent queries registered with the world. Persistent queries are
     * stateful and automatically matched with existing and new tables. */
    ecs_sparse_t *queries;

    /* Keep track of components that were added/removed to/from monitored
     * entities. Monitored entities are entities that a query has matched with
     * specifically, as is the case with PARENT / CASCADE columns, FromEntity
     * columns and columns matched from prefabs. 
     * When these entities change type, queries may have to be rematched.
     * Queries register themselves as component monitors for specific components
     * and when these components change they are rematched. The component 
     * monitors are evaluated during a merge. */
    ecs_component_monitor_t component_monitors;

    /* Parent monitors are like normal component monitors except that the
     * conditions under which a parent component is flagged dirty is different.
     * Parent component flags are marked dirty when an entity that is a parent
     * adds or removes a CHILDOF flag. In that case, every component of that
     * parent will be marked dirty. This allows column modifiers like CASCADE
     * to correctly determine when the depth ranking of a table has changed. */
    ecs_component_monitor_t parent_monitors; 


    /* -- Systems -- */
    
    ecs_entity_t pipeline;             /* Current pipeline */
    ecs_map_t *on_activate_components; /* Trigger on activate of [in] column */
    ecs_map_t *on_enable_components;   /* Trigger on enable of [in] column */
    ecs_vector_t *fini_tasks;          /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_handles;          /* Handles to named types */


    /* -- Staging -- */

    ecs_stage_t stage;               /* Main storage */
    ecs_stage_t temp_stage;          /* Stage for when processing systems */
    ecs_vector_t *worker_stages;     /* Stages for worker threads */
    int32_t stage_count;            /* Number of stages in world */


    /* -- Hierarchy administration -- */

    ecs_map_t *child_tables;        /* Child tables per parent entity */
    const char *name_prefix;        /* Remove prefix from C names in modules */


    /* -- Multithreading -- */

    ecs_vector_t *workers;           /* Worker threads */
    
    ecs_os_cond_t worker_cond;       /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;         /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;       /* Mutex for job_cond */
    int32_t workers_running;         /* Number of threads running */
    int32_t workers_waiting;         /* Number of workers waiting on sync */


    /* -- Time management -- */

    ecs_time_t world_start_time;  /* Timestamp of simulation start */
    ecs_time_t frame_start_time;  /* Timestamp of frame start */
    float fps_sleep;              /* Sleep time to prevent fps overshoot */


    /* -- Metrics -- */

    ecs_world_info_t stats;


    /* -- Settings from command line arguments -- */

    int arg_fps;
    int arg_threads;


    /* -- World lock -- */

    ecs_os_mutex_t mutex;         /* Locks the world if locking enabled */
    ecs_os_mutex_t thr_sync;      /* Used to signal threads at end of frame */
    ecs_os_cond_t thr_cond;       /* Used to signal threads at end of frame */


    /* -- World state -- */

    bool valid_schedule;          /* Is job schedule still valid */
    bool quit_workers;            /* Signals worker threads to quit */
    bool in_progress;             /* Is world being progressed */
    bool is_merging;              /* Is world currently being merged */
    bool auto_merge;              /* Are stages auto-merged by ecs_progress */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
    bool locking_enabled;         /* Lock world when in progress */    
};

#endif

////////////////////////////////////////////////////////////////////////////////
//// Core bootstrap functions
////////////////////////////////////////////////////////////////////////////////

#define ECS_TYPE_DECL(component)\
static const ecs_entity_t __##component = ecs_entity(component);\
ECS_VECTOR_DECL(FLECS__T##component, ecs_entity_t, 1)

#define ECS_TYPE_IMPL(component)\
ECS_VECTOR_IMPL(FLECS__T##component, ecs_entity_t, &__##component, 1)

/* Bootstrap world */
void ecs_bootstrap(
    ecs_world_t *world);

ecs_type_t ecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity);

#define ecs_bootstrap_component(world, name)\
    ecs_new_component(world, ecs_entity(name), #name, sizeof(name), ECS_ALIGNOF(name))

#define ecs_bootstrap_tag(world, name)\
    ecs_set(world, name, EcsName, {.value = &#name[ecs_os_strlen("Ecs")], .symbol = #name});\
    ecs_add_entity(world, name, ECS_CHILDOF | ecs_get_scope(world))


////////////////////////////////////////////////////////////////////////////////
//// Entity API
////////////////////////////////////////////////////////////////////////////////

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

/* Does one of the entity containers has specified component */
ecs_entity_t ecs_find_in_type(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t component,
    ecs_entity_t flags);

/* Obtain entity info */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

void ecs_run_monitors(
    ecs_world_t *world, 
    ecs_stage_t *stage, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors);


////////////////////////////////////////////////////////////////////////////////
//// World API
////////////////////////////////////////////////////////////////////////////////

/* Notify systems that there is a new table, which triggers matching */
void ecs_notify_queries_of_table(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get current thread-specific stage */
ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr);

/* Get component callbacks */
ecs_c_info_t *ecs_get_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks */
ecs_c_info_t * ecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

void ecs_eval_component_monitors(
    ecs_world_t *world);

void ecs_component_monitor_mark(
    ecs_component_monitor_t *mon,
    ecs_entity_t component);

void ecs_component_monitor_register(
    ecs_component_monitor_t *mon,
    ecs_entity_t component,
    ecs_query_t *query);

bool ecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entities_t *components,
    const void *value,
    ecs_size_t size);

void ecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage);

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);

ecs_flags32_t ecs_get_component_action_flags(
    ecs_c_info_t *c_info);    

void ecs_notify_tables_of_component_actions(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_c_info_t *c_info);

////////////////////////////////////////////////////////////////////////////////
//// Stage API
////////////////////////////////////////////////////////////////////////////////

/* Initialize stage data structures */
void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Deinitialize stage */
void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Merge stage with main stage */
void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage);


////////////////////////////////////////////////////////////////////////////////
//// Type API
////////////////////////////////////////////////////////////////////////////////

/* Merge add/remove types */
ecs_type_t ecs_type_merge_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t cur_id,
    ecs_type_t to_add_id,
    ecs_type_t to_remove_id);

/* Test if type_id_1 contains type_id_2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab);

/* Add component to type */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component);

/* Find entity in prefabs of type */
ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous);

void ecs_get_column_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *components,
    ecs_column_info_t *cinfo,
    bool get_all);

void ecs_components_construct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count);

void ecs_components_destruct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count);

void ecs_run_add_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added,
    ecs_comp_mask_t set_mask,
    bool get_all,
    bool run_on_set);   

void ecs_run_remove_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *removed,
    bool get_all);

void ecs_run_set_systems(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool set_all);


////////////////////////////////////////////////////////////////////////////////
//// Table API
////////////////////////////////////////////////////////////////////////////////

/* Initialize root table */
void ecs_init_root_table(
    ecs_world_t *world,
    ecs_stage_t *stage);

void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index);

/* Unset components in table */
void ecs_table_unset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Destruct columns */
void ecs_table_destruct(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_data_t *data, 
    int32_t row, 
    int32_t count);

/* Free table */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Merge table data */
void ecs_table_merge_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);
    
/* Replace data */
void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);

/* Merge data of one table into another table */
void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table);

void ecs_table_swap(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2);

ecs_table_t *ecs_table_traverse_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *to_add,
    ecs_entities_t *added);

ecs_table_t *ecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed);    

void ecs_table_mark_dirty_w_index(
    ecs_table_t *table,
    int32_t index);

void ecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component);

const EcsComponent* ecs_component_from_id(
    ecs_world_t *world,
    ecs_entity_t e);

////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system, 
    ecs_sig_t *sig);

void ecs_query_activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active);

void ecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table);

void ecs_query_set_iter(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count);

void ecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *query);

void ecs_run_monitor(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_matched_query_t *monitor,
    ecs_entities_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities);

bool ecs_query_match(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    ecs_match_failure_t *failure_info);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig);

void ecs_sig_deinit(
    ecs_sig_t *sig);

/* Check if all non-table column constraints are met */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig);


////////////////////////////////////////////////////////////////////////////////
//// Time API
////////////////////////////////////////////////////////////////////////////////

void ecs_os_time_setup(void);

uint64_t ecs_os_time_now(void);

void ecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec);

/* Increase or reset timer resolution (Windows only) */
FLECS_EXPORT
void ecs_increase_timer_resolution(
    bool enable);

////////////////////////////////////////////////////////////////////////////////
//// Utilities
////////////////////////////////////////////////////////////////////////////////

/* Convert 64 bit signed integer to 16 bit */
int8_t ecs_to_i8(
    int64_t v);

/* Convert 64 bit signed integer to 16 bit */
int16_t ecs_to_i16(
    int64_t v);

/* Convert 64 bit signed integer to 32 bit */
int32_t ecs_to_i32(
    int64_t v);    

/* Convert signed integer to size_t */
size_t ecs_to_size_t(
    int64_t size);

/* Convert size_t to ecs_size_t */
ecs_size_t ecs_from_size_t(
    size_t size);    

/* Convert int64_t to entity */
ecs_entity_t ecs_to_entity(
    int64_t v);

/* Convert 64bit value to ecs_record_t type. ecs_record_t is stored as 64bit int in the
 * entity index */
ecs_record_t ecs_to_row(
    uint64_t value);

/* Get 64bit integer from ecs_record_t */
uint64_t ecs_from_row(
    ecs_record_t record);

/* Get actual row from record row */
int32_t ecs_record_to_row(
    int32_t row, 
    bool *is_watched_out);

/* Convert actual row to record row */
int32_t ecs_row_to_record(
    int32_t row, 
    bool is_watched);

/* Convert type to entity array */
ecs_entities_t ecs_type_to_entities(
    ecs_type_t type); 

/* Convert a symbol name to an entity name by removing the prefix */
const char* ecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name); 

/* Lookup an entity by name with a specific type */
ecs_entity_t ecs_lookup_w_type(
    ecs_world_t *world,
    const char *name,
    ecs_type_t type);      

/* Utility that print a descriptive error string*/
//void ecs_print_error_string(const char *error_description, const char* signature, const char* system_id, const char* component_id);
//void ecs_print_error_string(const char* signature, const char *system_id, const char *error_description, const char *component_id);

/* Utility that parses system signature */
int ecs_parse_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *system_id,
    void *ctx);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *func);

#endif

static
char *ecs_vasprintf(
    const char *fmt,
    va_list args)
{
    ecs_size_t size = 0;
    char *result  = NULL;
    va_list tmpa;

    va_copy(tmpa, args);

    size = vsnprintf(result, ecs_to_size_t(size), fmt, tmpa);

    va_end(tmpa);

    if ((int32_t)size < 0) { 
        return NULL; 
    }

    result = (char *) ecs_os_malloc(size + 1);

    if (!result) { 
        return NULL; 
    }

    ecs_os_vsprintf(result, fmt, args);

    return result;
}

static
char* ecs_colorize(
    char *msg)
{
    ecs_strbuf_t buff = ECS_STRBUF_INIT;
    char *ptr, ch, prev = '\0';
    bool isNum = false;
    char isStr = '\0';
    bool isVar = false;
    bool overrideColor = false;
    bool autoColor = true;
    bool dontAppend = false;
    bool use_colors = true;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dontAppend = false;

        if (!overrideColor) {
            if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr &&
                !isalpha(prev) && (prev != '\\'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) ||
                (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar &&
                 !isalpha(prev) && !isdigit(prev) && (prev != '_') &&
                 (prev != '.'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isVar = true;
            }
        }

        if (!isVar && !isStr && !isNum && ch == '#' && ptr[1] == '[') {
            bool isColor = true;
            overrideColor = true;

            /* Custom colors */
            if (!ecs_os_strncmp(&ptr[2], "]", ecs_os_strlen("]"))) {
                autoColor = false;
            } else if (!ecs_os_strncmp(&ptr[2], "green]", ecs_os_strlen("green]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
            } else if (!ecs_os_strncmp(&ptr[2], "red]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_RED);
            } else if (!ecs_os_strncmp(&ptr[2], "blue]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BLUE);
            } else if (!ecs_os_strncmp(&ptr[2], "magenta]", ecs_os_strlen("magenta]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_MAGENTA);
            } else if (!ecs_os_strncmp(&ptr[2], "cyan]", ecs_os_strlen("cyan]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
            } else if (!ecs_os_strncmp(&ptr[2], "yellow]", ecs_os_strlen("yellow]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_YELLOW);
            } else if (!ecs_os_strncmp(&ptr[2], "grey]", ecs_os_strlen("grey]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREY);
            } else if (!ecs_os_strncmp(&ptr[2], "white]", ecs_os_strlen("white]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "bold]", ecs_os_strlen("bold]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BOLD);
            } else if (!ecs_os_strncmp(&ptr[2], "normal]", ecs_os_strlen("normal]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "reset]", ecs_os_strlen("reset]"))) {
                overrideColor = false;
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else {
                isColor = false;
                overrideColor = false;
            }

            if (isColor) {
                ptr += 2;
                while ((ch = *ptr) != ']') ptr ++;
                dontAppend = true;
            }
            if (!autoColor) {
                overrideColor = true;
            }
        }

        if (ch == '\n') {
            if (isNum || isStr || isVar || overrideColor) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                overrideColor = false;
                isNum = false;
                isStr = false;
                isVar = false;
            }
        }

        if (!dontAppend) {
            ecs_strbuf_appendstrn(&buff, ptr, 1);
        }

        if (!overrideColor) {
            if (((ch == '\'') || (ch == '"')) && !isStr) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
    }

    return ecs_strbuf_get(&buff);
}

static int trace_indent = 0;
static int trace_level = 0;

void ecs_log_print(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list valist)
{
    (void)level;
    (void)line;

    if (level > trace_level) {
        return;
    }

    /* Massage filename so it doesn't take up too much space */
    char filebuff[256];
    ecs_os_strcpy(filebuff, file);
    file = filebuff;
    char *file_ptr = strrchr(file, '/');
    if (file_ptr) {
        file = file_ptr + 1;
    }

    /* Extension is likely the same for all files */
    file_ptr = strrchr(file, '.');
    if (file_ptr) {
        *file_ptr = '\0';
    }

    char indent[32];
    int i;
    for (i = 0; i < trace_indent; i ++) {
        indent[i * 2] = '|';
        indent[i * 2 + 1] = ' ';
    }
    indent[i * 2] = '\0';

    char *msg = ecs_vasprintf(fmt, valist);
    char *color_msg = ecs_colorize(msg);

    if (level >= 0) {
        ecs_os_log("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    } else if (level == -2) {
        ecs_os_warn("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    } else if (level <= -2) {
        ecs_os_err("%sinfo%s: %s%s%s%s",
            ECS_MAGENTA, ECS_NORMAL, ECS_GREY, indent, ECS_NORMAL, color_msg);
    }

    ecs_os_free(color_msg);
    ecs_os_free(msg);
}

void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(level, file, line, fmt, valist);
}

void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(-2, file, line, fmt, valist);
}

void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list valist;
    va_start(valist, fmt);

    ecs_log_print(-3, file, line, fmt, valist);
}

void ecs_log_push(void) {
    trace_indent ++;
}

void ecs_log_pop(void) {
    trace_indent --;
}

void ecs_tracing_enable(
    int level)
{
    trace_level = level;
}

void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (trace_level != -2) {
        va_list valist;
        va_start(valist, fmt);
        char *msg = ecs_vasprintf(fmt, valist);

        ecs_os_err("%s:%d: error: %s", name, column + 1, msg);
        ecs_os_err("    %s", expr);
        ecs_os_err("    %*s^", column, "");
        
        ecs_os_free(msg);        
    }

    ecs_os_abort();
}

void _ecs_abort(
    int32_t error_code,
    const char *param,
    const char *file,
    int32_t line)
{
    if (param) {
        ecs_err("abort %s:%d: %s (%s)",
            file, line, ecs_strerror(error_code), param);
    } else {
        ecs_err("abort %s:%d: %s", file, line, ecs_strerror(error_code));
    }

    ecs_os_abort();
}

void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    int32_t line)
{
    if (!condition) {
        if (param) {
            ecs_err("assert(%s) %s:%d: %s (%s)",
                condition_str, file, line, ecs_strerror(error_code), param);
        } else {
            ecs_err("assert(%s) %s:%d: %s",
                condition_str, file, line, ecs_strerror(error_code));
        }

        ecs_os_abort();
    }
}

const char* ecs_strerror(
    int32_t error_code)
{
    switch (error_code) {
    case ECS_INVALID_HANDLE:
        return "invalid handle";
    case ECS_INVALID_PARAMETER:
        return "invalid parameters";
    case ECS_INVALID_COMPONENT_ID:
        return "invalid component id";
    case ECS_INVALID_TYPE_EXPRESSION:
        return "invalid type expression";
    case ECS_INVALID_SIGNATURE:
        return "invalid system signature";
    case ECS_INVALID_EXPRESSION:
        return "invalid type expression/signature";
    case ECS_MISSING_SYSTEM_CONTEXT:
        return "missing system context";
    case ECS_UNKNOWN_COMPONENT_ID:
        return "unknown component id";
    case ECS_UNKNOWN_TYPE_ID:
        return "unknown type id";
    case ECS_TYPE_NOT_AN_ENTITY:
        return "type contains more than one entity";
    case ECS_NOT_A_COMPONENT:
        return "handle is not a component";
    case ECS_INTERNAL_ERROR:
        return "internal error";
    case ECS_MORE_THAN_ONE_PREFAB:
        return "more than one prefab added to entity";
    case ECS_ALREADY_DEFINED:
        return "entity has already been defined";
    case ECS_INVALID_COMPONENT_SIZE:
        return "the specified size does not match the component";
    case ECS_OUT_OF_MEMORY:
        return "out of memory";
    case ECS_MODULE_UNDEFINED:
        return "module is undefined";
    case ECS_COLUMN_INDEX_OUT_OF_RANGE:
        return "column index out of range";
    case ECS_COLUMN_IS_NOT_SHARED:
        return "column is not shared";
    case ECS_COLUMN_IS_SHARED:
        return "column is shared";
    case ECS_COLUMN_HAS_NO_DATA:
        return "column has no data";
    case ECS_COLUMN_TYPE_MISMATCH:
        return "column retrieved with mismatching type";
    case ECS_INVALID_WHILE_MERGING:
        return "operation is invalid while merging";
    case ECS_INVALID_WHILE_ITERATING:
        return "operation is invalid while iterating";    
    case ECS_INVALID_FROM_WORKER:
        return "operation is invalid from worker thread";
    case ECS_UNRESOLVED_IDENTIFIER:
        return "unresolved identifier";
    case ECS_OUT_OF_RANGE:
        return "index is out of range";
    case ECS_COLUMN_IS_NOT_SET:
        return "column is not set (use ecs_column_test for optional columns)";
    case ECS_UNRESOLVED_REFERENCE:
        return "unresolved reference for system";
    case ECS_THREAD_ERROR:
        return "failed to create thread";
    case ECS_MISSING_OS_API:
        return "missing implementation for OS API function";
    case ECS_TYPE_TOO_LARGE:
        return "type contains too many entities";
    case ECS_INVALID_PREFAB_CHILD_TYPE:
        return "a prefab child type must have at least one INSTANCEOF element";
    case ECS_UNSUPPORTED:
        return "operation is unsupported";
    case ECS_NO_OUT_COLUMNS:
        return "on demand system has no out columns";
    case ECS_COLUMN_ACCESS_VIOLATION:
        return "invalid access to readonly column (use const)";
    case ECS_DESERIALIZE_COMPONENT_ID_CONFLICT:
        return "serialized data contains conflicting component id";
    case ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT:
        return "serialized data contains conflicting component size";   
    case ECS_DESERIALIZE_FORMAT_ERROR:
        return "serialized data has invalid format";
    case ECS_INVALID_REACTIVE_SIGNATURE:
        return "signature is not valid for reactive system (must contain at least one ANY column)";
    case ECS_INCONSISTENT_COMPONENT_NAME:
        return "component registered twice with a different name";
    case ECS_TYPE_CONSTRAINT_VIOLATION:
        return "type constraint violated";
    }

    return "unknown error code";
}

static
ecs_data_t* init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, count = table->column_count;
    
    result->entities = NULL;
    result->record_ptrs = NULL;
    result->marked_dirty = false;

    /* Root tables don't have columns */
    if (!count) {
        result->columns = NULL;
        return result;
    }

    result->columns = ecs_os_calloc(ECS_SIZEOF(ecs_column_t) * count);
    
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* Is the column a component? */
        const EcsComponent *component = ecs_component_from_id(world, e);
        if (component) {
            /* Is the component associated wit a (non-empty) type? */
            if (component->size) {
                /* This is a regular component column */
                result->columns[i].size = ecs_to_i16(component->size);
                result->columns[i].alignment = ecs_to_i16(component->alignment);
            } else {
                /* This is a tag */
            }
        } else {
            /* This is an entity that was added to the type */
        }
    }

    return result;
}

/* Delete data for a stage */
static
void deinit_data(
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = table->column_count;
        for (c = 0; c < column_count; c ++) {
            ecs_vector_free(columns[c].data);
        }
        ecs_os_free(columns);
        data->columns = NULL;
    }

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;
}

/* Utility function to free data for all stages */
static
void deinit_all_data(
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_vector_first(table->data, ecs_data_t);
    int32_t i, count = ecs_vector_count(table->data);
    
    for (i = 0; i < count; i ++) {
        deinit_data(table, &data[i]);
    }

    ecs_vector_free(table->data);

    table->data = NULL; 
}

static
void run_un_set_handlers(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);

    if (count) {
        ecs_run_monitors(world, &world->stage, table, table->un_set_all, 
            0, count, NULL);
    }
}

static
void run_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool dtor_only)
{
    if (count) {
        ecs_entities_t components = ecs_type_to_entities(table->type);
        ecs_column_info_t *cinfo = NULL;
        ecs_column_info_t cinfo_buff[ECS_MAX_ADD_REMOVE];
        if (components.count < ECS_MAX_ADD_REMOVE) {
            cinfo = cinfo_buff;
            ecs_get_column_info(world, table, &components, cinfo, true);
        } else {
            cinfo = ecs_os_malloc(ECS_SIZEOF(ecs_column_info_t) * components.count);
            ecs_get_column_info(world, table, &components, cinfo, true);
        }

        if (!dtor_only) {
            ecs_run_monitors(world, &world->stage, table, NULL, 
                    row, count, table->un_set_all);
        }

        /* Run deinit actions (dtors) for components. Don't run triggers */
        ecs_components_destruct(world, &world->stage, data, row, count, 
            cinfo, components.count);

        if (cinfo != cinfo_buff) {
            ecs_os_free(cinfo);
        }
    }
}

void ecs_table_destruct(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_data_t *data, 
    int32_t row, 
    int32_t count)
{
    if (table->flags & EcsTableHasDtors) {
        run_remove_actions(world, table, data, row, count, true);
    }
}

static
int compare_matched_query(
    const void *ptr1,
    const void *ptr2)
{
    const ecs_matched_query_t *m1 = ptr1;
    const ecs_matched_query_t *m2 = ptr2;
    ecs_query_t *q1 = m1->query;
    ecs_query_t *q2 = m2->query;
    ecs_assert(q1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(q2 != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t s1 = q1->system;
    ecs_entity_t s2 = q2->system;
    ecs_assert(s1 != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(s2 != 0, ECS_INTERNAL_ERROR, NULL);

    return (s1 > s2) - (s1 < s2);
}

static
void add_monitor(
    ecs_vector_t **array,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Add the system to a list that contains all OnSet systems matched with
     * this table. This makes it easy to get the list of systems that need to be
     * executed when all components are set, like when new_w_data is used */
    ecs_matched_query_t *m = ecs_vector_add(array, ecs_matched_query_t);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    m->query = query;
    m->matched_table_index = matched_table_index;

    /* Sort the system list so that it is easy to get the difference OnSet
     * OnSet systems between two tables. */
    qsort(
        ecs_vector_first(*array, ecs_matched_query_t), 
        ecs_to_size_t(ecs_vector_count(*array)),
        ECS_SIZEOF(ecs_matched_query_t), 
        compare_matched_query);
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
static
void register_monitor(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* First check if system is already registered as monitor. It is possible
     * the query just wants to update the matched_table_index (for example, if
     * query tables got reordered) */
    ecs_vector_each(table->monitors, ecs_matched_query_t, m, {
        if (m->query == query) {
            m->matched_table_index = matched_table_index;
            return;
        }
    });

    add_monitor(&table->monitors, query, matched_table_index);

#ifndef NDEBUG
    char *str = ecs_type_str(world, table->type);
    ecs_trace_2("monitor #[green]%s#[reset] registered with table #[red]%s",
        ecs_get_name(world, query->system), str);
    ecs_os_free(str);
#endif
}

static
bool is_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t comp)
{
    if (!(table->flags & EcsTableHasBase)) {
        return false;
    }

    ecs_type_t type = table->type;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities[i];
        if (e & ECS_INSTANCEOF) {
            if (ecs_has_entity(world, e & ECS_ENTITY_MASK, comp)) {
                return true;
            }
        } else {
            /* ECS_INSTANCEOF will always appear at the end of a type */
            return false;
        }
    }

    return false;
}

static
void register_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;

    if (table->column_count) {
        if (!table->on_set) {
            table->on_set = ecs_os_calloc(ECS_SIZEOF(ecs_vector_t) * table->column_count);
        }

        /* Keep track of whether query matches overrides. When a component is
         * removed, diffing these arrays between the source and detination
         * tables gives the list of OnSet systems to run, after exposing the
         * component that was overridden. */
        bool match_override = false;

        /* Add system to each matched column. This makes it easy to get the list of
        * systems when setting a single component. */
        ecs_vector_each(query->sig.columns, ecs_sig_column_t, column, {
            ecs_sig_oper_kind_t oper_kind = column->oper_kind;
            ecs_sig_from_kind_t from_kind = column->from_kind;

            if (from_kind != EcsFromAny && from_kind != EcsFromOwned) {
                continue;
            }

            if (oper_kind == EcsOperAnd || oper_kind == EcsOperOptional) {
                ecs_entity_t comp = column->is.component;
                int32_t index = ecs_type_index_of(table->type, comp);
                if (index == -1) {
                    continue;
                }

                if (index >= table->column_count) {
                    continue;
                }
                
                ecs_vector_t *set_c = table->on_set[index];
                ecs_matched_query_t *m = ecs_vector_add(&set_c, ecs_matched_query_t);
                m->query = query;
                m->matched_table_index = matched_table_index;
                table->on_set[index] = set_c;
                
                match_override |= is_override(world, table, comp);
            }
        });   

        if (match_override) {
            add_monitor(&table->on_set_override, query, matched_table_index);
        }
    }

    add_monitor(&table->on_set_all, query, matched_table_index);   
}

static
void register_un_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    add_monitor(&table->un_set_all, query, matched_table_index);
}

/* -- Private functions -- */

/* If table goes from 0 to >0 entities or from >0 entities to 0 entities notify
 * queries. This allows systems associated with queries to move inactive tables
 * out of the main loop. */
void ecs_table_activate(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    bool activate)
{
    if (query) {
        ecs_query_activate_table(world, query, table, activate);
        #ifndef NDEBUG
            char *expr = ecs_type_str(world, table->type);
            ecs_trace_2("table #[green][%s]#[reset] %s for single query", expr, 
                activate ? "activated" : "deactivatd");
            ecs_os_free(expr);
        #endif           
    } else {
        ecs_vector_t *queries = table->queries;
        
        if (queries) {
            ecs_query_t **buffer = ecs_vector_first(queries, ecs_query_t*);
            int32_t i, count = ecs_vector_count(queries);
            for (i = 0; i < count; i ++) {
                ecs_query_activate_table(world, buffer[i], table, activate);
            }
        }

        #ifndef NDEBUG
            if (ecs_vector_count(queries)) {
                char *expr = ecs_type_str(world, table->type);
                ecs_trace_2("table #[green][%s]#[reset] %s for %d queries", expr, 
                    activate ? "activated" : "deactivated",
                    ecs_vector_count(queries));
                ecs_os_free(expr);
            }
        #endif         
    }     
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Register system with the table */
    if (!(query->flags & EcsQueryNoActivation)) {
        ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
        if (q) *q = query;

        ecs_data_t *data = ecs_table_get_data(world, table);
        if (data && ecs_vector_count(data->entities)) {
            ecs_table_activate(world, table, query, true);
        }
    }

    /* Register the query as a monitor */
    if (query->flags & EcsQueryMonitor) {
        table->flags |= EcsTableHasMonitors;
        register_monitor(world, table, query, matched_table_index);
    }

    /* Register the query as an on_set system */
    if (query->flags & EcsQueryOnSet) {
        register_on_set(world, table, query, matched_table_index);
    }

    /* Register the query as an un_set system */
    if (query->flags & EcsQueryUnSet) {
        register_un_set(world, table, query, matched_table_index);
    }
}

static
ecs_data_t* ecs_table_get_data_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    bool create)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *data = table->data;

    /* If the table doesn't contain any staged data and we're not asked to
     * create data, don't allocate the array. This will reduce memory footprint
     * for tables that don't contain data but are used for graph traversal. */
    if (!data && !create) {
        return NULL;
    }

    ecs_data_t *data_array = ecs_vector_first(data, ecs_data_t);
    int32_t count = ecs_vector_count(data);
    int32_t stage_count = world->stage_count;
    int32_t id = stage->id;

    ecs_assert(id < stage_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!create || id || !world->in_progress, ECS_INTERNAL_ERROR, NULL);

    /* Make sure the array is large enough for the number of active stages. This
     * guarantees that any pointers returned by this function are stable, unless
     * the number of stages changes (can happen when the number of worker 
     * threads changes) */
    if (count != stage_count) {
        if (stage_count > count) {
            /* Grow array, initialize table data to 0 */
            ecs_vector_set_count(&table->data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->data, ecs_data_t);
            ecs_os_memset(&data_array[count], 
                0, ECS_SIZEOF(ecs_data_t) * (stage_count - count));
        } else {
            /* If the number of stages is reduced, deinit redudant stages */
            int i;
            for (i = stage_count; i < count; i ++) {
                deinit_data(table, &data_array[i]);
            }

            ecs_vector_set_count(&table->data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->data, ecs_data_t);           
        }
    }
    
    return &data_array[id];
}

ecs_data_t* ecs_table_get_data(
    ecs_world_t *world,
    ecs_table_t *table)
{
    return ecs_table_get_data_intern(world, &world->stage, table, false);
}

ecs_data_t* ecs_table_get_staged_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    return ecs_table_get_data_intern(world, stage, table, false);
}

ecs_data_t* ecs_table_get_or_create_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    ecs_data_t *result = ecs_table_get_data_intern(world, stage, table, true);

    /* If write access is requested from table and stage is not the main 
     * stage mark this table as dirty as it will contain staged data. */
    if (world->in_progress && !result->marked_dirty) {
        ecs_table_t** table_ptr = ecs_vector_add(
                &stage->dirty_tables, ecs_table_t*);
        *table_ptr = table;

        /* Don't add table multiple times. Value is reset during merge */
        result->marked_dirty = true;
    }

    return result;   
}

void ecs_table_clear_data(
    ecs_table_t *table,
    ecs_data_t *data)
{
    deinit_data(table, data);
}

/* Clear columns. Deactivate table in systems if necessary, but do not invoke
 * OnRemove handlers. This is typically used when restoring a table to a
 * previous state. */
void ecs_table_clear_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data) {
        return;
    }

    int32_t count = ecs_vector_count(data->entities);
    
    deinit_all_data(table);

    if (count) {
        ecs_table_activate(world, table, 0, false);
    }
}

/* Delete all entities in table, invoke OnRemove handlers. This function is used
 * when an application invokes delete_w_filter. Use ecs_table_clear_silent, as the
 * table may have to be deactivated with systems. */
void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        run_remove_actions(
            world, table, data, 0, ecs_table_data_count(data), false);

        ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
        int32_t i, count = ecs_vector_count(data->entities);
        for(i = 0; i < count; i ++) {
            ecs_eis_delete(&world->stage, entities[i]);
        }
    }

    ecs_table_clear_silent(world, table);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void ecs_table_unset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        run_un_set_handlers(world, table, data);
    }   
}

/* Free table resources. Do not invoke handlers and do not activate/deactivate
 * table with systems. This function is used when the world is freed. */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        run_remove_actions(
            world, table, data, 0, ecs_table_data_count(data), false);
    }

    deinit_all_data(table);
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_vector_free((ecs_vector_t*)table->type);
    ecs_os_free(table->dirty_state);
    ecs_vector_free(table->monitors);
    ecs_vector_free(table->on_set_all);
    ecs_vector_free(table->on_set_override);
    ecs_vector_free(table->un_set_all);
    
    if (table->on_set) {
        int32_t i;
        for (i = 0; i < table->column_count; i ++) {
            ecs_vector_free(table->on_set[i]);
        }
        ecs_os_free(table->on_set);
    }
}

/* Reset a table to its initial state. This is used to reset the root table of a
 * stage after a merge has occurred, so that it no longer refers to tables that
 * were created in the stage. */
void ecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;

    if (table->lo_edges) {
        memset(table->lo_edges, 0, ECS_SIZEOF(ecs_edge_t) * ECS_HI_COMPONENT_ID);
        ecs_map_clear(table->hi_edges);
    }
}

static
void mark_table_dirty(
    ecs_table_t *table,
    int32_t index)
{
    if (table->dirty_state) {
        table->dirty_state[index] ++;
    }
}

void ecs_table_mark_dirty_w_index(
    ecs_table_t *table,
    int32_t index)
{
    mark_table_dirty(table, index);
}

void ecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (table->dirty_state) {
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        table->dirty_state[index] ++;
    }
}

int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t column_count = table->column_count;
    bool realloc = false;

    if (column_count) {
        ecs_column_t *columns = data->columns;

        /* It is possible that the table data was created without content. Now that
        * data is going to be written to the table, initialize it */ 
        if (!columns) {
            init_data(world, table, data);
            columns = data->columns;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Add elements to each column array */
        int32_t i;
        for (i = 0; i < column_count; i ++) {
            int16_t size = columns[i].size;
            int16_t alignment = columns[i].alignment;
            if (size) {
                ecs_vector_t *prev = columns[i].data;
                ecs_vector_add_t(&columns[i].data, size, alignment);
                realloc = realloc || (prev != columns[i].data);
            }
        }
    }

    /* Fist add entity to array with entity ids */
    ecs_vector_t *prev_e = data->entities;
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;
    realloc = realloc || (prev_e != data->entities);

    /* Add record ptr to array with record ptrs */
    ecs_vector_t *prev_r = data->record_ptrs;
    ecs_record_t **r = ecs_vector_add(&data->record_ptrs, ecs_record_t*);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    *r = record;
    realloc = realloc || (prev_r != data->record_ptrs);

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);

    int32_t index = ecs_vector_count(data->entities) - 1;

    if (!world->in_progress && !index) {
        ecs_table_activate(world, table, 0, true);
    }

    /* Keep track of alloc count */
    table->alloc_count += realloc;

    /* Return index of last added entity */
    return index;
}

void ecs_table_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *entity_column = data->entities;
    int32_t count = ecs_vector_count(entity_column);

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = table->column_count;
    int32_t i;

    if (index != count) {
        /* Move last entity id to index */     
        ecs_entity_t *entities = ecs_vector_first(entity_column, ecs_entity_t);
        ecs_entity_t entity_to_move = entities[count];
        entities[index] = entity_to_move;
        ecs_vector_remove_last(entity_column);

        /* Move last record ptr to index */
        ecs_vector_t *record_column = data->record_ptrs;     
        ecs_record_t **records = ecs_vector_first(record_column, ecs_record_t*);
        ecs_record_t *record_to_move = records[count];

        records[index] = record_to_move;
        ecs_vector_remove_last(record_column);

        /* Move each component value in array to index */
        ecs_column_t *components = data->columns;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *component_column = &components[i];
            int16_t size = component_column->size;
            int16_t alignment = component_column->alignment;
            if (size) {
                ecs_vector_remove_index_t(
                    component_column->data, size, alignment, index);
            }
        }

        /* Update record of moved entity in entity index */
        if (!world->in_progress && record_to_move) {
            record_to_move->row = index + 1;
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
        } else {
            ecs_record_t row;
            row.table = table;
            row.row = index + 1;
            ecs_eis_set(stage, entity_to_move, &row);
        }

    /* If this is the last entity in the table, just decrease column counts */
    } else {
        ecs_vector_remove_last(entity_column);
        ecs_vector_remove_last(data->record_ptrs);

        ecs_column_t *components = data->columns;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *component_column = &components[i];
            if (component_column->size) {
                ecs_vector_remove_last(component_column->data);
            }
        }
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    if (!world->in_progress && !count) {
        ecs_table_activate(world, table, NULL, false);
    }
}

int32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count,
    ecs_entity_t first_entity)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = table->column_count;
    ecs_column_t *columns = NULL;

    if (column_count) {
        columns = data->columns;

        /* It is possible that the table data was created without content. Now that
        * data is going to be written to the table, initialize it */ 
        if (!columns) {
            init_data(world, table, data);
            columns = data->columns;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    /* Fist grow record ptr array */
    ecs_record_t **r = ecs_vector_addn(&data->record_ptrs, ecs_record_t*, count);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);    

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_addn(&data->entities, ecs_entity_t, count);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < count; i ++) {
        e[i] = first_entity + (ecs_entity_t)i;
        r[i] = NULL;
    }

    /* Add elements to each column array */
    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        if (!size) {
            continue;
        }
        int16_t alignment = columns[i].alignment;

        ecs_vector_addn_t(&columns[i].data, size, alignment, count);
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    int32_t row_count = ecs_vector_count(data->entities);
    if (!world->in_progress && row_count == count) {
        ecs_table_activate(world, table, 0, true);
    }

    table->alloc_count ++;

    /* Return index of first added entity */
    return row_count - count;
}

int16_t ecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    int32_t i, column_count = table->column_count;

    if (!columns && column_count) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_vector_set_size(&data->entities, ecs_entity_t, count);
    ecs_vector_set_size(&data->record_ptrs, ecs_record_t*, count);

    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;
        if (size) {
            ecs_vector_set_size_t(&columns[i].data, size, alignment, count);
        }
    }
    
    table->alloc_count ++;

    return 0;
}

int16_t ecs_table_set_count(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    int32_t i, column_count = table->column_count;

    if (!columns && column_count) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_vector_set_count(&data->entities, ecs_entity_t, count);
    ecs_vector_set_count(&data->record_ptrs, ecs_record_t*, count);

    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;
        if (size) {
            ecs_vector_set_count_t(&columns[i].data, size, alignment, count);
        }
    }

    table->alloc_count ++;

    return 0;
}

int32_t ecs_table_data_count(
    ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

int32_t ecs_table_count(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = ecs_vector_first(table->data, ecs_data_t);
    if (!data) {
        return 0;
    }

    return ecs_table_data_count(data);
}

void ecs_table_swap(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2)
{    
    (void)world;

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);
    
    if (row_1 == row_2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_record_t *record_ptr_1 = record_ptrs[row_1];
    ecs_record_t *record_ptr_2 = record_ptrs[row_2];
    
    /* Get pointers to records in entity index */
    if (!record_ptr_1) {
        record_ptr_1 = ecs_eis_get(stage, e1);
    }

    if (!record_ptr_2) {
        record_ptr_2 = ecs_eis_get(stage, e2);
    }

    /* Swap entities */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = row_2 + 1;
    record_ptr_2->row = row_1 + 1;
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    /* Swap columns */
    int32_t i, column_count = table->column_count;
    
    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;
        void *ptr = ecs_vector_first_t(columns[i].data, size, alignment);

        if (size) {
            void *tmp = ecs_os_alloca(size);

            void *el_1 = ECS_OFFSET(ptr, size * row_1);
            void *el_2 = ECS_OFFSET(ptr, size * row_2);

            ecs_os_memcpy(tmp, el_1, size);
            ecs_os_memcpy(el_1, el_2, size);
            ecs_os_memcpy(el_2, tmp, size);
        }
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    
}

static
void merge_vector(
    ecs_vector_t **dst_out,
    ecs_vector_t *src,
    int16_t size,
    int16_t alignment)
{
    ecs_vector_t *dst = *dst_out;
    int32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        *dst_out = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = ecs_vector_count(src);
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vector_free(src);
        *dst_out = dst;
    }
}

static
void merge_table_data(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t *old_data,
    ecs_data_t *new_data)
{
    int32_t i_new, new_component_count = new_table->column_count;
    int32_t i_old = 0, old_component_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_table->type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_table->type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    if (!new_columns && !new_data->entities) {
        init_data(world, new_table, new_data);
        new_columns = new_data->columns;
    }

    if (!old_count) {
        return;
    }

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];
        int16_t size = new_columns[i_new].size;
        int16_t alignment = new_columns[i_new].alignment;

        if ((new_component & ECS_TYPE_ROLE_MASK) || 
            (old_component & ECS_TYPE_ROLE_MASK)) 
        {
            break;
        }

        if (new_component == old_component) {
            merge_vector(
                &new_columns[i_new].data, old_columns[i_old].data, size, 
                alignment);

            old_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_table_dirty(new_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            if (size) {
                ecs_vector_set_count_t(&new_columns[i_new].data, size, alignment,
                    old_count + new_count);
            }
            i_new ++;
        } else if (new_component > old_component) {
            /* Old column does not occur in new table, remove */
            ecs_vector_free(old_columns[i_old].data);
            old_columns[i_old].data = NULL;
            i_old ++;
        }
    }

    /* Initialize remaining columns */
    for (; i_new < new_component_count; i_new ++) {
        int16_t size = new_columns[i_new].size;
        int16_t alignment = new_columns[i_new].alignment;

        if (size) {
            ecs_vector_set_count_t(&new_columns[i_new].data, size, alignment,
            old_count + new_count);
        }
    }

    /* Merge entities */
    merge_vector(&new_data->entities, old_data->entities, ECS_SIZEOF(ecs_entity_t), 
        ECS_ALIGNOF(ecs_entity_t));
    old_data->entities = NULL;

    ecs_assert(ecs_vector_count(new_data->entities) == old_count + new_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    merge_vector(&new_data->record_ptrs, old_data->record_ptrs, 
        ECS_SIZEOF(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    old_data->record_ptrs = NULL;    

    /* Mark entity column as dirty */
    mark_table_dirty(new_table, 0); 
}

void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_table != old_table, ECS_INTERNAL_ERROR, NULL);

    /* If there is nothing to merge to, just clear the old table */
    if (!new_table) {
        ecs_table_clear(world, old_table);
        return;
    }

    /* If there is no data to merge, drop out */
    ecs_data_t *old_data = ecs_table_get_data(world, old_table);
    if (!old_data) {
        return;
    }

    ecs_data_t *new_data = ecs_table_get_or_create_data(
        world, &world->stage, new_table);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *old_entities = ecs_vector_first(old_data->entities, ecs_entity_t);

    int32_t old_count = ecs_vector_count(old_data->entities);
    int32_t new_count = ecs_vector_count(new_data->entities);

    ecs_record_t **old_records = ecs_vector_first(old_data->record_ptrs, ecs_record_t*);

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < old_count; i ++) {
        ecs_record_t *record = old_records[i];
        if (!record) {
            record = ecs_eis_get(&world->stage, old_entities[i]);
        }

        bool is_monitored = record->row < 0;
        record->row = ecs_row_to_record(new_count + i, is_monitored);
        record->table = new_table;
    }

    /* Merge table columns */
    merge_table_data(world, new_table, old_table, old_count, new_count, 
        old_data, new_data);

    new_table->alloc_count ++;
}

void ecs_table_merge_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t old_count = 0, new_count = new_count = ecs_table_data_count(data);
    ecs_data_t *old_data = ecs_vector_first(table->data, ecs_data_t);
    ecs_assert(!data || data != old_data, ECS_INTERNAL_ERROR, NULL);

    /* Merge data with existing table data */
    if (old_data) {
        old_count = ecs_table_data_count(old_data);
        if (!old_count) {
            /* If table was empty, just copy in data */
            deinit_data(table, old_data);
            *old_data = *data;
        } else {
            /* If table has entities, merge with new data */
            merge_table_data(world, table, table, new_count, old_count, 
                data, old_data);
            ecs_os_free(data->columns);
        }
    } else {
        old_data = ecs_table_get_or_create_data(world, &world->stage, table);
        *old_data = *data;
    }

    /* Make sure record ptrs array is large enough */
    ecs_vector_set_count(&old_data->record_ptrs, ecs_record_t*, 
        old_count + new_count);

    /* Update entity index */
    ecs_entity_t *entities = ecs_vector_first(old_data->entities, ecs_entity_t);
    ecs_record_t **record_ptrs = ecs_vector_first(old_data->record_ptrs, ecs_record_t*);
    int32_t i, count = ecs_vector_count(old_data->entities);

    ecs_assert(count == (old_count + new_count), ECS_INTERNAL_ERROR, NULL);

    for (i = old_count; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_record_t *r = ecs_ei_get_or_create(&world->stage.entity_index, e);
        record_ptrs[i] = r;

        r->table = table;
        r->row = ecs_row_to_record(i, false);
    }

    /* Run OnSet systems for merged entities */
    ecs_entities_t components = ecs_type_to_entities(table->type);
    ecs_run_set_systems(world, &world->stage, &components, table, old_data,
        old_count, new_count, true);

    /* Activate table if it went from empty to not empty */
    if (!old_count && new_count) {
        ecs_table_activate(world, table, 0, true);
    }

    table->alloc_count ++;
}

void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = ecs_vector_first(table->data, ecs_data_t);
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        run_remove_actions(
            world, table, table_data, 0, ecs_table_data_count(table_data), false);
        deinit_data(table, table_data);
    }

    if (data) {
        table_data = ecs_table_get_or_create_data(world, &world->stage, table);
        *table_data = *data;
    } else {
        return;
    }

    int32_t count = ecs_table_count(table);

    if (!prev_count && count) {
        ecs_table_activate(world, table, 0, true);
    } else if (prev_count && !count) {
        ecs_table_activate(world, table, 0, false);
    }
}

void ecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool is_copy)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t i_new = 0, new_column_count = new_table->column_count;
    int32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            int16_t size = new_column->size;
            int16_t alignment = new_column->alignment;

            if (size) {
                void *dst = ecs_vector_get_t(new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(old_column->data, size, alignment, old_index);
                    
                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

                if (is_copy) {
                    ecs_c_info_t *cdata = ecs_get_c_info(
                        world, new_component);

                    ecs_xtor_t ctor;
                    ecs_copy_t copy;
                    if (cdata && (ctor = cdata->lifecycle.ctor) && (copy = cdata->lifecycle.copy)) {
                        void *ctx = cdata->lifecycle.ctx;
                        ctor(world, new_component, &dst_entity, dst, 
                            ecs_to_size_t(size), 1, ctx);
                        copy(world, new_component, &dst_entity, &src_entity, 
                            dst, src, ecs_to_size_t(size), 1, ctx);
                    } else {
                        ecs_os_memcpy(dst, src, size);
                    }
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

bool ecs_table_match_filter(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_filter_t *filter)
{
    if (!filter) {
        return true;
    }

    ecs_type_t type = table->type;
    
    if (filter->include) {
        /* If filter kind is exact, types must be the same */
        if (filter->include_kind == EcsMatchExact) {
            if (type != filter->include) {
                return false;
            }

        /* Default for include_kind is MatchAll */
        } else if (!ecs_type_contains(world, type, filter->include, 
            filter->include_kind != EcsMatchAny, true)) 
        {
            return false;
        }
    }

    if (filter->exclude) {
        /* If filter kind is exact, types must be the same */
        if (filter->exclude_kind == EcsMatchExact) {
            if (type == filter->exclude) {
                return false;
            }
        
        /* Default for exclude_kind is MatchAny */                
        } else if (ecs_type_contains(world, type, filter->exclude, 
            filter->exclude_kind == EcsMatchAll, true))
        {
            return false;
        }
    }

    return true;
}

int32_t* ecs_table_get_dirty_state(
    ecs_table_t *table)
{
    if (!table->dirty_state) {
        table->dirty_state = ecs_os_calloc(ECS_SIZEOF(int32_t) * (table->column_count + 1));
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    return table->dirty_state;
}

int32_t* ecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = ecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = table->column_count;
    return ecs_os_memdup(dirty_state, (column_count + 1) * ECS_SIZEOF(int32_t));
}


static
int32_t comp_mask_index(
    int32_t value)
{
    return value >> 6;
}

static
void comp_mask_set(
    ecs_comp_mask_t mask,
    ecs_entity_t value)
{
    ecs_assert(value < UINT_MAX, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    int32_t index = comp_mask_index((int32_t)value);
    mask[index] |= (ecs_entity_t)1 << (value & 0x3F);
}

static
bool comp_mask_is_set(
    ecs_comp_mask_t mask,
    ecs_entity_t value)
{
    ecs_assert(value < UINT_MAX, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value < ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
    int32_t index = comp_mask_index((int32_t)value);
    return (mask[index] & (ecs_entity_t)1 << (value & 0x3F)) != 0;
}

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **c_info);

static 
void* get_component_w_index(
    ecs_entity_info_t *info,
    int32_t index)
{
    ecs_data_t *data = info->data;
    ecs_column_t *columns;
    if (!data || !(columns = data->columns)) {
        return NULL;
    }

    ecs_assert(index < info->table->column_count, ECS_INVALID_COMPONENT_ID, NULL);

    ecs_column_t *column = &columns[index];
    ecs_vector_t *data_vec = column->data;
    int16_t size = column->size; 

    ecs_assert(!size || data_vec != NULL, ECS_INTERNAL_ERROR, NULL);

    if (size) {
        void *ptr = ecs_vector_first_t(data_vec, size, column->alignment);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        return ECS_OFFSET(ptr, info->row * size);
    }

    return NULL;
}

/* Get pointer to single component value */
static
void* get_component(
    ecs_entity_info_t *info,
    ecs_entity_t component)
{
    ecs_table_t *table = info->table;
    if (!table) {
        return NULL;
    }

    ecs_assert(info->row >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == component) {
            return get_component_w_index(info, c_ptr_i);
        }
    });
    
    return NULL;
}

/* Utility to compute actual row from row in record */
static
int32_t set_row_info(
    ecs_entity_info_t *info,
    int32_t row)
{
    return info->row = ecs_record_to_row(row, &info->is_watched);
}

/* Utility to set info from main stage record */
static
void set_info_from_record(
    ecs_world_t *world,
    ecs_entity_info_t *info,
    ecs_record_t *record)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    info->record = record;

    ecs_table_t *table = record->table;

    set_row_info(info, record->row);

    info->table = table;
    if (!info->table) {
        return;
    }

    ecs_data_t *data = ecs_table_get_data(world, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    info->data = data;

    ecs_assert(ecs_vector_count(data->entities) > info->row, 
        ECS_INTERNAL_ERROR, NULL);
}

/* Get info from main stage */
static
bool get_info(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_record_t *record = ecs_eis_get(&world->stage, entity);
    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        info->record = NULL;
        return false;
    }

    set_info_from_record(world, info, record);

    return true;
}

/* Get info from any stage but main stage */
static
bool get_staged_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_get(stage, entity);
    if (!record) {
        info->table = NULL;
        info->is_watched = false;
        info->record = NULL;
        return false;
    }

    int32_t row = set_row_info(info, record->row);    

    ecs_table_t *table = record->table;

    info->table = table;
    if (!info->table) {
        info->record = NULL;
        return true;
    }

    ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
    info->data = data;

    ecs_assert(data->record_ptrs != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(data->entities) > row, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(data->record_ptrs) > row, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t**record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);

    info->record = record_ptrs[row];

    return true;
}

/* Get entity info */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info)
{
    if (stage == &world->stage) {
        return get_info(world, entity, info);
    } else {
        if (!get_staged_info(world, stage, entity, info)) {
            return get_info(world, entity, info);
        }

        return true;
    }    
}

void ecs_get_column_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *components,
    ecs_column_info_t *cinfo,
    bool get_all)
{
    int32_t column_count = table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(table->type, ecs_entity_t);

    if (get_all) {
        int32_t i, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = type_array[i];
            cinfo[i].id = id;
            cinfo[i].ci = ecs_get_c_info(world, id);
            cinfo[i].column = i;            
        }
    } else {
        ecs_entity_t *array = components->array;
        int32_t i, cur, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = array[i];
            cinfo[i].id = id;
            cinfo[i].ci = ecs_get_c_info(world, id);
            cinfo[i].column = -1;

            for (cur = 0; cur < column_count; cur ++) {
                if (type_array[cur] == id) {
                    cinfo[i].column = cur;
                    break;
                }
            }
        }
    }
}

static
void run_component_trigger_for_entities(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *trigger_vec,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{    
    (void)world;
    int32_t i, trigger_count = ecs_vector_count(trigger_vec);
    if (trigger_count) {
        EcsTrigger *triggers = ecs_vector_first(trigger_vec, EcsTrigger);
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
        index ++;

        ecs_entity_t components[1] = { component };
        int32_t columns[1] = { index };

        ecs_iter_t it = {
            .world = stage->world,
            .columns = columns,
            .table_count = 1,
            .inactive_table_count = 1,
            .column_count = 1,
            .table = table,
            .table_columns = data->columns,
            .components = components,
            .entities = entities,
            .offset = row,
            .count = count,
        };

        for (i = 0; i < trigger_count; i ++) {
            it.system = triggers[i].self;
            it.param = triggers[i].ctx;
            triggers[i].action(&it);
        }
    }
}

void ecs_run_component_trigger(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_vector_t *trigger_vec,
    ecs_entity_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    if (table->flags & EcsTableIsPrefab) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    run_component_trigger_for_entities(
        world, stage, trigger_vec, component, table, data, row, count, entities);
}

#ifdef FLECS_SYSTEM 
static
void run_set_systems_for_entities(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities,
    bool set_all)
{   
    /* Run OnSet systems */
    if (set_all) {
        ecs_vector_t *queries = table->on_set_all;
        ecs_vector_each(queries, ecs_matched_query_t, m, {
            ecs_run_monitor(world, stage, m, components, row, count, entities);
        });
    } else {
        ecs_vector_t **on_set_systems = table->on_set;
        if (on_set_systems) {
            int32_t index = ecs_type_index_of(table->type, components->array[0]);
            ecs_vector_t *queries = on_set_systems[index];
            ecs_vector_each(queries, ecs_matched_query_t, m, {
                ecs_run_monitor(world, stage, m, components, row, count, entities);
            });
        }
    }
}
#endif

void ecs_run_set_systems(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool set_all)
{
#ifdef FLECS_SYSTEM    
    if (!count || !data) {
        return;
    }
    
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    run_set_systems_for_entities(world, stage, components, table, row, 
        count, entities, set_all);
#endif
}

void ecs_run_monitors(
    ecs_world_t *world, 
    ecs_stage_t *stage, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors)
{
#ifdef FLECS_SYSTEM    
    if (v_dst_monitors == v_src_monitors) {
        return;
    }

    if (!v_dst_monitors) {
        return;
    }

    if (dst_table->flags & EcsTableIsPrefab) {
        return;
    }

    if (!v_src_monitors) {
        ecs_vector_each(v_dst_monitors, ecs_matched_query_t, monitor, {
            ecs_run_monitor(world, stage, monitor, NULL, dst_row, count, NULL);
        });
    } else {
        /* If both tables have monitors, run the ones that dst_table has and
         * src_table doesn't have */
        int32_t i, m_count = ecs_vector_count(v_dst_monitors);
        int32_t j = 0, src_count = ecs_vector_count(v_src_monitors);
        ecs_matched_query_t *dst_monitors = ecs_vector_first(v_dst_monitors, ecs_matched_query_t);
        ecs_matched_query_t *src_monitors = ecs_vector_first(v_src_monitors, ecs_matched_query_t);

        for (i = 0; i < m_count; i ++) {
            ecs_matched_query_t *dst = &dst_monitors[i];

            ecs_entity_t system = dst->query->system;
            ecs_assert(system != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_matched_query_t *src = 0;
            while (j < src_count) {
                src = &src_monitors[j];
                if (src->query->system < system) {
                    j ++;
                } else {
                    break;
                }
            }

            if (src->query->system == system) {
                continue;
            }

            ecs_run_monitor(world, stage, dst, NULL, dst_row, count, NULL);
        }
    }
#endif
}

static
int32_t find_prefab(
    ecs_type_t type,
    int32_t n)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *buffer = ecs_vector_first(type, ecs_entity_t);

    for (i = n + 1; i < count; i ++) {
        ecs_entity_t e = buffer[i];
        if (e & ECS_INSTANCEOF) {
            return i;
        }
    }

    return -1;
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_table_t *child_table)
{
    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = ecs_table_get_data(world, child_table);
    int32_t column_count = child_table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t type_count = ecs_vector_count(type);

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_entities_t components = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * type_count)
    };

    void **c_info = NULL;

    if (child_data) {
        c_info = ecs_os_alloca(ECS_SIZEOF(void*) * column_count);
    } else {
        return;
    }

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int i, base_index = -1, pos = 0;

    for (i = 0; i < type_count; i ++) {
        ecs_entity_t c = type_array[i];
        
        /* Make sure instances don't have EcsPrefab */
        if (c == EcsPrefab) {
            continue;
        }

        /* Keep track of the element that creates the CHILDOF relationship with
        * the prefab parent. We need to replace this element to make sure the
        * created children point to the instance and not the prefab */ 
        if (c & ECS_CHILDOF && (c & ECS_ENTITY_MASK) == base) {
            base_index = pos;
        }        

        /* Store pointer to component array. We'll use this component array to
        * create our new entities in bulk with new_w_data */
        if (i < column_count) {
            ecs_column_t *column = &child_data->columns[i];
            c_info[pos] = ecs_vector_first_t(column->data, column->size, column->alignment);
        }

        components.array[pos] = c;
        pos ++;
    }

    ecs_assert(base_index != -1, ECS_INTERNAL_ERROR, NULL);

    components.count = pos;

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t child_count = ecs_vector_count(child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t e = entities[i];

        /* Replace CHILDOF element in the component array with instance id */
        components.array[base_index] = ECS_CHILDOF | e;

        /* Find or create table */
        ecs_table_t *table = ecs_table_find_or_create(
            world, stage, &components);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create children */
        int32_t child_row = new_w_data(
            world, stage, table, NULL, child_count, c_info);

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = ecs_table_get_staged_data(world, stage, table);
        ecs_entity_t *children = ecs_vector_first(child_data->entities, ecs_entity_t);

        int j;
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, stage, child, i_data, child_row, 1);
        }
    }    
}

static
void instantiate(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t base,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{    
    /* If base is a parent, instantiate children of base for instances */
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, base);

    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, child_table_ptr, {
            ecs_table_t *child_table = *child_table_ptr;
            if (!ecs_table_count(child_table)) {
                continue;
            }

            instantiate_children(
                world, stage, base, data, row, count, child_table);
        });
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count);

static
bool override_from_base(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t component,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_entity_info_t base_info;
    if (!get_info(world, base, &base_info)) {
        return false;
    }

    void *base_ptr = get_component(&base_info, component);
    if (base_ptr) {
        int16_t data_size = column->size;
        void *data_array = ecs_vector_first_t(column->data, column->size, column->alignment);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        int32_t index;

        ecs_copy_t copy = cdata ? cdata->lifecycle.copy : NULL;
        if (copy) {
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            void *ctx = cdata->lifecycle.ctx;
            for (index = 0; index < count; index ++) {
                copy(world, component, &base, &entities[row], 
                    data_ptr, base_ptr, ecs_to_size_t(data_size), 1, ctx);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }
        } else {
            for (index = 0; index < count; index ++) {
                ecs_os_memcpy(data_ptr, base_ptr, data_size);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }                    
        }

        return true;
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_info.table->type;
        return override_component(world, component, base_type, data, column, row, 
            count);
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t i, type_count = ecs_vector_count(type);

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (e < ECS_TYPE_ROLE_START) {
            break;
        }

        if (e & ECS_INSTANCEOF) {
            return override_from_base(world, e & ECS_ENTITY_MASK, component, 
                data, column, row, count);
        }
    } while (--i >= 0);

    return false;
}

void ecs_components_construct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    (void) world;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_xtor_t ctor;
        if (!c_info || !(ctor = c_info->lifecycle.ctor)) {
            continue;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_column_t *column = &columns[component_info[i].column];
        int16_t size = column->size;
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t component = component_info[i].id;
        ctor(stage->world, component, &entities[row], 
            ECS_OFFSET(array, size * row), ecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);
    }
}

void ecs_components_destruct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    (void)world;

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);
    if (!component_count) {
        return;
    }

    ecs_column_t *columns = data->columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_xtor_t dtor;
        if (!c_info || !(dtor = c_info->lifecycle.dtor)) {
            continue;
        }

        ecs_column_t *column = &columns[component_info[i].column];
        int16_t size = column->size;
        void *array = ecs_vector_first_t(column->data, size, column->alignment);
        ecs_assert(array != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_entity_t component = component_info[i].id;
        dtor(stage->world, component, &entities[row], 
            ECS_OFFSET(array, size * row), ecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);
    }
}

void ecs_components_override(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count,
    ecs_comp_mask_t set_mask,
    bool run_on_set)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);
    if (!component_count) {
        return;
    }

    ecs_table_t *table_without_base = table;
    ecs_column_t *columns = data->columns;
    ecs_type_t type = table->type;
    int32_t column_count = table->column_count;

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_entity_t component = component_info[i].id;

        if (component >= ECS_HI_COMPONENT_ID) {
            if (component & ECS_INSTANCEOF) {
                ecs_entity_t base = component & ECS_ENTITY_MASK;
                instantiate(world, stage, base, data, row, count);

                /* If table has on_set systems, get table without the base
                 * entity that was just added. This is needed to determine the
                 * diff between the on_set systems of the current table and the
                 * table without the base, as these are the systems that need to
                 * be invoked */
                ecs_entities_t to_remove = {
                    .array = &component,
                    .count = 1
                };
                table_without_base = ecs_table_traverse_remove(world, stage, 
                    table_without_base, &to_remove, NULL);
            }
        }

        int32_t column_index = component_info[i].column;
        if (column_index == -1 || column_index >= column_count) {
            continue;
        }

        if (!comp_mask_is_set(set_mask, component)) {
            ecs_column_t *column = &columns[column_index];
            if (override_component(world, component, type, data, column, 
                row, count)) 
            {
                ecs_entities_t to_remove = {
                    .array = &component,
                    .count = 1
                };
                table_without_base = ecs_table_traverse_remove(world, 
                    stage, table_without_base, &to_remove, NULL);
            }
        }
    }

    /* Run OnSet actions when a base entity is added to the entity for 
     * components not overridden by the entity. */
    if (run_on_set && table_without_base != table) {
        ecs_run_monitors(world, stage, table, table->on_set_all, row, count, 
            table_without_base->on_set_all);
    }
}

void ecs_components_on_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_vector_t *triggers;
        if (!c_info || !(triggers = c_info->on_add)) {
            continue;
        }

        ecs_entity_t component = component_info[i].id;
        ecs_run_component_trigger(
            world, stage, triggers, component, table, data, row, count);
    }
}

void ecs_components_on_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_c_info_t *c_info = component_info[i].ci;
        ecs_vector_t *triggers;
        if (!c_info || !(triggers = c_info->on_remove)) {
            continue;
        }

        ecs_entity_t component = component_info[i].id;
        ecs_run_component_trigger(
            world, stage, triggers, component, table, data, row, count);
    }
}

static
void ecs_delete_children(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, parent);

    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, tptr, {
            ecs_table_t *table = *tptr;
            ecs_table_clear(world, table);
        });
    }
}

void ecs_run_add_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added,
    ecs_comp_mask_t set_mask,
    bool get_all,
    bool run_on_set)
{
    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(added->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];
    ecs_get_column_info(world, table, added, cinfo, get_all);
    int added_count = added->count;

    if (table->flags & EcsTableHasCtors) {
        ecs_components_construct(
            world, stage, data, row, count, cinfo, added_count);
    }

    if (table->flags & EcsTableHasBase) {
        ecs_components_override(
            world, stage, table, data, row, count, cinfo, 
            added_count, set_mask, run_on_set);
    }

    if (table->flags & EcsTableHasOnAdd) {
        ecs_components_on_add(world, stage, table, data, row, count, 
            cinfo, added_count);
    }
}

void ecs_run_remove_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *removed,
    bool get_all)
{
    ecs_assert(removed != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(removed->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];
    ecs_get_column_info(world, table, removed, cinfo, get_all);
    int removed_count = removed->count;

    if (table->flags & EcsTableHasOnRemove) {
        ecs_components_on_remove(world, stage, table, data, 
            row, count, cinfo, removed_count);
    }

    if (table->flags & EcsTableHasDtors) {
        ecs_components_destruct(
            world, stage, data, row, count, cinfo, removed_count);
    }    
}

static
int32_t new_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *new_table,
    ecs_entities_t *added)
{
    ecs_record_t *record = info->record;
    ecs_data_t *new_data = ecs_table_get_or_create_data(world, stage, new_table);
    int32_t new_row;

    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (stage == &world->stage) {
        if (!record) {
            record = ecs_eis_get_or_create(stage, entity);
        }

        new_row = ecs_table_append(
            world, new_table, new_data, entity, record);

        record->table = new_table;
        record->row = ecs_row_to_record(new_row, info->is_watched);
    } else {
        new_row = ecs_table_append(
            world, new_table, new_data, entity, NULL);  

        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = new_table,
            .row = ecs_row_to_record(new_row, info->is_watched)
        });
    }

    ecs_assert(
        ecs_vector_count(new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);    

    if (new_table->flags & EcsTableHasAddActions) {
        ecs_comp_mask_t set_mask = {0};
        ecs_run_add_actions(
            world, stage, new_table, new_data, new_row, 1, added, set_mask, 
            true, true);

        if (new_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(
                world, stage, new_table, new_table->monitors, new_row, 1, NULL);              
        }        
    }   

    info->data = new_data;
    
    return new_row;
}

static
bool compare_stage_w_data(
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_data_t *data_array = ecs_vector_first(table->data, ecs_data_t);
    if (&data_array[stage->id] == data) {
        return true;
    }
    return false;
}

static
int32_t move_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_table_t *dst_table,
    ecs_entities_t *added,
    ecs_entities_t *removed)
{    
    ecs_data_t *dst_data = ecs_table_get_or_create_data(world, stage, dst_table);
    if (src_data == dst_data) {
        return src_row;
    }

    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data->entities) > src_row, ECS_INTERNAL_ERROR, NULL);

    bool main_stage = stage == &world->stage;
    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(&world->stage, entity), ECS_INTERNAL_ERROR, NULL);

    int32_t dst_row = ecs_table_append(world, dst_table, dst_data, entity, record);
    bool same_stage = compare_stage_w_data(stage, src_table, src_data);

    if (main_stage && record) {
        record->table = dst_table;
        record->row = ecs_row_to_record(dst_row, info->is_watched);
    } else {
        ecs_eis_set(stage, entity, &(ecs_record_t){
            .table = dst_table,
            .row = ecs_row_to_record(dst_row, info->is_watched)
        });
    }

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        ecs_table_move(
            world, entity, entity, dst_table, dst_data, dst_row, src_table, 
            src_data, src_row, !same_stage);

        /* If entity was moved, invoke UnSet monitors for each component that
         * the entity no longer has */
        ecs_run_monitors(world, stage, dst_table, src_table->un_set_all, 
            dst_row, 1, dst_table->un_set_all);

        /* If components were removed, invoke remove actions before deleting */
        if (removed && (src_table->flags & EcsTableHasRemoveActions)) {   
            ecs_run_remove_actions(
                world, stage, src_table, src_data, src_row, 1, removed, false);
        }            
    }

    /* Only delete from source table if moving to the same stage */
    if (same_stage) {
        ecs_table_delete(world, stage, src_table, src_data, src_row);
    }

    /* If components were added, invoke add actions */
    if (src_table != dst_table) {
        if (added && (dst_table->flags & EcsTableHasAddActions)) {
            ecs_comp_mask_t set_mask = {0};
            ecs_run_add_actions(
                world, stage, dst_table, dst_data, dst_row, 1, added, set_mask, 
                false, true);
        }

        /* Run monitors */
        if (dst_table->flags & EcsTableHasMonitors) {
            ecs_run_monitors(world, stage, dst_table, dst_table->monitors, dst_row, 
                1, src_table->monitors);
        }

        /* If removed components were overrides, run OnSet systems for those, as 
         * the value of those components changed from the removed component to 
         * the value of component on the base entity */
        if (removed && dst_table->flags & EcsTableHasBase) {
            ecs_run_monitors(world, stage, dst_table, src_table->on_set_override, 
                dst_row, 1, dst_table->on_set_override);          
        }
    }

    info->data = dst_data;

    return dst_row;
}

static
void delete_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_row,
    ecs_entities_t *removed)
{
    if (removed) {
        ecs_run_monitors(world, stage, src_table, src_table->un_set_all, 
            src_row, 1, NULL);

        /* Invoke remove actions before deleting */
        if (src_table->flags & EcsTableHasRemoveActions) {   
            ecs_run_remove_actions(
                world, stage, src_table, src_data, src_row, 1, removed, true);
        } 
    }

    ecs_table_delete(world, stage, src_table, src_data, src_row);
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
bool update_component_monitor_w_array(
    ecs_component_monitor_t *mon,
    ecs_entities_t *entities)
{
    bool childof_changed = false;

    if (!entities) {
        return false;
    }

    int i;
    for (i = 0; i < entities->count; i ++) {
        ecs_entity_t component = entities->array[i];
        if (component < ECS_HI_COMPONENT_ID) {
            ecs_component_monitor_mark(mon, component);
        } else if (component & ECS_CHILDOF) {
            childof_changed = true;
        }
    }

    return childof_changed;
}

static
void update_component_monitors(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *added,
    ecs_entities_t *removed)
{
    bool childof_changed = update_component_monitor_w_array(
        &world->component_monitors, added);

    childof_changed |= update_component_monitor_w_array(
        &world->component_monitors, removed);

    /* If this entity is a parent, check if anything changed that could impact
     * its place in the hierarchy. If so, we need to mark all of the parent's
     * entities as dirty. */
    if (childof_changed && 
        ecs_map_get(world->child_tables, ecs_vector_t*, entity)) 
    {
        ecs_type_t type = ecs_get_type(world, entity);
        ecs_entities_t entities = ecs_type_to_entities(type);
        update_component_monitor_w_array(&world->parent_monitors, &entities);
    }
}

static
void commit(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_table_t *dst_table,   
    ecs_entities_t *added,
    ecs_entities_t *removed)
{
    ecs_table_t *src_table = info->table;
    bool in_progress = world->in_progress;

    if (src_table == dst_table && &world->stage == stage) {
        /* If source and destination table are the same, and stage is main stage
         * no action is required. When this is not the main stage, this can
         * happen when an entity is copied from the main stage table to the
         * same table in the stage, for example when ecs_set is invoked. */
        return;
    }

    if (src_table) {
        ecs_data_t *src_data = info->data;
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Only move entity when it is not moved to the root table, unless we're
         * iterating. In this case the entities need to be kept around so that
         * the merge knows to remove them from their previous tables. */
        if (dst_table->type || in_progress) {            
            info->row = move_entity(world, stage, entity, info, src_table, 
                src_data, info->row, dst_table, added, removed);
            info->table = dst_table;
        } else {
            delete_entity(
                world, stage, src_table, src_data, info->row, 
                removed);

            ecs_eis_set(stage, entity, &(ecs_record_t){
                NULL, -info->is_watched
            });
        }      
    } else {        
        if (dst_table->type) {
            info->row = new_entity(world, stage, entity, info, dst_table, added);
            info->table = dst_table;
        }        
    }

    /* If the entity is being watched, it is being monitored for changes and
    * requires rematching systems when components are added or removed. This
    * ensures that systems that rely on components from containers or prefabs
    * update the matched tables when the application adds or removes a 
    * component from, for example, a container. */
    if (info->is_watched) {
        update_component_monitors(world, entity, added, removed);
    }

    if ((!src_table || !src_table->type) && stage->range_check_enabled) {
        ecs_assert(!world->stats.max_id || entity <= world->stats.max_id, ECS_OUT_OF_RANGE, 0);
        ecs_assert(entity >= world->stats.min_id, ECS_OUT_OF_RANGE, 0);
    }
}

static
void* get_base_component(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entity_t previous,
    ecs_entity_t component)
{
    ecs_type_t type = info->table->type;
    ecs_entity_t *type_buffer = ecs_vector_first(type, ecs_entity_t);
    int32_t p = -1;
    void *ptr = NULL;

    while (!ptr && (p = find_prefab(type, p)) != -1) {
        ecs_entity_t prefab = type_buffer[p] & ECS_ENTITY_MASK;

        /* Detect cycles with two entities */
        if (prefab == previous) {
            continue;
        }

        ecs_entity_info_t prefab_info;
        if (get_info(world, prefab, &prefab_info)) {
            ptr = get_component(&prefab_info, component);
            if (!ptr) {
                ptr = get_base_component(
                    world, stage, prefab, &prefab_info, entity, component);
            }
        }
    }

    return ptr;
}

static
void new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *to_add)
{
    ecs_entity_info_t info = {0};
    ecs_table_t *table = ecs_table_traverse_add(
        world, stage, stage->scope_table, to_add, NULL);
    new_entity(world, stage, entity, &info, table, to_add);
}

static
int32_t new_w_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *component_ids,
    int32_t count,
    void **component_data)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_type_t type = table->type;
    ecs_entity_t e = world->stats.last_id + 1;
    world->stats.last_id += ecs_to_entity(count);

    if (!type) {
        return 0;
    }

    ecs_entities_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = ecs_vector_first(type, ecs_entity_t);
        component_array.count = ecs_vector_count(type);
    }

    ecs_data_t *data = ecs_table_get_or_create_data(world, stage, table);
    int32_t row = ecs_table_grow(world, table, data, count, e);
    ecs_entities_t added = ecs_type_to_entities(type);
    
    /* Update entity index. If entities are being created in the main stage,
     * set the record pointers in the table. */
    int i;
    if (stage == &world->stage) {
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
        for (i = 0; i < count; i ++) { 
            record_ptrs[row + i] = ecs_eis_set(stage, e + ecs_to_entity(i), 
            &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_eis_set(stage, e + ecs_to_entity(i), &(ecs_record_t){
                .table = table,
                .row = row + i + 1
            });
        }
    }
    
    ecs_defer_begin(world, stage, EcsOpNone, 0, 0, NULL, 0);

    ecs_comp_mask_t set_mask = { 0 };
    ecs_run_add_actions(world, stage, table, data, row, count, &added, set_mask, 
        true, component_data == NULL);

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            ecs_entity_t c = component_ids->array[c_i];
            if (c >= ECS_HI_COMPONENT_ID) {
                break;
            }
            
            comp_mask_set(set_mask, c);

            /* Copy component data */
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            /* Bulk copy column data into new table */
            int32_t table_index = ecs_type_index_of(type, c);
            ecs_column_t *column = &data->columns[table_index];
            int16_t size = column->size;
            int16_t alignment = column->alignment;
            void *ptr = ecs_vector_first_t(column->data, size, alignment);
            ptr = ECS_OFFSET(ptr, size * row);

            ecs_c_info_t *cdata = ecs_get_c_info(world, c);
            ecs_copy_t copy = cdata->lifecycle.copy;
            if (copy) {
                ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, c, entities, entities, ptr, src_ptr, 
                    ecs_to_size_t(size), count, cdata->lifecycle.ctx);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            }
        };

        ecs_run_set_systems(world, stage, &added, 
            table, data, row, count, true);        
    }

    ecs_run_monitors(world, stage, table, table->monitors, row, count, NULL);

    ecs_defer_end(world, stage);

    return row;
}

static
bool has_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    bool match_any,
    bool match_prefabs)    
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return false;
    }

    if (!type) {
        return true;
    }

    ecs_world_t *world_arg = world;
    ecs_type_t entity_type = ecs_get_type(world_arg, entity);

    return ecs_type_contains(
        world, entity_type, type, match_any, match_prefabs) != 0;
}

static
void add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t add_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entity_t remove_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = add_buffer };
    ecs_entities_t removed = { .array = remove_buffer };

    ecs_table_t *src_table = info.table;

    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, to_remove, &removed);

    dst_table = ecs_table_traverse_add(
        world, stage, dst_table, to_add, &added);    

    commit(world, stage, entity, &info, dst_table, &added, &removed);
}

static
void add_entities_w_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, stage, src_table, components, &added);

    commit(world, stage, entity, info, dst_table, &added, NULL);
}

static
void remove_entities_w_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info,
    ecs_entities_t *components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, components, &removed);

    commit(world, stage, entity, info, dst_table, NULL, &removed);
}

static
void add_entities(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    if (ecs_defer_begin(world, stage, EcsOpAdd, entity, components, NULL, 0)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t added = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_add(
        world, stage, src_table, components, &added);

    commit(world, stage, entity, &info, dst_table, &added, NULL);

    ecs_defer_end(world, stage);
}

static
void remove_entities(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);    
    ecs_stage_t *stage = ecs_get_stage(&world);

    if (ecs_defer_begin(world, stage, EcsOpRemove, entity, components, NULL, 0)) {
        return;
    }

    ecs_entity_info_t info;
    ecs_get_info(world, stage, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_entities_t removed = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = ecs_table_traverse_remove(
        world, stage, src_table, components, &removed);

    commit(world, stage, entity, &info, dst_table, NULL, &removed);

    ecs_defer_end(world, stage);
}

static
void *get_mutable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_entity_info_t *info,
    bool *is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((component & ECS_ENTITY_MASK) == component || component & ECS_TRAIT, ECS_INVALID_PARAMETER, NULL);

    void *dst = NULL;
    if (stage == &world->stage) {
        if (get_info(world, entity, info)) {
            dst = get_component(info, component);
        }
    } else {
        if (get_staged_info(world, stage, entity, info)) {
            dst = get_component(info, component);
        } else {
            /* If the entity isn't stored in the current stage, we still need to
             * get the data from the main stage to pass it to commit */
            get_info(world, entity, info);
        }
    }

    ecs_table_t *table = info->table;

    if (!dst) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        add_entities_w_info(world, stage, entity, info, &to_add);

        /* Reobtain info, as triggers could have changed the entity */
        ecs_get_info(world, stage, entity, info);

        dst = get_component(info, component);

        if (is_added) {
            *is_added = table != info->table;
        }

        return dst;
    } else {
        if (is_added) {
            *is_added = false;
        }

        return dst;
    }
}

/* -- Private functions -- */

int32_t ecs_record_to_row(
    int32_t row, 
    bool *is_watched_out) 
{
    bool is_watched = row < 0;
    row = row * -(is_watched * 2 - 1) - 1 * (row != 0);
    *is_watched_out = is_watched;
    return row;
}

int32_t ecs_row_to_record(
    int32_t row, 
    bool is_watched) 
{
    return (row + 1) * -(is_watched * 2 - 1);
}

ecs_entities_t ecs_type_to_entities(
    ecs_type_t type)
{
    return (ecs_entities_t){
        .array = ecs_vector_first(type, ecs_entity_t),
        .count = ecs_vector_count(type)
    };
}

void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{    
    (void)world;

    ecs_record_t *record = ecs_eis_get(stage, entity);
    if (!record) {
        ecs_record_t new_record = {.row = -1, .table = NULL};
        ecs_eis_set(stage, entity, &new_record);
    } else {
        if (record->row > 0) {
            record->row *= -1;

        } else if (record->row == 0) {
            /* If entity is empty, there is no index to change the sign of. In
             * this case, set the index to -1, and assign an empty type. */
            record->row = -1;
            record->table = NULL;
        }
    }
}

ecs_entity_t ecs_find_in_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t flags)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        ecs_entity_t c = *c_ptr;

        if (flags) {
            if ((c & flags) != flags) {
                continue;
            }
        }

        ecs_entity_t e = c & ECS_ENTITY_MASK;

        if (component) {
           ecs_type_t component_type = ecs_get_type(world, e);
           if (!ecs_type_has_entity(world, component_type, component)) {
               continue;
           }
        }

        return e;
    });

    return 0;
}


/* -- Public functions -- */

ecs_entity_t ecs_new_id(
    ecs_world_t *world)
{
    ecs_entity_t entity;

    if (!world->in_progress) {
        if (!(entity = ecs_eis_recycle(&world->stage))) {
            entity = ++ world->stats.last_id;
        }
    } else {
        int32_t thread_count = ecs_vector_count(world->workers);
        if (thread_count >= 1) { 
            /* Can't atomically increase number above max int */
            ecs_assert(
                world->stats.last_id < UINT_MAX, ECS_INTERNAL_ERROR, NULL);

            entity = (ecs_entity_t)ecs_os_ainc((int32_t*)&world->stats.last_id);
        } else {
            entity = ++ world->stats.last_id;
        } 
    }

    ecs_assert(!world->stats.max_id || entity <= world->stats.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    return entity;
}

ecs_entity_t ecs_new_component_id(
    ecs_world_t *world)
{
    /* Cannot issue new component ids while world is in progress */
    ecs_assert(!world->in_progress, ECS_INVALID_PARAMETER, NULL);

    if (world->stats.last_component_id < ECS_HI_COMPONENT_ID) {
        return world->stats.last_component_id ++;
    } else {
        /* If the low component ids are depleted, return a regular entity id */
        return ecs_new_id(world);
    }
}

ecs_entity_t ecs_new_w_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    if (type || stage->scope) {
        ecs_entities_t to_add = ecs_type_to_entities(type);
        new(world, stage, entity, &to_add);
    } else {
        ecs_eis_set(&world->stage, entity, &(ecs_record_t){ 0 });
    }

    return entity;
}

ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    if (component || stage->scope) {
        ecs_entities_t to_add = {
            .array = &component,
            .count = 1
        };

        ecs_entity_t old_scope = 0;
        if (component & ECS_CHILDOF) {
            old_scope = ecs_set_scope(world, 0);
        }

        new(world, stage, entity, &to_add);

        if (component & ECS_CHILDOF) {
            ecs_set_scope(world, old_scope);
        }
    } else {
        ecs_eis_set(&world->stage, entity, &(ecs_record_t){ 0 });
    }

    return entity;
}

ecs_entity_t ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    ecs_entities_t *component_ids,
    void *data)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_type_t type = ecs_type_find(world, 
        component_ids->array, component_ids->count);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, data);
    return result;
}

ecs_entity_t ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, NULL);
    return result;
}

ecs_entity_t ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entities_t type = {
        .array = &entity,
        .count = 1
    };
    ecs_table_t *table = ecs_table_find_or_create(world, stage, &type);
    ecs_entity_t result = world->stats.last_id + 1;
    new_w_data(world, stage, table, NULL, count, NULL);
    return result;
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;
    info.table = NULL;

    if (stage == &world->stage) {
        get_info(world, entity, &info);
    } else {
        if (!get_staged_info(world, stage, entity, &info)) {
            get_info(world, entity, &info);
        }
    }

    ecs_delete_children(world, entity);

    /* If entity has components, remove them */
    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_entities_t to_remove = ecs_type_to_entities(type);
        remove_entities_w_info(world, stage, entity, &info, &to_remove);
    }

    /* If this is a staged delete, set the table in the staged entity index to
     * NULL. That way the merge will know to delete this entity vs. just to
     * remove its components */
    if (stage != &world->stage) {
        if (!table) {
            /* If entity was empty, add it to the root table so its id will be
             * recycled when merging the stage */
            table = &stage->root;
            ecs_data_t *data = ecs_table_get_or_create_data(world, stage, table);
            ecs_table_append(world, table, data, entity, NULL);
        }
        ecs_eis_set(stage, entity, &(ecs_record_t){ NULL, 0 });
    } else {
        ecs_eis_delete(stage, entity);
    }
}

void ecs_add_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
    add_entities(world, entity, &components);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add)
{
    ecs_entities_t components = { .array = &to_add, .count = 1 };
    add_entities(world, entity, &components);
}

void ecs_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
    remove_entities(world, entity, &components);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_remove)
{
    ecs_entities_t components = { .array = &to_remove, .count = 1 };
    remove_entities(world, entity, &components);
}

void ecs_add_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t to_add,
    ecs_entity_t to_remove)
{
    ecs_entities_t components_add = { .array = &to_add, .count = 1 };      
    ecs_entities_t components_remove = { .array = &to_remove, .count = 1 };      
    add_remove(world, entity, &components_add, &components_remove);
}

void ecs_add_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_entities_t components_add = ecs_type_to_entities(to_add);
    ecs_entities_t components_remove = ecs_type_to_entities(to_remove);
    add_remove(world, entity, &components_add, &components_remove);
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(!world->is_merging, ECS_INVALID_WHILE_MERGING, NULL);
    
    if (!dst) {
        dst = ++ world->stats.last_id;
    }

    ecs_entity_info_t src_info;
    bool found = ecs_get_info(world, stage, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        return dst;
    }

    ecs_type_t src_type = src_table->type;
    ecs_entities_t to_add = ecs_type_to_entities(src_type);

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(
        world, stage, dst, &dst_info, src_table, &to_add);

    if (copy_value) {
        ecs_table_move(world, dst, src, src_table, dst_info.data, dst_info.row,
            src_table, src_info.data, src_info.row, true);

        int i;
        for (i = 0; i < to_add.count; i ++) {
            ecs_entity_t component = to_add.array[i];
            ecs_c_info_t *cdata = ecs_get_c_info(world, component);
            if (!cdata) {
                continue;
            }

            ecs_run_set_systems(world, stage, &to_add, 
                src_table, src_info.data, dst_info.row, 1, true);
        }
    }    

    return dst;
}

const void* ecs_get_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;
    void *ptr = NULL;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    bool found = ecs_get_info(world, stage, entity, &info);
    if (found) {
        if (!info.table) {
            return NULL;
        }

        ptr = get_component(&info, component);
        if (!ptr) {
            if (component != ecs_entity(EcsName) && component != EcsPrefab) {
                ptr = get_base_component(
                    world, stage, entity, &info, 0, component);
            }
        }        
    }

    return ptr;
}

const void* ecs_get_ref_w_entity(
    ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!entity || !ref->entity || entity == ref->entity, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!component || !ref->component || component == ref->component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_record_t *record = ref->record;

    entity |= ref->entity;

    if (stage != &world->stage) {
        ecs_record_t *staged = ecs_eis_get(stage, entity);
        if (staged) {
            record = staged;
        } else {
            stage = &world->stage;
        }
    }

    if (!record && stage == &world->stage) {
        record = ecs_eis_get(&world->stage, entity);
    }

    if (!record || !record->table) {
        return NULL;
    }

    ecs_table_t *table = record->table;

    if (ref->stage == stage &&
        ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->alloc_count == table->alloc_count)
    {
        return ref->ptr;
    }

    component |= ref->component;

    ref->entity = entity;
    ref->component = component;
    ref->stage = stage;
    ref->table = table;
    ref->row = record->row;
    ref->alloc_count = table->alloc_count;

    ecs_entity_info_t info = {0};
    set_info_from_record(world, &info, record);
    ref->ptr = get_component(&info, component);

    if (&world->stage == stage) {
        ref->record = record;
    } else {
        ref->record = NULL;
    }

    return ref->ptr;
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool *is_added)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info;
    return get_mutable(world, stage, entity, component, &info, is_added);
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_assert((component & ECS_ENTITY_MASK) == component, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_entity_info_t info = {0};

    if (ecs_get_info(world, stage, entity, &info)) {
        ecs_entities_t added = {
            .array = &component,
            .count = 1
        };
        ecs_run_set_systems(world, stage, &added, 
            info.table, info.data, info.row, 1, false);
    }
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component,
    size_t size,
    const void *ptr)
{    
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entities_t added = {
        .array = &component,
        .count = 1
    };

    if (!entity) {
        entity = ecs_new_id(world);
        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_entity(world, entity, ECS_CHILDOF | scope);
        }
    }

    if (ecs_defer_begin(world, stage, EcsOpSet, entity, &added, ptr, 
        ecs_from_size_t(size))) 
    {
        return entity;
    }

    ecs_entity_info_t info;
    void *dst = get_mutable(world, stage, entity, component, &info, NULL);

    /* This can no longer happen since we defer operations */
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ptr) {
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        ecs_copy_t copy;

        if (cdata && (copy = cdata->lifecycle.copy)) {
            copy(world, component, &entity, &entity, dst, ptr, size, 1, 
                cdata->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst, ptr, ecs_from_size_t(size));
        }
    } else {
        memset(dst, 0, size);
    }

    ecs_table_mark_dirty(info.table, component);

    ecs_run_set_systems(world, stage, &added, 
        info.table, info.data, info.row, 1, false);

    ecs_defer_end(world, stage);

    return entity;
}

bool ecs_has_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_type_t type = ecs_get_type(world, entity);
    return ecs_type_has_entity(world, type, component);
}

bool ecs_has_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    return has_type(world, entity, type, true, true);
}

ecs_entity_t ecs_get_parent_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    ecs_type_t type = ecs_get_type(world, entity);    
    ecs_entity_t parent = ecs_find_in_type(world, type, component, ECS_CHILDOF);
    return parent;
}

const char* ecs_get_name(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    if (entity == EcsSingleton) {
        return "$";
    }

    const EcsName *id = ecs_get(world, entity, EcsName);

    if (id) {
        return id->value;
    } else {
        return NULL;
    }
}

ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return NULL;
    }

    const EcsType *type = ecs_get(world, entity, EcsType);
    if (type) {
        return type->normalized;
    }

    return ecs_type_find(world, &entity, 1);
}

ecs_entity_t ecs_type_to_entity(
    ecs_world_t *world, 
    ecs_type_t type)
{
    (void)world;

    if (!type) {
        return 0;
    }
    
    /* If array contains n entities, it cannot be reduced to a single entity */
    if (ecs_vector_count(type) != 1) {
        ecs_abort(ECS_TYPE_NOT_AN_ENTITY, NULL);
    }

    return *(ecs_vector_first(type, ecs_entity_t));
}

ecs_type_t ecs_get_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_record_t *record = NULL;

    if (stage != &world->stage) {
        record = ecs_eis_get(stage, entity);
    }

    if (!record) {
        record = ecs_eis_get(&world->stage, entity);
    }

    ecs_table_t *table;
    if (record && (table = record->table)) {
        return table->type;
    }
    
    return NULL;
}

int32_t ecs_count_type(
    ecs_world_t *world,
    ecs_type_t type)
{    
    if (!type) {
        return 0;
    }

    return ecs_count_w_filter(world, &(ecs_filter_t){
        .include = type
    });
}

int32_t ecs_count_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{    
    if (!entity) {
        return 0;
    }

    ecs_type_t type = ecs_type_from_entity(world, entity);

    return ecs_count_w_filter(world, &(ecs_filter_t){
        .include = type
    });
}

int32_t ecs_count_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);
    int32_t result = 0;

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        if (!filter || ecs_table_match_filter(world, table, filter)) {
            result += ecs_table_count(table);
        }
    }
    
    return result;
}

/* Enter safe section. Record all operations so they can be executed after
 * leaving the safe section. */
bool ecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entities_t *components,
    const void *value,
    ecs_size_t size)
{
    (void)world;
    
    if (stage->defer && op_kind != EcsOpNone) {
        if (!components->count) {
            return true;
        }

        ecs_op_t *op = ecs_vector_add(&stage->defer_queue, ecs_op_t);
        op->kind = op_kind;
        op->entity = entity;
        if (components->count == 1) {
            op->component = components->array[0];
            op->components = (ecs_entities_t) {
                .array = NULL,
                .count = 1
            };
        } else {
            ecs_size_t array_size = components->count * ECS_SIZEOF(ecs_entity_t);
            op->components.array = ecs_os_malloc(array_size);
            ecs_os_memcpy(op->components.array, components->array, array_size);
            op->components.count = components->count;
        }

        if (size) {
            ecs_assert(value != NULL, ECS_INTERNAL_ERROR, NULL);
            op->value = ecs_os_memdup(value, size);
        } else {
            op->value = NULL;
        }

        op->size = size;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

/* Leave safe section. Run all deferred commands. */
void ecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    if (!--stage->defer) {
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;
        if (defer_queue) {
            ecs_vector_each(defer_queue, ecs_op_t, op, {
                if (op->components.count == 1) {
                    op->components.array = &op->component;
                }

                switch(op->kind) {
                case EcsOpNone:
                    break;
                case EcsOpAdd:
                    add_entities(world, op->entity, &op->components);
                    break;
                case EcsOpRemove:
                    remove_entities(world, op->entity, &op->components);
                    break;
                case EcsOpSet:
                    ecs_set_ptr_w_entity(world, op->entity, 
                        op->components.array[0], ecs_to_size_t(op->size), 
                        op->value);
                    break;
                }

                if (op->components.count > 1) {
                    ecs_os_free(op->components.array);
                }

                if (op->value) {
                    ecs_os_free(op->value);
                }
            });

            ecs_vector_free(defer_queue);
        }
    }
}



/* If all components were removed, don't store the entity in a table. If the
 * entity was deleted, also remove the entity from the entity index so that its
 * id can be recycled. */
static
void clear_columns(
    ecs_world_t *world,
    ecs_stage_t *stage, 
    ecs_data_t *data)
{
    int32_t i, entity_count = ecs_vector_count(data->entities);
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);

    for (i = 0; i < entity_count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_record_t *record = record_ptrs[i];
        
        if (!record && e > ECS_HI_ENTITY_ID) {
            record = ecs_eis_get(stage, e);
        }

        /* Remove component data from previous main stage table */
        if (record) {
            bool is_watched;
            int32_t row = ecs_record_to_row(record->row, &is_watched);

            ecs_table_t *src_table = record->table;
            if (record && record->table) {
                ecs_data_t *src_data = ecs_table_get_data(world, src_table);
                ecs_table_delete(world, stage, src_table, src_data, row);
            }

            /* If the staged record has the table set to the root, this is an entity
            * without components. If the table is NULL, this is a delete. */
            ecs_record_t *staged_record = ecs_eis_get(stage, e);
            if (staged_record->table) {
                /* Clear the entity record. This will set the table to NULL but
                 * if necessary, retain information about whether the entity is
                 * being watched or not. This is not the same as a delete, where
                 * the is_watched information is lost, and the entity id can be
                 * recycled. */
                ecs_eis_clear_entity(&world->stage, e, is_watched);
            } else {
                ecs_eis_delete(&world->stage, e);
            }
        } else {
            ecs_eis_delete(&world->stage, e);
        }
    }
}

static
void merge_columns(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *dst_data,
    ecs_data_t *src_data,
    int32_t dst_entity_count,
    int32_t src_entity_count)
{
    /* Copy data column by column from the stage to the main stage */
    int32_t c, column_count = table->column_count;
    ecs_entity_t *components = ecs_vector_first(table->type, ecs_entity_t);
    ecs_entity_t *dst_entities = ecs_vector_first(dst_data->entities, ecs_entity_t);
    ecs_entity_t *src_entities = ecs_vector_first(src_data->entities, ecs_entity_t);

    for (c = 0; c < column_count; c ++) {
        ecs_column_t *main_column = &dst_data->columns[c];
        if (!main_column->size) {
            continue;
        }
        
        ecs_column_t *column = &src_data->columns[c];
        int16_t size = column->size;
        int16_t alignment = column->alignment;
        ecs_assert(size == main_column->size, ECS_INTERNAL_ERROR, NULL);

        void *src = ecs_vector_first_t(column->data, size, alignment);
        ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

        void *dst = ecs_vector_first_t(main_column->data, size, alignment);
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        dst = ECS_OFFSET(dst, dst_entity_count * size);

        ecs_entity_t component = components[c];
        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        ecs_xtor_t ctor;
        ecs_move_t move;
        if (cdata && (move = cdata->lifecycle.move) && (ctor = cdata->lifecycle.ctor)) {
            void *ctx = cdata->lifecycle.ctx;
            ctor(world, component, dst_entities, dst, ecs_to_size_t(size), 
                src_entity_count, ctx);
            move(world, component, dst_entities, src_entities, dst, src, 
                ecs_to_size_t(size), src_entity_count, ctx);
        } else {
            ecs_os_memcpy(dst, src, size * src_entity_count);
        }

        ecs_vector_clear(column->data);
    } 
}

static
void merge_commits(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Loop the tables for which the stage has modified data */
    int32_t i, table_count = ecs_vector_count(stage->dirty_tables);
    ecs_table_t **tables = ecs_vector_first(stage->dirty_tables, ecs_table_t*);

    for (i = 0; i < table_count; i ++) {
        ecs_table_t *table = tables[i];
        ecs_data_t *main_data = ecs_table_get_data(world, table);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        int32_t e, entity_count = ecs_table_data_count(data);
        ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
        int32_t component_count = ecs_vector_count(table->type);
        
        ecs_assert(main_data != data, ECS_INTERNAL_ERROR, NULL);

        data->marked_dirty = false;

        /* If the table contains no data, this was a staged table that was
         * merged with the main stage. The main stage table will also have been
         * added to the dirty_table list, so we'll iterate it eventually */
        if (!data->entities) {
            continue;
        }

        /* If the table has no columns, this is the root table. Entities that
         * are in a root table need to be either deleted or emptied */
        if (!component_count) {
            clear_columns(world, stage, data);
            ecs_table_clear_data(table, data);
            continue;
        }

        /* If there are no entities to be merged, clear table data. This can
         * happen if a table was populated in a stage causing it to be marked as
         * dirty, and then the data got moved to another staged table. */
        if (!entity_count) {
            ecs_table_clear_data(table, data);
            continue;
        }

        /* Delete the entity from its previous main stage table, if it already 
         * existed. Doing this before copying the component data ensures that if 
         * an entity was staged for the same table, it can be simply appended to 
         * the end of the table without creating a duplicate record. */
        for (e = 0; e < entity_count; e ++) {
            ecs_record_t *record = record_ptrs[e];            
            ecs_entity_t entity = entities[e];

            /* If the entity did not yet exist in the main stage, register it */
            if (!record) {
                record = ecs_eis_get_or_create(&world->stage, entity);
                record_ptrs[e] = record;
            }

            bool is_watched;
            int32_t row = ecs_record_to_row(record->row, &is_watched);            

            ecs_table_t *src_table = record->table;
            if (src_table) {
                /* Delete entity from old table */
                ecs_data_t *src_data = ecs_table_get_data(world, src_table);
                ecs_table_destruct(world, src_table, src_data, row, 1);
                ecs_table_delete(world, stage, src_table, src_data, row);
            }
        }

        /* Ensure that the main table is large enough to store new entities */
        int32_t main_entity_count = ecs_table_count(table);
        ecs_table_set_count(world, table, main_data, 
            main_entity_count + entity_count);

        /* Copy entity ids */
        ecs_entity_t *main_entities = ecs_vector_first(main_data->entities, ecs_entity_t);
        ecs_os_memcpy(&main_entities[main_entity_count], entities, 
            entity_count * ECS_SIZEOF(ecs_entity_t));

        /* Copy record ptrs */
        ecs_record_t **main_record_ptrs = ecs_vector_first(main_data->record_ptrs, ecs_record_t*);
        ecs_os_memcpy(&main_record_ptrs[main_entity_count], record_ptrs, 
            entity_count * ECS_SIZEOF(ecs_record_t*));

        /* Copy component data */
        merge_columns(
            world, table, main_data, data, main_entity_count, 
            entity_count);

        /* Update entity index */
        for (e = 0; e < entity_count; e ++) {
            ecs_record_t *record = record_ptrs[e];

            /* We just retrieved or created the record, it should be there */
            ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

            bool is_watched;
            ecs_record_to_row(record->row, &is_watched);  

            record->table = table;
            record->row = ecs_row_to_record(main_entity_count + e, is_watched);
        }

        /* Clear staged table data */
        ecs_table_clear_data(table, data);

        /* If the table was empty, activate it explicitly since we bypassed the
         * regular append path */
        if (!main_entity_count) {
            ecs_table_activate(world, table, NULL, true);
        }
    }

    /* All dirty tables are processed, clear array for next frame. */
    ecs_vector_clear(stage->dirty_tables);
}

static
void clean_tables(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *t = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_table_free(world, t);
    }

    /* Clear the root table */
    if (count) {
        ecs_table_reset(world, &stage->root);
    }

    ecs_sparse_clear(stage->tables);
}

/* If a table was created while staged, it was not yet matched with OnSet
 * systems and monitors. Invoke them during the merge. When this applies to a
 * large amount of entities this can be expensive. To reduce this overhead, an
 * application can define the table beforehand by preallocating memory for it */
static
void merge_on_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_record_t **r_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    int32_t i, count = ecs_table_data_count(data);
    
    world->in_progress = true;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_record_t *r = r_ptrs[i];
        ecs_table_t *src_table = NULL;
        ecs_vector_t *src_mon = NULL;
        ecs_vector_t *src_set = NULL;

        /* Get record from main table so we can figure out which components were
         * added/set while the entity was staged */
        if (!r) {
            r = ecs_eis_get(&world->stage, e);
            if (r) {
                src_table = r->table;
            }
        } else {
            src_table = r->table;
        }

        /* Update record in stage to the table global table */
        ecs_record_t *staged_r = ecs_eis_get(stage, e);
        ecs_assert(staged_r != NULL, ECS_INTERNAL_ERROR, NULL);
        staged_r->table = table;

        if (src_table) {
            src_set = src_table->on_set_all;
            src_mon = src_table->monitors;

            /* We don't know if components were added or removed, so we have to 
             * run UnSet systems too */
            ecs_run_monitors(world, stage, table, src_table->un_set_all, 
                i, 1, table->un_set_all);  

            /* It is possible that removing a component caused a base component
             * to get exposed, which should trigger an OnSet system. */
            if (table->flags & EcsTableHasBase) {
                ecs_run_monitors(world, stage, table, 
                    src_table->on_set_override, i, 1, table->on_set_override);
            }                
        }

        /* Run OnSet systems */
        if (table->on_set_all) {
            ecs_run_monitors(world, stage, table, table->on_set_all, i, 1, src_set);
        }

        /* Run monitors */
        if (table->monitors) {
            ecs_run_monitors(world, stage, table, table->monitors, i, 1, src_mon);
        }

    }
    
    world->in_progress = false;
}

static
void merge_tables(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t start)
{
    /* Merge tables created in stage */
    int32_t i, count = ecs_sparse_count(stage->tables);

    if (start == count) {
        return;
    }

    for (i = start; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);

        /* Find or create the table in the main stage. Even though the
         * table may not have been created in the main stage when the stage
         * looked for it, other stages could have been merged before this
         * stage that already created the table.
         *
         * If this is the first time that the table is created in the main
         * stage, this will also trigger table notifications for queries. */

        ecs_table_t *main_table = ecs_table_from_type(
            world, &world->stage, table->type);
        ecs_assert(main_table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(main_table != table, ECS_INTERNAL_ERROR, NULL);

        /* Make sure the main stage table does not yet contain data for this
         * stage. That should never happen, as this stage is the only one
         * that could have populated the *stage-specific* table data, and we
         * wouldn't be here if the stage had found the table in the main
         * stage. */
#ifndef NDEBUG
        ecs_data_t *staged_data = ecs_table_get_staged_data(
            world, stage, main_table);

        ecs_assert(!staged_data || !staged_data->columns, 
            ECS_INTERNAL_ERROR, NULL);
#endif

        ecs_data_t *main_staged_data = ecs_table_get_or_create_data(
            world, stage, main_table);
        
        ecs_assert(main_staged_data != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Move the staged data from the staged table to the stage-specific
         * location in the main stage. This will ensure that the data will
         * be merged in the next step. Reset the data pointer to NULL in the
         * staged table so it won't be cleaned up */
        if (data) {
            *main_staged_data = *data;
        }

        /* If the main_table has been matched with OnSet systems, these
         * systems have not yet been invoked for the entities in this table
         * as systems aren't matched while in progress. Invoke them now. */
        if (main_table->on_set_all || main_table->un_set_all || 
            main_table->monitors || main_table->on_set_override) 
        {
            /* TODO: if an entity is moved by an OnSet handler to a new
             * table that is also created in the stage, it is possible that
             * an OnSet handler is executed twice. To prevent this from
             * happening, only invoke OnSet handlers on tables that were
             * created while iterating. This introduces an edge case where
             * an OnSet system that is applicable exclusively to the new
             * staged table is not invoked, but to address this case we need
             * to keep a shadow entity index while merging, as the only way
             * to determine the exclusive set of OnSet systems is by taking
             * the previous staged table and the new staged table. */
            if (start == 0) {
                merge_on_set(world, stage, main_table, main_staged_data);
            }
        }

        /* Add main stage table to dirty_tables. This will cause both the
         * staged table as well as the main stage table to be added to
         * the array. This is ok, as the staged table is now empty, so 
         * entities won't be added twice. */
        ecs_table_t **el = ecs_vector_add(&stage->dirty_tables, ecs_table_t*);
        *el = main_table;
    }

    /* It is possible that new tables were introduced to the stage while OnSet
     * handlers were executed. Merge those tables as well */
    merge_tables(world, stage, count);

    /* Reset table data */
    for (i = start; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        if (data) {
            *data = (ecs_data_t){ 0 };
        }
    }    
}

/* -- Private functions -- */

void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage->tables != world->stage.tables, ECS_INTERNAL_ERROR, NULL);

    /* First merge tables created by stage. This only happens if a new table was
     * created while iterating that did not yet exist in the main stage. Tables
     * are not modified in the main stage while iterating as this could cause
     * corruption of the administration while iterating, and in the case of
     * multithreaded applications wouldn't be safe. */
    merge_tables(world, stage, 0);

    /* Merge entities. This can create tables if a new combination of components
     * is found after merging the staged type with the non-staged type. */
    merge_commits(world, stage);

    /* Clear temporary tables used by stage */
    clean_tables(world, stage);
    ecs_eis_clear(stage);
}

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_main_stage = stage == &world->stage;
    bool is_temp_stage = stage == &world->temp_stage;

    memset(stage, 0, sizeof(ecs_stage_t));

    /* Initialize entity index */
    ecs_eis_new(stage);

    if (is_main_stage) {
        stage->id = 0;
    } else if (is_temp_stage) {
        stage->id = 1;
    }

    /* Initialize root table */
    stage->tables = ecs_sparse_new(ecs_table_t, 64);

    /* Initialize one root table per stage */
    ecs_init_root_table(world, stage);

    stage->scope_table = &world->stage.root;
    stage->scope = 0;
    stage->defer = 0;
    stage->defer_queue = NULL;
    stage->post_frame_actions = NULL;

    stage->range_check_enabled = true;
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    clean_tables(world, stage);
    ecs_sparse_free(stage->tables);
    ecs_table_free(world, &stage->root);
    ecs_vector_free(stage->dirty_tables);
    ecs_eis_free(stage);
}

/** Resize the vector buffer */
static
ecs_vector_t* resize(
    ecs_vector_t *vector,
    int16_t offset,
    int32_t size)
{
    ecs_vector_t *result = ecs_os_realloc(vector, offset + size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, 0);
    return result;
}

/* -- Public functions -- */

ecs_vector_t* _ecs_vector_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->count = 0;
    result->size = elem_count;
#ifndef NDEBUG
    result->elem_size = elem_size;
#endif
    return result;
}

ecs_vector_t* _ecs_vector_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_os_memcpy(ECS_OFFSET(result, offset), array, elem_size * elem_count);

    result->count = elem_count;
    result->size = elem_count;
#ifndef NDEBUG
    result->elem_size = elem_size;
#endif
    return result;   
}

void ecs_vector_free(
    ecs_vector_t *vector)
{
    ecs_os_free(vector);
}

void ecs_vector_clear(
    ecs_vector_t *vector)
{
    if (vector) {
        vector->count = 0;
    }
}

void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_vector_t *vector = *array_inout;
    if (!vector) {
        vector = _ecs_vector_new(elem_size, offset, 1);
        *array_inout = vector;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t max_count = vector->size;
    int32_t old_count = vector->count;
    int32_t new_count = old_count + elem_count;

    if ((new_count - 1) >= max_count) {
        if (!max_count) {
            max_count = elem_count;
        } else {
            while (max_count < new_count) {
                max_count *= 2;
            }
        }

        vector = resize(vector, offset, max_count * elem_size);
        vector->size = max_count;
        *array_inout = vector;
    }

    vector->count = new_count;

    return ECS_OFFSET(vector, offset + elem_size * old_count);
}

void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_vector_t *vector = *array_inout;

    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        int32_t count = vector->count;
        if (count < vector->size) {
            vector->count ++;
            return ECS_OFFSET(vector, offset + elem_size * count);
        }
    }

    return _ecs_vector_addn(array_inout, elem_size, offset, 1);
}

int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert((*dst)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    void *dst_elem = _ecs_vector_add(dst, elem_size, offset);
    void *src_elem = _ecs_vector_get(src, elem_size, offset, index);

    ecs_os_memcpy(dst_elem, src_elem, elem_size);
    return _ecs_vector_remove_index(src, elem_size, offset, index);
}

int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *elem)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    if (!elem) {
        return 0;
    }

    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);
    int32_t index = (ecs_size_t)((char*)elem - (char*)buffer) / elem_size;

    ecs_assert(index >= 0, ECS_INVALID_PARAMETER, NULL);

    if (index >= count) {
        return count;
    }

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * (count - 1));
        ecs_os_memcpy(elem, last_elem, elem_size);
    }

    count --;
    vector->count = count;

    return count;
}

void ecs_vector_remove_last(
    ecs_vector_t *vector)
{
    if (vector->count) vector->count --;
}

bool _ecs_vector_pop(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *value)
{
    if (!vector) {
        return false;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t count = vector->count;
    if (!count) {
        return false;
    }

    void *elem = ECS_OFFSET(vector, offset + (count - 1) * elem_size);

    if (value) {
        ecs_os_memcpy(value, elem, elem_size);
    }

    ecs_vector_remove_last(vector);

    return true;
}

int32_t _ecs_vector_remove_index(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);
    void *elem = ECS_OFFSET(buffer, index * elem_size);

    ecs_assert(index < count, ECS_INVALID_PARAMETER, NULL);

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * (count - 1));
        ecs_os_memcpy(elem, last_elem, elem_size);
    }

    count --;
    vector->count = count;

    return count;
}

void _ecs_vector_reclaim(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_vector_t *vector = *array_inout;

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t size = vector->size;
    int32_t count = vector->count;

    if (count < size) {
        size = count;
        vector = resize(vector, offset, size * elem_size);
        vector->size = size;
        *array_inout = vector;
    }
}

int32_t ecs_vector_count(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->count;
}

int32_t ecs_vector_size(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->size;
}

int32_t _ecs_vector_set_size(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_vector_t *vector = *array_inout;

    if (!vector) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
        return elem_count;
    } else {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        int32_t result = vector->size;

        if (elem_count < vector->count) {
            elem_count = vector->count;
        }

        if (result < elem_count) {
            vector = resize(vector, offset, elem_count * elem_size);
            vector->size = elem_count;
            *array_inout = vector;
            result = elem_count;
        }

        return result;
    }
}

int32_t _ecs_vector_grow(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    int32_t current = ecs_vector_count(*array_inout);
    return _ecs_vector_set_size(array_inout, elem_size, offset, current + elem_count);
}

int32_t _ecs_vector_set_count(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*array_inout) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
    }

    ecs_assert((*array_inout)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    (*array_inout)->count = elem_count;
    ecs_size_t size = _ecs_vector_set_size(array_inout, elem_size, offset, elem_count);
    return size;
}

int32_t _ecs_vector_set_min_size(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*vector_inout || (*vector_inout)->size < elem_count) {
        return _ecs_vector_set_size(vector_inout, elem_size, offset, elem_count);
    } else {
        return (*vector_inout)->size;
    }
}

int32_t _ecs_vector_set_min_count(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    _ecs_vector_set_min_size(vector_inout, elem_size, offset, elem_count);

    ecs_vector_t *v = *vector_inout;
    if (v && v->count < elem_count) {
        v->count = elem_count;
    }

    return v->count;
}

void* _ecs_vector_first(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    (void)elem_size;

    ecs_assert(!vector || vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    if (vector && vector->size) {
        return ECS_OFFSET(vector, offset);
    } else {
        return NULL;
    }
}

void* _ecs_vector_get(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    if (!vector) {
        return NULL;
    }
    
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    int32_t count = vector->count;

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(vector, offset + elem_size * index);
}

void* _ecs_vector_last(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        int32_t count = vector->count;
        if (!count) {
            return NULL;
        } else {
            return ECS_OFFSET(vector, offset + elem_size * (count - 1));
        }
    } else {
        return NULL;
    }
}

void _ecs_vector_sort(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    ecs_comparator_t compare_action)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    

    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);

    if (count > 1) {
        qsort(buffer, (size_t)count, (size_t)elem_size, compare_action);
    }
}

void _ecs_vector_memory(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t *allocd,
    int32_t *used)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    if (allocd) {
        *allocd += vector->size * elem_size + offset;
    }
    if (used) {
        *used += vector->count * elem_size;
    }
}

ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (!src) {
        return NULL;
    }

    ecs_vector_t *dst = _ecs_vector_new(elem_size, offset, src->size);
    ecs_os_memcpy(dst, src, offset + elem_size * src->count);
    return dst;
}

#define CHUNK_ALLOC_SIZE (65536)

typedef struct chunk_t {
    void *data;
    int32_t count;
} chunk_t;

typedef struct sparse_elem_t {
    int32_t dense;
    void *ptr;
} sparse_elem_t;

struct ecs_sparse_t {
    ecs_vector_t *chunks;       /* Vector with chunk pointers */
    ecs_vector_t *dense;        /* Dense array */
    ecs_vector_t *sparse;       /* Sparse array + element pointers */
    ecs_vector_t *unused_chunks;   /* Unused chunks */
    ecs_vector_t *unused_elements; /* Unused elements */
    ecs_size_t elem_size;          /* Side of payload */
    int32_t chunk_size;            /* Number of elements in chunk */
};

static
void add_chunk(
    ecs_sparse_t *sparse)
{
    /* Add chunk to sparse instance */
    int32_t chunk_count = ecs_vector_count(sparse->chunks);
    
    chunk_t recycled_chunk, *chunk = ecs_vector_add(&sparse->chunks, chunk_t);

    /* Check if we have chunks we can recycle */
    if (ecs_vector_pop(sparse->unused_chunks, chunk_t, &recycled_chunk)) {
        *chunk = recycled_chunk;
    } else {
        chunk->data = ecs_os_malloc(CHUNK_ALLOC_SIZE);
    }

    int32_t chunk_size = sparse->chunk_size;
    ecs_size_t elem_size = sparse->elem_size;

    /* Allocate data vector for chunk */
    chunk->count = 0;

    /* Create room in sparse array for chunk */
    int32_t prev_total = chunk_count * chunk_size;

    ecs_vector_set_count(&sparse->sparse, sparse_elem_t, prev_total + chunk_size);

    ecs_assert(sparse->sparse != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Prepare cached pointers to chunk elements in sparse array */
    sparse_elem_t *sparse_array = ecs_vector_first(sparse->sparse, sparse_elem_t);
    ecs_assert(sparse_array != NULL, ECS_INTERNAL_ERROR, NULL);

    sparse_array = &sparse_array[prev_total];

    int32_t i;
    for (i = 0; i < chunk_size; i ++) {
        sparse_array[i].dense = 0;
        sparse_array[i].ptr = ECS_OFFSET(chunk->data, i * elem_size);
    }
}

static
chunk_t* last_chunk(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_last(sparse->chunks, chunk_t);
}

static
void* add_sparse(
    ecs_sparse_t *sparse,
    int32_t index)
{
    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse, sparse_elem_t);
    
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    sparse_arr[index].dense = ecs_vector_count(sparse->dense);
    int32_t *dense = ecs_vector_add(&sparse->dense, int32_t);
    *dense = index;

    return sparse_arr[index].ptr;
}

static
void* get_sparse(
    const ecs_sparse_t *sparse,
    int32_t index,
    bool remove)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        return NULL;
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse, sparse_elem_t);
    int32_t dense = sparse_arr[index].dense;

    int32_t *dense_array = ecs_vector_first(sparse->dense, int32_t);
    int32_t dense_count = ecs_vector_count(sparse->dense);

    if (dense >= dense_count) {
        return NULL;
    }

    if (dense_array[dense] != index) {
        return NULL;
    }

    if (remove) {
        int32_t last_sparse_index = dense_array[dense_count - 1];
        dense_array[dense] = last_sparse_index;
        sparse_arr[last_sparse_index].dense = dense;
        ecs_vector_remove_last(sparse->dense);
    }

    return sparse_arr[index].ptr;  
}

static
void* get_or_set_sparse(
    ecs_sparse_t *sparse,
    int32_t index,
    bool *is_new)
{
    if (index >= ecs_vector_count(sparse->sparse)) {
        ecs_sparse_set_size(sparse, index + 1);
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INTERNAL_ERROR, NULL);
    }

    sparse_elem_t *sparse_arr = ecs_vector_first(sparse->sparse, sparse_elem_t);
    int32_t dense = sparse_arr[index].dense;

    int32_t *dense_array = ecs_vector_first(sparse->dense, int32_t);
    int32_t dense_count = ecs_vector_count(sparse->dense);

    if (dense >= dense_count || dense_array[dense] != index) {
        ecs_assert(index < ecs_vector_count(sparse->sparse), ECS_INVALID_PARAMETER, NULL);

        ecs_vector_add(&sparse->dense, int32_t);

        dense_array = ecs_vector_first(sparse->dense, int32_t);
        sparse_arr[index].dense = dense_count;
        dense_array[dense_count] = index;

        if (is_new) {
            *is_new = true;
        }
    }

    return sparse_arr[index].ptr;  
}

ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size,
    int32_t element_count)
{
    ecs_sparse_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_sparse_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->chunk_size = CHUNK_ALLOC_SIZE / elem_size;
    result->elem_size = elem_size;

    ecs_sparse_set_size(result, element_count);

    return result;
}

static
void free_chunks(ecs_vector_t *chunks) {
    int i, count = ecs_vector_count(chunks);
    chunk_t *array = ecs_vector_first(chunks, chunk_t);

    for (i = 0; i < count; i ++) {
        ecs_os_free(array[i].data);
    }    

    ecs_vector_free(chunks);
}

void ecs_sparse_free(
    ecs_sparse_t *sparse)
{
    if (sparse) {
        free_chunks(sparse->chunks);
        free_chunks(sparse->unused_chunks);

        ecs_vector_free(sparse->unused_elements);
        ecs_vector_free(sparse->sparse);
        ecs_vector_free(sparse->dense);

        ecs_os_free(sparse);
    }
}

void ecs_sparse_clear(
    ecs_sparse_t *sparse)
{        
    /* Recycle chunks */
    if (!sparse->unused_chunks) {
        sparse->unused_chunks = sparse->chunks;
        sparse->chunks = NULL;
    } else {
        chunk_t chunk;
        while (ecs_vector_pop(sparse->chunks, chunk_t, &chunk)) {
            chunk_t *unused = ecs_vector_add(&sparse->unused_chunks, chunk_t);
            *unused = chunk;
        }
    }

    ecs_vector_clear(sparse->dense);
    ecs_vector_clear(sparse->sparse);
    ecs_vector_clear(sparse->unused_elements);
}

void* _ecs_sparse_recycle(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t *sparse_index_out)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    int32_t index = 0;
    
    if (ecs_vector_pop(sparse->unused_elements, int32_t, &index)) {
        if (sparse_index_out) {
            *sparse_index_out = index;
        }
        return add_sparse(sparse, index);
    } else {
        return NULL;
    }
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    void *result = _ecs_sparse_recycle(sparse, elem_size, NULL);
    if (!result) {
        int32_t index = 0;
        chunk_t *chunk = last_chunk(sparse);

        int32_t elements_per_chunk = sparse->chunk_size;
        if (!chunk || chunk->count == elements_per_chunk) {
            add_chunk(sparse);
            chunk = last_chunk(sparse);
        }

        ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(chunk->count < elements_per_chunk, ECS_INTERNAL_ERROR, NULL);

        int32_t chunk_count = ecs_vector_count(sparse->chunks);
        ecs_assert(chunk_count > 0, ECS_INTERNAL_ERROR, NULL);

        index = (chunk_count - 1) * elements_per_chunk + chunk->count;
        chunk->count ++;

        result = add_sparse(sparse, index);
    }

    return result;
}

void* _ecs_sparse_remove(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{   
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    void *result = get_sparse(sparse, index, true);
    if (result) {
        /* Only add to unused elements if index was set */
        int32_t *free_elem = ecs_vector_add(&sparse->unused_elements, int32_t);
        *free_elem = index;
    }

    return result;
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{
    (void)elem_size;
    ecs_assert(index < ecs_vector_count(sparse->dense), 
        ECS_INVALID_PARAMETER, NULL);
    
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    const int32_t *it = ecs_vector_first(sparse->dense, int32_t);

    void *result = get_sparse(sparse, it[index], false);

    ecs_assert(result != NULL, ECS_INVALID_PARAMETER, NULL);

    return result;
}

void* _ecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_sparse(sparse, index, false);
}

void* _ecs_sparse_get_or_set_sparse(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index,
    bool *is_new)
{
    (void)elem_size;
    ecs_assert(!elem_size || elem_size == sparse->elem_size, 
        ECS_INVALID_PARAMETER, NULL);

    return get_or_set_sparse(sparse, index, is_new);
}

int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return ecs_vector_count(sparse->dense);
}

int32_t ecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vector_count(sparse->sparse);
}

const int32_t* ecs_sparse_indices(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_first(sparse->dense, int32_t);
}

const int32_t* ecs_sparse_unused_indices(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_first(sparse->unused_elements, int32_t);
}

int32_t ecs_sparse_unused_count(
    const ecs_sparse_t *sparse)
{
    return ecs_vector_count(sparse->unused_elements);
}

ecs_sparse_t* ecs_sparse_copy(
    const ecs_sparse_t *src)
{
    if (!src) {
        return NULL;
    }
    
    ecs_sparse_t *dst = ecs_os_memdup(src, ECS_SIZEOF(ecs_sparse_t));
    dst->chunks = ecs_vector_copy(src->chunks, chunk_t);
    dst->dense = ecs_vector_copy(src->dense, int32_t);
    dst->sparse = ecs_vector_copy(src->sparse, sparse_elem_t);
    dst->unused_elements = ecs_vector_copy(src->unused_elements, int32_t);

    /* Iterate chunks, copy data */
    sparse_elem_t *sparse_array = ecs_vector_first(dst->sparse, sparse_elem_t);
    chunk_t *chunks = ecs_vector_first(dst->chunks, chunk_t);
    int32_t i, count = ecs_vector_count(dst->chunks);

    for (i = 0; i < count; i ++) {
        chunks[i].data = ecs_os_memdup(
            chunks[i].data, dst->chunk_size * dst->elem_size);
        
        int32_t j;
        for (j = 0; j < dst->chunk_size; j ++) {
            sparse_array[i * dst->chunk_size + j].ptr = 
                ECS_OFFSET(chunks[i].data, j * dst->elem_size);
        }
    }

    return dst;
}

void ecs_sparse_restore(
    ecs_sparse_t *dst,
    ecs_sparse_t *src)
{
    ecs_assert(dst->elem_size == src->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dst->chunk_size == src->chunk_size, ECS_INVALID_PARAMETER, NULL);

    /* Copy chunk data */
    chunk_t *src_chunks = ecs_vector_first(src->chunks, chunk_t);
    chunk_t *dst_chunks = ecs_vector_first(dst->chunks, chunk_t);
    int32_t i, count = ecs_vector_count(src->chunks);

    for (i = 0; i < count; i ++) {
        ecs_os_memcpy(dst_chunks[i].data, src_chunks[i].data,
            dst->chunk_size * dst->elem_size);
    }

    /* Clean up remaining chunks */
    int32_t dst_count = ecs_vector_count(dst->chunks);
    for (i = count; i < dst_count; i ++) {
        ecs_os_free(dst_chunks[i].data);
    }

    ecs_vector_set_count(&dst->chunks, chunk_t, count);

    /* Copy dense array */
    int32_t elem_count = ecs_vector_count(src->dense);
    ecs_vector_set_count(&dst->dense, int32_t, elem_count);

    int32_t *dst_dense = ecs_vector_first(dst->dense, int32_t);
    int32_t *src_dense = ecs_vector_first(src->dense, int32_t);
    ecs_os_memcpy(dst_dense, src_dense, elem_count * ECS_SIZEOF(int32_t));

    /* Copy sparse array */
    int32_t sparse_count = ecs_vector_count(src->sparse);
    ecs_vector_set_count(&dst->sparse, sparse_elem_t, sparse_count);

    sparse_elem_t *dst_sparse = ecs_vector_first(dst->sparse, sparse_elem_t);
    sparse_elem_t *src_sparse = ecs_vector_first(src->sparse, sparse_elem_t);
    
    for (i = 0; i < sparse_count; i ++) {
        dst_sparse[i].dense = src_sparse[i].dense;
    }

    /* Copy unused elements */
    int32_t unused_count = ecs_vector_count(src->unused_elements);
    ecs_vector_set_count(&dst->unused_elements, int32_t, unused_count);

    if (unused_count) {
        int32_t *dst_unused = ecs_vector_first(dst->unused_elements, int32_t);
        int32_t *src_unused = ecs_vector_first(src->unused_elements, int32_t);
        ecs_os_memcpy(dst_unused, src_unused, unused_count * ECS_SIZEOF(int32_t));
    }
}

void ecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used)
{
    if (!sparse) {
        return;
    }

    ecs_vector_memory(sparse->chunks, chunk_t, allocd, used);
    ecs_vector_memory(sparse->dense, int32_t, allocd, used);
    ecs_vector_memory(sparse->sparse, sparse_elem_t, allocd, used);
    ecs_vector_memory(sparse->unused_elements, int32_t, allocd, used);

    int32_t data_total = sparse->chunk_size * 
        sparse->elem_size * ecs_vector_count(sparse->chunks);

    int32_t data_not_used = ecs_vector_count(sparse->unused_elements) * 
        sparse->elem_size;

    if (allocd) {
        *allocd += data_total;
    }

    if (used) {
        *used += data_total - data_not_used;
    }
}

void ecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{   
    int32_t current = ecs_sparse_size(sparse);
    int32_t to_add = elem_count - current;

    if (to_add > 0) {
        ecs_vector_set_size(&sparse->sparse, sparse_elem_t, elem_count);
        int32_t cur = ecs_vector_count(sparse->chunks) * sparse->chunk_size;
        
        while (cur < elem_count) {
            add_chunk(sparse);
            cur += sparse->chunk_size;
        }
    }

    if (ecs_vector_size(sparse->dense) < elem_count) {
        ecs_vector_set_size(&sparse->dense, int32_t, elem_count);
    }
}

void ecs_sparse_grow(
    ecs_sparse_t *sparse,
    int32_t count)
{
    int32_t current = ecs_sparse_count(sparse);

    while (ecs_sparse_size(sparse) <= (count + current)) {
        add_chunk(sparse);
    }
}

#ifdef FLECS_READER_WRITER


static
void ecs_name_writer_alloc(
    ecs_name_writer_t *writer,
    int32_t len)
{
    writer->len = len;
    if (writer->len > writer->max_len) {
        ecs_os_free(writer->name);
        writer->name = ecs_os_malloc(writer->len);
        writer->max_len = writer->len;
    }
    writer->written = 0;
}

static
bool ecs_name_writer_write(
    ecs_name_writer_t *writer,
    const char *buffer)
{
    ecs_size_t written = writer->len - writer->written;
    char *name_ptr = ECS_OFFSET(writer->name, writer->written);

    ecs_assert(name_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(buffer != NULL, ECS_INTERNAL_ERROR, NULL);

    if (written >= ECS_SIZEOF(int32_t)) {
        *(int32_t*)name_ptr = *(int32_t*)buffer;
        writer->written += ECS_SIZEOF(int32_t);
        return writer->written != writer->len;
    } else {
        ecs_os_memcpy(name_ptr, buffer, written);
        writer->written += written;
        return false;
    }
}

static
void ecs_name_writer_reset(
    ecs_name_writer_t *writer)
{
    writer->name = NULL;
    writer->max_len = 0;
    writer->len = 0;
}

static
void ecs_table_writer_register_table(
    ecs_writer_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_table_writer_t *writer = &stream->table;
    ecs_type_t type = ecs_type_find(world, writer->type_array, writer->type_count);

    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    writer->table = ecs_table_from_type(world, &world->stage, type);
    ecs_assert(writer->table != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, writer->table);
    if (data->entities) {
        /* Remove any existing entities from entity index */
        ecs_vector_each(data->entities, ecs_entity_t, e_ptr, {
            ecs_eis_delete(&world->stage, *e_ptr);
        });
      
        return;
    } else {
        /* Set size of table to 0. This will initialize columns */
        ecs_table_set_size(world, writer->table, data, 0);
    }

    ecs_assert(writer->table != NULL, ECS_INTERNAL_ERROR, NULL);
}

static
void ecs_table_writer_finalize_table(
    ecs_writer_t *stream)
{
    ecs_world_t *world = stream->world;
    ecs_table_writer_t *writer = &stream->table;

    /* Register entities in table in entity index */
    ecs_data_t *data = ecs_table_get_data(world, writer->table);
    ecs_vector_t *entity_vector = data->entities;
    ecs_entity_t *entities = ecs_vector_first(entity_vector, ecs_entity_t);
    int32_t i, count = ecs_vector_count(entity_vector);

    for (i = 0; i < count; i ++) {
        ecs_record_t *record_ptr = ecs_eis_get(&world->stage, entities[i]);

        if (record_ptr) {
            if (record_ptr->table != writer->table) {
                ecs_table_t *table = record_ptr->table;                
                ecs_data_t *table_data = ecs_table_get_data(world, table);

                ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

                ecs_table_delete(world, &world->stage, 
                    table, table_data, record_ptr->row - 1);
            }
        }

        ecs_record_t record = (ecs_record_t){
            .row = i + 1,
            .table = writer->table
        };

        ecs_eis_set(&world->stage, entities[i], &record);

        if (entities[i] >= world->stats.last_id) {
            world->stats.last_id = entities[i] + 1;
        }
    }
}

static
void ecs_table_writer_prepare_column(
    ecs_writer_t *stream,
    int32_t size)
{
    ecs_table_writer_t *writer = &stream->table;
    ecs_world_t *world = stream->world;
    ecs_data_t *data = ecs_table_get_or_create_data(
        world, &world->stage, writer->table);
        
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (writer->column_index) {
        ecs_column_t *column = &data->columns[writer->column_index - 1];

        if (size) {
            int32_t old_count = ecs_vector_count(column->data);
            _ecs_vector_set_count(&column->data, ECS_VECTOR_U(size, 0), writer->row_count);

            /* Initialize new elements to 0 */
            void *buffer = ecs_vector_first_t(column->data, size, 0);
            ecs_os_memset(ECS_OFFSET(buffer, old_count * size), 0, 
                (writer->row_count - old_count) * size);
        }

        writer->column_vector = column->data;
        writer->column_size = ecs_to_i16(size);
    } else {
        ecs_vector_set_count(
            &data->entities, ecs_entity_t, writer->row_count);

        writer->column_vector = data->entities;
        writer->column_size = ECS_SIZEOF(ecs_entity_t);      
    }

    writer->column_data = ecs_vector_first_t(writer->column_vector,
        writer->column_size, 
        writer->column_alignment);
        
    writer->column_written = 0;
}

static
void ecs_table_writer_next(
    ecs_writer_t *stream)
{
    ecs_table_writer_t *writer = &stream->table;

    switch(writer->state) {
    case EcsTableTypeSize:
        writer->state = EcsTableType;
        break;

    case EcsTableType:
        writer->state = EcsTableSize;
        break;

    case EcsTableSize:
        writer->state = EcsTableColumn;
        break;

    case EcsTableColumnHeader:
        writer->state = EcsTableColumnSize;
        break;

    case EcsTableColumnSize:
        writer->state = EcsTableColumnData;
        break;

    case EcsTableColumnNameHeader:
        writer->state = EcsTableColumnNameLength;
        break;

    case EcsTableColumnNameLength:
        writer->state = EcsTableColumnName;
        break;

    case EcsTableColumnName:
        writer->row_index ++;
        if (writer->row_index < writer->row_count) {
            writer->state = EcsTableColumnNameLength;
            break;
        }
        // fall through

    case EcsTableColumnData:
        writer->column_index ++;

        if (writer->column_index > writer->table->column_count) {
            ecs_table_writer_finalize_table(stream);
            stream->state = EcsStreamHeader;
            writer->column_written = 0;
            writer->state = 0;
            writer->column_index = 0;
            writer->row_index = 0;
        } else {
            writer->state = EcsTableColumn;
        }
        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }
}

static
ecs_size_t ecs_table_writer(
    const char *buffer,
    ecs_size_t size,
    ecs_writer_t *stream)
{
    ecs_table_writer_t *writer = &stream->table;
    ecs_size_t written = 0;

    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size >= ECS_SIZEOF(int32_t), ECS_INVALID_PARAMETER, NULL);

    if (!writer->state) {
        writer->state = EcsTableTypeSize;
    }

    switch(writer->state) {
    case EcsTableTypeSize:
        writer->type_count = *(int32_t*)buffer;
        if (writer->type_count > writer->type_max_count) {
            ecs_os_free(writer->type_array);
            writer->type_array = ecs_os_malloc(writer->type_count * ECS_SIZEOF(ecs_entity_t));
            writer->type_max_count = writer->type_count;
        }
        writer->type_written = 0;
        written = ECS_SIZEOF(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableType:
        *(int32_t*)ECS_OFFSET(writer->type_array, writer->type_written) = *(int32_t*)buffer;
        written = ECS_SIZEOF(int32_t);
        writer->type_written += written;

        if (writer->type_written == writer->type_count * ECS_SIZEOF(ecs_entity_t)) {
            ecs_table_writer_register_table(stream);
            ecs_table_writer_next(stream);
        }
        break;

    case EcsTableSize:
        writer->row_count = *(int32_t*)buffer;
        written += ECS_SIZEOF(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumn:
        writer->state = *(ecs_blob_header_kind_t*)buffer;
        if (writer->state != EcsTableColumnHeader &&
            writer->state != EcsTableColumnNameHeader)
        {
            stream->error = ECS_DESERIALIZE_FORMAT_ERROR;
            goto error;
        }
        written += ECS_SIZEOF(int32_t);
        break;

    case EcsTableColumnHeader:
    case EcsTableColumnSize:
        ecs_table_writer_prepare_column(stream, *(int32_t*)buffer);
        ecs_table_writer_next(stream);

        written += ECS_SIZEOF(int32_t);
        ecs_table_writer_next(stream);

        /* If column has no size, there will be no column data, so skip to the
         * next state. */
        if (!writer->column_size) {
            ecs_table_writer_next(stream);
        }
        break;

    case EcsTableColumnData: {
        written = writer->row_count * writer->column_size - writer->column_written;
        if (written > size) {
            written = size;
        }

        ecs_os_memcpy(ECS_OFFSET(writer->column_data, writer->column_written), buffer, written);
        writer->column_written += written;
        written = (((written - 1) / ECS_SIZEOF(int32_t)) + 1) * ECS_SIZEOF(int32_t);

        if (writer->column_written == writer->row_count * writer->column_size) {
            ecs_table_writer_next(stream);
        }
        break;
    }

    case EcsTableColumnNameHeader:
        ecs_table_writer_prepare_column(stream, ECS_SIZEOF(EcsName));
        ecs_table_writer_next(stream);
        // fall through

    case EcsTableColumnNameLength:
        ecs_name_writer_alloc(&writer->name, *(int32_t*)buffer);
        written = ECS_SIZEOF(int32_t);
        ecs_table_writer_next(stream);
        break;

    case EcsTableColumnName: {
        written = ECS_SIZEOF(int32_t);
        if (!ecs_name_writer_write(&writer->name, buffer)) {
            EcsName *name_ptr = &((EcsName*)writer->column_data)[writer->row_index];
            name_ptr->value = writer->name.name;

            if (name_ptr->alloc_value) {
                ecs_os_free(name_ptr->alloc_value);
            }

            name_ptr->alloc_value = writer->name.name;

            /* Don't overwrite entity name */
            ecs_name_writer_reset(&writer->name);   

            ecs_table_writer_next(stream);
        }
        break;
    }

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
        break;
    }

    ecs_assert(written <= size, ECS_INTERNAL_ERROR, NULL);

    return written;
error:
    return -1;
}

int ecs_writer_write(
    const char *buffer,
    ecs_size_t size,
    ecs_writer_t *writer)
{
    ecs_size_t written = 0, total_written = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= ECS_SIZEOF(int32_t), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(size % 4 == 0, ECS_INVALID_PARAMETER, NULL);

    while (total_written < size) {
        if (writer->state == EcsStreamHeader) {
            writer->state = *(ecs_blob_header_kind_t*)ECS_OFFSET(buffer, 
                total_written);

            if (writer->state != EcsTableHeader) {
                writer->error = ECS_DESERIALIZE_FORMAT_ERROR;
                goto error;
            }

            written = ECS_SIZEOF(ecs_blob_header_kind_t);
        } else
        if (writer->state == EcsTableHeader) {
            written = ecs_table_writer(ECS_OFFSET(buffer, total_written), 
                remaining, writer);
        }

        if (!written) {
            break;
        }

        if (written == (ecs_size_t)-1) {
            goto error;
        }

        remaining -= written;
        total_written += written;    
    }

    ecs_assert(total_written <= size, ECS_INTERNAL_ERROR, NULL);

    return total_written == 0;
error:
    return -1;
}

ecs_writer_t ecs_writer_init(
    ecs_world_t *world)
{
    return (ecs_writer_t){
        .world = world,
        .state = EcsStreamHeader,
    };
}

#endif

#ifdef FLECS_MODULE


char* ecs_module_path_from_c(
    const char *c_name)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    const char *ptr;
    char ch;

    for (ptr = c_name; (ch = *ptr); ptr++) {
        if (isupper(ch)) {
            ch = ecs_to_i8(tolower(ch));
            if (ptr != c_name) {
                ecs_strbuf_appendstrn(&str, ".", 1);
            }
        }

        ecs_strbuf_appendstrn(&str, &ch, 1);
    }

    return ecs_strbuf_get(&str);
}

ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t init_action,
    const char *module_name,
    void *handles_out,
    size_t handles_size)
{
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t old_scope = ecs_set_scope(world, 0);
    const char *old_name_prefix = world->name_prefix;

    ecs_entity_t e = ecs_lookup_fullpath(world, module_name);
    if (!e) {
        ecs_trace_1("import %s", module_name);
        ecs_log_push();

        /* Load module */
        init_action(world);

        /* Lookup module entity (must be registered by module) */
        e = ecs_lookup_fullpath(world, module_name);
        ecs_assert(e != 0, ECS_MODULE_UNDEFINED, module_name);

        ecs_log_pop();
    }

    /* Copy value of module component in handles_out parameter */
    if (handles_size && handles_out) {
        void *handles_ptr = ecs_get_mut_w_entity(world, e, e, NULL);
        ecs_os_memcpy(handles_out, handles_ptr, ecs_from_size_t(handles_size));   
    }

    /* Restore to previous state */
    ecs_set_scope(world, old_scope);
    world->name_prefix = old_name_prefix;

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name)
{
    ecs_assert(library_name != NULL, ECS_INVALID_PARAMETER, NULL);

    char *import_func = (char*)module_name; /* safe */
    char *module = (char*)module_name;

    if (!ecs_os_api.module_to_dl || 
        !ecs_os_api.dlopen || 
        !ecs_os_api.dlproc || 
        !ecs_os_api.dlclose) 
    {
        ecs_os_err(
            "library loading not supported, set module_to_dl, dlopen, dlclose "
            "and dlproc os API callbacks first");
        return 0;
    }

    /* If no module name is specified, try default naming convention for loading
     * the main module from the library */
    if (!import_func) {
        import_func = ecs_os_malloc(ecs_os_strlen(library_name) + ECS_SIZEOF("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = ecs_to_i8(toupper(ch));
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = ecs_to_i8(tolower(ch));
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        ecs_os_strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_trace_1("found file '%s' for library '%s'", 
            library_filename, library_name);
    }

    ecs_os_dl_t dl = ecs_os_dlopen(library_filename);
    if (!dl) {
        ecs_os_err("failed to load library '%s' ('%s')", 
            library_name, library_filename);
        
        ecs_os_free(library_filename);

        if (module != module_name) {
            ecs_os_free(module);
        }    

        return 0;
    } else {
        ecs_trace_1("library '%s' ('%s') loaded", 
            library_name, library_filename);
    }

    ecs_module_action_t action = (ecs_module_action_t)
        ecs_os_dlproc(dl, import_func);
    if (!action) {
        ecs_os_err("failed to load import function %s from library %s",
            import_func, library_name);
        ecs_os_free(library_filename);
        ecs_os_dlclose(dl);            
        return 0;
    } else {
        ecs_trace_1("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = ecs_import(world, action, module, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    ecs_os_free(library_filename);

    return result;
}

ecs_entity_t ecs_new_module(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    if (!e) {
        char *module_path = ecs_module_path_from_c(name);
        e = ecs_new_from_fullpath(world, module_path);

        EcsName *name_ptr = ecs_get_mut(world, e, EcsName, NULL);
        name_ptr->symbol = name;

        ecs_os_free(module_path);
    }

    ecs_entity_t result = ecs_new_component(world, e, NULL, size, alignment);
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);

    /* Add module tag */
    ecs_add_entity(world, result, EcsModule);

    /* Add module to itself. This way we have all the module information stored
     * in a single contained entity that we can use for namespacing */
    ecs_set_ptr_w_entity(world, result, result, size, NULL);

    /* Set the current scope to the module */
    ecs_set_scope(world, result);

    return result;
}

#endif

#ifdef FLECS_QUEUE

struct ecs_queue_t {
    ecs_vector_t *data;
    int32_t index;
};

ecs_queue_t* _ecs_queue_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_new(elem_size, offset, elem_count);
    result->index = 0;
    return result;
}

ecs_queue_t* _ecs_queue_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_from_array(elem_size, offset, elem_count, array);
    result->index = 0;
    return result;    
}

void* _ecs_queue_push(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t size = ecs_vector_size(buffer->data);
    int32_t count = ecs_vector_count(buffer->data);
    void *result;

    if (count == buffer->index) {
        result = _ecs_vector_add(&buffer->data, elem_size, offset);
    } else {
        result = _ecs_vector_get(buffer->data, elem_size, offset, buffer->index);
    }

    buffer->index = (buffer->index + 1) % size;

    return result;
}

void ecs_queue_free(
    ecs_queue_t *buffer)
{
    ecs_vector_free(buffer->data);
    ecs_os_free(buffer);
}

void* _ecs_queue_get(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    int32_t count = ecs_vector_count(buffer->data);
    int32_t size = ecs_vector_size(buffer->data);
    index = ((buffer->index - count + size) + (int32_t)index) % size;
    return _ecs_vector_get(buffer->data, elem_size, offset, index);
}

void* _ecs_queue_last(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t index = buffer->index;
    if (!index) {
        index = ecs_vector_size(buffer->data);
    }

    return _ecs_vector_get(buffer->data, elem_size, offset, index - 1);
}

int32_t ecs_queue_index(
    ecs_queue_t *buffer)
{
    return buffer->index;
}

int32_t ecs_queue_count(
    ecs_queue_t *buffer)
{
    return ecs_vector_count(buffer->data);
}

#endif

#ifdef FLECS_SNAPSHOT


/* World snapshot */
struct ecs_snapshot_t {
    ecs_world_t *world;
    ecs_ei_t entity_index;
    ecs_vector_t *tables;
    ecs_entity_t last_id;
    ecs_filter_t filter;
};

static
ecs_data_t* duplicate_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *main_data)
{
    ecs_data_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_data_t));

    int32_t i, column_count = table->column_count;
    ecs_entity_t *components = ecs_vector_first(table->type, ecs_entity_t);

    result->columns = ecs_os_memdup(
        main_data->columns, ECS_SIZEOF(ecs_column_t) * column_count);

    /* Copy entities */
    result->entities = ecs_vector_copy(main_data->entities, ecs_entity_t);
    ecs_entity_t *entities = ecs_vector_first(result->entities, ecs_entity_t);

    /* Copy record ptrs */
    result->record_ptrs = ecs_vector_copy(main_data->record_ptrs, ecs_record_t*);

    /* Copy each column */
    for (i = 0; i < column_count; i ++) {
        ecs_entity_t component = components[i];
        ecs_column_t *column = &result->columns[i];

        if (component > ECS_HI_COMPONENT_ID) {
            column->data = NULL;
            continue;
        }

        ecs_c_info_t *cdata = ecs_get_c_info(world, component);
        int16_t size = column->size;
        int16_t alignment = column->alignment;
        ecs_copy_t copy;

        if ((copy = cdata->lifecycle.copy)) {
            int32_t count = ecs_vector_count(column->data);
            ecs_vector_t *dst_vec = ecs_vector_new_t(size, alignment, count);
            ecs_vector_set_count_t(&dst_vec, size, alignment, count);
            void *dst_ptr = ecs_vector_first_t(dst_vec, size, alignment);
            void *ctx = cdata->lifecycle.ctx;
            
            ecs_xtor_t ctor = cdata->lifecycle.ctor;
            if (ctor) {
                ctor(world, component, entities, dst_ptr, ecs_to_size_t(size), 
                    count, ctx);
            }

            void *src_ptr = ecs_vector_first_t(column->data, size, alignment);
            copy(world, component, entities, entities, dst_ptr, src_ptr, 
                ecs_to_size_t(size), count, ctx);

            column->data = dst_vec;
        } else {
            column->data = ecs_vector_copy_t(column->data, size, alignment);
        }
    }

    return result;
}

static
ecs_snapshot_t* snapshot_create(
    ecs_world_t *world,
    const ecs_ei_t *entity_index,
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_snapshot_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_snapshot_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->world = world;

    /* If no iterator is provided, the snapshot will be taken of the entire
     * world, and we can simply copy the entity index as it will be restored
     * entirely upon snapshote restore. */
    if (!iter && entity_index) {
        result->entity_index = ecs_ei_copy(entity_index);
        result->tables = ecs_vector_new(ecs_table_leaf_t, 0);
    }

    ecs_iter_t iter_stack;
    if (!iter) {
        iter_stack = ecs_filter_iter(world, NULL);
        iter = &iter_stack;
        next = ecs_filter_next;
    }

    /* If an iterator is provided, this is a filterred snapshot. In this case we
     * have to patch the entity index one by one upon restore, as we don't want
     * to affect entities that were not part of the snapshot. */
    else {
        result->entity_index.hi = NULL;
        result->entity_index.lo = NULL;
    }

    /* Iterate tables in iterator */
    while (next(iter)) {
        ecs_table_t *t = iter->table;

        if (t->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_data_t *data = ecs_table_get_data(world, t);
        if (!data || !data->entities || !ecs_vector_count(data->entities)) {
            continue;
        }

        ecs_table_leaf_t *l = ecs_vector_add(&result->tables, ecs_table_leaf_t);

        l->table = t;
        l->type = t->type;
        l->data = duplicate_data(world, t, data);
    }

    return result;
}

/** Create a snapshot */
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world)
{
    ecs_snapshot_t *result = snapshot_create(
        world,
        &world->stage.entity_index,
        NULL,
        NULL);

    result->last_id = world->stats.last_id;

    return result;
}

/** Create a filtered snapshot */
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_world_t *world = iter->world;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_snapshot_t *result = snapshot_create(
        world,
        &world->stage.entity_index,
        iter,
        next);

    result->last_id = world->stats.last_id;

    return result;
}

/** Restore a snapshot */
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    bool is_filtered = true;

    if (snapshot->entity_index.lo || snapshot->entity_index.hi) {
        ecs_sparse_restore(world->stage.entity_index.lo, snapshot->entity_index.lo);
        ecs_sparse_free(snapshot->entity_index.lo);
        ecs_map_free(world->stage.entity_index.hi);
        world->stage.entity_index.hi = snapshot->entity_index.hi;
        is_filtered = false;
    }   

    if (!is_filtered) {
        world->stats.last_id = snapshot->last_id;
    }

    ecs_table_leaf_t *leafs = ecs_vector_first(snapshot->tables, ecs_table_leaf_t);
    int32_t l = 0, count = ecs_vector_count(snapshot->tables);
    int32_t t, table_count = ecs_sparse_count(world->stage.tables);

    for (t = 0; t < table_count; t ++) {
        ecs_table_t *table = ecs_sparse_get(world->stage.tables, ecs_table_t, t);
        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_table_leaf_t *leaf = NULL;
        if (l < count) {
            leaf = &leafs[l];
        }

        if (leaf && leaf->table == table) {
            /* If the snapshot is filtered, update the entity index for the
             * entities in the snapshot. If the snapshot was not filtered
             * the entity index would have been replaced entirely, and this
             * is not necessary. */
            if (is_filtered) {
                ecs_vector_each(leaf->data->entities, ecs_entity_t, e_ptr, {
                    ecs_record_t *r = ecs_eis_get(&world->stage, *e_ptr);
                    if (r && r->table) {
                        ecs_data_t *data = ecs_table_get_data(world, r->table);
                        
                        /* Data must be not NULL, otherwise entity index could
                         * not point to it */
                        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

                        bool is_monitored;
                        int32_t row = ecs_record_to_row(r->row, &is_monitored);
                        
                        /* Always delete entity, so that even if the entity is
                        * in the current table, there won't be duplicates */
                        ecs_table_delete(world, &world->stage, r->table, 
                            data, row);
                    }
                });

                ecs_table_merge_data(world, table, leaf->data);
            } else {
                ecs_table_replace_data(world, table, leaf->data);
            }
            
            ecs_os_free(leaf->data);
            l ++;
        } else {
            /* If the snapshot is not filtered, the snapshot should restore the
             * world to the exact state it was in. When a snapshot is filtered,
             * it should only update the entities that were in the snapshot.
             * If a table is found that was not in the snapshot, and the
             * snapshot was not filtered, clear the table. */
            if (!is_filtered) {
                /* Use clear_silent so no triggers are fired */
                ecs_table_clear_silent(world, table);
            }
        }

        table->alloc_count ++;
    }

    /* If snapshot was not filtered, run OnSet systems now. This cannot be done
     * while restoring the snapshot, because the world is in an inconsistent
     * state while restoring. When a snapshot is filtered, the world is not left
     * in an inconsistent state, which makes running OnSet systems while
     * restoring safe */
    if (!is_filtered) {
        for (t = 0; t < table_count; t ++) {
            ecs_table_t *table = ecs_sparse_get(world->stage.tables, ecs_table_t, t);
            if (table->flags & EcsTableHasBuiltins) {
                continue;
            }

            ecs_entities_t components = ecs_type_to_entities(table->type);
            ecs_data_t *table_data = ecs_table_get_data(world, table);
            int32_t entity_count = ecs_table_data_count(table_data);

            ecs_run_set_systems(world, &world->stage, &components, table, 
                table_data, 0, entity_count, true);            
        }
    }

    ecs_vector_free(snapshot->tables);   

    ecs_os_free(snapshot);
}

ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    ecs_snapshot_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = snapshot->tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = snapshot->world,
        .table_count = ecs_vector_count(snapshot->tables),
        .iter.snapshot = iter
    };
}

bool ecs_snapshot_next(
    ecs_iter_t *it)
{
    ecs_snapshot_iter_t *iter = &it->iter.snapshot;
    ecs_table_leaf_t *tables = ecs_vector_first(iter->tables, ecs_table_leaf_t);
    int32_t count = ecs_vector_count(iter->tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = tables[i].table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_data_t *data = tables[i].data;
        if (!data) {
            continue;
        }

        if (!ecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_data_count(data);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;    
}

/** Cleanup snapshot */
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot)
{
    ecs_ei_free(&snapshot->entity_index);

    ecs_table_leaf_t *tables = ecs_vector_first(snapshot->tables, ecs_table_leaf_t);
    int32_t i, count = ecs_vector_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_leaf_t *leaf = &tables[i];
        ecs_table_clear_data(leaf->table, leaf->data);
        ecs_os_free(leaf->data);
    }    

    ecs_vector_free(snapshot->tables);
    ecs_os_free(snapshot);
}

#endif

#ifdef FLECS_DBG


ecs_table_t *ecs_dbg_find_table(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_table_t *table = ecs_table_from_type(
        world, &world->stage, type);
        
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table;
}

void ecs_dbg_table(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_dbg_table_t *dbg_out)
{
    ecs_assert(dbg_out != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);

    *dbg_out = (ecs_dbg_table_t){.table = table};

    dbg_out->type = table->type;
    dbg_out->systems_matched = table->queries;

    /* Determine components from parent/base entities */
    ecs_entity_t *entities = ecs_vector_first(table->type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(table->type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        if (e & ECS_CHILDOF) {
            ecs_dbg_entity_t parent_dbg;
            ecs_dbg_entity(world, e & ECS_ENTITY_MASK, &parent_dbg);

            ecs_dbg_table_t parent_table_dbg;
            ecs_dbg_table(world, parent_dbg.table, &parent_table_dbg);

            /* Owned and shared components are available from container */
            dbg_out->container = ecs_type_merge(
                world, dbg_out->container, parent_dbg.type, NULL);
            dbg_out->container = ecs_type_merge(
                world, dbg_out->container, parent_table_dbg.shared, NULL);

            /* Add entity to list of parent entities */
            dbg_out->parent_entities = ecs_type_add(
                world, dbg_out->parent_entities, e & ECS_ENTITY_MASK);
        }

        if (e & ECS_INSTANCEOF) {
            ecs_dbg_entity_t base_dbg;
            ecs_dbg_entity(world, e & ECS_ENTITY_MASK, &base_dbg);

            ecs_dbg_table_t base_table_dbg;
            ecs_dbg_table(world, base_dbg.table, &base_table_dbg);            

            /* Owned and shared components are available from base */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, base_dbg.type, NULL);
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, base_table_dbg.shared, NULL);

            /* Never inherit EcsName or EcsPrefab */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, ecs_type(EcsName));
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, ecs_type_from_entity(world, EcsPrefab));

            /* Shared components are always masked by owned components */
            dbg_out->shared = ecs_type_merge(
                world, dbg_out->shared, NULL, table->type);

            /* Add entity to list of base entities */
            dbg_out->base_entities = ecs_type_add(
                world, dbg_out->base_entities, e & ECS_ENTITY_MASK);

            /* Add base entities of entity to list of base entities */
            dbg_out->base_entities = ecs_type_add(
                world, base_table_dbg.base_entities, e & ECS_ENTITY_MASK);                                                       
        }
    }

    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        dbg_out->entities = ecs_vector_first(data->entities, ecs_entity_t);
        dbg_out->entities_count = ecs_vector_count(data->entities);
    }
}

ecs_table_t* ecs_dbg_get_table(
    ecs_world_t *world,
    int32_t index)
{
    if (ecs_sparse_count(world->stage.tables) <= index) {
        return NULL;
    }

    return ecs_sparse_get(
        world->stage.tables, ecs_table_t, index);
}

bool ecs_dbg_filter_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_filter_t *filter)
{
    return ecs_table_match_filter(world, table, filter);
}

ecs_type_t ecs_dbg_table_get_type(
    ecs_table_t *table)
{
    return table->type;
}

void ecs_dbg_entity(
    ecs_world_t *world, 
    ecs_entity_t entity, 
    ecs_dbg_entity_t *dbg_out)
{
    *dbg_out = (ecs_dbg_entity_t){.entity = entity};
    
    ecs_entity_info_t info = { 0 };
    if (ecs_get_info(world, &world->stage, entity, &info)) {
        dbg_out->table = info.table;
        dbg_out->row = info.row;
        dbg_out->is_watched = info.is_watched;
        dbg_out->type = info.table ? info.table->type : NULL;
    }
}

#endif

#ifdef FLECS_READER_WRITER


static
bool iter_table(
    ecs_world_t *world,
    ecs_table_reader_t *reader,
    ecs_iter_t *it,
    ecs_iter_next_action_t next,
    bool skip_builtin)    
{
    bool table_found = false;

    while (next(it)) {
        ecs_table_t *table = it->table;

        reader->table = table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_data_t *data = ecs_table_get_data(world, table);
        reader->data = data;
        reader->table_index ++;

        if (skip_builtin && reader->table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!data || !it->count) {
            continue;
        }

        table_found = true;
        break;
    }

    return table_found;
}

static
void next_table(
    ecs_world_t *world,
    ecs_reader_t *stream,
    ecs_table_reader_t *reader)
{    
    int32_t count;

    /* First iterate all component tables, as component data must always be
     * stored in a blob before anything else */
    bool table_found = iter_table(
        world, reader, &stream->component_iter, stream->component_next, 
        false);

    /* If all components have been added, add the regular data tables. Make sure
     * to not add component tables again, in case the provided iterator also
     * matches component tables. */
    if (!table_found) {
        table_found = iter_table(
            world, reader, &stream->data_iter, stream->data_next, true);
        count = stream->data_iter.count;
    } else {
        count = stream->component_iter.count;
    }

    if (!table_found) {
        stream->state = EcsFooterSegment;
    } else {
        reader->type = reader->table->type;
        reader->total_columns = reader->table->column_count + 1;
        reader->column_index = 0;
        reader->row_count = count;
    }
}

static
void ecs_table_reader_next(
    ecs_reader_t *stream)
{
    ecs_table_reader_t *reader = &stream->table;
    ecs_world_t *world = stream->world;

    switch(reader->state) {
    case EcsTableHeader:
        reader->state = EcsTableTypeSize;
        break;
    case EcsTableTypeSize:
        reader->state = EcsTableType;
        reader->type_written = 0;
        break;

    case EcsTableType:
        reader->state = EcsTableSize;
        break;

    case EcsTableSize: {
        reader->state = EcsTableColumnHeader;
        break;
    }

    case EcsTableColumnHeader:
        reader->state = EcsTableColumnSize;
        if (!reader->column_index) {
            reader->column_vector = reader->data->entities;
            reader->column_size = ECS_SIZEOF(ecs_entity_t);
        } else {
            ecs_column_t *column = 
                &reader->data->columns[reader->column_index - 1];
            reader->column_vector = column->data;
            reader->column_size = column->size;
            reader->column_alignment = column->alignment;
        }
        break;

    case EcsTableColumnSize:
        reader->state = EcsTableColumnData;
        reader->column_data = ecs_vector_first_t(reader->column_vector, 
            reader->column_size, reader->column_alignment);
        reader->column_written = 0;
        break;

    case EcsTableColumnNameHeader: {
        reader->state = EcsTableColumnNameLength;
        ecs_column_t *column = 
                &reader->data->columns[reader->column_index - 1];
        reader->column_vector = column->data;                
        reader->column_data = ecs_vector_first(reader->column_vector, EcsName);
        reader->row_index = 0;
        break;
    }

    case EcsTableColumnNameLength:
        reader->state = EcsTableColumnName;
        reader->row_index ++;
        break;

    case EcsTableColumnName:
        if (reader->row_index < reader->row_count) {
            reader->state = EcsTableColumnNameLength;
            break;
        }
        // fall through

    case EcsTableColumnData:
        reader->column_index ++;
        if (reader->column_index == reader->total_columns) {
            reader->state = EcsTableHeader;
            next_table(world, stream, reader);
        } else {
            ecs_entity_t *type_buffer = ecs_vector_first(reader->type, ecs_entity_t);
            if (reader->column_index >= 1) {
                ecs_entity_t e = type_buffer[reader->column_index - 1];
                
                if (e != ecs_entity(EcsName)) {
                    reader->state = EcsTableColumnHeader;
                } else {
                    reader->state = EcsTableColumnNameHeader;
                }
            } else {
                reader->state = EcsTableColumnHeader;
            }            
        }
        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    return;
}

static
ecs_size_t ecs_table_reader(
    char *buffer,
    ecs_size_t size,
    ecs_reader_t *stream)
{
    if (!size) {
        return 0;
    }

    if (size < ECS_SIZEOF(int32_t)) {
        return -1;
    }

    ecs_table_reader_t *reader = &stream->table;
    ecs_size_t read = 0;

    if (!reader->state) {
        next_table(stream->world, stream, reader);
        reader->state = EcsTableHeader;
    }

    switch(reader->state) {
    case EcsTableHeader:  
        *(ecs_blob_header_kind_t*)buffer = EcsTableHeader;
        read = ECS_SIZEOF(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableTypeSize:
        *(int32_t*)buffer = ecs_vector_count(reader->type);
        read = ECS_SIZEOF(int32_t);
        ecs_table_reader_next(stream);
        break;  

    case EcsTableType: {
        ecs_entity_t *type_array = ecs_vector_first(reader->type, ecs_entity_t);
        *(int32_t*)buffer = *(int32_t*)ECS_OFFSET(type_array, reader->type_written);
        reader->type_written += ECS_SIZEOF(int32_t);
        read = ECS_SIZEOF(int32_t);

        if (reader->type_written == ecs_vector_count(reader->type) * ECS_SIZEOF(ecs_entity_t)) {
            ecs_table_reader_next(stream);
        }
        break;                
    }

    case EcsTableSize:
        *(int32_t*)buffer = ecs_table_count(reader->table);
        read = ECS_SIZEOF(int32_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableColumnHeader:
        *(ecs_blob_header_kind_t*)buffer = EcsTableColumnHeader;
        read = ECS_SIZEOF(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break; 

    case EcsTableColumnSize:
        *(int32_t*)buffer = reader->column_size;
        read = ECS_SIZEOF(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);

        if (!reader->column_size) {
            ecs_table_reader_next(stream);
        }
        break; 

    case EcsTableColumnData: {
        ecs_size_t column_bytes = reader->column_size * reader->row_count;
        read = column_bytes - reader->column_written;
        if (read > size) {
            read = size;
        }

        ecs_os_memcpy(buffer, ECS_OFFSET(reader->column_data, reader->column_written), read);
        reader->column_written += read;
        ecs_assert(reader->column_written <= column_bytes, ECS_INTERNAL_ERROR, NULL);

        ecs_size_t align = (((read - 1) / ECS_SIZEOF(int32_t)) + 1) * ECS_SIZEOF(int32_t);
        if (align != read) {
            /* Initialize padding bytes to 0 to keep valgrind happy */
            ecs_os_memset(ECS_OFFSET(buffer, read), 0, align - read);

            /* Set read to align so that data is always aligned to 4 bytes */
            read = align;

            /* Buffer sizes are expected to be aligned to 4 bytes and the rest
             * of the serialized data is aligned to 4 bytes. Should never happen
             * that adding padding bytes exceeds the size. */
            ecs_assert(read <= size, ECS_INTERNAL_ERROR, NULL);
        }

        if (reader->column_written == column_bytes) {
            ecs_table_reader_next(stream);
        }
        break;
    }

    case EcsTableColumnNameHeader:
        *(ecs_blob_header_kind_t*)buffer = EcsTableColumnNameHeader;
        read = ECS_SIZEOF(ecs_blob_header_kind_t);
        ecs_table_reader_next(stream);
        break;

    case EcsTableColumnNameLength:
        reader->name = ((EcsName*)reader->column_data)[reader->row_index].value;
        reader->name_len = ecs_os_strlen(reader->name) + 1;
        reader->name_written = 0;
        *(int32_t*)buffer = reader->name_len;
        read = ECS_SIZEOF(int32_t);
        ecs_table_reader_next(stream);    
        break;

    case EcsTableColumnName:   
        read = reader->name_len - reader->name_written;
        if (read >= ECS_SIZEOF(int32_t)) {

            int32_t i;
            for (i = 0; i < 4; i ++) {
                *(char*)ECS_OFFSET(buffer, i) = 
                    *(char*)ECS_OFFSET(reader->name, reader->name_written + i);
            }

            reader->name_written += ECS_SIZEOF(int32_t);
        } else {
            ecs_os_memcpy(buffer, ECS_OFFSET(reader->name, reader->name_written), read);
            ecs_os_memset(ECS_OFFSET(buffer, read), 0, ECS_SIZEOF(int32_t) - read);
            reader->name_written += read;
        }

        /* Always align buffer to multiples of 4 bytes */
        read = ECS_SIZEOF(int32_t);

        if (reader->name_written == reader->name_len) {
            ecs_table_reader_next(stream);
        }

        break;

    default:
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(read % 4 == 0, ECS_INTERNAL_ERROR, NULL);

    return read;
}

ecs_size_t ecs_reader_read(
    char *buffer,
    ecs_size_t size,
    ecs_reader_t *reader)
{
    ecs_size_t read, total_read = 0, remaining = size;

    if (!size) {
        return 0;
    }

    ecs_assert(size >= ECS_SIZEOF(int32_t), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(size % 4 == 0, ECS_INVALID_PARAMETER, NULL);

    if (reader->state == EcsTableSegment) {
        while ((read = ecs_table_reader(ECS_OFFSET(buffer, total_read), remaining, reader))) {
            remaining -= read;
            total_read += read;

            if (reader->state != EcsTableSegment) {
                break;
            }

            ecs_assert(remaining % 4 == 0, ECS_INTERNAL_ERROR, NULL);        
        }
    }  
    
    return total_read;
}

ecs_reader_t ecs_reader_init(
    ecs_world_t *world)
{
    ecs_reader_t result = {
        .world = world,
        .state = EcsTableSegment,
        .component_iter = ecs_filter_iter(world, &(ecs_filter_t){
            .include = ecs_type(EcsComponent)
        }),
        .component_next = ecs_filter_next,
        .data_iter = ecs_filter_iter(world, NULL),
        .data_next = ecs_filter_next
    };

    return result;
}

ecs_reader_t ecs_reader_init_w_iter(
    ecs_iter_t *it,
    ecs_iter_next_action_t next)
{
    ecs_world_t *world = it->world;

    ecs_reader_t result = {
        .world = world,
        .state = EcsTableSegment,
        .component_iter = ecs_filter_iter(world, &(ecs_filter_t){
            .include = ecs_type(EcsComponent)
        }),
        .component_next = ecs_filter_next,
        .data_iter = *it,
        .data_next = next
    };

    return result;
}

#endif

#ifdef FLECS_BULK


static
void bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    bool is_delete)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, 
        "delete_w_filter currently only supported on main stage");

    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_data_t *data = ecs_table_get_data(world, table);
        if (!data) {
            /* If table has no data, there's nothing to delete */
            continue;
        }

        ecs_vector_t *entities = NULL;
        if (data) {
            entities = data->entities;
        }

        ecs_vector_each(entities, ecs_entity_t, e_ptr, {
            ecs_eis_delete(&world->stage, *e_ptr);
        })

        /* Both filters passed, clear table */
        if (is_delete) {
            ecs_table_clear(world, table);
        } else {
            ecs_table_clear_silent(world, table);
        }
    }
}

static
void merge_table(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    ecs_entities_t *to_add,
    ecs_entities_t *to_remove)
{    
    if (!dst_table->type) {
        /* If this removes all components, clear table */
        ecs_table_clear(world, src_table);
    } else {
        /* Merge table into dst_table */
        if (dst_table != src_table) {
            ecs_data_t *src_data = ecs_table_get_data(world, src_table);
            int32_t dst_count = ecs_table_count(dst_table);
            int32_t src_count = ecs_table_count(src_table);

            if (to_remove && to_remove->count && src_data) {
                ecs_run_remove_actions(world, &world->stage, src_table, 
                    src_data, 0, src_count, to_remove, false);
            }

            ecs_table_merge(world, dst_table, src_table);
            ecs_data_t *dst_data = ecs_table_get_data(world, dst_table);

            if (to_add && to_add->count && dst_data) {
                ecs_comp_mask_t set_mask = {0};
                ecs_run_add_actions(world, &world->stage, dst_table, dst_data, 
                    dst_count, src_count, to_add, set_mask, false, true);
            }
        }
    }
}

/* -- Public API -- */

void ecs_bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, true);
}

void ecs_bulk_add_remove_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_add_array = ecs_type_to_entities(to_add);
    ecs_entities_t to_remove_array = ecs_type_to_entities(to_remove);

    ecs_entities_t added = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_add_array.count),
        .count = 0
    }; 

    ecs_entities_t removed = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);
        
        dst_table = ecs_table_traverse_add(
            world, stage, dst_table, &to_add_array, &added);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (table == dst_table || (!added.count && !removed.count)) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   

        merge_table(world, dst_table, table, &added, &removed);
        added.count = 0;
        removed.count = 0;
    }    
}

void ecs_bulk_add_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_add_array = ecs_type_to_entities(to_add);
    ecs_entities_t added = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_add_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_array, &added);
        
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!added.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
    }    
}

void ecs_bulk_add_entity(
    ecs_world_t *world,
    ecs_entity_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_add_array = { .array = &to_add, .count = 1 };

    ecs_entity_t added_entity;
    ecs_entities_t added = {
        .array = &added_entity,
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_add(
            world, stage, table, &to_add_array, &added);

        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);
        
        if (!added.count) {
            continue;
        }         

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
    }    
}

void ecs_bulk_remove_type(
    ecs_world_t *world,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_remove_array = ecs_type_to_entities(to_remove);
    ecs_entities_t removed = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
    }    
}

void ecs_bulk_remove_entity(
    ecs_world_t *world,
    ecs_entity_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = ecs_get_stage(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);

    ecs_entities_t to_remove_array = { .array = &to_remove, .count = 1 };

    ecs_entity_t removed_entity;
    ecs_entities_t removed = {
        .array = &removed_entity,
        .count = 0
    };

    int32_t i, count = ecs_sparse_count(stage->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_match_filter(world, table, filter)) {
            continue;
        }            

        ecs_table_t *dst_table = ecs_table_traverse_remove(
            world, stage, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
    }    
}

#endif

#ifdef __BAKE__

static
ecs_os_thread_t bake_thread_new(
    ecs_os_thread_callback_t callback, 
    void *param)
{
    return (ecs_os_thread_t)ut_thread_new(callback, param);
}

static
void* bake_thread_join(
    ecs_os_thread_t thread)
{
    void *arg;
    ut_thread_join((ut_thread)thread, &arg);
    return arg;
}

static
int32_t bake_ainc(int32_t *value) {
    return ut_ainc(value);
}

static
int32_t bake_adec(int32_t *value) {
    return ut_adec(value);
}

static
ecs_os_mutex_t bake_mutex_new(void) {
    struct ut_mutex_s *m = ecs_os_api.malloc(sizeof(struct ut_mutex_s));
    ut_mutex_new(m);
    return (ecs_os_mutex_t)(uintptr_t)m;
}

static
void bake_mutex_free(ecs_os_mutex_t mutex) {
    ut_mutex_free((struct ut_mutex_s*)mutex);
    ecs_os_api.free((struct ut_mutex_s*)mutex);
}

static
void bake_mutex_lock(ecs_os_mutex_t mutex) {
    ut_mutex_lock((struct ut_mutex_s*)mutex);
}

static
void bake_mutex_unlock(ecs_os_mutex_t mutex) {
    ut_mutex_unlock((struct ut_mutex_s*)mutex);
}

static
ecs_os_cond_t bake_cond_new(void) {
    struct ut_cond_s *c = ecs_os_api.malloc(sizeof(struct ut_cond_s));
    ut_cond_new(c);
    return (ecs_os_cond_t)(uintptr_t)c;
}

static 
void bake_cond_free(ecs_os_cond_t cond) {
    ut_cond_free((struct ut_cond_s *)cond);
    ecs_os_api.free((struct ut_cond_s *)cond);
}

static 
void bake_cond_signal(ecs_os_cond_t cond) {
    ut_cond_signal((struct ut_cond_s *)cond);
}

static 
void bake_cond_broadcast(ecs_os_cond_t cond) {
    ut_cond_broadcast((struct ut_cond_s *)cond);
}

static 
void bake_cond_wait(ecs_os_cond_t cond, ecs_os_mutex_t mutex) {
    ut_cond_wait((struct ut_cond_s *)cond, (struct ut_mutex_s *)mutex);
}

static
ecs_os_dl_t bake_dlopen(
    const char *dlname)
{
    return (ecs_os_dl_t)ut_dl_open(dlname);
}

static
void bake_dlclose(
    ecs_os_dl_t dl)
{
    ut_dl_close((ut_dl)dl);
}

static
ecs_os_proc_t bake_dlproc(
    ecs_os_dl_t dl,
    const char *procname)
{
    ecs_os_proc_t result = (ecs_os_proc_t)ut_dl_proc((ut_dl)dl, procname);
    if (!result) {
        ut_raise();
    }
    return result;
}

static
char* bake_module_to_dl(
    const char *module_id)
{
    const char *result = ut_locate(module_id, NULL, UT_LOCATE_LIB);
    if (result) {
        return ut_strdup(result);
    } else {
        return NULL;
    }
}

static
char* bake_module_to_etc(
    const char *module_id)
{
    const char *result = ut_locate(module_id, NULL, UT_LOCATE_ETC);
    if (result) {
        return ut_strdup(result);
    } else {
        return NULL;
    }
}

void ecs_os_api_impl(ecs_os_api_t *api) {
    api->thread_new = bake_thread_new;
    api->thread_join = bake_thread_join;
    api->ainc = bake_ainc;
    api->adec = bake_adec;
    api->mutex_new = bake_mutex_new;
    api->mutex_free = bake_mutex_free;
    api->mutex_lock = bake_mutex_lock;
    api->mutex_unlock = bake_mutex_unlock;
    api->cond_new = bake_cond_new;
    api->cond_free = bake_cond_free;
    api->cond_signal = bake_cond_signal;
    api->cond_broadcast = bake_cond_broadcast;
    api->cond_wait = bake_cond_wait;
    api->dlopen = bake_dlopen;
    api->dlproc = bake_dlproc;
    api->dlclose = bake_dlclose;
    api->module_to_dl = bake_module_to_dl;
    api->module_to_etc = bake_module_to_etc;
}

#else

void ecs_os_api_impl(ecs_os_api_t *api) {
    (void)api;
}

#endif

/* -- Global variables -- */

#ifndef NDEBUG
static
void no_threading(
    const char *function)
{
    ecs_trace(1, "threading unavailable: %s not implemented", function);
}

static
void no_time(
    const char *function)
{
    ecs_trace(1, "time management: %s not implemented", function);
}
#endif

/* -- Private functions -- */

ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_THREAD_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        if (world->in_progress) {
            return &world->temp_stage;
        } else {
            return &world->stage;
        }
    } else if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thread = (ecs_thread_t*)world;
        *world_ptr = thread->world;
        return thread->stage;
    }
    
    return NULL;
}

/* Evaluate component monitor. If a monitored entity changed it will have set a
 * flag in one of the world's component monitors. Queries can register 
 * themselves with component monitors to determine whether they need to rematch
 * with tables. */
static
void eval_component_monitor(
    ecs_world_t *world,
    ecs_component_monitor_t *mon)
{
    if (!mon->rematch) {
        return;
    }

    ecs_vector_t *eval[ECS_HI_COMPONENT_ID];
    int32_t eval_count = 0;

    int32_t i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        if (mon->dirty_flags[i]) {
            eval[eval_count ++] = mon->monitors[i];
            mon->dirty_flags[i] = 0;
        }
    }

    for (i = 0; i < eval_count; i ++) {
        ecs_vector_each(eval[i], ecs_query_t*, q_ptr, {
            ecs_query_rematch(world, *q_ptr);
        });
    }
    
    mon->rematch = false;
}

void ecs_component_monitor_mark(
    ecs_component_monitor_t *mon,
    ecs_entity_t component)
{
    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    if (mon->monitors[component]) {
        mon->dirty_flags[component] = true;
        mon->rematch = true;
    }
}

void ecs_component_monitor_register(
    ecs_component_monitor_t *mon,
    ecs_entity_t component,
    ecs_query_t *query)
{
    ecs_assert(mon != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Ignore component ids > ECS_HI_COMPONENT_ID */
    if(component >= ECS_HI_COMPONENT_ID) {
        return;
    }    

    ecs_query_t **q = ecs_vector_add(&mon->monitors[component], ecs_query_t*);
    *q = query;
}

void ecs_component_monitor_free(
    ecs_component_monitor_t *mon)
{
    int i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_vector_free(mon->monitors[i]);
    }
}

/* -- Public functions -- */

ecs_world_t *ecs_mini(void) {
    ecs_os_set_api_defaults();

    ecs_trace_1("bootstrap");
    ecs_log_push();

#ifdef __BAKE__
    ut_init(NULL);
    if (ut_load_init(NULL, NULL, NULL, NULL)) {
        ecs_os_err("warning: failed to initialize package loader");
    }
#endif

    ecs_assert(ecs_os_api.malloc != NULL, ECS_MISSING_OS_API, "malloc");
    ecs_assert(ecs_os_api.realloc != NULL, ECS_MISSING_OS_API, "realloc");
    ecs_assert(ecs_os_api.calloc != NULL, ECS_MISSING_OS_API, "calloc");

    bool time_ok = true;

#ifndef NDEBUG
    bool thr_ok = true;
    if (!ecs_os_api.mutex_new) {thr_ok = false; no_threading("mutex_new");}
    if (!ecs_os_api.mutex_free) {thr_ok = false; no_threading("mutex_free");}
    if (!ecs_os_api.mutex_lock) {thr_ok = false; no_threading("mutex_lock");}
    if (!ecs_os_api.mutex_unlock) {thr_ok = false; no_threading("mutex_unlock");}
    if (!ecs_os_api.cond_new) {thr_ok = false; no_threading("cond_new");}
    if (!ecs_os_api.cond_free) {thr_ok = false; no_threading("cond_free");}
    if (!ecs_os_api.cond_wait) {thr_ok = false; no_threading("cond_wait");}
    if (!ecs_os_api.cond_signal) {thr_ok = false; no_threading("cond_signal");}
    if (!ecs_os_api.cond_broadcast) {thr_ok = false; no_threading("cond_broadcast"); }
    if (!ecs_os_api.thread_new) {thr_ok = false; no_threading("thread_new");}
    if (!ecs_os_api.thread_join) {thr_ok = false; no_threading("thread_join");}
    if (thr_ok) {
        ecs_trace_1("threading available");
    } else {
        ecs_trace_1("threading unavailable");
    }

    if (!ecs_os_api.get_time) {time_ok = false; no_time("get_time");}
    if (!ecs_os_api.sleep) {time_ok = false; no_time("sleep");}
    if (time_ok) {
        ecs_trace_1("time management available");
    } else {
        ecs_trace_1("time management unavailable");
    }
#endif

    ecs_world_t *world = ecs_os_malloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;
    memset(&world->c_info, 0, sizeof(ecs_c_info_t) * ECS_HI_COMPONENT_ID); 
    world->t_info = ecs_map_new(ecs_c_info_t, 0);  
    world->fini_actions = NULL; 

    world->queries = ecs_sparse_new(ecs_query_t, 0);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    world->child_tables = NULL;
    world->name_prefix = NULL;

    memset(&world->component_monitors, 0, sizeof(world->component_monitors));
    memset(&world->parent_monitors, 0, sizeof(world->parent_monitors));

    world->type_handles = ecs_map_new(ecs_entity_t, 0);
    world->on_activate_components = ecs_map_new(ecs_on_demand_in_t, 0);
    world->on_enable_components = ecs_map_new(ecs_on_demand_in_t, 0);

    world->stage_count = 2;
    world->worker_stages = NULL;
    world->workers = NULL;
    world->workers_waiting = 0;
    world->workers_running = 0;
    world->valid_schedule = false;
    world->quit_workers = false;
    world->in_progress = false;
    world->is_merging = false;
    world->auto_merge = true;
    world->measure_frame_time = false;
    world->measure_system_time = false;
    world->should_quit = false;
    world->locking_enabled = false;
    world->pipeline = 0;

    world->frame_start_time = (ecs_time_t){0, 0};
    if (time_ok) {
        ecs_os_get_time(&world->world_start_time);
    }

    world->stats.target_fps = 0;
    world->stats.last_id = 0;

    world->stats.delta_time_raw = 0;
    world->stats.delta_time = 0;
    world->stats.time_scale = 1.0;
    world->stats.frame_time_total = 0;
    world->stats.sleep_err = 0;
    world->stats.system_time_total = 0;
    world->stats.merge_time_total = 0;
    world->stats.world_time_total = 0;
    world->stats.frame_count_total = 0;
    world->stats.merge_count_total = 0;
    world->stats.systems_ran_frame = 0;
    world->stats.pipeline_build_count_total = 0;

    world->fps_sleep = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    ecs_stage_init(world, &world->stage);
    ecs_stage_init(world, &world->temp_stage);

    world->stage.world = world;
    world->temp_stage.world = world;

    ecs_bootstrap(world);

    ecs_log_pop();

    return world;
}

ecs_world_t *ecs_init(void) {
    ecs_world_t *world = ecs_mini();

#ifdef FLECS_MODULE_H
    ecs_trace_1("import builtin modules");
    ecs_log_push();
#ifdef FLECS_SYSTEM_H
    ECS_IMPORT(world, FlecsSystem);
#endif
#ifdef FLECS_PIPELINE_H
    ECS_IMPORT(world, FlecsPipeline);
#endif
#ifdef FLECS_TIMER_H
    ECS_IMPORT(world, FlecsTimer);
#endif
    ecs_log_pop();
#endif

    return world;
}

#define ARG(short, long, action)\
    if (i < argc) {\
        if (argv[i][0] == '-') {\
            if (argv[i][1] == '-') {\
                if (long && !strcmp(&argv[i][2], long ? long : "")) {\
                    action;\
                    parsed = true;\
                }\
            } else {\
                if (short && argv[i][1] == short) {\
                    action;\
                    parsed = true;\
                }\
            }\
        }\
    }

ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[])
{
    (void)argc;
    (void)argv;
    return ecs_init();
}

static
void on_demand_in_map_deinit(
    ecs_map_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_on_demand_in_t *elem;

    while ((elem = ecs_map_next(&it, ecs_on_demand_in_t, NULL))) {
        ecs_vector_free(elem->systems);
    }

    ecs_map_free(map);
}

static
void ctor_init_zero(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entity_ptr,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entity_ptr;
    (void)ctx;
    ecs_os_memset(ptr, 0, ecs_from_size_t(size) * count);
}

ecs_flags32_t ecs_get_component_action_flags(
    ecs_c_info_t *c_info) 
{
    ecs_flags32_t flags = 0;

    if (c_info->lifecycle.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (c_info->lifecycle.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (c_info->on_add) {
        flags |= EcsTableHasOnAdd;
    }
    if (c_info->on_remove) {
        flags |= EcsTableHasOnRemove;
    }    

    return flags;  
}

void ecs_notify_tables_of_component_actions(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_c_info_t *c_info)
{
    ecs_flags32_t flags = ecs_get_component_action_flags(c_info);

    /* Iterate tables to set flags based on what actions have been registered */
    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        if (ecs_type_owns_entity(world, table->type, component, true)) {
            /* Reset lifecycle flags before setting */
            table->flags &= ~EcsTableHasLifecycle;

            /* Set lifecycle flags */
            table->flags |= flags;
        }
    }
}

void ecs_set_component_actions_w_entity(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *lifecycle)
{
    const EcsComponent *component_ptr = ecs_get(world, component, EcsComponent);
    
    /* Cannot register lifecycle actions for things that aren't a component */
    ecs_assert(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Cannot register lifecycle actions for components with size 0 */
    ecs_assert(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, component);
    c_info->lifecycle = *lifecycle;

    /* If no constructor is set, invoking any of the other lifecycle actions is
     * not safe as they will potentially access uninitialized memory. For ease
     * of use, if no constructor is specified, set a default one that 
     * initializes the component to 0. */
    if (!lifecycle->ctor) {
        c_info->lifecycle.ctor = ctor_init_zero;   
    }

    ecs_notify_tables_of_component_actions(world, component, c_info);
}

void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_action_elem_t *elem = ecs_vector_add(&world->fini_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;
}

void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_action_elem_t *elem = ecs_vector_add(&stage->post_frame_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;    
}

int ecs_fini(
    ecs_world_t *world)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    assert(!world->in_progress);
    assert(!world->is_merging);

    /* Unset data in tables */
    ecs_sparse_each(world->stage.tables, ecs_table_t, table, {
        ecs_table_unset(world, table);
    });

    /* Execute fini actions */
    ecs_vector_each(world->fini_actions, ecs_action_elem_t, elem, {
        elem->action(world, elem->ctx);
    });

    ecs_vector_free(world->fini_actions);

    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    /* Cleanup stages */
    ecs_stage_deinit(world, &world->stage);
    ecs_stage_deinit(world, &world->temp_stage);

    /* Cleanup component lifecycle callbacks & systems */
    int32_t i;
    for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
        ecs_vector_free(world->c_info[i].on_add);
        ecs_vector_free(world->c_info[i].on_remove);
    }

    ecs_map_iter_t it = ecs_map_iter(world->t_info);
    ecs_c_info_t *c_info;
    while ((c_info = ecs_map_next(&it, ecs_c_info_t, NULL))) {
        ecs_vector_free(c_info->on_add);
        ecs_vector_free(c_info->on_remove);
    }    

    ecs_map_free(world->t_info);

    /* Cleanup queries */
    int32_t count = ecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *q = ecs_sparse_get(world->queries, ecs_query_t, i);
        ecs_query_free(q);
    }
    ecs_sparse_free(world->queries);

    /* Cleanup child tables */
    it = ecs_map_iter(world->child_tables);
    ecs_vector_t *tables;
    while ((tables = ecs_map_next_ptr(&it, ecs_vector_t*, NULL))) {
        ecs_vector_free(tables);
    }

    ecs_map_free(world->child_tables);

    /* Cleanup misc data structures */
    on_demand_in_map_deinit(world->on_activate_components);
    on_demand_in_map_deinit(world->on_enable_components);
    ecs_map_free(world->type_handles);
    ecs_vector_free(world->fini_tasks);
    ecs_component_monitor_free(&world->component_monitors);
    ecs_component_monitor_free(&world->parent_monitors);

    /* In case the application tries to use the memory of the freed world, this
     * will trigger an assert */
    world->magic = 0;

    ecs_increase_timer_resolution(0);

    /* The end of the world */
    ecs_os_free(world);

#ifdef __BAKE__
    ut_deinit();
#endif    

    return 0;
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    ecs_eis_set_size(&world->stage, entity_count + ECS_HI_COMPONENT_ID);
}

void ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t entity_count)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    if (type) {
        ecs_table_t *table = ecs_table_from_type(
            world, &world->stage, type);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, table);
        ecs_table_set_size(world, table, data, entity_count);
    }
}

void ecs_eval_component_monitors(
    ecs_world_t *world)
{
    eval_component_monitor(world, &world->component_monitors);
    eval_component_monitor(world, &world->parent_monitors);
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    assert(world->is_merging == false);

    bool measure_frame_time = world->measure_frame_time;

    world->is_merging = true;

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    ecs_stage_merge(world, &world->temp_stage);

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_stage_merge(world, stage);
    });

    world->is_merging = false;

    ecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->stats.merge_time_total += (float)ecs_time_measure(&t_start);
    }

    world->stats.merge_count_total ++;

    /* Execute post frame actions */
    ecs_vector_each(world->stage.post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });

    ecs_vector_free(world->stage.post_frame_actions);
    world->stage.post_frame_actions = NULL;

    ecs_vector_each(world->temp_stage.post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });
    ecs_vector_free(world->temp_stage.post_frame_actions);
    world->temp_stage.post_frame_actions = NULL;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_vector_each(stage->post_frame_actions, ecs_action_elem_t, action, {
            action->action(world, action->ctx);
        });
        ecs_vector_free(stage->post_frame_actions);
        stage->post_frame_actions = NULL;
    });    
}

void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->auto_merge = auto_merge;
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");

    if (world->stats.target_fps == 0.0 || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    world->measure_system_time = enable;
}

/* Increase timer resolution based on target fps */
static void set_timer_resolution(float fps)
{
    if(fps >= 60.0f) ecs_increase_timer_resolution(1);
    else ecs_increase_timer_resolution(0);
}

void ecs_set_target_fps(
    ecs_world_t *world,
    float fps)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(ecs_os_api.get_time != NULL, ECS_MISSING_OS_API, "get_time");
    ecs_assert(ecs_os_api.sleep != NULL, ECS_MISSING_OS_API, "sleep");

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->stats.target_fps = fps;
        set_timer_resolution(fps);
    }
}

void* ecs_get_context(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    return world->context;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!id_end || id_end > id_start, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!id_end || id_end > world->stats.last_id, ECS_INVALID_PARAMETER, NULL);

    if (world->stats.last_id < id_start) {
        world->stats.last_id = id_start - 1;
    }

    world->stats.min_id = id_start;
    world->stats.max_id = id_end;
}

bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool old_value = stage->range_check_enabled;
    stage->range_check_enabled = enable;
    return old_value;
}

int32_t ecs_get_thread_index(
    ecs_world_t *world)
{
    if (world->magic == ECS_THREAD_MAGIC) {
        ecs_thread_t *thr = (ecs_thread_t*)world;
        return thr->index;
    } else if (world->magic == ECS_WORLD_MAGIC) {
        return 0;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

int32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->workers);
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
    if (enable) {
        if (!world->locking_enabled) {
            world->mutex = ecs_os_mutex_new();
            world->thr_sync = ecs_os_mutex_new();
            world->thr_cond = ecs_os_cond_new();
        }
    } else {
        if (world->locking_enabled) {
            ecs_os_mutex_free(world->mutex);
            ecs_os_mutex_free(world->thr_sync);
            ecs_os_cond_free(world->thr_cond);
        }
    }

    bool old = world->locking_enabled;
    world->locking_enabled = enable;
    return old;
}

void ecs_lock(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->mutex);
}

void ecs_unlock(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->mutex);
}

void ecs_begin_wait(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->thr_sync);
    ecs_os_cond_wait(world->thr_cond, world->thr_sync);
}

void ecs_end_wait(
    ecs_world_t *world)
{
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->thr_sync);
}

ecs_c_info_t * ecs_get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    if (component < ECS_HI_COMPONENT_ID) {
        return &world->c_info[component];
    } else {
        return ecs_map_get(world->t_info, ecs_c_info_t, component);
    }
}

ecs_c_info_t * ecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_c_info_t *c_info = ecs_get_c_info(world, component);
    if (!c_info) {
        ecs_assert(component >= ECS_HI_COMPONENT_ID, ECS_INTERNAL_ERROR, NULL);
        ecs_c_info_t t_info = { 0 };
        ecs_map_set(world->t_info, component, &t_info);
        c_info = ecs_map_get(world->t_info, ecs_c_info_t, component);
        ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);      
    }

    return c_info;      
}

bool ecs_staging_begin(
    ecs_world_t *world)
{
    bool in_progress = world->in_progress;
    world->in_progress = true;
    return in_progress;
}

bool ecs_staging_end(
    ecs_world_t *world,
    bool is_staged)
{
    bool result = world->in_progress;

    if (!is_staged) {
        world->in_progress = false;
        if (world->auto_merge) {
            ecs_merge(world);
        }
    }

    return result;
}

const ecs_world_info_t* ecs_get_world_info(
    ecs_world_t *world)
{
    return &world->stats;
}

/* Get entity */
ecs_record_t* ecs_ei_get(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        return ecs_map_get(
            entity_index->hi, ecs_record_t, entity);
    } else {
        return ecs_sparse_get_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity);
    }
}

/* Get or create entity */
ecs_record_t* ecs_ei_get_or_create(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    if (entity > ECS_HI_ENTITY_ID) {
        ecs_record_t *record =  ecs_map_get(
            entity_index->hi, ecs_record_t, entity);

        if (!record) {
            ecs_record_t new_record = { 0 };
            ecs_map_set(
                entity_index->hi, entity, &new_record);

            record = ecs_map_get(
                entity_index->hi, ecs_record_t, entity);  

            record->table = NULL;  
            record->row = 0;            
        }

        return record;
    } else {
        bool is_new = false;

        ecs_record_t *record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity, &is_new);

        if (is_new) {
            record->table = NULL;
            record->row = 0;
        }
        
        return record;
    }
}

/* Set entity */
ecs_record_t* ecs_ei_set(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        ecs_map_set(entity_index->hi, entity, record);
    } else {
        bool is_new;
        ecs_record_t *dst_record = ecs_sparse_get_or_set_sparse(
            entity_index->lo, ecs_record_t, (int32_t)entity, &is_new);
        *dst_record = *record;

        /* Only return record ptrs of the sparse set, as these pointers are
         * stable. Tables store pointers to records only of they are stable */
        return dst_record;
    }

    return NULL;
}

/* Delete entity */
void ecs_ei_delete(
    ecs_ei_t *entity_index,
    ecs_entity_t entity)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (entity > ECS_HI_ENTITY_ID) {
        ecs_map_remove(entity_index->hi, entity);
    } else {
        ecs_sparse_remove(entity_index->lo, ecs_record_t, (int32_t)entity);
    }
}

void ecs_ei_clear_entity(
    ecs_ei_t *entity_index,
    ecs_entity_t entity,
    bool is_watched)
{
    if (entity > ECS_HI_ENTITY_ID) {
        if (!is_watched) {
            ecs_map_remove(entity_index->hi, entity);
        } else {
            ecs_ei_set(entity_index, entity, &(ecs_record_t){
                .table = NULL,
                .row = -1
            });
        }
    } else {
        ecs_ei_set(entity_index, entity, &(ecs_record_t){
            .table = NULL,
            .row = -is_watched
        });
    }    
}

ecs_entity_t ecs_ei_recycle(
    ecs_ei_t *entity_index)
{
    int32_t result;
    if (ecs_sparse_recycle(entity_index->lo, ecs_record_t, &result)) {
        ecs_assert(result > 0, ECS_INTERNAL_ERROR, NULL);
        return (ecs_entity_t)result; // implicit upcast
    } else {
        return 0;
    }
}

/* Grow entity idex */
void ecs_ei_grow(
    ecs_ei_t *entity_index,
    int32_t count)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t sparse_size = ecs_sparse_size(entity_index->lo);
    int32_t to_grow = count;

    if (sparse_size < ECS_HI_ENTITY_ID) {
        if (to_grow + sparse_size > ECS_HI_ENTITY_ID) {
            to_grow -= ECS_HI_ENTITY_ID - sparse_size;
        }

        ecs_sparse_grow(entity_index->lo, to_grow);

        count -= to_grow;
    }

    if (count) {   
        ecs_map_grow(entity_index->hi, count);
    }
}

/* Set size of entity index */
void ecs_ei_set_size(
    ecs_ei_t *entity_index,
    int32_t size)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    if (size > ECS_HI_ENTITY_ID) {
        ecs_sparse_set_size(entity_index->lo, ECS_HI_ENTITY_ID);
        ecs_map_set_size(entity_index->hi, size - ECS_HI_ENTITY_ID);
    } else {
        ecs_sparse_set_size(entity_index->lo, size);
        ecs_map_set_size(entity_index->hi, 0);
    }
}

/* Count number of entities in index */
int32_t ecs_ei_count(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    return ecs_map_count(entity_index->hi) + 
        ecs_sparse_count(entity_index->lo);
}

/* Create new entity index */
void ecs_ei_new(
    ecs_ei_t *entity_index)
{
    entity_index->lo = ecs_sparse_new(ecs_record_t, 0);
    entity_index->hi = ecs_map_new(ecs_record_t, 0);
}

/* Clear entities from index */
void ecs_ei_clear(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_clear(entity_index->hi);
    ecs_sparse_clear(entity_index->lo);
}

/* Free entity index */
void ecs_ei_free(
    ecs_ei_t *entity_index)
{
    ecs_assert(entity_index != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_map_free(entity_index->hi);
    ecs_sparse_free(entity_index->lo);
}

/* Copy entity index */
ecs_ei_t ecs_ei_copy(
    const ecs_ei_t *entity_index)
{
    return (ecs_ei_t){
        .hi = ecs_map_copy(entity_index->hi),
        .lo = ecs_sparse_copy(entity_index->lo)
    };
}

/* Get memory occupied by entity index */
void ecs_ei_memory(
    ecs_ei_t *entity_index,
    int32_t *allocd,
    int32_t *used)
{
    ecs_sparse_memory(entity_index->lo, allocd, used);
    ecs_map_memory(entity_index->hi, allocd, used);
}

ecs_ei_iter_t ecs_ei_iter(
    ecs_ei_t *entity_index)
{
    ecs_ei_iter_t result;
    result.index = 0;
    result.sparse_indices = ecs_sparse_indices(entity_index->lo);
    result.sparse_count = ecs_sparse_count(entity_index->lo);
    result.map_iter = ecs_map_iter(entity_index->hi);
    result.lo = entity_index->lo;
    return result;
}

/* Return next record for iterator (return NULL when end is reached) */
ecs_record_t *ecs_ei_next(
    ecs_ei_iter_t *iter,
    ecs_entity_t *entity_out)
{
    const int32_t *sparse_indices = iter->sparse_indices;

    if (sparse_indices) {
        int32_t index = iter->index;
        if (iter->index < iter->sparse_count) {
            ecs_entity_t entity = (ecs_entity_t)sparse_indices[index];
            ecs_record_t *result = ecs_sparse_get_sparse(
                    iter->lo, ecs_record_t, (int32_t)entity);
            *entity_out = entity;
            iter->index ++;
            return result;
        } else {
            iter->sparse_indices = NULL;
        }
    }

    return ecs_map_next(&iter->map_iter, ecs_record_t, entity_out);
}


ecs_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = world->stage.tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = world,
        .iter.filter = iter
    };
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_filter_iter_t *iter = &it->iter.filter;
    ecs_sparse_t *tables = iter->tables;
    int32_t count = ecs_sparse_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_vector_first(table->data, ecs_data_t);

        if (!data) {
            continue;
        }

        if (!ecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;
}

/* Add an extra element to the buffer */
static
void ecs_strbuf_grow(
    ecs_strbuf_t *b)
{
    /* Allocate new element */
    ecs_strbuf_element_embedded *e = ecs_os_malloc(sizeof(ecs_strbuf_element_embedded));
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = true;
    e->super.buf = e->buf;
    e->super.pos = 0;
    e->super.next = NULL;
}

/* Add an extra dynamic element */
static
void ecs_strbuf_grow_str(
    ecs_strbuf_t *b,
    char *str,
    char *alloc_str,
    int32_t size)
{
    /* Allocate new element */
    ecs_strbuf_element_str *e = ecs_os_malloc(sizeof(ecs_strbuf_element_str));
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = false;
    e->super.pos = size ? size : (int32_t)ecs_os_strlen(str);
    e->super.next = NULL;
    e->super.buf = str;
    e->alloc_str = alloc_str;
}

static
char* ecs_strbuf_ptr(
    ecs_strbuf_t *b)
{
    if (b->buf) {
        return &b->buf[b->current->pos];
    } else {
        return &b->current->buf[b->current->pos];
    }
}

/* Compute the amount of space left in the current element */
static
int32_t ecs_strbuf_memLeftInCurrentElement(
    ecs_strbuf_t *b)
{
    if (b->current->buffer_embedded) {
        return ECS_STRBUF_ELEMENT_SIZE - b->current->pos;
    } else {
        return 0;
    }
}

/* Compute the amount of space left */
static
int32_t ecs_strbuf_memLeft(
    ecs_strbuf_t *b)
{
    if (b->max) {
        return b->max - b->size - b->current->pos;
    } else {
        return INT_MAX;
    }
}

static
void ecs_strbuf_init(
    ecs_strbuf_t *b)
{
    /* Initialize buffer structure only once */
    if (!b->elementCount) {
        b->size = 0;
        b->firstElement.super.next = NULL;
        b->firstElement.super.pos = 0;
        b->firstElement.super.buffer_embedded = true;
        b->firstElement.super.buf = b->firstElement.buf;
        b->elementCount ++;
        b->current = (ecs_strbuf_element*)&b->firstElement;
    }
}

/* Quick custom function to copy a maxium number of characters and
 * simultaneously determine length of source string. */
static
int32_t fast_strncpy(
    char * dst,
    const char * src,
    int n_cpy,
    int n)
{
    ecs_assert(n_cpy >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(n >= 0, ECS_INTERNAL_ERROR, NULL);

    const char *ptr, *orig = src;
    char ch;

    for (ptr = src; (ptr - orig < n) && (ch = *ptr); ptr ++) {
        if (ptr - orig < n_cpy) {
            *dst = ch;
            dst ++;
        }
    }

    ecs_assert(ptr - orig < INT32_MAX, ECS_INTERNAL_ERROR, NULL);

    return (int32_t)(ptr - orig);
}

/* Append a format string to a buffer */
static
bool ecs_strbuf_vappend_intern(
    ecs_strbuf_t *b,
    const char* str,
    va_list args)
{
    bool result = true;
    va_list arg_cpy;

    if (!str) {
        return result;
    }

    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);

    if (!memLeft) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    va_copy(arg_cpy, args);
    memRequired = vsnprintf(
        ecs_strbuf_ptr(b), (size_t)(max_copy + 1), str, args);

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* If string is a format string, a new buffer of size memRequired is
         * needed to re-evaluate the format string and only use the part that
         * wasn't already copied to the previous element */
        if (memRequired <= ECS_STRBUF_ELEMENT_SIZE) {
            /* Resulting string fits in standard-size buffer. Note that the
             * entire string needs to fit, not just the remainder, as the
             * format string cannot be partially evaluated */
            ecs_strbuf_grow(b);

            /* Copy entire string to new buffer */
            ecs_os_vsprintf(ecs_strbuf_ptr(b), str, arg_cpy);

            /* Ignore the part of the string that was copied into the
             * previous buffer. The string copied into the new buffer could
             * be memmoved so that only the remainder is left, but that is
             * most likely more expensive than just keeping the entire
             * string. */

            /* Update position in buffer */
            b->current->pos += memRequired;
        } else {
            /* Resulting string does not fit in standard-size buffer.
             * Allocate a new buffer that can hold the entire string. */
            char *dst = ecs_os_malloc(memRequired + 1);
            ecs_os_vsprintf(dst, str, arg_cpy);
            ecs_strbuf_grow_str(b, dst, dst, memRequired);
        }
    } else {
        /* Buffer max has been reached */
        result = false;
    }

    va_end(arg_cpy);

    return result;
}

static
bool ecs_strbuf_append_intern(
    ecs_strbuf_t *b,
    const char* str,
    int n)
{
    bool result = true;

    if (!str) {
        return result;
    }

    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);

    if (!memLeft) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    if (n < 0) n = INT_MAX;

    memRequired = fast_strncpy(ecs_strbuf_ptr(b), str, max_copy, n);

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* Element was not large enough, but buffer still has space */
        b->current->pos += memLeftInElement;
        memRequired -= memLeftInElement;

        /* Current element was too small, copy remainder into new element */
        if (memRequired < ECS_STRBUF_ELEMENT_SIZE) {
            /* A standard-size buffer is large enough for the new string */
            ecs_strbuf_grow(b);

            /* Copy the remainder to the new buffer */
            if (n) {
                /* If a max number of characters to write is set, only a
                    * subset of the string should be copied to the buffer */
                ecs_os_strncpy(
                    ecs_strbuf_ptr(b),
                    str + memLeftInElement,
                    (size_t)memRequired);
            } else {
                ecs_os_strcpy(ecs_strbuf_ptr(b), str + memLeftInElement);
            }

            /* Update to number of characters copied to new buffer */
            b->current->pos += memRequired;
        } else {
            char *remainder = ecs_os_strdup(str + memLeftInElement);
            ecs_strbuf_grow_str(b, remainder, remainder, memRequired);
        }
    } else {
        /* Buffer max has been reached */
        result = false;
    }

    return result;
}

bool ecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* fmt,
    va_list args)
{
    bool result = ecs_strbuf_vappend_intern(
        b, fmt, args
    );

    return result;
}

bool ecs_strbuf_append(
    ecs_strbuf_t *b,
    const char* fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    bool result = ecs_strbuf_vappend_intern(
        b, fmt, args
    );
    va_end(args);

    return result;
}

bool ecs_strbuf_appendstrn(
    ecs_strbuf_t *b,
    const char* str,
    int32_t len)
{
    return ecs_strbuf_append_intern(
        b, str, len
    );
}

bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *b,
    char* str)
{
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, str, str, 0);
    return true;
}

bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *b,
    const char* str)
{
    /* Removes const modifier, but logic prevents changing / delete string */
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, (char*)str, NULL, 0);
    return true;
}

bool ecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str)
{
    return ecs_strbuf_append_intern(
        b, str, -1
    );
}

bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer)
{
    if (src_buffer->elementCount) {
        if (src_buffer->buf) {
            return ecs_strbuf_appendstr(dst_buffer, src_buffer->buf);
        } else {
            ecs_strbuf_element *e = (ecs_strbuf_element*)&src_buffer->firstElement;

            /* Copy first element as it is inlined in the src buffer */
            ecs_strbuf_appendstrn(dst_buffer, e->buf, e->pos);

            while ((e = e->next)) {
                dst_buffer->current->next = ecs_os_malloc(sizeof(ecs_strbuf_element));
                *dst_buffer->current->next = *e;
            }
        }

        *src_buffer = ECS_STRBUF_INIT;
    }

    return true;
}

char* ecs_strbuf_get(ecs_strbuf_t *b) {
    char* result = NULL;

    if (b->elementCount) {
        if (b->buf) {
            result = ecs_os_strdup(b->buf);
        } else {
            void *next = NULL;
            int32_t len = b->size + b->current->pos + 1;

            ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;

            result = ecs_os_malloc(len);
            char* ptr = result;

            do {
                ecs_os_memcpy(ptr, e->buf, e->pos);
                ptr += e->pos;
                next = e->next;
                if (e != &b->firstElement.super) {
                    if (!e->buffer_embedded) {
                        ecs_os_free(((ecs_strbuf_element_str*)e)->alloc_str);
                    }
                    ecs_os_free(e);
                }
            } while ((e = next));

            result[len - 1] = '\0';
        }
    } else {
        result = NULL;
    }

    b->elementCount = 0;

    return result;
}

void ecs_strbuf_reset(ecs_strbuf_t *b) {
    if (b->elementCount && !b->buf) {
        void *next = NULL;
        ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;
        do {
            next = e->next;
            if (e != (ecs_strbuf_element*)&b->firstElement) {
                ecs_os_free(e);
            }
        } while ((e = next));
    }

    *b = ECS_STRBUF_INIT;
}

void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator)
{
    buffer->list_sp ++;
    buffer->list_stack[buffer->list_sp].count = 0;
    buffer->list_stack[buffer->list_sp].separator = separator;

    if (list_open) {
        ecs_strbuf_appendstr(buffer, list_open);
    }
}

void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close)
{
    buffer->list_sp --;
    
    if (list_close) {
        ecs_strbuf_appendstr(buffer, list_close);
    }
}

void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer)
{
    int32_t list_sp = buffer->list_sp;
    if (buffer->list_stack[list_sp].count != 0) {
        ecs_strbuf_appendstr(buffer, buffer->list_stack[list_sp].separator);
    }
    buffer->list_stack[list_sp].count ++;
}

bool ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...)
{
    ecs_strbuf_list_next(buffer);

    va_list args;
    va_start(args, fmt);
    bool result = ecs_strbuf_vappend_intern(
        buffer, fmt, args
    );
    va_end(args);

    return result;
}

bool ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str)
{
    ecs_strbuf_list_next(buffer);
    return ecs_strbuf_appendstr(buffer, str);
}

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_SOURCE ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_ROLE '|'
#define TOK_NAME_SEP '.'
#define TOK_ANNOTATE_OPEN '['
#define TOK_ANNOTATE_CLOSE ']'

#define TOK_ANY "ANY"
#define TOK_OWNED "OWNED"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

#define TOK_ROLE_CHILDOF "CHILDOF"
#define TOK_ROLE_INSTANCEOF "INSTANCEOF"
#define TOK_ROLE_TRAIT "TRAIT"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_XOR "XOR"
#define TOK_ROLE_NOT "NOT"

#define TOK_IN "in"
#define TOK_OUT "out"
#define TOK_INOUT "inout"

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }
    return ptr;
}

/** Parse element with a dot-separated qualifier ('PARENT:Foo') */
static
char* parse_complex_elem(
    const char *name,
    const char *sig,
    int64_t column,
    char *ptr,
    ecs_sig_from_kind_t *from_kind,
    ecs_sig_oper_kind_t *oper_kind,
    ecs_entity_t *flags,
    const char * *source)
{
    char *bptr = ptr;
    if (bptr[0] == TOK_NOT) {
        *oper_kind = EcsOperNot;
        if (!bptr[1]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(name, sig, column, 
                "not must be followed by an identifier");
        }
        bptr ++;

    } else if (bptr[0] == TOK_OPTIONAL) {
        *oper_kind = EcsOperOptional;
        if (!bptr[1]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(name, sig, column, 
                "optional must be followed by an identifier");
        }
        bptr ++;
    }

    *source = NULL;

    char *src = strchr(bptr, TOK_SOURCE);
    if (src) {
        size_t token_len = ecs_to_size_t(src - bptr);
        if (bptr == src) {
            *from_kind = EcsFromEmpty;
        } else if (!strncmp(bptr, TOK_PARENT, token_len)) {
            *from_kind = EcsFromParent;
        } else if (!strncmp(bptr, TOK_SYSTEM, token_len)) {
            *from_kind = EcsFromSystem;
        } else if (!strncmp(bptr, TOK_ANY, token_len)) {
            *from_kind = EcsFromAny;
        } else if (!strncmp(bptr, TOK_OWNED, token_len)) {
            *from_kind = EcsFromOwned;
        } else if (!strncmp(bptr, TOK_SHARED, token_len)) {
            *from_kind = EcsFromShared;
        } else if (!strncmp(bptr, TOK_CASCADE, token_len)) {
            *from_kind = EcsCascade;   
        } else {
            *from_kind = EcsFromEntity;
            *source = bptr;
        }
        
        bptr = src + 1;

        if (!bptr[0]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + src - bptr,
                 "%s must be followed by an identifier", 
                 ptr);
        }
    }

    char *or = strchr(bptr, TOK_ROLE);
    if (or) {
        size_t token_len = ecs_to_size_t(or - bptr);
        if (!strncmp(bptr, TOK_ROLE_CHILDOF, token_len)) {
            *flags = ECS_CHILDOF;
        } else if (!strncmp(bptr, TOK_ROLE_INSTANCEOF, token_len)) {
            *flags = ECS_INSTANCEOF;
        } else if (!strncmp(bptr, TOK_ROLE_TRAIT, token_len)) {
            *flags = ECS_TRAIT;            
        } else if (!strncmp(bptr, TOK_ROLE_AND, token_len)) {
            *flags = ECS_AND;
        } else if (!strncmp(bptr, TOK_ROLE_OR, token_len)) {
            *flags = ECS_OR;
        } else if (!strncmp(bptr, TOK_ROLE_XOR, token_len)) {
            *flags = ECS_XOR;
        } else if (!strncmp(bptr, TOK_ROLE_NOT, token_len)) {
            *flags = ECS_NOT;
        } else {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + or - bptr,
                 "invalid flag identifier '%s'", 
                 bptr);
        }

        bptr = or + 1;

        if (!bptr[0]) {
            if (!name) {
                return NULL;
            }

            ecs_parser_error(
                name, sig, column + or - bptr,
                 "%s must be followed by an identifier", 
                 ptr);
        }        
    }

    return bptr;
}

int entity_compare(
    const void *ptr1,
    const void *ptr2)
{
    ecs_entity_t e1 = *(ecs_entity_t*)ptr1;
    ecs_entity_t e2 = *(ecs_entity_t*)ptr2;
    return (e1 > e2) - (e1 < e2);
}

static
void vec_add_entity(
    ecs_vector_t **vec,
    ecs_entity_t entity)
{
    ecs_entity_t *e = ecs_vector_add(vec, ecs_entity_t);
    *e = entity;

    /* Keep array sorted so that we can use it in type compare operations */
    ecs_vector_sort(*vec, ecs_entity_t, entity_compare);
}


/* -- Private functions -- */

static
const char* parse_annotation(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    ecs_sig_inout_kind_t *inout_kind_out)
{
    char *bptr, buffer[ECS_ANNOTATION_LENGTH_MAX + 1];
    char ch;

    ptr = skip_space(ptr);

    for (bptr = buffer; (ch = ptr[0]); ptr ++) {        
        if (ch == TOK_AND || ch == TOK_ANNOTATE_CLOSE) {
            /* Even though currently only one simultaneous annotation is 
             * useful, more annotations may be added in the future. */
            bptr[0] = '\0';

            if (!strcmp(buffer, TOK_IN)) {
                *inout_kind_out = EcsIn;
            } else if (!strcmp(buffer, TOK_OUT)) {
                *inout_kind_out = EcsOut;
            } else if (!strcmp(buffer, TOK_INOUT)) {
                *inout_kind_out = EcsInOut;
            } else {
                if (!name) {
                    return NULL;
                }

                ecs_parser_error(
                    name, sig, column, "unknown annotation '%s'", buffer);
            }

            if (ch == TOK_ANNOTATE_CLOSE) {
                break;
            } else {
                ptr = skip_space(ptr + 1);
            }

            bptr = buffer;
        } else {
            if (bptr - buffer >= ECS_ANNOTATION_LENGTH_MAX) {
                if (!name) {
                    return NULL;
                }

                ecs_parser_error(
                    name, sig, column, "annotation is too long");
            }

            bptr[0] = ch;
            bptr ++;
        }
    }

    if (!ch) {
        if (!name) {
            return NULL;
        }

        ecs_parser_error(name, sig, column,
            "annotation cannot appear at end of a column");
    }

    return ptr;
}

/** Parse type expression or signature */
int ecs_parse_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *name,
    void *ctx)
{
    ecs_size_t len = ecs_os_strlen(sig);
    ecs_assert(len > 0, ECS_INVALID_SIGNATURE, NULL);
    
    const char *ptr;
    char ch, *bptr, *buffer = ecs_os_malloc(len + 1);
    ecs_assert(buffer != NULL, ECS_OUT_OF_MEMORY, NULL);

    bool complex_expr = false;
    bool prev_is_0 = false;
    ecs_sig_from_kind_t from_kind = EcsFromOwned;
    ecs_sig_oper_kind_t oper_kind = EcsOperAnd;
    ecs_sig_inout_kind_t inout_kind = EcsInOut;
    ecs_entity_t flags = 0;
    const char *source;

    for (bptr = buffer, ch = sig[0], ptr = sig; ch; ptr++) {
        ptr = skip_space(ptr);
        ch = *ptr;

        if (prev_is_0) {
            if (!name) {
                return -1;
            }

            /* 0 can only apppear by itself */
            ecs_parser_error(
                name, sig, ptr - sig, "0 can only appear by itself");
        }

        if (ch == TOK_ANNOTATE_OPEN) {
            /* Annotations should appear at the beginning of a column */
            if (bptr != buffer) {
                if (!name) {
                    return -1;
                }

                ecs_parser_error(name, sig, ptr - sig, 
                    "[...] should appear at start of column");
            }

            ptr = parse_annotation(name, sig, ptr - sig, ptr + 1, &inout_kind);
            if (!ptr) {
                return -1;
            }

        } else if (ch == TOK_AND || (ch == TOK_OR[0] && ptr[1] == TOK_OR[1]) || ch == '\0') {
            /* Separators should not appear after an empty column */
            if (bptr == buffer) {
                if (ch) {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, "%c unexpected here", ch);
                } else {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, "unexpected end of expression");
                }
            }

            *bptr = '\0';
            bptr = buffer;

            source = NULL;

            if (complex_expr) {
                ecs_sig_oper_kind_t prev = oper_kind;
                bptr = parse_complex_elem(
                    name, sig, ptr - sig, bptr, &from_kind, &oper_kind, 
                    &flags, &source);
                if (!bptr) {
                    return -1;
                }

                if (oper_kind == EcsOperNot && prev == EcsOperOr) {
                    if (!name) {
                        return -1;
                    }

                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "cannot use ! in | expression");
                }
            }

            if (oper_kind == EcsOperOr) {
                if (from_kind == EcsFromEmpty) {
                    if (!name) {
                        return -1;
                    }

                    /* Cannot OR handles */
                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "cannot use | on columns without a source");
                }
            }

            if (!strcmp(bptr, "0")) {
                if (bptr != buffer) {
                    if (!name) {
                        return -1;
                    }

                    /* 0 can only appear by itself */
                    ecs_parser_error(
                        name, sig, ptr - sig, 
                        "0 can only appear by itself");
                }

                from_kind = EcsFromEmpty;
                prev_is_0 = true;
            }

            /* If retrieving a component from a system, only the AND operator is
             * supported. The set of system components is expected to be 
             * constant, and thus no conditional operators are needed. */
            if (from_kind == EcsFromSystem && oper_kind != EcsOperAnd) {
                if (!name) {
                    return -1;
                }

                ecs_parser_error(name, sig, ptr - sig,
                    "invalid operator for SYSTEM column");
            }     

            char *source_id = NULL;
            if (source) {
                char *src = strchr(source, TOK_SOURCE);
                source_id = ecs_os_malloc(ecs_to_i32(src - source + 1));
                ecs_assert(source_id != NULL, ECS_OUT_OF_MEMORY, NULL);

                ecs_os_strncpy(source_id, source, ecs_to_i32(src - source));
                source_id[src - source] = '\0';
            }

            if (action(world, name, sig, ptr - sig, 
                from_kind, oper_kind, inout_kind, flags, bptr, source_id, ctx)) 
            {
                if (!name) {
                    return -1;
                }
                
                ecs_abort(ECS_INVALID_SIGNATURE, sig);
            }

            if (source_id) {
                ecs_os_free(source_id);
            }

            /* Reset variables for next column */
            complex_expr = false;
            from_kind = EcsFromOwned;
            oper_kind = EcsOperAnd;
            flags = 0;

            if (ch == TOK_ROLE) {
                if (ptr[1] == TOK_OR[1]) {
                    ptr ++;
                    if (oper_kind == EcsOperNot) {
                        if (!name) {
                            return -1;
                        }

                        ecs_parser_error(name, sig, ptr - sig, 
                            "cannot use ! in | expression");
                    }
                    oper_kind = EcsOperOr;
                }
            }

            inout_kind = EcsInOut;

            bptr = buffer;
        } else {
            *bptr = ch;
            bptr ++;

            if (ch == TOK_SOURCE || ch == TOK_NOT || ch == TOK_OPTIONAL || ch == TOK_ROLE) {
                complex_expr = true;
            }
        }
    }

    ecs_os_free(buffer);
    return 0;
}

/** Parse callback that adds component to the components array for a system */
int ecs_parse_signature_action(
    ecs_world_t *world,
    const char *system_id,
    const char *expr,
    int64_t column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *component_id,
    const char *source_id,
    void *data)
{
    ecs_sig_t *sig = data;

    ecs_assert(sig != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Lookup component handly by string identifier */
    ecs_entity_t component = ecs_lookup_fullpath(world, component_id);
    if (!component) {
        /* "0" is a valid expression used to indicate that a system matches no
         * components */
        if (strcmp(component_id, "0")) {
            ecs_parser_error(system_id, expr, column, 
                "unresolved component identifier '%s'", component_id);
        } else {
            /* No need to add 0 component to signature */
            return 0;
        }
    }

    component |= flags;

    ecs_entity_t source = 0;
    if (from_kind == EcsFromEntity) {
        if (from_kind == EcsFromEntity) {
            source = ecs_lookup_fullpath(world, source_id);
            if (!source) {
                ecs_parser_error(system_id, expr, column, 
                    "unresolved source identifier '%s'", source_id);
            }
        }
    }

    return ecs_sig_add(
        sig, from_kind, oper_kind, inout_kind, component, source);
}

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig)
{
    if (expr && ecs_os_strlen(expr)) {
        sig->expr = ecs_os_strdup(expr);
    } else {
        sig->expr = ecs_os_strdup("0");
    }

    ecs_parse_expr(
        world, sig->expr, ecs_parse_signature_action, name, sig);
}

int ecs_sig_add(
    ecs_sig_t *sig,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t component,
    ecs_entity_t source)
{
    ecs_sig_column_t *elem;

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind != EcsOperOr) {
        elem = ecs_vector_add(&sig->columns, ecs_sig_column_t);
        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
        elem->inout_kind = inout_kind;
        elem->is.component = component;
        elem->source = source;

    /* OR columns store a type id instead of a single component */
    } else {
        ecs_assert(inout_kind != EcsOut, ECS_INVALID_SIGNATURE, NULL);
        elem = ecs_vector_last(sig->columns, ecs_sig_column_t);

        if (elem->oper_kind == EcsOperAnd) {
            ecs_entity_t prev = elem->is.component;
            elem->is.type = NULL;
            vec_add_entity(&elem->is.type, prev);
            vec_add_entity(&elem->is.type, component);
        } else {
            if (elem->from_kind != from_kind) {
                /* Cannot mix FromEntity and FromComponent in OR */
                ecs_parser_error(sig->name, sig->expr, 0, 
                    "cannot mix source kinds in || expression");
                goto error;
            }

            if (elem->oper_kind != EcsOperAnd && elem->oper_kind != EcsOperOr) {
                ecs_parser_error(sig->name, sig->expr, 0, 
                    "cannot mix operators in || expression");                
            }

            vec_add_entity(&elem->is.type, component);
        }

        elem->from_kind = from_kind;
        elem->oper_kind = oper_kind;
    }

    return 0;
error:
    return -1;

}

void ecs_sig_deinit(
    ecs_sig_t *sig)
{   
    ecs_vector_each(sig->columns, ecs_sig_column_t, column, {
        if (column->oper_kind == EcsOperOr) {
            ecs_vector_free(column->is.type);
        }
    });

    ecs_vector_free(sig->columns);
    ecs_os_free(sig->expr);
}

/* Check if system meets constraints of non-table columns */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig)
{
    ecs_vector_each(sig->columns, ecs_sig_column_t, elem, {
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        if (from_kind == EcsFromEntity) {
            ecs_type_t type = ecs_get_type(world, elem->source);
            if (ecs_type_has_entity(world, type, elem->is.component)) {
                if (oper_kind == EcsOperNot) {
                    return false;
                }
            } else {
                if (oper_kind != EcsOperNot) {
                    return false;
                }
            }
        }
    });

    return true;
}

static
ecs_entity_t split_entity_id(
    ecs_entity_t id,
    ecs_entity_t *entity)
{
    *entity = (id & ECS_ENTITY_MASK);
    return id;
}

ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    /* Walk from back to front, as prefabs are always located
     * at the end of the type. */
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = array[i];

        if (e & ECS_INSTANCEOF) {
            ecs_entity_t prefab = e & ECS_ENTITY_MASK;
            ecs_type_t prefab_type = ecs_get_type(world, prefab);

            if (prefab == previous) {
                continue;
            }

            if (ecs_type_owns_entity(
                world, prefab_type, component, true)) 
            {
                return prefab;
            } else {
                prefab = ecs_find_entity_in_prefabs(
                    world, prefab, prefab_type, component, entity);
                if (prefab) {
                    return prefab;
                }
            }
        } else {
            /* If this is not a prefab, the following entities won't
                * be prefabs either because the array is sorted, and
                * the prefab bit is 2^63 which ensures that prefabs are
                * guaranteed to be the last entities in the type */
            break;
        }
    }

    return 0;
}

/* -- Private functions -- */

ecs_type_t ecs_type_find_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *array,
    int32_t count)
{
    ecs_entities_t entities = {
        .array = array,
        .count = count
    };

    ecs_table_t *table = ecs_table_find_or_create(world, stage, &entities);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Extend existing type with additional entity */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_add(world, stage, table, &entities, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Remove entity from type */
ecs_type_t ecs_type_remove_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_table_t *table = ecs_table_from_type(world, stage, type);

    ecs_entities_t entities = {
        .array = &e,
        .count = 1
    };

    table = ecs_table_traverse_remove(world, stage, table, &entities, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_type_t type_1,
    ecs_type_t type_2,
    bool match_all,
    bool match_prefab)
{
    if (!type_1) {
        return 0;
    }

    ecs_assert(type_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_1 == type_2) {
        return *(ecs_vector_first(type_1, ecs_entity_t));
    }

    int32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1, ecs_entity_t);
    ecs_entity_t *t2_array = ecs_vector_first(type_2, ecs_entity_t);

    ecs_assert(t1_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t2_array != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t t1_count = ecs_vector_count(type_1);
    int32_t t2_count = ecs_vector_count(type_2);

    for (i_2 = 0; i_2 < t2_count; i_2 ++) {
        ecs_entity_t e2 = t2_array[i_2];

        if (i_1 >= t1_count) {
            return 0;
        }

        e1 = t1_array[i_1];

        if (e2 > e1) {
            do {
                i_1 ++;
                if (i_1 >= t1_count) {
                    return 0;
                }
                e1 = t1_array[i_1];
            } while (e2 > e1);
        }

        if (e1 != e2) {
            if (match_prefab && e2 != 
                ecs_entity(EcsName) && e2 != 
                EcsPrefab && e2 != 
                EcsDisabled) 
            {
                if (ecs_find_entity_in_prefabs(world, 0, type_1, e2, 0)) {
                    e1 = e2;
                }
            }

            if (e1 != e2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return e1;
            }
        } else {
            if (!match_all) return e1;
            i_1 ++;
            if (i_1 < t1_count) {
                e1 = t1_array[i_1];
            }
        }
    }

    if (match_all) {
        return e1;
    } else {
        return 0;
    }
}

/* -- Public API -- */

int32_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == entity) {
            return c_ptr_i; 
        }
    });

    return -1;
}

ecs_type_t ecs_type_merge_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_table_t *table = ecs_table_from_type(world, stage, type);
    ecs_entities_t add_array = ecs_type_to_entities(to_add);
    ecs_entities_t remove_array = ecs_type_to_entities(to_remove);
    
    table = ecs_table_traverse_remove(
        world, stage, table, &remove_array, NULL); 

    table = ecs_table_traverse_add(
        world, stage, table, &add_array, NULL); 

    if (!table) {
        return NULL;
    } else {
        return table->type;
    }
}

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_merge_intern(world, stage, type, to_add, to_remove);
}

ecs_type_t ecs_type_find(
    ecs_world_t *world,
    ecs_entity_t *array,
    int32_t count)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_find_intern(world, stage, array, count);
}

bool ecs_type_has_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    ecs_entity_t trait = 0;

    if (!entity) {
        return true;
    }

    if (entity & ECS_TRAIT) {
        trait = entity & ECS_ENTITY_MASK;
    }

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        ecs_entity_t e = *c_ptr;
        if (e == entity) {
            return true;
        }

        if (e & ECS_INSTANCEOF && entity != EcsPrefab && entity != EcsDisabled){
            ecs_entity_t base = e & ECS_ENTITY_MASK;
            if (ecs_has_entity(world, base, entity)) {
                return true;
            }
        } else 
        if (trait && e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (trait == ecs_entity_t_hi(e)) {
                return true;
            }
        }
    });

    return false;
}

bool ecs_type_owns_entity(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    bool is_trait = false;

    if (!type) {
        return false;
    }

    if (entity & ECS_TRAIT) {
        is_trait = true;
        entity = entity & ECS_ENTITY_MASK;
    }
    
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    int i, count = ecs_vector_count(type);

    if (owned) {
        if (is_trait) {
             for (i = 0; i < count; i ++) {
                 ecs_entity_t e = array[i];
                 if (e & ECS_TRAIT) {
                     e &= ECS_ENTITY_MASK;
                     if (ecs_entity_t_hi(e) == entity) {
                         return true;
                     }
                 }
             }
        } else {
            ecs_entity_t e = array[0];
            for (i = 0; i < count && entity != e && e < entity; i ++) {
                e = array[i];
            }
            return e == entity;
        }
    } else {
        for (i = count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (e < ECS_INSTANCEOF) {
                return false;
            } else
            if (e & ECS_INSTANCEOF) {
                ecs_entity_t base = e & ECS_ENTITY_MASK;
                if (ecs_has_entity(world, base, entity)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool ecs_type_has_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has)
{
    return ecs_type_contains(world, type, has, true, false) != 0;
}

bool ecs_type_owns_type(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return ecs_type_contains(world, type, has, true, !owned) != 0;
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_add_intern(world, stage, type, e);
}

ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return ecs_type_remove_intern(world, stage, type, e);
}

static
void append_name(
    ecs_world_t *world,
    ecs_vector_t **chbuf,
    ecs_entity_t h)
{
    const char *str = NULL;
    char *dst;

    if (h == 1) {
        /* Prevent issues during bootstrap */
        str = "EcsComponent";
    } else {
        str = ecs_get_fullpath(world, h);
    }

    ecs_assert(str != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t len = ecs_os_strlen(str);
    dst = ecs_vector_addn(chbuf, char, len);
    ecs_os_memcpy(dst, str, len);
    if (h != 1) {
        ecs_os_free((char*)str);
    }
}

char* ecs_type_str(
    ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;

    ecs_entity_t *handles = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t h;
        ecs_entity_t trait = 0;
        ecs_entity_t flags = split_entity_id(handles[i], &h) & ECS_TYPE_ROLE_MASK;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (flags & ECS_INSTANCEOF) {
            int len = sizeof("INSTANCEOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "INSTANCEOF|", len);
        }

        if (flags & ECS_CHILDOF) {
            int len = sizeof("CHILDOF|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "CHILDOF|", len);
        }

        if (flags & ECS_TRAIT) {
            int len = sizeof("TRAIT|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "TRAIT|", len);
            trait = ecs_entity_t_hi(h);
            h = ecs_entity_t_lo(h);
        }

        if (flags & ECS_XOR) {
            int len = sizeof("XOR|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "XOR|", len);
        }

        if (flags & ECS_OR) {
            int len = sizeof("OR|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "OR|", len);
        }

        if (flags & ECS_AND) {
            int len = sizeof("AND|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "AND|", len);
        }

        if (flags & ECS_NOT) {
            int len = sizeof("NOT|") - 1;
            dst = ecs_vector_addn(&chbuf, char, len);
            ecs_os_memcpy(dst, "NOT|", len);
        }

        append_name(world, &chbuf, h);

        if (trait) {
            char *ch = ecs_vector_add(&chbuf, char);
            *ch = '<';
            append_name(world, &chbuf, trait);
        }
    }

    *(char*)ecs_vector_add(&chbuf, char) = '\0';

    char* result = ecs_os_strdup(ecs_vector_first(chbuf, char));
    ecs_vector_free(chbuf);
    return result;
}

ecs_entity_t ecs_type_get_entity_for_xor(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t xor)
{
    ecs_assert(
        ecs_type_owns_entity(world, type, ECS_XOR | xor, true),
        ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get(world, xor, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t xor_type = type_ptr->normalized;
    ecs_assert(xor_type != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        if (ecs_type_owns_entity(world, xor_type, array[i], true)) {
            return array[i];
        }
    }

    return 0;
}

int32_t ecs_type_trait_index_of(
    ecs_type_t type, 
    int32_t start_index, 
    ecs_entity_t trait)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    for (i = start_index; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (trait == ecs_entity_t_hi(e)) {
                return i;
            }
        }
    }

    return -1;
}

static bool ecs_os_api_initialized = false;

ecs_os_api_t ecs_os_api;

int64_t ecs_os_api_malloc_count = 0;
int64_t ecs_os_api_realloc_count = 0;
int64_t ecs_os_api_calloc_count = 0;
int64_t ecs_os_api_free_count = 0;

void ecs_os_set_api(
    ecs_os_api_t *os_api)
{
    if (!ecs_os_api_initialized) {
        ecs_os_api = *os_api;
        ecs_os_api_initialized = true;
    }
}

static
void ecs_log(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_error(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

static
void ecs_log_debug(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_warning(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void ecs_os_dbg(const char *fmt, ...) {
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_debug) {
        ecs_os_api.log_debug(fmt, args);
    }
    va_end(args);
#else
    (void)fmt;
#endif
}

void ecs_os_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_warning) {
        ecs_os_api.log_warning(fmt, args);
    }
    va_end(args);
}

void ecs_os_log(const char *fmt, ...) {
    va_list args;;
    va_start(args, fmt);
    if (ecs_os_api.log) {
        ecs_os_api.log(fmt, args);
    }
    va_end(args);
}

void ecs_os_err(const char *fmt, ...) {
    va_list args;;
    va_start(args, fmt);
    if (ecs_os_api.log_error) {
        ecs_os_api.log_error(fmt, args);
    }
    va_end(args);
}

void ecs_os_gettime(ecs_time_t *time)
{
    uint64_t now = ecs_os_time_now();
    uint64_t sec = now / 1000000000;

    assert(sec < UINT32_MAX);
    assert((now - sec * 1000000000) < UINT32_MAX);

    time->sec = (uint32_t)sec;
    time->nanosec = (uint32_t)(now - sec * 1000000000);
}

static
void* ecs_os_api_malloc(ecs_size_t size) {
    ecs_os_api_malloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return malloc((size_t)size);
}

static
void* ecs_os_api_calloc(ecs_size_t size) {
    ecs_os_api_calloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return calloc(1, (size_t)size);
}

static
void* ecs_os_api_realloc(void *ptr, ecs_size_t size) {
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_os_api_realloc_count ++;
    } else {
        /* If not actually reallocing, treat as malloc */
        ecs_os_api_malloc_count ++; 
    }
    
    return realloc(ptr, (size_t)size);
}

static
void ecs_os_api_free(void *ptr) {
    if (ptr) {
        ecs_os_api_free_count ++;
    }
    free(ptr);
}

static
char* ecs_os_api_strdup(const char *str) {
    int len = ecs_os_strlen(str);
    char *result = ecs_os_api_malloc(len + 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_os_strcpy(result, str);
    return result;
}

/* Replace dots with underscores */
static
char *module_file_base(const char *module, char sep) {
    char *base = ecs_os_strdup(module);
    ecs_size_t i, len = ecs_os_strlen(base);
    for (i = 0; i < len; i ++) {
        if (base[i] == '.') {
            base[i] = sep;
        }
    }

    return base;
}

static
char* ecs_os_api_module_to_dl(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with underscores + OS library extension */
    char *file_base = module_file_base(module, '_');

#if defined(ECS_OS_LINUX)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".so");
#elif defined(ECS_OS_DARWIN)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dylib");
#elif defined(ECS_OS_WINDOWS)
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dll");
#endif

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

static
char* ecs_os_api_module_to_etc(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with dashes + /etc */
    char *file_base = module_file_base(module, '-');

    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, "/etc");

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

void ecs_os_api_impl(ecs_os_api_t *api);

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized) {
        return;
    }

    ecs_os_time_setup();
    
    /* Memory management */
    ecs_os_api.malloc = ecs_os_api_malloc;
    ecs_os_api.free = ecs_os_api_free;
    ecs_os_api.realloc = ecs_os_api_realloc;
    ecs_os_api.calloc = ecs_os_api_calloc;

    /* Strings */
    ecs_os_api.strdup = ecs_os_api_strdup;

    ecs_os_api_impl(&ecs_os_api);

    /* Time */
    ecs_os_api.sleep = ecs_os_time_sleep;
    ecs_os_api.get_time = ecs_os_gettime;

    /* Logging */
    ecs_os_api.log = ecs_log;
    ecs_os_api.log_error = ecs_log_error;
    ecs_os_api.log_debug = ecs_log_debug;
    ecs_os_api.log_warning = ecs_log_warning;

    /* Modules */
    if (!ecs_os_api.module_to_dl) {
        ecs_os_api.module_to_dl = ecs_os_api_module_to_dl;
    }

    if (!ecs_os_api.module_to_etc) {
        ecs_os_api.module_to_etc = ecs_os_api_module_to_etc;
    }

    ecs_os_api.abort = abort;
}

#ifdef FLECS_SYSTEMS_H
#ifndef FLECS_SYSTEM_PRIVATE_H
#define FLECS_SYSTEM_PRIVATE_H


typedef struct EcsSystem {
    ecs_iter_action_t action;       /* Callback to be invoked for matching it */
    void *ctx;                      /* Userdata for system */

    ecs_entity_t entity;                  /* Entity id of system, used for ordering */
    ecs_query_t *query;                   /* System query */
    ecs_on_demand_out_t *on_demand;       /* Keep track of [out] column refs */
    ecs_system_status_action_t status_action; /* Status action */
    void *status_ctx;                     /* User data for status action */    
    ecs_entity_t tick_source;             /* Tick source associated with system */
    
    int32_t invoke_count;                 /* Number of times system is invoked */
    float time_spent;                     /* Time spent on running system */
    float time_passed;                    /* Time passed since last invocation */
    bool has_out_columns;                 /* True if system has out columns */
} EcsSystem;

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate);

/* Internal function to run a system */
ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param,
    bool ran_by_app);

#endif
#endif

static
ecs_entity_t components_contains(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    ecs_vector_each(table_type, ecs_entity_t, c_ptr, {
        ecs_entity_t entity = *c_ptr;

        if (entity & ECS_CHILDOF) {
            entity &= ECS_ENTITY_MASK;

            ecs_record_t *record = ecs_eis_get(&world->stage, entity);
            ecs_assert(record != 0, ECS_INTERNAL_ERROR, NULL);

            if (record->table) {
                ecs_entity_t component = ecs_type_contains(
                    world, record->table->type, type, match_all, true);

                if (component) {
                    if (entity_out) *entity_out = entity;
                    return component;
                }
            }
        }
    });

    return 0;
}

/* Get actual entity on which specified component is stored */
static
ecs_entity_t get_entity_for_component(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component)
{
    if (entity) {
        ecs_record_t *record = ecs_eis_get(&world->stage, entity);
        ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        if (record->table) {
            type = record->table->type;
        } else {
            type = NULL;
        }
    }

    ecs_vector_each(type, ecs_entity_t, c_ptr, {
        if (*c_ptr == component) {
            return entity;
        }
    });

    return ecs_find_entity_in_prefabs(world, entity, type, component, 0);
}

#ifndef NDEBUG
static
ecs_entity_t get_cascade_component(
    ecs_query_t *query)
{
    ecs_sig_column_t *column = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    return column[query->cascade_by - 1].is.component;
}
#endif

static
int32_t rank_by_depth(
    ecs_world_t *world,
    ecs_entity_t rank_by_component,
    ecs_type_t type)
{
    int32_t result = 0;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    for (i = count - 1; i >= 0; i --) {
        if (array[i] & ECS_CHILDOF) {
            ecs_type_t c_type = ecs_get_type(world, array[i] & ECS_ENTITY_MASK);
            int32_t j, c_count = ecs_vector_count(c_type);
            ecs_entity_t *c_array = ecs_vector_first(c_type, ecs_entity_t);

            for (j = 0; j < c_count; j ++) {
                if (c_array[j] == rank_by_component) {
                    result ++;
                    result += rank_by_depth(world, rank_by_component, c_type);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }
        } else if (!(array[i] & ECS_TYPE_ROLE_MASK)) {
            /* No more parents after this */
            break;
        }
    }

    return result;
}

static
int table_compare(
    const void *t1,
    const void *t2)
{
    const ecs_matched_table_t *table_1 = t1;
    const ecs_matched_table_t *table_2 = t2;

    return table_1->rank - table_2->rank;
}

static
void order_ranked_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_table) {
        ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);       
    }

    /* Re-register monitors after tables have been reordered. This will update
     * the table administration with the new matched_table ids, so that when a
     * monitor is executed we can quickly find the right matched_table. */
    if (query->flags & EcsQueryMonitor) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            ecs_table_register_query(
                world, table->table, query, table_i);
        });
    }

    query->match_count ++; 
}

static
void group_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_matched_table_t *table)
{
    if (query->group_table) {
        table->rank = query->group_table(
            world, query->rank_on_component, table->table->type);
    } else {
        table->rank = 0;
    }
}

/* Rank all tables of query. Only necessary if a new ranking function was
 * provided or if a monitored entity set the component used for ranking. */
static
void group_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_table) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });

        ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });              
    }
}

static
bool has_auto_activation(
    ecs_query_t *q)
{
    /* Only a basic query with no additional features does table activation */
    return !(q->flags & EcsQueryNoActivation);
}

#ifndef NDEBUG

static
const char* query_name(
    ecs_world_t *world,
    ecs_query_t *q)
{
    if (q->system) {
        return ecs_get_name(world, q->system);
    } else {
        return q->sig.expr;
    }
}

#endif

static
void get_comp_and_src(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_type_t table_type,
    ecs_sig_column_t *column,
    ecs_sig_oper_kind_t op,
    ecs_sig_from_kind_t from,
    ecs_entity_t *component_out,
    ecs_entity_t *entity_out)
{
    ecs_entity_t component = 0, entity = 0;

    if (op == EcsOperNot) {
        entity = column->source;
    }

    /* Column that retrieves data from self or a fixed entity */
    if (from == EcsFromAny || from == EcsFromEntity || 
        from == EcsFromOwned || from == EcsFromShared) 
    {
        if (op == EcsOperAnd || op == EcsOperNot) {
            component = column->is.component;
        } else if (op == EcsOperOptional) {
            component = column->is.component;
        } else if (op == EcsOperOr) {
            component = ecs_type_contains(
                world, table_type, column->is.type, 
                false, true);
        }

        if (from == EcsFromEntity) {
            entity = column->source;
        }

    /* Column that just passes a handle to the system (no data) */
    } else if (from == EcsFromEmpty) {
        component = column->is.component;

    /* Column that retrieves data from a dynamic entity */
    } else if (from == EcsFromParent || from == EcsCascade) {
        if (op == EcsOperAnd ||
            op == EcsOperOptional)
        {
            component = column->is.component;
            entity = ecs_find_in_type(
                world, table_type, component, ECS_CHILDOF);

        } else if (op == EcsOperOr) {
            component = components_contains(
                world,
                table_type,
                column->is.type,
                &entity,
                false);
        }

    /* Column that retrieves data from a system */
    } else if (from == EcsFromSystem) {
        if (op == EcsOperAnd) {
            component = column->is.component;
        }

        entity = query->system;
    }

    *component_out = component;
    *entity_out = entity;
}

static
int32_t get_component_index(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t *component_out,
    int32_t column_index,
    ecs_sig_oper_kind_t op,
    int32_t *trait_index_offsets)
{
    int32_t result = 0;
    ecs_entity_t component = *component_out;

    if (component) {
        if (component & ECS_TRAIT) {
            ecs_assert(trait_index_offsets != NULL, ECS_INTERNAL_ERROR, NULL);

            component &= ECS_ENTITY_MASK;

            result = ecs_type_trait_index_of(table_type, 
                trait_index_offsets[column_index], component);

            if (result != -1) {
                trait_index_offsets[column_index] = result + 1;

                /* If component of current column is a trait, get the actual trait
                 * type for the table, so the system can see which component the
                 * trait was applied to */   
                ecs_entity_t *trait = ecs_vector_get(
                    table_type, ecs_entity_t, result);
                *component_out = *trait;

                /* Check if the trait is a tag or whether it has data */
                if (ecs_get(world, component, EcsComponent) == NULL) {
                    /* If trait has no data associated with it, use the
                     * component to which the trait has been added */
                    component = ecs_entity_t_lo(*trait);
                }
            }
        } else {
            /* Get column index for component */
            result = ecs_type_index_of(table_type, component);
        }

        /* If column is found, add one to the index, as column zero in
        * a table is reserved for entity id's */
        if (result != -1) {
            result ++;

            /* Check if component is a tag. If it is, set table_data to
            * zero, so that a system won't try to access the data */
            const EcsComponent *data = ecs_get(
                world, component, EcsComponent);

            if (!data || !data->size) {
                result = 0;
            }
        }
        
        /* ecs_table_column_offset may return -1 if the component comes
        * from a prefab. If so, the component will be resolved as a
        * reference (see below) */           
    }

    if (op == EcsOperOptional) {
        /* If table doesn't have the field, mark it as no data */
        if (!ecs_type_has_entity(
            world, table_type, component))
        {
            result = 0;
        }
    }

    return result;
}

static
void add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_type_t table_type,
    int32_t column_index,
    ecs_matched_table_t *table_data,
    ecs_entity_t component,
    ecs_entity_t entity,
    ecs_sig_from_kind_t from)
{
    const EcsComponent *c_info = ecs_get(world, component, EcsComponent);
    
    ecs_entity_t e;
    ecs_ref_t *ref = ecs_vector_add(
            &table_data->references, ecs_ref_t);
    
    table_data->columns[column_index] = -ecs_vector_count(table_data->references);

    /* Find the entity for the component */
    if (from == EcsFromEntity || from == EcsFromEmpty) {
        e = entity;
    } else if (from == EcsCascade) {
        e = entity;
    } else if (from == EcsFromSystem) {
        e = entity;
    } else {
        e = get_entity_for_component(
            world, entity, table_type, component);
    }

    if (from != EcsCascade) {
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
    }
    
    *ref = (ecs_ref_t){0};
    ref->entity = e;
    ref->component = component;

    if (ecs_has(world, component, EcsComponent)) {
        if (c_info->size && from != EcsFromEmpty) {
            if (e) {
                ecs_get_ref_w_entity(
                    world, ref, e, component);
                ecs_set_watch(world, &world->stage, e);                     
            }

            query->flags |= EcsQueryHasRefs;
        }
    }
}

static
ecs_entity_t is_column_trait(
    ecs_sig_column_t *column)
{
    ecs_sig_from_kind_t from_kind = column->from_kind;
    ecs_sig_oper_kind_t oper_kind = column->oper_kind;

    /* For now traits are only supported on owned columns */
    if (from_kind == EcsFromOwned && oper_kind == EcsOperAnd) {
        if (column->is.component & ECS_TRAIT) {
            return column->is.component;
        }
    }

    return 0;
}

static
int32_t type_trait_count(
    ecs_type_t type,
    ecs_entity_t trait)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t result = 0;

    trait &= ECS_ENTITY_MASK;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (e & ECS_TRAIT) {
            e &= ECS_ENTITY_MASK;
            if (ecs_entity_t_hi(e) == trait) {
                result ++;
            }
        }
    }

    return result;
}

/* For each trait that the query subscribes for, count the occurrences in the
 * table. Cardinality of subscribed for traits must be the same as in the table
 * or else the table won't match. */
static
int32_t count_traits(
    ecs_query_t *query,
    ecs_type_t type)
{
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, count = ecs_vector_count(query->sig.columns);
    int32_t first_count = 0, trait_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_entity_t trait = is_column_trait(&columns[i]);
        if (trait) {
            trait_count = type_trait_count(type, trait);
            if (!first_count) {
                first_count = trait_count;
            } else {
                if (first_count != trait_count) {
                    /* The traits that this query subscribed for occur in the
                     * table but don't have the same cardinality. Ignore the
                     * table. This could typically happen for empty tables along
                     * a path in the table graph. */
                    return -1;
                }
            }
        }
    }

    return first_count;
}

/** Add table to system, compute offsets for system components in table it */
static
void add_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_matched_table_t *table_data;
    ecs_type_t table_type = NULL;
    int32_t c, column_count = ecs_vector_count(query->sig.columns);

    if (table) {
        table_type = table->type;
    }

    int32_t trait_cur = 0, trait_count = count_traits(query, table_type);
    
    /* If the query has traits, we need to account for the fact that a table may
     * have multiple components to which the trait is applied, which means the
     * table has to be registered with the query multiple times, with different
     * table columns. If so, allocate a small array for each trait in which the
     * last added table index of the trait is stored, so that in the next 
     * iteration we can start the search from the correct offset type. */
    int32_t *trait_index_offsets = NULL;
    if (trait_count) {
        trait_index_offsets = ecs_os_calloc(ECS_SIZEOF(int32_t) * column_count);
    }


    /* From here we recurse */
add_trait:

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    if (table && has_auto_activation(query)) {
        table_type = table->type;
        table_data = ecs_vector_add(&query->empty_tables, ecs_matched_table_t);

        #ifndef NDEBUG
        char *type_expr = ecs_type_str(world, table->type);
        ecs_trace_2("query #[green]%s#[reset] matched with table #[green][%s]",
            query_name(world, query), type_expr);
        ecs_os_free(type_expr);
        #endif
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_data = ecs_vector_add(
            &query->tables, ecs_matched_table_t);
    }

    table_data->table = table;
    table_data->references = NULL;
    table_data->columns = NULL;
    table_data->components = NULL;
    table_data->monitor = NULL;

    /* If grouping is enabled for query, assign the group rank to the table */
    group_table(world, query, table_data);

    if (column_count) {
        /* Array that contains the system column to table column mapping */
        table_data->columns = ecs_os_malloc(ECS_SIZEOF(int32_t) * column_count);
        ecs_assert(table_data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Store the components of the matched table. In the case of OR expressions,
        * components may differ per matched table. */
        table_data->components = ecs_os_malloc(ECS_SIZEOF(ecs_entity_t) * column_count);
        ecs_assert(table_data->components != NULL, ECS_OUT_OF_MEMORY, NULL);
    }

    /* Walk columns parsed from the system signature */
    ecs_sig_column_t *columns = ecs_vector_first(
        query->sig.columns, ecs_sig_column_t);

    for (c = 0; c < column_count; c ++) {
        ecs_sig_column_t *column = &columns[c];
        ecs_entity_t entity = 0, component = 0;
        ecs_sig_oper_kind_t op = column->oper_kind;
        ecs_sig_from_kind_t from = column->from_kind;

        if (op == EcsOperNot) {
            from = EcsFromEmpty;
        }

        table_data->columns[c] = 0;

        /* Get actual component and component source for current column */
        get_comp_and_src(world, query, table_type, column, op, from, &component, 
            &entity);

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromParent) and is not just a handle */
        if (!entity && from != EcsFromEmpty) {
            int32_t index = get_component_index(
                world, table_type, &component, c, op, trait_index_offsets);
            
            table_data->columns[c] = index;
        }

        /* Check if a the component is a reference. If 'entity' is set, the
         * component must be resolved from another entity, which is the case
         * for FromEntity and FromContainer. 
         * 
         * If no entity is set but the component is not found in the table, it
         * must come from a prefab. This is guaranteed, as at this point it is
         * already validated that the table matches with the system.
         * 
         * If the column from is Cascade, there may not be an entity in case the
         * current table contains root entities. In that case, still add a
         * reference field. The application can, after the table has matched,
         * change the set of components, so that this column will turn into a
         * reference. Having the reference already linked to the system table
         * makes changing this administation easier when the change happens.
         */
        if ((entity || table_data->columns[c] == -1 || from == EcsCascade)) {
            add_ref(world, query, table_type, c, table_data, component, entity, 
                from);
        }

        table_data->components[c] = component;
    }

    if (table) {
        int32_t matched_table_index = 0;
        if (!has_auto_activation(query)) {
            /* If query doesn't automatically activates/inactivates tables, the
             * table has been added to query->tables, and we can get the count
             * to determine the current table index. */
            matched_table_index = ecs_vector_count(query->tables) - 1;
            ecs_assert(matched_table_index >= 0, ECS_INTERNAL_ERROR, NULL);
        }
        
        ecs_table_register_query(world, table, query, matched_table_index);
    }

    /* Use tail recursion when adding table for multiple traits */
    trait_cur ++;
    if (trait_cur < trait_count) {
        goto add_trait;
    }

    if (trait_index_offsets) {
        ecs_os_free(trait_index_offsets);
    }
}

static
bool match_column(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_sig_from_kind_t from_kind,
    ecs_entity_t component,
    ecs_entity_t source,
    ecs_match_failure_t *failure_info)
{
    if (from_kind == EcsFromAny) {
        failure_info->reason = EcsMatchFromSelf;
        return ecs_type_has_entity(world, type, component);
        
    } else if (from_kind == EcsFromOwned) {
        failure_info->reason = EcsMatchFromOwned;
        return ecs_type_owns_entity(world, type, component, true);

    } else if (from_kind == EcsFromShared) {
        failure_info->reason = EcsMatchFromShared;
        return !ecs_type_owns_entity(world, type, component, true) &&
            ecs_type_owns_entity(world, type, component, false);

    } else if (from_kind == EcsFromParent) {
        failure_info->reason = EcsMatchFromContainer;
        return ecs_find_in_type(world, type, component, ECS_CHILDOF) != 0;

    } else if (from_kind == EcsFromEntity) {
        failure_info->reason = EcsMatchFromEntity;
        ecs_type_t source_type = ecs_get_type(world, source);
        return ecs_type_has_entity(world, source_type, component);
    } else {
        return true;
    }
}

/* Match table with system */
bool ecs_query_match(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    ecs_match_failure_t *failure_info)
{
    /* Prevent having to add if not null checks everywhere */
    ecs_match_failure_t tmp_failure_info;
    if (!failure_info) {
        failure_info = &tmp_failure_info;
    }

    failure_info->reason = EcsMatchOk;
    failure_info->column = 0;

    if (!(query->flags & EcsQueryNeedsTables)) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t type, table_type = table->type;

    /* Don't match disabled entities */
    if (!(query->flags & EcsQueryMatchDisabled) && ecs_type_owns_entity(
        world, table_type, EcsDisabled, true))
    {
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    /* Don't match prefab entities */
    if (!(query->flags & EcsQueryMatchPrefab) && ecs_type_owns_entity(
        world, table_type, EcsPrefab, true))
    {
        failure_info->reason = EcsMatchEntityIsPrefab;
        return false;
    }

    /* Check if trait cardinality matches traits in query, if any */
    if (count_traits(query, table->type) == -1) {
        return false;
    }

    int32_t i, column_count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);

    for (i = 0; i < column_count; i ++) {
        ecs_sig_column_t *elem = &columns[i];
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        failure_info->column = i + 1;

        if (oper_kind == EcsOperAnd) {
            if (!match_column(
                world, table_type, from_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperNot) {
            if (match_column(
                world, table_type, from_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;

            if (from_kind == EcsFromAny) {
                if (!ecs_type_contains(
                    world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    return false;
                }
            } else if (from_kind == EcsFromOwned) {
                if (!ecs_type_contains(
                    world, table_type, type, false, false))
                {
                    failure_info->reason = EcsMatchOrFromOwned;
                    return false;
                }
            } else if (from_kind == EcsFromShared) {
                if (ecs_type_contains(
                        world, table_type, type, false, false) ||
                    !ecs_type_contains(
                        world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromShared;
                    return false;
                }                                
            } else if (from_kind == EcsFromParent) {
                if (!(table->flags & EcsTableHasParent)) {
                    failure_info->reason = EcsMatchOrFromContainer;
                    return false;
                }

                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    failure_info->reason = EcsMatchOrFromContainer;
                    return false;
                }
            }
        }
    }

    return true;
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int32_t i, count = ecs_sparse_count(world->stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(
            world->stage.tables, ecs_table_t, i);

        if (ecs_query_match(world, table, query, NULL)) {
            add_table(world, query, table);
        }
    }

    order_ranked_tables(world, query);
}

/** Get index of table in system's matched tables */
static
int32_t get_table_param_index(
    ecs_table_t *table,
    ecs_vector_t *tables)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

    return i;
}

/** Check if a table was matched with the system */
static
int32_t table_matched(
    ecs_vector_t *tables,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            return i;
        }
    }

    return -1;
}

static
void resolve_cascade_container(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t table_data_index,
    ecs_type_t table_type)
{
    ecs_matched_table_t *table_data = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_data_index);
    
    ecs_assert(table_data->references != 0, ECS_INTERNAL_ERROR, NULL);

    /* Obtain reference index */
    int32_t *column_indices = table_data->columns;
    int32_t column = query->cascade_by - 1;
    int32_t ref_index = -column_indices[column] - 1;

    /* Obtain pointer to the reference data */
    ecs_ref_t *references = ecs_vector_first(table_data->references, ecs_ref_t);
    ecs_assert(ref_index < ecs_vector_count(table_data->references), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_ref_t *ref = &references[ref_index];
    ecs_assert(ref->component == get_cascade_component(query), 
        ECS_INTERNAL_ERROR, NULL);

    /* Resolve container entity */
    ecs_entity_t container = ecs_find_in_type(
        world, table_type, ref->component, ECS_CHILDOF);    

    /* If container was found, update the reference */
    if (container) {
        references[ref_index].entity = container;
        ecs_get_ref_w_entity(
            world, &references[ref_index], container, 
            ref->component);
    } else {
        references[ref_index].entity = 0;
    }
}

#define ELEM(ptr, size, index) ECS_OFFSET(ptr, size * index)

static
int32_t qsort_partition(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,    
    int32_t elem_size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{
    int32_t p = (hi + lo) / 2;
    void *pivot = ELEM(ptr, elem_size, p);
    ecs_entity_t pivot_e = entities[p];
    int32_t i = lo - 1, j = hi + 1;
    void *el;    

repeat:
    {
        do {
            i ++;
            el = ELEM(ptr, elem_size, i);
        } while ( compare(entities[i], el, pivot_e, pivot) < 0);

        do {
            j --;
            el = ELEM(ptr, elem_size, j);
        } while ( compare(entities[j], el, pivot_e, pivot) > 0);

        if (i >= j) {
            return j;
        }

        ecs_table_swap(world, &world->stage, table, data, i, j);

        if (p == i) {
            pivot = ELEM(ptr, elem_size, j);
            pivot_e = entities[j];
        } else if (p == j) {
            pivot = ELEM(ptr, elem_size, i);
            pivot_e = entities[i];
        }

        goto repeat;
    }
}

static
void qsort_array(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,
    int32_t size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{   
    if ((hi - lo) < 1)  {
        return;
    }   

    int32_t p = qsort_partition(
        world, table, data, entities, ptr, size, lo, hi, compare);

    qsort_array(world, table, data, entities, ptr, size, lo, p, compare);

    qsort_array(world, table, data, entities, ptr, size, p + 1, hi, compare); 
}

static
void sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_index,
    ecs_compare_action_t compare)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data || !data->entities) {
        /* Nothing to sort */
        return;
    }

    int32_t count = ecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_column_t *column = &data->columns[column_index];
        size = column->size;
        ptr = ecs_vector_first_t(column->data, size, column->alignment);
    }

    qsort_array(world, table, data, entities, ptr, size, 0, count - 1, compare);
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_matched_table_t *table;
    ecs_entity_t *entities;
    void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
} sort_helper_t;

static
void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    return ELEM(helper->ptr, helper->elem_size, helper->row);
}

static
ecs_entity_t e_from_helper(
    sort_helper_t *helper)
{
    if (helper->row < helper->count) {
        return helper->entities[helper->row];
    } else {
        return 0;
    }
}

static
void build_sorted_table_range(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t start,
    int32_t end)
{
    ecs_entity_t component = query->sort_on_component;
    ecs_compare_action_t compare = query->compare;

    /* Fetch data from all matched tables */
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    sort_helper_t *helper = ecs_os_malloc((end - start) * ECS_SIZEOF(sort_helper_t));

    int i, to_sort = 0;
    for (i = start; i < end; i ++) {
        ecs_matched_table_t *table = &tables[i];
        ecs_data_t *data = ecs_table_get_data(world, table->table);
        if (!data || !data->entities || !ecs_table_count(table->table)) {
            continue;
        }

        int32_t index = ecs_type_index_of(table->table->type, component);
        if (index != -1) {
            ecs_column_t *column = &data->columns[index];
            int16_t size = column->size;
            int16_t align = column->alignment;
            helper[to_sort].ptr = ecs_vector_first_t(column->data, size, align);
            helper[to_sort].elem_size = size;
        }

        helper[to_sort].table = table;
        helper[to_sort].entities = ecs_vector_first(data->entities, ecs_entity_t);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table->table);
        to_sort ++;
    }

    ecs_table_range_t *cur = NULL;

    bool proceed;
    do {
        int32_t j, min = 0;
        proceed = true;

        ecs_entity_t e1;
        while (!(e1 = e_from_helper(&helper[min]))) {
            min ++;
            if (min == to_sort) {
                proceed = false;
                break;
            }
        }

        if (!proceed) {
            break;
        }

        for (j = min + 1; j < to_sort; j++) {
            ecs_entity_t e2 = e_from_helper(&helper[j]);
            if (!e2) {
                continue;
            }

            void *ptr1 = ptr_from_helper(&helper[min]);
            void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
            }
        }

        sort_helper_t *cur_helper = &helper[min];

        if (!cur || cur->table != cur_helper->table) {
            cur = ecs_vector_add(
                &query->table_ranges, ecs_table_range_t);
            ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);
            cur->table = cur_helper->table;
            cur->start_row = cur_helper->row;
            cur->count = 1;
        } else {
            cur->count ++;
        }

        cur_helper->row ++;
    } while (proceed);

    ecs_os_free(helper);
}

static
void build_sorted_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    /* Clean previous sorted tables */
    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_matched_table_t *table = NULL;

    int32_t start = 0, rank = 0;
    for (i = 0; i < count; i ++) {
        table = &tables[i];
        if (rank != table->rank) {
            if (start != i) {
                build_sorted_table_range(world, query, start, i);
                start = i;
            }
            rank = table->rank;
        }
    }

    if (start != i) {
        build_sorted_table_range(world, query, start, i);
    }
}

static
bool tables_dirty(
    ecs_query_t *query)
{
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    bool is_dirty = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            is_dirty = is_dirty || (dirty_state[t] != m_table->monitor[t]);
        }
    }

    is_dirty = is_dirty || (query->match_count != query->prev_match_count);

    return is_dirty;
}

static
void tables_reset_dirty(
    ecs_query_t *query)
{
    query->prev_match_count = query->match_count;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        if (!m_table->monitor) {
            /* If one table doesn't have a monitor, none of the tables will have
             * a monitor, so early out. */
            return;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            m_table->monitor[t] = dirty_state[t];
        }
    }
}

static
void sort_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_compare_action_t compare = query->compare;
    if (!compare) {
        return;
    }
    
    ecs_entity_t sort_on_component = query->sort_on_component;

    /* Iterate over active tables. Don't bother with inactive tables, since
     * they're empty */
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    bool tables_sorted = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);

        is_dirty = is_dirty || (dirty_state[0] != m_table->monitor[0]);

        int32_t index = -1;
        if (sort_on_component) {
            /* Get index of sorted component. We only care if the component we're
            * sorting on has changed or if entities have been added / re(moved) */
            index = ecs_type_index_of(table->type, sort_on_component);
            ecs_assert(index != -1, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(index < ecs_vector_count(table->type), ECS_INTERNAL_ERROR, NULL); 
            is_dirty = is_dirty || (dirty_state[index + 1] != m_table->monitor[index + 1]);
        }      
        
        /* Check both if entities have moved (element 0) or if the component
         * we're sorting on has changed (index + 1) */
        if (is_dirty) {
            /* Sort the table */
            sort_table(world, table, index, compare);
            tables_sorted = true;
        }
    }

    if (tables_sorted || query->match_count != query->prev_match_count) {
        build_sorted_tables(world, query);
        query->match_count ++; /* Increase version if tables changed */
    }
}

static
bool has_refs(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        ecs_sig_from_kind_t from_kind = columns[i].from_kind;

        if (columns[i].oper_kind == EcsOperNot && from_kind == EcsFromEmpty) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromId to
             * prevent resolving the ref */
            return true;
        } else if (from_kind != EcsFromAny && from_kind != EcsFromEmpty) {
            /* If the component is not from the entity being iterated over, and
             * the column is not just passing an id, it must be a reference to
             * another entity. */
            return true;
        }
    }

    return false;
}

static
bool has_traits(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        if (is_column_trait(&columns[i])) {
            return true;
        }
    }

    return false;    
}

static
void register_monitors(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_vector_each(query->sig.columns, ecs_sig_column_t, column, {
        /* If component is requested with CASCADE source register component as a
         * parent monitor. Parent monitors keep track of whether an entity moved
         * in the hierarchy, which potentially requires the query to reorder its
         * tables. 
         * Also register a regular component monitor for EcsCascade columns.
         * This ensures that when the component used in the CASCADE column
         * is added or removed tables are updated accordingly*/
        if (column->from_kind == EcsCascade) {
            if (column->oper_kind != EcsOperOr) {
                ecs_component_monitor_register(
                    &world->parent_monitors, column->is.component, query);

                ecs_component_monitor_register(
                    &world->component_monitors, column->is.component, query);
            } else {
                ecs_vector_each(column->is.type, ecs_entity_t, e_ptr, {
                    ecs_component_monitor_register(
                        &world->parent_monitors, *e_ptr, query);

                    ecs_component_monitor_register(
                        &world->component_monitors, *e_ptr, query);
                });
            }

        /* FromSelf also requires registering a monitor, as FromSelf columns can
         * be matched with prefabs. The only column kinds that do not require
         * registering a monitor are FromOwned and FromNothing. */
        } else if (column->from_kind == EcsFromAny || 
            column->from_kind == EcsFromShared ||
            column->from_kind == EcsFromEntity ||
            column->from_kind == EcsFromParent)
        {
            if (column->oper_kind != EcsOperOr) {
                ecs_component_monitor_register(
                    &world->component_monitors, column->is.component, query);
            } else {
                ecs_vector_each(column->is.type, ecs_entity_t, e_ptr, {
                    ecs_component_monitor_register(
                        &world->component_monitors, *e_ptr, query);
                });
            }
        }
    });
}

static
void process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int i, count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);

    for (i = 0; i < count; i ++) {
        ecs_sig_column_t *column = &columns[i];
        ecs_sig_oper_kind_t op = column->oper_kind; 
        ecs_sig_from_kind_t from = column->from_kind; 

        if (!(query->flags & EcsQueryMatchDisabled)) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsDisabled,
                 * signal that disabled entities should be matched. By default,
                 * disabled entities are not matched. */
                if (ecs_type_owns_entity(
                    world, column->is.type, EcsDisabled, true))
                {
                    query->flags |= EcsQueryMatchDisabled;
                }         
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == EcsDisabled) {
                    query->flags |= EcsQueryMatchDisabled;
                }
            }
        }

        if (!(query->flags & EcsQueryMatchPrefab)) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsPrefab,
                * signal that disabled entities should be matched. By default,
                * prefab entities are not matched. */
                if (ecs_type_owns_entity(
                    world, column->is.type, EcsPrefab, true))
                {
                    query->flags |= EcsQueryMatchPrefab;
                }            
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == EcsPrefab) {
                    query->flags |= EcsQueryMatchPrefab;
                }
            }
        }

        if (from == EcsFromAny || 
            from == EcsFromOwned ||
            from == EcsFromShared ||
            from == EcsFromParent) 
        {
            query->flags |= EcsQueryNeedsTables;
        }

        if (from == EcsCascade) {
            query->cascade_by = i + 1;
            query->rank_on_component = column->is.component;
        }

        if (from == EcsFromEntity) {
            ecs_assert(column->source != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_set_watch(world, &world->stage, column->source);
        }
    }

    query->flags |= (ecs_flags32_t)(has_refs(&query->sig) * EcsQueryHasRefs);
    query->flags |= (ecs_flags32_t)(has_traits(&query->sig) * EcsQueryHasTraits);

    register_monitors(world, query);
}


/* -- Private API -- */

void ecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (ecs_query_match(world, table, query, NULL)) {
        add_table(world, query, table);
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_query_activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active)
{
    ecs_vector_t *src_array, *dst_array;

    if (active) {
        src_array = query->empty_tables;
        dst_array = query->tables;
    } else {
        src_array = query->tables;
        dst_array = query->empty_tables;
    }

    int32_t i = get_table_param_index(table, src_array);
    int32_t src_count = ecs_vector_move_index(
        &dst_array, src_array, ecs_matched_table_t, i);

    if (active) {
        query->tables = dst_array;
    } else {
        query->empty_tables = dst_array;
    }

    /* Activate system if registered with query */
#ifdef FLECS_SYSTEMS_H
    if (query->system) {
        int32_t dst_count = ecs_vector_count(dst_array);
        if (active) {
            if (dst_count == 1) {
                ecs_system_activate(world, query->system, true);
            }
        } else if (src_count == 0) {
            ecs_system_activate(world, query->system, false);
        }
    }
#else
    (void)src_count;
#endif

    order_ranked_tables(world, query);
}

static
void free_matched_table(
    ecs_matched_table_t *table)
{
    ecs_os_free(table->columns);
    ecs_os_free(table->components);
    ecs_vector_free(table->references);
    ecs_os_free(table->monitor);
}

/* Remove table */
static
void remove_table(
    ecs_vector_t *tables,
    int32_t index)
{
    ecs_matched_table_t *table = ecs_vector_get(
        tables, ecs_matched_table_t, index);
    free_matched_table(table);
    ecs_vector_remove_index(tables, ecs_matched_table_t, index);
}

/* Rematch system with tables after a change happened to a watched entity */
void ecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_trace_1("rematch query %s", query_name(world, query));

    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        /* Is the system currently matched with the table? */
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        int32_t match = table_matched(query->tables, table);

        if (ecs_query_match(world, table, query, NULL)) {
            /* If the table matches, and it is not currently matched, add */
            if (match == -1) {
                if (table_matched(query->empty_tables, table) == -1) {
                    add_table(world, query, table);
                }

            /* If table still matches and has cascade column, reevaluate the
                * sources of references. This may have changed in case 
                * components were added/removed to container entities */ 
            } else if (query->cascade_by) {
                resolve_cascade_container(
                    world, query, match, table->type);
            }
        } else {
            /* If table no longer matches, remove it */
            if (match != -1) {
                remove_table(query->tables, match);
            } else {
                /* Make sure the table is removed if it was inactive */
                match = table_matched(
                    query->empty_tables, table);
                if (match != -1) {
                    remove_table(query->empty_tables, match);
                }
            }
        }
    }

    group_tables(world, query);

    order_ranked_tables(world, query);

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    if (query->system) {
        if (ecs_sig_check_constraints(world, &query->sig)) {
            ecs_remove_entity(world, query->system, EcsDisabledIntern);
        } else {
            ecs_add_entity(world, query->system, EcsDisabledIntern);
        }
    }
}


/* -- Public API -- */

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig)
{
    ecs_query_t *result = ecs_sparse_add(world->queries, ecs_query_t);
    memset(result, 0, sizeof(ecs_query_t));
    result->world = world;
    result->sig = *sig;
    result->tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->empty_tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->system = system;
    result->match_count = 0;
    result->prev_match_count = -1;
    result->flags = 0;

    process_signature(world, result);

    ecs_trace_2("query #[green]%s#[reset] created with expression #[red]%s", 
        query_name(world, result), result->sig.expr);

    ecs_log_push();

    if (result->flags & EcsQueryNeedsTables) {
        if (ecs_has_entity(world, system, EcsMonitor)) {
            result->flags |= EcsQueryMonitor;
        }
        
        if (ecs_has_entity(world, system, EcsOnSet)) {
            result->flags |= EcsQueryOnSet;
        }

        if (ecs_has_entity(world, system, EcsUnSet)) {
            result->flags |= EcsQueryUnSet;
        }        

        match_tables(world, result);
    } else {
        /* Add stub table that resolves references (if any) so everything is
         * preprocessed when the query is evaluated. */
        add_table(world, result, NULL);
    }

    if (result->cascade_by) {
        result->group_table = rank_by_depth;
    }

    ecs_log_pop();

    /* Make sure application can't try to free sig resources */
    *sig = (ecs_sig_t){ 0 };

    return result;
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    ecs_sig_t sig = { 0 };
    ecs_sig_init(world, NULL, expr, &sig);
    return ecs_query_new_w_sig(world, 0, &sig);
}

void ecs_query_free(
    ecs_query_t *query)
{
    ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
        free_matched_table(table);
    });

    ecs_vector_each(query->tables, ecs_matched_table_t, table, {
        free_matched_table(table);
    });

    ecs_vector_free(query->tables);
    ecs_vector_free(query->empty_tables);
    ecs_vector_free(query->table_ranges);
    ecs_sig_deinit(&query->sig);
}

/* Create query iterator */
ecs_iter_t ecs_query_iter_page(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);

    sort_tables(query->world, query);

    tables_reset_dirty(query);

    int32_t table_count;
    if (query->table_ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_query_iter_t it = {
        .query = query,
        .offset = offset,
        .limit = limit,
        .remaining = limit,
        .index = 0,
    };

    return (ecs_iter_t){
        .world = query->world,
        .query = query,
        .column_count = ecs_vector_count(query->sig.columns),
        .table_count = table_count,
        .inactive_table_count = ecs_vector_count(query->empty_tables),
        .iter.query = it
    };
}

ecs_iter_t ecs_query_iter(
    ecs_query_t *query)
{
    return ecs_query_iter_page(query, 0, 0);
}

void ecs_query_set_iter(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count)
{
    ecs_matched_table_t *table = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_index);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *world_table = table->table;
    ecs_data_t *table_data = ecs_table_get_staged_data(world, stage, world_table);
    ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities, ecs_entity_t);  
    it->entities = &entity_buffer[row];

    it->world = world;
    it->query = query;
    it->column_count = ecs_vector_count(query->sig.columns);
    it->table_count = 1;
    it->inactive_table_count = 0;
    it->table = world_table;
    it->table_columns = table_data->columns;
    it->columns = table->columns;
    it->components = table->components;
    it->references = ecs_vector_first(table->references, ecs_ref_t);
    it->offset = row;
    it->count = count;
    it->total_count = count;
}

/* Return next table */
bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_query_iter_t *iter = &it->iter.query;
    ecs_query_t *query = iter->query;
    ecs_world_t *world = it->world;

    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_table_range_t *ranges = ecs_vector_first(query->table_ranges, ecs_table_range_t);

    ecs_assert(!ranges || query->compare, ECS_INTERNAL_ERROR, NULL);
    
    int32_t table_count;
    if (ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_get_stage(&world);

    int32_t offset = iter->offset;
    int32_t limit = iter->limit;
    int32_t remaining = iter->remaining;
    int32_t prev_count = it->total_count;
    bool offset_limit = (offset | limit) != 0;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table;

        if (ranges) {
            table = ranges[i].table;
        } else {
            table = &tables[i];
        }

        ecs_table_t *world_table = table->table;
        ecs_data_t *table_data = NULL;
        int32_t first = 0, count = 0;

        if (world_table) {
            table_data = ecs_table_get_data(world, world_table);
            ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
            it->table_columns = table_data->columns;
            
            if (ranges) {
                first = ranges[i].start_row;
                count = ranges[i].count;
            } else {
                count = ecs_table_count(world_table);
            }
        }

        if (table_data) {
            if (offset_limit) {
                if (offset) {
                    if (offset > count) {
                        /* No entities to iterate in current table */
                        offset = iter->offset -= count;
                        continue;
                    } else {
                        first += offset;
                        count -= offset;
                        offset = iter->offset = 0;
                    }
                }

                if (remaining) {
                    if (remaining > count) {
                        remaining = iter->remaining -= count;
                    } else {
                        count = remaining;
                        remaining = iter->remaining = 0;
                    }
                } else if (limit) {
                    /* Limit hit: no more entities left to iterate */
                    return false;
                }
            }

            if (!count) {
                /* No entities to iterate in current table */
                continue;
            }

            ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities, ecs_entity_t); 
            it->entities = &entity_buffer[first];
            it->offset = first;
            it->count = count;
            it->total_count = count;
        }

        it->table = world_table;
        it->columns = table->columns;
        it->components = table->components;
        it->references = ecs_vector_first(table->references, ecs_ref_t);
        it->frame_offset += prev_count;

        /* Table is ready to be iterated, return it struct */
        iter->index = i + 1;

        return true;
    }

    return false;
}

bool ecs_query_next_worker(
    ecs_iter_t *it,
    int32_t current,
    int32_t total)
{
    int32_t per_worker, first, prev_offset = it->offset;

    do {
        if (!ecs_query_next(it)) {
            return false;
        }

        int32_t count = it->count;
        per_worker = count / total;
        first = per_worker * current;

        count -= per_worker * total;

        if (count) {
            if (current < count) {
                per_worker ++;
                first += current;
            } else {
                first += count;
            }
        }

        if (!per_worker && !(it->query->flags & EcsQueryNeedsTables)) {
            if (current == 0) {
                return true;
            } else {
                return false;
            }
        }
    } while (!per_worker);

    it->frame_offset -= prev_offset;
    it->count = per_worker;
    it->offset += first;
    it->entities = &it->entities[first];
    it->frame_offset += first;

    return true;
}

void ecs_query_order_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_compare_action_t compare)
{
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->sort_on_component = sort_component;
    query->compare = compare;

    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    sort_tables(world, query);    

    if (!query->table_ranges) {
        build_sorted_tables(world, query);
    }
}

void ecs_query_group_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_rank_type_action_t group_table_action)
{
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->rank_on_component = sort_component;
    query->group_table = group_table_action;

    group_tables(world, query);

    order_ranked_tables(world, query);

    build_sorted_tables(world, query);
}

bool ecs_query_changed(
    ecs_query_t *query)
{
    return tables_dirty(query);
}

/* When a new table is created, match it with the queries registered with the
 * world. If a query matches, it will call the esc_table_register_query 
 * function which will add the query to the table administration. */
void ecs_notify_queries_of_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t i, count = ecs_sparse_count(world->queries);

    for (i = 0; i < count; i ++) {
        ecs_query_t *query = ecs_sparse_get(world->queries, ecs_query_t, i);
        ecs_query_match_table(world, query, table);
    }
}

const EcsComponent* ecs_component_from_id(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t trait = 0;

    /* If this is a trait, get the trait component from the identifier */
    if (e & ECS_TRAIT) {
        trait = e;
        e = e & ECS_ENTITY_MASK;
        e = ecs_entity_t_hi(e);
    }

    const EcsComponent *component = ecs_get(world, e, EcsComponent);
    if (!component && trait) {
        /* If this is a trait column and the trait is not a component, use
            * the component type of the component the trait is applied to. */
        e = ecs_entity_t_lo(trait);
        component = ecs_get(world, e, EcsComponent);
    }    

    return component;
}

/* Count number of columns with data (excluding tags) */
static
int32_t data_column_count(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        /* Typically all components will be clustered together at the start of
         * the type as components are created from a separate id pool, and type
         * vectors are sorted. 
         * Explicitly check for EcsComponent and EcsName since the ecs_has check
         * doesn't work during bootstrap. */
        if ((component == ecs_entity(EcsComponent)) || 
            (component == ecs_entity(EcsName)) || 
            ecs_component_from_id(world, component) != NULL) 
        {
            count = c_ptr_i + 1;
        }
    });

    return count;
}

static
ecs_type_t entities_to_type(
    ecs_entities_t *entities)
{
    if (entities->count) {
        ecs_vector_t *result = NULL;
        ecs_vector_set_count(&result, ecs_entity_t, entities->count);
        ecs_entity_t *array = ecs_vector_first(result, ecs_entity_t);
        ecs_os_memcpy(array, entities->array, ECS_SIZEOF(ecs_entity_t) * entities->count);
        return result;
    } else {
        return NULL;
    }
}

static
void register_child_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entity_t parent)
{
    if (stage == &world->stage) {
        /* Register child table with parent */
        ecs_vector_t *child_tables = ecs_map_get_ptr(
                world->child_tables, ecs_vector_t*, parent);
        if (!child_tables) {
            child_tables = ecs_vector_new(ecs_table_t*, 1);
        }
        
        ecs_table_t **el = ecs_vector_add(&child_tables, ecs_table_t*);
        *el = table;

        if (!world->child_tables) {
            world->child_tables = ecs_map_new(ecs_vector_t*, 1);
        }

        ecs_map_set(world->child_tables, parent, &child_tables);
    }
}

static
void init_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    ecs_entity_t *entities = ecs_vector_first(table->type, ecs_entity_t);
    int32_t count = ecs_vector_count(table->type);

    table->lo_edges = ecs_os_calloc(sizeof(ecs_edge_t) * ECS_HI_COMPONENT_ID);
    table->hi_edges = ecs_map_new(ecs_edge_t, 0);

    table->lo_edges[0].add = table;
    
    /* Make add edges to own components point to self */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        if (e >= ECS_HI_COMPONENT_ID) {
            ecs_edge_t edge = { .add = table };

            if (count == 1) {
                edge.remove = &stage->root;
            }

            ecs_map_set(table->hi_edges, e, &edge);
        } else {
            table->lo_edges[e].add = table;

            if (count == 1) {
                table->lo_edges[e].remove = &stage->root;
            } else {
                table->lo_edges[e].remove = NULL;
            }
        }

        /* As we're iterating over the table components, also set the table
         * flags. These allow us to quickly determine if the table contains
         * data that needs to be handled in a special way, like prefabs or 
         * containers */
        if (e <= EcsLastInternalComponentId) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (e == EcsPrefab) {
            table->flags |= EcsTableIsPrefab;
            table->flags |= EcsTableIsDisabled;
        }

        if (e == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        }

        if (e == ecs_entity(EcsComponent)) {
            table->flags |= EcsTableHasComponentData;
        }

        if (e & ECS_XOR) {
            table->flags |= EcsTableHasXor;
        }

        if (e & ECS_INSTANCEOF) {
            table->flags |= EcsTableHasBase;
        }

        if (e & ECS_CHILDOF) {
            table->flags |= EcsTableHasParent;

            ecs_entity_t parent = e & ECS_ENTITY_MASK;
            register_child_table(world, stage, table, parent);
        }

        if (e & (ECS_CHILDOF | ECS_INSTANCEOF)) {
            if (stage == &world->stage) {
                ecs_set_watch(world, stage, e & ECS_ENTITY_MASK);
            }
        }

        /* Set flags based on component actions */
        ecs_c_info_t *ci = ecs_get_c_info(world, e & ECS_ENTITY_MASK);
        if (ci) {
            table->flags |= ecs_get_component_action_flags(ci);
        }
    }
    
    /* Register as root table */
    if (!(table->flags & EcsTableHasParent)) {
        register_child_table(world, stage, table, 0);
    }
}

static
void init_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *entities)
{
    table->type = entities_to_type(entities);
    table->data = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->monitors = NULL;
    table->on_set = NULL;
    table->on_set_all = NULL;
    table->on_set_override = NULL;
    table->un_set_all = NULL;
    table->alloc_count = 0;
    
    init_edges(world, stage, table);

    table->queries = NULL;
    table->column_count = data_column_count(world, table);
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *entities)
{
    ecs_table_t *result = ecs_sparse_add(stage->tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    init_table(world, stage, result, entities);

#ifndef NDEBUG
    if (stage == &world->stage) {
        char *expr = ecs_type_str(world, result->type);
        ecs_trace_2("table #[green][%s]#[normal] created", expr);
        ecs_os_free(expr);
    }
#endif
    ecs_log_push();

    /* Don't notify queries if table is created in stage */
    if (stage == &world->stage) {
        ecs_notify_queries_of_table(world, result);
    }

    ecs_log_pop();

    return result;
}

void ecs_init_root_table(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_entities_t entities = {
        .array = NULL,
        .count = 0
    };

    init_table(world, stage, &stage->root, &entities);
}

static
void add_entity_to_type(
    ecs_type_t type,
    ecs_entity_t add,
    ecs_entity_t replace,
    ecs_entities_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);    
    bool added = false;

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e == replace) {
            continue;
        }

        if (e > add && !added) {
            out->array[el ++] = add;
            added = true;
        }
        
        out->array[el ++] = e;

        ecs_assert(el <= out->count, ECS_INTERNAL_ERROR, NULL);
    }

    if (!added) {
        out->array[el ++] = add;
    }

    out->count = el;

    ecs_assert(out->count != 0, ECS_INTERNAL_ERROR, NULL);
}

static
void remove_entity_from_type(
    ecs_type_t type,
    ecs_entity_t remove,
    ecs_entities_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e != remove) {
            out->array[el ++] = e;
            ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
        }
    }

    out->count = el;
}

static
ecs_edge_t* get_edge(
    ecs_table_t *node,
    ecs_entity_t e)
{
    ecs_edge_t *edge;

    if (e < ECS_HI_COMPONENT_ID) {
        edge = &node->lo_edges[e];
    } else {
        edge = ecs_map_get(node->hi_edges, ecs_edge_t, e);        
        if (!edge) {
            ecs_edge_t new_edge = {0};
            ecs_map_set(node->hi_edges, e, &new_edge);
            edge = ecs_map_get(node->hi_edges, ecs_edge_t, e);    
            ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        }
    }

    return edge;
}

static
void create_backlink_after_add(
    ecs_table_t *next,
    ecs_table_t *prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->remove) {
        edge->remove = prev;
    }
}

static
void create_backlink_after_remove(
    ecs_table_t *next,
    ecs_table_t *prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->add) {
        edge->add = prev;
    }
}

static
ecs_table_t *find_or_create_table_include(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entity_t add)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_entities_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * (count + 1)),
        .count = count + 1
    };

    /* If table has a XOR column, check if the entity that is being added to the
     * table is part of the XOR type, and if it is, find the current entity in
     * the table type matching the XOR type. This entity must be replaced in
     * the new table, to ensure the XOR constraint isn't violated. */
    ecs_entity_t replace = 0;
    if (node->flags & EcsTableHasXor) {
        ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
        int32_t i, type_count = ecs_vector_count(type);
        ecs_type_t xor_type = NULL;

        for (i = type_count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (e & ECS_XOR) {
                ecs_entity_t e_type = e & ECS_ENTITY_MASK;
                const EcsType *type_ptr = ecs_get(world, e_type, EcsType);
                ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                if (ecs_type_owns_entity(
                    world, type_ptr->normalized, add, true)) 
                {
                    xor_type = type_ptr->normalized;
                }
            } else if (xor_type) {
                if (ecs_type_owns_entity(world, xor_type, e, true)) {
                    replace = e;
                    break;
                }
            }
        }

        ecs_assert(!xor_type || replace != 0, ECS_INTERNAL_ERROR, NULL);
    }

    add_entity_to_type(type, add, replace, &entities);

    ecs_table_t *result = ecs_table_find_or_create(world, stage, &entities);
    
    if (result != node && stage == &world->stage) {
        create_backlink_after_add(result, node, add);
    }

    return result;
}

static
ecs_table_t *find_or_create_table_exclude(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entity_t remove)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_entities_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * count),
        .count = count
    };

    remove_entity_from_type(type, remove, &entities);

    ecs_table_t *result = ecs_table_find_or_create(world, stage, &entities);
    if (!result) {
        return NULL;
    }

    if (result != node && stage == &world->stage) {
        create_backlink_after_remove(result, node, remove);
    }

    return result;    
}

static
ecs_table_t* traverse_remove_hi_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    int32_t i,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed)
{
    int32_t count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;

    for (; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t next_e = e;
        ecs_table_t *next;
        ecs_edge_t *edge;

        edge = get_edge(node, e);

        next = edge->remove;

        if (!next) {
            next = find_or_create_table_exclude(
                world, stage, node, next_e);
            if (!next) {
                return NULL;
            }

            /* Only make links when not staged, to prevent tables from the main
            * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->remove = next;
            }
        }

        if (removed && node != next) removed->array[removed->count ++] = e;

        node = next;        
    }

    return node;
}

ecs_table_t* ecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed)
{
    int32_t i, count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;
    node = node ? node : &world->stage.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_remove_hi_edges(world, stage, node, i, to_remove, 
                removed);
        }

        ecs_edge_t *edge = &node->lo_edges[e];
        ecs_table_t *next = edge->remove;

        if (!next) {
            if (edge->add == node) {
                /* Find table with all components of node except 'e' */
                next = find_or_create_table_exclude(world, stage, node, e);
                if (!next) {
                    return NULL;
                }

                /* Only make links when not staged, to prevent tables from the main
                * stage pointing to staged tables. */
                if (&world->stage == stage) {
                    edge->remove = next;
                }
            } else {
                /* If the add edge does not point to self, the table
                    * does not have the entity in to_remove. */
                continue;
            }
        }

        if (removed) removed->array[removed->count ++] = e;

        node = next;
    }    

    return node;
}

static
ecs_table_t* traverse_add_hi_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    int32_t i,
    ecs_entities_t *to_add,
    ecs_entities_t *added)
{
    int32_t count = to_add->count;
    ecs_entity_t *entities = to_add->array;

    for (; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t next_e = e;
        ecs_table_t *next;
        ecs_edge_t *edge;

        edge = get_edge(node, e);

        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(
                world, stage, node, next_e);
            
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);                    

            /* Only make links when not staged, to prevent tables from the main
             * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->add = next;
            }
        }

        if (added && node != next) added->array[added->count ++] = e;

        node = next;        
    }

    return node;
}

ecs_table_t* ecs_table_traverse_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entities_t *to_add,
    ecs_entities_t *added)    
{
    int32_t i, count = to_add->count;
    ecs_entity_t *entities = to_add->array;
    node = node ? node : &world->stage.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_table_t *next;

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_add_hi_edges(world, stage, node, i, to_add, added);
        }

        /* There should always be an edge for adding */
        ecs_edge_t *edge = &node->lo_edges[e];
        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(world, stage, node, e);
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);

            /* Only make links when not staged, to prevent tables from the main
             * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->add = next;
            }
        }

        if (added && node != next) {
            added->array[added->count ++] = e;
        }

        node = next;
    }

    return node;
}

static
int ecs_entity_compare(
    const void *e1,
    const void *e2)
{
    ecs_entity_t v1 = *(ecs_entity_t*)e1;
    ecs_entity_t v2 = *(ecs_entity_t*)e2;
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    } else {
        return 0;
    }
}

static
bool ecs_entity_array_is_ordered(
    ecs_entities_t *entities)
{
    ecs_entity_t prev = 0;
    ecs_entity_t *array = entities->array;
    int32_t i, count = entities->count;

    for (i = 0; i < count; i ++) {
        if (!array[i] && !prev) {
            continue;
        }
        if (array[i] <= prev) {
            return false;
        }
        prev = array[i];
    }    

    return true;
}

static
int32_t ecs_entity_array_dedup(
    ecs_entity_t *array,
    int32_t count)
{
    int32_t j, k;
    ecs_entity_t prev = array[0];

    for (k = j = 1; k < count; j ++, k++) {
        ecs_entity_t e = array[k];
        if (e == prev) {
            k ++;
        }

        array[j] = e;
        prev = e;
    }

    return count - (k - j);
}

#ifndef NDEBUG

static
int32_t count_occurrences(
    ecs_world_t *world,
    ecs_entities_t *entities,
    ecs_entity_t entity,
    int32_t constraint_index)    
{
    const EcsType *type_ptr = ecs_get(world, entity, EcsType);
    ecs_assert(type_ptr != NULL, 
        ECS_INVALID_PARAMETER, "flag must be applied to type");

    ecs_type_t type = type_ptr->normalized;
    int32_t count = 0;
    
    int i;
    for (i = 0; i < constraint_index; i ++) {
        ecs_entity_t e = entities->array[i];
        if (e & ECS_TYPE_ROLE_MASK) {
            break;
        }

        if (ecs_type_has_entity(world, type, e)) {
            count ++;
        }
    }

    return count;
}

static
void verify_constraints(
    ecs_world_t *world,
    ecs_entities_t *entities)
{
    int i, count = entities->count;
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities->array[i];
        ecs_entity_t mask = e & ECS_TYPE_ROLE_MASK;
        if (!mask || !(mask & (ECS_OR | ECS_XOR | ECS_NOT))) {
            break;
        }

        ecs_entity_t entity = e & ECS_ENTITY_MASK;
        int32_t matches = count_occurrences(world, entities, entity, i);
        switch(mask) {
        case ECS_OR:
            ecs_assert(matches >= 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_XOR:
            ecs_assert(matches == 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_NOT:
            ecs_assert(matches == 0, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);    
            break;
        }
    }
}

#endif

static
ecs_table_t *find_or_create(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_stage_t *search_stage,
    ecs_entities_t *entities)
{    
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);    

    /* Make sure array is ordered and does not contain duplicates */
    int32_t type_count = entities->count;
    ecs_entity_t *ordered = NULL;

    if (!type_count) {
        return &search_stage->root;
    }

    if (!ecs_entity_array_is_ordered(entities)) {
        ecs_size_t size = ECS_SIZEOF(ecs_entity_t) * type_count;
        ordered = ecs_os_alloca(size);
        ecs_os_memcpy(ordered, entities->array, size);
        qsort(ordered, (size_t)type_count, sizeof(ecs_entity_t), ecs_entity_compare);
        type_count = ecs_entity_array_dedup(ordered, type_count);
    } else {
        ordered = entities->array;
    }    

    /* Iterate tables, look if a table matches the type */
    ecs_sparse_t *tables = search_stage->tables;
    int32_t i, count = ecs_sparse_count(tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_type_t type = table->type;
        int32_t table_type_count = ecs_vector_count(type);

        /* If types do not contain same number of entities, table won't match */
        if (table_type_count != type_count) {
            continue;
        }

        /* Memcmp the types, as they must match exactly */
        ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
        if (!ecs_os_memcmp(ordered, type_array, type_count * ECS_SIZEOF(ecs_entity_t))) {
            /* Table found */
            return table;
        }
    }  

    ecs_entities_t ordered_entities = {
        .array = ordered,
        .count = type_count
    };

#ifndef NDEBUG
    /* Check for constraint violations */
    verify_constraints(world, &ordered_entities);
#endif

    /* Table has not been found. If we were searching in the main stage, but
     * we are staged, try to find the table in the stage */
    if (stage != &world->stage) {
        if (search_stage != stage) {
            /* The table does not yet exist in the main stage, so create it in
             * the stage. Table will be merged later. This is an expensive
             * operation that should rarely happen */
            return find_or_create(world, stage, stage, &ordered_entities);
        }
    }

    /* If we get here, the table has not been found in any stages. It has to be
     * created. */
    
    ecs_table_t *result = create_table(world, stage, &ordered_entities);

    ecs_assert(ordered_entities.count == ecs_vector_count(result->type), 
        ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_t* ecs_table_find_or_create(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    return find_or_create(
        world, stage, &world->stage, components);
}

ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
    return ecs_table_find_or_create(
        world, stage, &components);
}

#define LOAD_FACTOR (1.5)
#define KEY_SIZE (ECS_SIZEOF(ecs_map_key_t))
#define BUCKET_COUNT (8)
#define ELEM_SIZE(elem_size) (KEY_SIZE + elem_size)
#define BUCKET_SIZE(elem_size, offset)\
    (offset + BUCKET_COUNT * (ELEM_SIZE(elem_size)))

#define NEXT_ELEM(elem, elem_size) \
    ECS_OFFSET(elem, ELEM_SIZE(elem_size))

#define GET_ELEM(array, elem_size, index) \
    ECS_OFFSET(array, ELEM_SIZE(elem_size) * index)

#define PAYLOAD_ARRAY(bucket, offset) \
    ECS_OFFSET(bucket, offset)

#define PAYLOAD(elem) \
    ECS_OFFSET(elem, KEY_SIZE)

struct ecs_bucket_t {
    int32_t count;
};

struct ecs_map_t {
    ecs_sparse_t *buckets;
    int32_t elem_size;
    int32_t type_elem_size;
    int32_t bucket_size;
    int32_t bucket_count;
    int32_t count;
    int32_t offset;
};

static
int32_t next_pow_of_2(
    int32_t n)
{
    n --;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n ++;

    return n;
}

static
int32_t get_bucket_count(
    int32_t element_count)
{
    return next_pow_of_2((int32_t)((float)element_count * LOAD_FACTOR));
}

static
int32_t get_bucket_id(
    int32_t bucket_count,
    ecs_map_key_t key) 
{
    ecs_assert(bucket_count > 0, ECS_INTERNAL_ERROR, NULL);
    uint64_t result = key & ((uint64_t)bucket_count - 1);
    ecs_assert(result < INT32_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int32_t)result;
}

static
ecs_bucket_t* find_bucket(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_sparse_t *buckets = map->buckets;
    int32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        return NULL;
    }

    int32_t bucket_id = get_bucket_id(bucket_count, key);

    return _ecs_sparse_get_sparse(buckets, 0, bucket_id);
}

static
ecs_bucket_t* find_or_create_bucket(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_sparse_t *buckets = map->buckets;
    int32_t bucket_count = map->bucket_count;

    if (!bucket_count) {
        ecs_sparse_set_size(buckets, 8);
        bucket_count = 8;
    }

    int32_t bucket_id = get_bucket_id(bucket_count, key);
    
    bool is_new = false;
    ecs_bucket_t *bucket = _ecs_sparse_get_or_set_sparse(
        buckets, 0, bucket_id, &is_new);

    if (is_new) {
        bucket->count = 0;
    }

    return bucket;    
}

static
void remove_bucket(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    int32_t bucket_count = map->bucket_count;
    int32_t bucket_id = get_bucket_id(bucket_count, key);
    _ecs_sparse_remove(map->buckets, 0, bucket_id);
}

static
int32_t add_to_bucket(
    ecs_bucket_t *bucket,
    ecs_size_t elem_size,
    int32_t offset,
    ecs_map_key_t key,
    void *payload)
{
    ecs_assert(bucket->count < BUCKET_COUNT, ECS_INTERNAL_ERROR, NULL);

    void *array = PAYLOAD_ARRAY(bucket, offset);
    ecs_map_key_t *elem = GET_ELEM(array, elem_size, bucket->count);
    *elem = key;
    ecs_os_memcpy(PAYLOAD(elem), payload, elem_size);
    return ++ bucket->count;
}

static
void remove_from_bucket(
    ecs_bucket_t *bucket,
    ecs_map_key_t key,
    ecs_size_t elem_size,
    int32_t offset,
    int32_t index)
{
    (void)key;

    ecs_assert(bucket->count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(index < bucket->count, ECS_INTERNAL_ERROR, NULL);

    bucket->count--;

    if (index != bucket->count) {
        void *array = PAYLOAD_ARRAY(bucket, offset);
        ecs_map_key_t *elem = GET_ELEM(array, elem_size, index);
        ecs_map_key_t *last_elem = GET_ELEM(array, elem_size, bucket->count);

        ecs_assert(key == *elem, ECS_INTERNAL_ERROR, NULL);
        ecs_os_memcpy(elem, last_elem, ELEM_SIZE(elem_size));
    }
}

static
void rehash(
    ecs_map_t *map,
    int32_t bucket_count)
{
    bool rehash_again;

    ecs_assert(bucket_count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(bucket_count > map->bucket_count, ECS_INTERNAL_ERROR, NULL);

    do {
        rehash_again = false;

        ecs_sparse_t *buckets = map->buckets;
        ecs_size_t elem_size = map->elem_size;
        int32_t offset = map->offset;

        ecs_sparse_set_size(buckets, bucket_count);
        map->bucket_count = bucket_count;

        /* Only iterate over old buckets with elements */
        int32_t b, filled_bucket_count = ecs_sparse_count(buckets);
        const int32_t *indices = ecs_sparse_indices(buckets);

        /* Iterate backwards as elements could otherwise be moved to existing
         * buckets which could temporarily cause the number of elements in a
         * bucket to exceed BUCKET_COUNT. */
        for (b = filled_bucket_count - 1; b >= 0; b --) {
            int32_t bucket_id = indices[b];
            ecs_bucket_t *bucket = _ecs_sparse_get_sparse(buckets, 0, bucket_id);

            int i, count = bucket->count;
            ecs_map_key_t *array = PAYLOAD_ARRAY(bucket, offset);

            for (i = 0; i < count; i ++) {
                ecs_map_key_t *elem = GET_ELEM(array, elem_size, i);
                ecs_map_key_t key = *elem;
                int32_t new_bucket_id = get_bucket_id(bucket_count, key);

                if (new_bucket_id != bucket_id) {
                    bool is_new = false;
                    ecs_bucket_t *new_bucket = _ecs_sparse_get_or_set_sparse(
                        buckets, 0, new_bucket_id, &is_new);

                    if (is_new) {
                        new_bucket->count = 0;
                        indices = ecs_sparse_indices(buckets);
                    }

                    if (add_to_bucket(new_bucket, elem_size, offset, 
                        key, PAYLOAD(elem)) == BUCKET_COUNT) 
                    {
                        rehash_again = true;
                    }

                    remove_from_bucket(bucket, key, elem_size, offset, i);

                    count --;
                    i --;
                }
            }
        }

        bucket_count *= 2;        
    } while (rehash_again);
}

ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    ecs_size_t alignment, 
    int32_t element_count)
{
    ecs_map_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_map_t) * 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    int32_t bucket_count = get_bucket_count(element_count);

    result->count = 0;
    result->type_elem_size = elem_size;

    if (elem_size < ECS_SIZEOF(ecs_map_key_t)) {
        result->elem_size = ECS_SIZEOF(ecs_map_key_t);
    } else {
        result->elem_size = elem_size;
    }
    
    if (alignment < ECS_SIZEOF(ecs_map_key_t)) {
        result->offset = ECS_SIZEOF(ecs_map_key_t);
    } else {
        result->offset = alignment;
    }

    result->bucket_count = bucket_count;
    result->buckets = _ecs_sparse_new(BUCKET_SIZE(elem_size, result->offset), bucket_count);

    return result;
}

void ecs_map_free(
    ecs_map_t *map)
{
    if (map) {
        ecs_sparse_free(map->buckets);
        ecs_os_free(map);
    }
}

void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    (void)elem_size;

    if (!map) {
        return NULL;
    }

    ecs_assert(elem_size == map->type_elem_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_bucket(map, key);

    if (!bucket) {
        return NULL;
    }

    ecs_map_key_t *elem = PAYLOAD_ARRAY(bucket, map->offset);

    uint8_t i = 0;
    while (i++ < bucket->count) {
        if (*elem == key) {
            return PAYLOAD(elem);
        }

        elem = NEXT_ELEM(elem, map->elem_size);
    }

    return NULL;
}

bool _ecs_map_has(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    void *payload)
{
    const void *result = _ecs_map_get(map, elem_size, key);
    if (result) {
        ecs_os_memcpy(payload, result, elem_size);
        return true;
    } else {
        return false;
    }
}

void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    void * ptr_ptr = _ecs_map_get(map, ECS_SIZEOF(void*), key);

    if (ptr_ptr) {
        return *(void**)ptr_ptr;
    } else {
        return NULL;
    }
}

void _ecs_map_set(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem_size == map->type_elem_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_or_create_bucket(map, key);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t bucket_count = bucket->count;
    void *array = PAYLOAD_ARRAY(bucket, map->offset);
    ecs_map_key_t *elem = array, *found = NULL;

    uint8_t i = 0;
    while (i++ < bucket_count) {
        if (*elem == key) {
            found = elem;
            break;
        }

        elem = NEXT_ELEM(elem, map->elem_size);
    }

    if (!found) {
        if (bucket->count == BUCKET_COUNT) {
            /* Can't fit in current bucket, need to grow the map first */
            rehash(map, map->bucket_count * 2);
            _ecs_map_set(map, elem_size, key, payload);
        } else {
            ecs_assert(bucket->count < BUCKET_COUNT, ECS_INTERNAL_ERROR, NULL);

            bucket_count = ++bucket->count;
            int32_t map_count = ++map->count;
            
            *elem = key;
            ecs_os_memcpy(PAYLOAD(elem), payload, elem_size);

            int32_t target_bucket_count = get_bucket_count(map_count);
            int32_t map_bucket_count = map->bucket_count;

            if (bucket_count == BUCKET_COUNT) {
                rehash(map, map_bucket_count * 2);
            } else {
                if (target_bucket_count > map_bucket_count) {
                    rehash(map, target_bucket_count);
                }
            }            
        }
    } else {
        *found = key;
        ecs_os_memcpy(PAYLOAD(found), payload, elem_size);
    }

    ecs_assert(map->bucket_count != 0, ECS_INTERNAL_ERROR, NULL);
}

void ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = find_bucket(map, key);
    if (!bucket) {
        return;
    }
   
    ecs_size_t elem_size = map->elem_size;
    void *array = PAYLOAD_ARRAY(bucket, map->offset);
    ecs_map_key_t *elem = array;
    int32_t bucket_count = bucket->count;

    ecs_assert(bucket_count > 0, ECS_INTERNAL_ERROR, NULL);

    uint8_t i = 0;
    do {
        if (*elem == key) {
            ecs_map_key_t *last_elem = GET_ELEM(array, elem_size, (bucket_count - 1));
            if (last_elem > elem) {
                ecs_os_memcpy(elem, last_elem, ELEM_SIZE(elem_size));
            }

            map->count --;
            if (!--bucket->count) {
                remove_bucket(map, key);
            }

            break;
        }

        elem = NEXT_ELEM(elem, elem_size);
    } while (++i < bucket_count);   
}

int32_t ecs_map_count(
    const ecs_map_t *map)
{
    return map ? map->count : 0;
}

int32_t ecs_map_bucket_count(
    const ecs_map_t *map)
{
    return map ? map->bucket_count : 0;
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_sparse_clear(map->buckets);
    map->count = 0;
}

ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    return (ecs_map_iter_t){
        .map = map,
        .bucket = NULL,
        .bucket_index = 0,
        .element_index = 0
    };
}

void* _ecs_map_next(
    ecs_map_iter_t *iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key_out)
{
    const ecs_map_t *map = iter->map;
    
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!elem_size || elem_size == map->type_elem_size, ECS_INVALID_PARAMETER, NULL);
 
    ecs_bucket_t *bucket = iter->bucket;
    int32_t element_index = iter->element_index;
    elem_size = map->elem_size;

    do {
        if (!bucket) {
            int32_t bucket_index = iter->bucket_index;
            ecs_sparse_t *buckets = map->buckets;
            if (bucket_index < ecs_sparse_count(buckets)) {
                bucket = _ecs_sparse_get(buckets, 0, bucket_index);
                iter->bucket = bucket;

                element_index = 0;
                iter->element_index = 0;
            } else {
                return NULL;
            }
        }

        if (element_index < bucket->count) {
            iter->element_index = element_index + 1;
            break;
        } else {
            bucket = NULL;
            iter->bucket_index ++;
        }
    } while (true);

    void *array = PAYLOAD_ARRAY(bucket, map->offset);
    ecs_map_key_t *elem = GET_ELEM(array, elem_size, element_index);
    
    if (key_out) {
        *key_out = *elem;
    }

    return PAYLOAD(elem);
}

void* _ecs_map_next_ptr(
    ecs_map_iter_t *iter,
    ecs_map_key_t *key_out)
{
    void *result = _ecs_map_next(iter, ECS_SIZEOF(void*), key_out);
    if (result) {
        return *(void**)result;
    } else {
        return NULL;
    }
}

void ecs_map_grow(
    ecs_map_t *map, 
    int32_t element_count)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t target_count = map->count + element_count;
    int32_t bucket_count = get_bucket_count(target_count);

    if (bucket_count > map->bucket_count) {
        rehash(map, bucket_count);
    }
}

void ecs_map_set_size(
    ecs_map_t *map, 
    int32_t element_count)
{    
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t bucket_count = get_bucket_count(element_count);

    if (bucket_count) {
        rehash(map, bucket_count);
    }
}

void ecs_map_memory(
    ecs_map_t *map, 
    int32_t *allocd,
    int32_t *used)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_sparse_memory(map->buckets, allocd, NULL);

    if (used) {
        *used = map->count * ELEM_SIZE(map->elem_size);
    }
}

ecs_map_t* ecs_map_copy(
    const ecs_map_t *src)
{
    if (!src) {
        return NULL;
    }
    
    ecs_map_t *dst = ecs_os_memdup(src, ECS_SIZEOF(ecs_map_t));
    
    dst->buckets = ecs_sparse_copy(src->buckets);

    return dst;
}

static
void* get_owned_column_ptr(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t table_column,
    int32_t row)
{
    ecs_assert(it->table_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_column_t *column = &((ecs_column_t*)it->table_columns)[table_column - 1];
    ecs_assert(column->size != 0, ECS_COLUMN_HAS_NO_DATA, NULL);
    ecs_assert(!size || column->size == size, ECS_COLUMN_TYPE_MISMATCH, NULL);
    void *buffer = ecs_vector_first_t(column->data, column->size, column->alignment);
    return ECS_OFFSET(buffer, column->size * (it->offset + row));
}

static
const void* get_shared_column(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t table_column)
{
    ecs_assert(it->references != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)size;

#ifndef NDEBUG
    if (size) {
        const EcsComponent *cdata = ecs_get(
            it->world, it->references[-table_column - 1].component, 
            EcsComponent);

        ecs_assert(cdata != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdata->size == size, ECS_COLUMN_TYPE_MISMATCH, 
            ecs_get_name(it->world, it->system));
    }
#endif

    ecs_ref_t *ref = &it->references[-table_column - 1];

    return (void*)ecs_get_ref_w_entity(
        it->world, ref, ref->entity, ref->component);
}

static
bool get_table_column(
    const ecs_iter_t *it,
    int32_t column,
    int32_t *table_column_out)
{
    ecs_assert(column <= it->column_count, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = 0;

    if (column != 0) {
        ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table_column = it->columns[column - 1];
        if (!table_column) {
            /* column is not set */
            return false;
        }
    }

    *table_column_out = table_column;

    return true;
}

static
void* get_column(
    const ecs_iter_t *it,
    ecs_size_t size,
    int32_t column,
    int32_t row)
{
    int32_t table_column;

    if (!column) {
        return it->entities;
    }

    if (!get_table_column(it, column, &table_column)) {
        return NULL;
    }

    if (table_column < 0) {
        return (void*)get_shared_column(it, size, table_column);
    } else {
        return get_owned_column_ptr(it, size, table_column, row);
    }
}


/* --- Public API --- */

void* ecs_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column)
{
    return get_column(it, ecs_from_size_t(size), column, 0);
}

void* ecs_element_w_size(
    const ecs_iter_t *it, 
    size_t size,
    int32_t column,
    int32_t row)
{
    return get_column(it, ecs_from_size_t(size), column, row);
}

bool ecs_is_owned(
    const ecs_iter_t *it,
    int32_t column)
{
    int32_t table_column;

    if (!get_table_column(it, column, &table_column)) {
        return true;
    }

    return table_column >= 0;
}

bool ecs_is_readonly(
    const ecs_iter_t *it,
    int32_t column)
{
    if (!ecs_is_owned(it, column)) {
        return true;
    }

    ecs_query_t *query = it->query;
    if (query) {
        ecs_sig_column_t *column_data = ecs_vector_get(
            it->query->sig.columns, ecs_sig_column_t, column - 1);
        return column_data->inout_kind == EcsIn;
    } else {
        return true;
    }
}

ecs_entity_t ecs_column_source(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t table_column = it->columns[index - 1];
    if (table_column >= 0) {
        return 0;
    }

    ecs_assert(it->references != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_ref_t *ref = &it->references[-table_column - 1];

    return ref->entity;
}

ecs_type_t ecs_column_type(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->components != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t component = it->components[index - 1];
    return ecs_type_from_entity(it->world, component);
}

ecs_entity_t ecs_column_entity(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->components != NULL, ECS_INTERNAL_ERROR, NULL);

    return it->components[index - 1];
}

ecs_type_t ecs_iter_type(
    const ecs_iter_t *it)
{
    ecs_table_t *table = it->table;
    return table->type;
}

int32_t ecs_table_component_index(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    return ecs_type_index_of(it->table->type, component);
}

void* ecs_table_column(
    const ecs_iter_t *it,
    int32_t column_index)
{
    ecs_table_t *table = it->table;

    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);

    if (table->column_count < column_index) {
        return NULL;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    return ecs_vector_first_t(column->data, column->size, column->alignment);
}

size_t ecs_table_column_size(
    const ecs_iter_t *it,
    int32_t column_index)
{
    ecs_table_t *table = it->table;

    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);

    if (table->column_count < column_index) {
        return 0;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    
    return ecs_to_size_t(column->size);
}

int8_t ecs_to_i8(
    int64_t v)
{
    ecs_assert(v < INT8_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int8_t)v;
}

int16_t ecs_to_i16(
    int64_t v)
{
    ecs_assert(v < INT16_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int16_t)v;
}

int32_t ecs_to_i32(
    int64_t v)
{
    ecs_assert(v < INT32_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int32_t)v;
}

size_t ecs_to_size_t(
    int64_t size)
{
    ecs_assert(size >= 0, ECS_INTERNAL_ERROR, NULL);
    return (size_t)size;
}

ecs_size_t ecs_from_size_t(
    size_t size)
{
   ecs_assert(size < INT64_MAX, ECS_INTERNAL_ERROR, NULL); 
   return (ecs_size_t)size;
}

ecs_entity_t ecs_to_entity(
    int64_t v)
{
    ecs_assert(v >= 0, ECS_INTERNAL_ERROR, NULL);
    return (ecs_entity_t)v;
}

/** Convert time to double */
double ecs_time_to_double(
    ecs_time_t t)
{
    double result;
    result = t.sec;
    return result + (double)t.nanosec / (double)1000000000;;
}

ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2)
{
    ecs_time_t result;

    if (t1.nanosec >= t2.nanosec) {
        result.nanosec = t1.nanosec - t2.nanosec;
        result.sec = t1.sec - t2.sec;
    } else {
        result.nanosec = t1.nanosec - t2.nanosec + 1000000000;
        result.sec = t1.sec - t2.sec - 1;
    }

    return result;
}

void ecs_sleepf(
    double t)
{
    if (t > 0) {
        int sec = (int)t;
        int nsec = (int)((t - sec) * 1000000000);
        ecs_os_sleep(sec, nsec);
    }
}

double ecs_time_measure(
    ecs_time_t *start)
{
    ecs_time_t stop, temp;
    ecs_os_get_time(&stop);
    temp = stop;
    stop = ecs_time_sub(stop, *start);
    *start = temp;
    return ecs_time_to_double(stop);
}

void* ecs_os_memdup(
    const void *src, 
    ecs_size_t size) 
{
    if (!src) {
        return NULL;
    }
    
    void *dst = ecs_os_malloc(size);
    ecs_assert(dst != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_os_memcpy(dst, src, size);  
    return dst;  
}

/*
    This code was taken from sokol_time.h 
    
    zlib/libpng license
    Copyright (c) 2018 Andre Weissflog
    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.
        3. This notice may not be removed or altered from any source
        distribution.
*/


static int ecs_os_time_initialized;

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
static double _ecs_os_time_win_freq;
static LARGE_INTEGER _ecs_os_time_win_start;
#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
static mach_timebase_info_data_t _ecs_os_time_osx_timebase;
static uint64_t _ecs_os_time_osx_start;
#else /* anything else, this will need more care for non-Linux platforms */
#include <time.h>
static uint64_t _ecs_os_time_posix_start;
#endif

/* prevent 64-bit overflow when computing relative timestamp
    see https://gist.github.com/jspohr/3dc4f00033d79ec5bdaf67bc46c813e3
*/
#if defined(_WIN32) || (defined(__APPLE__) && defined(__MACH__))
int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

void ecs_os_time_setup(void) {
    if ( ecs_os_time_initialized) {
        return;
    }
    
    ecs_os_time_initialized = 1;
    #if defined(_WIN32)
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&_ecs_os_time_win_start);
        _ecs_os_time_win_freq = (double)freq.QuadPart / 1000000000.0;
    #elif defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&_ecs_os_time_osx_timebase);
        _ecs_os_time_osx_start = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        _ecs_os_time_posix_start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec; 
    #endif
}

uint64_t ecs_os_time_now(void) {
    ecs_assert(ecs_os_time_initialized != 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t now;

    #if defined(_WIN32)
        LARGE_INTEGER qpc_t;
        QueryPerformanceCounter(&qpc_t);
        now = (uint64_t)(qpc_t.QuadPart / _ecs_os_time_win_freq);
    #elif defined(__APPLE__) && defined(__MACH__)
        now = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec);
    #endif

    return now;
}

void ecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec) 
{
#ifndef _WIN32
    struct timespec sleepTime;
    ecs_assert(sec >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(nanosec >= 0, ECS_INTERNAL_ERROR, NULL);

    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nanosec;
    if (nanosleep(&sleepTime, NULL)) {
        ecs_os_err("nanosleep failed");
    }
#else
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -((int64_t)sec * 10000000 + (int64_t)nanosec / 100);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#endif
}


#if defined(_WIN32)

static ULONG win32_current_resolution;

void ecs_increase_timer_resolution(bool enable)
{
    HMODULE hntdll = GetModuleHandle((LPCTSTR)"ntdll.dll");
    if (!hntdll) {
        return;
    }

    LONG (__stdcall *pNtSetTimerResolution)(
        ULONG desired, BOOLEAN set, ULONG * current);

    pNtSetTimerResolution = (LONG(__stdcall*)(ULONG, BOOLEAN, ULONG*))
        GetProcAddress(hntdll, "NtSetTimerResolution");

    if(!pNtSetTimerResolution) {
        return;
    }

    ULONG current, resolution = 10000; /* 1 ms */

    if (!enable && win32_current_resolution) {
        pNtSetTimerResolution(win32_current_resolution, 0, &current);
        win32_current_resolution = 0;
        return;
    } else if (!enable) {
        return;
    }

    if (resolution == win32_current_resolution) {
        return;
    }

    if (win32_current_resolution) {
        pNtSetTimerResolution(win32_current_resolution, 0, &current);
    }

    if (pNtSetTimerResolution(resolution, 1, &current)) {
        /* Try setting a lower resolution */
        resolution *= 2;
        if(pNtSetTimerResolution(resolution, 1, &current)) return;
    }

    win32_current_resolution = resolution;
}

#else
void ecs_increase_timer_resolution(bool enable)
{
    (void)enable;
    return;
}
#endif

#ifdef FLECS_PIPELINE

#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H


/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline and contains
 * information about the set of systems that need to be ran before a merge. */
typedef struct ecs_pipeline_op_t {
    int32_t count;              /**< Number of systems to run before merge */
} ecs_pipeline_op_t;

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline);

int32_t ecs_pipeline_begin(
    ecs_world_t *world,
    ecs_entity_t pipeline);

void ecs_pipeline_end(
    ecs_world_t *world);

void ecs_pipeline_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    float delta_time);


////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

void ecs_worker_begin(
    ecs_world_t *world);

bool ecs_worker_sync(
    ecs_world_t *world);

void ecs_worker_end(
    ecs_world_t *world);

void ecs_workers_progress(
    ecs_world_t *world);

#endif

/* Worker thread */
static
void* worker(void *arg) {
    ecs_thread_t *thread = arg;
    ecs_world_t *world = thread->world;

    /* Start worker thread, increase counter so main thread knows how many
     * workers are ready */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!world->quit_workers) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!world->quit_workers) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)thread, 0);
        ecs_pipeline_progress(
            (ecs_world_t*)thread, 
            world->pipeline, 
            world->stats.delta_time);
        ecs_set_scope((ecs_world_t*)thread, old_scope);
    }

    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running --;
    ecs_os_mutex_unlock(world->sync_mutex);

    return NULL;
}

/* Start threads */
static
void start_workers(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_assert(world->workers == NULL, ECS_INTERNAL_ERROR, NULL);

    world->workers = ecs_vector_new(ecs_thread_t, threads);
    world->worker_stages = ecs_vector_new(ecs_stage_t, threads);

    int32_t i;
    for (i = 0; i < threads; i ++) {
        ecs_thread_t *thread =
            ecs_vector_add(&world->workers, ecs_thread_t);

        thread->magic = ECS_THREAD_MAGIC;
        thread->world = world;
        thread->thread = 0;
        thread->index = i;

        thread->stage = ecs_vector_add(&world->worker_stages, ecs_stage_t);
        ecs_stage_init(world, thread->stage);
        thread->stage->id = 2 + i; /* 0 and 1 are reserved for main and temp */
        thread->stage->world = (ecs_world_t*)thread;

        thread->thread = ecs_os_thread_new(worker, thread);
        ecs_assert(thread->thread != 0, ECS_THREAD_ERROR, NULL);
    }
}

/* Wait until all workers are running */
static
void wait_for_workers(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    bool wait = true;

    do {
        ecs_os_mutex_lock(world->sync_mutex);
        if (world->workers_running == thread_count) {
            wait = false;
        }
        ecs_os_mutex_unlock(world->sync_mutex);
    } while (wait);
}

/* Synchronize worker threads */
static
void sync_worker(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++ world->workers_waiting == thread_count) {
        /* Only signal main thread when all threads are waiting */
        ecs_os_cond_signal(world->sync_cond);
    }

    /* Wait until main thread signals that thread can continue */
    ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Wait until all threads are waiting on sync point */
static
void wait_for_sync(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);

    ecs_os_mutex_lock(world->sync_mutex);
    if (world->workers_waiting != thread_count) {
        ecs_os_cond_wait(world->sync_cond, world->sync_mutex);
    }
    
    /* We should have been signalled unless all workers are waiting on sync */
    ecs_assert(world->workers_waiting == thread_count, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Signal workers that they can start/resume work */
static
void signal_workers(
    ecs_world_t *world)
{
    ecs_os_mutex_lock(world->sync_mutex);
    ecs_os_cond_broadcast(world->worker_cond);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/** Stop worker threads */
static
void ecs_stop_threads(
    ecs_world_t *world)
{
    world->quit_workers = true;
    signal_workers(world);

    ecs_vector_each(world->workers, ecs_thread_t, thr, {
        ecs_os_thread_join(thr->thread);
        ecs_stage_deinit(world, thr->stage);
    });

    ecs_vector_free(world->workers);
    ecs_vector_free(world->worker_stages);
    world->worker_stages = NULL;
    world->workers = NULL;
    world->quit_workers = false;
    
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);
}

/* -- Private functions -- */

void ecs_worker_begin(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_begin(world);
    }
}

bool ecs_worker_sync(
    ecs_world_t *world)
{
    int32_t build_count = world->stats.pipeline_build_count_total;

    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_end(world, false);

        ecs_pipeline_update(world, world->pipeline);

        ecs_staging_begin(world);
    } else {
        sync_worker(world);
    }

    return world->stats.pipeline_build_count_total != build_count;
}

void ecs_worker_end(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_end(world, false);
    } else {
        sync_worker(world);
    }
}

void ecs_workers_progress(
    ecs_world_t *world)
{
    ecs_entity_t pipeline = world->pipeline;
    int32_t thread_count = ecs_vector_count(world->workers);

    ecs_time_t start = {0};
    if (world->measure_frame_time) {
        ecs_time_measure(&start);
    }

    world->stats.systems_ran_frame = 0;

    if (thread_count <= 1) {
        ecs_pipeline_begin(world, pipeline);
        ecs_entity_t old_scope = ecs_set_scope(world, 0);
        ecs_pipeline_progress(world, pipeline, world->stats.delta_time);
        ecs_set_scope(world, old_scope);
        ecs_pipeline_end(world);
    } else {
        int32_t i, sync_count = ecs_pipeline_begin(world, pipeline);

        /* Make sure workers are running and ready */
        wait_for_workers(world);

        /* Synchronize n times for each op in the pipeline */
        for (i = 0; i < sync_count; i ++) {
            ecs_staging_begin(world);

            /* Signal workers that they should start running systems */
            world->workers_waiting = 0;
            signal_workers(world);

            /* Wait until all workers are waiting on sync point */
            wait_for_sync(world);

            /* Merge */
            ecs_staging_end(world, false);

            int32_t update_count;
            if ((update_count = ecs_pipeline_update(world, pipeline))) {
                /* The number of operations in the pipeline could have changed
                 * as result of the merge */
                sync_count = update_count;
            }
        }

        ecs_pipeline_end(world);
    }

    if (world->measure_frame_time) {
        world->stats.system_time_total += (float)ecs_time_measure(&start);
    }    
}


/* -- Public functions -- */

void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_assert(!threads || ecs_os_api.thread_new, ECS_MISSING_OS_API, "thread_new");
    ecs_assert(!threads || ecs_os_api.thread_join, ECS_MISSING_OS_API, "thread_join");
    ecs_assert(!threads || ecs_os_api.mutex_new, ECS_MISSING_OS_API, "mutex_new");
    ecs_assert(!threads || ecs_os_api.mutex_free, ECS_MISSING_OS_API, "mutex_free");
    ecs_assert(!threads || ecs_os_api.mutex_lock, ECS_MISSING_OS_API, "mutex_lock");
    ecs_assert(!threads || ecs_os_api.mutex_unlock, ECS_MISSING_OS_API, "mutex_unlock");
    ecs_assert(!threads || ecs_os_api.cond_new, ECS_MISSING_OS_API, "cond_new");
    ecs_assert(!threads || ecs_os_api.cond_free, ECS_MISSING_OS_API, "cond_free");
    ecs_assert(!threads || ecs_os_api.cond_wait, ECS_MISSING_OS_API, "cond_wait");
    ecs_assert(!threads || ecs_os_api.cond_signal, ECS_MISSING_OS_API, "cond_signal");
    ecs_assert(!threads || ecs_os_api.cond_broadcast, ECS_MISSING_OS_API, "cond_broadcast");

    int32_t thread_count = ecs_vector_count(world->workers);

    if (!world->arg_threads && thread_count != threads) {
        /* Stop existing threads */
        if (ecs_vector_count(world->workers)) {
            ecs_stop_threads(world);
            ecs_os_cond_free(world->worker_cond);
            ecs_os_cond_free(world->sync_cond);
            ecs_os_mutex_free(world->sync_mutex);
        }

        /* Start threads if number of threads > 1 */
        if (threads > 1) {
            world->worker_cond = ecs_os_cond_new();
            world->sync_cond = ecs_os_cond_new();
            world->sync_mutex = ecs_os_mutex_new();
            world->stage_count = 2 + threads;
            start_workers(world, threads);
        }

        /* Iterate tables, make sure the ecs_data_t arrays are large enough */
        ecs_sparse_each(world->stage.tables, ecs_table_t, table, {
            ecs_table_get_data(world, table);
        });
    }
}

#endif

#ifdef FLECS_PIPELINE


ECS_TYPE_DECL(EcsPipelineQuery);

typedef struct EcsPipelineQuery {
    ecs_query_t *query;
    ecs_query_t *build_query;
    int32_t match_count;
    ecs_vector_t *ops;
} EcsPipelineQuery;

ECS_CTOR(EcsPipelineQuery, ptr, {
    memset(ptr, 0, _size);
})

ECS_DTOR(EcsPipelineQuery, ptr, {
    ecs_vector_free(ptr->ops);
})

static
int compare_entity(
    ecs_entity_t e1, 
    void *ptr1, 
    ecs_entity_t e2, 
    void *ptr2) 
{
    (void)ptr1;
    (void)ptr2;
    return (e1 > e2) - (e1 < e2);
}

static
int rank_phase(
    ecs_world_t *world,
    ecs_entity_t rank_component,
    ecs_type_t type) 
{
    const EcsType *pipeline_type = ecs_get(world, rank_component, EcsType);
    ecs_assert(pipeline_type != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Find tag in system that belongs to pipeline */
    ecs_entity_t *sys_comps = ecs_vector_first(type, ecs_entity_t);
    int32_t c, t, count = ecs_vector_count(type);

    ecs_entity_t *tags = ecs_vector_first(pipeline_type->normalized, ecs_entity_t);
    int32_t tag_count = ecs_vector_count(pipeline_type->normalized);

    ecs_entity_t result = 0;

    for (c = 0; c < count; c ++) {
        ecs_entity_t comp = sys_comps[c];
        for (t = 0; t < tag_count; t ++) {
            if (comp == tags[t]) {
                result = comp;
                break;
            }
        }
        if (result) {
            break;
        }
    }

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result < INT_MAX, ECS_INTERNAL_ERROR, NULL);

    return (int)result;
}

typedef enum ComponentWriteState {
    NotWritten = 0,
    WriteToMain,
    WriteToStage
} ComponentWriteState;

static
int32_t get_write_state(
    ecs_map_t *write_state,
    ecs_entity_t component)
{
    int32_t *ptr = ecs_map_get(write_state, int32_t, component);
    if (ptr) {
        return *ptr;
    } else {
        return 0;
    }
}

static
void set_write_state(
    ecs_map_t *write_state,
    ecs_entity_t component,
    int32_t value)
{
    ecs_map_set(write_state, component, &value);
}

static
void reset_write_state(
    ecs_map_t *write_state)
{
    ecs_map_clear(write_state);
}

static
bool check_column_component(
    ecs_sig_column_t *column,
    bool is_active,
    ecs_entity_t component,
    ecs_map_t *write_state)    
{
    int32_t state = get_write_state(write_state, component);

    if ((column->from_kind == EcsFromAny || column->from_kind == EcsFromOwned) && column->oper_kind != EcsOperNot) {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsIn:
            if (state == WriteToStage) {
                return true;
            }
            // fall through
        case EcsOut:
            if (is_active && column->inout_kind != EcsIn) {
                set_write_state(write_state, component, WriteToMain);
            }
        };
    } else if (column->from_kind == EcsFromEmpty || column->oper_kind == EcsOperNot) {
        switch(column->inout_kind) {
        case EcsInOut:
        case EcsOut:
            if (is_active) {
                set_write_state(write_state, component, WriteToStage);
            }
            break;
        default:
            break;
        };
    }

    return false;
}

static
bool check_column(
    ecs_sig_column_t *column,
    bool is_active,
    ecs_map_t *write_state)
{
    if (column->oper_kind != EcsOperOr) {
        return check_column_component(
            column, is_active,column->is.component, write_state);
    }  

    return false;
}

static
bool build_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    EcsPipelineQuery *pq)
{
    (void)pipeline;

    ecs_query_iter(pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    ecs_trace_1("rebuilding pipeline #[green]%s", 
        ecs_get_name(world, pipeline));

    world->stats.pipeline_build_count_total ++;

    ecs_map_t *write_state = ecs_map_new(int32_t, ECS_HI_COMPONENT_ID);
    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->build_query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_iter_t it = ecs_query_iter(query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_column(&it, EcsSystem, 1);

        int i;
        for (i = 0; i < it.count; i ++) {
            ecs_query_t *q = sys[i].query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            bool is_active = !ecs_has_entity(
                world, it.entities[i], EcsInactive);

            ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                needs_merge |= check_column(column, is_active, write_state);
            });

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                reset_write_state(write_state);
                op = NULL;

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    ecs_vector_each(q->sig.columns, ecs_sig_column_t, column, {
                        needs_merge |= check_column(column, true, write_state);
                    });
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                op->count ++;
            }
        }
    }

    ecs_map_free(write_state);

    /* Force sort of query as this could increase the match_count */
    pq->match_count = pq->query->match_count;
    pq->ops = ops;

    return true;
}

static
int32_t iter_reset(
    const EcsPipelineQuery *pq,
    ecs_iter_t *iter_out,
    ecs_pipeline_op_t **op_out,
    ecs_entity_t move_to)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    ecs_iter_t it = ecs_query_iter(pq->query);
    while (ecs_query_next(&it)) {
        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];

            ran_since_merge ++;
            if (ran_since_merge == op->count) {
                ran_since_merge = 0;
                op ++;
            }

            if (e == move_to) {
                *iter_out = it;
                *op_out = op;
                return i;
            }
        }
    }

    ecs_abort(ECS_UNSUPPORTED, NULL);

    return -1;
}

int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    EcsPipelineQuery *pq = ecs_get_mut(world, pipeline, EcsPipelineQuery, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    if (build_pipeline(world, pipeline, pq)) {
        return ecs_vector_count(pq->ops);
    } else {
        return 0;
    }
}

int32_t ecs_pipeline_begin(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    ecs_eval_component_monitors(world);

    EcsPipelineQuery *pq = ecs_get_mut(
        world, pipeline, EcsPipelineQuery, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    build_pipeline(world, pipeline, pq);

    return ecs_vector_count(pq->ops);
}

void ecs_pipeline_end(
    ecs_world_t *world)
{
    (void)world;
}

void ecs_pipeline_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    float delta_time)
{
    const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    ecs_worker_begin(world);
    
    ecs_iter_t it = ecs_query_iter(pq->query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_column(&it, EcsSystem, 1);

        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];
            
            ecs_run_intern(world, stage, e, &sys[i], delta_time, 0, 0, 
                NULL, NULL, false);

            ran_since_merge ++;
            world->stats.systems_ran_frame ++;

            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;
                op++;

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                if (ecs_worker_sync(world)) {
                    i = iter_reset(pq, &it, &op, e);
                    op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
                    sys = ecs_column(&it, EcsSystem, 1);
                }
            }
        }
    }

    ecs_worker_end(world);
}

static
void add_pipeline_tags_to_sig(
    ecs_world_t *world,
    ecs_sig_t *sig,
    ecs_type_t type)
{
    (void)world;
    
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        if (!i) {
            ecs_sig_add(sig, EcsFromAny, EcsOperAnd, EcsIn, entities[i], 0);
        } else {
            ecs_sig_add(sig, EcsFromAny, EcsOperOr, EcsIn, entities[i], 0);
        }
    }
}

static 
void EcsOnAddPipeline(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    int32_t i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        ecs_sig_t sig = { 0 };

        const EcsType *type_ptr = ecs_get(world, pipeline, EcsType);
        ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
        
#ifndef NDEBUG
        char *str = ecs_type_str(world, type_ptr->normalized);
        ecs_trace_1("pipeline #[green]%s#[normal] created with #[red][%s]",
            ecs_get_name(world, pipeline), str);
        ecs_os_free(str);
#endif
        ecs_log_push();

        /* Build signature for pipeline quey that matches EcsSystems, has the
         * pipeline as a XOR column, and ignores systems with EcsInactive and
         * EcsDisabledIntern. Note that EcsDisabled is automatically ignored by
         * the regular query matching */
        ecs_sig_add(&sig, EcsFromAny, EcsOperAnd, EcsIn, ecs_entity(EcsSystem), 0);
        ecs_sig_add(&sig, EcsFromAny, EcsOperNot, EcsIn, EcsInactive, 0);
        ecs_sig_add(&sig, EcsFromAny, EcsOperNot, EcsIn, EcsDisabledIntern, 0);
        add_pipeline_tags_to_sig(world, &sig, type_ptr->normalized);

        /* Create the query. Sort the query by system id and phase */
        ecs_query_t *query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_order_by(world, query, 0, compare_entity);
        ecs_query_group_by(world, query, pipeline, rank_phase);

        /* Build signature for pipeline build query. The build query includes
         * systems that are inactive, as an inactive system may become active as
         * a result of another system, and as a result the correct merge 
         * operations need to be put in place. */
        ecs_sig_add(&sig, EcsFromAny, EcsOperAnd, EcsIn, ecs_entity(EcsSystem), 0);
        ecs_sig_add(&sig, EcsFromAny, EcsOperNot, EcsIn, EcsDisabledIntern, 0);
        add_pipeline_tags_to_sig(world, &sig, type_ptr->normalized);

        /* Use the same sorting functions for the build query */
        ecs_query_t *build_query = ecs_query_new_w_sig(world, 0, &sig);
        ecs_query_order_by(world, build_query, 0, compare_entity);
        ecs_query_group_by(world, build_query, pipeline, rank_phase);       

        EcsPipelineQuery *pq = ecs_get_mut(
            world, pipeline, EcsPipelineQuery, NULL);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        pq->query = query;
        pq->build_query = build_query;
        pq->match_count = -1;
        pq->ops = NULL;

        ecs_log_pop();
    }
}

static
double insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    ecs_time_t start = *stop;
    double delta_time = ecs_time_measure(stop);

    if (world->stats.target_fps == 0) {
        return delta_time;
    }

    double target_delta_time = (1.0 / world->stats.target_fps);
    double world_sleep_err = 
        world->stats.sleep_err / (double)world->stats.frame_count_total;

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    double sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 20 times lower than the time one frame
     * should take. This means that this function at most iterates 20 times in
     * a busy loop */
    double sleep_time = target_delta_time / 20;

    /* Measure at least two frames before interpreting sleep error */
    if (world->stats.frame_count_total > 1) {
        /* If the ratio between the sleep error and the sleep time is too high,
         * just do a busy loop */
        if (world_sleep_err / sleep_time > 0.1) {
            sleep_time = 0;
        } 
    }

    /* If the time we need to sleep is large enough to warrant a sleep, sleep */
    if (sleep > (sleep_time - world_sleep_err)) {
        if (sleep_time > sleep) {
            /* Make sure we don't sleep longer than we should */
            sleep_time = sleep;
        }

        double sleep_err = 0;
        int32_t iterations = 0;

        do {
            /* Only call sleep when sleep_time is not 0. On some platforms, even
             * a sleep with a timeout of 0 can cause stutter. */
            if (sleep_time != 0) {
                ecs_sleepf(sleep_time);
            }

            ecs_time_t now = start;
            double prev_delta_time = delta_time;
            delta_time = ecs_time_measure(&now);

            /* Measure the error of the sleep by taking the difference between 
             * the time we expected to sleep, and the measured time. This 
             * assumes that a sleep is less accurate than a high resolution 
             * timer which should be true in most cases. */
            sleep_err = delta_time - prev_delta_time - sleep_time;
            iterations ++;
        } while ((target_delta_time - delta_time) > (sleep_time - world_sleep_err));

        /* Add sleep error measurement to sleep error, with a bias towards the
         * latest measured values. */
        world->stats.sleep_err = (float)
            (world_sleep_err * 0.9 + sleep_err * 0.1) * 
                (float)world->stats.frame_count_total;
    }

    /*  Make last minute corrections if due to a larger clock error delta_time
     * is still more than 5% away from the target. The 5% buffer is to account
     * for the fact that measuring the time also takes time. */
    while (delta_time < target_delta_time * 0.95) {
        ecs_time_t now = start;
        delta_time = ecs_time_measure(&now);
    }

    return delta_time;
}

static
float start_measure_frame(
    ecs_world_t *world,
    float user_delta_time)
{
    double delta_time = 0;

    if (world->measure_frame_time || (user_delta_time == 0)) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.sec) {
                delta_time = insert_sleep(world, &t);

                ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->stats.target_fps != 0) {
                    delta_time = 1.0 / world->stats.target_fps;
                } else {
                    delta_time = 1.0 / 60.0; /* Best guess */
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Keep track of total time passed in world */
        world->stats.world_time_total_raw += (float)delta_time;
    }

    return (float)delta_time;
}

static
void stop_measure_frame(
    ecs_world_t* world)
{
    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        world->stats.frame_time_total += (float)ecs_time_measure(&t);
    }
}

/* -- Public API -- */

float ecs_frame_begin(
    ecs_world_t *world,
    float user_delta_time)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(user_delta_time != 0 || ecs_os_api.get_time, ECS_MISSING_OS_API, "get_time");

    if (world->locking_enabled) {
        ecs_lock(world);
    }

    /* Start measuring total frame time */
    float delta_time = start_measure_frame(world, user_delta_time);
    if (user_delta_time == 0) {
        user_delta_time = delta_time;
    }

    world->stats.delta_time_raw = user_delta_time;
    world->stats.delta_time = user_delta_time * world->stats.time_scale;

    /* Keep track of total scaled time passed in world */
    world->stats.world_time_total += world->stats.delta_time;
    
    return user_delta_time;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    world->stats.frame_count_total ++;

    if (world->locking_enabled) {
        ecs_unlock(world);

        ecs_os_mutex_lock(world->thr_sync);
        ecs_os_cond_broadcast(world->thr_cond);
        ecs_os_mutex_unlock(world->thr_sync);
    }

    stop_measure_frame(world);
}

bool ecs_progress(
    ecs_world_t *world,
    float user_delta_time)
{
    ecs_frame_begin(world, user_delta_time);

    ecs_workers_progress(world);

    ecs_frame_end(world);

    return !world->should_quit;
}

void ecs_set_time_scale(
    ecs_world_t *world,
    float scale)
{
    world->stats.time_scale = scale;
}

void ecs_reset_clock(
    ecs_world_t *world)
{
    world->stats.world_time_total = 0;
    world->stats.world_time_total_raw = 0;
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_get_stage(&world);
    world->should_quit = true;
}

void ecs_deactivate_systems(
    ecs_world_t *world)
{
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t pipeline = world->pipeline;
    const EcsPipelineQuery *pq = ecs_get( world, pipeline, EcsPipelineQuery);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Iterate over all systems, add EcsInvalid tag if queries aren't matched
     * with any tables */
    ecs_iter_t it = ecs_query_iter(pq->build_query);

    /* Make sure that we defer adding the inactive tags until after iterating
     * the query */
    ecs_defer_begin(world, &world->stage, EcsOpNone, 0, NULL, NULL, 0);

    while( ecs_query_next(&it)) {
        EcsSystem *sys = ecs_column(&it, EcsSystem, 1);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_query_t *query = sys[i].query;
            if (query) {
                if (!ecs_vector_count(query->tables)) {
                    ecs_add_entity(world, it.entities[i], EcsInactive);
                }
            }
        }
    }

    ecs_defer_end(world, &world->stage);
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_assert( ecs_get(world, pipeline, EcsPipelineQuery) != NULL, 
        ECS_INVALID_PARAMETER, NULL);

    world->pipeline = pipeline;
}

ecs_entity_t ecs_get_pipeline(
    ecs_world_t *world)
{
    return world->pipeline;
}

ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t result = ecs_new_type(world, e, name, expr);
    ecs_assert(ecs_get(world, result, EcsType) != NULL, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_add_entity(world, result, EcsPipeline);

    return result;
}

/* -- Module implementation -- */

static
void FlecsPipelineFini(
    ecs_world_t *world,
    void *ctx)
{
    (void)ctx;
    if (world->workers) {
        ecs_set_threads(world, 0);
    }
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);

    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_tag(world, EcsPipeline);
    ecs_bootstrap_component(world, EcsPipelineQuery);

    /* Phases of the builtin pipeline are regular entities. Names are set so
     * they can be resolved by type expressions. */
    ecs_bootstrap_tag(world, EcsPreFrame);
    ecs_bootstrap_tag(world, EcsOnLoad);
    ecs_bootstrap_tag(world, EcsPostLoad);
    ecs_bootstrap_tag(world, EcsPreUpdate);
    ecs_bootstrap_tag(world, EcsOnUpdate);
    ecs_bootstrap_tag(world, EcsOnValidate);
    ecs_bootstrap_tag(world, EcsPostUpdate);
    ecs_bootstrap_tag(world, EcsPreStore);
    ecs_bootstrap_tag(world, EcsOnStore);
    ecs_bootstrap_tag(world, EcsPostFrame);

    ECS_TYPE_IMPL(EcsPipelineQuery);

    /* Set ctor and dtor for PipelineQuery */
    ecs_set(world, ecs_entity(EcsPipelineQuery), EcsComponentLifecycle, {
        .ctor = ecs_ctor(EcsPipelineQuery),
        .dtor = ecs_dtor(EcsPipelineQuery)
    });

    /* When the Pipeline tag is added a pipeline will be created */
    ECS_TRIGGER(world, EcsOnAddPipeline, EcsOnAdd, Pipeline);

    /* Create the builtin pipeline */
    world->pipeline = ecs_new_pipeline(world, 0, "BuiltinPipeline",
        "PreFrame, OnLoad, PostLoad, PreUpdate, OnUpdate,"
        " OnValidate, PostUpdate, PreStore, OnStore, PostFrame");

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif

#ifdef FLECS_TIMER


ecs_type_t ecs_type(EcsTimer);
ecs_type_t ecs_type(EcsRateFilter);

static
void AddTickSource(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTickSource, {0});
    }
}

static
void ProgressTimers(ecs_iter_t *it) {
    EcsTimer *timer = ecs_column(it, EcsTimer, 1);
    EcsTickSource *tick_source = ecs_column(it, EcsTickSource, 2);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < it->count; i ++) {
        tick_source[i].tick = false;

        if (!timer[i].active) {
            continue;
        }

        float time_elapsed = timer[i].time + it->world->stats.delta_time_raw;
        float timeout = timer[i].timeout;
        
        if (time_elapsed >= timeout) {
            float t = time_elapsed - timeout;
            if (t > timeout) {
                t = 0;
            }

            timer[i].time = t; /* Initialize with remainder */            
            tick_source[i].tick = true;
            tick_source[i].time_elapsed = time_elapsed;

            if (timer[i].single_shot) {
                timer[i].active = false;
            }
        } else {
            timer[i].time = time_elapsed;
        }  
    }
}

static
void ProgressRateFilters(ecs_iter_t *it) {
    EcsRateFilter *filter = ecs_column(it, EcsRateFilter, 1);
    EcsTickSource *tick_dst = ecs_column(it, EcsTickSource, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t src = filter[i].src;
        bool inc = false;

        filter[i].time_elapsed += it->delta_time;

        if (src) {
            const EcsTickSource *tick_src = ecs_get(it->world, src, EcsTickSource);
            if (tick_src) {
                inc = tick_src->tick;
            }
        } else {
            inc = true;
        }

        if (inc) {
            filter[i].tick_count ++;
            bool triggered = !(filter[i].tick_count % filter[i].rate);
            tick_dst[i].tick = triggered;
            tick_dst[i].time_elapsed = filter[i].time_elapsed;

            if (triggered) {
                filter[i].time_elapsed = 0;
            }            
        } else {
            tick_dst[i].tick = false;
        }
    }
}

ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t timer,
    float timeout)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = timeout,
        .single_shot = true,
        .active = true
    });

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = timer;
    }

    return timer;
}

float ecs_get_timeout(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(timer != 0, ECS_INVALID_PARAMETER, NULL);

    EcsTimer *value = ecs_get_mut(world, timer, EcsTimer, NULL);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t timer,
    float interval)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = interval,
        .active = true
    });

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = timer;
    }  

    return timer;  
}

float ecs_get_interval(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        return 0;
    }

    EcsTimer *value = ecs_get_mut(world, timer, EcsTimer, NULL);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = true;
    ptr->time = 0;
}

void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = false;
}

ecs_entity_t ecs_set_rate_filter(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    filter = ecs_set(world, filter, EcsRateFilter, {
        .rate = rate,
        .src = source
    });

    EcsSystem *system_data = ecs_get_mut(world, filter, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = filter;
    }  

    return filter;     
}

void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(tick_source != 0, ECS_INVALID_PARAMETER, NULL);

    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->tick_source = tick_source;
}

void FlecsTimerImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsTimer);

    ECS_IMPORT(world, FlecsPipeline);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_component(world, EcsTimer);
    ecs_bootstrap_component(world, EcsRateFilter);

    /* Add EcsTickSource to timers and rate filters */
    ECS_SYSTEM(world, AddTickSource, EcsPreFrame, [in] Timer || RateFilter, [out] !flecs.system.TickSource);

    /* Timer handling */
    ECS_SYSTEM(world, ProgressTimers, EcsPreFrame, Timer, flecs.system.TickSource);

    /* Rate filter handling */
    ECS_SYSTEM(world, ProgressRateFilters, EcsPreFrame, [in] RateFilter, [out] flecs.system.TickSource);
}

#endif

#ifdef FLECS_STATS


typedef struct EcsTablePtr {
    ecs_table_t *table;
} EcsTablePtr;

/* -- Systems that add components on interest */

static
void StatsAddWorldStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsWorldStats, 1);

    ecs_set(it->world, EcsWorld, EcsWorldStats, {0});
}

static
void StatsAddAllocStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsAllocStats, 1);

    ecs_set(it->world, EcsWorld, EcsAllocStats, {0});
}

static
void StatsAddMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsMemoryStats, 1);

    ecs_set(it->world, EcsWorld, EcsMemoryStats, {0});
}

static
void StatsAddSystemStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsSystemStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsSystemStats, {0});
    }
}

static
void StatsAddColSystemMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsSystemMemoryStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsSystemMemoryStats, {0});
    }
}

static
void StatsAddComponentStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsComponentStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsComponentStats, {0});
    }
}

static
void StatsAddTableStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsTableStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTableStats, {0});
    }
}

static
void StatsAddTypeStats(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, EcsTypeStats, 2);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTypeStats, {0});
    }
}

/* -- Systems that collect metrics on interest -- */

static
void StatsCollectWorldStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
    (void)system;
    (void)ctx;

    if (status == EcsSystemActivated) {
        ecs_measure_frame_time(world, true);
    } else if (status == EcsSystemDeactivated) {
        ecs_measure_frame_time(world, false);
    }
}

static
void StatsCollectWorldStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsWorldStats, stats, 1);

    ecs_world_t *world = it->world;

    stats->entities_count = ecs_eis_count(&world->stage);
    stats->components_count = ecs_count(world, EcsComponent);
    stats->col_systems_count = ecs_count(world, EcsSystem);
    stats->tables_count = ecs_sparse_count(world->stage.tables);
    stats->threads_count = ecs_vector_count(world->workers);
    
    stats->frame_seconds_total = world->stats.frame_time_total;
    stats->system_seconds_total = world->stats.system_time_total;
    stats->merge_seconds_total = world->stats.merge_time_total;
    stats->world_seconds_total = world->stats.world_time_total;
    stats->target_fps_hz = world->stats.target_fps;
    stats->frame_count_total = world->stats.frame_count_total;
}

static
void StatsCollectAllocStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsAllocStats, stats, 1);

    stats->malloc_count_total = ecs_os_api_malloc_count;
    stats->calloc_count_total = ecs_os_api_calloc_count;
    stats->realloc_count_total = ecs_os_api_realloc_count;
    stats->free_count_total = ecs_os_api_free_count;
}

static
void StatsCollectColSystemMemoryTotals(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsSystemMemoryStats, stats, 1);

    ecs_memory_stat_t *stat = it->param;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        stat->allocd_bytes += 
            stats[i].base_memory_bytes +
            stats[i].columns_memory.allocd_bytes +
            stats[i].active_tables_memory.allocd_bytes +
            stats[i].inactive_tables_memory.allocd_bytes +
            stats[i].jobs_memory.allocd_bytes +
            stats[i].other_memory_bytes;

        stat->used_bytes += 
            stats[i].base_memory_bytes +
            stats[i].columns_memory.used_bytes +
            stats[i].active_tables_memory.used_bytes +
            stats[i].inactive_tables_memory.used_bytes +
            stats[i].jobs_memory.used_bytes +
            stats[i].other_memory_bytes;            
    }
}

static
void StatsCollectTableMemoryTotals(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsTableStats, stats, 1);

    EcsMemoryStats *world_stats = it->param;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        world_stats->components_memory.used_bytes += stats[i].component_memory.used_bytes;
        world_stats->components_memory.allocd_bytes += stats[i].component_memory.allocd_bytes;

        world_stats->entities_memory.used_bytes += stats[i].entity_memory.used_bytes;
        world_stats->entities_memory.allocd_bytes += stats[i].entity_memory.allocd_bytes;

        world_stats->tables_memory.used_bytes += stats[i].other_memory_bytes;
        world_stats->tables_memory.allocd_bytes += stats[i].other_memory_bytes; 
    }
}

static
void compute_stage_memory(
    ecs_stage_t *stage, 
    EcsMemoryStats *stats)
{
    ecs_eis_memory(stage, 
        &stats->entities_memory.allocd_bytes, 
        &stats->entities_memory.used_bytes);

    ecs_sparse_memory(stage->tables,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);    
}

static
void compute_world_memory(
    ecs_world_t *world,
    EcsMemoryStats *stats)
{
    stats->world_memory.used_bytes = 
    stats->world_memory.allocd_bytes = ECS_SIZEOF(ecs_world_t);

    /* Add memory spent on worker threads to world memory */
    ecs_vector_memory(world->workers, ecs_thread_t,
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);

    /* Add memory spent on on demand lookup structures to system memory */
    ecs_map_memory(world->on_enable_components, 
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);        
    ecs_map_memory(world->on_activate_components,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);     

    ecs_vector_memory(world->fini_tasks, ecs_entity_t,
        &stats->systems_memory.allocd_bytes, &stats->systems_memory.used_bytes);

    /* Add table array to table memory */
    ecs_sparse_memory(world->stage.tables,
        &stats->tables_memory.allocd_bytes, &stats->tables_memory.used_bytes);

    /* Add misc lookup indices to world memory */
    ecs_map_memory(world->type_handles, 
        &stats->world_memory.allocd_bytes, &stats->world_memory.used_bytes);   

    stats->stages_memory = (ecs_memory_stat_t){0};
    
    /* Compute memory used in temporary stage */
    compute_stage_memory(&world->temp_stage, stats);
    stats->stages_memory.used_bytes += ECS_SIZEOF(ecs_stage_t);
    stats->stages_memory.allocd_bytes += ECS_SIZEOF(ecs_stage_t);    

    /* Compute memory used in thread stages */
    int32_t i, count = ecs_vector_count(world->worker_stages);
    ecs_stage_t *stages = ecs_vector_first(world->worker_stages, ecs_stage_t);

    for (i = 0; i < count; i ++) {
        compute_stage_memory(&stages[i], stats);
    }

    /* Add memory used / allocated by worker_stages array */
    ecs_vector_memory(world->worker_stages, ecs_stage_t,
        &stats->stages_memory.allocd_bytes, &stats->stages_memory.used_bytes);
}

static
void StatsCollectMemoryStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsMemoryStats, stats, 1);
    ECS_COLUMN_ENTITY(it, StatsCollectColSystemMemoryTotals, 2);
    ECS_COLUMN_ENTITY(it, StatsCollectTableMemoryTotals, 3);

    ecs_world_t *world = it->world;

    /* Compute entity memory (entity index) */
    stats->entities_memory = (ecs_memory_stat_t){0};
    
    ecs_eis_memory(&world->stage, 
        &stats->entities_memory.allocd_bytes, 
        &stats->entities_memory.used_bytes);
    
    /* Compute entity (entity columns), component and table memory */
    stats->components_memory = (ecs_memory_stat_t){0};
    stats->tables_memory = (ecs_memory_stat_t){0};
    ecs_run(world, StatsCollectTableMemoryTotals, 0, stats);

    /* Compute system memory */
    stats->systems_memory = (ecs_memory_stat_t){0};
    ecs_run(world, StatsCollectColSystemMemoryTotals, 0, &stats->systems_memory);

    /* Compute world memory */
    compute_world_memory(world, stats);

    /* Add everything up to compute total memory */
    stats->total_memory.used_bytes =
        stats->entities_memory.used_bytes +
        stats->components_memory.used_bytes +
        stats->tables_memory.used_bytes +
        stats->types_memory.used_bytes +
        stats->stages_memory.used_bytes +
        stats->systems_memory.used_bytes;

    stats->total_memory.allocd_bytes =
        stats->entities_memory.allocd_bytes +
        stats->components_memory.allocd_bytes +
        stats->tables_memory.allocd_bytes +
        stats->types_memory.allocd_bytes +
        stats->stages_memory.allocd_bytes +
        stats->systems_memory.allocd_bytes;        
}

static
void StatsCollectSystemStats_StatusAction(
    ecs_world_t *world, 
    ecs_entity_t system,
    ecs_system_status_t status, 
    void *ctx)
{
    (void)system;
    (void)ctx;

    if (status == EcsSystemActivated) {
        ecs_measure_system_time(world, true);
    } else if (status == EcsSystemDeactivated) {
        ecs_measure_system_time(world, false);
    }
}

static
int32_t system_tables_matched(EcsSystem *system) {
    return ecs_vector_count(system->query->tables) +
           ecs_vector_count(system->query->empty_tables);
}

static
int32_t system_entities_matched(EcsSystem *system) {
    ecs_matched_table_t *tables = ecs_vector_first(system->query->tables, ecs_matched_table_t);
    int32_t i, total = 0, count = ecs_vector_count(system->query->tables);

    for (i = 0; i < count; i ++) {
        if (tables[i].table) {
            total += ecs_table_count(tables[i].table);
        }
    }

    return total;
}

static
void StatsCollectSystemStats(ecs_iter_t *it) {
    EcsSystem *system = ecs_column(it, EcsSystem, 1);
    ECS_COLUMN(it, EcsSystemStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t entity = it->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_name(it->world, entity);
        stats[i].signature = system[i].query->sig.expr;
        stats[i].phase = 0;
        stats[i].tables_matched_count = system_tables_matched(&system[i]);
        stats[i].entities_matched_count = system_entities_matched(&system[i]);
        stats[i].period_seconds = ecs_get_interval(it->world, system[i].tick_source);
        stats[i].seconds_total = system[i].time_spent;
        stats[i].invoke_count_total = system[i].invoke_count;
        stats[i].is_enabled = !ecs_has_entity(it->world, entity, EcsDisabled);
        stats[i].is_active = ecs_vector_count(system[i].query->tables) != 0;
        stats[i].is_hidden = ecs_has_entity(it->world, entity, EcsHidden);
    }
}

static
void collect_system_table_metrics(
    EcsSystem *system,
    ecs_vector_t *tables,
    ecs_memory_stat_t *stat)
{
    int32_t column_count = ecs_vector_count(system->query->sig.columns);

    ecs_vector_memory(tables, ecs_matched_table_t, 
        &stat->allocd_bytes, &stat->used_bytes);

    ecs_matched_table_t *tables_buffer = ecs_vector_first(tables, ecs_matched_table_t);
    int32_t i, count = ecs_vector_count(tables);

    /* The 'column' member in ecs_matched_table_t */
    stat->allocd_bytes += (ECS_SIZEOF(int32_t) * column_count) * count;
    stat->used_bytes += (ECS_SIZEOF(int32_t) * column_count) * count;

    /* The 'components' member of ecs_matched_table_t */
    stat->allocd_bytes += (ECS_SIZEOF(ecs_entity_t) * column_count) * count;
    stat->used_bytes += (ECS_SIZEOF(ecs_entity_t) * column_count) * count;

    /* Refs are different for each table, so we'll have to loop to get accurate 
     * numbers */
    for (i = 0; i < count; i ++) {
        ecs_vector_memory(tables_buffer[i].references, ecs_ref_t, 
            &stat->allocd_bytes, &stat->used_bytes);
    }
}

static
void StatsCollectColSystemMemoryStats(ecs_iter_t *it) {
    EcsSystem *system = ecs_column(it, EcsSystem, 1);
    ECS_COLUMN(it, EcsSystemMemoryStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        stats[i].base_memory_bytes = ECS_SIZEOF(EcsSystem);
        stats[i].columns_memory = (ecs_memory_stat_t){0};
        stats[i].active_tables_memory = (ecs_memory_stat_t){0};
        stats[i].inactive_tables_memory = (ecs_memory_stat_t){0};
        stats[i].jobs_memory = (ecs_memory_stat_t){0};
        stats[i].other_memory_bytes = 0;
        
        ecs_vector_memory(system->query->sig.columns, ecs_sig_column_t, 
            &stats[i].columns_memory.allocd_bytes, 
            &stats[i].columns_memory.used_bytes);

        collect_system_table_metrics(system, system->query->tables, 
            &stats[i].active_tables_memory);

        collect_system_table_metrics(system, system->query->empty_tables, 
            &stats[i].inactive_tables_memory);            

        if (system->on_demand) {
            stats[i].other_memory_bytes += ECS_SIZEOF(ecs_on_demand_out_t);
        }
    }
}

static
void StatsCollectComponentStats(ecs_iter_t *it) {
    EcsComponent *component = ecs_column(it, EcsComponent, 1);
    ECS_COLUMN(it, EcsComponentStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t entity = it->entities[i];

        stats[i].entity = entity;
        stats[i].name = ecs_get_name(it->world, entity);
        stats[i].size_bytes = component[i].size;
        
        /* Reset values */
        stats[i].tables_count = 0;
        stats[i].entities_count = 0;
        stats[i].memory = (ecs_memory_stat_t){0};

        /* Walk tables to collect memory and entity stats per component */
        ecs_sparse_t *tables = it->world->stage.tables;
        int32_t t, count = ecs_sparse_count(it->world->stage.tables);

        for (t = 0; t < count; t ++) {
            ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, t);
            ecs_entity_t *components = ecs_vector_first(table->type, ecs_entity_t);
            int32_t c, c_count = table->column_count;

            /* Iterate over table columns until component is found */
            ecs_data_t *data = ecs_table_get_data(it->world, table);
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_column_t *columns = data->columns;
            ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

            for (c = 0; c < c_count; c ++) {
                if (components[c] == entity) {
                    ecs_vector_t *column = columns[c].data;
                    stats[i].tables_count ++;
                    stats[i].entities_count += ecs_vector_count(column);
                    ecs_vector_memory_t(column, columns[c].size, columns[c].alignment, 
                        &stats[i].memory.allocd_bytes, 
                        &stats[i].memory.used_bytes);
                    break;
                }
            }
        }
    }
}

static
void StatsCollectTableStats_StatusAction(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_t status,
    void *ctx)
{
    ecs_type_t ecs_type(EcsTablePtr) = ctx;
    ecs_entity_t ecs_entity(EcsTablePtr) = ecs_type_to_entity(world, ecs_type(EcsTablePtr));

    (void)system;

    if (status == EcsSystemEnabled) {
        /* Create an entity for every table */
        ecs_sparse_t *tables = world->stage.tables;
        int32_t i, count = ecs_sparse_count(tables);

        for (i = 0; i < count; i ++) {
            ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
            ecs_set(world, 0, EcsTablePtr, {table});
        }
    } else if (status == EcsSystemDisabled) {
        /* Delete all entities with EcsTable tag */
        ecs_bulk_delete(world, &(ecs_filter_t){
            .include = ecs_type(EcsTablePtr),
            .include_kind = EcsMatchAny
        });
    }
}

static
void collect_table_data_memory(
    ecs_world_t *world,
    ecs_table_t *table,
    EcsTableStats *stats)
{
    int32_t c, count = table->column_count;
    ecs_data_t *data = ecs_table_get_data(world, table);

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    stats->entity_memory = (ecs_memory_stat_t){0};

    ecs_vector_memory(data->entities, ecs_entity_t, 
        &stats->entity_memory.allocd_bytes, 
        &stats->entity_memory.used_bytes);

    stats->component_memory = (ecs_memory_stat_t){0};

    for (c = 0; c < count; c ++) {
        ecs_column_t *column = &columns[c];
        ecs_vector_memory_t(column->data, columns->size, columns->alignment,
            &stats->component_memory.allocd_bytes, 
            &stats->component_memory.used_bytes);
    }
}

static
void StatsCollectTableStats(ecs_iter_t *it) {
    ECS_COLUMN(it, EcsTablePtr, table_ptr, 1);
    ECS_COLUMN(it, EcsTableStats, stats, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_table_t *table = table_ptr[i].table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_data_t *data = ecs_table_get_data(it->world, table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_column_t *columns = data->columns;
        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_type_t type = table->type;
        stats[i].type = table->type;
        stats[i].columns_count = ecs_vector_count(type);
        stats[i].rows_count = ecs_vector_count(columns[0].data);
        stats[i].systems_matched_count = ecs_vector_count(table->queries);
        stats[i].other_memory_bytes = 
            ECS_SIZEOF(ecs_column_t) + ecs_vector_count(type) +
            ECS_SIZEOF(ecs_entity_t) * ecs_vector_count(table->queries);

        collect_table_data_memory(it->world, table, &stats[i]);
    }
}

static
void StatsCollectTypeStats(ecs_iter_t *it) {
    EcsType *type_component = ecs_column(it, EcsType, 1);
    ECS_COLUMN(it, EcsTypeStats, stats, 2);

    ecs_world_t *world = it->world;

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        stats[i].name = ecs_get_name(world, it->entities[i]);
        stats[i].entity = it->entities[i];
        stats[i].type = type_component[i].type;
        stats[i].normalized_type = type_component[i].normalized;
        stats[i].is_hidden = ecs_has_entity(world, it->entities[i], EcsHidden);
        stats[i].entities_count = 0;
        stats[i].entities_childof_count = 0;
        stats[i].entities_instanceof_count = 0;
        stats[i].components_count = 0;
        stats[i].col_systems_count = 0;
        stats[i].row_systems_count = 0;
        stats[i].enabled_systems_count = 0;
        stats[i].active_systems_count = 0;
        stats[i].instance_count = ecs_count_type(world, type_component[i].normalized);

        int32_t j, count = ecs_vector_count(type_component[i].normalized);
        ecs_entity_t *entities = ecs_vector_first(type_component[i].normalized, ecs_entity_t);
        for (j = 0; j < count; j ++) {
            ecs_entity_t e = entities[j];
            bool has_flags = false;
            
            if (e & ECS_CHILDOF) {
                stats[i].entities_childof_count ++;
                has_flags = true;
            }
            if (e & ECS_INSTANCEOF) {
                stats[i].entities_instanceof_count ++;
                has_flags = true;
            }
            if (!has_flags) {
                if (ecs_has(world, e, EcsComponent)) {
                    stats[i].components_count ++;
                } else
                if (ecs_has(world, e, EcsSystem)) {
                    stats[i].col_systems_count ++;
                    if (!ecs_has_entity(world, e, EcsDisabled)) {
                        stats[i].enabled_systems_count ++;
                    }

                    const EcsSystem *ptr = ecs_get(world, e, EcsSystem);
                    if (ecs_vector_count(ptr->query->tables)) {
                        stats[i].active_systems_count ++;
                    }
                } else {
                    stats[i].entities_count ++;
                }
            }
        }
    }
}

/* -- Module import function -- */

void FlecsStatsImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsStats);

    ECS_IMPORT(world, FlecsPipeline);

    ecs_set_name_prefix(world, "Ecs");

    ECS_COMPONENT(world, EcsAllocStats);
    ECS_COMPONENT(world, EcsWorldStats);
    ECS_COMPONENT(world, EcsMemoryStats);
    ECS_COMPONENT(world, EcsSystemStats);
    ECS_COMPONENT(world, EcsSystemMemoryStats);
    ECS_COMPONENT(world, EcsRowSystemMemoryStats);
    ECS_COMPONENT(world, EcsComponentStats);
    ECS_COMPONENT(world, EcsTableStats);
    ECS_COMPONENT(world, EcsTablePtr);
    ECS_COMPONENT(world, EcsTypeStats);
    
    ECS_ENTITY(world, EcsStatsSkipCollect, 0);

    /* -- Helper systems -- */

    ECS_SYSTEM(world, StatsCollectColSystemMemoryTotals, 0, 
        [in] EcsSystemMemoryStats,
        [out] World:EcsMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectTableMemoryTotals, 0, 
        [in] EcsTableStats,
        [out] World:EcsMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* -- Component creation systems -- */

    ECS_SYSTEM(world, StatsAddWorldStats, EcsOnStore, [out] !World:EcsWorldStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddAllocStats, EcsOnStore, [out] !World:EcsAllocStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddMemoryStats, EcsPostLoad, [out] !World:EcsMemoryStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddSystemStats, EcsOnStore,
        flecs.system.EcsSystem, [out] !EcsSystemStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden, 
        SYSTEM:EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddColSystemMemoryStats, EcsOnStore,
        flecs.system.EcsSystem, [out] !EcsSystemMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden, 
        SYSTEM:EcsStatsSkipCollect, !EcsStatsSkipCollect);

    ECS_SYSTEM(world, StatsAddComponentStats, EcsOnStore,
        EcsComponent, [out] !EcsComponentStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddTableStats, EcsOnStore,
        EcsTablePtr, [out] !EcsTableStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsAddTypeStats, EcsOnStore,
        EcsType, [out] !EcsTypeStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* -- Metrics collection systems -- */

    ECS_SYSTEM(world, StatsCollectWorldStats, EcsPostLoad,
        [out] EcsWorldStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler enables frame time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectWorldStats, StatsCollectWorldStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectAllocStats, EcsPostLoad,
        [out] EcsAllocStats, 
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectMemoryStats, EcsPostLoad,
        [out] EcsMemoryStats,
        :StatsCollectColSystemMemoryTotals,
        :StatsCollectTableMemoryTotals,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);      

    ECS_SYSTEM(world, StatsCollectSystemStats, EcsPostLoad,
        flecs.system.EcsSystem, [out] EcsSystemStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler enables system time monitoring when system is activated */
    ecs_set_system_status_action(
        world, StatsCollectSystemStats, StatsCollectSystemStats_StatusAction, NULL);

    ECS_SYSTEM(world, StatsCollectColSystemMemoryStats, EcsPostLoad,
        flecs.system.EcsSystem, [out] EcsSystemMemoryStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectComponentStats, EcsPostLoad,
        EcsComponent, [out] EcsComponentStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    ECS_SYSTEM(world, StatsCollectTableStats, EcsPostLoad,
        EcsTablePtr, [out] EcsTableStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* This handler creates entities for tables when system is enabled */
    ecs_set_system_status_action(
        world, StatsCollectTableStats, StatsCollectTableStats_StatusAction, 
        ecs_type(EcsTablePtr));

    ECS_SYSTEM(world, StatsCollectTypeStats, EcsPostLoad,
        EcsType, [out] EcsTypeStats,
        SYSTEM:EcsOnDemand, SYSTEM:Hidden);

    /* Export components to module */
    ECS_EXPORT_COMPONENT(EcsAllocStats);
    ECS_EXPORT_COMPONENT(EcsWorldStats);
    ECS_EXPORT_COMPONENT(EcsMemoryStats);
    ECS_EXPORT_COMPONENT(EcsSystemStats);
    ECS_EXPORT_COMPONENT(EcsSystemMemoryStats);
    ECS_EXPORT_COMPONENT(EcsComponentStats);
    ECS_EXPORT_COMPONENT(EcsTableStats);
    ECS_EXPORT_COMPONENT(EcsTablePtr);
    ECS_EXPORT_COMPONENT(EcsTypeStats);
}

#endif

#ifdef FLECS_SYSTEM


/* Global type variables */
ECS_TYPE_DECL(EcsComponentLifecycle);
ECS_TYPE_DECL(EcsTrigger);
ECS_TYPE_DECL(EcsSystem);
ECS_TYPE_DECL(EcsTickSource);
ECS_TYPE_DECL(EcsSignatureExpr);
ECS_TYPE_DECL(EcsSignature);
ECS_TYPE_DECL(EcsQuery);
ECS_TYPE_DECL(EcsIterAction);
ECS_TYPE_DECL(EcsContext);

static
ecs_on_demand_in_t* get_in_component(
    ecs_map_t *component_map,
    ecs_entity_t component)
{
    ecs_on_demand_in_t *in = ecs_map_get(
        component_map, ecs_on_demand_in_t, component);
    if (!in) {
        ecs_on_demand_in_t in_value = {0};
        ecs_map_set(component_map, component, &in_value);
        in = ecs_map_get(component_map, ecs_on_demand_in_t, component);
        ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return in;
}

static
void activate_in_columns(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_map_t *component_map,
    bool activate)
{
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsIn) {
            ecs_on_demand_in_t *in = get_in_component(
                component_map, columns[i].is.component);

            ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

            in->count += activate ? 1 : -1;

            ecs_assert(in->count >= 0, ECS_INTERNAL_ERROR, NULL);

            /* If this is the first system that registers the in component, walk
             * over all already registered systems to enable them */
            if (in->systems && 
               ((activate && in->count == 1) || 
                (!activate && !in->count))) 
            {
                ecs_on_demand_out_t **out = ecs_vector_first(in->systems, ecs_on_demand_out_t*);
                int32_t s, in_count = ecs_vector_count(in->systems);

                for (s = 0; s < in_count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        ecs_remove_entity(world, out[s]->system, EcsDisabledIntern);
                    } else if (!activate && !out[s]->count) {
                        ecs_add_entity(world, out[s]->system, EcsDisabledIntern);             
                    }
                }
            }
        }
    }    
}

static
void register_out_column(
    ecs_map_t *component_map,
    ecs_entity_t component,
    ecs_on_demand_out_t *on_demand_out)
{
    ecs_on_demand_in_t *in = get_in_component(component_map, component);
    ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

    on_demand_out->count += in->count;
    ecs_on_demand_out_t **elem = ecs_vector_add(&in->systems, ecs_on_demand_out_t*);
    *elem = on_demand_out;
}

static
void register_out_columns(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data)
{
    ecs_query_t *query = system_data->query;
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
    int32_t i, out_count = 0, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsOut) {
            if (!system_data->on_demand) {
                system_data->on_demand = ecs_os_malloc(sizeof(ecs_on_demand_out_t));
                ecs_assert(system_data->on_demand != NULL, ECS_OUT_OF_MEMORY, NULL);

                system_data->on_demand->system = system;
                system_data->on_demand->count = 0;
            }

            /* If column operator is NOT and the inout kind is [out], the system
             * explicitly states that it will create the component (it is not
             * there, yet it is an out column). In this case it doesn't make
             * sense to wait until [in] columns get activated (matched with
             * entities) since the component is not there yet. Therefore add it
             * to the on_enable_components list, so this system will be enabled
             * when a [in] column is enabled, rather than activated */
            ecs_map_t *component_map;
            if (columns[i].oper_kind == EcsOperNot) {
                component_map = world->on_enable_components;
            } else {
                component_map = world->on_activate_components;
            }

            register_out_column(
                component_map, columns[i].is.component, 
                system_data->on_demand);

            out_count ++;
        }
    }

    /* If there are no out columns in the on-demand system, the system will
     * never be enabled */
    ecs_assert(out_count != 0, ECS_NO_OUT_COLUMNS, ecs_get_name(world, system));
}

static
void invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    const EcsSystem *system_data,
    ecs_system_status_t status)
{
    ecs_system_status_action_t action = system_data->status_action;
    if (action) {
        action(world, system, status, system_data->status_ctx);
    }
}

static
void mark_columns_dirty(
    ecs_iter_t *it,
    EcsSystem *system_data)
{
    ecs_table_t *table = it->table;
    if (table && table->dirty_state) {
        ecs_query_t *q = system_data->query;
        int32_t i, count = ecs_vector_count(q->sig.columns);
        ecs_sig_column_t *columns = ecs_vector_first(
            q->sig.columns, ecs_sig_column_t);

        for (i = 0; i < count; i ++) {
            if (columns[i].inout_kind != EcsIn) {
                int32_t table_column = it->columns[i];
                if (table_column > 0) {
                    table->dirty_state[table_column - 1] ++;
                }
            }
        }
    }
}

/* Invoked when system becomes active or inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    if (activate) {
        ecs_remove_entity(world, system, EcsInactive);
    }

    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data || !system_data->query) {
        return;
    }

    /* If system contains in columns, signal that they are now in use */
    activate_in_columns(
        world, system_data->query, world->on_activate_components, activate);

    /* Invoke system status action */
    invoke_status_action(world, system, system_data, 
        activate ? EcsSystemActivated : EcsSystemDeactivated);

    ecs_trace_2("system #[green]%s#[reset] %s", 
        ecs_get_name(world, system), 
        activate ? "activated" : "deactivated");
}

/* Actually enable or disable system */
void ecs_enable_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data,
    bool enabled)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    ecs_query_t *query = system_data->query;
    if (!query) {
        return;
    }

    if (ecs_vector_count(query->tables)) {
        /* Only (de)activate system if it has non-empty tables. */
        ecs_system_activate(world, system, enabled);
        system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    }

    /* Enable/disable systems that trigger on [in] enablement */
    activate_in_columns(
        world, 
        query, 
        world->on_enable_components, 
        enabled);
    
    /* Invoke action for enable/disable status */
    invoke_status_action(
        world, system, system_data,
        enabled ? EcsSystemEnabled : EcsSystemDisabled);
}

void ecs_init_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_iter_action_t action,
    ecs_query_t *query,
    void *ctx)
{
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    /* Add & initialize the EcsSystem component */
    bool is_added = false;
    EcsSystem *sptr = ecs_get_mut(world, system, EcsSystem, &is_added);
    ecs_assert(sptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!is_added) {
        ecs_assert(sptr->query == query, ECS_INVALID_PARAMETER, NULL);
    } else {
        memset(sptr, 0, sizeof(EcsSystem));
        sptr->query = query;
        sptr->entity = system;
        sptr->tick_source = 0;
        sptr->time_spent = 0;
        sptr->has_out_columns = false;
    }

    /* Sanity check to make sure creating the query didn't add any additional
     * tags or components to the system */
    sptr->action = action;
    sptr->ctx = ctx;

    /* Only run this code when the system is created for the first time */
    if (is_added) {
        /* If tables have been matched with this system it is active, and we
        * should activate the in-columns, if any. This will ensure that any
        * OnDemand systems get enabled. */
        if (ecs_vector_count(query->tables)) {
            ecs_system_activate(world, system, true);
        } else {
            /* If system isn't matched with any tables, mark it as inactive. This
            * causes it to be ignored by the main loop. When the system matches
            * with a table it will be activated. */
            ecs_add_entity(world, system, EcsInactive);
        }

        /* If system is enabled, trigger enable components */
        activate_in_columns(world, query, world->on_enable_components, true);

        /* Check if all non-table column constraints are met. If not, disable
        * system (system will be enabled once constraints are met) */
        if (!ecs_sig_check_constraints(world, &query->sig)) {
            ecs_add_entity(world, system, EcsDisabledIntern);
        }

        /* If the query has a OnDemand system tag, register its [out] columns */
        if (ecs_has_entity(world, system, EcsOnDemand)) {
            sptr = ecs_get_mut(world, system, EcsSystem, NULL);

            register_out_columns(world, system, sptr);
            ecs_assert(sptr->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

            /* If there are no systems currently interested in any of the [out]
            * columns of the on demand system, disable it */
            if (!sptr->on_demand->count) {
                ecs_add_entity(world, system, EcsDisabledIntern);
            }        
        }

        /* Check if system has out columns */
        int32_t i, count = ecs_vector_count(query->sig.columns);
        ecs_sig_column_t *columns = ecs_vector_first(
                query->sig.columns, ecs_sig_column_t);
        
        for (i = 0; i < count; i ++) {
            if (columns[i].inout_kind != EcsIn) {
                sptr->has_out_columns = true;
                break;
            }
        }
    }

    ecs_trace_1("system #[green]%s#[reset] created with #[red]%s", 
        ecs_get_name(world, system), query->sig.expr);
}


void ecs_col_system_free(
    EcsSystem *system_data)
{
    ecs_query_free(system_data->query);
}

/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    const EcsType *type_ptr = ecs_get( world, entity, EcsType);
    if (type_ptr) {
        /* If entity is a type, disable all entities in the type */
        ecs_vector_each(type_ptr->normalized, ecs_entity_t, e, {
            ecs_enable(world, *e, enabled);
        });
    } else {
        if (enabled) {
            ecs_remove_entity(world, entity, EcsDisabled);
        } else {
            ecs_add_entity(world, entity, EcsDisabled);
        }
    }
}

void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx)
{
    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->status_action = action;
    system_data->status_ctx = (void*)ctx;

    if (!ecs_has_entity(world, system, EcsDisabled)) {
        /* If system is already enabled, generate enable status. The API 
         * should guarantee that it exactly matches enable-disable 
         * notifications and activate-deactivate notifications. */
        invoke_status_action(world, system, system_data, EcsSystemEnabled);

        /* If column system has active (non-empty) tables, also generate the
         * activate status. */
        if (ecs_vector_count(system_data->query->tables)) {
            invoke_status_action(
                world, system, system_data, EcsSystemActivated);
        }
    }
}

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param,
    bool ran_by_app) 
{
    if (!param) {
        param = system_data->ctx;
    }

    float time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    if (tick_source) {
        const EcsTickSource *tick = ecs_get(
            world, tick_source, EcsTickSource);

        if (tick) {
            time_elapsed = tick->time_elapsed;

            /* If timer hasn't fired we shouldn't run the system */
            if (!tick->tick) {
                return 0;
            }
        } else {
            /* If a timer has been set but the timer entity does not have the
             * EcsTimer component, don't run the system. This can be the result
             * of a single-shot timer that has fired already. Not resetting the
             * timer field of the system will ensure that the system won't be
             * ran after the timer has fired. */
            return 0;
        }
    }

    ecs_time_t time_start;
    bool measure_time = world->measure_system_time;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    /* Used to check if table columns must be marked dirty */
    bool has_out_columns = system_data->has_out_columns;

    /* Prepare the query iterator */
    ecs_iter_t it = ecs_query_iter_page(system_data->query, offset, limit);
    it.world = stage->world;
    it.system = system;
    it.delta_time = delta_time;
    it.delta_system_time = time_elapsed;
    it.world_time = world->stats.world_time_total;
    it.frame_offset = offset;
    
    /* Set param if provided, otherwise use system context */
    if (param) {
        it.param = param;
    } else {
        it.param = system_data->ctx;
    }

    ecs_iter_action_t action = system_data->action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (!filter) {
        if (ran_by_app || world == stage->world) {
            while (ecs_query_next(&it)) {
                action(&it);
                if (has_out_columns) {
                    mark_columns_dirty(&it, system_data);
                }
            }
        } else {
            ecs_thread_t *thread = (ecs_thread_t*)stage->world;
            int32_t total = ecs_vector_count(world->workers);
            int32_t current = thread->index;

            while (ecs_query_next_worker(&it, current, total)) {
                action(&it);
                if (has_out_columns) {
                    mark_columns_dirty(&it, system_data);
                }                
            }
        }

    /* If filter is provided, match each table with the provided filter */
    } else {
        while (ecs_query_next(&it)) {
            ecs_table_t *table = it.table;
            if (!ecs_table_match_filter(world, table, filter)) {
                continue;
            }

            action(&it);
            if (has_out_columns) {
                mark_columns_dirty(&it, system_data);
            }            
        }        
    }

    if (measure_time) {
        system_data->time_spent += (float)ecs_time_measure(&time_start);
    }
    
    system_data->invoke_count ++;

    return it.interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    bool in_progress = ecs_staging_begin(world);

    EcsSystem *system_data = (EcsSystem*)ecs_get(
        world, system, EcsSystem);
    assert(system_data != NULL);

    ecs_entity_t interrupted_by = ecs_run_intern(
        world, stage, system, system_data, delta_time, offset, limit, 
        filter, param, true);

    /* If world wasn't in progress when we entered this function, we need to
     * merge and reset the in_progress value */
    ecs_staging_end(world, in_progress);

    return interrupted_by;
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, NULL, param);
}

void ecs_run_monitor(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_matched_query_t *monitor,
    ecs_entities_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{
    ecs_query_t *query = monitor->query;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t system = query->system;
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!system_data->action) {
        return;
    }

    ecs_iter_t it = {0};
    ecs_query_set_iter( world, stage, query, &it, 
        monitor->matched_table_index, row, count);

    it.world = stage->world;
    it.triggered_by = components;
    it.param = system_data->ctx;

    if (entities) {
        it.entities = entities;
    }

    it.system = system;
    system_data->action(&it);
}

/* Generic constructor to initialize a component to 0 */
static
void sys_ctor_init_zero(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entities;
    (void)ctx;
    memset(ptr, 0, size * (size_t)count);
}

/* System destructor */
static
void ecs_colsystem_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)component;
    (void)ctx;
    (void)size;

    EcsSystem *system_data = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        EcsSystem *cur = &system_data[i];
        ecs_entity_t e = entities[i];

        /* Invoke Deactivated action for active systems */
        if (cur->query && ecs_vector_count(cur->query->tables)) {
            invoke_status_action(world, e, ptr, EcsSystemDeactivated);
        }

        /* Invoke Disabled action for enabled systems */
        if (!ecs_has_entity(world, e, EcsDisabled) && 
            !ecs_has_entity(world, e, EcsDisabledIntern)) 
        {
            invoke_status_action(world, e, ptr, EcsSystemDisabled);
        }           

        ecs_os_free(cur->on_demand);
    }
}

/* Register a trigger for a component */
static
EcsTrigger* trigger_find_or_create(
    ecs_vector_t **triggers,
    ecs_entity_t entity)
{
    ecs_vector_each(*triggers, EcsTrigger, trigger, {
        if (trigger->self == entity) {
            return trigger;
        }
    });

    EcsTrigger *result = ecs_vector_add(triggers, EcsTrigger);
    return result;
}

static
void trigger_set(
    ecs_world_t *world,
    const ecs_entity_t *entities,
    EcsTrigger *ct,
    int32_t count)
{
    EcsTrigger *el = NULL;

    int i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = ct[i].component;

        ecs_c_info_t *cdata = ecs_get_or_create_c_info(world, e);
        switch(ct[i].kind) {
        case EcsOnAdd:
            el = trigger_find_or_create(&cdata->on_add, entities[i]);
            break;
        case EcsOnRemove:
            el = trigger_find_or_create(&cdata->on_remove, entities[i]);
            break;
        default:
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
            break;
        }
        
        ecs_assert(el != NULL, ECS_INTERNAL_ERROR, NULL);

        *el = ct[i];
        el->self = entities[i];

        ecs_notify_tables_of_component_actions(world, e, cdata);

        ecs_trace_1("trigger #[green]%s#[normal] created for component #[red]%s",
            ct[i].kind == EcsOnAdd
                ? "OnAdd"
                : "OnRemove", ecs_get_name(world, e));
    }
}

static
void OnSetTrigger(
    ecs_iter_t *it)
{
    EcsTrigger *ct = ecs_column(it, EcsTrigger, 1);
    
    trigger_set(it->world, it->entities, ct, it->count);
}

static
void OnSetTriggerCtx(
    ecs_iter_t *it)
{
    EcsTrigger *ct = ecs_column(it, EcsTrigger, 1);
    EcsContext *ctx = ecs_column(it, EcsContext, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ct[i].ctx = (void*)ctx[i].ctx;
    }

    trigger_set(it->world, it->entities, ct, it->count);    
}

/* System that registers component lifecycle callbacks */
static
void OnSetComponentLifecycle(
    ecs_iter_t *it)
{
    EcsComponentLifecycle *cl = ecs_column(it, EcsComponentLifecycle, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_set_component_actions_w_entity(world, e, &cl[i]);   
    }
}

/* Disable system when EcsDisabled is added */
static 
void DisableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_column(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], false);
    }
}

/* Enable system when EcsDisabled is removed */
static
void EnableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_column(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], true);
    }
}

/* Parse a signature expression into the ecs_sig_t data structure */
static
void CreateSignature(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsSignatureExpr *signature = ecs_column(it, EcsSignatureExpr, 1);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = entities[i];
        const char *name = ecs_get_name(world, e);

        /* Parse the signature and add the result to the entity */
        EcsSignature sig = {0};
        ecs_sig_init(world, name, signature[0].expr, &sig.signature);
        ecs_set_ptr(world, e, EcsSignature, &sig);

        /* If sig has FromSystem columns, add components to the entity */
        ecs_vector_each(sig.signature.columns, ecs_sig_column_t, column, {
            if (column->from_kind == EcsFromSystem) {
                ecs_add_entity(world, e, column->is.component);
            }
        });    
    }
}

/* Create a query from a signature */
static
void CreateQuery(
    ecs_iter_t *it) 
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsSignature *signature = ecs_column(it, EcsSignature, 1);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = entities[i];

        if (!ecs_has(world, e, EcsQuery)) {
            EcsQuery query = {0};
            query.query = ecs_query_new_w_sig(world, e, &signature[i].signature);
            ecs_set_ptr(world, e, EcsQuery, &query);
        }
    }
}

/* Create a system from a query and an action */
static
void CreateSystem(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    EcsQuery *query = ecs_column(it, EcsQuery, 1);
    EcsIterAction *action = ecs_column(it, EcsIterAction, 2);
    EcsContext *ctx = ecs_column(it, EcsContext, 3);
    
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = entities[i];
        void *ctx_ptr = NULL;
        if (ctx) {
            ctx_ptr = (void*)ctx[i].ctx;
        }

        ecs_init_system(world, e, action[i].action, query[i].query, ctx_ptr);
    }
}

static
void bootstrap_set_system(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_iter_action_t action)
{
    ecs_sig_t sig = {0};
    ecs_entity_t sys = ecs_set(world, 0, EcsName, {.value = name});
    ecs_add_entity(world, sys, EcsOnSet);
    ecs_sig_init(world, name, expr, &sig);
    ecs_query_t *query = ecs_query_new_w_sig(world, sys, &sig);
    ecs_init_system(world, sys, action, query, NULL);
}

ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t tag,
    const char *signature,
    ecs_iter_action_t action)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!world->in_progress, ECS_INVALID_WHILE_ITERATING, NULL);

    const char *e_name = ecs_name_from_symbol(world, name);
    
    ecs_entity_t result = ecs_lookup_w_type(world, name, ecs_type(EcsSignatureExpr));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        if (tag) {
            ecs_add_entity(world, result, tag);
        }

        ecs_set(world, result, EcsSignatureExpr, {signature});
        ecs_set(world, result, EcsIterAction, {action});
    } else {
        EcsSignatureExpr *ptr = ecs_get_mut(world, result, EcsSignatureExpr, NULL);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (strcmp(ptr->expr, signature)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t kind,
    const char *component_name,
    ecs_iter_action_t action)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    ecs_entity_t component = ecs_lookup_fullpath(world, component_name);
    ecs_assert(component != 0, ECS_INVALID_COMPONENT_ID, component_name);

    const char *e_name = ecs_name_from_symbol(world, name);
    
    ecs_entity_t result = ecs_lookup_w_type(world, name, ecs_type(EcsTrigger));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        ecs_set(world, result, EcsTrigger, {
            .kind = kind,
            .action = action,
            .component = component,
            .ctx = NULL
        });
    } else {
        EcsTrigger *ptr = ecs_get_mut(world, result, EcsTrigger, NULL);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->kind != kind) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->component != component) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }

        if (ptr->action != action) {
            ptr->action = action;
        }
    }

    return result;
}

void FlecsSystemImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    ecs_bootstrap_component(world, EcsComponentLifecycle);
    ecs_bootstrap_component(world, EcsTrigger);
    ecs_bootstrap_component(world, EcsSystem);
    ecs_bootstrap_component(world, EcsTickSource);
    ecs_bootstrap_component(world, EcsSignatureExpr);
    ecs_bootstrap_component(world, EcsSignature);
    ecs_bootstrap_component(world, EcsQuery);
    ecs_bootstrap_component(world, EcsIterAction);
    ecs_bootstrap_component(world, EcsContext);

    ecs_bootstrap_tag(world, EcsOnAdd);
    ecs_bootstrap_tag(world, EcsOnRemove);
    ecs_bootstrap_tag(world, EcsOnSet);
    ecs_bootstrap_tag(world, EcsUnSet);

    ecs_bootstrap_tag(world, EcsDisabledIntern);
    ecs_bootstrap_tag(world, EcsInactive);

    /* Put EcsOnDemand and EcsMonitor in flecs.core so they can be looked up
     * without using the flecs.systems prefix */
    ecs_entity_t old_scope = ecs_set_scope(world, EcsFlecsCore);
    ecs_bootstrap_tag(world, EcsOnDemand);
    ecs_bootstrap_tag(world, EcsMonitor);
    ecs_set_scope(world, old_scope);

    ECS_TYPE_IMPL(EcsComponentLifecycle);
    ECS_TYPE_IMPL(EcsTrigger);
    ECS_TYPE_IMPL(EcsSystem);
    ECS_TYPE_IMPL(EcsTickSource);
    ECS_TYPE_IMPL(EcsSignatureExpr);
    ECS_TYPE_IMPL(EcsSignature);
    ECS_TYPE_IMPL(EcsQuery);
    ECS_TYPE_IMPL(EcsIterAction);
    ECS_TYPE_IMPL(EcsContext);

    /* Bootstrap ctor and dtor for EcsSystem */
    ecs_c_info_t *c_info = ecs_get_or_create_c_info(world, ecs_entity(EcsSystem));
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);
    c_info->lifecycle.ctor = sys_ctor_init_zero;
    c_info->lifecycle.dtor = ecs_colsystem_dtor;

    /* Create systems necessary to create systems */
    bootstrap_set_system(world, "CreateSignature", "SignatureExpr", CreateSignature);
    bootstrap_set_system(world, "CreateQuery", "Signature, IterAction", CreateQuery);
    bootstrap_set_system(world, "CreateSystem", "Query, IterAction, ?Context", CreateSystem);

    /* From here we can create systems */

    /* Register OnSet system for EcsComponentLifecycle */
    ECS_SYSTEM(world, OnSetComponentLifecycle, EcsOnSet, ComponentLifecycle, SYSTEM:Hidden);

    /* Register OnSet system for triggers */
    ECS_SYSTEM(world, OnSetTrigger, EcsOnSet, Trigger, SYSTEM:Hidden);

    /* System that sets ctx for a trigger */
    ECS_SYSTEM(world, OnSetTriggerCtx, EcsOnSet, Trigger, Context, SYSTEM:Hidden);

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, DisableSystem, EcsMonitor, System, Disabled || DisabledIntern, SYSTEM:Hidden);
    ECS_SYSTEM(world, EnableSystem, EcsMonitor, System, !Disabled, !DisabledIntern, SYSTEM:Hidden);
}

#endif


#ifdef FLECS_SYSTEM
#ifdef FLECS_DBG


int ecs_dbg_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_dbg_system_t *dbg_out)
{
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        return -1;
    }

    *dbg_out = (ecs_dbg_system_t){.system = system};
    dbg_out->active_table_count = ecs_vector_count(system_data->query->tables);
    dbg_out->inactive_table_count = ecs_vector_count(system_data->query->empty_tables);
    dbg_out->enabled = !ecs_has_entity(world, system, EcsDisabled);

    ecs_vector_each(system_data->query->tables, ecs_matched_table_t, mt, {
        ecs_table_t *table = mt->table;
        if (table) {
            dbg_out->entities_matched_count += ecs_table_count(table);
        }        
    });

    /* Inactive tables are inactive because they are empty, so no need to 
     * iterate them */

    return 0;
}

bool ecs_dbg_match_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t system,
    ecs_match_failure_t *failure_info_out)
{
    ecs_dbg_entity_t dbg;
    ecs_dbg_entity(world, entity, &dbg);

    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        failure_info_out->reason = EcsMatchNotASystem;
        failure_info_out->column = -1;
        return false;
    }

    return ecs_query_match(
        world, dbg.table, system_data->query, failure_info_out);
}


ecs_table_t* ecs_dbg_active_table(
    ecs_world_t *world,
    ecs_dbg_system_t *dbg,
    int32_t index)
{
    (void)world;

    EcsSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->query->tables, ecs_matched_table_t, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;
}

ecs_table_t* ecs_dbg_inactive_table(
    ecs_world_t *world,
    ecs_dbg_system_t *dbg,
    int32_t index)
{
    (void)world;

    EcsSystem *system_data = dbg->system_data;
    ecs_matched_table_t *table = ecs_vector_get(
        system_data->query->empty_tables, ecs_matched_table_t, index);
    if (!table) {
        return NULL;
    }
    
    return table->table;    
}

ecs_type_t ecs_dbg_get_column_type(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t column_index)
{
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data) {
        return NULL;
    }
    
    ecs_sig_column_t *columns = ecs_vector_first(
        system_data->query->sig.columns, ecs_sig_column_t);
    int32_t count = ecs_vector_count(system_data->query->sig.columns);

    if (count < column_index) {
        return NULL;
    }

    ecs_sig_column_t *column = &columns[column_index - 1];
    ecs_sig_oper_kind_t oper_kind = column->oper_kind;
    ecs_type_t result;

    switch(oper_kind) {
    case EcsOperOr:
        result = column->is.type;
        break;
    default:
        result = ecs_type_from_entity(world, column->is.component);
        break;
    }
    
    return result;
}

#endif
#endif

/** Parse callback that adds type to type identifier for ecs_new_type */
static
int parse_type_action(
    ecs_world_t *world,
    const char *name,
    const char *sig,
    int64_t column,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t inout_kind,
    ecs_entity_t flags,
    const char *entity_id,
    const char *source_id,
    void *data)
{
    ecs_vector_t **array = data;
    (void)source_id;
    (void)inout_kind;

    if (strcmp(entity_id, "0")) {
        ecs_entity_t entity = 0;

        if (from_kind != EcsFromOwned) {
            if (!name) {
                return -1;
            }

            ecs_parser_error(name, sig, column, 
                "source modifiers not supported for type expressions");
            return -1;
        }

        entity = ecs_lookup_fullpath(world, entity_id);
        if (!entity) {
            if (!name) {
                return -1;
            }

            ecs_parser_error(name, sig, column, 
                "unresolved identifier '%s'", entity_id);
            return -1;
        }

        if (oper_kind == EcsOperAnd) {
            ecs_entity_t* e_ptr = ecs_vector_add(array, ecs_entity_t);
            *e_ptr = entity | flags;
        } else {
            if (!name) {
                return -1;
            }

            /* Only AND and OR operators are supported for type expressions */
            ecs_parser_error(name, sig, column, 
                "invalid operator for type expression");
            return -1;
        }
    }

    return 0;
}

static
EcsType type_from_vec(
    ecs_world_t *world,
    ecs_vector_t *vec)
{
    EcsType result = {0, 0};
    ecs_entity_t *array = ecs_vector_first(vec, ecs_entity_t);
    int32_t i, count = ecs_vector_count(vec);

    ecs_entities_t entities = {
        .array = array,
        .count = count
    };

    ecs_table_t *table = ecs_table_find_or_create(
        world, &world->stage, &entities);
    if (!table) {
        return (EcsType){ 0 };
    }    

    result.type = table->type;

    /* Create normalized type. A normalized type resolves all elements with an
     * AND flag and appends them to the resulting type, where the default type
     * maintains the original type hierarchy. */
    ecs_vector_t *normalized = NULL;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e & ECS_AND) {
            ecs_entity_t entity = e & ECS_ENTITY_MASK;
            const EcsType *type_ptr = ecs_get(world, entity, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INVALID_PARAMETER, 
                "flag must be applied to type");

            ecs_vector_each(type_ptr->normalized, ecs_entity_t, c_ptr, {
                ecs_entity_t *el = ecs_vector_add(&normalized, ecs_entity_t);
                *el = *c_ptr;
            })
        }       
    }

    /* Only get normalized type if it's different from the type */
    if (normalized) {
        ecs_entities_t normalized_array = ecs_type_to_entities(normalized);
        ecs_table_t *norm_table = ecs_table_traverse_add(
            world, &world->stage, table, &normalized_array, NULL);

        result.normalized = norm_table->type;

        ecs_vector_free(normalized);
    } else {
        result.normalized = result.type;
    }

    return result;
}

static
EcsType type_from_expr(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
    if (expr) {
        ecs_vector_t *vec = ecs_vector_new(ecs_entity_t, 1);
        ecs_parse_expr(world, expr, parse_type_action, name, &vec);
        EcsType result = type_from_vec(world, vec);
        ecs_vector_free(vec);
        return result;
    } else {
        return (EcsType){0, 0};
    }
}

/* If a name prefix is set with ecs_set_name_prefix, check if the entity name
 * has the prefix, and if so remove it. This enables using prefixed names in C
 * for components / systems while storing a canonical / language independent 
 * identifier. */
const char* ecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name)
{
    const char *prefix = world->name_prefix;
    if (type_name && prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(type_name, prefix, len) && 
           (isupper(type_name[len]) || type_name[len] == '_')) 
        {
            if (type_name[len] == '_') {
                return type_name + len + 1;
            } else {
                return type_name + len;
            }
        }
    }

    return type_name;
}

ecs_entity_t ecs_lookup_w_type(
    ecs_world_t *world,
    const char *name,
    ecs_type_t type)
{
    if (!name) {
        return 0;
    }
    
    ecs_entity_t result = ecs_lookup(world, name);
    if (result) {
        ecs_type_t entity_type = ecs_get_type(world, result);
        if (type && (!entity_type || !ecs_type_contains(world, entity_type, type, true, false))) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }
    
    return result;
}


/* -- Public functions -- */

ecs_type_t ecs_type_from_str(
    ecs_world_t *world,
    const char *expr)
{
    EcsType type = type_from_expr(world, NULL, expr);
    return type.normalized;
}

ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = ecs_name_from_symbol(world, name);

    ecs_entity_t result = ecs_lookup_w_type(world, name, type.normalized);
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_add_type(world, result, type.normalized);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
    }

    return result;
}

ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = ecs_name_from_symbol(world, name);

    ecs_entity_t result = ecs_lookup_w_type(world, name, type.normalized);
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_add_entity(world, result, EcsPrefab);
        ecs_add_type(world, result, type.normalized);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
    } else {
        if (!ecs_has_entity(world, result, EcsPrefab)) {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}

ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    assert(world->magic == ECS_WORLD_MAGIC);

    const char *e_name = ecs_name_from_symbol(world, name);

    ecs_entity_t result = e ? e : ecs_lookup_w_type(world, e_name, ecs_type(EcsComponent));
    if (!result || e) {
        ecs_stage_t *stage = ecs_get_stage(&world);
        result = e ? e : ecs_new_component_id(world);

        ecs_set(world, result, EcsComponent, {
            .size = ecs_from_size_t(size),
            .alignment = ecs_from_size_t(alignment)
        });

        if (name) {
            ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        }

        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_entity(world, result, ECS_CHILDOF | scope);
        }
    } else {
        const EcsComponent *ptr = ecs_get(world, result, EcsComponent);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, name);
        if (ptr->size != ecs_from_size_t(size)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }
        if (ptr->alignment != ecs_from_size_t(alignment)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, name);
        }        
    }

    return result;
}

ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr)
{
    assert(world->magic == ECS_WORLD_MAGIC);  
    EcsType type = type_from_expr(world, name, expr);

    const char *e_name = ecs_name_from_symbol(world, name);
    
    ecs_entity_t result = ecs_lookup_w_type(world, name, ecs_type(EcsType));
    if (!result) {
        result = e ? e : ecs_new(world, 0);
        ecs_set(world, result, EcsName, {.value = e_name, .symbol = name});
        ecs_set(world, result, EcsType, {
            .type = type.type, .normalized = type.normalized
        });        

        /* This will allow the type to show up in debug tools */
        ecs_map_set(world->type_handles, (uintptr_t)type.type, &result);
    } else {
        const EcsType *ptr = ecs_get(world, result, EcsType);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ptr->type != type.type || 
            ptr->normalized != type.normalized) 
        {
            ecs_abort(ECS_ALREADY_DEFINED, name);
        }
    }

    return result;
}


/* Global type variables */
ecs_type_t ecs_type(EcsComponent);
ecs_type_t ecs_type(EcsType);
ecs_type_t ecs_type(EcsName);
ecs_type_t ecs_type(EcsPrefab);

/* Component lifecycle actions for EcsName */
ECS_CTOR(EcsName, ptr, {
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

ECS_DTOR(EcsName, ptr, {
    ecs_os_free(ptr->alloc_value);
    ptr->value = NULL;
    ptr->alloc_value = NULL;
    ptr->symbol = NULL;
})

ECS_COPY(EcsName, dst, src, {
    if (src->alloc_value) {
        dst->alloc_value = ecs_os_strdup(src->alloc_value);
        dst->value = dst->alloc_value;
    } else {
        dst->alloc_value = NULL;
        dst->value = src->value;
    }
    dst->symbol = src->symbol;
})

ECS_MOVE(EcsName, dst, src, {
    dst->value = src->value;
    dst->alloc_value = src->alloc_value;
    dst->symbol = src->symbol;

    src->value = NULL;
    src->alloc_value = NULL;
    src->symbol = NULL;
})


/* -- Bootstrapping -- */

#define bootstrap_component(world, table, name)\
    _bootstrap_component(world, table, ecs_entity(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

void _bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *id,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Create record in entity index */
    ecs_record_t *record = ecs_eis_get_or_create(&world->stage, entity);
    record->table = table;

    /* Insert row into table to store EcsComponent itself */
    int32_t index = ecs_table_append(world, table, data, entity, record);
    record->row = index + 1;

    /* Set size and id */
    EcsComponent *c_info = ecs_vector_first(columns[0].data, EcsComponent);
    EcsName *id_data = ecs_vector_first(columns[1].data, EcsName);
    
    c_info[index].size = size;
    c_info[index].alignment = alignment;
    id_data[index].value = &id[ecs_os_strlen("Ecs")]; /* Skip prefix */
    id_data[index].symbol = id;
    id_data[index].alloc_value = NULL;
}

/** Create type for component */
ecs_type_t ecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_table_find_or_create(world, &world->stage, 
        &(ecs_entities_t){
            .array = (ecs_entity_t[]){entity},
            .count = 1
        }
    );

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Bootstrap types for builtin components and tags */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = ecs_bootstrap_type(world, ecs_entity(EcsComponent));
    ecs_type(EcsType) = ecs_bootstrap_type(world, ecs_entity(EcsType));
    ecs_type(EcsName) = ecs_bootstrap_type(world, ecs_entity(EcsName));
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. 
 * The reason this table is constructed manually is because it requires the size
 * and alignment of the EcsComponent and EcsName components, which haven't been 
 * created yet */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {ecs_entity(EcsComponent), ecs_entity(EcsName), ECS_CHILDOF | EcsFlecsCore};
    ecs_entities_t array = {
        .array = entities,
        .count = 3
    };

    ecs_table_t *result = ecs_table_find_or_create(
        world, &world->stage, &array);

    ecs_data_t *data = ecs_table_get_or_create_data(world, &world->stage, result);

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsFirstUserComponentId);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsFirstUserComponentId);

    data->columns = ecs_os_malloc(sizeof(ecs_column_t) * 2);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsFirstUserComponentId);
    data->columns[0].size = sizeof(EcsComponent);
    data->columns[0].alignment = ECS_ALIGNOF(EcsComponent);
    data->columns[1].data = ecs_vector_new(EcsName, EcsFirstUserComponentId);
    data->columns[1].size = sizeof(EcsName);
    data->columns[1].alignment = ECS_ALIGNOF(EcsName);

    result->column_count = 2;
    
    return result;
}

void ecs_bootstrap(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = NULL;

    ecs_trace_1("bootstrap core components");
    ecs_log_push();

    /* Create table that will hold components (EcsComponent, EcsName) */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsName);

    world->stats.last_component_id = EcsFirstUserComponentId;
    world->stats.last_id = EcsFirstUserEntityId;
    world->stats.min_id = 0;
    world->stats.max_id = 0;

    bootstrap_types(world);

    ecs_set_scope(world, EcsFlecsCore);

    ecs_bootstrap_tag(world, EcsModule);
    ecs_bootstrap_tag(world, EcsPrefab);
    ecs_bootstrap_tag(world, EcsHidden);
    ecs_bootstrap_tag(world, EcsDisabled);

    ecs_set_component_actions(world, EcsName, {
        .ctor = ecs_ctor(EcsName),
        .dtor = ecs_dtor(EcsName),
        .copy = ecs_copy(EcsName),
        .move = ecs_move(EcsName)
    });

    /* Initialize scopes */
    ecs_set(world, EcsFlecs, EcsName, {.value = "flecs"});
    ecs_add_entity(world, EcsFlecs, EcsModule);
    ecs_set(world, EcsFlecsCore, EcsName, {.value = "core"});
    ecs_add_entity(world, EcsFlecsCore, EcsModule);
    ecs_add_entity(world, EcsFlecsCore, ECS_CHILDOF | EcsFlecs);

    /* Initialize EcsWorld */
    ecs_set(world, EcsWorld, EcsName, {.value = "World"});
    ecs_assert(ecs_get_name(world, EcsWorld) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "World") == EcsWorld, ECS_INTERNAL_ERROR, NULL);
    ecs_add_entity(world, EcsWorld, ECS_CHILDOF | EcsFlecsCore);

    /* Initialize EcsSingleton */
    ecs_set(world, EcsSingleton, EcsName, {.value = "$"});
    ecs_assert(ecs_get_name(world, EcsSingleton) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_lookup(world, "$") == EcsSingleton, ECS_INTERNAL_ERROR, NULL);
    ecs_add_entity(world, EcsSingleton, ECS_CHILDOF | EcsFlecsCore);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}


static
bool path_append(
    ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    ecs_entity_t component,
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_type_t type = ecs_get_type(world, child);
    ecs_entity_t cur = ecs_find_in_type(world, type, component, ECS_CHILDOF);
    
    if (cur) {
        if (cur != parent && cur != EcsFlecsCore) {
            path_append(world, parent, cur, component, sep, prefix, buf);
            ecs_strbuf_appendstr(buf, sep);
        }
    } else if (prefix) {
        ecs_strbuf_appendstr(buf, prefix);
    }

    char buff[22];
    const char *name = ecs_get_name(world, child);
    if (!name) {
        ecs_os_sprintf(buff, "%u", (uint32_t)child);
        name = buff;
    }

    ecs_strbuf_appendstr(buf, name);

    return cur != 0;
}

char* ecs_get_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    ecs_entity_t component,
    const char *sep,
    const char *prefix)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (parent != child) {
        path_append(world, parent, child, component, sep, prefix, &buf);
    } else {
        ecs_strbuf_appendstr(&buf, "");
    }

    return ecs_strbuf_get(&buf);
}

static 
ecs_entity_t name_to_id(
    const char *name)
{
    long int result = atol(name);
    ecs_assert(result >= 0, ECS_INTERNAL_ERROR, NULL);
    return (ecs_entity_t)result;
}

static
ecs_entity_t find_child_in_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    const char *name)
{
    /* If table doesn't have EcsName, then don't bother */
    int32_t name_index = ecs_type_index_of(table->type, ecs_entity(EcsName));
    if (name_index == -1) {
        return 0;
    }

    ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
    if (!data || !data->columns) {
        return 0;
    }

    int32_t i, count = ecs_vector_count(data->entities);
    if (!count) {
        return 0;
    }

    int is_number = isdigit(name[0]);
    if (is_number) {
        return name_to_id(name);
    }

    ecs_column_t *column = &data->columns[name_index];
    EcsName *names = ecs_vector_first(column->data, EcsName);

    for (i = 0; i < count; i ++) {
        const char *cur_name = names[i].value;
        const char *cur_sym = names[i].symbol;
        if ((cur_name && !strcmp(cur_name, name)) || (cur_sym && !strcmp(cur_sym, name))) {
            return *ecs_vector_get(data->entities, ecs_entity_t, i);
        }
    }

    return 0;
}

static
ecs_entity_t find_child_in_stage(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t parent,
    const char *name)
{
    (void)parent;
    
    ecs_sparse_each(stage->tables, ecs_table_t, table, {
        ecs_entity_t result = find_child_in_table(world, stage, table, name);
        if (result) {
            return result;
        }
    });    

    return 0;
}

ecs_entity_t ecs_lookup_child(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_entity_t result = 0;
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_vector_t *child_tables = ecs_map_get_ptr(
        world->child_tables, ecs_vector_t*, parent);
    
    if (child_tables) {
        ecs_vector_each(child_tables, ecs_table_t*, table_ptr, {
            ecs_table_t *table = *table_ptr;

            result = find_child_in_table(world, stage, table, name);
            if (!result) {
                if (stage != &world->stage) {
                    result = find_child_in_table(world, &world->stage, table, 
                        name);
                }
            }

            if (result) {
                return result;
            }
        });
    }

    /* If child hasn't been found it is possible that it was
     * created in a new table while staged, and the table hasn't
     * been registered with the child_table map yet. In that case we
     * have to look for the entity in the staged tables.
     * This edge case should rarely result in a lot of overhead,
     * since the number of tables should stabilize over time, which
     * means table creation while staged should be infrequent. */    
    if (!result && stage != &world->stage) {
        result = find_child_in_stage(world, stage, parent, name);
    }

    return result;
}

ecs_entity_t ecs_lookup(
    ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    if (isdigit(name[0])) {
        return name_to_id(name);
    }
    
    return ecs_lookup_child(world, 0, name);
}

ecs_entity_t ecs_lookup_symbol(
    ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    if (isdigit(name[0])) {
        return name_to_id(name);
    }
    
    return find_child_in_stage(world, &world->stage, 0, name);
}

static
bool is_sep(
    const char **ptr,
    const char *sep)
{
    ecs_size_t len = ecs_os_strlen(sep);

    if (!ecs_os_strncmp(*ptr, sep, len)) {
        *ptr += len - 1;
        return true;
    } else {
        return false;
    }
}

static
const char *path_elem(
    const char *path,
    char *buff,
    const char *sep)
{
    const char *ptr;
    char *bptr, ch;

    for (bptr = buff, ptr = path; (ch = *ptr); ptr ++) {
        if (is_sep(&ptr, sep)) {
            *bptr = '\0';
            return ptr + 1;
        } else {
            *bptr = ch;
            bptr ++;
        }
    }

    if (bptr != buff) {
        *bptr = '\0';
        return ptr;
    } else {
        return NULL;
    }
}

static
ecs_entity_t get_parent_from_path(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char **path_ptr,
    const char *prefix)
{
    bool start_from_root = false;
    const char *path = *path_ptr;

    if (prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(path, prefix, len)) {
            path += len;
            parent = 0;
            start_from_root = true;
        }
    }

    if (!start_from_root && !parent) {
        ecs_stage_t *stage = ecs_get_stage(&world);
        parent = stage->scope;
    }

    *path_ptr = path;

    return parent;
}

ecs_entity_t ecs_lookup_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr;
    ecs_entity_t cur;
    bool core_searched = false;

    if (!sep) {
        sep = ".";
    }

    parent = get_parent_from_path(world, parent, &path, prefix);

retry:
    cur = parent;
    ptr = path;

    while ((ptr = path_elem(ptr, buff, sep))) {
        cur = ecs_lookup_child(world, cur, buff);
        if (!cur) {
            goto tail;
        }
    }

tail:
    if (!cur) {
        if (!core_searched) {
            if (parent) {
                parent = ecs_get_parent_w_entity(world, parent, 0);
            } else {
                parent = EcsFlecsCore;
                core_searched = true;
            }
            goto retry;
        }
    }

    return cur;
}

ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope)
{
    ecs_stage_t *stage = ecs_get_stage(&world);

    ecs_entity_t e = ECS_CHILDOF | scope;
    ecs_entities_t to_add = {
        .array = &e,
        .count = 1
    };

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    if (scope) {
        stage->scope_table = ecs_table_traverse_add(
            world, stage, &world->stage.root, &to_add, NULL);
    } else {
        stage->scope_table = &world->stage.root;
    }

    return cur;
}

ecs_entity_t ecs_get_scope(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_get_stage(&world);
    return stage->scope;
}

int32_t ecs_get_child_count(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_vector_t *tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, entity);
    if (!tables) {
        return 0;
    } else {
        int32_t count = 0;

        ecs_vector_each(tables, ecs_table_t*, table_ptr, {
            ecs_table_t *table = *table_ptr;
            count += ecs_table_count(table);
        });

        return count;
    }
}

ecs_iter_t ecs_scope_iter(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_scope_iter_t iter = {
        .tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, parent),
        .index = 0
    };

    return (ecs_iter_t) {
        .world = world,
        .iter.parent = iter
    };
}

ecs_iter_t ecs_scope_iter_w_filter(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_filter_t *filter)
{
    ecs_scope_iter_t iter = {
        .filter = *filter,
        .tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, parent),
        .index = 0
    };

    return (ecs_iter_t) {
        .world = world,
        .iter.parent = iter,
        .table_count = ecs_vector_count(iter.tables)
    };
}

bool ecs_scope_next(
    ecs_iter_t *it)
{
    ecs_scope_iter_t *iter = &it->iter.parent;
    ecs_vector_t *tables = iter->tables;
    ecs_filter_t filter = iter->filter;

    int32_t count = ecs_vector_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = *ecs_vector_get(tables, ecs_table_t*, i);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_vector_first(table->data, ecs_data_t);
        if (!data) {
            continue;
        }

        it->count = ecs_table_count(table);
        if (!it->count) {
            continue;
        }

        if (filter.include || filter.exclude) {
            if (!ecs_table_match_filter(it->world, table, &filter)) {
                continue;
            }
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;    
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    const char *old_prefix = world->name_prefix;
    world->name_prefix = prefix;
    return old_prefix;
}

ecs_entity_t ecs_add_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    char buff[ECS_MAX_NAME_LENGTH];
    const char *ptr = path;

    parent = get_parent_from_path(world, parent, &path, prefix);

    ecs_entity_t cur = parent;

    while ((ptr = path_elem(ptr, buff, sep))) {
        ecs_entity_t e = ecs_lookup_child(world, cur, buff);
        if (!e) {
            char *name = ecs_os_strdup(buff);

            /* If this is the last entity in the path, use the provided id */
            if (entity && !path_elem(ptr, buff, sep)) {
                e = entity;
            }
            
            e = ecs_set(world, e, EcsName, {
                .value = name,
                .alloc_value = name
            });

            ecs_os_free(name);

            if (cur) {
                ecs_add_entity(world, e, ECS_CHILDOF | cur);
            }
        }

        cur = e;
    }

    return cur;
}

ecs_entity_t ecs_new_from_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    return ecs_add_path_w_sep(world, 0, parent, path, sep, prefix);
}
