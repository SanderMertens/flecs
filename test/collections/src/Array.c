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
ecs_vector_params_t arr_params = {
    .element_size = sizeof(int)
};

ecs_vector_t* fill_array(
    ecs_vector_t *array)
{
    int *elem;
    int i;
    for (i = 0; i < 4; i ++) {
        elem = ecs_vector_add(&array, &arr_params);
        *elem = i;
    }

    return array;
}

void Array_setup() {
    ecs_set_os_api_defaults();
}

void Array_free_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);
    test_assert(array != NULL);
    ecs_vector_free(array);
}

void Array_count() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 4);
    ecs_vector_free(array);
}

void Array_count_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);
    test_assert(array != NULL);
    test_int(ecs_vector_count(array), 0);
    ecs_vector_free(array);
}

void Array_get() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, &arr_params, 1);
    test_assert(elem != NULL);
    test_int(*elem, 1);
    ecs_vector_free(array);
}

void Array_get_first() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, &arr_params, 0);
    test_assert(elem != NULL);
    test_int(*elem, 0);
    ecs_vector_free(array);
}

void Array_get_last() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, &arr_params, 3);
    test_assert(elem != NULL);
    test_int(*elem, 3);
    ecs_vector_free(array);
}

void Array_get_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    int *elem = ecs_vector_get(array, &arr_params, 1);
    test_assert(elem == NULL);
    ecs_vector_free(array);
}

void Array_get_out_of_bound() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, &arr_params, 4);
    test_assert(elem == NULL);
    ecs_vector_free(array);
}

void Array_add_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);
    test_int(ecs_vector_count(array), 0);
    test_int(ecs_vector_size(array), 0);

    ecs_vector_add(&array, &arr_params);
    test_int(ecs_vector_count(array), 1);
    test_int(ecs_vector_size(array), 1);
    ecs_vector_free(array);
}

void Array_add_resize() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    int *elem = ecs_vector_add(&array, &arr_params);
    *elem = 4;
    test_int(ecs_vector_size(array), 8);
    test_int(ecs_vector_count(array), 5);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 0), 0);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 1), 1);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 2), 2);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 3), 3);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 4), 4);
    test_assert(ecs_vector_get(array, &arr_params, 5) == NULL);
    ecs_vector_free(array);
}

void Array_remove() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 1));
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 3);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 0), 0);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 1), 3);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 2), 2);
    test_assert(ecs_vector_get(array, &arr_params, 3) == NULL);
    ecs_vector_free(array);
}

void Array_remove_first() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    test_int(ecs_vector_count(array), 3);
    test_int(*(int*)ecs_vector_get(array, &arr_params, 0), 3);
    ecs_vector_free(array);
}

void Array_remove_last() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 3));
    test_int(ecs_vector_count(array), 3);
    ecs_vector_free(array);
}

void Array_remove_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    ecs_vector_free(array);
    test_assert(true);
}

void Array_remove_all() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 3);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 2);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 1);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 0));
    test_assert(array != NULL);
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 0);
    ecs_vector_free(array);
}

void Array_remove_out_of_bound() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 4);
    array = fill_array(array);
    ecs_vector_remove(array, &arr_params, ecs_vector_get(array, &arr_params, 4));
    ecs_vector_free(array);
    test_assert(true);
}

void Array_sort_rnd() {
    int nums[] = {23, 16, 21, 13, 30, 5, 28, 31, 8, 19, 29, 12, 24, 14, 15, 1, 26, 18, 9, 25, 22, 0, 10, 3, 2, 17, 27, 20, 6, 11, 4, 7};
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);

    uint32_t i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_vector_add(&array, &arr_params);
        *elem = nums[i];
    }

    test_int(ecs_vector_count(array), sizeof(nums) / sizeof(int));

    ecs_vector_sort(array, &arr_params, compare_int);

    int *buffer = ecs_vector_first(array);
    count = ecs_vector_count(array);

    for (i = 0; i < count; i ++) {
        test_int(buffer[i], i);
    }

    ecs_vector_free(array);
}

void Array_sort_sorted() {
    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);

    uint32_t i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_vector_add(&array, &arr_params);
        *elem = nums[i];
    }

    test_int(ecs_vector_count(array), sizeof(nums) / sizeof(int));

    ecs_vector_sort(array, &arr_params, compare_int);

    int *buffer = ecs_vector_first(array);
    count = ecs_vector_count(array);

    for (i = 0; i < count; i ++) {
        test_int(buffer[i], i);
    }

    ecs_vector_free(array);
}

void Array_sort_empty() {
    ecs_vector_t *array = ecs_vector_new(&arr_params, 0);
    ecs_vector_sort(array, &arr_params, compare_int);
    test_assert(true);
    ecs_vector_free(array);
}

