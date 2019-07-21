#include <collections.h>

void Chunked_setup() {
    ecs_os_set_api_defaults();
}

void populate(ecs_chunked_t *ch, int count) {
    int i;
    for (i = 0; i < count; i ++) {
        int *elem = ecs_chunked_add(ch, int);
        test_assert(elem != NULL);
        *elem = i;
    }

    test_int(ecs_chunked_count(ch), count);
}

void Chunked_add_1() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    int* elem = ecs_chunked_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_chunked_count(ch), 1);
    
    int *ptr = ecs_chunked_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_chunked_free(ch);
}

void Chunked_add_1_to_empty() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 0);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    int* elem = ecs_chunked_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_chunked_count(ch), 1);
    
    int *ptr = ecs_chunked_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_chunked_free(ch);
}

void Chunked_add_1_chunk_size_1() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 1, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    int* elem = ecs_chunked_add(ch, int);
    test_assert(elem != NULL);
    test_int(ecs_chunked_count(ch), 1);
    
    int *ptr = ecs_chunked_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    ecs_chunked_free(ch);
}

void Chunked_add_n() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    int* elem1 = ecs_chunked_add(ch, int);
    test_assert(elem1 != NULL);
    test_int(ecs_chunked_count(ch), 1);

    int* elem2 = ecs_chunked_add(ch, int);
    test_assert(elem2 != NULL);
    test_int(ecs_chunked_count(ch), 2);    
    
    int *ptr = ecs_chunked_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = ecs_chunked_get(ch, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    ecs_chunked_free(ch);
}

void Chunked_add_n_chunk_size_1() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 1, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    int* elem1 = ecs_chunked_add(ch, int);
    test_assert(elem1 != NULL);
    test_int(ecs_chunked_count(ch), 1);

    int* elem2 = ecs_chunked_add(ch, int);
    test_assert(elem2 != NULL);
    test_int(ecs_chunked_count(ch), 2);    
    
    int *ptr = ecs_chunked_get(ch, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = ecs_chunked_get(ch, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    ecs_chunked_free(ch);
}

void Chunked_remove() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 3);
    
    const uint32_t *indices = ecs_chunked_indices(ch);
    int32_t i0 = indices[0];
    int32_t i1 = indices[2];
    void *elem = ecs_chunked_get(ch, int, 1);

    void *ptr = ecs_chunked_remove(ch, int, 1);
    test_assert(elem == ptr);
    test_int(ecs_chunked_count(ch), 2);

    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i1));
    test_assert(ecs_chunked_get(ch, int, 0) == ecs_chunked_get_sparse(ch, int, i0));
    test_assert(ecs_chunked_get(ch, int, 1) == ecs_chunked_get_sparse(ch, int, i1));

    ecs_chunked_free(ch);
}

void Chunked_remove_first() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 3);

    const uint32_t *indices = ecs_chunked_indices(ch);
    int32_t i0 = indices[1];
    int32_t i1 = indices[2];
    void *elem = ecs_chunked_get(ch, int, 0);

    void *ptr = ecs_chunked_remove(ch, int, 0);
    test_assert(elem == ptr);
    test_int(ecs_chunked_count(ch), 2);

    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i1));
    test_assert(ecs_chunked_get(ch, int, 0) == ecs_chunked_get_sparse(ch, int, i1));
    test_assert(ecs_chunked_get(ch, int, 1) == ecs_chunked_get_sparse(ch, int, i0));

    ecs_chunked_free(ch);
}

void Chunked_remove_last() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 3);
    
    const uint32_t *indices = ecs_chunked_indices(ch);
    int32_t i0 = indices[0];
    int32_t i1 = indices[1];
    void *elem = ecs_chunked_get(ch, int, 2);

    void *ptr = ecs_chunked_remove(ch, int, 2);
    test_assert(elem == ptr);
    test_int(ecs_chunked_count(ch), 2);

    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i0));
    test_assert(ptr != ecs_chunked_get_sparse(ch, int, i1));
    test_assert(ecs_chunked_get(ch, int, 0) == ecs_chunked_get_sparse(ch, int, i0));
    test_assert(ecs_chunked_get(ch, int, 1) == ecs_chunked_get_sparse(ch, int, i1));

    ecs_chunked_free(ch);
}

void Chunked_remove_all() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 3);

    void *elem1 = ecs_chunked_get(ch, int, 0);
    void *elem2 = ecs_chunked_get(ch, int, 1);
    void *elem3 = ecs_chunked_get(ch, int, 2);

    void *ptr = ecs_chunked_remove(ch, int, 0);
    test_assert(elem1 == ptr);
    test_int(ecs_chunked_count(ch), 2);

    ptr = ecs_chunked_remove(ch, int, 1);
    test_assert(elem2 == ptr);
    test_int(ecs_chunked_count(ch), 1);

    ptr = ecs_chunked_remove(ch, int, 2);
    test_assert(elem3 == ptr);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_free(ch);
}

void Chunked_remove_all_n_chunks() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 128);

    void *ptrs[128];

    int i;
    for (i = 0; i < 128; i ++) {
        ptrs[i] = ecs_chunked_get(ch, int, i);
    }

    for (i = 0; i < 128; i ++) {
        test_assert(ptrs[i] == ecs_chunked_remove(ch, int, i));
        test_int(ecs_chunked_count(ch), 127 - i);
    }

    ecs_chunked_free(ch);
}

void Chunked_clear_1() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 1);

    ecs_chunked_clear(ch);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_free(ch);
}

void Chunked_clear_empty() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_clear(ch);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_free(ch);
}

void Chunked_clear_n() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 3);

    ecs_chunked_clear(ch);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_free(ch);
}

void Chunked_clear_n_chunks() {
    ecs_chunked_t *ch = ecs_chunked_new(int, 64, 1);
    test_assert(ch != NULL);
    test_int(ecs_chunked_count(ch), 0);

    populate(ch, 128);

    ecs_chunked_clear(ch);
    test_int(ecs_chunked_count(ch), 0);

    ecs_chunked_free(ch);
}

void Chunked_memory_null() {
    uint32_t allocd = 0, used = 0; 
    ecs_chunked_memory(NULL, &allocd, &used);
    test_int(allocd, 0);
    test_int(used, 0);
}
