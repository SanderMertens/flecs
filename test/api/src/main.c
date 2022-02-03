
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <api.h>

// Testsuite 'Entity'
void Entity_init_id(void);
void Entity_init_id_name(void);
void Entity_init_id_path(void);
void Entity_init_id_add_1_comp(void);
void Entity_init_id_add_2_comp(void);
void Entity_init_id_w_scope(void);
void Entity_init_id_name_w_scope(void);
void Entity_init_id_path_w_scope(void);
void Entity_init_id_name_1_comp(void);
void Entity_init_id_name_2_comp(void);
void Entity_init_id_name_2_comp_w_scope(void);
void Entity_id_add_1_comp(void);
void Entity_id_add_2_comp(void);
void Entity_init_id_path_w_sep(void);
void Entity_find_id_name(void);
void Entity_find_w_existing_id_name(void);
void Entity_find_id_name_w_scope(void);
void Entity_find_id_path(void);
void Entity_find_id_path_w_scope(void);
void Entity_find_id_name_match(void);
void Entity_find_id_name_match_w_scope(void);
void Entity_find_id_path_match(void);
void Entity_find_id_path_match_w_scope(void);
void Entity_find_id_name_mismatch(void);
void Entity_find_id_name_mismatch_w_scope(void);
void Entity_find_id_path_mismatch(void);
void Entity_find_id_path_mismatch_w_scope(void);
void Entity_find_id_add_1_comp(void);
void Entity_find_id_add_2_comp(void);
void Entity_init_w_scope_name(void);
void Entity_init_w_core_name(void);
void Entity_init_w_with(void);
void Entity_init_w_with_w_name(void);
void Entity_init_w_with_w_scope(void);
void Entity_init_w_with_w_name_scope(void);
void Entity_is_valid(void);
void Entity_is_recycled_valid(void);
void Entity_is_0_valid(void);
void Entity_is_junk_valid(void);
void Entity_is_not_alive_valid(void);
void Entity_is_nonzero_gen_valid(void);
void Entity_init_w_name_deferred(void);
void Entity_init_w_name_staged(void);
void Entity_record_find_for_empty(void);
void Entity_record_find(void);
void Entity_record_find_from_stage(void);
void Entity_ensure_zero_gen(void);
void Entity_ensure_nonzero_gen(void);
void Entity_ensure_zero_gen_exists(void);
void Entity_ensure_nonzero_gen_exists(void);
void Entity_ensure_zero_gen_exists_alive(void);
void Entity_ensure_nonzero_gen_exists_alive(void);
void Entity_set_scope_w_entity_init_from_stage(void);
void Entity_entity_init_w_childof_and_scope(void);
void Entity_entity_init_w_childof_and_scope_and_scoped_name(void);
void Entity_deferred_entity_init_w_childof_and_scope(void);
void Entity_deferred_entity_init_w_childof_and_scope_and_scoped_name(void);
void Entity_deferred_entity_init_w_name_twice(void);

// Testsuite 'Event'
void Event_table_1_id_w_trigger(void);
void Event_table_2_ids_w_trigger(void);
void Event_table_1_id_w_observer(void);
void Event_table_2_ids_w_observer(void);
void Event_emit_event_for_empty_table(void);
void Event_emit_event_switch_id(void);
void Event_emit_event_case_for_switch_id(void);
void Event_emit_event_case_for_case_id(void);
void Event_emit_event_case_for_case_id_wildcard(void);
void Event_emit_event_switch_for_case_id(void);
void Event_emit_table_event(void);
void Event_emit_staged(void);

// Testsuite 'New'
void New_setup(void);
void New_empty(void);
void New_component(void);
void New_tag(void);
void New_redefine_component(void);
void New_recycle_id_empty(void);
void New_recycle_id_w_entity(void);
void New_recycle_empty_staged_delete(void);
void New_recycle_staged_delete(void);
void New_new_id(void);
void New_new_component_id(void);
void New_new_hi_component_id(void);
void New_new_component_id_skip_used(void);
void New_new_component_id_skip_to_hi_id(void);
void New_new_w_entity_0(void);
void New_create_w_explicit_id_2_worlds(void);
void New_new_w_id_0_w_with(void);
void New_new_w_id_w_with(void);
void New_new_w_type_0_w_with(void);
void New_new_w_type_w_with(void);
void New_new_w_id_w_with_w_scope(void);
void New_new_w_type_w_with_w_scope(void);
void New_new_w_id_w_with_defer(void);
void New_new_w_id_w_with_defer_w_scope(void);
void New_new_w_type_w_with_defer(void);
void New_new_w_type_w_with_defer_w_scope(void);

// Testsuite 'New_w_Count'
void New_w_Count_empty(void);
void New_w_Count_component(void);
void New_w_Count_tag(void);
void New_w_Count_bulk_init_empty(void);
void New_w_Count_bulk_init_empty_w_entities(void);
void New_w_Count_bulk_init_1_tag(void);
void New_w_Count_bulk_init_1_tag_w_entities(void);
void New_w_Count_bulk_init_2_tags(void);
void New_w_Count_bulk_init_1_component(void);
void New_w_Count_bulk_init_2_components(void);
void New_w_Count_bulk_init_1_component_w_value(void);
void New_w_Count_bulk_init_2_components_w_value(void);
void New_w_Count_bulk_init_2_components_tag_w_value(void);

// Testsuite 'Add'
void Add_zero(void);
void Add_component(void);
void Add_component_again(void);
void Add_2_components(void);
void Add_2_components_again(void);
void Add_2_components_overlap(void);
void Add_component_to_nonempty(void);
void Add_component_to_nonempty_again(void);
void Add_component_to_nonempty_overlap(void);
void Add_tag(void);
void Add_add_entity(void);
void Add_remove_entity(void);
void Add_add_0_entity(void);
void Add_remove_0_entity(void);

// Testsuite 'Switch'
void Switch_get_case_empty(void);
void Switch_get_case_no_switch(void);
void Switch_get_case_unset(void);
void Switch_get_case_set(void);
void Switch_get_case_change(void);
void Switch_remove_case(void);
void Switch_remove_last(void);
void Switch_delete_first(void);
void Switch_delete_last(void);
void Switch_delete_first_last(void);
void Switch_3_entities_same_case(void);
void Switch_2_entities_1_change_case(void);
void Switch_3_entities_change_case(void);
void Switch_query_switch(void);
void Switch_query_1_case_1_type(void);
void Switch_query_1_case_2_types(void);
void Switch_query_2_cases_1_type(void);
void Switch_query_2_cases_2_types(void);
void Switch_query_after_remove(void);
void Switch_add_switch_in_stage(void);
void Switch_add_case_in_stage(void);
void Switch_change_case_in_stage(void);
void Switch_change_one_case_in_stage(void);
void Switch_remove_switch_in_stage(void);
void Switch_switch_no_match_for_case(void);
void Switch_empty_entity_has_case(void);
void Switch_zero_entity_has_case(void);
void Switch_add_to_entity_w_switch(void);
void Switch_add_pair_to_entity_w_switch(void);
void Switch_sort(void);
void Switch_recycled_tags(void);
void Switch_query_recycled_tags(void);
void Switch_single_case(void);
void Switch_match_switch_on_base_instance(void);
void Switch_switch_w_bitset_query(void);
void Switch_switch_w_bitset_query_inv(void);
void Switch_switch_w_bitset_query_2_elems(void);
void Switch_switch_w_bitset_query_2_elems_skip(void);
void Switch_switch_w_bitset_query_elems_interleaved(void);
void Switch_switch_w_bitset_query_elems_interleaved_2_types(void);

// Testsuite 'EnabledComponents'
void EnabledComponents_is_component_enabled(void);
void EnabledComponents_is_empty_entity_disabled(void);
void EnabledComponents_is_0_entity_disabled(void);
void EnabledComponents_is_0_component_disabled(void);
void EnabledComponents_is_nonexist_component_disabled(void);
void EnabledComponents_is_enabled_component_enabled(void);
void EnabledComponents_is_disabled_component_enabled(void);
void EnabledComponents_has_enabled_component(void);
void EnabledComponents_is_enabled_after_add(void);
void EnabledComponents_is_enabled_after_remove(void);
void EnabledComponents_is_enabled_after_disable(void);
void EnabledComponents_is_disabled_after_enable(void);
void EnabledComponents_is_enabled_randomized(void);
void EnabledComponents_is_enabled_after_add_randomized(void);
void EnabledComponents_is_enabled_after_randomized_add_randomized(void);
void EnabledComponents_is_enabled_2(void);
void EnabledComponents_is_enabled_3(void);
void EnabledComponents_is_enabled_2_after_add(void);
void EnabledComponents_is_enabled_3_after_add(void);
void EnabledComponents_query_disabled(void);
void EnabledComponents_query_disabled_skip_initial(void);
void EnabledComponents_query_mod_2(void);
void EnabledComponents_query_mod_8(void);
void EnabledComponents_query_mod_64(void);
void EnabledComponents_query_mod_256(void);
void EnabledComponents_query_mod_1024(void);
void EnabledComponents_query_enable_mod_10(void);
void EnabledComponents_query_mod_2_2_bitsets(void);
void EnabledComponents_query_mod_8_2_bitsets(void);
void EnabledComponents_query_mod_64_2_bitsets(void);
void EnabledComponents_query_mod_256_2_bitsets(void);
void EnabledComponents_query_mod_1024_2_bitsets(void);
void EnabledComponents_query_randomized_2_bitsets(void);
void EnabledComponents_query_randomized_3_bitsets(void);
void EnabledComponents_query_randomized_4_bitsets(void);
void EnabledComponents_defer_enable(void);
void EnabledComponents_sort(void);

// Testsuite 'Remove'
void Remove_zero(void);
void Remove_1_of_1(void);
void Remove_1_of_1_again(void);
void Remove_2_again(void);
void Remove_2_overlap(void);
void Remove_1_from_empty(void);
void Remove_not_added(void);

// Testsuite 'Parser'
void Parser_resolve_this(void);
void Parser_resolve_wildcard(void);
void Parser_resolve_any(void);
void Parser_resolve_is_a(void);
void Parser_0(void);
void Parser_component_implicit_subject(void);
void Parser_component_explicit_subject(void);
void Parser_component_explicit_subject_this(void);
void Parser_component_explicit_subject_this_by_name(void);
void Parser_component_explicit_subject_wildcard(void);
void Parser_component_explicit_subject_any(void);
void Parser_this_as_predicate(void);
void Parser_this_as_object(void);
void Parser_pair_implicit_subject(void);
void Parser_pair_implicit_subject_wildcard_pred(void);
void Parser_pair_implicit_subject_wildcard_obj(void);
void Parser_pair_implicit_subject_any_pred(void);
void Parser_pair_implicit_subject_any_obj(void);
void Parser_pair_implicit_subject_this_pred(void);
void Parser_pair_implicit_subject_this_obj(void);
void Parser_pair_explicit_subject(void);
void Parser_pair_explicit_subject_this(void);
void Parser_pair_explicit_subject_this_by_name(void);
void Parser_pair_explicit_subject_wildcard_pred(void);
void Parser_pair_explicit_subject_wildcard_subj(void);
void Parser_pair_explicit_subject_wildcard_obj(void);
void Parser_pair_implicit_subject_0_object(void);
void Parser_pair_explicit_subject_0_object(void);
void Parser_in_component_implicit_subject(void);
void Parser_in_component_explicit_subject(void);
void Parser_in_pair_implicit_subject(void);
void Parser_in_pair_explicit_subject(void);
void Parser_inout_component_implicit_subject(void);
void Parser_inout_component_explicit_subject(void);
void Parser_inout_pair_implicit_subject(void);
void Parser_inout_pair_explicit_subject(void);
void Parser_out_component_implicit_subject(void);
void Parser_out_component_explicit_subject(void);
void Parser_out_pair_implicit_subject(void);
void Parser_out_pair_explicit_subject(void);
void Parser_inout_filter_component(void);
void Parser_component_singleton(void);
void Parser_this_singleton(void);
void Parser_component_implicit_no_subject(void);
void Parser_component_explicit_no_subject(void);
void Parser_pair_no_subject(void);
void Parser_variable_single_char(void);
void Parser_variable_multi_char(void);
void Parser_variable_multi_char_w_underscore(void);
void Parser_variable_multi_char_w_number(void);
void Parser_variable_multi_char_not_allcaps(void);
void Parser_escaped_all_caps_single_char(void);
void Parser_escaped_all_caps_multi_char(void);
void Parser_component_not(void);
void Parser_pair_implicit_subject_not(void);
void Parser_pair_explicit_subject_not(void);
void Parser_2_component_not(void);
void Parser_2_component_not_no_space(void);
void Parser_component_optional(void);
void Parser_2_component_optional(void);
void Parser_2_component_optional_no_space(void);
void Parser_from_and(void);
void Parser_from_or(void);
void Parser_from_not(void);
void Parser_pair_implicit_subject_optional(void);
void Parser_pair_explicit_subject_optional(void);
void Parser_pred_implicit_subject_w_role(void);
void Parser_pred_explicit_subject_w_role(void);
void Parser_pred_no_subject_w_role(void);
void Parser_pair_implicit_subject_w_role(void);
void Parser_pair_explicit_subject_w_role(void);
void Parser_inout_role_pred_implicit_subject(void);
void Parser_inout_role_pred_no_subject(void);
void Parser_inout_role_pred_explicit_subject(void);
void Parser_inout_role_pair_implicit_subject(void);
void Parser_inout_role_pair_explicit_subject(void);
void Parser_2_pred_implicit_subject(void);
void Parser_2_pred_no_subject(void);
void Parser_2_pred_explicit_subject(void);
void Parser_2_pair_implicit_subject(void);
void Parser_2_pair_explicit_subject(void);
void Parser_2_pred_role(void);
void Parser_2_pair_implicit_subj_role(void);
void Parser_2_pair_explicit_subj_role(void);
void Parser_2_or_pred_implicit_subj(void);
void Parser_2_or_pred_explicit_subj(void);
void Parser_2_or_pair_implicit_subj(void);
void Parser_2_or_pair_explicit_subj(void);
void Parser_2_or_pred_inout(void);
void Parser_1_digit_pred_implicit_subj(void);
void Parser_1_digit_pred_no_subj(void);
void Parser_1_digit_pred_explicit_subj(void);
void Parser_1_digit_pair_implicit_subj(void);
void Parser_1_digit_pair_explicit_subj(void);
void Parser_pred_implicit_subject_self(void);
void Parser_pred_implicit_subject_superset(void);
void Parser_pred_implicit_subject_subset(void);
void Parser_pred_implicit_subject_superset_inclusive(void);
void Parser_pred_implicit_subject_subset_inclusive(void);
void Parser_pred_implicit_subject_superset_cascade(void);
void Parser_pred_implicit_subject_subset_cascade(void);
void Parser_pred_implicit_subject_superset_inclusive_cascade(void);
void Parser_pred_implicit_subject_subset_inclusive_cascade(void);
void Parser_pred_implicit_subject_implicit_superset_cascade(void);
void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade(void);
void Parser_pred_implicit_subject_implicit_superset_cascade_w_rel(void);
void Parser_pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel(void);
void Parser_pred_implicit_subject_superset_depth_1_digit(void);
void Parser_pred_implicit_subject_subset_depth_1_digit(void);
void Parser_pred_implicit_subject_superset_depth_2_digits(void);
void Parser_pred_implicit_subject_subset_depth_2_digits(void);
void Parser_pred_implicit_superset_min_max_depth(void);
void Parser_pred_implicit_superset_childof_min_max_depth(void);
void Parser_pred_implicit_subject_superset_childof(void);
void Parser_pred_implicit_subject_cascade_superset_childof(void);
void Parser_pred_implicit_subject_superset_cascade_childof(void);
void Parser_pred_implicit_subject_superset_cascade_childof_optional(void);
void Parser_expr_w_symbol(void);
void Parser_expr_w_newline(void);
void Parser_subj_entity_w_explicit_self(void);
void Parser_subj_entity_w_explicit_self_superset(void);
void Parser_subj_entity_w_explicit_superset_relation(void);
void Parser_subj_entity_w_explicit_self_superset_relation(void);
void Parser_obj_entity_w_explicit_self(void);
void Parser_obj_entity_w_explicit_self_superset(void);
void Parser_obj_entity_w_explicit_superset_relation(void);
void Parser_obj_entity_w_explicit_self_superset_relation(void);
void Parser_pred_entity_w_explicit_self(void);
void Parser_pred_entity_w_explicit_self_superset(void);
void Parser_pred_entity_w_explicit_superset_relation(void);
void Parser_pred_entity_w_explicit_self_superset_relation(void);
void Parser_pred_entity_no_args_w_explicit_self(void);
void Parser_pred_entity_no_args_w_explicit_self_superset(void);
void Parser_pred_entity_no_args_w_explicit_superset_relation(void);
void Parser_pred_entity_no_args_w_explicit_self_superset_relation(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_self(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_superset_relation(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset_relation(void);
void Parser_newline(void);
void Parser_2_newlines(void);
void Parser_3_newlines(void);
void Parser_space(void);
void Parser_2_spaces(void);
void Parser_trailing_newline(void);
void Parser_2_trailing_newlines(void);
void Parser_trailing_space(void);
void Parser_2_trailing_spaces(void);
void Parser_template_type(void);
void Parser_predicate_w_parens(void);
void Parser_switch_id(void);
void Parser_case_pair(void);
void Parser_pair_w_invalid_role(void);
void Parser_case_w_missing_obj(void);

// Testsuite 'Plecs'
void Plecs_null(void);
void Plecs_empty(void);
void Plecs_space(void);
void Plecs_space_newline(void);
void Plecs_two_empty_newlines(void);
void Plecs_three_empty_newlines(void);
void Plecs_newline_trailing_space(void);
void Plecs_newline_trailing_spaces(void);
void Plecs_multiple_trailing_newlines(void);
void Plecs_invalid_2_identifiers_separated_by_space(void);
void Plecs_entity(void);
void Plecs_entity_w_entity(void);
void Plecs_entity_w_pair(void);
void Plecs_2_entities(void);
void Plecs_2_entities_w_entities(void);
void Plecs_3_entities_w_pairs(void);
void Plecs_line_comment(void);
void Plecs_line_comment_before_stmt(void);
void Plecs_line_comment_after_stmt(void);
void Plecs_line_comment_between_stmt(void);
void Plecs_multiple_line_comment(void);
void Plecs_line_comment_after_stmt_same_line(void);
void Plecs_comma_separated_pred(void);
void Plecs_comma_separated_pred_w_subj(void);
void Plecs_comma_separated_pred_w_subj_obj(void);
void Plecs_comma_separated_pred_trailing_comma(void);
void Plecs_comma_separated_pred_trailing_comma_newline(void);
void Plecs_comma_separated_pred_trailing_comma_newline_multiline(void);
void Plecs_hierarchy_1_child(void);
void Plecs_hierarchy_2_children(void);
void Plecs_hierarchy_1_child_same_line(void);
void Plecs_hierarchy_2_children_same_line(void);
void Plecs_entity_after_hierarchy(void);
void Plecs_newline_before_scope_open(void);
void Plecs_comment_before_scope_open(void);
void Plecs_comment_after_newline_before_scope_open(void);
void Plecs_hierarchy_2_levels(void);
void Plecs_hierarchy_2_levels_2_subtrees(void);
void Plecs_missing_end_of_scope(void);
void Plecs_missing_end_of_predicate_scope(void);
void Plecs_create_in_scope(void);
void Plecs_hierarchy_w_pred_subj(void);
void Plecs_hierarchy_custom_relation(void);
void Plecs_hierarchy_custom_relation_2_levels(void);
void Plecs_hierarchy_custom_relation_apply_to_object(void);
void Plecs_hierarchy_custom_relation_apply_to_object_2_levels(void);
void Plecs_entity_after_hierarchy_custom_relation(void);
void Plecs_entity_after_hierarchy_custom_relation_2_levels(void);
void Plecs_pred_scope(void);
void Plecs_pred_scope_2_levels(void);
void Plecs_pred_scope_inside_with(void);
void Plecs_pred_scope_nested_w_subj_scope(void);
void Plecs_with_tag(void);
void Plecs_with_tag_2_entities(void);
void Plecs_with_tag_same_line(void);
void Plecs_with_tag_2_entities_same_line(void);
void Plecs_with_tag_2_levels(void);
void Plecs_with_tag_2_levels_2_subtrees(void);
void Plecs_with_n_tags(void);
void Plecs_with_n_tags_2_levels(void);
void Plecs_with_after_scope(void);
void Plecs_with_after_with(void);
void Plecs_scope_inside_with_inside_scope(void);
void Plecs_with_inside_scope(void);
void Plecs_assignment_w_1(void);
void Plecs_assignment_w_2(void);
void Plecs_assignment_w_pair(void);
void Plecs_assignment_w_invalid_subject(void);
void Plecs_assignment_w_invalid_with(void);
void Plecs_inherit_w_colon(void);
void Plecs_inherit_w_colon_w_scope(void);
void Plecs_inherit_w_colon_w_assign(void);
void Plecs_assign_component_value(void);
void Plecs_assign_2_component_values(void);
void Plecs_assign_component_value_in_assign_scope(void);
void Plecs_assign_2_component_values_in_assign_scope(void);
void Plecs_type_and_assign_in_plecs(void);
void Plecs_type_and_assign_in_plecs_w_2_members(void);
void Plecs_type_and_assign_in_plecs_w_3_members(void);
void Plecs_type_and_assign_in_plecs_w_enum(void);
void Plecs_type_and_assign_in_plecs_w_enum_using_meta(void);
void Plecs_type_and_assign_in_plecs_w_enum_primitive_using_meta(void);
void Plecs_type_and_assign_in_plecs_w_enum_primitive_and_struct(void);
void Plecs_type_and_assign_in_plecs_nested_member(void);
void Plecs_open_scope_no_parent(void);
void Plecs_create_subject_in_root_scope_w_resolvable_id(void);
void Plecs_create_subject_in_scope_w_resolvable_id(void);
void Plecs_create_subject_in_scope_w_resolvable_id_using(void);
void Plecs_using_scope(void);
void Plecs_using_nested_scope(void);
void Plecs_using_nested_in_scope(void);
void Plecs_using_with_scope(void);
void Plecs_using_w_entity_ref_in_value_2_members(void);
void Plecs_using_w_entity_ref_in_value_3_members(void);
void Plecs_2_using_scope(void);
void Plecs_2_using_in_different_scope(void);
void Plecs_empty_scope_after_using(void);
void Plecs_assignment_to_non_component(void);
void Plecs_struct_w_member_w_assignment_to_nothing(void);
void Plecs_struct_w_member_w_assignment_to_empty_scope(void);
void Plecs_scope_after_assign(void);
void Plecs_assign_after_inherit(void);
void Plecs_multiple_assignments_single_line(void);
void Plecs_2_stmts_in_scope_w_no_parent(void);
void Plecs_scope_after_assign_1_tag(void);
void Plecs_scope_after_assign_2_tags(void);
void Plecs_invalid_nested_assignment(void);
void Plecs_invalid_partial_pair_assignment(void);
void Plecs_empty_assignment(void);
void Plecs_assign_tag_to_parent(void);
void Plecs_assign_component_to_parent(void);
void Plecs_empty_assignment_before_end_of_scope(void);
void Plecs_assign_to_parent_pair_w_new_entities_in_scope(void);
void Plecs_assign_to_parent_pair_w_existing_entities_in_scope(void);
void Plecs_default_child_component(void);
void Plecs_default_child_component_w_assign(void);
void Plecs_struct_type_w_default_child_component(void);
void Plecs_struct_type_w_default_child_component_nested_member(void);
void Plecs_enum_type_w_default_child_component(void);
void Plecs_comment_as_brief_doc(void);
void Plecs_comment_as_brief_doc_after_using(void);
void Plecs_comment_as_brief_doc_2_stmts(void);
void Plecs_comment_type(void);
void Plecs_empty_doc_comment(void);
void Plecs_newline_after_doc_comment(void);
void Plecs_default_type_from_with(void);
void Plecs_scope_w_1_subj_and_2_pairs(void);
void Plecs_inherit_from_multiple(void);
void Plecs_assign_pair_component(void);
void Plecs_assign_pair_component_in_scope(void);
void Plecs_set_entity_names(void);

// Testsuite 'GlobalComponentIds'
void GlobalComponentIds_declare(void);
void GlobalComponentIds_declare_w_entity(void);
void GlobalComponentIds_declare_2_world(void);
void GlobalComponentIds_declare_tag(void);
void GlobalComponentIds_declare_tag_w_entity(void);
void GlobalComponentIds_declare_entity(void);
void GlobalComponentIds_reuse_300_component_ids(void);

// Testsuite 'Doc'
void Doc_get_set_brief(void);
void Doc_get_set_detail(void);
void Doc_get_set_link(void);

// Testsuite 'Hierarchies'
void Hierarchies_setup(void);
void Hierarchies_empty_scope(void);
void Hierarchies_get_parent(void);
void Hierarchies_get_parent_from_nested(void);
void Hierarchies_get_parent_from_nested_2(void);
void Hierarchies_get_object_from_0(void);
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
void Hierarchies_scope_component_no_macro(void);
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
void Hierarchies_add_path_from_scope(void);
void Hierarchies_add_path_from_scope_new_entity(void);
void Hierarchies_new_w_child_in_root(void);
void Hierarchies_delete_child(void);
void Hierarchies_delete_2_children(void);
void Hierarchies_delete_2_children_different_type(void);
void Hierarchies_delete_tree_2_levels(void);
void Hierarchies_delete_tree_3_levels(void);
void Hierarchies_delete_tree_count_tables(void);
void Hierarchies_delete_tree_staged(void);
void Hierarchies_delete_tree_empty_table(void);
void Hierarchies_delete_tree_recreate(void);
void Hierarchies_delete_tree_w_onremove(void);
void Hierarchies_delete_tree_w_dtor(void);
void Hierarchies_get_child_count(void);
void Hierarchies_get_child_count_2_tables(void);
void Hierarchies_get_child_count_no_children(void);
void Hierarchies_scope_iter_after_delete_tree(void);
void Hierarchies_add_child_after_delete_tree(void);
void Hierarchies_add_child_to_recycled_parent(void);
void Hierarchies_get_type_after_recycled_parent_add(void);
void Hierarchies_rematch_after_add_to_recycled_parent(void);
void Hierarchies_cascade_after_recycled_parent_change(void);
void Hierarchies_long_name_depth_0(void);
void Hierarchies_long_name_depth_1(void);
void Hierarchies_long_name_depth_2(void);
void Hierarchies_ensure_1_parent_after_adding_2(void);
void Hierarchies_ensure_child_alive_after_deleting_prev_parent(void);

// Testsuite 'Has'
void Has_zero(void);
void Has_1_of_0(void);
void Has_1_of_1(void);
void Has_1_of_2(void);
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
void Has_has_wildcard(void);
void Has_has_wildcard_pair(void);

// Testsuite 'Count'
void Count_count_empty(void);
void Count_count_w_entity_0(void);
void Count_count_1_component(void);

// Testsuite 'Get_component'
void Get_component_setup(void);
void Get_component_get_empty(void);
void Get_component_get_1_from_1(void);
void Get_component_get_1_from_2(void);
void Get_component_get_2_from_2(void);
void Get_component_get_2_from_3(void);
void Get_component_get_1_from_2_in_progress_from_main_stage(void);
void Get_component_get_1_from_2_add_in_progress(void);
void Get_component_get_both_from_2_add_in_progress(void);
void Get_component_get_both_from_2_add_remove_in_progress(void);
void Get_component_get_childof_component(void);

// Testsuite 'Reference'
void Reference_setup(void);
void Reference_get_ref(void);
void Reference_get_ref_after_add(void);
void Reference_get_ref_after_remove(void);
void Reference_get_ref_after_delete(void);
void Reference_get_ref_after_realloc(void);
void Reference_get_ref_after_realloc_w_lifecycle(void);
void Reference_get_ref_staged(void);
void Reference_get_ref_after_new_in_stage(void);
void Reference_get_ref_monitored(void);
void Reference_get_nonexisting(void);

// Testsuite 'Delete'
void Delete_setup(void);
void Delete_delete_1(void);
void Delete_delete_1_again(void);
void Delete_delete_empty(void);
void Delete_delete_nonexist(void);
void Delete_delete_1st_of_3(void);
void Delete_delete_2nd_of_3(void);
void Delete_delete_2_of_3(void);
void Delete_delete_3_of_3(void);
void Delete_delete_w_on_remove(void);
void Delete_clear_1_component(void);
void Delete_clear_2_components(void);
void Delete_alive_after_delete(void);
void Delete_alive_after_clear(void);
void Delete_alive_after_staged_delete(void);
void Delete_alive_while_staged(void);
void Delete_alive_while_staged_w_delete(void);
void Delete_alive_while_staged_w_delete_recycled_id(void);
void Delete_alive_after_recycle(void);
void Delete_delete_recycled(void);
void Delete_get_alive_for_alive(void);
void Delete_get_alive_for_recycled(void);
void Delete_get_alive_for_not_alive(void);
void Delete_get_alive_w_generation_for_recycled_alive(void);
void Delete_get_alive_w_generation_for_recycled_not_alive(void);
void Delete_get_alive_for_0(void);
void Delete_get_alive_for_nonexistent(void);
void Delete_move_w_dtor_move(void);
void Delete_move_w_dtor_no_move(void);
void Delete_move_w_no_dtor_move(void);
void Delete_wrap_generation_count(void);

// Testsuite 'OnDelete'
void OnDelete_on_delete_flags(void);
void OnDelete_on_delete_id_default(void);
void OnDelete_on_delete_id_remove(void);
void OnDelete_on_delete_id_delete(void);
void OnDelete_on_delete_relation_default(void);
void OnDelete_on_delete_relation_remove(void);
void OnDelete_on_delete_relation_delete(void);
void OnDelete_on_delete_object_default(void);
void OnDelete_on_delete_object_remove(void);
void OnDelete_on_delete_object_delete(void);
void OnDelete_on_delete_id_throw(void);
void OnDelete_on_delete_relation_throw(void);
void OnDelete_on_delete_object_throw(void);
void OnDelete_on_delete_id_remove_no_instances(void);
void OnDelete_on_delete_id_delete_no_instances(void);
void OnDelete_on_delete_id_throw_no_instances(void);
void OnDelete_on_delete_cyclic_id_default(void);
void OnDelete_on_delete_cyclic_id_remove(void);
void OnDelete_on_delete_cyclic_id_remove_both(void);
void OnDelete_on_delete_cyclic_id_delete(void);
void OnDelete_on_delete_cyclic_id_delete_both(void);
void OnDelete_on_delete_cyclic_relation_default(void);
void OnDelete_on_delete_cyclic_relation_remove(void);
void OnDelete_on_delete_cyclic_relation_remove_both(void);
void OnDelete_on_delete_cyclic_relation_delete(void);
void OnDelete_on_delete_cyclic_object_default(void);
void OnDelete_on_delete_cyclic_object_remove(void);
void OnDelete_on_delete_cyclic_object_delete(void);
void OnDelete_on_delete_remove_2_comps(void);
void OnDelete_on_delete_remove_2_comps_to_existing_table(void);
void OnDelete_on_delete_delete_recursive(void);
void OnDelete_on_delete_component_throw(void);
void OnDelete_on_delete_remove_2_relations(void);
void OnDelete_on_delete_remove_object_w_2_relations(void);
void OnDelete_on_delete_remove_object_w_5_relations(void);
void OnDelete_on_delete_remove_object_w_50_relations(void);
void OnDelete_on_delete_remove_object_w_50_relations_3_tables(void);
void OnDelete_remove_id_from_2_tables(void);
void OnDelete_remove_relation_from_2_tables(void);
void OnDelete_remove_object_from_2_tables(void);
void OnDelete_remove_id_and_relation(void);
void OnDelete_remove_id_and_relation_from_2_tables(void);
void OnDelete_stresstest_many_objects(void);
void OnDelete_stresstest_many_relations(void);
void OnDelete_stresstest_many_objects_on_delete(void);
void OnDelete_stresstest_many_relations_on_delete(void);
void OnDelete_on_delete_empty_table_w_on_remove(void);
void OnDelete_delete_table_in_on_remove_during_fini(void);
void OnDelete_delete_other_in_on_remove_during_fini(void);
void OnDelete_on_delete_remove_id_w_role(void);
void OnDelete_on_delete_merge_pair_component(void);
void OnDelete_delete_with_tag(void);
void OnDelete_delete_with_component(void);
void OnDelete_delete_with_pair(void);
void OnDelete_delete_with_object_wildcard(void);
void OnDelete_delete_with_relation_wildcard(void);
void OnDelete_delete_all_with_entity(void);
void OnDelete_remove_childof_entity(void);
void OnDelete_remove_childof_wildcard(void);
void OnDelete_delete_child_of_delete_with(void);

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
void Set_get_mut_new(void);
void Set_get_mut_existing(void);
void Set_get_mut_tag_new(void);
void Set_get_mut_tag_existing(void);
void Set_get_mut_tag_new_w_comp(void);
void Set_get_mut_tag_existing_w_comp(void);
void Set_get_mut_tag_new_w_pair(void);
void Set_get_mut_tag_existing_w_pair(void);
void Set_modified_w_on_set(void);
void Set_modified_no_component(void);
void Set_get_mut_w_add_in_on_add(void);
void Set_get_mut_w_remove_in_on_add(void);
void Set_emplace(void);
void Set_emplace_existing(void);
void Set_emplace_w_move(void);

// Testsuite 'Lookup'
void Lookup_setup(void);
void Lookup_lookup(void);
void Lookup_lookup_component(void);
void Lookup_lookup_not_found(void);
void Lookup_lookup_child(void);
void Lookup_lookup_w_null_name(void);
void Lookup_get_name(void);
void Lookup_get_name_no_name(void);
void Lookup_get_name_from_empty(void);
void Lookup_lookup_by_id(void);
void Lookup_lookup_recycled_by_id(void);
void Lookup_lookup_symbol_by_id(void);
void Lookup_lookup_name_w_digit(void);
void Lookup_lookup_symbol_w_digit(void);
void Lookup_lookup_path_w_digit(void);
void Lookup_set_name_of_existing(void);
void Lookup_change_name_of_existing(void);
void Lookup_lookup_alias(void);
void Lookup_lookup_scoped_alias(void);
void Lookup_define_duplicate_alias(void);
void Lookup_lookup_null(void);
void Lookup_lookup_symbol_null(void);
void Lookup_lookup_this(void);
void Lookup_lookup_wildcard(void);
void Lookup_lookup_any(void);
void Lookup_lookup_path_this(void);
void Lookup_lookup_path_wildcard(void);
void Lookup_lookup_path_this_from_scope(void);
void Lookup_lookup_path_wildcard_from_scope(void);
void Lookup_resolve_builtin_symbols(void);

// Testsuite 'Singleton'
void Singleton_set_get_singleton(void);
void Singleton_get_mut_singleton(void);
void Singleton_singleton_system(void);

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
void ComponentLifecycle_setup(void);
void ComponentLifecycle_ctor_on_add(void);
void ComponentLifecycle_ctor_on_new(void);
void ComponentLifecycle_dtor_on_remove(void);
void ComponentLifecycle_dtor_on_delete(void);
void ComponentLifecycle_copy_on_set(void);
void ComponentLifecycle_copy_on_override(void);
void ComponentLifecycle_copy_on_clone(void);
void ComponentLifecycle_no_copy_on_move(void);
void ComponentLifecycle_ctor_copy_on_snapshot(void);
void ComponentLifecycle_copy_on_snapshot(void);
void ComponentLifecycle_dtor_on_restore(void);
void ComponentLifecycle_ctor_on_tag(void);
void ComponentLifecycle_dtor_on_tag(void);
void ComponentLifecycle_copy_on_tag(void);
void ComponentLifecycle_move_on_tag(void);
void ComponentLifecycle_merge_to_different_table(void);
void ComponentLifecycle_merge_to_new_table(void);
void ComponentLifecycle_delete_in_stage(void);
void ComponentLifecycle_ctor_on_add_pair(void);
void ComponentLifecycle_ctor_on_add_pair_set_ctor_after_table(void);
void ComponentLifecycle_ctor_on_add_pair_tag(void);
void ComponentLifecycle_ctor_on_add_pair_tag_set_ctor_after_table(void);
void ComponentLifecycle_ctor_on_move_pair(void);
void ComponentLifecycle_move_on_realloc(void);
void ComponentLifecycle_move_on_bulk_new(void);
void ComponentLifecycle_move_on_delete(void);
void ComponentLifecycle_move_dtor_on_delete(void);
void ComponentLifecycle_copy_on_override_pair(void);
void ComponentLifecycle_copy_on_override_pair_tag(void);
void ComponentLifecycle_copy_on_set_pair(void);
void ComponentLifecycle_copy_on_set_pair_tag(void);
void ComponentLifecycle_prevent_lifecycle_overwrite(void);
void ComponentLifecycle_prevent_lifecycle_overwrite_null_callbacks(void);
void ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks(void);
void ComponentLifecycle_set_lifecycle_after_trigger(void);
void ComponentLifecycle_valid_entity_in_dtor_after_delete(void);
void ComponentLifecycle_ctor_w_emplace(void);
void ComponentLifecycle_dtor_on_fini(void);
void ComponentLifecycle_valid_type_in_dtor_on_fini(void);
void ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini(void);
void ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_fini(void);
void ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_delete_parent(void);
void ComponentLifecycle_delete_in_dtor_same_type_on_fini(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_fini(void);
void ComponentLifecycle_delete_self_in_dtor_on_fini(void);
void ComponentLifecycle_delete_in_dtor_same_type_on_delete_parent(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent(void);
void ComponentLifecycle_delete_self_in_dtor_on_delete_parent(void);
void ComponentLifecycle_delete_in_dtor_same_type_on_delete(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_delete(void);
void ComponentLifecycle_delete_self_in_dtor_on_delete(void);
void ComponentLifecycle_on_set_after_set(void);

// Testsuite 'Pipeline'
void Pipeline_setup(void);
void Pipeline_system_order_same_phase(void);
void Pipeline_system_order_same_phase_after_disable(void);
void Pipeline_system_order_same_phase_after_activate(void);
void Pipeline_system_order_different_phase(void);
void Pipeline_system_order_different_phase_after_disable(void);
void Pipeline_system_order_different_phase_after_activate(void);
void Pipeline_system_order_after_new_system_lower_id(void);
void Pipeline_system_order_after_new_system_inbetween_id(void);
void Pipeline_system_order_after_new_system_higher_id(void);
void Pipeline_system_reverse_order_by_phase_custom_pipeline(void);
void Pipeline_merge_after_staged_out(void);
void Pipeline_merge_after_not_out(void);
void Pipeline_no_merge_after_main_out(void);
void Pipeline_merge_after_staged_in_out(void);
void Pipeline_merge_after_staged_inout_main_implicit_inout(void);
void Pipeline_merge_after_staged_inout_main_inout(void);
void Pipeline_merge_after_staged_out_before_owned(void);
void Pipeline_switch_pipeline(void);
void Pipeline_run_pipeline(void);
void Pipeline_get_pipeline_from_stage(void);
void Pipeline_3_systems_3_types(void);
void Pipeline_random_read_after_random_write_out_in(void);
void Pipeline_random_read_after_random_write_inout_in(void);
void Pipeline_random_read_after_random_write_out_inout(void);
void Pipeline_random_read_after_random_write_inout_inout(void);
void Pipeline_random_read_after_random_write_w_not_write(void);
void Pipeline_random_read_after_random_write_w_not_read(void);
void Pipeline_random_read_after_random_write_w_wildcard(void);
void Pipeline_random_in_after_random_inout_after_random_out(void);
void Pipeline_stage_write_before_read(void);
void Pipeline_mixed_multithreaded(void);
void Pipeline_mixed_staging(void);
void Pipeline_no_staging_system_create_query(void);
void Pipeline_single_threaded_pipeline_change(void);
void Pipeline_multi_threaded_pipeline_change(void);
void Pipeline_activate_after_add(void);

// Testsuite 'SystemMisc'
void SystemMisc_setup(void);
void SystemMisc_invalid_not_without_id(void);
void SystemMisc_invalid_optional_without_id(void);
void SystemMisc_invalid_entity_without_id(void);
void SystemMisc_invalid_empty_without_id(void);
void SystemMisc_invalid_empty_element(void);
void SystemMisc_invalid_empty_element_w_space(void);
void SystemMisc_invalid_empty_or(void);
void SystemMisc_invalid_empty_or_w_space(void);
void SystemMisc_invalid_or_w_not(void);
void SystemMisc_invalid_not_w_or(void);
void SystemMisc_invalid_0_w_and(void);
void SystemMisc_invalid_component_id(void);
void SystemMisc_invalid_entity_id(void);
void SystemMisc_invalid_null_string(void);
void SystemMisc_invalid_empty_string(void);
void SystemMisc_invalid_empty_string_w_space(void);
void SystemMisc_invalid_mixed_src_modifier(void);
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
void SystemMisc_system_initial_state(void);
void SystemMisc_add_own_component(void);
void SystemMisc_change_system_action(void);
void SystemMisc_system_readeactivate(void);
void SystemMisc_system_readeactivate_w_2_systems(void);
void SystemMisc_add_to_system_in_progress(void);
void SystemMisc_redefine_null_signature(void);
void SystemMisc_redefine_0_signature(void);
void SystemMisc_one_named_column_of_two(void);
void SystemMisc_two_named_columns_of_two(void);
void SystemMisc_redeclare_system_same_expr(void);
void SystemMisc_redeclare_system_null_expr(void);
void SystemMisc_redeclare_system_0_expr(void);
void SystemMisc_redeclare_system_different_expr(void);
void SystemMisc_redeclare_system_null_and_expr(void);
void SystemMisc_redeclare_system_expr_and_null(void);
void SystemMisc_redeclare_system_expr_and_0(void);
void SystemMisc_redeclare_system_0_and_expr(void);
void SystemMisc_redeclare_system_0_and_null(void);
void SystemMisc_redeclare_system_null_and_0(void);
void SystemMisc_redeclare_system_explicit_id(void);
void SystemMisc_redeclare_system_explicit_id_null_expr(void);
void SystemMisc_redeclare_system_explicit_id_no_name(void);
void SystemMisc_declare_different_id_same_name(void);
void SystemMisc_declare_different_id_same_name_w_scope(void);
void SystemMisc_rw_in_implicit_any(void);
void SystemMisc_rw_in_implicit_shared(void);
void SystemMisc_rw_in_implicit_from_empty(void);
void SystemMisc_rw_in_implicit_from_entity(void);
void SystemMisc_rw_out_explicit_any(void);
void SystemMisc_rw_out_explicit_shared(void);
void SystemMisc_rw_out_explicit_from_empty(void);
void SystemMisc_rw_out_explicit_from_entity(void);
void SystemMisc_activate_system_for_table_w_n_pairs(void);
void SystemMisc_get_query(void);
void SystemMisc_set_get_context(void);
void SystemMisc_set_get_binding_context(void);
void SystemMisc_deactivate_after_disable(void);
void SystemMisc_system_w_self(void);
void SystemMisc_delete_system(void);
void SystemMisc_delete_pipeline_system(void);
void SystemMisc_delete_system_w_ctx(void);

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
void Sorting_sort_after_query(void);
void Sorting_sort_by_component_same_value_1(void);
void Sorting_sort_by_component_same_value_2(void);
void Sorting_sort_by_component_move_pivot(void);
void Sorting_sort_1000_entities(void);
void Sorting_sort_1000_entities_w_duplicates(void);
void Sorting_sort_1000_entities_again(void);
void Sorting_sort_1000_entities_2_types(void);
void Sorting_sort_1500_entities_3_types(void);
void Sorting_sort_2000_entities_4_types(void);
void Sorting_sort_2_entities_2_types(void);
void Sorting_sort_3_entities_3_types(void);
void Sorting_sort_3_entities_3_types_2(void);
void Sorting_sort_4_entities_4_types(void);
void Sorting_sort_1000_entities_2_types_again(void);
void Sorting_sort_1000_entities_add_type_after_sort(void);
void Sorting_sort_shared_component(void);
void Sorting_sort_w_tags_only(void);
void Sorting_sort_childof_marked(void);
void Sorting_sort_isa_marked(void);
void Sorting_sort_relation_marked(void);
void Sorting_dont_resort_after_set_unsorted_component(void);
void Sorting_dont_resort_after_set_unsorted_component_w_tag(void);
void Sorting_dont_resort_after_set_unsorted_component_w_tag_w_out_term(void);

// Testsuite 'Filter'
void Filter_filter_1_term(void);
void Filter_filter_2_terms(void);
void Filter_filter_3_terms(void);
void Filter_filter_3_terms_w_or(void);
void Filter_filter_4_terms_w_or_at_1(void);
void Filter_filter_1_term_wildcard(void);
void Filter_filter_1_term_any(void);
void Filter_filter_1_term_same_subj_obj(void);
void Filter_filter_1_term_acyclic_same_subj_obj(void);
void Filter_filter_1_term_acyclic_reflexive_same_subj_obj(void);
void Filter_filter_1_term_same_subj_obj_var(void);
void Filter_filter_1_term_acyclic_same_subj_obj_var(void);
void Filter_filter_1_term_acyclic_reflexive_same_subj_obj_var(void);
void Filter_filter_1_term_non_acyclic_superset(void);
void Filter_filter_1_term_dont_inherit_default_set(void);
void Filter_filter_1_term_dont_inherit_pair_default_set(void);
void Filter_filter_w_pair_id(void);
void Filter_filter_w_pred_obj(void);
void Filter_filter_move(void);
void Filter_filter_copy(void);
void Filter_filter_w_resources_copy(void);
void Filter_filter_w_10_terms(void);
void Filter_filter_w_10_terms_move(void);
void Filter_filter_w_10_terms_copy(void);
void Filter_filter_w_and_flag(void);
void Filter_filter_w_or_flag(void);
void Filter_filter_w_not_flag(void);
void Filter_filter_filter(void);
void Filter_term_w_id(void);
void Filter_term_w_pair_id(void);
void Filter_term_w_pred_obj(void);
void Filter_term_w_pair_finalize_twice(void);
void Filter_term_w_role(void);
void Filter_term_w_pred_role(void);
void Filter_term_w_self(void);
void Filter_term_w_superset(void);
void Filter_term_w_subset(void);
void Filter_term_w_self_superset(void);
void Filter_term_w_superset_custom_relation(void);
void Filter_term_w_self_superset_custom_relation(void);
void Filter_term_w_self_min_max_depth(void);
void Filter_term_w_self_invalid_min_depth(void);
void Filter_term_w_self_invalid_max_depth(void);
void Filter_term_w_self_invalid_min_max_depth(void);
void Filter_term_w_superset_min_depth(void);
void Filter_term_w_superset_max_depth(void);
void Filter_term_w_superset_min_max_depth(void);
void Filter_term_iter_component(void);
void Filter_term_iter_tag(void);
void Filter_term_iter_pair(void);
void Filter_term_iter_pair_w_rel_wildcard(void);
void Filter_term_iter_pair_w_obj_wildcard(void);
void Filter_term_iter_pair_w_rel_wildcard_n_matches(void);
void Filter_term_iter_pair_w_rel_wildcard_n_matches_w_data(void);
void Filter_term_iter_pair_w_obj_wildcard_n_matches(void);
void Filter_term_iter_pair_w_obj_wildcard_n_matches_w_data(void);
void Filter_term_iter_w_superset(void);
void Filter_term_iter_w_superset_base_w_2_components(void);
void Filter_term_iter_w_superset_childof(void);
void Filter_term_iter_w_superset_self(void);
void Filter_term_iter_w_superset_self_childof(void);
void Filter_term_iter_w_superset_tag(void);
void Filter_term_iter_w_superset_pair(void);
void Filter_term_iter_w_superset_pair_obj_wildcard(void);
void Filter_term_iter_in_stage(void);
void Filter_term_iter_w_readonly_term(void);
void Filter_term_iter_type_set(void);
void Filter_filter_iter_1_tag(void);
void Filter_filter_iter_2_tags(void);
void Filter_filter_iter_2_tags_1_not(void);
void Filter_filter_iter_3_tags_2_or(void);
void Filter_filter_iter_1_component(void);
void Filter_filter_iter_2_components(void);
void Filter_filter_iter_pair_id(void);
void Filter_filter_iter_2_pair_ids(void);
void Filter_filter_iter_childof_pair_0_parent(void);
void Filter_filter_iter_pair_pred_obj(void);
void Filter_filter_iter_pair_2_pred_obj(void);
void Filter_filter_iter_null(void);
void Filter_filter_iter_1_not_tag(void);
void Filter_filter_iter_2_tags_1_optional(void);
void Filter_filter_iter_2_components_1_optional(void);
void Filter_filter_iter_in_stage(void);
void Filter_filter_iter_10_tags(void);
void Filter_filter_iter_20_tags(void);
void Filter_filter_iter_10_components(void);
void Filter_filter_iter_20_components(void);
void Filter_filter_iter_superset(void);
void Filter_filter_iter_superset_childof(void);
void Filter_filter_iter_type_set(void);
void Filter_filter_iter_w_readonly_term(void);
void Filter_filter_iter_w_from_nothing_term(void);
void Filter_filter_iter_pair_w_rel_wildcard_n_matches(void);
void Filter_filter_iter_pair_w_obj_wildcard_n_matches(void);
void Filter_filter_iter_pair_w_2_wildcards_1_match(void);
void Filter_filter_iter_pair_w_2_wildcards_2x1_matches(void);
void Filter_filter_iter_pair_w_2_wildcards_2x2_matches(void);
void Filter_filter_iter_pair_w_3_wildcards_2x2x2_matches(void);
void Filter_match_disabled(void);
void Filter_match_prefab(void);
void Filter_chain_term_iter(void);
void Filter_chain_filter_iter(void);
void Filter_chain_query_iter(void);
void Filter_chain_rule_iter(void);
void Filter_chain_iter_2_levels(void);
void Filter_filter_from_expr_2_terms_err(void);
void Filter_chain_term_iter_w_term_iter(void);
void Filter_chain_filter_iter_w_term_iter(void);
void Filter_chain_w_term_iter_component(void);
void Filter_filter_w_recycled_object_and_id(void);
void Filter_term_iter_w_filter_term(void);
void Filter_filter_iter_w_filter_term(void);
void Filter_filter_iter_w_2_terms_1_filter(void);
void Filter_filter_iter_w_3_terms_2_filter(void);
void Filter_filter_iter_2_terms_filter_all(void);
void Filter_filter_iter_2_terms_filter_all_w_out(void);
void Filter_filter_instanced_w_singleton(void);
void Filter_filter_instanced_w_base(void);
void Filter_filter_no_instancing_w_singleton(void);
void Filter_filter_no_instancing_w_base(void);
void Filter_filter_no_this_tag(void);
void Filter_filter_no_this_component(void);
void Filter_filter_no_this_tag_2_ents(void);
void Filter_filter_no_this_component_2_ents(void);
void Filter_filter_no_this_tag_2_ents_1_not(void);
void Filter_filter_no_this_component_2_ents_1_not(void);
void Filter_filter_no_this_component_1_not(void);
void Filter_filter_iter_frame_offset(void);
void Filter_filter_1_term_no_alloc(void);
void Filter_filter_cache_size_terms_no_alloc(void);
void Filter_filter_lt_cache_size_terms_no_alloc(void);
void Filter_move_self(void);
void Filter_match_empty_tables(void);
void Filter_match_switch_w_switch(void);
void Filter_match_switch_w_case(void);
void Filter_match_switch_w_case_2_terms(void);
void Filter_and_term(void);
void Filter_or_term(void);
void Filter_iter_while_creating_components(void);

// Testsuite 'Query'
void Query_query_changed_after_new(void);
void Query_query_changed_after_delete(void);
void Query_query_changed_after_add(void);
void Query_query_changed_after_remove(void);
void Query_query_changed_after_set(void);
void Query_query_change_after_modified(void);
void Query_query_change_after_out_system(void);
void Query_query_change_after_in_system(void);
void Query_query_change_after_modified_out_term(void);
void Query_query_change_check_iter(void);
void Query_query_change_check_iter_after_skip_read(void);
void Query_query_change_check_iter_after_skip_write(void);
void Query_query_change_parent_term(void);
void Query_query_change_prefab_term(void);
void Query_subquery_match_existing(void);
void Query_subquery_match_new(void);
void Query_subquery_inactive(void);
void Query_subquery_unmatch(void);
void Query_subquery_rematch(void);
void Query_subquery_rematch_w_parent_optional(void);
void Query_subquery_rematch_w_sub_optional(void);
void Query_query_single_pairs(void);
void Query_query_single_instanceof(void);
void Query_query_single_childof(void);
void Query_query_optional_owned(void);
void Query_query_optional_shared(void);
void Query_query_optional_shared_nested(void);
void Query_query_optional_any(void);
void Query_query_rematch_optional_after_add(void);
void Query_get_owned_tag(void);
void Query_get_shared_tag(void);
void Query_explicit_delete(void);
void Query_get_column_size(void);
void Query_orphaned_query(void);
void Query_nested_orphaned_query(void);
void Query_invalid_access_orphaned_query(void);
void Query_stresstest_query_free(void);
void Query_only_from_entity(void);
void Query_only_from_singleton(void);
void Query_only_not_from_entity(void);
void Query_only_not_from_singleton(void);
void Query_get_filter(void);
void Query_group_by(void);
void Query_group_by_w_ctx(void);
void Query_group_by_w_sort_reverse_group_creation(void);
void Query_iter_valid(void);
void Query_query_optional_tag(void);
void Query_query_optional_shared_tag(void);
void Query_query_iter_10_tags(void);
void Query_query_iter_20_tags(void);
void Query_query_iter_10_components(void);
void Query_query_iter_20_components(void);
void Query_iter_type_set(void);
void Query_only_optional_from_entity(void);
void Query_only_optional_from_entity_no_match(void);
void Query_filter_term(void);
void Query_2_terms_1_filter(void);
void Query_3_terms_2_filter(void);
void Query_no_instancing_w_singleton(void);
void Query_no_instancing_w_shared(void);
void Query_query_iter_frame_offset(void);

// Testsuite 'Iter'
void Iter_page_iter_0_0(void);
void Iter_page_iter_1_0(void);
void Iter_page_iter_0_1(void);
void Iter_page_iter_n_0(void);
void Iter_page_iter_0_n(void);
void Iter_page_iter_m_n(void);
void Iter_page_iter_skip_1_table(void);
void Iter_page_iter_skip_2_tables(void);
void Iter_worker_iter_1(void);
void Iter_worker_iter_2(void);
void Iter_worker_iter_3(void);
void Iter_worker_iter_4(void);
void Iter_paged_iter_w_shared_comp(void);
void Iter_worker_iter_w_shared_comp(void);
void Iter_paged_iter_w_task_query(void);
void Iter_worker_iter_w_task_query(void);
void Iter_worker_iter_w_singleton(void);
void Iter_worker_iter_w_singleton_instanced(void);
void Iter_paged_iter_w_singleton(void);
void Iter_paged_iter_w_singleton_instanced(void);
void Iter_iter_1_term_no_alloc(void);
void Iter_iter_cache_size_terms_no_alloc(void);
void Iter_iter_lt_cache_size_terms_alloc(void);

// Testsuite 'Pairs'
void Pairs_type_w_one_pair(void);
void Pairs_type_w_two_pairs(void);
void Pairs_add_pair(void);
void Pairs_remove_pair(void);
void Pairs_add_tag_pair_for_tag(void);
void Pairs_add_tag_pair_for_component(void);
void Pairs_query_2_pairs(void);
void Pairs_query_2_pairs_2_instances_per_type(void);
void Pairs_query_pair_or_component(void);
void Pairs_query_pair_or_pair(void);
void Pairs_query_not_pair(void);
void Pairs_override_pair(void);
void Pairs_override_tag_pair(void);
void Pairs_pair_w_component_query(void);
void Pairs_on_add_pair(void);
void Pairs_on_add_pair_tag(void);
void Pairs_on_remove_pair(void);
void Pairs_on_remove_pair_tag(void);
void Pairs_on_remove_pair_on_delete(void);
void Pairs_on_remove_pair_tag_on_delete(void);
void Pairs_get_typeid_w_recycled_rel(void);
void Pairs_get_typeid_w_recycled_obj(void);
void Pairs_id_str_w_recycled_rel(void);
void Pairs_id_str_w_recycled_obj(void);
void Pairs_set_object_w_zero_sized_rel_comp(void);
void Pairs_dsl_pair(void);
void Pairs_dsl_pair_w_pred_wildcard(void);
void Pairs_dsl_pair_w_obj_wildcard(void);
void Pairs_dsl_pair_w_both_wildcard(void);
void Pairs_dsl_pair_w_explicit_subj_this(void);
void Pairs_dsl_pair_w_explicit_subj(void);
void Pairs_api_pair(void);
void Pairs_api_pair_w_pred_wildcard(void);
void Pairs_api_pair_w_obj_wildcard(void);
void Pairs_api_pair_w_both_wildcard(void);
void Pairs_api_pair_w_explicit_subj_this(void);
void Pairs_api_pair_w_explicit_subj(void);
void Pairs_typeid_from_tag(void);
void Pairs_typeid_from_component(void);
void Pairs_typeid_from_pair(void);
void Pairs_typeid_from_pair_w_rel_type(void);
void Pairs_typeid_from_pair_w_obj_type(void);
void Pairs_typeid_from_pair_w_rel_obj_type(void);
void Pairs_typeid_from_pair_w_rel_0_obj_type(void);
void Pairs_typeid_from_pair_w_rel_obj_0_type(void);
void Pairs_typeid_from_pair_w_rel_0_obj_0_type(void);
void Pairs_typeid_from_pair_with_0_obj(void);
void Pairs_tag_pair_w_rel_comp(void);
void Pairs_tag_pair_w_obj_comp(void);
void Pairs_tag_pair_w_rel_obj_comp(void);
void Pairs_get_tag_pair_w_rel_comp(void);
void Pairs_get_tag_pair_w_obj_comp(void);
void Pairs_get_tag_pair_w_rel_obj_comp(void);
void Pairs_tag_pair_w_childof_w_comp(void);
void Pairs_tag_pair_w_isa_w_comp(void);
void Pairs_get_1_object(void);
void Pairs_get_1_object_not_found(void);
void Pairs_get_n_objects(void);
void Pairs_get_object_for_id_from_self(void);
void Pairs_get_object_for_id_from_base(void);
void Pairs_get_object_for_id_from_nested_base(void);
void Pairs_get_object_for_id_not_found(void);
void Pairs_get_object_for_wildcard_from_self(void);
void Pairs_get_object_for_wildcard_from_base(void);
void Pairs_get_object_for_wildcard_from_nested_base(void);
void Pairs_ignore_childof_from_base(void);
void Pairs_add_exclusive_relation_twice(void);
void Pairs_add_same_exclusive_relation_twice(void);
void Pairs_set_exclusive_relation_twice(void);
void Pairs_add_exclusive_non_empty_table(void);
void Pairs_add_exclusive_non_empty_table_w_pairs(void);
void Pairs_add_pair_to_entity_w_exclusive_pair(void);
void Pairs_add_pair_to_entity_w_scope(void);
void Pairs_add_existing_exclusive_pair_after_pair(void);
void Pairs_add_symmetric_relation(void);
void Pairs_remove_symmetric_relation(void);
void Pairs_delete_entity_w_symmetric_relation(void);
void Pairs_add_symmetric_exclusive_relation(void);
void Pairs_with(void);
void Pairs_2_with(void);
void Pairs_nested_with(void);
void Pairs_with_relation(void);
void Pairs_2_with_relation(void);
void Pairs_nested_with_relation(void);
void Pairs_65k_relations(void);
void Pairs_add_not_alive_relation(void);
void Pairs_remove_wildcard(void);
void Pairs_remove_relation_wildcard(void);
void Pairs_remove_wildcard_all(void);
void Pairs_inherit_exclusive(void);
void Pairs_dont_inherit(void);

// Testsuite 'Rules'
void Rules_empty_rule(void);
void Rules_invalid_rule(void);
void Rules_comp_recycled_final_id(void);
void Rules_comp_recycled_id(void);
void Rules_pair_recycled_final_pred(void);
void Rules_pair_recycled_pred(void);
void Rules_pair_recycled_obj(void);
void Rules_pair_recycled_pred_obj(void);
void Rules_pair_recycled_matched_obj(void);
void Rules_pair_recycled_matched_obj_2_terms(void);
void Rules_pair_recycled_matched_pred_2_terms(void);
void Rules_1_comp(void);
void Rules_2_comp(void);
void Rules_1_comp_explicit_subject(void);
void Rules_2_comp_explicit_subject(void);
void Rules_1_fact_true(void);
void Rules_1_fact_false(void);
void Rules_2_facts_true(void);
void Rules_2_facts_1_false(void);
void Rules_2_facts_false(void);
void Rules_1_fact_pair_true(void);
void Rules_1_fact_pair_false(void);
void Rules_2_fact_pairs_true(void);
void Rules_2_fact_pairs_1_false(void);
void Rules_2_fact_pairs_false(void);
void Rules_wildcard_pred(void);
void Rules_wildcard_subj(void);
void Rules_wildcard_obj(void);
void Rules_any_pred(void);
void Rules_any_subj(void);
void Rules_any_obj(void);
void Rules_2_wildcard_as_subject(void);
void Rules_find_1_pair(void);
void Rules_find_2_pairs(void);
void Rules_find_w_pred_var(void);
void Rules_find_w_pred_var_explicit_subject(void);
void Rules_find_1_pair_w_object_var(void);
void Rules_find_2_pairs_w_object_var(void);
void Rules_find_1_pair_w_pred_var(void);
void Rules_find_2_pairs_w_pred_var(void);
void Rules_find_cyclic_pairs(void);
void Rules_join_by_object(void);
void Rules_join_by_predicate(void);
void Rules_join_by_predicate_from_subject(void);
void Rules_find_transitive(void);
void Rules_find_transitive_2_branches(void);
void Rules_transitive_subsets(void);
void Rules_transitive_subsets_2_terms(void);
void Rules_transitive_w_table_object(void);
void Rules_transitive_supersets(void);
void Rules_transitive_superset_w_subj_var(void);
void Rules_transitive_superset_w_subj_var_2_term(void);
void Rules_transitive_supersets_2_terms(void);
void Rules_transitive_constraint_on_superset_var(void);
void Rules_transitive_instances(void);
void Rules_transitive_instances_2_terms(void);
void Rules_transitive_implicit_isa_instances(void);
void Rules_transitive_implicit_isa_instances_2_terms(void);
void Rules_transitive_fact_false(void);
void Rules_transitive_fact_true_depth_1(void);
void Rules_transitive_fact_true_depth_2(void);
void Rules_transitive_fact_true_depth_3(void);
void Rules_transitive_fact_true_depth_4(void);
void Rules_transitive_fact_true_depth_5(void);
void Rules_transitive_fact_true_2_relationships(void);
void Rules_transitive_fact_same_subj_obj(void);
void Rules_transitive_fact_subset_superset(void);
void Rules_transitive_nonfinal_fact(void);
void Rules_transitive_nonfinal_fact_w_implicit_pred_subset(void);
void Rules_transitive_all(void);
void Rules_same_pred_obj(void);
void Rules_same_pred_obj_explicit_subject(void);
void Rules_implicit_is_a_tag_fact(void);
void Rules_implicit_is_a_tag_fact_2_lvls(void);
void Rules_implicit_is_a_tag_fact_3_lvls(void);
void Rules_implicit_is_a_tag_fact_2_lvls_both_matching(void);
void Rules_implicit_is_a_tag_fact_2_lvls_entity_matching(void);
void Rules_implicit_is_a_pair_fact(void);
void Rules_implicit_is_a_pair_2_lvls(void);
void Rules_implicit_is_a_pair_3_lvls(void);
void Rules_implicit_is_a_pair_2_lvls_both_matching(void);
void Rules_implicit_is_a_pair_2_lvls_entity_matching(void);
void Rules_implicit_is_a_pred_var(void);
void Rules_implicit_is_a_pair_var(void);
void Rules_implicit_is_a_transitive_pair_fact(void);
void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_subset(void);
void Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_obj(void);
void Rules_2_constrained_vars_by_subject_literal(void);
void Rules_2_constrained_vars_by_subject_literal_2_var_terms(void);
void Rules_term_w_nothing_set(void);
void Rules_term_w_nothing_set_w_this_term(void);
void Rules_comp_w_not_term(void);
void Rules_not_term_w_subj_var(void);
void Rules_not_term_w_subj_var_match_n_per_type(void);
void Rules_invalid_rule_w_only_not_term(void);
void Rules_invalid_rule_w_not_term_unknown_var(void);
void Rules_invalid_rule_w_not_term_unknown_pair_var(void);
void Rules_invalid_rule_w_not_term_unknown_pair_var_subj_var(void);
void Rules_rules_w_desc_id(void);
void Rules_rules_w_desc_pair(void);
void Rules_rules_w_desc_pair_empty_rel_obj(void);
void Rules_rules_w_desc_pair_pred_obj(void);
void Rules_rules_w_desc_wildcard(void);
void Rules_childof_0(void);
void Rules_childof_this(void);
void Rules_childof_this_as_identifier(void);
void Rules_optional_term(void);
void Rules_optional_term_w_component(void);
void Rules_optional_term_on_entity(void);
void Rules_optional_term_on_variable(void);
void Rules_optional_term_on_wildcard(void);
void Rules_optional_term_on_relation_this_obj(void);
void Rules_optional_w_subj_var(void);
void Rules_terms_set(void);
void Rules_value_set(void);
void Rules_term_w_this_this_this(void);
void Rules_term_w_x_x_x(void);
void Rules_term_w_same_subj_obj_var(void);
void Rules_filter_term(void);
void Rules_2_terms_1_filter(void);
void Rules_3_terms_2_filter(void);
void Rules_term_obj_w_this(void);
void Rules_term_subj_w_this(void);
void Rules_rule_iter_frame_offset(void);
void Rules_rule_iter_set_var(void);
void Rules_rule_iter_set_2_vars(void);
void Rules_rule_iter_set_pred_var(void);
void Rules_rule_iter_set_var_for_2_terms(void);
void Rules_rule_iter_set_cyclic_variable(void);
void Rules_rule_iter_set_cyclic_variable_w_this(void);

// Testsuite 'TransitiveRules'
void TransitiveRules_trans_X_X(void);
void TransitiveRules_trans_reflexive_X_X(void);
void TransitiveRules_trans_reflexive_X_X_2(void);
void TransitiveRules_trans_reflexive_X_Y(void);
void TransitiveRules_trans_X_Y_2_levels(void);
void TransitiveRules_trans_X_Y_2_levels_nonfinal(void);
void TransitiveRules_trans_pred_This_X__pred_X(void);
void TransitiveRules_trans_constrained_x_y(void);
void TransitiveRules_trans_entity_X_non_inclusive(void);
void TransitiveRules_trans_X_entity_non_inclusive(void);
void TransitiveRules_trans_entity_entity_non_reflexive(void);
void TransitiveRules_trans_this_x_after_tag_this(void);
void TransitiveRules_trans_this_x_before_tag_this(void);
void TransitiveRules_trans_this_x_after_tag_this_2_lvls(void);
void TransitiveRules_trans_this_x_before_tag_this_2_lvls(void);
void TransitiveRules_transitive_not_w_var(void);
void TransitiveRules_transitive_w_not_nonfinal(void);
void TransitiveRules_transitive_w_optional_nonfinal(void);
void TransitiveRules_transitive_w_optional_nonfinal_w_var(void);
void TransitiveRules_rule_iter_set_transitive_variable(void);
void TransitiveRules_rule_iter_set_transitive_self_variable(void);
void TransitiveRules_rule_iter_set_transitive_2_variables_set_one(void);
void TransitiveRules_rule_iter_set_transitive_2_variables_set_both(void);
void TransitiveRules_rule_iter_set_transitive_self_2_variables_set_both(void);

// Testsuite 'Trigger'
void Trigger_on_add_trigger_before_table(void);
void Trigger_on_add_trigger_after_table(void);
void Trigger_on_remove_trigger_before_table(void);
void Trigger_on_remove_trigger_after_table(void);
void Trigger_on_add_tag(void);
void Trigger_on_add_component(void);
void Trigger_on_add_wildcard(void);
void Trigger_on_add_pair(void);
void Trigger_on_add_pair_obj_wildcard(void);
void Trigger_on_add_pair_pred_wildcard(void);
void Trigger_on_add_pair_wildcard(void);
void Trigger_on_remove_tag(void);
void Trigger_on_remove_component(void);
void Trigger_on_remove_wildcard(void);
void Trigger_on_remove_pair(void);
void Trigger_on_remove_pair_obj_wildcard(void);
void Trigger_on_remove_pair_pred_wildcard(void);
void Trigger_on_remove_pair_wildcard(void);
void Trigger_wildcard_pair_w_pred_component(void);
void Trigger_wildcard_pair_w_obj_component(void);
void Trigger_on_add_remove(void);
void Trigger_on_set_component(void);
void Trigger_on_set_wildcard(void);
void Trigger_on_set_pair(void);
void Trigger_on_set_pair_w_obj_wildcard(void);
void Trigger_on_set_pair_pred_wildcard(void);
void Trigger_on_set_pair_wildcard(void);
void Trigger_on_set_component_after_modified(void);
void Trigger_un_set_component(void);
void Trigger_un_set_wildcard(void);
void Trigger_un_set_pair(void);
void Trigger_un_set_pair_w_obj_wildcard(void);
void Trigger_un_set_pair_pred_wildcard(void);
void Trigger_un_set_pair_wildcard(void);
void Trigger_on_add_not_tag(void);
void Trigger_on_remove_not_tag(void);
void Trigger_on_add_superset(void);
void Trigger_on_add_superset_2_levels(void);
void Trigger_on_remove_superset(void);
void Trigger_on_add_superset_childof(void);
void Trigger_on_remove_superset_childof(void);
void Trigger_on_add_self_superset(void);
void Trigger_on_remove_self_superset(void);
void Trigger_add_twice(void);
void Trigger_remove_twice(void);
void Trigger_on_remove_w_clear(void);
void Trigger_on_remove_w_delete(void);
void Trigger_on_remove_w_world_fini(void);
void Trigger_on_add_w_clone(void);
void Trigger_add_in_trigger(void);
void Trigger_remove_in_trigger(void);
void Trigger_clear_in_trigger(void);
void Trigger_delete_in_trigger(void);
void Trigger_trigger_w_named_entity(void);
void Trigger_on_remove_tree(void);
void Trigger_set_get_context(void);
void Trigger_set_get_binding_context(void);
void Trigger_trigger_w_self(void);
void Trigger_delete_trigger_w_delete_ctx(void);
void Trigger_trigger_w_index(void);
void Trigger_iter_type_set(void);
void Trigger_readonly_term(void);
void Trigger_trigger_on_prefab(void);
void Trigger_trigger_on_disabled(void);
void Trigger_trigger_on_prefab_tag(void);
void Trigger_trigger_on_disabled_tag(void);
void Trigger_trigger_cleanup_2_w_self_super_id(void);
void Trigger_on_add_yield_existing(void);
void Trigger_on_add_yield_existing_2_tables(void);
void Trigger_on_add_yield_existing_wildcard_pair(void);
void Trigger_on_set_yield_existing(void);
void Trigger_filter_term(void);
void Trigger_on_add_remove_after_exclusive_add(void);
void Trigger_on_add_base(void);
void Trigger_on_remove_base(void);
void Trigger_on_set_base(void);
void Trigger_on_unset_base(void);
void Trigger_on_add_base_superset_trigger(void);
void Trigger_on_add_base_superset_trigger_2_lvls(void);
void Trigger_on_add_base_2_entities(void);
void Trigger_on_add_base_2_entities_filter(void);
void Trigger_on_set_base_w_value_2_entities(void);
void Trigger_on_set_base_w_value_2_entities_instanced(void);
void Trigger_on_add_base_w_override(void);
void Trigger_on_set_base_w_override(void);
void Trigger_entity_source_1_trigger(void);
void Trigger_entity_source_2_triggers(void);
void Trigger_entity_source_base_set(void);
void Trigger_not_from_superset(void);
void Trigger_create_stresstest(void);
void Trigger_add_non_existing_entity(void);
void Trigger_on_add_self_trigger_with_add_isa(void);
void Trigger_on_set_self_trigger_with_add_isa(void);
void Trigger_notify_propagated_twice(void);
void Trigger_trigger_superset_wildcard(void);
void Trigger_remove_wildcard_1_id(void);
void Trigger_remove_wildcard_2_ids(void);

// Testsuite 'Observer'
void Observer_2_terms_w_on_add(void);
void Observer_2_terms_w_on_remove(void);
void Observer_2_terms_w_on_set_value(void);
void Observer_2_terms_w_on_remove_value(void);
void Observer_2_terms_w_on_add_2nd(void);
void Observer_2_terms_w_on_remove_2nd(void);
void Observer_2_pair_terms_w_on_add(void);
void Observer_2_pair_terms_w_on_remove(void);
void Observer_2_wildcard_pair_terms_w_on_add(void);
void Observer_2_wildcard_pair_terms_w_on_add_2_matching(void);
void Observer_2_wildcard_pair_terms_w_on_add_3_matching(void);
void Observer_2_wildcard_pair_terms_w_on_remove(void);
void Observer_on_set_n_entities(void);
void Observer_on_set_n_entities_2_comp(void);
void Observer_wildcard_pair_w_pred_component(void);
void Observer_wildcard_pair_w_obj_component(void);
void Observer_2_terms_1_not_w_on_add(void);
void Observer_2_terms_1_not_w_on_remove(void);
void Observer_2_terms_w_on_set(void);
void Observer_2_terms_w_un_set(void);
void Observer_3_terms_2_or_on_add(void);
void Observer_3_terms_2_or_on_remove(void);
void Observer_2_terms_w_from_entity_on_add(void);
void Observer_2_terms_on_remove_on_clear(void);
void Observer_2_terms_on_remove_on_delete(void);
void Observer_observer_w_self(void);
void Observer_add_after_delete_observer(void);
void Observer_remove_after_delete_observer(void);
void Observer_delete_observer_w_ctx(void);
void Observer_filter_w_strings(void);
void Observer_iter_type_set(void);
void Observer_readonly_term(void);
void Observer_trigger_on_prefab(void);
void Observer_trigger_on_disabled(void);
void Observer_unset_1_of_1(void);
void Observer_unset_1_of_2(void);
void Observer_unset_1_of_3(void);
void Observer_unset_on_delete_1(void);
void Observer_unset_on_delete_2(void);
void Observer_unset_on_delete_3(void);
void Observer_unset_on_fini_1(void);
void Observer_unset_on_fini_2(void);
void Observer_unset_on_fini_3(void);
void Observer_overlapping_unset_systems(void);
void Observer_unset_move_to_nonempty_table(void);
void Observer_write_in_unset(void);
void Observer_filter_term(void);
void Observer_2_terms_1_filter(void);
void Observer_3_terms_2_filter(void);
void Observer_and_from(void);
void Observer_or_from(void);
void Observer_notify_propagated_twice(void);
void Observer_on_add_yield_existing(void);
void Observer_on_add_yield_existing_2_tables(void);
void Observer_on_add_yield_existing_2_terms(void);
void Observer_observer_superset_wildcard(void);
void Observer_observer_superset_wildcard_add_isa(void);
void Observer_observer_superset_wildcard_add_isa_at_offset(void);

// Testsuite 'TriggerOnAdd'
void TriggerOnAdd_setup(void);
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
void TriggerOnAdd_emplace(void);
void TriggerOnAdd_add_after_delete_trigger(void);
void TriggerOnAdd_add_after_delete_wildcard_id_trigger(void);

// Testsuite 'TriggerOnRemove'
void TriggerOnRemove_remove(void);
void TriggerOnRemove_remove_no_match(void);
void TriggerOnRemove_delete(void);
void TriggerOnRemove_delete_no_match(void);
void TriggerOnRemove_remove_watched(void);
void TriggerOnRemove_delete_watched(void);
void TriggerOnRemove_valid_entity_after_delete(void);
void TriggerOnRemove_remove_after_delete_trigger(void);
void TriggerOnRemove_remove_after_delete_wildcard_id_trigger(void);
void TriggerOnRemove_has_removed_tag_trigger_1_tag(void);
void TriggerOnRemove_has_removed_tag_trigger_2_tags(void);

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
void TriggerOnSet_emplace(void);
void TriggerOnSet_un_set_tag_w_remove(void);
void TriggerOnSet_un_set_tag_w_clear(void);
void TriggerOnSet_un_set_tag_w_delete(void);

// Testsuite 'Monitor'
void Monitor_1_comp(void);
void Monitor_2_comps(void);
void Monitor_1_comp_1_not(void);
void Monitor_1_parent(void);
void Monitor_1_comp_1_parent(void);
void Monitor_1_comp_prefab_new(void);
void Monitor_1_comp_prefab_add(void);
void Monitor_monitor_w_and(void);
void Monitor_monitor_w_or(void);
void Monitor_monitor_w_not(void);
void Monitor_monitor_w_optional(void);
void Monitor_monitor_w_superset(void);
void Monitor_monitor_w_self_superset(void);
void Monitor_monitor_w_wildcard(void);

// Testsuite 'SystemOnSet'
void SystemOnSet_set_1_of_1(void);
void SystemOnSet_set_1_of_2(void);
void SystemOnSet_set_1_of_3(void);
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
void SystemOnSet_add_0_entity_in_on_set(void);
void SystemOnSet_on_set_prefab(void);

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
void SystemPeriodic_not_from_entity(void);
void SystemPeriodic_sys_context(void);
void SystemPeriodic_get_sys_context_from_param(void);
void SystemPeriodic_owned_only(void);
void SystemPeriodic_shared_only(void);
void SystemPeriodic_is_in_readonly(void);
void SystemPeriodic_get_period(void);
void SystemPeriodic_and_type(void);
void SystemPeriodic_or_type(void);

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
void Timer_rate_filter_with_empty_src(void);
void Timer_one_shot_timer_entity(void);
void Timer_interval_timer_entity(void);
void Timer_rate_entity(void);
void Timer_nested_rate_entity(void);
void Timer_nested_rate_entity_empty_src(void);
void Timer_naked_tick_entity(void);

// Testsuite 'SystemCascade'
void SystemCascade_cascade_depth_1(void);
void SystemCascade_cascade_depth_2(void);
void SystemCascade_cascade_depth_2_new_syntax(void);
void SystemCascade_add_after_match(void);
void SystemCascade_adopt_after_match(void);
void SystemCascade_rematch_w_empty_table(void);
void SystemCascade_query_w_only_cascade(void);
void SystemCascade_custom_relation_cascade_depth_1(void);
void SystemCascade_custom_relation_cascade_depth_2(void);
void SystemCascade_custom_relation_add_after_match(void);
void SystemCascade_custom_relation_adopt_after_match(void);
void SystemCascade_custom_relation_rematch_w_empty_table(void);

// Testsuite 'SystemManual'
void SystemManual_setup(void);
void SystemManual_1_type_1_component(void);
void SystemManual_activate_status(void);
void SystemManual_no_automerge(void);
void SystemManual_dont_run_w_unmatching_entity_query(void);

// Testsuite 'Tasks'
void Tasks_no_components(void);
void Tasks_one_tag(void);
void Tasks_from_system(void);
void Tasks_tasks_in_phases(void);

// Testsuite 'Prefab'
void Prefab_setup(void);
void Prefab_new_w_prefab(void);
void Prefab_new_w_count_prefab(void);
void Prefab_new_w_type_w_prefab(void);
void Prefab_add_prefab(void);
void Prefab_remove_prefab_after_new(void);
void Prefab_remove_prefab_after_add(void);
void Prefab_override_component(void);
void Prefab_override_remove_component(void);
void Prefab_override_2_of_3_components_1_self(void);
void Prefab_new_type_w_1_override(void);
void Prefab_new_type_w_2_overrides(void);
void Prefab_get_ptr_prefab(void);
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
void Prefab_prefab_tree_1_2_1(void);
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
void Prefab_add_to_empty_base_in_system(void);
void Prefab_dont_inherit_disabled(void);
void Prefab_clone_after_inherit_in_on_add(void);
void Prefab_override_from_nested(void);
void Prefab_create_multiple_nested_w_on_set(void);
void Prefab_create_multiple_nested_w_on_set_in_progress(void);
void Prefab_single_on_set_on_child_w_override(void);
void Prefab_force_owned(void);
void Prefab_force_owned_2(void);
void Prefab_force_owned_nested(void);
void Prefab_prefab_instanceof_hierarchy(void);
void Prefab_override_tag(void);
void Prefab_empty_prefab(void);
void Prefab_instanceof_0(void);
void Prefab_instantiate_empty_child_table(void);
void Prefab_instantiate_emptied_child_table(void);
void Prefab_override_2_prefabs(void);
void Prefab_rematch_after_add_instanceof_to_parent(void);
void Prefab_child_of_instance(void);
void Prefab_rematch_after_prefab_delete(void);
void Prefab_add_tag_w_low_id_to_instance(void);
void Prefab_get_type_after_base_add(void);
void Prefab_get_type_after_recycled_base_add(void);
void Prefab_new_w_recycled_base(void);
void Prefab_add_recycled_base(void);
void Prefab_remove_recycled_base(void);
void Prefab_get_from_recycled_base(void);
void Prefab_override_from_recycled_base(void);
void Prefab_remove_override_from_recycled_base(void);
void Prefab_instantiate_tree_from_recycled_base(void);
void Prefab_rematch_after_add_to_recycled_base(void);
void Prefab_get_tag_from_2nd_base(void);
void Prefab_get_component_from_2nd_base(void);
void Prefab_get_component_from_1st_base(void);
void Prefab_get_component_from_2nd_base_of_base(void);
void Prefab_get_component_from_1st_base_of_base(void);
void Prefab_get_component_from_2nd_base_prefab_base(void);
void Prefab_get_component_from_1st_base_prefab_base(void);
void Prefab_get_component_from_2nd_base_of_base_prefab_base(void);
void Prefab_get_component_from_1st_base_of_base_prefab_base(void);
void Prefab_fail_on_override_final(void);
void Prefab_instantiate_tree_once(void);
void Prefab_nested_prefab_w_named_children(void);
void Prefab_dont_copy_children_for_non_prefab_base(void);
void Prefab_get_component_pair_from_base(void);
void Prefab_get_component_pair_from_prefab_base(void);

// Testsuite 'System_w_FromParent'
void System_w_FromParent_setup(void);
void System_w_FromParent_1_column_from_container(void);
void System_w_FromParent_2_column_1_from_container(void);
void System_w_FromParent_3_column_2_from_container(void);
void System_w_FromParent_2_column_1_from_container_w_not(void);
void System_w_FromParent_2_column_1_from_container_w_not_prefab(void);
void System_w_FromParent_3_column_1_from_comtainer_1_from_container_w_not(void);
void System_w_FromParent_2_column_1_from_container_w_or(void);
void System_w_FromParent_select_same_from_container(void);
void System_w_FromParent_add_component_after_match(void);
void System_w_FromParent_add_component_after_match_and_rematch(void);
void System_w_FromParent_add_component_after_match_unmatch(void);
void System_w_FromParent_add_component_after_match_unmatch_match(void);
void System_w_FromParent_add_component_after_match_2_systems(void);
void System_w_FromParent_add_component_in_progress_after_match(void);
void System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr(void);
void System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr_in_progress(void);
void System_w_FromParent_adopt_after_match(void);
void System_w_FromParent_new_child_after_match(void);
void System_w_FromParent_realloc_after_match(void);

// Testsuite 'System_w_Empty'
void System_w_Empty_2_column_1_from_id(void);
void System_w_Empty_3_column_2_from_id(void);
void System_w_Empty_column_type(void);

// Testsuite 'System_w_FromSystem'
void System_w_FromSystem_2_column_1_from_system(void);
void System_w_FromSystem_3_column_2_from_system(void);
void System_w_FromSystem_auto_add_tag(void);

// Testsuite 'System_w_FromEntity'
void System_w_FromEntity_2_column_1_from_entity(void);
void System_w_FromEntity_task_from_entity(void);
void System_w_FromEntity_task_not_from_entity(void);

// Testsuite 'World'
void World_setup(void);
void World_progress_w_0(void);
void World_progress_w_t(void);
void World_get_tick(void);
void World_entity_range_offset(void);
void World_entity_range_offset_out_of_range(void);
void World_entity_range_limit_out_of_range(void);
void World_entity_range_add_existing_staged(void);
void World_entity_range_add_in_range_staged(void);
void World_entity_range_add_out_of_range_staged(void);
void World_entity_range_out_of_range_check_disabled(void);
void World_entity_range_check_after_delete(void);
void World_dim(void);
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
void World_system_time_scale(void);
void World_quit(void);
void World_get_delta_time(void);
void World_get_delta_time_auto(void);
void World_recreate_world(void);
void World_recreate_world_w_component(void);
void World_no_threading(void);
void World_no_time(void);
void World_is_entity_enabled(void);
void World_ensure_empty_root(void);
void World_register_alias_twice_same_entity(void);
void World_register_alias_twice_different_entity(void);
void World_redefine_component(void);

// Testsuite 'Stats'
void Stats_get_world_stats(void);
void Stats_get_pipeline_stats_before_progress_mini_world(void);
void Stats_get_pipeline_stats_before_progress(void);
void Stats_get_pipeline_stats_after_progress_no_systems(void);
void Stats_get_pipeline_stats_after_progress_1_system(void);
void Stats_get_pipeline_stats_after_progress_1_inactive_system(void);
void Stats_get_pipeline_stats_after_progress_2_systems(void);
void Stats_get_pipeline_stats_after_progress_2_systems_one_merge(void);

// Testsuite 'Type'
void Type_setup(void);
void Type_type_of_1_tostr(void);
void Type_type_of_2_tostr(void);
void Type_type_of_2_tostr_no_id(void);
void Type_type_redefine(void);
void Type_invalid_entity_type_expression(void);
void Type_invalid_container_type_expression(void);
void Type_invalid_system_type_expression(void);
void Type_get_type(void);
void Type_get_type_from_empty(void);
void Type_get_type_from_0(void);
void Type_type_to_expr_1_comp(void);
void Type_type_to_expr_2_comp(void);
void Type_type_to_expr_instanceof(void);
void Type_type_to_expr_childof(void);
void Type_type_to_expr_pair(void);
void Type_type_to_expr_pair_w_comp(void);
void Type_type_to_expr_scope(void);
void Type_entity_str(void);
void Type_entity_path_str(void);
void Type_entity_instanceof_str(void);
void Type_entity_childof_str(void);
void Type_entity_pair_str(void);
void Type_entity_switch_str(void);
void Type_entity_case_str(void);
void Type_entity_and_str(void);
void Type_entity_or_str(void);
void Type_entity_xor_str(void);
void Type_entity_not_str(void);
void Type_entity_str_small_buffer(void);
void Type_role_pair_str(void);
void Type_role_switch_str(void);
void Type_role_case_str(void);
void Type_role_and_str(void);
void Type_role_or_str(void);
void Type_role_xor_str(void);
void Type_role_not_str(void);
void Type_role_owned_str(void);
void Type_role_disabled_str(void);
void Type_large_type_expr(void);
void Type_large_type_expr_limit(void);

// Testsuite 'Run'
void Run_setup(void);
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
void Run_run_comb_10_entities_1_type(void);
void Run_run_comb_10_entities_2_types(void);
void Run_run_w_interrupt(void);
void Run_run_staging(void);

// Testsuite 'MultiThread'
void MultiThread_setup(void);
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
void MultiThread_2_thread_1_entity_instanced(void);
void MultiThread_2_thread_5_entity_instanced(void);
void MultiThread_2_thread_10_entity_instanced(void);
void MultiThread_2_thread_test_combs_100_entity_w_next_worker(void);
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
void MultiThread_fini_after_set_threads(void);
void MultiThread_2_threads_single_threaded_system(void);

// Testsuite 'DeferredActions'
void DeferredActions_defer_new(void);
void DeferredActions_defer_bulk_new(void);
void DeferredActions_defer_add(void);
void DeferredActions_defer_add_two(void);
void DeferredActions_defer_remove(void);
void DeferredActions_defer_remove_two(void);
void DeferredActions_defer_set(void);
void DeferredActions_defer_delete(void);
void DeferredActions_defer_twice(void);
void DeferredActions_defer_twice_in_progress(void);
void DeferredActions_run_w_defer(void);
void DeferredActions_system_in_progress_w_defer(void);
void DeferredActions_defer_get_mut_no_modify(void);
void DeferredActions_defer_get_mut_w_modify(void);
void DeferredActions_defer_modify(void);
void DeferredActions_defer_set_pair(void);
void DeferredActions_defer_clear(void);
void DeferredActions_defer_add_after_delete(void);
void DeferredActions_defer_set_after_delete(void);
void DeferredActions_defer_get_mut_after_delete(void);
void DeferredActions_defer_get_mut_after_delete_2nd_to_last(void);
void DeferredActions_defer_add_child_to_deleted_parent(void);
void DeferredActions_recreate_deleted_entity_while_deferred(void);
void DeferredActions_defer_add_to_recycled_id(void);
void DeferredActions_defer_add_to_recycled_id_w_role(void);
void DeferredActions_defer_add_to_recycled_relation(void);
void DeferredActions_defer_add_to_recycled_object(void);
void DeferredActions_defer_add_to_recycled_object_childof(void);
void DeferredActions_defer_add_to_deleted_id(void);
void DeferredActions_defer_add_to_deleted_id_w_role(void);
void DeferredActions_defer_add_to_deleted_relation(void);
void DeferredActions_defer_add_to_deleted_object(void);
void DeferredActions_defer_add_to_deleted_object_childof(void);
void DeferredActions_defer_delete_added_id(void);
void DeferredActions_defer_delete_added_id_w_role(void);
void DeferredActions_defer_delete_added_relation(void);
void DeferredActions_defer_delete_added_object(void);
void DeferredActions_defer_delete_added_object_childof(void);
void DeferredActions_discard_add(void);
void DeferredActions_discard_remove(void);
void DeferredActions_discard_add_two(void);
void DeferredActions_discard_remove_two(void);
void DeferredActions_discard_child(void);
void DeferredActions_discard_child_w_add(void);
void DeferredActions_defer_return_value(void);
void DeferredActions_defer_get_mut_pair(void);
void DeferredActions_async_stage_add(void);
void DeferredActions_async_stage_add_twice(void);
void DeferredActions_async_stage_remove(void);
void DeferredActions_async_stage_clear(void);
void DeferredActions_async_stage_delete(void);
void DeferredActions_async_stage_new(void);
void DeferredActions_async_stage_no_get(void);
void DeferredActions_async_stage_readonly(void);
void DeferredActions_async_stage_is_async(void);
void DeferredActions_register_component_while_in_progress(void);
void DeferredActions_register_component_while_staged(void);
void DeferredActions_register_component_while_deferred(void);
void DeferredActions_defer_enable(void);
void DeferredActions_defer_disable(void);
void DeferredActions_defer_delete_with(void);
void DeferredActions_defer_remove_all(void);

// Testsuite 'SingleThreadStaging'
void SingleThreadStaging_setup(void);
void SingleThreadStaging_new_empty(void);
void SingleThreadStaging_new_w_component(void);
void SingleThreadStaging_new_empty_w_count(void);
void SingleThreadStaging_new_component_w_count(void);
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
void SingleThreadStaging_clear_stage_after_merge(void);
void SingleThreadStaging_get_mutable(void);
void SingleThreadStaging_get_mutable_from_main(void);
void SingleThreadStaging_get_mutable_w_add(void);
void SingleThreadStaging_on_add_after_new_type_in_progress(void);
void SingleThreadStaging_lock_table(void);
void SingleThreadStaging_recursive_lock_table(void);
void SingleThreadStaging_modify_after_lock(void);
void SingleThreadStaging_get_empty_case_from_stage(void);
void SingleThreadStaging_get_case_from_stage(void);
void SingleThreadStaging_get_object_from_stage(void);

// Testsuite 'MultiThreadStaging'
void MultiThreadStaging_setup(void);
void MultiThreadStaging_2_threads_add_to_current(void);
void MultiThreadStaging_3_threads_add_to_current(void);
void MultiThreadStaging_4_threads_add_to_current(void);
void MultiThreadStaging_5_threads_add_to_current(void);
void MultiThreadStaging_6_threads_add_to_current(void);
void MultiThreadStaging_2_threads_on_add(void);
void MultiThreadStaging_new_w_count(void);
void MultiThreadStaging_custom_thread_auto_merge(void);
void MultiThreadStaging_custom_thread_manual_merge(void);
void MultiThreadStaging_custom_thread_partial_manual_merge(void);

// Testsuite 'Stresstests'
void Stresstests_setup(void);
void Stresstests_create_1m_set_two_components(void);
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
void Stresstests_create_1m_set_two_components(void);

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
void Snapshot_snapshot_free_filtered_w_dtor(void);
void Snapshot_snapshot_activate_table_w_filter(void);
void Snapshot_snapshot_copy(void);
void Snapshot_snapshot_get_ref_after_restore(void);
void Snapshot_new_after_snapshot(void);
void Snapshot_new_empty_after_snapshot(void);
void Snapshot_add_after_snapshot(void);
void Snapshot_delete_after_snapshot(void);
void Snapshot_set_after_snapshot(void);
void Snapshot_restore_recycled(void);
void Snapshot_snapshot_w_new_in_onset(void);
void Snapshot_snapshot_w_new_in_onset_in_snapshot_table(void);
void Snapshot_snapshot_from_stage(void);

// Testsuite 'Modules'
void Modules_setup(void);
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
void Modules_name_prefix_prefab(void);
void Modules_name_prefix_pipeline(void);
void Modules_name_prefix_trigger(void);
void Modules_name_prefix_underscore(void);
void Modules_lookup_by_symbol(void);
void Modules_nested_module(void);
void Modules_module_tag_on_namespace(void);
void Modules_module_tag_on_namespace_on_add(void);
void Modules_module_tag_on_namespace_on_add_2_levels(void);

// Testsuite 'MixinIterable'
void MixinIterable_iter_query(void);
void MixinIterable_iter_query_w_filter(void);
void MixinIterable_iter_world(void);
void MixinIterable_iter_world_w_filter(void);
void MixinIterable_iter_rule(void);
void MixinIterable_iter_rule_w_filter(void);
void MixinIterable_iter_filter(void);
void MixinIterable_iter_filter_w_filter(void);

// Testsuite 'Internals'
void Internals_setup(void);
void Internals_deactivate_table(void);
void Internals_activate_table(void);
void Internals_activate_deactivate_table(void);
void Internals_activate_deactivate_reactive(void);
void Internals_activate_deactivate_activate_other(void);
void Internals_no_double_system_table_after_merge(void);
void Internals_recreate_deleted_table(void);
void Internals_create_65k_tables(void);
void Internals_no_duplicate_root_table_id(void);
void Internals_override_os_api_w_addon(void);

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
void Error_last_error(void);

// Testsuite 'App'
void App_app_w_frame_action(void);

bake_test_case Entity_testcases[] = {
    {
        "init_id",
        Entity_init_id
    },
    {
        "init_id_name",
        Entity_init_id_name
    },
    {
        "init_id_path",
        Entity_init_id_path
    },
    {
        "init_id_add_1_comp",
        Entity_init_id_add_1_comp
    },
    {
        "init_id_add_2_comp",
        Entity_init_id_add_2_comp
    },
    {
        "init_id_w_scope",
        Entity_init_id_w_scope
    },
    {
        "init_id_name_w_scope",
        Entity_init_id_name_w_scope
    },
    {
        "init_id_path_w_scope",
        Entity_init_id_path_w_scope
    },
    {
        "init_id_name_1_comp",
        Entity_init_id_name_1_comp
    },
    {
        "init_id_name_2_comp",
        Entity_init_id_name_2_comp
    },
    {
        "init_id_name_2_comp_w_scope",
        Entity_init_id_name_2_comp_w_scope
    },
    {
        "id_add_1_comp",
        Entity_id_add_1_comp
    },
    {
        "id_add_2_comp",
        Entity_id_add_2_comp
    },
    {
        "init_id_path_w_sep",
        Entity_init_id_path_w_sep
    },
    {
        "find_id_name",
        Entity_find_id_name
    },
    {
        "find_w_existing_id_name",
        Entity_find_w_existing_id_name
    },
    {
        "find_id_name_w_scope",
        Entity_find_id_name_w_scope
    },
    {
        "find_id_path",
        Entity_find_id_path
    },
    {
        "find_id_path_w_scope",
        Entity_find_id_path_w_scope
    },
    {
        "find_id_name_match",
        Entity_find_id_name_match
    },
    {
        "find_id_name_match_w_scope",
        Entity_find_id_name_match_w_scope
    },
    {
        "find_id_path_match",
        Entity_find_id_path_match
    },
    {
        "find_id_path_match_w_scope",
        Entity_find_id_path_match_w_scope
    },
    {
        "find_id_name_mismatch",
        Entity_find_id_name_mismatch
    },
    {
        "find_id_name_mismatch_w_scope",
        Entity_find_id_name_mismatch_w_scope
    },
    {
        "find_id_path_mismatch",
        Entity_find_id_path_mismatch
    },
    {
        "find_id_path_mismatch_w_scope",
        Entity_find_id_path_mismatch_w_scope
    },
    {
        "find_id_add_1_comp",
        Entity_find_id_add_1_comp
    },
    {
        "find_id_add_2_comp",
        Entity_find_id_add_2_comp
    },
    {
        "init_w_scope_name",
        Entity_init_w_scope_name
    },
    {
        "init_w_core_name",
        Entity_init_w_core_name
    },
    {
        "init_w_with",
        Entity_init_w_with
    },
    {
        "init_w_with_w_name",
        Entity_init_w_with_w_name
    },
    {
        "init_w_with_w_scope",
        Entity_init_w_with_w_scope
    },
    {
        "init_w_with_w_name_scope",
        Entity_init_w_with_w_name_scope
    },
    {
        "is_valid",
        Entity_is_valid
    },
    {
        "is_recycled_valid",
        Entity_is_recycled_valid
    },
    {
        "is_0_valid",
        Entity_is_0_valid
    },
    {
        "is_junk_valid",
        Entity_is_junk_valid
    },
    {
        "is_not_alive_valid",
        Entity_is_not_alive_valid
    },
    {
        "is_nonzero_gen_valid",
        Entity_is_nonzero_gen_valid
    },
    {
        "init_w_name_deferred",
        Entity_init_w_name_deferred
    },
    {
        "init_w_name_staged",
        Entity_init_w_name_staged
    },
    {
        "record_find_for_empty",
        Entity_record_find_for_empty
    },
    {
        "record_find",
        Entity_record_find
    },
    {
        "record_find_from_stage",
        Entity_record_find_from_stage
    },
    {
        "ensure_zero_gen",
        Entity_ensure_zero_gen
    },
    {
        "ensure_nonzero_gen",
        Entity_ensure_nonzero_gen
    },
    {
        "ensure_zero_gen_exists",
        Entity_ensure_zero_gen_exists
    },
    {
        "ensure_nonzero_gen_exists",
        Entity_ensure_nonzero_gen_exists
    },
    {
        "ensure_zero_gen_exists_alive",
        Entity_ensure_zero_gen_exists_alive
    },
    {
        "ensure_nonzero_gen_exists_alive",
        Entity_ensure_nonzero_gen_exists_alive
    },
    {
        "set_scope_w_entity_init_from_stage",
        Entity_set_scope_w_entity_init_from_stage
    },
    {
        "entity_init_w_childof_and_scope",
        Entity_entity_init_w_childof_and_scope
    },
    {
        "entity_init_w_childof_and_scope_and_scoped_name",
        Entity_entity_init_w_childof_and_scope_and_scoped_name
    },
    {
        "deferred_entity_init_w_childof_and_scope",
        Entity_deferred_entity_init_w_childof_and_scope
    },
    {
        "deferred_entity_init_w_childof_and_scope_and_scoped_name",
        Entity_deferred_entity_init_w_childof_and_scope_and_scoped_name
    },
    {
        "deferred_entity_init_w_name_twice",
        Entity_deferred_entity_init_w_name_twice
    }
};

bake_test_case Event_testcases[] = {
    {
        "table_1_id_w_trigger",
        Event_table_1_id_w_trigger
    },
    {
        "table_2_ids_w_trigger",
        Event_table_2_ids_w_trigger
    },
    {
        "table_1_id_w_observer",
        Event_table_1_id_w_observer
    },
    {
        "table_2_ids_w_observer",
        Event_table_2_ids_w_observer
    },
    {
        "emit_event_for_empty_table",
        Event_emit_event_for_empty_table
    },
    {
        "emit_event_switch_id",
        Event_emit_event_switch_id
    },
    {
        "emit_event_case_for_switch_id",
        Event_emit_event_case_for_switch_id
    },
    {
        "emit_event_case_for_case_id",
        Event_emit_event_case_for_case_id
    },
    {
        "emit_event_case_for_case_id_wildcard",
        Event_emit_event_case_for_case_id_wildcard
    },
    {
        "emit_event_switch_for_case_id",
        Event_emit_event_switch_for_case_id
    },
    {
        "emit_table_event",
        Event_emit_table_event
    },
    {
        "emit_staged",
        Event_emit_staged
    }
};

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
        "tag",
        New_tag
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
    },
    {
        "new_component_id_skip_used",
        New_new_component_id_skip_used
    },
    {
        "new_component_id_skip_to_hi_id",
        New_new_component_id_skip_to_hi_id
    },
    {
        "new_w_entity_0",
        New_new_w_entity_0
    },
    {
        "create_w_explicit_id_2_worlds",
        New_create_w_explicit_id_2_worlds
    },
    {
        "new_w_id_0_w_with",
        New_new_w_id_0_w_with
    },
    {
        "new_w_id_w_with",
        New_new_w_id_w_with
    },
    {
        "new_w_type_0_w_with",
        New_new_w_type_0_w_with
    },
    {
        "new_w_type_w_with",
        New_new_w_type_w_with
    },
    {
        "new_w_id_w_with_w_scope",
        New_new_w_id_w_with_w_scope
    },
    {
        "new_w_type_w_with_w_scope",
        New_new_w_type_w_with_w_scope
    },
    {
        "new_w_id_w_with_defer",
        New_new_w_id_w_with_defer
    },
    {
        "new_w_id_w_with_defer_w_scope",
        New_new_w_id_w_with_defer_w_scope
    },
    {
        "new_w_type_w_with_defer",
        New_new_w_type_w_with_defer
    },
    {
        "new_w_type_w_with_defer_w_scope",
        New_new_w_type_w_with_defer_w_scope
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
        "tag",
        New_w_Count_tag
    },
    {
        "bulk_init_empty",
        New_w_Count_bulk_init_empty
    },
    {
        "bulk_init_empty_w_entities",
        New_w_Count_bulk_init_empty_w_entities
    },
    {
        "bulk_init_1_tag",
        New_w_Count_bulk_init_1_tag
    },
    {
        "bulk_init_1_tag_w_entities",
        New_w_Count_bulk_init_1_tag_w_entities
    },
    {
        "bulk_init_2_tags",
        New_w_Count_bulk_init_2_tags
    },
    {
        "bulk_init_1_component",
        New_w_Count_bulk_init_1_component
    },
    {
        "bulk_init_2_components",
        New_w_Count_bulk_init_2_components
    },
    {
        "bulk_init_1_component_w_value",
        New_w_Count_bulk_init_1_component_w_value
    },
    {
        "bulk_init_2_components_w_value",
        New_w_Count_bulk_init_2_components_w_value
    },
    {
        "bulk_init_2_components_tag_w_value",
        New_w_Count_bulk_init_2_components_tag_w_value
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
        "tag",
        Add_tag
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
    }
};

bake_test_case Switch_testcases[] = {
    {
        "get_case_empty",
        Switch_get_case_empty
    },
    {
        "get_case_no_switch",
        Switch_get_case_no_switch
    },
    {
        "get_case_unset",
        Switch_get_case_unset
    },
    {
        "get_case_set",
        Switch_get_case_set
    },
    {
        "get_case_change",
        Switch_get_case_change
    },
    {
        "remove_case",
        Switch_remove_case
    },
    {
        "remove_last",
        Switch_remove_last
    },
    {
        "delete_first",
        Switch_delete_first
    },
    {
        "delete_last",
        Switch_delete_last
    },
    {
        "delete_first_last",
        Switch_delete_first_last
    },
    {
        "3_entities_same_case",
        Switch_3_entities_same_case
    },
    {
        "2_entities_1_change_case",
        Switch_2_entities_1_change_case
    },
    {
        "3_entities_change_case",
        Switch_3_entities_change_case
    },
    {
        "query_switch",
        Switch_query_switch
    },
    {
        "query_1_case_1_type",
        Switch_query_1_case_1_type
    },
    {
        "query_1_case_2_types",
        Switch_query_1_case_2_types
    },
    {
        "query_2_cases_1_type",
        Switch_query_2_cases_1_type
    },
    {
        "query_2_cases_2_types",
        Switch_query_2_cases_2_types
    },
    {
        "query_after_remove",
        Switch_query_after_remove
    },
    {
        "add_switch_in_stage",
        Switch_add_switch_in_stage
    },
    {
        "add_case_in_stage",
        Switch_add_case_in_stage
    },
    {
        "change_case_in_stage",
        Switch_change_case_in_stage
    },
    {
        "change_one_case_in_stage",
        Switch_change_one_case_in_stage
    },
    {
        "remove_switch_in_stage",
        Switch_remove_switch_in_stage
    },
    {
        "switch_no_match_for_case",
        Switch_switch_no_match_for_case
    },
    {
        "empty_entity_has_case",
        Switch_empty_entity_has_case
    },
    {
        "zero_entity_has_case",
        Switch_zero_entity_has_case
    },
    {
        "add_to_entity_w_switch",
        Switch_add_to_entity_w_switch
    },
    {
        "add_pair_to_entity_w_switch",
        Switch_add_pair_to_entity_w_switch
    },
    {
        "sort",
        Switch_sort
    },
    {
        "recycled_tags",
        Switch_recycled_tags
    },
    {
        "query_recycled_tags",
        Switch_query_recycled_tags
    },
    {
        "single_case",
        Switch_single_case
    },
    {
        "match_switch_on_base_instance",
        Switch_match_switch_on_base_instance
    },
    {
        "switch_w_bitset_query",
        Switch_switch_w_bitset_query
    },
    {
        "switch_w_bitset_query_inv",
        Switch_switch_w_bitset_query_inv
    },
    {
        "switch_w_bitset_query_2_elems",
        Switch_switch_w_bitset_query_2_elems
    },
    {
        "switch_w_bitset_query_2_elems_skip",
        Switch_switch_w_bitset_query_2_elems_skip
    },
    {
        "switch_w_bitset_query_elems_interleaved",
        Switch_switch_w_bitset_query_elems_interleaved
    },
    {
        "switch_w_bitset_query_elems_interleaved_2_types",
        Switch_switch_w_bitset_query_elems_interleaved_2_types
    }
};

bake_test_case EnabledComponents_testcases[] = {
    {
        "is_component_enabled",
        EnabledComponents_is_component_enabled
    },
    {
        "is_empty_entity_disabled",
        EnabledComponents_is_empty_entity_disabled
    },
    {
        "is_0_entity_disabled",
        EnabledComponents_is_0_entity_disabled
    },
    {
        "is_0_component_disabled",
        EnabledComponents_is_0_component_disabled
    },
    {
        "is_nonexist_component_disabled",
        EnabledComponents_is_nonexist_component_disabled
    },
    {
        "is_enabled_component_enabled",
        EnabledComponents_is_enabled_component_enabled
    },
    {
        "is_disabled_component_enabled",
        EnabledComponents_is_disabled_component_enabled
    },
    {
        "has_enabled_component",
        EnabledComponents_has_enabled_component
    },
    {
        "is_enabled_after_add",
        EnabledComponents_is_enabled_after_add
    },
    {
        "is_enabled_after_remove",
        EnabledComponents_is_enabled_after_remove
    },
    {
        "is_enabled_after_disable",
        EnabledComponents_is_enabled_after_disable
    },
    {
        "is_disabled_after_enable",
        EnabledComponents_is_disabled_after_enable
    },
    {
        "is_enabled_randomized",
        EnabledComponents_is_enabled_randomized
    },
    {
        "is_enabled_after_add_randomized",
        EnabledComponents_is_enabled_after_add_randomized
    },
    {
        "is_enabled_after_randomized_add_randomized",
        EnabledComponents_is_enabled_after_randomized_add_randomized
    },
    {
        "is_enabled_2",
        EnabledComponents_is_enabled_2
    },
    {
        "is_enabled_3",
        EnabledComponents_is_enabled_3
    },
    {
        "is_enabled_2_after_add",
        EnabledComponents_is_enabled_2_after_add
    },
    {
        "is_enabled_3_after_add",
        EnabledComponents_is_enabled_3_after_add
    },
    {
        "query_disabled",
        EnabledComponents_query_disabled
    },
    {
        "query_disabled_skip_initial",
        EnabledComponents_query_disabled_skip_initial
    },
    {
        "query_mod_2",
        EnabledComponents_query_mod_2
    },
    {
        "query_mod_8",
        EnabledComponents_query_mod_8
    },
    {
        "query_mod_64",
        EnabledComponents_query_mod_64
    },
    {
        "query_mod_256",
        EnabledComponents_query_mod_256
    },
    {
        "query_mod_1024",
        EnabledComponents_query_mod_1024
    },
    {
        "query_enable_mod_10",
        EnabledComponents_query_enable_mod_10
    },
    {
        "query_mod_2_2_bitsets",
        EnabledComponents_query_mod_2_2_bitsets
    },
    {
        "query_mod_8_2_bitsets",
        EnabledComponents_query_mod_8_2_bitsets
    },
    {
        "query_mod_64_2_bitsets",
        EnabledComponents_query_mod_64_2_bitsets
    },
    {
        "query_mod_256_2_bitsets",
        EnabledComponents_query_mod_256_2_bitsets
    },
    {
        "query_mod_1024_2_bitsets",
        EnabledComponents_query_mod_1024_2_bitsets
    },
    {
        "query_randomized_2_bitsets",
        EnabledComponents_query_randomized_2_bitsets
    },
    {
        "query_randomized_3_bitsets",
        EnabledComponents_query_randomized_3_bitsets
    },
    {
        "query_randomized_4_bitsets",
        EnabledComponents_query_randomized_4_bitsets
    },
    {
        "defer_enable",
        EnabledComponents_defer_enable
    },
    {
        "sort",
        EnabledComponents_sort
    }
};

bake_test_case Remove_testcases[] = {
    {
        "zero",
        Remove_zero
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
        "2_again",
        Remove_2_again
    },
    {
        "2_overlap",
        Remove_2_overlap
    },
    {
        "1_from_empty",
        Remove_1_from_empty
    },
    {
        "not_added",
        Remove_not_added
    }
};

bake_test_case Parser_testcases[] = {
    {
        "resolve_this",
        Parser_resolve_this
    },
    {
        "resolve_wildcard",
        Parser_resolve_wildcard
    },
    {
        "resolve_any",
        Parser_resolve_any
    },
    {
        "resolve_is_a",
        Parser_resolve_is_a
    },
    {
        "0",
        Parser_0
    },
    {
        "component_implicit_subject",
        Parser_component_implicit_subject
    },
    {
        "component_explicit_subject",
        Parser_component_explicit_subject
    },
    {
        "component_explicit_subject_this",
        Parser_component_explicit_subject_this
    },
    {
        "component_explicit_subject_this_by_name",
        Parser_component_explicit_subject_this_by_name
    },
    {
        "component_explicit_subject_wildcard",
        Parser_component_explicit_subject_wildcard
    },
    {
        "component_explicit_subject_any",
        Parser_component_explicit_subject_any
    },
    {
        "this_as_predicate",
        Parser_this_as_predicate
    },
    {
        "this_as_object",
        Parser_this_as_object
    },
    {
        "pair_implicit_subject",
        Parser_pair_implicit_subject
    },
    {
        "pair_implicit_subject_wildcard_pred",
        Parser_pair_implicit_subject_wildcard_pred
    },
    {
        "pair_implicit_subject_wildcard_obj",
        Parser_pair_implicit_subject_wildcard_obj
    },
    {
        "pair_implicit_subject_any_pred",
        Parser_pair_implicit_subject_any_pred
    },
    {
        "pair_implicit_subject_any_obj",
        Parser_pair_implicit_subject_any_obj
    },
    {
        "pair_implicit_subject_this_pred",
        Parser_pair_implicit_subject_this_pred
    },
    {
        "pair_implicit_subject_this_obj",
        Parser_pair_implicit_subject_this_obj
    },
    {
        "pair_explicit_subject",
        Parser_pair_explicit_subject
    },
    {
        "pair_explicit_subject_this",
        Parser_pair_explicit_subject_this
    },
    {
        "pair_explicit_subject_this_by_name",
        Parser_pair_explicit_subject_this_by_name
    },
    {
        "pair_explicit_subject_wildcard_pred",
        Parser_pair_explicit_subject_wildcard_pred
    },
    {
        "pair_explicit_subject_wildcard_subj",
        Parser_pair_explicit_subject_wildcard_subj
    },
    {
        "pair_explicit_subject_wildcard_obj",
        Parser_pair_explicit_subject_wildcard_obj
    },
    {
        "pair_implicit_subject_0_object",
        Parser_pair_implicit_subject_0_object
    },
    {
        "pair_explicit_subject_0_object",
        Parser_pair_explicit_subject_0_object
    },
    {
        "in_component_implicit_subject",
        Parser_in_component_implicit_subject
    },
    {
        "in_component_explicit_subject",
        Parser_in_component_explicit_subject
    },
    {
        "in_pair_implicit_subject",
        Parser_in_pair_implicit_subject
    },
    {
        "in_pair_explicit_subject",
        Parser_in_pair_explicit_subject
    },
    {
        "inout_component_implicit_subject",
        Parser_inout_component_implicit_subject
    },
    {
        "inout_component_explicit_subject",
        Parser_inout_component_explicit_subject
    },
    {
        "inout_pair_implicit_subject",
        Parser_inout_pair_implicit_subject
    },
    {
        "inout_pair_explicit_subject",
        Parser_inout_pair_explicit_subject
    },
    {
        "out_component_implicit_subject",
        Parser_out_component_implicit_subject
    },
    {
        "out_component_explicit_subject",
        Parser_out_component_explicit_subject
    },
    {
        "out_pair_implicit_subject",
        Parser_out_pair_implicit_subject
    },
    {
        "out_pair_explicit_subject",
        Parser_out_pair_explicit_subject
    },
    {
        "inout_filter_component",
        Parser_inout_filter_component
    },
    {
        "component_singleton",
        Parser_component_singleton
    },
    {
        "this_singleton",
        Parser_this_singleton
    },
    {
        "component_implicit_no_subject",
        Parser_component_implicit_no_subject
    },
    {
        "component_explicit_no_subject",
        Parser_component_explicit_no_subject
    },
    {
        "pair_no_subject",
        Parser_pair_no_subject
    },
    {
        "variable_single_char",
        Parser_variable_single_char
    },
    {
        "variable_multi_char",
        Parser_variable_multi_char
    },
    {
        "variable_multi_char_w_underscore",
        Parser_variable_multi_char_w_underscore
    },
    {
        "variable_multi_char_w_number",
        Parser_variable_multi_char_w_number
    },
    {
        "variable_multi_char_not_allcaps",
        Parser_variable_multi_char_not_allcaps
    },
    {
        "escaped_all_caps_single_char",
        Parser_escaped_all_caps_single_char
    },
    {
        "escaped_all_caps_multi_char",
        Parser_escaped_all_caps_multi_char
    },
    {
        "component_not",
        Parser_component_not
    },
    {
        "pair_implicit_subject_not",
        Parser_pair_implicit_subject_not
    },
    {
        "pair_explicit_subject_not",
        Parser_pair_explicit_subject_not
    },
    {
        "2_component_not",
        Parser_2_component_not
    },
    {
        "2_component_not_no_space",
        Parser_2_component_not_no_space
    },
    {
        "component_optional",
        Parser_component_optional
    },
    {
        "2_component_optional",
        Parser_2_component_optional
    },
    {
        "2_component_optional_no_space",
        Parser_2_component_optional_no_space
    },
    {
        "from_and",
        Parser_from_and
    },
    {
        "from_or",
        Parser_from_or
    },
    {
        "from_not",
        Parser_from_not
    },
    {
        "pair_implicit_subject_optional",
        Parser_pair_implicit_subject_optional
    },
    {
        "pair_explicit_subject_optional",
        Parser_pair_explicit_subject_optional
    },
    {
        "pred_implicit_subject_w_role",
        Parser_pred_implicit_subject_w_role
    },
    {
        "pred_explicit_subject_w_role",
        Parser_pred_explicit_subject_w_role
    },
    {
        "pred_no_subject_w_role",
        Parser_pred_no_subject_w_role
    },
    {
        "pair_implicit_subject_w_role",
        Parser_pair_implicit_subject_w_role
    },
    {
        "pair_explicit_subject_w_role",
        Parser_pair_explicit_subject_w_role
    },
    {
        "inout_role_pred_implicit_subject",
        Parser_inout_role_pred_implicit_subject
    },
    {
        "inout_role_pred_no_subject",
        Parser_inout_role_pred_no_subject
    },
    {
        "inout_role_pred_explicit_subject",
        Parser_inout_role_pred_explicit_subject
    },
    {
        "inout_role_pair_implicit_subject",
        Parser_inout_role_pair_implicit_subject
    },
    {
        "inout_role_pair_explicit_subject",
        Parser_inout_role_pair_explicit_subject
    },
    {
        "2_pred_implicit_subject",
        Parser_2_pred_implicit_subject
    },
    {
        "2_pred_no_subject",
        Parser_2_pred_no_subject
    },
    {
        "2_pred_explicit_subject",
        Parser_2_pred_explicit_subject
    },
    {
        "2_pair_implicit_subject",
        Parser_2_pair_implicit_subject
    },
    {
        "2_pair_explicit_subject",
        Parser_2_pair_explicit_subject
    },
    {
        "2_pred_role",
        Parser_2_pred_role
    },
    {
        "2_pair_implicit_subj_role",
        Parser_2_pair_implicit_subj_role
    },
    {
        "2_pair_explicit_subj_role",
        Parser_2_pair_explicit_subj_role
    },
    {
        "2_or_pred_implicit_subj",
        Parser_2_or_pred_implicit_subj
    },
    {
        "2_or_pred_explicit_subj",
        Parser_2_or_pred_explicit_subj
    },
    {
        "2_or_pair_implicit_subj",
        Parser_2_or_pair_implicit_subj
    },
    {
        "2_or_pair_explicit_subj",
        Parser_2_or_pair_explicit_subj
    },
    {
        "2_or_pred_inout",
        Parser_2_or_pred_inout
    },
    {
        "1_digit_pred_implicit_subj",
        Parser_1_digit_pred_implicit_subj
    },
    {
        "1_digit_pred_no_subj",
        Parser_1_digit_pred_no_subj
    },
    {
        "1_digit_pred_explicit_subj",
        Parser_1_digit_pred_explicit_subj
    },
    {
        "1_digit_pair_implicit_subj",
        Parser_1_digit_pair_implicit_subj
    },
    {
        "1_digit_pair_explicit_subj",
        Parser_1_digit_pair_explicit_subj
    },
    {
        "pred_implicit_subject_self",
        Parser_pred_implicit_subject_self
    },
    {
        "pred_implicit_subject_superset",
        Parser_pred_implicit_subject_superset
    },
    {
        "pred_implicit_subject_subset",
        Parser_pred_implicit_subject_subset
    },
    {
        "pred_implicit_subject_superset_inclusive",
        Parser_pred_implicit_subject_superset_inclusive
    },
    {
        "pred_implicit_subject_subset_inclusive",
        Parser_pred_implicit_subject_subset_inclusive
    },
    {
        "pred_implicit_subject_superset_cascade",
        Parser_pred_implicit_subject_superset_cascade
    },
    {
        "pred_implicit_subject_subset_cascade",
        Parser_pred_implicit_subject_subset_cascade
    },
    {
        "pred_implicit_subject_superset_inclusive_cascade",
        Parser_pred_implicit_subject_superset_inclusive_cascade
    },
    {
        "pred_implicit_subject_subset_inclusive_cascade",
        Parser_pred_implicit_subject_subset_inclusive_cascade
    },
    {
        "pred_implicit_subject_implicit_superset_cascade",
        Parser_pred_implicit_subject_implicit_superset_cascade
    },
    {
        "pred_implicit_subject_implicit_superset_inclusive_cascade",
        Parser_pred_implicit_subject_implicit_superset_inclusive_cascade
    },
    {
        "pred_implicit_subject_implicit_superset_cascade_w_rel",
        Parser_pred_implicit_subject_implicit_superset_cascade_w_rel
    },
    {
        "pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel",
        Parser_pred_implicit_subject_implicit_superset_inclusive_cascade_w_rel
    },
    {
        "pred_implicit_subject_superset_depth_1_digit",
        Parser_pred_implicit_subject_superset_depth_1_digit
    },
    {
        "pred_implicit_subject_subset_depth_1_digit",
        Parser_pred_implicit_subject_subset_depth_1_digit
    },
    {
        "pred_implicit_subject_superset_depth_2_digits",
        Parser_pred_implicit_subject_superset_depth_2_digits
    },
    {
        "pred_implicit_subject_subset_depth_2_digits",
        Parser_pred_implicit_subject_subset_depth_2_digits
    },
    {
        "pred_implicit_superset_min_max_depth",
        Parser_pred_implicit_superset_min_max_depth
    },
    {
        "pred_implicit_superset_childof_min_max_depth",
        Parser_pred_implicit_superset_childof_min_max_depth
    },
    {
        "pred_implicit_subject_superset_childof",
        Parser_pred_implicit_subject_superset_childof
    },
    {
        "pred_implicit_subject_cascade_superset_childof",
        Parser_pred_implicit_subject_cascade_superset_childof
    },
    {
        "pred_implicit_subject_superset_cascade_childof",
        Parser_pred_implicit_subject_superset_cascade_childof
    },
    {
        "pred_implicit_subject_superset_cascade_childof_optional",
        Parser_pred_implicit_subject_superset_cascade_childof_optional
    },
    {
        "expr_w_symbol",
        Parser_expr_w_symbol
    },
    {
        "expr_w_newline",
        Parser_expr_w_newline
    },
    {
        "subj_entity_w_explicit_self",
        Parser_subj_entity_w_explicit_self
    },
    {
        "subj_entity_w_explicit_self_superset",
        Parser_subj_entity_w_explicit_self_superset
    },
    {
        "subj_entity_w_explicit_superset_relation",
        Parser_subj_entity_w_explicit_superset_relation
    },
    {
        "subj_entity_w_explicit_self_superset_relation",
        Parser_subj_entity_w_explicit_self_superset_relation
    },
    {
        "obj_entity_w_explicit_self",
        Parser_obj_entity_w_explicit_self
    },
    {
        "obj_entity_w_explicit_self_superset",
        Parser_obj_entity_w_explicit_self_superset
    },
    {
        "obj_entity_w_explicit_superset_relation",
        Parser_obj_entity_w_explicit_superset_relation
    },
    {
        "obj_entity_w_explicit_self_superset_relation",
        Parser_obj_entity_w_explicit_self_superset_relation
    },
    {
        "pred_entity_w_explicit_self",
        Parser_pred_entity_w_explicit_self
    },
    {
        "pred_entity_w_explicit_self_superset",
        Parser_pred_entity_w_explicit_self_superset
    },
    {
        "pred_entity_w_explicit_superset_relation",
        Parser_pred_entity_w_explicit_superset_relation
    },
    {
        "pred_entity_w_explicit_self_superset_relation",
        Parser_pred_entity_w_explicit_self_superset_relation
    },
    {
        "pred_entity_no_args_w_explicit_self",
        Parser_pred_entity_no_args_w_explicit_self
    },
    {
        "pred_entity_no_args_w_explicit_self_superset",
        Parser_pred_entity_no_args_w_explicit_self_superset
    },
    {
        "pred_entity_no_args_w_explicit_superset_relation",
        Parser_pred_entity_no_args_w_explicit_superset_relation
    },
    {
        "pred_entity_no_args_w_explicit_self_superset_relation",
        Parser_pred_entity_no_args_w_explicit_self_superset_relation
    },
    {
        "pred_entity_no_args_2_terms_w_explicit_self",
        Parser_pred_entity_no_args_2_terms_w_explicit_self
    },
    {
        "pred_entity_no_args_2_terms_w_explicit_self_superset",
        Parser_pred_entity_no_args_2_terms_w_explicit_self_superset
    },
    {
        "pred_entity_no_args_2_terms_w_explicit_superset_relation",
        Parser_pred_entity_no_args_2_terms_w_explicit_superset_relation
    },
    {
        "pred_entity_no_args_2_terms_w_explicit_self_superset_relation",
        Parser_pred_entity_no_args_2_terms_w_explicit_self_superset_relation
    },
    {
        "newline",
        Parser_newline
    },
    {
        "2_newlines",
        Parser_2_newlines
    },
    {
        "3_newlines",
        Parser_3_newlines
    },
    {
        "space",
        Parser_space
    },
    {
        "2_spaces",
        Parser_2_spaces
    },
    {
        "trailing_newline",
        Parser_trailing_newline
    },
    {
        "2_trailing_newlines",
        Parser_2_trailing_newlines
    },
    {
        "trailing_space",
        Parser_trailing_space
    },
    {
        "2_trailing_spaces",
        Parser_2_trailing_spaces
    },
    {
        "template_type",
        Parser_template_type
    },
    {
        "predicate_w_parens",
        Parser_predicate_w_parens
    },
    {
        "switch_id",
        Parser_switch_id
    },
    {
        "case_pair",
        Parser_case_pair
    },
    {
        "pair_w_invalid_role",
        Parser_pair_w_invalid_role
    },
    {
        "case_w_missing_obj",
        Parser_case_w_missing_obj
    }
};

bake_test_case Plecs_testcases[] = {
    {
        "null",
        Plecs_null
    },
    {
        "empty",
        Plecs_empty
    },
    {
        "space",
        Plecs_space
    },
    {
        "space_newline",
        Plecs_space_newline
    },
    {
        "two_empty_newlines",
        Plecs_two_empty_newlines
    },
    {
        "three_empty_newlines",
        Plecs_three_empty_newlines
    },
    {
        "newline_trailing_space",
        Plecs_newline_trailing_space
    },
    {
        "newline_trailing_spaces",
        Plecs_newline_trailing_spaces
    },
    {
        "multiple_trailing_newlines",
        Plecs_multiple_trailing_newlines
    },
    {
        "invalid_2_identifiers_separated_by_space",
        Plecs_invalid_2_identifiers_separated_by_space
    },
    {
        "entity",
        Plecs_entity
    },
    {
        "entity_w_entity",
        Plecs_entity_w_entity
    },
    {
        "entity_w_pair",
        Plecs_entity_w_pair
    },
    {
        "2_entities",
        Plecs_2_entities
    },
    {
        "2_entities_w_entities",
        Plecs_2_entities_w_entities
    },
    {
        "3_entities_w_pairs",
        Plecs_3_entities_w_pairs
    },
    {
        "line_comment",
        Plecs_line_comment
    },
    {
        "line_comment_before_stmt",
        Plecs_line_comment_before_stmt
    },
    {
        "line_comment_after_stmt",
        Plecs_line_comment_after_stmt
    },
    {
        "line_comment_between_stmt",
        Plecs_line_comment_between_stmt
    },
    {
        "multiple_line_comment",
        Plecs_multiple_line_comment
    },
    {
        "line_comment_after_stmt_same_line",
        Plecs_line_comment_after_stmt_same_line
    },
    {
        "comma_separated_pred",
        Plecs_comma_separated_pred
    },
    {
        "comma_separated_pred_w_subj",
        Plecs_comma_separated_pred_w_subj
    },
    {
        "comma_separated_pred_w_subj_obj",
        Plecs_comma_separated_pred_w_subj_obj
    },
    {
        "comma_separated_pred_trailing_comma",
        Plecs_comma_separated_pred_trailing_comma
    },
    {
        "comma_separated_pred_trailing_comma_newline",
        Plecs_comma_separated_pred_trailing_comma_newline
    },
    {
        "comma_separated_pred_trailing_comma_newline_multiline",
        Plecs_comma_separated_pred_trailing_comma_newline_multiline
    },
    {
        "hierarchy_1_child",
        Plecs_hierarchy_1_child
    },
    {
        "hierarchy_2_children",
        Plecs_hierarchy_2_children
    },
    {
        "hierarchy_1_child_same_line",
        Plecs_hierarchy_1_child_same_line
    },
    {
        "hierarchy_2_children_same_line",
        Plecs_hierarchy_2_children_same_line
    },
    {
        "entity_after_hierarchy",
        Plecs_entity_after_hierarchy
    },
    {
        "newline_before_scope_open",
        Plecs_newline_before_scope_open
    },
    {
        "comment_before_scope_open",
        Plecs_comment_before_scope_open
    },
    {
        "comment_after_newline_before_scope_open",
        Plecs_comment_after_newline_before_scope_open
    },
    {
        "hierarchy_2_levels",
        Plecs_hierarchy_2_levels
    },
    {
        "hierarchy_2_levels_2_subtrees",
        Plecs_hierarchy_2_levels_2_subtrees
    },
    {
        "missing_end_of_scope",
        Plecs_missing_end_of_scope
    },
    {
        "missing_end_of_predicate_scope",
        Plecs_missing_end_of_predicate_scope
    },
    {
        "create_in_scope",
        Plecs_create_in_scope
    },
    {
        "hierarchy_w_pred_subj",
        Plecs_hierarchy_w_pred_subj
    },
    {
        "hierarchy_custom_relation",
        Plecs_hierarchy_custom_relation
    },
    {
        "hierarchy_custom_relation_2_levels",
        Plecs_hierarchy_custom_relation_2_levels
    },
    {
        "hierarchy_custom_relation_apply_to_object",
        Plecs_hierarchy_custom_relation_apply_to_object
    },
    {
        "hierarchy_custom_relation_apply_to_object_2_levels",
        Plecs_hierarchy_custom_relation_apply_to_object_2_levels
    },
    {
        "entity_after_hierarchy_custom_relation",
        Plecs_entity_after_hierarchy_custom_relation
    },
    {
        "entity_after_hierarchy_custom_relation_2_levels",
        Plecs_entity_after_hierarchy_custom_relation_2_levels
    },
    {
        "pred_scope",
        Plecs_pred_scope
    },
    {
        "pred_scope_2_levels",
        Plecs_pred_scope_2_levels
    },
    {
        "pred_scope_inside_with",
        Plecs_pred_scope_inside_with
    },
    {
        "pred_scope_nested_w_subj_scope",
        Plecs_pred_scope_nested_w_subj_scope
    },
    {
        "with_tag",
        Plecs_with_tag
    },
    {
        "with_tag_2_entities",
        Plecs_with_tag_2_entities
    },
    {
        "with_tag_same_line",
        Plecs_with_tag_same_line
    },
    {
        "with_tag_2_entities_same_line",
        Plecs_with_tag_2_entities_same_line
    },
    {
        "with_tag_2_levels",
        Plecs_with_tag_2_levels
    },
    {
        "with_tag_2_levels_2_subtrees",
        Plecs_with_tag_2_levels_2_subtrees
    },
    {
        "with_n_tags",
        Plecs_with_n_tags
    },
    {
        "with_n_tags_2_levels",
        Plecs_with_n_tags_2_levels
    },
    {
        "with_after_scope",
        Plecs_with_after_scope
    },
    {
        "with_after_with",
        Plecs_with_after_with
    },
    {
        "scope_inside_with_inside_scope",
        Plecs_scope_inside_with_inside_scope
    },
    {
        "with_inside_scope",
        Plecs_with_inside_scope
    },
    {
        "assignment_w_1",
        Plecs_assignment_w_1
    },
    {
        "assignment_w_2",
        Plecs_assignment_w_2
    },
    {
        "assignment_w_pair",
        Plecs_assignment_w_pair
    },
    {
        "assignment_w_invalid_subject",
        Plecs_assignment_w_invalid_subject
    },
    {
        "assignment_w_invalid_with",
        Plecs_assignment_w_invalid_with
    },
    {
        "inherit_w_colon",
        Plecs_inherit_w_colon
    },
    {
        "inherit_w_colon_w_scope",
        Plecs_inherit_w_colon_w_scope
    },
    {
        "inherit_w_colon_w_assign",
        Plecs_inherit_w_colon_w_assign
    },
    {
        "assign_component_value",
        Plecs_assign_component_value
    },
    {
        "assign_2_component_values",
        Plecs_assign_2_component_values
    },
    {
        "assign_component_value_in_assign_scope",
        Plecs_assign_component_value_in_assign_scope
    },
    {
        "assign_2_component_values_in_assign_scope",
        Plecs_assign_2_component_values_in_assign_scope
    },
    {
        "type_and_assign_in_plecs",
        Plecs_type_and_assign_in_plecs
    },
    {
        "type_and_assign_in_plecs_w_2_members",
        Plecs_type_and_assign_in_plecs_w_2_members
    },
    {
        "type_and_assign_in_plecs_w_3_members",
        Plecs_type_and_assign_in_plecs_w_3_members
    },
    {
        "type_and_assign_in_plecs_w_enum",
        Plecs_type_and_assign_in_plecs_w_enum
    },
    {
        "type_and_assign_in_plecs_w_enum_using_meta",
        Plecs_type_and_assign_in_plecs_w_enum_using_meta
    },
    {
        "type_and_assign_in_plecs_w_enum_primitive_using_meta",
        Plecs_type_and_assign_in_plecs_w_enum_primitive_using_meta
    },
    {
        "type_and_assign_in_plecs_w_enum_primitive_and_struct",
        Plecs_type_and_assign_in_plecs_w_enum_primitive_and_struct
    },
    {
        "type_and_assign_in_plecs_nested_member",
        Plecs_type_and_assign_in_plecs_nested_member
    },
    {
        "open_scope_no_parent",
        Plecs_open_scope_no_parent
    },
    {
        "create_subject_in_root_scope_w_resolvable_id",
        Plecs_create_subject_in_root_scope_w_resolvable_id
    },
    {
        "create_subject_in_scope_w_resolvable_id",
        Plecs_create_subject_in_scope_w_resolvable_id
    },
    {
        "create_subject_in_scope_w_resolvable_id_using",
        Plecs_create_subject_in_scope_w_resolvable_id_using
    },
    {
        "using_scope",
        Plecs_using_scope
    },
    {
        "using_nested_scope",
        Plecs_using_nested_scope
    },
    {
        "using_nested_in_scope",
        Plecs_using_nested_in_scope
    },
    {
        "using_with_scope",
        Plecs_using_with_scope
    },
    {
        "using_w_entity_ref_in_value_2_members",
        Plecs_using_w_entity_ref_in_value_2_members
    },
    {
        "using_w_entity_ref_in_value_3_members",
        Plecs_using_w_entity_ref_in_value_3_members
    },
    {
        "2_using_scope",
        Plecs_2_using_scope
    },
    {
        "2_using_in_different_scope",
        Plecs_2_using_in_different_scope
    },
    {
        "empty_scope_after_using",
        Plecs_empty_scope_after_using
    },
    {
        "assignment_to_non_component",
        Plecs_assignment_to_non_component
    },
    {
        "struct_w_member_w_assignment_to_nothing",
        Plecs_struct_w_member_w_assignment_to_nothing
    },
    {
        "struct_w_member_w_assignment_to_empty_scope",
        Plecs_struct_w_member_w_assignment_to_empty_scope
    },
    {
        "scope_after_assign",
        Plecs_scope_after_assign
    },
    {
        "assign_after_inherit",
        Plecs_assign_after_inherit
    },
    {
        "multiple_assignments_single_line",
        Plecs_multiple_assignments_single_line
    },
    {
        "2_stmts_in_scope_w_no_parent",
        Plecs_2_stmts_in_scope_w_no_parent
    },
    {
        "scope_after_assign_1_tag",
        Plecs_scope_after_assign_1_tag
    },
    {
        "scope_after_assign_2_tags",
        Plecs_scope_after_assign_2_tags
    },
    {
        "invalid_nested_assignment",
        Plecs_invalid_nested_assignment
    },
    {
        "invalid_partial_pair_assignment",
        Plecs_invalid_partial_pair_assignment
    },
    {
        "empty_assignment",
        Plecs_empty_assignment
    },
    {
        "assign_tag_to_parent",
        Plecs_assign_tag_to_parent
    },
    {
        "assign_component_to_parent",
        Plecs_assign_component_to_parent
    },
    {
        "empty_assignment_before_end_of_scope",
        Plecs_empty_assignment_before_end_of_scope
    },
    {
        "assign_to_parent_pair_w_new_entities_in_scope",
        Plecs_assign_to_parent_pair_w_new_entities_in_scope
    },
    {
        "assign_to_parent_pair_w_existing_entities_in_scope",
        Plecs_assign_to_parent_pair_w_existing_entities_in_scope
    },
    {
        "default_child_component",
        Plecs_default_child_component
    },
    {
        "default_child_component_w_assign",
        Plecs_default_child_component_w_assign
    },
    {
        "struct_type_w_default_child_component",
        Plecs_struct_type_w_default_child_component
    },
    {
        "struct_type_w_default_child_component_nested_member",
        Plecs_struct_type_w_default_child_component_nested_member
    },
    {
        "enum_type_w_default_child_component",
        Plecs_enum_type_w_default_child_component
    },
    {
        "comment_as_brief_doc",
        Plecs_comment_as_brief_doc
    },
    {
        "comment_as_brief_doc_after_using",
        Plecs_comment_as_brief_doc_after_using
    },
    {
        "comment_as_brief_doc_2_stmts",
        Plecs_comment_as_brief_doc_2_stmts
    },
    {
        "comment_type",
        Plecs_comment_type
    },
    {
        "empty_doc_comment",
        Plecs_empty_doc_comment
    },
    {
        "newline_after_doc_comment",
        Plecs_newline_after_doc_comment
    },
    {
        "default_type_from_with",
        Plecs_default_type_from_with
    },
    {
        "scope_w_1_subj_and_2_pairs",
        Plecs_scope_w_1_subj_and_2_pairs
    },
    {
        "inherit_from_multiple",
        Plecs_inherit_from_multiple
    },
    {
        "assign_pair_component",
        Plecs_assign_pair_component
    },
    {
        "assign_pair_component_in_scope",
        Plecs_assign_pair_component_in_scope
    },
    {
        "set_entity_names",
        Plecs_set_entity_names
    }
};

bake_test_case GlobalComponentIds_testcases[] = {
    {
        "declare",
        GlobalComponentIds_declare
    },
    {
        "declare_w_entity",
        GlobalComponentIds_declare_w_entity
    },
    {
        "declare_2_world",
        GlobalComponentIds_declare_2_world
    },
    {
        "declare_tag",
        GlobalComponentIds_declare_tag
    },
    {
        "declare_tag_w_entity",
        GlobalComponentIds_declare_tag_w_entity
    },
    {
        "declare_entity",
        GlobalComponentIds_declare_entity
    },
    {
        "reuse_300_component_ids",
        GlobalComponentIds_reuse_300_component_ids
    }
};

bake_test_case Doc_testcases[] = {
    {
        "get_set_brief",
        Doc_get_set_brief
    },
    {
        "get_set_detail",
        Doc_get_set_detail
    },
    {
        "get_set_link",
        Doc_get_set_link
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
        "get_object_from_0",
        Hierarchies_get_object_from_0
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
        "scope_component_no_macro",
        Hierarchies_scope_component_no_macro
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
        "add_path_from_scope",
        Hierarchies_add_path_from_scope
    },
    {
        "add_path_from_scope_new_entity",
        Hierarchies_add_path_from_scope_new_entity
    },
    {
        "new_w_child_in_root",
        Hierarchies_new_w_child_in_root
    },
    {
        "delete_child",
        Hierarchies_delete_child
    },
    {
        "delete_2_children",
        Hierarchies_delete_2_children
    },
    {
        "delete_2_children_different_type",
        Hierarchies_delete_2_children_different_type
    },
    {
        "delete_tree_2_levels",
        Hierarchies_delete_tree_2_levels
    },
    {
        "delete_tree_3_levels",
        Hierarchies_delete_tree_3_levels
    },
    {
        "delete_tree_count_tables",
        Hierarchies_delete_tree_count_tables
    },
    {
        "delete_tree_staged",
        Hierarchies_delete_tree_staged
    },
    {
        "delete_tree_empty_table",
        Hierarchies_delete_tree_empty_table
    },
    {
        "delete_tree_recreate",
        Hierarchies_delete_tree_recreate
    },
    {
        "delete_tree_w_onremove",
        Hierarchies_delete_tree_w_onremove
    },
    {
        "delete_tree_w_dtor",
        Hierarchies_delete_tree_w_dtor
    },
    {
        "get_child_count",
        Hierarchies_get_child_count
    },
    {
        "get_child_count_2_tables",
        Hierarchies_get_child_count_2_tables
    },
    {
        "get_child_count_no_children",
        Hierarchies_get_child_count_no_children
    },
    {
        "scope_iter_after_delete_tree",
        Hierarchies_scope_iter_after_delete_tree
    },
    {
        "add_child_after_delete_tree",
        Hierarchies_add_child_after_delete_tree
    },
    {
        "add_child_to_recycled_parent",
        Hierarchies_add_child_to_recycled_parent
    },
    {
        "get_type_after_recycled_parent_add",
        Hierarchies_get_type_after_recycled_parent_add
    },
    {
        "rematch_after_add_to_recycled_parent",
        Hierarchies_rematch_after_add_to_recycled_parent
    },
    {
        "cascade_after_recycled_parent_change",
        Hierarchies_cascade_after_recycled_parent_change
    },
    {
        "long_name_depth_0",
        Hierarchies_long_name_depth_0
    },
    {
        "long_name_depth_1",
        Hierarchies_long_name_depth_1
    },
    {
        "long_name_depth_2",
        Hierarchies_long_name_depth_2
    },
    {
        "ensure_1_parent_after_adding_2",
        Hierarchies_ensure_1_parent_after_adding_2
    },
    {
        "ensure_child_alive_after_deleting_prev_parent",
        Hierarchies_ensure_child_alive_after_deleting_prev_parent
    }
};

bake_test_case Has_testcases[] = {
    {
        "zero",
        Has_zero
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
    },
    {
        "has_wildcard",
        Has_has_wildcard
    },
    {
        "has_wildcard_pair",
        Has_has_wildcard_pair
    }
};

bake_test_case Count_testcases[] = {
    {
        "count_empty",
        Count_count_empty
    },
    {
        "count_w_entity_0",
        Count_count_w_entity_0
    },
    {
        "count_1_component",
        Count_count_1_component
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
    },
    {
        "get_childof_component",
        Get_component_get_childof_component
    }
};

bake_test_case Reference_testcases[] = {
    {
        "get_ref",
        Reference_get_ref
    },
    {
        "get_ref_after_add",
        Reference_get_ref_after_add
    },
    {
        "get_ref_after_remove",
        Reference_get_ref_after_remove
    },
    {
        "get_ref_after_delete",
        Reference_get_ref_after_delete
    },
    {
        "get_ref_after_realloc",
        Reference_get_ref_after_realloc
    },
    {
        "get_ref_after_realloc_w_lifecycle",
        Reference_get_ref_after_realloc_w_lifecycle
    },
    {
        "get_ref_staged",
        Reference_get_ref_staged
    },
    {
        "get_ref_after_new_in_stage",
        Reference_get_ref_after_new_in_stage
    },
    {
        "get_ref_monitored",
        Reference_get_ref_monitored
    },
    {
        "get_nonexisting",
        Reference_get_nonexisting
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
    },
    {
        "clear_1_component",
        Delete_clear_1_component
    },
    {
        "clear_2_components",
        Delete_clear_2_components
    },
    {
        "alive_after_delete",
        Delete_alive_after_delete
    },
    {
        "alive_after_clear",
        Delete_alive_after_clear
    },
    {
        "alive_after_staged_delete",
        Delete_alive_after_staged_delete
    },
    {
        "alive_while_staged",
        Delete_alive_while_staged
    },
    {
        "alive_while_staged_w_delete",
        Delete_alive_while_staged_w_delete
    },
    {
        "alive_while_staged_w_delete_recycled_id",
        Delete_alive_while_staged_w_delete_recycled_id
    },
    {
        "alive_after_recycle",
        Delete_alive_after_recycle
    },
    {
        "delete_recycled",
        Delete_delete_recycled
    },
    {
        "get_alive_for_alive",
        Delete_get_alive_for_alive
    },
    {
        "get_alive_for_recycled",
        Delete_get_alive_for_recycled
    },
    {
        "get_alive_for_not_alive",
        Delete_get_alive_for_not_alive
    },
    {
        "get_alive_w_generation_for_recycled_alive",
        Delete_get_alive_w_generation_for_recycled_alive
    },
    {
        "get_alive_w_generation_for_recycled_not_alive",
        Delete_get_alive_w_generation_for_recycled_not_alive
    },
    {
        "get_alive_for_0",
        Delete_get_alive_for_0
    },
    {
        "get_alive_for_nonexistent",
        Delete_get_alive_for_nonexistent
    },
    {
        "move_w_dtor_move",
        Delete_move_w_dtor_move
    },
    {
        "move_w_dtor_no_move",
        Delete_move_w_dtor_no_move
    },
    {
        "move_w_no_dtor_move",
        Delete_move_w_no_dtor_move
    },
    {
        "wrap_generation_count",
        Delete_wrap_generation_count
    }
};

bake_test_case OnDelete_testcases[] = {
    {
        "on_delete_flags",
        OnDelete_on_delete_flags
    },
    {
        "on_delete_id_default",
        OnDelete_on_delete_id_default
    },
    {
        "on_delete_id_remove",
        OnDelete_on_delete_id_remove
    },
    {
        "on_delete_id_delete",
        OnDelete_on_delete_id_delete
    },
    {
        "on_delete_relation_default",
        OnDelete_on_delete_relation_default
    },
    {
        "on_delete_relation_remove",
        OnDelete_on_delete_relation_remove
    },
    {
        "on_delete_relation_delete",
        OnDelete_on_delete_relation_delete
    },
    {
        "on_delete_object_default",
        OnDelete_on_delete_object_default
    },
    {
        "on_delete_object_remove",
        OnDelete_on_delete_object_remove
    },
    {
        "on_delete_object_delete",
        OnDelete_on_delete_object_delete
    },
    {
        "on_delete_id_throw",
        OnDelete_on_delete_id_throw
    },
    {
        "on_delete_relation_throw",
        OnDelete_on_delete_relation_throw
    },
    {
        "on_delete_object_throw",
        OnDelete_on_delete_object_throw
    },
    {
        "on_delete_id_remove_no_instances",
        OnDelete_on_delete_id_remove_no_instances
    },
    {
        "on_delete_id_delete_no_instances",
        OnDelete_on_delete_id_delete_no_instances
    },
    {
        "on_delete_id_throw_no_instances",
        OnDelete_on_delete_id_throw_no_instances
    },
    {
        "on_delete_cyclic_id_default",
        OnDelete_on_delete_cyclic_id_default
    },
    {
        "on_delete_cyclic_id_remove",
        OnDelete_on_delete_cyclic_id_remove
    },
    {
        "on_delete_cyclic_id_remove_both",
        OnDelete_on_delete_cyclic_id_remove_both
    },
    {
        "on_delete_cyclic_id_delete",
        OnDelete_on_delete_cyclic_id_delete
    },
    {
        "on_delete_cyclic_id_delete_both",
        OnDelete_on_delete_cyclic_id_delete_both
    },
    {
        "on_delete_cyclic_relation_default",
        OnDelete_on_delete_cyclic_relation_default
    },
    {
        "on_delete_cyclic_relation_remove",
        OnDelete_on_delete_cyclic_relation_remove
    },
    {
        "on_delete_cyclic_relation_remove_both",
        OnDelete_on_delete_cyclic_relation_remove_both
    },
    {
        "on_delete_cyclic_relation_delete",
        OnDelete_on_delete_cyclic_relation_delete
    },
    {
        "on_delete_cyclic_object_default",
        OnDelete_on_delete_cyclic_object_default
    },
    {
        "on_delete_cyclic_object_remove",
        OnDelete_on_delete_cyclic_object_remove
    },
    {
        "on_delete_cyclic_object_delete",
        OnDelete_on_delete_cyclic_object_delete
    },
    {
        "on_delete_remove_2_comps",
        OnDelete_on_delete_remove_2_comps
    },
    {
        "on_delete_remove_2_comps_to_existing_table",
        OnDelete_on_delete_remove_2_comps_to_existing_table
    },
    {
        "on_delete_delete_recursive",
        OnDelete_on_delete_delete_recursive
    },
    {
        "on_delete_component_throw",
        OnDelete_on_delete_component_throw
    },
    {
        "on_delete_remove_2_relations",
        OnDelete_on_delete_remove_2_relations
    },
    {
        "on_delete_remove_object_w_2_relations",
        OnDelete_on_delete_remove_object_w_2_relations
    },
    {
        "on_delete_remove_object_w_5_relations",
        OnDelete_on_delete_remove_object_w_5_relations
    },
    {
        "on_delete_remove_object_w_50_relations",
        OnDelete_on_delete_remove_object_w_50_relations
    },
    {
        "on_delete_remove_object_w_50_relations_3_tables",
        OnDelete_on_delete_remove_object_w_50_relations_3_tables
    },
    {
        "remove_id_from_2_tables",
        OnDelete_remove_id_from_2_tables
    },
    {
        "remove_relation_from_2_tables",
        OnDelete_remove_relation_from_2_tables
    },
    {
        "remove_object_from_2_tables",
        OnDelete_remove_object_from_2_tables
    },
    {
        "remove_id_and_relation",
        OnDelete_remove_id_and_relation
    },
    {
        "remove_id_and_relation_from_2_tables",
        OnDelete_remove_id_and_relation_from_2_tables
    },
    {
        "stresstest_many_objects",
        OnDelete_stresstest_many_objects
    },
    {
        "stresstest_many_relations",
        OnDelete_stresstest_many_relations
    },
    {
        "stresstest_many_objects_on_delete",
        OnDelete_stresstest_many_objects_on_delete
    },
    {
        "stresstest_many_relations_on_delete",
        OnDelete_stresstest_many_relations_on_delete
    },
    {
        "on_delete_empty_table_w_on_remove",
        OnDelete_on_delete_empty_table_w_on_remove
    },
    {
        "delete_table_in_on_remove_during_fini",
        OnDelete_delete_table_in_on_remove_during_fini
    },
    {
        "delete_other_in_on_remove_during_fini",
        OnDelete_delete_other_in_on_remove_during_fini
    },
    {
        "on_delete_remove_id_w_role",
        OnDelete_on_delete_remove_id_w_role
    },
    {
        "on_delete_merge_pair_component",
        OnDelete_on_delete_merge_pair_component
    },
    {
        "delete_with_tag",
        OnDelete_delete_with_tag
    },
    {
        "delete_with_component",
        OnDelete_delete_with_component
    },
    {
        "delete_with_pair",
        OnDelete_delete_with_pair
    },
    {
        "delete_with_object_wildcard",
        OnDelete_delete_with_object_wildcard
    },
    {
        "delete_with_relation_wildcard",
        OnDelete_delete_with_relation_wildcard
    },
    {
        "delete_all_with_entity",
        OnDelete_delete_all_with_entity
    },
    {
        "remove_childof_entity",
        OnDelete_remove_childof_entity
    },
    {
        "remove_childof_wildcard",
        OnDelete_remove_childof_wildcard
    },
    {
        "delete_child_of_delete_with",
        OnDelete_delete_child_of_delete_with
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
        "get_mut_new",
        Set_get_mut_new
    },
    {
        "get_mut_existing",
        Set_get_mut_existing
    },
    {
        "get_mut_tag_new",
        Set_get_mut_tag_new
    },
    {
        "get_mut_tag_existing",
        Set_get_mut_tag_existing
    },
    {
        "get_mut_tag_new_w_comp",
        Set_get_mut_tag_new_w_comp
    },
    {
        "get_mut_tag_existing_w_comp",
        Set_get_mut_tag_existing_w_comp
    },
    {
        "get_mut_tag_new_w_pair",
        Set_get_mut_tag_new_w_pair
    },
    {
        "get_mut_tag_existing_w_pair",
        Set_get_mut_tag_existing_w_pair
    },
    {
        "modified_w_on_set",
        Set_modified_w_on_set
    },
    {
        "modified_no_component",
        Set_modified_no_component
    },
    {
        "get_mut_w_add_in_on_add",
        Set_get_mut_w_add_in_on_add
    },
    {
        "get_mut_w_remove_in_on_add",
        Set_get_mut_w_remove_in_on_add
    },
    {
        "emplace",
        Set_emplace
    },
    {
        "emplace_existing",
        Set_emplace_existing
    },
    {
        "emplace_w_move",
        Set_emplace_w_move
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
    },
    {
        "lookup_recycled_by_id",
        Lookup_lookup_recycled_by_id
    },
    {
        "lookup_symbol_by_id",
        Lookup_lookup_symbol_by_id
    },
    {
        "lookup_name_w_digit",
        Lookup_lookup_name_w_digit
    },
    {
        "lookup_symbol_w_digit",
        Lookup_lookup_symbol_w_digit
    },
    {
        "lookup_path_w_digit",
        Lookup_lookup_path_w_digit
    },
    {
        "set_name_of_existing",
        Lookup_set_name_of_existing
    },
    {
        "change_name_of_existing",
        Lookup_change_name_of_existing
    },
    {
        "lookup_alias",
        Lookup_lookup_alias
    },
    {
        "lookup_scoped_alias",
        Lookup_lookup_scoped_alias
    },
    {
        "define_duplicate_alias",
        Lookup_define_duplicate_alias
    },
    {
        "lookup_null",
        Lookup_lookup_null
    },
    {
        "lookup_symbol_null",
        Lookup_lookup_symbol_null
    },
    {
        "lookup_this",
        Lookup_lookup_this
    },
    {
        "lookup_wildcard",
        Lookup_lookup_wildcard
    },
    {
        "lookup_any",
        Lookup_lookup_any
    },
    {
        "lookup_path_this",
        Lookup_lookup_path_this
    },
    {
        "lookup_path_wildcard",
        Lookup_lookup_path_wildcard
    },
    {
        "lookup_path_this_from_scope",
        Lookup_lookup_path_this_from_scope
    },
    {
        "lookup_path_wildcard_from_scope",
        Lookup_lookup_path_wildcard_from_scope
    },
    {
        "resolve_builtin_symbols",
        Lookup_resolve_builtin_symbols
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
        "singleton_system",
        Singleton_singleton_system
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
        "copy_on_clone",
        ComponentLifecycle_copy_on_clone
    },
    {
        "no_copy_on_move",
        ComponentLifecycle_no_copy_on_move
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
    },
    {
        "merge_to_different_table",
        ComponentLifecycle_merge_to_different_table
    },
    {
        "merge_to_new_table",
        ComponentLifecycle_merge_to_new_table
    },
    {
        "delete_in_stage",
        ComponentLifecycle_delete_in_stage
    },
    {
        "ctor_on_add_pair",
        ComponentLifecycle_ctor_on_add_pair
    },
    {
        "ctor_on_add_pair_set_ctor_after_table",
        ComponentLifecycle_ctor_on_add_pair_set_ctor_after_table
    },
    {
        "ctor_on_add_pair_tag",
        ComponentLifecycle_ctor_on_add_pair_tag
    },
    {
        "ctor_on_add_pair_tag_set_ctor_after_table",
        ComponentLifecycle_ctor_on_add_pair_tag_set_ctor_after_table
    },
    {
        "ctor_on_move_pair",
        ComponentLifecycle_ctor_on_move_pair
    },
    {
        "move_on_realloc",
        ComponentLifecycle_move_on_realloc
    },
    {
        "move_on_bulk_new",
        ComponentLifecycle_move_on_bulk_new
    },
    {
        "move_on_delete",
        ComponentLifecycle_move_on_delete
    },
    {
        "move_dtor_on_delete",
        ComponentLifecycle_move_dtor_on_delete
    },
    {
        "copy_on_override_pair",
        ComponentLifecycle_copy_on_override_pair
    },
    {
        "copy_on_override_pair_tag",
        ComponentLifecycle_copy_on_override_pair_tag
    },
    {
        "copy_on_set_pair",
        ComponentLifecycle_copy_on_set_pair
    },
    {
        "copy_on_set_pair_tag",
        ComponentLifecycle_copy_on_set_pair_tag
    },
    {
        "prevent_lifecycle_overwrite",
        ComponentLifecycle_prevent_lifecycle_overwrite
    },
    {
        "prevent_lifecycle_overwrite_null_callbacks",
        ComponentLifecycle_prevent_lifecycle_overwrite_null_callbacks
    },
    {
        "allow_lifecycle_overwrite_equal_callbacks",
        ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks
    },
    {
        "set_lifecycle_after_trigger",
        ComponentLifecycle_set_lifecycle_after_trigger
    },
    {
        "valid_entity_in_dtor_after_delete",
        ComponentLifecycle_valid_entity_in_dtor_after_delete
    },
    {
        "ctor_w_emplace",
        ComponentLifecycle_ctor_w_emplace
    },
    {
        "dtor_on_fini",
        ComponentLifecycle_dtor_on_fini
    },
    {
        "valid_type_in_dtor_on_fini",
        ComponentLifecycle_valid_type_in_dtor_on_fini
    },
    {
        "valid_other_type_of_entity_in_dtor_on_fini",
        ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini
    },
    {
        "valid_same_type_comp_of_entity_in_dtor_on_fini",
        ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_fini
    },
    {
        "valid_same_type_comp_of_entity_in_dtor_on_delete_parent",
        ComponentLifecycle_valid_same_type_comp_of_entity_in_dtor_on_delete_parent
    },
    {
        "delete_in_dtor_same_type_on_fini",
        ComponentLifecycle_delete_in_dtor_same_type_on_fini
    },
    {
        "delete_in_dtor_other_type_on_fini",
        ComponentLifecycle_delete_in_dtor_other_type_on_fini
    },
    {
        "delete_self_in_dtor_on_fini",
        ComponentLifecycle_delete_self_in_dtor_on_fini
    },
    {
        "delete_in_dtor_same_type_on_delete_parent",
        ComponentLifecycle_delete_in_dtor_same_type_on_delete_parent
    },
    {
        "delete_in_dtor_other_type_on_delete_parent",
        ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent
    },
    {
        "delete_self_in_dtor_on_delete_parent",
        ComponentLifecycle_delete_self_in_dtor_on_delete_parent
    },
    {
        "delete_in_dtor_same_type_on_delete",
        ComponentLifecycle_delete_in_dtor_same_type_on_delete
    },
    {
        "delete_in_dtor_other_type_on_delete",
        ComponentLifecycle_delete_in_dtor_other_type_on_delete
    },
    {
        "delete_self_in_dtor_on_delete",
        ComponentLifecycle_delete_self_in_dtor_on_delete
    },
    {
        "on_set_after_set",
        ComponentLifecycle_on_set_after_set
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
        "system_reverse_order_by_phase_custom_pipeline",
        Pipeline_system_reverse_order_by_phase_custom_pipeline
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
        "merge_after_staged_in_out",
        Pipeline_merge_after_staged_in_out
    },
    {
        "merge_after_staged_inout_main_implicit_inout",
        Pipeline_merge_after_staged_inout_main_implicit_inout
    },
    {
        "merge_after_staged_inout_main_inout",
        Pipeline_merge_after_staged_inout_main_inout
    },
    {
        "merge_after_staged_out_before_owned",
        Pipeline_merge_after_staged_out_before_owned
    },
    {
        "switch_pipeline",
        Pipeline_switch_pipeline
    },
    {
        "run_pipeline",
        Pipeline_run_pipeline
    },
    {
        "get_pipeline_from_stage",
        Pipeline_get_pipeline_from_stage
    },
    {
        "3_systems_3_types",
        Pipeline_3_systems_3_types
    },
    {
        "random_read_after_random_write_out_in",
        Pipeline_random_read_after_random_write_out_in
    },
    {
        "random_read_after_random_write_inout_in",
        Pipeline_random_read_after_random_write_inout_in
    },
    {
        "random_read_after_random_write_out_inout",
        Pipeline_random_read_after_random_write_out_inout
    },
    {
        "random_read_after_random_write_inout_inout",
        Pipeline_random_read_after_random_write_inout_inout
    },
    {
        "random_read_after_random_write_w_not_write",
        Pipeline_random_read_after_random_write_w_not_write
    },
    {
        "random_read_after_random_write_w_not_read",
        Pipeline_random_read_after_random_write_w_not_read
    },
    {
        "random_read_after_random_write_w_wildcard",
        Pipeline_random_read_after_random_write_w_wildcard
    },
    {
        "random_in_after_random_inout_after_random_out",
        Pipeline_random_in_after_random_inout_after_random_out
    },
    {
        "stage_write_before_read",
        Pipeline_stage_write_before_read
    },
    {
        "mixed_multithreaded",
        Pipeline_mixed_multithreaded
    },
    {
        "mixed_staging",
        Pipeline_mixed_staging
    },
    {
        "no_staging_system_create_query",
        Pipeline_no_staging_system_create_query
    },
    {
        "single_threaded_pipeline_change",
        Pipeline_single_threaded_pipeline_change
    },
    {
        "multi_threaded_pipeline_change",
        Pipeline_multi_threaded_pipeline_change
    },
    {
        "activate_after_add",
        Pipeline_activate_after_add
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
        "invalid_entity_without_id",
        SystemMisc_invalid_entity_without_id
    },
    {
        "invalid_empty_without_id",
        SystemMisc_invalid_empty_without_id
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
        "invalid_component_id",
        SystemMisc_invalid_component_id
    },
    {
        "invalid_entity_id",
        SystemMisc_invalid_entity_id
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
        "invalid_mixed_src_modifier",
        SystemMisc_invalid_mixed_src_modifier
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
    },
    {
        "add_to_system_in_progress",
        SystemMisc_add_to_system_in_progress
    },
    {
        "redefine_null_signature",
        SystemMisc_redefine_null_signature
    },
    {
        "redefine_0_signature",
        SystemMisc_redefine_0_signature
    },
    {
        "one_named_column_of_two",
        SystemMisc_one_named_column_of_two
    },
    {
        "two_named_columns_of_two",
        SystemMisc_two_named_columns_of_two
    },
    {
        "redeclare_system_same_expr",
        SystemMisc_redeclare_system_same_expr
    },
    {
        "redeclare_system_null_expr",
        SystemMisc_redeclare_system_null_expr
    },
    {
        "redeclare_system_0_expr",
        SystemMisc_redeclare_system_0_expr
    },
    {
        "redeclare_system_different_expr",
        SystemMisc_redeclare_system_different_expr
    },
    {
        "redeclare_system_null_and_expr",
        SystemMisc_redeclare_system_null_and_expr
    },
    {
        "redeclare_system_expr_and_null",
        SystemMisc_redeclare_system_expr_and_null
    },
    {
        "redeclare_system_expr_and_0",
        SystemMisc_redeclare_system_expr_and_0
    },
    {
        "redeclare_system_0_and_expr",
        SystemMisc_redeclare_system_0_and_expr
    },
    {
        "redeclare_system_0_and_null",
        SystemMisc_redeclare_system_0_and_null
    },
    {
        "redeclare_system_null_and_0",
        SystemMisc_redeclare_system_null_and_0
    },
    {
        "redeclare_system_explicit_id",
        SystemMisc_redeclare_system_explicit_id
    },
    {
        "redeclare_system_explicit_id_null_expr",
        SystemMisc_redeclare_system_explicit_id_null_expr
    },
    {
        "redeclare_system_explicit_id_no_name",
        SystemMisc_redeclare_system_explicit_id_no_name
    },
    {
        "declare_different_id_same_name",
        SystemMisc_declare_different_id_same_name
    },
    {
        "declare_different_id_same_name_w_scope",
        SystemMisc_declare_different_id_same_name_w_scope
    },
    {
        "rw_in_implicit_any",
        SystemMisc_rw_in_implicit_any
    },
    {
        "rw_in_implicit_shared",
        SystemMisc_rw_in_implicit_shared
    },
    {
        "rw_in_implicit_from_empty",
        SystemMisc_rw_in_implicit_from_empty
    },
    {
        "rw_in_implicit_from_entity",
        SystemMisc_rw_in_implicit_from_entity
    },
    {
        "rw_out_explicit_any",
        SystemMisc_rw_out_explicit_any
    },
    {
        "rw_out_explicit_shared",
        SystemMisc_rw_out_explicit_shared
    },
    {
        "rw_out_explicit_from_empty",
        SystemMisc_rw_out_explicit_from_empty
    },
    {
        "rw_out_explicit_from_entity",
        SystemMisc_rw_out_explicit_from_entity
    },
    {
        "activate_system_for_table_w_n_pairs",
        SystemMisc_activate_system_for_table_w_n_pairs
    },
    {
        "get_query",
        SystemMisc_get_query
    },
    {
        "set_get_context",
        SystemMisc_set_get_context
    },
    {
        "set_get_binding_context",
        SystemMisc_set_get_binding_context
    },
    {
        "deactivate_after_disable",
        SystemMisc_deactivate_after_disable
    },
    {
        "system_w_self",
        SystemMisc_system_w_self
    },
    {
        "delete_system",
        SystemMisc_delete_system
    },
    {
        "delete_pipeline_system",
        SystemMisc_delete_pipeline_system
    },
    {
        "delete_system_w_ctx",
        SystemMisc_delete_system_w_ctx
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
        "sort_after_query",
        Sorting_sort_after_query
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
    },
    {
        "sort_1000_entities_w_duplicates",
        Sorting_sort_1000_entities_w_duplicates
    },
    {
        "sort_1000_entities_again",
        Sorting_sort_1000_entities_again
    },
    {
        "sort_1000_entities_2_types",
        Sorting_sort_1000_entities_2_types
    },
    {
        "sort_1500_entities_3_types",
        Sorting_sort_1500_entities_3_types
    },
    {
        "sort_2000_entities_4_types",
        Sorting_sort_2000_entities_4_types
    },
    {
        "sort_2_entities_2_types",
        Sorting_sort_2_entities_2_types
    },
    {
        "sort_3_entities_3_types",
        Sorting_sort_3_entities_3_types
    },
    {
        "sort_3_entities_3_types_2",
        Sorting_sort_3_entities_3_types_2
    },
    {
        "sort_4_entities_4_types",
        Sorting_sort_4_entities_4_types
    },
    {
        "sort_1000_entities_2_types_again",
        Sorting_sort_1000_entities_2_types_again
    },
    {
        "sort_1000_entities_add_type_after_sort",
        Sorting_sort_1000_entities_add_type_after_sort
    },
    {
        "sort_shared_component",
        Sorting_sort_shared_component
    },
    {
        "sort_w_tags_only",
        Sorting_sort_w_tags_only
    },
    {
        "sort_childof_marked",
        Sorting_sort_childof_marked
    },
    {
        "sort_isa_marked",
        Sorting_sort_isa_marked
    },
    {
        "sort_relation_marked",
        Sorting_sort_relation_marked
    },
    {
        "dont_resort_after_set_unsorted_component",
        Sorting_dont_resort_after_set_unsorted_component
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag",
        Sorting_dont_resort_after_set_unsorted_component_w_tag
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag_w_out_term",
        Sorting_dont_resort_after_set_unsorted_component_w_tag_w_out_term
    }
};

bake_test_case Filter_testcases[] = {
    {
        "filter_1_term",
        Filter_filter_1_term
    },
    {
        "filter_2_terms",
        Filter_filter_2_terms
    },
    {
        "filter_3_terms",
        Filter_filter_3_terms
    },
    {
        "filter_3_terms_w_or",
        Filter_filter_3_terms_w_or
    },
    {
        "filter_4_terms_w_or_at_1",
        Filter_filter_4_terms_w_or_at_1
    },
    {
        "filter_1_term_wildcard",
        Filter_filter_1_term_wildcard
    },
    {
        "filter_1_term_any",
        Filter_filter_1_term_any
    },
    {
        "filter_1_term_same_subj_obj",
        Filter_filter_1_term_same_subj_obj
    },
    {
        "filter_1_term_acyclic_same_subj_obj",
        Filter_filter_1_term_acyclic_same_subj_obj
    },
    {
        "filter_1_term_acyclic_reflexive_same_subj_obj",
        Filter_filter_1_term_acyclic_reflexive_same_subj_obj
    },
    {
        "filter_1_term_same_subj_obj_var",
        Filter_filter_1_term_same_subj_obj_var
    },
    {
        "filter_1_term_acyclic_same_subj_obj_var",
        Filter_filter_1_term_acyclic_same_subj_obj_var
    },
    {
        "filter_1_term_acyclic_reflexive_same_subj_obj_var",
        Filter_filter_1_term_acyclic_reflexive_same_subj_obj_var
    },
    {
        "filter_1_term_non_acyclic_superset",
        Filter_filter_1_term_non_acyclic_superset
    },
    {
        "filter_1_term_dont_inherit_default_set",
        Filter_filter_1_term_dont_inherit_default_set
    },
    {
        "filter_1_term_dont_inherit_pair_default_set",
        Filter_filter_1_term_dont_inherit_pair_default_set
    },
    {
        "filter_w_pair_id",
        Filter_filter_w_pair_id
    },
    {
        "filter_w_pred_obj",
        Filter_filter_w_pred_obj
    },
    {
        "filter_move",
        Filter_filter_move
    },
    {
        "filter_copy",
        Filter_filter_copy
    },
    {
        "filter_w_resources_copy",
        Filter_filter_w_resources_copy
    },
    {
        "filter_w_10_terms",
        Filter_filter_w_10_terms
    },
    {
        "filter_w_10_terms_move",
        Filter_filter_w_10_terms_move
    },
    {
        "filter_w_10_terms_copy",
        Filter_filter_w_10_terms_copy
    },
    {
        "filter_w_and_flag",
        Filter_filter_w_and_flag
    },
    {
        "filter_w_or_flag",
        Filter_filter_w_or_flag
    },
    {
        "filter_w_not_flag",
        Filter_filter_w_not_flag
    },
    {
        "filter_filter",
        Filter_filter_filter
    },
    {
        "term_w_id",
        Filter_term_w_id
    },
    {
        "term_w_pair_id",
        Filter_term_w_pair_id
    },
    {
        "term_w_pred_obj",
        Filter_term_w_pred_obj
    },
    {
        "term_w_pair_finalize_twice",
        Filter_term_w_pair_finalize_twice
    },
    {
        "term_w_role",
        Filter_term_w_role
    },
    {
        "term_w_pred_role",
        Filter_term_w_pred_role
    },
    {
        "term_w_self",
        Filter_term_w_self
    },
    {
        "term_w_superset",
        Filter_term_w_superset
    },
    {
        "term_w_subset",
        Filter_term_w_subset
    },
    {
        "term_w_self_superset",
        Filter_term_w_self_superset
    },
    {
        "term_w_superset_custom_relation",
        Filter_term_w_superset_custom_relation
    },
    {
        "term_w_self_superset_custom_relation",
        Filter_term_w_self_superset_custom_relation
    },
    {
        "term_w_self_min_max_depth",
        Filter_term_w_self_min_max_depth
    },
    {
        "term_w_self_invalid_min_depth",
        Filter_term_w_self_invalid_min_depth
    },
    {
        "term_w_self_invalid_max_depth",
        Filter_term_w_self_invalid_max_depth
    },
    {
        "term_w_self_invalid_min_max_depth",
        Filter_term_w_self_invalid_min_max_depth
    },
    {
        "term_w_superset_min_depth",
        Filter_term_w_superset_min_depth
    },
    {
        "term_w_superset_max_depth",
        Filter_term_w_superset_max_depth
    },
    {
        "term_w_superset_min_max_depth",
        Filter_term_w_superset_min_max_depth
    },
    {
        "term_iter_component",
        Filter_term_iter_component
    },
    {
        "term_iter_tag",
        Filter_term_iter_tag
    },
    {
        "term_iter_pair",
        Filter_term_iter_pair
    },
    {
        "term_iter_pair_w_rel_wildcard",
        Filter_term_iter_pair_w_rel_wildcard
    },
    {
        "term_iter_pair_w_obj_wildcard",
        Filter_term_iter_pair_w_obj_wildcard
    },
    {
        "term_iter_pair_w_rel_wildcard_n_matches",
        Filter_term_iter_pair_w_rel_wildcard_n_matches
    },
    {
        "term_iter_pair_w_rel_wildcard_n_matches_w_data",
        Filter_term_iter_pair_w_rel_wildcard_n_matches_w_data
    },
    {
        "term_iter_pair_w_obj_wildcard_n_matches",
        Filter_term_iter_pair_w_obj_wildcard_n_matches
    },
    {
        "term_iter_pair_w_obj_wildcard_n_matches_w_data",
        Filter_term_iter_pair_w_obj_wildcard_n_matches_w_data
    },
    {
        "term_iter_w_superset",
        Filter_term_iter_w_superset
    },
    {
        "term_iter_w_superset_base_w_2_components",
        Filter_term_iter_w_superset_base_w_2_components
    },
    {
        "term_iter_w_superset_childof",
        Filter_term_iter_w_superset_childof
    },
    {
        "term_iter_w_superset_self",
        Filter_term_iter_w_superset_self
    },
    {
        "term_iter_w_superset_self_childof",
        Filter_term_iter_w_superset_self_childof
    },
    {
        "term_iter_w_superset_tag",
        Filter_term_iter_w_superset_tag
    },
    {
        "term_iter_w_superset_pair",
        Filter_term_iter_w_superset_pair
    },
    {
        "term_iter_w_superset_pair_obj_wildcard",
        Filter_term_iter_w_superset_pair_obj_wildcard
    },
    {
        "term_iter_in_stage",
        Filter_term_iter_in_stage
    },
    {
        "term_iter_w_readonly_term",
        Filter_term_iter_w_readonly_term
    },
    {
        "term_iter_type_set",
        Filter_term_iter_type_set
    },
    {
        "filter_iter_1_tag",
        Filter_filter_iter_1_tag
    },
    {
        "filter_iter_2_tags",
        Filter_filter_iter_2_tags
    },
    {
        "filter_iter_2_tags_1_not",
        Filter_filter_iter_2_tags_1_not
    },
    {
        "filter_iter_3_tags_2_or",
        Filter_filter_iter_3_tags_2_or
    },
    {
        "filter_iter_1_component",
        Filter_filter_iter_1_component
    },
    {
        "filter_iter_2_components",
        Filter_filter_iter_2_components
    },
    {
        "filter_iter_pair_id",
        Filter_filter_iter_pair_id
    },
    {
        "filter_iter_2_pair_ids",
        Filter_filter_iter_2_pair_ids
    },
    {
        "filter_iter_childof_pair_0_parent",
        Filter_filter_iter_childof_pair_0_parent
    },
    {
        "filter_iter_pair_pred_obj",
        Filter_filter_iter_pair_pred_obj
    },
    {
        "filter_iter_pair_2_pred_obj",
        Filter_filter_iter_pair_2_pred_obj
    },
    {
        "filter_iter_null",
        Filter_filter_iter_null
    },
    {
        "filter_iter_1_not_tag",
        Filter_filter_iter_1_not_tag
    },
    {
        "filter_iter_2_tags_1_optional",
        Filter_filter_iter_2_tags_1_optional
    },
    {
        "filter_iter_2_components_1_optional",
        Filter_filter_iter_2_components_1_optional
    },
    {
        "filter_iter_in_stage",
        Filter_filter_iter_in_stage
    },
    {
        "filter_iter_10_tags",
        Filter_filter_iter_10_tags
    },
    {
        "filter_iter_20_tags",
        Filter_filter_iter_20_tags
    },
    {
        "filter_iter_10_components",
        Filter_filter_iter_10_components
    },
    {
        "filter_iter_20_components",
        Filter_filter_iter_20_components
    },
    {
        "filter_iter_superset",
        Filter_filter_iter_superset
    },
    {
        "filter_iter_superset_childof",
        Filter_filter_iter_superset_childof
    },
    {
        "filter_iter_type_set",
        Filter_filter_iter_type_set
    },
    {
        "filter_iter_w_readonly_term",
        Filter_filter_iter_w_readonly_term
    },
    {
        "filter_iter_w_from_nothing_term",
        Filter_filter_iter_w_from_nothing_term
    },
    {
        "filter_iter_pair_w_rel_wildcard_n_matches",
        Filter_filter_iter_pair_w_rel_wildcard_n_matches
    },
    {
        "filter_iter_pair_w_obj_wildcard_n_matches",
        Filter_filter_iter_pair_w_obj_wildcard_n_matches
    },
    {
        "filter_iter_pair_w_2_wildcards_1_match",
        Filter_filter_iter_pair_w_2_wildcards_1_match
    },
    {
        "filter_iter_pair_w_2_wildcards_2x1_matches",
        Filter_filter_iter_pair_w_2_wildcards_2x1_matches
    },
    {
        "filter_iter_pair_w_2_wildcards_2x2_matches",
        Filter_filter_iter_pair_w_2_wildcards_2x2_matches
    },
    {
        "filter_iter_pair_w_3_wildcards_2x2x2_matches",
        Filter_filter_iter_pair_w_3_wildcards_2x2x2_matches
    },
    {
        "match_disabled",
        Filter_match_disabled
    },
    {
        "match_prefab",
        Filter_match_prefab
    },
    {
        "chain_term_iter",
        Filter_chain_term_iter
    },
    {
        "chain_filter_iter",
        Filter_chain_filter_iter
    },
    {
        "chain_query_iter",
        Filter_chain_query_iter
    },
    {
        "chain_rule_iter",
        Filter_chain_rule_iter
    },
    {
        "chain_iter_2_levels",
        Filter_chain_iter_2_levels
    },
    {
        "filter_from_expr_2_terms_err",
        Filter_filter_from_expr_2_terms_err
    },
    {
        "chain_term_iter_w_term_iter",
        Filter_chain_term_iter_w_term_iter
    },
    {
        "chain_filter_iter_w_term_iter",
        Filter_chain_filter_iter_w_term_iter
    },
    {
        "chain_w_term_iter_component",
        Filter_chain_w_term_iter_component
    },
    {
        "filter_w_recycled_object_and_id",
        Filter_filter_w_recycled_object_and_id
    },
    {
        "term_iter_w_filter_term",
        Filter_term_iter_w_filter_term
    },
    {
        "filter_iter_w_filter_term",
        Filter_filter_iter_w_filter_term
    },
    {
        "filter_iter_w_2_terms_1_filter",
        Filter_filter_iter_w_2_terms_1_filter
    },
    {
        "filter_iter_w_3_terms_2_filter",
        Filter_filter_iter_w_3_terms_2_filter
    },
    {
        "filter_iter_2_terms_filter_all",
        Filter_filter_iter_2_terms_filter_all
    },
    {
        "filter_iter_2_terms_filter_all_w_out",
        Filter_filter_iter_2_terms_filter_all_w_out
    },
    {
        "filter_instanced_w_singleton",
        Filter_filter_instanced_w_singleton
    },
    {
        "filter_instanced_w_base",
        Filter_filter_instanced_w_base
    },
    {
        "filter_no_instancing_w_singleton",
        Filter_filter_no_instancing_w_singleton
    },
    {
        "filter_no_instancing_w_base",
        Filter_filter_no_instancing_w_base
    },
    {
        "filter_no_this_tag",
        Filter_filter_no_this_tag
    },
    {
        "filter_no_this_component",
        Filter_filter_no_this_component
    },
    {
        "filter_no_this_tag_2_ents",
        Filter_filter_no_this_tag_2_ents
    },
    {
        "filter_no_this_component_2_ents",
        Filter_filter_no_this_component_2_ents
    },
    {
        "filter_no_this_tag_2_ents_1_not",
        Filter_filter_no_this_tag_2_ents_1_not
    },
    {
        "filter_no_this_component_2_ents_1_not",
        Filter_filter_no_this_component_2_ents_1_not
    },
    {
        "filter_no_this_component_1_not",
        Filter_filter_no_this_component_1_not
    },
    {
        "filter_iter_frame_offset",
        Filter_filter_iter_frame_offset
    },
    {
        "filter_1_term_no_alloc",
        Filter_filter_1_term_no_alloc
    },
    {
        "filter_cache_size_terms_no_alloc",
        Filter_filter_cache_size_terms_no_alloc
    },
    {
        "filter_lt_cache_size_terms_no_alloc",
        Filter_filter_lt_cache_size_terms_no_alloc
    },
    {
        "move_self",
        Filter_move_self
    },
    {
        "match_empty_tables",
        Filter_match_empty_tables
    },
    {
        "match_switch_w_switch",
        Filter_match_switch_w_switch
    },
    {
        "match_switch_w_case",
        Filter_match_switch_w_case
    },
    {
        "match_switch_w_case_2_terms",
        Filter_match_switch_w_case_2_terms
    },
    {
        "and_term",
        Filter_and_term
    },
    {
        "or_term",
        Filter_or_term
    },
    {
        "iter_while_creating_components",
        Filter_iter_while_creating_components
    }
};

bake_test_case Query_testcases[] = {
    {
        "query_changed_after_new",
        Query_query_changed_after_new
    },
    {
        "query_changed_after_delete",
        Query_query_changed_after_delete
    },
    {
        "query_changed_after_add",
        Query_query_changed_after_add
    },
    {
        "query_changed_after_remove",
        Query_query_changed_after_remove
    },
    {
        "query_changed_after_set",
        Query_query_changed_after_set
    },
    {
        "query_change_after_modified",
        Query_query_change_after_modified
    },
    {
        "query_change_after_out_system",
        Query_query_change_after_out_system
    },
    {
        "query_change_after_in_system",
        Query_query_change_after_in_system
    },
    {
        "query_change_after_modified_out_term",
        Query_query_change_after_modified_out_term
    },
    {
        "query_change_check_iter",
        Query_query_change_check_iter
    },
    {
        "query_change_check_iter_after_skip_read",
        Query_query_change_check_iter_after_skip_read
    },
    {
        "query_change_check_iter_after_skip_write",
        Query_query_change_check_iter_after_skip_write
    },
    {
        "query_change_parent_term",
        Query_query_change_parent_term
    },
    {
        "query_change_prefab_term",
        Query_query_change_prefab_term
    },
    {
        "subquery_match_existing",
        Query_subquery_match_existing
    },
    {
        "subquery_match_new",
        Query_subquery_match_new
    },
    {
        "subquery_inactive",
        Query_subquery_inactive
    },
    {
        "subquery_unmatch",
        Query_subquery_unmatch
    },
    {
        "subquery_rematch",
        Query_subquery_rematch
    },
    {
        "subquery_rematch_w_parent_optional",
        Query_subquery_rematch_w_parent_optional
    },
    {
        "subquery_rematch_w_sub_optional",
        Query_subquery_rematch_w_sub_optional
    },
    {
        "query_single_pairs",
        Query_query_single_pairs
    },
    {
        "query_single_instanceof",
        Query_query_single_instanceof
    },
    {
        "query_single_childof",
        Query_query_single_childof
    },
    {
        "query_optional_owned",
        Query_query_optional_owned
    },
    {
        "query_optional_shared",
        Query_query_optional_shared
    },
    {
        "query_optional_shared_nested",
        Query_query_optional_shared_nested
    },
    {
        "query_optional_any",
        Query_query_optional_any
    },
    {
        "query_rematch_optional_after_add",
        Query_query_rematch_optional_after_add
    },
    {
        "get_owned_tag",
        Query_get_owned_tag
    },
    {
        "get_shared_tag",
        Query_get_shared_tag
    },
    {
        "explicit_delete",
        Query_explicit_delete
    },
    {
        "get_column_size",
        Query_get_column_size
    },
    {
        "orphaned_query",
        Query_orphaned_query
    },
    {
        "nested_orphaned_query",
        Query_nested_orphaned_query
    },
    {
        "invalid_access_orphaned_query",
        Query_invalid_access_orphaned_query
    },
    {
        "stresstest_query_free",
        Query_stresstest_query_free
    },
    {
        "only_from_entity",
        Query_only_from_entity
    },
    {
        "only_from_singleton",
        Query_only_from_singleton
    },
    {
        "only_not_from_entity",
        Query_only_not_from_entity
    },
    {
        "only_not_from_singleton",
        Query_only_not_from_singleton
    },
    {
        "get_filter",
        Query_get_filter
    },
    {
        "group_by",
        Query_group_by
    },
    {
        "group_by_w_ctx",
        Query_group_by_w_ctx
    },
    {
        "group_by_w_sort_reverse_group_creation",
        Query_group_by_w_sort_reverse_group_creation
    },
    {
        "iter_valid",
        Query_iter_valid
    },
    {
        "query_optional_tag",
        Query_query_optional_tag
    },
    {
        "query_optional_shared_tag",
        Query_query_optional_shared_tag
    },
    {
        "query_iter_10_tags",
        Query_query_iter_10_tags
    },
    {
        "query_iter_20_tags",
        Query_query_iter_20_tags
    },
    {
        "query_iter_10_components",
        Query_query_iter_10_components
    },
    {
        "query_iter_20_components",
        Query_query_iter_20_components
    },
    {
        "iter_type_set",
        Query_iter_type_set
    },
    {
        "only_optional_from_entity",
        Query_only_optional_from_entity
    },
    {
        "only_optional_from_entity_no_match",
        Query_only_optional_from_entity_no_match
    },
    {
        "filter_term",
        Query_filter_term
    },
    {
        "2_terms_1_filter",
        Query_2_terms_1_filter
    },
    {
        "3_terms_2_filter",
        Query_3_terms_2_filter
    },
    {
        "no_instancing_w_singleton",
        Query_no_instancing_w_singleton
    },
    {
        "no_instancing_w_shared",
        Query_no_instancing_w_shared
    },
    {
        "query_iter_frame_offset",
        Query_query_iter_frame_offset
    }
};

bake_test_case Iter_testcases[] = {
    {
        "page_iter_0_0",
        Iter_page_iter_0_0
    },
    {
        "page_iter_1_0",
        Iter_page_iter_1_0
    },
    {
        "page_iter_0_1",
        Iter_page_iter_0_1
    },
    {
        "page_iter_n_0",
        Iter_page_iter_n_0
    },
    {
        "page_iter_0_n",
        Iter_page_iter_0_n
    },
    {
        "page_iter_m_n",
        Iter_page_iter_m_n
    },
    {
        "page_iter_skip_1_table",
        Iter_page_iter_skip_1_table
    },
    {
        "page_iter_skip_2_tables",
        Iter_page_iter_skip_2_tables
    },
    {
        "worker_iter_1",
        Iter_worker_iter_1
    },
    {
        "worker_iter_2",
        Iter_worker_iter_2
    },
    {
        "worker_iter_3",
        Iter_worker_iter_3
    },
    {
        "worker_iter_4",
        Iter_worker_iter_4
    },
    {
        "paged_iter_w_shared_comp",
        Iter_paged_iter_w_shared_comp
    },
    {
        "worker_iter_w_shared_comp",
        Iter_worker_iter_w_shared_comp
    },
    {
        "paged_iter_w_task_query",
        Iter_paged_iter_w_task_query
    },
    {
        "worker_iter_w_task_query",
        Iter_worker_iter_w_task_query
    },
    {
        "worker_iter_w_singleton",
        Iter_worker_iter_w_singleton
    },
    {
        "worker_iter_w_singleton_instanced",
        Iter_worker_iter_w_singleton_instanced
    },
    {
        "paged_iter_w_singleton",
        Iter_paged_iter_w_singleton
    },
    {
        "paged_iter_w_singleton_instanced",
        Iter_paged_iter_w_singleton_instanced
    },
    {
        "iter_1_term_no_alloc",
        Iter_iter_1_term_no_alloc
    },
    {
        "iter_cache_size_terms_no_alloc",
        Iter_iter_cache_size_terms_no_alloc
    },
    {
        "iter_lt_cache_size_terms_alloc",
        Iter_iter_lt_cache_size_terms_alloc
    }
};

bake_test_case Pairs_testcases[] = {
    {
        "type_w_one_pair",
        Pairs_type_w_one_pair
    },
    {
        "type_w_two_pairs",
        Pairs_type_w_two_pairs
    },
    {
        "add_pair",
        Pairs_add_pair
    },
    {
        "remove_pair",
        Pairs_remove_pair
    },
    {
        "add_tag_pair_for_tag",
        Pairs_add_tag_pair_for_tag
    },
    {
        "add_tag_pair_for_component",
        Pairs_add_tag_pair_for_component
    },
    {
        "query_2_pairs",
        Pairs_query_2_pairs
    },
    {
        "query_2_pairs_2_instances_per_type",
        Pairs_query_2_pairs_2_instances_per_type
    },
    {
        "query_pair_or_component",
        Pairs_query_pair_or_component
    },
    {
        "query_pair_or_pair",
        Pairs_query_pair_or_pair
    },
    {
        "query_not_pair",
        Pairs_query_not_pair
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
        "pair_w_component_query",
        Pairs_pair_w_component_query
    },
    {
        "on_add_pair",
        Pairs_on_add_pair
    },
    {
        "on_add_pair_tag",
        Pairs_on_add_pair_tag
    },
    {
        "on_remove_pair",
        Pairs_on_remove_pair
    },
    {
        "on_remove_pair_tag",
        Pairs_on_remove_pair_tag
    },
    {
        "on_remove_pair_on_delete",
        Pairs_on_remove_pair_on_delete
    },
    {
        "on_remove_pair_tag_on_delete",
        Pairs_on_remove_pair_tag_on_delete
    },
    {
        "get_typeid_w_recycled_rel",
        Pairs_get_typeid_w_recycled_rel
    },
    {
        "get_typeid_w_recycled_obj",
        Pairs_get_typeid_w_recycled_obj
    },
    {
        "id_str_w_recycled_rel",
        Pairs_id_str_w_recycled_rel
    },
    {
        "id_str_w_recycled_obj",
        Pairs_id_str_w_recycled_obj
    },
    {
        "set_object_w_zero_sized_rel_comp",
        Pairs_set_object_w_zero_sized_rel_comp
    },
    {
        "dsl_pair",
        Pairs_dsl_pair
    },
    {
        "dsl_pair_w_pred_wildcard",
        Pairs_dsl_pair_w_pred_wildcard
    },
    {
        "dsl_pair_w_obj_wildcard",
        Pairs_dsl_pair_w_obj_wildcard
    },
    {
        "dsl_pair_w_both_wildcard",
        Pairs_dsl_pair_w_both_wildcard
    },
    {
        "dsl_pair_w_explicit_subj_this",
        Pairs_dsl_pair_w_explicit_subj_this
    },
    {
        "dsl_pair_w_explicit_subj",
        Pairs_dsl_pair_w_explicit_subj
    },
    {
        "api_pair",
        Pairs_api_pair
    },
    {
        "api_pair_w_pred_wildcard",
        Pairs_api_pair_w_pred_wildcard
    },
    {
        "api_pair_w_obj_wildcard",
        Pairs_api_pair_w_obj_wildcard
    },
    {
        "api_pair_w_both_wildcard",
        Pairs_api_pair_w_both_wildcard
    },
    {
        "api_pair_w_explicit_subj_this",
        Pairs_api_pair_w_explicit_subj_this
    },
    {
        "api_pair_w_explicit_subj",
        Pairs_api_pair_w_explicit_subj
    },
    {
        "typeid_from_tag",
        Pairs_typeid_from_tag
    },
    {
        "typeid_from_component",
        Pairs_typeid_from_component
    },
    {
        "typeid_from_pair",
        Pairs_typeid_from_pair
    },
    {
        "typeid_from_pair_w_rel_type",
        Pairs_typeid_from_pair_w_rel_type
    },
    {
        "typeid_from_pair_w_obj_type",
        Pairs_typeid_from_pair_w_obj_type
    },
    {
        "typeid_from_pair_w_rel_obj_type",
        Pairs_typeid_from_pair_w_rel_obj_type
    },
    {
        "typeid_from_pair_w_rel_0_obj_type",
        Pairs_typeid_from_pair_w_rel_0_obj_type
    },
    {
        "typeid_from_pair_w_rel_obj_0_type",
        Pairs_typeid_from_pair_w_rel_obj_0_type
    },
    {
        "typeid_from_pair_w_rel_0_obj_0_type",
        Pairs_typeid_from_pair_w_rel_0_obj_0_type
    },
    {
        "typeid_from_pair_with_0_obj",
        Pairs_typeid_from_pair_with_0_obj
    },
    {
        "tag_pair_w_rel_comp",
        Pairs_tag_pair_w_rel_comp
    },
    {
        "tag_pair_w_obj_comp",
        Pairs_tag_pair_w_obj_comp
    },
    {
        "tag_pair_w_rel_obj_comp",
        Pairs_tag_pair_w_rel_obj_comp
    },
    {
        "get_tag_pair_w_rel_comp",
        Pairs_get_tag_pair_w_rel_comp
    },
    {
        "get_tag_pair_w_obj_comp",
        Pairs_get_tag_pair_w_obj_comp
    },
    {
        "get_tag_pair_w_rel_obj_comp",
        Pairs_get_tag_pair_w_rel_obj_comp
    },
    {
        "tag_pair_w_childof_w_comp",
        Pairs_tag_pair_w_childof_w_comp
    },
    {
        "tag_pair_w_isa_w_comp",
        Pairs_tag_pair_w_isa_w_comp
    },
    {
        "get_1_object",
        Pairs_get_1_object
    },
    {
        "get_1_object_not_found",
        Pairs_get_1_object_not_found
    },
    {
        "get_n_objects",
        Pairs_get_n_objects
    },
    {
        "get_object_for_id_from_self",
        Pairs_get_object_for_id_from_self
    },
    {
        "get_object_for_id_from_base",
        Pairs_get_object_for_id_from_base
    },
    {
        "get_object_for_id_from_nested_base",
        Pairs_get_object_for_id_from_nested_base
    },
    {
        "get_object_for_id_not_found",
        Pairs_get_object_for_id_not_found
    },
    {
        "get_object_for_wildcard_from_self",
        Pairs_get_object_for_wildcard_from_self
    },
    {
        "get_object_for_wildcard_from_base",
        Pairs_get_object_for_wildcard_from_base
    },
    {
        "get_object_for_wildcard_from_nested_base",
        Pairs_get_object_for_wildcard_from_nested_base
    },
    {
        "ignore_childof_from_base",
        Pairs_ignore_childof_from_base
    },
    {
        "add_exclusive_relation_twice",
        Pairs_add_exclusive_relation_twice
    },
    {
        "add_same_exclusive_relation_twice",
        Pairs_add_same_exclusive_relation_twice
    },
    {
        "set_exclusive_relation_twice",
        Pairs_set_exclusive_relation_twice
    },
    {
        "add_exclusive_non_empty_table",
        Pairs_add_exclusive_non_empty_table
    },
    {
        "add_exclusive_non_empty_table_w_pairs",
        Pairs_add_exclusive_non_empty_table_w_pairs
    },
    {
        "add_pair_to_entity_w_exclusive_pair",
        Pairs_add_pair_to_entity_w_exclusive_pair
    },
    {
        "add_pair_to_entity_w_scope",
        Pairs_add_pair_to_entity_w_scope
    },
    {
        "add_existing_exclusive_pair_after_pair",
        Pairs_add_existing_exclusive_pair_after_pair
    },
    {
        "add_symmetric_relation",
        Pairs_add_symmetric_relation
    },
    {
        "remove_symmetric_relation",
        Pairs_remove_symmetric_relation
    },
    {
        "delete_entity_w_symmetric_relation",
        Pairs_delete_entity_w_symmetric_relation
    },
    {
        "add_symmetric_exclusive_relation",
        Pairs_add_symmetric_exclusive_relation
    },
    {
        "with",
        Pairs_with
    },
    {
        "2_with",
        Pairs_2_with
    },
    {
        "nested_with",
        Pairs_nested_with
    },
    {
        "with_relation",
        Pairs_with_relation
    },
    {
        "2_with_relation",
        Pairs_2_with_relation
    },
    {
        "nested_with_relation",
        Pairs_nested_with_relation
    },
    {
        "65k_relations",
        Pairs_65k_relations
    },
    {
        "add_not_alive_relation",
        Pairs_add_not_alive_relation
    },
    {
        "remove_wildcard",
        Pairs_remove_wildcard
    },
    {
        "remove_relation_wildcard",
        Pairs_remove_relation_wildcard
    },
    {
        "remove_wildcard_all",
        Pairs_remove_wildcard_all
    },
    {
        "inherit_exclusive",
        Pairs_inherit_exclusive
    },
    {
        "dont_inherit",
        Pairs_dont_inherit
    }
};

bake_test_case Rules_testcases[] = {
    {
        "empty_rule",
        Rules_empty_rule
    },
    {
        "invalid_rule",
        Rules_invalid_rule
    },
    {
        "comp_recycled_final_id",
        Rules_comp_recycled_final_id
    },
    {
        "comp_recycled_id",
        Rules_comp_recycled_id
    },
    {
        "pair_recycled_final_pred",
        Rules_pair_recycled_final_pred
    },
    {
        "pair_recycled_pred",
        Rules_pair_recycled_pred
    },
    {
        "pair_recycled_obj",
        Rules_pair_recycled_obj
    },
    {
        "pair_recycled_pred_obj",
        Rules_pair_recycled_pred_obj
    },
    {
        "pair_recycled_matched_obj",
        Rules_pair_recycled_matched_obj
    },
    {
        "pair_recycled_matched_obj_2_terms",
        Rules_pair_recycled_matched_obj_2_terms
    },
    {
        "pair_recycled_matched_pred_2_terms",
        Rules_pair_recycled_matched_pred_2_terms
    },
    {
        "1_comp",
        Rules_1_comp
    },
    {
        "2_comp",
        Rules_2_comp
    },
    {
        "1_comp_explicit_subject",
        Rules_1_comp_explicit_subject
    },
    {
        "2_comp_explicit_subject",
        Rules_2_comp_explicit_subject
    },
    {
        "1_fact_true",
        Rules_1_fact_true
    },
    {
        "1_fact_false",
        Rules_1_fact_false
    },
    {
        "2_facts_true",
        Rules_2_facts_true
    },
    {
        "2_facts_1_false",
        Rules_2_facts_1_false
    },
    {
        "2_facts_false",
        Rules_2_facts_false
    },
    {
        "1_fact_pair_true",
        Rules_1_fact_pair_true
    },
    {
        "1_fact_pair_false",
        Rules_1_fact_pair_false
    },
    {
        "2_fact_pairs_true",
        Rules_2_fact_pairs_true
    },
    {
        "2_fact_pairs_1_false",
        Rules_2_fact_pairs_1_false
    },
    {
        "2_fact_pairs_false",
        Rules_2_fact_pairs_false
    },
    {
        "wildcard_pred",
        Rules_wildcard_pred
    },
    {
        "wildcard_subj",
        Rules_wildcard_subj
    },
    {
        "wildcard_obj",
        Rules_wildcard_obj
    },
    {
        "any_pred",
        Rules_any_pred
    },
    {
        "any_subj",
        Rules_any_subj
    },
    {
        "any_obj",
        Rules_any_obj
    },
    {
        "2_wildcard_as_subject",
        Rules_2_wildcard_as_subject
    },
    {
        "find_1_pair",
        Rules_find_1_pair
    },
    {
        "find_2_pairs",
        Rules_find_2_pairs
    },
    {
        "find_w_pred_var",
        Rules_find_w_pred_var
    },
    {
        "find_w_pred_var_explicit_subject",
        Rules_find_w_pred_var_explicit_subject
    },
    {
        "find_1_pair_w_object_var",
        Rules_find_1_pair_w_object_var
    },
    {
        "find_2_pairs_w_object_var",
        Rules_find_2_pairs_w_object_var
    },
    {
        "find_1_pair_w_pred_var",
        Rules_find_1_pair_w_pred_var
    },
    {
        "find_2_pairs_w_pred_var",
        Rules_find_2_pairs_w_pred_var
    },
    {
        "find_cyclic_pairs",
        Rules_find_cyclic_pairs
    },
    {
        "join_by_object",
        Rules_join_by_object
    },
    {
        "join_by_predicate",
        Rules_join_by_predicate
    },
    {
        "join_by_predicate_from_subject",
        Rules_join_by_predicate_from_subject
    },
    {
        "find_transitive",
        Rules_find_transitive
    },
    {
        "find_transitive_2_branches",
        Rules_find_transitive_2_branches
    },
    {
        "transitive_subsets",
        Rules_transitive_subsets
    },
    {
        "transitive_subsets_2_terms",
        Rules_transitive_subsets_2_terms
    },
    {
        "transitive_w_table_object",
        Rules_transitive_w_table_object
    },
    {
        "transitive_supersets",
        Rules_transitive_supersets
    },
    {
        "transitive_superset_w_subj_var",
        Rules_transitive_superset_w_subj_var
    },
    {
        "transitive_superset_w_subj_var_2_term",
        Rules_transitive_superset_w_subj_var_2_term
    },
    {
        "transitive_supersets_2_terms",
        Rules_transitive_supersets_2_terms
    },
    {
        "transitive_constraint_on_superset_var",
        Rules_transitive_constraint_on_superset_var
    },
    {
        "transitive_instances",
        Rules_transitive_instances
    },
    {
        "transitive_instances_2_terms",
        Rules_transitive_instances_2_terms
    },
    {
        "transitive_implicit_isa_instances",
        Rules_transitive_implicit_isa_instances
    },
    {
        "transitive_implicit_isa_instances_2_terms",
        Rules_transitive_implicit_isa_instances_2_terms
    },
    {
        "transitive_fact_false",
        Rules_transitive_fact_false
    },
    {
        "transitive_fact_true_depth_1",
        Rules_transitive_fact_true_depth_1
    },
    {
        "transitive_fact_true_depth_2",
        Rules_transitive_fact_true_depth_2
    },
    {
        "transitive_fact_true_depth_3",
        Rules_transitive_fact_true_depth_3
    },
    {
        "transitive_fact_true_depth_4",
        Rules_transitive_fact_true_depth_4
    },
    {
        "transitive_fact_true_depth_5",
        Rules_transitive_fact_true_depth_5
    },
    {
        "transitive_fact_true_2_relationships",
        Rules_transitive_fact_true_2_relationships
    },
    {
        "transitive_fact_same_subj_obj",
        Rules_transitive_fact_same_subj_obj
    },
    {
        "transitive_fact_subset_superset",
        Rules_transitive_fact_subset_superset
    },
    {
        "transitive_nonfinal_fact",
        Rules_transitive_nonfinal_fact
    },
    {
        "transitive_nonfinal_fact_w_implicit_pred_subset",
        Rules_transitive_nonfinal_fact_w_implicit_pred_subset
    },
    {
        "transitive_all",
        Rules_transitive_all
    },
    {
        "same_pred_obj",
        Rules_same_pred_obj
    },
    {
        "same_pred_obj_explicit_subject",
        Rules_same_pred_obj_explicit_subject
    },
    {
        "implicit_is_a_tag_fact",
        Rules_implicit_is_a_tag_fact
    },
    {
        "implicit_is_a_tag_fact_2_lvls",
        Rules_implicit_is_a_tag_fact_2_lvls
    },
    {
        "implicit_is_a_tag_fact_3_lvls",
        Rules_implicit_is_a_tag_fact_3_lvls
    },
    {
        "implicit_is_a_tag_fact_2_lvls_both_matching",
        Rules_implicit_is_a_tag_fact_2_lvls_both_matching
    },
    {
        "implicit_is_a_tag_fact_2_lvls_entity_matching",
        Rules_implicit_is_a_tag_fact_2_lvls_entity_matching
    },
    {
        "implicit_is_a_pair_fact",
        Rules_implicit_is_a_pair_fact
    },
    {
        "implicit_is_a_pair_2_lvls",
        Rules_implicit_is_a_pair_2_lvls
    },
    {
        "implicit_is_a_pair_3_lvls",
        Rules_implicit_is_a_pair_3_lvls
    },
    {
        "implicit_is_a_pair_2_lvls_both_matching",
        Rules_implicit_is_a_pair_2_lvls_both_matching
    },
    {
        "implicit_is_a_pair_2_lvls_entity_matching",
        Rules_implicit_is_a_pair_2_lvls_entity_matching
    },
    {
        "implicit_is_a_pred_var",
        Rules_implicit_is_a_pred_var
    },
    {
        "implicit_is_a_pair_var",
        Rules_implicit_is_a_pair_var
    },
    {
        "implicit_is_a_transitive_pair_fact",
        Rules_implicit_is_a_transitive_pair_fact
    },
    {
        "implicit_is_a_transitive_pair_fact_w_implicit_pred_subset",
        Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_subset
    },
    {
        "implicit_is_a_transitive_pair_fact_w_implicit_pred_obj",
        Rules_implicit_is_a_transitive_pair_fact_w_implicit_pred_obj
    },
    {
        "2_constrained_vars_by_subject_literal",
        Rules_2_constrained_vars_by_subject_literal
    },
    {
        "2_constrained_vars_by_subject_literal_2_var_terms",
        Rules_2_constrained_vars_by_subject_literal_2_var_terms
    },
    {
        "term_w_nothing_set",
        Rules_term_w_nothing_set
    },
    {
        "term_w_nothing_set_w_this_term",
        Rules_term_w_nothing_set_w_this_term
    },
    {
        "comp_w_not_term",
        Rules_comp_w_not_term
    },
    {
        "not_term_w_subj_var",
        Rules_not_term_w_subj_var
    },
    {
        "not_term_w_subj_var_match_n_per_type",
        Rules_not_term_w_subj_var_match_n_per_type
    },
    {
        "invalid_rule_w_only_not_term",
        Rules_invalid_rule_w_only_not_term
    },
    {
        "invalid_rule_w_not_term_unknown_var",
        Rules_invalid_rule_w_not_term_unknown_var
    },
    {
        "invalid_rule_w_not_term_unknown_pair_var",
        Rules_invalid_rule_w_not_term_unknown_pair_var
    },
    {
        "invalid_rule_w_not_term_unknown_pair_var_subj_var",
        Rules_invalid_rule_w_not_term_unknown_pair_var_subj_var
    },
    {
        "rules_w_desc_id",
        Rules_rules_w_desc_id
    },
    {
        "rules_w_desc_pair",
        Rules_rules_w_desc_pair
    },
    {
        "rules_w_desc_pair_empty_rel_obj",
        Rules_rules_w_desc_pair_empty_rel_obj
    },
    {
        "rules_w_desc_pair_pred_obj",
        Rules_rules_w_desc_pair_pred_obj
    },
    {
        "rules_w_desc_wildcard",
        Rules_rules_w_desc_wildcard
    },
    {
        "childof_0",
        Rules_childof_0
    },
    {
        "childof_this",
        Rules_childof_this
    },
    {
        "childof_this_as_identifier",
        Rules_childof_this_as_identifier
    },
    {
        "optional_term",
        Rules_optional_term
    },
    {
        "optional_term_w_component",
        Rules_optional_term_w_component
    },
    {
        "optional_term_on_entity",
        Rules_optional_term_on_entity
    },
    {
        "optional_term_on_variable",
        Rules_optional_term_on_variable
    },
    {
        "optional_term_on_wildcard",
        Rules_optional_term_on_wildcard
    },
    {
        "optional_term_on_relation_this_obj",
        Rules_optional_term_on_relation_this_obj
    },
    {
        "optional_w_subj_var",
        Rules_optional_w_subj_var
    },
    {
        "terms_set",
        Rules_terms_set
    },
    {
        "value_set",
        Rules_value_set
    },
    {
        "term_w_this_this_this",
        Rules_term_w_this_this_this
    },
    {
        "term_w_x_x_x",
        Rules_term_w_x_x_x
    },
    {
        "term_w_same_subj_obj_var",
        Rules_term_w_same_subj_obj_var
    },
    {
        "filter_term",
        Rules_filter_term
    },
    {
        "2_terms_1_filter",
        Rules_2_terms_1_filter
    },
    {
        "3_terms_2_filter",
        Rules_3_terms_2_filter
    },
    {
        "term_obj_w_this",
        Rules_term_obj_w_this
    },
    {
        "term_subj_w_this",
        Rules_term_subj_w_this
    },
    {
        "rule_iter_frame_offset",
        Rules_rule_iter_frame_offset
    },
    {
        "rule_iter_set_var",
        Rules_rule_iter_set_var
    },
    {
        "rule_iter_set_2_vars",
        Rules_rule_iter_set_2_vars
    },
    {
        "rule_iter_set_pred_var",
        Rules_rule_iter_set_pred_var
    },
    {
        "rule_iter_set_var_for_2_terms",
        Rules_rule_iter_set_var_for_2_terms
    },
    {
        "rule_iter_set_cyclic_variable",
        Rules_rule_iter_set_cyclic_variable
    },
    {
        "rule_iter_set_cyclic_variable_w_this",
        Rules_rule_iter_set_cyclic_variable_w_this
    }
};

bake_test_case TransitiveRules_testcases[] = {
    {
        "trans_X_X",
        TransitiveRules_trans_X_X
    },
    {
        "trans_reflexive_X_X",
        TransitiveRules_trans_reflexive_X_X
    },
    {
        "trans_reflexive_X_X_2",
        TransitiveRules_trans_reflexive_X_X_2
    },
    {
        "trans_reflexive_X_Y",
        TransitiveRules_trans_reflexive_X_Y
    },
    {
        "trans_X_Y_2_levels",
        TransitiveRules_trans_X_Y_2_levels
    },
    {
        "trans_X_Y_2_levels_nonfinal",
        TransitiveRules_trans_X_Y_2_levels_nonfinal
    },
    {
        "trans_pred_This_X__pred_X",
        TransitiveRules_trans_pred_This_X__pred_X
    },
    {
        "trans_constrained_x_y",
        TransitiveRules_trans_constrained_x_y
    },
    {
        "trans_entity_X_non_inclusive",
        TransitiveRules_trans_entity_X_non_inclusive
    },
    {
        "trans_X_entity_non_inclusive",
        TransitiveRules_trans_X_entity_non_inclusive
    },
    {
        "trans_entity_entity_non_reflexive",
        TransitiveRules_trans_entity_entity_non_reflexive
    },
    {
        "trans_this_x_after_tag_this",
        TransitiveRules_trans_this_x_after_tag_this
    },
    {
        "trans_this_x_before_tag_this",
        TransitiveRules_trans_this_x_before_tag_this
    },
    {
        "trans_this_x_after_tag_this_2_lvls",
        TransitiveRules_trans_this_x_after_tag_this_2_lvls
    },
    {
        "trans_this_x_before_tag_this_2_lvls",
        TransitiveRules_trans_this_x_before_tag_this_2_lvls
    },
    {
        "transitive_not_w_var",
        TransitiveRules_transitive_not_w_var
    },
    {
        "transitive_w_not_nonfinal",
        TransitiveRules_transitive_w_not_nonfinal
    },
    {
        "transitive_w_optional_nonfinal",
        TransitiveRules_transitive_w_optional_nonfinal
    },
    {
        "transitive_w_optional_nonfinal_w_var",
        TransitiveRules_transitive_w_optional_nonfinal_w_var
    },
    {
        "rule_iter_set_transitive_variable",
        TransitiveRules_rule_iter_set_transitive_variable
    },
    {
        "rule_iter_set_transitive_self_variable",
        TransitiveRules_rule_iter_set_transitive_self_variable
    },
    {
        "rule_iter_set_transitive_2_variables_set_one",
        TransitiveRules_rule_iter_set_transitive_2_variables_set_one
    },
    {
        "rule_iter_set_transitive_2_variables_set_both",
        TransitiveRules_rule_iter_set_transitive_2_variables_set_both
    },
    {
        "rule_iter_set_transitive_self_2_variables_set_both",
        TransitiveRules_rule_iter_set_transitive_self_2_variables_set_both
    }
};

bake_test_case Trigger_testcases[] = {
    {
        "on_add_trigger_before_table",
        Trigger_on_add_trigger_before_table
    },
    {
        "on_add_trigger_after_table",
        Trigger_on_add_trigger_after_table
    },
    {
        "on_remove_trigger_before_table",
        Trigger_on_remove_trigger_before_table
    },
    {
        "on_remove_trigger_after_table",
        Trigger_on_remove_trigger_after_table
    },
    {
        "on_add_tag",
        Trigger_on_add_tag
    },
    {
        "on_add_component",
        Trigger_on_add_component
    },
    {
        "on_add_wildcard",
        Trigger_on_add_wildcard
    },
    {
        "on_add_pair",
        Trigger_on_add_pair
    },
    {
        "on_add_pair_obj_wildcard",
        Trigger_on_add_pair_obj_wildcard
    },
    {
        "on_add_pair_pred_wildcard",
        Trigger_on_add_pair_pred_wildcard
    },
    {
        "on_add_pair_wildcard",
        Trigger_on_add_pair_wildcard
    },
    {
        "on_remove_tag",
        Trigger_on_remove_tag
    },
    {
        "on_remove_component",
        Trigger_on_remove_component
    },
    {
        "on_remove_wildcard",
        Trigger_on_remove_wildcard
    },
    {
        "on_remove_pair",
        Trigger_on_remove_pair
    },
    {
        "on_remove_pair_obj_wildcard",
        Trigger_on_remove_pair_obj_wildcard
    },
    {
        "on_remove_pair_pred_wildcard",
        Trigger_on_remove_pair_pred_wildcard
    },
    {
        "on_remove_pair_wildcard",
        Trigger_on_remove_pair_wildcard
    },
    {
        "wildcard_pair_w_pred_component",
        Trigger_wildcard_pair_w_pred_component
    },
    {
        "wildcard_pair_w_obj_component",
        Trigger_wildcard_pair_w_obj_component
    },
    {
        "on_add_remove",
        Trigger_on_add_remove
    },
    {
        "on_set_component",
        Trigger_on_set_component
    },
    {
        "on_set_wildcard",
        Trigger_on_set_wildcard
    },
    {
        "on_set_pair",
        Trigger_on_set_pair
    },
    {
        "on_set_pair_w_obj_wildcard",
        Trigger_on_set_pair_w_obj_wildcard
    },
    {
        "on_set_pair_pred_wildcard",
        Trigger_on_set_pair_pred_wildcard
    },
    {
        "on_set_pair_wildcard",
        Trigger_on_set_pair_wildcard
    },
    {
        "on_set_component_after_modified",
        Trigger_on_set_component_after_modified
    },
    {
        "un_set_component",
        Trigger_un_set_component
    },
    {
        "un_set_wildcard",
        Trigger_un_set_wildcard
    },
    {
        "un_set_pair",
        Trigger_un_set_pair
    },
    {
        "un_set_pair_w_obj_wildcard",
        Trigger_un_set_pair_w_obj_wildcard
    },
    {
        "un_set_pair_pred_wildcard",
        Trigger_un_set_pair_pred_wildcard
    },
    {
        "un_set_pair_wildcard",
        Trigger_un_set_pair_wildcard
    },
    {
        "on_add_not_tag",
        Trigger_on_add_not_tag
    },
    {
        "on_remove_not_tag",
        Trigger_on_remove_not_tag
    },
    {
        "on_add_superset",
        Trigger_on_add_superset
    },
    {
        "on_add_superset_2_levels",
        Trigger_on_add_superset_2_levels
    },
    {
        "on_remove_superset",
        Trigger_on_remove_superset
    },
    {
        "on_add_superset_childof",
        Trigger_on_add_superset_childof
    },
    {
        "on_remove_superset_childof",
        Trigger_on_remove_superset_childof
    },
    {
        "on_add_self_superset",
        Trigger_on_add_self_superset
    },
    {
        "on_remove_self_superset",
        Trigger_on_remove_self_superset
    },
    {
        "add_twice",
        Trigger_add_twice
    },
    {
        "remove_twice",
        Trigger_remove_twice
    },
    {
        "on_remove_w_clear",
        Trigger_on_remove_w_clear
    },
    {
        "on_remove_w_delete",
        Trigger_on_remove_w_delete
    },
    {
        "on_remove_w_world_fini",
        Trigger_on_remove_w_world_fini
    },
    {
        "on_add_w_clone",
        Trigger_on_add_w_clone
    },
    {
        "add_in_trigger",
        Trigger_add_in_trigger
    },
    {
        "remove_in_trigger",
        Trigger_remove_in_trigger
    },
    {
        "clear_in_trigger",
        Trigger_clear_in_trigger
    },
    {
        "delete_in_trigger",
        Trigger_delete_in_trigger
    },
    {
        "trigger_w_named_entity",
        Trigger_trigger_w_named_entity
    },
    {
        "on_remove_tree",
        Trigger_on_remove_tree
    },
    {
        "set_get_context",
        Trigger_set_get_context
    },
    {
        "set_get_binding_context",
        Trigger_set_get_binding_context
    },
    {
        "trigger_w_self",
        Trigger_trigger_w_self
    },
    {
        "delete_trigger_w_delete_ctx",
        Trigger_delete_trigger_w_delete_ctx
    },
    {
        "trigger_w_index",
        Trigger_trigger_w_index
    },
    {
        "iter_type_set",
        Trigger_iter_type_set
    },
    {
        "readonly_term",
        Trigger_readonly_term
    },
    {
        "trigger_on_prefab",
        Trigger_trigger_on_prefab
    },
    {
        "trigger_on_disabled",
        Trigger_trigger_on_disabled
    },
    {
        "trigger_on_prefab_tag",
        Trigger_trigger_on_prefab_tag
    },
    {
        "trigger_on_disabled_tag",
        Trigger_trigger_on_disabled_tag
    },
    {
        "trigger_cleanup_2_w_self_super_id",
        Trigger_trigger_cleanup_2_w_self_super_id
    },
    {
        "on_add_yield_existing",
        Trigger_on_add_yield_existing
    },
    {
        "on_add_yield_existing_2_tables",
        Trigger_on_add_yield_existing_2_tables
    },
    {
        "on_add_yield_existing_wildcard_pair",
        Trigger_on_add_yield_existing_wildcard_pair
    },
    {
        "on_set_yield_existing",
        Trigger_on_set_yield_existing
    },
    {
        "filter_term",
        Trigger_filter_term
    },
    {
        "on_add_remove_after_exclusive_add",
        Trigger_on_add_remove_after_exclusive_add
    },
    {
        "on_add_base",
        Trigger_on_add_base
    },
    {
        "on_remove_base",
        Trigger_on_remove_base
    },
    {
        "on_set_base",
        Trigger_on_set_base
    },
    {
        "on_unset_base",
        Trigger_on_unset_base
    },
    {
        "on_add_base_superset_trigger",
        Trigger_on_add_base_superset_trigger
    },
    {
        "on_add_base_superset_trigger_2_lvls",
        Trigger_on_add_base_superset_trigger_2_lvls
    },
    {
        "on_add_base_2_entities",
        Trigger_on_add_base_2_entities
    },
    {
        "on_add_base_2_entities_filter",
        Trigger_on_add_base_2_entities_filter
    },
    {
        "on_set_base_w_value_2_entities",
        Trigger_on_set_base_w_value_2_entities
    },
    {
        "on_set_base_w_value_2_entities_instanced",
        Trigger_on_set_base_w_value_2_entities_instanced
    },
    {
        "on_add_base_w_override",
        Trigger_on_add_base_w_override
    },
    {
        "on_set_base_w_override",
        Trigger_on_set_base_w_override
    },
    {
        "entity_source_1_trigger",
        Trigger_entity_source_1_trigger
    },
    {
        "entity_source_2_triggers",
        Trigger_entity_source_2_triggers
    },
    {
        "entity_source_base_set",
        Trigger_entity_source_base_set
    },
    {
        "not_from_superset",
        Trigger_not_from_superset
    },
    {
        "create_stresstest",
        Trigger_create_stresstest
    },
    {
        "add_non_existing_entity",
        Trigger_add_non_existing_entity
    },
    {
        "on_add_self_trigger_with_add_isa",
        Trigger_on_add_self_trigger_with_add_isa
    },
    {
        "on_set_self_trigger_with_add_isa",
        Trigger_on_set_self_trigger_with_add_isa
    },
    {
        "notify_propagated_twice",
        Trigger_notify_propagated_twice
    },
    {
        "trigger_superset_wildcard",
        Trigger_trigger_superset_wildcard
    },
    {
        "remove_wildcard_1_id",
        Trigger_remove_wildcard_1_id
    },
    {
        "remove_wildcard_2_ids",
        Trigger_remove_wildcard_2_ids
    }
};

bake_test_case Observer_testcases[] = {
    {
        "2_terms_w_on_add",
        Observer_2_terms_w_on_add
    },
    {
        "2_terms_w_on_remove",
        Observer_2_terms_w_on_remove
    },
    {
        "2_terms_w_on_set_value",
        Observer_2_terms_w_on_set_value
    },
    {
        "2_terms_w_on_remove_value",
        Observer_2_terms_w_on_remove_value
    },
    {
        "2_terms_w_on_add_2nd",
        Observer_2_terms_w_on_add_2nd
    },
    {
        "2_terms_w_on_remove_2nd",
        Observer_2_terms_w_on_remove_2nd
    },
    {
        "2_pair_terms_w_on_add",
        Observer_2_pair_terms_w_on_add
    },
    {
        "2_pair_terms_w_on_remove",
        Observer_2_pair_terms_w_on_remove
    },
    {
        "2_wildcard_pair_terms_w_on_add",
        Observer_2_wildcard_pair_terms_w_on_add
    },
    {
        "2_wildcard_pair_terms_w_on_add_2_matching",
        Observer_2_wildcard_pair_terms_w_on_add_2_matching
    },
    {
        "2_wildcard_pair_terms_w_on_add_3_matching",
        Observer_2_wildcard_pair_terms_w_on_add_3_matching
    },
    {
        "2_wildcard_pair_terms_w_on_remove",
        Observer_2_wildcard_pair_terms_w_on_remove
    },
    {
        "on_set_n_entities",
        Observer_on_set_n_entities
    },
    {
        "on_set_n_entities_2_comp",
        Observer_on_set_n_entities_2_comp
    },
    {
        "wildcard_pair_w_pred_component",
        Observer_wildcard_pair_w_pred_component
    },
    {
        "wildcard_pair_w_obj_component",
        Observer_wildcard_pair_w_obj_component
    },
    {
        "2_terms_1_not_w_on_add",
        Observer_2_terms_1_not_w_on_add
    },
    {
        "2_terms_1_not_w_on_remove",
        Observer_2_terms_1_not_w_on_remove
    },
    {
        "2_terms_w_on_set",
        Observer_2_terms_w_on_set
    },
    {
        "2_terms_w_un_set",
        Observer_2_terms_w_un_set
    },
    {
        "3_terms_2_or_on_add",
        Observer_3_terms_2_or_on_add
    },
    {
        "3_terms_2_or_on_remove",
        Observer_3_terms_2_or_on_remove
    },
    {
        "2_terms_w_from_entity_on_add",
        Observer_2_terms_w_from_entity_on_add
    },
    {
        "2_terms_on_remove_on_clear",
        Observer_2_terms_on_remove_on_clear
    },
    {
        "2_terms_on_remove_on_delete",
        Observer_2_terms_on_remove_on_delete
    },
    {
        "observer_w_self",
        Observer_observer_w_self
    },
    {
        "add_after_delete_observer",
        Observer_add_after_delete_observer
    },
    {
        "remove_after_delete_observer",
        Observer_remove_after_delete_observer
    },
    {
        "delete_observer_w_ctx",
        Observer_delete_observer_w_ctx
    },
    {
        "filter_w_strings",
        Observer_filter_w_strings
    },
    {
        "iter_type_set",
        Observer_iter_type_set
    },
    {
        "readonly_term",
        Observer_readonly_term
    },
    {
        "trigger_on_prefab",
        Observer_trigger_on_prefab
    },
    {
        "trigger_on_disabled",
        Observer_trigger_on_disabled
    },
    {
        "unset_1_of_1",
        Observer_unset_1_of_1
    },
    {
        "unset_1_of_2",
        Observer_unset_1_of_2
    },
    {
        "unset_1_of_3",
        Observer_unset_1_of_3
    },
    {
        "unset_on_delete_1",
        Observer_unset_on_delete_1
    },
    {
        "unset_on_delete_2",
        Observer_unset_on_delete_2
    },
    {
        "unset_on_delete_3",
        Observer_unset_on_delete_3
    },
    {
        "unset_on_fini_1",
        Observer_unset_on_fini_1
    },
    {
        "unset_on_fini_2",
        Observer_unset_on_fini_2
    },
    {
        "unset_on_fini_3",
        Observer_unset_on_fini_3
    },
    {
        "overlapping_unset_systems",
        Observer_overlapping_unset_systems
    },
    {
        "unset_move_to_nonempty_table",
        Observer_unset_move_to_nonempty_table
    },
    {
        "write_in_unset",
        Observer_write_in_unset
    },
    {
        "filter_term",
        Observer_filter_term
    },
    {
        "2_terms_1_filter",
        Observer_2_terms_1_filter
    },
    {
        "3_terms_2_filter",
        Observer_3_terms_2_filter
    },
    {
        "and_from",
        Observer_and_from
    },
    {
        "or_from",
        Observer_or_from
    },
    {
        "notify_propagated_twice",
        Observer_notify_propagated_twice
    },
    {
        "on_add_yield_existing",
        Observer_on_add_yield_existing
    },
    {
        "on_add_yield_existing_2_tables",
        Observer_on_add_yield_existing_2_tables
    },
    {
        "on_add_yield_existing_2_terms",
        Observer_on_add_yield_existing_2_terms
    },
    {
        "observer_superset_wildcard",
        Observer_observer_superset_wildcard
    },
    {
        "observer_superset_wildcard_add_isa",
        Observer_observer_superset_wildcard_add_isa
    },
    {
        "observer_superset_wildcard_add_isa_at_offset",
        Observer_observer_superset_wildcard_add_isa_at_offset
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
    },
    {
        "emplace",
        TriggerOnAdd_emplace
    },
    {
        "add_after_delete_trigger",
        TriggerOnAdd_add_after_delete_trigger
    },
    {
        "add_after_delete_wildcard_id_trigger",
        TriggerOnAdd_add_after_delete_wildcard_id_trigger
    }
};

bake_test_case TriggerOnRemove_testcases[] = {
    {
        "remove",
        TriggerOnRemove_remove
    },
    {
        "remove_no_match",
        TriggerOnRemove_remove_no_match
    },
    {
        "delete",
        TriggerOnRemove_delete
    },
    {
        "delete_no_match",
        TriggerOnRemove_delete_no_match
    },
    {
        "remove_watched",
        TriggerOnRemove_remove_watched
    },
    {
        "delete_watched",
        TriggerOnRemove_delete_watched
    },
    {
        "valid_entity_after_delete",
        TriggerOnRemove_valid_entity_after_delete
    },
    {
        "remove_after_delete_trigger",
        TriggerOnRemove_remove_after_delete_trigger
    },
    {
        "remove_after_delete_wildcard_id_trigger",
        TriggerOnRemove_remove_after_delete_wildcard_id_trigger
    },
    {
        "has_removed_tag_trigger_1_tag",
        TriggerOnRemove_has_removed_tag_trigger_1_tag
    },
    {
        "has_removed_tag_trigger_2_tags",
        TriggerOnRemove_has_removed_tag_trigger_2_tags
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
    },
    {
        "emplace",
        TriggerOnSet_emplace
    },
    {
        "un_set_tag_w_remove",
        TriggerOnSet_un_set_tag_w_remove
    },
    {
        "un_set_tag_w_clear",
        TriggerOnSet_un_set_tag_w_clear
    },
    {
        "un_set_tag_w_delete",
        TriggerOnSet_un_set_tag_w_delete
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
    },
    {
        "1_comp_prefab_new",
        Monitor_1_comp_prefab_new
    },
    {
        "1_comp_prefab_add",
        Monitor_1_comp_prefab_add
    },
    {
        "monitor_w_and",
        Monitor_monitor_w_and
    },
    {
        "monitor_w_or",
        Monitor_monitor_w_or
    },
    {
        "monitor_w_not",
        Monitor_monitor_w_not
    },
    {
        "monitor_w_optional",
        Monitor_monitor_w_optional
    },
    {
        "monitor_w_superset",
        Monitor_monitor_w_superset
    },
    {
        "monitor_w_self_superset",
        Monitor_monitor_w_self_superset
    },
    {
        "monitor_w_wildcard",
        Monitor_monitor_w_wildcard
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
        "add_0_entity_in_on_set",
        SystemOnSet_add_0_entity_in_on_set
    },
    {
        "on_set_prefab",
        SystemOnSet_on_set_prefab
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
    },
    {
        "and_type",
        SystemPeriodic_and_type
    },
    {
        "or_type",
        SystemPeriodic_or_type
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
    },
    {
        "rate_filter_with_empty_src",
        Timer_rate_filter_with_empty_src
    },
    {
        "one_shot_timer_entity",
        Timer_one_shot_timer_entity
    },
    {
        "interval_timer_entity",
        Timer_interval_timer_entity
    },
    {
        "rate_entity",
        Timer_rate_entity
    },
    {
        "nested_rate_entity",
        Timer_nested_rate_entity
    },
    {
        "nested_rate_entity_empty_src",
        Timer_nested_rate_entity_empty_src
    },
    {
        "naked_tick_entity",
        Timer_naked_tick_entity
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
        "cascade_depth_2_new_syntax",
        SystemCascade_cascade_depth_2_new_syntax
    },
    {
        "add_after_match",
        SystemCascade_add_after_match
    },
    {
        "adopt_after_match",
        SystemCascade_adopt_after_match
    },
    {
        "rematch_w_empty_table",
        SystemCascade_rematch_w_empty_table
    },
    {
        "query_w_only_cascade",
        SystemCascade_query_w_only_cascade
    },
    {
        "custom_relation_cascade_depth_1",
        SystemCascade_custom_relation_cascade_depth_1
    },
    {
        "custom_relation_cascade_depth_2",
        SystemCascade_custom_relation_cascade_depth_2
    },
    {
        "custom_relation_add_after_match",
        SystemCascade_custom_relation_add_after_match
    },
    {
        "custom_relation_adopt_after_match",
        SystemCascade_custom_relation_adopt_after_match
    },
    {
        "custom_relation_rematch_w_empty_table",
        SystemCascade_custom_relation_rematch_w_empty_table
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
    },
    {
        "dont_run_w_unmatching_entity_query",
        SystemManual_dont_run_w_unmatching_entity_query
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
        "get_ptr_prefab",
        Prefab_get_ptr_prefab
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
        "prefab_tree_1_2_1",
        Prefab_prefab_tree_1_2_1
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
    },
    {
        "force_owned",
        Prefab_force_owned
    },
    {
        "force_owned_2",
        Prefab_force_owned_2
    },
    {
        "force_owned_nested",
        Prefab_force_owned_nested
    },
    {
        "prefab_instanceof_hierarchy",
        Prefab_prefab_instanceof_hierarchy
    },
    {
        "override_tag",
        Prefab_override_tag
    },
    {
        "empty_prefab",
        Prefab_empty_prefab
    },
    {
        "instanceof_0",
        Prefab_instanceof_0
    },
    {
        "instantiate_empty_child_table",
        Prefab_instantiate_empty_child_table
    },
    {
        "instantiate_emptied_child_table",
        Prefab_instantiate_emptied_child_table
    },
    {
        "override_2_prefabs",
        Prefab_override_2_prefabs
    },
    {
        "rematch_after_add_instanceof_to_parent",
        Prefab_rematch_after_add_instanceof_to_parent
    },
    {
        "child_of_instance",
        Prefab_child_of_instance
    },
    {
        "rematch_after_prefab_delete",
        Prefab_rematch_after_prefab_delete
    },
    {
        "add_tag_w_low_id_to_instance",
        Prefab_add_tag_w_low_id_to_instance
    },
    {
        "get_type_after_base_add",
        Prefab_get_type_after_base_add
    },
    {
        "get_type_after_recycled_base_add",
        Prefab_get_type_after_recycled_base_add
    },
    {
        "new_w_recycled_base",
        Prefab_new_w_recycled_base
    },
    {
        "add_recycled_base",
        Prefab_add_recycled_base
    },
    {
        "remove_recycled_base",
        Prefab_remove_recycled_base
    },
    {
        "get_from_recycled_base",
        Prefab_get_from_recycled_base
    },
    {
        "override_from_recycled_base",
        Prefab_override_from_recycled_base
    },
    {
        "remove_override_from_recycled_base",
        Prefab_remove_override_from_recycled_base
    },
    {
        "instantiate_tree_from_recycled_base",
        Prefab_instantiate_tree_from_recycled_base
    },
    {
        "rematch_after_add_to_recycled_base",
        Prefab_rematch_after_add_to_recycled_base
    },
    {
        "get_tag_from_2nd_base",
        Prefab_get_tag_from_2nd_base
    },
    {
        "get_component_from_2nd_base",
        Prefab_get_component_from_2nd_base
    },
    {
        "get_component_from_1st_base",
        Prefab_get_component_from_1st_base
    },
    {
        "get_component_from_2nd_base_of_base",
        Prefab_get_component_from_2nd_base_of_base
    },
    {
        "get_component_from_1st_base_of_base",
        Prefab_get_component_from_1st_base_of_base
    },
    {
        "get_component_from_2nd_base_prefab_base",
        Prefab_get_component_from_2nd_base_prefab_base
    },
    {
        "get_component_from_1st_base_prefab_base",
        Prefab_get_component_from_1st_base_prefab_base
    },
    {
        "get_component_from_2nd_base_of_base_prefab_base",
        Prefab_get_component_from_2nd_base_of_base_prefab_base
    },
    {
        "get_component_from_1st_base_of_base_prefab_base",
        Prefab_get_component_from_1st_base_of_base_prefab_base
    },
    {
        "fail_on_override_final",
        Prefab_fail_on_override_final
    },
    {
        "instantiate_tree_once",
        Prefab_instantiate_tree_once
    },
    {
        "nested_prefab_w_named_children",
        Prefab_nested_prefab_w_named_children
    },
    {
        "dont_copy_children_for_non_prefab_base",
        Prefab_dont_copy_children_for_non_prefab_base
    },
    {
        "get_component_pair_from_base",
        Prefab_get_component_pair_from_base
    },
    {
        "get_component_pair_from_prefab_base",
        Prefab_get_component_pair_from_prefab_base
    }
};

bake_test_case System_w_FromParent_testcases[] = {
    {
        "1_column_from_container",
        System_w_FromParent_1_column_from_container
    },
    {
        "2_column_1_from_container",
        System_w_FromParent_2_column_1_from_container
    },
    {
        "3_column_2_from_container",
        System_w_FromParent_3_column_2_from_container
    },
    {
        "2_column_1_from_container_w_not",
        System_w_FromParent_2_column_1_from_container_w_not
    },
    {
        "2_column_1_from_container_w_not_prefab",
        System_w_FromParent_2_column_1_from_container_w_not_prefab
    },
    {
        "3_column_1_from_comtainer_1_from_container_w_not",
        System_w_FromParent_3_column_1_from_comtainer_1_from_container_w_not
    },
    {
        "2_column_1_from_container_w_or",
        System_w_FromParent_2_column_1_from_container_w_or
    },
    {
        "select_same_from_container",
        System_w_FromParent_select_same_from_container
    },
    {
        "add_component_after_match",
        System_w_FromParent_add_component_after_match
    },
    {
        "add_component_after_match_and_rematch",
        System_w_FromParent_add_component_after_match_and_rematch
    },
    {
        "add_component_after_match_unmatch",
        System_w_FromParent_add_component_after_match_unmatch
    },
    {
        "add_component_after_match_unmatch_match",
        System_w_FromParent_add_component_after_match_unmatch_match
    },
    {
        "add_component_after_match_2_systems",
        System_w_FromParent_add_component_after_match_2_systems
    },
    {
        "add_component_in_progress_after_match",
        System_w_FromParent_add_component_in_progress_after_match
    },
    {
        "add_component_after_match_and_rematch_w_entity_type_expr",
        System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr
    },
    {
        "add_component_after_match_and_rematch_w_entity_type_expr_in_progress",
        System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr_in_progress
    },
    {
        "adopt_after_match",
        System_w_FromParent_adopt_after_match
    },
    {
        "new_child_after_match",
        System_w_FromParent_new_child_after_match
    },
    {
        "realloc_after_match",
        System_w_FromParent_realloc_after_match
    }
};

bake_test_case System_w_Empty_testcases[] = {
    {
        "2_column_1_from_id",
        System_w_Empty_2_column_1_from_id
    },
    {
        "3_column_2_from_id",
        System_w_Empty_3_column_2_from_id
    },
    {
        "column_type",
        System_w_Empty_column_type
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
        "entity_range_add_existing_staged",
        World_entity_range_add_existing_staged
    },
    {
        "entity_range_add_in_range_staged",
        World_entity_range_add_in_range_staged
    },
    {
        "entity_range_add_out_of_range_staged",
        World_entity_range_add_out_of_range_staged
    },
    {
        "entity_range_out_of_range_check_disabled",
        World_entity_range_out_of_range_check_disabled
    },
    {
        "entity_range_check_after_delete",
        World_entity_range_check_after_delete
    },
    {
        "dim",
        World_dim
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
        "system_time_scale",
        World_system_time_scale
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
    },
    {
        "ensure_empty_root",
        World_ensure_empty_root
    },
    {
        "register_alias_twice_same_entity",
        World_register_alias_twice_same_entity
    },
    {
        "register_alias_twice_different_entity",
        World_register_alias_twice_different_entity
    },
    {
        "redefine_component",
        World_redefine_component
    }
};

bake_test_case Stats_testcases[] = {
    {
        "get_world_stats",
        Stats_get_world_stats
    },
    {
        "get_pipeline_stats_before_progress_mini_world",
        Stats_get_pipeline_stats_before_progress_mini_world
    },
    {
        "get_pipeline_stats_before_progress",
        Stats_get_pipeline_stats_before_progress
    },
    {
        "get_pipeline_stats_after_progress_no_systems",
        Stats_get_pipeline_stats_after_progress_no_systems
    },
    {
        "get_pipeline_stats_after_progress_1_system",
        Stats_get_pipeline_stats_after_progress_1_system
    },
    {
        "get_pipeline_stats_after_progress_1_inactive_system",
        Stats_get_pipeline_stats_after_progress_1_inactive_system
    },
    {
        "get_pipeline_stats_after_progress_2_systems",
        Stats_get_pipeline_stats_after_progress_2_systems
    },
    {
        "get_pipeline_stats_after_progress_2_systems_one_merge",
        Stats_get_pipeline_stats_after_progress_2_systems_one_merge
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
        "invalid_entity_type_expression",
        Type_invalid_entity_type_expression
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
    },
    {
        "type_to_expr_pair",
        Type_type_to_expr_pair
    },
    {
        "type_to_expr_pair_w_comp",
        Type_type_to_expr_pair_w_comp
    },
    {
        "type_to_expr_scope",
        Type_type_to_expr_scope
    },
    {
        "entity_str",
        Type_entity_str
    },
    {
        "entity_path_str",
        Type_entity_path_str
    },
    {
        "entity_instanceof_str",
        Type_entity_instanceof_str
    },
    {
        "entity_childof_str",
        Type_entity_childof_str
    },
    {
        "entity_pair_str",
        Type_entity_pair_str
    },
    {
        "entity_switch_str",
        Type_entity_switch_str
    },
    {
        "entity_case_str",
        Type_entity_case_str
    },
    {
        "entity_and_str",
        Type_entity_and_str
    },
    {
        "entity_or_str",
        Type_entity_or_str
    },
    {
        "entity_xor_str",
        Type_entity_xor_str
    },
    {
        "entity_not_str",
        Type_entity_not_str
    },
    {
        "entity_str_small_buffer",
        Type_entity_str_small_buffer
    },
    {
        "role_pair_str",
        Type_role_pair_str
    },
    {
        "role_switch_str",
        Type_role_switch_str
    },
    {
        "role_case_str",
        Type_role_case_str
    },
    {
        "role_and_str",
        Type_role_and_str
    },
    {
        "role_or_str",
        Type_role_or_str
    },
    {
        "role_xor_str",
        Type_role_xor_str
    },
    {
        "role_not_str",
        Type_role_not_str
    },
    {
        "role_owned_str",
        Type_role_owned_str
    },
    {
        "role_disabled_str",
        Type_role_disabled_str
    },
    {
        "large_type_expr",
        Type_large_type_expr
    },
    {
        "large_type_expr_limit",
        Type_large_type_expr_limit
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
        "2_thread_1_entity_instanced",
        MultiThread_2_thread_1_entity_instanced
    },
    {
        "2_thread_5_entity_instanced",
        MultiThread_2_thread_5_entity_instanced
    },
    {
        "2_thread_10_entity_instanced",
        MultiThread_2_thread_10_entity_instanced
    },
    {
        "2_thread_test_combs_100_entity_w_next_worker",
        MultiThread_2_thread_test_combs_100_entity_w_next_worker
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
    },
    {
        "fini_after_set_threads",
        MultiThread_fini_after_set_threads
    },
    {
        "2_threads_single_threaded_system",
        MultiThread_2_threads_single_threaded_system
    }
};

bake_test_case DeferredActions_testcases[] = {
    {
        "defer_new",
        DeferredActions_defer_new
    },
    {
        "defer_bulk_new",
        DeferredActions_defer_bulk_new
    },
    {
        "defer_add",
        DeferredActions_defer_add
    },
    {
        "defer_add_two",
        DeferredActions_defer_add_two
    },
    {
        "defer_remove",
        DeferredActions_defer_remove
    },
    {
        "defer_remove_two",
        DeferredActions_defer_remove_two
    },
    {
        "defer_set",
        DeferredActions_defer_set
    },
    {
        "defer_delete",
        DeferredActions_defer_delete
    },
    {
        "defer_twice",
        DeferredActions_defer_twice
    },
    {
        "defer_twice_in_progress",
        DeferredActions_defer_twice_in_progress
    },
    {
        "run_w_defer",
        DeferredActions_run_w_defer
    },
    {
        "system_in_progress_w_defer",
        DeferredActions_system_in_progress_w_defer
    },
    {
        "defer_get_mut_no_modify",
        DeferredActions_defer_get_mut_no_modify
    },
    {
        "defer_get_mut_w_modify",
        DeferredActions_defer_get_mut_w_modify
    },
    {
        "defer_modify",
        DeferredActions_defer_modify
    },
    {
        "defer_set_pair",
        DeferredActions_defer_set_pair
    },
    {
        "defer_clear",
        DeferredActions_defer_clear
    },
    {
        "defer_add_after_delete",
        DeferredActions_defer_add_after_delete
    },
    {
        "defer_set_after_delete",
        DeferredActions_defer_set_after_delete
    },
    {
        "defer_get_mut_after_delete",
        DeferredActions_defer_get_mut_after_delete
    },
    {
        "defer_get_mut_after_delete_2nd_to_last",
        DeferredActions_defer_get_mut_after_delete_2nd_to_last
    },
    {
        "defer_add_child_to_deleted_parent",
        DeferredActions_defer_add_child_to_deleted_parent
    },
    {
        "recreate_deleted_entity_while_deferred",
        DeferredActions_recreate_deleted_entity_while_deferred
    },
    {
        "defer_add_to_recycled_id",
        DeferredActions_defer_add_to_recycled_id
    },
    {
        "defer_add_to_recycled_id_w_role",
        DeferredActions_defer_add_to_recycled_id_w_role
    },
    {
        "defer_add_to_recycled_relation",
        DeferredActions_defer_add_to_recycled_relation
    },
    {
        "defer_add_to_recycled_object",
        DeferredActions_defer_add_to_recycled_object
    },
    {
        "defer_add_to_recycled_object_childof",
        DeferredActions_defer_add_to_recycled_object_childof
    },
    {
        "defer_add_to_deleted_id",
        DeferredActions_defer_add_to_deleted_id
    },
    {
        "defer_add_to_deleted_id_w_role",
        DeferredActions_defer_add_to_deleted_id_w_role
    },
    {
        "defer_add_to_deleted_relation",
        DeferredActions_defer_add_to_deleted_relation
    },
    {
        "defer_add_to_deleted_object",
        DeferredActions_defer_add_to_deleted_object
    },
    {
        "defer_add_to_deleted_object_childof",
        DeferredActions_defer_add_to_deleted_object_childof
    },
    {
        "defer_delete_added_id",
        DeferredActions_defer_delete_added_id
    },
    {
        "defer_delete_added_id_w_role",
        DeferredActions_defer_delete_added_id_w_role
    },
    {
        "defer_delete_added_relation",
        DeferredActions_defer_delete_added_relation
    },
    {
        "defer_delete_added_object",
        DeferredActions_defer_delete_added_object
    },
    {
        "defer_delete_added_object_childof",
        DeferredActions_defer_delete_added_object_childof
    },
    {
        "discard_add",
        DeferredActions_discard_add
    },
    {
        "discard_remove",
        DeferredActions_discard_remove
    },
    {
        "discard_add_two",
        DeferredActions_discard_add_two
    },
    {
        "discard_remove_two",
        DeferredActions_discard_remove_two
    },
    {
        "discard_child",
        DeferredActions_discard_child
    },
    {
        "discard_child_w_add",
        DeferredActions_discard_child_w_add
    },
    {
        "defer_return_value",
        DeferredActions_defer_return_value
    },
    {
        "defer_get_mut_pair",
        DeferredActions_defer_get_mut_pair
    },
    {
        "async_stage_add",
        DeferredActions_async_stage_add
    },
    {
        "async_stage_add_twice",
        DeferredActions_async_stage_add_twice
    },
    {
        "async_stage_remove",
        DeferredActions_async_stage_remove
    },
    {
        "async_stage_clear",
        DeferredActions_async_stage_clear
    },
    {
        "async_stage_delete",
        DeferredActions_async_stage_delete
    },
    {
        "async_stage_new",
        DeferredActions_async_stage_new
    },
    {
        "async_stage_no_get",
        DeferredActions_async_stage_no_get
    },
    {
        "async_stage_readonly",
        DeferredActions_async_stage_readonly
    },
    {
        "async_stage_is_async",
        DeferredActions_async_stage_is_async
    },
    {
        "register_component_while_in_progress",
        DeferredActions_register_component_while_in_progress
    },
    {
        "register_component_while_staged",
        DeferredActions_register_component_while_staged
    },
    {
        "register_component_while_deferred",
        DeferredActions_register_component_while_deferred
    },
    {
        "defer_enable",
        DeferredActions_defer_enable
    },
    {
        "defer_disable",
        DeferredActions_defer_disable
    },
    {
        "defer_delete_with",
        DeferredActions_defer_delete_with
    },
    {
        "defer_remove_all",
        DeferredActions_defer_remove_all
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
        "new_empty_w_count",
        SingleThreadStaging_new_empty_w_count
    },
    {
        "new_component_w_count",
        SingleThreadStaging_new_component_w_count
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
        "clear_stage_after_merge",
        SingleThreadStaging_clear_stage_after_merge
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
    },
    {
        "lock_table",
        SingleThreadStaging_lock_table
    },
    {
        "recursive_lock_table",
        SingleThreadStaging_recursive_lock_table
    },
    {
        "modify_after_lock",
        SingleThreadStaging_modify_after_lock
    },
    {
        "get_empty_case_from_stage",
        SingleThreadStaging_get_empty_case_from_stage
    },
    {
        "get_case_from_stage",
        SingleThreadStaging_get_case_from_stage
    },
    {
        "get_object_from_stage",
        SingleThreadStaging_get_object_from_stage
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
    },
    {
        "custom_thread_auto_merge",
        MultiThreadStaging_custom_thread_auto_merge
    },
    {
        "custom_thread_manual_merge",
        MultiThreadStaging_custom_thread_manual_merge
    },
    {
        "custom_thread_partial_manual_merge",
        MultiThreadStaging_custom_thread_partial_manual_merge
    }
};

bake_test_case Stresstests_testcases[] = {
    {
        "create_1m_set_two_components",
        Stresstests_create_1m_set_two_components
    },
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
    },
    {
        "create_1m_set_two_components",
        Stresstests_create_1m_set_two_components
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
        "snapshot_free_filtered_w_dtor",
        Snapshot_snapshot_free_filtered_w_dtor
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
    },
    {
        "snapshot_from_stage",
        Snapshot_snapshot_from_stage
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
    },
    {
        "nested_module",
        Modules_nested_module
    },
    {
        "module_tag_on_namespace",
        Modules_module_tag_on_namespace
    },
    {
        "module_tag_on_namespace_on_add",
        Modules_module_tag_on_namespace_on_add
    },
    {
        "module_tag_on_namespace_on_add_2_levels",
        Modules_module_tag_on_namespace_on_add_2_levels
    }
};

bake_test_case MixinIterable_testcases[] = {
    {
        "iter_query",
        MixinIterable_iter_query
    },
    {
        "iter_query_w_filter",
        MixinIterable_iter_query_w_filter
    },
    {
        "iter_world",
        MixinIterable_iter_world
    },
    {
        "iter_world_w_filter",
        MixinIterable_iter_world_w_filter
    },
    {
        "iter_rule",
        MixinIterable_iter_rule
    },
    {
        "iter_rule_w_filter",
        MixinIterable_iter_rule_w_filter
    },
    {
        "iter_filter",
        MixinIterable_iter_filter
    },
    {
        "iter_filter_w_filter",
        MixinIterable_iter_filter_w_filter
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
    },
    {
        "recreate_deleted_table",
        Internals_recreate_deleted_table
    },
    {
        "create_65k_tables",
        Internals_create_65k_tables
    },
    {
        "no_duplicate_root_table_id",
        Internals_no_duplicate_root_table_id
    },
    {
        "override_os_api_w_addon",
        Internals_override_os_api_w_addon
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
    },
    {
        "last_error",
        Error_last_error
    }
};

bake_test_case App_testcases[] = {
    {
        "app_w_frame_action",
        App_app_w_frame_action
    }
};

static bake_test_suite suites[] = {
    {
        "Entity",
        NULL,
        NULL,
        58,
        Entity_testcases
    },
    {
        "Event",
        NULL,
        NULL,
        12,
        Event_testcases
    },
    {
        "New",
        New_setup,
        NULL,
        25,
        New_testcases
    },
    {
        "New_w_Count",
        NULL,
        NULL,
        13,
        New_w_Count_testcases
    },
    {
        "Add",
        NULL,
        NULL,
        14,
        Add_testcases
    },
    {
        "Switch",
        NULL,
        NULL,
        40,
        Switch_testcases
    },
    {
        "EnabledComponents",
        NULL,
        NULL,
        37,
        EnabledComponents_testcases
    },
    {
        "Remove",
        NULL,
        NULL,
        7,
        Remove_testcases
    },
    {
        "Parser",
        NULL,
        NULL,
        154,
        Parser_testcases
    },
    {
        "Plecs",
        NULL,
        NULL,
        131,
        Plecs_testcases
    },
    {
        "GlobalComponentIds",
        NULL,
        NULL,
        7,
        GlobalComponentIds_testcases
    },
    {
        "Doc",
        NULL,
        NULL,
        3,
        Doc_testcases
    },
    {
        "Hierarchies",
        Hierarchies_setup,
        NULL,
        83,
        Hierarchies_testcases
    },
    {
        "Has",
        NULL,
        NULL,
        16,
        Has_testcases
    },
    {
        "Count",
        NULL,
        NULL,
        3,
        Count_testcases
    },
    {
        "Get_component",
        Get_component_setup,
        NULL,
        10,
        Get_component_testcases
    },
    {
        "Reference",
        Reference_setup,
        NULL,
        10,
        Reference_testcases
    },
    {
        "Delete",
        Delete_setup,
        NULL,
        30,
        Delete_testcases
    },
    {
        "OnDelete",
        NULL,
        NULL,
        60,
        OnDelete_testcases
    },
    {
        "Set",
        NULL,
        NULL,
        28,
        Set_testcases
    },
    {
        "Lookup",
        Lookup_setup,
        NULL,
        29,
        Lookup_testcases
    },
    {
        "Singleton",
        NULL,
        NULL,
        3,
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
        ComponentLifecycle_setup,
        NULL,
        52,
        ComponentLifecycle_testcases
    },
    {
        "Pipeline",
        Pipeline_setup,
        NULL,
        36,
        Pipeline_testcases
    },
    {
        "SystemMisc",
        SystemMisc_setup,
        NULL,
        74,
        SystemMisc_testcases
    },
    {
        "Sorting",
        NULL,
        NULL,
        33,
        Sorting_testcases
    },
    {
        "Filter",
        NULL,
        NULL,
        139,
        Filter_testcases
    },
    {
        "Query",
        NULL,
        NULL,
        61,
        Query_testcases
    },
    {
        "Iter",
        NULL,
        NULL,
        23,
        Iter_testcases
    },
    {
        "Pairs",
        NULL,
        NULL,
        91,
        Pairs_testcases
    },
    {
        "Rules",
        NULL,
        NULL,
        130,
        Rules_testcases
    },
    {
        "TransitiveRules",
        NULL,
        NULL,
        24,
        TransitiveRules_testcases
    },
    {
        "Trigger",
        NULL,
        NULL,
        97,
        Trigger_testcases
    },
    {
        "Observer",
        NULL,
        NULL,
        58,
        Observer_testcases
    },
    {
        "TriggerOnAdd",
        TriggerOnAdd_setup,
        NULL,
        29,
        TriggerOnAdd_testcases
    },
    {
        "TriggerOnRemove",
        NULL,
        NULL,
        11,
        TriggerOnRemove_testcases
    },
    {
        "TriggerOnSet",
        NULL,
        NULL,
        15,
        TriggerOnSet_testcases
    },
    {
        "Monitor",
        NULL,
        NULL,
        14,
        Monitor_testcases
    },
    {
        "SystemOnSet",
        NULL,
        NULL,
        21,
        SystemOnSet_testcases
    },
    {
        "SystemPeriodic",
        NULL,
        NULL,
        47,
        SystemPeriodic_testcases
    },
    {
        "Timer",
        NULL,
        NULL,
        16,
        Timer_testcases
    },
    {
        "SystemCascade",
        NULL,
        NULL,
        12,
        SystemCascade_testcases
    },
    {
        "SystemManual",
        SystemManual_setup,
        NULL,
        4,
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
        Prefab_setup,
        NULL,
        95,
        Prefab_testcases
    },
    {
        "System_w_FromParent",
        System_w_FromParent_setup,
        NULL,
        19,
        System_w_FromParent_testcases
    },
    {
        "System_w_Empty",
        NULL,
        NULL,
        3,
        System_w_Empty_testcases
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
        World_setup,
        NULL,
        36,
        World_testcases
    },
    {
        "Stats",
        NULL,
        NULL,
        8,
        Stats_testcases
    },
    {
        "Type",
        Type_setup,
        NULL,
        40,
        Type_testcases
    },
    {
        "Run",
        Run_setup,
        NULL,
        21,
        Run_testcases
    },
    {
        "MultiThread",
        MultiThread_setup,
        NULL,
        40,
        MultiThread_testcases
    },
    {
        "DeferredActions",
        NULL,
        NULL,
        62,
        DeferredActions_testcases
    },
    {
        "SingleThreadStaging",
        SingleThreadStaging_setup,
        NULL,
        64,
        SingleThreadStaging_testcases
    },
    {
        "MultiThreadStaging",
        MultiThreadStaging_setup,
        NULL,
        10,
        MultiThreadStaging_testcases
    },
    {
        "Stresstests",
        Stresstests_setup,
        NULL,
        15,
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
        "Modules",
        Modules_setup,
        NULL,
        20,
        Modules_testcases
    },
    {
        "MixinIterable",
        NULL,
        NULL,
        8,
        MixinIterable_testcases
    },
    {
        "Internals",
        Internals_setup,
        NULL,
        10,
        Internals_testcases
    },
    {
        "Error",
        Error_setup,
        NULL,
        12,
        Error_testcases
    },
    {
        "App",
        NULL,
        NULL,
        1,
        App_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("api", argc, argv, suites, 65);
}
