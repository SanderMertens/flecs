/**
 * @file addons/meta/meta.h
 * @brief Private functions for meta addon.
 */

#ifndef FLECS_META_PRIVATE_H
#define FLECS_META_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_META

void ecs_meta_type_serialized_init(
    ecs_iter_t *it);

void ecs_meta_dtor_serialized(
    EcsTypeSerializer *ptr);

ecs_meta_type_op_kind_t flecs_meta_primitive_to_op_kind(
    ecs_primitive_kind_t kind);

bool flecs_unit_validate(
    ecs_world_t *world,
    ecs_entity_t t,
    EcsUnit *data);

void flecs_meta_import_definitions(
    ecs_world_t *world);

int flecs_expr_ser_primitive(
    const ecs_world_t *world,
    ecs_primitive_kind_t kind,
    const void *base, 
    ecs_strbuf_t *str,
    bool is_expr);

void flecs_meta_rtt_dtor(void *ptr, int32_t count, const ecs_type_info_t *type_info); 
void flecs_meta_rtt_ctor(void *ptr, int32_t count, const ecs_type_info_t *type_info); 
void flecs_meta_rtt_move(void *dst_ptr, void *src_ptr, int32_t count, const ecs_type_info_t *type_info);
void flecs_meta_rtt_copy(void *dst_ptr, const void *src_ptr, int32_t count, const ecs_type_info_t *type_info);

#endif

#endif
