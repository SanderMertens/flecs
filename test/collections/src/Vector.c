#include <collections.h>

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

ecs_vector_t* fill_array(
    ecs_vector_t *array)
{
    int *elem;
    int i;
    for (i = 0; i < 4; i ++) {
        elem = ecs_vector_add(&array, int);
        *elem = i;
    }

    return array;
}

void Vector_setup() {
    ecs_os_set_api_defaults();
}

void Vector_free_empty() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    test_assert(array != NULL);
    ecs_vector_free(array);
}

void Vector_count() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    test_int(ecs_vector_size(array), 4);
    test_int(ecs_vector_count(array), 4);
    ecs_vector_free(array);
}

void Vector_count_empty() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    test_assert(array != NULL);
    test_int(ecs_vector_count(array), 0);
    ecs_vector_free(array);
}

void Vector_get() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, int, 1);
    test_assert(elem != NULL);
    test_int(*elem, 1);
    ecs_vector_free(array);
}

void Vector_get_first() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, int, 0);
    test_assert(elem != NULL);
    test_int(*elem, 0);
    ecs_vector_free(array);
}

void Vector_get_last() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    int *elem = ecs_vector_get(array, int, 3);
    test_assert(elem != NULL);
    test_int(*elem, 3);
    test_assert(elem == ecs_vector_last(array, int));
    ecs_vector_free(array);
}

void Vector_get_last_from_empty() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    int *elem = ecs_vector_last(array, int);
    test_assert(elem == NULL);
    ecs_vector_free(array);
}

void Vector_get_last_from_null() {
    int *elem = ecs_vector_last(NULL, int);
    test_assert(elem == NULL);
}

void Vector_get_empty() {
    install_test_abort();
    ecs_vector_t *array = ecs_vector_new(int, 4);
    test_expect_abort();
    ecs_vector_get(array, int, 1);
}

void Vector_get_out_of_bound() {
    install_test_abort();
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    test_expect_abort();
    ecs_vector_get(array, int, 4);
}

void Vector_add_empty() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    test_int(ecs_vector_count(array), 0);
    test_int(ecs_vector_size(array), 0);

    ecs_vector_add(&array, int);
    test_int(ecs_vector_count(array), 1);
    test_int(ecs_vector_size(array), 2);
    ecs_vector_free(array);
}

void Vector_add_resize() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    int *elem = ecs_vector_add(&array, int);
    *elem = 4;
    test_int(ecs_vector_size(array), 8);
    test_int(ecs_vector_count(array), 5);
    test_int(*(int*)ecs_vector_get(array, int, 0), 0);
    test_int(*(int*)ecs_vector_get(array, int, 1), 1);
    test_int(*(int*)ecs_vector_get(array, int, 2), 2);
    test_int(*(int*)ecs_vector_get(array, int, 3), 3);
    test_int(*(int*)ecs_vector_get(array, int, 4), 4);
    test_assert(ecs_vector_count(array) == 5);
    ecs_vector_free(array);
}

void Vector_sort_rnd() {
    int nums[] = {23, 16, 21, 13, 30, 5, 28, 31, 8, 19, 29, 12, 24, 14, 15, 1, 26, 18, 9, 25, 22, 0, 10, 3, 2, 17, 27, 20, 6, 11, 4, 7};
    ecs_vector_t *array = ecs_vector_new(int, 0);

    int32_t i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_vector_add(&array, int);
        *elem = nums[i];
    }

    test_int(ecs_vector_count(array), sizeof(nums) / sizeof(int));

    ecs_vector_sort(array, int, compare_int);

    int *buffer = ecs_vector_first(array, int);
    count = ecs_vector_count(array);

    for (i = 0; i < count; i ++) {
        test_int(buffer[i], i);
    }

    ecs_vector_free(array);
}

void Vector_sort_sorted() {
    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    ecs_vector_t *array = ecs_vector_new(int, 0);

    int32_t i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_vector_add(&array, int);
        *elem = nums[i];
    }

    test_int(ecs_vector_count(array), sizeof(nums) / sizeof(int));

    ecs_vector_sort(array, int, compare_int);

    int *buffer = ecs_vector_first(array, int);
    count = ecs_vector_count(array);

    for (i = 0; i < count; i ++) {
        test_int(buffer[i], i);
    }

    ecs_vector_free(array);
}

void Vector_sort_empty() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    ecs_vector_sort(array, int, compare_int);
    test_assert(true); // No observable side effects
    ecs_vector_free(array);
}

void Vector_sort_null() {
    ecs_vector_sort(NULL, int, compare_int);
    test_assert(true); // No observable side effects
}

void Vector_size_of_null() {
    test_int(ecs_vector_size(NULL), 0);
}

void Vector_set_size_smaller_than_count() {
    ecs_vector_t *vector = ecs_vector_new(int, 4);
    vector = fill_array(vector);
    test_int(ecs_vector_count(vector), 4);
    test_int(ecs_vector_size(vector), 4);

    ecs_vector_set_size(&vector, int, 3);

    /* Vector contains 4 elements, will not be downsized */
    test_int(ecs_vector_size(vector), 4);

    ecs_vector_free(vector);
}

void Vector_pop_elements() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);
    int value;

    test_assert( ecs_vector_pop(array, int, &value));
    test_int(value, 3);

    test_assert( ecs_vector_pop(array, int, &value));
    test_int(value, 2);

    test_assert( ecs_vector_pop(array, int, &value));
    test_int(value, 1);

    test_assert( ecs_vector_pop(array, int, &value));
    test_int(value, 0);

    test_assert( !ecs_vector_pop(array, int, &value));

    ecs_vector_free(array);
}

void Vector_pop_null() {
    test_assert( !ecs_vector_pop(NULL, int, NULL));
}

void Vector_reclaim() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    array = fill_array(array);
    array = fill_array(array);
    array = fill_array(array);

    test_int(ecs_vector_count(array), 12);
    test_int(ecs_vector_size(array), 16);

    ecs_vector_reclaim(&array, int);

    test_int(ecs_vector_count(array), 12);
    test_int(ecs_vector_size(array), 12);

    ecs_vector_free(array);
}

void Vector_grow() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    array = fill_array(array);

    test_int(ecs_vector_count(array), 4);
    test_int(ecs_vector_size(array), 4);

    ecs_vector_grow(&array, int, 8);

    test_int(ecs_vector_count(array), 4);
    test_int(ecs_vector_size(array), 16);

    ecs_vector_free(array);
}

void Vector_copy() {
    ecs_vector_t *array = ecs_vector_new(int, 4);
    array = fill_array(array);

    ecs_vector_t *copy = ecs_vector_copy(array, int);
    test_int(ecs_vector_count(copy), 4);
    test_int(*(int*)ecs_vector_get(copy, int, 0), 0);
    test_int(*(int*)ecs_vector_get(copy, int, 1), 1);
    test_int(*(int*)ecs_vector_get(copy, int, 2), 2);
    test_int(*(int*)ecs_vector_get(copy, int, 3), 3);

    ecs_vector_free(array);
    ecs_vector_free(copy);
}

void Vector_copy_null() {
    test_assert( ecs_vector_copy(NULL, int) == NULL);
}

void Vector_memory() {
    ecs_size_t allocd = 0, used = 0;
    ecs_vector_t *array = ecs_vector_new(int, 0);
    array = fill_array(array);

    ecs_vector_memory(array, int, &allocd, &used);
    test_int(allocd, 8 * sizeof(int));
    test_int(used, 4 * sizeof(int));

    ecs_vector_free(array);
}

void Vector_memory_from_null() {
    ecs_size_t allocd = 0, used = 0;
    ecs_vector_memory(NULL, int, &allocd, &used);
    test_int(allocd, 0);
    test_int(used, 0);
}

void Vector_addn_to_null() {
    ecs_vector_t *array = NULL;
    void *ptr = ecs_vector_addn(&array, int, 4);
    test_assert(array != NULL);
    test_assert(ptr != NULL);
    test_int(ecs_vector_count(array), 4);
    ecs_vector_free(array);
}

void Vector_addn_to_0_size() {
    ecs_vector_t *array = ecs_vector_new(int, 0);
    test_assert(array != NULL);
    void *ptr = ecs_vector_addn(&array, int, 4);
    test_assert(array != NULL);
    test_assert(ptr != NULL);
    test_int(ecs_vector_count(array), 4);
    ecs_vector_free(array);
}

void Vector_set_min_count() {
    ecs_vector_t *array = ecs_vector_new(int, 0);

    ecs_vector_set_min_count(&array, int, 4);
    test_int(ecs_vector_count(array), 4);
    test_int(ecs_vector_size(array), 4);

    ecs_vector_free(array);
}

void Vector_set_min_size() {
    ecs_vector_t *array = ecs_vector_new(int, 0);

    ecs_vector_set_min_size(&array, int, 4);
    test_int(ecs_vector_count(array), 0);
    test_int(ecs_vector_size(array), 4);

    ecs_vector_free(array);
}

void Vector_set_min_size_to_smaller() {
    ecs_vector_t *array = ecs_vector_new(int, 4);

    ecs_vector_set_min_size(&array, int, 2);
    test_int(ecs_vector_count(array), 0);
    test_int(ecs_vector_size(array), 4);

    ecs_vector_free(array);
}
