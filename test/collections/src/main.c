
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <collections.h>

// Testsuite 'Vector'
void Vector_setup(void);
void Vector_free_empty(void);
void Vector_count(void);
void Vector_count_empty(void);
void Vector_get(void);
void Vector_get_first(void);
void Vector_get_last(void);
void Vector_get_empty(void);
void Vector_get_out_of_bound(void);
void Vector_add_empty(void);
void Vector_add_resize(void);
void Vector_remove(void);
void Vector_remove_first(void);
void Vector_remove_last(void);
void Vector_remove_empty(void);
void Vector_remove_all(void);
void Vector_remove_out_of_bound(void);
void Vector_sort_rnd(void);
void Vector_sort_sorted(void);
void Vector_sort_empty(void);
void Vector_size_of_null(void);
void Vector_set_size_smaller_than_count(void);
void Vector_pop_elements(void);

// Testsuite 'Queue'
void Queue_setup(void);
void Queue_free_empty(void);
void Queue_push(void);

// Testsuite 'Map'
void Map_setup(void);
void Map_count(void);
void Map_count_empty(void);
void Map_set_overwrite(void);
void Map_set_rehash(void);
void Map_set_zero_buckets(void);
void Map_get(void);
void Map_get_all(void);
void Map_get_empty(void);
void Map_get_unknown(void);
void Map_iter(void);
void Map_iter_empty(void);
void Map_iter_zero_buckets(void);
void Map_remove(void);
void Map_remove_empty(void);
void Map_remove_unknown(void);
void Map_grow(void);
void Map_set_size_0(void);

// Testsuite 'Sparse'
void Sparse_setup(void);
void Sparse_add_1(void);
void Sparse_add_1_to_empty(void);
void Sparse_add_1_chunk_size_1(void);
void Sparse_add_n(void);
void Sparse_add_n_chunk_size_1(void);
void Sparse_remove(void);
void Sparse_remove_first(void);
void Sparse_remove_last(void);
void Sparse_remove_all(void);
void Sparse_remove_all_n_chunks(void);
void Sparse_clear_1(void);
void Sparse_clear_empty(void);
void Sparse_clear_n(void);
void Sparse_clear_n_chunks(void);
void Sparse_memory_null(void);

bake_test_case Vector_testcases[] = {
    {
        "free_empty",
        Vector_free_empty
    },
    {
        "count",
        Vector_count
    },
    {
        "count_empty",
        Vector_count_empty
    },
    {
        "get",
        Vector_get
    },
    {
        "get_first",
        Vector_get_first
    },
    {
        "get_last",
        Vector_get_last
    },
    {
        "get_empty",
        Vector_get_empty
    },
    {
        "get_out_of_bound",
        Vector_get_out_of_bound
    },
    {
        "add_empty",
        Vector_add_empty
    },
    {
        "add_resize",
        Vector_add_resize
    },
    {
        "remove",
        Vector_remove
    },
    {
        "remove_first",
        Vector_remove_first
    },
    {
        "remove_last",
        Vector_remove_last
    },
    {
        "remove_empty",
        Vector_remove_empty
    },
    {
        "remove_all",
        Vector_remove_all
    },
    {
        "remove_out_of_bound",
        Vector_remove_out_of_bound
    },
    {
        "sort_rnd",
        Vector_sort_rnd
    },
    {
        "sort_sorted",
        Vector_sort_sorted
    },
    {
        "sort_empty",
        Vector_sort_empty
    },
    {
        "size_of_null",
        Vector_size_of_null
    },
    {
        "set_size_smaller_than_count",
        Vector_set_size_smaller_than_count
    },
    {
        "pop_elements",
        Vector_pop_elements
    }
};

bake_test_case Queue_testcases[] = {
    {
        "free_empty",
        Queue_free_empty
    },
    {
        "push",
        Queue_push
    }
};

bake_test_case Map_testcases[] = {
    {
        "count",
        Map_count
    },
    {
        "count_empty",
        Map_count_empty
    },
    {
        "set_overwrite",
        Map_set_overwrite
    },
    {
        "set_rehash",
        Map_set_rehash
    },
    {
        "set_zero_buckets",
        Map_set_zero_buckets
    },
    {
        "get",
        Map_get
    },
    {
        "get_all",
        Map_get_all
    },
    {
        "get_empty",
        Map_get_empty
    },
    {
        "get_unknown",
        Map_get_unknown
    },
    {
        "iter",
        Map_iter
    },
    {
        "iter_empty",
        Map_iter_empty
    },
    {
        "iter_zero_buckets",
        Map_iter_zero_buckets
    },
    {
        "remove",
        Map_remove
    },
    {
        "remove_empty",
        Map_remove_empty
    },
    {
        "remove_unknown",
        Map_remove_unknown
    },
    {
        "grow",
        Map_grow
    },
    {
        "set_size_0",
        Map_set_size_0
    }
};

bake_test_case Sparse_testcases[] = {
    {
        "add_1",
        Sparse_add_1
    },
    {
        "add_1_to_empty",
        Sparse_add_1_to_empty
    },
    {
        "add_1_chunk_size_1",
        Sparse_add_1_chunk_size_1
    },
    {
        "add_n",
        Sparse_add_n
    },
    {
        "add_n_chunk_size_1",
        Sparse_add_n_chunk_size_1
    },
    {
        "remove",
        Sparse_remove
    },
    {
        "remove_first",
        Sparse_remove_first
    },
    {
        "remove_last",
        Sparse_remove_last
    },
    {
        "remove_all",
        Sparse_remove_all
    },
    {
        "remove_all_n_chunks",
        Sparse_remove_all_n_chunks
    },
    {
        "clear_1",
        Sparse_clear_1
    },
    {
        "clear_empty",
        Sparse_clear_empty
    },
    {
        "clear_n",
        Sparse_clear_n
    },
    {
        "clear_n_chunks",
        Sparse_clear_n_chunks
    },
    {
        "memory_null",
        Sparse_memory_null
    }
};

static bake_test_suite suites[] = {
    {
        "Vector",
        Vector_setup,
        NULL,
        22,
        Vector_testcases
    },
    {
        "Queue",
        Queue_setup,
        NULL,
        2,
        Queue_testcases
    },
    {
        "Map",
        Map_setup,
        NULL,
        17,
        Map_testcases
    },
    {
        "Sparse",
        Sparse_setup,
        NULL,
        15,
        Sparse_testcases
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("collections", argc, argv, suites, 4);
}
