/**
 * @file vector.h
 * @brief Vector datastructure.
 */

#ifndef FLECS_VECTOR_H
#define FLECS_VECTOR_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FLECS_NDEBUG
#define ECS_VECTOR_T_SIZE\
    (ECS_SIZEOF(int32_t) + ECS_SIZEOF(int32_t))
#else
#define ECS_VECTOR_T_SIZE\
    (ECS_SIZEOF(int32_t) + ECS_SIZEOF(int32_t) + ECS_SIZEOF(int64_t))
#endif

/* Compute the header size of the vector from size & alignment */
#define ECS_VECTOR_U(size, alignment) size, ECS_CAST(int16_t, ECS_MAX(ECS_VECTOR_T_SIZE, alignment))

/* Compute the header size of the vector from a provided compile-time type */
#define ECS_VECTOR_T(T) ECS_VECTOR_U(ECS_SIZEOF(T), ECS_ALIGNOF(T))

typedef struct ecs_vector_t ecs_vector_t;

typedef int (*ecs_comparator_t)(
    const void* p1,
    const void *p2);

/** Create new vector. */
FLECS_API
ecs_vector_t* _ecs_vector_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_new(T, elem_count) \
    _ecs_vector_new(ECS_VECTOR_T(T), elem_count)

#define ecs_vector_new_t(size, alignment, elem_count) \
    _ecs_vector_new(ECS_VECTOR_U(size, alignment), elem_count)    

/* Create new vector, initialize it with provided array */
FLECS_API
ecs_vector_t* _ecs_vector_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array);

#define ecs_vector_from_array(T, elem_count, array)\
    _ecs_vector_from_array(ECS_VECTOR_T(T), elem_count, array)

/* Initialize vector with zero's */
FLECS_API
void _ecs_vector_zero(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_zero(vector, T) \
    _ecs_vector_zero(vector, ECS_VECTOR_T(T))

/** Free vector */
FLECS_API
void ecs_vector_free(
    ecs_vector_t *vector);

/** Clear values in vector */
FLECS_API
void ecs_vector_clear(
    ecs_vector_t *vector);

/** Assert when the provided size does not match the vector type. */
FLECS_API
void ecs_vector_assert_size(
    ecs_vector_t* vector_inout,
    ecs_size_t elem_size);

/** Add element to vector. */
FLECS_API
void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_add(vector, T) \
    ((T*)_ecs_vector_add(vector, ECS_VECTOR_T(T)))

#define ecs_vector_add_t(vector, size, alignment) \
    _ecs_vector_add(vector, ECS_VECTOR_U(size, alignment))

/** Insert element to vector. */
FLECS_API
void* _ecs_vector_insert_at(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_insert_at(vector, T, index) \
    ((T*)_ecs_vector_insert_at(vector, ECS_VECTOR_T(T), index))

#define ecs_vector_insert_at_t(vector, size, alignment, index) \
    _ecs_vector_insert_at(vector, ECS_VECTOR_U(size, alignment), index)

/** Add n elements to the vector. */
FLECS_API
void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_addn(vector, T, elem_count) \
    ((T*)_ecs_vector_addn(vector, ECS_VECTOR_T(T), elem_count))

#define ecs_vector_addn_t(vector, size, alignment, elem_count) \
    _ecs_vector_addn(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Get element from vector. */
FLECS_API
void* _ecs_vector_get(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_get(vector, T, index) \
    ((T*)_ecs_vector_get(vector, ECS_VECTOR_T(T), index))

#define ecs_vector_get_t(vector, size, alignment, index) \
    _ecs_vector_get(vector, ECS_VECTOR_U(size, alignment), index)

/** Get last element from vector. */
FLECS_API
void* _ecs_vector_last(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_last(vector, T) \
    (T*)_ecs_vector_last(vector, ECS_VECTOR_T(T))

#define ecs_vector_last_t(vector, size, alignment) \
    _ecs_vector_last(vector, ECS_VECTOR_U(size, alignment))

/** Set minimum size for vector. If the current size of the vector is larger, 
 * the function will have no side effects. */
FLECS_API
int32_t _ecs_vector_set_min_size(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_min_size(vector, T, size) \
    _ecs_vector_set_min_size(vector, ECS_VECTOR_T(T), size)

/** Set minimum count for vector. If the current count of the vector is larger, 
 * the function will have no side effects. */
FLECS_API
int32_t _ecs_vector_set_min_count(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_min_count(vector, T, elem_count) \
    _ecs_vector_set_min_count(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_min_count_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_min_count(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Remove last element. This operation requires no swapping of values. */
FLECS_API
void ecs_vector_remove_last(
    ecs_vector_t *vector);

/** Remove last value, store last element in provided value. */
FLECS_API
bool _ecs_vector_pop(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *value);

#define ecs_vector_pop(vector, T, value) \
    _ecs_vector_pop(vector, ECS_VECTOR_T(T), value)

/** Append element at specified index to another vector. */
FLECS_API
int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_move_index(dst, src, T, index) \
    _ecs_vector_move_index(dst, src, ECS_VECTOR_T(T), index)

#define ecs_vector_move_index_t(dst, src, size, alignment, index) \
    _ecs_vector_move_index(dst, src, ECS_VECTOR_U(size, alignment), index)

/** Remove element at specified index. Moves the last value to the index. */
FLECS_API
int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index);

#define ecs_vector_remove(vector, T, index) \
    _ecs_vector_remove(vector, ECS_VECTOR_T(T), index)

#define ecs_vector_remove_t(vector, size, alignment, index) \
    _ecs_vector_remove(vector, ECS_VECTOR_U(size, alignment), index)

/** Shrink vector to make the size match the count. */
FLECS_API
void _ecs_vector_reclaim(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_reclaim(vector, T)\
    _ecs_vector_reclaim(vector, ECS_VECTOR_T(T))

#define ecs_vector_reclaim_t(vector, size, alignment)\
    _ecs_vector_reclaim(vector, ECS_VECTOR_U(size, alignment))

/** Grow size of vector with provided number of elements. */
FLECS_API
int32_t _ecs_vector_grow(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_grow(vector, T, size) \
    _ecs_vector_grow(vector, ECS_VECTOR_T(T), size)

/** Set allocation size of vector. */
FLECS_API
int32_t _ecs_vector_set_size(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_size(vector, T, elem_count) \
    _ecs_vector_set_size(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_size_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_size(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Set count of vector. If the size of the vector is smaller than the provided
 * count, the vector is resized. */
FLECS_API
int32_t _ecs_vector_set_count(
    ecs_vector_t **vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count);

#define ecs_vector_set_count(vector, T, elem_count) \
    _ecs_vector_set_count(vector, ECS_VECTOR_T(T), elem_count)

#define ecs_vector_set_count_t(vector, size, alignment, elem_count) \
    _ecs_vector_set_count(vector, ECS_VECTOR_U(size, alignment), elem_count)

/** Return number of elements in vector. */
FLECS_API
int32_t ecs_vector_count(
    const ecs_vector_t *vector);

/** Return size of vector. */
FLECS_API
int32_t ecs_vector_size(
    const ecs_vector_t *vector);

/** Return first element of vector. */
FLECS_API
void* _ecs_vector_first(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_first(vector, T) \
    ((T*)_ecs_vector_first(vector, ECS_VECTOR_T(T)))

#define ecs_vector_first_t(vector, size, alignment) \
    _ecs_vector_first(vector, ECS_VECTOR_U(size, alignment))

/** Sort elements in vector. */
FLECS_API
void _ecs_vector_sort(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    ecs_comparator_t compare_action);

#define ecs_vector_sort(vector, T, compare_action) \
    _ecs_vector_sort(vector, ECS_VECTOR_T(T), compare_action)

/** Copy vectors */
FLECS_API
ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset);

#define ecs_vector_copy(src, T) \
    _ecs_vector_copy(src, ECS_VECTOR_T(T))

#define ecs_vector_copy_t(src, size, alignment) \
    _ecs_vector_copy(src, ECS_VECTOR_U(size, alignment))

#ifndef FLECS_LEGACY
#define ecs_vector_each(vector, T, var, ...)\
    {\
        int var##_i, var##_count = ecs_vector_count(vector);\
        T* var##_array = ecs_vector_first(vector, T);\
        for (var##_i = 0; var##_i < var##_count; var##_i ++) {\
            T* var = &var##_array[var##_i];\
            __VA_ARGS__\
        }\
    }
#endif
#ifdef __cplusplus
}
#endif


/** C++ wrapper for vector class. */
#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {

template <typename T>
class vector_iterator
{
public:
    explicit vector_iterator(T* value, int index) {
        m_value = value;
        m_index = index;
    }

    bool operator!=(vector_iterator const& other) const
    {
        return m_index != other.m_index;
    }

    T const& operator*() const
    {
        return m_value[m_index];
    }

    vector_iterator& operator++()
    {
        ++m_index;
        return *this;
    }

private:
    T* m_value;
    int m_index;
};

/* C++ class mainly used as wrapper around internal ecs_vector_t. Do not use
 * this class as a replacement for STL datastructures! */
template <typename T>
class vector {
public:
    explicit vector(ecs_vector_t *v) : m_vector( v ) { }

    vector(size_t count = 0) : m_vector( nullptr ) { 
        if (count) {
            init(count);
        }
    }

    T& operator[](size_t index) {
        return *static_cast<T*>(_ecs_vector_get(m_vector, ECS_VECTOR_T(T), index));
    }

    vector_iterator<T> begin() {
        return vector_iterator<T>(
            static_cast<T*>(_ecs_vector_first(m_vector, ECS_VECTOR_T(T))), 0);
    }

    vector_iterator<T> end() {
        return vector_iterator<T>(
            static_cast<T*>(_ecs_vector_last(m_vector, ECS_VECTOR_T(T))),
                ecs_vector_count(m_vector));
    }    

    void clear() {
        ecs_vector_clear(m_vector);
    }

    void destruct() {
        ecs_vector_free(m_vector);
    }

    void add(T& value) {
        T* elem = static_cast<T*>(_ecs_vector_add(&m_vector, ECS_VECTOR_T(T)));
        *elem = value;
    }

    void add(T&& value) {
        T* elem = static_cast<T*>(_ecs_vector_add(&m_vector, ECS_VECTOR_T(T)));
        *elem = value;
    }

    T& get(int32_t index) {
        return *static_cast<T*>(_ecs_vector_get(m_vector, ECS_VECTOR_T(T), index));
    }

    T& first() {
        return *static_cast<T*>(_ecs_vector_first(m_vector, ECS_VECTOR_T(T)));
    }

    T& last() {
        return *static_cast<T*>(_ecs_vector_last(m_vector, ECS_VECTOR_T(T)));
    }

    int32_t count() {
        return ecs_vector_count(m_vector);
    }

    int32_t size() {
        return ecs_vector_size(m_vector);
    }

    ecs_vector_t *ptr() {
        return m_vector;
    }

    void ptr(ecs_vector_t *ptr) {
        m_vector = ptr;
    }

private:
    void init(size_t count) {
        m_vector = ecs_vector_new(T, static_cast<ecs_size_t>(count));
    }

    ecs_vector_t *m_vector;
};

}

#endif
#endif

#endif
