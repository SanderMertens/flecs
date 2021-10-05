#ifndef FLECS_META_PRIVATE_H
#define FLECS_META_PRIVATE_H

#include "../../private_api.h"

void ecs_meta_type_serialized_init(
    ecs_iter_t *it);

void ecs_meta_dtor_serialized(
    EcsMetaTypeSerialized *ptr);
    
#endif
