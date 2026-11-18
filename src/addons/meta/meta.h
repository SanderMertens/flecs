/**
 * @file addons/meta/meta.h
 * @brief Private functions for meta addon.
 */

#ifndef FLECS_META_PRIVATE_H
#define FLECS_META_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_META

#define flecs_meta_set_t(T, ptr, value)\
    ((T*)ptr)[0] = ((T)value)

bool flecs_meta_valid_digit(
    const char *str);

bool flecs_meta_type_is_integer(
    const ecs_world_t *world,
    ecs_entity_t type);

void flecs_meta_type_serializer_init(
    ecs_iter_t *it);

void flecs_type_serializer_dtor(
    EcsTypeSerializer *ptr);

ecs_meta_op_kind_t flecs_meta_primitive_to_op_kind(
    ecs_primitive_kind_t kind);

ecs_entity_t flecs_meta_primitive_type(
    ecs_primitive_kind_t kind);

ecs_entity_t flecs_meta_op_kind_to_type(
    ecs_meta_op_kind_t kind);

const char* flecs_meta_op_kind_str(
    ecs_meta_op_kind_t kind);

int flecs_value_blit_u64(
    const ecs_world_t *world,
    const ecs_value_t *key_value,
    uint64_t *key_out);

typedef enum flecs_meta_format_t {
    EcsMetaStr,
    EcsMetaExpr,
    EcsMetaExprPrecise,
    EcsMetaJson,
    EcsMetaExprPositional,
    EcsMetaExprPrecisePositional
} flecs_meta_format_t;

int flecs_meta_serialize(
    const ecs_world_t *world,
    const ecs_vec_t *ops,
    const void *ptr,
    ecs_strbuf_t *str,
    flecs_meta_format_t format);

int flecs_meta_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base,
    ecs_strbuf_t *str,
    bool is_expr);

int flecs_meta_ser_enum(
    const ecs_world_t *world,
    ecs_entity_t enum_type,
    ecs_meta_op_kind_t op_kind,
    ecs_map_t *constants,
    const void *ptr,
    ecs_strbuf_t *str);

int flecs_meta_ser_bitmask(
    const ecs_world_t *world,
    ecs_entity_t bitmask_type,
    ecs_map_t *constants,
    const void *ptr,
    const char *delim,
    ecs_strbuf_t *str);

int flecs_meta_parse_bitmask(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_map_t *constants,
    void *out,
    const char *value);

int flecs_meta_parse_enum(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_map_t *constants,
    void *out,
    const char *value);

int flecs_meta_find_constant(
    const ecs_world_t *world,
    const ecs_map_t *constants,
    const char *name,
    uint64_t *value_out);

int flecs_meta_ser_map_key(
    const ecs_world_t *world,
    const ecs_meta_op_t *op,
    const void *key,
    ecs_strbuf_t *str);

/* Map storage.
 * Values of map types are stored in an ecs_map_t. The functions below provide
 * access to map values for all supported key types, and should be used instead
 * of accessing the ecs_map_t directly.
 *
 * Keys of integer types (bool, char, byte, integers, entities, ids, enums and
 * bitmasks) are stored directly as map key. Values that fit in an
 * ecs_map_val_t are stored inline, larger values are stored in a separate
 * allocation.
 *
 * Keys of other types (strings) cannot be stored as map key. These keys are
 * stored in an allocated entry, together with the value. The map key is the
 * hash of the key and the map value points to the entry. Hash collisions are
 * resolved by probing the next map key, so that each map element occupies
 * exactly one map key. Supporting a new key type only requires a hash function
 * for the type and type hooks for lifecycle and equality.
 *
 * Functions that accept a key expect a pointer to a value of the key type. For
 * keys that are stored as map key, the key is read from the first bytes of the
 * pointed to value. */

/* Hash function for keys that cannot be stored as map key. */
typedef ecs_map_key_t (*flecs_meta_map_hash_t)(
    const void *key);

/* Storage information for a map type. */
typedef struct ecs_meta_map_info_t {
    const ecs_type_info_t *key_ti;   /* Key type info */
    const ecs_type_info_t *value_ti; /* Value type info */
    flecs_meta_map_hash_t hash;      /* Set if keys are stored in entries */
    ecs_size_t value_offset;         /* Offset of value in entry */
    ecs_size_t entry_size;           /* Size of entry */
} ecs_meta_map_info_t;

/* Iterator for map values. */
typedef struct ecs_meta_map_iter_t {
    ecs_meta_map_info_t info;
    ecs_map_iter_t it;
} ecs_meta_map_iter_t;

/* Test whether type can be used as map key type. */
bool flecs_meta_map_key_type_is_valid(
    const ecs_world_t *world,
    ecs_entity_t key_type);

/* Get storage information for key and value type. */
int flecs_meta_map_info_init(
    const ecs_world_t *world,
    ecs_entity_t key_type,
    ecs_entity_t value_type,
    ecs_meta_map_info_t *info);

/* Get storage information for map type. */
int flecs_meta_map_info_from_type(
    const ecs_world_t *world,
    ecs_entity_t map_type,
    ecs_meta_map_info_t *info);

/* Get value for key. Returns NULL if map does not contain key. */
void* flecs_meta_map_get(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map,
    const void *key);

/* Get or create value for key. New values are constructed. */
void* flecs_meta_map_ensure(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key,
    bool *is_new);

/* Remove key from map. Returns false if map did not contain key. */
bool flecs_meta_map_remove(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map,
    const void *key);

/* Remove all elements from map. */
void flecs_meta_map_clear(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map);

/* Remove all elements from map and free map storage. */
void flecs_meta_map_fini(
    const ecs_meta_map_info_t *info,
    ecs_map_t *map);

/* Replace contents of dst map with copy of src map. */
void flecs_meta_map_copy(
    const ecs_meta_map_info_t *info,
    ecs_map_t *dst,
    const ecs_map_t *src);

/* Iterate map. Map may be NULL or not initialized. */
ecs_meta_map_iter_t flecs_meta_map_iter(
    const ecs_meta_map_info_t *info,
    const ecs_map_t *map);

/* Progress map iterator. */
bool flecs_meta_map_next(
    ecs_meta_map_iter_t *it);

/* Key of current element. For keys stored as map key, this points to the
 * ecs_map_key_t that stores the key. */
void* flecs_meta_map_iter_key(
    const ecs_meta_map_iter_t *it);

/* Value of current element. */
void* flecs_meta_map_iter_value(
    const ecs_meta_map_iter_t *it);

void flecs_rtt_init_default_hooks(
    ecs_iter_t *it);

int flecs_value_ensure_type(
    const ecs_world_t *world,
    ecs_value_t *v,
    ecs_entity_t type);

void flecs_meta_value_init(
    ecs_world_t *world);

void flecs_value_hooks(
    ecs_world_t *world,
    ecs_type_hooks_t *hooks);

bool flecs_type_is_value(
    const ecs_world_t *world,
    ecs_entity_t type);

ecs_entity_t flecs_value_type_constraint(
    const ecs_world_t *world,
    ecs_entity_t value_type);

int flecs_meta_value_type_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str);

void flecs_meta_import_definitions(
    ecs_world_t *world);

int32_t flecs_struct_inherited_count(
    ecs_world_t *world,
    ecs_entity_t struct_type,
    const EcsStruct *s);

bool flecs_struct_is_derived_from(
    const ecs_world_t *world,
    ecs_entity_t derived,
    ecs_entity_t base);

#endif

#endif
