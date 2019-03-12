#include <include/collections.h>
#include "../../include/private/types.h"

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
    .element_size = sizeof(int)
};

EcsArray* fill_array(
    EcsArray *array)
{
    int *elem;
    int i;
    for (i = 0; i < 4; i ++) {
        elem = ecs_array_add(&array, &arr_params);
        *elem = i;
    }

    return array;
}

void Array_free_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 0);
    test_assert(array != NULL);
    ecs_array_free(array);
}

void Array_count() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 4);
    ecs_array_free(array);
}

void Array_count_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 0);
    test_assert(array != NULL);
    test_int(ecs_array_count(array), 0);
    ecs_array_free(array);
}

void Array_get() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 1);
    test_assert(elem != NULL);
    test_int(*elem, 1);
    ecs_array_free(array);
}

void Array_get_first() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 0);
    test_assert(elem != NULL);
    test_int(*elem, 0);
    ecs_array_free(array);
}

void Array_get_last() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 3);
    test_assert(elem != NULL);
    test_int(*elem, 3);
    ecs_array_free(array);
}

void Array_get_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    int *elem = ecs_array_get(array, &arr_params, 1);
    test_assert(elem == NULL);
    ecs_array_free(array);
}

void Array_get_out_of_bound() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_array_get(array, &arr_params, 4);
    test_assert(elem == NULL);
    ecs_array_free(array);
}

void Array_add_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 0);
    test_int(ecs_array_count(array), 0);
    test_int(ecs_array_size(array), 0);

    ecs_array_add(&array, &arr_params);
    test_int(ecs_array_count(array), 1);
    test_int(ecs_array_size(array), 1);
    ecs_array_free(array);
}

void Array_add_resize() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_array_add(&array, &arr_params);
    *elem = 4;
    test_int(ecs_array_size(array), 8);
    test_int(ecs_array_count(array), 5);
    test_int(*(int*)ecs_array_get(array, &arr_params, 0), 0);
    test_int(*(int*)ecs_array_get(array, &arr_params, 1), 1);
    test_int(*(int*)ecs_array_get(array, &arr_params, 2), 2);
    test_int(*(int*)ecs_array_get(array, &arr_params, 3), 3);
    test_int(*(int*)ecs_array_get(array, &arr_params, 4), 4);
    test_assert(ecs_array_get(array, &arr_params, 5) == NULL);
    ecs_array_free(array);
}

void Array_iter() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    EcsIter it = ecs_array_iter(array, &arr_params);
    int count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *elem = ecs_iter_next(&it);
        test_assert(elem != NULL);
        test_int(*elem, count);
        count ++;
    }
    test_int(count, 4);
    ecs_array_free(array);
}

void Array_iter_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    EcsIter it = ecs_array_iter(array, &arr_params);
    test_assert(!ecs_iter_hasnext(&it));
    ecs_array_free(array);
}

void Array_remove() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 1));
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 3);
    test_int(*(int*)ecs_array_get(array, &arr_params, 0), 0);
    test_int(*(int*)ecs_array_get(array, &arr_params, 1), 3);
    test_int(*(int*)ecs_array_get(array, &arr_params, 2), 2);
    test_assert(ecs_array_get(array, &arr_params, 3) == NULL);
    ecs_array_free(array);
}

void Array_remove_first() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    test_int(ecs_array_count(array), 3);
    test_int(*(int*)ecs_array_get(array, &arr_params, 0), 3);
    ecs_array_free(array);
}

void Array_remove_last() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 3));
    test_int(ecs_array_count(array), 3);
    ecs_array_free(array);
}

void Array_remove_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    ecs_array_free(array);
    test_assert(true);
}

void Array_remove_all() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 3);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 2);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 1);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 0));
    test_assert(array != NULL);
    test_int(ecs_array_size(array), 4);
    test_int(ecs_array_count(array), 0);
    ecs_array_free(array);
}

void Array_remove_out_of_bound() {
    EcsArray *array = ecs_array_new(&arr_params, 4);
    array = fill_array(array);
    ecs_array_remove(array, &arr_params, ecs_array_get(array, &arr_params, 4));
    ecs_array_free(array);
    test_assert(true);
}

void Array_sort_rnd() {
    int nums[] = {23, 16, 21, 13, 30, 5, 28, 31, 8, 19, 29, 12, 24, 14, 15, 1, 26, 18, 9, 25, 22, 0, 10, 3, 2, 17, 27, 20, 6, 11, 4, 7};
    EcsArray *array = ecs_array_new(&arr_params, 0);

    int i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_array_add(&array, &arr_params);
        *elem = nums[i];
    }

    test_int(ecs_array_count(array), sizeof(nums) / sizeof(int));

    ecs_array_sort(array, &arr_params, compare_int);

    EcsIter it = ecs_array_iter(array, &arr_params);
    count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_int(*v, count);
        count ++;
    }

    ecs_array_free(array);
}

void Array_sort_sorted() {
    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    EcsArray *array = ecs_array_new(&arr_params, 0);

    int i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_array_add(&array, &arr_params);
        *elem = nums[i];
    }

    test_int(ecs_array_count(array), sizeof(nums) / sizeof(int));

    ecs_array_sort(array, &arr_params, compare_int);

    EcsIter it = ecs_array_iter(array, &arr_params);
    count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_int(*v, count);
        count ++;
    }

    ecs_array_free(array);
}

void Array_sort_empty() {
    EcsArray *array = ecs_array_new(&arr_params, 0);
    ecs_array_sort(array, &arr_params, compare_int);
    test_assert(true);
    ecs_array_free(array);
}
