/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
#include <reflecs/array.h>

static
int compare_int(
    const void *p1,
    const void *p2)
{
    int v1 = *(int*)p1;
    int v2 = *(int*)p2;

    if (v1 == v2) {
        return 0;
    } else if (v1 < v2) {
        return -1;
    } else {
        return 1;
    }
}

static
EcsArrayParams arr_params = {
    .element_size = sizeof(int),
    .compare_action = compare_int
};

EcsArray* fill_array(
    EcsArray *array)
{
    int *elem;
    int i;
    for (i = 0; i < 4; i ++) {
        array = ecs_array_add(array, &arr_params, &elem);
        *elem = i;
    }

    return array;
}

void test_Array_tc_count(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    test_assertint(ecs_array_size(array), 4);
    test_assertint(ecs_array_count(array), 4);
    ecs_array_free(array);
}


void test_Array_tc_count_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(0, &arr_params);
    test_assert(array != NULL);
    test_assertint(ecs_array_count(array), 0);
    ecs_array_free(array);
}


void test_Array_tc_free_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(0, &arr_params);
    test_assert(array != NULL);
    ecs_array_free(array);
}


void test_Array_tc_get(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 1);
    test_assert(elem != NULL);
    test_assertint(*elem, 1);
    ecs_array_free(array);
}


void test_Array_tc_get_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    int *elem = ecs_array_get(array, &arr_params, 1);
    test_assert(elem == NULL);
    ecs_array_free(array);
}


void test_Array_tc_get_first(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 0);
    test_assert(elem != NULL);
    test_assertint(*elem, 0);
    ecs_array_free(array);
}


void test_Array_tc_get_last(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 3);
    test_assert(elem != NULL);
    test_assertint(*elem, 3);
    ecs_array_free(array);
}


void test_Array_tc_get_out_of_bound(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 4);
    test_assert(elem == NULL);
    ecs_array_free(array);
}


void test_Array_tc_iter(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    EcsIter it = ecs_array_iter(array, &arr_params);
    int count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *elem = ecs_iter_next(&it);
        test_assert(elem != NULL);
        test_assertint(*elem, count);
        count ++;
    }
    test_assertint(count, 4);
    ecs_array_free(array);
}


void test_Array_tc_iter_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    EcsIter it = ecs_array_iter(array, &arr_params);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_array_free(array);
}


void test_Array_tc_remove(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    array = ecs_array_remove(array, &arr_params, 1);
    test_assertint(ecs_array_size(array), 4);
    test_assertint(ecs_array_count(array), 3);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 0), 0);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 1), 3);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 2), 2);
    test_assert(ecs_array_get(array, &arr_params, 3) == NULL);
    ecs_array_free(array);
}


void test_Array_tc_remove_all(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    array = ecs_array_remove(array, &arr_params, 0);
    test_assertint(ecs_array_size(array), 4);
    test_assertint(ecs_array_count(array), 3);
    array = ecs_array_remove(array, &arr_params, 0);
    test_assertint(ecs_array_size(array), 4);
    test_assertint(ecs_array_count(array), 2);
    array = ecs_array_remove(array, &arr_params, 0);
    test_assertint(ecs_array_size(array), 2);
    test_assertint(ecs_array_count(array), 1);
    array = ecs_array_remove(array, &arr_params, 0);
    test_assert(array != NULL);
    test_assertint(ecs_array_size(array), 1);
    test_assertint(ecs_array_count(array), 0);
    ecs_array_free(array);
}


void test_Array_tc_remove_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    EcsArray *array2 = ecs_array_remove(array, &arr_params, 0);
    test_assert(array2 == NULL);
    ecs_array_free(array);
}


void test_Array_tc_remove_first(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    array = ecs_array_remove(array, &arr_params, 0);
    test_assertint(ecs_array_count(array), 3);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 0), 3);
    ecs_array_free(array);
}


void test_Array_tc_remove_last(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    array = ecs_array_remove(array, &arr_params, 3);
    test_assertint(ecs_array_count(array), 3);
    ecs_array_free(array);
}


void test_Array_tc_remove_out_of_bound(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    EcsArray *array2 = ecs_array_remove(array, &arr_params, 4);
    test_assert(array2 == NULL);
    ecs_array_free(array);
}


void test_Array_tc_sort_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(0, &arr_params);
    ecs_array_sort(array, &arr_params);
    test_assert(true);
    ecs_array_free(array);
}


void test_Array_tc_sort_rnd(
    test_Array this)
{
    int nums[] = {23, 16, 21, 13, 30, 5, 28, 31, 8, 19, 29, 12, 24, 14, 15, 1, 26, 18, 9, 25, 22, 0, 10, 3, 2, 17, 27, 20, 6, 11, 4, 7};
    EcsArray *array = ecs_array_new(0, &arr_params);

    int i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem;
        array = ecs_array_add(array, &arr_params, &elem);
        *elem = nums[i];
    }

    test_assertint(ecs_array_count(array), sizeof(nums) / sizeof(int));

    ecs_array_sort(array, &arr_params);

    EcsIter it = ecs_array_iter(array, &arr_params);
    count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_assertint(*v, count);
        count ++;
    }

    ecs_array_free(array);
}


void test_Array_tc_sort_sorted(
    test_Array this)
{
    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    EcsArray *array = ecs_array_new(0, &arr_params);

    int i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem;
        array = ecs_array_add(array, &arr_params, &elem);
        *elem = nums[i];
    }

    test_assertint(ecs_array_count(array), sizeof(nums) / sizeof(int));

    ecs_array_sort(array, &arr_params);

    EcsIter it = ecs_array_iter(array, &arr_params);
    count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_assertint(*v, count);
        count ++;
    }

    ecs_array_free(array);
}

void test_Array_tc_add_empty(
    test_Array this)
{
    EcsArray *array = ecs_array_new(0, &arr_params);
    test_assertint(ecs_array_count(array), 0);
    test_assertint(ecs_array_size(array), 0);

    int *elem;
    array = ecs_array_add(array, &arr_params, &elem);
    test_assertint(ecs_array_count(array), 1);
    test_assertint(ecs_array_size(array), 1);
    ecs_array_free(array);
}

void test_Array_tc_add_resize(
    test_Array this)
{
    EcsArray *array = ecs_array_new(4, &arr_params);
    array = fill_array(array);
    int *elem;
    array = ecs_array_add(array, &arr_params, &elem);
    *elem = 4;
    test_assertint(ecs_array_size(array), 8);
    test_assertint(ecs_array_count(array), 5);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 0), 0);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 1), 1);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 2), 2);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 3), 3);
    test_assertint(*(int*)ecs_array_get(array, &arr_params, 4), 4);
    test_assert(ecs_array_get(array, &arr_params, 5) == NULL);
    ecs_array_free(array);
}
