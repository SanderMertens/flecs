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
        ecs_map_set(map, elems[i].hash, &elems[i].value);
        test_assert(ecs_map_get(map, char*, elems[i].hash) != NULL);
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
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_count_empty() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_set_overwrite() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);

    ecs_map_set(map, 1, &(char*){"foobar"});
    
    value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);
    test_str(value, "foobar");
    ecs_map_free(map);
}

void Map_set_rehash() {
    ecs_map_t *map = ecs_map_new(char*, 8);
    fill_map(map);

    test_int(ecs_map_bucket_count(map), 16);

    int i;
    for (i = 5; i < 16; i ++) {
        ecs_map_set(map, i, &(char*){"zzz"});
    }

    test_int(ecs_map_bucket_count(map), 32);
    test_str(ecs_map_get_ptr(map, char*, 1), "hello");
    test_str(ecs_map_get_ptr(map, char*, 2), "world");
    test_str(ecs_map_get_ptr(map, char*, 3), "foo");
    test_str(ecs_map_get_ptr(map, char*, 4), "bar");
    test_str(ecs_map_get_ptr(map, char*, 5), "zzz");
    test_str(ecs_map_get_ptr(map, char*, 6), "zzz");
    test_str(ecs_map_get_ptr(map, char*, 7), "zzz");

    for (i = 1; i < 8; i ++) {
        ecs_map_set(map, i + 1000000, &(char*){"yyy"});
    }    

    test_int(ecs_map_bucket_count(map), 32);
    test_str(ecs_map_get_ptr(map, char*, 1 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 2 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 3 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 4 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 5 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 6 + 1000000), "yyy");
    test_str(ecs_map_get_ptr(map, char*, 7 + 1000000), "yyy");

    ecs_map_free(map);
}

void Map_set_size() {
    ecs_map_t *map = ecs_map_new(char*, 8);
    fill_map(map);

    test_int(ecs_map_bucket_count(map), 16);
    test_int(ecs_map_count(map), 4);
    
    ecs_map_set_size(map, 16);
    test_int(ecs_map_count(map), 4);
    test_int(ecs_map_bucket_count(map), 32);

    ecs_map_free(map);
}

void Map_set_zero_buckets() {
    ecs_map_t *map = ecs_map_new(char*, 0);
    ecs_map_set(map, 1, &(char*){"hello"});
    test_int(ecs_map_count(map), 1);
    ecs_map_free(map);
}

void Map_get() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value != NULL);
    test_str(value, "hello");
    ecs_map_free(map);
}

void Map_get_all() {
    ecs_map_t *map = ecs_map_new(char*, 16);
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
    ecs_map_t *map = ecs_map_new(char*, 16);
    char *value = ecs_map_get_ptr(map, char*, 1);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_get_unknown() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    char *value = ecs_map_get_ptr(map, char*, 5);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_iter() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    
    int32_t str_world_count = 0;
    int32_t str_bar_count = 0;
    int32_t str_hello_count = 0;
    int32_t str_foo_count = 0;

    ecs_map_iter_t it = ecs_map_iter(map);
    int32_t index;
    for(index = 0; index < 4; ++index) {
        void *value = ecs_map_next_ptr(&it, char*, NULL);
        test_assert(value != NULL);
        if(strcmp(value, "world") == 0) {
            ++str_world_count;
        }
        else if(strcmp(value, "bar") == 0) {
            ++str_bar_count;
        }
        else if(strcmp(value, "hello") == 0) {
            ++str_hello_count;
        }
        else if(strcmp(value, "foo") == 0) {
            ++str_foo_count;
        }
    }
    test_int(str_world_count, 1);
    test_int(str_bar_count, 1);
    test_int(str_hello_count, 1);
    test_int(str_foo_count, 1);
    test_assert(ecs_map_next_ptr(&it, char*, NULL) == NULL);

    ecs_map_free(map);
}

void Map_iter_empty() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_next(&it, char*, NULL));
    ecs_map_free(map);
}

void Map_iter_zero_buckets() {
    ecs_map_t *map = ecs_map_new(char*, 0);
    ecs_map_iter_t it = ecs_map_iter(map);
    test_assert(!ecs_map_next(&it, char*, NULL));
    ecs_map_free(map);
}

void Map_iter_null() {
    ecs_map_iter_t it = ecs_map_iter(NULL);
    test_assert(!ecs_map_next(&it, char*, NULL));
}


void Map_remove() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    ecs_map_remove(map, 3);
    test_assert(ecs_map_get(map, char*, 3) == NULL);
    test_int(ecs_map_count(map), 3);
    ecs_map_free(map);
}

void Map_remove_empty() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    ecs_map_remove(map, 3);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_empty_no_buckets() {
    ecs_map_t *map = ecs_map_new(char*, 0);
    ecs_map_remove(map, 3);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_unknown() {
    ecs_map_t *map = ecs_map_new(char*, 16);
    fill_map(map);
    ecs_map_remove(map, 5);
    test_str(ecs_map_get_ptr(map, char*, 1), "hello");
    test_str(ecs_map_get_ptr(map, char*, 2), "world");
    test_str(ecs_map_get_ptr(map, char*, 3), "foo");
    test_str(ecs_map_get_ptr(map, char*, 4), "bar");
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_remove_1_from_n_in_bucket() {
    ecs_map_t *map = ecs_map_new(char*, 8);

    ecs_map_set(map, 0, &(char*){"hello"});
    ecs_map_set(map, 16, &(char*){"world"});
    ecs_map_set(map, 32, &(char*){"foo"});
    ecs_map_remove(map, 16);
    test_int(ecs_map_count(map), 2);
    test_str(ecs_map_get_ptr(map, char*, 0), "hello");
    test_str(ecs_map_get_ptr(map, char*, 32), "foo");
    ecs_map_free(map);
}

void Map_remove_from_empty_bucket() {
    ecs_map_t *map = ecs_map_new(char*, 8);

    ecs_map_set(map, 0, &(char*){"hello"});
    ecs_map_remove(map, 0);
    ecs_map_remove(map, 0);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_grow() {
    ecs_map_t *map = ecs_map_new(char*, 1);
    
    ecs_map_grow(map, 10);

    malloc_count = 0;

    const char *v = "foo";
    int i;
    for(i = 0; i < 10; i ++) {
        ecs_map_set(map, i, &v);
    }

    test_int(malloc_count, 2);

    ecs_map_free(map);
}

void Map_set_size_0() {
    ecs_map_t *map = ecs_map_new(char*, 1);
    
    ecs_map_set_size(map, 0);

    test_int( ecs_map_count(map), 0);

    ecs_map_free(map);
}

void Map_ensure() {
    ecs_map_t *map = ecs_map_new(char*, 1);

    char **ptr = ecs_map_ensure(map, char*, 2);
    test_assert(ptr != NULL);

    test_int( ecs_map_count(map), 1);

    ecs_map_free(map);
}
