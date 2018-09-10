/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
#include <reflecs/vector.h>

#define CHUNK_COUNT (64)
#define ELEMENT_COUNT (32)

static
int compare_int(
    void *p1,
    void *p2)
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
EcsVectorParams vec_params = {
    .chunk_count = CHUNK_COUNT,
    .element_size = sizeof(int),
    .compare_action = compare_int
};

static
void fill_vector(
    EcsVector *vec)
{
    int i;
    for (i = 0; i < ELEMENT_COUNT; i ++) {
        int *elem = ecs_vector_add(vec, &vec_params);
        *elem = i;
    }
}

void test_OneChunk_tc_count(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_count_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    test_assertint(ecs_vector_count(vec), 0);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_free_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    ecs_vector_free(vec);
    test_assert(true);
}


void test_OneChunk_tc_get(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    int *elem = ecs_vector_get(vec, &vec_params, 10);
    test_assert(elem != NULL);
    test_assertint(*elem, 10);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_get_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    test_assert(ecs_vector_get(vec, &vec_params, 10) == NULL);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_get_first(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    int *elem = ecs_vector_get(vec, &vec_params, 0);
    test_assert(elem != NULL);
    test_assertint(*elem, 0);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_get_last(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    int *elem = ecs_vector_get(vec, &vec_params, ELEMENT_COUNT - 1);
    test_assert(elem != NULL);
    test_assertint(*elem, ELEMENT_COUNT - 1);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_get_out_of_bound(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    int *elem = ecs_vector_get(vec, &vec_params, ELEMENT_COUNT);
    test_assert(elem == NULL);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_iter(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);

    int count = 0;
    EcsIter it = ecs_vector_iter(vec, &vec_params);
    while (ecs_iter_hasnext(&it)) {
        int *elem = ecs_iter_next(&it);
        test_assertint(count, *elem);
        count ++;
    }

    test_assertint(count, ELEMENT_COUNT);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_iter_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    EcsIter it = ecs_vector_iter(vec, &vec_params);
    test_assert(ecs_iter_hasnext(&it) == false);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_remove(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    int *elem = ecs_vector_get(vec, &vec_params, 10);
    test_assert(elem != NULL);
    test_assertint(*elem, 10);
    test_assert(ecs_vector_remove(vec, &vec_params, elem) == EcsOk);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT - 1);
    test_assert(ecs_vector_get(vec, &vec_params, 10) == elem);
    test_assert(*elem != 10);

    elem = ecs_vector_get(vec, &vec_params, 11);
    test_assert(elem != NULL);
    test_assertint(*elem, 11);

    ecs_vector_free(vec);
}

void test_OneChunk_tc_remove_all(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    int i;
    void *elem = ecs_vector_get(vec, &vec_params, 0);
    for (i = 0; i < ELEMENT_COUNT; i ++) {
        test_assert(ecs_vector_remove(vec, &vec_params, elem) == EcsOk);
        test_assertint(ecs_vector_count(vec), ELEMENT_COUNT - i - 1);
    }

    ecs_vector_free(vec);
}


void test_OneChunk_tc_remove_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    test_assert(ecs_vector_remove(vec, &vec_params, NULL) == EcsError);
    ecs_vector_free(vec);
}


void test_OneChunk_tc_remove_first(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    int *elem = ecs_vector_get(vec, &vec_params, 0);
    test_assert(elem != NULL);
    test_assertint(*elem, 0);
    test_assert(ecs_vector_remove(vec, &vec_params, elem) == EcsOk);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT - 1);
    test_assert(ecs_vector_get(vec, &vec_params, 0) == elem);
    test_assert(*elem != 0);

    elem = ecs_vector_get(vec, &vec_params, 1);
    test_assert(elem != NULL);
    test_assertint(*elem, 1);

    ecs_vector_free(vec);
}


void test_OneChunk_tc_remove_last(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    int *elem = ecs_vector_get(vec, &vec_params, ELEMENT_COUNT - 1);
    test_assert(elem != NULL);
    test_assertint(*elem, ELEMENT_COUNT - 1);
    test_assert(ecs_vector_remove(vec, &vec_params, elem) == EcsOk);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT - 1);

    elem = ecs_vector_get(vec, &vec_params, ELEMENT_COUNT - 2);
    test_assert(elem != NULL);
    test_assertint(*elem, ELEMENT_COUNT - 2);

    ecs_vector_free(vec);
}


void test_OneChunk_tc_remove_out_of_bound(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    int *elem = ecs_vector_get(vec, &vec_params, ELEMENT_COUNT - 1);
    test_assert(elem != NULL);
    test_assertint(*elem, ELEMENT_COUNT - 1);
    test_assert(ecs_vector_remove(vec, &vec_params, elem + 1) == EcsError);

    ecs_vector_free(vec);
}

void test_OneChunk_tc_sort_empty(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);

    ecs_vector_sort(vec, &vec_params);
    test_assertint(ecs_vector_count(vec), 0);

    ecs_vector_free(vec);
}

void test_OneChunk_tc_sort_rnd(
    test_OneChunk this)
{
    int nums[] = {23, 16, 21, 13, 30, 5, 28, 31, 8, 19, 29, 12, 24, 14, 15, 1, 26, 18, 9, 25, 22, 0, 10, 3, 2, 17, 27, 20, 6, 11, 4, 7};
    EcsVector *vec = ecs_vector_new(&vec_params);

    int i, count = sizeof(nums) / sizeof(int);
    for (i = 0; i < count; i ++) {
        int *elem = ecs_vector_add(vec, &vec_params);
        *elem = nums[i];
    }

    test_assertint(ecs_vector_count(vec), sizeof(nums) / sizeof(int));

    ecs_vector_sort(vec, &vec_params);

    EcsIter it = ecs_vector_iter(vec, &vec_params);
    count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_assertint(*v, count);
        count ++;
    }

    ecs_vector_free(vec);
}

void test_OneChunk_tc_sort_sorted(
    test_OneChunk this)
{
    EcsVector *vec = ecs_vector_new(&vec_params);
    fill_vector(vec);
    test_assertint(ecs_vector_count(vec), ELEMENT_COUNT);

    ecs_vector_sort(vec, &vec_params);

    EcsIter it = ecs_vector_iter(vec, &vec_params);

    int count = 0;
    while (ecs_iter_hasnext(&it)) {
        int *v = ecs_iter_next(&it);
        test_assertint(*v, count);
        count ++;
    }

    ecs_vector_free(vec);
}
