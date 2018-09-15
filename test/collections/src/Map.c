/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
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

void test_Map_tc_count(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_assertint(ecs_map_count(map), 4);
    ecs_map_free(map);
}


void test_Map_tc_count_empty(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    test_assertint(ecs_map_count(map), 0);
    ecs_map_free(map);
}


void test_Map_tc_get(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    char *value = ecs_map_get(map, 1);
    test_assertstr(value, "hello");
    ecs_map_free(map);
}


void test_Map_tc_get_empty(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    char *value = ecs_map_get(map, 1);
    test_assert(value == NULL);
    ecs_map_free(map);
}


void test_Map_tc_get_unknown(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    char *value = ecs_map_get(map, 5);
    test_assert(value == NULL);
    ecs_map_free(map);
}

void test_Map_tc_get_all(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);

    char *value = ecs_map_get(map, 1);
    test_assertstr(value, "hello");

    value = ecs_map_get(map, 2);
    test_assertstr(value, "world");

    value = ecs_map_get(map, 3);
    test_assertstr(value, "foo");

    value = ecs_map_get(map, 4);
    test_assertstr(value, "bar");

    ecs_map_free(map);
}

void test_Map_tc_remove(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_assert(ecs_map_remove(map, 3) == EcsOk);
    test_assert(ecs_map_get(map, 3) == NULL);
    test_assertint(ecs_map_count(map), 3);
    ecs_map_free(map);
}


void test_Map_tc_remove_empty(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    test_assert(ecs_map_remove(map, 3) == EcsError);
    test_assertint(ecs_map_count(map), 0);
    ecs_map_free(map);
}


void test_Map_tc_remove_unknown(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    test_assert(ecs_map_remove(map, 5) == EcsError);
    test_assertstr(ecs_map_get(map, 1), "hello");
    test_assertstr(ecs_map_get(map, 2), "world");
    test_assertstr(ecs_map_get(map, 3), "foo");
    test_assertstr(ecs_map_get(map, 4), "bar");
    test_assertint(ecs_map_count(map), 4);
    ecs_map_free(map);
}

void test_Map_tc_set_overwrite(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);
    ecs_map_set(map, 1, "foobar");
    char *value = ecs_map_get(map, 1);
    test_assertstr(value, "foobar");
    ecs_map_free(map);
}

void test_Map_tc_set_rehash(
    test_Map this)
{
    EcsMap *map = ecs_map_new(8);
    fill_map(map);

    test_assertint(ecs_map_bucket_count(map), 8);

    int i;
    for (i = 5; i < 8; i ++) {
        ecs_map_set(map, i, "zzz");
    }

    test_assertint(ecs_map_bucket_count(map), 16);
    test_assertstr(ecs_map_get(map, 1), "hello");
    test_assertstr(ecs_map_get(map, 2), "world");
    test_assertstr(ecs_map_get(map, 3), "foo");
    test_assertstr(ecs_map_get(map, 4), "bar");
    test_assertstr(ecs_map_get(map, 5), "zzz");
    test_assertstr(ecs_map_get(map, 6), "zzz");
    test_assertstr(ecs_map_get(map, 7), "zzz");

    ecs_map_free(map);
}

void test_Map_tc_iter(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    fill_map(map);

    EcsIter it = ecs_map_iter(map);
    test_assert(ecs_iter_hasnext(&it) == true);
    test_assertstr(ecs_iter_next(&it), "hello");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_assertstr(ecs_iter_next(&it), "world");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_assertstr(ecs_iter_next(&it), "foo");

    test_assert(ecs_iter_hasnext(&it) == true);
    test_assertstr(ecs_iter_next(&it), "bar");

    test_assert(ecs_iter_hasnext(&it) == false);
}

void test_Map_tc_iter_empty(
    test_Map this)
{
    EcsMap *map = ecs_map_new(16);
    EcsIter it = ecs_map_iter(map);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_map_free(map);
}

void test_Map_tc_iter_zero_buckets(
    test_Map this)
{
    EcsMap *map = ecs_map_new(0);
    EcsIter it = ecs_map_iter(map);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_map_free(map);
}

void test_Map_tc_set_zero_buckets(
    test_Map this)
{
    EcsMap *map = ecs_map_new(0);
    ecs_map_set(map, 1, "hello");
    test_assertint(ecs_map_count(map), 1);
    ecs_map_free(map);
}
