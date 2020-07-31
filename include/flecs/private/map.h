#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_map_t ecs_map_t;
typedef struct ecs_bucket_t ecs_bucket_t;
typedef uint64_t ecs_map_key_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    int32_t bucket_index;
    int32_t element_index;
    void *payload;
} ecs_map_iter_t;

FLECS_EXPORT
ecs_map_t * _ecs_map_new(
    ecs_size_t elem_size,
    ecs_size_t alignment, 
    int32_t elem_count);

#define ecs_map_new(T, elem_count)\
    _ecs_map_new(sizeof(T), ECS_ALIGNOF(T), elem_count)

FLECS_EXPORT
void * _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_get(map, T, key)\
    (T*)_ecs_map_get(map, sizeof(T), (ecs_map_key_t)key)

FLECS_EXPORT
bool _ecs_map_has(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    void *payload);

#define ecs_map_has(map, key, payload)\
    _ecs_map_has(map, sizeof(*payload), (ecs_map_key_t)key, payload)

FLECS_EXPORT
void * _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key);

#define ecs_map_get_ptr(map, T, key)\
    (T)_ecs_map_get_ptr(map, key)

FLECS_EXPORT
void _ecs_map_set(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload);

#define ecs_map_set(map, key, payload)\
    _ecs_map_set(map, sizeof(*payload), (ecs_map_key_t)key, payload);

FLECS_EXPORT
void ecs_map_free(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

FLECS_EXPORT
void ecs_map_clear(
    ecs_map_t *map);

FLECS_EXPORT
int32_t ecs_map_count(
    const ecs_map_t *map);

FLECS_EXPORT
int32_t ecs_map_bucket_count(
    const ecs_map_t *map);

FLECS_EXPORT
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

FLECS_EXPORT
void* _ecs_map_next(
    ecs_map_iter_t* iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key);

#define ecs_map_next(iter, T, key) \
    (T*)_ecs_map_next(iter, sizeof(T), key)

FLECS_EXPORT
void* _ecs_map_next_ptr(
    ecs_map_iter_t* iter,
    ecs_map_key_t *key);

#define ecs_map_next_ptr(iter, T, key) \
    (T)_ecs_map_next_ptr(iter, key)

FLECS_EXPORT
void ecs_map_grow(
    ecs_map_t *map, 
    int32_t elem_count);

FLECS_EXPORT
void ecs_map_set_size(
    ecs_map_t *map, 
    int32_t elem_count);

FLECS_EXPORT
void ecs_map_memory(
    ecs_map_t *map, 
    int32_t *allocd,
    int32_t *used);

FLECS_EXPORT
ecs_map_t* ecs_map_copy(
    const ecs_map_t *map);

#ifndef FLECS_LEGACY
#define ecs_map_each(map, T, key, var, ...)\
    {\
        ecs_map_iter_t it = ecs_map_iter(map);\
        ecs_map_key_t key;\
        T* var;\
        (void)key;\
        (void)var;\
        while ((var = ecs_map_next(&it, T, &key))) {\
            __VA_ARGS__\
        }\
    }
#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

#include <iostream>

namespace flecs {

template <typename K, typename T>
class map {
public:
    map(int32_t count = 0) { 
        init(count);
    }

    map(std::initializer_list<std::pair<K, T>> elems) {
        init(elems.size());
        *this = elems;
    }

    void operator=(std::initializer_list<std::pair<K, T>> elems) {
        for (auto elem : elems) {
            this->set(elem.first, elem.second);
        }
    }

    void clear() {
        ecs_map_clear(m_map);
    }

    int32_t count() {
        return ecs_map_count(m_map);
    }

    void set(K& key, T& value) {
        ecs_map_set(m_map, reinterpret_cast<ecs_map_key_t>(key), &value);
    }

    T& get(K& key) {
        *(T*)ecs_map_get(m_map, T, reinterpret_cast<ecs_map_key_t>(key));
    }

private:
    void init(int32_t count) {
        m_map = ecs_map_new(T, count);
    }

    ecs_map_t *m_map;
};

}

#endif
#endif

#endif
