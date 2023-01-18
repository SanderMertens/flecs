#include <collections.h>
#include <flecs/private/sparse.h>

void Sparse_setup() {
    ecs_os_set_api_defaults();
}

ecs_sparse_t* _flecs_sparse_new(
    struct ecs_allocator_t *allocator,
    struct ecs_block_allocator_t *page_allocator,
    ecs_size_t elem_size)
{
    ecs_sparse_t *sp = ecs_os_calloc_t(ecs_sparse_t);
    flecs_sparse_init(sp, allocator, page_allocator, elem_size);
    return sp;
}

#define flecs_sparse_new(a, p, T) _flecs_sparse_new(a, p, ECS_SIZEOF(T))

void flecs_sparse_free(
    ecs_sparse_t *sp)
{
    flecs_sparse_fini(sp);
    ecs_os_free(sp);
}

static
void populate(ecs_sparse_t *sp, int count) {
    int prev_count = flecs_sparse_count(sp);
    int i;
    for (i = 0; i < count; i ++) {
        int *elem = flecs_sparse_add_t(sp, int);
        test_assert(elem != NULL);
        *elem = i;
    }

    test_int(flecs_sparse_count(sp), count + prev_count);
}

void Sparse_add_1() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_1_to_empty() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_1_chunk_size_1() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_n() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem1 = flecs_sparse_add_t(sp, int);
    test_assert(elem1 != NULL);
    test_int(flecs_sparse_count(sp), 1);

    int* elem2 = flecs_sparse_add_t(sp, int);
    test_assert(elem2 != NULL);
    test_int(flecs_sparse_count(sp), 2);    
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = flecs_sparse_get_dense_t(sp, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_n_chunk_size_1() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem1 = flecs_sparse_add_t(sp, int);
    test_assert(elem1 != NULL);
    test_int(flecs_sparse_count(sp), 1);

    int* elem2 = flecs_sparse_add_t(sp, int);
    test_assert(elem2 != NULL);
    test_int(flecs_sparse_count(sp), 2);    
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = flecs_sparse_get_dense_t(sp, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    flecs_sparse_free(sp);
}

void Sparse_remove() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);
    
    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[0];
    uint64_t i1 = indices[2];

    flecs_sparse_remove_t(sp, int, 1);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i0));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i1));

    flecs_sparse_free(sp);
}

void Sparse_remove_first() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[1];
    uint64_t i1 = indices[2];

    flecs_sparse_remove_t(sp, int, 0);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i1));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i0));

    flecs_sparse_free(sp);
}

void Sparse_remove_last() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);
    
    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[0];
    uint64_t i1 = indices[1];

    flecs_sparse_remove_t(sp, int, 2);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i0));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i1));

    flecs_sparse_free(sp);
}

void Sparse_remove_all() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    void *elem2 = flecs_sparse_get_dense_t(sp, int, 1);
    void *elem3 = flecs_sparse_get_dense_t(sp, int, 2);

    flecs_sparse_remove_t(sp, int, 0);
    test_int(flecs_sparse_count(sp), 2);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == elem2);
    test_assert(flecs_sparse_try_t(sp, int, 2) == elem3);

    flecs_sparse_remove_t(sp, int, 1);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 2) == elem3);    

    flecs_sparse_remove_t(sp, int, 2);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 2) == NULL);    

    flecs_sparse_free(sp);
}

void Sparse_remove_all_n_chunks() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);

    int i;
    for (i = 0; i < 128; i ++) {
        flecs_sparse_remove_t(sp, int, i);
        test_int(flecs_sparse_count(sp), 127 - i);
    }

    flecs_sparse_free(sp);
}

void Sparse_clear_1() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 1);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_empty() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_n() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_n_chunks() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_copy() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);
    test_int(flecs_sparse_count(sp), 128);

    ecs_sparse_t sp2;
    flecs_sparse_copy(&sp2, sp);
    flecs_sparse_free(sp);
    test_int(flecs_sparse_count(&sp2), 128);

    int i;
    for (i = 0; i < 128; i ++) {
        test_int(*flecs_sparse_get_t(&sp2, int, i), i);
    }

    flecs_sparse_fini(&sp2);
}

void Sparse_restore() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);
    test_int(flecs_sparse_count(sp), 128);

    ecs_sparse_t sp2;
    flecs_sparse_copy(&sp2, sp);
    test_int(flecs_sparse_count(&sp2), 128);

    populate(sp, 128);
    test_int(flecs_sparse_count(sp), 256);

    flecs_sparse_restore(sp, &sp2);
    test_int(flecs_sparse_count(sp), 128);

    int i;
    for (i = 0; i < 128; i ++) {
        test_int(*flecs_sparse_get_t(sp, int, i), i);
    }

    flecs_sparse_free(sp);
    flecs_sparse_fini(&sp2);
}

void Sparse_add_after_clear() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 1);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    *elem = 10;

    const uint64_t *indices = flecs_sparse_ids(sp);
    test_assert(indices != NULL);

    int *ptr = flecs_sparse_get_t(sp, int, indices[0]);
    test_assert(ptr != NULL);
    test_assert(ptr == elem);
    test_int(*ptr, 10);

    flecs_sparse_free(sp);
}

void Sparse_create_delete() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    uint64_t id = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 1);

    flecs_sparse_remove_t(sp, int, id);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(!flecs_sparse_is_alive(sp, id));

    flecs_sparse_free(sp);
}

void Sparse_create_delete_2() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    uint64_t id_1 = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(flecs_sparse_is_alive(sp, id_1));
    
    uint64_t id_2 = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 2);
    test_assert(flecs_sparse_is_alive(sp, id_2));

    flecs_sparse_remove_t(sp, int, id_1);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(!flecs_sparse_is_alive(sp, id_1));

    flecs_sparse_remove_t(sp, int, id_2);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(!flecs_sparse_is_alive(sp, id_2));

    flecs_sparse_free(sp);
}

void Sparse_count_of_null() {
    test_int(flecs_sparse_count(NULL), 0);
}

void Sparse_size_of_null() {
    test_int(flecs_sparse_size(NULL), 0);
}

void Sparse_try_low_after_ensure_high() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    int *ptr_2 = flecs_sparse_try_t(sp, int, 100);
    test_assert(ptr_2 == NULL);

    flecs_sparse_free(sp);
}

void Sparse_is_alive_low_after_ensure_high() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    bool v = flecs_sparse_is_alive(sp, 100);
    test_bool(v, false);

    flecs_sparse_free(sp);
}

void Sparse_get_current_low_after_ensure_high() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    uint64_t v = flecs_sparse_get_current(sp, 100);
    test_uint(v, 0);

    flecs_sparse_free(sp);
}

void Sparse_remove_low_after_ensure_high() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    flecs_sparse_remove_t(sp, int, 100);

    flecs_sparse_free(sp);
}

void Sparse_exists_low_after_ensure_high() {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    bool v = flecs_sparse_exists(sp, 100);
    test_bool(v, false);

    flecs_sparse_free(sp);
}
