/**
 * @file id_record.h
 * @brief Index for looking up tables by (component) id.
 */

#ifndef FLECS_ID_RECORD_H
#define FLECS_ID_RECORD_H

/* Payload for id cache */
typedef struct ecs_table_record_t {
    ecs_table_cache_hdr_t hdr;  /* Table cache header */
    int32_t column;             /* First column where id occurs in table */
    int32_t count;              /* Number of times id occurs in table */
} ecs_table_record_t;

/* Linked list of id records */
typedef struct ecs_id_record_elem_t {
    struct ecs_id_record_t *prev, *next;
} ecs_id_record_elem_t;

/* Payload for id index which contains all datastructures for an id. */
struct ecs_id_record_t {
    /* Cache with all tables that contain the id. Must be first member. */
    ecs_table_cache_t cache; /* table_cache<ecs_table_record_t> */

    /* Flags for id */
    ecs_flags32_t flags;

    /* Refcount */
    int32_t refcount;

    /* Name lookup index (currently only used for ChildOf pairs) */
    ecs_hashmap_t *name_index;

    /* Cached pointer to type info for id, if id contains data. */
    const ecs_type_info_t *type_info;

    /* Id of record */
    ecs_id_t id;

    /* Parent id record. For pair records the parent is the (R, *) record. */
    ecs_id_record_t *parent;
    
    /* Lists for all id records that match a pair wildcard. The wildcard id
     * record is at the head of the list. */
    ecs_id_record_elem_t first;   /* (R, *) */
    ecs_id_record_elem_t second;  /* (*, O) */
    ecs_id_record_elem_t acyclic; /* (*, O) with only acyclic relations */
};

/* Get id record for id */
ecs_id_record_t* flecs_id_record_get(
    const ecs_world_t *world,
    ecs_id_t id);

/* Get id record for id for searching.
 * Same as flecs_id_record_get, but replaces (R, *) with (Union, R) if R is a
 * union relationship. */
ecs_id_record_t* flecs_query_id_record_get(
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
const ecs_table_record_t* flecs_id_record_get_table(
    const ecs_id_record_t *idr,
    const ecs_table_t *table);

/* Cleanup all id records in world */
void flecs_fini_id_records(
    ecs_world_t *world);

#endif
