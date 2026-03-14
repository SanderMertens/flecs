/**
 * @file storage/table_graph.h
 * @brief Table graph types and functions.
 */

#ifndef FLECS_TABLE_GRAPH_H
#define FLECS_TABLE_GRAPH_H

/* Default initializer for table diff */
#define ECS_TABLE_DIFF_INIT { .added = {0}}

/* Builder for table diff. Uses ecs_vec_t for convenience during construction. */
typedef struct ecs_table_diff_builder_t {
    ecs_vec_t added;
    ecs_vec_t removed;
    ecs_flags32_t added_flags;
    ecs_flags32_t removed_flags;
} ecs_table_diff_builder_t;

/* Edge linked list (used to keep track of incoming edges) */
typedef struct ecs_graph_edge_hdr_t {
    struct ecs_graph_edge_hdr_t *prev;
    struct ecs_graph_edge_hdr_t *next;
} ecs_graph_edge_hdr_t;

/* Single edge. */
typedef struct ecs_graph_edge_t {
    ecs_graph_edge_hdr_t hdr;
    ecs_table_t *from;               /* Edge source table */
    ecs_table_t *to;                 /* Edge destination table */
    ecs_table_diff_t *diff;          /* Added/removed components for edge */
    ecs_id_t id;                     /* Id associated with edge */
} ecs_graph_edge_t;

/* Edges to other tables. */
typedef struct ecs_graph_edges_t {
    ecs_graph_edge_t *lo;            /* Direct-indexed array for id < FLECS_HI_COMPONENT_ID */
    ecs_map_t *hi;                   /* Map for high ids (map<id, edge_t>) */
} ecs_graph_edges_t;

/* Table graph node */
typedef struct ecs_graph_node_t {
    /* Outgoing edges */
    ecs_graph_edges_t add;    
    ecs_graph_edges_t remove; 

    /* Sentinel for incoming edges: next links add-edges, prev links remove-edges */
    ecs_graph_edge_hdr_t refs;
} ecs_graph_node_t;

void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add);

void flecs_type_remove(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove);

void flecs_type_remove_ignoring_generation(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove);

ecs_type_t flecs_type_copy(
    ecs_world_t *world,
    const ecs_type_t *src);

void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type);

ecs_table_t *flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_diff_builder_init(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder);

void flecs_table_diff_builder_fini(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder);

void flecs_table_diff_builder_clear(
    ecs_table_diff_builder_t *builder);

void flecs_table_diff_build_append_table(
    ecs_world_t *world,
    ecs_table_diff_builder_t *dst,
    ecs_table_diff_t *src);

void flecs_table_diff_build(
    ecs_world_t *world,
    ecs_table_diff_builder_t *builder,
    ecs_table_diff_t *diff,
    int32_t added_offset,
    int32_t removed_offset);

void flecs_table_diff_build_noalloc(
    ecs_table_diff_builder_t *builder,
    ecs_table_diff_t *diff);

void flecs_table_edges_add_flags(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_flags32_t flags);

ecs_table_t* flecs_find_table_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_table_diff_builder_t *diff);

void flecs_table_hashmap_init(
    ecs_world_t *world,
    ecs_hashmap_t *hm);

void flecs_table_clear_edges_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t component);

#endif
