#ifndef ECS_VECTOR_H
#define ECS_VECTOR_H

#include <ecs/platform.h>
#include <ecs/iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsVectorChunk {
    struct EcsVectorChunk *next;
    void *buffer;
} EcsVectorChunk;

typedef struct EcsVector {
    uint32_t count;
    EcsVectorChunk *current;
    EcsVectorChunk first;
} EcsVector;

typedef struct EcsVectorParams {
    uint32_t element_size; /* Size of an element */
    uint32_t chunk_count;  /* Number of elements in a chunk */
} EcsVectorParams;

typedef struct EcsVectorIter {
    const EcsVectorParams *params;
    EcsVectorChunk *current;
    uint32_t index;
} EcsVectorIter;

EcsVector* ecs_vector_new(
    const EcsVectorParams *params);

void ecs_vector_free(
    EcsVector *me);

void* ecs_vector_add(
    EcsVector *me,
    const EcsVectorParams *params);

EcsResult ecs_vector_remove(
    EcsVector *me,
    const EcsVectorParams *params,
    void *element);

EcsIter _ecs_vector_iter(
    EcsVector *me,
    const EcsVectorParams *params,
    EcsVectorIter *iter_data);

#define ecs_vector_iter(me, params)\
    _ecs_vector_iter(me, params, alloca(sizeof(EcsVectorIter)))

#ifdef __cplusplus
}
#endif

#endif
