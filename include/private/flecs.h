#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

/* This file contains declarations to private flecs functions */

#include "types.h"

/* -- Entity API -- */

/* Merge entity with stage */
void ecs_merge_entity(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_row_t *staged_row);


/* Notify row system of entity (identified by row_index) */
bool ecs_notify(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_map_t *systems,
    ecs_type_t type_id,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    int32_t offset,
    int32_t limit);

/* -- World API -- */

/* Get (or create) table from type */
ecs_table_t* ecs_world_get_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id);

/* Activate system (move from inactive array to on_frame array or vice versa) */
void ecs_world_activate_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystemKind kind,
    bool active);

/* Get current thread-specific stage */
ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr);

void* get_ptr(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool staged_only,
    bool search_prefab,
    ecs_entity_info_t *info);

/* -- Stage API -- */

/* Initialize stage data structures */
void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Deinitialize stage */
void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Merge stage with main stage */
void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* -- Type utility API -- */

/* Get type from entity handle (component, type, prefab) */
ecs_type_t ecs_type_from_handle(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

/* Merge add/remove families */
ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t cur_id,
    ecs_type_t to_add_id,
    ecs_type_t to_remove_id);

/* Merge add/remove families using arrays */
ecs_type_t ecs_type_merge_arr(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_array_t *arr_cur,
    ecs_array_t *to_add,
    ecs_array_t *to_del);

/* Test if type_id_1 contains type_id_2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab);

/* Test if type contains component */
bool ecs_type_contains_component(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component,
    bool match_prefab);

/* Register new type from either a single component, an array of component
 * handles, or a combination */
ecs_type_t ecs_type_register(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t to_add,
    ecs_array_t *set);

/* Add component to type */
ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component);

/* Get array with component handles from type */
ecs_array_t* ecs_type_get(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id);

/* Convert type to string */
char* ecs_type_tostr(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id);

/* Get index for entity in type */
int16_t ecs_type_index_of(
    ecs_array_t *type,
    ecs_entity_t component);

/* -- Table API -- */

/* Initialize table */
int ecs_table_init(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table);

/* Allocate a set of columns for a type */
ecs_table_column_t *ecs_table_get_columns(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_array_t *type);

/* Initialize table with component size (used during bootstrap) */
int ecs_table_init_w_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_array_t *type,
    uint32_t size);

void ecs_table_register_system(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t system);    

/* Insert row into table (or stage) */
uint32_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    ecs_entity_t entity);

/* Insert multiple rows into table (or stage) */
uint32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    uint32_t count,
    ecs_entity_t first_entity);

/* Dimension array to have n rows (doesn't add entities) */
int16_t ecs_table_dim(
    ecs_table_t *table,
    uint32_t count);

/* Return number of entities in table */
uint64_t ecs_table_count(
    ecs_table_t *table);

/* Return size of table row */
uint32_t ecs_table_row_size(
    ecs_table_t *table);

/* Return size of table row */
uint32_t ecs_table_rows_dimensioned(
    ecs_table_t *table);    

/* Delete row from table */
void ecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    uint32_t index);

/* Get row from table (or stage) */
void* ecs_table_get(
    ecs_table_t *table,
    ecs_array_t *rows,
    uint32_t index);

/* Test if table has component */
bool ecs_table_has_components(
    ecs_table_t *table,
    ecs_array_t *components);

/* Deinitialize table. This invokes all matching on_remove systems */
void ecs_table_deinit(
    ecs_world_t *world,
    ecs_table_t *table);

/* Free table */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table);

/* -- System API -- */

/* Compute the AND type from the system columns */
void ecs_system_compute_and_families(
    ecs_world_t *world,
    EcsSystem *system_data);

/* Create new table system */
ecs_entity_t ecs_new_col_system(
    ecs_world_t *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    ecs_system_action_t action);

/* Notify column system of a new table, which initiates system-table matching */
void ecs_col_system_notify_of_table(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_table_t *table);

/* Notify row system of a new type, which initiates system-type matching */
void ecs_row_system_notify_of_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_type_t type);

/* Activate table for system (happens if table goes from empty to not empty) */
void ecs_system_activate_table(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_table_t *table,
    bool active);

/* Run a task (periodic system that is not matched against any tables) */
void ecs_run_task(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time);

/* Invoke row system */
bool ecs_notify_row_system(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_array_t *type,
    ecs_table_column_t *table_columns,
    uint32_t offset,
    uint32_t limit);

/* Callback for parse_component_expr that stores result as ecs_system_column_t's */
int ecs_parse_component_action(
    ecs_world_t *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    const char *source_id,
    void *data);

/* -- Worker API -- */

/* Compute schedule based on current number of entities matching system */
void ecs_schedule_jobs(
    ecs_world_t *world,
    ecs_entity_t system);

/* Prepare jobs */
void ecs_prepare_jobs(
    ecs_world_t *world,
    ecs_entity_t system);

/* Run jobs */
void ecs_run_jobs(
    ecs_world_t *world);

/* -- Private utilities -- */

/* Compute hash */
void ecs_hash(
    const void *key,
    size_t length,
    uint32_t *result);

/* Convert 64bit value to ecs_row_t type. ecs_row_t is stored as 64bit int in the
 * entity index */
ecs_row_t ecs_to_row(
    uint64_t value);

/* Get 64bit integer from ecs_row_t */
uint64_t ecs_from_row(
    ecs_row_t row);

/* Utility that parses system signature */
int ecs_parse_component_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    void *ctx);

/* Test whether signature has columns that must be retrieved from a table */
bool ecs_needs_tables(
    ecs_world_t *world,
    const char *signature);

/* Count number of columns signature */
uint32_t ecs_columns_count(
    const char *sig);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    uint32_t line,
    const char *func);

#endif
