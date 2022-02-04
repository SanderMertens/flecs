#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#include "private_types.h"
#include "table_cache.h"
#include "datastructures/qsort.h"

////////////////////////////////////////////////////////////////////////////////
//// Core bootstrap functions
////////////////////////////////////////////////////////////////////////////////

/* Bootstrap world */
void flecs_bootstrap(
    ecs_world_t *world);

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
    ecs_add_id(world, name, EcsFinal);\
    ecs_add_pair(world, name, EcsChildOf, ecs_get_scope(world));\
    ecs_set(world, name, EcsComponent, {.size = 0});\
    ecs_set_name(world, name, (char*)&#name[ecs_os_strlen(world->name_prefix)]);\
    ecs_set_symbol(world, name, #name)


/* Bootstrap functions for other parts in the code */
void flecs_bootstrap_hierarchy(ecs_world_t *world);

////////////////////////////////////////////////////////////////////////////////
//// Entity API
////////////////////////////////////////////////////////////////////////////////

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void flecs_add_flag(
    ecs_world_t *world,
    ecs_entity_t entity,
    uint32_t flag);

/* Obtain entity info */
bool flecs_get_info(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

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
ecs_type_info_t* flecs_get_or_create_c_info(
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

void flecs_table_set_empty(
    ecs_world_t *world,
    ecs_table_t *table);

ecs_id_record_t* flecs_ensure_id_record(
    ecs_world_t *world,
    ecs_id_t id);

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

const ecs_table_record_t* flecs_id_record_table(
    ecs_id_record_t *idr,
    ecs_table_t *table);

void flecs_process_pending_tables(
    const ecs_world_t *world);

ecs_id_record_t* flecs_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_iter_t *out);

ecs_id_record_t* flecs_empty_table_iter(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_iter_t *out);

bool flecs_idr_iter(
    ecs_id_record_t *idr,
    ecs_table_iter_t *out);

bool flecs_idr_empty_iter(
    ecs_id_record_t *idr,
    ecs_table_iter_t *out);

void flecs_register_add_ref(
    ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

void flecs_register_remove_ref(
    ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

void flecs_clear_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr);

void flecs_triggers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    ecs_ids_t *ids,
    ecs_entity_t event);

void flecs_set_triggers_notify(
    ecs_iter_t *it,
    ecs_observable_t *observable,
    ecs_ids_t *ids,
    ecs_entity_t event,
    ecs_id_t set_id);

bool flecs_check_triggers_for_event(
    const ecs_poly_t *world,
    ecs_id_t id,
    ecs_entity_t event);

void flecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger);

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer);

/* Suspend/resume readonly state. To fully support implicit registration of
 * components, it should be possible to register components while the world is
 * in readonly mode. It is not uncommon that a component is used first from
 * within a system, which are often ran while in readonly mode.
 * 
 * Suspending readonly mode is only allowed when the world is not multithreaded.
 * When a world is multithreaded, it is not safe to (even temporarily) leave
 * readonly mode, so a multithreaded application should always explicitly
 * register components in advance. 
 * 
 * These operations also suspend deferred mode.
 */
typedef struct {
    bool is_readonly;
    bool is_deferred;
    int32_t defer_count;
    ecs_entity_t scope;
    ecs_entity_t with;
    ecs_vector_t *defer_queue;
    ecs_stage_t *stage;
} ecs_suspend_readonly_state_t;

ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state);

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
    ecs_id_t id);

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
    ecs_id_t id,
    const ecs_entity_t **ids_out);

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_on_delete_action(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action);

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
    ecs_id_t id);

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id);

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
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
//// Notifications
//////////////////////////////////////////////////////////////////////////////// 

void flecs_notify_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    ecs_table_diff_t *diff);

void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_ids_t *ids,
    bool owned);


////////////////////////////////////////////////////////////////////////////////
//// Table API
////////////////////////////////////////////////////////////////////////////////

/** Find or create table for a set of components */
ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *type);

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
void flecs_table_merge(
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

void flecs_table_clear_add_edge(
    ecs_table_t *table,
    ecs_id_t id);

void flecs_table_clear_remove_edge(
    ecs_table_t *table,
    ecs_id_t id);

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

/* Match table with term */
bool flecs_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    int32_t *match_indices,
    bool first);

/* Match table with filter */
bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_entity_t *subjects,
    int32_t *match_indices,
    int32_t *matches_left,
    bool first,
    int32_t skip_term);

bool flecs_query_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_query_t *query);

void flecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event);

void flecs_iter_init(
    ecs_iter_t *it);

void flecs_iter_populate_data(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    void **ptrs,
    ecs_size_t *sizes);

bool flecs_iter_next_row(
    ecs_iter_t *it);

bool flecs_iter_next_instanced(
    ecs_iter_t *it,
    bool result);


////////////////////////////////////////////////////////////////////////////////
//// Poly API
////////////////////////////////////////////////////////////////////////////////

/* Initialize object header & mixins for specified type */
void* _ecs_poly_init(
    ecs_poly_t *object,
    int32_t kind,
    ecs_size_t size,
    ecs_mixins_t *mixins);

#define ecs_poly_init(object, type)\
    _ecs_poly_init(object, ECS_##type##_MAGIC, sizeof(type), &type##_mixins)

/* Deinitialize object for specified type */
void _ecs_poly_fini(
    ecs_poly_t *object,
    int32_t kind);

#define ecs_poly_fini(object, type)\
    _ecs_poly_fini(object, ECS_##type##_MAGIC)

/* Utility functions for creating an object on the heap */
#define ecs_poly_new(type)\
    (type*)ecs_poly_init(ecs_os_calloc_t(type), type)

#define ecs_poly_free(obj, type)\
    ecs_poly_fini(obj, type);\
    ecs_os_free(obj)

/* Utilities for testing/asserting an object type */
#ifndef NDEBUG
void _ecs_poly_assert(
    const ecs_poly_t *object,
    int32_t type,
    const char *file,
    int32_t line);

#define ecs_poly_assert(object, type)\
    _ecs_poly_assert(object, ECS_##type##_MAGIC, __FILE__, __LINE__)
#else
#define ecs_poly_assert(object, type)
#endif

bool _ecs_poly_is(
    const ecs_poly_t *object,
    int32_t type);

#define ecs_poly_is(object, type)\
    _ecs_poly_is(object, ECS_##type##_MAGIC)

/* Utility functions for getting a mixin from an object */
ecs_iterable_t* ecs_get_iterable(
    const ecs_poly_t *poly);

ecs_observable_t* ecs_get_observable(
    const ecs_poly_t *object);


////////////////////////////////////////////////////////////////////////////////
//// Observables
////////////////////////////////////////////////////////////////////////////////

void flecs_observable_init(
    ecs_observable_t *observable);

void flecs_observable_fini(
    ecs_observable_t *observable);


////////////////////////////////////////////////////////////////////////////////
//// Safe(r) integer casting
////////////////////////////////////////////////////////////////////////////////

#define FLECS_CONVERSION_ERR(T, value)\
    "illegal conversion from value " #value " to type " #T

#define flecs_signed_char__ (CHAR_MIN < 0)
#define flecs_signed_short__ true
#define flecs_signed_int__ true
#define flecs_signed_long__ true
#define flecs_signed_size_t__ false
#define flecs_signed_int8_t__ true
#define flecs_signed_int16_t__ true
#define flecs_signed_int32_t__ true
#define flecs_signed_int64_t__ true
#define flecs_signed_intptr_t__ true
#define flecs_signed_uint8_t__ false
#define flecs_signed_uint16_t__ false
#define flecs_signed_uint32_t__ false
#define flecs_signed_uint64_t__ false
#define flecs_signed_uintptr_t__ false
#define flecs_signed_ecs_size_t__ true
#define flecs_signed_ecs_entity_t__ false

uint64_t _flecs_ito(
    size_t dst_size,
    bool dst_signed,
    bool lt_zero,
    uint64_t value,
    const char *err);

#ifndef NDEBUG
#define flecs_ito(T, value)\
    (T)_flecs_ito(\
        sizeof(T),\
        flecs_signed_##T##__,\
        value < 0,\
        (uint64_t)value,\
        FLECS_CONVERSION_ERR(T, value))

#define flecs_uto(T, value)\
    (T)_flecs_ito(\
        sizeof(T),\
        flecs_signed_##T##__,\
        false,\
        (uint64_t)value,\
        FLECS_CONVERSION_ERR(T, value))
#else
#define flecs_ito(T, value) (T)value
#define flecs_uto(T, value) (T)value
#endif

#define flecs_itosize(value) flecs_ito(size_t, (value))
#define flecs_utosize(value) flecs_uto(ecs_size_t, (value))
#define flecs_itoi16(value) flecs_ito(int16_t, (value))
#define flecs_itoi32(value) flecs_ito(int32_t, (value))

////////////////////////////////////////////////////////////////////////////////
//// Utilities
////////////////////////////////////////////////////////////////////////////////

uint64_t flecs_hash(
    const void *data,
    ecs_size_t length);

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
uint32_t flecs_record_to_row(
    uint32_t row, 
    bool *is_watched_out);

/* Convert actual row to record row */
uint32_t flecs_row_to_record(
    uint32_t row, 
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

/* Convert floating point to string */
char * ecs_ftoa(
    double f, 
    char * buf, 
    int precision);

uint64_t flecs_string_hash(
    const void *ptr);

ecs_hashmap_t flecs_table_hashmap_new(void);
ecs_hashmap_t _flecs_string_hashmap_new(ecs_size_t size);

#define flecs_string_hashmap_new(T)\
    _flecs_string_hashmap_new(ECS_SIZEOF(T))

ecs_hashed_string_t ecs_get_hashed_string(
    const char *name,
    ecs_size_t length,
    uint64_t hash);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *func);

#endif
