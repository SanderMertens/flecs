 /**
 * @file query/cache/cache.h
 * @brief Query cache functions.
 */

#include "../types.h"

/** Table match data.
 * Each table matched by the query is represented by an ecs_query_cache_match_t
 * instance, which are linked together in a list. A table may match a query
 * multiple times (due to wildcard queries) with different columns being matched
 * by the query. */
typedef struct ecs_query_triv_cache_match_t {
    ecs_table_t *table;              /* The current table. */
    const ecs_table_record_t **trs;  /* Information about where to find field in table. */
    void **ptrs;                     /* Cached column pointers for match. */
    uint32_t table_version;          /* Used to check if pointers need to be revalidated. */
    ecs_termset_t set_fields;        /* Fields that are set (used by fields with Optional/Not). */
} ecs_query_triv_cache_match_t;

struct ecs_query_cache_match_t {
    ecs_query_triv_cache_match_t base;
    int32_t _offset;                  /* Starting point in table. */
    int32_t _count;                   /* Number of entities to iterate in table. */
    ecs_id_t *_ids;                   /* Resolved (component) ids for current table. */
    ecs_entity_t *_sources;           /* Subjects (sources) of ids. */
    ecs_table_t **_tables;            /* Tables for fields with non-$this source. */
    ecs_termset_t _up_fields;         /* Fields that are matched through traversal. */
    int32_t *_monitor;                /* Used to monitor table for changes. */
    int32_t rematch_count;            /* Track whether table was rematched. */
    ecs_vec_t *wildcard_matches;      /* Additional matches for table for wildcard queries. */
};

/** Query group */
struct ecs_query_cache_group_t {
    ecs_vec_t tables;                 /* vec<ecs_query_cache_match_t> */
    ecs_query_group_info_t info;      /* Group info available to application. */
    ecs_query_cache_group_t *next;    /* Next group to iterate (only set for queries with group_by). */
};

/** Table record type for query table cache. A query only has one per table. */
typedef struct ecs_query_cache_table_t {
    ecs_query_cache_group_t *group;   /* Group the table is added to. */
    int32_t index;                    /* Index into group->tables. */
} ecs_query_cache_table_t;

/* Query level block allocators have sizes that depend on query field count */
typedef struct ecs_query_cache_allocators_t {
    ecs_block_allocator_t pointers;
    ecs_block_allocator_t ids;
    ecs_block_allocator_t monitors;
} ecs_query_cache_allocators_t;

/** Query that is automatically matched against tables */
typedef struct ecs_query_cache_t {
    /* Uncached query used to populate the cache */
    ecs_query_t *query;

    /* Observer to keep the cache in sync */
    ecs_observer_t *observer;

    /* Tables matched with query */
    ecs_map_t tables;

    /* Query groups, if group_by is used */
    ecs_map_t groups;

    /* Default query group */
    ecs_query_cache_group_t default_group;

    /* Groups in iteration order */
    ecs_query_cache_group_t *first_group;

    /* Table sorting */
    ecs_entity_t order_by;
    ecs_order_by_action_t order_by_callback;
    ecs_sort_table_action_t order_by_table_callback;
    ecs_vec_t table_slices;
    int32_t order_by_term;

    /* Table grouping */
    ecs_entity_t group_by;
    ecs_group_by_action_t group_by_callback;
    ecs_group_create_action_t on_group_create;
    ecs_group_delete_action_t on_group_delete;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Monitor generation */
    int32_t monitor_generation;

    int32_t cascade_by;              /* Identify cascade term */
    int32_t match_count;             /* How often have tables been (un)matched */
    int32_t prev_match_count;        /* Track if sorting is needed */
    int32_t rematch_count;           /* Track which tables were added during rematch */
    
    ecs_entity_t entity;             /* Entity associated with query */

    /* Zero'd out sources array, used for results that only match on $this */
    ecs_entity_t *sources;

    /* Map field indices from cache query to actual query */
    int8_t *field_map;

    /* Query-level allocators */
    ecs_query_cache_allocators_t allocators;
} ecs_query_cache_t;

ecs_query_cache_t* flecs_query_cache_init(
    ecs_query_impl_t *impl,
    const ecs_query_desc_t *desc);

void flecs_query_cache_fini(
    ecs_query_impl_t *impl);

void flecs_query_cache_sort_tables(
    ecs_world_t *world,
    ecs_query_impl_t *impl);

void flecs_query_cache_build_sorted_tables(
    ecs_query_cache_t *cache);

bool flecs_query_cache_is_trivial(
    const ecs_query_cache_t *cache);

ecs_size_t flecs_query_cache_elem_size(
    const ecs_query_cache_t *cache);

#include "cache_iter.h"
#include "group.h"
#include "match.h"
#include "change_detection.h"
