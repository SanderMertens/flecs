
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <include/api.h>

// Testsuite 'New'
void New_empty(void);
void New_component(void);
void New_type(void);
void New_type_of_2(void);
void New_type_w_type(void);
void New_type_w_2_types(void);
void New_type_mixed(void);
void New_tag(void);
void New_type_w_tag(void);
void New_type_w_2_tags(void);
void New_type_w_tag_mixed(void);

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
void New_w_Count_tag(void);
void New_w_Count_type_w_tag(void);
void New_w_Count_type_w_2_tags(void);
void New_w_Count_type_w_tag_mixed(void);

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
void Add_tag(void);
void Add_type_w_tag(void);
void Add_type_w_2_tags(void);
void Add_type_w_tag_mixed(void);

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
void Has_has_in_progress(void);

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

// Testsuite 'Singleton'
void Singleton_set(void);
void Singleton_set_ptr(void);
void Singleton_system_w_singleton(void);
void Singleton_system_w_singleton_not_set(void);

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
void Clone_tag(void);
void Clone_tag_w_value(void);
void Clone_1_tag_1_component(void);
void Clone_1_tag_1_component_w_value(void);

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

// Testsuite 'SystemOnSet'
void SystemOnSet_set(void);
void SystemOnSet_set_new(void);
void SystemOnSet_set_again(void);
void SystemOnSet_clone(void);
void SystemOnSet_clone_w_value(void);
void SystemOnSet_set_w_optional(void);
void SystemOnSet_set_and_add_system(void);

// Testsuite 'SystemOnFrame'
void SystemOnFrame_1_type_1_component(void);
void SystemOnFrame_1_type_3_component(void);
void SystemOnFrame_3_type_1_component(void);
void SystemOnFrame_2_type_3_component(void);
void SystemOnFrame_1_type_1_component_1_tag(void);
void SystemOnFrame_2_type_1_component_1_tag(void);
void SystemOnFrame_2_type_1_and_1_not(void);
void SystemOnFrame_2_type_2_and_1_not(void);
void SystemOnFrame_2_type_2_and_2_not(void);
void SystemOnFrame_4_type_1_and_1_or(void);
void SystemOnFrame_4_type_1_and_1_or_of_3(void);
void SystemOnFrame_1_type_1_and_1_or(void);
void SystemOnFrame_2_type_1_and_1_optional(void);
void SystemOnFrame_2_type_2_and_1_optional(void);
void SystemOnFrame_6_type_1_and_2_optional(void);

// Testsuite 'SystemManual'
void SystemManual_1_type_1_component(void);

// Testsuite 'Container'
void Container_child(void);
void Container_child_w_id(void);
void Container_child_w_component(void);
void Container_child_w_type(void);
void Container_contains_w_empty_parent(void);
void Container_contains_w_child_as_parent(void);
void Container_contains_w_0_as_child(void);
void Container_contains_w_0_as_parent(void);
void Container_contains_w_0_for_both(void);
void Container_adopt(void);
void Container_adopt_again(void);
void Container_adopt_twice(void);
void Container_orphan_empty(void);
void Container_orphan(void);
void Container_orphan_again(void);
void Container_orphan_twice(void);
void Container_adopt_orphan(void);
void Container_adopt_2_orphan_1(void);
void Container_get_ptr_container(void);

// Testsuite 'Prefab'
void Prefab_new_w_prefab(void);
void Prefab_new_w_count_prefab(void);
void Prefab_new_w_type_w_prefab(void);
void Prefab_add_prefab(void);
void Prefab_add_type_w_prefab(void);
void Prefab_remove_prefab_after_new(void);
void Prefab_remove_prefab_after_add(void);
void Prefab_override_component(void);
void Prefab_override_remove_component(void);
void Prefab_override_2_of_3_components_1_self(void);
void Prefab_new_type_w_1_override(void);
void Prefab_new_type_w_2_overrides(void);
void Prefab_add_type_w_1_overrides(void);
void Prefab_add_type_w_2_overrides(void);
void Prefab_get_ptr_prefab(void);

// Testsuite 'System_w_FromContainer'
void System_w_FromContainer_1_column_from_container(void);
void System_w_FromContainer_2_column_1_from_container(void);
void System_w_FromContainer_3_column_2_from_container(void);
void System_w_FromContainer_3_column_2_from_different_container(void);
void System_w_FromContainer_2_column_1_from_container_w_not(void);
void System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not(void);
void System_w_FromContainer_2_column_1_from_container_w_or(void);

// Testsuite 'System_w_FromId'
void System_w_FromId_2_column_1_from_id(void);
void System_w_FromId_3_column_2_from_id(void);
void System_w_FromId_column_type(void);

// Testsuite 'System_w_FromSystem'
void System_w_FromSystem_2_column_1_from_system(void);
void System_w_FromSystem_3_column_2_from_system(void);
void System_w_FromSystem_2_column_1_from_system_on_add(void);
void System_w_FromSystem_2_column_1_from_system_on_remove(void);
void System_w_FromSystem_2_column_1_from_system_on_set(void);
void System_w_FromSystem_auto_add_tag(void);

// Testsuite 'Run'
void Run_run(void);
void Run_run_w_param(void);
void Run_run_no_match(void);
void Run_run_w_offset(void);
void Run_run_w_offset_skip_1_archetype(void);
void Run_run_w_offset_skip_1_archetype_plus_one(void);
void Run_run_w_offset_skip_2_archetypes(void);
void Run_run_w_limit_skip_1_archetype(void);
void Run_run_w_limit_skip_1_archetype_minus_one(void);
void Run_run_w_limit_skip_2_archetypes(void);
void Run_run_w_offset_1_limit_max(void);
void Run_run_w_offset_1_limit_minus_1(void);
void Run_run_w_offset_2_type_limit_max(void);
void Run_run_w_offset_2_type_limit_minus_1(void);
void Run_run_w_limit_1_all_offsets(void);
void Run_run_w_offset_out_of_bounds(void);
void Run_run_w_limit_out_of_bounds(void);
void Run_run_w_component_filter(void);
void Run_run_w_type_filter_of_2(void);
void Run_run_w_container_filter(void);

// Testsuite 'SingleThreadStaging'
void SingleThreadStaging_new_empty(void);
void SingleThreadStaging_new_w_component(void);
void SingleThreadStaging_new_w_type_of_2(void);
void SingleThreadStaging_add_to_new_empty(void);
void SingleThreadStaging_2_add_to_new_empty(void);
void SingleThreadStaging_add_remove_same_to_new_empty(void);
void SingleThreadStaging_add_remove_2_same_to_new_empty(void);
void SingleThreadStaging_add_remove_same_to_new_w_component(void);
void SingleThreadStaging_2_add_1_remove_to_new_empty(void);
void SingleThreadStaging_2_add_1_remove_same_to_new_empty(void);
void SingleThreadStaging_clone(void);
void SingleThreadStaging_clone_w_value(void);
void SingleThreadStaging_add_to_current(void);
void SingleThreadStaging_2_add_to_current(void);
void SingleThreadStaging_remove_from_current(void);
void SingleThreadStaging_remove_2_from_current(void);
void SingleThreadStaging_add_remove_same_to_current(void);
void SingleThreadStaging_add_remove_same_existing_to_current(void);
void SingleThreadStaging_remove_add_same_to_current(void);
void SingleThreadStaging_remove_add_same_existing_to_current(void);
void SingleThreadStaging_add_remove_2_same_to_current(void);
void SingleThreadStaging_add_remove_2_same_existing_to_current(void);
void SingleThreadStaging_remove_add_2_same_to_current(void);
void SingleThreadStaging_remove_add_2_same_existing_to_current(void);
void SingleThreadStaging_add_remove_different_to_current(void);
void SingleThreadStaging_2_add_1_remove_to_current(void);
void SingleThreadStaging_1_add_2_remove_to_current(void);
void SingleThreadStaging_delete_current(void);
void SingleThreadStaging_delete_even(void);
void SingleThreadStaging_delete_new_empty(void);
void SingleThreadStaging_delete_new_w_component(void);
void SingleThreadStaging_set_current(void);
void SingleThreadStaging_set_new_empty(void);
void SingleThreadStaging_set_new_w_component(void);
void SingleThreadStaging_set_existing_new_w_component(void);
void SingleThreadStaging_set_new_after_add(void);
void SingleThreadStaging_remove_after_set(void);
void SingleThreadStaging_delete_after_set(void);
void SingleThreadStaging_add_to_current_in_on_add(void);
void SingleThreadStaging_remove_from_current_in_on_add(void);
void SingleThreadStaging_add_to_current_in_on_set(void);
void SingleThreadStaging_remove_from_current_in_on_set(void);
void SingleThreadStaging_remove_set_component_in_on_set(void);
void SingleThreadStaging_remove_added_component_in_on_add(void);
void SingleThreadStaging_remove_added_component_in_on_add_w_set(void);
void SingleThreadStaging_on_add_in_on_add(void);
void SingleThreadStaging_on_remove_in_on_add(void);
void SingleThreadStaging_on_set_in_on_add(void);
void SingleThreadStaging_on_add_in_on_frame(void);
void SingleThreadStaging_on_remove_in_on_frame(void);
void SingleThreadStaging_match_table_created_in_progress(void);
void SingleThreadStaging_match_table_created_w_add_in_on_set(void);
void SingleThreadStaging_match_table_created_w_set_in_on_set(void);
void SingleThreadStaging_match_table_created_w_new_in_progress(void);
void SingleThreadStaging_match_table_created_w_new_in_on_set(void);
void SingleThreadStaging_merge_table_w_container_added_in_progress(void);

// Testsuite 'Modules'
void Modules_simple_module(void);
void Modules_import_module_from_system(void);
void Modules_import_from_on_add_system(void);
void Modules_import_from_on_set_system(void);

// Testsuite 'Internals'
void Internals_deactivate_table(void);
void Internals_activate_table(void);
void Internals_activate_deactivate_table(void);
void Internals_activate_deactivate_reactive(void);
void Internals_activate_deactivate_activate_other(void);

static bake_test_suite suites[] = {
    {
        .id = "New",
        .testcase_count = 11,
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
            },
            {
                .id = "tag",
                .function = New_tag
            },
            {
                .id = "type_w_tag",
                .function = New_type_w_tag
            },
            {
                .id = "type_w_2_tags",
                .function = New_type_w_2_tags
            },
            {
                .id = "type_w_tag_mixed",
                .function = New_type_w_tag_mixed
            }
        }
    },
    {
        .id = "New_w_Count",
        .testcase_count = 13,
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
            },
            {
                .id = "tag",
                .function = New_w_Count_tag
            },
            {
                .id = "type_w_tag",
                .function = New_w_Count_type_w_tag
            },
            {
                .id = "type_w_2_tags",
                .function = New_w_Count_type_w_2_tags
            },
            {
                .id = "type_w_tag_mixed",
                .function = New_w_Count_type_w_tag_mixed
            }
        }
    },
    {
        .id = "Add",
        .testcase_count = 25,
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
            },
            {
                .id = "tag",
                .function = Add_tag
            },
            {
                .id = "type_w_tag",
                .function = Add_type_w_tag
            },
            {
                .id = "type_w_2_tags",
                .function = Add_type_w_2_tags
            },
            {
                .id = "type_w_tag_mixed",
                .function = Add_type_w_tag_mixed
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
        .testcase_count = 18,
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
            },
            {
                .id = "has_in_progress",
                .function = Has_has_in_progress
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
        .id = "Singleton",
        .testcase_count = 4,
        .testcases = (bake_test_case[]){
            {
                .id = "set",
                .function = Singleton_set
            },
            {
                .id = "set_ptr",
                .function = Singleton_set_ptr
            },
            {
                .id = "system_w_singleton",
                .function = Singleton_system_w_singleton
            },
            {
                .id = "system_w_singleton_not_set",
                .function = Singleton_system_w_singleton_not_set
            }
        }
    },
    {
        .id = "Clone",
        .testcase_count = 14,
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
            },
            {
                .id = "tag",
                .function = Clone_tag
            },
            {
                .id = "tag_w_value",
                .function = Clone_tag_w_value
            },
            {
                .id = "1_tag_1_component",
                .function = Clone_1_tag_1_component
            },
            {
                .id = "1_tag_1_component_w_value",
                .function = Clone_1_tag_1_component_w_value
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
        .id = "SystemOnSet",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "set",
                .function = SystemOnSet_set
            },
            {
                .id = "set_new",
                .function = SystemOnSet_set_new
            },
            {
                .id = "set_again",
                .function = SystemOnSet_set_again
            },
            {
                .id = "clone",
                .function = SystemOnSet_clone
            },
            {
                .id = "clone_w_value",
                .function = SystemOnSet_clone_w_value
            },
            {
                .id = "set_w_optional",
                .function = SystemOnSet_set_w_optional
            },
            {
                .id = "set_and_add_system",
                .function = SystemOnSet_set_and_add_system
            }
        }
    },
    {
        .id = "SystemOnFrame",
        .testcase_count = 15,
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
                .id = "1_type_1_component_1_tag",
                .function = SystemOnFrame_1_type_1_component_1_tag
            },
            {
                .id = "2_type_1_component_1_tag",
                .function = SystemOnFrame_2_type_1_component_1_tag
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
            },
            {
                .id = "4_type_1_and_1_or",
                .function = SystemOnFrame_4_type_1_and_1_or
            },
            {
                .id = "4_type_1_and_1_or_of_3",
                .function = SystemOnFrame_4_type_1_and_1_or_of_3
            },
            {
                .id = "1_type_1_and_1_or",
                .function = SystemOnFrame_1_type_1_and_1_or
            },
            {
                .id = "2_type_1_and_1_optional",
                .function = SystemOnFrame_2_type_1_and_1_optional
            },
            {
                .id = "2_type_2_and_1_optional",
                .function = SystemOnFrame_2_type_2_and_1_optional
            },
            {
                .id = "6_type_1_and_2_optional",
                .function = SystemOnFrame_6_type_1_and_2_optional
            }
        }
    },
    {
        .id = "SystemManual",
        .testcase_count = 1,
        .testcases = (bake_test_case[]){
            {
                .id = "1_type_1_component",
                .function = SystemManual_1_type_1_component
            }
        }
    },
    {
        .id = "Container",
        .testcase_count = 19,
        .testcases = (bake_test_case[]){
            {
                .id = "child",
                .function = Container_child
            },
            {
                .id = "child_w_id",
                .function = Container_child_w_id
            },
            {
                .id = "child_w_component",
                .function = Container_child_w_component
            },
            {
                .id = "child_w_type",
                .function = Container_child_w_type
            },
            {
                .id = "contains_w_empty_parent",
                .function = Container_contains_w_empty_parent
            },
            {
                .id = "contains_w_child_as_parent",
                .function = Container_contains_w_child_as_parent
            },
            {
                .id = "contains_w_0_as_child",
                .function = Container_contains_w_0_as_child
            },
            {
                .id = "contains_w_0_as_parent",
                .function = Container_contains_w_0_as_parent
            },
            {
                .id = "contains_w_0_for_both",
                .function = Container_contains_w_0_for_both
            },
            {
                .id = "adopt",
                .function = Container_adopt
            },
            {
                .id = "adopt_again",
                .function = Container_adopt_again
            },
            {
                .id = "adopt_twice",
                .function = Container_adopt_twice
            },
            {
                .id = "orphan_empty",
                .function = Container_orphan_empty
            },
            {
                .id = "orphan",
                .function = Container_orphan
            },
            {
                .id = "orphan_again",
                .function = Container_orphan_again
            },
            {
                .id = "orphan_twice",
                .function = Container_orphan_twice
            },
            {
                .id = "adopt_orphan",
                .function = Container_adopt_orphan
            },
            {
                .id = "adopt_2_orphan_1",
                .function = Container_adopt_2_orphan_1
            },
            {
                .id = "get_ptr_container",
                .function = Container_get_ptr_container
            }
        }
    },
    {
        .id = "Prefab",
        .testcase_count = 15,
        .testcases = (bake_test_case[]){
            {
                .id = "new_w_prefab",
                .function = Prefab_new_w_prefab
            },
            {
                .id = "new_w_count_prefab",
                .function = Prefab_new_w_count_prefab
            },
            {
                .id = "new_w_type_w_prefab",
                .function = Prefab_new_w_type_w_prefab
            },
            {
                .id = "add_prefab",
                .function = Prefab_add_prefab
            },
            {
                .id = "add_type_w_prefab",
                .function = Prefab_add_type_w_prefab
            },
            {
                .id = "remove_prefab_after_new",
                .function = Prefab_remove_prefab_after_new
            },
            {
                .id = "remove_prefab_after_add",
                .function = Prefab_remove_prefab_after_add
            },
            {
                .id = "override_component",
                .function = Prefab_override_component
            },
            {
                .id = "override_remove_component",
                .function = Prefab_override_remove_component
            },
            {
                .id = "override_2_of_3_components_1_self",
                .function = Prefab_override_2_of_3_components_1_self
            },
            {
                .id = "new_type_w_1_override",
                .function = Prefab_new_type_w_1_override
            },
            {
                .id = "new_type_w_2_overrides",
                .function = Prefab_new_type_w_2_overrides
            },
            {
                .id = "add_type_w_1_overrides",
                .function = Prefab_add_type_w_1_overrides
            },
            {
                .id = "add_type_w_2_overrides",
                .function = Prefab_add_type_w_2_overrides
            },
            {
                .id = "get_ptr_prefab",
                .function = Prefab_get_ptr_prefab
            }
        }
    },
    {
        .id = "System_w_FromContainer",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "1_column_from_container",
                .function = System_w_FromContainer_1_column_from_container
            },
            {
                .id = "2_column_1_from_container",
                .function = System_w_FromContainer_2_column_1_from_container
            },
            {
                .id = "3_column_2_from_container",
                .function = System_w_FromContainer_3_column_2_from_container
            },
            {
                .id = "3_column_2_from_different_container",
                .function = System_w_FromContainer_3_column_2_from_different_container
            },
            {
                .id = "2_column_1_from_container_w_not",
                .function = System_w_FromContainer_2_column_1_from_container_w_not
            },
            {
                .id = "3_column_1_from_comtainer_1_from_container_w_not",
                .function = System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not
            },
            {
                .id = "2_column_1_from_container_w_or",
                .function = System_w_FromContainer_2_column_1_from_container_w_or
            }
        }
    },
    {
        .id = "System_w_FromId",
        .testcase_count = 3,
        .testcases = (bake_test_case[]){
            {
                .id = "2_column_1_from_id",
                .function = System_w_FromId_2_column_1_from_id
            },
            {
                .id = "3_column_2_from_id",
                .function = System_w_FromId_3_column_2_from_id
            },
            {
                .id = "column_type",
                .function = System_w_FromId_column_type
            }
        }
    },
    {
        .id = "System_w_FromSystem",
        .testcase_count = 6,
        .testcases = (bake_test_case[]){
            {
                .id = "2_column_1_from_system",
                .function = System_w_FromSystem_2_column_1_from_system
            },
            {
                .id = "3_column_2_from_system",
                .function = System_w_FromSystem_3_column_2_from_system
            },
            {
                .id = "2_column_1_from_system_on_add",
                .function = System_w_FromSystem_2_column_1_from_system_on_add
            },
            {
                .id = "2_column_1_from_system_on_remove",
                .function = System_w_FromSystem_2_column_1_from_system_on_remove
            },
            {
                .id = "2_column_1_from_system_on_set",
                .function = System_w_FromSystem_2_column_1_from_system_on_set
            },
            {
                .id = "auto_add_tag",
                .function = System_w_FromSystem_auto_add_tag
            }
        }
    },
    {
        .id = "Run",
        .testcase_count = 20,
        .testcases = (bake_test_case[]){
            {
                .id = "run",
                .function = Run_run
            },
            {
                .id = "run_w_param",
                .function = Run_run_w_param
            },
            {
                .id = "run_no_match",
                .function = Run_run_no_match
            },
            {
                .id = "run_w_offset",
                .function = Run_run_w_offset
            },
            {
                .id = "run_w_offset_skip_1_archetype",
                .function = Run_run_w_offset_skip_1_archetype
            },
            {
                .id = "run_w_offset_skip_1_archetype_plus_one",
                .function = Run_run_w_offset_skip_1_archetype_plus_one
            },
            {
                .id = "run_w_offset_skip_2_archetypes",
                .function = Run_run_w_offset_skip_2_archetypes
            },
            {
                .id = "run_w_limit_skip_1_archetype",
                .function = Run_run_w_limit_skip_1_archetype
            },
            {
                .id = "run_w_limit_skip_1_archetype_minus_one",
                .function = Run_run_w_limit_skip_1_archetype_minus_one
            },
            {
                .id = "run_w_limit_skip_2_archetypes",
                .function = Run_run_w_limit_skip_2_archetypes
            },
            {
                .id = "run_w_offset_1_limit_max",
                .function = Run_run_w_offset_1_limit_max
            },
            {
                .id = "run_w_offset_1_limit_minus_1",
                .function = Run_run_w_offset_1_limit_minus_1
            },
            {
                .id = "run_w_offset_2_type_limit_max",
                .function = Run_run_w_offset_2_type_limit_max
            },
            {
                .id = "run_w_offset_2_type_limit_minus_1",
                .function = Run_run_w_offset_2_type_limit_minus_1
            },
            {
                .id = "run_w_limit_1_all_offsets",
                .function = Run_run_w_limit_1_all_offsets
            },
            {
                .id = "run_w_offset_out_of_bounds",
                .function = Run_run_w_offset_out_of_bounds
            },
            {
                .id = "run_w_limit_out_of_bounds",
                .function = Run_run_w_limit_out_of_bounds
            },
            {
                .id = "run_w_component_filter",
                .function = Run_run_w_component_filter
            },
            {
                .id = "run_w_type_filter_of_2",
                .function = Run_run_w_type_filter_of_2
            },
            {
                .id = "run_w_container_filter",
                .function = Run_run_w_container_filter
            }
        }
    },
    {
        .id = "SingleThreadStaging",
        .testcase_count = 56,
        .testcases = (bake_test_case[]){
            {
                .id = "new_empty",
                .function = SingleThreadStaging_new_empty
            },
            {
                .id = "new_w_component",
                .function = SingleThreadStaging_new_w_component
            },
            {
                .id = "new_w_type_of_2",
                .function = SingleThreadStaging_new_w_type_of_2
            },
            {
                .id = "add_to_new_empty",
                .function = SingleThreadStaging_add_to_new_empty
            },
            {
                .id = "2_add_to_new_empty",
                .function = SingleThreadStaging_2_add_to_new_empty
            },
            {
                .id = "add_remove_same_to_new_empty",
                .function = SingleThreadStaging_add_remove_same_to_new_empty
            },
            {
                .id = "add_remove_2_same_to_new_empty",
                .function = SingleThreadStaging_add_remove_2_same_to_new_empty
            },
            {
                .id = "add_remove_same_to_new_w_component",
                .function = SingleThreadStaging_add_remove_same_to_new_w_component
            },
            {
                .id = "2_add_1_remove_to_new_empty",
                .function = SingleThreadStaging_2_add_1_remove_to_new_empty
            },
            {
                .id = "2_add_1_remove_same_to_new_empty",
                .function = SingleThreadStaging_2_add_1_remove_same_to_new_empty
            },
            {
                .id = "clone",
                .function = SingleThreadStaging_clone
            },
            {
                .id = "clone_w_value",
                .function = SingleThreadStaging_clone_w_value
            },
            {
                .id = "add_to_current",
                .function = SingleThreadStaging_add_to_current
            },
            {
                .id = "2_add_to_current",
                .function = SingleThreadStaging_2_add_to_current
            },
            {
                .id = "remove_from_current",
                .function = SingleThreadStaging_remove_from_current
            },
            {
                .id = "remove_2_from_current",
                .function = SingleThreadStaging_remove_2_from_current
            },
            {
                .id = "add_remove_same_to_current",
                .function = SingleThreadStaging_add_remove_same_to_current
            },
            {
                .id = "add_remove_same_existing_to_current",
                .function = SingleThreadStaging_add_remove_same_existing_to_current
            },
            {
                .id = "remove_add_same_to_current",
                .function = SingleThreadStaging_remove_add_same_to_current
            },
            {
                .id = "remove_add_same_existing_to_current",
                .function = SingleThreadStaging_remove_add_same_existing_to_current
            },
            {
                .id = "add_remove_2_same_to_current",
                .function = SingleThreadStaging_add_remove_2_same_to_current
            },
            {
                .id = "add_remove_2_same_existing_to_current",
                .function = SingleThreadStaging_add_remove_2_same_existing_to_current
            },
            {
                .id = "remove_add_2_same_to_current",
                .function = SingleThreadStaging_remove_add_2_same_to_current
            },
            {
                .id = "remove_add_2_same_existing_to_current",
                .function = SingleThreadStaging_remove_add_2_same_existing_to_current
            },
            {
                .id = "add_remove_different_to_current",
                .function = SingleThreadStaging_add_remove_different_to_current
            },
            {
                .id = "2_add_1_remove_to_current",
                .function = SingleThreadStaging_2_add_1_remove_to_current
            },
            {
                .id = "1_add_2_remove_to_current",
                .function = SingleThreadStaging_1_add_2_remove_to_current
            },
            {
                .id = "delete_current",
                .function = SingleThreadStaging_delete_current
            },
            {
                .id = "delete_even",
                .function = SingleThreadStaging_delete_even
            },
            {
                .id = "delete_new_empty",
                .function = SingleThreadStaging_delete_new_empty
            },
            {
                .id = "delete_new_w_component",
                .function = SingleThreadStaging_delete_new_w_component
            },
            {
                .id = "set_current",
                .function = SingleThreadStaging_set_current
            },
            {
                .id = "set_new_empty",
                .function = SingleThreadStaging_set_new_empty
            },
            {
                .id = "set_new_w_component",
                .function = SingleThreadStaging_set_new_w_component
            },
            {
                .id = "set_existing_new_w_component",
                .function = SingleThreadStaging_set_existing_new_w_component
            },
            {
                .id = "set_new_after_add",
                .function = SingleThreadStaging_set_new_after_add
            },
            {
                .id = "remove_after_set",
                .function = SingleThreadStaging_remove_after_set
            },
            {
                .id = "delete_after_set",
                .function = SingleThreadStaging_delete_after_set
            },
            {
                .id = "add_to_current_in_on_add",
                .function = SingleThreadStaging_add_to_current_in_on_add
            },
            {
                .id = "remove_from_current_in_on_add",
                .function = SingleThreadStaging_remove_from_current_in_on_add
            },
            {
                .id = "add_to_current_in_on_set",
                .function = SingleThreadStaging_add_to_current_in_on_set
            },
            {
                .id = "remove_from_current_in_on_set",
                .function = SingleThreadStaging_remove_from_current_in_on_set
            },
            {
                .id = "remove_set_component_in_on_set",
                .function = SingleThreadStaging_remove_set_component_in_on_set
            },
            {
                .id = "remove_added_component_in_on_add",
                .function = SingleThreadStaging_remove_added_component_in_on_add
            },
            {
                .id = "remove_added_component_in_on_add_w_set",
                .function = SingleThreadStaging_remove_added_component_in_on_add_w_set
            },
            {
                .id = "on_add_in_on_add",
                .function = SingleThreadStaging_on_add_in_on_add
            },
            {
                .id = "on_remove_in_on_add",
                .function = SingleThreadStaging_on_remove_in_on_add
            },
            {
                .id = "on_set_in_on_add",
                .function = SingleThreadStaging_on_set_in_on_add
            },
            {
                .id = "on_add_in_on_frame",
                .function = SingleThreadStaging_on_add_in_on_frame
            },
            {
                .id = "on_remove_in_on_frame",
                .function = SingleThreadStaging_on_remove_in_on_frame
            },
            {
                .id = "match_table_created_in_progress",
                .function = SingleThreadStaging_match_table_created_in_progress
            },
            {
                .id = "match_table_created_w_add_in_on_set",
                .function = SingleThreadStaging_match_table_created_w_add_in_on_set
            },
            {
                .id = "match_table_created_w_set_in_on_set",
                .function = SingleThreadStaging_match_table_created_w_set_in_on_set
            },
            {
                .id = "match_table_created_w_new_in_progress",
                .function = SingleThreadStaging_match_table_created_w_new_in_progress
            },
            {
                .id = "match_table_created_w_new_in_on_set",
                .function = SingleThreadStaging_match_table_created_w_new_in_on_set
            },
            {
                .id = "merge_table_w_container_added_in_progress",
                .function = SingleThreadStaging_merge_table_w_container_added_in_progress
            }
        }
    },
    {
        .id = "Modules",
        .testcase_count = 4,
        .testcases = (bake_test_case[]){
            {
                .id = "simple_module",
                .function = Modules_simple_module
            },
            {
                .id = "import_module_from_system",
                .function = Modules_import_module_from_system
            },
            {
                .id = "import_from_on_add_system",
                .function = Modules_import_from_on_add_system
            },
            {
                .id = "import_from_on_set_system",
                .function = Modules_import_from_on_set_system
            }
        }
    },
    {
        .id = "Internals",
        .testcase_count = 5,
        .testcases = (bake_test_case[]){
            {
                .id = "deactivate_table",
                .function = Internals_deactivate_table
            },
            {
                .id = "activate_table",
                .function = Internals_activate_table
            },
            {
                .id = "activate_deactivate_table",
                .function = Internals_activate_deactivate_table
            },
            {
                .id = "activate_deactivate_reactive",
                .function = Internals_activate_deactivate_reactive
            },
            {
                .id = "activate_deactivate_activate_other",
                .function = Internals_activate_deactivate_activate_other
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("api", argc, argv, suites, 23);
}
