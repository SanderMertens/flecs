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
    ecs_flags32_t added_flags;
    ecs_flags32_t removed_flags;
} ecs_table_diff_builder_t;

typedef struct ecs_graph_edges_t ecs_graph_edges_t;

typedef struct ecs_graph_edge_t {
    struct ecs_graph_edge_t *next;
    struct ecs_graph_edge_t **prev;
    ecs_graph_edges_t *from;
    ecs_table_t *to;
    ecs_table_diff_t *diff;
    ecs_id_t id;
} ecs_graph_edge_t;

struct ecs_graph_edges_t {
    ecs_graph_edge_t *lo;
    ecs_map_t *hi;
};

typedef struct ecs_graph_node_t {
    ecs_graph_edges_t add;
    ecs_graph_edges_t remove;
    ecs_graph_edge_t *incoming[2];
} ecs_graph_node_t;

/** Add to existing type */
void flecs_type_add(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t add);

/* Remove from existing type, matching entity ids by raw id only. */
void flecs_type_remove_ignoring_generation(
    ecs_world_t *world,
    ecs_type_t *type,
    ecs_id_t remove);

/** Free type. */
void flecs_type_free(
    ecs_world_t *world,
    ecs_type_t *type);

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
