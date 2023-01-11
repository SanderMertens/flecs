#include <collections.h>
#include <stdlib.h>

struct elem { int hash; char *value; } elems[] = {
    {1, "hello"},
    {2, "world"},
    {3, "foo"},
    {4, "bar"}
};

static
void fill_map(
    ecs_map_t *map)
{
    int i, count = sizeof(elems) / sizeof(struct elem);

    for (i = 0; i < count; i ++) {
        char ** ptr = ecs_map_ensure(map, char*, elems[i].hash);
        *ptr = elems[i].value;
        test_assert(ecs_map_get(map, char*, elems[i].hash) != NULL);
    }
}

static
uint64_t* generate_keys(
    int count)
{
    uint64_t *keys = ecs_os_malloc_n(uint64_t, count);
    for (int i = 0; i < count; i ++) {
        uint64_t key;
        int j = 0;
        do {
            key = rand() % RAND_MAX;
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
void populate_random(
    ecs_map_t *map,
    uint64_t *keys,
    int count)
{
    ecs_map_init(map, uint64_t, NULL, 0);

    for (int i = 0; i < count; i ++) {
        for (int j = 0; j < i; j ++) {
            uint64_t *v = ecs_map_get(map, uint64_t, keys[j]);
            test_assert(v != NULL);
            test_assert(v[0] == keys[j]);
        }

        test_assert(ecs_map_count(map) == i);
        test_assert(ecs_map_get(map, uint64_t, keys[i]) == NULL);
        uint64_t *el = ecs_map_insert(map, uint64_t, keys[i]);
        test_assert(ecs_map_get(map, uint64_t, keys[i]) == el);
        test_assert(ecs_map_count(map) == (i + 1));
        *el = keys[i];
    }
}

static
void free_random(
    ecs_map_t *map,
    uint64_t *keys,
    int count)
{
    for (int i = 0; i < count; i ++) {
        for (int j = i; j < count; j ++) {
            uint64_t *v = ecs_map_get(map, uint64_t, keys[j]);
            test_assert(v != NULL);
            test_assert(v[0] == keys[j]);
        }

        test_assert(ecs_map_count(map) == (count - i));
        test_assert(ecs_map_get(map, uint64_t, keys[i]) != NULL);
        ecs_map_remove(map, keys[i]);
        test_assert(ecs_map_count(map) == ((count - i) - 1));
        test_assert(ecs_map_get(map, uint64_t, keys[i]) == NULL);
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
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_count_empty() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_set_overwrite() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);

    char **ptr = ecs_map_ensure(map, char*, 1);
    *ptr = (char*)"foobar";
    
    value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);
    test_str(value, "foobar");
    ecs_map_free(map);
}

void Map_set_rehash() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 8);
    fill_map(map);

    test_int(map->bucket_count, 8);

    int i;
    for (i = 5; i < 16; i ++) {
        char **ptr = ecs_map_ensure(map, char*, i);
        *ptr = (char*)"zzz";
    }

    test_int(map->bucket_count, 32);
    test_str(ecs_map_get_ptr(map, char*, 1), "hello");
    test_str(ecs_map_get_ptr(map, char*, 2), "world");
    test_str(ecs_map_get_ptr(map, char*, 3), "foo");
    test_str(ecs_map_get_ptr(map, char*, 4), "bar");
    test_str(ecs_map_get_ptr(map, char*, 5), "zzz");
    test_str(ecs_map_get_ptr(map, char*, 6), "zzz");
    test_str(ecs_map_get_ptr(map, char*, 7), "zzz");

    for (i = 1; i < 8; i ++) {
        char **ptr = ecs_map_ensure(map, char*, i + 1000000);
        *ptr = (char*)"yyy";
    }    

    test_int(map->bucket_count, 32);
    test_str(ecs_map_get_ptr(map, char*, 1 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 2 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 3 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 4 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 5 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 6 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 7 + 1000000), "yyy");

    ecs_map_free(map);
}

void Map_set_zero_buckets() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 0);
    char **ptr = ecs_map_ensure(map, char*, 1);
    *ptr = (char*)"hello";
    test_int(ecs_map_count(map), 1);
    ecs_map_free(map);
}

void Map_get() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);
    test_str(value, "hello");
    ecs_map_free(map);
}

void Map_get_all() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);

    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);
    test_str(value, "hello");

    value = ecs_map_get_ptr(map, char*, 2);
    test_assert(value != NULL);
    test_str(value, "world");

    value = ecs_map_get_ptr(map, char*, 3);
    test_assert(value != NULL);
    test_str(value, "foo");

    value = ecs_map_get_ptr(map, char*, 4);
    test_assert(value != NULL);
    test_str(value, "bar");

    ecs_map_free(map);
}

void Map_get_empty() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_get_unknown() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 5);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_iter() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    
    int i = 0;
    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_map_key_t key;
    char** el;
    while ((el = ecs_map_next(&it, char*, &key))) {
        switch(i) {
        case 0:
            test_assert(el[0] != NULL);
            test_str(el[0], "world");
            test_int(key, 2);
            break;
        case 1:
            test_assert(el[0] != NULL);
            test_str(el[0], "bar");
            test_int(key, 4);
            break;
        case 2:
            test_assert(el[0] != NULL);
            test_str(el[0], "hello");
            test_int(key, 1);
            break;
        case 3:
            test_assert(el[0] != NULL);
            test_str(el[0], "foo");
            test_int(key, 3);
            break;
        default:
            test_assert(false);
        }
        i ++;
    }

    ecs_map_free(map);
}

void Map_iter_empty() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_next(&it, char*, NULL));
    ecs_map_free(map);
}

void Map_iter_zero_buckets() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 0);
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_next(&it, char*, NULL));
    ecs_map_free(map);
}

void Map_iter_null() {
    ecs_map_iter_t it = ecs_map_iter(NULL);
    test_assert(!ecs_map_next(&it, char*, NULL));
}

void Map_remove() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    ecs_map_remove(map, 3);
    test_assert(ecs_map_get(map, char*, 3) == NULL);
    test_int(ecs_map_count(map), 3);
    ecs_map_free(map);
}

void Map_remove_empty() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    ecs_map_remove(map, 3);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_empty_no_buckets() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 0);
    ecs_map_remove(map, 3);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_unknown() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    ecs_map_remove(map, 5);
    test_str(ecs_map_get_ptr(map, char*, 1), "hello");
    test_str(ecs_map_get_ptr(map, char*, 2), "world");
    test_str(ecs_map_get_ptr(map, char*, 3), "foo");
    test_str(ecs_map_get_ptr(map, char*, 4), "bar");
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_remove_twice() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 16);
    fill_map(map);
    test_int(ecs_map_count(map), 4);
    ecs_map_remove(map, 3);
    test_assert(ecs_map_get(map, char*, 3) == NULL);
    test_int(ecs_map_count(map), 3);
    ecs_map_remove(map, 3);
    test_assert(ecs_map_get(map, char*, 3) == NULL);
    test_int(ecs_map_count(map), 3);
    ecs_map_free(map);
}

void Map_remove_1_from_n_in_bucket() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 8);

    char **el;
    el = ecs_map_ensure(map, char*, 0);
    *el = (char*)"hello";
    el = ecs_map_ensure(map, char*, 16);
    *el = (char*)"world";
    el = ecs_map_ensure(map, char*, 32);
    *el = (char*)"foo";

    ecs_map_remove(map, 16);
    test_int(ecs_map_count(map), 2);
    test_str(ecs_map_get_ptr(map, char*, 0), "hello");
    test_str(ecs_map_get_ptr(map, char*, 32), "foo");
    ecs_map_free(map);
}

void Map_remove_from_empty_bucket() {
    ecs_map_t *map = ecs_map_new(char*, NULL, 8);

    char **ptr = ecs_map_ensure(map, char*, 0);
    *ptr = (char*)"hello";
    ecs_map_remove(map, 0);
    ecs_map_remove(map, 0);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_randomized_insert() {
    uint64_t *keys = generate_keys(100);

    for (int count = 0; count < 100; count ++) {
        ecs_map_t map = ECS_MAP_INIT(uint64_t);
        populate_random(&map, keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_remove() {
    uint64_t *keys = generate_keys(100);

    for (int count = 0; count < 100; count ++) {
        ecs_map_t map = ECS_MAP_INIT(uint64_t);
        populate_random(&map, keys, count);
        test_int(ecs_map_count(&map), count);
        free_random(&map, keys, count);
        test_int(ecs_map_count(&map), 0);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_insert_large() {
    uint64_t *keys = generate_keys(1000);

    for (int count = 0; count < 1000; count += 50) {
        ecs_map_t map = ECS_MAP_INIT(uint64_t);
        populate_random(&map, keys, count);
        test_int(ecs_map_count(&map), count);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}

void Map_randomized_remove_large() {
    uint64_t *keys = generate_keys(1000);

    for (int count = 0; count < 1000; count += 50) {
        ecs_map_t map = ECS_MAP_INIT(uint64_t);
        populate_random(&map, keys, count);
        test_int(ecs_map_count(&map), count);
        free_random(&map, keys, count);
        test_int(ecs_map_count(&map), 0);
        ecs_map_fini(&map);
    }

    ecs_os_free(keys);
}
