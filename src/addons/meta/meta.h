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

void flecs_meta_type_serializer_init(
    ecs_iter_t *it);

void flecs_type_serializer_dtor(
    EcsTypeSerializer *ptr);

ecs_meta_op_kind_t flecs_meta_primitive_to_op_kind(
    ecs_primitive_kind_t kind);

ecs_entity_t flecs_meta_op_kind_to_type(
    ecs_meta_op_kind_t kind);

const char* flecs_meta_op_kind_str(
    ecs_meta_op_kind_t kind);

int flecs_value_blit_u64(
    const ecs_world_t *world,
    const ecs_value_t *key_value,
    uint64_t *key_out);

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
    ecs_map_key_t key,
    ecs_strbuf_t *str);

void flecs_meta_map_clear(
    ecs_map_t *map,
    const ecs_type_info_t *value_ti);

int flecs_meta_map_set_key(
    ecs_meta_cursor_t *cursor,
    const ecs_value_t *key_value);

void flecs_rtt_init_default_hooks(
    ecs_iter_t *it);

int flecs_value_ensure_type(
    const ecs_world_t *world,
    ecs_value_t *v,
    ecs_entity_t type);

void flecs_meta_value_init(
    ecs_world_t *world);

int flecs_meta_value_type_str(
    const ecs_world_t *world,
    ecs_entity_t type,
    ecs_strbuf_t *str);

void flecs_meta_import_definitions(
    ecs_world_t *world);

#endif

#endif
