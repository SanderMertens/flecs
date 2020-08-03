
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
void New_recycle_id_empty(void);
void New_recycle_id_w_entity(void);
void New_recycle_id_w_type(void);
void New_recycle_empty_staged_delete(void);
void New_recycle_staged_delete(void);
void New_new_id(void);
void New_new_component_id(void);
void New_new_hi_component_id(void);

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
void New_w_Count_new_w_on_add_on_set_monitor(void);

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
void Add_add_entity(void);
void Add_remove_entity(void);
void Add_add_0_entity(void);
void Add_remove_0_entity(void);
void Add_add_w_xor(void);
void Add_add_same_w_xor(void);
void Add_add_after_remove_xor(void);

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

// Testsuite 'Hierarchies'
void Hierarchies_empty_scope(void);
void Hierarchies_get_parent(void);
void Hierarchies_get_parent_from_nested(void);
void Hierarchies_get_parent_from_nested_2(void);
void Hierarchies_get_parent_from_root(void);
void Hierarchies_tree_iter_empty(void);
void Hierarchies_tree_iter_1_table(void);
void Hierarchies_tree_iter_2_tables(void);
void Hierarchies_path_depth_0(void);
void Hierarchies_path_depth_1(void);
void Hierarchies_path_depth_2(void);
void Hierarchies_rel_path_from_root(void);
void Hierarchies_rel_path_from_self(void);
void Hierarchies_rel_path_depth_1(void);
void Hierarchies_rel_path_depth_2(void);
void Hierarchies_rel_path_no_match(void);
void Hierarchies_path_for_component_0(void);
void Hierarchies_path_for_component_1(void);
void Hierarchies_path_for_component_2(void);
void Hierarchies_path_custom_sep(void);
void Hierarchies_path_custom_prefix(void);
void Hierarchies_path_prefix_rel_match(void);
void Hierarchies_path_prefix_rel_no_match(void);
void Hierarchies_fullpath_for_core(void);
void Hierarchies_path_w_number(void);
void Hierarchies_lookup_depth_0(void);
void Hierarchies_lookup_depth_1(void);
void Hierarchies_lookup_depth_2(void);
void Hierarchies_lookup_rel_0(void);
void Hierarchies_lookup_rel_1(void);
void Hierarchies_lookup_rel_2(void);
void Hierarchies_lookup_custom_sep(void);
void Hierarchies_lookup_custom_prefix(void);
void Hierarchies_lookup_custom_prefix_from_root(void);
void Hierarchies_lookup_self(void);
void Hierarchies_lookup_in_parent_from_scope(void);
void Hierarchies_lookup_in_root_from_scope(void);
void Hierarchies_lookup_number(void);
void Hierarchies_delete_children(void);
void Hierarchies_scope_set(void);
void Hierarchies_scope_set_again(void);
void Hierarchies_scope_set_w_new(void);
void Hierarchies_scope_set_w_new_staged(void);
void Hierarchies_scope_set_w_lookup(void);
void Hierarchies_scope_component(void);
void Hierarchies_new_from_path_depth_0(void);
void Hierarchies_new_from_path_depth_1(void);
void Hierarchies_new_from_path_depth_2(void);
void Hierarchies_new_from_path_existing_depth_0(void);
void Hierarchies_new_from_path_existing_depth_1(void);
void Hierarchies_new_from_path_existing_depth_2(void);
void Hierarchies_add_path_depth_0(void);
void Hierarchies_add_path_depth_1(void);
void Hierarchies_add_path_depth_2(void);
void Hierarchies_add_path_existing_depth_0(void);
void Hierarchies_add_path_existing_depth_1(void);
void Hierarchies_add_path_existing_depth_2(void);
void Hierarchies_new_w_child_in_root(void);

// Testsuite 'Add_bulk'
void Add_bulk_add_comp_from_comp_to_empty(void);
void Add_bulk_add_comp_from_comp_to_existing(void);
void Add_bulk_add_comp_from_tag_to_empty(void);
void Add_bulk_add_comp_from_tag_to_existing(void);
void Add_bulk_add_tag_from_tag_to_empty(void);
void Add_bulk_add_tag_from_tag_to_existing(void);
void Add_bulk_add_comp_to_more_existing(void);
void Add_bulk_add_comp_to_fewer_existing(void);
void Add_bulk_on_add(void);
void Add_bulk_add_entity_comp(void);
void Add_bulk_add_entity_tag(void);
void Add_bulk_add_entity_on_add(void);
void Add_bulk_add_entity_existing(void);

// Testsuite 'Remove_bulk'
void Remove_bulk_remove_comp_from_comp_to_empty(void);
void Remove_bulk_remove_comp_from_comp_to_existing(void);
void Remove_bulk_remove_comp_from_tag_to_empty(void);
void Remove_bulk_remove_comp_from_tag_to_existing(void);
void Remove_bulk_remove_tag_from_tag_to_empty(void);
void Remove_bulk_remove_tag_from_tag_to_existing(void);
void Remove_bulk_remove_all_comp(void);
void Remove_bulk_remove_all_tag(void);
void Remove_bulk_on_remove(void);
void Remove_bulk_remove_entity_comp(void);
void Remove_bulk_remove_entity_tag(void);
void Remove_bulk_remove_entity_on_remove(void);

// Testsuite 'Add_remove_bulk'
void Add_remove_bulk_add_remove_add_only(void);
void Add_remove_bulk_add_remove_remove_only(void);
void Add_remove_bulk_add_remove_both(void);
void Add_remove_bulk_add_remove_same(void);

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
void Has_has_in_progress(void);
void Has_has_of_zero(void);
void Has_owns(void);
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
void Set_get_mutable_new(void);
void Set_get_mutable_existing(void);
void Set_modified_w_on_set(void);

// Testsuite 'Lookup'
void Lookup_lookup(void);
void Lookup_lookup_component(void);
void Lookup_lookup_not_found(void);
void Lookup_lookup_child(void);
void Lookup_lookup_in_progress(void);
void Lookup_lookup_child_in_progress(void);
void Lookup_lookup_w_null_name(void);
void Lookup_get_name(void);
void Lookup_get_name_no_name(void);
void Lookup_get_name_from_empty(void);
void Lookup_lookup_by_id(void);

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

// Testsuite 'ComponentLifecycle'
void ComponentLifecycle_ctor_on_add(void);
void ComponentLifecycle_ctor_on_new(void);
void ComponentLifecycle_dtor_on_remove(void);
void ComponentLifecycle_dtor_on_delete(void);
void ComponentLifecycle_copy_on_set(void);
void ComponentLifecycle_copy_on_override(void);
void ComponentLifecycle_copy_on_new_w_data(void);
void ComponentLifecycle_copy_on_clone(void);
void ComponentLifecycle_copy_on_stage(void);
void ComponentLifecycle_no_copy_on_move(void);
void ComponentLifecycle_move_on_merge(void);
void ComponentLifecycle_ctor_on_bulk_add(void);
void ComponentLifecycle_dtor_on_bulk_remove(void);
void ComponentLifecycle_ctor_on_bulk_add_entity(void);
void ComponentLifecycle_dtor_on_bulk_remove_entity(void);
void ComponentLifecycle_ctor_dtor_on_bulk_add_remove(void);
void ComponentLifecycle_ctor_copy_on_snapshot(void);
void ComponentLifecycle_copy_on_snapshot(void);
void ComponentLifecycle_dtor_on_restore(void);
void ComponentLifecycle_ctor_on_tag(void);
void ComponentLifecycle_dtor_on_tag(void);
void ComponentLifecycle_copy_on_tag(void);
void ComponentLifecycle_move_on_tag(void);

// Testsuite 'Pipeline'
void Pipeline_system_order_same_phase(void);
void Pipeline_system_order_same_phase_after_disable(void);
void Pipeline_system_order_same_phase_after_activate(void);
void Pipeline_system_order_different_phase(void);
void Pipeline_system_order_different_phase_after_disable(void);
void Pipeline_system_order_different_phase_after_activate(void);
void Pipeline_system_order_after_new_system_lower_id(void);
void Pipeline_system_order_after_new_system_inbetween_id(void);
void Pipeline_system_order_after_new_system_higher_id(void);
void Pipeline_merge_after_staged_out(void);
void Pipeline_merge_after_not_out(void);
void Pipeline_no_merge_after_main_out(void);
void Pipeline_no_merge_after_staged_in_out(void);
void Pipeline_merge_after_staged_out_before_owned(void);
void Pipeline_switch_pipeline(void);

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
void SystemMisc_dont_enable_after_rematch(void);
void SystemMisc_ensure_single_merge(void);
void SystemMisc_table_count(void);
void SystemMisc_match_system(void);
void SystemMisc_match_system_w_filter(void);
void SystemMisc_system_initial_state(void);
void SystemMisc_add_own_component(void);
void SystemMisc_change_system_action(void);
void SystemMisc_system_readeactivate(void);
void SystemMisc_system_readeactivate_w_2_systems(void);

// Testsuite 'Sorting'
void Sorting_sort_by_component(void);
void Sorting_sort_by_component_2_tables(void);
void Sorting_sort_by_component_3_tables(void);
void Sorting_sort_by_entity(void);
void Sorting_sort_after_add(void);
void Sorting_sort_after_remove(void);
void Sorting_sort_after_delete(void);
void Sorting_sort_after_set(void);
void Sorting_sort_after_system(void);
void Sorting_sort_by_component_same_value_1(void);
void Sorting_sort_by_component_same_value_2(void);
void Sorting_sort_by_component_move_pivot(void);
void Sorting_sort_1000_entities(void);

// Testsuite 'Queries'
void Queries_query_changed_after_new(void);
void Queries_query_changed_after_delete(void);
void Queries_query_changed_after_add(void);
void Queries_query_changed_after_remove(void);
void Queries_query_changed_after_set(void);
void Queries_query_change_after_out_system(void);
void Queries_query_change_after_in_system(void);

// Testsuite 'Traits'
void Traits_type_w_one_trait(void);
void Traits_type_w_two_traits(void);
void Traits_add_trait(void);
void Traits_remove_trait(void);
void Traits_add_tag_trait_for_tag(void);
void Traits_add_tag_trait_for_component(void);
void Traits_query_2_traits(void);
void Traits_query_2_traits_2_instances_per_type(void);

// Testsuite 'TriggerOnAdd'
void TriggerOnAdd_new_match_1_of_1(void);
void TriggerOnAdd_new_match_1_of_2(void);
void TriggerOnAdd_new_no_match_1(void);
void TriggerOnAdd_new_w_count_match_1_of_1(void);
void TriggerOnAdd_add_match_1_of_1(void);
void TriggerOnAdd_add_match_1_of_2(void);
void TriggerOnAdd_add_no_match_1(void);
void TriggerOnAdd_set_match_1_of_1(void);
void TriggerOnAdd_set_no_match_1(void);
void TriggerOnAdd_clone_match_1_of_1(void);
void TriggerOnAdd_clone_match_1_of_2(void);
void TriggerOnAdd_add_again_1(void);
void TriggerOnAdd_set_again_1(void);
void TriggerOnAdd_add_again_2(void);
void TriggerOnAdd_override_after_add_in_on_add(void);
void TriggerOnAdd_add_again_in_progress(void);
void TriggerOnAdd_add_in_progress_before_system_def(void);
void TriggerOnAdd_2_systems_w_table_creation(void);
void TriggerOnAdd_2_systems_w_table_creation_in_progress(void);
void TriggerOnAdd_sys_context(void);
void TriggerOnAdd_get_sys_context_from_param(void);
void TriggerOnAdd_remove_added_component_in_on_add_w_set(void);
void TriggerOnAdd_on_add_in_on_add(void);
void TriggerOnAdd_on_remove_in_on_add(void);
void TriggerOnAdd_on_set_in_on_add(void);
void TriggerOnAdd_on_add_in_on_update(void);

// Testsuite 'TriggerOnRemove'
void TriggerOnRemove_remove_match_1_of_1(void);
void TriggerOnRemove_remove_match_1_of_2(void);
void TriggerOnRemove_remove_no_match_1(void);
void TriggerOnRemove_delete_match_1_of_1(void);
void TriggerOnRemove_delete_match_1_of_2(void);
void TriggerOnRemove_delete_no_match_1(void);
void TriggerOnRemove_remove_watched(void);
void TriggerOnRemove_delete_watched(void);

// Testsuite 'TriggerOnSet'
void TriggerOnSet_set(void);
void TriggerOnSet_set_new(void);
void TriggerOnSet_set_again(void);
void TriggerOnSet_clone(void);
void TriggerOnSet_clone_w_value(void);
void TriggerOnSet_set_and_add_system(void);
void TriggerOnSet_on_set_after_override(void);
void TriggerOnSet_on_set_after_override_w_new(void);
void TriggerOnSet_on_set_after_override_w_new_w_count(void);
void TriggerOnSet_on_set_after_override_1_of_2_overridden(void);
void TriggerOnSet_on_set_after_snapshot_restore(void);

// Testsuite 'Monitor'
void Monitor_1_comp(void);
void Monitor_2_comps(void);
void Monitor_1_comp_1_not(void);
void Monitor_1_parent(void);
void Monitor_1_comp_1_parent(void);

// Testsuite 'SystemOnSet'
void SystemOnSet_set_1_of_1(void);
void SystemOnSet_set_1_of_2(void);
void SystemOnSet_set_1_of_3(void);
void SystemOnSet_bulk_new_1(void);
void SystemOnSet_bulk_new_2(void);
void SystemOnSet_bulk_new_2_of_1(void);
void SystemOnSet_bulk_new_3(void);
void SystemOnSet_bulk_new_3_of_2(void);
void SystemOnSet_bulk_new_1_from_base(void);
void SystemOnSet_set_1_of_2_1_from_base(void);
void SystemOnSet_set_1_of_3_1_from_base(void);
void SystemOnSet_add_base(void);
void SystemOnSet_add_base_to_1_overridden(void);
void SystemOnSet_add_base_to_2_overridden(void);
void SystemOnSet_add_base_to_1_of_2_overridden(void);
void SystemOnSet_on_set_after_remove_override(void);
void SystemOnSet_no_set_after_remove_base(void);
void SystemOnSet_un_set_after_remove(void);
void SystemOnSet_un_set_after_remove_base(void);
void SystemOnSet_add_to_current_in_on_set(void);
void SystemOnSet_remove_from_current_in_on_set(void);
void SystemOnSet_remove_set_component_in_on_set(void);
void SystemOnSet_match_table_created_w_add_in_on_set(void);
void SystemOnSet_set_optional(void);
void SystemOnSet_set_from_nothing(void);
void SystemOnSet_add_null_type_in_on_set(void);
void SystemOnSet_add_0_entity_in_on_set(void);

// Testsuite 'SystemPeriodic'
void SystemPeriodic_1_type_1_component(void);
void SystemPeriodic_1_type_3_component(void);
void SystemPeriodic_3_type_1_component(void);
void SystemPeriodic_2_type_3_component(void);
void SystemPeriodic_1_type_1_component_1_tag(void);
void SystemPeriodic_2_type_1_component_1_tag(void);
void SystemPeriodic_2_type_1_and_1_not(void);
void SystemPeriodic_2_type_2_and_1_not(void);
void SystemPeriodic_2_type_2_and_2_not(void);
void SystemPeriodic_4_type_1_and_1_or(void);
void SystemPeriodic_4_type_1_and_1_or_of_3(void);
void SystemPeriodic_1_type_1_and_1_or(void);
void SystemPeriodic_2_type_1_and_1_optional(void);
void SystemPeriodic_2_type_2_and_1_optional(void);
void SystemPeriodic_6_type_1_and_2_optional(void);
void SystemPeriodic_ensure_optional_is_unset_column(void);
void SystemPeriodic_ensure_optional_is_null_shared(void);
void SystemPeriodic_ensure_optional_is_null_field_owned(void);
void SystemPeriodic_ensure_optional_is_null_field_shared(void);
void SystemPeriodic_use_fields_2_owned(void);
void SystemPeriodic_use_fields_1_owned_1_shared(void);
void SystemPeriodic_match_2_systems_w_populated_table(void);
void SystemPeriodic_on_period(void);
void SystemPeriodic_on_period_long_delta(void);
void SystemPeriodic_disabled(void);
void SystemPeriodic_disabled_feature(void);
void SystemPeriodic_disabled_nested_feature(void);
void SystemPeriodic_two_refs(void);
void SystemPeriodic_filter_disabled(void);
void SystemPeriodic_match_disabled(void);
void SystemPeriodic_match_disabled_and_enabled(void);
void SystemPeriodic_match_prefab(void);
void SystemPeriodic_match_prefab_and_normal(void);
void SystemPeriodic_is_shared_on_column_not_set(void);
void SystemPeriodic_owned_column(void);
void SystemPeriodic_owned_not_column(void);
void SystemPeriodic_owned_or_column(void);
void SystemPeriodic_shared_column(void);
void SystemPeriodic_shared_not_column(void);
void SystemPeriodic_shared_or_column(void);
void SystemPeriodic_container_dont_match_inheritance(void);
void SystemPeriodic_cascade_dont_match_inheritance(void);
void SystemPeriodic_not_from_singleton(void);
void SystemPeriodic_not_from_entity(void);
void SystemPeriodic_sys_context(void);
void SystemPeriodic_get_sys_context_from_param(void);
void SystemPeriodic_use_field_w_0_size(void);
void SystemPeriodic_owned_only(void);
void SystemPeriodic_shared_only(void);
void SystemPeriodic_is_in_readonly(void);
void SystemPeriodic_get_period(void);

// Testsuite 'Timer'
void Timer_timeout(void);
void Timer_interval(void);
void Timer_shared_timeout(void);
void Timer_shared_interval(void);
void Timer_start_stop_one_shot(void);
void Timer_start_stop_interval(void);
void Timer_rate_filter(void);
void Timer_rate_filter_w_rate_filter_src(void);
void Timer_rate_filter_w_timer_src(void);

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
void SystemOnDemand_enable_after_user_disable(void);

// Testsuite 'SystemCascade'
void SystemCascade_cascade_depth_1(void);
void SystemCascade_cascade_depth_2(void);
void SystemCascade_add_after_match(void);
void SystemCascade_adopt_after_match(void);

// Testsuite 'SystemManual'
void SystemManual_1_type_1_component(void);
void SystemManual_activate_status(void);
void SystemManual_no_automerge(void);

// Testsuite 'Tasks'
void Tasks_no_components(void);
void Tasks_one_tag(void);
void Tasks_from_system(void);
void Tasks_tasks_in_phases(void);

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
void Prefab_ref_after_realloc(void);
void Prefab_revalidate_ref_w_mixed_table_refs(void);
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
void Prefab_clone_after_inherit_in_on_add(void);
void Prefab_override_from_nested(void);
void Prefab_create_multiple_nested_w_on_set(void);
void Prefab_create_multiple_nested_w_on_set_in_progress(void);
void Prefab_single_on_set_on_child_w_override(void);

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
void World_measure_fps_vs_actual(void);
void World_measure_delta_time_vs_actual(void);
void World_world_stats(void);
void World_quit(void);
void World_get_delta_time(void);
void World_get_delta_time_auto(void);
void World_recreate_world(void);
void World_recreate_world_w_component(void);
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
void SingleThreadStaging_remove_added_component_in_on_add(void);
void SingleThreadStaging_match_table_created_in_progress(void);
void SingleThreadStaging_match_table_created_w_new_in_progress(void);
void SingleThreadStaging_match_table_created_w_new_in_on_set(void);
void SingleThreadStaging_merge_table_w_container_added_in_progress(void);
void SingleThreadStaging_merge_table_w_container_added_on_set(void);
void SingleThreadStaging_merge_table_w_container_added_on_set_reverse(void);
void SingleThreadStaging_merge_after_tasks(void);
void SingleThreadStaging_override_after_remove_in_progress(void);
void SingleThreadStaging_get_parent_in_progress(void);
void SingleThreadStaging_merge_once(void);
void SingleThreadStaging_get_mutable(void);
void SingleThreadStaging_get_mutable_from_main(void);
void SingleThreadStaging_get_mutable_w_add(void);
void SingleThreadStaging_on_add_after_new_type_in_progress(void);

// Testsuite 'MultiThreadStaging'
void MultiThreadStaging_2_threads_add_to_current(void);
void MultiThreadStaging_3_threads_add_to_current(void);
void MultiThreadStaging_4_threads_add_to_current(void);
void MultiThreadStaging_5_threads_add_to_current(void);
void MultiThreadStaging_6_threads_add_to_current(void);
void MultiThreadStaging_2_threads_on_add(void);
void MultiThreadStaging_new_w_count(void);

// Testsuite 'Stresstests'
void Stresstests_create_delete_entity_random_components(void);
void Stresstests_set_entity_random_components(void);
void Stresstests_create_delete_entity_random_components_staged(void);
void Stresstests_set_entity_random_components_staged(void);
void Stresstests_create_delete_entity_random_components_2_threads(void);
void Stresstests_set_entity_random_components_2_threads(void);
void Stresstests_create_delete_entity_random_components_6_threads(void);
void Stresstests_set_entity_random_components_6_threads(void);
void Stresstests_create_delete_entity_random_components_12_threads(void);
void Stresstests_set_entity_random_components_12_threads(void);
void Stresstests_create_2m_entities_1_comp(void);
void Stresstests_create_2m_entities_bulk_1_comp(void);
void Stresstests_add_1k_tags(void);

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
void Snapshot_snapshot_get_ref_after_restore(void);
void Snapshot_new_after_snapshot(void);
void Snapshot_new_empty_after_snapshot(void);
void Snapshot_add_after_snapshot(void);
void Snapshot_delete_after_snapshot(void);
void Snapshot_set_after_snapshot(void);
void Snapshot_restore_recycled(void);
void Snapshot_snapshot_w_new_in_onset(void);
void Snapshot_snapshot_w_new_in_onset_in_snapshot_table(void);

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
void Modules_import_again(void);
void Modules_scoped_component(void);
void Modules_scoped_tag(void);
void Modules_scoped_system(void);
void Modules_scoped_entity(void);
void Modules_name_prefix_component(void);
void Modules_name_prefix_tag(void);
void Modules_name_prefix_system(void);
void Modules_name_prefix_entity(void);
void Modules_name_prefix_type(void);
void Modules_name_prefix_prefab(void);
void Modules_name_prefix_pipeline(void);
void Modules_name_prefix_trigger(void);
void Modules_name_prefix_underscore(void);
void Modules_lookup_by_symbol(void);

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

bake_test_case New_testcases[] = {
    {
        "empty",
        New_empty
    },
    {
        "component",
        New_component
    },
    {
        "type",
        New_type
    },
    {
        "type_of_2",
        New_type_of_2
    },
    {
        "type_w_type",
        New_type_w_type
    },
    {
        "type_w_2_types",
        New_type_w_2_types
    },
    {
        "type_mixed",
        New_type_mixed
    },
    {
        "tag",
        New_tag
    },
    {
        "type_w_tag",
        New_type_w_tag
    },
    {
        "type_w_2_tags",
        New_type_w_2_tags
    },
    {
        "type_w_tag_mixed",
        New_type_w_tag_mixed
    },
    {
        "redefine_component",
        New_redefine_component
    },
    {
        "recycle_id_empty",
        New_recycle_id_empty
    },
    {
        "recycle_id_w_entity",
        New_recycle_id_w_entity
    },
    {
        "recycle_id_w_type",
        New_recycle_id_w_type
    },
    {
        "recycle_empty_staged_delete",
        New_recycle_empty_staged_delete
    },
    {
        "recycle_staged_delete",
        New_recycle_staged_delete
    },
    {
        "new_id",
        New_new_id
    },
    {
        "new_component_id",
        New_new_component_id
    },
    {
        "new_hi_component_id",
        New_new_hi_component_id
    }
};

bake_test_case New_w_Count_testcases[] = {
    {
        "empty",
        New_w_Count_empty
    },
    {
        "component",
        New_w_Count_component
    },
    {
        "type",
        New_w_Count_type
    },
    {
        "type_of_2",
        New_w_Count_type_of_2
    },
    {
        "type_w_type",
        New_w_Count_type_w_type
    },
    {
        "type_w_2_types",
        New_w_Count_type_w_2_types
    },
    {
        "type_mixed",
        New_w_Count_type_mixed
    },
    {
        "tag",
        New_w_Count_tag
    },
    {
        "type_w_tag",
        New_w_Count_type_w_tag
    },
    {
        "type_w_2_tags",
        New_w_Count_type_w_2_tags
    },
    {
        "type_w_tag_mixed",
        New_w_Count_type_w_tag_mixed
    },
    {
        "new_w_on_add_on_set_monitor",
        New_w_Count_new_w_on_add_on_set_monitor
    }
};

bake_test_case Add_testcases[] = {
    {
        "zero",
        Add_zero
    },
    {
        "component",
        Add_component
    },
    {
        "component_again",
        Add_component_again
    },
    {
        "2_components",
        Add_2_components
    },
    {
        "2_components_again",
        Add_2_components_again
    },
    {
        "2_components_overlap",
        Add_2_components_overlap
    },
    {
        "type",
        Add_type
    },
    {
        "type_of_2",
        Add_type_of_2
    },
    {
        "type_w_type",
        Add_type_w_type
    },
    {
        "type_w_2_types",
        Add_type_w_2_types
    },
    {
        "type_mixed",
        Add_type_mixed
    },
    {
        "type_again",
        Add_type_again
    },
    {
        "type_overlap",
        Add_type_overlap
    },
    {
        "type_again_component",
        Add_type_again_component
    },
    {
        "type_overlap_component",
        Add_type_overlap_component
    },
    {
        "component_to_nonempty",
        Add_component_to_nonempty
    },
    {
        "component_to_nonempty_again",
        Add_component_to_nonempty_again
    },
    {
        "component_to_nonempty_overlap",
        Add_component_to_nonempty_overlap
    },
    {
        "type_to_nonempty",
        Add_type_to_nonempty
    },
    {
        "type_to_nonempty_again",
        Add_type_to_nonempty_again
    },
    {
        "type_to_nonempty_overlap",
        Add_type_to_nonempty_overlap
    },
    {
        "tag",
        Add_tag
    },
    {
        "type_w_tag",
        Add_type_w_tag
    },
    {
        "type_w_2_tags",
        Add_type_w_2_tags
    },
    {
        "type_w_tag_mixed",
        Add_type_w_tag_mixed
    },
    {
        "add_remove",
        Add_add_remove
    },
    {
        "add_remove_same",
        Add_add_remove_same
    },
    {
        "add_2_remove",
        Add_add_2_remove
    },
    {
        "add_entity",
        Add_add_entity
    },
    {
        "remove_entity",
        Add_remove_entity
    },
    {
        "add_0_entity",
        Add_add_0_entity
    },
    {
        "remove_0_entity",
        Add_remove_0_entity
    },
    {
        "add_w_xor",
        Add_add_w_xor
    },
    {
        "add_same_w_xor",
        Add_add_same_w_xor
    },
    {
        "add_after_remove_xor",
        Add_add_after_remove_xor
    }
};

bake_test_case Remove_testcases[] = {
    {
        "zero",
        Remove_zero
    },
    {
        "zero_from_nonzero",
        Remove_zero_from_nonzero
    },
    {
        "1_of_1",
        Remove_1_of_1
    },
    {
        "1_of_1_again",
        Remove_1_of_1_again
    },
    {
        "1_of_2",
        Remove_1_of_2
    },
    {
        "2_of_2",
        Remove_2_of_2
    },
    {
        "2_of_3",
        Remove_2_of_3
    },
    {
        "2_again",
        Remove_2_again
    },
    {
        "2_overlap",
        Remove_2_overlap
    },
    {
        "type_of_1_of_2",
        Remove_type_of_1_of_2
    },
    {
        "type_of_2_of_2",
        Remove_type_of_2_of_2
    },
    {
        "type_of_2_of_3",
        Remove_type_of_2_of_3
    },
    {
        "1_from_empty",
        Remove_1_from_empty
    },
    {
        "type_from_empty",
        Remove_type_from_empty
    },
    {
        "not_added",
        Remove_not_added
    }
};

bake_test_case Hierarchies_testcases[] = {
    {
        "empty_scope",
        Hierarchies_empty_scope
    },
    {
        "get_parent",
        Hierarchies_get_parent
    },
    {
        "get_parent_from_nested",
        Hierarchies_get_parent_from_nested
    },
    {
        "get_parent_from_nested_2",
        Hierarchies_get_parent_from_nested_2
    },
    {
        "get_parent_from_root",
        Hierarchies_get_parent_from_root
    },
    {
        "tree_iter_empty",
        Hierarchies_tree_iter_empty
    },
    {
        "tree_iter_1_table",
        Hierarchies_tree_iter_1_table
    },
    {
        "tree_iter_2_tables",
        Hierarchies_tree_iter_2_tables
    },
    {
        "path_depth_0",
        Hierarchies_path_depth_0
    },
    {
        "path_depth_1",
        Hierarchies_path_depth_1
    },
    {
        "path_depth_2",
        Hierarchies_path_depth_2
    },
    {
        "rel_path_from_root",
        Hierarchies_rel_path_from_root
    },
    {
        "rel_path_from_self",
        Hierarchies_rel_path_from_self
    },
    {
        "rel_path_depth_1",
        Hierarchies_rel_path_depth_1
    },
    {
        "rel_path_depth_2",
        Hierarchies_rel_path_depth_2
    },
    {
        "rel_path_no_match",
        Hierarchies_rel_path_no_match
    },
    {
        "path_for_component_0",
        Hierarchies_path_for_component_0
    },
    {
        "path_for_component_1",
        Hierarchies_path_for_component_1
    },
    {
        "path_for_component_2",
        Hierarchies_path_for_component_2
    },
    {
        "path_custom_sep",
        Hierarchies_path_custom_sep
    },
    {
        "path_custom_prefix",
        Hierarchies_path_custom_prefix
    },
    {
        "path_prefix_rel_match",
        Hierarchies_path_prefix_rel_match
    },
    {
        "path_prefix_rel_no_match",
        Hierarchies_path_prefix_rel_no_match
    },
    {
        "fullpath_for_core",
        Hierarchies_fullpath_for_core
    },
    {
        "path_w_number",
        Hierarchies_path_w_number
    },
    {
        "lookup_depth_0",
        Hierarchies_lookup_depth_0
    },
    {
        "lookup_depth_1",
        Hierarchies_lookup_depth_1
    },
    {
        "lookup_depth_2",
        Hierarchies_lookup_depth_2
    },
    {
        "lookup_rel_0",
        Hierarchies_lookup_rel_0
    },
    {
        "lookup_rel_1",
        Hierarchies_lookup_rel_1
    },
    {
        "lookup_rel_2",
        Hierarchies_lookup_rel_2
    },
    {
        "lookup_custom_sep",
        Hierarchies_lookup_custom_sep
    },
    {
        "lookup_custom_prefix",
        Hierarchies_lookup_custom_prefix
    },
    {
        "lookup_custom_prefix_from_root",
        Hierarchies_lookup_custom_prefix_from_root
    },
    {
        "lookup_self",
        Hierarchies_lookup_self
    },
    {
        "lookup_in_parent_from_scope",
        Hierarchies_lookup_in_parent_from_scope
    },
    {
        "lookup_in_root_from_scope",
        Hierarchies_lookup_in_root_from_scope
    },
    {
        "lookup_number",
        Hierarchies_lookup_number
    },
    {
        "delete_children",
        Hierarchies_delete_children
    },
    {
        "scope_set",
        Hierarchies_scope_set
    },
    {
        "scope_set_again",
        Hierarchies_scope_set_again
    },
    {
        "scope_set_w_new",
        Hierarchies_scope_set_w_new
    },
    {
        "scope_set_w_new_staged",
        Hierarchies_scope_set_w_new_staged
    },
    {
        "scope_set_w_lookup",
        Hierarchies_scope_set_w_lookup
    },
    {
        "scope_component",
        Hierarchies_scope_component
    },
    {
        "new_from_path_depth_0",
        Hierarchies_new_from_path_depth_0
    },
    {
        "new_from_path_depth_1",
        Hierarchies_new_from_path_depth_1
    },
    {
        "new_from_path_depth_2",
        Hierarchies_new_from_path_depth_2
    },
    {
        "new_from_path_existing_depth_0",
        Hierarchies_new_from_path_existing_depth_0
    },
    {
        "new_from_path_existing_depth_1",
        Hierarchies_new_from_path_existing_depth_1
    },
    {
        "new_from_path_existing_depth_2",
        Hierarchies_new_from_path_existing_depth_2
    },
    {
        "add_path_depth_0",
        Hierarchies_add_path_depth_0
    },
    {
        "add_path_depth_1",
        Hierarchies_add_path_depth_1
    },
    {
        "add_path_depth_2",
        Hierarchies_add_path_depth_2
    },
    {
        "add_path_existing_depth_0",
        Hierarchies_add_path_existing_depth_0
    },
    {
        "add_path_existing_depth_1",
        Hierarchies_add_path_existing_depth_1
    },
    {
        "add_path_existing_depth_2",
        Hierarchies_add_path_existing_depth_2
    },
    {
        "new_w_child_in_root",
        Hierarchies_new_w_child_in_root
    }
};

bake_test_case Add_bulk_testcases[] = {
    {
        "add_comp_from_comp_to_empty",
        Add_bulk_add_comp_from_comp_to_empty
    },
    {
        "add_comp_from_comp_to_existing",
        Add_bulk_add_comp_from_comp_to_existing
    },
    {
        "add_comp_from_tag_to_empty",
        Add_bulk_add_comp_from_tag_to_empty
    },
    {
        "add_comp_from_tag_to_existing",
        Add_bulk_add_comp_from_tag_to_existing
    },
    {
        "add_tag_from_tag_to_empty",
        Add_bulk_add_tag_from_tag_to_empty
    },
    {
        "add_tag_from_tag_to_existing",
        Add_bulk_add_tag_from_tag_to_existing
    },
    {
        "add_comp_to_more_existing",
        Add_bulk_add_comp_to_more_existing
    },
    {
        "add_comp_to_fewer_existing",
        Add_bulk_add_comp_to_fewer_existing
    },
    {
        "on_add",
        Add_bulk_on_add
    },
    {
        "add_entity_comp",
        Add_bulk_add_entity_comp
    },
    {
        "add_entity_tag",
        Add_bulk_add_entity_tag
    },
    {
        "add_entity_on_add",
        Add_bulk_add_entity_on_add
    },
    {
        "add_entity_existing",
        Add_bulk_add_entity_existing
    }
};

bake_test_case Remove_bulk_testcases[] = {
    {
        "remove_comp_from_comp_to_empty",
        Remove_bulk_remove_comp_from_comp_to_empty
    },
    {
        "remove_comp_from_comp_to_existing",
        Remove_bulk_remove_comp_from_comp_to_existing
    },
    {
        "remove_comp_from_tag_to_empty",
        Remove_bulk_remove_comp_from_tag_to_empty
    },
    {
        "remove_comp_from_tag_to_existing",
        Remove_bulk_remove_comp_from_tag_to_existing
    },
    {
        "remove_tag_from_tag_to_empty",
        Remove_bulk_remove_tag_from_tag_to_empty
    },
    {
        "remove_tag_from_tag_to_existing",
        Remove_bulk_remove_tag_from_tag_to_existing
    },
    {
        "remove_all_comp",
        Remove_bulk_remove_all_comp
    },
    {
        "remove_all_tag",
        Remove_bulk_remove_all_tag
    },
    {
        "on_remove",
        Remove_bulk_on_remove
    },
    {
        "remove_entity_comp",
        Remove_bulk_remove_entity_comp
    },
    {
        "remove_entity_tag",
        Remove_bulk_remove_entity_tag
    },
    {
        "remove_entity_on_remove",
        Remove_bulk_remove_entity_on_remove
    }
};

bake_test_case Add_remove_bulk_testcases[] = {
    {
        "add_remove_add_only",
        Add_remove_bulk_add_remove_add_only
    },
    {
        "add_remove_remove_only",
        Add_remove_bulk_add_remove_remove_only
    },
    {
        "add_remove_both",
        Add_remove_bulk_add_remove_both
    },
    {
        "add_remove_same",
        Add_remove_bulk_add_remove_same
    }
};

bake_test_case Has_testcases[] = {
    {
        "zero",
        Has_zero
    },
    {
        "zero_from_nonzero",
        Has_zero_from_nonzero
    },
    {
        "1_of_0",
        Has_1_of_0
    },
    {
        "1_of_1",
        Has_1_of_1
    },
    {
        "1_of_2",
        Has_1_of_2
    },
    {
        "2_of_0",
        Has_2_of_0
    },
    {
        "2_of_2",
        Has_2_of_2
    },
    {
        "3_of_2",
        Has_3_of_2
    },
    {
        "2_of_1",
        Has_2_of_1
    },
    {
        "1_of_empty",
        Has_1_of_empty
    },
    {
        "has_in_progress",
        Has_has_in_progress
    },
    {
        "has_of_zero",
        Has_has_of_zero
    },
    {
        "owns",
        Has_owns
    },
    {
        "has_entity",
        Has_has_entity
    },
    {
        "has_entity_0",
        Has_has_entity_0
    },
    {
        "has_entity_0_component",
        Has_has_entity_0_component
    },
    {
        "has_entity_owned",
        Has_has_entity_owned
    },
    {
        "has_entity_owned_0",
        Has_has_entity_owned_0
    },
    {
        "has_entity_owned_0_component",
        Has_has_entity_owned_0_component
    }
};

bake_test_case Count_testcases[] = {
    {
        "count_empty",
        Count_count_empty
    },
    {
        "count_1_component",
        Count_count_1_component
    },
    {
        "count_2_components",
        Count_count_2_components
    },
    {
        "count_3_components",
        Count_count_3_components
    },
    {
        "count_2_types_2_comps",
        Count_count_2_types_2_comps
    }
};

bake_test_case Get_component_testcases[] = {
    {
        "get_empty",
        Get_component_get_empty
    },
    {
        "get_1_from_1",
        Get_component_get_1_from_1
    },
    {
        "get_1_from_2",
        Get_component_get_1_from_2
    },
    {
        "get_2_from_2",
        Get_component_get_2_from_2
    },
    {
        "get_2_from_3",
        Get_component_get_2_from_3
    },
    {
        "get_1_from_2_in_progress_from_main_stage",
        Get_component_get_1_from_2_in_progress_from_main_stage
    },
    {
        "get_1_from_2_add_in_progress",
        Get_component_get_1_from_2_add_in_progress
    },
    {
        "get_both_from_2_add_in_progress",
        Get_component_get_both_from_2_add_in_progress
    },
    {
        "get_both_from_2_add_remove_in_progress",
        Get_component_get_both_from_2_add_remove_in_progress
    }
};

bake_test_case Delete_testcases[] = {
    {
        "delete_1",
        Delete_delete_1
    },
    {
        "delete_1_again",
        Delete_delete_1_again
    },
    {
        "delete_empty",
        Delete_delete_empty
    },
    {
        "delete_nonexist",
        Delete_delete_nonexist
    },
    {
        "delete_1st_of_3",
        Delete_delete_1st_of_3
    },
    {
        "delete_2nd_of_3",
        Delete_delete_2nd_of_3
    },
    {
        "delete_2_of_3",
        Delete_delete_2_of_3
    },
    {
        "delete_3_of_3",
        Delete_delete_3_of_3
    },
    {
        "delete_w_on_remove",
        Delete_delete_w_on_remove
    }
};

bake_test_case Delete_w_filter_testcases[] = {
    {
        "delete_1",
        Delete_w_filter_delete_1
    },
    {
        "delete_2",
        Delete_w_filter_delete_2
    },
    {
        "delete_1_2_types",
        Delete_w_filter_delete_1_2_types
    },
    {
        "delete_2_2_types",
        Delete_w_filter_delete_2_2_types
    },
    {
        "delete_except_1",
        Delete_w_filter_delete_except_1
    },
    {
        "delete_except_2",
        Delete_w_filter_delete_except_2
    },
    {
        "delete_with_any_of_2",
        Delete_w_filter_delete_with_any_of_2
    },
    {
        "delete_except_all_of_2",
        Delete_w_filter_delete_except_all_of_2
    },
    {
        "include_exact",
        Delete_w_filter_include_exact
    },
    {
        "exclude_exact",
        Delete_w_filter_exclude_exact
    },
    {
        "system_activate_test",
        Delete_w_filter_system_activate_test
    },
    {
        "skip_builtin_tables",
        Delete_w_filter_skip_builtin_tables
    }
};

bake_test_case Set_testcases[] = {
    {
        "set_empty",
        Set_set_empty
    },
    {
        "set_nonempty",
        Set_set_nonempty
    },
    {
        "set_non_empty_override",
        Set_set_non_empty_override
    },
    {
        "set_again",
        Set_set_again
    },
    {
        "set_2",
        Set_set_2
    },
    {
        "add_set",
        Set_add_set
    },
    {
        "set_add",
        Set_set_add
    },
    {
        "set_add_other",
        Set_set_add_other
    },
    {
        "set_remove",
        Set_set_remove
    },
    {
        "set_remove_other",
        Set_set_remove_other
    },
    {
        "set_remove_twice",
        Set_set_remove_twice
    },
    {
        "set_and_new",
        Set_set_and_new
    },
    {
        "set_null",
        Set_set_null
    },
    {
        "get_mutable_new",
        Set_get_mutable_new
    },
    {
        "get_mutable_existing",
        Set_get_mutable_existing
    },
    {
        "modified_w_on_set",
        Set_modified_w_on_set
    }
};

bake_test_case Lookup_testcases[] = {
    {
        "lookup",
        Lookup_lookup
    },
    {
        "lookup_component",
        Lookup_lookup_component
    },
    {
        "lookup_not_found",
        Lookup_lookup_not_found
    },
    {
        "lookup_child",
        Lookup_lookup_child
    },
    {
        "lookup_in_progress",
        Lookup_lookup_in_progress
    },
    {
        "lookup_child_in_progress",
        Lookup_lookup_child_in_progress
    },
    {
        "lookup_w_null_name",
        Lookup_lookup_w_null_name
    },
    {
        "get_name",
        Lookup_get_name
    },
    {
        "get_name_no_name",
        Lookup_get_name_no_name
    },
    {
        "get_name_from_empty",
        Lookup_get_name_from_empty
    },
    {
        "lookup_by_id",
        Lookup_lookup_by_id
    }
};

bake_test_case Singleton_testcases[] = {
    {
        "set",
        Singleton_set
    },
    {
        "set_ptr",
        Singleton_set_ptr
    },
    {
        "system_w_singleton",
        Singleton_system_w_singleton
    },
    {
        "system_w_singleton_no_match",
        Singleton_system_w_singleton_no_match
    },
    {
        "system_w_not_singleton",
        Singleton_system_w_not_singleton
    },
    {
        "lookup_singleton",
        Singleton_lookup_singleton
    },
    {
        "get_singleton_id",
        Singleton_get_singleton_id
    }
};

bake_test_case Clone_testcases[] = {
    {
        "empty",
        Clone_empty
    },
    {
        "empty_w_value",
        Clone_empty_w_value
    },
    {
        "null",
        Clone_null
    },
    {
        "null_w_value",
        Clone_null_w_value
    },
    {
        "1_component",
        Clone_1_component
    },
    {
        "2_component",
        Clone_2_component
    },
    {
        "1_component_w_value",
        Clone_1_component_w_value
    },
    {
        "2_component_w_value",
        Clone_2_component_w_value
    },
    {
        "3_component",
        Clone_3_component
    },
    {
        "3_component_w_value",
        Clone_3_component_w_value
    },
    {
        "tag",
        Clone_tag
    },
    {
        "tag_w_value",
        Clone_tag_w_value
    },
    {
        "1_tag_1_component",
        Clone_1_tag_1_component
    },
    {
        "1_tag_1_component_w_value",
        Clone_1_tag_1_component_w_value
    }
};

bake_test_case ComponentLifecycle_testcases[] = {
    {
        "ctor_on_add",
        ComponentLifecycle_ctor_on_add
    },
    {
        "ctor_on_new",
        ComponentLifecycle_ctor_on_new
    },
    {
        "dtor_on_remove",
        ComponentLifecycle_dtor_on_remove
    },
    {
        "dtor_on_delete",
        ComponentLifecycle_dtor_on_delete
    },
    {
        "copy_on_set",
        ComponentLifecycle_copy_on_set
    },
    {
        "copy_on_override",
        ComponentLifecycle_copy_on_override
    },
    {
        "copy_on_new_w_data",
        ComponentLifecycle_copy_on_new_w_data
    },
    {
        "copy_on_clone",
        ComponentLifecycle_copy_on_clone
    },
    {
        "copy_on_stage",
        ComponentLifecycle_copy_on_stage
    },
    {
        "no_copy_on_move",
        ComponentLifecycle_no_copy_on_move
    },
    {
        "move_on_merge",
        ComponentLifecycle_move_on_merge
    },
    {
        "ctor_on_bulk_add",
        ComponentLifecycle_ctor_on_bulk_add
    },
    {
        "dtor_on_bulk_remove",
        ComponentLifecycle_dtor_on_bulk_remove
    },
    {
        "ctor_on_bulk_add_entity",
        ComponentLifecycle_ctor_on_bulk_add_entity
    },
    {
        "dtor_on_bulk_remove_entity",
        ComponentLifecycle_dtor_on_bulk_remove_entity
    },
    {
        "ctor_dtor_on_bulk_add_remove",
        ComponentLifecycle_ctor_dtor_on_bulk_add_remove
    },
    {
        "ctor_copy_on_snapshot",
        ComponentLifecycle_ctor_copy_on_snapshot
    },
    {
        "copy_on_snapshot",
        ComponentLifecycle_copy_on_snapshot
    },
    {
        "dtor_on_restore",
        ComponentLifecycle_dtor_on_restore
    },
    {
        "ctor_on_tag",
        ComponentLifecycle_ctor_on_tag
    },
    {
        "dtor_on_tag",
        ComponentLifecycle_dtor_on_tag
    },
    {
        "copy_on_tag",
        ComponentLifecycle_copy_on_tag
    },
    {
        "move_on_tag",
        ComponentLifecycle_move_on_tag
    }
};

bake_test_case Pipeline_testcases[] = {
    {
        "system_order_same_phase",
        Pipeline_system_order_same_phase
    },
    {
        "system_order_same_phase_after_disable",
        Pipeline_system_order_same_phase_after_disable
    },
    {
        "system_order_same_phase_after_activate",
        Pipeline_system_order_same_phase_after_activate
    },
    {
        "system_order_different_phase",
        Pipeline_system_order_different_phase
    },
    {
        "system_order_different_phase_after_disable",
        Pipeline_system_order_different_phase_after_disable
    },
    {
        "system_order_different_phase_after_activate",
        Pipeline_system_order_different_phase_after_activate
    },
    {
        "system_order_after_new_system_lower_id",
        Pipeline_system_order_after_new_system_lower_id
    },
    {
        "system_order_after_new_system_inbetween_id",
        Pipeline_system_order_after_new_system_inbetween_id
    },
    {
        "system_order_after_new_system_higher_id",
        Pipeline_system_order_after_new_system_higher_id
    },
    {
        "merge_after_staged_out",
        Pipeline_merge_after_staged_out
    },
    {
        "merge_after_not_out",
        Pipeline_merge_after_not_out
    },
    {
        "no_merge_after_main_out",
        Pipeline_no_merge_after_main_out
    },
    {
        "no_merge_after_staged_in_out",
        Pipeline_no_merge_after_staged_in_out
    },
    {
        "merge_after_staged_out_before_owned",
        Pipeline_merge_after_staged_out_before_owned
    },
    {
        "switch_pipeline",
        Pipeline_switch_pipeline
    }
};

bake_test_case SystemMisc_testcases[] = {
    {
        "invalid_not_without_id",
        SystemMisc_invalid_not_without_id
    },
    {
        "invalid_optional_without_id",
        SystemMisc_invalid_optional_without_id
    },
    {
        "invalid_system_without_id",
        SystemMisc_invalid_system_without_id
    },
    {
        "invalid_container_without_id",
        SystemMisc_invalid_container_without_id
    },
    {
        "invalid_cascade_without_id",
        SystemMisc_invalid_cascade_without_id
    },
    {
        "invalid_entity_without_id",
        SystemMisc_invalid_entity_without_id
    },
    {
        "invalid_empty_without_id",
        SystemMisc_invalid_empty_without_id
    },
    {
        "invalid_singleton_without_id",
        SystemMisc_invalid_singleton_without_id
    },
    {
        "invalid_empty_element",
        SystemMisc_invalid_empty_element
    },
    {
        "invalid_empty_element_w_space",
        SystemMisc_invalid_empty_element_w_space
    },
    {
        "invalid_empty_or",
        SystemMisc_invalid_empty_or
    },
    {
        "invalid_empty_or_w_space",
        SystemMisc_invalid_empty_or_w_space
    },
    {
        "invalid_or_w_not",
        SystemMisc_invalid_or_w_not
    },
    {
        "invalid_not_w_or",
        SystemMisc_invalid_not_w_or
    },
    {
        "invalid_0_w_and",
        SystemMisc_invalid_0_w_and
    },
    {
        "invalid_0_w_from_system",
        SystemMisc_invalid_0_w_from_system
    },
    {
        "invalid_0_w_from_container",
        SystemMisc_invalid_0_w_from_container
    },
    {
        "invalid_0_w_from_cascade",
        SystemMisc_invalid_0_w_from_cascade
    },
    {
        "invalid_0_w_from_entity",
        SystemMisc_invalid_0_w_from_entity
    },
    {
        "invalid_0_w_from_empty",
        SystemMisc_invalid_0_w_from_empty
    },
    {
        "invalid_0_w_from_singleton",
        SystemMisc_invalid_0_w_from_singleton
    },
    {
        "invalid_or_w_empty",
        SystemMisc_invalid_or_w_empty
    },
    {
        "invalid_component_id",
        SystemMisc_invalid_component_id
    },
    {
        "invalid_entity_id",
        SystemMisc_invalid_entity_id
    },
    {
        "invalid_or_from_system",
        SystemMisc_invalid_or_from_system
    },
    {
        "invalid_phase",
        SystemMisc_invalid_phase
    },
    {
        "invalid_null_string",
        SystemMisc_invalid_null_string
    },
    {
        "invalid_empty_string",
        SystemMisc_invalid_empty_string
    },
    {
        "invalid_empty_string_w_space",
        SystemMisc_invalid_empty_string_w_space
    },
    {
        "redefine_row_system",
        SystemMisc_redefine_row_system
    },
    {
        "system_w_or_prefab",
        SystemMisc_system_w_or_prefab
    },
    {
        "system_w_or_disabled",
        SystemMisc_system_w_or_disabled
    },
    {
        "system_w_or_disabled_and_prefab",
        SystemMisc_system_w_or_disabled_and_prefab
    },
    {
        "table_columns_access",
        SystemMisc_table_columns_access
    },
    {
        "status_enable_after_new",
        SystemMisc_status_enable_after_new
    },
    {
        "status_enable_after_disable",
        SystemMisc_status_enable_after_disable
    },
    {
        "status_disable_after_new",
        SystemMisc_status_disable_after_new
    },
    {
        "status_disable_after_disable",
        SystemMisc_status_disable_after_disable
    },
    {
        "status_activate_after_new",
        SystemMisc_status_activate_after_new
    },
    {
        "status_deactivate_after_delete",
        SystemMisc_status_deactivate_after_delete
    },
    {
        "dont_enable_after_rematch",
        SystemMisc_dont_enable_after_rematch
    },
    {
        "ensure_single_merge",
        SystemMisc_ensure_single_merge
    },
    {
        "table_count",
        SystemMisc_table_count
    },
    {
        "match_system",
        SystemMisc_match_system
    },
    {
        "match_system_w_filter",
        SystemMisc_match_system_w_filter
    },
    {
        "system_initial_state",
        SystemMisc_system_initial_state
    },
    {
        "add_own_component",
        SystemMisc_add_own_component
    },
    {
        "change_system_action",
        SystemMisc_change_system_action
    },
    {
        "system_readeactivate",
        SystemMisc_system_readeactivate
    },
    {
        "system_readeactivate_w_2_systems",
        SystemMisc_system_readeactivate_w_2_systems
    }
};

bake_test_case Sorting_testcases[] = {
    {
        "sort_by_component",
        Sorting_sort_by_component
    },
    {
        "sort_by_component_2_tables",
        Sorting_sort_by_component_2_tables
    },
    {
        "sort_by_component_3_tables",
        Sorting_sort_by_component_3_tables
    },
    {
        "sort_by_entity",
        Sorting_sort_by_entity
    },
    {
        "sort_after_add",
        Sorting_sort_after_add
    },
    {
        "sort_after_remove",
        Sorting_sort_after_remove
    },
    {
        "sort_after_delete",
        Sorting_sort_after_delete
    },
    {
        "sort_after_set",
        Sorting_sort_after_set
    },
    {
        "sort_after_system",
        Sorting_sort_after_system
    },
    {
        "sort_by_component_same_value_1",
        Sorting_sort_by_component_same_value_1
    },
    {
        "sort_by_component_same_value_2",
        Sorting_sort_by_component_same_value_2
    },
    {
        "sort_by_component_move_pivot",
        Sorting_sort_by_component_move_pivot
    },
    {
        "sort_1000_entities",
        Sorting_sort_1000_entities
    }
};

bake_test_case Queries_testcases[] = {
    {
        "query_changed_after_new",
        Queries_query_changed_after_new
    },
    {
        "query_changed_after_delete",
        Queries_query_changed_after_delete
    },
    {
        "query_changed_after_add",
        Queries_query_changed_after_add
    },
    {
        "query_changed_after_remove",
        Queries_query_changed_after_remove
    },
    {
        "query_changed_after_set",
        Queries_query_changed_after_set
    },
    {
        "query_change_after_out_system",
        Queries_query_change_after_out_system
    },
    {
        "query_change_after_in_system",
        Queries_query_change_after_in_system
    }
};

bake_test_case Traits_testcases[] = {
    {
        "type_w_one_trait",
        Traits_type_w_one_trait
    },
    {
        "type_w_two_traits",
        Traits_type_w_two_traits
    },
    {
        "add_trait",
        Traits_add_trait
    },
    {
        "remove_trait",
        Traits_remove_trait
    },
    {
        "add_tag_trait_for_tag",
        Traits_add_tag_trait_for_tag
    },
    {
        "add_tag_trait_for_component",
        Traits_add_tag_trait_for_component
    },
    {
        "query_2_traits",
        Traits_query_2_traits
    },
    {
        "query_2_traits_2_instances_per_type",
        Traits_query_2_traits_2_instances_per_type
    }
};

bake_test_case TriggerOnAdd_testcases[] = {
    {
        "new_match_1_of_1",
        TriggerOnAdd_new_match_1_of_1
    },
    {
        "new_match_1_of_2",
        TriggerOnAdd_new_match_1_of_2
    },
    {
        "new_no_match_1",
        TriggerOnAdd_new_no_match_1
    },
    {
        "new_w_count_match_1_of_1",
        TriggerOnAdd_new_w_count_match_1_of_1
    },
    {
        "add_match_1_of_1",
        TriggerOnAdd_add_match_1_of_1
    },
    {
        "add_match_1_of_2",
        TriggerOnAdd_add_match_1_of_2
    },
    {
        "add_no_match_1",
        TriggerOnAdd_add_no_match_1
    },
    {
        "set_match_1_of_1",
        TriggerOnAdd_set_match_1_of_1
    },
    {
        "set_no_match_1",
        TriggerOnAdd_set_no_match_1
    },
    {
        "clone_match_1_of_1",
        TriggerOnAdd_clone_match_1_of_1
    },
    {
        "clone_match_1_of_2",
        TriggerOnAdd_clone_match_1_of_2
    },
    {
        "add_again_1",
        TriggerOnAdd_add_again_1
    },
    {
        "set_again_1",
        TriggerOnAdd_set_again_1
    },
    {
        "add_again_2",
        TriggerOnAdd_add_again_2
    },
    {
        "override_after_add_in_on_add",
        TriggerOnAdd_override_after_add_in_on_add
    },
    {
        "add_again_in_progress",
        TriggerOnAdd_add_again_in_progress
    },
    {
        "add_in_progress_before_system_def",
        TriggerOnAdd_add_in_progress_before_system_def
    },
    {
        "2_systems_w_table_creation",
        TriggerOnAdd_2_systems_w_table_creation
    },
    {
        "2_systems_w_table_creation_in_progress",
        TriggerOnAdd_2_systems_w_table_creation_in_progress
    },
    {
        "sys_context",
        TriggerOnAdd_sys_context
    },
    {
        "get_sys_context_from_param",
        TriggerOnAdd_get_sys_context_from_param
    },
    {
        "remove_added_component_in_on_add_w_set",
        TriggerOnAdd_remove_added_component_in_on_add_w_set
    },
    {
        "on_add_in_on_add",
        TriggerOnAdd_on_add_in_on_add
    },
    {
        "on_remove_in_on_add",
        TriggerOnAdd_on_remove_in_on_add
    },
    {
        "on_set_in_on_add",
        TriggerOnAdd_on_set_in_on_add
    },
    {
        "on_add_in_on_update",
        TriggerOnAdd_on_add_in_on_update
    }
};

bake_test_case TriggerOnRemove_testcases[] = {
    {
        "remove_match_1_of_1",
        TriggerOnRemove_remove_match_1_of_1
    },
    {
        "remove_match_1_of_2",
        TriggerOnRemove_remove_match_1_of_2
    },
    {
        "remove_no_match_1",
        TriggerOnRemove_remove_no_match_1
    },
    {
        "delete_match_1_of_1",
        TriggerOnRemove_delete_match_1_of_1
    },
    {
        "delete_match_1_of_2",
        TriggerOnRemove_delete_match_1_of_2
    },
    {
        "delete_no_match_1",
        TriggerOnRemove_delete_no_match_1
    },
    {
        "remove_watched",
        TriggerOnRemove_remove_watched
    },
    {
        "delete_watched",
        TriggerOnRemove_delete_watched
    }
};

bake_test_case TriggerOnSet_testcases[] = {
    {
        "set",
        TriggerOnSet_set
    },
    {
        "set_new",
        TriggerOnSet_set_new
    },
    {
        "set_again",
        TriggerOnSet_set_again
    },
    {
        "clone",
        TriggerOnSet_clone
    },
    {
        "clone_w_value",
        TriggerOnSet_clone_w_value
    },
    {
        "set_and_add_system",
        TriggerOnSet_set_and_add_system
    },
    {
        "on_set_after_override",
        TriggerOnSet_on_set_after_override
    },
    {
        "on_set_after_override_w_new",
        TriggerOnSet_on_set_after_override_w_new
    },
    {
        "on_set_after_override_w_new_w_count",
        TriggerOnSet_on_set_after_override_w_new_w_count
    },
    {
        "on_set_after_override_1_of_2_overridden",
        TriggerOnSet_on_set_after_override_1_of_2_overridden
    },
    {
        "on_set_after_snapshot_restore",
        TriggerOnSet_on_set_after_snapshot_restore
    }
};

bake_test_case Monitor_testcases[] = {
    {
        "1_comp",
        Monitor_1_comp
    },
    {
        "2_comps",
        Monitor_2_comps
    },
    {
        "1_comp_1_not",
        Monitor_1_comp_1_not
    },
    {
        "1_parent",
        Monitor_1_parent
    },
    {
        "1_comp_1_parent",
        Monitor_1_comp_1_parent
    }
};

bake_test_case SystemOnSet_testcases[] = {
    {
        "set_1_of_1",
        SystemOnSet_set_1_of_1
    },
    {
        "set_1_of_2",
        SystemOnSet_set_1_of_2
    },
    {
        "set_1_of_3",
        SystemOnSet_set_1_of_3
    },
    {
        "bulk_new_1",
        SystemOnSet_bulk_new_1
    },
    {
        "bulk_new_2",
        SystemOnSet_bulk_new_2
    },
    {
        "bulk_new_2_of_1",
        SystemOnSet_bulk_new_2_of_1
    },
    {
        "bulk_new_3",
        SystemOnSet_bulk_new_3
    },
    {
        "bulk_new_3_of_2",
        SystemOnSet_bulk_new_3_of_2
    },
    {
        "bulk_new_1_from_base",
        SystemOnSet_bulk_new_1_from_base
    },
    {
        "set_1_of_2_1_from_base",
        SystemOnSet_set_1_of_2_1_from_base
    },
    {
        "set_1_of_3_1_from_base",
        SystemOnSet_set_1_of_3_1_from_base
    },
    {
        "add_base",
        SystemOnSet_add_base
    },
    {
        "add_base_to_1_overridden",
        SystemOnSet_add_base_to_1_overridden
    },
    {
        "add_base_to_2_overridden",
        SystemOnSet_add_base_to_2_overridden
    },
    {
        "add_base_to_1_of_2_overridden",
        SystemOnSet_add_base_to_1_of_2_overridden
    },
    {
        "on_set_after_remove_override",
        SystemOnSet_on_set_after_remove_override
    },
    {
        "no_set_after_remove_base",
        SystemOnSet_no_set_after_remove_base
    },
    {
        "un_set_after_remove",
        SystemOnSet_un_set_after_remove
    },
    {
        "un_set_after_remove_base",
        SystemOnSet_un_set_after_remove_base
    },
    {
        "add_to_current_in_on_set",
        SystemOnSet_add_to_current_in_on_set
    },
    {
        "remove_from_current_in_on_set",
        SystemOnSet_remove_from_current_in_on_set
    },
    {
        "remove_set_component_in_on_set",
        SystemOnSet_remove_set_component_in_on_set
    },
    {
        "match_table_created_w_add_in_on_set",
        SystemOnSet_match_table_created_w_add_in_on_set
    },
    {
        "set_optional",
        SystemOnSet_set_optional
    },
    {
        "set_from_nothing",
        SystemOnSet_set_from_nothing
    },
    {
        "add_null_type_in_on_set",
        SystemOnSet_add_null_type_in_on_set
    },
    {
        "add_0_entity_in_on_set",
        SystemOnSet_add_0_entity_in_on_set
    }
};

bake_test_case SystemPeriodic_testcases[] = {
    {
        "1_type_1_component",
        SystemPeriodic_1_type_1_component
    },
    {
        "1_type_3_component",
        SystemPeriodic_1_type_3_component
    },
    {
        "3_type_1_component",
        SystemPeriodic_3_type_1_component
    },
    {
        "2_type_3_component",
        SystemPeriodic_2_type_3_component
    },
    {
        "1_type_1_component_1_tag",
        SystemPeriodic_1_type_1_component_1_tag
    },
    {
        "2_type_1_component_1_tag",
        SystemPeriodic_2_type_1_component_1_tag
    },
    {
        "2_type_1_and_1_not",
        SystemPeriodic_2_type_1_and_1_not
    },
    {
        "2_type_2_and_1_not",
        SystemPeriodic_2_type_2_and_1_not
    },
    {
        "2_type_2_and_2_not",
        SystemPeriodic_2_type_2_and_2_not
    },
    {
        "4_type_1_and_1_or",
        SystemPeriodic_4_type_1_and_1_or
    },
    {
        "4_type_1_and_1_or_of_3",
        SystemPeriodic_4_type_1_and_1_or_of_3
    },
    {
        "1_type_1_and_1_or",
        SystemPeriodic_1_type_1_and_1_or
    },
    {
        "2_type_1_and_1_optional",
        SystemPeriodic_2_type_1_and_1_optional
    },
    {
        "2_type_2_and_1_optional",
        SystemPeriodic_2_type_2_and_1_optional
    },
    {
        "6_type_1_and_2_optional",
        SystemPeriodic_6_type_1_and_2_optional
    },
    {
        "ensure_optional_is_unset_column",
        SystemPeriodic_ensure_optional_is_unset_column
    },
    {
        "ensure_optional_is_null_shared",
        SystemPeriodic_ensure_optional_is_null_shared
    },
    {
        "ensure_optional_is_null_field_owned",
        SystemPeriodic_ensure_optional_is_null_field_owned
    },
    {
        "ensure_optional_is_null_field_shared",
        SystemPeriodic_ensure_optional_is_null_field_shared
    },
    {
        "use_fields_2_owned",
        SystemPeriodic_use_fields_2_owned
    },
    {
        "use_fields_1_owned_1_shared",
        SystemPeriodic_use_fields_1_owned_1_shared
    },
    {
        "match_2_systems_w_populated_table",
        SystemPeriodic_match_2_systems_w_populated_table
    },
    {
        "on_period",
        SystemPeriodic_on_period
    },
    {
        "on_period_long_delta",
        SystemPeriodic_on_period_long_delta
    },
    {
        "disabled",
        SystemPeriodic_disabled
    },
    {
        "disabled_feature",
        SystemPeriodic_disabled_feature
    },
    {
        "disabled_nested_feature",
        SystemPeriodic_disabled_nested_feature
    },
    {
        "two_refs",
        SystemPeriodic_two_refs
    },
    {
        "filter_disabled",
        SystemPeriodic_filter_disabled
    },
    {
        "match_disabled",
        SystemPeriodic_match_disabled
    },
    {
        "match_disabled_and_enabled",
        SystemPeriodic_match_disabled_and_enabled
    },
    {
        "match_prefab",
        SystemPeriodic_match_prefab
    },
    {
        "match_prefab_and_normal",
        SystemPeriodic_match_prefab_and_normal
    },
    {
        "is_shared_on_column_not_set",
        SystemPeriodic_is_shared_on_column_not_set
    },
    {
        "owned_column",
        SystemPeriodic_owned_column
    },
    {
        "owned_not_column",
        SystemPeriodic_owned_not_column
    },
    {
        "owned_or_column",
        SystemPeriodic_owned_or_column
    },
    {
        "shared_column",
        SystemPeriodic_shared_column
    },
    {
        "shared_not_column",
        SystemPeriodic_shared_not_column
    },
    {
        "shared_or_column",
        SystemPeriodic_shared_or_column
    },
    {
        "container_dont_match_inheritance",
        SystemPeriodic_container_dont_match_inheritance
    },
    {
        "cascade_dont_match_inheritance",
        SystemPeriodic_cascade_dont_match_inheritance
    },
    {
        "not_from_singleton",
        SystemPeriodic_not_from_singleton
    },
    {
        "not_from_entity",
        SystemPeriodic_not_from_entity
    },
    {
        "sys_context",
        SystemPeriodic_sys_context
    },
    {
        "get_sys_context_from_param",
        SystemPeriodic_get_sys_context_from_param
    },
    {
        "use_field_w_0_size",
        SystemPeriodic_use_field_w_0_size
    },
    {
        "owned_only",
        SystemPeriodic_owned_only
    },
    {
        "shared_only",
        SystemPeriodic_shared_only
    },
    {
        "is_in_readonly",
        SystemPeriodic_is_in_readonly
    },
    {
        "get_period",
        SystemPeriodic_get_period
    }
};

bake_test_case Timer_testcases[] = {
    {
        "timeout",
        Timer_timeout
    },
    {
        "interval",
        Timer_interval
    },
    {
        "shared_timeout",
        Timer_shared_timeout
    },
    {
        "shared_interval",
        Timer_shared_interval
    },
    {
        "start_stop_one_shot",
        Timer_start_stop_one_shot
    },
    {
        "start_stop_interval",
        Timer_start_stop_interval
    },
    {
        "rate_filter",
        Timer_rate_filter
    },
    {
        "rate_filter_w_rate_filter_src",
        Timer_rate_filter_w_rate_filter_src
    },
    {
        "rate_filter_w_timer_src",
        Timer_rate_filter_w_timer_src
    }
};

bake_test_case SystemOnDemand_testcases[] = {
    {
        "enable_out_after_in",
        SystemOnDemand_enable_out_after_in
    },
    {
        "enable_in_after_out",
        SystemOnDemand_enable_in_after_out
    },
    {
        "enable_out_after_in_2_out_1_in",
        SystemOnDemand_enable_out_after_in_2_out_1_in
    },
    {
        "enable_out_after_in_1_out_2_in",
        SystemOnDemand_enable_out_after_in_1_out_2_in
    },
    {
        "enable_in_after_out_2_out_1_in",
        SystemOnDemand_enable_in_after_out_2_out_1_in
    },
    {
        "enable_in_after_out_1_out_2_in",
        SystemOnDemand_enable_in_after_out_1_out_2_in
    },
    {
        "disable_after_disable_in",
        SystemOnDemand_disable_after_disable_in
    },
    {
        "disable_after_disable_in_2_out_1_in",
        SystemOnDemand_disable_after_disable_in_2_out_1_in
    },
    {
        "disable_after_disable_in_1_out_2_in",
        SystemOnDemand_disable_after_disable_in_1_out_2_in
    },
    {
        "table_after_out",
        SystemOnDemand_table_after_out
    },
    {
        "table_after_out_and_in",
        SystemOnDemand_table_after_out_and_in
    },
    {
        "table_after_out_and_in_overlapping_columns",
        SystemOnDemand_table_after_out_and_in_overlapping_columns
    },
    {
        "1_out_system_2_in_systems",
        SystemOnDemand_1_out_system_2_in_systems
    },
    {
        "1_out_system_2_in_systems_different_columns",
        SystemOnDemand_1_out_system_2_in_systems_different_columns
    },
    {
        "1_out_system_2_in_systems_overlapping_columns",
        SystemOnDemand_1_out_system_2_in_systems_overlapping_columns
    },
    {
        "disable_after_inactive_in_system",
        SystemOnDemand_disable_after_inactive_in_system
    },
    {
        "disable_after_2_inactive_in_systems",
        SystemOnDemand_disable_after_2_inactive_in_systems
    },
    {
        "disable_after_2_inactive_in_systems_different_columns",
        SystemOnDemand_disable_after_2_inactive_in_systems_different_columns
    },
    {
        "enable_2_output_1_input_system",
        SystemOnDemand_enable_2_output_1_input_system
    },
    {
        "enable_2_output_1_input_system_different_columns",
        SystemOnDemand_enable_2_output_1_input_system_different_columns
    },
    {
        "enable_2_output_1_input_system_overlapping_columns",
        SystemOnDemand_enable_2_output_1_input_system_overlapping_columns
    },
    {
        "out_not_column",
        SystemOnDemand_out_not_column
    },
    {
        "trigger_on_manual",
        SystemOnDemand_trigger_on_manual
    },
    {
        "trigger_on_manual_not_column",
        SystemOnDemand_trigger_on_manual_not_column
    },
    {
        "on_demand_task_w_from_entity",
        SystemOnDemand_on_demand_task_w_from_entity
    },
    {
        "on_demand_task_w_not_from_entity",
        SystemOnDemand_on_demand_task_w_not_from_entity
    },
    {
        "enable_after_user_disable",
        SystemOnDemand_enable_after_user_disable
    }
};

bake_test_case SystemCascade_testcases[] = {
    {
        "cascade_depth_1",
        SystemCascade_cascade_depth_1
    },
    {
        "cascade_depth_2",
        SystemCascade_cascade_depth_2
    },
    {
        "add_after_match",
        SystemCascade_add_after_match
    },
    {
        "adopt_after_match",
        SystemCascade_adopt_after_match
    }
};

bake_test_case SystemManual_testcases[] = {
    {
        "1_type_1_component",
        SystemManual_1_type_1_component
    },
    {
        "activate_status",
        SystemManual_activate_status
    },
    {
        "no_automerge",
        SystemManual_no_automerge
    }
};

bake_test_case Tasks_testcases[] = {
    {
        "no_components",
        Tasks_no_components
    },
    {
        "one_tag",
        Tasks_one_tag
    },
    {
        "from_system",
        Tasks_from_system
    },
    {
        "tasks_in_phases",
        Tasks_tasks_in_phases
    }
};

bake_test_case Prefab_testcases[] = {
    {
        "new_w_prefab",
        Prefab_new_w_prefab
    },
    {
        "new_w_count_prefab",
        Prefab_new_w_count_prefab
    },
    {
        "new_w_type_w_prefab",
        Prefab_new_w_type_w_prefab
    },
    {
        "add_prefab",
        Prefab_add_prefab
    },
    {
        "add_type_w_prefab",
        Prefab_add_type_w_prefab
    },
    {
        "remove_prefab_after_new",
        Prefab_remove_prefab_after_new
    },
    {
        "remove_prefab_after_add",
        Prefab_remove_prefab_after_add
    },
    {
        "override_component",
        Prefab_override_component
    },
    {
        "override_remove_component",
        Prefab_override_remove_component
    },
    {
        "override_2_of_3_components_1_self",
        Prefab_override_2_of_3_components_1_self
    },
    {
        "new_type_w_1_override",
        Prefab_new_type_w_1_override
    },
    {
        "new_type_w_2_overrides",
        Prefab_new_type_w_2_overrides
    },
    {
        "add_type_w_1_overrides",
        Prefab_add_type_w_1_overrides
    },
    {
        "add_type_w_2_overrides",
        Prefab_add_type_w_2_overrides
    },
    {
        "get_ptr_prefab",
        Prefab_get_ptr_prefab
    },
    {
        "iterate_w_prefab_field",
        Prefab_iterate_w_prefab_field
    },
    {
        "iterate_w_prefab_shared",
        Prefab_iterate_w_prefab_shared
    },
    {
        "match_entity_prefab_w_system_optional",
        Prefab_match_entity_prefab_w_system_optional
    },
    {
        "prefab_in_system_expr",
        Prefab_prefab_in_system_expr
    },
    {
        "dont_match_prefab",
        Prefab_dont_match_prefab
    },
    {
        "new_w_count_w_override",
        Prefab_new_w_count_w_override
    },
    {
        "override_2_components_different_size",
        Prefab_override_2_components_different_size
    },
    {
        "ignore_prefab_parent_component",
        Prefab_ignore_prefab_parent_component
    },
    {
        "match_table_created_in_progress",
        Prefab_match_table_created_in_progress
    },
    {
        "prefab_w_1_child",
        Prefab_prefab_w_1_child
    },
    {
        "prefab_w_2_children",
        Prefab_prefab_w_2_children
    },
    {
        "prefab_w_grandchild",
        Prefab_prefab_w_grandchild
    },
    {
        "prefab_w_base_w_child",
        Prefab_prefab_w_base_w_child
    },
    {
        "prefab_w_child_w_base",
        Prefab_prefab_w_child_w_base
    },
    {
        "prefab_w_child_w_base_w_children",
        Prefab_prefab_w_child_w_base_w_children
    },
    {
        "prefab_w_child_new_w_count",
        Prefab_prefab_w_child_new_w_count
    },
    {
        "prefab_auto_override_child_component",
        Prefab_prefab_auto_override_child_component
    },
    {
        "ignore_on_add",
        Prefab_ignore_on_add
    },
    {
        "ignore_on_remove",
        Prefab_ignore_on_remove
    },
    {
        "ignore_on_set",
        Prefab_ignore_on_set
    },
    {
        "on_set_on_instance",
        Prefab_on_set_on_instance
    },
    {
        "instantiate_in_progress",
        Prefab_instantiate_in_progress
    },
    {
        "copy_from_prefab_in_progress",
        Prefab_copy_from_prefab_in_progress
    },
    {
        "copy_from_prefab_first_instance_in_progress",
        Prefab_copy_from_prefab_first_instance_in_progress
    },
    {
        "ref_after_realloc",
        Prefab_ref_after_realloc
    },
    {
        "revalidate_ref_w_mixed_table_refs",
        Prefab_revalidate_ref_w_mixed_table_refs
    },
    {
        "no_overwrite_on_2nd_add",
        Prefab_no_overwrite_on_2nd_add
    },
    {
        "no_overwrite_on_2nd_add_in_progress",
        Prefab_no_overwrite_on_2nd_add_in_progress
    },
    {
        "no_instantiate_on_2nd_add",
        Prefab_no_instantiate_on_2nd_add
    },
    {
        "no_instantiate_on_2nd_add_in_progress",
        Prefab_no_instantiate_on_2nd_add_in_progress
    },
    {
        "nested_prefab_in_progress_w_count",
        Prefab_nested_prefab_in_progress_w_count
    },
    {
        "nested_prefab_in_progress_w_count_set_after_override",
        Prefab_nested_prefab_in_progress_w_count_set_after_override
    },
    {
        "get_ptr_from_prefab_from_new_table_in_progress",
        Prefab_get_ptr_from_prefab_from_new_table_in_progress
    },
    {
        "match_base",
        Prefab_match_base
    },
    {
        "match_base_after_add_in_prev_phase",
        Prefab_match_base_after_add_in_prev_phase
    },
    {
        "override_watched_prefab",
        Prefab_override_watched_prefab
    },
    {
        "rematch_twice",
        Prefab_rematch_twice
    },
    {
        "inherit_in_system",
        Prefab_inherit_in_system
    },
    {
        "add_to_empty_base_in_system",
        Prefab_add_to_empty_base_in_system
    },
    {
        "dont_inherit_disabled",
        Prefab_dont_inherit_disabled
    },
    {
        "clone_after_inherit_in_on_add",
        Prefab_clone_after_inherit_in_on_add
    },
    {
        "override_from_nested",
        Prefab_override_from_nested
    },
    {
        "create_multiple_nested_w_on_set",
        Prefab_create_multiple_nested_w_on_set
    },
    {
        "create_multiple_nested_w_on_set_in_progress",
        Prefab_create_multiple_nested_w_on_set_in_progress
    },
    {
        "single_on_set_on_child_w_override",
        Prefab_single_on_set_on_child_w_override
    }
};

bake_test_case System_w_FromContainer_testcases[] = {
    {
        "1_column_from_container",
        System_w_FromContainer_1_column_from_container
    },
    {
        "2_column_1_from_container",
        System_w_FromContainer_2_column_1_from_container
    },
    {
        "3_column_2_from_container",
        System_w_FromContainer_3_column_2_from_container
    },
    {
        "3_column_2_from_different_container",
        System_w_FromContainer_3_column_2_from_different_container
    },
    {
        "2_column_1_from_container_w_not",
        System_w_FromContainer_2_column_1_from_container_w_not
    },
    {
        "2_column_1_from_container_w_not_prefab",
        System_w_FromContainer_2_column_1_from_container_w_not_prefab
    },
    {
        "3_column_1_from_comtainer_1_from_container_w_not",
        System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not
    },
    {
        "2_column_1_from_container_w_or",
        System_w_FromContainer_2_column_1_from_container_w_or
    },
    {
        "select_same_from_container",
        System_w_FromContainer_select_same_from_container
    },
    {
        "add_component_after_match",
        System_w_FromContainer_add_component_after_match
    },
    {
        "add_component_after_match_and_rematch",
        System_w_FromContainer_add_component_after_match_and_rematch
    },
    {
        "add_component_after_match_unmatch",
        System_w_FromContainer_add_component_after_match_unmatch
    },
    {
        "add_component_after_match_unmatch_match",
        System_w_FromContainer_add_component_after_match_unmatch_match
    },
    {
        "add_component_after_match_2_systems",
        System_w_FromContainer_add_component_after_match_2_systems
    },
    {
        "add_component_in_progress_after_match",
        System_w_FromContainer_add_component_in_progress_after_match
    },
    {
        "add_component_after_match_and_rematch_w_entity_type_expr",
        System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr
    },
    {
        "add_component_after_match_and_rematch_w_entity_type_expr_in_progress",
        System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr_in_progress
    },
    {
        "adopt_after_match",
        System_w_FromContainer_adopt_after_match
    },
    {
        "new_child_after_match",
        System_w_FromContainer_new_child_after_match
    },
    {
        "realloc_after_match",
        System_w_FromContainer_realloc_after_match
    }
};

bake_test_case System_w_FromId_testcases[] = {
    {
        "2_column_1_from_id",
        System_w_FromId_2_column_1_from_id
    },
    {
        "3_column_2_from_id",
        System_w_FromId_3_column_2_from_id
    },
    {
        "column_type",
        System_w_FromId_column_type
    }
};

bake_test_case System_w_FromSystem_testcases[] = {
    {
        "2_column_1_from_system",
        System_w_FromSystem_2_column_1_from_system
    },
    {
        "3_column_2_from_system",
        System_w_FromSystem_3_column_2_from_system
    },
    {
        "auto_add_tag",
        System_w_FromSystem_auto_add_tag
    }
};

bake_test_case System_w_FromEntity_testcases[] = {
    {
        "2_column_1_from_entity",
        System_w_FromEntity_2_column_1_from_entity
    },
    {
        "task_from_entity",
        System_w_FromEntity_task_from_entity
    },
    {
        "task_not_from_entity",
        System_w_FromEntity_task_not_from_entity
    }
};

bake_test_case World_testcases[] = {
    {
        "progress_w_0",
        World_progress_w_0
    },
    {
        "progress_w_t",
        World_progress_w_t
    },
    {
        "get_tick",
        World_get_tick
    },
    {
        "entity_range_offset",
        World_entity_range_offset
    },
    {
        "entity_range_offset_out_of_range",
        World_entity_range_offset_out_of_range
    },
    {
        "entity_range_limit_out_of_range",
        World_entity_range_limit_out_of_range
    },
    {
        "entity_range_add_existing_in_progress",
        World_entity_range_add_existing_in_progress
    },
    {
        "entity_range_add_in_range_in_progress",
        World_entity_range_add_in_range_in_progress
    },
    {
        "entity_range_add_out_of_range_in_progress",
        World_entity_range_add_out_of_range_in_progress
    },
    {
        "entity_range_out_of_range_check_disabled",
        World_entity_range_out_of_range_check_disabled
    },
    {
        "dim",
        World_dim
    },
    {
        "dim_dim_type",
        World_dim_dim_type
    },
    {
        "phases",
        World_phases
    },
    {
        "phases_w_merging",
        World_phases_w_merging
    },
    {
        "phases_match_in_create",
        World_phases_match_in_create
    },
    {
        "measure_time",
        World_measure_time
    },
    {
        "control_fps",
        World_control_fps
    },
    {
        "control_fps_busy_system",
        World_control_fps_busy_system
    },
    {
        "control_fps_busy_app",
        World_control_fps_busy_app
    },
    {
        "control_fps_random_system",
        World_control_fps_random_system
    },
    {
        "control_fps_random_app",
        World_control_fps_random_app
    },
    {
        "measure_fps_vs_actual",
        World_measure_fps_vs_actual
    },
    {
        "measure_delta_time_vs_actual",
        World_measure_delta_time_vs_actual
    },
    {
        "world_stats",
        World_world_stats
    },
    {
        "quit",
        World_quit
    },
    {
        "get_delta_time",
        World_get_delta_time
    },
    {
        "get_delta_time_auto",
        World_get_delta_time_auto
    },
    {
        "recreate_world",
        World_recreate_world
    },
    {
        "recreate_world_w_component",
        World_recreate_world_w_component
    },
    {
        "no_threading",
        World_no_threading
    },
    {
        "no_time",
        World_no_time
    },
    {
        "is_entity_enabled",
        World_is_entity_enabled
    }
};

bake_test_case Type_testcases[] = {
    {
        "type_of_1_tostr",
        Type_type_of_1_tostr
    },
    {
        "type_of_2_tostr",
        Type_type_of_2_tostr
    },
    {
        "type_of_2_tostr_no_id",
        Type_type_of_2_tostr_no_id
    },
    {
        "type_redefine",
        Type_type_redefine
    },
    {
        "type_from_array",
        Type_type_from_array
    },
    {
        "type_from_array_unsorted",
        Type_type_from_array_unsorted
    },
    {
        "type_has",
        Type_type_has
    },
    {
        "type_has_not",
        Type_type_has_not
    },
    {
        "zero_type_has_not",
        Type_zero_type_has_not
    },
    {
        "type_has_prefab",
        Type_type_has_prefab
    },
    {
        "type_has_container",
        Type_type_has_container
    },
    {
        "type_has_prefab_container",
        Type_type_has_prefab_container
    },
    {
        "type_merge",
        Type_type_merge
    },
    {
        "type_merge_overlap",
        Type_type_merge_overlap
    },
    {
        "type_merge_overlap_one",
        Type_type_merge_overlap_one
    },
    {
        "type_merge_overlap_w_flag",
        Type_type_merge_overlap_w_flag
    },
    {
        "type_merge_overlap_w_flags_from_both",
        Type_type_merge_overlap_w_flags_from_both
    },
    {
        "type_add",
        Type_type_add
    },
    {
        "type_add_empty",
        Type_type_add_empty
    },
    {
        "type_add_entity_again",
        Type_type_add_entity_again
    },
    {
        "type_add_out_of_order",
        Type_type_add_out_of_order
    },
    {
        "type_add_existing",
        Type_type_add_existing
    },
    {
        "type_add_empty_existing",
        Type_type_add_empty_existing
    },
    {
        "type_add_out_of_order_existing",
        Type_type_add_out_of_order_existing
    },
    {
        "type_remove",
        Type_type_remove
    },
    {
        "type_remove_empty",
        Type_type_remove_empty
    },
    {
        "type_remove_non_existing",
        Type_type_remove_non_existing
    },
    {
        "type_of_2_add",
        Type_type_of_2_add
    },
    {
        "type_of_3_add_entity_again",
        Type_type_of_3_add_entity_again
    },
    {
        "invalid_entity_type_expression",
        Type_invalid_entity_type_expression
    },
    {
        "invalid_singleton_type_expression",
        Type_invalid_singleton_type_expression
    },
    {
        "invalid_container_type_expression",
        Type_invalid_container_type_expression
    },
    {
        "invalid_system_type_expression",
        Type_invalid_system_type_expression
    },
    {
        "type_from_empty_entity",
        Type_type_from_empty_entity
    },
    {
        "type_from_empty_entity_in_progress",
        Type_type_from_empty_entity_in_progress
    },
    {
        "get_type",
        Type_get_type
    },
    {
        "get_type_from_empty",
        Type_get_type_from_empty
    },
    {
        "get_type_from_0",
        Type_get_type_from_0
    },
    {
        "entity_from_type",
        Type_entity_from_type
    },
    {
        "entity_from_empty_type",
        Type_entity_from_empty_type
    },
    {
        "entity_from_type_w_2_elements",
        Type_entity_from_type_w_2_elements
    },
    {
        "type_from_entity",
        Type_type_from_entity
    },
    {
        "type_from_empty",
        Type_type_from_empty
    },
    {
        "type_from_0",
        Type_type_from_0
    },
    {
        "type_to_expr_1_comp",
        Type_type_to_expr_1_comp
    },
    {
        "type_to_expr_2_comp",
        Type_type_to_expr_2_comp
    },
    {
        "type_to_expr_instanceof",
        Type_type_to_expr_instanceof
    },
    {
        "type_to_expr_childof",
        Type_type_to_expr_childof
    }
};

bake_test_case Run_testcases[] = {
    {
        "run",
        Run_run
    },
    {
        "run_w_param",
        Run_run_w_param
    },
    {
        "run_no_match",
        Run_run_no_match
    },
    {
        "run_w_offset",
        Run_run_w_offset
    },
    {
        "run_w_offset_skip_1_archetype",
        Run_run_w_offset_skip_1_archetype
    },
    {
        "run_w_offset_skip_1_archetype_plus_one",
        Run_run_w_offset_skip_1_archetype_plus_one
    },
    {
        "run_w_offset_skip_2_archetypes",
        Run_run_w_offset_skip_2_archetypes
    },
    {
        "run_w_limit_skip_1_archetype",
        Run_run_w_limit_skip_1_archetype
    },
    {
        "run_w_limit_skip_1_archetype_minus_one",
        Run_run_w_limit_skip_1_archetype_minus_one
    },
    {
        "run_w_limit_skip_2_archetypes",
        Run_run_w_limit_skip_2_archetypes
    },
    {
        "run_w_offset_1_limit_max",
        Run_run_w_offset_1_limit_max
    },
    {
        "run_w_offset_1_limit_minus_1",
        Run_run_w_offset_1_limit_minus_1
    },
    {
        "run_w_offset_2_type_limit_max",
        Run_run_w_offset_2_type_limit_max
    },
    {
        "run_w_offset_2_type_limit_minus_1",
        Run_run_w_offset_2_type_limit_minus_1
    },
    {
        "run_w_limit_1_all_offsets",
        Run_run_w_limit_1_all_offsets
    },
    {
        "run_w_offset_out_of_bounds",
        Run_run_w_offset_out_of_bounds
    },
    {
        "run_w_limit_out_of_bounds",
        Run_run_w_limit_out_of_bounds
    },
    {
        "run_w_component_filter",
        Run_run_w_component_filter
    },
    {
        "run_w_type_filter_of_2",
        Run_run_w_type_filter_of_2
    },
    {
        "run_w_container_filter",
        Run_run_w_container_filter
    },
    {
        "run_comb_10_entities_1_type",
        Run_run_comb_10_entities_1_type
    },
    {
        "run_comb_10_entities_2_types",
        Run_run_comb_10_entities_2_types
    },
    {
        "run_w_interrupt",
        Run_run_w_interrupt
    },
    {
        "run_staging",
        Run_run_staging
    }
};

bake_test_case MultiThread_testcases[] = {
    {
        "2_thread_1_entity",
        MultiThread_2_thread_1_entity
    },
    {
        "2_thread_2_entity",
        MultiThread_2_thread_2_entity
    },
    {
        "2_thread_5_entity",
        MultiThread_2_thread_5_entity
    },
    {
        "2_thread_10_entity",
        MultiThread_2_thread_10_entity
    },
    {
        "3_thread_1_entity",
        MultiThread_3_thread_1_entity
    },
    {
        "3_thread_2_entity",
        MultiThread_3_thread_2_entity
    },
    {
        "3_thread_5_entity",
        MultiThread_3_thread_5_entity
    },
    {
        "3_thread_10_entity",
        MultiThread_3_thread_10_entity
    },
    {
        "4_thread_1_entity",
        MultiThread_4_thread_1_entity
    },
    {
        "4_thread_2_entity",
        MultiThread_4_thread_2_entity
    },
    {
        "4_thread_5_entity",
        MultiThread_4_thread_5_entity
    },
    {
        "4_thread_10_entity",
        MultiThread_4_thread_10_entity
    },
    {
        "5_thread_1_entity",
        MultiThread_5_thread_1_entity
    },
    {
        "5_thread_2_entity",
        MultiThread_5_thread_2_entity
    },
    {
        "5_thread_5_entity",
        MultiThread_5_thread_5_entity
    },
    {
        "5_thread_10_entity",
        MultiThread_5_thread_10_entity
    },
    {
        "6_thread_1_entity",
        MultiThread_6_thread_1_entity
    },
    {
        "6_thread_2_entity",
        MultiThread_6_thread_2_entity
    },
    {
        "6_thread_5_entity",
        MultiThread_6_thread_5_entity
    },
    {
        "6_thread_10_entity",
        MultiThread_6_thread_10_entity
    },
    {
        "2_thread_test_combs_100_entity",
        MultiThread_2_thread_test_combs_100_entity
    },
    {
        "3_thread_test_combs_100_entity",
        MultiThread_3_thread_test_combs_100_entity
    },
    {
        "4_thread_test_combs_100_entity",
        MultiThread_4_thread_test_combs_100_entity
    },
    {
        "5_thread_test_combs_100_entity",
        MultiThread_5_thread_test_combs_100_entity
    },
    {
        "6_thread_test_combs_100_entity",
        MultiThread_6_thread_test_combs_100_entity
    },
    {
        "2_thread_test_combs_100_entity_2_types",
        MultiThread_2_thread_test_combs_100_entity_2_types
    },
    {
        "3_thread_test_combs_100_entity_2_types",
        MultiThread_3_thread_test_combs_100_entity_2_types
    },
    {
        "4_thread_test_combs_100_entity_2_types",
        MultiThread_4_thread_test_combs_100_entity_2_types
    },
    {
        "5_thread_test_combs_100_entity_2_types",
        MultiThread_5_thread_test_combs_100_entity_2_types
    },
    {
        "6_thread_test_combs_100_entity_2_types",
        MultiThread_6_thread_test_combs_100_entity_2_types
    },
    {
        "change_thread_count",
        MultiThread_change_thread_count
    },
    {
        "multithread_quit",
        MultiThread_multithread_quit
    },
    {
        "schedule_w_tasks",
        MultiThread_schedule_w_tasks
    },
    {
        "reactive_system",
        MultiThread_reactive_system
    }
};

bake_test_case SingleThreadStaging_testcases[] = {
    {
        "new_empty",
        SingleThreadStaging_new_empty
    },
    {
        "new_w_component",
        SingleThreadStaging_new_w_component
    },
    {
        "new_w_type_of_2",
        SingleThreadStaging_new_w_type_of_2
    },
    {
        "new_empty_w_count",
        SingleThreadStaging_new_empty_w_count
    },
    {
        "new_component_w_count",
        SingleThreadStaging_new_component_w_count
    },
    {
        "new_type_w_count",
        SingleThreadStaging_new_type_w_count
    },
    {
        "add_to_new_empty",
        SingleThreadStaging_add_to_new_empty
    },
    {
        "2_add_to_new_empty",
        SingleThreadStaging_2_add_to_new_empty
    },
    {
        "add_remove_same_to_new_empty",
        SingleThreadStaging_add_remove_same_to_new_empty
    },
    {
        "add_remove_2_same_to_new_empty",
        SingleThreadStaging_add_remove_2_same_to_new_empty
    },
    {
        "add_remove_same_to_new_w_component",
        SingleThreadStaging_add_remove_same_to_new_w_component
    },
    {
        "2_add_1_remove_to_new_empty",
        SingleThreadStaging_2_add_1_remove_to_new_empty
    },
    {
        "2_add_1_remove_same_to_new_empty",
        SingleThreadStaging_2_add_1_remove_same_to_new_empty
    },
    {
        "clone",
        SingleThreadStaging_clone
    },
    {
        "clone_w_value",
        SingleThreadStaging_clone_w_value
    },
    {
        "add_to_current",
        SingleThreadStaging_add_to_current
    },
    {
        "2_add_to_current",
        SingleThreadStaging_2_add_to_current
    },
    {
        "remove_from_current",
        SingleThreadStaging_remove_from_current
    },
    {
        "remove_2_from_current",
        SingleThreadStaging_remove_2_from_current
    },
    {
        "add_remove_same_to_current",
        SingleThreadStaging_add_remove_same_to_current
    },
    {
        "add_remove_same_existing_to_current",
        SingleThreadStaging_add_remove_same_existing_to_current
    },
    {
        "remove_add_same_to_current",
        SingleThreadStaging_remove_add_same_to_current
    },
    {
        "remove_add_same_existing_to_current",
        SingleThreadStaging_remove_add_same_existing_to_current
    },
    {
        "add_remove_2_same_to_current",
        SingleThreadStaging_add_remove_2_same_to_current
    },
    {
        "add_remove_2_same_existing_to_current",
        SingleThreadStaging_add_remove_2_same_existing_to_current
    },
    {
        "remove_add_2_same_to_current",
        SingleThreadStaging_remove_add_2_same_to_current
    },
    {
        "remove_add_2_same_existing_to_current",
        SingleThreadStaging_remove_add_2_same_existing_to_current
    },
    {
        "add_remove_different_to_current",
        SingleThreadStaging_add_remove_different_to_current
    },
    {
        "add_remove_add_same_to_current",
        SingleThreadStaging_add_remove_add_same_to_current
    },
    {
        "2_add_1_remove_to_current",
        SingleThreadStaging_2_add_1_remove_to_current
    },
    {
        "1_add_2_remove_to_current",
        SingleThreadStaging_1_add_2_remove_to_current
    },
    {
        "delete_current",
        SingleThreadStaging_delete_current
    },
    {
        "delete_even",
        SingleThreadStaging_delete_even
    },
    {
        "delete_new_empty",
        SingleThreadStaging_delete_new_empty
    },
    {
        "delete_new_w_component",
        SingleThreadStaging_delete_new_w_component
    },
    {
        "set_current",
        SingleThreadStaging_set_current
    },
    {
        "set_new_empty",
        SingleThreadStaging_set_new_empty
    },
    {
        "set_new_w_component",
        SingleThreadStaging_set_new_w_component
    },
    {
        "set_existing_new_w_component",
        SingleThreadStaging_set_existing_new_w_component
    },
    {
        "set_new_after_add",
        SingleThreadStaging_set_new_after_add
    },
    {
        "remove_after_set",
        SingleThreadStaging_remove_after_set
    },
    {
        "delete_after_set",
        SingleThreadStaging_delete_after_set
    },
    {
        "add_to_current_in_on_add",
        SingleThreadStaging_add_to_current_in_on_add
    },
    {
        "remove_from_current_in_on_add",
        SingleThreadStaging_remove_from_current_in_on_add
    },
    {
        "remove_added_component_in_on_add",
        SingleThreadStaging_remove_added_component_in_on_add
    },
    {
        "match_table_created_in_progress",
        SingleThreadStaging_match_table_created_in_progress
    },
    {
        "match_table_created_w_new_in_progress",
        SingleThreadStaging_match_table_created_w_new_in_progress
    },
    {
        "match_table_created_w_new_in_on_set",
        SingleThreadStaging_match_table_created_w_new_in_on_set
    },
    {
        "merge_table_w_container_added_in_progress",
        SingleThreadStaging_merge_table_w_container_added_in_progress
    },
    {
        "merge_table_w_container_added_on_set",
        SingleThreadStaging_merge_table_w_container_added_on_set
    },
    {
        "merge_table_w_container_added_on_set_reverse",
        SingleThreadStaging_merge_table_w_container_added_on_set_reverse
    },
    {
        "merge_after_tasks",
        SingleThreadStaging_merge_after_tasks
    },
    {
        "override_after_remove_in_progress",
        SingleThreadStaging_override_after_remove_in_progress
    },
    {
        "get_parent_in_progress",
        SingleThreadStaging_get_parent_in_progress
    },
    {
        "merge_once",
        SingleThreadStaging_merge_once
    },
    {
        "get_mutable",
        SingleThreadStaging_get_mutable
    },
    {
        "get_mutable_from_main",
        SingleThreadStaging_get_mutable_from_main
    },
    {
        "get_mutable_w_add",
        SingleThreadStaging_get_mutable_w_add
    },
    {
        "on_add_after_new_type_in_progress",
        SingleThreadStaging_on_add_after_new_type_in_progress
    }
};

bake_test_case MultiThreadStaging_testcases[] = {
    {
        "2_threads_add_to_current",
        MultiThreadStaging_2_threads_add_to_current
    },
    {
        "3_threads_add_to_current",
        MultiThreadStaging_3_threads_add_to_current
    },
    {
        "4_threads_add_to_current",
        MultiThreadStaging_4_threads_add_to_current
    },
    {
        "5_threads_add_to_current",
        MultiThreadStaging_5_threads_add_to_current
    },
    {
        "6_threads_add_to_current",
        MultiThreadStaging_6_threads_add_to_current
    },
    {
        "2_threads_on_add",
        MultiThreadStaging_2_threads_on_add
    },
    {
        "new_w_count",
        MultiThreadStaging_new_w_count
    }
};

bake_test_case Stresstests_testcases[] = {
    {
        "create_delete_entity_random_components",
        Stresstests_create_delete_entity_random_components
    },
    {
        "set_entity_random_components",
        Stresstests_set_entity_random_components
    },
    {
        "create_delete_entity_random_components_staged",
        Stresstests_create_delete_entity_random_components_staged
    },
    {
        "set_entity_random_components_staged",
        Stresstests_set_entity_random_components_staged
    },
    {
        "create_delete_entity_random_components_2_threads",
        Stresstests_create_delete_entity_random_components_2_threads
    },
    {
        "set_entity_random_components_2_threads",
        Stresstests_set_entity_random_components_2_threads
    },
    {
        "create_delete_entity_random_components_6_threads",
        Stresstests_create_delete_entity_random_components_6_threads
    },
    {
        "set_entity_random_components_6_threads",
        Stresstests_set_entity_random_components_6_threads
    },
    {
        "create_delete_entity_random_components_12_threads",
        Stresstests_create_delete_entity_random_components_12_threads
    },
    {
        "set_entity_random_components_12_threads",
        Stresstests_set_entity_random_components_12_threads
    },
    {
        "create_2m_entities_1_comp",
        Stresstests_create_2m_entities_1_comp
    },
    {
        "create_2m_entities_bulk_1_comp",
        Stresstests_create_2m_entities_bulk_1_comp
    },
    {
        "add_1k_tags",
        Stresstests_add_1k_tags
    }
};

bake_test_case Snapshot_testcases[] = {
    {
        "simple_snapshot",
        Snapshot_simple_snapshot
    },
    {
        "snapshot_after_new",
        Snapshot_snapshot_after_new
    },
    {
        "snapshot_after_delete",
        Snapshot_snapshot_after_delete
    },
    {
        "snapshot_after_new_type",
        Snapshot_snapshot_after_new_type
    },
    {
        "snapshot_after_add",
        Snapshot_snapshot_after_add
    },
    {
        "snapshot_after_remove",
        Snapshot_snapshot_after_remove
    },
    {
        "snapshot_w_include_filter",
        Snapshot_snapshot_w_include_filter
    },
    {
        "snapshot_w_exclude_filter",
        Snapshot_snapshot_w_exclude_filter
    },
    {
        "snapshot_w_filter_after_new",
        Snapshot_snapshot_w_filter_after_new
    },
    {
        "snapshot_w_filter_after_delete",
        Snapshot_snapshot_w_filter_after_delete
    },
    {
        "snapshot_free_empty",
        Snapshot_snapshot_free_empty
    },
    {
        "snapshot_free",
        Snapshot_snapshot_free
    },
    {
        "snapshot_free_filtered",
        Snapshot_snapshot_free_filtered
    },
    {
        "snapshot_activate_table_w_filter",
        Snapshot_snapshot_activate_table_w_filter
    },
    {
        "snapshot_copy",
        Snapshot_snapshot_copy
    },
    {
        "snapshot_copy_filtered",
        Snapshot_snapshot_copy_filtered
    },
    {
        "snapshot_copy_w_filter",
        Snapshot_snapshot_copy_w_filter
    },
    {
        "snapshot_get_ref_after_restore",
        Snapshot_snapshot_get_ref_after_restore
    },
    {
        "new_after_snapshot",
        Snapshot_new_after_snapshot
    },
    {
        "new_empty_after_snapshot",
        Snapshot_new_empty_after_snapshot
    },
    {
        "add_after_snapshot",
        Snapshot_add_after_snapshot
    },
    {
        "delete_after_snapshot",
        Snapshot_delete_after_snapshot
    },
    {
        "set_after_snapshot",
        Snapshot_set_after_snapshot
    },
    {
        "restore_recycled",
        Snapshot_restore_recycled
    },
    {
        "snapshot_w_new_in_onset",
        Snapshot_snapshot_w_new_in_onset
    },
    {
        "snapshot_w_new_in_onset_in_snapshot_table",
        Snapshot_snapshot_w_new_in_onset_in_snapshot_table
    }
};

bake_test_case ReaderWriter_testcases[] = {
    {
        "simple",
        ReaderWriter_simple
    },
    {
        "id",
        ReaderWriter_id
    },
    {
        "id_w_simple",
        ReaderWriter_id_w_simple
    },
    {
        "unaligned",
        ReaderWriter_unaligned
    },
    {
        "empty",
        ReaderWriter_empty
    },
    {
        "tag",
        ReaderWriter_tag
    },
    {
        "simple_w_tag",
        ReaderWriter_simple_w_tag
    },
    {
        "tag_w_simple",
        ReaderWriter_tag_w_simple
    },
    {
        "empty_parent",
        ReaderWriter_empty_parent
    },
    {
        "parent",
        ReaderWriter_parent
    },
    {
        "simple_w_parent",
        ReaderWriter_simple_w_parent
    },
    {
        "inheritance",
        ReaderWriter_inheritance
    },
    {
        "simple_w_inheritance",
        ReaderWriter_simple_w_inheritance
    },
    {
        "deserialize_twice",
        ReaderWriter_deserialize_twice
    },
    {
        "entity_conflict",
        ReaderWriter_entity_conflict
    },
    {
        "snapshot_reader_simple",
        ReaderWriter_snapshot_reader_simple
    },
    {
        "snapshot_reader_id",
        ReaderWriter_snapshot_reader_id
    },
    {
        "read_zero_size",
        ReaderWriter_read_zero_size
    },
    {
        "write_zero_size",
        ReaderWriter_write_zero_size
    },
    {
        "invalid_header",
        ReaderWriter_invalid_header
    }
};

bake_test_case FilterIter_testcases[] = {
    {
        "iter_one_table",
        FilterIter_iter_one_table
    },
    {
        "iter_two_tables",
        FilterIter_iter_two_tables
    },
    {
        "iter_two_comps",
        FilterIter_iter_two_comps
    },
    {
        "iter_snapshot_one_table",
        FilterIter_iter_snapshot_one_table
    },
    {
        "iter_snapshot_two_tables",
        FilterIter_iter_snapshot_two_tables
    },
    {
        "iter_snapshot_two_comps",
        FilterIter_iter_snapshot_two_comps
    },
    {
        "iter_snapshot_filtered_table",
        FilterIter_iter_snapshot_filtered_table
    }
};

bake_test_case Modules_testcases[] = {
    {
        "simple_module",
        Modules_simple_module
    },
    {
        "import_module_from_system",
        Modules_import_module_from_system
    },
    {
        "import_again",
        Modules_import_again
    },
    {
        "scoped_component",
        Modules_scoped_component
    },
    {
        "scoped_tag",
        Modules_scoped_tag
    },
    {
        "scoped_system",
        Modules_scoped_system
    },
    {
        "scoped_entity",
        Modules_scoped_entity
    },
    {
        "name_prefix_component",
        Modules_name_prefix_component
    },
    {
        "name_prefix_tag",
        Modules_name_prefix_tag
    },
    {
        "name_prefix_system",
        Modules_name_prefix_system
    },
    {
        "name_prefix_entity",
        Modules_name_prefix_entity
    },
    {
        "name_prefix_type",
        Modules_name_prefix_type
    },
    {
        "name_prefix_prefab",
        Modules_name_prefix_prefab
    },
    {
        "name_prefix_pipeline",
        Modules_name_prefix_pipeline
    },
    {
        "name_prefix_trigger",
        Modules_name_prefix_trigger
    },
    {
        "name_prefix_underscore",
        Modules_name_prefix_underscore
    },
    {
        "lookup_by_symbol",
        Modules_lookup_by_symbol
    }
};

bake_test_case Internals_testcases[] = {
    {
        "deactivate_table",
        Internals_deactivate_table
    },
    {
        "activate_table",
        Internals_activate_table
    },
    {
        "activate_deactivate_table",
        Internals_activate_deactivate_table
    },
    {
        "activate_deactivate_reactive",
        Internals_activate_deactivate_reactive
    },
    {
        "activate_deactivate_activate_other",
        Internals_activate_deactivate_activate_other
    },
    {
        "no_double_system_table_after_merge",
        Internals_no_double_system_table_after_merge
    }
};

bake_test_case Error_testcases[] = {
    {
        "abort",
        Error_abort
    },
    {
        "abort_w_param",
        Error_abort_w_param
    },
    {
        "override_abort",
        Error_override_abort
    },
    {
        "assert_true",
        Error_assert_true
    },
    {
        "assert_false",
        Error_assert_false
    },
    {
        "assert_false_w_param",
        Error_assert_false_w_param
    },
    {
        "error_codes",
        Error_error_codes
    },
    {
        "log_dbg",
        Error_log_dbg
    },
    {
        "log_log",
        Error_log_log
    },
    {
        "log_warning",
        Error_log_warning
    },
    {
        "log_error",
        Error_log_error
    }
};

static bake_test_suite suites[] = {
    {
        "New",
        NULL,
        NULL,
        20,
        New_testcases
    },
    {
        "New_w_Count",
        NULL,
        NULL,
        12,
        New_w_Count_testcases
    },
    {
        "Add",
        NULL,
        NULL,
        35,
        Add_testcases
    },
    {
        "Remove",
        NULL,
        NULL,
        15,
        Remove_testcases
    },
    {
        "Hierarchies",
        NULL,
        NULL,
        58,
        Hierarchies_testcases
    },
    {
        "Add_bulk",
        NULL,
        NULL,
        13,
        Add_bulk_testcases
    },
    {
        "Remove_bulk",
        NULL,
        NULL,
        12,
        Remove_bulk_testcases
    },
    {
        "Add_remove_bulk",
        NULL,
        NULL,
        4,
        Add_remove_bulk_testcases
    },
    {
        "Has",
        NULL,
        NULL,
        19,
        Has_testcases
    },
    {
        "Count",
        NULL,
        NULL,
        5,
        Count_testcases
    },
    {
        "Get_component",
        NULL,
        NULL,
        9,
        Get_component_testcases
    },
    {
        "Delete",
        NULL,
        NULL,
        9,
        Delete_testcases
    },
    {
        "Delete_w_filter",
        NULL,
        NULL,
        12,
        Delete_w_filter_testcases
    },
    {
        "Set",
        NULL,
        NULL,
        16,
        Set_testcases
    },
    {
        "Lookup",
        NULL,
        NULL,
        11,
        Lookup_testcases
    },
    {
        "Singleton",
        NULL,
        NULL,
        7,
        Singleton_testcases
    },
    {
        "Clone",
        NULL,
        NULL,
        14,
        Clone_testcases
    },
    {
        "ComponentLifecycle",
        NULL,
        NULL,
        23,
        ComponentLifecycle_testcases
    },
    {
        "Pipeline",
        NULL,
        NULL,
        15,
        Pipeline_testcases
    },
    {
        "SystemMisc",
        NULL,
        NULL,
        50,
        SystemMisc_testcases
    },
    {
        "Sorting",
        NULL,
        NULL,
        13,
        Sorting_testcases
    },
    {
        "Queries",
        NULL,
        NULL,
        7,
        Queries_testcases
    },
    {
        "Traits",
        NULL,
        NULL,
        8,
        Traits_testcases
    },
    {
        "TriggerOnAdd",
        NULL,
        NULL,
        26,
        TriggerOnAdd_testcases
    },
    {
        "TriggerOnRemove",
        NULL,
        NULL,
        8,
        TriggerOnRemove_testcases
    },
    {
        "TriggerOnSet",
        NULL,
        NULL,
        11,
        TriggerOnSet_testcases
    },
    {
        "Monitor",
        NULL,
        NULL,
        5,
        Monitor_testcases
    },
    {
        "SystemOnSet",
        NULL,
        NULL,
        27,
        SystemOnSet_testcases
    },
    {
        "SystemPeriodic",
        NULL,
        NULL,
        51,
        SystemPeriodic_testcases
    },
    {
        "Timer",
        NULL,
        NULL,
        9,
        Timer_testcases
    },
    {
        "SystemOnDemand",
        NULL,
        NULL,
        27,
        SystemOnDemand_testcases
    },
    {
        "SystemCascade",
        NULL,
        NULL,
        4,
        SystemCascade_testcases
    },
    {
        "SystemManual",
        NULL,
        NULL,
        3,
        SystemManual_testcases
    },
    {
        "Tasks",
        NULL,
        NULL,
        4,
        Tasks_testcases
    },
    {
        "Prefab",
        NULL,
        NULL,
        60,
        Prefab_testcases
    },
    {
        "System_w_FromContainer",
        NULL,
        NULL,
        20,
        System_w_FromContainer_testcases
    },
    {
        "System_w_FromId",
        NULL,
        NULL,
        3,
        System_w_FromId_testcases
    },
    {
        "System_w_FromSystem",
        NULL,
        NULL,
        3,
        System_w_FromSystem_testcases
    },
    {
        "System_w_FromEntity",
        NULL,
        NULL,
        3,
        System_w_FromEntity_testcases
    },
    {
        "World",
        NULL,
        NULL,
        32,
        World_testcases
    },
    {
        "Type",
        NULL,
        NULL,
        48,
        Type_testcases
    },
    {
        "Run",
        NULL,
        NULL,
        24,
        Run_testcases
    },
    {
        "MultiThread",
        NULL,
        NULL,
        34,
        MultiThread_testcases
    },
    {
        "SingleThreadStaging",
        NULL,
        NULL,
        59,
        SingleThreadStaging_testcases
    },
    {
        "MultiThreadStaging",
        NULL,
        NULL,
        7,
        MultiThreadStaging_testcases
    },
    {
        "Stresstests",
        NULL,
        NULL,
        13,
        Stresstests_testcases
    },
    {
        "Snapshot",
        NULL,
        NULL,
        26,
        Snapshot_testcases
    },
    {
        "ReaderWriter",
        NULL,
        NULL,
        20,
        ReaderWriter_testcases
    },
    {
        "FilterIter",
        NULL,
        NULL,
        7,
        FilterIter_testcases
    },
    {
        "Modules",
        NULL,
        NULL,
        17,
        Modules_testcases
    },
    {
        "Internals",
        NULL,
        NULL,
        6,
        Internals_testcases
    },
    {
        "Error",
        Error_setup,
        NULL,
        11,
        Error_testcases
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("api", argc, argv, suites, 52);
}
