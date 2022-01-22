
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
void Vector_get_last_from_empty(void);
void Vector_get_last_from_null(void);
void Vector_get_empty(void);
void Vector_get_out_of_bound(void);
void Vector_add_empty(void);
void Vector_add_resize(void);
void Vector_sort_rnd(void);
void Vector_sort_sorted(void);
void Vector_sort_empty(void);
void Vector_sort_null(void);
void Vector_size_of_null(void);
void Vector_set_size_smaller_than_count(void);
void Vector_pop_elements(void);
void Vector_pop_null(void);
void Vector_reclaim(void);
void Vector_grow(void);
void Vector_copy(void);
void Vector_copy_null(void);
void Vector_memory(void);
void Vector_memory_from_null(void);
void Vector_addn_to_null(void);
void Vector_addn_to_0_size(void);
void Vector_set_min_count(void);
void Vector_set_min_size(void);
void Vector_set_min_size_to_smaller(void);

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
void Map_iter_null(void);
void Map_remove(void);
void Map_remove_empty(void);
void Map_remove_unknown(void);
void Map_grow(void);
void Map_set_size_0(void);
void Map_ensure(void);

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
void Sparse_add_after_clear(void);
void Sparse_memory_null(void);
void Sparse_copy(void);
void Sparse_restore(void);
void Sparse_create_delete(void);
void Sparse_create_delete_2(void);
void Sparse_count_of_null(void);
void Sparse_size_of_null(void);
void Sparse_copy_null(void);

// Testsuite 'Strbuf'
void Strbuf_setup(void);
void Strbuf_append(void);
void Strbuf_appendstr(void);
void Strbuf_appendstrn(void);
void Strbuf_appendstr_null(void);
void Strbuf_append_list(void);
void Strbuf_append_nested_list(void);
void Strbuf_large_str(void);
void Strbuf_empty_str(void);
void Strbuf_append_zerocopy(void);
void Strbuf_append_zerocopy_only(void);
void Strbuf_append_zerocopy_const(void);
void Strbuf_reset(void);
void Strbuf_merge(void);
void Strbuf_app_buffer(void);
void Strbuf_append_char(void);
void Strbuf_append_511_chars(void);
void Strbuf_append_512_chars(void);
void Strbuf_append_513_chars(void);
void Strbuf_append_flt(void);
void Strbuf_append_nan(void);
void Strbuf_append_inf(void);
void Strbuf_append_nan_delim(void);
void Strbuf_append_inf_delim(void);

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
        "get_last_from_empty",
        Vector_get_last_from_empty
    },
    {
        "get_last_from_null",
        Vector_get_last_from_null
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
        "sort_null",
        Vector_sort_null
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
    },
    {
        "pop_null",
        Vector_pop_null
    },
    {
        "reclaim",
        Vector_reclaim
    },
    {
        "grow",
        Vector_grow
    },
    {
        "copy",
        Vector_copy
    },
    {
        "copy_null",
        Vector_copy_null
    },
    {
        "memory",
        Vector_memory
    },
    {
        "memory_from_null",
        Vector_memory_from_null
    },
    {
        "addn_to_null",
        Vector_addn_to_null
    },
    {
        "addn_to_0_size",
        Vector_addn_to_0_size
    },
    {
        "set_min_count",
        Vector_set_min_count
    },
    {
        "set_min_size",
        Vector_set_min_size
    },
    {
        "set_min_size_to_smaller",
        Vector_set_min_size_to_smaller
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
        "iter_null",
        Map_iter_null
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
    },
    {
        "ensure",
        Map_ensure
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
        "add_after_clear",
        Sparse_add_after_clear
    },
    {
        "memory_null",
        Sparse_memory_null
    },
    {
        "copy",
        Sparse_copy
    },
    {
        "restore",
        Sparse_restore
    },
    {
        "create_delete",
        Sparse_create_delete
    },
    {
        "create_delete_2",
        Sparse_create_delete_2
    },
    {
        "count_of_null",
        Sparse_count_of_null
    },
    {
        "size_of_null",
        Sparse_size_of_null
    },
    {
        "copy_null",
        Sparse_copy_null
    }
};

bake_test_case Strbuf_testcases[] = {
    {
        "append",
        Strbuf_append
    },
    {
        "appendstr",
        Strbuf_appendstr
    },
    {
        "appendstrn",
        Strbuf_appendstrn
    },
    {
        "appendstr_null",
        Strbuf_appendstr_null
    },
    {
        "append_list",
        Strbuf_append_list
    },
    {
        "append_nested_list",
        Strbuf_append_nested_list
    },
    {
        "large_str",
        Strbuf_large_str
    },
    {
        "empty_str",
        Strbuf_empty_str
    },
    {
        "append_zerocopy",
        Strbuf_append_zerocopy
    },
    {
        "append_zerocopy_only",
        Strbuf_append_zerocopy_only
    },
    {
        "append_zerocopy_const",
        Strbuf_append_zerocopy_const
    },
    {
        "reset",
        Strbuf_reset
    },
    {
        "merge",
        Strbuf_merge
    },
    {
        "app_buffer",
        Strbuf_app_buffer
    },
    {
        "append_char",
        Strbuf_append_char
    },
    {
        "append_511_chars",
        Strbuf_append_511_chars
    },
    {
        "append_512_chars",
        Strbuf_append_512_chars
    },
    {
        "append_513_chars",
        Strbuf_append_513_chars
    },
    {
        "append_flt",
        Strbuf_append_flt
    },
    {
        "append_nan",
        Strbuf_append_nan
    },
    {
        "append_inf",
        Strbuf_append_inf
    },
    {
        "append_nan_delim",
        Strbuf_append_nan_delim
    },
    {
        "append_inf_delim",
        Strbuf_append_inf_delim
    }
};

static bake_test_suite suites[] = {
    {
        "Vector",
        Vector_setup,
        NULL,
        31,
        Vector_testcases
    },
    {
        "Map",
        Map_setup,
        NULL,
        19,
        Map_testcases
    },
    {
        "Sparse",
        Sparse_setup,
        NULL,
        23,
        Sparse_testcases
    },
    {
        "Strbuf",
        Strbuf_setup,
        NULL,
        23,
        Strbuf_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("collections", argc, argv, suites, 4);
}
