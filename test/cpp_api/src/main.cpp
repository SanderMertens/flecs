
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpp_api.h>

// Testsuite 'PrettyFunction'
void PrettyFunction_component(void);
void PrettyFunction_enum(void);

// Testsuite 'Entity'
void Entity_new(void);
void Entity_new_named(void);
void Entity_new_named_from_scope(void);
void Entity_new_nested_named_from_scope(void);
void Entity_new_nested_named_from_nested_scope(void);
void Entity_new_add(void);
void Entity_new_add_2(void);
void Entity_new_set(void);
void Entity_new_set_2(void);
void Entity_add(void);
void Entity_add_2(void);
void Entity_add_entity(void);
void Entity_add_childof(void);
void Entity_add_instanceof(void);
void Entity_remove(void);
void Entity_remove_2(void);
void Entity_remove_entity(void);
void Entity_remove_childof(void);
void Entity_remove_instanceof(void);
void Entity_set(void);
void Entity_set_2(void);
void Entity_emplace(void);
void Entity_emplace_2(void);
void Entity_emplace_after_add(void);
void Entity_emplace_after_add_pair(void);
void Entity_emplace_pair(void);
void Entity_emplace_pair_w_entity(void);
void Entity_emplace_pair_type(void);
void Entity_emplace_pair_second(void);
void Entity_get_generic(void);
void Entity_get_mut_generic(void);
void Entity_get_generic_w_id(void);
void Entity_get_generic_w_id_t(void);
void Entity_get_mut_generic_w_id(void);
void Entity_get_mut_generic_w_id_t(void);
void Entity_set_generic(void);
void Entity_set_generic_w_id(void);
void Entity_set_generic_w_id_t(void);
void Entity_set_generic_no_size(void);
void Entity_set_generic_no_size_w_id(void);
void Entity_set_generic_no_size_w_id_t(void);
void Entity_add_role(void);
void Entity_remove_role(void);
void Entity_has_role(void);
void Entity_pair_role(void);
void Entity_equals(void);
void Entity_compare_0(void);
void Entity_compare_id_t(void);
void Entity_compare_id(void);
void Entity_compare_literal(void);
void Entity_greater_than(void);
void Entity_less_than(void);
void Entity_not_0_or_1(void);
void Entity_not_true_or_false(void);
void Entity_has_childof(void);
void Entity_has_instanceof(void);
void Entity_has_instanceof_indirect(void);
void Entity_null_string(void);
void Entity_set_name(void);
void Entity_change_name(void);
void Entity_delete(void);
void Entity_clear(void);
void Entity_foce_owned(void);
void Entity_force_owned_2(void);
void Entity_force_owned_nested(void);
void Entity_tag_has_size_zero(void);
void Entity_get_null_name(void);
void Entity_get_target(void);
void Entity_get_parent(void);
void Entity_is_component_enabled(void);
void Entity_is_enabled_component_enabled(void);
void Entity_is_disabled_component_enabled(void);
void Entity_is_pair_enabled(void);
void Entity_is_enabled_pair_enabled(void);
void Entity_is_disabled_pair_enabled(void);
void Entity_is_pair_enabled_w_ids(void);
void Entity_is_enabled_pair_enabled_w_ids(void);
void Entity_is_disabled_pair_enabled_w_ids(void);
void Entity_is_pair_enabled_w_tgt_id(void);
void Entity_is_enabled_pair_enabled_w_tgt_id(void);
void Entity_is_disabled_pair_enabled_w_tgt_id(void);
void Entity_get_type(void);
void Entity_get_nonempty_type(void);
void Entity_set_no_copy(void);
void Entity_set_copy(void);
void Entity_set_deduced(void);
void Entity_override(void);
void Entity_override_id(void);
void Entity_override_pair(void);
void Entity_override_pair_w_tgt_id(void);
void Entity_override_pair_w_ids(void);
void Entity_set_override(void);
void Entity_set_override_lvalue(void);
void Entity_set_override_pair(void);
void Entity_set_override_pair_w_tgt_id(void);
void Entity_set_override_pair_w_rel_tag(void);
void Entity_emplace_override(void);
void Entity_emplace_override_pair(void);
void Entity_implicit_name_to_char(void);
void Entity_implicit_path_to_char(void);
void Entity_implicit_type_str_to_char(void);
void Entity_entity_to_entity_view(void);
void Entity_entity_view_to_entity_world(void);
void Entity_entity_view_to_entity_stage(void);
void Entity_create_entity_view_from_stage(void);
void Entity_set_template(void);
void Entity_get_1_component_w_callback(void);
void Entity_get_2_components_w_callback(void);
void Entity_set_1_component_w_callback(void);
void Entity_set_2_components_w_callback(void);
void Entity_set_3_components_w_callback(void);
void Entity_get_mut_1_component_w_callback(void);
void Entity_get_mut_2_components_w_callback(void);
void Entity_get_component_w_callback_nested(void);
void Entity_get_mut_component_w_callback_nested(void);
void Entity_defer_set_1_component(void);
void Entity_defer_set_2_components(void);
void Entity_defer_set_3_components(void);
void Entity_set_2_w_on_set(void);
void Entity_defer_set_2_w_on_set(void);
void Entity_set_2_after_fluent(void);
void Entity_set_2_before_fluent(void);
void Entity_set_2_after_set_1(void);
void Entity_set_2_after_set_2(void);
void Entity_with_self(void);
void Entity_with_relation_type_self(void);
void Entity_with_relation_self(void);
void Entity_with_self_w_name(void);
void Entity_with_self_nested(void);
void Entity_with_after_builder_method(void);
void Entity_with_before_builder_method(void);
void Entity_with_scope(void);
void Entity_with_scope_nested(void);
void Entity_with_scope_nested_same_name_as_parent(void);
void Entity_scope_after_builder_method(void);
void Entity_scope_before_builder_method(void);
void Entity_no_recursive_lookup(void);
void Entity_defer_new_w_name(void);
void Entity_defer_new_w_nested_name(void);
void Entity_defer_new_w_scope_name(void);
void Entity_defer_new_w_scope_nested_name(void);
void Entity_defer_new_w_deferred_scope_nested_name(void);
void Entity_defer_new_w_scope(void);
void Entity_defer_new_w_with(void);
void Entity_defer_new_w_name_scope_with(void);
void Entity_defer_w_with_implicit_component(void);
void Entity_defer_suspend_resume(void);
void Entity_entity_id_str(void);
void Entity_pair_id_str(void);
void Entity_role_id_str(void);
void Entity_id_str_from_entity_view(void);
void Entity_id_str_from_entity(void);
void Entity_null_entity(void);
void Entity_null_entity_w_world(void);
void Entity_null_entity_w_0(void);
void Entity_null_entity_w_world_w_0(void);
void Entity_entity_view_null_entity(void);
void Entity_entity_view_null_entity_w_world(void);
void Entity_entity_view_null_entity_w_0(void);
void Entity_entity_view_null_entity_w_world_w_0(void);
void Entity_is_wildcard(void);
void Entity_has_id_t(void);
void Entity_has_pair_id_t(void);
void Entity_has_pair_id_t_w_type(void);
void Entity_has_id(void);
void Entity_has_pair_id(void);
void Entity_has_pair_id_w_type(void);
void Entity_has_wildcard_id(void);
void Entity_has_wildcard_pair_id(void);
void Entity_owns_id_t(void);
void Entity_owns_pair_id_t(void);
void Entity_owns_pair_id_t_w_type(void);
void Entity_owns_id(void);
void Entity_owns_pair_id(void);
void Entity_owns_pair_id_w_type(void);
void Entity_owns_wildcard_id(void);
void Entity_owns_wildcard_pair(void);
void Entity_id_from_world(void);
void Entity_id_pair_from_world(void);
void Entity_id_default_from_world(void);
void Entity_is_a(void);
void Entity_is_a_w_type(void);
void Entity_child_of(void);
void Entity_child_of_w_type(void);
void Entity_slot_of(void);
void Entity_slot_of_w_type(void);
void Entity_slot(void);
void Entity_id_get_entity(void);
void Entity_id_get_invalid_entity(void);
void Entity_each_in_stage(void);
void Entity_iter_recycled_parent(void);
void Entity_get_lambda_from_stage(void);
void Entity_default_ctor(void);
void Entity_get_obj_by_template(void);
void Entity_create_named_twice_deferred(void);
void Entity_clone(void);
void Entity_clone_w_value(void);
void Entity_clone_to_existing(void);
void Entity_clone_to_existing_overlap(void);
void Entity_set_doc_name(void);
void Entity_set_doc_brief(void);
void Entity_set_doc_detail(void);
void Entity_set_doc_link(void);
void Entity_entity_w_root_name(void);
void Entity_entity_w_root_name_from_scope(void);
void Entity_entity_w_type(void);
void Entity_entity_w_nested_type(void);
void Entity_entity_w_type_defer(void);
void Entity_prefab_hierarchy_w_types(void);
void Entity_prefab_hierarchy_w_root_types(void);
void Entity_prefab_hierarchy_w_child_override(void);
void Entity_entity_array(void);
void Entity_add_if_true_T(void);
void Entity_add_if_false_T(void);
void Entity_add_if_true_id(void);
void Entity_add_if_false_id(void);
void Entity_add_if_true_R_O(void);
void Entity_add_if_false_R_O(void);
void Entity_add_if_true_R_o(void);
void Entity_add_if_false_R_o(void);
void Entity_add_if_true_r_o(void);
void Entity_add_if_exclusive_r_o(void);
void Entity_add_if_exclusive_R_o(void);
void Entity_add_if_exclusive_R_O(void);
void Entity_add_if_pair_w_0_object(void);
void Entity_children_w_custom_relation(void);
void Entity_children_w_custom_relation_type(void);
void Entity_get_depth(void);
void Entity_get_depth_w_type(void);

// Testsuite 'Pairs'
void Pairs_add_component_pair(void);
void Pairs_add_tag_pair(void);
void Pairs_add_tag_pair_to_tag(void);
void Pairs_remove_component_pair(void);
void Pairs_remove_tag_pair(void);
void Pairs_remove_tag_pair_to_tag(void);
void Pairs_set_component_pair(void);
void Pairs_set_tag_pair(void);
void Pairs_system_1_pair_instance(void);
void Pairs_system_2_pair_instances(void);
void Pairs_override_pair(void);
void Pairs_override_tag_pair(void);
void Pairs_get_mut_pair(void);
void Pairs_get_mut_pair_existing(void);
void Pairs_get_mut_pair_tag(void);
void Pairs_get_mut_pair_tag_existing(void);
void Pairs_get_mut_R_tag_O(void);
void Pairs_get_relation_from_id(void);
void Pairs_get_second_from_id(void);
void Pairs_get_recycled_relation_from_id(void);
void Pairs_get_recycled_object_from_id(void);
void Pairs_get_rel_obj(void);
void Pairs_get_rel_obj_id(void);
void Pairs_get_rel_obj_id_t(void);
void Pairs_get_R_obj(void);
void Pairs_get_R_obj_id(void);
void Pairs_get_R_obj_id_t(void);
void Pairs_get_R_O(void);
void Pairs_get_R_tag_O(void);
void Pairs_get_second(void);
void Pairs_get_second_id(void);
void Pairs_get_second_id_t(void);
void Pairs_each(void);
void Pairs_each_pair(void);
void Pairs_each_pair_by_type(void);
void Pairs_each_pair_w_isa(void);
void Pairs_each_pair_w_recycled_rel(void);
void Pairs_each_pair_w_recycled_obj(void);
void Pairs_match_pair(void);
void Pairs_match_pair_obj_wildcard(void);
void Pairs_match_pair_rel_wildcard(void);
void Pairs_match_pair_both_wildcard(void);
void Pairs_has_tag_w_object(void);
void Pairs_has_second_tag(void);
void Pairs_add_pair_type(void);
void Pairs_remove_pair_type(void);
void Pairs_set_pair_type(void);
void Pairs_has_pair_type(void);
void Pairs_get_1_pair_arg(void);
void Pairs_get_2_pair_arg(void);
void Pairs_set_1_pair_arg(void);
void Pairs_set_2_pair_arg(void);
void Pairs_get_inline_pair_type(void);
void Pairs_set_inline_pair_type(void);
void Pairs_get_pair_type_object(void);
void Pairs_set_pair_type_object(void);
void Pairs_set_get_second_variants(void);
void Pairs_get_object_for_type_self(void);
void Pairs_get_object_for_type_base(void);
void Pairs_get_object_for_id_self(void);
void Pairs_get_object_for_id_base(void);
void Pairs_get_object_for_id_not_found(void);
void Pairs_deref_pair(void);
void Pairs_deref_const_pair(void);
void Pairs_deref_pair_obj(void);
void Pairs_deref_const_pair_obj(void);
void Pairs_set_R_existing_value(void);

// Testsuite 'Enum'
void Enum_standard_enum_reflection(void);
void Enum_sparse_enum_reflection(void);
void Enum_enum_class_reflection(void);
void Enum_prefixed_enum_reflection(void);
void Enum_constant_with_num_reflection(void);
void Enum_get_constant_id(void);
void Enum_add_enum_constant(void);
void Enum_add_enum_class_constant(void);
void Enum_replace_enum_constants(void);
void Enum_has_enum(void);
void Enum_has_enum_wildcard(void);
void Enum_get_enum(void);
void Enum_remove_enum(void);
void Enum_remove_wildcard(void);
void Enum_enum_as_component(void);
void Enum_query_enum_wildcard(void);
void Enum_query_enum_constant(void);
void Enum_enum_type_from_stage(void);
void Enum_add_enum_from_stage(void);
void Enum_enum_w_2_worlds(void);
void Enum_add_enum_constant_w_tag(void);
void Enum_remove_enum_constant_w_tag(void);
void Enum_set_enum_constant_w_tag(void);
void Enum_enum_w_incorrect_size(void);
void Enum_add_union_enum(void);
void Enum_add_2_union_enums(void);
void Enum_add_2_union_enums_reverse(void);
void Enum_constant_from_entity(void);
void Enum_add_if(void);
void Enum_add_if_other(void);
void Enum_query_union_enum(void);
void Enum_query_union_enum_invalid_query_type(void);

// Testsuite 'Switch'
void Switch_add_case(void);
void Switch_get_case(void);
void Switch_system_w_case(void);
void Switch_system_w_case_builder(void);
void Switch_system_w_sw_type_builder(void);
void Switch_system_w_switch(void);
void Switch_add_case_w_type(void);
void Switch_add_switch_w_type(void);
void Switch_add_remove_switch_w_type(void);
void Switch_switch_enum_type(void);

// Testsuite 'Paths'
void Paths_name(void);
void Paths_path_depth_1(void);
void Paths_path_depth_2(void);
void Paths_entity_lookup_name(void);
void Paths_entity_lookup_depth_1(void);
void Paths_entity_lookup_depth_2(void);
void Paths_entity_lookup_from_0(void);
void Paths_entity_lookup_from_0_w_world(void);
void Paths_alias_component(void);
void Paths_alias_scoped_component(void);
void Paths_alias_scoped_component_w_name(void);
void Paths_alias_entity(void);
void Paths_alias_entity_by_name(void);
void Paths_alias_entity_by_scoped_name(void);

// Testsuite 'System'
void System_iter(void);
void System_iter_const(void);
void System_iter_shared(void);
void System_iter_optional(void);
void System_each(void);
void System_each_const(void);
void System_each_shared(void);
void System_each_optional(void);
void System_signature(void);
void System_signature_const(void);
void System_signature_shared(void);
void System_signature_optional(void);
void System_copy_name_on_create(void);
void System_nested_system(void);
void System_empty_signature(void);
void System_iter_tag(void);
void System_each_tag(void);
void System_set_interval(void);
void System_order_by_type(void);
void System_order_by_id(void);
void System_order_by_type_after_create(void);
void System_order_by_id_after_create(void);
void System_get_query(void);
void System_add_from_each(void);
void System_delete_from_each(void);
void System_add_from_each_world_handle(void);
void System_new_from_each(void);
void System_add_from_iter(void);
void System_delete_from_iter(void);
void System_add_from_iter_world_handle(void);
void System_new_from_iter(void);
void System_each_w_mut_children_it(void);
void System_readonly_children_iter(void);
void System_rate_filter(void);
void System_update_rate_filter(void);
void System_default_ctor(void);
void System_test_auto_defer_each(void);
void System_test_auto_defer_iter(void);
void System_custom_pipeline(void);
void System_custom_pipeline_w_kind(void);
void System_instanced_query_w_singleton_each(void);
void System_instanced_query_w_base_each(void);
void System_un_instanced_query_w_singleton_each(void);
void System_un_instanced_query_w_base_each(void);
void System_instanced_query_w_singleton_iter(void);
void System_instanced_query_w_base_iter(void);
void System_un_instanced_query_w_singleton_iter(void);
void System_un_instanced_query_w_base_iter(void);
void System_create_w_no_template_args(void);
void System_system_w_type_kind_type_pipeline(void);
void System_default_ctor(void);
void System_entity_ctor(void);
void System_ensure_instanced_w_each(void);
void System_multithread_system_w_query_each(void);
void System_multithread_system_w_query_each_w_iter(void);
void System_multithread_system_w_query_each_w_world(void);
void System_multithread_system_w_query_iter(void);
void System_multithread_system_w_query_iter_w_iter(void);
void System_multithread_system_w_query_iter_w_world(void);
void System_run_callback(void);
void System_startup_system(void);
void System_interval_tick_source(void);
void System_rate_tick_source(void);
void System_nested_rate_tick_source(void);

// Testsuite 'Event'
void Event_evt_1_id_entity(void);
void Event_evt_2_ids_entity(void);
void Event_evt_1_id_table(void);
void Event_evt_2_ids_table(void);
void Event_evt_type(void);
void Event_evt_1_component(void);
void Event_evt_2_components(void);
void Event_evt_void_ctx(void);
void Event_evt_typed_ctx(void);
void Event_evt_implicit_typed_ctx(void);
void Event_evt_1_id_pair_rel_id_obj_id_entity(void);
void Event_evt_1_id_pair_rel_obj_id_entity(void);
void Event_evt_1_id_pair_rel_obj_entity(void);
void Event_emit_staged_from_world(void);
void Event_emit_staged_from_stage(void);

// Testsuite 'Iterable'
void Iterable_page_each(void);
void Iterable_page_iter(void);
void Iterable_worker_each(void);
void Iterable_worker_iter(void);

// Testsuite 'Query'
void Query_action(void);
void Query_action_const(void);
void Query_action_shared(void);
void Query_action_optional(void);
void Query_each(void);
void Query_each_const(void);
void Query_each_shared(void);
void Query_each_optional(void);
void Query_signature(void);
void Query_signature_const(void);
void Query_signature_shared(void);
void Query_signature_optional(void);
void Query_subquery(void);
void Query_subquery_w_expr(void);
void Query_query_single_pair(void);
void Query_tag_w_each(void);
void Query_shared_tag_w_each(void);
void Query_sort_by(void);
void Query_changed(void);
void Query_orphaned(void);
void Query_default_ctor(void);
void Query_default_ctor_no_assign(void);
void Query_expr_w_template(void);
void Query_query_type_w_template(void);
void Query_compare_term_id(void);
void Query_test_no_defer_each(void);
void Query_test_no_defer_iter(void);
void Query_inspect_terms(void);
void Query_inspect_terms_w_each(void);
void Query_comp_to_str(void);
void Query_pair_to_str(void);
void Query_oper_not_to_str(void);
void Query_oper_optional_to_str(void);
void Query_oper_or_to_str(void);
void Query_each_pair_type(void);
void Query_iter_pair_type(void);
void Query_term_pair_type(void);
void Query_each_no_entity_1_comp(void);
void Query_each_no_entity_2_comps(void);
void Query_iter_no_comps_1_comp(void);
void Query_iter_no_comps_2_comps(void);
void Query_iter_no_comps_no_comps(void);
void Query_each_pair_object(void);
void Query_iter_pair_object(void);
void Query_iter_query_in_system(void);
void Query_iter_type(void);
void Query_instanced_query_w_singleton_each(void);
void Query_instanced_query_w_base_each(void);
void Query_un_instanced_query_w_singleton_each(void);
void Query_un_instanced_query_w_base_each(void);
void Query_instanced_query_w_singleton_iter(void);
void Query_instanced_query_w_base_iter(void);
void Query_un_instanced_query_w_singleton_iter(void);
void Query_un_instanced_query_w_base_iter(void);
void Query_query_each_from_component(void);
void Query_query_iter_from_component(void);
void Query_query_each_w_func_ptr(void);
void Query_query_iter_w_func_ptr(void);
void Query_query_each_w_func_no_ptr(void);
void Query_query_iter_w_func_no_ptr(void);
void Query_query_each_w_iter(void);
void Query_change_tracking(void);
void Query_not_w_write(void);
void Query_get_first(void);
void Query_get_count_direct(void);
void Query_get_is_true_direct(void);
void Query_get_first_direct(void);
void Query_each_w_no_this(void);
void Query_each_w_iter_no_this(void);
void Query_invalid_each_w_no_this(void);
void Query_named_query(void);
void Query_named_scoped_query(void);
void Query_instanced_nested_query_w_iter(void);
void Query_instanced_nested_query_w_entity(void);
void Query_instanced_nested_query_w_world(void);

// Testsuite 'QueryBuilder'
void QueryBuilder_builder_assign_same_type(void);
void QueryBuilder_builder_assign_from_empty(void);
void QueryBuilder_builder_assign_to_empty(void);
void QueryBuilder_builder_build(void);
void QueryBuilder_builder_build_to_auto(void);
void QueryBuilder_builder_build_n_statements(void);
void QueryBuilder_1_type(void);
void QueryBuilder_add_1_type(void);
void QueryBuilder_add_2_types(void);
void QueryBuilder_add_1_type_w_1_type(void);
void QueryBuilder_add_2_types_w_1_type(void);
void QueryBuilder_add_pair(void);
void QueryBuilder_add_not(void);
void QueryBuilder_add_or(void);
void QueryBuilder_add_optional(void);
void QueryBuilder_ptr_type(void);
void QueryBuilder_const_type(void);
void QueryBuilder_string_term(void);
void QueryBuilder_singleton_term(void);
void QueryBuilder_isa_superset_term(void);
void QueryBuilder_isa_self_superset_term(void);
void QueryBuilder_childof_superset_term(void);
void QueryBuilder_childof_self_superset_term(void);
void QueryBuilder_isa_superset_term_w_each(void);
void QueryBuilder_isa_self_superset_term_w_each(void);
void QueryBuilder_childof_superset_term_w_each(void);
void QueryBuilder_childof_self_superset_term_w_each(void);
void QueryBuilder_isa_superset_shortcut(void);
void QueryBuilder_isa_superset_shortcut_w_self(void);
void QueryBuilder_childof_superset_shortcut(void);
void QueryBuilder_childof_superset_shortcut_w_self(void);
void QueryBuilder_relation(void);
void QueryBuilder_relation_w_object_wildcard(void);
void QueryBuilder_relation_w_predicate_wildcard(void);
void QueryBuilder_add_pair_w_rel_type(void);
void QueryBuilder_template_term(void);
void QueryBuilder_explicit_subject_w_id(void);
void QueryBuilder_explicit_subject_w_type(void);
void QueryBuilder_explicit_object_w_id(void);
void QueryBuilder_explicit_object_w_type(void);
void QueryBuilder_explicit_term(void);
void QueryBuilder_explicit_term_w_type(void);
void QueryBuilder_explicit_term_w_pair_type(void);
void QueryBuilder_explicit_term_w_id(void);
void QueryBuilder_explicit_term_w_pair_id(void);
void QueryBuilder_1_term_to_empty(void);
void QueryBuilder_2_subsequent_args(void);
void QueryBuilder_optional_tag_is_set(void);
void QueryBuilder_10_terms(void);
void QueryBuilder_20_terms(void);
void QueryBuilder_group_by_raw(void);
void QueryBuilder_group_by_template(void);
void QueryBuilder_group_by_iter_one(void);
void QueryBuilder_group_by_iter_one_template(void);
void QueryBuilder_group_by_iter_one_all_groups(void);
void QueryBuilder_group_by_default_func_w_id(void);
void QueryBuilder_group_by_default_func_w_type(void);
void QueryBuilder_group_by_callbacks(void);
void QueryBuilder_create_w_no_template_args(void);
void QueryBuilder_any_wildcard(void);
void QueryBuilder_cascade(void);
void QueryBuilder_cascade_w_relationship(void);
void QueryBuilder_up_w_type(void);
void QueryBuilder_cascade_w_type(void);
void QueryBuilder_named_query(void);
void QueryBuilder_term_w_write(void);
void QueryBuilder_term_w_read(void);

// Testsuite 'FilterBuilder'
void FilterBuilder_builder_assign_same_type(void);
void FilterBuilder_builder_assign_from_empty(void);
void FilterBuilder_builder_assign_to_empty(void);
void FilterBuilder_builder_build(void);
void FilterBuilder_builder_build_to_auto(void);
void FilterBuilder_builder_build_n_statements(void);
void FilterBuilder_builder_force_assign_operator(void);
void FilterBuilder_1_type(void);
void FilterBuilder_add_1_type(void);
void FilterBuilder_add_2_types(void);
void FilterBuilder_add_1_type_w_1_type(void);
void FilterBuilder_add_2_types_w_1_type(void);
void FilterBuilder_add_pair(void);
void FilterBuilder_add_not(void);
void FilterBuilder_add_or(void);
void FilterBuilder_add_optional(void);
void FilterBuilder_ptr_type(void);
void FilterBuilder_const_type(void);
void FilterBuilder_string_term(void);
void FilterBuilder_singleton_term(void);
void FilterBuilder_isa_superset_term(void);
void FilterBuilder_isa_self_superset_term(void);
void FilterBuilder_childof_superset_term(void);
void FilterBuilder_childof_self_superset_term(void);
void FilterBuilder_isa_superset_term_w_each(void);
void FilterBuilder_isa_self_superset_term_w_each(void);
void FilterBuilder_childof_superset_term_w_each(void);
void FilterBuilder_childof_self_superset_term_w_each(void);
void FilterBuilder_isa_superset_shortcut(void);
void FilterBuilder_isa_superset_shortcut_w_self(void);
void FilterBuilder_childof_superset_shortcut(void);
void FilterBuilder_childof_superset_shortcut_w_self(void);
void FilterBuilder_relation(void);
void FilterBuilder_relation_w_object_wildcard(void);
void FilterBuilder_relation_w_predicate_wildcard(void);
void FilterBuilder_add_pair_w_rel_type(void);
void FilterBuilder_template_term(void);
void FilterBuilder_explicit_subject_w_id(void);
void FilterBuilder_explicit_subject_w_type(void);
void FilterBuilder_explicit_object_w_id(void);
void FilterBuilder_explicit_object_w_type(void);
void FilterBuilder_explicit_term(void);
void FilterBuilder_explicit_term_w_type(void);
void FilterBuilder_explicit_term_w_pair_type(void);
void FilterBuilder_explicit_term_w_id(void);
void FilterBuilder_explicit_term_w_pair_id(void);
void FilterBuilder_1_term_to_empty(void);
void FilterBuilder_2_subsequent_args(void);
void FilterBuilder_filter_as_arg(void);
void FilterBuilder_filter_as_move_arg(void);
void FilterBuilder_filter_as_return(void);
void FilterBuilder_filter_copy(void);
void FilterBuilder_world_each_filter_1_component(void);
void FilterBuilder_world_each_filter_2_components(void);
void FilterBuilder_world_each_filter_1_component_no_entity(void);
void FilterBuilder_world_each_filter_2_components_no_entity(void);
void FilterBuilder_10_terms(void);
void FilterBuilder_20_terms(void);
void FilterBuilder_term_after_arg(void);
void FilterBuilder_name_arg(void);
void FilterBuilder_const_in_term(void);
void FilterBuilder_const_optional(void);
void FilterBuilder_create_w_no_template_args(void);
void FilterBuilder_2_terms_w_expr(void);
void FilterBuilder_assert_on_uninitialized_term(void);
void FilterBuilder_operator_shortcuts(void);
void FilterBuilder_inout_shortcuts(void);
void FilterBuilder_iter_column_w_const_as_array(void);
void FilterBuilder_iter_column_w_const_as_ptr(void);
void FilterBuilder_iter_column_w_const_deref(void);
void FilterBuilder_with_id(void);
void FilterBuilder_with_name(void);
void FilterBuilder_with_component(void);
void FilterBuilder_with_pair_id(void);
void FilterBuilder_with_pair_name(void);
void FilterBuilder_with_pair_components(void);
void FilterBuilder_with_pair_component_id(void);
void FilterBuilder_with_pair_component_name(void);
void FilterBuilder_with_enum(void);
void FilterBuilder_without_id(void);
void FilterBuilder_without_name(void);
void FilterBuilder_without_component(void);
void FilterBuilder_without_pair_id(void);
void FilterBuilder_without_pair_name(void);
void FilterBuilder_without_pair_components(void);
void FilterBuilder_without_pair_component_id(void);
void FilterBuilder_without_pair_component_name(void);
void FilterBuilder_without_enum(void);
void FilterBuilder_write_id(void);
void FilterBuilder_write_name(void);
void FilterBuilder_write_component(void);
void FilterBuilder_write_pair_id(void);
void FilterBuilder_write_pair_name(void);
void FilterBuilder_write_pair_components(void);
void FilterBuilder_write_pair_component_id(void);
void FilterBuilder_write_pair_component_name(void);
void FilterBuilder_write_enum(void);
void FilterBuilder_read_id(void);
void FilterBuilder_read_name(void);
void FilterBuilder_read_component(void);
void FilterBuilder_read_pair_id(void);
void FilterBuilder_read_pair_name(void);
void FilterBuilder_read_pair_components(void);
void FilterBuilder_read_pair_component_id(void);
void FilterBuilder_read_pair_component_name(void);
void FilterBuilder_read_enum(void);
void FilterBuilder_assign_after_init(void);

// Testsuite 'RuleBuilder'
void RuleBuilder_1_type(void);
void RuleBuilder_2_types(void);
void RuleBuilder_id_term(void);
void RuleBuilder_type_term(void);
void RuleBuilder_id_pair_term(void);
void RuleBuilder_id_pair_wildcard_term(void);
void RuleBuilder_type_pair_term(void);
void RuleBuilder_pair_term_w_var(void);
void RuleBuilder_2_pair_terms_w_var(void);
void RuleBuilder_set_var(void);
void RuleBuilder_set_2_vars(void);
void RuleBuilder_set_var_by_name(void);
void RuleBuilder_set_2_vars_by_name(void);
void RuleBuilder_expr_w_var(void);
void RuleBuilder_get_first(void);
void RuleBuilder_get_count_direct(void);
void RuleBuilder_get_is_true_direct(void);
void RuleBuilder_get_first_direct(void);
void RuleBuilder_var_src_w_prefixed_name(void);
void RuleBuilder_var_first_w_prefixed_name(void);
void RuleBuilder_var_second_w_prefixed_name(void);
void RuleBuilder_term_w_second_var_string(void);
void RuleBuilder_term_type_w_second_var_string(void);
void RuleBuilder_named_rule(void);
void RuleBuilder_named_scoped_rule(void);

// Testsuite 'SystemBuilder'
void SystemBuilder_builder_assign_same_type(void);
void SystemBuilder_builder_build_to_auto(void);
void SystemBuilder_builder_build_n_statements(void);
void SystemBuilder_1_type(void);
void SystemBuilder_add_1_type(void);
void SystemBuilder_add_2_types(void);
void SystemBuilder_add_1_type_w_1_type(void);
void SystemBuilder_add_2_types_w_1_type(void);
void SystemBuilder_add_pair(void);
void SystemBuilder_add_not(void);
void SystemBuilder_add_or(void);
void SystemBuilder_add_optional(void);
void SystemBuilder_ptr_type(void);
void SystemBuilder_const_type(void);
void SystemBuilder_string_term(void);
void SystemBuilder_singleton_term(void);
void SystemBuilder_10_terms(void);
void SystemBuilder_20_terms(void);
void SystemBuilder_name_arg(void);
void SystemBuilder_create_w_no_template_args(void);
void SystemBuilder_write_annotation(void);

// Testsuite 'Observer'
void Observer_2_terms_on_add(void);
void Observer_2_terms_on_remove(void);
void Observer_2_terms_on_set(void);
void Observer_2_terms_un_set(void);
void Observer_10_terms(void);
void Observer_20_terms(void);
void Observer_2_entities_iter(void);
void Observer_2_entities_table_column(void);
void Observer_2_entities_each(void);
void Observer_create_w_no_template_args(void);
void Observer_yield_existing(void);
void Observer_yield_existing_2_terms(void);
void Observer_default_ctor(void);
void Observer_entity_ctor(void);
void Observer_on_add(void);
void Observer_on_remove(void);
void Observer_on_add_tag_action(void);
void Observer_on_add_tag_iter(void);
void Observer_on_add_tag_each(void);
void Observer_on_add_expr(void);
void Observer_observer_w_filter_term(void);
void Observer_run_callback(void);
void Observer_get_query(void);

// Testsuite 'Filter'
void Filter_term_each_component(void);
void Filter_term_each_tag(void);
void Filter_term_each_id(void);
void Filter_term_each_pair_type(void);
void Filter_term_each_pair_id(void);
void Filter_term_each_pair_relation_wildcard(void);
void Filter_term_each_pair_object_wildcard(void);
void Filter_default_ctor_no_assign(void);
void Filter_term_get_id(void);
void Filter_term_get_subj(void);
void Filter_term_get_pred(void);
void Filter_term_get_obj(void);
void Filter_get_first(void);
void Filter_get_count_direct(void);
void Filter_get_is_true_direct(void);
void Filter_get_first_direct(void);
void Filter_each_w_no_this(void);
void Filter_each_w_iter_no_this(void);
void Filter_invalid_each_w_no_this(void);
void Filter_named_filter(void);
void Filter_named_scoped_filter(void);

// Testsuite 'ComponentLifecycle'
void ComponentLifecycle_ctor_on_add(void);
void ComponentLifecycle_dtor_on_remove(void);
void ComponentLifecycle_move_on_add(void);
void ComponentLifecycle_move_on_remove(void);
void ComponentLifecycle_copy_on_set(void);
void ComponentLifecycle_copy_on_override(void);
void ComponentLifecycle_struct_w_string_add(void);
void ComponentLifecycle_struct_w_string_remove(void);
void ComponentLifecycle_struct_w_string_set(void);
void ComponentLifecycle_struct_w_string_override(void);
void ComponentLifecycle_struct_w_string_add_2_remove(void);
void ComponentLifecycle_struct_w_string_set_2_remove(void);
void ComponentLifecycle_struct_w_string_add_2_remove_w_tag(void);
void ComponentLifecycle_struct_w_string_set_2_remove_w_tag(void);
void ComponentLifecycle_struct_w_vector_add(void);
void ComponentLifecycle_struct_w_vector_remove(void);
void ComponentLifecycle_struct_w_vector_set(void);
void ComponentLifecycle_struct_w_vector_override(void);
void ComponentLifecycle_struct_w_vector_add_2_remove(void);
void ComponentLifecycle_struct_w_vector_set_2_remove(void);
void ComponentLifecycle_struct_w_vector_add_2_remove_w_tag(void);
void ComponentLifecycle_struct_w_vector_set_2_remove_w_tag(void);
void ComponentLifecycle_get_mut_new(void);
void ComponentLifecycle_get_mut_existing(void);
void ComponentLifecycle_implicit_component(void);
void ComponentLifecycle_implicit_after_query(void);
void ComponentLifecycle_deleted_copy(void);
void ComponentLifecycle_no_default_ctor_emplace(void);
void ComponentLifecycle_default_init(void);
void ComponentLifecycle_no_default_ctor_add(void);
void ComponentLifecycle_no_default_ctor_add_relation(void);
void ComponentLifecycle_no_default_ctor_add_second(void);
void ComponentLifecycle_no_default_ctor_set(void);
void ComponentLifecycle_no_copy_ctor(void);
void ComponentLifecycle_no_move_ctor(void);
void ComponentLifecycle_no_copy_assign(void);
void ComponentLifecycle_no_move_assign(void);
void ComponentLifecycle_no_copy(void);
void ComponentLifecycle_no_move(void);
void ComponentLifecycle_no_dtor(void);
void ComponentLifecycle_default_ctor_w_value_ctor(void);
void ComponentLifecycle_no_default_ctor_move_ctor_on_set(void);
void ComponentLifecycle_emplace_w_ctor(void);
void ComponentLifecycle_emplace_no_default_ctor(void);
void ComponentLifecycle_emplace_defer_use_move_ctor(void);
void ComponentLifecycle_emplace_existing(void);
void ComponentLifecycle_emplace_singleton(void);
void ComponentLifecycle_dtor_w_non_trivial_implicit_move(void);
void ComponentLifecycle_dtor_w_non_trivial_explicit_move(void);
void ComponentLifecycle_grow_no_default_ctor(void);
void ComponentLifecycle_grow_no_default_ctor_move(void);
void ComponentLifecycle_grow_no_default_ctor_move_w_component(void);
void ComponentLifecycle_delete_no_default_ctor(void);
void ComponentLifecycle_on_add_hook(void);
void ComponentLifecycle_on_remove_hook(void);
void ComponentLifecycle_on_set_hook(void);
void ComponentLifecycle_on_add_hook_w_entity(void);
void ComponentLifecycle_on_remove_hook_w_entity(void);
void ComponentLifecycle_on_set_hook_w_entity(void);
void ComponentLifecycle_chained_hooks(void);
void ComponentLifecycle_ctor_w_2_worlds(void);
void ComponentLifecycle_ctor_w_2_worlds_explicit_registration(void);
void ComponentLifecycle_defer_emplace(void);
void ComponentLifecycle_emplace_w_on_add(void);
void ComponentLifecycle_emplace_w_on_add_existing(void);
void ComponentLifecycle_set_pair_no_copy(void);
void ComponentLifecycle_set_pair_w_entity_no_copy(void);
void ComponentLifecycle_set_pair_second_no_copy(void);
void ComponentLifecycle_set_override_no_copy(void);
void ComponentLifecycle_set_override_pair_no_copy(void);
void ComponentLifecycle_set_override_pair_w_entity_no_copy(void);

// Testsuite 'Refs'
void Refs_get_ref_by_ptr(void);
void Refs_get_ref_by_method(void);
void Refs_ref_after_add(void);
void Refs_ref_after_remove(void);
void Refs_ref_after_set(void);
void Refs_ref_before_set(void);
void Refs_non_const_ref(void);
void Refs_pair_ref(void);
void Refs_pair_ref_w_entity(void);
void Refs_pair_ref_second(void);
void Refs_from_stage(void);

// Testsuite 'Module'
void Module_import(void);
void Module_lookup_from_scope(void);
void Module_nested_module(void);
void Module_nested_type_module(void);
void Module_component_redefinition_outside_module(void);
void Module_module_tag_on_namespace(void);
void Module_dtor_on_fini(void);
void Module_implicit_module(void);
void Module_module_in_namespace_w_root_name(void);
void Module_module_as_entity(void);
void Module_module_as_component(void);

// Testsuite 'ImplicitComponents'
void ImplicitComponents_add(void);
void ImplicitComponents_remove(void);
void ImplicitComponents_has(void);
void ImplicitComponents_set(void);
void ImplicitComponents_get(void);
void ImplicitComponents_add_pair(void);
void ImplicitComponents_remove_pair(void);
void ImplicitComponents_module(void);
void ImplicitComponents_system(void);
void ImplicitComponents_system_optional(void);
void ImplicitComponents_system_const(void);
void ImplicitComponents_query(void);
void ImplicitComponents_implicit_name(void);
void ImplicitComponents_reinit(void);
void ImplicitComponents_reinit_scoped(void);
void ImplicitComponents_reinit_w_lifecycle(void);
void ImplicitComponents_first_use_in_system(void);
void ImplicitComponents_first_use_tag_in_system(void);
void ImplicitComponents_first_use_enum_in_system(void);
void ImplicitComponents_use_const(void);
void ImplicitComponents_use_const_w_stage(void);
void ImplicitComponents_use_const_w_threads(void);
void ImplicitComponents_implicit_base(void);
void ImplicitComponents_implicit_const(void);
void ImplicitComponents_implicit_ref(void);
void ImplicitComponents_implicit_ptr(void);
void ImplicitComponents_implicit_const_ref(void);

// Testsuite 'Snapshot'
void Snapshot_simple_snapshot(void);

// Testsuite 'WorldFactory'
void WorldFactory_entity(void);
void WorldFactory_entity_w_name(void);
void WorldFactory_entity_w_id(void);
void WorldFactory_prefab(void);
void WorldFactory_prefab_w_name(void);
void WorldFactory_system(void);
void WorldFactory_system_w_name(void);
void WorldFactory_system_w_expr(void);
void WorldFactory_query(void);
void WorldFactory_query_w_expr(void);
void WorldFactory_snapshot(void);
void WorldFactory_module(void);

// Testsuite 'World'
void World_builtin_components(void);
void World_multi_world_empty(void);
void World_multi_world_component(void);
void World_multi_world_component_namespace(void);
void World_multi_world_module(void);
void World_multi_world_recycled_component(void);
void World_multi_world_recycled_component_different_generation(void);
void World_type_id(void);
void World_different_comp_same_name(void);
void World_reregister_after_reset(void);
void World_reregister_after_reset_w_namespace(void);
void World_reregister_namespace(void);
void World_implicit_reregister_after_reset(void);
void World_reregister_after_reset_different_name(void);
void World_register_short_template(void);
void World_reimport(void);
void World_reimport_module_after_reset(void);
void World_reimport_module_new_world(void);
void World_reimport_namespaced_module(void);
void World_c_interop_module(void);
void World_c_interop_after_reset(void);
void World_implicit_register_w_new_world(void);
void World_implicit_register_after_reset_register_w_custom_name(void);
void World_register_after_reset_register_w_custom_name(void);
void World_register_builtin_after_reset(void);
void World_register_meta_after_reset(void);
void World_reregister_after_reset_w_hooks_and_in_use(void);
void World_reregister_after_reset_w_hooks_and_in_use_implicit(void);
void World_count(void);
void World_count_id(void);
void World_count_pair(void);
void World_count_pair_type_id(void);
void World_count_pair_id(void);
void World_staged_count(void);
void World_async_stage_add(void);
void World_with_tag(void);
void World_with_tag_type(void);
void World_with_relation(void);
void World_with_relation_type(void);
void World_with_relation_object_type(void);
void World_with_tag_nested(void);
void World_with_scope(void);
void World_with_scope_nested(void);
void World_with_scope_type(void);
void World_with_scope_type_staged(void);
void World_with_scope_no_lambda(void);
void World_with_scope_type_no_lambda(void);
void World_recursive_lookup(void);
void World_type_w_tag_name(void);
void World_entity_w_tag_name(void);
void World_template_component_name(void);
void World_template_component_w_namespace_name(void);
void World_template_component_w_same_namespace_name(void);
void World_template_component_w_namespace_name_and_namespaced_arg(void);
void World_template_component_w_same_namespace_name_and_namespaced_arg(void);
void World_template_component_from_module_2_args(void);
void World_entity_as_tag(void);
void World_entity_w_name_as_tag(void);
void World_entity_as_component(void);
void World_entity_w_name_as_component(void);
void World_entity_as_component_2_worlds(void);
void World_entity_as_namespaced_component_2_worlds(void);
void World_entity_as_component_2_worlds_implicit_namespaced(void);
void World_delete_with_id(void);
void World_delete_with_type(void);
void World_delete_with_pair(void);
void World_delete_with_pair_type(void);
void World_delete_with_implicit(void);
void World_delete_with_pair_implicit(void);
void World_remove_all_id(void);
void World_remove_all_type(void);
void World_remove_all_pair(void);
void World_remove_all_pair_type(void);
void World_remove_all_implicit(void);
void World_remove_all_pair_implicit(void);
void World_get_scope(void);
void World_get_scope_type(void);
void World_register_namespace_after_component(void);
void World_is_alive(void);
void World_is_valid(void);
void World_exists(void);
void World_get_alive(void);
void World_ensure(void);
void World_reset_all(void);
void World_get_tick(void);
void World_register_from_scope(void);
void World_register_nested_from_scope(void);
void World_register_w_root_name(void);
void World_register_nested_w_root_name(void);
void World_set_lookup_path(void);
void World_run_post_frame(void);
void World_component_w_low_id(void);

// Testsuite 'Singleton'
void Singleton_set_get_singleton(void);
void Singleton_get_mut_singleton(void);
void Singleton_emplace_singleton(void);
void Singleton_modified_singleton(void);
void Singleton_add_singleton(void);
void Singleton_remove_singleton(void);
void Singleton_has_singleton(void);
void Singleton_singleton_system(void);
void Singleton_get_singleton(void);
void Singleton_type_id_from_world(void);
void Singleton_set_lambda(void);
void Singleton_get_lambda(void);
void Singleton_get_write_lambda(void);

// Testsuite 'Misc'
void Misc_setup(void);
void Misc_string_compare_w_char_ptr(void);
void Misc_string_compare_w_char_ptr_length_diff(void);
void Misc_string_compare_w_string(void);
void Misc_string_view_compare_w_char_ptr(void);
void Misc_string_view_compare_w_string(void);
void Misc_string_compare_nullptr(void);
void Misc_nullptr_string_compare(void);
void Misc_nullptr_string_compare_nullptr(void);
void Misc_c_macros(void);
void Misc_app_run(void);
void Misc_app_run_target_fps(void);
void Misc_app_run_set_frames(void);

// Testsuite 'Meta'
void Meta_struct(void);
void Meta_nested_struct(void);
void Meta_struct_w_portable_type(void);
void Meta_partial_struct(void);
void Meta_partial_struct_custom_offset(void);
void Meta_units(void);
void Meta_unit_w_quantity(void);
void Meta_unit_w_prefix(void);
void Meta_unit_w_over(void);
void Meta_bitmask(void);

// Testsuite 'Table'
void Table_each(void);
void Table_each_locked(void);
void Table_each_without_entity(void);
void Table_each_without_entity_locked(void);
void Table_iter(void);
void Table_iter_locked(void);
void Table_iter_without_components(void);
void Table_iter_without_components_locked(void);
void Table_multi_get(void);
void Table_multi_get_locked(void);
void Table_multi_set(void);
void Table_multi_set_locked(void);
void Table_count(void);
void Table_has_id(void);
void Table_has_T(void);
void Table_has_pair_r_t(void);
void Table_has_pair_R_t(void);
void Table_has_pair_R_T(void);
void Table_get_id(void);
void Table_get_T(void);
void Table_get_pair_r_t(void);
void Table_get_pair_R_t(void);
void Table_get_pair_R_T(void);
void Table_range_get_id(void);
void Table_range_get_T(void);
void Table_range_get_pair_r_t(void);
void Table_range_get_pair_R_t(void);
void Table_range_get_pair_R_T(void);
void Table_get_depth(void);
void Table_get_depth_w_type(void);

// Testsuite 'Doc'
void Doc_set_brief(void);
void Doc_set_name(void);
void Doc_set_link(void);
void Doc_set_color(void);
void Doc_get_name_no_doc_name(void);

bake_test_case PrettyFunction_testcases[] = {
    {
        "component",
        PrettyFunction_component
    },
    {
        "enum",
        PrettyFunction_enum
    }
};

bake_test_case Entity_testcases[] = {
    {
        "new",
        Entity_new
    },
    {
        "new_named",
        Entity_new_named
    },
    {
        "new_named_from_scope",
        Entity_new_named_from_scope
    },
    {
        "new_nested_named_from_scope",
        Entity_new_nested_named_from_scope
    },
    {
        "new_nested_named_from_nested_scope",
        Entity_new_nested_named_from_nested_scope
    },
    {
        "new_add",
        Entity_new_add
    },
    {
        "new_add_2",
        Entity_new_add_2
    },
    {
        "new_set",
        Entity_new_set
    },
    {
        "new_set_2",
        Entity_new_set_2
    },
    {
        "add",
        Entity_add
    },
    {
        "add_2",
        Entity_add_2
    },
    {
        "add_entity",
        Entity_add_entity
    },
    {
        "add_childof",
        Entity_add_childof
    },
    {
        "add_instanceof",
        Entity_add_instanceof
    },
    {
        "remove",
        Entity_remove
    },
    {
        "remove_2",
        Entity_remove_2
    },
    {
        "remove_entity",
        Entity_remove_entity
    },
    {
        "remove_childof",
        Entity_remove_childof
    },
    {
        "remove_instanceof",
        Entity_remove_instanceof
    },
    {
        "set",
        Entity_set
    },
    {
        "set_2",
        Entity_set_2
    },
    {
        "emplace",
        Entity_emplace
    },
    {
        "emplace_2",
        Entity_emplace_2
    },
    {
        "emplace_after_add",
        Entity_emplace_after_add
    },
    {
        "emplace_after_add_pair",
        Entity_emplace_after_add_pair
    },
    {
        "emplace_pair",
        Entity_emplace_pair
    },
    {
        "emplace_pair_w_entity",
        Entity_emplace_pair_w_entity
    },
    {
        "emplace_pair_type",
        Entity_emplace_pair_type
    },
    {
        "emplace_pair_second",
        Entity_emplace_pair_second
    },
    {
        "get_generic",
        Entity_get_generic
    },
    {
        "get_mut_generic",
        Entity_get_mut_generic
    },
    {
        "get_generic_w_id",
        Entity_get_generic_w_id
    },
    {
        "get_generic_w_id_t",
        Entity_get_generic_w_id_t
    },
    {
        "get_mut_generic_w_id",
        Entity_get_mut_generic_w_id
    },
    {
        "get_mut_generic_w_id_t",
        Entity_get_mut_generic_w_id_t
    },
    {
        "set_generic",
        Entity_set_generic
    },
    {
        "set_generic_w_id",
        Entity_set_generic_w_id
    },
    {
        "set_generic_w_id_t",
        Entity_set_generic_w_id_t
    },
    {
        "set_generic_no_size",
        Entity_set_generic_no_size
    },
    {
        "set_generic_no_size_w_id",
        Entity_set_generic_no_size_w_id
    },
    {
        "set_generic_no_size_w_id_t",
        Entity_set_generic_no_size_w_id_t
    },
    {
        "add_role",
        Entity_add_role
    },
    {
        "remove_role",
        Entity_remove_role
    },
    {
        "has_role",
        Entity_has_role
    },
    {
        "pair_role",
        Entity_pair_role
    },
    {
        "equals",
        Entity_equals
    },
    {
        "compare_0",
        Entity_compare_0
    },
    {
        "compare_id_t",
        Entity_compare_id_t
    },
    {
        "compare_id",
        Entity_compare_id
    },
    {
        "compare_literal",
        Entity_compare_literal
    },
    {
        "greater_than",
        Entity_greater_than
    },
    {
        "less_than",
        Entity_less_than
    },
    {
        "not_0_or_1",
        Entity_not_0_or_1
    },
    {
        "not_true_or_false",
        Entity_not_true_or_false
    },
    {
        "has_childof",
        Entity_has_childof
    },
    {
        "has_instanceof",
        Entity_has_instanceof
    },
    {
        "has_instanceof_indirect",
        Entity_has_instanceof_indirect
    },
    {
        "null_string",
        Entity_null_string
    },
    {
        "set_name",
        Entity_set_name
    },
    {
        "change_name",
        Entity_change_name
    },
    {
        "delete",
        Entity_delete
    },
    {
        "clear",
        Entity_clear
    },
    {
        "foce_owned",
        Entity_foce_owned
    },
    {
        "force_owned_2",
        Entity_force_owned_2
    },
    {
        "force_owned_nested",
        Entity_force_owned_nested
    },
    {
        "tag_has_size_zero",
        Entity_tag_has_size_zero
    },
    {
        "get_null_name",
        Entity_get_null_name
    },
    {
        "get_target",
        Entity_get_target
    },
    {
        "get_parent",
        Entity_get_parent
    },
    {
        "is_component_enabled",
        Entity_is_component_enabled
    },
    {
        "is_enabled_component_enabled",
        Entity_is_enabled_component_enabled
    },
    {
        "is_disabled_component_enabled",
        Entity_is_disabled_component_enabled
    },
    {
        "is_pair_enabled",
        Entity_is_pair_enabled
    },
    {
        "is_enabled_pair_enabled",
        Entity_is_enabled_pair_enabled
    },
    {
        "is_disabled_pair_enabled",
        Entity_is_disabled_pair_enabled
    },
    {
        "is_pair_enabled_w_ids",
        Entity_is_pair_enabled_w_ids
    },
    {
        "is_enabled_pair_enabled_w_ids",
        Entity_is_enabled_pair_enabled_w_ids
    },
    {
        "is_disabled_pair_enabled_w_ids",
        Entity_is_disabled_pair_enabled_w_ids
    },
    {
        "is_pair_enabled_w_tgt_id",
        Entity_is_pair_enabled_w_tgt_id
    },
    {
        "is_enabled_pair_enabled_w_tgt_id",
        Entity_is_enabled_pair_enabled_w_tgt_id
    },
    {
        "is_disabled_pair_enabled_w_tgt_id",
        Entity_is_disabled_pair_enabled_w_tgt_id
    },
    {
        "get_type",
        Entity_get_type
    },
    {
        "get_nonempty_type",
        Entity_get_nonempty_type
    },
    {
        "set_no_copy",
        Entity_set_no_copy
    },
    {
        "set_copy",
        Entity_set_copy
    },
    {
        "set_deduced",
        Entity_set_deduced
    },
    {
        "override",
        Entity_override
    },
    {
        "override_id",
        Entity_override_id
    },
    {
        "override_pair",
        Entity_override_pair
    },
    {
        "override_pair_w_tgt_id",
        Entity_override_pair_w_tgt_id
    },
    {
        "override_pair_w_ids",
        Entity_override_pair_w_ids
    },
    {
        "set_override",
        Entity_set_override
    },
    {
        "set_override_lvalue",
        Entity_set_override_lvalue
    },
    {
        "set_override_pair",
        Entity_set_override_pair
    },
    {
        "set_override_pair_w_tgt_id",
        Entity_set_override_pair_w_tgt_id
    },
    {
        "set_override_pair_w_rel_tag",
        Entity_set_override_pair_w_rel_tag
    },
    {
        "emplace_override",
        Entity_emplace_override
    },
    {
        "emplace_override_pair",
        Entity_emplace_override_pair
    },
    {
        "implicit_name_to_char",
        Entity_implicit_name_to_char
    },
    {
        "implicit_path_to_char",
        Entity_implicit_path_to_char
    },
    {
        "implicit_type_str_to_char",
        Entity_implicit_type_str_to_char
    },
    {
        "entity_to_entity_view",
        Entity_entity_to_entity_view
    },
    {
        "entity_view_to_entity_world",
        Entity_entity_view_to_entity_world
    },
    {
        "entity_view_to_entity_stage",
        Entity_entity_view_to_entity_stage
    },
    {
        "create_entity_view_from_stage",
        Entity_create_entity_view_from_stage
    },
    {
        "set_template",
        Entity_set_template
    },
    {
        "get_1_component_w_callback",
        Entity_get_1_component_w_callback
    },
    {
        "get_2_components_w_callback",
        Entity_get_2_components_w_callback
    },
    {
        "set_1_component_w_callback",
        Entity_set_1_component_w_callback
    },
    {
        "set_2_components_w_callback",
        Entity_set_2_components_w_callback
    },
    {
        "set_3_components_w_callback",
        Entity_set_3_components_w_callback
    },
    {
        "get_mut_1_component_w_callback",
        Entity_get_mut_1_component_w_callback
    },
    {
        "get_mut_2_components_w_callback",
        Entity_get_mut_2_components_w_callback
    },
    {
        "get_component_w_callback_nested",
        Entity_get_component_w_callback_nested
    },
    {
        "get_mut_component_w_callback_nested",
        Entity_get_mut_component_w_callback_nested
    },
    {
        "defer_set_1_component",
        Entity_defer_set_1_component
    },
    {
        "defer_set_2_components",
        Entity_defer_set_2_components
    },
    {
        "defer_set_3_components",
        Entity_defer_set_3_components
    },
    {
        "set_2_w_on_set",
        Entity_set_2_w_on_set
    },
    {
        "defer_set_2_w_on_set",
        Entity_defer_set_2_w_on_set
    },
    {
        "set_2_after_fluent",
        Entity_set_2_after_fluent
    },
    {
        "set_2_before_fluent",
        Entity_set_2_before_fluent
    },
    {
        "set_2_after_set_1",
        Entity_set_2_after_set_1
    },
    {
        "set_2_after_set_2",
        Entity_set_2_after_set_2
    },
    {
        "with_self",
        Entity_with_self
    },
    {
        "with_relation_type_self",
        Entity_with_relation_type_self
    },
    {
        "with_relation_self",
        Entity_with_relation_self
    },
    {
        "with_self_w_name",
        Entity_with_self_w_name
    },
    {
        "with_self_nested",
        Entity_with_self_nested
    },
    {
        "with_after_builder_method",
        Entity_with_after_builder_method
    },
    {
        "with_before_builder_method",
        Entity_with_before_builder_method
    },
    {
        "with_scope",
        Entity_with_scope
    },
    {
        "with_scope_nested",
        Entity_with_scope_nested
    },
    {
        "with_scope_nested_same_name_as_parent",
        Entity_with_scope_nested_same_name_as_parent
    },
    {
        "scope_after_builder_method",
        Entity_scope_after_builder_method
    },
    {
        "scope_before_builder_method",
        Entity_scope_before_builder_method
    },
    {
        "no_recursive_lookup",
        Entity_no_recursive_lookup
    },
    {
        "defer_new_w_name",
        Entity_defer_new_w_name
    },
    {
        "defer_new_w_nested_name",
        Entity_defer_new_w_nested_name
    },
    {
        "defer_new_w_scope_name",
        Entity_defer_new_w_scope_name
    },
    {
        "defer_new_w_scope_nested_name",
        Entity_defer_new_w_scope_nested_name
    },
    {
        "defer_new_w_deferred_scope_nested_name",
        Entity_defer_new_w_deferred_scope_nested_name
    },
    {
        "defer_new_w_scope",
        Entity_defer_new_w_scope
    },
    {
        "defer_new_w_with",
        Entity_defer_new_w_with
    },
    {
        "defer_new_w_name_scope_with",
        Entity_defer_new_w_name_scope_with
    },
    {
        "defer_w_with_implicit_component",
        Entity_defer_w_with_implicit_component
    },
    {
        "defer_suspend_resume",
        Entity_defer_suspend_resume
    },
    {
        "entity_id_str",
        Entity_entity_id_str
    },
    {
        "pair_id_str",
        Entity_pair_id_str
    },
    {
        "role_id_str",
        Entity_role_id_str
    },
    {
        "id_str_from_entity_view",
        Entity_id_str_from_entity_view
    },
    {
        "id_str_from_entity",
        Entity_id_str_from_entity
    },
    {
        "null_entity",
        Entity_null_entity
    },
    {
        "null_entity_w_world",
        Entity_null_entity_w_world
    },
    {
        "null_entity_w_0",
        Entity_null_entity_w_0
    },
    {
        "null_entity_w_world_w_0",
        Entity_null_entity_w_world_w_0
    },
    {
        "entity_view_null_entity",
        Entity_entity_view_null_entity
    },
    {
        "entity_view_null_entity_w_world",
        Entity_entity_view_null_entity_w_world
    },
    {
        "entity_view_null_entity_w_0",
        Entity_entity_view_null_entity_w_0
    },
    {
        "entity_view_null_entity_w_world_w_0",
        Entity_entity_view_null_entity_w_world_w_0
    },
    {
        "is_wildcard",
        Entity_is_wildcard
    },
    {
        "has_id_t",
        Entity_has_id_t
    },
    {
        "has_pair_id_t",
        Entity_has_pair_id_t
    },
    {
        "has_pair_id_t_w_type",
        Entity_has_pair_id_t_w_type
    },
    {
        "has_id",
        Entity_has_id
    },
    {
        "has_pair_id",
        Entity_has_pair_id
    },
    {
        "has_pair_id_w_type",
        Entity_has_pair_id_w_type
    },
    {
        "has_wildcard_id",
        Entity_has_wildcard_id
    },
    {
        "has_wildcard_pair_id",
        Entity_has_wildcard_pair_id
    },
    {
        "owns_id_t",
        Entity_owns_id_t
    },
    {
        "owns_pair_id_t",
        Entity_owns_pair_id_t
    },
    {
        "owns_pair_id_t_w_type",
        Entity_owns_pair_id_t_w_type
    },
    {
        "owns_id",
        Entity_owns_id
    },
    {
        "owns_pair_id",
        Entity_owns_pair_id
    },
    {
        "owns_pair_id_w_type",
        Entity_owns_pair_id_w_type
    },
    {
        "owns_wildcard_id",
        Entity_owns_wildcard_id
    },
    {
        "owns_wildcard_pair",
        Entity_owns_wildcard_pair
    },
    {
        "id_from_world",
        Entity_id_from_world
    },
    {
        "id_pair_from_world",
        Entity_id_pair_from_world
    },
    {
        "id_default_from_world",
        Entity_id_default_from_world
    },
    {
        "is_a",
        Entity_is_a
    },
    {
        "is_a_w_type",
        Entity_is_a_w_type
    },
    {
        "child_of",
        Entity_child_of
    },
    {
        "child_of_w_type",
        Entity_child_of_w_type
    },
    {
        "slot_of",
        Entity_slot_of
    },
    {
        "slot_of_w_type",
        Entity_slot_of_w_type
    },
    {
        "slot",
        Entity_slot
    },
    {
        "id_get_entity",
        Entity_id_get_entity
    },
    {
        "id_get_invalid_entity",
        Entity_id_get_invalid_entity
    },
    {
        "each_in_stage",
        Entity_each_in_stage
    },
    {
        "iter_recycled_parent",
        Entity_iter_recycled_parent
    },
    {
        "get_lambda_from_stage",
        Entity_get_lambda_from_stage
    },
    {
        "default_ctor",
        Entity_default_ctor
    },
    {
        "get_obj_by_template",
        Entity_get_obj_by_template
    },
    {
        "create_named_twice_deferred",
        Entity_create_named_twice_deferred
    },
    {
        "clone",
        Entity_clone
    },
    {
        "clone_w_value",
        Entity_clone_w_value
    },
    {
        "clone_to_existing",
        Entity_clone_to_existing
    },
    {
        "clone_to_existing_overlap",
        Entity_clone_to_existing_overlap
    },
    {
        "set_doc_name",
        Entity_set_doc_name
    },
    {
        "set_doc_brief",
        Entity_set_doc_brief
    },
    {
        "set_doc_detail",
        Entity_set_doc_detail
    },
    {
        "set_doc_link",
        Entity_set_doc_link
    },
    {
        "entity_w_root_name",
        Entity_entity_w_root_name
    },
    {
        "entity_w_root_name_from_scope",
        Entity_entity_w_root_name_from_scope
    },
    {
        "entity_w_type",
        Entity_entity_w_type
    },
    {
        "entity_w_nested_type",
        Entity_entity_w_nested_type
    },
    {
        "entity_w_type_defer",
        Entity_entity_w_type_defer
    },
    {
        "prefab_hierarchy_w_types",
        Entity_prefab_hierarchy_w_types
    },
    {
        "prefab_hierarchy_w_root_types",
        Entity_prefab_hierarchy_w_root_types
    },
    {
        "prefab_hierarchy_w_child_override",
        Entity_prefab_hierarchy_w_child_override
    },
    {
        "entity_array",
        Entity_entity_array
    },
    {
        "add_if_true_T",
        Entity_add_if_true_T
    },
    {
        "add_if_false_T",
        Entity_add_if_false_T
    },
    {
        "add_if_true_id",
        Entity_add_if_true_id
    },
    {
        "add_if_false_id",
        Entity_add_if_false_id
    },
    {
        "add_if_true_R_O",
        Entity_add_if_true_R_O
    },
    {
        "add_if_false_R_O",
        Entity_add_if_false_R_O
    },
    {
        "add_if_true_R_o",
        Entity_add_if_true_R_o
    },
    {
        "add_if_false_R_o",
        Entity_add_if_false_R_o
    },
    {
        "add_if_true_r_o",
        Entity_add_if_true_r_o
    },
    {
        "add_if_exclusive_r_o",
        Entity_add_if_exclusive_r_o
    },
    {
        "add_if_exclusive_R_o",
        Entity_add_if_exclusive_R_o
    },
    {
        "add_if_exclusive_R_O",
        Entity_add_if_exclusive_R_O
    },
    {
        "add_if_pair_w_0_object",
        Entity_add_if_pair_w_0_object
    },
    {
        "children_w_custom_relation",
        Entity_children_w_custom_relation
    },
    {
        "children_w_custom_relation_type",
        Entity_children_w_custom_relation_type
    },
    {
        "get_depth",
        Entity_get_depth
    },
    {
        "get_depth_w_type",
        Entity_get_depth_w_type
    }
};

bake_test_case Pairs_testcases[] = {
    {
        "add_component_pair",
        Pairs_add_component_pair
    },
    {
        "add_tag_pair",
        Pairs_add_tag_pair
    },
    {
        "add_tag_pair_to_tag",
        Pairs_add_tag_pair_to_tag
    },
    {
        "remove_component_pair",
        Pairs_remove_component_pair
    },
    {
        "remove_tag_pair",
        Pairs_remove_tag_pair
    },
    {
        "remove_tag_pair_to_tag",
        Pairs_remove_tag_pair_to_tag
    },
    {
        "set_component_pair",
        Pairs_set_component_pair
    },
    {
        "set_tag_pair",
        Pairs_set_tag_pair
    },
    {
        "system_1_pair_instance",
        Pairs_system_1_pair_instance
    },
    {
        "system_2_pair_instances",
        Pairs_system_2_pair_instances
    },
    {
        "override_pair",
        Pairs_override_pair
    },
    {
        "override_tag_pair",
        Pairs_override_tag_pair
    },
    {
        "get_mut_pair",
        Pairs_get_mut_pair
    },
    {
        "get_mut_pair_existing",
        Pairs_get_mut_pair_existing
    },
    {
        "get_mut_pair_tag",
        Pairs_get_mut_pair_tag
    },
    {
        "get_mut_pair_tag_existing",
        Pairs_get_mut_pair_tag_existing
    },
    {
        "get_mut_R_tag_O",
        Pairs_get_mut_R_tag_O
    },
    {
        "get_relation_from_id",
        Pairs_get_relation_from_id
    },
    {
        "get_second_from_id",
        Pairs_get_second_from_id
    },
    {
        "get_recycled_relation_from_id",
        Pairs_get_recycled_relation_from_id
    },
    {
        "get_recycled_object_from_id",
        Pairs_get_recycled_object_from_id
    },
    {
        "get_rel_obj",
        Pairs_get_rel_obj
    },
    {
        "get_rel_obj_id",
        Pairs_get_rel_obj_id
    },
    {
        "get_rel_obj_id_t",
        Pairs_get_rel_obj_id_t
    },
    {
        "get_R_obj",
        Pairs_get_R_obj
    },
    {
        "get_R_obj_id",
        Pairs_get_R_obj_id
    },
    {
        "get_R_obj_id_t",
        Pairs_get_R_obj_id_t
    },
    {
        "get_R_O",
        Pairs_get_R_O
    },
    {
        "get_R_tag_O",
        Pairs_get_R_tag_O
    },
    {
        "get_second",
        Pairs_get_second
    },
    {
        "get_second_id",
        Pairs_get_second_id
    },
    {
        "get_second_id_t",
        Pairs_get_second_id_t
    },
    {
        "each",
        Pairs_each
    },
    {
        "each_pair",
        Pairs_each_pair
    },
    {
        "each_pair_by_type",
        Pairs_each_pair_by_type
    },
    {
        "each_pair_w_isa",
        Pairs_each_pair_w_isa
    },
    {
        "each_pair_w_recycled_rel",
        Pairs_each_pair_w_recycled_rel
    },
    {
        "each_pair_w_recycled_obj",
        Pairs_each_pair_w_recycled_obj
    },
    {
        "match_pair",
        Pairs_match_pair
    },
    {
        "match_pair_obj_wildcard",
        Pairs_match_pair_obj_wildcard
    },
    {
        "match_pair_rel_wildcard",
        Pairs_match_pair_rel_wildcard
    },
    {
        "match_pair_both_wildcard",
        Pairs_match_pair_both_wildcard
    },
    {
        "has_tag_w_object",
        Pairs_has_tag_w_object
    },
    {
        "has_second_tag",
        Pairs_has_second_tag
    },
    {
        "add_pair_type",
        Pairs_add_pair_type
    },
    {
        "remove_pair_type",
        Pairs_remove_pair_type
    },
    {
        "set_pair_type",
        Pairs_set_pair_type
    },
    {
        "has_pair_type",
        Pairs_has_pair_type
    },
    {
        "get_1_pair_arg",
        Pairs_get_1_pair_arg
    },
    {
        "get_2_pair_arg",
        Pairs_get_2_pair_arg
    },
    {
        "set_1_pair_arg",
        Pairs_set_1_pair_arg
    },
    {
        "set_2_pair_arg",
        Pairs_set_2_pair_arg
    },
    {
        "get_inline_pair_type",
        Pairs_get_inline_pair_type
    },
    {
        "set_inline_pair_type",
        Pairs_set_inline_pair_type
    },
    {
        "get_pair_type_object",
        Pairs_get_pair_type_object
    },
    {
        "set_pair_type_object",
        Pairs_set_pair_type_object
    },
    {
        "set_get_second_variants",
        Pairs_set_get_second_variants
    },
    {
        "get_object_for_type_self",
        Pairs_get_object_for_type_self
    },
    {
        "get_object_for_type_base",
        Pairs_get_object_for_type_base
    },
    {
        "get_object_for_id_self",
        Pairs_get_object_for_id_self
    },
    {
        "get_object_for_id_base",
        Pairs_get_object_for_id_base
    },
    {
        "get_object_for_id_not_found",
        Pairs_get_object_for_id_not_found
    },
    {
        "deref_pair",
        Pairs_deref_pair
    },
    {
        "deref_const_pair",
        Pairs_deref_const_pair
    },
    {
        "deref_pair_obj",
        Pairs_deref_pair_obj
    },
    {
        "deref_const_pair_obj",
        Pairs_deref_const_pair_obj
    },
    {
        "set_R_existing_value",
        Pairs_set_R_existing_value
    }
};

bake_test_case Enum_testcases[] = {
    {
        "standard_enum_reflection",
        Enum_standard_enum_reflection
    },
    {
        "sparse_enum_reflection",
        Enum_sparse_enum_reflection
    },
    {
        "enum_class_reflection",
        Enum_enum_class_reflection
    },
    {
        "prefixed_enum_reflection",
        Enum_prefixed_enum_reflection
    },
    {
        "constant_with_num_reflection",
        Enum_constant_with_num_reflection
    },
    {
        "get_constant_id",
        Enum_get_constant_id
    },
    {
        "add_enum_constant",
        Enum_add_enum_constant
    },
    {
        "add_enum_class_constant",
        Enum_add_enum_class_constant
    },
    {
        "replace_enum_constants",
        Enum_replace_enum_constants
    },
    {
        "has_enum",
        Enum_has_enum
    },
    {
        "has_enum_wildcard",
        Enum_has_enum_wildcard
    },
    {
        "get_enum",
        Enum_get_enum
    },
    {
        "remove_enum",
        Enum_remove_enum
    },
    {
        "remove_wildcard",
        Enum_remove_wildcard
    },
    {
        "enum_as_component",
        Enum_enum_as_component
    },
    {
        "query_enum_wildcard",
        Enum_query_enum_wildcard
    },
    {
        "query_enum_constant",
        Enum_query_enum_constant
    },
    {
        "enum_type_from_stage",
        Enum_enum_type_from_stage
    },
    {
        "add_enum_from_stage",
        Enum_add_enum_from_stage
    },
    {
        "enum_w_2_worlds",
        Enum_enum_w_2_worlds
    },
    {
        "add_enum_constant_w_tag",
        Enum_add_enum_constant_w_tag
    },
    {
        "remove_enum_constant_w_tag",
        Enum_remove_enum_constant_w_tag
    },
    {
        "set_enum_constant_w_tag",
        Enum_set_enum_constant_w_tag
    },
    {
        "enum_w_incorrect_size",
        Enum_enum_w_incorrect_size
    },
    {
        "add_union_enum",
        Enum_add_union_enum
    },
    {
        "add_2_union_enums",
        Enum_add_2_union_enums
    },
    {
        "add_2_union_enums_reverse",
        Enum_add_2_union_enums_reverse
    },
    {
        "constant_from_entity",
        Enum_constant_from_entity
    },
    {
        "add_if",
        Enum_add_if
    },
    {
        "add_if_other",
        Enum_add_if_other
    },
    {
        "query_union_enum",
        Enum_query_union_enum
    },
    {
        "query_union_enum_invalid_query_type",
        Enum_query_union_enum_invalid_query_type
    }
};

bake_test_case Switch_testcases[] = {
    {
        "add_case",
        Switch_add_case
    },
    {
        "get_case",
        Switch_get_case
    },
    {
        "system_w_case",
        Switch_system_w_case
    },
    {
        "system_w_case_builder",
        Switch_system_w_case_builder
    },
    {
        "system_w_sw_type_builder",
        Switch_system_w_sw_type_builder
    },
    {
        "system_w_switch",
        Switch_system_w_switch
    },
    {
        "add_case_w_type",
        Switch_add_case_w_type
    },
    {
        "add_switch_w_type",
        Switch_add_switch_w_type
    },
    {
        "add_remove_switch_w_type",
        Switch_add_remove_switch_w_type
    },
    {
        "switch_enum_type",
        Switch_switch_enum_type
    }
};

bake_test_case Paths_testcases[] = {
    {
        "name",
        Paths_name
    },
    {
        "path_depth_1",
        Paths_path_depth_1
    },
    {
        "path_depth_2",
        Paths_path_depth_2
    },
    {
        "entity_lookup_name",
        Paths_entity_lookup_name
    },
    {
        "entity_lookup_depth_1",
        Paths_entity_lookup_depth_1
    },
    {
        "entity_lookup_depth_2",
        Paths_entity_lookup_depth_2
    },
    {
        "entity_lookup_from_0",
        Paths_entity_lookup_from_0
    },
    {
        "entity_lookup_from_0_w_world",
        Paths_entity_lookup_from_0_w_world
    },
    {
        "alias_component",
        Paths_alias_component
    },
    {
        "alias_scoped_component",
        Paths_alias_scoped_component
    },
    {
        "alias_scoped_component_w_name",
        Paths_alias_scoped_component_w_name
    },
    {
        "alias_entity",
        Paths_alias_entity
    },
    {
        "alias_entity_by_name",
        Paths_alias_entity_by_name
    },
    {
        "alias_entity_by_scoped_name",
        Paths_alias_entity_by_scoped_name
    }
};

bake_test_case System_testcases[] = {
    {
        "iter",
        System_iter
    },
    {
        "iter_const",
        System_iter_const
    },
    {
        "iter_shared",
        System_iter_shared
    },
    {
        "iter_optional",
        System_iter_optional
    },
    {
        "each",
        System_each
    },
    {
        "each_const",
        System_each_const
    },
    {
        "each_shared",
        System_each_shared
    },
    {
        "each_optional",
        System_each_optional
    },
    {
        "signature",
        System_signature
    },
    {
        "signature_const",
        System_signature_const
    },
    {
        "signature_shared",
        System_signature_shared
    },
    {
        "signature_optional",
        System_signature_optional
    },
    {
        "copy_name_on_create",
        System_copy_name_on_create
    },
    {
        "nested_system",
        System_nested_system
    },
    {
        "empty_signature",
        System_empty_signature
    },
    {
        "iter_tag",
        System_iter_tag
    },
    {
        "each_tag",
        System_each_tag
    },
    {
        "set_interval",
        System_set_interval
    },
    {
        "order_by_type",
        System_order_by_type
    },
    {
        "order_by_id",
        System_order_by_id
    },
    {
        "order_by_type_after_create",
        System_order_by_type_after_create
    },
    {
        "order_by_id_after_create",
        System_order_by_id_after_create
    },
    {
        "get_query",
        System_get_query
    },
    {
        "add_from_each",
        System_add_from_each
    },
    {
        "delete_from_each",
        System_delete_from_each
    },
    {
        "add_from_each_world_handle",
        System_add_from_each_world_handle
    },
    {
        "new_from_each",
        System_new_from_each
    },
    {
        "add_from_iter",
        System_add_from_iter
    },
    {
        "delete_from_iter",
        System_delete_from_iter
    },
    {
        "add_from_iter_world_handle",
        System_add_from_iter_world_handle
    },
    {
        "new_from_iter",
        System_new_from_iter
    },
    {
        "each_w_mut_children_it",
        System_each_w_mut_children_it
    },
    {
        "readonly_children_iter",
        System_readonly_children_iter
    },
    {
        "rate_filter",
        System_rate_filter
    },
    {
        "update_rate_filter",
        System_update_rate_filter
    },
    {
        "default_ctor",
        System_default_ctor
    },
    {
        "test_auto_defer_each",
        System_test_auto_defer_each
    },
    {
        "test_auto_defer_iter",
        System_test_auto_defer_iter
    },
    {
        "custom_pipeline",
        System_custom_pipeline
    },
    {
        "custom_pipeline_w_kind",
        System_custom_pipeline_w_kind
    },
    {
        "instanced_query_w_singleton_each",
        System_instanced_query_w_singleton_each
    },
    {
        "instanced_query_w_base_each",
        System_instanced_query_w_base_each
    },
    {
        "un_instanced_query_w_singleton_each",
        System_un_instanced_query_w_singleton_each
    },
    {
        "un_instanced_query_w_base_each",
        System_un_instanced_query_w_base_each
    },
    {
        "instanced_query_w_singleton_iter",
        System_instanced_query_w_singleton_iter
    },
    {
        "instanced_query_w_base_iter",
        System_instanced_query_w_base_iter
    },
    {
        "un_instanced_query_w_singleton_iter",
        System_un_instanced_query_w_singleton_iter
    },
    {
        "un_instanced_query_w_base_iter",
        System_un_instanced_query_w_base_iter
    },
    {
        "create_w_no_template_args",
        System_create_w_no_template_args
    },
    {
        "system_w_type_kind_type_pipeline",
        System_system_w_type_kind_type_pipeline
    },
    {
        "default_ctor",
        System_default_ctor
    },
    {
        "entity_ctor",
        System_entity_ctor
    },
    {
        "ensure_instanced_w_each",
        System_ensure_instanced_w_each
    },
    {
        "multithread_system_w_query_each",
        System_multithread_system_w_query_each
    },
    {
        "multithread_system_w_query_each_w_iter",
        System_multithread_system_w_query_each_w_iter
    },
    {
        "multithread_system_w_query_each_w_world",
        System_multithread_system_w_query_each_w_world
    },
    {
        "multithread_system_w_query_iter",
        System_multithread_system_w_query_iter
    },
    {
        "multithread_system_w_query_iter_w_iter",
        System_multithread_system_w_query_iter_w_iter
    },
    {
        "multithread_system_w_query_iter_w_world",
        System_multithread_system_w_query_iter_w_world
    },
    {
        "run_callback",
        System_run_callback
    },
    {
        "startup_system",
        System_startup_system
    },
    {
        "interval_tick_source",
        System_interval_tick_source
    },
    {
        "rate_tick_source",
        System_rate_tick_source
    },
    {
        "nested_rate_tick_source",
        System_nested_rate_tick_source
    }
};

bake_test_case Event_testcases[] = {
    {
        "evt_1_id_entity",
        Event_evt_1_id_entity
    },
    {
        "evt_2_ids_entity",
        Event_evt_2_ids_entity
    },
    {
        "evt_1_id_table",
        Event_evt_1_id_table
    },
    {
        "evt_2_ids_table",
        Event_evt_2_ids_table
    },
    {
        "evt_type",
        Event_evt_type
    },
    {
        "evt_1_component",
        Event_evt_1_component
    },
    {
        "evt_2_components",
        Event_evt_2_components
    },
    {
        "evt_void_ctx",
        Event_evt_void_ctx
    },
    {
        "evt_typed_ctx",
        Event_evt_typed_ctx
    },
    {
        "evt_implicit_typed_ctx",
        Event_evt_implicit_typed_ctx
    },
    {
        "evt_1_id_pair_rel_id_obj_id_entity",
        Event_evt_1_id_pair_rel_id_obj_id_entity
    },
    {
        "evt_1_id_pair_rel_obj_id_entity",
        Event_evt_1_id_pair_rel_obj_id_entity
    },
    {
        "evt_1_id_pair_rel_obj_entity",
        Event_evt_1_id_pair_rel_obj_entity
    },
    {
        "emit_staged_from_world",
        Event_emit_staged_from_world
    },
    {
        "emit_staged_from_stage",
        Event_emit_staged_from_stage
    }
};

bake_test_case Iterable_testcases[] = {
    {
        "page_each",
        Iterable_page_each
    },
    {
        "page_iter",
        Iterable_page_iter
    },
    {
        "worker_each",
        Iterable_worker_each
    },
    {
        "worker_iter",
        Iterable_worker_iter
    }
};

bake_test_case Query_testcases[] = {
    {
        "action",
        Query_action
    },
    {
        "action_const",
        Query_action_const
    },
    {
        "action_shared",
        Query_action_shared
    },
    {
        "action_optional",
        Query_action_optional
    },
    {
        "each",
        Query_each
    },
    {
        "each_const",
        Query_each_const
    },
    {
        "each_shared",
        Query_each_shared
    },
    {
        "each_optional",
        Query_each_optional
    },
    {
        "signature",
        Query_signature
    },
    {
        "signature_const",
        Query_signature_const
    },
    {
        "signature_shared",
        Query_signature_shared
    },
    {
        "signature_optional",
        Query_signature_optional
    },
    {
        "subquery",
        Query_subquery
    },
    {
        "subquery_w_expr",
        Query_subquery_w_expr
    },
    {
        "query_single_pair",
        Query_query_single_pair
    },
    {
        "tag_w_each",
        Query_tag_w_each
    },
    {
        "shared_tag_w_each",
        Query_shared_tag_w_each
    },
    {
        "sort_by",
        Query_sort_by
    },
    {
        "changed",
        Query_changed
    },
    {
        "orphaned",
        Query_orphaned
    },
    {
        "default_ctor",
        Query_default_ctor
    },
    {
        "default_ctor_no_assign",
        Query_default_ctor_no_assign
    },
    {
        "expr_w_template",
        Query_expr_w_template
    },
    {
        "query_type_w_template",
        Query_query_type_w_template
    },
    {
        "compare_term_id",
        Query_compare_term_id
    },
    {
        "test_no_defer_each",
        Query_test_no_defer_each
    },
    {
        "test_no_defer_iter",
        Query_test_no_defer_iter
    },
    {
        "inspect_terms",
        Query_inspect_terms
    },
    {
        "inspect_terms_w_each",
        Query_inspect_terms_w_each
    },
    {
        "comp_to_str",
        Query_comp_to_str
    },
    {
        "pair_to_str",
        Query_pair_to_str
    },
    {
        "oper_not_to_str",
        Query_oper_not_to_str
    },
    {
        "oper_optional_to_str",
        Query_oper_optional_to_str
    },
    {
        "oper_or_to_str",
        Query_oper_or_to_str
    },
    {
        "each_pair_type",
        Query_each_pair_type
    },
    {
        "iter_pair_type",
        Query_iter_pair_type
    },
    {
        "term_pair_type",
        Query_term_pair_type
    },
    {
        "each_no_entity_1_comp",
        Query_each_no_entity_1_comp
    },
    {
        "each_no_entity_2_comps",
        Query_each_no_entity_2_comps
    },
    {
        "iter_no_comps_1_comp",
        Query_iter_no_comps_1_comp
    },
    {
        "iter_no_comps_2_comps",
        Query_iter_no_comps_2_comps
    },
    {
        "iter_no_comps_no_comps",
        Query_iter_no_comps_no_comps
    },
    {
        "each_pair_object",
        Query_each_pair_object
    },
    {
        "iter_pair_object",
        Query_iter_pair_object
    },
    {
        "iter_query_in_system",
        Query_iter_query_in_system
    },
    {
        "iter_type",
        Query_iter_type
    },
    {
        "instanced_query_w_singleton_each",
        Query_instanced_query_w_singleton_each
    },
    {
        "instanced_query_w_base_each",
        Query_instanced_query_w_base_each
    },
    {
        "un_instanced_query_w_singleton_each",
        Query_un_instanced_query_w_singleton_each
    },
    {
        "un_instanced_query_w_base_each",
        Query_un_instanced_query_w_base_each
    },
    {
        "instanced_query_w_singleton_iter",
        Query_instanced_query_w_singleton_iter
    },
    {
        "instanced_query_w_base_iter",
        Query_instanced_query_w_base_iter
    },
    {
        "un_instanced_query_w_singleton_iter",
        Query_un_instanced_query_w_singleton_iter
    },
    {
        "un_instanced_query_w_base_iter",
        Query_un_instanced_query_w_base_iter
    },
    {
        "query_each_from_component",
        Query_query_each_from_component
    },
    {
        "query_iter_from_component",
        Query_query_iter_from_component
    },
    {
        "query_each_w_func_ptr",
        Query_query_each_w_func_ptr
    },
    {
        "query_iter_w_func_ptr",
        Query_query_iter_w_func_ptr
    },
    {
        "query_each_w_func_no_ptr",
        Query_query_each_w_func_no_ptr
    },
    {
        "query_iter_w_func_no_ptr",
        Query_query_iter_w_func_no_ptr
    },
    {
        "query_each_w_iter",
        Query_query_each_w_iter
    },
    {
        "change_tracking",
        Query_change_tracking
    },
    {
        "not_w_write",
        Query_not_w_write
    },
    {
        "get_first",
        Query_get_first
    },
    {
        "get_count_direct",
        Query_get_count_direct
    },
    {
        "get_is_true_direct",
        Query_get_is_true_direct
    },
    {
        "get_first_direct",
        Query_get_first_direct
    },
    {
        "each_w_no_this",
        Query_each_w_no_this
    },
    {
        "each_w_iter_no_this",
        Query_each_w_iter_no_this
    },
    {
        "invalid_each_w_no_this",
        Query_invalid_each_w_no_this
    },
    {
        "named_query",
        Query_named_query
    },
    {
        "named_scoped_query",
        Query_named_scoped_query
    },
    {
        "instanced_nested_query_w_iter",
        Query_instanced_nested_query_w_iter
    },
    {
        "instanced_nested_query_w_entity",
        Query_instanced_nested_query_w_entity
    },
    {
        "instanced_nested_query_w_world",
        Query_instanced_nested_query_w_world
    }
};

bake_test_case QueryBuilder_testcases[] = {
    {
        "builder_assign_same_type",
        QueryBuilder_builder_assign_same_type
    },
    {
        "builder_assign_from_empty",
        QueryBuilder_builder_assign_from_empty
    },
    {
        "builder_assign_to_empty",
        QueryBuilder_builder_assign_to_empty
    },
    {
        "builder_build",
        QueryBuilder_builder_build
    },
    {
        "builder_build_to_auto",
        QueryBuilder_builder_build_to_auto
    },
    {
        "builder_build_n_statements",
        QueryBuilder_builder_build_n_statements
    },
    {
        "1_type",
        QueryBuilder_1_type
    },
    {
        "add_1_type",
        QueryBuilder_add_1_type
    },
    {
        "add_2_types",
        QueryBuilder_add_2_types
    },
    {
        "add_1_type_w_1_type",
        QueryBuilder_add_1_type_w_1_type
    },
    {
        "add_2_types_w_1_type",
        QueryBuilder_add_2_types_w_1_type
    },
    {
        "add_pair",
        QueryBuilder_add_pair
    },
    {
        "add_not",
        QueryBuilder_add_not
    },
    {
        "add_or",
        QueryBuilder_add_or
    },
    {
        "add_optional",
        QueryBuilder_add_optional
    },
    {
        "ptr_type",
        QueryBuilder_ptr_type
    },
    {
        "const_type",
        QueryBuilder_const_type
    },
    {
        "string_term",
        QueryBuilder_string_term
    },
    {
        "singleton_term",
        QueryBuilder_singleton_term
    },
    {
        "isa_superset_term",
        QueryBuilder_isa_superset_term
    },
    {
        "isa_self_superset_term",
        QueryBuilder_isa_self_superset_term
    },
    {
        "childof_superset_term",
        QueryBuilder_childof_superset_term
    },
    {
        "childof_self_superset_term",
        QueryBuilder_childof_self_superset_term
    },
    {
        "isa_superset_term_w_each",
        QueryBuilder_isa_superset_term_w_each
    },
    {
        "isa_self_superset_term_w_each",
        QueryBuilder_isa_self_superset_term_w_each
    },
    {
        "childof_superset_term_w_each",
        QueryBuilder_childof_superset_term_w_each
    },
    {
        "childof_self_superset_term_w_each",
        QueryBuilder_childof_self_superset_term_w_each
    },
    {
        "isa_superset_shortcut",
        QueryBuilder_isa_superset_shortcut
    },
    {
        "isa_superset_shortcut_w_self",
        QueryBuilder_isa_superset_shortcut_w_self
    },
    {
        "childof_superset_shortcut",
        QueryBuilder_childof_superset_shortcut
    },
    {
        "childof_superset_shortcut_w_self",
        QueryBuilder_childof_superset_shortcut_w_self
    },
    {
        "relation",
        QueryBuilder_relation
    },
    {
        "relation_w_object_wildcard",
        QueryBuilder_relation_w_object_wildcard
    },
    {
        "relation_w_predicate_wildcard",
        QueryBuilder_relation_w_predicate_wildcard
    },
    {
        "add_pair_w_rel_type",
        QueryBuilder_add_pair_w_rel_type
    },
    {
        "template_term",
        QueryBuilder_template_term
    },
    {
        "explicit_subject_w_id",
        QueryBuilder_explicit_subject_w_id
    },
    {
        "explicit_subject_w_type",
        QueryBuilder_explicit_subject_w_type
    },
    {
        "explicit_object_w_id",
        QueryBuilder_explicit_object_w_id
    },
    {
        "explicit_object_w_type",
        QueryBuilder_explicit_object_w_type
    },
    {
        "explicit_term",
        QueryBuilder_explicit_term
    },
    {
        "explicit_term_w_type",
        QueryBuilder_explicit_term_w_type
    },
    {
        "explicit_term_w_pair_type",
        QueryBuilder_explicit_term_w_pair_type
    },
    {
        "explicit_term_w_id",
        QueryBuilder_explicit_term_w_id
    },
    {
        "explicit_term_w_pair_id",
        QueryBuilder_explicit_term_w_pair_id
    },
    {
        "1_term_to_empty",
        QueryBuilder_1_term_to_empty
    },
    {
        "2_subsequent_args",
        QueryBuilder_2_subsequent_args
    },
    {
        "optional_tag_is_set",
        QueryBuilder_optional_tag_is_set
    },
    {
        "10_terms",
        QueryBuilder_10_terms
    },
    {
        "20_terms",
        QueryBuilder_20_terms
    },
    {
        "group_by_raw",
        QueryBuilder_group_by_raw
    },
    {
        "group_by_template",
        QueryBuilder_group_by_template
    },
    {
        "group_by_iter_one",
        QueryBuilder_group_by_iter_one
    },
    {
        "group_by_iter_one_template",
        QueryBuilder_group_by_iter_one_template
    },
    {
        "group_by_iter_one_all_groups",
        QueryBuilder_group_by_iter_one_all_groups
    },
    {
        "group_by_default_func_w_id",
        QueryBuilder_group_by_default_func_w_id
    },
    {
        "group_by_default_func_w_type",
        QueryBuilder_group_by_default_func_w_type
    },
    {
        "group_by_callbacks",
        QueryBuilder_group_by_callbacks
    },
    {
        "create_w_no_template_args",
        QueryBuilder_create_w_no_template_args
    },
    {
        "any_wildcard",
        QueryBuilder_any_wildcard
    },
    {
        "cascade",
        QueryBuilder_cascade
    },
    {
        "cascade_w_relationship",
        QueryBuilder_cascade_w_relationship
    },
    {
        "up_w_type",
        QueryBuilder_up_w_type
    },
    {
        "cascade_w_type",
        QueryBuilder_cascade_w_type
    },
    {
        "named_query",
        QueryBuilder_named_query
    },
    {
        "term_w_write",
        QueryBuilder_term_w_write
    },
    {
        "term_w_read",
        QueryBuilder_term_w_read
    }
};

bake_test_case FilterBuilder_testcases[] = {
    {
        "builder_assign_same_type",
        FilterBuilder_builder_assign_same_type
    },
    {
        "builder_assign_from_empty",
        FilterBuilder_builder_assign_from_empty
    },
    {
        "builder_assign_to_empty",
        FilterBuilder_builder_assign_to_empty
    },
    {
        "builder_build",
        FilterBuilder_builder_build
    },
    {
        "builder_build_to_auto",
        FilterBuilder_builder_build_to_auto
    },
    {
        "builder_build_n_statements",
        FilterBuilder_builder_build_n_statements
    },
    {
        "builder_force_assign_operator",
        FilterBuilder_builder_force_assign_operator
    },
    {
        "1_type",
        FilterBuilder_1_type
    },
    {
        "add_1_type",
        FilterBuilder_add_1_type
    },
    {
        "add_2_types",
        FilterBuilder_add_2_types
    },
    {
        "add_1_type_w_1_type",
        FilterBuilder_add_1_type_w_1_type
    },
    {
        "add_2_types_w_1_type",
        FilterBuilder_add_2_types_w_1_type
    },
    {
        "add_pair",
        FilterBuilder_add_pair
    },
    {
        "add_not",
        FilterBuilder_add_not
    },
    {
        "add_or",
        FilterBuilder_add_or
    },
    {
        "add_optional",
        FilterBuilder_add_optional
    },
    {
        "ptr_type",
        FilterBuilder_ptr_type
    },
    {
        "const_type",
        FilterBuilder_const_type
    },
    {
        "string_term",
        FilterBuilder_string_term
    },
    {
        "singleton_term",
        FilterBuilder_singleton_term
    },
    {
        "isa_superset_term",
        FilterBuilder_isa_superset_term
    },
    {
        "isa_self_superset_term",
        FilterBuilder_isa_self_superset_term
    },
    {
        "childof_superset_term",
        FilterBuilder_childof_superset_term
    },
    {
        "childof_self_superset_term",
        FilterBuilder_childof_self_superset_term
    },
    {
        "isa_superset_term_w_each",
        FilterBuilder_isa_superset_term_w_each
    },
    {
        "isa_self_superset_term_w_each",
        FilterBuilder_isa_self_superset_term_w_each
    },
    {
        "childof_superset_term_w_each",
        FilterBuilder_childof_superset_term_w_each
    },
    {
        "childof_self_superset_term_w_each",
        FilterBuilder_childof_self_superset_term_w_each
    },
    {
        "isa_superset_shortcut",
        FilterBuilder_isa_superset_shortcut
    },
    {
        "isa_superset_shortcut_w_self",
        FilterBuilder_isa_superset_shortcut_w_self
    },
    {
        "childof_superset_shortcut",
        FilterBuilder_childof_superset_shortcut
    },
    {
        "childof_superset_shortcut_w_self",
        FilterBuilder_childof_superset_shortcut_w_self
    },
    {
        "relation",
        FilterBuilder_relation
    },
    {
        "relation_w_object_wildcard",
        FilterBuilder_relation_w_object_wildcard
    },
    {
        "relation_w_predicate_wildcard",
        FilterBuilder_relation_w_predicate_wildcard
    },
    {
        "add_pair_w_rel_type",
        FilterBuilder_add_pair_w_rel_type
    },
    {
        "template_term",
        FilterBuilder_template_term
    },
    {
        "explicit_subject_w_id",
        FilterBuilder_explicit_subject_w_id
    },
    {
        "explicit_subject_w_type",
        FilterBuilder_explicit_subject_w_type
    },
    {
        "explicit_object_w_id",
        FilterBuilder_explicit_object_w_id
    },
    {
        "explicit_object_w_type",
        FilterBuilder_explicit_object_w_type
    },
    {
        "explicit_term",
        FilterBuilder_explicit_term
    },
    {
        "explicit_term_w_type",
        FilterBuilder_explicit_term_w_type
    },
    {
        "explicit_term_w_pair_type",
        FilterBuilder_explicit_term_w_pair_type
    },
    {
        "explicit_term_w_id",
        FilterBuilder_explicit_term_w_id
    },
    {
        "explicit_term_w_pair_id",
        FilterBuilder_explicit_term_w_pair_id
    },
    {
        "1_term_to_empty",
        FilterBuilder_1_term_to_empty
    },
    {
        "2_subsequent_args",
        FilterBuilder_2_subsequent_args
    },
    {
        "filter_as_arg",
        FilterBuilder_filter_as_arg
    },
    {
        "filter_as_move_arg",
        FilterBuilder_filter_as_move_arg
    },
    {
        "filter_as_return",
        FilterBuilder_filter_as_return
    },
    {
        "filter_copy",
        FilterBuilder_filter_copy
    },
    {
        "world_each_filter_1_component",
        FilterBuilder_world_each_filter_1_component
    },
    {
        "world_each_filter_2_components",
        FilterBuilder_world_each_filter_2_components
    },
    {
        "world_each_filter_1_component_no_entity",
        FilterBuilder_world_each_filter_1_component_no_entity
    },
    {
        "world_each_filter_2_components_no_entity",
        FilterBuilder_world_each_filter_2_components_no_entity
    },
    {
        "10_terms",
        FilterBuilder_10_terms
    },
    {
        "20_terms",
        FilterBuilder_20_terms
    },
    {
        "term_after_arg",
        FilterBuilder_term_after_arg
    },
    {
        "name_arg",
        FilterBuilder_name_arg
    },
    {
        "const_in_term",
        FilterBuilder_const_in_term
    },
    {
        "const_optional",
        FilterBuilder_const_optional
    },
    {
        "create_w_no_template_args",
        FilterBuilder_create_w_no_template_args
    },
    {
        "2_terms_w_expr",
        FilterBuilder_2_terms_w_expr
    },
    {
        "assert_on_uninitialized_term",
        FilterBuilder_assert_on_uninitialized_term
    },
    {
        "operator_shortcuts",
        FilterBuilder_operator_shortcuts
    },
    {
        "inout_shortcuts",
        FilterBuilder_inout_shortcuts
    },
    {
        "iter_column_w_const_as_array",
        FilterBuilder_iter_column_w_const_as_array
    },
    {
        "iter_column_w_const_as_ptr",
        FilterBuilder_iter_column_w_const_as_ptr
    },
    {
        "iter_column_w_const_deref",
        FilterBuilder_iter_column_w_const_deref
    },
    {
        "with_id",
        FilterBuilder_with_id
    },
    {
        "with_name",
        FilterBuilder_with_name
    },
    {
        "with_component",
        FilterBuilder_with_component
    },
    {
        "with_pair_id",
        FilterBuilder_with_pair_id
    },
    {
        "with_pair_name",
        FilterBuilder_with_pair_name
    },
    {
        "with_pair_components",
        FilterBuilder_with_pair_components
    },
    {
        "with_pair_component_id",
        FilterBuilder_with_pair_component_id
    },
    {
        "with_pair_component_name",
        FilterBuilder_with_pair_component_name
    },
    {
        "with_enum",
        FilterBuilder_with_enum
    },
    {
        "without_id",
        FilterBuilder_without_id
    },
    {
        "without_name",
        FilterBuilder_without_name
    },
    {
        "without_component",
        FilterBuilder_without_component
    },
    {
        "without_pair_id",
        FilterBuilder_without_pair_id
    },
    {
        "without_pair_name",
        FilterBuilder_without_pair_name
    },
    {
        "without_pair_components",
        FilterBuilder_without_pair_components
    },
    {
        "without_pair_component_id",
        FilterBuilder_without_pair_component_id
    },
    {
        "without_pair_component_name",
        FilterBuilder_without_pair_component_name
    },
    {
        "without_enum",
        FilterBuilder_without_enum
    },
    {
        "write_id",
        FilterBuilder_write_id
    },
    {
        "write_name",
        FilterBuilder_write_name
    },
    {
        "write_component",
        FilterBuilder_write_component
    },
    {
        "write_pair_id",
        FilterBuilder_write_pair_id
    },
    {
        "write_pair_name",
        FilterBuilder_write_pair_name
    },
    {
        "write_pair_components",
        FilterBuilder_write_pair_components
    },
    {
        "write_pair_component_id",
        FilterBuilder_write_pair_component_id
    },
    {
        "write_pair_component_name",
        FilterBuilder_write_pair_component_name
    },
    {
        "write_enum",
        FilterBuilder_write_enum
    },
    {
        "read_id",
        FilterBuilder_read_id
    },
    {
        "read_name",
        FilterBuilder_read_name
    },
    {
        "read_component",
        FilterBuilder_read_component
    },
    {
        "read_pair_id",
        FilterBuilder_read_pair_id
    },
    {
        "read_pair_name",
        FilterBuilder_read_pair_name
    },
    {
        "read_pair_components",
        FilterBuilder_read_pair_components
    },
    {
        "read_pair_component_id",
        FilterBuilder_read_pair_component_id
    },
    {
        "read_pair_component_name",
        FilterBuilder_read_pair_component_name
    },
    {
        "read_enum",
        FilterBuilder_read_enum
    },
    {
        "assign_after_init",
        FilterBuilder_assign_after_init
    }
};

bake_test_case RuleBuilder_testcases[] = {
    {
        "1_type",
        RuleBuilder_1_type
    },
    {
        "2_types",
        RuleBuilder_2_types
    },
    {
        "id_term",
        RuleBuilder_id_term
    },
    {
        "type_term",
        RuleBuilder_type_term
    },
    {
        "id_pair_term",
        RuleBuilder_id_pair_term
    },
    {
        "id_pair_wildcard_term",
        RuleBuilder_id_pair_wildcard_term
    },
    {
        "type_pair_term",
        RuleBuilder_type_pair_term
    },
    {
        "pair_term_w_var",
        RuleBuilder_pair_term_w_var
    },
    {
        "2_pair_terms_w_var",
        RuleBuilder_2_pair_terms_w_var
    },
    {
        "set_var",
        RuleBuilder_set_var
    },
    {
        "set_2_vars",
        RuleBuilder_set_2_vars
    },
    {
        "set_var_by_name",
        RuleBuilder_set_var_by_name
    },
    {
        "set_2_vars_by_name",
        RuleBuilder_set_2_vars_by_name
    },
    {
        "expr_w_var",
        RuleBuilder_expr_w_var
    },
    {
        "get_first",
        RuleBuilder_get_first
    },
    {
        "get_count_direct",
        RuleBuilder_get_count_direct
    },
    {
        "get_is_true_direct",
        RuleBuilder_get_is_true_direct
    },
    {
        "get_first_direct",
        RuleBuilder_get_first_direct
    },
    {
        "var_src_w_prefixed_name",
        RuleBuilder_var_src_w_prefixed_name
    },
    {
        "var_first_w_prefixed_name",
        RuleBuilder_var_first_w_prefixed_name
    },
    {
        "var_second_w_prefixed_name",
        RuleBuilder_var_second_w_prefixed_name
    },
    {
        "term_w_second_var_string",
        RuleBuilder_term_w_second_var_string
    },
    {
        "term_type_w_second_var_string",
        RuleBuilder_term_type_w_second_var_string
    },
    {
        "named_rule",
        RuleBuilder_named_rule
    },
    {
        "named_scoped_rule",
        RuleBuilder_named_scoped_rule
    }
};

bake_test_case SystemBuilder_testcases[] = {
    {
        "builder_assign_same_type",
        SystemBuilder_builder_assign_same_type
    },
    {
        "builder_build_to_auto",
        SystemBuilder_builder_build_to_auto
    },
    {
        "builder_build_n_statements",
        SystemBuilder_builder_build_n_statements
    },
    {
        "1_type",
        SystemBuilder_1_type
    },
    {
        "add_1_type",
        SystemBuilder_add_1_type
    },
    {
        "add_2_types",
        SystemBuilder_add_2_types
    },
    {
        "add_1_type_w_1_type",
        SystemBuilder_add_1_type_w_1_type
    },
    {
        "add_2_types_w_1_type",
        SystemBuilder_add_2_types_w_1_type
    },
    {
        "add_pair",
        SystemBuilder_add_pair
    },
    {
        "add_not",
        SystemBuilder_add_not
    },
    {
        "add_or",
        SystemBuilder_add_or
    },
    {
        "add_optional",
        SystemBuilder_add_optional
    },
    {
        "ptr_type",
        SystemBuilder_ptr_type
    },
    {
        "const_type",
        SystemBuilder_const_type
    },
    {
        "string_term",
        SystemBuilder_string_term
    },
    {
        "singleton_term",
        SystemBuilder_singleton_term
    },
    {
        "10_terms",
        SystemBuilder_10_terms
    },
    {
        "20_terms",
        SystemBuilder_20_terms
    },
    {
        "name_arg",
        SystemBuilder_name_arg
    },
    {
        "create_w_no_template_args",
        SystemBuilder_create_w_no_template_args
    },
    {
        "write_annotation",
        SystemBuilder_write_annotation
    }
};

bake_test_case Observer_testcases[] = {
    {
        "2_terms_on_add",
        Observer_2_terms_on_add
    },
    {
        "2_terms_on_remove",
        Observer_2_terms_on_remove
    },
    {
        "2_terms_on_set",
        Observer_2_terms_on_set
    },
    {
        "2_terms_un_set",
        Observer_2_terms_un_set
    },
    {
        "10_terms",
        Observer_10_terms
    },
    {
        "20_terms",
        Observer_20_terms
    },
    {
        "2_entities_iter",
        Observer_2_entities_iter
    },
    {
        "2_entities_table_column",
        Observer_2_entities_table_column
    },
    {
        "2_entities_each",
        Observer_2_entities_each
    },
    {
        "create_w_no_template_args",
        Observer_create_w_no_template_args
    },
    {
        "yield_existing",
        Observer_yield_existing
    },
    {
        "yield_existing_2_terms",
        Observer_yield_existing_2_terms
    },
    {
        "default_ctor",
        Observer_default_ctor
    },
    {
        "entity_ctor",
        Observer_entity_ctor
    },
    {
        "on_add",
        Observer_on_add
    },
    {
        "on_remove",
        Observer_on_remove
    },
    {
        "on_add_tag_action",
        Observer_on_add_tag_action
    },
    {
        "on_add_tag_iter",
        Observer_on_add_tag_iter
    },
    {
        "on_add_tag_each",
        Observer_on_add_tag_each
    },
    {
        "on_add_expr",
        Observer_on_add_expr
    },
    {
        "observer_w_filter_term",
        Observer_observer_w_filter_term
    },
    {
        "run_callback",
        Observer_run_callback
    },
    {
        "get_query",
        Observer_get_query
    }
};

bake_test_case Filter_testcases[] = {
    {
        "term_each_component",
        Filter_term_each_component
    },
    {
        "term_each_tag",
        Filter_term_each_tag
    },
    {
        "term_each_id",
        Filter_term_each_id
    },
    {
        "term_each_pair_type",
        Filter_term_each_pair_type
    },
    {
        "term_each_pair_id",
        Filter_term_each_pair_id
    },
    {
        "term_each_pair_relation_wildcard",
        Filter_term_each_pair_relation_wildcard
    },
    {
        "term_each_pair_object_wildcard",
        Filter_term_each_pair_object_wildcard
    },
    {
        "default_ctor_no_assign",
        Filter_default_ctor_no_assign
    },
    {
        "term_get_id",
        Filter_term_get_id
    },
    {
        "term_get_subj",
        Filter_term_get_subj
    },
    {
        "term_get_pred",
        Filter_term_get_pred
    },
    {
        "term_get_obj",
        Filter_term_get_obj
    },
    {
        "get_first",
        Filter_get_first
    },
    {
        "get_count_direct",
        Filter_get_count_direct
    },
    {
        "get_is_true_direct",
        Filter_get_is_true_direct
    },
    {
        "get_first_direct",
        Filter_get_first_direct
    },
    {
        "each_w_no_this",
        Filter_each_w_no_this
    },
    {
        "each_w_iter_no_this",
        Filter_each_w_iter_no_this
    },
    {
        "invalid_each_w_no_this",
        Filter_invalid_each_w_no_this
    },
    {
        "named_filter",
        Filter_named_filter
    },
    {
        "named_scoped_filter",
        Filter_named_scoped_filter
    }
};

bake_test_case ComponentLifecycle_testcases[] = {
    {
        "ctor_on_add",
        ComponentLifecycle_ctor_on_add
    },
    {
        "dtor_on_remove",
        ComponentLifecycle_dtor_on_remove
    },
    {
        "move_on_add",
        ComponentLifecycle_move_on_add
    },
    {
        "move_on_remove",
        ComponentLifecycle_move_on_remove
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
        "struct_w_string_add",
        ComponentLifecycle_struct_w_string_add
    },
    {
        "struct_w_string_remove",
        ComponentLifecycle_struct_w_string_remove
    },
    {
        "struct_w_string_set",
        ComponentLifecycle_struct_w_string_set
    },
    {
        "struct_w_string_override",
        ComponentLifecycle_struct_w_string_override
    },
    {
        "struct_w_string_add_2_remove",
        ComponentLifecycle_struct_w_string_add_2_remove
    },
    {
        "struct_w_string_set_2_remove",
        ComponentLifecycle_struct_w_string_set_2_remove
    },
    {
        "struct_w_string_add_2_remove_w_tag",
        ComponentLifecycle_struct_w_string_add_2_remove_w_tag
    },
    {
        "struct_w_string_set_2_remove_w_tag",
        ComponentLifecycle_struct_w_string_set_2_remove_w_tag
    },
    {
        "struct_w_vector_add",
        ComponentLifecycle_struct_w_vector_add
    },
    {
        "struct_w_vector_remove",
        ComponentLifecycle_struct_w_vector_remove
    },
    {
        "struct_w_vector_set",
        ComponentLifecycle_struct_w_vector_set
    },
    {
        "struct_w_vector_override",
        ComponentLifecycle_struct_w_vector_override
    },
    {
        "struct_w_vector_add_2_remove",
        ComponentLifecycle_struct_w_vector_add_2_remove
    },
    {
        "struct_w_vector_set_2_remove",
        ComponentLifecycle_struct_w_vector_set_2_remove
    },
    {
        "struct_w_vector_add_2_remove_w_tag",
        ComponentLifecycle_struct_w_vector_add_2_remove_w_tag
    },
    {
        "struct_w_vector_set_2_remove_w_tag",
        ComponentLifecycle_struct_w_vector_set_2_remove_w_tag
    },
    {
        "get_mut_new",
        ComponentLifecycle_get_mut_new
    },
    {
        "get_mut_existing",
        ComponentLifecycle_get_mut_existing
    },
    {
        "implicit_component",
        ComponentLifecycle_implicit_component
    },
    {
        "implicit_after_query",
        ComponentLifecycle_implicit_after_query
    },
    {
        "deleted_copy",
        ComponentLifecycle_deleted_copy
    },
    {
        "no_default_ctor_emplace",
        ComponentLifecycle_no_default_ctor_emplace
    },
    {
        "default_init",
        ComponentLifecycle_default_init
    },
    {
        "no_default_ctor_add",
        ComponentLifecycle_no_default_ctor_add
    },
    {
        "no_default_ctor_add_relation",
        ComponentLifecycle_no_default_ctor_add_relation
    },
    {
        "no_default_ctor_add_second",
        ComponentLifecycle_no_default_ctor_add_second
    },
    {
        "no_default_ctor_set",
        ComponentLifecycle_no_default_ctor_set
    },
    {
        "no_copy_ctor",
        ComponentLifecycle_no_copy_ctor
    },
    {
        "no_move_ctor",
        ComponentLifecycle_no_move_ctor
    },
    {
        "no_copy_assign",
        ComponentLifecycle_no_copy_assign
    },
    {
        "no_move_assign",
        ComponentLifecycle_no_move_assign
    },
    {
        "no_copy",
        ComponentLifecycle_no_copy
    },
    {
        "no_move",
        ComponentLifecycle_no_move
    },
    {
        "no_dtor",
        ComponentLifecycle_no_dtor
    },
    {
        "default_ctor_w_value_ctor",
        ComponentLifecycle_default_ctor_w_value_ctor
    },
    {
        "no_default_ctor_move_ctor_on_set",
        ComponentLifecycle_no_default_ctor_move_ctor_on_set
    },
    {
        "emplace_w_ctor",
        ComponentLifecycle_emplace_w_ctor
    },
    {
        "emplace_no_default_ctor",
        ComponentLifecycle_emplace_no_default_ctor
    },
    {
        "emplace_defer_use_move_ctor",
        ComponentLifecycle_emplace_defer_use_move_ctor
    },
    {
        "emplace_existing",
        ComponentLifecycle_emplace_existing
    },
    {
        "emplace_singleton",
        ComponentLifecycle_emplace_singleton
    },
    {
        "dtor_w_non_trivial_implicit_move",
        ComponentLifecycle_dtor_w_non_trivial_implicit_move
    },
    {
        "dtor_w_non_trivial_explicit_move",
        ComponentLifecycle_dtor_w_non_trivial_explicit_move
    },
    {
        "grow_no_default_ctor",
        ComponentLifecycle_grow_no_default_ctor
    },
    {
        "grow_no_default_ctor_move",
        ComponentLifecycle_grow_no_default_ctor_move
    },
    {
        "grow_no_default_ctor_move_w_component",
        ComponentLifecycle_grow_no_default_ctor_move_w_component
    },
    {
        "delete_no_default_ctor",
        ComponentLifecycle_delete_no_default_ctor
    },
    {
        "on_add_hook",
        ComponentLifecycle_on_add_hook
    },
    {
        "on_remove_hook",
        ComponentLifecycle_on_remove_hook
    },
    {
        "on_set_hook",
        ComponentLifecycle_on_set_hook
    },
    {
        "on_add_hook_w_entity",
        ComponentLifecycle_on_add_hook_w_entity
    },
    {
        "on_remove_hook_w_entity",
        ComponentLifecycle_on_remove_hook_w_entity
    },
    {
        "on_set_hook_w_entity",
        ComponentLifecycle_on_set_hook_w_entity
    },
    {
        "chained_hooks",
        ComponentLifecycle_chained_hooks
    },
    {
        "ctor_w_2_worlds",
        ComponentLifecycle_ctor_w_2_worlds
    },
    {
        "ctor_w_2_worlds_explicit_registration",
        ComponentLifecycle_ctor_w_2_worlds_explicit_registration
    },
    {
        "defer_emplace",
        ComponentLifecycle_defer_emplace
    },
    {
        "emplace_w_on_add",
        ComponentLifecycle_emplace_w_on_add
    },
    {
        "emplace_w_on_add_existing",
        ComponentLifecycle_emplace_w_on_add_existing
    },
    {
        "set_pair_no_copy",
        ComponentLifecycle_set_pair_no_copy
    },
    {
        "set_pair_w_entity_no_copy",
        ComponentLifecycle_set_pair_w_entity_no_copy
    },
    {
        "set_pair_second_no_copy",
        ComponentLifecycle_set_pair_second_no_copy
    },
    {
        "set_override_no_copy",
        ComponentLifecycle_set_override_no_copy
    },
    {
        "set_override_pair_no_copy",
        ComponentLifecycle_set_override_pair_no_copy
    },
    {
        "set_override_pair_w_entity_no_copy",
        ComponentLifecycle_set_override_pair_w_entity_no_copy
    }
};

bake_test_case Refs_testcases[] = {
    {
        "get_ref_by_ptr",
        Refs_get_ref_by_ptr
    },
    {
        "get_ref_by_method",
        Refs_get_ref_by_method
    },
    {
        "ref_after_add",
        Refs_ref_after_add
    },
    {
        "ref_after_remove",
        Refs_ref_after_remove
    },
    {
        "ref_after_set",
        Refs_ref_after_set
    },
    {
        "ref_before_set",
        Refs_ref_before_set
    },
    {
        "non_const_ref",
        Refs_non_const_ref
    },
    {
        "pair_ref",
        Refs_pair_ref
    },
    {
        "pair_ref_w_entity",
        Refs_pair_ref_w_entity
    },
    {
        "pair_ref_second",
        Refs_pair_ref_second
    },
    {
        "from_stage",
        Refs_from_stage
    }
};

bake_test_case Module_testcases[] = {
    {
        "import",
        Module_import
    },
    {
        "lookup_from_scope",
        Module_lookup_from_scope
    },
    {
        "nested_module",
        Module_nested_module
    },
    {
        "nested_type_module",
        Module_nested_type_module
    },
    {
        "component_redefinition_outside_module",
        Module_component_redefinition_outside_module
    },
    {
        "module_tag_on_namespace",
        Module_module_tag_on_namespace
    },
    {
        "dtor_on_fini",
        Module_dtor_on_fini
    },
    {
        "implicit_module",
        Module_implicit_module
    },
    {
        "module_in_namespace_w_root_name",
        Module_module_in_namespace_w_root_name
    },
    {
        "module_as_entity",
        Module_module_as_entity
    },
    {
        "module_as_component",
        Module_module_as_component
    }
};

bake_test_case ImplicitComponents_testcases[] = {
    {
        "add",
        ImplicitComponents_add
    },
    {
        "remove",
        ImplicitComponents_remove
    },
    {
        "has",
        ImplicitComponents_has
    },
    {
        "set",
        ImplicitComponents_set
    },
    {
        "get",
        ImplicitComponents_get
    },
    {
        "add_pair",
        ImplicitComponents_add_pair
    },
    {
        "remove_pair",
        ImplicitComponents_remove_pair
    },
    {
        "module",
        ImplicitComponents_module
    },
    {
        "system",
        ImplicitComponents_system
    },
    {
        "system_optional",
        ImplicitComponents_system_optional
    },
    {
        "system_const",
        ImplicitComponents_system_const
    },
    {
        "query",
        ImplicitComponents_query
    },
    {
        "implicit_name",
        ImplicitComponents_implicit_name
    },
    {
        "reinit",
        ImplicitComponents_reinit
    },
    {
        "reinit_scoped",
        ImplicitComponents_reinit_scoped
    },
    {
        "reinit_w_lifecycle",
        ImplicitComponents_reinit_w_lifecycle
    },
    {
        "first_use_in_system",
        ImplicitComponents_first_use_in_system
    },
    {
        "first_use_tag_in_system",
        ImplicitComponents_first_use_tag_in_system
    },
    {
        "first_use_enum_in_system",
        ImplicitComponents_first_use_enum_in_system
    },
    {
        "use_const",
        ImplicitComponents_use_const
    },
    {
        "use_const_w_stage",
        ImplicitComponents_use_const_w_stage
    },
    {
        "use_const_w_threads",
        ImplicitComponents_use_const_w_threads
    },
    {
        "implicit_base",
        ImplicitComponents_implicit_base
    },
    {
        "implicit_const",
        ImplicitComponents_implicit_const
    },
    {
        "implicit_ref",
        ImplicitComponents_implicit_ref
    },
    {
        "implicit_ptr",
        ImplicitComponents_implicit_ptr
    },
    {
        "implicit_const_ref",
        ImplicitComponents_implicit_const_ref
    }
};

bake_test_case Snapshot_testcases[] = {
    {
        "simple_snapshot",
        Snapshot_simple_snapshot
    }
};

bake_test_case WorldFactory_testcases[] = {
    {
        "entity",
        WorldFactory_entity
    },
    {
        "entity_w_name",
        WorldFactory_entity_w_name
    },
    {
        "entity_w_id",
        WorldFactory_entity_w_id
    },
    {
        "prefab",
        WorldFactory_prefab
    },
    {
        "prefab_w_name",
        WorldFactory_prefab_w_name
    },
    {
        "system",
        WorldFactory_system
    },
    {
        "system_w_name",
        WorldFactory_system_w_name
    },
    {
        "system_w_expr",
        WorldFactory_system_w_expr
    },
    {
        "query",
        WorldFactory_query
    },
    {
        "query_w_expr",
        WorldFactory_query_w_expr
    },
    {
        "snapshot",
        WorldFactory_snapshot
    },
    {
        "module",
        WorldFactory_module
    }
};

bake_test_case World_testcases[] = {
    {
        "builtin_components",
        World_builtin_components
    },
    {
        "multi_world_empty",
        World_multi_world_empty
    },
    {
        "multi_world_component",
        World_multi_world_component
    },
    {
        "multi_world_component_namespace",
        World_multi_world_component_namespace
    },
    {
        "multi_world_module",
        World_multi_world_module
    },
    {
        "multi_world_recycled_component",
        World_multi_world_recycled_component
    },
    {
        "multi_world_recycled_component_different_generation",
        World_multi_world_recycled_component_different_generation
    },
    {
        "type_id",
        World_type_id
    },
    {
        "different_comp_same_name",
        World_different_comp_same_name
    },
    {
        "reregister_after_reset",
        World_reregister_after_reset
    },
    {
        "reregister_after_reset_w_namespace",
        World_reregister_after_reset_w_namespace
    },
    {
        "reregister_namespace",
        World_reregister_namespace
    },
    {
        "implicit_reregister_after_reset",
        World_implicit_reregister_after_reset
    },
    {
        "reregister_after_reset_different_name",
        World_reregister_after_reset_different_name
    },
    {
        "register_short_template",
        World_register_short_template
    },
    {
        "reimport",
        World_reimport
    },
    {
        "reimport_module_after_reset",
        World_reimport_module_after_reset
    },
    {
        "reimport_module_new_world",
        World_reimport_module_new_world
    },
    {
        "reimport_namespaced_module",
        World_reimport_namespaced_module
    },
    {
        "c_interop_module",
        World_c_interop_module
    },
    {
        "c_interop_after_reset",
        World_c_interop_after_reset
    },
    {
        "implicit_register_w_new_world",
        World_implicit_register_w_new_world
    },
    {
        "implicit_register_after_reset_register_w_custom_name",
        World_implicit_register_after_reset_register_w_custom_name
    },
    {
        "register_after_reset_register_w_custom_name",
        World_register_after_reset_register_w_custom_name
    },
    {
        "register_builtin_after_reset",
        World_register_builtin_after_reset
    },
    {
        "register_meta_after_reset",
        World_register_meta_after_reset
    },
    {
        "reregister_after_reset_w_hooks_and_in_use",
        World_reregister_after_reset_w_hooks_and_in_use
    },
    {
        "reregister_after_reset_w_hooks_and_in_use_implicit",
        World_reregister_after_reset_w_hooks_and_in_use_implicit
    },
    {
        "count",
        World_count
    },
    {
        "count_id",
        World_count_id
    },
    {
        "count_pair",
        World_count_pair
    },
    {
        "count_pair_type_id",
        World_count_pair_type_id
    },
    {
        "count_pair_id",
        World_count_pair_id
    },
    {
        "staged_count",
        World_staged_count
    },
    {
        "async_stage_add",
        World_async_stage_add
    },
    {
        "with_tag",
        World_with_tag
    },
    {
        "with_tag_type",
        World_with_tag_type
    },
    {
        "with_relation",
        World_with_relation
    },
    {
        "with_relation_type",
        World_with_relation_type
    },
    {
        "with_relation_object_type",
        World_with_relation_object_type
    },
    {
        "with_tag_nested",
        World_with_tag_nested
    },
    {
        "with_scope",
        World_with_scope
    },
    {
        "with_scope_nested",
        World_with_scope_nested
    },
    {
        "with_scope_type",
        World_with_scope_type
    },
    {
        "with_scope_type_staged",
        World_with_scope_type_staged
    },
    {
        "with_scope_no_lambda",
        World_with_scope_no_lambda
    },
    {
        "with_scope_type_no_lambda",
        World_with_scope_type_no_lambda
    },
    {
        "recursive_lookup",
        World_recursive_lookup
    },
    {
        "type_w_tag_name",
        World_type_w_tag_name
    },
    {
        "entity_w_tag_name",
        World_entity_w_tag_name
    },
    {
        "template_component_name",
        World_template_component_name
    },
    {
        "template_component_w_namespace_name",
        World_template_component_w_namespace_name
    },
    {
        "template_component_w_same_namespace_name",
        World_template_component_w_same_namespace_name
    },
    {
        "template_component_w_namespace_name_and_namespaced_arg",
        World_template_component_w_namespace_name_and_namespaced_arg
    },
    {
        "template_component_w_same_namespace_name_and_namespaced_arg",
        World_template_component_w_same_namespace_name_and_namespaced_arg
    },
    {
        "template_component_from_module_2_args",
        World_template_component_from_module_2_args
    },
    {
        "entity_as_tag",
        World_entity_as_tag
    },
    {
        "entity_w_name_as_tag",
        World_entity_w_name_as_tag
    },
    {
        "entity_as_component",
        World_entity_as_component
    },
    {
        "entity_w_name_as_component",
        World_entity_w_name_as_component
    },
    {
        "entity_as_component_2_worlds",
        World_entity_as_component_2_worlds
    },
    {
        "entity_as_namespaced_component_2_worlds",
        World_entity_as_namespaced_component_2_worlds
    },
    {
        "entity_as_component_2_worlds_implicit_namespaced",
        World_entity_as_component_2_worlds_implicit_namespaced
    },
    {
        "delete_with_id",
        World_delete_with_id
    },
    {
        "delete_with_type",
        World_delete_with_type
    },
    {
        "delete_with_pair",
        World_delete_with_pair
    },
    {
        "delete_with_pair_type",
        World_delete_with_pair_type
    },
    {
        "delete_with_implicit",
        World_delete_with_implicit
    },
    {
        "delete_with_pair_implicit",
        World_delete_with_pair_implicit
    },
    {
        "remove_all_id",
        World_remove_all_id
    },
    {
        "remove_all_type",
        World_remove_all_type
    },
    {
        "remove_all_pair",
        World_remove_all_pair
    },
    {
        "remove_all_pair_type",
        World_remove_all_pair_type
    },
    {
        "remove_all_implicit",
        World_remove_all_implicit
    },
    {
        "remove_all_pair_implicit",
        World_remove_all_pair_implicit
    },
    {
        "get_scope",
        World_get_scope
    },
    {
        "get_scope_type",
        World_get_scope_type
    },
    {
        "register_namespace_after_component",
        World_register_namespace_after_component
    },
    {
        "is_alive",
        World_is_alive
    },
    {
        "is_valid",
        World_is_valid
    },
    {
        "exists",
        World_exists
    },
    {
        "get_alive",
        World_get_alive
    },
    {
        "ensure",
        World_ensure
    },
    {
        "reset_all",
        World_reset_all
    },
    {
        "get_tick",
        World_get_tick
    },
    {
        "register_from_scope",
        World_register_from_scope
    },
    {
        "register_nested_from_scope",
        World_register_nested_from_scope
    },
    {
        "register_w_root_name",
        World_register_w_root_name
    },
    {
        "register_nested_w_root_name",
        World_register_nested_w_root_name
    },
    {
        "set_lookup_path",
        World_set_lookup_path
    },
    {
        "run_post_frame",
        World_run_post_frame
    },
    {
        "component_w_low_id",
        World_component_w_low_id
    }
};

bake_test_case Singleton_testcases[] = {
    {
        "set_get_singleton",
        Singleton_set_get_singleton
    },
    {
        "get_mut_singleton",
        Singleton_get_mut_singleton
    },
    {
        "emplace_singleton",
        Singleton_emplace_singleton
    },
    {
        "modified_singleton",
        Singleton_modified_singleton
    },
    {
        "add_singleton",
        Singleton_add_singleton
    },
    {
        "remove_singleton",
        Singleton_remove_singleton
    },
    {
        "has_singleton",
        Singleton_has_singleton
    },
    {
        "singleton_system",
        Singleton_singleton_system
    },
    {
        "get_singleton",
        Singleton_get_singleton
    },
    {
        "type_id_from_world",
        Singleton_type_id_from_world
    },
    {
        "set_lambda",
        Singleton_set_lambda
    },
    {
        "get_lambda",
        Singleton_get_lambda
    },
    {
        "get_write_lambda",
        Singleton_get_write_lambda
    }
};

bake_test_case Misc_testcases[] = {
    {
        "string_compare_w_char_ptr",
        Misc_string_compare_w_char_ptr
    },
    {
        "string_compare_w_char_ptr_length_diff",
        Misc_string_compare_w_char_ptr_length_diff
    },
    {
        "string_compare_w_string",
        Misc_string_compare_w_string
    },
    {
        "string_view_compare_w_char_ptr",
        Misc_string_view_compare_w_char_ptr
    },
    {
        "string_view_compare_w_string",
        Misc_string_view_compare_w_string
    },
    {
        "string_compare_nullptr",
        Misc_string_compare_nullptr
    },
    {
        "nullptr_string_compare",
        Misc_nullptr_string_compare
    },
    {
        "nullptr_string_compare_nullptr",
        Misc_nullptr_string_compare_nullptr
    },
    {
        "c_macros",
        Misc_c_macros
    },
    {
        "app_run",
        Misc_app_run
    },
    {
        "app_run_target_fps",
        Misc_app_run_target_fps
    },
    {
        "app_run_set_frames",
        Misc_app_run_set_frames
    }
};

bake_test_case Meta_testcases[] = {
    {
        "struct",
        Meta_struct
    },
    {
        "nested_struct",
        Meta_nested_struct
    },
    {
        "struct_w_portable_type",
        Meta_struct_w_portable_type
    },
    {
        "partial_struct",
        Meta_partial_struct
    },
    {
        "partial_struct_custom_offset",
        Meta_partial_struct_custom_offset
    },
    {
        "units",
        Meta_units
    },
    {
        "unit_w_quantity",
        Meta_unit_w_quantity
    },
    {
        "unit_w_prefix",
        Meta_unit_w_prefix
    },
    {
        "unit_w_over",
        Meta_unit_w_over
    },
    {
        "bitmask",
        Meta_bitmask
    }
};

bake_test_case Table_testcases[] = {
    {
        "each",
        Table_each
    },
    {
        "each_locked",
        Table_each_locked
    },
    {
        "each_without_entity",
        Table_each_without_entity
    },
    {
        "each_without_entity_locked",
        Table_each_without_entity_locked
    },
    {
        "iter",
        Table_iter
    },
    {
        "iter_locked",
        Table_iter_locked
    },
    {
        "iter_without_components",
        Table_iter_without_components
    },
    {
        "iter_without_components_locked",
        Table_iter_without_components_locked
    },
    {
        "multi_get",
        Table_multi_get
    },
    {
        "multi_get_locked",
        Table_multi_get_locked
    },
    {
        "multi_set",
        Table_multi_set
    },
    {
        "multi_set_locked",
        Table_multi_set_locked
    },
    {
        "count",
        Table_count
    },
    {
        "has_id",
        Table_has_id
    },
    {
        "has_T",
        Table_has_T
    },
    {
        "has_pair_r_t",
        Table_has_pair_r_t
    },
    {
        "has_pair_R_t",
        Table_has_pair_R_t
    },
    {
        "has_pair_R_T",
        Table_has_pair_R_T
    },
    {
        "get_id",
        Table_get_id
    },
    {
        "get_T",
        Table_get_T
    },
    {
        "get_pair_r_t",
        Table_get_pair_r_t
    },
    {
        "get_pair_R_t",
        Table_get_pair_R_t
    },
    {
        "get_pair_R_T",
        Table_get_pair_R_T
    },
    {
        "range_get_id",
        Table_range_get_id
    },
    {
        "range_get_T",
        Table_range_get_T
    },
    {
        "range_get_pair_r_t",
        Table_range_get_pair_r_t
    },
    {
        "range_get_pair_R_t",
        Table_range_get_pair_R_t
    },
    {
        "range_get_pair_R_T",
        Table_range_get_pair_R_T
    },
    {
        "get_depth",
        Table_get_depth
    },
    {
        "get_depth_w_type",
        Table_get_depth_w_type
    }
};

bake_test_case Doc_testcases[] = {
    {
        "set_brief",
        Doc_set_brief
    },
    {
        "set_name",
        Doc_set_name
    },
    {
        "set_link",
        Doc_set_link
    },
    {
        "set_color",
        Doc_set_color
    },
    {
        "get_name_no_doc_name",
        Doc_get_name_no_doc_name
    }
};

static bake_test_suite suites[] = {
    {
        "PrettyFunction",
        NULL,
        NULL,
        2,
        PrettyFunction_testcases
    },
    {
        "Entity",
        NULL,
        NULL,
        229,
        Entity_testcases
    },
    {
        "Pairs",
        NULL,
        NULL,
        67,
        Pairs_testcases
    },
    {
        "Enum",
        NULL,
        NULL,
        32,
        Enum_testcases
    },
    {
        "Switch",
        NULL,
        NULL,
        10,
        Switch_testcases
    },
    {
        "Paths",
        NULL,
        NULL,
        14,
        Paths_testcases
    },
    {
        "System",
        NULL,
        NULL,
        64,
        System_testcases
    },
    {
        "Event",
        NULL,
        NULL,
        15,
        Event_testcases
    },
    {
        "Iterable",
        NULL,
        NULL,
        4,
        Iterable_testcases
    },
    {
        "Query",
        NULL,
        NULL,
        75,
        Query_testcases
    },
    {
        "QueryBuilder",
        NULL,
        NULL,
        67,
        QueryBuilder_testcases
    },
    {
        "FilterBuilder",
        NULL,
        NULL,
        107,
        FilterBuilder_testcases
    },
    {
        "RuleBuilder",
        NULL,
        NULL,
        25,
        RuleBuilder_testcases
    },
    {
        "SystemBuilder",
        NULL,
        NULL,
        21,
        SystemBuilder_testcases
    },
    {
        "Observer",
        NULL,
        NULL,
        23,
        Observer_testcases
    },
    {
        "Filter",
        NULL,
        NULL,
        21,
        Filter_testcases
    },
    {
        "ComponentLifecycle",
        NULL,
        NULL,
        71,
        ComponentLifecycle_testcases
    },
    {
        "Refs",
        NULL,
        NULL,
        11,
        Refs_testcases
    },
    {
        "Module",
        NULL,
        NULL,
        11,
        Module_testcases
    },
    {
        "ImplicitComponents",
        NULL,
        NULL,
        27,
        ImplicitComponents_testcases
    },
    {
        "Snapshot",
        NULL,
        NULL,
        1,
        Snapshot_testcases
    },
    {
        "WorldFactory",
        NULL,
        NULL,
        12,
        WorldFactory_testcases
    },
    {
        "World",
        NULL,
        NULL,
        92,
        World_testcases
    },
    {
        "Singleton",
        NULL,
        NULL,
        13,
        Singleton_testcases
    },
    {
        "Misc",
        Misc_setup,
        NULL,
        12,
        Misc_testcases
    },
    {
        "Meta",
        NULL,
        NULL,
        10,
        Meta_testcases
    },
    {
        "Table",
        NULL,
        NULL,
        30,
        Table_testcases
    },
    {
        "Doc",
        NULL,
        NULL,
        5,
        Doc_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("cpp_api", argc, argv, suites, 28);
}
