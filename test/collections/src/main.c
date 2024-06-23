
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <collections.h>

// Testsuite 'Map'
void Map_setup(void);
void Map_count(void);
void Map_count_empty(void);
void Map_set_overwrite(void);
void Map_set_rehash(void);
void Map_get(void);
void Map_get_all(void);
void Map_get_empty(void);
void Map_get_unknown(void);
void Map_get_0_from_empty(void);
void Map_get_0_from_populated(void);
void Map_get_0_after_insert(void);
void Map_get_0_after_ensure(void);
void Map_iter(void);
void Map_iter_empty(void);
void Map_iter_null(void);
void Map_remove(void);
void Map_remove_empty(void);
void Map_remove_unknown(void);
void Map_remove_twice(void);
void Map_clear_empty(void);
void Map_clear_populated(void);
void Map_clear_empty_twice(void);
void Map_clear_populated_twice(void);
void Map_populate_after_clear(void);
void Map_randomized_insert(void);
void Map_randomized_remove(void);
void Map_randomized_insert_large(void);
void Map_randomized_remove_large(void);
void Map_randomized_after_clear(void);

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
void Sparse_create_delete(void);
void Sparse_create_delete_2(void);
void Sparse_count_of_null(void);
void Sparse_try_low_after_ensure_high(void);
void Sparse_is_alive_low_after_ensure_high(void);
void Sparse_remove_low_after_ensure_high(void);
void Sparse_ensure_skip_generation(void);

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
void Strbuf_reset(void);
void Strbuf_merge(void);
void Strbuf_merge_empty(void);
void Strbuf_append_char(void);
void Strbuf_append_511_chars(void);
void Strbuf_append_512_chars(void);
void Strbuf_append_513_chars(void);
void Strbuf_append_1023_chars(void);
void Strbuf_append_1024_chars(void);
void Strbuf_append_1025_chars(void);
void Strbuf_append_2047_chars(void);
void Strbuf_append_2048_chars(void);
void Strbuf_append_2049_chars(void);
void Strbuf_append_511_str(void);
void Strbuf_append_512_str(void);
void Strbuf_append_513_str(void);
void Strbuf_append_1023_str(void);
void Strbuf_append_1024_str(void);
void Strbuf_append_1025_str(void);
void Strbuf_append_2047_str(void);
void Strbuf_append_2048_str(void);
void Strbuf_append_2049_str(void);
void Strbuf_append_flt(void);
void Strbuf_append_nan(void);
void Strbuf_append_inf(void);
void Strbuf_append_nan_delim(void);
void Strbuf_append_inf_delim(void);

// Testsuite 'Allocator'
void Allocator_setup(void);
void Allocator_init_fini_empty(void);

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
        "get_0_from_empty",
        Map_get_0_from_empty
    },
    {
        "get_0_from_populated",
        Map_get_0_from_populated
    },
    {
        "get_0_after_insert",
        Map_get_0_after_insert
    },
    {
        "get_0_after_ensure",
        Map_get_0_after_ensure
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
        "remove_twice",
        Map_remove_twice
    },
    {
        "clear_empty",
        Map_clear_empty
    },
    {
        "clear_populated",
        Map_clear_populated
    },
    {
        "clear_empty_twice",
        Map_clear_empty_twice
    },
    {
        "clear_populated_twice",
        Map_clear_populated_twice
    },
    {
        "populate_after_clear",
        Map_populate_after_clear
    },
    {
        "randomized_insert",
        Map_randomized_insert
    },
    {
        "randomized_remove",
        Map_randomized_remove
    },
    {
        "randomized_insert_large",
        Map_randomized_insert_large
    },
    {
        "randomized_remove_large",
        Map_randomized_remove_large
    },
    {
        "randomized_after_clear",
        Map_randomized_after_clear
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
        "try_low_after_ensure_high",
        Sparse_try_low_after_ensure_high
    },
    {
        "is_alive_low_after_ensure_high",
        Sparse_is_alive_low_after_ensure_high
    },
    {
        "remove_low_after_ensure_high",
        Sparse_remove_low_after_ensure_high
    },
    {
        "ensure_skip_generation",
        Sparse_ensure_skip_generation
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
        "reset",
        Strbuf_reset
    },
    {
        "merge",
        Strbuf_merge
    },
    {
        "merge_empty",
        Strbuf_merge_empty
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
        "append_1023_chars",
        Strbuf_append_1023_chars
    },
    {
        "append_1024_chars",
        Strbuf_append_1024_chars
    },
    {
        "append_1025_chars",
        Strbuf_append_1025_chars
    },
    {
        "append_2047_chars",
        Strbuf_append_2047_chars
    },
    {
        "append_2048_chars",
        Strbuf_append_2048_chars
    },
    {
        "append_2049_chars",
        Strbuf_append_2049_chars
    },
    {
        "append_511_str",
        Strbuf_append_511_str
    },
    {
        "append_512_str",
        Strbuf_append_512_str
    },
    {
        "append_513_str",
        Strbuf_append_513_str
    },
    {
        "append_1023_str",
        Strbuf_append_1023_str
    },
    {
        "append_1024_str",
        Strbuf_append_1024_str
    },
    {
        "append_1025_str",
        Strbuf_append_1025_str
    },
    {
        "append_2047_str",
        Strbuf_append_2047_str
    },
    {
        "append_2048_str",
        Strbuf_append_2048_str
    },
    {
        "append_2049_str",
        Strbuf_append_2049_str
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

bake_test_case Allocator_testcases[] = {
    {
        "init_fini_empty",
        Allocator_init_fini_empty
    }
};


static bake_test_suite suites[] = {
    {
        "Map",
        Map_setup,
        NULL,
        29,
        Map_testcases
    },
    {
        "Sparse",
        Sparse_setup,
        NULL,
        22,
        Sparse_testcases
    },
    {
        "Strbuf",
        Strbuf_setup,
        NULL,
        35,
        Strbuf_testcases
    },
    {
        "Allocator",
        Allocator_setup,
        NULL,
        1,
        Allocator_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("collections", argc, argv, suites, 4);
}
