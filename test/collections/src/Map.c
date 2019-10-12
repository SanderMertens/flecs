#include <collections.h>

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
        ecs_map_set(map, elems[i].hash, &elems[i].value);
    }
}

static int32_t malloc_count;

static
void *test_malloc(size_t size) {
    malloc_count ++;
    return malloc(size);
}

static
void *test_calloc(size_t size, size_t n) {
    malloc_count ++;
    return calloc(size, n);
}

static
void *test_realloc(void *old_ptr, size_t size) {
    malloc_count ++;
    return realloc(old_ptr, size);
}


void Map_setup() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc = test_malloc;
    os_api.calloc = test_calloc;
    os_api.realloc = test_realloc;
    ecs_os_set_api(&os_api);    
}

void Map_count() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_count_empty() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_set_overwrite() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    ecs_map_set(map, 1, &(char*){"foobar"});
    char *value;
    test_bool(ecs_map_has(map, 1, &value), true);
    test_str(value, "foobar");
    ecs_map_free(map);
}

void Map_set_rehash() {
    ecs_map_t *map = ecs_map_new(8, sizeof(char*));
    fill_map(map);

    test_int(ecs_map_bucket_count(map), 10);

    int i;
    for (i = 5; i < 10; i ++) {
        ecs_map_set(map, i, &(char*){"zzz"});
    }

    test_int(ecs_map_bucket_count(map), 20);
    test_str(*(char**)ecs_map_get_ptr(map, 1), "hello");
    test_str(*(char**)ecs_map_get_ptr(map, 2), "world");
    test_str(*(char**)ecs_map_get_ptr(map, 3), "foo");
    test_str(*(char**)ecs_map_get_ptr(map, 4), "bar");
    test_str(*(char**)ecs_map_get_ptr(map, 5), "zzz");
    test_str(*(char**)ecs_map_get_ptr(map, 6), "zzz");
    test_str(*(char**)ecs_map_get_ptr(map, 7), "zzz");

    ecs_map_free(map);
}

void Map_set_zero_buckets() {
    ecs_map_t *map = ecs_map_new(0, sizeof(char*));
    ecs_map_set(map, 1, &(char*){"hello"});
    test_int(ecs_map_count(map), 1);
    ecs_map_free(map);
}

void Map_get() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    char *value;
    test_bool( ecs_map_has(map, 1, &value), true);
    test_str(value, "hello");
    ecs_map_free(map);
}

void Map_get_all() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);

    char *value;
    test_bool(ecs_map_has(map, 1, &value), true);
    test_str(value, "hello");

    test_bool(ecs_map_has(map, 2, &value), true);
    test_str(value, "world");

    test_bool(ecs_map_has(map, 3, &value), true);
    test_str(value, "foo");

    test_bool(ecs_map_has(map, 4, &value), true);
    test_str(value, "bar");

    ecs_map_free(map);
}

void Map_get_empty() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    char *value;
    test_bool(ecs_map_has(map, 1, &value), false);
    ecs_map_free(map);
}

void Map_get_unknown() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    char *value;
    test_bool(ecs_map_has(map, 5, &value), false);
    ecs_map_free(map);
}

void Map_iter() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);

    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(ecs_map_hasnext(&it) == true);
    test_str(*(char**)ecs_map_next(&it), "hello");

    test_assert(ecs_map_hasnext(&it) == true);
    test_str(*(char**)ecs_map_next(&it), "world");

    test_assert(ecs_map_hasnext(&it) == true);
    test_str(*(char**)ecs_map_next(&it), "foo");

    test_assert(ecs_map_hasnext(&it) == true);
    test_str(*(char**)ecs_map_next(&it), "bar");

    test_assert(ecs_map_hasnext(&it) == false);
}

void Map_iter_empty() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_hasnext(&it));
    ecs_map_free(map);
}

void Map_iter_zero_buckets() {
    ecs_map_t *map = ecs_map_new(0, sizeof(char*));
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_hasnext(&it));
    ecs_map_free(map);
}

void Map_remove() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    test_assert(ecs_map_remove(map, 3) == 0);
    test_bool(ecs_map_has(map, 3, (void**)NULL), false);
    test_int(ecs_map_count(map), 3);
    ecs_map_free(map);
}

void Map_remove_empty() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    test_assert(ecs_map_remove(map, 3) == -1);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_unknown() {
    ecs_map_t *map = ecs_map_new(16, sizeof(char*));
    fill_map(map);
    test_assert(ecs_map_remove(map, 5) == -1);
    test_str(*(char**)ecs_map_get_ptr(map, 1), "hello");
    test_str(*(char**)ecs_map_get_ptr(map, 2), "world");
    test_str(*(char**)ecs_map_get_ptr(map, 3), "foo");
    test_str(*(char**)ecs_map_get_ptr(map, 4), "bar");
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_grow() {
    ecs_map_t *map = ecs_map_new(0, sizeof(char*));
    
    ecs_map_grow(map, 10);

    malloc_count = 0;

    const char *v = "foo";
    int i;
    for(i = 0; i < 10; i ++) {
        ecs_map_set(map, i, &v);
    }

    test_int(malloc_count, 0);

    ecs_map_set(map, i, &v);

    test_int(malloc_count, 2);
}
