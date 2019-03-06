#include <include/collections.h>
#include "../../include/private/types.h"

struct elem { int hash; char *value; } elems[] = {
    {1, "hello"},
    {2, "world"},
    {3, "foo"},
    {4, "bar"}
};

static
void fill_map(
    EcsMap *map)
{
    int i, count = sizeof(elems) / sizeof(struct elem);

    for (i = 0; i < count; i ++) {
        ecs_map_set(map, elems[i].hash, elems[i].value);
    }
}

void Map_count() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void Map_count_empty() {
    EcsMap *map = ecs_map_new(16);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_set_overwrite() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    ecs_map_set(map, 1, "foobar");
    char *value = ecs_map_get(map, 1);
    test_str(value, "foobar");
    ecs_map_free(map);
}

void Map_set_rehash() {
    EcsMap *map = ecs_map_new(8);
    fill_map(map);

    test_int(ecs_map_bucket_count(map), 10);

    int i;
    for (i = 5; i < 10; i ++) {
        ecs_map_set(map, i, "zzz");
    }

    test_int(ecs_map_bucket_count(map), 20);
    test_str(ecs_map_get(map, 1), "hello");
    test_str(ecs_map_get(map, 2), "world");
    test_str(ecs_map_get(map, 3), "foo");
    test_str(ecs_map_get(map, 4), "bar");
    test_str(ecs_map_get(map, 5), "zzz");
    test_str(ecs_map_get(map, 6), "zzz");
    test_str(ecs_map_get(map, 7), "zzz");

    ecs_map_free(map);
}

void Map_set_zero_buckets() {
    EcsMap *map = ecs_map_new(0);
    ecs_map_set(map, 1, "hello");
    test_int(ecs_map_count(map), 1);
    ecs_map_free(map);
}

void Map_get() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    char *value = ecs_map_get(map, 1);
    test_str(value, "hello");
    ecs_map_free(map);
}

void Map_get_all() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);

    char *value = ecs_map_get(map, 1);
    test_str(value, "hello");

    value = ecs_map_get(map, 2);
    test_str(value, "world");

    value = ecs_map_get(map, 3);
    test_str(value, "foo");

    value = ecs_map_get(map, 4);
    test_str(value, "bar");

    ecs_map_free(map);
}

void Map_get_empty() {
    EcsMap *map = ecs_map_new(16);
    char *value = ecs_map_get(map, 1);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_get_unknown() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    char *value = ecs_map_get(map, 5);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void Map_iter() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);

    EcsIter it = ecs_map_iter(map);
    test_assert(ecs_iter_hasnext(&it) == true);
    test_str(ecs_iter_next(&it), "hello");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_str(ecs_iter_next(&it), "world");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_str(ecs_iter_next(&it), "foo");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_str(ecs_iter_next(&it), "bar");

    test_assert(ecs_iter_hasnext(&it) == false);
}

void Map_iter_empty() {
    EcsMap *map = ecs_map_new(16);
    EcsIter it = ecs_map_iter(map);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_map_free(map);
}

void Map_iter_zero_buckets() {
    EcsMap *map = ecs_map_new(0);
    EcsIter it = ecs_map_iter(map);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_map_free(map);
}

void Map_remove() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_assert(ecs_map_remove(map, 3) == EcsOk);
    test_assert(ecs_map_get(map, 3) == NULL);
    test_int(ecs_map_count(map), 3);
    ecs_map_free(map);
}

void Map_remove_empty() {
    EcsMap *map = ecs_map_new(16);
    test_assert(ecs_map_remove(map, 3) == EcsError);
    test_int(ecs_map_count(map), 0);
    ecs_map_free(map);
}

void Map_remove_unknown() {
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_assert(ecs_map_remove(map, 5) == EcsError);
    test_str(ecs_map_get(map, 1), "hello");
    test_str(ecs_map_get(map, 2), "world");
    test_str(ecs_map_get(map, 3), "foo");
    test_str(ecs_map_get(map, 4), "bar");
    test_int(ecs_map_count(map), 4);
    ecs_map_free(map);
}
