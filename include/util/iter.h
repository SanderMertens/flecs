#ifndef FLECS_ITER_H
#define FLECS_ITER_H

#include "../flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsIter EcsIter;
struct EcsIter {
    void *ctx;
    void *data;
    bool (*hasnext)(EcsIter*);
    void* (*next)(EcsIter*);
    void (*release)(EcsIter*);
};

FLECS_EXPORT
bool ecs_iter_hasnext(
    EcsIter* iter);

FLECS_EXPORT
void* ecs_iter_next(
    EcsIter* iter);

FLECS_EXPORT
void ecs_iter_release(
    EcsIter* iter);

#ifdef __cplusplus
}
#endif

#endif

