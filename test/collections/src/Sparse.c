#include <collections.h>

void Sparse_setup() {
    ecs_os_set_api_defaults();
}

static
void populate(ecs_sparse_t *ch, int count) {
    int i;
    for (i = 0; i < count; i ++) {
        int *elem = ecs_sparse_add(ch, int);
        test_assert(elem != NULL);
        *elem = i;
    }

    test_int(ecs_sparse_count(ch), count);
}

void Sparse_add_1() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    int* elem = ecs_sparse_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_sparse_count(ch), 1);
    
    int *ptr = ecs_sparse_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_sparse_free(ch);
}

void Sparse_add_1_to_empty() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 0);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    int* elem = ecs_sparse_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_sparse_count(ch), 1);
    
    int *ptr = ecs_sparse_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_sparse_free(ch);
}

void Sparse_add_1_chunk_size_1() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    int* elem = ecs_sparse_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_sparse_count(ch), 1);
    
    int *ptr = ecs_sparse_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_sparse_free(ch);
}

void Sparse_add_n() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    int* elem1 = ecs_sparse_add(ch, int);
    test_assert(elem1 != NULL);
    test_int(ecs_sparse_count(ch), 1);

    int* elem2 = ecs_sparse_add(ch, int);
    test_assert(elem2 != NULL);
    test_int(ecs_sparse_count(ch), 2);    
    
    int *ptr = ecs_sparse_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = ecs_sparse_get(ch, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    ecs_sparse_free(ch);
}

void Sparse_add_n_chunk_size_1() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    int* elem1 = ecs_sparse_add(ch, int);
    test_assert(elem1 != NULL);
    test_int(ecs_sparse_count(ch), 1);

    int* elem2 = ecs_sparse_add(ch, int);
    test_assert(elem2 != NULL);
    test_int(ecs_sparse_count(ch), 2);    
    
    int *ptr = ecs_sparse_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = ecs_sparse_get(ch, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    ecs_sparse_free(ch);
}

void Sparse_remove() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 3);
    
    const int32_t *indices = ecs_sparse_indices(ch);
    int32_t i0 = indices[0];
    int32_t i1 = indices[2];
    void *elem = ecs_sparse_get(ch, int, 1);

    void *ptr = ecs_sparse_remove(ch, int, 1);
    test_assert(elem == ptr);
    test_int(ecs_sparse_count(ch), 2);

    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i1));
    test_assert(ecs_sparse_get(ch, int, 0) == ecs_sparse_get_sparse(ch, int, i0));
    test_assert(ecs_sparse_get(ch, int, 1) == ecs_sparse_get_sparse(ch, int, i1));

    ecs_sparse_free(ch);
}

void Sparse_remove_first() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 3);

    const int32_t *indices = ecs_sparse_indices(ch);
    int32_t i0 = indices[1];
    int32_t i1 = indices[2];
    void *elem = ecs_sparse_get(ch, int, 0);

    void *ptr = ecs_sparse_remove(ch, int, 0);
    test_assert(elem == ptr);
    test_int(ecs_sparse_count(ch), 2);

    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i1));
    test_assert(ecs_sparse_get(ch, int, 0) == ecs_sparse_get_sparse(ch, int, i1));
    test_assert(ecs_sparse_get(ch, int, 1) == ecs_sparse_get_sparse(ch, int, i0));

    ecs_sparse_free(ch);
}

void Sparse_remove_last() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 3);
    
    const int32_t *indices = ecs_sparse_indices(ch);
    int32_t i0 = indices[0];
    int32_t i1 = indices[1];
    void *elem = ecs_sparse_get(ch, int, 2);

    void *ptr = ecs_sparse_remove(ch, int, 2);
    test_assert(elem == ptr);
    test_int(ecs_sparse_count(ch), 2);

    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_sparse_get_sparse(ch, int, i1));
    test_assert(ecs_sparse_get(ch, int, 0) == ecs_sparse_get_sparse(ch, int, i0));
    test_assert(ecs_sparse_get(ch, int, 1) == ecs_sparse_get_sparse(ch, int, i1));

    ecs_sparse_free(ch);
}

void Sparse_remove_all() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 3);

    void *elem1 = ecs_sparse_get(ch, int, 0);
    void *elem2 = ecs_sparse_get(ch, int, 1);
    void *elem3 = ecs_sparse_get(ch, int, 2);

    void *ptr = ecs_sparse_remove(ch, int, 0);
    test_assert(elem1 == ptr);
    test_int(ecs_sparse_count(ch), 2);

    ptr = ecs_sparse_remove(ch, int, 1);
    test_assert(elem2 == ptr);
    test_int(ecs_sparse_count(ch), 1);

    ptr = ecs_sparse_remove(ch, int, 2);
    test_assert(elem3 == ptr);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_free(ch);
}

void Sparse_remove_all_n_chunks() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 128);

    void *ptrs[128];

    int i;
    for (i = 0; i < 128; i ++) {
        ptrs[i] = ecs_sparse_get(ch, int, i);
    }

    for (i = 0; i < 128; i ++) {
        test_assert(ptrs[i] == ecs_sparse_remove(ch, int, i));
        test_int(ecs_sparse_count(ch), 127 - i);
    }

    ecs_sparse_free(ch);
}

void Sparse_clear_1() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 1);

    ecs_sparse_clear(ch);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_free(ch);
}

void Sparse_clear_empty() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_clear(ch);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_free(ch);
}

void Sparse_clear_n() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 3);

    ecs_sparse_clear(ch);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_free(ch);
}

void Sparse_clear_n_chunks() {
    ecs_sparse_t *ch = ecs_sparse_new(int, 1);
    test_assert(ch != NULL);
    test_int(ecs_sparse_count(ch), 0);

    populate(ch, 128);

    ecs_sparse_clear(ch);
    test_int(ecs_sparse_count(ch), 0);

    ecs_sparse_free(ch);
}

void Sparse_memory_null() {
    int32_t allocd = 0, used = 0; 
    ecs_sparse_memory(NULL, &allocd, &used);
    test_int(allocd, 0);
    test_int(used, 0);
}