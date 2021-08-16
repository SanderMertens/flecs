#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#include "private_types.h"

////////////////////////////////////////////////////////////////////////////////
//// Core bootstrap functions
////////////////////////////////////////////////////////////////////////////////

/* Bootstrap world */
void flecs_bootstrap(
    ecs_world_t *world);

ecs_type_t flecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity);

#define flecs_bootstrap_component(world, id)\
    ecs_component_init(world, &(ecs_component_desc_t){\
        .entity = {\
            .entity = ecs_id(id),\
            .name = #id,\
            .symbol = #id\
        },\
        .size = sizeof(id),\
        .alignment = ECS_ALIGNOF(id)\
    });

#define flecs_bootstrap_tag(world, name)\
    ecs_set_name(world, name, (char*)&#name[ecs_os_strlen("Ecs")]);\
    ecs_set_symbol(world, name, #name);\
    ecs_add_pair(world, name, EcsChildOf, ecs_get_scope(world))


/* Bootstrap functions for other parts in the code */
void flecs_bootstrap_hierarchy(ecs_world_t *world);

////////////////////////////////////////////////////////////////////////////////
//// Entity API
////////////////////////////////////////////////////////////////////////////////

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void flecs_set_watch(
    ecs_world_t *world,
    ecs_entity_t entity);

/* Obtain entity info */
bool flecs_get_info(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

void flecs_run_monitors(
    ecs_world_t *world, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors);

void flecs_register_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

void flecs_unregister_name(
    ecs_world_t *world,
    ecs_entity_t entity);


////////////////////////////////////////////////////////////////////////////////
//// World API
////////////////////////////////////////////////////////////////////////////////

/* Get current stage */
ecs_stage_t* flecs_stage_from_world(
    ecs_world_t **world_ptr);

/* Get current thread-specific stage from readonly world */
const ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world);

/* Get component callbacks */
const ecs_type_info_t *flecs_get_c_info(
    const ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks */
ecs_type_info_t * flecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

void flecs_eval_component_monitors(
    ecs_world_t *world);

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id);

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id,
    ecs_query_t *query);

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event);

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event);

void flecs_register_table(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_unregister_table(
    ecs_world_t *world,
    ecs_table_t *table);

ecs_id_record_t* flecs_ensure_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

void flecs_clear_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

void flecs_triggers_notify(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

ecs_map_t* flecs_triggers_get(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event);

void flecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger);

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer);

void flecs_use_intern(
    ecs_entity_t entity,
    const char *name,
    ecs_vector_t **alias_vector);


////////////////////////////////////////////////////////////////////////////////
//// Stage API
////////////////////////////////////////////////////////////////////////////////

/* Initialize stage data structures */
void flecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Deinitialize stage */
void flecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Post-frame merge actions */
void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage);  

/* Delete table from stage */
void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table);


////////////////////////////////////////////////////////////////////////////////
//// Defer API
////////////////////////////////////////////////////////////////////////////////

bool flecs_defer_none(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component);

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value);

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    const ecs_ids_t *components,
    const ecs_entity_t **ids_out);

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_enable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool enable);    

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_size_t size,
    const void *value,
    void **value_out,
    bool *is_added);

bool flecs_defer_flush(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage);

////////////////////////////////////////////////////////////////////////////////
//// Type API
////////////////////////////////////////////////////////////////////////////////

/* Test if type_id_1 contains type_id_2 */
ecs_entity_t flecs_type_contains(
    const ecs_world_t *world,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab);

void flecs_run_add_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *added,
    bool get_all,
    bool run_on_set);   

void flecs_run_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *removed);

void flecs_run_set_systems(
    ecs_world_t *world,
    ecs_id_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool set_all);


////////////////////////////////////////////////////////////////////////////////
//// Table API
////////////////////////////////////////////////////////////////////////////////

/** Find or create table for a set of components */
ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *type);   

/* Get table data */
ecs_data_t *flecs_table_get_data(
    const ecs_table_t *table);

/* Get or create data */
ecs_data_t *flecs_table_get_or_create_data(
    ecs_table_t *table);

/* Initialize columns for data */
ecs_data_t* flecs_init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result); 

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
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record,
    bool construct);

/* Delete an entity from the table. */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index,
    bool destruct);

/* Move a row from one table to another */
void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
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

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_entity_t *subjects,
    ecs_size_t *sizes,
    void **ptrs);

/* Get dirty state for table columns */
int32_t* flecs_table_get_dirty_state(
    ecs_table_t *table);

/* Get monitor for monitoring table changes */
int32_t* flecs_table_get_monitor(
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
    ecs_table_t *table);     
    
/* Replace data */
void flecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);

/* Merge data of one table into another table */
ecs_data_t* flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data);

void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2);

ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_ids_t *to_add,
    ecs_ids_t *added);

ecs_table_t *flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_ids_t *to_remove,
    ecs_ids_t *removed);

void flecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component);

const EcsComponent* flecs_component_from_id(
    const ecs_world_t *world,
    ecs_entity_t e);

int32_t flecs_table_switch_from_case(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t add);    

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

ecs_column_t *ecs_table_column_for_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

void flecs_query_set_iter(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count);

void flecs_run_monitor(
    ecs_world_t *world,
    ecs_matched_query_t *monitor,
    ecs_ids_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities);

bool flecs_query_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_query_t *query,
    ecs_match_failure_t *failure_info);

void flecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event);

void ecs_iter_init(
    ecs_iter_t *it);

void ecs_iter_fini(
    ecs_iter_t *it);

////////////////////////////////////////////////////////////////////////////////
//// Time API
////////////////////////////////////////////////////////////////////////////////

void flecs_os_time_setup(void);

uint64_t flecs_os_time_now(void);

void flecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec);

/* Increase or reset timer resolution (Windows only) */
FLECS_API
void flecs_increase_timer_resolution(
    bool enable);

////////////////////////////////////////////////////////////////////////////////
//// Utilities
////////////////////////////////////////////////////////////////////////////////

uint64_t flecs_hash(
    const void *data,
    ecs_size_t length);

/* Convert 64 bit signed integer to 16 bit */
int8_t flflecs_to_i8(
    int64_t v);

/* Convert 64 bit signed integer to 16 bit */
int16_t flecs_to_i16(
    int64_t v);

/* Convert 64 bit unsigned integer to 32 bit */
uint32_t flecs_to_u32(
    uint64_t v);        

/* Convert signed integer to size_t */
size_t flecs_to_size_t(
    int64_t size);

/* Convert size_t to ecs_size_t */
ecs_size_t flecs_from_size_t(
    size_t size);    

/* Get next power of 2 */
int32_t flecs_next_pow_of_2(
    int32_t n);

/* Convert 64bit value to ecs_record_t type. ecs_record_t is stored as 64bit int in the
 * entity index */
ecs_record_t flecs_to_row(
    uint64_t value);

/* Get 64bit integer from ecs_record_t */
uint64_t flecs_from_row(
    ecs_record_t record);

/* Get actual row from record row */
int32_t flecs_record_to_row(
    int32_t row, 
    bool *is_watched_out);

/* Convert actual row to record row */
int32_t flecs_row_to_record(
    int32_t row, 
    bool is_watched);

/* Convert type to entity array */
ecs_ids_t flecs_type_to_ids(
    ecs_type_t type); 

/* Convert a symbol name to an entity name by removing the prefix */
const char* flecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name);

/* Compare function for entity ids */
int flecs_entity_compare(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2); 

/* Compare function for entity ids which can be used with qsort */
int flecs_entity_compare_qsort(
    const void *e1,
    const void *e2);

uint64_t flecs_string_hash(
    const void *ptr);

ecs_hashmap_t flecs_table_hashmap_new(void);
ecs_hashmap_t flecs_string_hashmap_new(void);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *func);

#endif
