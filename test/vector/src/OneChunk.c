/* This is a managed file. Do not delete this comment. */

#include <include/test.h>
#include <reflecs/vector.h>

#define CHUNK_COUNT (64)
#define ELEMENT_COUNT (32)

static
EcsVectorParams vec_params = {
    .chunk_count = CHUNK_COUNT,
    .element_size = sizeof(int)
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
    /* Insert implementation */
}


void test_OneChunk_tc_free_empty(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_get(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_get_empty(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_get_first(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_get_last(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_get_out_of_bound(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_iter(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_iter_empty(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove_all(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove_empty(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove_first(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove_last(
    test_OneChunk this)
{
    /* Insert implementation */
}


void test_OneChunk_tc_remove_out_of_bound(
    test_OneChunk this)
{
    /* Insert implementation */
}
