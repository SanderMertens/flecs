#ifndef REFLECS_COMPONENTS_H
#define REFLECS_COMPONENTS_H

#include <reflecs/platform.h>
#include <reflecs/entity.h>

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
