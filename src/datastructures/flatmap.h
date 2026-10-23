/**
 * @file datastructures/flatmap.h
 * @brief Open addressing hashmap with inline elements and 64-bit keys.
 */

#ifndef FLECS_FLATMAP_H
#define FLECS_FLATMAP_H

#define FLECS_FLATMAP_MIN_SIZE_LOG2 (3)

static inline uint32_t flecs_flatmap_mask(
    uint32_t shift)
{
    return (uint32_t)(UINT64_MAX >> shift);
}

static inline uint32_t flecs_flatmap_slot(
    uint64_t key,
    uint32_t shift)
{
    return (uint32_t)((key * 0x9E3779B97F4A7C15ull) >> shift);
}

#define flecs_flatmap_size(map)\
    ((map)->data ? (int32_t)(flecs_flatmap_mask((map)->shift) + 1) : 0)

#define FLECS_FLATMAP_DECLARE(map_t, T)\
    typedef struct map_t {\
        T *data;\
        int32_t count;\
        uint32_t shift;\
    } map_t

#define FLECS_FLATMAP_IMPL(name, map_t, T, key_field)\
    static inline T* name##_get(\
        const map_t *map,\
        uint64_t key)\
    {\
        T *data = map->data;\
        if (!data) {\
            return NULL;\
        }\
        \
        uint32_t shift = map->shift;\
        uint32_t slot = flecs_flatmap_slot(key, shift);\
        for (;;) {\
            T *elem = &data[slot];\
            if (elem->key_field == key) {\
                return elem;\
            }\
            \
            if (!elem->key_field) {\
                return NULL;\
            }\
            \
            slot = (slot + 1) & flecs_flatmap_mask(shift);\
        }\
    }\
    \
    static inline T* name##_find_slot(\
        T *data,\
        uint32_t shift,\
        uint64_t key)\
    {\
        uint32_t mask = flecs_flatmap_mask(shift);\
        uint32_t slot = flecs_flatmap_slot(key, shift);\
        while (data[slot].key_field && (data[slot].key_field != key)) {\
            slot = (slot + 1) & mask;\
        }\
        \
        return &data[slot];\
    }\
    \
    static inline void name##_rehash(\
        ecs_allocator_t *allocator,\
        map_t *map,\
        int32_t size_log2)\
    {\
        T *old = map->data;\
        int32_t i, old_size = flecs_flatmap_size(map);\
        uint32_t shift = flecs_ito(uint32_t, 64 - size_log2);\
        T *data = flecs_calloc_n(allocator, T, 1 << size_log2);\
        \
        for (i = 0; i < old_size; i ++) {\
            if (old[i].key_field) {\
                *name##_find_slot(data, shift, old[i].key_field) = old[i];\
            }\
        }\
        \
        if (old) {\
            flecs_free_n(allocator, T, old_size, old);\
        }\
        \
        map->data = data;\
        map->shift = shift;\
    }\
    \
    static inline T* name##_ensure(\
        ecs_allocator_t *allocator,\
        map_t *map,\
        uint64_t key)\
    {\
        ecs_assert(key != 0, ECS_INVALID_PARAMETER, NULL);\
        if (!map->data) {\
            name##_rehash(allocator, map, FLECS_FLATMAP_MIN_SIZE_LOG2);\
        } else if (((map->count + 1) * 2) > flecs_flatmap_size(map)) {\
            name##_rehash(allocator, map,\
                flecs_ito(int32_t, 64 - map->shift + 1));\
        }\
        \
        T *elem = name##_find_slot(map->data, map->shift, key);\
        if (!elem->key_field) {\
            elem->key_field = key;\
            map->count ++;\
        }\
        \
        return elem;\
    }\
    \
    static inline void name##_remove_elem(\
        map_t *map,\
        T *elem)\
    {\
        T *data = map->data;\
        uint32_t shift = map->shift;\
        uint32_t mask = flecs_flatmap_mask(shift);\
        uint32_t hole = flecs_ito(uint32_t, elem - data);\
        uint32_t cur = hole;\
        \
        for (;;) {\
            cur = (cur + 1) & mask;\
            uint64_t key = data[cur].key_field;\
            if (!key) {\
                break;\
            }\
            \
            uint32_t home = flecs_flatmap_slot(key, shift);\
            if (((cur - home) & mask) >= ((cur - hole) & mask)) {\
                data[hole] = data[cur];\
                hole = cur;\
            }\
        }\
        \
        ecs_os_zeromem(&data[hole]);\
        map->count --;\
    }\
    \
    static inline void name##_fini(\
        ecs_allocator_t *allocator,\
        map_t *map)\
    {\
        if (map->data) {\
            flecs_free_n(allocator, T, flecs_flatmap_size(map), map->data);\
        }\
        \
        ecs_os_zeromem(map);\
    }

#endif
