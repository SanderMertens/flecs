/**
 * @file meta/meta.h
 * @brief Private functions for meta addon.
 */

#ifndef FLECS_META_PRIVATE_H
#define FLECS_META_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_META

void ecs_meta_type_serialized_init(
    ecs_iter_t *it);

void ecs_meta_dtor_serialized(
    EcsMetaTypeSerialized *ptr);

ecs_meta_type_op_kind_t flecs_meta_primitive_to_op_kind(
    ecs_primitive_kind_t kind);

bool flecs_unit_validate(
    ecs_world_t *world,
    ecs_entity_t t,
    EcsUnit *data);

#endif
    
#endif
