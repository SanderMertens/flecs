/**
 * @file addons/seri/json.h
 * @brief Internal functions for SERIALIZE addon.
 */

#ifndef FLECS_SERIALIZE_PRIVATE_H
#define FLECS_SERIALIZE_PRIVATE_H

#include "../../private_api.h"

#ifdef FLECS_SERIALIZE
#include "flecs/addons/serialize.h"

int flecs_ser_type(
    const ecs_world_t *world,
    const ecs_vec_t *v_ops,
    const void *base, 
    ecs_visitor_desc_t *visitor_desc);

int flecs_ser_array_w_type_data(
    const ecs_world_t *world,
    const void *ptr,
    int32_t count,
    ecs_visitor_desc_t *visitor_desc,
    const EcsComponent *comp,
    const EcsTypeSerializer *ser);

ecs_primitive_kind_t flecs_op_to_primitive_kind(
    ecs_meta_type_op_kind_t kind);

#endif

#endif /* FLECS_SERIALIZE_PRIVATE_H */
