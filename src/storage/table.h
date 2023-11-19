/**
 * @file table.h
 * @brief Table storage implementation.
 */

#ifndef FLECS_TABLE_H
#define FLECS_TABLE_H

#include "table_graph.h"
#include "table_data.h"

/* Table event type for notifying tables of world events */
typedef enum ecs_table_eventkind_t {
    EcsTableTriggersForId,
    EcsTableNoTriggersForId,
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Query event */
    ecs_query_t *query;

    /* Component info event */
    ecs_entity_t component;

    /* Event match */
    ecs_entity_t event;

    /* If the nubmer of fields gets out of hand, this can be turned into a union
     * but since events are very temporary objects, this works for now and makes
     * initializing an event a bit simpler. */
} ecs_table_event_t;

/** Infrequently accessed data not stored inline in ecs_table_t */
typedef struct ecs_table__t {
    uint64_t hash;                   /* Type hash */
    int32_t lock;                    /* Prevents modifications */
    int32_t traversable_count;       /* Traversable relationship targets in table */
    int16_t ft_offset;               /* First flattened id in type */
    uint16_t generation;             /* Used for table cleanup */
    int16_t record_count;            /* Table record count including wildcards */
    
    struct ecs_table_record_t *records; /* Array with table records */
    ecs_hashmap_t *name_index;       /* Cached pointer to name index */
} ecs_table__t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_flags32_t flags;             /* Flags for testing table properties */
    ecs_type_t type;                 /* Vector with component ids */

    ecs_vec_t entities;              /* Entity ids */
    ecs_vec_t rows;                  /* Rows of entities in table data */
    ecs_table_data_t *data;          /* Component storage */
    ecs_graph_node_t node;           /* Graph node */
    
    int32_t *column_map;             /* Map type index <-> column
                                      *  - 0..count(T):        type index -> column
                                      *  - count(T)..count(C): column -> type index
                                      */

    ecs_table__t *_;                 /* Infrequently accessed table metadata */
};

/* Get table data */
ecs_table_data_t* flecs_table_data(
    const ecs_table_t *table);

/* Get table columns */
ecs_column_t* flecs_table_columns(
    const ecs_table_t *table);

/* Get table column */
ecs_column_t* flecs_table_column(
    const ecs_table_t *table,
    int32_t column);

/* Get table entities */
ecs_vec_t* flecs_table_entities(
    const ecs_table_t *table);

/* Get table entities array */
ecs_entity_t* flecs_table_entities_array(
    const ecs_table_t *table);

/* Get table rows */
ecs_vec_t* flecs_table_rows(
    const ecs_table_t *table);

/* Get table rows array */
int32_t* flecs_table_rows_array(
    const ecs_table_t *table);

/** Copy type. */
ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src);

/** Free type. */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type);

/** Find or create table for a set of components */
ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    ecs_type_t *type);

void flecs_table_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from);

/* Initialize columns for data */
void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from);

/* Add a new entry to the table for the specified entity */
int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    int32_t data_row,
    bool construct,
    bool on_add);

/* Delete an entity from the table. */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool delete_from_data,
    bool destruct);

/* Move a row from one table to another */
void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    int32_t new_index,
    ecs_table_t *old_table,
    int32_t old_index,
    bool construct);

/* Merge data of one table into another table */
void flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table);

/* Swap rows in table */
void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2);

/* Grow table with specified number of records. Populate table with entities,
 * starting from specified entity id. */
int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t count,
    const ecs_entity_t *ids);

/* Reset a table to its initial state */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Shrink table to contents */
bool flecs_table_shrink(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get dirty state for table columns */
int32_t* flecs_table_get_dirty_state(
    ecs_world_t *world,
    ecs_table_t *table);

/* Initialize root table */
void flecs_init_root_table(
    ecs_world_t *world);

/* Mark component column dirty */
void flecs_table_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component);

/* Keep track of number of traversable entities in table */
void flecs_table_traversable_add(
    ecs_table_t *table,
    int32_t value);

/* Notify table of event (used for propagating observer related flags) */
void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t *event);

/* Clear entities from a table. */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear entities from a table, delete entities from entity index. */
void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table);

/* Free table */
void flecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Get id record for id in table type */
ecs_id_record_t* flecs_table_idr_for(
    ecs_table_t *table,
    int32_t index);

#endif
