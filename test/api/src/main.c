
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <api.h>

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
void New_redefine_component(void);

// Testsuite 'New_w_Count'
void New_w_Count_empty(void);
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

// Testsuite 'Set_w_data'
void Set_w_data_1_column_3_rows(void);
void Set_w_data_2_columns_3_rows(void);
void Set_w_data_1_column_3_rows_w_entities(void);
void Set_w_data_2_columns_3_rows_w_entities(void);
void Set_w_data_1_column_3_rows_overwrite_entities(void);
void Set_w_data_2_columns_3_rows_overwrite_entities(void);
void Set_w_data_overwrite_different_order(void);
void Set_w_data_overwrite_different_type(void);
void Set_w_data_2_columns_3_rows_no_data(void);
void Set_w_data_2_columns_3_rows_1_tag(void);
void Set_w_data_2_columns_3_rows_1_parent(void);
void Set_w_data_2_columns_3_rows_1_base(void);
void Set_w_data_2_columns_3_rows_1_base_w_entities(void);
void Set_w_data_2_columns_3_rows_1_nested_prefab(void);
void Set_w_data_2_columns_3_rows_component_order(void);
void Set_w_data_overwrite_w_unset_column(void);
void Set_w_data_overwrite_from_other_type_w_unset_column(void);
void Set_w_data_staged_1_column_3_rows(void);
void Set_w_data_staged_1_column_3_rows_w_entities(void);
void Set_w_data_staged_1_column_3_rows_w_entities_w_base(void);
void Set_w_data_staged_set_existing_in_stage(void);
void Set_w_data_on_add(void);
void Set_w_data_on_add_w_entities(void);
void Set_w_data_on_add_w_entities_again(void);
void Set_w_data_on_set(void);
void Set_w_data_on_set_w_entities(void);
void Set_w_data_on_set_no_data_after_on_add(void);
void Set_w_data_on_remove_existing(void);
void Set_w_data_on_add_different_origin(void);
void Set_w_data_on_add_different_overlapping_origin(void);
void Set_w_data_on_add_different_overlapping_origin_reorder(void);
void Set_w_data_on_set_different_origin(void);
void Set_w_data_on_remove_different_origin(void);
void Set_w_data_existing_different_type_out_of_order(void);

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
void Add_add_remove(void);
void Add_add_remove_same(void);
void Add_add_2_remove(void);
void Add_on_add_after_new_type_in_progress(void);
void Add_add_entity(void);
void Add_remove_entity(void);

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

// Testsuite 'Add_remove_w_filter'
void Add_remove_w_filter_remove_1_no_filter(void);
void Add_remove_w_filter_remove_2_no_filter(void);
void Add_remove_w_filter_remove_1_to_empty_no_filter(void);
void Add_remove_w_filter_remove_2_to_empty_no_filter(void);
void Add_remove_w_filter_remove_1_all_no_filter(void);
void Add_remove_w_filter_remove_2_all_no_filter(void);
void Add_remove_w_filter_remove_1_exclude_1(void);
void Add_remove_w_filter_remove_1_exclude_2(void);
void Add_remove_w_filter_remove_1_include_1(void);
void Add_remove_w_filter_remove_1_include_2(void);
void Add_remove_w_filter_add_1(void);
void Add_remove_w_filter_add_2(void);

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
void Has_any_of_0_0(void);
void Has_any_of_0_1(void);
void Has_any_of_0_zero_entity(void);
void Has_has_in_progress(void);
void Has_has_of_zero(void);
void Has_has_owned(void);
void Has_has_owned_any(void);
void Has_has_entity(void);
void Has_has_entity_0(void);
void Has_has_entity_0_component(void);
void Has_has_entity_owned(void);
void Has_has_entity_owned_0(void);
void Has_has_entity_owned_0_component(void);

// Testsuite 'Count'
void Count_count_empty(void);
void Count_count_1_component(void);
void Count_count_2_components(void);
void Count_count_3_components(void);
void Count_count_2_types_2_comps(void);

// Testsuite 'Get_component'
void Get_component_get_empty(void);
void Get_component_get_1_from_1(void);
void Get_component_get_1_from_2(void);
void Get_component_get_2_from_2(void);
void Get_component_get_2_from_3(void);
void Get_component_get_1_from_2_in_progress_from_main_stage(void);
void Get_component_get_1_from_2_add_in_progress(void);
void Get_component_get_both_from_2_add_in_progress(void);
void Get_component_get_both_from_2_add_remove_in_progress(void);

// Testsuite 'Delete'
void Delete_delete_1(void);
void Delete_delete_1_again(void);
void Delete_delete_empty(void);
void Delete_delete_nonexist(void);
void Delete_delete_1st_of_3(void);
void Delete_delete_2nd_of_3(void);
void Delete_delete_2_of_3(void);
void Delete_delete_3_of_3(void);
void Delete_delete_w_on_remove(void);

// Testsuite 'Delete_w_filter'
void Delete_w_filter_delete_1(void);
void Delete_w_filter_delete_2(void);
void Delete_w_filter_delete_1_2_types(void);
void Delete_w_filter_delete_2_2_types(void);
void Delete_w_filter_delete_except_1(void);
void Delete_w_filter_delete_except_2(void);
void Delete_w_filter_delete_with_any_of_2(void);
void Delete_w_filter_delete_except_all_of_2(void);
void Delete_w_filter_include_exact(void);
void Delete_w_filter_exclude_exact(void);
void Delete_w_filter_system_activate_test(void);
void Delete_w_filter_skip_builtin_tables(void);

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
void Set_set_null(void);

// Testsuite 'Lookup'
void Lookup_lookup(void);
void Lookup_lookup_component(void);
void Lookup_lookup_not_found(void);
void Lookup_lookup_child(void);
void Lookup_lookup_child_w_component(void);
void Lookup_lookup_in_progress(void);
void Lookup_lookup_child_in_progress(void);
void Lookup_lookup_w_null_id(void);
void Lookup_get_id(void);
void Lookup_get_id_no_id(void);
void Lookup_get_id_from_empty(void);

// Testsuite 'Singleton'
void Singleton_set(void);
void Singleton_set_ptr(void);
void Singleton_system_w_singleton(void);
void Singleton_system_w_singleton_no_match(void);
void Singleton_system_w_not_singleton(void);
void Singleton_lookup_singleton(void);
void Singleton_get_singleton_id(void);

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

// Testsuite 'SystemMisc'
void SystemMisc_invalid_not_without_id(void);
void SystemMisc_invalid_optional_without_id(void);
void SystemMisc_invalid_system_without_id(void);
void SystemMisc_invalid_container_without_id(void);
void SystemMisc_invalid_cascade_without_id(void);
void SystemMisc_invalid_entity_without_id(void);
void SystemMisc_invalid_empty_without_id(void);
void SystemMisc_invalid_singleton_without_id(void);
void SystemMisc_invalid_empty_element(void);
void SystemMisc_invalid_empty_element_w_space(void);
void SystemMisc_invalid_empty_or(void);
void SystemMisc_invalid_empty_or_w_space(void);
void SystemMisc_invalid_or_w_not(void);
void SystemMisc_invalid_not_w_or(void);
void SystemMisc_invalid_0_w_and(void);
void SystemMisc_invalid_0_w_from_system(void);
void SystemMisc_invalid_0_w_from_container(void);
void SystemMisc_invalid_0_w_from_cascade(void);
void SystemMisc_invalid_0_w_from_entity(void);
void SystemMisc_invalid_0_w_from_empty(void);
void SystemMisc_invalid_0_w_from_singleton(void);
void SystemMisc_invalid_or_w_empty(void);
void SystemMisc_invalid_component_id(void);
void SystemMisc_invalid_entity_id(void);
void SystemMisc_invalid_or_from_system(void);
void SystemMisc_invalid_phase(void);
void SystemMisc_invalid_null_string(void);
void SystemMisc_invalid_empty_string(void);
void SystemMisc_invalid_empty_string_w_space(void);
void SystemMisc_redefine_row_system(void);
void SystemMisc_system_w_or_prefab(void);
void SystemMisc_system_w_or_disabled(void);
void SystemMisc_system_w_or_disabled_and_prefab(void);
void SystemMisc_table_columns_access(void);
void SystemMisc_status_enable_after_new(void);
void SystemMisc_status_enable_after_disable(void);
void SystemMisc_status_disable_after_new(void);
void SystemMisc_status_disable_after_disable(void);
void SystemMisc_status_activate_after_new(void);
void SystemMisc_status_deactivate_after_delete(void);

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
void SystemOnAdd_override_after_add_in_on_add(void);
void SystemOnAdd_set_after_add_in_on_add(void);
void SystemOnAdd_add_again_in_progress(void);
void SystemOnAdd_add_in_progress_before_system_def(void);
void SystemOnAdd_disabled_system(void);
void SystemOnAdd_2_systems_w_table_creation(void);
void SystemOnAdd_2_systems_w_table_creation_in_progress(void);
void SystemOnAdd_sys_context(void);
void SystemOnAdd_get_sys_context_from_param(void);

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
void SystemOnRemove_disabled_system(void);

// Testsuite 'SystemOnSet'
void SystemOnSet_set(void);
void SystemOnSet_set_new(void);
void SystemOnSet_set_again(void);
void SystemOnSet_clone(void);
void SystemOnSet_clone_w_value(void);
void SystemOnSet_set_w_optional(void);
void SystemOnSet_set_and_add_system(void);
void SystemOnSet_on_set_after_on_add(void);
void SystemOnSet_on_set_after_on_add_w_new(void);
void SystemOnSet_on_set_after_on_add_w_new_w_count(void);
void SystemOnSet_on_set_after_on_add_1_of_2_matched(void);
void SystemOnSet_on_set_after_override(void);
void SystemOnSet_on_set_after_override_w_new(void);
void SystemOnSet_on_set_after_override_w_new_w_count(void);
void SystemOnSet_on_set_after_override_1_of_2_overridden(void);
void SystemOnSet_disabled_system(void);

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
void SystemOnFrame_ensure_optional_is_unset_column(void);
void SystemOnFrame_ensure_optional_is_null_shared(void);
void SystemOnFrame_ensure_optional_is_null_field_owned(void);
void SystemOnFrame_ensure_optional_is_null_field_shared(void);
void SystemOnFrame_use_fields_2_owned(void);
void SystemOnFrame_use_fields_1_owned_1_shared(void);
void SystemOnFrame_match_2_systems_w_populated_table(void);
void SystemOnFrame_on_period(void);
void SystemOnFrame_on_period_long_delta(void);
void SystemOnFrame_disabled(void);
void SystemOnFrame_disabled_feature(void);
void SystemOnFrame_disabled_nested_feature(void);
void SystemOnFrame_two_refs(void);
void SystemOnFrame_filter_disabled(void);
void SystemOnFrame_match_disabled(void);
void SystemOnFrame_match_disabled_and_enabled(void);
void SystemOnFrame_match_prefab(void);
void SystemOnFrame_match_prefab_and_normal(void);
void SystemOnFrame_is_shared_on_column_not_set(void);
void SystemOnFrame_owned_column(void);
void SystemOnFrame_owned_not_column(void);
void SystemOnFrame_shared_column(void);
void SystemOnFrame_shared_not_column(void);
void SystemOnFrame_container_dont_match_inheritance(void);
void SystemOnFrame_cascade_dont_match_inheritance(void);
void SystemOnFrame_not_from_singleton(void);
void SystemOnFrame_not_from_entity(void);
void SystemOnFrame_sys_context(void);
void SystemOnFrame_get_sys_context_from_param(void);
void SystemOnFrame_use_field_w_0_size(void);
void SystemOnFrame_owned_only(void);
void SystemOnFrame_shared_only(void);
void SystemOnFrame_is_in_readonly(void);

// Testsuite 'SystemOnDemand'
void SystemOnDemand_enable_out_after_in(void);
void SystemOnDemand_enable_in_after_out(void);
void SystemOnDemand_enable_out_after_in_2_out_1_in(void);
void SystemOnDemand_enable_out_after_in_1_out_2_in(void);
void SystemOnDemand_enable_in_after_out_2_out_1_in(void);
void SystemOnDemand_enable_in_after_out_1_out_2_in(void);
void SystemOnDemand_disable_after_disable_in(void);
void SystemOnDemand_disable_after_disable_in_2_out_1_in(void);
void SystemOnDemand_disable_after_disable_in_1_out_2_in(void);
void SystemOnDemand_table_after_out(void);
void SystemOnDemand_table_after_out_and_in(void);
void SystemOnDemand_table_after_out_and_in_overlapping_columns(void);
void SystemOnDemand_1_out_system_2_in_systems(void);
void SystemOnDemand_1_out_system_2_in_systems_different_columns(void);
void SystemOnDemand_1_out_system_2_in_systems_overlapping_columns(void);
void SystemOnDemand_disable_after_inactive_in_system(void);
void SystemOnDemand_disable_after_2_inactive_in_systems(void);
void SystemOnDemand_disable_after_2_inactive_in_systems_different_columns(void);
void SystemOnDemand_enable_2_output_1_input_system(void);
void SystemOnDemand_enable_2_output_1_input_system_different_columns(void);
void SystemOnDemand_enable_2_output_1_input_system_overlapping_columns(void);
void SystemOnDemand_out_not_column(void);
void SystemOnDemand_trigger_on_manual(void);
void SystemOnDemand_trigger_on_manual_not_column(void);
void SystemOnDemand_on_demand_task_w_from_entity(void);
void SystemOnDemand_on_demand_task_w_not_from_entity(void);

// Testsuite 'SystemCascade'
void SystemCascade_cascade_depth_1(void);
void SystemCascade_cascade_depth_2(void);
void SystemCascade_add_after_match(void);
void SystemCascade_adopt_after_match(void);

// Testsuite 'SystemManual'
void SystemManual_1_type_1_component(void);
void SystemManual_disabled(void);
void SystemManual_activate_status(void);

// Testsuite 'Tasks'
void Tasks_no_components(void);
void Tasks_one_tag(void);
void Tasks_from_system(void);
void Tasks_on_remove_no_components(void);
void Tasks_on_remove_one_tag(void);
void Tasks_on_remove_from_system(void);
void Tasks_tasks_in_phases(void);

// Testsuite 'Container'
void Container_child(void);
void Container_child_w_component(void);
void Container_child_w_type(void);
void Container_child_w_type_w_childof(void);
void Container_child_w_count_component(void);
void Container_child_w_count_w_type(void);
void Container_child_w_count_type_w_childof(void);
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
void Container_get_parent(void);
void Container_get_parent_no_matching_comp(void);
void Container_get_parent_two_parents(void);
void Container_get_parent_no_parent(void);
void Container_singleton_as_container(void);

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
void Prefab_iterate_w_prefab_field(void);
void Prefab_iterate_w_prefab_shared(void);
void Prefab_match_entity_prefab_w_system_optional(void);
void Prefab_prefab_in_system_expr(void);
void Prefab_dont_match_prefab(void);
void Prefab_new_w_count_w_override(void);
void Prefab_override_2_components_different_size(void);
void Prefab_ignore_prefab_parent_component(void);
void Prefab_match_table_created_in_progress(void);
void Prefab_prefab_w_1_child(void);
void Prefab_prefab_w_2_children(void);
void Prefab_prefab_w_grandchild(void);
void Prefab_prefab_w_base_w_child(void);
void Prefab_prefab_w_child_w_base(void);
void Prefab_prefab_w_child_w_base_w_children(void);
void Prefab_prefab_w_child_new_w_count(void);
void Prefab_prefab_auto_override_child_component(void);
void Prefab_ignore_on_add(void);
void Prefab_ignore_on_remove(void);
void Prefab_ignore_on_set(void);
void Prefab_on_set_on_instance(void);
void Prefab_instantiate_in_progress(void);
void Prefab_copy_from_prefab_in_progress(void);
void Prefab_copy_from_prefab_first_instance_in_progress(void);
void Prefab_cached_ptr_after_realloc(void);
void Prefab_revalidate_cached_ptr_w_mixed_table_refs(void);
void Prefab_no_overwrite_on_2nd_add(void);
void Prefab_no_overwrite_on_2nd_add_in_progress(void);
void Prefab_no_instantiate_on_2nd_add(void);
void Prefab_no_instantiate_on_2nd_add_in_progress(void);
void Prefab_nested_prefab_in_progress_w_count(void);
void Prefab_nested_prefab_in_progress_w_count_set_after_override(void);
void Prefab_get_ptr_from_prefab_from_new_table_in_progress(void);
void Prefab_match_base(void);
void Prefab_match_base_after_add_in_prev_phase(void);
void Prefab_override_watched_prefab(void);
void Prefab_rematch_twice(void);
void Prefab_inherit_in_system(void);
void Prefab_add_to_empty_base_in_system(void);
void Prefab_dont_inherit_disabled(void);
void Prefab_cyclic_inheritance(void);
void Prefab_cyclic_inheritance_w_clone_in_progress(void);
void Prefab_cyclic_inheritance_get_unavailable(void);
void Prefab_cyclic_inheritance_has_unavailable(void);
void Prefab_clone_after_inherit_in_on_add(void);
void Prefab_override_from_nested(void);
void Prefab_create_multiple_nested_w_on_add(void);
void Prefab_create_multiple_nested_w_on_add_in_progress(void);

// Testsuite 'System_w_FromContainer'
void System_w_FromContainer_1_column_from_container(void);
void System_w_FromContainer_2_column_1_from_container(void);
void System_w_FromContainer_3_column_2_from_container(void);
void System_w_FromContainer_3_column_2_from_different_container(void);
void System_w_FromContainer_2_column_1_from_container_w_not(void);
void System_w_FromContainer_2_column_1_from_container_w_not_prefab(void);
void System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not(void);
void System_w_FromContainer_2_column_1_from_container_w_or(void);
void System_w_FromContainer_select_same_from_container(void);
void System_w_FromContainer_add_component_after_match(void);
void System_w_FromContainer_add_component_after_match_and_rematch(void);
void System_w_FromContainer_add_component_after_match_unmatch(void);
void System_w_FromContainer_add_component_after_match_unmatch_match(void);
void System_w_FromContainer_add_component_after_match_2_systems(void);
void System_w_FromContainer_add_component_in_progress_after_match(void);
void System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr(void);
void System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr_in_progress(void);
void System_w_FromContainer_adopt_after_match(void);
void System_w_FromContainer_new_child_after_match(void);
void System_w_FromContainer_realloc_after_match(void);

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

// Testsuite 'System_w_FromEntity'
void System_w_FromEntity_2_column_1_from_entity(void);
void System_w_FromEntity_task_from_entity(void);
void System_w_FromEntity_task_not_from_entity(void);

// Testsuite 'World'
void World_progress_w_0(void);
void World_progress_w_t(void);
void World_get_tick(void);
void World_entity_range_offset(void);
void World_entity_range_offset_out_of_range(void);
void World_entity_range_limit_out_of_range(void);
void World_entity_range_add_existing_in_progress(void);
void World_entity_range_add_in_range_in_progress(void);
void World_entity_range_add_out_of_range_in_progress(void);
void World_entity_range_out_of_range_check_disabled(void);
void World_dim(void);
void World_dim_type(void);
void World_dim_dim_type(void);
void World_phases(void);
void World_phases_w_merging(void);
void World_phases_match_in_create(void);
void World_measure_time(void);
void World_control_fps(void);
void World_control_fps_busy_system(void);
void World_control_fps_busy_app(void);
void World_control_fps_random_system(void);
void World_control_fps_random_app(void);
void World_world_stats(void);
void World_quit(void);
void World_get_delta_time(void);
void World_get_delta_time_auto(void);
void World_recreate_world(void);
void World_init_w_args_set_threads(void);
void World_init_w_args_set_fps(void);
void World_init_w_args_enable_dbg(void);
void World_no_threading(void);
void World_no_time(void);
void World_is_entity_enabled(void);

// Testsuite 'Type'
void Type_type_of_1_tostr(void);
void Type_type_of_2_tostr(void);
void Type_type_of_2_tostr_no_id(void);
void Type_type_redefine(void);
void Type_type_from_array(void);
void Type_type_from_array_unsorted(void);
void Type_type_has(void);
void Type_type_has_not(void);
void Type_zero_type_has_not(void);
void Type_type_has_prefab(void);
void Type_type_has_container(void);
void Type_type_has_prefab_container(void);
void Type_type_merge(void);
void Type_type_merge_overlap(void);
void Type_type_merge_overlap_one(void);
void Type_type_merge_overlap_w_flag(void);
void Type_type_merge_overlap_w_flags_from_both(void);
void Type_type_add(void);
void Type_type_add_empty(void);
void Type_type_add_entity_again(void);
void Type_type_add_out_of_order(void);
void Type_type_add_existing(void);
void Type_type_add_empty_existing(void);
void Type_type_add_out_of_order_existing(void);
void Type_type_remove(void);
void Type_type_remove_empty(void);
void Type_type_remove_non_existing(void);
void Type_type_of_2_add(void);
void Type_type_of_3_add_entity_again(void);
void Type_invalid_entity_type_expression(void);
void Type_invalid_singleton_type_expression(void);
void Type_invalid_container_type_expression(void);
void Type_invalid_system_type_expression(void);
void Type_type_from_empty_entity(void);
void Type_type_from_empty_entity_in_progress(void);
void Type_get_type(void);
void Type_get_type_from_empty(void);
void Type_get_type_from_0(void);
void Type_entity_from_type(void);
void Type_entity_from_empty_type(void);
void Type_entity_from_type_w_2_elements(void);
void Type_type_from_entity(void);
void Type_type_from_empty(void);
void Type_type_from_0(void);
void Type_type_to_expr_1_comp(void);
void Type_type_to_expr_2_comp(void);
void Type_type_to_expr_instanceof(void);
void Type_type_to_expr_childof(void);

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
void Run_run_comb_10_entities_1_type(void);
void Run_run_comb_10_entities_2_types(void);
void Run_run_w_interrupt(void);
void Run_run_staging(void);

// Testsuite 'MultiThread'
void MultiThread_2_thread_1_entity(void);
void MultiThread_2_thread_2_entity(void);
void MultiThread_2_thread_5_entity(void);
void MultiThread_2_thread_10_entity(void);
void MultiThread_3_thread_1_entity(void);
void MultiThread_3_thread_2_entity(void);
void MultiThread_3_thread_5_entity(void);
void MultiThread_3_thread_10_entity(void);
void MultiThread_4_thread_1_entity(void);
void MultiThread_4_thread_2_entity(void);
void MultiThread_4_thread_5_entity(void);
void MultiThread_4_thread_10_entity(void);
void MultiThread_5_thread_1_entity(void);
void MultiThread_5_thread_2_entity(void);
void MultiThread_5_thread_5_entity(void);
void MultiThread_5_thread_10_entity(void);
void MultiThread_6_thread_1_entity(void);
void MultiThread_6_thread_2_entity(void);
void MultiThread_6_thread_5_entity(void);
void MultiThread_6_thread_10_entity(void);
void MultiThread_2_thread_test_combs_100_entity(void);
void MultiThread_3_thread_test_combs_100_entity(void);
void MultiThread_4_thread_test_combs_100_entity(void);
void MultiThread_5_thread_test_combs_100_entity(void);
void MultiThread_6_thread_test_combs_100_entity(void);
void MultiThread_2_thread_test_combs_100_entity_2_types(void);
void MultiThread_3_thread_test_combs_100_entity_2_types(void);
void MultiThread_4_thread_test_combs_100_entity_2_types(void);
void MultiThread_5_thread_test_combs_100_entity_2_types(void);
void MultiThread_6_thread_test_combs_100_entity_2_types(void);
void MultiThread_change_thread_count(void);
void MultiThread_multithread_quit(void);
void MultiThread_schedule_w_tasks(void);
void MultiThread_reactive_system(void);

// Testsuite 'SingleThreadStaging'
void SingleThreadStaging_new_empty(void);
void SingleThreadStaging_new_w_component(void);
void SingleThreadStaging_new_w_type_of_2(void);
void SingleThreadStaging_new_empty_w_count(void);
void SingleThreadStaging_new_component_w_count(void);
void SingleThreadStaging_new_type_w_count(void);
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
void SingleThreadStaging_add_remove_add_same_to_current(void);
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
void SingleThreadStaging_on_add_in_on_update(void);
void SingleThreadStaging_on_remove_in_on_update(void);
void SingleThreadStaging_match_table_created_in_progress(void);
void SingleThreadStaging_match_table_created_w_add_in_on_set(void);
void SingleThreadStaging_match_table_created_w_set_in_on_set(void);
void SingleThreadStaging_match_table_created_w_new_in_progress(void);
void SingleThreadStaging_match_table_created_w_new_in_on_set(void);
void SingleThreadStaging_merge_table_w_container_added_in_progress(void);
void SingleThreadStaging_merge_table_w_container_added_on_set(void);
void SingleThreadStaging_merge_table_w_container_added_on_set_reverse(void);
void SingleThreadStaging_merge_after_tasks(void);
void SingleThreadStaging_override_after_remove_in_progress(void);
void SingleThreadStaging_get_parent_in_progress(void);

// Testsuite 'MultiThreadStaging'
void MultiThreadStaging_2_threads_add_to_current(void);
void MultiThreadStaging_3_threads_add_to_current(void);
void MultiThreadStaging_4_threads_add_to_current(void);
void MultiThreadStaging_5_threads_add_to_current(void);
void MultiThreadStaging_6_threads_add_to_current(void);
void MultiThreadStaging_stress_create_delete_entity_random_components(void);
void MultiThreadStaging_stress_set_entity_random_components(void);
void MultiThreadStaging_2_threads_on_add(void);
void MultiThreadStaging_new_w_count(void);

// Testsuite 'Snapshot'
void Snapshot_simple_snapshot(void);
void Snapshot_snapshot_after_new(void);
void Snapshot_snapshot_after_delete(void);
void Snapshot_snapshot_after_new_type(void);
void Snapshot_snapshot_after_add(void);
void Snapshot_snapshot_after_remove(void);
void Snapshot_snapshot_w_include_filter(void);
void Snapshot_snapshot_w_exclude_filter(void);
void Snapshot_snapshot_w_filter_after_new(void);
void Snapshot_snapshot_w_filter_after_delete(void);
void Snapshot_snapshot_free_empty(void);
void Snapshot_snapshot_free(void);
void Snapshot_snapshot_free_filtered(void);
void Snapshot_snapshot_activate_table_w_filter(void);
void Snapshot_snapshot_copy(void);
void Snapshot_snapshot_copy_filtered(void);
void Snapshot_snapshot_copy_w_filter(void);

// Testsuite 'ReaderWriter'
void ReaderWriter_simple(void);
void ReaderWriter_id(void);
void ReaderWriter_id_w_simple(void);
void ReaderWriter_unaligned(void);
void ReaderWriter_empty(void);
void ReaderWriter_tag(void);
void ReaderWriter_simple_w_tag(void);
void ReaderWriter_tag_w_simple(void);
void ReaderWriter_empty_parent(void);
void ReaderWriter_parent(void);
void ReaderWriter_simple_w_parent(void);
void ReaderWriter_inheritance(void);
void ReaderWriter_simple_w_inheritance(void);
void ReaderWriter_deserialize_twice(void);
void ReaderWriter_entity_conflict(void);
void ReaderWriter_snapshot_reader_simple(void);
void ReaderWriter_snapshot_reader_id(void);
void ReaderWriter_component_id_conflict_w_component(void);
void ReaderWriter_component_id_conflict_w_entity(void);
void ReaderWriter_component_size_conflict(void);
void ReaderWriter_read_zero_size(void);
void ReaderWriter_write_zero_size(void);
void ReaderWriter_invalid_header(void);

// Testsuite 'FilterIter'
void FilterIter_iter_one_table(void);
void FilterIter_iter_two_tables(void);
void FilterIter_iter_two_comps(void);
void FilterIter_iter_snapshot_one_table(void);
void FilterIter_iter_snapshot_two_tables(void);
void FilterIter_iter_snapshot_two_comps(void);
void FilterIter_iter_snapshot_filtered_table(void);

// Testsuite 'Modules'
void Modules_simple_module(void);
void Modules_import_module_from_system(void);
void Modules_import_from_on_add_system(void);
void Modules_import_from_on_set_system(void);
void Modules_import_again(void);

// Testsuite 'Internals'
void Internals_deactivate_table(void);
void Internals_activate_table(void);
void Internals_activate_deactivate_table(void);
void Internals_activate_deactivate_reactive(void);
void Internals_activate_deactivate_activate_other(void);
void Internals_no_double_system_table_after_merge(void);

// Testsuite 'Error'
void Error_setup(void);
void Error_abort(void);
void Error_abort_w_param(void);
void Error_override_abort(void);
void Error_assert_true(void);
void Error_assert_false(void);
void Error_assert_false_w_param(void);
void Error_error_codes(void);
void Error_log_dbg(void);
void Error_log_log(void);
void Error_log_warning(void);
void Error_log_error(void);

static bake_test_suite suites[] = {
    {
        .id = "New",
        .testcase_count = 12,
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
            },
            {
                .id = "redefine_component",
                .function = New_redefine_component
            }
        }
    },
    {
        .id = "New_w_Count",
        .testcase_count = 11,
        .testcases = (bake_test_case[]){
            {
                .id = "empty",
                .function = New_w_Count_empty
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
        .id = "Set_w_data",
        .testcase_count = 34,
        .testcases = (bake_test_case[]){
            {
                .id = "1_column_3_rows",
                .function = Set_w_data_1_column_3_rows
            },
            {
                .id = "2_columns_3_rows",
                .function = Set_w_data_2_columns_3_rows
            },
            {
                .id = "1_column_3_rows_w_entities",
                .function = Set_w_data_1_column_3_rows_w_entities
            },
            {
                .id = "2_columns_3_rows_w_entities",
                .function = Set_w_data_2_columns_3_rows_w_entities
            },
            {
                .id = "1_column_3_rows_overwrite_entities",
                .function = Set_w_data_1_column_3_rows_overwrite_entities
            },
            {
                .id = "2_columns_3_rows_overwrite_entities",
                .function = Set_w_data_2_columns_3_rows_overwrite_entities
            },
            {
                .id = "overwrite_different_order",
                .function = Set_w_data_overwrite_different_order
            },
            {
                .id = "overwrite_different_type",
                .function = Set_w_data_overwrite_different_type
            },
            {
                .id = "2_columns_3_rows_no_data",
                .function = Set_w_data_2_columns_3_rows_no_data
            },
            {
                .id = "2_columns_3_rows_1_tag",
                .function = Set_w_data_2_columns_3_rows_1_tag
            },
            {
                .id = "2_columns_3_rows_1_parent",
                .function = Set_w_data_2_columns_3_rows_1_parent
            },
            {
                .id = "2_columns_3_rows_1_base",
                .function = Set_w_data_2_columns_3_rows_1_base
            },
            {
                .id = "2_columns_3_rows_1_base_w_entities",
                .function = Set_w_data_2_columns_3_rows_1_base_w_entities
            },
            {
                .id = "2_columns_3_rows_1_nested_prefab",
                .function = Set_w_data_2_columns_3_rows_1_nested_prefab
            },
            {
                .id = "2_columns_3_rows_component_order",
                .function = Set_w_data_2_columns_3_rows_component_order
            },
            {
                .id = "overwrite_w_unset_column",
                .function = Set_w_data_overwrite_w_unset_column
            },
            {
                .id = "overwrite_from_other_type_w_unset_column",
                .function = Set_w_data_overwrite_from_other_type_w_unset_column
            },
            {
                .id = "staged_1_column_3_rows",
                .function = Set_w_data_staged_1_column_3_rows
            },
            {
                .id = "staged_1_column_3_rows_w_entities",
                .function = Set_w_data_staged_1_column_3_rows_w_entities
            },
            {
                .id = "staged_1_column_3_rows_w_entities_w_base",
                .function = Set_w_data_staged_1_column_3_rows_w_entities_w_base
            },
            {
                .id = "staged_set_existing_in_stage",
                .function = Set_w_data_staged_set_existing_in_stage
            },
            {
                .id = "on_add",
                .function = Set_w_data_on_add
            },
            {
                .id = "on_add_w_entities",
                .function = Set_w_data_on_add_w_entities
            },
            {
                .id = "on_add_w_entities_again",
                .function = Set_w_data_on_add_w_entities_again
            },
            {
                .id = "on_set",
                .function = Set_w_data_on_set
            },
            {
                .id = "on_set_w_entities",
                .function = Set_w_data_on_set_w_entities
            },
            {
                .id = "on_set_no_data_after_on_add",
                .function = Set_w_data_on_set_no_data_after_on_add
            },
            {
                .id = "on_remove_existing",
                .function = Set_w_data_on_remove_existing
            },
            {
                .id = "on_add_different_origin",
                .function = Set_w_data_on_add_different_origin
            },
            {
                .id = "on_add_different_overlapping_origin",
                .function = Set_w_data_on_add_different_overlapping_origin
            },
            {
                .id = "on_add_different_overlapping_origin_reorder",
                .function = Set_w_data_on_add_different_overlapping_origin_reorder
            },
            {
                .id = "on_set_different_origin",
                .function = Set_w_data_on_set_different_origin
            },
            {
                .id = "on_remove_different_origin",
                .function = Set_w_data_on_remove_different_origin
            },
            {
                .id = "existing_different_type_out_of_order",
                .function = Set_w_data_existing_different_type_out_of_order
            }
        }
    },
    {
        .id = "Add",
        .testcase_count = 31,
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
            },
            {
                .id = "add_remove",
                .function = Add_add_remove
            },
            {
                .id = "add_remove_same",
                .function = Add_add_remove_same
            },
            {
                .id = "add_2_remove",
                .function = Add_add_2_remove
            },
            {
                .id = "on_add_after_new_type_in_progress",
                .function = Add_on_add_after_new_type_in_progress
            },
            {
                .id = "add_entity",
                .function = Add_add_entity
            },
            {
                .id = "remove_entity",
                .function = Add_remove_entity
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
        .id = "Add_remove_w_filter",
        .testcase_count = 12,
        .testcases = (bake_test_case[]){
            {
                .id = "remove_1_no_filter",
                .function = Add_remove_w_filter_remove_1_no_filter
            },
            {
                .id = "remove_2_no_filter",
                .function = Add_remove_w_filter_remove_2_no_filter
            },
            {
                .id = "remove_1_to_empty_no_filter",
                .function = Add_remove_w_filter_remove_1_to_empty_no_filter
            },
            {
                .id = "remove_2_to_empty_no_filter",
                .function = Add_remove_w_filter_remove_2_to_empty_no_filter
            },
            {
                .id = "remove_1_all_no_filter",
                .function = Add_remove_w_filter_remove_1_all_no_filter
            },
            {
                .id = "remove_2_all_no_filter",
                .function = Add_remove_w_filter_remove_2_all_no_filter
            },
            {
                .id = "remove_1_exclude_1",
                .function = Add_remove_w_filter_remove_1_exclude_1
            },
            {
                .id = "remove_1_exclude_2",
                .function = Add_remove_w_filter_remove_1_exclude_2
            },
            {
                .id = "remove_1_include_1",
                .function = Add_remove_w_filter_remove_1_include_1
            },
            {
                .id = "remove_1_include_2",
                .function = Add_remove_w_filter_remove_1_include_2
            },
            {
                .id = "add_1",
                .function = Add_remove_w_filter_add_1
            },
            {
                .id = "add_2",
                .function = Add_remove_w_filter_add_2
            }
        }
    },
    {
        .id = "Has",
        .testcase_count = 30,
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
                .id = "any_of_0_0",
                .function = Has_any_of_0_0
            },
            {
                .id = "any_of_0_1",
                .function = Has_any_of_0_1
            },
            {
                .id = "any_of_0_zero_entity",
                .function = Has_any_of_0_zero_entity
            },
            {
                .id = "has_in_progress",
                .function = Has_has_in_progress
            },
            {
                .id = "has_of_zero",
                .function = Has_has_of_zero
            },
            {
                .id = "has_owned",
                .function = Has_has_owned
            },
            {
                .id = "has_owned_any",
                .function = Has_has_owned_any
            },
            {
                .id = "has_entity",
                .function = Has_has_entity
            },
            {
                .id = "has_entity_0",
                .function = Has_has_entity_0
            },
            {
                .id = "has_entity_0_component",
                .function = Has_has_entity_0_component
            },
            {
                .id = "has_entity_owned",
                .function = Has_has_entity_owned
            },
            {
                .id = "has_entity_owned_0",
                .function = Has_has_entity_owned_0
            },
            {
                .id = "has_entity_owned_0_component",
                .function = Has_has_entity_owned_0_component
            }
        }
    },
    {
        .id = "Count",
        .testcase_count = 5,
        .testcases = (bake_test_case[]){
            {
                .id = "count_empty",
                .function = Count_count_empty
            },
            {
                .id = "count_1_component",
                .function = Count_count_1_component
            },
            {
                .id = "count_2_components",
                .function = Count_count_2_components
            },
            {
                .id = "count_3_components",
                .function = Count_count_3_components
            },
            {
                .id = "count_2_types_2_comps",
                .function = Count_count_2_types_2_comps
            }
        }
    },
    {
        .id = "Get_component",
        .testcase_count = 9,
        .testcases = (bake_test_case[]){
            {
                .id = "get_empty",
                .function = Get_component_get_empty
            },
            {
                .id = "get_1_from_1",
                .function = Get_component_get_1_from_1
            },
            {
                .id = "get_1_from_2",
                .function = Get_component_get_1_from_2
            },
            {
                .id = "get_2_from_2",
                .function = Get_component_get_2_from_2
            },
            {
                .id = "get_2_from_3",
                .function = Get_component_get_2_from_3
            },
            {
                .id = "get_1_from_2_in_progress_from_main_stage",
                .function = Get_component_get_1_from_2_in_progress_from_main_stage
            },
            {
                .id = "get_1_from_2_add_in_progress",
                .function = Get_component_get_1_from_2_add_in_progress
            },
            {
                .id = "get_both_from_2_add_in_progress",
                .function = Get_component_get_both_from_2_add_in_progress
            },
            {
                .id = "get_both_from_2_add_remove_in_progress",
                .function = Get_component_get_both_from_2_add_remove_in_progress
            }
        }
    },
    {
        .id = "Delete",
        .testcase_count = 9,
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
            },
            {
                .id = "delete_w_on_remove",
                .function = Delete_delete_w_on_remove
            }
        }
    },
    {
        .id = "Delete_w_filter",
        .testcase_count = 12,
        .testcases = (bake_test_case[]){
            {
                .id = "delete_1",
                .function = Delete_w_filter_delete_1
            },
            {
                .id = "delete_2",
                .function = Delete_w_filter_delete_2
            },
            {
                .id = "delete_1_2_types",
                .function = Delete_w_filter_delete_1_2_types
            },
            {
                .id = "delete_2_2_types",
                .function = Delete_w_filter_delete_2_2_types
            },
            {
                .id = "delete_except_1",
                .function = Delete_w_filter_delete_except_1
            },
            {
                .id = "delete_except_2",
                .function = Delete_w_filter_delete_except_2
            },
            {
                .id = "delete_with_any_of_2",
                .function = Delete_w_filter_delete_with_any_of_2
            },
            {
                .id = "delete_except_all_of_2",
                .function = Delete_w_filter_delete_except_all_of_2
            },
            {
                .id = "include_exact",
                .function = Delete_w_filter_include_exact
            },
            {
                .id = "exclude_exact",
                .function = Delete_w_filter_exclude_exact
            },
            {
                .id = "system_activate_test",
                .function = Delete_w_filter_system_activate_test
            },
            {
                .id = "skip_builtin_tables",
                .function = Delete_w_filter_skip_builtin_tables
            }
        }
    },
    {
        .id = "Set",
        .testcase_count = 13,
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
            },
            {
                .id = "set_null",
                .function = Set_set_null
            }
        }
    },
    {
        .id = "Lookup",
        .testcase_count = 11,
        .testcases = (bake_test_case[]){
            {
                .id = "lookup",
                .function = Lookup_lookup
            },
            {
                .id = "lookup_component",
                .function = Lookup_lookup_component
            },
            {
                .id = "lookup_not_found",
                .function = Lookup_lookup_not_found
            },
            {
                .id = "lookup_child",
                .function = Lookup_lookup_child
            },
            {
                .id = "lookup_child_w_component",
                .function = Lookup_lookup_child_w_component
            },
            {
                .id = "lookup_in_progress",
                .function = Lookup_lookup_in_progress
            },
            {
                .id = "lookup_child_in_progress",
                .function = Lookup_lookup_child_in_progress
            },
            {
                .id = "lookup_w_null_id",
                .function = Lookup_lookup_w_null_id
            },
            {
                .id = "get_id",
                .function = Lookup_get_id
            },
            {
                .id = "get_id_no_id",
                .function = Lookup_get_id_no_id
            },
            {
                .id = "get_id_from_empty",
                .function = Lookup_get_id_from_empty
            }
        }
    },
    {
        .id = "Singleton",
        .testcase_count = 7,
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
                .id = "system_w_singleton_no_match",
                .function = Singleton_system_w_singleton_no_match
            },
            {
                .id = "system_w_not_singleton",
                .function = Singleton_system_w_not_singleton
            },
            {
                .id = "lookup_singleton",
                .function = Singleton_lookup_singleton
            },
            {
                .id = "get_singleton_id",
                .function = Singleton_get_singleton_id
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
        .id = "SystemMisc",
        .testcase_count = 40,
        .testcases = (bake_test_case[]){
            {
                .id = "invalid_not_without_id",
                .function = SystemMisc_invalid_not_without_id
            },
            {
                .id = "invalid_optional_without_id",
                .function = SystemMisc_invalid_optional_without_id
            },
            {
                .id = "invalid_system_without_id",
                .function = SystemMisc_invalid_system_without_id
            },
            {
                .id = "invalid_container_without_id",
                .function = SystemMisc_invalid_container_without_id
            },
            {
                .id = "invalid_cascade_without_id",
                .function = SystemMisc_invalid_cascade_without_id
            },
            {
                .id = "invalid_entity_without_id",
                .function = SystemMisc_invalid_entity_without_id
            },
            {
                .id = "invalid_empty_without_id",
                .function = SystemMisc_invalid_empty_without_id
            },
            {
                .id = "invalid_singleton_without_id",
                .function = SystemMisc_invalid_singleton_without_id
            },
            {
                .id = "invalid_empty_element",
                .function = SystemMisc_invalid_empty_element
            },
            {
                .id = "invalid_empty_element_w_space",
                .function = SystemMisc_invalid_empty_element_w_space
            },
            {
                .id = "invalid_empty_or",
                .function = SystemMisc_invalid_empty_or
            },
            {
                .id = "invalid_empty_or_w_space",
                .function = SystemMisc_invalid_empty_or_w_space
            },
            {
                .id = "invalid_or_w_not",
                .function = SystemMisc_invalid_or_w_not
            },
            {
                .id = "invalid_not_w_or",
                .function = SystemMisc_invalid_not_w_or
            },
            {
                .id = "invalid_0_w_and",
                .function = SystemMisc_invalid_0_w_and
            },
            {
                .id = "invalid_0_w_from_system",
                .function = SystemMisc_invalid_0_w_from_system
            },
            {
                .id = "invalid_0_w_from_container",
                .function = SystemMisc_invalid_0_w_from_container
            },
            {
                .id = "invalid_0_w_from_cascade",
                .function = SystemMisc_invalid_0_w_from_cascade
            },
            {
                .id = "invalid_0_w_from_entity",
                .function = SystemMisc_invalid_0_w_from_entity
            },
            {
                .id = "invalid_0_w_from_empty",
                .function = SystemMisc_invalid_0_w_from_empty
            },
            {
                .id = "invalid_0_w_from_singleton",
                .function = SystemMisc_invalid_0_w_from_singleton
            },
            {
                .id = "invalid_or_w_empty",
                .function = SystemMisc_invalid_or_w_empty
            },
            {
                .id = "invalid_component_id",
                .function = SystemMisc_invalid_component_id
            },
            {
                .id = "invalid_entity_id",
                .function = SystemMisc_invalid_entity_id
            },
            {
                .id = "invalid_or_from_system",
                .function = SystemMisc_invalid_or_from_system
            },
            {
                .id = "invalid_phase",
                .function = SystemMisc_invalid_phase
            },
            {
                .id = "invalid_null_string",
                .function = SystemMisc_invalid_null_string
            },
            {
                .id = "invalid_empty_string",
                .function = SystemMisc_invalid_empty_string
            },
            {
                .id = "invalid_empty_string_w_space",
                .function = SystemMisc_invalid_empty_string_w_space
            },
            {
                .id = "redefine_row_system",
                .function = SystemMisc_redefine_row_system
            },
            {
                .id = "system_w_or_prefab",
                .function = SystemMisc_system_w_or_prefab
            },
            {
                .id = "system_w_or_disabled",
                .function = SystemMisc_system_w_or_disabled
            },
            {
                .id = "system_w_or_disabled_and_prefab",
                .function = SystemMisc_system_w_or_disabled_and_prefab
            },
            {
                .id = "table_columns_access",
                .function = SystemMisc_table_columns_access
            },
            {
                .id = "status_enable_after_new",
                .function = SystemMisc_status_enable_after_new
            },
            {
                .id = "status_enable_after_disable",
                .function = SystemMisc_status_enable_after_disable
            },
            {
                .id = "status_disable_after_new",
                .function = SystemMisc_status_disable_after_new
            },
            {
                .id = "status_disable_after_disable",
                .function = SystemMisc_status_disable_after_disable
            },
            {
                .id = "status_activate_after_new",
                .function = SystemMisc_status_activate_after_new
            },
            {
                .id = "status_deactivate_after_delete",
                .function = SystemMisc_status_deactivate_after_delete
            }
        }
    },
    {
        .id = "SystemOnAdd",
        .testcase_count = 31,
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
            },
            {
                .id = "override_after_add_in_on_add",
                .function = SystemOnAdd_override_after_add_in_on_add
            },
            {
                .id = "set_after_add_in_on_add",
                .function = SystemOnAdd_set_after_add_in_on_add
            },
            {
                .id = "add_again_in_progress",
                .function = SystemOnAdd_add_again_in_progress
            },
            {
                .id = "add_in_progress_before_system_def",
                .function = SystemOnAdd_add_in_progress_before_system_def
            },
            {
                .id = "disabled_system",
                .function = SystemOnAdd_disabled_system
            },
            {
                .id = "2_systems_w_table_creation",
                .function = SystemOnAdd_2_systems_w_table_creation
            },
            {
                .id = "2_systems_w_table_creation_in_progress",
                .function = SystemOnAdd_2_systems_w_table_creation_in_progress
            },
            {
                .id = "sys_context",
                .function = SystemOnAdd_sys_context
            },
            {
                .id = "get_sys_context_from_param",
                .function = SystemOnAdd_get_sys_context_from_param
            }
        }
    },
    {
        .id = "SystemOnRemove",
        .testcase_count = 15,
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
            },
            {
                .id = "disabled_system",
                .function = SystemOnRemove_disabled_system
            }
        }
    },
    {
        .id = "SystemOnSet",
        .testcase_count = 16,
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
            },
            {
                .id = "on_set_after_on_add",
                .function = SystemOnSet_on_set_after_on_add
            },
            {
                .id = "on_set_after_on_add_w_new",
                .function = SystemOnSet_on_set_after_on_add_w_new
            },
            {
                .id = "on_set_after_on_add_w_new_w_count",
                .function = SystemOnSet_on_set_after_on_add_w_new_w_count
            },
            {
                .id = "on_set_after_on_add_1_of_2_matched",
                .function = SystemOnSet_on_set_after_on_add_1_of_2_matched
            },
            {
                .id = "on_set_after_override",
                .function = SystemOnSet_on_set_after_override
            },
            {
                .id = "on_set_after_override_w_new",
                .function = SystemOnSet_on_set_after_override_w_new
            },
            {
                .id = "on_set_after_override_w_new_w_count",
                .function = SystemOnSet_on_set_after_override_w_new_w_count
            },
            {
                .id = "on_set_after_override_1_of_2_overridden",
                .function = SystemOnSet_on_set_after_override_1_of_2_overridden
            },
            {
                .id = "disabled_system",
                .function = SystemOnSet_disabled_system
            }
        }
    },
    {
        .id = "SystemOnFrame",
        .testcase_count = 48,
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
            },
            {
                .id = "ensure_optional_is_unset_column",
                .function = SystemOnFrame_ensure_optional_is_unset_column
            },
            {
                .id = "ensure_optional_is_null_shared",
                .function = SystemOnFrame_ensure_optional_is_null_shared
            },
            {
                .id = "ensure_optional_is_null_field_owned",
                .function = SystemOnFrame_ensure_optional_is_null_field_owned
            },
            {
                .id = "ensure_optional_is_null_field_shared",
                .function = SystemOnFrame_ensure_optional_is_null_field_shared
            },
            {
                .id = "use_fields_2_owned",
                .function = SystemOnFrame_use_fields_2_owned
            },
            {
                .id = "use_fields_1_owned_1_shared",
                .function = SystemOnFrame_use_fields_1_owned_1_shared
            },
            {
                .id = "match_2_systems_w_populated_table",
                .function = SystemOnFrame_match_2_systems_w_populated_table
            },
            {
                .id = "on_period",
                .function = SystemOnFrame_on_period
            },
            {
                .id = "on_period_long_delta",
                .function = SystemOnFrame_on_period_long_delta
            },
            {
                .id = "disabled",
                .function = SystemOnFrame_disabled
            },
            {
                .id = "disabled_feature",
                .function = SystemOnFrame_disabled_feature
            },
            {
                .id = "disabled_nested_feature",
                .function = SystemOnFrame_disabled_nested_feature
            },
            {
                .id = "two_refs",
                .function = SystemOnFrame_two_refs
            },
            {
                .id = "filter_disabled",
                .function = SystemOnFrame_filter_disabled
            },
            {
                .id = "match_disabled",
                .function = SystemOnFrame_match_disabled
            },
            {
                .id = "match_disabled_and_enabled",
                .function = SystemOnFrame_match_disabled_and_enabled
            },
            {
                .id = "match_prefab",
                .function = SystemOnFrame_match_prefab
            },
            {
                .id = "match_prefab_and_normal",
                .function = SystemOnFrame_match_prefab_and_normal
            },
            {
                .id = "is_shared_on_column_not_set",
                .function = SystemOnFrame_is_shared_on_column_not_set
            },
            {
                .id = "owned_column",
                .function = SystemOnFrame_owned_column
            },
            {
                .id = "owned_not_column",
                .function = SystemOnFrame_owned_not_column
            },
            {
                .id = "shared_column",
                .function = SystemOnFrame_shared_column
            },
            {
                .id = "shared_not_column",
                .function = SystemOnFrame_shared_not_column
            },
            {
                .id = "container_dont_match_inheritance",
                .function = SystemOnFrame_container_dont_match_inheritance
            },
            {
                .id = "cascade_dont_match_inheritance",
                .function = SystemOnFrame_cascade_dont_match_inheritance
            },
            {
                .id = "not_from_singleton",
                .function = SystemOnFrame_not_from_singleton
            },
            {
                .id = "not_from_entity",
                .function = SystemOnFrame_not_from_entity
            },
            {
                .id = "sys_context",
                .function = SystemOnFrame_sys_context
            },
            {
                .id = "get_sys_context_from_param",
                .function = SystemOnFrame_get_sys_context_from_param
            },
            {
                .id = "use_field_w_0_size",
                .function = SystemOnFrame_use_field_w_0_size
            },
            {
                .id = "owned_only",
                .function = SystemOnFrame_owned_only
            },
            {
                .id = "shared_only",
                .function = SystemOnFrame_shared_only
            },
            {
                .id = "is_in_readonly",
                .function = SystemOnFrame_is_in_readonly
            }
        }
    },
    {
        .id = "SystemOnDemand",
        .testcase_count = 26,
        .testcases = (bake_test_case[]){
            {
                .id = "enable_out_after_in",
                .function = SystemOnDemand_enable_out_after_in
            },
            {
                .id = "enable_in_after_out",
                .function = SystemOnDemand_enable_in_after_out
            },
            {
                .id = "enable_out_after_in_2_out_1_in",
                .function = SystemOnDemand_enable_out_after_in_2_out_1_in
            },
            {
                .id = "enable_out_after_in_1_out_2_in",
                .function = SystemOnDemand_enable_out_after_in_1_out_2_in
            },
            {
                .id = "enable_in_after_out_2_out_1_in",
                .function = SystemOnDemand_enable_in_after_out_2_out_1_in
            },
            {
                .id = "enable_in_after_out_1_out_2_in",
                .function = SystemOnDemand_enable_in_after_out_1_out_2_in
            },
            {
                .id = "disable_after_disable_in",
                .function = SystemOnDemand_disable_after_disable_in
            },
            {
                .id = "disable_after_disable_in_2_out_1_in",
                .function = SystemOnDemand_disable_after_disable_in_2_out_1_in
            },
            {
                .id = "disable_after_disable_in_1_out_2_in",
                .function = SystemOnDemand_disable_after_disable_in_1_out_2_in
            },
            {
                .id = "table_after_out",
                .function = SystemOnDemand_table_after_out
            },
            {
                .id = "table_after_out_and_in",
                .function = SystemOnDemand_table_after_out_and_in
            },
            {
                .id = "table_after_out_and_in_overlapping_columns",
                .function = SystemOnDemand_table_after_out_and_in_overlapping_columns
            },
            {
                .id = "1_out_system_2_in_systems",
                .function = SystemOnDemand_1_out_system_2_in_systems
            },
            {
                .id = "1_out_system_2_in_systems_different_columns",
                .function = SystemOnDemand_1_out_system_2_in_systems_different_columns
            },
            {
                .id = "1_out_system_2_in_systems_overlapping_columns",
                .function = SystemOnDemand_1_out_system_2_in_systems_overlapping_columns
            },
            {
                .id = "disable_after_inactive_in_system",
                .function = SystemOnDemand_disable_after_inactive_in_system
            },
            {
                .id = "disable_after_2_inactive_in_systems",
                .function = SystemOnDemand_disable_after_2_inactive_in_systems
            },
            {
                .id = "disable_after_2_inactive_in_systems_different_columns",
                .function = SystemOnDemand_disable_after_2_inactive_in_systems_different_columns
            },
            {
                .id = "enable_2_output_1_input_system",
                .function = SystemOnDemand_enable_2_output_1_input_system
            },
            {
                .id = "enable_2_output_1_input_system_different_columns",
                .function = SystemOnDemand_enable_2_output_1_input_system_different_columns
            },
            {
                .id = "enable_2_output_1_input_system_overlapping_columns",
                .function = SystemOnDemand_enable_2_output_1_input_system_overlapping_columns
            },
            {
                .id = "out_not_column",
                .function = SystemOnDemand_out_not_column
            },
            {
                .id = "trigger_on_manual",
                .function = SystemOnDemand_trigger_on_manual
            },
            {
                .id = "trigger_on_manual_not_column",
                .function = SystemOnDemand_trigger_on_manual_not_column
            },
            {
                .id = "on_demand_task_w_from_entity",
                .function = SystemOnDemand_on_demand_task_w_from_entity
            },
            {
                .id = "on_demand_task_w_not_from_entity",
                .function = SystemOnDemand_on_demand_task_w_not_from_entity
            }
        }
    },
    {
        .id = "SystemCascade",
        .testcase_count = 4,
        .testcases = (bake_test_case[]){
            {
                .id = "cascade_depth_1",
                .function = SystemCascade_cascade_depth_1
            },
            {
                .id = "cascade_depth_2",
                .function = SystemCascade_cascade_depth_2
            },
            {
                .id = "add_after_match",
                .function = SystemCascade_add_after_match
            },
            {
                .id = "adopt_after_match",
                .function = SystemCascade_adopt_after_match
            }
        }
    },
    {
        .id = "SystemManual",
        .testcase_count = 3,
        .testcases = (bake_test_case[]){
            {
                .id = "1_type_1_component",
                .function = SystemManual_1_type_1_component
            },
            {
                .id = "disabled",
                .function = SystemManual_disabled
            },
            {
                .id = "activate_status",
                .function = SystemManual_activate_status
            }
        }
    },
    {
        .id = "Tasks",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "no_components",
                .function = Tasks_no_components
            },
            {
                .id = "one_tag",
                .function = Tasks_one_tag
            },
            {
                .id = "from_system",
                .function = Tasks_from_system
            },
            {
                .id = "on_remove_no_components",
                .function = Tasks_on_remove_no_components
            },
            {
                .id = "on_remove_one_tag",
                .function = Tasks_on_remove_one_tag
            },
            {
                .id = "on_remove_from_system",
                .function = Tasks_on_remove_from_system
            },
            {
                .id = "tasks_in_phases",
                .function = Tasks_tasks_in_phases
            }
        }
    },
    {
        .id = "Container",
        .testcase_count = 27,
        .testcases = (bake_test_case[]){
            {
                .id = "child",
                .function = Container_child
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
                .id = "child_w_type_w_childof",
                .function = Container_child_w_type_w_childof
            },
            {
                .id = "child_w_count_component",
                .function = Container_child_w_count_component
            },
            {
                .id = "child_w_count_w_type",
                .function = Container_child_w_count_w_type
            },
            {
                .id = "child_w_count_type_w_childof",
                .function = Container_child_w_count_type_w_childof
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
            },
            {
                .id = "get_parent",
                .function = Container_get_parent
            },
            {
                .id = "get_parent_no_matching_comp",
                .function = Container_get_parent_no_matching_comp
            },
            {
                .id = "get_parent_two_parents",
                .function = Container_get_parent_two_parents
            },
            {
                .id = "get_parent_no_parent",
                .function = Container_get_parent_no_parent
            },
            {
                .id = "singleton_as_container",
                .function = Container_singleton_as_container
            }
        }
    },
    {
        .id = "Prefab",
        .testcase_count = 63,
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
            },
            {
                .id = "iterate_w_prefab_field",
                .function = Prefab_iterate_w_prefab_field
            },
            {
                .id = "iterate_w_prefab_shared",
                .function = Prefab_iterate_w_prefab_shared
            },
            {
                .id = "match_entity_prefab_w_system_optional",
                .function = Prefab_match_entity_prefab_w_system_optional
            },
            {
                .id = "prefab_in_system_expr",
                .function = Prefab_prefab_in_system_expr
            },
            {
                .id = "dont_match_prefab",
                .function = Prefab_dont_match_prefab
            },
            {
                .id = "new_w_count_w_override",
                .function = Prefab_new_w_count_w_override
            },
            {
                .id = "override_2_components_different_size",
                .function = Prefab_override_2_components_different_size
            },
            {
                .id = "ignore_prefab_parent_component",
                .function = Prefab_ignore_prefab_parent_component
            },
            {
                .id = "match_table_created_in_progress",
                .function = Prefab_match_table_created_in_progress
            },
            {
                .id = "prefab_w_1_child",
                .function = Prefab_prefab_w_1_child
            },
            {
                .id = "prefab_w_2_children",
                .function = Prefab_prefab_w_2_children
            },
            {
                .id = "prefab_w_grandchild",
                .function = Prefab_prefab_w_grandchild
            },
            {
                .id = "prefab_w_base_w_child",
                .function = Prefab_prefab_w_base_w_child
            },
            {
                .id = "prefab_w_child_w_base",
                .function = Prefab_prefab_w_child_w_base
            },
            {
                .id = "prefab_w_child_w_base_w_children",
                .function = Prefab_prefab_w_child_w_base_w_children
            },
            {
                .id = "prefab_w_child_new_w_count",
                .function = Prefab_prefab_w_child_new_w_count
            },
            {
                .id = "prefab_auto_override_child_component",
                .function = Prefab_prefab_auto_override_child_component
            },
            {
                .id = "ignore_on_add",
                .function = Prefab_ignore_on_add
            },
            {
                .id = "ignore_on_remove",
                .function = Prefab_ignore_on_remove
            },
            {
                .id = "ignore_on_set",
                .function = Prefab_ignore_on_set
            },
            {
                .id = "on_set_on_instance",
                .function = Prefab_on_set_on_instance
            },
            {
                .id = "instantiate_in_progress",
                .function = Prefab_instantiate_in_progress
            },
            {
                .id = "copy_from_prefab_in_progress",
                .function = Prefab_copy_from_prefab_in_progress
            },
            {
                .id = "copy_from_prefab_first_instance_in_progress",
                .function = Prefab_copy_from_prefab_first_instance_in_progress
            },
            {
                .id = "cached_ptr_after_realloc",
                .function = Prefab_cached_ptr_after_realloc
            },
            {
                .id = "revalidate_cached_ptr_w_mixed_table_refs",
                .function = Prefab_revalidate_cached_ptr_w_mixed_table_refs
            },
            {
                .id = "no_overwrite_on_2nd_add",
                .function = Prefab_no_overwrite_on_2nd_add
            },
            {
                .id = "no_overwrite_on_2nd_add_in_progress",
                .function = Prefab_no_overwrite_on_2nd_add_in_progress
            },
            {
                .id = "no_instantiate_on_2nd_add",
                .function = Prefab_no_instantiate_on_2nd_add
            },
            {
                .id = "no_instantiate_on_2nd_add_in_progress",
                .function = Prefab_no_instantiate_on_2nd_add_in_progress
            },
            {
                .id = "nested_prefab_in_progress_w_count",
                .function = Prefab_nested_prefab_in_progress_w_count
            },
            {
                .id = "nested_prefab_in_progress_w_count_set_after_override",
                .function = Prefab_nested_prefab_in_progress_w_count_set_after_override
            },
            {
                .id = "get_ptr_from_prefab_from_new_table_in_progress",
                .function = Prefab_get_ptr_from_prefab_from_new_table_in_progress
            },
            {
                .id = "match_base",
                .function = Prefab_match_base
            },
            {
                .id = "match_base_after_add_in_prev_phase",
                .function = Prefab_match_base_after_add_in_prev_phase
            },
            {
                .id = "override_watched_prefab",
                .function = Prefab_override_watched_prefab
            },
            {
                .id = "rematch_twice",
                .function = Prefab_rematch_twice
            },
            {
                .id = "inherit_in_system",
                .function = Prefab_inherit_in_system
            },
            {
                .id = "add_to_empty_base_in_system",
                .function = Prefab_add_to_empty_base_in_system
            },
            {
                .id = "dont_inherit_disabled",
                .function = Prefab_dont_inherit_disabled
            },
            {
                .id = "cyclic_inheritance",
                .function = Prefab_cyclic_inheritance
            },
            {
                .id = "cyclic_inheritance_w_clone_in_progress",
                .function = Prefab_cyclic_inheritance_w_clone_in_progress
            },
            {
                .id = "cyclic_inheritance_get_unavailable",
                .function = Prefab_cyclic_inheritance_get_unavailable
            },
            {
                .id = "cyclic_inheritance_has_unavailable",
                .function = Prefab_cyclic_inheritance_has_unavailable
            },
            {
                .id = "clone_after_inherit_in_on_add",
                .function = Prefab_clone_after_inherit_in_on_add
            },
            {
                .id = "override_from_nested",
                .function = Prefab_override_from_nested
            },
            {
                .id = "create_multiple_nested_w_on_add",
                .function = Prefab_create_multiple_nested_w_on_add
            },
            {
                .id = "create_multiple_nested_w_on_add_in_progress",
                .function = Prefab_create_multiple_nested_w_on_add_in_progress
            }
        }
    },
    {
        .id = "System_w_FromContainer",
        .testcase_count = 20,
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
                .id = "2_column_1_from_container_w_not_prefab",
                .function = System_w_FromContainer_2_column_1_from_container_w_not_prefab
            },
            {
                .id = "3_column_1_from_comtainer_1_from_container_w_not",
                .function = System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not
            },
            {
                .id = "2_column_1_from_container_w_or",
                .function = System_w_FromContainer_2_column_1_from_container_w_or
            },
            {
                .id = "select_same_from_container",
                .function = System_w_FromContainer_select_same_from_container
            },
            {
                .id = "add_component_after_match",
                .function = System_w_FromContainer_add_component_after_match
            },
            {
                .id = "add_component_after_match_and_rematch",
                .function = System_w_FromContainer_add_component_after_match_and_rematch
            },
            {
                .id = "add_component_after_match_unmatch",
                .function = System_w_FromContainer_add_component_after_match_unmatch
            },
            {
                .id = "add_component_after_match_unmatch_match",
                .function = System_w_FromContainer_add_component_after_match_unmatch_match
            },
            {
                .id = "add_component_after_match_2_systems",
                .function = System_w_FromContainer_add_component_after_match_2_systems
            },
            {
                .id = "add_component_in_progress_after_match",
                .function = System_w_FromContainer_add_component_in_progress_after_match
            },
            {
                .id = "add_component_after_match_and_rematch_w_entity_type_expr",
                .function = System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr
            },
            {
                .id = "add_component_after_match_and_rematch_w_entity_type_expr_in_progress",
                .function = System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr_in_progress
            },
            {
                .id = "adopt_after_match",
                .function = System_w_FromContainer_adopt_after_match
            },
            {
                .id = "new_child_after_match",
                .function = System_w_FromContainer_new_child_after_match
            },
            {
                .id = "realloc_after_match",
                .function = System_w_FromContainer_realloc_after_match
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
        .id = "System_w_FromEntity",
        .testcase_count = 3,
        .testcases = (bake_test_case[]){
            {
                .id = "2_column_1_from_entity",
                .function = System_w_FromEntity_2_column_1_from_entity
            },
            {
                .id = "task_from_entity",
                .function = System_w_FromEntity_task_from_entity
            },
            {
                .id = "task_not_from_entity",
                .function = System_w_FromEntity_task_not_from_entity
            }
        }
    },
    {
        .id = "World",
        .testcase_count = 33,
        .testcases = (bake_test_case[]){
            {
                .id = "progress_w_0",
                .function = World_progress_w_0
            },
            {
                .id = "progress_w_t",
                .function = World_progress_w_t
            },
            {
                .id = "get_tick",
                .function = World_get_tick
            },
            {
                .id = "entity_range_offset",
                .function = World_entity_range_offset
            },
            {
                .id = "entity_range_offset_out_of_range",
                .function = World_entity_range_offset_out_of_range
            },
            {
                .id = "entity_range_limit_out_of_range",
                .function = World_entity_range_limit_out_of_range
            },
            {
                .id = "entity_range_add_existing_in_progress",
                .function = World_entity_range_add_existing_in_progress
            },
            {
                .id = "entity_range_add_in_range_in_progress",
                .function = World_entity_range_add_in_range_in_progress
            },
            {
                .id = "entity_range_add_out_of_range_in_progress",
                .function = World_entity_range_add_out_of_range_in_progress
            },
            {
                .id = "entity_range_out_of_range_check_disabled",
                .function = World_entity_range_out_of_range_check_disabled
            },
            {
                .id = "dim",
                .function = World_dim
            },
            {
                .id = "dim_type",
                .function = World_dim_type
            },
            {
                .id = "dim_dim_type",
                .function = World_dim_dim_type
            },
            {
                .id = "phases",
                .function = World_phases
            },
            {
                .id = "phases_w_merging",
                .function = World_phases_w_merging
            },
            {
                .id = "phases_match_in_create",
                .function = World_phases_match_in_create
            },
            {
                .id = "measure_time",
                .function = World_measure_time
            },
            {
                .id = "control_fps",
                .function = World_control_fps
            },
            {
                .id = "control_fps_busy_system",
                .function = World_control_fps_busy_system
            },
            {
                .id = "control_fps_busy_app",
                .function = World_control_fps_busy_app
            },
            {
                .id = "control_fps_random_system",
                .function = World_control_fps_random_system
            },
            {
                .id = "control_fps_random_app",
                .function = World_control_fps_random_app
            },
            {
                .id = "world_stats",
                .function = World_world_stats
            },
            {
                .id = "quit",
                .function = World_quit
            },
            {
                .id = "get_delta_time",
                .function = World_get_delta_time
            },
            {
                .id = "get_delta_time_auto",
                .function = World_get_delta_time_auto
            },
            {
                .id = "recreate_world",
                .function = World_recreate_world
            },
            {
                .id = "init_w_args_set_threads",
                .function = World_init_w_args_set_threads
            },
            {
                .id = "init_w_args_set_fps",
                .function = World_init_w_args_set_fps
            },
            {
                .id = "init_w_args_enable_dbg",
                .function = World_init_w_args_enable_dbg
            },
            {
                .id = "no_threading",
                .function = World_no_threading
            },
            {
                .id = "no_time",
                .function = World_no_time
            },
            {
                .id = "is_entity_enabled",
                .function = World_is_entity_enabled
            }
        }
    },
    {
        .id = "Type",
        .testcase_count = 48,
        .testcases = (bake_test_case[]){
            {
                .id = "type_of_1_tostr",
                .function = Type_type_of_1_tostr
            },
            {
                .id = "type_of_2_tostr",
                .function = Type_type_of_2_tostr
            },
            {
                .id = "type_of_2_tostr_no_id",
                .function = Type_type_of_2_tostr_no_id
            },
            {
                .id = "type_redefine",
                .function = Type_type_redefine
            },
            {
                .id = "type_from_array",
                .function = Type_type_from_array
            },
            {
                .id = "type_from_array_unsorted",
                .function = Type_type_from_array_unsorted
            },
            {
                .id = "type_has",
                .function = Type_type_has
            },
            {
                .id = "type_has_not",
                .function = Type_type_has_not
            },
            {
                .id = "zero_type_has_not",
                .function = Type_zero_type_has_not
            },
            {
                .id = "type_has_prefab",
                .function = Type_type_has_prefab
            },
            {
                .id = "type_has_container",
                .function = Type_type_has_container
            },
            {
                .id = "type_has_prefab_container",
                .function = Type_type_has_prefab_container
            },
            {
                .id = "type_merge",
                .function = Type_type_merge
            },
            {
                .id = "type_merge_overlap",
                .function = Type_type_merge_overlap
            },
            {
                .id = "type_merge_overlap_one",
                .function = Type_type_merge_overlap_one
            },
            {
                .id = "type_merge_overlap_w_flag",
                .function = Type_type_merge_overlap_w_flag
            },
            {
                .id = "type_merge_overlap_w_flags_from_both",
                .function = Type_type_merge_overlap_w_flags_from_both
            },
            {
                .id = "type_add",
                .function = Type_type_add
            },
            {
                .id = "type_add_empty",
                .function = Type_type_add_empty
            },
            {
                .id = "type_add_entity_again",
                .function = Type_type_add_entity_again
            },
            {
                .id = "type_add_out_of_order",
                .function = Type_type_add_out_of_order
            },
            {
                .id = "type_add_existing",
                .function = Type_type_add_existing
            },
            {
                .id = "type_add_empty_existing",
                .function = Type_type_add_empty_existing
            },
            {
                .id = "type_add_out_of_order_existing",
                .function = Type_type_add_out_of_order_existing
            },
            {
                .id = "type_remove",
                .function = Type_type_remove
            },
            {
                .id = "type_remove_empty",
                .function = Type_type_remove_empty
            },
            {
                .id = "type_remove_non_existing",
                .function = Type_type_remove_non_existing
            },
            {
                .id = "type_of_2_add",
                .function = Type_type_of_2_add
            },
            {
                .id = "type_of_3_add_entity_again",
                .function = Type_type_of_3_add_entity_again
            },
            {
                .id = "invalid_entity_type_expression",
                .function = Type_invalid_entity_type_expression
            },
            {
                .id = "invalid_singleton_type_expression",
                .function = Type_invalid_singleton_type_expression
            },
            {
                .id = "invalid_container_type_expression",
                .function = Type_invalid_container_type_expression
            },
            {
                .id = "invalid_system_type_expression",
                .function = Type_invalid_system_type_expression
            },
            {
                .id = "type_from_empty_entity",
                .function = Type_type_from_empty_entity
            },
            {
                .id = "type_from_empty_entity_in_progress",
                .function = Type_type_from_empty_entity_in_progress
            },
            {
                .id = "get_type",
                .function = Type_get_type
            },
            {
                .id = "get_type_from_empty",
                .function = Type_get_type_from_empty
            },
            {
                .id = "get_type_from_0",
                .function = Type_get_type_from_0
            },
            {
                .id = "entity_from_type",
                .function = Type_entity_from_type
            },
            {
                .id = "entity_from_empty_type",
                .function = Type_entity_from_empty_type
            },
            {
                .id = "entity_from_type_w_2_elements",
                .function = Type_entity_from_type_w_2_elements
            },
            {
                .id = "type_from_entity",
                .function = Type_type_from_entity
            },
            {
                .id = "type_from_empty",
                .function = Type_type_from_empty
            },
            {
                .id = "type_from_0",
                .function = Type_type_from_0
            },
            {
                .id = "type_to_expr_1_comp",
                .function = Type_type_to_expr_1_comp
            },
            {
                .id = "type_to_expr_2_comp",
                .function = Type_type_to_expr_2_comp
            },
            {
                .id = "type_to_expr_instanceof",
                .function = Type_type_to_expr_instanceof
            },
            {
                .id = "type_to_expr_childof",
                .function = Type_type_to_expr_childof
            }
        }
    },
    {
        .id = "Run",
        .testcase_count = 24,
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
            },
            {
                .id = "run_comb_10_entities_1_type",
                .function = Run_run_comb_10_entities_1_type
            },
            {
                .id = "run_comb_10_entities_2_types",
                .function = Run_run_comb_10_entities_2_types
            },
            {
                .id = "run_w_interrupt",
                .function = Run_run_w_interrupt
            },
            {
                .id = "run_staging",
                .function = Run_run_staging
            }
        }
    },
    {
        .id = "MultiThread",
        .testcase_count = 34,
        .testcases = (bake_test_case[]){
            {
                .id = "2_thread_1_entity",
                .function = MultiThread_2_thread_1_entity
            },
            {
                .id = "2_thread_2_entity",
                .function = MultiThread_2_thread_2_entity
            },
            {
                .id = "2_thread_5_entity",
                .function = MultiThread_2_thread_5_entity
            },
            {
                .id = "2_thread_10_entity",
                .function = MultiThread_2_thread_10_entity
            },
            {
                .id = "3_thread_1_entity",
                .function = MultiThread_3_thread_1_entity
            },
            {
                .id = "3_thread_2_entity",
                .function = MultiThread_3_thread_2_entity
            },
            {
                .id = "3_thread_5_entity",
                .function = MultiThread_3_thread_5_entity
            },
            {
                .id = "3_thread_10_entity",
                .function = MultiThread_3_thread_10_entity
            },
            {
                .id = "4_thread_1_entity",
                .function = MultiThread_4_thread_1_entity
            },
            {
                .id = "4_thread_2_entity",
                .function = MultiThread_4_thread_2_entity
            },
            {
                .id = "4_thread_5_entity",
                .function = MultiThread_4_thread_5_entity
            },
            {
                .id = "4_thread_10_entity",
                .function = MultiThread_4_thread_10_entity
            },
            {
                .id = "5_thread_1_entity",
                .function = MultiThread_5_thread_1_entity
            },
            {
                .id = "5_thread_2_entity",
                .function = MultiThread_5_thread_2_entity
            },
            {
                .id = "5_thread_5_entity",
                .function = MultiThread_5_thread_5_entity
            },
            {
                .id = "5_thread_10_entity",
                .function = MultiThread_5_thread_10_entity
            },
            {
                .id = "6_thread_1_entity",
                .function = MultiThread_6_thread_1_entity
            },
            {
                .id = "6_thread_2_entity",
                .function = MultiThread_6_thread_2_entity
            },
            {
                .id = "6_thread_5_entity",
                .function = MultiThread_6_thread_5_entity
            },
            {
                .id = "6_thread_10_entity",
                .function = MultiThread_6_thread_10_entity
            },
            {
                .id = "2_thread_test_combs_100_entity",
                .function = MultiThread_2_thread_test_combs_100_entity
            },
            {
                .id = "3_thread_test_combs_100_entity",
                .function = MultiThread_3_thread_test_combs_100_entity
            },
            {
                .id = "4_thread_test_combs_100_entity",
                .function = MultiThread_4_thread_test_combs_100_entity
            },
            {
                .id = "5_thread_test_combs_100_entity",
                .function = MultiThread_5_thread_test_combs_100_entity
            },
            {
                .id = "6_thread_test_combs_100_entity",
                .function = MultiThread_6_thread_test_combs_100_entity
            },
            {
                .id = "2_thread_test_combs_100_entity_2_types",
                .function = MultiThread_2_thread_test_combs_100_entity_2_types
            },
            {
                .id = "3_thread_test_combs_100_entity_2_types",
                .function = MultiThread_3_thread_test_combs_100_entity_2_types
            },
            {
                .id = "4_thread_test_combs_100_entity_2_types",
                .function = MultiThread_4_thread_test_combs_100_entity_2_types
            },
            {
                .id = "5_thread_test_combs_100_entity_2_types",
                .function = MultiThread_5_thread_test_combs_100_entity_2_types
            },
            {
                .id = "6_thread_test_combs_100_entity_2_types",
                .function = MultiThread_6_thread_test_combs_100_entity_2_types
            },
            {
                .id = "change_thread_count",
                .function = MultiThread_change_thread_count
            },
            {
                .id = "multithread_quit",
                .function = MultiThread_multithread_quit
            },
            {
                .id = "schedule_w_tasks",
                .function = MultiThread_schedule_w_tasks
            },
            {
                .id = "reactive_system",
                .function = MultiThread_reactive_system
            }
        }
    },
    {
        .id = "SingleThreadStaging",
        .testcase_count = 65,
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
                .id = "new_empty_w_count",
                .function = SingleThreadStaging_new_empty_w_count
            },
            {
                .id = "new_component_w_count",
                .function = SingleThreadStaging_new_component_w_count
            },
            {
                .id = "new_type_w_count",
                .function = SingleThreadStaging_new_type_w_count
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
                .id = "add_remove_add_same_to_current",
                .function = SingleThreadStaging_add_remove_add_same_to_current
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
                .id = "on_add_in_on_update",
                .function = SingleThreadStaging_on_add_in_on_update
            },
            {
                .id = "on_remove_in_on_update",
                .function = SingleThreadStaging_on_remove_in_on_update
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
            },
            {
                .id = "merge_table_w_container_added_on_set",
                .function = SingleThreadStaging_merge_table_w_container_added_on_set
            },
            {
                .id = "merge_table_w_container_added_on_set_reverse",
                .function = SingleThreadStaging_merge_table_w_container_added_on_set_reverse
            },
            {
                .id = "merge_after_tasks",
                .function = SingleThreadStaging_merge_after_tasks
            },
            {
                .id = "override_after_remove_in_progress",
                .function = SingleThreadStaging_override_after_remove_in_progress
            },
            {
                .id = "get_parent_in_progress",
                .function = SingleThreadStaging_get_parent_in_progress
            }
        }
    },
    {
        .id = "MultiThreadStaging",
        .testcase_count = 9,
        .testcases = (bake_test_case[]){
            {
                .id = "2_threads_add_to_current",
                .function = MultiThreadStaging_2_threads_add_to_current
            },
            {
                .id = "3_threads_add_to_current",
                .function = MultiThreadStaging_3_threads_add_to_current
            },
            {
                .id = "4_threads_add_to_current",
                .function = MultiThreadStaging_4_threads_add_to_current
            },
            {
                .id = "5_threads_add_to_current",
                .function = MultiThreadStaging_5_threads_add_to_current
            },
            {
                .id = "6_threads_add_to_current",
                .function = MultiThreadStaging_6_threads_add_to_current
            },
            {
                .id = "stress_create_delete_entity_random_components",
                .function = MultiThreadStaging_stress_create_delete_entity_random_components
            },
            {
                .id = "stress_set_entity_random_components",
                .function = MultiThreadStaging_stress_set_entity_random_components
            },
            {
                .id = "2_threads_on_add",
                .function = MultiThreadStaging_2_threads_on_add
            },
            {
                .id = "new_w_count",
                .function = MultiThreadStaging_new_w_count
            }
        }
    },
    {
        .id = "Snapshot",
        .testcase_count = 17,
        .testcases = (bake_test_case[]){
            {
                .id = "simple_snapshot",
                .function = Snapshot_simple_snapshot
            },
            {
                .id = "snapshot_after_new",
                .function = Snapshot_snapshot_after_new
            },
            {
                .id = "snapshot_after_delete",
                .function = Snapshot_snapshot_after_delete
            },
            {
                .id = "snapshot_after_new_type",
                .function = Snapshot_snapshot_after_new_type
            },
            {
                .id = "snapshot_after_add",
                .function = Snapshot_snapshot_after_add
            },
            {
                .id = "snapshot_after_remove",
                .function = Snapshot_snapshot_after_remove
            },
            {
                .id = "snapshot_w_include_filter",
                .function = Snapshot_snapshot_w_include_filter
            },
            {
                .id = "snapshot_w_exclude_filter",
                .function = Snapshot_snapshot_w_exclude_filter
            },
            {
                .id = "snapshot_w_filter_after_new",
                .function = Snapshot_snapshot_w_filter_after_new
            },
            {
                .id = "snapshot_w_filter_after_delete",
                .function = Snapshot_snapshot_w_filter_after_delete
            },
            {
                .id = "snapshot_free_empty",
                .function = Snapshot_snapshot_free_empty
            },
            {
                .id = "snapshot_free",
                .function = Snapshot_snapshot_free
            },
            {
                .id = "snapshot_free_filtered",
                .function = Snapshot_snapshot_free_filtered
            },
            {
                .id = "snapshot_activate_table_w_filter",
                .function = Snapshot_snapshot_activate_table_w_filter
            },
            {
                .id = "snapshot_copy",
                .function = Snapshot_snapshot_copy
            },
            {
                .id = "snapshot_copy_filtered",
                .function = Snapshot_snapshot_copy_filtered
            },
            {
                .id = "snapshot_copy_w_filter",
                .function = Snapshot_snapshot_copy_w_filter
            }
        }
    },
    {
        .id = "ReaderWriter",
        .testcase_count = 23,
        .testcases = (bake_test_case[]){
            {
                .id = "simple",
                .function = ReaderWriter_simple
            },
            {
                .id = "id",
                .function = ReaderWriter_id
            },
            {
                .id = "id_w_simple",
                .function = ReaderWriter_id_w_simple
            },
            {
                .id = "unaligned",
                .function = ReaderWriter_unaligned
            },
            {
                .id = "empty",
                .function = ReaderWriter_empty
            },
            {
                .id = "tag",
                .function = ReaderWriter_tag
            },
            {
                .id = "simple_w_tag",
                .function = ReaderWriter_simple_w_tag
            },
            {
                .id = "tag_w_simple",
                .function = ReaderWriter_tag_w_simple
            },
            {
                .id = "empty_parent",
                .function = ReaderWriter_empty_parent
            },
            {
                .id = "parent",
                .function = ReaderWriter_parent
            },
            {
                .id = "simple_w_parent",
                .function = ReaderWriter_simple_w_parent
            },
            {
                .id = "inheritance",
                .function = ReaderWriter_inheritance
            },
            {
                .id = "simple_w_inheritance",
                .function = ReaderWriter_simple_w_inheritance
            },
            {
                .id = "deserialize_twice",
                .function = ReaderWriter_deserialize_twice
            },
            {
                .id = "entity_conflict",
                .function = ReaderWriter_entity_conflict
            },
            {
                .id = "snapshot_reader_simple",
                .function = ReaderWriter_snapshot_reader_simple
            },
            {
                .id = "snapshot_reader_id",
                .function = ReaderWriter_snapshot_reader_id
            },
            {
                .id = "component_id_conflict_w_component",
                .function = ReaderWriter_component_id_conflict_w_component
            },
            {
                .id = "component_id_conflict_w_entity",
                .function = ReaderWriter_component_id_conflict_w_entity
            },
            {
                .id = "component_size_conflict",
                .function = ReaderWriter_component_size_conflict
            },
            {
                .id = "read_zero_size",
                .function = ReaderWriter_read_zero_size
            },
            {
                .id = "write_zero_size",
                .function = ReaderWriter_write_zero_size
            },
            {
                .id = "invalid_header",
                .function = ReaderWriter_invalid_header
            }
        }
    },
    {
        .id = "FilterIter",
        .testcase_count = 7,
        .testcases = (bake_test_case[]){
            {
                .id = "iter_one_table",
                .function = FilterIter_iter_one_table
            },
            {
                .id = "iter_two_tables",
                .function = FilterIter_iter_two_tables
            },
            {
                .id = "iter_two_comps",
                .function = FilterIter_iter_two_comps
            },
            {
                .id = "iter_snapshot_one_table",
                .function = FilterIter_iter_snapshot_one_table
            },
            {
                .id = "iter_snapshot_two_tables",
                .function = FilterIter_iter_snapshot_two_tables
            },
            {
                .id = "iter_snapshot_two_comps",
                .function = FilterIter_iter_snapshot_two_comps
            },
            {
                .id = "iter_snapshot_filtered_table",
                .function = FilterIter_iter_snapshot_filtered_table
            }
        }
    },
    {
        .id = "Modules",
        .testcase_count = 5,
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
            },
            {
                .id = "import_again",
                .function = Modules_import_again
            }
        }
    },
    {
        .id = "Internals",
        .testcase_count = 6,
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
            },
            {
                .id = "no_double_system_table_after_merge",
                .function = Internals_no_double_system_table_after_merge
            }
        }
    },
    {
        .id = "Error",
        .testcase_count = 11,
        .setup = Error_setup,
        .testcases = (bake_test_case[]){
            {
                .id = "abort",
                .function = Error_abort
            },
            {
                .id = "abort_w_param",
                .function = Error_abort_w_param
            },
            {
                .id = "override_abort",
                .function = Error_override_abort
            },
            {
                .id = "assert_true",
                .function = Error_assert_true
            },
            {
                .id = "assert_false",
                .function = Error_assert_false
            },
            {
                .id = "assert_false_w_param",
                .function = Error_assert_false_w_param
            },
            {
                .id = "error_codes",
                .function = Error_error_codes
            },
            {
                .id = "log_dbg",
                .function = Error_log_dbg
            },
            {
                .id = "log_log",
                .function = Error_log_log
            },
            {
                .id = "log_warning",
                .function = Error_log_warning
            },
            {
                .id = "log_error",
                .function = Error_log_error
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("api", argc, argv, suites, 42);
}
