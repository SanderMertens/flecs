/**
 * @file table_data.h
 * @brief Table data implementation.
 */

#ifndef FLECS_TABLE_DATA_H
#define FLECS_TABLE_DATA_H

/** Component column */
typedef struct ecs_column_t {
    ecs_vec_t data;                  /* Vector with component data */
    ecs_id_t id;                     /* Column id */
    ecs_type_info_t *ti;             /* Component type info */
    ecs_size_t size;                 /* Component size */
} ecs_column_t;

/** Bitset column */
typedef struct ecs_bitset_column_t {
    ecs_bitset_t data;               /* Bitset columns */
    ecs_id_t id;                     /* Column id */
} ecs_bitset_column_t;

/** Table data */
struct ecs_table_data_t {
    ecs_vec_t entities;              /* Entity ids */
    ecs_column_t *columns;           /* Component data */
    int16_t column_count;            /* Number of components (excluding tags) */
    ecs_flags32_t flags;             /* Flags for testing table data properties */
    int32_t *dirty_state;            /* Keep track of changes in columns */

    ecs_bitset_column_t *bitsets;    /* Bitset columns */    
    int16_t bs_count;                /* Number of bitset columns */
    int16_t bs_offset;               /* First bitset id in type */
    int32_t refcount;                /* How many tables are using table_data */

    ecs_id_t *ids;                   /* Array with component ids */
    uint64_t hash;                   /* Type hash */
};

ecs_table_data_t* flecs_table_data_for_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_count);

void flecs_table_data_release(
    ecs_world_t *world,
    ecs_table_data_t *data);

void flecs_table_data_fini(
    ecs_world_t *world,
    ecs_table_t *table,
    bool do_on_remove,
    bool update_entity_index,
    bool is_delete,
    bool deactivate);

int32_t flecs_table_data_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    bool construct,
    bool on_add);

int32_t flecs_table_data_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t to_add,
    const ecs_entity_t *ids);

void flecs_table_data_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *dst_table,
    int32_t dst_index,
    ecs_table_t *src_table,
    int32_t src_index,
    bool construct);

int32_t flecs_table_data_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct);

void flecs_table_data_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2);

void flecs_table_data_merge(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table);

bool flecs_table_data_shrink(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_data_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component);

int32_t* flecs_table_data_get_dirty_state(
    ecs_world_t *world,
    ecs_table_t *table);

#endif
