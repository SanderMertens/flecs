/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
#include <reflecs/map.h>

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


void test_Map_tc_remove(
    test_Map this)
{
    /* Insert implementation */
}


void test_Map_tc_remove_empty(
    test_Map this)
{
    /* Insert implementation */
}


void test_Map_tc_remove_unknown(
    test_Map this)
{
    /* Insert implementation */
}


void test_Map_tc_set_empty(
    test_Map this)
{
    /* Insert implementation */
}


void test_Map_tc_set_new(
    test_Map this)
{
    /* Insert implementation */
}


void test_Map_tc_set_overwrite(
    test_Map this)
{
    /* Insert implementation */
}

void test_Map_tc_set_rehash(
    test_Map this)
{
    /* Insert implementation */
}
