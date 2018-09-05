#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H

#include <ecs/platform.h>
#include <ecs/entity.h>

#ifdef __cplusplus
extern "C" {
#endif

extern EcsEntity *EcsType_e;

typedef struct EcsType {
    uint32_t size;
} EcsType;

#ifdef __cplusplus
}
#endif

#endif
