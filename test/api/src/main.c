#include <include/api.h>

// Testsuite 'New'
void New_empty(void);
void New_component(void);
void New_type(void);
void New_type_of_2(void);
void New_type_w_type(void);
void New_type_w_2_types(void);
void New_type_mixed(void);

// Testsuite 'New_w_Count'
void New_w_Count_empty(void);
void New_w_Count_empty_w_handles(void);
void New_w_Count_nonempty_w_handles(void);
void New_w_Count_component(void);
void New_w_Count_type(void);
void New_w_Count_type_of_2(void);
void New_w_Count_type_w_type(void);
void New_w_Count_type_w_2_types(void);
void New_w_Count_type_mixed(void);

// Testsuite 'Add'
void Add_zero(void);
void Add_component(void);
void Add_component_again(void);
void Add_2_components(void);
void Add_2_components_again(void);
void Add_2_components_overlap(void);
void Add_type(void);
void Add_type_of_2(void);
void Add_type_w_type(void);
void Add_type_w_2_types(void);
void Add_type_mixed(void);
void Add_type_again(void);
void Add_type_overlap(void);
void Add_type_again_component(void);
void Add_type_overlap_component(void);
void Add_component_to_nonempty(void);
void Add_component_to_nonempty_again(void);
void Add_component_to_nonempty_overlap(void);
void Add_type_to_nonempty(void);
void Add_type_to_nonempty_again(void);
void Add_type_to_nonempty_overlap(void);

// Testsuite 'Remove'
void Remove_zero(void);
void Remove_zero_from_nonzero(void);
void Remove_1_of_1(void);
void Remove_1_of_1_again(void);
void Remove_1_of_2(void);
void Remove_2_of_2(void);
void Remove_2_of_3(void);
void Remove_2_again(void);
void Remove_2_overlap(void);
void Remove_type_of_1_of_2(void);
void Remove_type_of_2_of_2(void);
void Remove_type_of_2_of_3(void);
void Remove_1_from_empty(void);
void Remove_type_from_empty(void);
void Remove_not_added(void);

// Testsuite 'Has'
void Has_zero(void);
void Has_zero_from_nonzero(void);
void Has_1_of_0(void);
void Has_1_of_1(void);
void Has_1_of_2(void);
void Has_2_of_0(void);
void Has_2_of_2(void);
void Has_3_of_2(void);
void Has_2_of_1(void);
void Has_1_of_empty(void);
void Has_any_of_1_of_1(void);
void Has_any_of_1_of_2(void);
void Has_any_of_2_of_3(void);
void Has_any_of_3_of_2(void);
void Has_any_of_2_of_1(void);
void Has_any_of_1_of_0(void);
void Has_any_2_of_2_disjunct(void);

// Testsuite 'Delete'
void Delete_delete_1(void);
void Delete_delete_1_again(void);
void Delete_delete_empty(void);
void Delete_delete_nonexist(void);
void Delete_delete_1st_of_3(void);
void Delete_delete_2nd_of_3(void);
void Delete_delete_2_of_3(void);
void Delete_delete_3_of_3(void);

// Testsuite 'Set'
void Set_set_empty(void);
void Set_set_nonempty(void);
void Set_set_non_empty_override(void);
void Set_set_again(void);
void Set_set_2(void);
void Set_add_set(void);
void Set_set_add(void);
void Set_set_add_other(void);
void Set_set_remove(void);
void Set_set_remove_other(void);
void Set_set_remove_twice(void);
void Set_set_and_new(void);

// Testsuite 'Clone'
void Clone_empty(void);
void Clone_empty_w_value(void);
void Clone_null(void);
void Clone_null_w_value(void);
void Clone_1_component(void);
void Clone_2_component(void);
void Clone_1_component_w_value(void);
void Clone_2_component_w_value(void);
void Clone_3_component(void);
void Clone_3_component_w_value(void);

// Testsuite 'SystemOnAdd'
void SystemOnAdd_new_match_1_of_1(void);
void SystemOnAdd_new_match_1_of_2(void);
void SystemOnAdd_new_no_match_1(void);
void SystemOnAdd_new_no_match_2_of_1(void);
void SystemOnAdd_new_no_match_2_of_3(void);
void SystemOnAdd_new_w_count_match_1_of_1(void);
void SystemOnAdd_add_match_1_of_1(void);
void SystemOnAdd_add_match_1_of_2(void);
void SystemOnAdd_add_match_2_of_2(void);
void SystemOnAdd_add_match_2_of_3(void);
void SystemOnAdd_add_no_match_1(void);
void SystemOnAdd_add_no_match_2_of_1(void);
void SystemOnAdd_add_no_match_2_of_3(void);
void SystemOnAdd_set_match_1_of_1(void);
void SystemOnAdd_set_no_match_1(void);
void SystemOnAdd_clone_match_1_of_1(void);
void SystemOnAdd_clone_match_1_of_2(void);
void SystemOnAdd_clone_match_2_of_2(void);
void SystemOnAdd_clone_match_2_of_3(void);
void SystemOnAdd_add_again_1(void);
void SystemOnAdd_set_again_1(void);
void SystemOnAdd_add_again_2(void);

// Testsuite 'SystemOnRemove'
void SystemOnRemove_remove_match_1_of_1(void);
void SystemOnRemove_remove_match_1_of_2(void);
void SystemOnRemove_remove_match_2_of_2(void);
void SystemOnRemove_remove_match_2_of_3(void);
void SystemOnRemove_remove_no_match_1(void);
void SystemOnRemove_remove_no_match_2_of_1(void);
void SystemOnRemove_remove_no_match_2_of_3(void);
void SystemOnRemove_delete_match_1_of_1(void);
void SystemOnRemove_delete_match_1_of_2(void);
void SystemOnRemove_delete_match_2_of_2(void);
void SystemOnRemove_delete_match_2_of_3(void);
void SystemOnRemove_delete_no_match_1(void);
void SystemOnRemove_delete_no_match_2_of_1(void);
void SystemOnRemove_delete_no_match_2_of_3(void);

// Testsuite 'SystemOnFrame'
void SystemOnFrame_1_type_1_component(void);
void SystemOnFrame_1_type_3_component(void);
void SystemOnFrame_3_type_1_component(void);
void SystemOnFrame_2_type_3_component(void);
void SystemOnFrame_2_type_1_and_1_not(void);
void SystemOnFrame_2_type_2_and_1_not(void);
void SystemOnFrame_2_type_2_and_2_not(void);

static bake_test_suite suites[] = {
    {
        .id = "New",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "empty",
                .function = New_empty
            },
            {
                .id = "component",
                .function = New_component
            },
            {
                .id = "type",
                .function = New_type
            },
            {
                .id = "type_of_2",
                .function = New_type_of_2
            },
            {
                .id = "type_w_type",
                .function = New_type_w_type
            },
            {
                .id = "type_w_2_types",
                .function = New_type_w_2_types
            },
            {
                .id = "type_mixed",
                .function = New_type_mixed
            }
        }
    },
    {
        .id = "New_w_Count",
        .testcase_count = 9,
        .testcases = (bake_test_case[]){
            {
                .id = "empty",
                .function = New_w_Count_empty
            },
            {
                .id = "empty_w_handles",
                .function = New_w_Count_empty_w_handles
            },
            {
                .id = "nonempty_w_handles",
                .function = New_w_Count_nonempty_w_handles
            },
            {
                .id = "component",
                .function = New_w_Count_component
            },
            {
                .id = "type",
                .function = New_w_Count_type
            },
            {
                .id = "type_of_2",
                .function = New_w_Count_type_of_2
            },
            {
                .id = "type_w_type",
                .function = New_w_Count_type_w_type
            },
            {
                .id = "type_w_2_types",
                .function = New_w_Count_type_w_2_types
            },
            {
                .id = "type_mixed",
                .function = New_w_Count_type_mixed
            }
        }
    },
    {
        .id = "Add",
        .testcase_count = 21,
        .testcases = (bake_test_case[]){
            {
                .id = "zero",
                .function = Add_zero
            },
            {
                .id = "component",
                .function = Add_component
            },
            {
                .id = "component_again",
                .function = Add_component_again
            },
            {
                .id = "2_components",
                .function = Add_2_components
            },
            {
                .id = "2_components_again",
                .function = Add_2_components_again
            },
            {
                .id = "2_components_overlap",
                .function = Add_2_components_overlap
            },
            {
                .id = "type",
                .function = Add_type
            },
            {
                .id = "type_of_2",
                .function = Add_type_of_2
            },
            {
                .id = "type_w_type",
                .function = Add_type_w_type
            },
            {
                .id = "type_w_2_types",
                .function = Add_type_w_2_types
            },
            {
                .id = "type_mixed",
                .function = Add_type_mixed
            },
            {
                .id = "type_again",
                .function = Add_type_again
            },
            {
                .id = "type_overlap",
                .function = Add_type_overlap
            },
            {
                .id = "type_again_component",
                .function = Add_type_again_component
            },
            {
                .id = "type_overlap_component",
                .function = Add_type_overlap_component
            },
            {
                .id = "component_to_nonempty",
                .function = Add_component_to_nonempty
            },
            {
                .id = "component_to_nonempty_again",
                .function = Add_component_to_nonempty_again
            },
            {
                .id = "component_to_nonempty_overlap",
                .function = Add_component_to_nonempty_overlap
            },
            {
                .id = "type_to_nonempty",
                .function = Add_type_to_nonempty
            },
            {
                .id = "type_to_nonempty_again",
                .function = Add_type_to_nonempty_again
            },
            {
                .id = "type_to_nonempty_overlap",
                .function = Add_type_to_nonempty_overlap
            }
        }
    },
    {
        .id = "Remove",
        .testcase_count = 15,
        .testcases = (bake_test_case[]){
            {
                .id = "zero",
                .function = Remove_zero
            },
            {
                .id = "zero_from_nonzero",
                .function = Remove_zero_from_nonzero
            },
            {
                .id = "1_of_1",
                .function = Remove_1_of_1
            },
            {
                .id = "1_of_1_again",
                .function = Remove_1_of_1_again
            },
            {
                .id = "1_of_2",
                .function = Remove_1_of_2
            },
            {
                .id = "2_of_2",
                .function = Remove_2_of_2
            },
            {
                .id = "2_of_3",
                .function = Remove_2_of_3
            },
            {
                .id = "2_again",
                .function = Remove_2_again
            },
            {
                .id = "2_overlap",
                .function = Remove_2_overlap
            },
            {
                .id = "type_of_1_of_2",
                .function = Remove_type_of_1_of_2
            },
            {
                .id = "type_of_2_of_2",
                .function = Remove_type_of_2_of_2
            },
            {
                .id = "type_of_2_of_3",
                .function = Remove_type_of_2_of_3
            },
            {
                .id = "1_from_empty",
                .function = Remove_1_from_empty
            },
            {
                .id = "type_from_empty",
                .function = Remove_type_from_empty
            },
            {
                .id = "not_added",
                .function = Remove_not_added
            }
        }
    },
    {
        .id = "Has",
        .testcase_count = 17,
        .testcases = (bake_test_case[]){
            {
                .id = "zero",
                .function = Has_zero
            },
            {
                .id = "zero_from_nonzero",
                .function = Has_zero_from_nonzero
            },
            {
                .id = "1_of_0",
                .function = Has_1_of_0
            },
            {
                .id = "1_of_1",
                .function = Has_1_of_1
            },
            {
                .id = "1_of_2",
                .function = Has_1_of_2
            },
            {
                .id = "2_of_0",
                .function = Has_2_of_0
            },
            {
                .id = "2_of_2",
                .function = Has_2_of_2
            },
            {
                .id = "3_of_2",
                .function = Has_3_of_2
            },
            {
                .id = "2_of_1",
                .function = Has_2_of_1
            },
            {
                .id = "1_of_empty",
                .function = Has_1_of_empty
            },
            {
                .id = "any_of_1_of_1",
                .function = Has_any_of_1_of_1
            },
            {
                .id = "any_of_1_of_2",
                .function = Has_any_of_1_of_2
            },
            {
                .id = "any_of_2_of_3",
                .function = Has_any_of_2_of_3
            },
            {
                .id = "any_of_3_of_2",
                .function = Has_any_of_3_of_2
            },
            {
                .id = "any_of_2_of_1",
                .function = Has_any_of_2_of_1
            },
            {
                .id = "any_of_1_of_0",
                .function = Has_any_of_1_of_0
            },
            {
                .id = "any_2_of_2_disjunct",
                .function = Has_any_2_of_2_disjunct
            }
        }
    },
    {
        .id = "Delete",
        .testcase_count = 8,
        .testcases = (bake_test_case[]){
            {
                .id = "delete_1",
                .function = Delete_delete_1
            },
            {
                .id = "delete_1_again",
                .function = Delete_delete_1_again
            },
            {
                .id = "delete_empty",
                .function = Delete_delete_empty
            },
            {
                .id = "delete_nonexist",
                .function = Delete_delete_nonexist
            },
            {
                .id = "delete_1st_of_3",
                .function = Delete_delete_1st_of_3
            },
            {
                .id = "delete_2nd_of_3",
                .function = Delete_delete_2nd_of_3
            },
            {
                .id = "delete_2_of_3",
                .function = Delete_delete_2_of_3
            },
            {
                .id = "delete_3_of_3",
                .function = Delete_delete_3_of_3
            }
        }
    },
    {
        .id = "Set",
        .testcase_count = 12,
        .testcases = (bake_test_case[]){
            {
                .id = "set_empty",
                .function = Set_set_empty
            },
            {
                .id = "set_nonempty",
                .function = Set_set_nonempty
            },
            {
                .id = "set_non_empty_override",
                .function = Set_set_non_empty_override
            },
            {
                .id = "set_again",
                .function = Set_set_again
            },
            {
                .id = "set_2",
                .function = Set_set_2
            },
            {
                .id = "add_set",
                .function = Set_add_set
            },
            {
                .id = "set_add",
                .function = Set_set_add
            },
            {
                .id = "set_add_other",
                .function = Set_set_add_other
            },
            {
                .id = "set_remove",
                .function = Set_set_remove
            },
            {
                .id = "set_remove_other",
                .function = Set_set_remove_other
            },
            {
                .id = "set_remove_twice",
                .function = Set_set_remove_twice
            },
            {
                .id = "set_and_new",
                .function = Set_set_and_new
            }
        }
    },
    {
        .id = "Clone",
        .testcase_count = 10,
        .testcases = (bake_test_case[]){
            {
                .id = "empty",
                .function = Clone_empty
            },
            {
                .id = "empty_w_value",
                .function = Clone_empty_w_value
            },
            {
                .id = "null",
                .function = Clone_null
            },
            {
                .id = "null_w_value",
                .function = Clone_null_w_value
            },
            {
                .id = "1_component",
                .function = Clone_1_component
            },
            {
                .id = "2_component",
                .function = Clone_2_component
            },
            {
                .id = "1_component_w_value",
                .function = Clone_1_component_w_value
            },
            {
                .id = "2_component_w_value",
                .function = Clone_2_component_w_value
            },
            {
                .id = "3_component",
                .function = Clone_3_component
            },
            {
                .id = "3_component_w_value",
                .function = Clone_3_component_w_value
            }
        }
    },
    {
        .id = "SystemOnAdd",
        .testcase_count = 22,
        .testcases = (bake_test_case[]){
            {
                .id = "new_match_1_of_1",
                .function = SystemOnAdd_new_match_1_of_1
            },
            {
                .id = "new_match_1_of_2",
                .function = SystemOnAdd_new_match_1_of_2
            },
            {
                .id = "new_no_match_1",
                .function = SystemOnAdd_new_no_match_1
            },
            {
                .id = "new_no_match_2_of_1",
                .function = SystemOnAdd_new_no_match_2_of_1
            },
            {
                .id = "new_no_match_2_of_3",
                .function = SystemOnAdd_new_no_match_2_of_3
            },
            {
                .id = "new_w_count_match_1_of_1",
                .function = SystemOnAdd_new_w_count_match_1_of_1
            },
            {
                .id = "add_match_1_of_1",
                .function = SystemOnAdd_add_match_1_of_1
            },
            {
                .id = "add_match_1_of_2",
                .function = SystemOnAdd_add_match_1_of_2
            },
            {
                .id = "add_match_2_of_2",
                .function = SystemOnAdd_add_match_2_of_2
            },
            {
                .id = "add_match_2_of_3",
                .function = SystemOnAdd_add_match_2_of_3
            },
            {
                .id = "add_no_match_1",
                .function = SystemOnAdd_add_no_match_1
            },
            {
                .id = "add_no_match_2_of_1",
                .function = SystemOnAdd_add_no_match_2_of_1
            },
            {
                .id = "add_no_match_2_of_3",
                .function = SystemOnAdd_add_no_match_2_of_3
            },
            {
                .id = "set_match_1_of_1",
                .function = SystemOnAdd_set_match_1_of_1
            },
            {
                .id = "set_no_match_1",
                .function = SystemOnAdd_set_no_match_1
            },
            {
                .id = "clone_match_1_of_1",
                .function = SystemOnAdd_clone_match_1_of_1
            },
            {
                .id = "clone_match_1_of_2",
                .function = SystemOnAdd_clone_match_1_of_2
            },
            {
                .id = "clone_match_2_of_2",
                .function = SystemOnAdd_clone_match_2_of_2
            },
            {
                .id = "clone_match_2_of_3",
                .function = SystemOnAdd_clone_match_2_of_3
            },
            {
                .id = "add_again_1",
                .function = SystemOnAdd_add_again_1
            },
            {
                .id = "set_again_1",
                .function = SystemOnAdd_set_again_1
            },
            {
                .id = "add_again_2",
                .function = SystemOnAdd_add_again_2
            }
        }
    },
    {
        .id = "SystemOnRemove",
        .testcase_count = 14,
        .testcases = (bake_test_case[]){
            {
                .id = "remove_match_1_of_1",
                .function = SystemOnRemove_remove_match_1_of_1
            },
            {
                .id = "remove_match_1_of_2",
                .function = SystemOnRemove_remove_match_1_of_2
            },
            {
                .id = "remove_match_2_of_2",
                .function = SystemOnRemove_remove_match_2_of_2
            },
            {
                .id = "remove_match_2_of_3",
                .function = SystemOnRemove_remove_match_2_of_3
            },
            {
                .id = "remove_no_match_1",
                .function = SystemOnRemove_remove_no_match_1
            },
            {
                .id = "remove_no_match_2_of_1",
                .function = SystemOnRemove_remove_no_match_2_of_1
            },
            {
                .id = "remove_no_match_2_of_3",
                .function = SystemOnRemove_remove_no_match_2_of_3
            },
            {
                .id = "delete_match_1_of_1",
                .function = SystemOnRemove_delete_match_1_of_1
            },
            {
                .id = "delete_match_1_of_2",
                .function = SystemOnRemove_delete_match_1_of_2
            },
            {
                .id = "delete_match_2_of_2",
                .function = SystemOnRemove_delete_match_2_of_2
            },
            {
                .id = "delete_match_2_of_3",
                .function = SystemOnRemove_delete_match_2_of_3
            },
            {
                .id = "delete_no_match_1",
                .function = SystemOnRemove_delete_no_match_1
            },
            {
                .id = "delete_no_match_2_of_1",
                .function = SystemOnRemove_delete_no_match_2_of_1
            },
            {
                .id = "delete_no_match_2_of_3",
                .function = SystemOnRemove_delete_no_match_2_of_3
            }
        }
    },
    {
        .id = "SystemOnFrame",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "1_type_1_component",
                .function = SystemOnFrame_1_type_1_component
            },
            {
                .id = "1_type_3_component",
                .function = SystemOnFrame_1_type_3_component
            },
            {
                .id = "3_type_1_component",
                .function = SystemOnFrame_3_type_1_component
            },
            {
                .id = "2_type_3_component",
                .function = SystemOnFrame_2_type_3_component
            },
            {
                .id = "2_type_1_and_1_not",
                .function = SystemOnFrame_2_type_1_and_1_not
            },
            {
                .id = "2_type_2_and_1_not",
                .function = SystemOnFrame_2_type_2_and_1_not
            },
            {
                .id = "2_type_2_and_2_not",
                .function = SystemOnFrame_2_type_2_and_2_not
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("api", argc, argv, suites, 11);
}
