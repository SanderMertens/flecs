/**
 * @file id_record.h
 * @brief Index for looking up tables by (component) id.
 */

#ifndef FLECS_ID_RECORD_H
#define FLECS_ID_RECORD_H

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

    /* Name lookup index (currently only used for ChildOf pairs) */
    ecs_hashmap_t *name_index;

    /* Cached pointer to type info for id */
    const ecs_type_info_t *type_info;

    /* Lists for all id records that match a pair wildcard */
    ecs_id_record_elem_t first;
    ecs_id_record_elem_t second;
};

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_id_record_t* flecs_ensure_id_record(
    ecs_world_t *world,
    ecs_id_t id);

void flecs_clear_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr);

void flecs_remove_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr);

void flecs_register_for_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    const ecs_table_t *table,
    ecs_table_record_t *tr);

bool flecs_set_type_info_for_id_record(
    ecs_world_t *world,
    ecs_id_record_t *idr,
    const ecs_type_info_t *ti);

ecs_hashmap_t* flecs_ensure_id_name_index(
    ecs_world_t *world,
    ecs_id_t id);

ecs_hashmap_t* flecs_get_id_name_index(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

const ecs_table_record_t* flecs_id_record_table(
    ecs_id_record_t *idr,
    ecs_table_t *table);

ecs_id_record_t* flecs_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_cache_iter_t *out);

ecs_id_record_t* flecs_empty_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_cache_iter_t *out);

void flecs_fini_id_records(
    ecs_world_t *world);

#endif
