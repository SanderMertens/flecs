#ifndef FLECS_VECTOR_H
#define FLECS_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_vector_t ecs_vector_t;

typedef int (*ecs_comparator_t)(
    const void* p1,
    const void *p2);

FLECS_EXPORT
ecs_vector_t* _ecs_vector_new(
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_new(T, elem_count) \
    _ecs_vector_new(sizeof(T), elem_count)

FLECS_EXPORT
void ecs_vector_free(
    ecs_vector_t *vector);

FLECS_EXPORT
void ecs_vector_clear(
    ecs_vector_t *vector);

FLECS_EXPORT
void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    size_t elem_size);

#define ecs_vector_add(vector, T) \
    (T*)_ecs_vector_add(vector, sizeof(T))

FLECS_EXPORT
void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_addn(vector, T, elem_count) \
    (T*)_ecs_vector_addn(vector, sizeof(T), elem_count)

FLECS_EXPORT
void* _ecs_vector_get(
    const ecs_vector_t *vector,
    size_t elem_size,
    int32_t index);

#define ecs_vector_get(vector, T, index) \
    (T*)_ecs_vector_get(vector, sizeof(T), index)

FLECS_EXPORT
void* _ecs_vector_last(
    const ecs_vector_t *vector,
    size_t elem_size);

#define ecs_vector_last(vector, T) \
    _ecs_vector_last(vector, sizeof(T))

FLECS_EXPORT
int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    size_t elem_size,
    void *elem);

#define ecs_vector_remove(vector, T, index) \
    _ecs_vector_remove(vector, sizeof(T), index)

FLECS_EXPORT
void ecs_vector_remove_last(
    ecs_vector_t *vector);

FLECS_EXPORT
bool _ecs_vector_pop(
    ecs_vector_t *vector,
    size_t elem_size,
    void *value);

#define ecs_vector_pop(vector, value) \
    _ecs_vector_pop(vector, sizeof(*value), value)

FLECS_EXPORT
int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    size_t elem_size,
    int32_t index);

#define ecs_vector_move_index(dst, src, T, index) \
    _ecs_vector_move_index(dst, src, sizeof(T), index)

FLECS_EXPORT
int32_t _ecs_vector_remove_index(
    ecs_vector_t *vector,
    size_t elem_size,
    int32_t index);

#define ecs_vector_remove_index(vector, T, index) \
    _ecs_vector_remove_index(vector, sizeof(T), index)

FLECS_EXPORT
void _ecs_vector_reclaim(
    ecs_vector_t **vector,
    size_t elem_size);

#define ecs_vector_reclaim(vector, T)\
    _ecs_vector_reclaim(vector, sizeof(T))

FLECS_EXPORT
int32_t _ecs_vector_grow(
    ecs_vector_t **vector,
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_grow(vector, T, size) \
    _ecs_vector_grow(vector, sizeof(T), size)

FLECS_EXPORT
int32_t _ecs_vector_set_size(
    ecs_vector_t **vector,
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_set_size(vector, T, size) \
    _ecs_vector_set_size(vector, sizeof(T), size)

FLECS_EXPORT
int32_t _ecs_vector_set_count(
    ecs_vector_t **vector,
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_set_count(vector, T, size) \
    _ecs_vector_set_count(vector, sizeof(T), size)

FLECS_EXPORT
int32_t _ecs_vector_set_min_size(
    ecs_vector_t **array_inout,
    size_t elem_size,
    int32_t elem_count);

#define ecs_vector_set_min_size(vector, T, size) \
    _ecs_vector_set_min_size(vector, sizeof(T), size)

FLECS_EXPORT
int32_t ecs_vector_count(
    const ecs_vector_t *vector);

FLECS_EXPORT
int32_t ecs_vector_size(
    const ecs_vector_t *vector);

FLECS_EXPORT
void* ecs_vector_first(
    const ecs_vector_t *vector);

FLECS_EXPORT
void _ecs_vector_sort(
    ecs_vector_t *vector,
    size_t elem_size,
    ecs_comparator_t compare_action);

#define ecs_vector_sort(vector, T, compare_action) \
    _ecs_vector_sort(vector, sizeof(T), compare_action)

FLECS_EXPORT
void _ecs_vector_memory(
    const ecs_vector_t *vector,
    size_t elem_size,
    int32_t *allocd,
    int32_t *used);

#define ecs_vector_memory(vector, T, allocd, used) \
    _ecs_vector_memory(vector, sizeof(T), allocd, used)

ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    size_t elem_size);

#define ecs_vector_copy(src, T) \
    _ecs_vector_copy(src, sizeof(T))    

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

#include <iostream>

namespace flecs {

template <typename T>
class vector {
public:
    vector(int32_t count = 0) : m_vector( nullptr ) { 
        if (count) {
            init(count);
        }
    }

    vector(std::initializer_list<T> elems) : m_vector( nullptr) {
        init(elems.size());
        *this = elems;
    }

    void operator=(std::initializer_list<T> elems) {
        for (auto elem : elems) {
            this->add(elem);
        }
    }

    void clear() {
        ecs_vector_clear(m_vector);
    }

    void add(T& value) {
        T* elem = ecs_vector_add(&m_vector, T);
        *elem = value;
    }

    void add(T&& value) {
        T* elem = ecs_vector_add(&m_vector, T);
        *elem = value;
    }    

    T& get(int32_t index) {
        return ecs_vector_get(m_vector, T, index);
    }

    T& first() {
        return static_cast<T*>(ecs_vector_first(m_vector));
    }

    T& last() {
        return ecs_vector_last(m_vector, T);
    }

    int32_t count() {
        return ecs_vector_count(m_vector);
    }

    int32_t size() {
        return ecs_vector_size(m_vector);
    }

private:
    void init(int32_t count) {
        m_vector = ecs_vector_new(T, count);
    }

    ecs_vector_t *m_vector;
};

}

#endif
#endif

#endif