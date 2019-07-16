
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
void Vector_remove_index_w_move(void);
void Vector_set_size_smaller_than_count(void);
void Vector_pop_elements(void);

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

static bake_test_suite suites[] = {
    {
        .id = "Vector",
        .testcase_count = 23,
        .setup = Vector_setup,
        .testcases = (bake_test_case[]){
            {
                .id = "free_empty",
                .function = Vector_free_empty
            },
            {
                .id = "count",
                .function = Vector_count
            },
            {
                .id = "count_empty",
                .function = Vector_count_empty
            },
            {
                .id = "get",
                .function = Vector_get
            },
            {
                .id = "get_first",
                .function = Vector_get_first
            },
            {
                .id = "get_last",
                .function = Vector_get_last
            },
            {
                .id = "get_empty",
                .function = Vector_get_empty
            },
            {
                .id = "get_out_of_bound",
                .function = Vector_get_out_of_bound
            },
            {
                .id = "add_empty",
                .function = Vector_add_empty
            },
            {
                .id = "add_resize",
                .function = Vector_add_resize
            },
            {
                .id = "remove",
                .function = Vector_remove
            },
            {
                .id = "remove_first",
                .function = Vector_remove_first
            },
            {
                .id = "remove_last",
                .function = Vector_remove_last
            },
            {
                .id = "remove_empty",
                .function = Vector_remove_empty
            },
            {
                .id = "remove_all",
                .function = Vector_remove_all
            },
            {
                .id = "remove_out_of_bound",
                .function = Vector_remove_out_of_bound
            },
            {
                .id = "sort_rnd",
                .function = Vector_sort_rnd
            },
            {
                .id = "sort_sorted",
                .function = Vector_sort_sorted
            },
            {
                .id = "sort_empty",
                .function = Vector_sort_empty
            },
            {
                .id = "size_of_null",
                .function = Vector_size_of_null
            },
            {
                .id = "remove_index_w_move",
                .function = Vector_remove_index_w_move
            },
            {
                .id = "set_size_smaller_than_count",
                .function = Vector_set_size_smaller_than_count
            },
            {
                .id = "pop_elements",
                .function = Vector_pop_elements
            }
        }
    },
    {
        .id = "Map",
        .testcase_count = 15,
        .setup = Map_setup,
        .testcases = (bake_test_case[]){
            {
                .id = "count",
                .function = Map_count
            },
            {
                .id = "count_empty",
                .function = Map_count_empty
            },
            {
                .id = "set_overwrite",
                .function = Map_set_overwrite
            },
            {
                .id = "set_rehash",
                .function = Map_set_rehash
            },
            {
                .id = "set_zero_buckets",
                .function = Map_set_zero_buckets
            },
            {
                .id = "get",
                .function = Map_get
            },
            {
                .id = "get_all",
                .function = Map_get_all
            },
            {
                .id = "get_empty",
                .function = Map_get_empty
            },
            {
                .id = "get_unknown",
                .function = Map_get_unknown
            },
            {
                .id = "iter",
                .function = Map_iter
            },
            {
                .id = "iter_empty",
                .function = Map_iter_empty
            },
            {
                .id = "iter_zero_buckets",
                .function = Map_iter_zero_buckets
            },
            {
                .id = "remove",
                .function = Map_remove
            },
            {
                .id = "remove_empty",
                .function = Map_remove_empty
            },
            {
                .id = "remove_unknown",
                .function = Map_remove_unknown
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("collections", argc, argv, suites, 2);
}
