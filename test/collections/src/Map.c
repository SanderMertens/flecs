#include <collections.h>
#include <stdlib.h>

static
uint64_t* generate_keys(
    int count)
{
    uint64_t *keys = ecs_os_malloc_n(uint64_t, count);
    for (int i = 0; i < count; i ++) {
        keys[i] = i + 1;
    }
    return keys;
}

static
uint64_t* generate_random_keys(
    int count)
{
    uint64_t *keys = ecs_os_malloc_n(uint64_t, count);
    for (int i = 0; i < count; i ++) {
        uint64_t key;
        int j = 0;
        do {
            key = rand() % RAND_MAX;
            if (rand() % 2) {
                key = ecs_pair(key, rand() % RAND_MAX);
            }

            for (j = 0; j < i; j ++) {
                if (keys[j] == key) {
                    break;
                }
            }
        } while (j != i); /* no duplicate keys */
        keys[i] = key;
    }

    return keys;
}

static
void insert_map(
    ecs_map_t *map,
    uint64_t *keys,
    int count)
{
    for (int i = 0; i < count; i ++) {
        for (int j = 0; j < i; j ++) {
            uint64_t *v = ecs_map_get(map, keys[j]);
            test_assert(v != NULL);
            test_assert(v[0] == keys[j]);
        }

        test_assert(ecs_map_count(map) == i);
        test_assert(ecs_map_get(map, keys[i]) == NULL);
        ecs_map_insert(map, keys[i], keys[i]);
        test_assert(ecs_map_get(map, keys[i])[0] == keys[i]);
        test_assert(ecs_map_count(map) == (i + 1));
    }
}

static
ecs_map_t populate_map(
    uint64_t *keys,
    int count)
{
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    insert_map(&map, keys, count);
    return map;
}

static
void free_map(
    ecs_map_t *map,
    uint64_t *keys,
    int count)
{
    for (int i = 0; i < count; i ++) {
        for (int j = i; j < count; j ++) {
            uint64_t *v = ecs_map_get(map, keys[j]);
            test_assert(v != NULL);
            test_assert(v[0] == keys[j]);
        }

        test_assert(ecs_map_count(map) == (count - i));
        test_assert(ecs_map_get(map, keys[i]) != NULL);
        ecs_map_remove(map, keys[i]);
        test_assert(ecs_map_count(map) == ((count - i) - 1));
        test_assert(ecs_map_get(map, keys[i]) == NULL);
    }
}

static int32_t malloc_count;

static
void *test_malloc(ecs_size_t size) {
    malloc_count ++;
    return malloc(size);
}

static
void *test_calloc(ecs_size_t size) {
    malloc_count ++;
    return calloc(1, size);
}

static
void *test_realloc(void *old_ptr, ecs_size_t size) {
    malloc_count ++;
    return realloc(old_ptr, size);
}


void Map_setup() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc_ = test_malloc;
    os_api.calloc_ = test_calloc;
    os_api.realloc_ = test_realloc;
    ecs_os_set_api(&os_api);    
}

void Map_count() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_int(ecs_map_count(&map), 4);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_count_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    test_int(ecs_map_count(&map), 0);
    ecs_map_fini(&map);
}

void Map_set_overwrite() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    uint64_t *val = ecs_map_get(&map, 1);
    test_int(*val, 1);

    uint64_t *el = ecs_map_ensure(&map, 1);
    test_assert(val == el);
    test_int(ecs_map_count(&map), 4);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_set_rehash() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_int(map.bucket_count, 4);

    int i;
    for (i = 5; i < 16; i ++) {
        uint64_t *ptr = ecs_map_ensure(&map, i);
        *ptr = i;
    }

    test_int(map.bucket_count, 32);
    for (i = 1; i < 16; i ++) {
        test_int(ecs_map_get(&map, i)[0], i);
    }

    for (i = 1; i < 8; i ++) {
        uint64_t *ptr = ecs_map_ensure(&map, i + 1000000);
        *ptr = i + 1000000;
    }    

    test_int(map.bucket_count, 32);
    for (i = 1; i < 16; i ++) {
        test_int(ecs_map_get(&map, i)[0], i);
    }
    for (i = 1; i < 8; i ++) {
        test_int(ecs_map_get(&map, i + 1000000)[0], i + 1000000);
    }

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_get() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    uint64_t *value = ecs_map_get(&map, 1);
    test_assert(value != NULL);
    test_int(value[0], 1);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_get_all() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    uint64_t *value = ecs_map_get(&map, 1);
    test_assert(value != NULL);
    test_int(*value, 1);

    value = ecs_map_get(&map, 2);
    test_assert(value != NULL);
    test_int(*value, 2);

    value = ecs_map_get(&map, 3);
    test_assert(value != NULL);
    test_int(*value, 3);

    value = ecs_map_get(&map, 4);
    test_assert(value != NULL);
    test_int(*value, 4);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_get_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    uint64_t *value = ecs_map_get(&map, 1);
    test_assert(value == NULL);
    ecs_map_fini(&map);
}

void Map_get_unknown() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    uint64_t *value = ecs_map_get(&map, 5);
    test_assert(value == NULL);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_get_0_from_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    uint64_t *value = ecs_map_get(&map, 0);
    test_assert(value == NULL);
    ecs_map_fini(&map);
}

void Map_get_0_from_populated() {
    uint64_t *keys = generate_keys(32);
    ecs_map_t map = populate_map(keys, 32);

    uint64_t *value = ecs_map_get(&map, 0);
    test_assert(value == NULL);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_get_0_after_insert() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_insert(&map, 0, 10);
    uint64_t *value = ecs_map_get(&map, 0);
    test_assert(value != NULL);
    test_int(value[0], 10);
    ecs_map_fini(&map);
}

void Map_get_0_after_ensure() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_ensure(&map, 0)[0] = 10;
    uint64_t *value = ecs_map_get(&map, 0);
    test_assert(value != NULL);
    test_int(value[0], 10);
    ecs_map_fini(&map);
}

void Map_iter() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);
    
    int i = 0;
    ecs_map_iter_t it = ecs_map_iter(&map);
    while (ecs_map_next(&it)) {
        ecs_map_key_t key = ecs_map_key(&it);
        uint64_t val = ecs_map_value(&it);
        test_assert(key == val);
        i ++;
    }

    test_int(i, 4);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_iter_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_iter_t it = ecs_map_iter(&map);
    test_assert(!ecs_map_next(&it));
    ecs_map_fini(&map);
}

void Map_iter_null() {
    ecs_map_iter_t it = ecs_map_iter(NULL);
    test_assert(!ecs_map_next(&it));
}

void Map_remove() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_assert(ecs_map_get(&map, 3) != NULL);
    test_int(ecs_map_count(&map), 4);
    ecs_map_remove(&map, 3);
    test_assert(ecs_map_get(&map, 3) == NULL);
    test_int(ecs_map_count(&map), 3);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_remove_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_remove(&map, 3);
    test_int(ecs_map_count(&map), 0);
    ecs_map_fini(&map);
}

void Map_remove_unknown() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_assert(ecs_map_get(&map, 5) == NULL);
    test_int(ecs_map_count(&map), 4);
    ecs_map_remove(&map, 5);
    test_int(ecs_map_get(&map, 1)[0], 1);
    test_int(ecs_map_get(&map, 2)[0], 2);
    test_int(ecs_map_get(&map, 3)[0], 3);
    test_int(ecs_map_get(&map, 4)[0], 4);
    test_int(ecs_map_count(&map), 4);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_remove_twice() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_assert(ecs_map_get(&map, 3) != NULL);
    test_int(ecs_map_count(&map), 4);
    ecs_map_remove(&map, 3);
    test_assert(ecs_map_get(&map, 3) == NULL);
    test_int(ecs_map_count(&map), 3);
    ecs_map_remove(&map, 3);
    test_assert(ecs_map_get(&map, 3) == NULL);
    test_int(ecs_map_count(&map), 3);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_clear_empty() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);
    ecs_map_fini(&map);
}

void Map_clear_populated() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_int(ecs_map_count(&map), 4);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_clear_empty_twice() {
    ecs_map_t map;
    ecs_map_init(&map, NULL);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);
    ecs_map_fini(&map);
}

void Map_clear_populated_twice() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_int(ecs_map_count(&map), 4);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_populate_after_clear() {
    uint64_t *keys = generate_keys(4);
    ecs_map_t map = populate_map(keys, 4);

    test_int(ecs_map_count(&map), 4);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);
    
    insert_map(&map, keys, 4);
    test_int(ecs_map_count(&map), 4);
    ecs_map_clear(&map);
    test_int(ecs_map_count(&map), 0);

    ecs_os_free(keys);
    ecs_map_fini(&map);
}

void Map_randomized_insert() {
    uint64_t *keys = generate_random_keys(100);

    for (int count = 0; count < 100; count ++) {
        ecs_map_t map = populate_map(keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_remove() {
    uint64_t *keys = generate_random_keys(100);

    for (int count = 0; count < 100; count ++) {
        ecs_map_t map = populate_map(keys, count);
        test_int(ecs_map_count(&map), count);
        free_map(&map, keys, count);
        test_int(ecs_map_count(&map), 0);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_insert_large() {
    uint64_t *keys = generate_random_keys(1000);

    for (int count = 0; count < 1000; count += 27) {
        ecs_map_t map = populate_map(keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_remove_large() {
    uint64_t *keys = generate_random_keys(1000);

    for (int count = 0; count < 1000; count += 50) {
        ecs_map_t map = populate_map(keys, count);
        test_int(ecs_map_count(&map), count);
        free_map(&map, keys, count);
        test_int(ecs_map_count(&map), 0);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_after_clear() {
    uint64_t *keys = generate_random_keys(1000);

    for (int count = 0; count < 1000; count += 50) {
        ecs_map_t map = populate_map(keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_clear(&map);
        test_int(ecs_map_count(&map), 0);
        insert_map(&map, keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}
