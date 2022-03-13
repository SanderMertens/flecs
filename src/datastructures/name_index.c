#include "../private_api.h"

static
uint64_t name_index_hash(
    const void *ptr)
{
    const ecs_hashed_string_t *str = ptr;
    ecs_assert(str->hash != 0, ECS_INTERNAL_ERROR, NULL);
    return str->hash;
}

static
int name_index_compare(
    const void *ptr1, 
    const void *ptr2)
{
    const ecs_hashed_string_t *str1 = ptr1;
    const ecs_hashed_string_t *str2 = ptr2;
    ecs_size_t len1 = str1->length;
    ecs_size_t len2 = str2->length;
    if (len1 != len2) {
        return (len1 > len2) - (len1 < len2);
    }

    return ecs_os_memcmp(str1->value, str2->value, len1);
}

void flecs_name_index_init(
    ecs_hashmap_t *hm) 
{
    _flecs_hashmap_init(hm, 
        ECS_SIZEOF(ecs_hashed_string_t), ECS_SIZEOF(uint64_t), 
        name_index_hash, 
        name_index_compare);
}

ecs_hashmap_t* flecs_name_index_new(void) 
{
    ecs_hashmap_t *result = ecs_os_calloc_t(ecs_hashmap_t);
    flecs_name_index_init(result);
    return result;
}

void flecs_name_index_fini(
    ecs_hashmap_t *map)
{
    flecs_hashmap_fini(map);
}

void flecs_name_index_free(
    ecs_hashmap_t *map)
{
    if (map) {
        flecs_name_index_fini(map);
        ecs_os_free(map);
    }
}

ecs_hashed_string_t flecs_get_hashed_string(
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    if (!length) {
        length = ecs_os_strlen(name);
    } else {
        ecs_assert(length == ecs_os_strlen(name), ECS_INTERNAL_ERROR, NULL);
    }

    if (!hash) {
        hash = flecs_hash(name, length);
    } else {
        ecs_assert(hash == flecs_hash(name, length), ECS_INTERNAL_ERROR, NULL);
    }

    return (ecs_hashed_string_t) {
        .value = (char*)name,
        .length = length,
        .hash = hash
    };
}

const uint64_t* flecs_name_index_find_ptr(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_hashed_string_t hs = flecs_get_hashed_string(name, length, hash);

    ecs_hm_bucket_t *b = flecs_hashmap_get_bucket(map, hs.hash);
    if (!b) {
        return NULL;
    }

    ecs_hashed_string_t *keys = ecs_vector_first(b->keys, ecs_hashed_string_t);
    int32_t i, count = ecs_vector_count(b->keys);

    for (i = 0; i < count; i ++) {
        ecs_hashed_string_t *key = &keys[i];
        ecs_assert(key->hash == hs.hash, ECS_INTERNAL_ERROR, NULL);

        if (hs.length != key->length) {
            continue;
        }

        if (!ecs_os_strcmp(name, key->value)) {
            uint64_t *e = ecs_vector_get(b->values, uint64_t, i);
            ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
            return e;
        }
    }

    return NULL;
}

uint64_t flecs_name_index_find(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    const uint64_t *id = flecs_name_index_find_ptr(map, name, length, hash);
    if (id) {
        return id[0];
    }
    return 0;
}

void flecs_name_index_remove(
    ecs_hashmap_t *map,
    uint64_t e,
    uint64_t hash)
{
    ecs_hm_bucket_t *b = flecs_hashmap_get_bucket(map, hash);
    if (!b) {
        return;
    }

    uint64_t *ids = ecs_vector_first(b->values, uint64_t);
    int32_t i, count = ecs_vector_count(b->values);

    for (i = 0; i < count; i ++) {
        if (ids[i] == e) {
            flecs_hm_bucket_remove(map, b, hash, i);
            break;
        }
    }
}

void flecs_name_index_update_name(
    ecs_hashmap_t *map,
    uint64_t e,
    uint64_t hash,
    const char *name)
{
    ecs_hm_bucket_t *b = flecs_hashmap_get_bucket(map, hash);
    if (!b) {
        return;
    }

    uint64_t *ids = ecs_vector_first(b->values, uint64_t);
    int32_t i, count = ecs_vector_count(b->values);

    for (i = 0; i < count; i ++) {
        if (ids[i] == e) {
            ecs_hashed_string_t *key = ecs_vector_get(
                b->keys, ecs_hashed_string_t, i);
            key->value = (char*)name;
            ecs_assert(ecs_os_strlen(name) == key->length,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(flecs_hash(name, key->length) == key->hash,
                ECS_INTERNAL_ERROR, NULL);
            return;
        }
    }

    /* Record must already have been in the index */
    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

void flecs_name_index_ensure(
    ecs_hashmap_t *map,
    uint64_t id,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_check(name != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_hashed_string_t key = flecs_get_hashed_string(name, length, hash);
    
    uint64_t existing = flecs_name_index_find(
        map, name, key.length, key.hash);
    if (existing) {
        if (existing != id) {
            ecs_abort(ECS_ALREADY_DEFINED, 
                "conflicting id registered with name '%s'", name);
        }
    }

    flecs_hashmap_result_t hmr = flecs_hashmap_ensure(
        map, &key, uint64_t);

    *((uint64_t*)hmr.value) = id;
error:
    return;
}
