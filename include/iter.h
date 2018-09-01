#ifndef ECS_ITER_H_
#define ECS_ITER_H_

#include <ecs/platform.h>

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

int ecs_iter_hasnext(EcsIter* iter);
void* ecs_iter_next(EcsIter* iter);
void ecs_iter_release(EcsIter* iter);

#ifdef __cplusplus
}
#endif

#endif
