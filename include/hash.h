#ifndef ECS_HASH_H
#define ECS_HASH_H

#include <ecs/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

void ecs_hash(
    const void *key,
    size_t length,
    uint64_t *result);

#ifdef __cplusplus
}
#endif

#endif
