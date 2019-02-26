#ifndef REFLECS_PRIVATE_H
#define REFLECS_PRIVATE_H

/* This file contains declarations to private reflecs functions */

#include "types.h"

/* -- Entity API -- */

/* Create new entity with family */
EcsEntity ecs_new_w_family(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

/* Merge entity with stage */
void ecs_merge_entity(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    EcsRow *staged_row);


/* Notify row system of entity (identified by row_index) */
bool ecs_notify(
    EcsWorld *world,
    EcsStage *stage,
    EcsMap *systems,
    EcsFamily family_id,
    EcsTable *table,
    EcsTableColumn *table_columns,
    int32_t row_index);

/* -- World API -- */

/* Get (or create) table from family */
EcsTable* ecs_world_get_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

/* Activate system (move from inactive array to on_frame array or vice versa) */
void ecs_world_activate_system(
    EcsWorld *world,
    EcsEntity system,
    EcsSystemKind kind,
    bool active);

/* Get current thread-specific stage */
EcsStage *ecs_get_stage(
    EcsWorld **world_ptr);

/* -- Stage API -- */

/* Initialize stage data structures */
void ecs_stage_init(
    EcsStage *stage);

/* Deinitialize stage */
void ecs_stage_deinit(
    EcsStage *stage);

/* Merge stage with main stage */
void ecs_stage_merge(
    EcsWorld *world,
    EcsStage *stage);

/* -- Family utility API -- */

/* Get family from entity handle (component, family, prefab) */
EcsFamily ecs_family_from_handle(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity entity,
    EcsEntityInfo *info);

/* Merge add/remove families */
EcsFamily ecs_family_merge(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily cur_id,
    EcsFamily to_add_id,
    EcsFamily to_remove_id);

/* Test if family_id_1 contains family_id_2 */
EcsEntity ecs_family_contains(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id_1,
    EcsFamily family_id_2,
    bool match_all,
    bool match_prefab);

/* Test if family contains component */
bool ecs_family_contains_component(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family,
    EcsEntity component,
    bool match_prefab);

/* Register new family from either a single component, an array of component
 * handles, or a combination */
EcsFamily ecs_family_register(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity to_add,
    EcsArray *set);

/* Add component to family */
EcsFamily ecs_family_add(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family,
    EcsEntity component);

/* Get array with component handles from family */
EcsArray* ecs_family_get(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

/* Convert family to string */
char* ecs_family_tostr(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily family_id);

/* Get index for entity in family */
int16_t ecs_family_index_of(
    EcsArray *family,
    EcsEntity component);

/* -- Table API -- */

/* Initialize table */
EcsResult ecs_table_init(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table);

/* Initialize table with component size (used during bootstrap) */
EcsResult ecs_table_init_w_size(
    EcsWorld *world,
    EcsTable *table,
    EcsArray *family,
    uint32_t size);

/* Insert row into table (or stage) */
uint32_t ecs_table_insert(
    EcsWorld *world,
    EcsTable *table,
    EcsTableColumn *columns,
    EcsEntity entity);

/* Insert multiple rows into table (or stage) */
uint32_t ecs_table_grow(
    EcsWorld *world,
    EcsTable *table,
    EcsTableColumn *columns,
    uint32_t count,
    EcsEntity first_entity);

/* Dimension array to have n rows (doesn't add entities) */
int16_t ecs_table_dim(
    EcsTable *table,
    uint32_t count);

/* Return number of entities in table */
uint64_t ecs_table_count(
    EcsTable *table);

/* Return size of table row */
uint32_t ecs_table_row_size(
    EcsTable *table);

/* Return size of table row */
uint32_t ecs_table_rows_dimensioned(
    EcsTable *table);    

/* Delete row from table */
void ecs_table_delete(
    EcsWorld *world,
    EcsTable *table,
    uint32_t index);

/* Get row from table (or stage) */
void* ecs_table_get(
    EcsTable *table,
    EcsArray *rows,
    uint32_t index);

/* Test if table has component */
bool ecs_table_has_components(
    EcsTable *table,
    EcsArray *components);

/* Deinitialize table. This invokes all matching on_remove systems */
void ecs_table_deinit(
    EcsWorld *world,
    EcsTable *table);

/* Free table */
void ecs_table_free(
    EcsWorld *world,
    EcsTable *table);

/* -- System API -- */

/* Create new table system */
EcsEntity ecs_new_table_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action);

/* Notify system of a new table, which initiates system-table matching */
EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity system,
    EcsTable *table);

/* Activate table for system (happens if table goes from empty to not empty) */
void ecs_system_activate_table(
    EcsWorld *world,
    EcsEntity system,
    EcsTable *table,
    bool active);

/* Run a job (from a worker thread) */
void ecs_run_job(
    EcsWorld *world,
    EcsThread *thread,
    EcsJob *job);

/* Run a task (periodic system that is not matched against any tables) */
void ecs_run_task(
    EcsWorld *world,
    EcsEntity system,
    float delta_time);

/* Invoke row system */
void ecs_row_notify(
    EcsWorld *world,
    EcsEntity system,
    EcsRowSystem *system_data,
    int16_t *columns,
    EcsTableColumn *table_columns,
    uint32_t offset,
    uint32_t limit);

/* Callback for parse_component_expr that stores result as EcsSystemColumn's */
EcsResult ecs_parse_component_action(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    void *data);

/* -- Worker API -- */

/* Compute schedule based on current number of entities matching system */
void ecs_schedule_jobs(
    EcsWorld *world,
    EcsEntity system);

/* Prepare jobs */
void ecs_prepare_jobs(
    EcsWorld *world,
    EcsEntity system);

/* Run jobs */
void ecs_run_jobs(
    EcsWorld *world);

/* -- Private utilities -- */

/* Compute hash */
void ecs_hash(
    const void *key,
    size_t length,
    uint32_t *result);

/* Convert 64bit value to EcsRow type. EcsRow is stored as 64bit int in the
 * entity index */
EcsRow ecs_to_row(
    uint64_t value);

/* Get 64bit integer from EcsRow */
uint64_t ecs_from_row(
    EcsRow row);

/* Utility that parses system signature */
EcsResult ecs_parse_component_expr(
    EcsWorld *world,
    const char *sig,
    ecs_parse_action action,
    void *ctx);

/* Test whether signature has columns that must be retrieved from a table */
bool ecs_needs_tables(
    EcsWorld *world,
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
