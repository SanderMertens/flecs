#ifndef ECS_VECTOR_H
#define ECS_VECTOR_H

#include <ecs/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsVectorChunk {
    uint32_t count;
    void *buffer;
    struct EcsVectorChunk *next;
} EcsVectorChunk;

typedef struct EcsVector {
    uint32_t size;
    uint32_t buffer_count;
    bool initial_buffer_set;
    EcsVectorChunk *current;
    EcsVectorChunk first;
} EcsVector;

EcsVector* ecs_vector_new(
    uint32_t size,
    uint32_t block_size,
    void *initial_buffer);

void ecs_vector_free(
    EcsVector *me);

void* ecs_vector_add(
    EcsVector *me);

EcsResult ecs_vector_remove(
    EcsVector *me,
    void *el);

#ifdef __cplusplus
}
#endif

#endif
