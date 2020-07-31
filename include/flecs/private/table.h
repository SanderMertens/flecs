
#ifndef FLECS_TABLE_H_
#define FLECS_TABLE_H_

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Find or create table for a set of components */
ecs_table_t* ecs_table_find_or_create(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *type);

/** Find or create table for a type */
ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type);    

/* Get table data from main stage */
ecs_data_t *ecs_table_get_data(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get table data for specific stage */
ecs_data_t *ecs_table_get_staged_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table);

/* Get or create data for specific stage */
ecs_data_t *ecs_table_get_or_create_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table); 

/* Activates / deactivates table for systems. A deactivated table will not be
 * evaluated when the system is invoked. Tables automatically get activated /
 * deactivated when they become non-empty / empty. 
 *
 * If a query is provided, the table will only be activated / deactivated for
 * that query. */
void ecs_table_activate(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    bool activate);

/* Clear all entities from a table. */
void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table);

/* Reset a table to its initial state */
void ecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear all entities from the table. Do not invoke OnRemove systems */
void ecs_table_clear_silent(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear table data. Don't call OnRemove handlers. */
void ecs_table_clear_data(
    ecs_table_t *table,
    ecs_data_t *data);    

/* Return number of entities in table in main stage. */
int32_t ecs_table_count(
    ecs_table_t *table);

/* Return number of entities in stage-specific data */
int32_t ecs_table_data_count(
    ecs_data_t *data);

/* Add a new entry to the table for the specified entity */
int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record);

/* Delete an entity from the table. */
void ecs_table_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index);

/* Move a row from one table to another */
void ecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool is_copy);

/* Grow table with specified number of records. Populate table with entities,
 * starting from specified entity id. */
int32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count,
    ecs_entity_t first_entity);

/* Set table to a fixed size. Useful for preallocating memory in advance. */
int16_t ecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count); 

/* Set table to a fixed count. Useful for copying data in bulk. */
int16_t ecs_table_set_count(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count); 

/* Match table with filter */
bool ecs_table_match_filter(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_filter_t *filter);

/* Get dirty state for table columns */
int32_t* ecs_table_get_dirty_state(
    ecs_table_t *table);

/* Get monitor for monitoring table changes */
int32_t* ecs_table_get_monitor(
    ecs_table_t *table);

#ifdef __cplusplus
}
#endif

#endif
