/**
 * @file private_api.h
 * @brief Private functions.
 */

#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#include "private_types.h"
#include "storage/table_cache.h"
#include "storage/id_index.h"
#include "query/query.h"
#include "observable.h"
#include "iter.h"
#include "poly.h"
#include "stage.h"
#include "world.h"
#include "datastructures/name_index.h"


////////////////////////////////////////////////////////////////////////////////
//// Bootstrap API
////////////////////////////////////////////////////////////////////////////////

/* Bootstrap world */
void flecs_bootstrap(
    ecs_world_t *world);

#define flecs_bootstrap_component(world, id_)\
    ecs_component_init(world, &(ecs_component_desc_t){\
        .entity = ecs_entity(world, { .id = ecs_id(id_), .name = #id_, .symbol = #id_ }),\
        .type.size = sizeof(id_),\
        .type.alignment = ECS_ALIGNOF(id_)\
    });

#define flecs_bootstrap_tag(world, name)\
    ecs_make_alive(world, name);\
    ecs_add_id(world, name, EcsFinal);\
    ecs_add_pair(world, name, EcsChildOf, ecs_get_scope(world));\
    ecs_set_name(world, name, (const char*)&#name[ecs_os_strlen(world->info.name_prefix)]);\
    ecs_set_symbol(world, name, #name);

#define flecs_bootstrap_trait(world, name)\
    flecs_bootstrap_tag(world, name)\
    ecs_add_id(world, name, EcsTrait)


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

void flecs_record_add_flag(
    ecs_record_t *record,
    uint32_t flag);

ecs_entity_t flecs_get_oneof(
    const ecs_world_t *world,
    ecs_entity_t e);

void flecs_notify_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *other_table,
    int32_t row,
    int32_t count,
    const ecs_type_t *diff);

void flecs_notify_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    ecs_type_t *type,
    bool owned);

int32_t flecs_relation_depth(
    const ecs_world_t *world,
    ecs_entity_t r,
    const ecs_table_t *table);

void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    int32_t row,
    int32_t count);

void* flecs_get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_id_record_t *table_index,
    int32_t recur_depth);

void flecs_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t count,
    int32_t row,
    ecs_entity_t *entities,
    void *ptr,
    ecs_id_t id,
    const ecs_type_info_t *ti,
    ecs_entity_t event,
    ecs_iter_action_t hook);

////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

void flecs_query_apply_iter_flags(
    ecs_iter_t *it,
    const ecs_query_t *query);

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

uint64_t flecs_ito_(
    size_t dst_size,
    bool dst_signed,
    bool lt_zero,
    uint64_t value,
    const char *err);

#ifndef FLECS_NDEBUG
#define flecs_ito(T, value)\
    (T)flecs_ito_(\
        sizeof(T),\
        flecs_signed_##T##__,\
        (value) < 0,\
        (uint64_t)(value),\
        FLECS_CONVERSION_ERR(T, (value)))

#define flecs_uto(T, value)\
    (T)flecs_ito_(\
        sizeof(T),\
        flecs_signed_##T##__,\
        false,\
        (uint64_t)(value),\
        FLECS_CONVERSION_ERR(T, (value)))
#else
#define flecs_ito(T, value) (T)(value)
#define flecs_uto(T, value) (T)(value)
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

uint64_t flecs_wyhash(
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

/* Convert a symbol name to an entity name by removing the prefix */
const char* flecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name);

/* Compare function for entity ids used for order_by */
int flecs_entity_compare(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2); 

/* Compare function for component ids used for qsort */
int flecs_id_qsort_cmp(
    const void *a, 
    const void *b);

/* Load file contents into string */
char* flecs_load_from_file(
    const char *filename);

bool flecs_name_is_id(
    const char *name);

ecs_entity_t flecs_name_to_id(
    const char *name);

/* Convert floating point to string */
char * ecs_ftoa(
    double f, 
    char * buf, 
    int precision);

uint64_t flecs_string_hash(
    const void *ptr);

void flecs_table_hashmap_init(
    ecs_world_t *world,
    ecs_hashmap_t *hm);

void flecs_colorize_buf(
    char *msg,
    bool enable_colors,
    ecs_strbuf_t *buf);

int32_t flecs_search_w_idr(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t *id_out,
    ecs_id_record_t *idr);

int32_t flecs_search_relation_w_idr(
    const ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    ecs_flags64_t flags,
    ecs_entity_t *subject_out,
    ecs_id_t *id_out,
    struct ecs_table_record_t **tr_out,
    ecs_id_record_t *idr);

bool flecs_type_can_inherit_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_id_record_t *idr,
    ecs_id_t id);

int ecs_term_finalize(
    const ecs_world_t *world,
    ecs_term_t *term);

int32_t flecs_query_pivot_term(
    const ecs_world_t *world,
    const ecs_query_t *query);

#endif
