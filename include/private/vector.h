#ifndef REFLECS_VECTOR_H
#define REFLECS_VECTOR_H

#include <reflecs/reflecs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsVector EcsVector;
typedef struct EcsVectorChunk EcsVectorChunk;

typedef void (*EcsVectorMove)(void *to, void *from, void *ctx);

typedef struct EcsVectorParams {
    uint32_t element_size; /* Size of an element */
    uint32_t chunk_count;  /* Number of elements in a chunk */
    EcsComparator compare_action; /* Comparator function */
    EcsVectorMove move_action; /* Callback invoked when element is moved */
    void *ctx; /* Context passed to actions (if applicable) */
} EcsVectorParams;

typedef struct EcsVectorIter {
    const EcsVectorParams *params;
    EcsVectorChunk *current;
    uint32_t index;
} EcsVectorIter;

REFLECS_EXPORT
EcsVector* ecs_vector_new(
    const EcsVectorParams *params);

REFLECS_EXPORT
void ecs_vector_free(
    EcsVector *me);

REFLECS_EXPORT
void* ecs_vector_add(
    EcsVector *me,
    const EcsVectorParams *params);

REFLECS_EXPORT
EcsResult ecs_vector_remove(
    EcsVector *me,
    const EcsVectorParams *params,
    void *element);

REFLECS_EXPORT
EcsIter _ecs_vector_iter(
    EcsVector *me,
    const EcsVectorParams *params,
    EcsVectorIter *iter_data);

REFLECS_EXPORT
uint32_t ecs_vector_count(
    EcsVector *me);

REFLECS_EXPORT
void* ecs_vector_get(
    EcsVector *me,
    const EcsVectorParams *params,
    uint32_t index);

REFLECS_EXPORT
void ecs_vector_sort(
    EcsVector *me,
    const EcsVectorParams *params);

REFLECS_EXPORT
bool ecs_vector_compare_ptr(
    void *a,
    void *b);

#define ecs_vector_iter(me, params)\
    _ecs_vector_iter(me, params, alloca(sizeof(EcsVectorIter)))

#ifdef __cplusplus
}
#endif

#endif
