#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_sparse_t ecs_sparse_t;

FLECS_EXPORT
ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size);

FLECS_EXPORT
void ecs_sparse_set_id_source(
    ecs_sparse_t *sparse,
    uint64_t *id_source);

#define ecs_sparse_new(type)\
    _ecs_sparse_new(sizeof(type))

FLECS_EXPORT
void ecs_sparse_free(
    ecs_sparse_t *sparse);

FLECS_EXPORT
void ecs_sparse_clear(
    ecs_sparse_t *sparse);

FLECS_EXPORT
void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_add(sparse, type)\
    ((type*)_ecs_sparse_add(sparse, sizeof(type)))

FLECS_EXPORT
uint64_t ecs_sparse_last_id(
    ecs_sparse_t *sparse);

FLECS_EXPORT
uint64_t ecs_sparse_new_id(
    ecs_sparse_t *sparse);

FLECS_EXPORT
const uint64_t* ecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t count);

FLECS_EXPORT
void _ecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t index);

#define ecs_sparse_remove(sparse, index)\
    _ecs_sparse_remove(sparse, index)

FLECS_EXPORT
void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define ecs_sparse_get(sparse, type, index)\
    ((type*)_ecs_sparse_get(sparse, sizeof(type), index))

FLECS_EXPORT
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

FLECS_EXPORT
int32_t ecs_sparse_size(
    const ecs_sparse_t *sparse);

FLECS_EXPORT
void* _ecs_sparse_get_sparse(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t index);

#define ecs_sparse_get_sparse(sparse, type, index)\
    ((type*)_ecs_sparse_get_sparse(sparse, sizeof(type), index))

FLECS_EXPORT
void* _ecs_sparse_get_or_create(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t index);

#define ecs_sparse_get_or_create(sparse, type, index)\
    ((type*)_ecs_sparse_get_or_create(sparse, sizeof(type), index))

FLECS_EXPORT
void* _ecs_sparse_set(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t index,
    void *value);

#define ecs_sparse_set(sparse, type, index, value)\
    ((type*)_ecs_sparse_set(sparse, sizeof(type), index, value))


FLECS_EXPORT
const uint64_t* ecs_sparse_ids(
    const ecs_sparse_t *sparse);

FLECS_EXPORT
void ecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count);

FLECS_EXPORT
void ecs_sparse_grow(
    ecs_sparse_t *sparse,
    int32_t count);

FLECS_EXPORT
ecs_sparse_t* ecs_sparse_copy(
    const ecs_sparse_t *src);    

FLECS_EXPORT
void ecs_sparse_restore(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src);

FLECS_EXPORT
void ecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used);

#ifndef FLECS_LEGACY
#define ecs_sparse_each(sparse, T, var, ...)\
    {\
        int var##_i, var##_count = ecs_sparse_count(sparse);\
        for (var##_i = 0; var##_i < var##_count; var##_i ++) {\
            T* var = ecs_sparse_get(sparse, T, var##_i);\
            __VA_ARGS__\
        }\
    }
#endif

#ifdef __cplusplus
}
#endif

#endif
