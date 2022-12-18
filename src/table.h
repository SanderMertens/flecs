/**
 * @file table.c
 * @brief Table storage implementation.
 */

#ifndef FLECS_TABLE_H
#define FLECS_TABLE_H

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

/* Clear table data. Don't call OnRemove handlers. */
void flecs_table_clear_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);    

/* Return number of entities in data */
int32_t flecs_table_data_count(
    const ecs_data_t *data);

/* Add a new entry to the table for the specified entity */
int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record,
    bool construct,
    bool on_add);

/* Delete an entity from the table. */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct);

/* Make sure table records are in correct table cache list */
bool flecs_table_records_update_empty(
    ecs_table_t *table);

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
    ecs_data_t *data,
    int32_t count,
    const ecs_entity_t *ids);

/* Set table to a fixed size. Useful for preallocating memory in advance. */
void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count);

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
void flecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Free table */
void flecs_table_free_type(
    ecs_world_t *world,
    ecs_table_t *table);     
    
/* Replace data */
void flecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);

/* Merge data of one table into another table */
void flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data);

void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2);

ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

ecs_table_t *flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff);

void flecs_table_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component);

void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t *event);

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table);

ecs_vec_t *ecs_table_column_for_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

int32_t flecs_table_column_to_union_index(
    const ecs_table_t *table,
    int32_t column);

/* Increase refcount of table (prevents deletion) */
void flecs_table_claim(
    ecs_world_t *world, 
    ecs_table_t *table);

/* Decreases refcount of table (may delete) */
bool flecs_table_release(
    ecs_world_t *world, 
    ecs_table_t *table);

/* Increase observer count of table */
void flecs_table_observer_add(
    ecs_table_t *table,
    int32_t value);

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
