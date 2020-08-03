#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#include "private_types.h"

////////////////////////////////////////////////////////////////////////////////
//// Core bootstrap functions
////////////////////////////////////////////////////////////////////////////////

#define ECS_TYPE_DECL(component)\
static const ecs_entity_t __##component = ecs_entity(component);\
ECS_VECTOR_DECL(FLECS__T##component, ecs_entity_t, 1)

#define ECS_TYPE_IMPL(component)\
ECS_VECTOR_IMPL(FLECS__T##component, ecs_entity_t, &__##component, 1)

/* Bootstrap world */
void ecs_bootstrap(
    ecs_world_t *world);

ecs_type_t ecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity);

#define ecs_bootstrap_component(world, name)\
    ecs_new_component(world, ecs_entity(name), #name, sizeof(name), ECS_ALIGNOF(name))

#define ecs_bootstrap_tag(world, name)\
    ecs_set(world, name, EcsName, {.value = &#name[ecs_os_strlen("Ecs")], .symbol = #name});\
    ecs_add_entity(world, name, ECS_CHILDOF | ecs_get_scope(world))


////////////////////////////////////////////////////////////////////////////////
//// Entity API
////////////////////////////////////////////////////////////////////////////////

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void ecs_set_watch(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

/* Does one of the entity containers has specified component */
ecs_entity_t ecs_find_in_type(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_entity_t component,
    ecs_entity_t flags);

/* Obtain entity info */
bool ecs_get_info(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

void ecs_run_monitors(
    ecs_world_t *world, 
    ecs_stage_t *stage, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors);


////////////////////////////////////////////////////////////////////////////////
//// World API
////////////////////////////////////////////////////////////////////////////////

/* Notify systems that there is a new table, which triggers matching */
void ecs_notify_queries_of_table(
    ecs_world_t *world,
    ecs_table_t *table);

/* Get current thread-specific stage */
ecs_stage_t *ecs_get_stage(
    ecs_world_t **world_ptr);

/* Get component callbacks */
ecs_c_info_t *ecs_get_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks */
ecs_c_info_t * ecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

void ecs_eval_component_monitors(
    ecs_world_t *world);

void ecs_component_monitor_mark(
    ecs_component_monitor_t *mon,
    ecs_entity_t component);

void ecs_component_monitor_register(
    ecs_component_monitor_t *mon,
    ecs_entity_t component,
    ecs_query_t *query);

bool ecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entities_t *components,
    const void *value,
    ecs_size_t size);

void ecs_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage);

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);

ecs_flags32_t ecs_get_component_action_flags(
    ecs_c_info_t *c_info);    

void ecs_notify_tables_of_component_actions(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_c_info_t *c_info);

////////////////////////////////////////////////////////////////////////////////
//// Stage API
////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////
//// Type API
////////////////////////////////////////////////////////////////////////////////

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

/* Add component to type */
ecs_type_t ecs_type_add_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type,
    ecs_entity_t component);

/* Find entity in prefabs of type */
ecs_entity_t ecs_find_entity_in_prefabs(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component,
    ecs_entity_t previous);

void ecs_get_column_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *components,
    ecs_column_info_t *cinfo,
    bool get_all);

void ecs_components_construct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count);

void ecs_components_destruct(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_column_info_t *component_info,
    int32_t component_count);

void ecs_run_add_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *added,
    ecs_comp_mask_t set_mask,
    bool get_all,
    bool run_on_set);   

void ecs_run_remove_actions(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_entities_t *removed,
    bool get_all);

void ecs_run_set_systems(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool set_all);


////////////////////////////////////////////////////////////////////////////////
//// Table API
////////////////////////////////////////////////////////////////////////////////

/* Initialize root table */
void ecs_init_root_table(
    ecs_world_t *world,
    ecs_stage_t *stage);

void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index);

/* Unset components in table */
void ecs_table_unset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Destruct columns */
void ecs_table_destruct(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_data_t *data, 
    int32_t row, 
    int32_t count);

/* Free table */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Merge table data */
void ecs_table_merge_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);
    
/* Replace data */
void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);

/* Merge data of one table into another table */
void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table);

void ecs_table_swap(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2);

ecs_table_t *ecs_table_traverse_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *to_add,
    ecs_entities_t *added);

ecs_table_t *ecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed);    

void ecs_table_mark_dirty_w_index(
    ecs_table_t *table,
    int32_t index);

void ecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component);

const EcsComponent* ecs_component_from_id(
    ecs_world_t *world,
    ecs_entity_t e);

////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system, 
    ecs_sig_t *sig);

void ecs_query_activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active);

void ecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table);

void ecs_query_set_iter(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count);

void ecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *query);

void ecs_run_monitor(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_matched_query_t *monitor,
    ecs_entities_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities);

bool ecs_query_match(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    ecs_match_failure_t *failure_info);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

void ecs_sig_init(
    ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_sig_t *sig);

void ecs_sig_deinit(
    ecs_sig_t *sig);

/* Check if all non-table column constraints are met */
bool ecs_sig_check_constraints(
    ecs_world_t *world,
    ecs_sig_t *sig);


////////////////////////////////////////////////////////////////////////////////
//// Time API
////////////////////////////////////////////////////////////////////////////////

void ecs_os_time_setup(void);

uint64_t ecs_os_time_now(void);

void ecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec);

/* Increase or reset timer resolution (Windows only) */
FLECS_EXPORT
void ecs_increase_timer_resolution(
    bool enable);

////////////////////////////////////////////////////////////////////////////////
//// Utilities
////////////////////////////////////////////////////////////////////////////////

/* Convert 64 bit signed integer to 16 bit */
int8_t ecs_to_i8(
    int64_t v);

/* Convert 64 bit signed integer to 16 bit */
int16_t ecs_to_i16(
    int64_t v);

/* Convert 64 bit signed integer to 32 bit */
int32_t ecs_to_i32(
    int64_t v);    

/* Convert signed integer to size_t */
size_t ecs_to_size_t(
    int64_t size);

/* Convert size_t to ecs_size_t */
ecs_size_t ecs_from_size_t(
    size_t size);    

/* Convert int64_t to entity */
ecs_entity_t ecs_to_entity(
    int64_t v);

/* Convert 64bit value to ecs_record_t type. ecs_record_t is stored as 64bit int in the
 * entity index */
ecs_record_t ecs_to_row(
    uint64_t value);

/* Get 64bit integer from ecs_record_t */
uint64_t ecs_from_row(
    ecs_record_t record);

/* Get actual row from record row */
int32_t ecs_record_to_row(
    int32_t row, 
    bool *is_watched_out);

/* Convert actual row to record row */
int32_t ecs_row_to_record(
    int32_t row, 
    bool is_watched);

/* Convert type to entity array */
ecs_entities_t ecs_type_to_entities(
    ecs_type_t type); 

/* Convert a symbol name to an entity name by removing the prefix */
const char* ecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name); 

/* Lookup an entity by name with a specific type */
ecs_entity_t ecs_lookup_w_type(
    ecs_world_t *world,
    const char *name,
    ecs_type_t type);      

/* Utility that print a descriptive error string*/
//void ecs_print_error_string(const char *error_description, const char* signature, const char* system_id, const char* component_id);
//void ecs_print_error_string(const char* signature, const char *system_id, const char *error_description, const char *component_id);

/* Utility that parses system signature */
int ecs_parse_expr(
    ecs_world_t *world,
    const char *sig,
    ecs_parse_action_t action,
    const char *system_id,
    void *ctx);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *func);

#endif
