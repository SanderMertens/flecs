#include <ecs/iter.h>

int ecs_iter_hasnext(
    EcsIter* iter)
{
    if (iter->hasnext) {
        if (!iter->hasnext(iter)) {
            ecs_iter_release(iter);
            iter->hasnext = NULL;
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

void* ecs_iter_next(
    EcsIter* iter)
{
    return iter->next(iter);
}

void ecs_iter_release(
    EcsIter* iter)
{
    if (iter->release) {
        iter->release(iter);
        iter->release = NULL;
    }
}
