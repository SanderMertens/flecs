/**
 * @file datastructures/map.h
 * @brief Map data structure.
 */

#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Data type for map key-value storage. */
typedef uint64_t ecs_map_data_t;

/** Map key type. */
typedef ecs_map_data_t ecs_map_key_t;

/** Map value type. */
typedef ecs_map_data_t ecs_map_val_t;

/** A single entry in a map bucket (linked list node). */
typedef struct ecs_bucket_entry_t {
    ecs_map_key_t key;                /**< Key of the entry. */
    ecs_map_val_t value;              /**< Value of the entry. */
    struct ecs_bucket_entry_t *next;  /**< Next entry in the bucket chain. */
} ecs_bucket_entry_t;

/** A bucket in the map hash table. */
typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;        /**< First entry in this bucket. */
} ecs_bucket_t;

/** A hashmap data structure. */
struct ecs_map_t {
    ecs_bucket_t *buckets;            /**< Array of hash buckets. */
    int32_t bucket_count;             /**< Total number of buckets. */
    unsigned count : 26;              /**< Number of elements in the map. */
    unsigned bucket_shift : 6;        /**< Bit shift for bucket index computation. */
    struct ecs_allocator_t *allocator; /**< Allocator used for memory management. */
#ifdef FLECS_DEBUG
    int32_t change_count;             /**< Track modifications while iterating. */
    ecs_map_key_t last_iterated;      /**< Currently iterated element. */
#endif
};

/** Iterator for traversing map contents. */
typedef struct ecs_map_iter_t {
    const ecs_map_t *map;             /**< The map being iterated. */
    ecs_bucket_t *bucket;             /**< Current bucket. */
    ecs_bucket_entry_t *entry;        /**< Current entry in the bucket. */
    ecs_map_data_t *res;              /**< Pointer to current key-value pair. */
#ifdef FLECS_DEBUG
    int32_t change_count;             /**< Change count at iterator creation for modification detection. */
#endif
} ecs_map_iter_t;

/** Function and macro postfix meanings:
 *   - _ptr:    Access ecs_map_val_t as void*.
 *   - _ref:    Access ecs_map_val_t* as T**.
 *   - _deref:  Dereference a _ref.
 *   - _alloc:  If _ptr is NULL, alloc.
 *   - _free:   If _ptr is not NULL, free.
 */

/** Initialize a new map.
 *
 * @param map The map to initialize.
 * @param allocator Allocator to use for memory management.
 */
FLECS_API
void ecs_map_init(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Initialize a new map if uninitialized, leave as is otherwise.
 *
 * @param map The map to initialize.
 * @param allocator Allocator to use for memory management.
 */
FLECS_API
void ecs_map_init_if(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Reclaim map memory.
 *
 * @param map The map to reclaim memory from.
 */
FLECS_API
void ecs_map_reclaim(
    ecs_map_t *map);

/** Deinitialize a map.
 *
 * @param map The map to deinitialize.
 */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Get an element for a key. Returns NULL if the key doesn't exist.
 *
 * @param map The map to search.
 * @param key The key to look up.
 * @return Pointer to the value, or NULL if the key was not found.
 */
FLECS_API
ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get element as pointer (auto-dereferences _ptr).
 *
 * @param map The map to search.
 * @param key The key to look up.
 * @return Dereferenced pointer value, or NULL if the key was not found.
 */
FLECS_API
void* ecs_map_get_deref_(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert an element for a key.
 *
 * @param map The map to get or insert into.
 * @param key The key to look up or insert.
 * @return Pointer to the existing or newly inserted value.
 */
FLECS_API
ecs_map_val_t* ecs_map_ensure(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert a pointer element for a key. Allocate if the pointer is NULL.
 *
 * @param map The map to get or insert into.
 * @param elem_size Size of the element to allocate.
 * @param key The key to look up or insert.
 * @return Pointer to the existing or newly allocated element.
 */
FLECS_API
void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Insert an element for a key.
 *
 * @param map The map to insert into.
 * @param key The key for the new element.
 * @param value The value to insert.
 */
FLECS_API
void ecs_map_insert(
    ecs_map_t *map,
    ecs_map_key_t key,
    ecs_map_val_t value);

/** Insert a pointer element for a key, populate with a new allocation.
 *
 * @param map The map to insert into.
 * @param elem_size Size of the element to allocate.
 * @param key The key for the new element.
 * @return Pointer to the newly allocated element.
 */
FLECS_API
void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Remove a key from the map.
 *
 * @param map The map to remove from.
 * @param key The key to remove.
 * @return The removed value.
 */
FLECS_API
ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove a pointer element. Free if not NULL.
 *
 * @param map The map to remove from.
 * @param key The key to remove and free.
 */
FLECS_API
void ecs_map_remove_free(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove all elements from the map.
 *
 * @param map The map to clear.
 */
FLECS_API
void ecs_map_clear(
    ecs_map_t *map);

/** Return the number of elements in the map. */
#define ecs_map_count(map) ((map) ? (map)->count : 0)

/** Is the map initialized? */
#define ecs_map_is_init(map) ((map) ? (map)->bucket_shift != 0 : false)

/** Return an iterator to map contents.
 *
 * @param map The map to iterate.
 * @return A new iterator positioned before the first element.
 */
FLECS_API
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

/** Return whether the map iterator is valid.
 *
 * @param iter The iterator to check.
 * @return True if the iterator is valid.
 */
FLECS_API
bool ecs_map_iter_valid(
    ecs_map_iter_t *iter);

/** Obtain the next element in the map from the iterator.
 *
 * @param iter The iterator to advance.
 * @return True if a next element was found, false if iteration is done.
 */
FLECS_API
bool ecs_map_next(
    ecs_map_iter_t *iter);

/** Copy a map.
 *
 * @param dst The destination map.
 * @param src The source map to copy from.
 */
FLECS_API
void ecs_map_copy(
    ecs_map_t *dst,
    const ecs_map_t *src);

/** Get value as a typed reference (T**). */
#define ecs_map_get_ref(m, T, k) ECS_CAST(T**, ecs_map_get(m, k))

/** Get value as a typed dereferenced pointer (T*). */
#define ecs_map_get_deref(m, T, k) ECS_CAST(T*, ecs_map_get_deref_(m, k))

/** Get value as a void pointer. */
#define ecs_map_get_ptr(m, k) ECS_CAST(void*, ecs_map_get_deref_(m, k))

/** Ensure a typed reference (T**) for the key. */
#define ecs_map_ensure_ref(m, T, k) ECS_CAST(T**, ecs_map_ensure(m, k))

/** Insert a pointer value into the map. */
#define ecs_map_insert_ptr(m, k, v) ecs_map_insert(m, k, ECS_CAST(ecs_map_val_t, ECS_PTR_CAST(uintptr_t, v)))

/** Type-safe insert with allocation. */
#define ecs_map_insert_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_insert_alloc(m, ECS_SIZEOF(T), k))

/** Type-safe ensure with allocation. */
#define ecs_map_ensure_alloc_t(m, T, k) ECS_PTR_CAST(T*, (uintptr_t)ecs_map_ensure_alloc(m, ECS_SIZEOF(T), k))

/** Remove a pointer element from the map. */
#define ecs_map_remove_ptr(m, k) (ECS_PTR_CAST(void*, ECS_CAST(uintptr_t, (ecs_map_remove(m, k)))))

/** Get the key from an iterator. */
#define ecs_map_key(it) ((it)->res[0])

/** Get the value from an iterator. */
#define ecs_map_value(it) ((it)->res[1])

/** Get the value from an iterator as a void pointer. */
#define ecs_map_ptr(it) ECS_PTR_CAST(void*, ECS_CAST(uintptr_t, ecs_map_value(it)))

/** Get the value from an iterator as a typed reference (T**). */
#define ecs_map_ref(it, T) (ECS_CAST(T**, &((it)->res[1])))

#ifdef __cplusplus
}
#endif

#endif
