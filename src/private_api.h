/**
 * @file private_api.h
 * @brief Private functions.
 */

#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stddef.h>

#include "flecs.h"
#include "flecs/datastructures/bitset.h"
#include "datastructures/name_index.h"
#include "storage/entity_index.h"
#include "storage/table_cache.h"
#include "storage/component_index.h"
#include "storage/table.h"
#include "storage/sparse_storage.h"
#include "storage/ordered_children.h"
#include "storage/non_fragmenting_childof.h"
#include "query/query.h"
#include "component_actions.h"
#include "entity_name.h"
#include "commands.h"
#include "entity.h"
#include "instantiate.h"
#include "observable.h"
#include "iter.h"
#include "poly.h"
#include "tree_spawner.h"
#include "stage.h"
#include "world.h"
#include "addons/journal.h"

/* Used in id records to keep track of entities used with id flags */
extern const ecs_entity_t EcsFlag;

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
    ecs_add_pair(world, name, EcsChildOf, ecs_get_scope(world));\
    ecs_set_name(world, name, (const char*)&#name[ecs_os_strlen(world->info.name_prefix)]);

#define flecs_bootstrap_trait(world, name)\
    flecs_bootstrap_tag(world, name)\
    ecs_add_id(world, name, EcsTrait)


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

/* Check if component is valid, return reason if it's not */
const char* flecs_id_invalid_reason(
    const ecs_world_t *world,
    ecs_id_t id);

/* Generate 64bit hash from buffer. */
uint64_t flecs_hash(
    const void *data,
    ecs_size_t length);

/* Get next power of 2 */
int32_t flecs_next_pow_of_2(
    int32_t n);

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

/* Test whether entity name is an entity id (starts with a #). */
bool flecs_name_is_id(
    const char *name);

/* Convert entity name to entity id. */
ecs_entity_t flecs_name_to_id(
    const char *name);

/* Convert floating point to string */
char * ecs_ftoa(
    double f, 
    char * buf, 
    int precision);

/* Replace #[color] tokens with terminal color symbols. */
void flecs_colorize_buf(
    char *msg,
    bool enable_colors,
    ecs_strbuf_t *buf);

/* Check whether id can be inherited. */
bool flecs_type_can_inherit_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_component_record_t *cr,
    ecs_id_t id);

/* Cleanup type info data. */
void flecs_fini_type_info(
    ecs_world_t *world);

const ecs_type_info_t* flecs_determine_type_info_for_component(
    const ecs_world_t *world,
    ecs_id_t component);

ecs_size_t flecs_type_size(
    ecs_world_t *world, 
    ecs_entity_t type);

/* Utility for using allocated strings in assert/error messages */
const char* flecs_errstr(
    char *str);
const char* flecs_errstr_1(
    char *str);
const char* flecs_errstr_2(
    char *str);
const char* flecs_errstr_3(
    char *str);
const char* flecs_errstr_4(
    char *str);
const char* flecs_errstr_5(
    char *str);

#endif
