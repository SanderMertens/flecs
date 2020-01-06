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
    ecs_row_t staged_row);

/* Get prefab from type, even if type was introduced while in progress */
ecs_entity_t ecs_get_prefab_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    bool is_new_table,
    ecs_entity_t entity,
    ecs_type_t type_id);

/* Notify row system of entity (identified by row_index) */
ecs_type_t ecs_notify(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_map_t *systems,
    ecs_type_t type_id,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    int32_t offset,
    int32_t limit);

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

/* Does one of the entity containers has specified component */
bool ecs_components_contains_component(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t component,
    ecs_entity_t flags,
    ecs_entity_t *entity_out);

/* Get pointer to a component */
void* ecs_get_ptr_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_info_t *info,
    ecs_entity_t component,
    bool staged_only,
    bool search_prefab);

ecs_entity_t ecs_get_entity_for_component(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type_id,
    ecs_entity_t component);

void ecs_clear_w_filter(
    ecs_world_t *world,
    const ecs_filter_t *filter);

/* -- World API -- */

/* Get (or create) table from type */
ecs_table_t* ecs_world_get_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type_id);

/* Notify systems that there is a new table, which triggers matching */
void ecs_notify_systems_of_table(
    ecs_world_t *world,
    ecs_table_t *table);

/* Activate system (move from inactive array to on_update array or vice versa) */
void ecs_world_activate_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystemKind kind,
    bool active);

/* Get current thread-specific stage */
ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr);

/* Get array for system kind */
ecs_vector_t** ecs_system_array(
    ecs_world_t *world,
    EcsSystemKind kind);

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

ecs_type_t ecs_type_find_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t *buf,
    uint32_t count);

/* Merge add/remove types */
ecs_type_t ecs_type_merge_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t cur_id,
    ecs_type_t to_add_id,
    ecs_type_t to_remove_id);

/* Test if type_id_1 contains type_id_2 */
ecs_entity_t ecs_type_contains(
    ecs_world_t *world,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab);

/* Test if type contains component */
bool ecs_type_has_entity_intern(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    bool match_prefab);

/* Add component to type */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component);

/* Get index for entity in type */
int16_t ecs_type_index_of(
    ecs_type_t type,
    ecs_entity_t component);

/* Get number of containers (parents) for a type */
int32_t ecs_type_container_depth(
   ecs_world_t *world,
   ecs_type_t type,
   ecs_entity_t component);

/** Utility to iterate over prefabs in type */
int32_t ecs_type_get_prefab(
    ecs_type_t type,
    int32_t n);

/* Find entity in prefabs of type */
ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous);

/* -- Table API -- */

/* Initialize table */
void ecs_table_init(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table);

/* Evaluate table for special columns */
void ecs_table_eval_columns(
    ecs_world_t *world,
    ecs_table_t *table);

/* Allocate a set of columns for a type */
ecs_table_column_t *ecs_table_get_columns(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table);

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
    ecs_table_column_t *columns,
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
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    int32_t index);

/* Get row from table (or stage) */
void* ecs_table_get(
    ecs_table_t *table,
    ecs_vector_t *rows,
    uint32_t index);

/* Test if table has component */
bool ecs_table_has_components(
    ecs_table_t *table,
    ecs_vector_t *components);

/* Deinitialize table. This invokes all matching on_remove systems */
void ecs_table_deinit(
    ecs_world_t *world,
    ecs_table_t *table);

/* Free table */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear table data */
void ecs_table_delete_all(
    ecs_world_t *world,
    ecs_table_t *table);

void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table);    

/* Clear data in columns */
void ecs_table_replace_columns(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_column_t *columns);
    
/* Merge data of one table into another table */
void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table);

void ecs_table_swap(
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    int32_t row_1,
    int32_t row_2,
    ecs_row_t *row_ptr_1,
    ecs_row_t *row_ptr_2);

void ecs_table_move_back_and_swap(
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_table_column_t *columns,
    uint32_t row,
    uint32_t count);

/* -- System API -- */

void ecs_system_init_base(
    ecs_world_t *world,
    EcsSystem *base_data);

/* Compute the AND type from the system columns */
void ecs_system_compute_and_families(
    ecs_world_t *world,
    EcsSystem *system_data);

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate);

/* Invoke status action */
void ecs_invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data,
    ecs_system_status_t status);

/* Check if all non-table column constraints are met */
bool ecs_check_column_constraints(
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
    ecs_entity_t system);

/* Invoke row system */
ecs_type_t ecs_notify_row_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_table_column_t *table_columns,
    uint32_t offset,
    uint32_t limit);

/* Callback for parse_component_expr that stores result as ecs_system_column_t's */
int ecs_parse_signature_action(
    ecs_world_t *world,
    const char *system_id,
    const char *sig,
    int column,    
    ecs_system_expr_elem_kind_t elem_kind,
    ecs_system_expr_oper_kind_t oper_kind,
    ecs_system_expr_inout_kind_t inout_kind,
    const char *component_id,
    const char *source_id,
    void *data);

/* Trigger rematch of system */
void ecs_rematch_system(
    ecs_world_t *world,
    ecs_entity_t system);

/* Re-resolve references of system after table realloc */
void ecs_revalidate_system_refs(
    ecs_world_t *world,
    ecs_entity_t system);

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);

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

/* -- Os time api -- */

void ecs_os_time_setup(void);
uint64_t ecs_os_time_now(void);
void ecs_os_time_sleep(unsigned int sec, unsigned int nanosec);


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

/* Utility that print a descriptive error string*/
//void ecs_print_error_string(const char *error_description, const char* signature, const char* system_id, const char* component_id);
//void ecs_print_error_string(const char* signature, const char *system_id, const char *error_description, const char *component_id);

/* Utility that parses system signature */
int ecs_parse_component_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *system_id,
    void *ctx);

/* Test whether signature has columns that must be retrieved from a table */
bool ecs_needs_tables(
    ecs_world_t *world,
    const char *signature,
    const char *system_id);

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
