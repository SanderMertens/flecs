/**
 * @file storage/component_index.h
 * @brief Index for looking up tables by component id.
 */

#ifndef FLECS_COMPONENT_INDEX_H
#define FLECS_COMPONENT_INDEX_H

/* Linked list of component records */
typedef struct ecs_id_record_elem_t {
    struct ecs_component_record_t *prev, *next;
} ecs_id_record_elem_t;

typedef struct ecs_reachable_elem_t {
    const ecs_table_record_t *tr;
    ecs_record_t *record;
    ecs_entity_t src;
    ecs_id_t id;
#ifndef FLECS_NDEBUG
    ecs_table_t *table;
#endif
} ecs_reachable_elem_t;

typedef struct ecs_reachable_cache_t {
    int32_t generation;
    int32_t current;
    ecs_vec_t ids; /* vec<reachable_elem_t> */
} ecs_reachable_cache_t;

/* Pair-specific component record data */
typedef struct ecs_pair_record_t {
    /* Name lookup index (used for ChildOf pairs) */
    ecs_hashmap_t *name_index;

    /* Vector with ordered children */
    ecs_vec_t ordered_children;

    /* Tables with non-fragmenting children */
    ecs_map_t children_tables; /* map<table_id, ecs_parent_record_t> */

    /* Count of disabled/prefab tables, for query filtering of ordered children */
    int32_t disabled_tables;
    int32_t prefab_tables;

    /* Hierarchy depth (set for ChildOf pair) */
    int32_t depth;

    /* Wildcard pair lists (head is the wildcard record itself) */
    ecs_id_record_elem_t first;   /* (R, *) */
    ecs_id_record_elem_t second;  /* (*, T) */
    ecs_id_record_elem_t trav;    /* (*, T) with only traversable relationships */

    /* Parent (R, *) component record */
    ecs_component_record_t *parent;

    /* Cache for finding components that are reachable through a relationship */
    ecs_reachable_cache_t reachable;
} ecs_pair_record_t;

/* Component record: all per-id data structures and metadata. */
struct ecs_component_record_t {
    /* Table cache (must be first member for casting). */
    ecs_table_cache_t cache; /* table_cache<ecs_table_record_t> */

    /* Component id of record */
    ecs_id_t id;

    /* Flags for id */
    ecs_flags32_t flags;

#ifdef FLECS_DEBUG_INFO
    /* String representation of id (used for debug visualization) */
    char *str;
#endif

    /* Type info (NULL for tags) */
    const ecs_type_info_t *type_info;

    /* Sparse storage (ecs_sparse_t*): component data for Sparse ids,
     * or target tracking for DontFragment wildcard pairs */
    void *sparse;

    /* Table ids with graph edges for this non-fragmenting id (for edge cleanup) */
    ecs_vec_t dont_fragment_tables;

    /* Pair data */
    ecs_pair_record_t *pair;

    /* Linked list node for global list of non-fragmenting component records */
    ecs_id_record_elem_t non_fragmenting;

    /* Refcount */
    int32_t refcount;
};

void flecs_components_init(
    ecs_world_t *world);

void flecs_components_fini(
    ecs_world_t *world);

ecs_component_record_t* flecs_components_try_ensure(
    ecs_world_t *world,
    ecs_id_t id);

void flecs_component_claim(
    ecs_world_t *world,
    ecs_component_record_t *cr);

int32_t flecs_component_release(
    ecs_world_t *world,
    ecs_component_record_t *cr);

bool flecs_component_release_tables(
    ecs_world_t *world,
    ecs_component_record_t *cr);

bool flecs_component_set_type_info(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_type_info_t *ti);

ecs_component_record_t* flecs_component_first_next(
    ecs_component_record_t *cr);

ecs_component_record_t* flecs_component_second_next(
    ecs_component_record_t *cr);

ecs_component_record_t* flecs_component_trav_next(
    ecs_component_record_t *cr);

ecs_hashmap_t* flecs_component_name_index_ensure(
    ecs_world_t *world,
    ecs_component_record_t *cr);

ecs_hashmap_t* flecs_component_name_index_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_init_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr);

ecs_flags32_t flecs_id_flags_get(
    ecs_world_t *world,
    ecs_id_t id);

void flecs_component_delete_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_record_init_dont_fragment(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_record_init_exclusive(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_shrink(
    ecs_component_record_t *cr);

void flecs_component_update_childof_depth(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_entity_t tgt,
    const ecs_record_t *tgt_record);

void flecs_component_update_childof_w_depth(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    int32_t depth);

void flecs_component_ordered_children_init(
    ecs_world_t *world,
    ecs_component_record_t *cr);

#endif
