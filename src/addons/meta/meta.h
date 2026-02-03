/**
 * @file addons/meta/meta.h
 * @brief Private functions for meta addon.
 */

#ifndef FLECS_META_PRIVATE_H
#define FLECS_META_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_META

void flecs_meta_type_serializer_init(
    ecs_iter_t *it);

void flecs_type_serializer_dtor(
    EcsTypeSerializer *ptr);

ecs_meta_op_kind_t flecs_meta_primitive_to_op_kind(
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

void flecs_rtt_init_default_hooks(
    ecs_iter_t *it);

const char* flecs_type_kind_str(
    ecs_type_kind_t kind);

const char* flecs_primitive_type_kind_str(
    ecs_primitive_kind_t kind);

const char* flecs_meta_op_kind_str(
    ecs_meta_op_kind_t kind);

#endif

#endif
