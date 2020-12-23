#include <collections.h>

void Ptree_setup() {
    ecs_os_set_api_defaults();
}

void Ptree_get_first_65k() {
    ecs_ptree_t *pt = ecs_ptree_new(int);
    test_assert(pt != NULL);

    int i;
    for (i = 0; i < 65536; i ++) {
        int *ptr = ecs_ptree_get(pt, int, i);
        test_assert(ptr != NULL);
        test_int(*ptr, 0);

        test_assert(ptr == ecs_ptree_ensure(pt, int, i));
    }

    test_assert(ecs_ptree_get(pt, int, i) == NULL);

    ecs_ptree_free(pt);
}

void Ptree_get_existing() {
    ecs_ptree_t *pt = ecs_ptree_new(int);
    test_assert(pt != NULL);

    test_assert(ecs_ptree_get(pt, int, 100000) == NULL);

    int *ptr = ecs_ptree_ensure(pt, int, 100000);
    test_assert(ptr != NULL);
    *ptr = 10;

    int *ptr_get = ecs_ptree_get(pt, int, 100000);
    test_assert(ptr == ptr_get);
    test_int(*ptr_get, 10);

    ecs_ptree_free(pt);
}

void Ptree_new_index_below_prev() {
    ecs_ptree_t *pt = ecs_ptree_new(int);
    test_assert(pt != NULL);

    int *ptr_1 = ecs_ptree_ensure(pt, int, 100000);
    test_assert(ptr_1 != NULL);
    test_int(*ptr_1, 0);
    *ptr_1 = 10;

    int *ptr_2 = ecs_ptree_ensure(pt, int, 99999);
    test_assert(ptr_2 != NULL);
    test_int(*ptr_2, 0);
    *ptr_2 = 20;

    test_assert(ecs_ptree_get(pt, int, 100000) != NULL);
    test_assert(ecs_ptree_get(pt, int, 99999) != NULL);

    test_int(*ecs_ptree_get(pt, int, 100000), 10);
    test_int(*ecs_ptree_get(pt, int, 99999), 20);

    ecs_ptree_free(pt);
}

void Ptree_new_index_below_prev_page_size_limit() {
    ecs_ptree_t *pt = ecs_ptiny_new(int);
    test_assert(pt != NULL);

    int *ptr_1 = ecs_ptiny_ensure(pt, int, 65535);
    test_assert(ptr_1 != NULL);
    test_int(*ptr_1, 0);
    *ptr_1 = 10;

    int *ptr_2 = ecs_ptiny_ensure(pt, int, 65000);
    test_assert(ptr_2 != NULL);
    test_int(*ptr_2, 0);
    *ptr_2 = 15;

    int *ptr_3 = ecs_ptiny_ensure(pt, int, 1000);
    test_assert(ptr_3 != NULL);
    test_int(*ptr_3, 0);
    *ptr_3 = 20;

    test_assert(ecs_ptiny_get(pt, int, 65535) != NULL);
    test_assert(ecs_ptiny_get(pt, int, 65000) != NULL);
    test_assert(ecs_ptiny_get(pt, int, 1000) != NULL);

    test_int(*ecs_ptiny_get(pt, int, 65535), 10);
    test_int(*ecs_ptiny_get(pt, int, 65000), 15);
    test_int(*ecs_ptiny_get(pt, int, 1000), 20);

    ecs_ptree_free(pt);
}

void Ptree_new_index_above_prev() {
    ecs_ptree_t *pt = ecs_ptiny_new(int);
    test_assert(pt != NULL);

    int *ptr_1 = ecs_ptiny_ensure(pt, int, 1);
    test_assert(ptr_1 != NULL);
    test_int(*ptr_1, 0);
    *ptr_1 = 10;

    int *ptr_2 = ecs_ptiny_ensure(pt, int, 8);
    test_assert(ptr_2 != NULL);
    test_int(*ptr_2, 0);
    *ptr_2 = 20;
    
    test_assert(ecs_ptiny_get(pt, int, 1) != NULL);
    test_assert(ecs_ptiny_get(pt, int, 8) != NULL);

    test_int(*ecs_ptiny_get(pt, int, 1), 10);
    test_int(*ecs_ptiny_get(pt, int, 8), 20);

    ecs_ptree_free(pt);
}

void Ptree_new_index_above_prev_page_size_limit() {
    ecs_ptree_t *pt = ecs_ptiny_new(int);
    test_assert(pt != NULL);

    int *ptr_1 = ecs_ptiny_ensure(pt, int, 65000);
    test_assert(ptr_1 != NULL);
    test_int(*ptr_1, 0);
    *ptr_1 = 10;

    int *ptr_2 = ecs_ptiny_ensure(pt, int, 65535);
    test_assert(ptr_2 != NULL);
    test_int(*ptr_2, 0);
    *ptr_2 = 20;
    
    test_assert(ecs_ptiny_get(pt, int, 65000) != NULL);
    test_assert(ecs_ptiny_get(pt, int, 65535) != NULL);

    test_int(*ecs_ptiny_get(pt, int, 65000), 10);
    test_int(*ecs_ptiny_get(pt, int, 65535), 20);

    ecs_ptree_free(pt);
}

void Ptree_iter_tiny() {
    ecs_ptree_t *pt = ecs_ptiny_new(uint64_t);

    uint64_t *ptr;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 1);
    test_assert(ptr != NULL);
    *ptr = 1;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10);
    test_assert(ptr != NULL);
    *ptr = 10;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 100);
    test_assert(ptr != NULL);
    *ptr = 100;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000);
    test_assert(ptr != NULL);
    *ptr = 10000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10001);
    test_assert(ptr != NULL);
    *ptr = 10001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000);
    test_assert(ptr != NULL);
    *ptr = 10000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000001);
    test_assert(ptr != NULL);
    *ptr = 10000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000000u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000000u;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000001u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000001u;

    ecs_ptree_iter_t it = ecs_ptiny_iter(pt);
    int count = 0;
    while ((ptr = ecs_ptiny_next(&it, uint64_t))) {
        if (*ptr != 0) {
            test_int(*ptr, it.index);
            count ++;
        }
    }

    test_int(count, 15);

    ecs_ptiny_free(pt);
}

void Ptree_iter_tiny_mixed_get_ensure() {
    ecs_ptree_t *pt = ecs_ptiny_new(uint64_t);

    uint64_t *ptr;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 1);
    test_assert(ptr != NULL);
    *ptr = 1;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10);
    test_assert(ptr != NULL);
    *ptr = 10;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 100);
    test_assert(ptr != NULL);
    *ptr = 100;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000);
    test_assert(ptr != NULL);
    *ptr = 10000;
    ptr = ecs_ptiny_get(pt, uint64_t, 10001);
    test_assert(ptr != NULL);
    *ptr = 10001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000);
    test_assert(ptr != NULL);
    *ptr = 10000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000001);
    test_assert(ptr != NULL);
    *ptr = 10000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000000001;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000000u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000000u;
    ptr = ecs_ptiny_ensure(pt, uint64_t, 10000000000000000001u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000001u;

    ecs_ptree_iter_t it = ecs_ptiny_iter(pt);
    int count = 0;
    while ((ptr = ecs_ptiny_next(&it, uint64_t))) {
        if (*ptr != 0) {
            test_int(*ptr, it.index);
            count ++;
        }
    }

    test_int(count, 15);

    ecs_ptiny_free(pt);
}

void Ptree_iter() {
    ecs_ptree_t *pt = ecs_ptree_new(uint64_t);

    uint64_t *ptr;
    ptr = ecs_ptree_ensure(pt, uint64_t, 1);
    test_assert(ptr != NULL);
    *ptr = 1;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10);
    test_assert(ptr != NULL);
    *ptr = 10;
    ptr = ecs_ptree_ensure(pt, uint64_t, 100);
    test_assert(ptr != NULL);
    *ptr = 100;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000);
    test_assert(ptr != NULL);
    *ptr = 10000;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10001);
    test_assert(ptr != NULL);
    *ptr = 10001;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000);
    test_assert(ptr != NULL);
    *ptr = 10000000;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000001);
    test_assert(ptr != NULL);
    *ptr = 10000001;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000001;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000001;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000000000);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000000001);
    test_assert(ptr != NULL);
    *ptr = 10000000000000001;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000000000000u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000000u;
    ptr = ecs_ptree_ensure(pt, uint64_t, 10000000000000000001u);
    test_assert(ptr != NULL);
    *ptr = 10000000000000000001u;

    ecs_ptree_iter_t it = ecs_ptree_iter(pt);
    int count = 0;
    while ((ptr = ecs_ptree_next(&it, uint64_t))) {
        if (*ptr != 0) {
            test_int(*ptr, it.index);
            count ++;
        }
    }

    test_int(count, 15);

    ecs_ptree_free(pt);
}
