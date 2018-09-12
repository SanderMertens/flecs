#ifndef REFLECS_ITER_H_
#define REFLECS_ITER_H_

#include <reflecs/platform.h>

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

REFLECS_EXPORT
bool ecs_iter_hasnext(
    EcsIter* iter);

REFLECS_EXPORT
void* ecs_iter_next(
    EcsIter* iter);

REFLECS_EXPORT
void ecs_iter_release(
    EcsIter* iter);

#ifdef __cplusplus
}
#endif

#endif
