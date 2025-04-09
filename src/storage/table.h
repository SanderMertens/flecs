/**
 * @file storage/table.h
 * @brief Table storage implementation.
 */

#ifndef FLECS_TABLE_H
#define FLECS_TABLE_H

#include "table_graph.h"

#ifdef FLECS_SANITIZE
#define ecs_vec_from_column(arg_column, table, arg_elem_size) {\
    .array = (arg_column)->data,\
    .count = table->data.count,\
    .size = table->data.size,\
    .elem_size = arg_elem_size\
}

#define ecs_vec_from_column_ext(arg_column, arg_count, arg_size, arg_elem_size) {\
    .array = (arg_column)->data,\
    .count = arg_count,\
    .size = arg_size,\
    .elem_size = arg_elem_size\
}

#define ecs_vec_from_entities(table) {\
    .array = table->data.entities,\
    .count = table->data.count,\
    .size = table->data.size,\
    .elem_size = ECS_SIZEOF(ecs_entity_t)\
}
#else
#define ecs_vec_from_column(arg_column, table, arg_elem_size) {\
    .array = (arg_column)->data,\
    .count = table->data.count,\
    .size = table->data.size,\
}

#define ecs_vec_from_column_ext(arg_column, arg_count, arg_size, arg_elem_size) {\
    .array = (arg_column)->data,\
    .count = arg_count,\
    .size = arg_size,\
}

#define ecs_vec_from_entities(table) {\
    .array = table->data.entities,\
    .count = table->data.count,\
    .size = table->data.size,\
}
#endif

#define ecs_vec_from_column_t(arg_column, table, T)\
    ecs_vec_from_column(arg_column, table, ECS_SIZEOF(T))

/* Table event type for notifying tables of world events */
typedef enum ecs_table_eventkind_t {
    EcsTableTriggersForId,
    EcsTableNoTriggersForId,
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Component info event */
    ecs_entity_t component;

    /* Event match */
    ecs_entity_t event;

    /* If the number of fields gets out of hand, this can be turned into a union
     * but since events are very temporary objects, this works for now and makes
     * initializing an event a bit simpler. */
} ecs_table_event_t;

/** Infrequently accessed data not stored inline in ecs_table_t */
typedef struct ecs_table__t {
    uint64_t hash;                   /* Type hash */
    int32_t lock;                    /* Prevents modifications */
    int32_t traversable_count;       /* Traversable relationship targets in table */
    uint16_t generation;             /* Used for table cleanup */
    int16_t record_count;            /* Table record count including wildcards */
    
    struct ecs_table_record_t *records; /* Array with table records */
    ecs_hashmap_t *name_index;       /* Cached pointer to name index */

    ecs_bitset_t *bs_columns;        /* Bitset columns */
    int16_t bs_count;
    int16_t bs_offset;
    int16_t ft_offset;

#ifdef FLECS_DEBUG_INFO
    /* Fields used for debug visualization */
    struct {
        ecs_world_t *world;
        ecs_entity_t id;
    } parent;                        /* Parent. Include world so it can be cast
                                      * to a flecs::entity. */
    int16_t name_column;             /* Column with entity name */
    int16_t doc_name_column;         /* Column with entity doc name */
#endif
} ecs_table__t;

/** Table column */
typedef struct ecs_column_t {
    void *data;                      /* Array with component data */
    ecs_type_info_t *ti;             /* Component type info */
} ecs_column_t;

/** Table data */
struct ecs_data_t {
    ecs_entity_t *entities;          /* Entity ids */
    ecs_column_t *columns;           /* Component data */
    int32_t count;
    int32_t size;
};

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_flags32_t flags;             /* Flags for testing table properties */
    int16_t column_count;            /* Number of components (excluding tags) */
    ecs_type_t type;                 /* Vector with component ids */

    ecs_data_t data;                 /* Component storage */
    ecs_graph_node_t node;           /* Graph node */
    
    int32_t *dirty_state;            /* Keep track of changes in columns */
    int16_t *component_map;          /* Get column for component id */
    int16_t *column_map;             /* Map type index <-> column
                                      *  - 0..count(T):        type index -> column
                                      *  - count(T)..count(C): column -> type index
                                      */

    /* Custom storage components */
    ecs_entity_t *custom_storage_components; /* Components with custom storage */
    int32_t custom_storage_count;    /* Number of components with custom storage */
    
    ecs_table__t *_;                 /* Infrequently accessed table metadata */
};

/* Init table */
void flecs_table_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from);

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

/* Initialize columns for data */
void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Clear all entities from a table. */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table);

/* Reset a table to its initial state */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear all entities from the table. Do not invoke OnRemove systems */
void flecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table);

/* Add a new entry to the table for the specified entity */
int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    bool construct,
    bool on_add);

/* Delete an entity from the table. */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
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

/* Grow table with specified number of records. Populate table with entities,
 * starting from specified entity id. */
int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t count,
    const ecs_entity_t *ids);

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

/* Unset components in table */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table);

/* Free table */
void flecs_table_fini(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Free table */
void flecs_table_free_type(
    ecs_world_t *world,
    ecs_table_t *table);     

/* Merge data of one table into another table */
void flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table);

void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2);

void flecs_table_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component);

void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_event_t *event);

void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table);

/* Increase observer count of table */
void flecs_table_traversable_add(
    ecs_table_t *table,
    int32_t value);

void flecs_table_emit(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t event);

int32_t flecs_table_get_toggle_column(
    ecs_table_t *table,
    ecs_id_t id);

ecs_bitset_t* flecs_table_get_toggle(
    ecs_table_t *table,
    ecs_id_t id);

ecs_id_t flecs_column_id(
    ecs_table_t *table,
    int32_t column_index);

#endif
