/**
 * @file storage/id_index.h
 * @brief Index for looking up tables by (component) id.
 */

#ifndef FLECS_ID_INDEX_H
#define FLECS_ID_INDEX_H

/* Payload for id cache */
struct ecs_table_record_t {
    ecs_table_cache_hdr_t hdr;  /* Table cache header */
    int16_t index;              /* First type index where id occurs in table */
    int16_t count;              /* Number of times id occurs in table */
    int16_t column;             /* First column index where id occurs */
};

/* Linked list of id records */
typedef struct ecs_id_record_elem_t {
    struct ecs_id_record_t *prev, *next;
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

/* Payload for id index which contains all data structures for an id. */
struct ecs_id_record_t {
    /* Cache with all tables that contain the id. Must be first member. */
    ecs_table_cache_t cache; /* table_cache<ecs_table_record_t> */

    /* Id of record */
    ecs_id_t id;

    /* Flags for id */
    ecs_flags32_t flags;

    /* Cached pointer to type info for id, if id contains data. */
    const ecs_type_info_t *type_info;

    /* Name lookup index (currently only used for ChildOf pairs) */
    ecs_hashmap_t *name_index;

    /* Storage for sparse components or union relationships */
    void *sparse;

    /* Lists for all id records that match a pair wildcard. The wildcard id
     * record is at the head of the list. */
    ecs_id_record_elem_t first;   /* (R, *) */
    ecs_id_record_elem_t second;  /* (*, O) */
    ecs_id_record_elem_t trav;    /* (*, O) with only traversable relationships */

    /* Parent id record. For pair records the parent is the (R, *) record. */
    ecs_id_record_t *parent;

    /* Refcount */
    int32_t refcount;

    /* Keep alive count. This count must be 0 when the id record is deleted. If
     * it is not 0, an application attempted to delete an id that was still
     * queried for. */
    int32_t keep_alive;

    /* Cache for finding components that are reachable through a relationship */
    ecs_reachable_cache_t reachable;
};

/* Get id record for id */
ecs_id_record_t* flecs_id_record_get(
    const ecs_world_t *world,
    ecs_id_t id);

/* Ensure id record for id */
ecs_id_record_t* flecs_id_record_ensure(
    ecs_world_t *world,
    ecs_id_t id);

/* Increase refcount of id record */
void flecs_id_record_claim(
    ecs_world_t *world,
    ecs_id_record_t *idr);

/* Decrease refcount of id record, delete if 0 */
int32_t flecs_id_record_release(
    ecs_world_t *world,
    ecs_id_record_t *idr);

/* Release all empty tables in id record */
void flecs_id_record_release_tables(
    ecs_world_t *world,
    ecs_id_record_t *idr);

/* Set (component) type info for id record */
bool flecs_id_record_set_type_info(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    const ecs_type_info_t *ti);

/* Ensure id record has name index */
ecs_hashmap_t* flecs_id_name_index_ensure(
    ecs_world_t *world,
    ecs_id_t id);

ecs_hashmap_t* flecs_id_record_name_index_ensure(
    ecs_world_t *world,
    ecs_id_record_t *idr);

/* Get name index for id record */
ecs_hashmap_t* flecs_id_name_index_get(
    const ecs_world_t *world,
    ecs_id_t id);

/* Find table record for id */
ecs_table_record_t* flecs_table_record_get(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

/* Find table record for id record */
ecs_table_record_t* flecs_id_record_get_table(
    const ecs_id_record_t *idr,
    const ecs_table_t *table);

/* Init sparse storage */
void flecs_id_record_init_sparse(
    ecs_world_t *world,
    ecs_id_record_t *idr);

/* Bootstrap cached id records */
void flecs_init_id_records(
    ecs_world_t *world);

/* Cleanup all id records in world */
void flecs_fini_id_records(
    ecs_world_t *world);

#endif
