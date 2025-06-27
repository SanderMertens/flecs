/**
 * @file storage/component_index.h
 * @brief Index for (amongst others) looking up tables by component id.
 */

#ifndef FLECS_COMPONENT_INDEX_H
#define FLECS_COMPONENT_INDEX_H

/* Linked list of id records */
typedef struct ecs_id_record_elem_t {
    struct ecs_component_record_t *prev, *next;
} ecs_id_record_elem_t;

typedef struct ecs_reachable_elem_t {
    const ecs_table_record_t *tr;
    ecs_record_t *record;
    ecs_entity_t src;
    ecs_id_t id;
#ifndef NDEBUG
    ecs_table_t *table;
#endif
} ecs_reachable_elem_t;

typedef struct ecs_reachable_cache_t {
    int32_t generation;
    int32_t current;
    ecs_vec_t ids; /* vec<reachable_elem_t> */
} ecs_reachable_cache_t;

/* Component index data that just applies to pairs */
typedef struct ecs_pair_record_t {
    /* Name lookup index (currently only used for ChildOf pairs) */
    ecs_hashmap_t *name_index;

    /* Vector with ordered children */
    ecs_vec_t ordered_children;

    /* Lists for all id records that match a pair wildcard. The wildcard id
     * record is at the head of the list. */
    ecs_id_record_elem_t first;   /* (R, *) */
    ecs_id_record_elem_t second;  /* (*, T) */
    ecs_id_record_elem_t trav;    /* (*, T) with only traversable relationships */

    /* Parent component record. For pair records the parent is the (R, *) record. */
    ecs_component_record_t *parent;

    /* Cache for finding components that are reachable through a relationship */
    ecs_reachable_cache_t reachable;
} ecs_pair_record_t;

/* Payload for id index which contains all data structures for an id. */
struct ecs_component_record_t {
    /* Cache with all tables that contain the id. Must be first member. */
    ecs_table_cache_t cache; /* table_cache<ecs_table_record_t> */

    /* Component id of record */
    ecs_id_t id;

    /* Flags for id */
    ecs_flags32_t flags;

#ifdef FLECS_DEBUG_INFO
    /* String representation of id (used for debug visualization) */
    char *str;
#endif

    /* Cached pointer to type info for id, if id contains data. */
    const ecs_type_info_t *type_info;

    /* Storage for sparse components */
    void *sparse;

    /* Pair data */
    ecs_pair_record_t *pair;

    /* All non-fragmenting ids */
    ecs_id_record_elem_t non_fragmenting;

    /* Refcount */
    int32_t refcount;

    /* Keep alive count. This count must be 0 when the component record is deleted. If
     * it is not 0, an application attempted to delete an id that was still
     * queried for. */
    int32_t keep_alive;
};

/* Bootstrap cached id records */
void flecs_components_init(
    ecs_world_t *world);

/* Cleanup all id records in world */
void flecs_components_fini(
    ecs_world_t *world);

/* Ensure component record for id */
ecs_component_record_t* flecs_components_ensure(
    ecs_world_t *world,
    ecs_id_t id);

/* Increase refcount of component record */
void flecs_component_claim(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Decrease refcount of component record, delete if 0 */
int32_t flecs_component_release(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Release all empty tables in component record */
void flecs_component_release_tables(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Set (component) type info for component record */
bool flecs_component_set_type_info(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    const ecs_type_info_t *ti);

/* Return next (R, *) record */
ecs_component_record_t* flecs_component_first_next(
    ecs_component_record_t *cr);

/* Return next (*, T) record */
ecs_component_record_t* flecs_component_second_next(
    ecs_component_record_t *cr);

/* Return next traversable (*, T) record */
ecs_component_record_t* flecs_component_trav_next(
    ecs_component_record_t *cr);

/* Ensure name index for component record */
ecs_hashmap_t* flecs_component_name_index_ensure(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Get name index for component record */
ecs_hashmap_t* flecs_component_name_index_get(
    const ecs_world_t *world,
    ecs_component_record_t *cr);

/* Init sparse storage */
void flecs_component_init_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr);

/* Return flags for matching component records */
ecs_flags32_t flecs_id_flags_get(
    ecs_world_t *world,
    ecs_id_t id);

/* Delete entities in sparse storage */
void flecs_component_delete_sparse(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_record_init_dont_fragment(
    ecs_world_t *world,
    ecs_component_record_t *cr);

void flecs_component_record_init_exclusive(
    ecs_world_t *world,
    ecs_component_record_t *cr);

#endif
