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

    /* Id for which the event applies */
    ecs_entity_t component;

    /* Event match */
    ecs_entity_t event;

    /* Could be a union if more fields are added; kept flat for simplicity. */
} ecs_table_event_t;

/* Override type used for tables with a single IsA pair */
typedef struct ecs_table_1_override_t {
    const ecs_pair_record_t *pair;   /* Pair data for (IsA, base) */
    int32_t generation;              /* Reachable cache generation for IsA pair */
} ecs_table_1_override_t;

/* Override type used for tables with n IsA pairs (less common) */
typedef struct ecs_table_n_overrides_t {
    const ecs_table_record_t *tr;    /* Table record for (IsA, *) */
    int32_t *generations;            /* Reachable cache generations (one per IsA pair) */
} ecs_table_n_overrides_t;

typedef struct ecs_table_overrides_t {
    union {
        ecs_table_1_override_t _1;
        ecs_table_n_overrides_t _n;
    } is;
    ecs_ref_t *refs;                 /* Refs to base components (one for each column) */
} ecs_table_overrides_t;

/* Infrequently accessed data not stored inline in ecs_table_t */
typedef struct ecs_table__t {
    uint64_t hash;                   /* Type hash */
    int32_t lock;                    /* Prevents modifications */
    int32_t traversable_count;       /* Traversable relationship targets in table */

    uint16_t generation;             /* Used for table cleanup */
    int16_t record_count;            /* Table record count including wildcards */

    int16_t bs_count;                /* Number of toggle (bitset) columns */
    int16_t bs_offset;               /* Type index of first toggle id */
    ecs_bitset_t *bs_columns;        /* Bitset columns */

    struct ecs_table_record_t *records; /* Array with table records */

#ifdef FLECS_DEBUG_INFO
    /* Fields used for debug visualization */
    struct {
        ecs_world_t *world;
        ecs_entity_t id;
    } parent;                        /* Parent entity (world included for C++ flecs::entity cast) */
    int16_t name_column;             /* Column with entity name */
    int16_t doc_name_column;         /* Column with entity doc name */
#endif
} ecs_table__t;

/* Table column */
typedef struct ecs_column_t {
    void *data;                      /* Array with component data */
    ecs_type_info_t *ti;             /* Component type info */
} ecs_column_t;

/* Table data */
struct ecs_data_t {
    ecs_entity_t *entities;            /* Entity ids */
    ecs_column_t *columns;             /* Component data */
    ecs_table_overrides_t *overrides;  /* Component overrides (for tables with IsA pairs) */
    int32_t count;
    int32_t size;
};

/* A table (archetype) stores all entities with a specific set of components.
 * Created on demand and automatically matched with existing queries. */
struct ecs_table_t {
    uint64_t id;                     /* Table id in sparse set */
    ecs_flags32_t flags;             /* Flags for testing table properties */
    int16_t column_count;            /* Number of components (excluding tags) */
    uint16_t version;                /* Version of table */

    uint64_t bloom_filter;           /* For quick matching with queries */

    ecs_flags32_t trait_flags;       /* Cached trait flags for entities in table */
    int16_t keep;                    /* Refcount for keeping table alive. */
    int16_t childof_index;           /* Quick access to index of ChildOf pair in table. */

    ecs_type_t type;                 /* Vector with component ids */

    ecs_data_t data;                 /* Component storage */
    ecs_graph_node_t node;           /* Graph node */

    int16_t *component_map;          /* O(1) id->column: >0 = column+1, <0 = -(type_index+1) for tags/sparse, 0 = absent */
    int32_t *dirty_state;            /* Per-column change counters; [0] is entity column */
    int16_t *column_map;             /* [0..type_count): type->column, [type_count..): column->type */

    ecs_table__t *_;                 /* Infrequently accessed table metadata */
};

void flecs_table_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from);

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    ecs_type_t *type);

void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table); 

void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    bool construct,
    bool on_add);

void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct);

void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    int32_t new_index,
    ecs_table_t *old_table,
    int32_t old_index,
    bool construct);

int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t count,
    const ecs_entity_t *ids);

bool flecs_table_shrink(
    ecs_world_t *world,
    ecs_table_t *table);

int32_t* flecs_table_get_dirty_state(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_init_root_table(
    ecs_world_t *world);

void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_fini(
    ecs_world_t *world,
    ecs_table_t *table); 

void flecs_table_free_type(
    ecs_world_t *world,
    ecs_table_t *table);     

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

uint64_t flecs_table_bloom_filter_add(
    uint64_t filter,
    uint64_t value);

bool flecs_table_bloom_filter_test(
    const ecs_table_t *table,
    uint64_t filter);

const ecs_ref_t* flecs_table_get_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    const ecs_component_record_t *cr,
    ecs_ref_t *storage);

void flecs_table_keep(
    ecs_table_t *table);

void flecs_table_release(
    ecs_table_t *table);

ecs_component_record_t* flecs_table_get_childof_cr(
    const ecs_world_t *world,
    const ecs_table_t *table);

ecs_pair_record_t* flecs_table_get_childof_pr(
    const ecs_world_t *world,
    const ecs_table_t *table);

ecs_hashmap_t* flecs_table_get_name_index(
    const ecs_world_t *world,
    const ecs_table_t *table);

#endif
