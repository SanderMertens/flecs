/**
 * @file storage/table_graph.h
 * @brief Table graph types and functions.
 */

#ifndef FLECS_TABLE_GRAPH_H
#define FLECS_TABLE_GRAPH_H

/** Cache of added/removed components for non-trivial edges between tables */
#define ECS_TABLE_DIFF_INIT { .added = {0}}

/** Builder for table diff. The table diff type itself doesn't use ecs_vec_t to
 * conserve memory on table edges (a type doesn't have the size field), whereas
 * a vec for the builder is more convenient to use & has allocator support. */
typedef struct ecs_table_diff_builder_t {
    ecs_vec_t added;
    ecs_vec_t removed;
} ecs_table_diff_builder_t;

typedef struct ecs_table_diff_t {
    ecs_type_t added;                /* Components added between tables */
    ecs_type_t removed;              /* Components removed between tables */
} ecs_table_diff_t;

/** Edge linked list (used to keep track of incoming edges) */
typedef struct ecs_graph_edge_hdr_t {
    struct ecs_graph_edge_hdr_t *prev;
    struct ecs_graph_edge_hdr_t *next;
} ecs_graph_edge_hdr_t;

/** Single edge. */
typedef struct ecs_graph_edge_t {
    ecs_graph_edge_hdr_t hdr;
    ecs_table_t *from;               /* Edge source table */
    ecs_table_t *to;                 /* Edge destination table */
    ecs_table_diff_t *diff;          /* Index into diff vector, if non trivial edge */
    ecs_id_t id;                     /* Id associated with edge */
} ecs_graph_edge_t;

/* Edges to other tables. */
typedef struct ecs_graph_edges_t {
    ecs_graph_edge_t *lo;            /* Small array optimized for low edges */
    ecs_map_t *hi;                   /* Map for hi edges (map<id, edge_t>) */
} ecs_graph_edges_t;

/* Table graph node */
typedef struct ecs_graph_node_t {
    /* Outgoing edges */
    ecs_graph_edges_t add;    
    ecs_graph_edges_t remove; 

    /* Incoming edges (next = add edges, prev = remove edges) */
    ecs_graph_edge_hdr_t refs;
} ecs_graph_node_t;

/* Find table by adding id to current table */
ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

/* Find table by removing id from current table */
ecs_table_t *flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

/* Cleanup incoming and outgoing edges for table */
void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table);

/* Table diff builder, used to build id lists that indicate the difference in
 * ids between two tables. */
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

#endif
