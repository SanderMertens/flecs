#ifndef REFLECS_HASH_H
#define REFLECS_HASH_H

#include <reflecs/platform.h>

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
