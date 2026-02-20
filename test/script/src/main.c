
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <script.h>

// Testsuite 'Eval'
void Eval_null(void);
void Eval_empty(void);
void Eval_space(void);
void Eval_space_newline(void);
void Eval_two_empty_newlines(void);
void Eval_three_empty_newlines(void);
void Eval_newline_trailing_space(void);
void Eval_newline_trailing_spaces(void);
void Eval_multiple_trailing_newlines(void);
void Eval_space_crlf(void);
void Eval_two_empty_crlfs(void);
void Eval_three_empty_crlfs(void);
void Eval_crlf_trailing_space(void);
void Eval_crlf_trailing_spaces(void);
void Eval_multiple_trailing_crlfs(void);
void Eval_entity(void);
void Eval_entity_newline(void);
void Eval_entity_crlf(void);
void Eval_entity_w_core_name(void);
void Eval_2_entities(void);
void Eval_2_entities_crlf(void);
void Eval_line_comment(void);
void Eval_line_comment_before_stmt(void);
void Eval_line_comment_after_stmt(void);
void Eval_line_comment_between_stmt(void);
void Eval_multiple_line_comment(void);
void Eval_multiple_line_comment_w_newlines(void);
void Eval_line_comment_after_stmt_same_line(void);
void Eval_line_comment_before_scope_open(void);
void Eval_line_comment_after_newline_before_scope_open(void);
void Eval_line_comment_after_newline_before_newline_scope_open(void);
void Eval_multi_line_comment(void);
void Eval_multi_line_comment_before_stmt(void);
void Eval_multi_line_comment_after_stmt(void);
void Eval_multi_line_comment_between_stmt(void);
void Eval_multiple_multi_line_comment(void);
void Eval_multiple_multi_line_comment_w_newlines(void);
void Eval_multi_line_comment_after_stmt_same_line(void);
void Eval_multi_line_comment_before_scope_open(void);
void Eval_multi_line_comment_after_newline_before_scope_open(void);
void Eval_multi_line_comment_multiple_lines(void);
void Eval_hierarchy_1_child(void);
void Eval_hierarchy_2_children(void);
void Eval_hierarchy_1_child_same_line(void);
void Eval_hierarchy_2_children_same_line(void);
void Eval_hierarchy_2_children_same_line_no_trailing_comma(void);
void Eval_entity_after_hierarchy(void);
void Eval_newline_before_scope_open(void);
void Eval_newline_w_whitespace_before_scope_open(void);
void Eval_2_newline_before_scope_open(void);
void Eval_2_newline_w_whitespace_before_scope_open(void);
void Eval_hierarchy_2_levels(void);
void Eval_hierarchy_2_levels_2_subtrees(void);
void Eval_create_in_scope(void);
void Eval_hierarchy_w_pred_subj(void);
void Eval_hierarchy_custom_relation(void);
void Eval_hierarchy_custom_relation_2_levels(void);
void Eval_hierarchy_custom_relation_apply_to_object(void);
void Eval_hierarchy_custom_relation_apply_to_object_2_levels(void);
void Eval_entity_after_hierarchy_custom_relation(void);
void Eval_entity_after_hierarchy_custom_relation_2_levels(void);
void Eval_with_tag(void);
void Eval_with_tag_2_entities(void);
void Eval_with_tag_same_line(void);
void Eval_with_tag_2_entities_same_line(void);
void Eval_with_tag_2_entities_same_line_no_trailing_comma(void);
void Eval_with_tag_2_levels(void);
void Eval_with_tag_2_levels_2_subtrees(void);
void Eval_with_n_tags(void);
void Eval_with_n_tags_2_levels(void);
void Eval_with_after_scope(void);
void Eval_with_after_with(void);
void Eval_scope_inside_with_inside_scope(void);
void Eval_with_inside_scope(void);
void Eval_with_tag_core_name(void);
void Eval_inherit(void);
void Eval_inherit_newline(void);
void Eval_inherit_w_colon(void);
void Eval_inherit_w_colon_w_scope(void);
void Eval_assign_component_w_value(void);
void Eval_assign_tag_in_assign_scope(void);
void Eval_assign_tag_in_assign_scope_same_line(void);
void Eval_assign_tag_in_assign_scope_core_name(void);
void Eval_assign_component_value_in_assign_scope(void);
void Eval_assign_2_component_values_in_assign_scope(void);
void Eval_type_and_assign_in_plecs(void);
void Eval_type_and_assign_in_plecs_w_2_members(void);
void Eval_type_and_assign_in_plecs_w_3_members(void);
void Eval_type_and_assign_in_plecs_w_enum(void);
void Eval_type_and_assign_in_plecs_w_enum_using_meta(void);
void Eval_type_and_assign_in_plecs_w_enum_primitive_using_meta(void);
void Eval_type_and_assign_in_plecs_w_enum_primitive_and_struct(void);
void Eval_type_and_assign_in_plecs_nested_member(void);
void Eval_dot_assign_nested_member(void);
void Eval_dot_assign_binary_expr(void);
void Eval_open_scope_no_parent(void);
void Eval_create_subject_in_root_scope_w_resolvable_id(void);
void Eval_create_subject_in_scope_w_resolvable_id(void);
void Eval_create_subject_in_scope_w_resolvable_id_using(void);
void Eval_using_scope(void);
void Eval_using_nested_scope(void);
void Eval_using_nested_in_scope(void);
void Eval_using_with_scope(void);
void Eval_using_w_entity_ref_in_value_2_members(void);
void Eval_using_w_entity_ref_in_value_3_members(void);
void Eval_script_w_only_using(void);
void Eval_2_using_scope(void);
void Eval_2_using_in_different_scope(void);
void Eval_empty_scope_after_using(void);
void Eval_scope_after_assign(void);
void Eval_assign_after_inherit(void);
void Eval_multiple_tags_single_line(void);
void Eval_multiple_pairs_single_line(void);
void Eval_var_single_line_no_newline(void);
void Eval_var_single_line_composite_type_no_newline(void);
void Eval_multiple_vars_single_line(void);
void Eval_multiple_assignments_single_line(void);
void Eval_2_stmts_in_scope_w_no_parent(void);
void Eval_assign_tag_to_parent(void);
void Eval_assign_component_to_parent(void);
void Eval_assign_to_parent_pair_w_new_entities_in_scope(void);
void Eval_assign_to_parent_pair_w_existing_entities_in_scope(void);
void Eval_default_child_component(void);
void Eval_default_child_component_w_assign(void);
void Eval_struct_type_w_default_child_component(void);
void Eval_struct_type_w_default_child_component_nested_member(void);
void Eval_enum_type_w_default_child_component(void);
void Eval_enum_type_w_underlying_type(void);
void Eval_default_type_from_with(void);
void Eval_default_type_from_nested_with(void);
void Eval_default_type_from_with_in_entity_scope_w_default_type(void);
void Eval_default_type_from_entity_scope_in_with(void);
void Eval_scope_w_1_subj_and_2_pairs(void);
void Eval_inherit_from_multiple(void);
void Eval_assign_pair_component(void);
void Eval_assign_pair_component_in_scope(void);
void Eval_assign_pair_component_in_script(void);
void Eval_assign_pair_component_in_script_update(void);
void Eval_assign_pair_component_w_newline(void);
void Eval_assign_pair_component_w_newline_and_spaces(void);
void Eval_assign_pair_component_w_empty(void);
void Eval_assign_pair_component_w_newline_empty(void);
void Eval_assign_pair_component_w_newline_and_spaces_empty(void);
void Eval_assign_pair_component_after_component(void);
void Eval_assign_pair_component_after_int_component(void);
void Eval_assign_pair_component_after_entity_component(void);
void Eval_set_entity_names(void);
void Eval_oneof(void);
void Eval_brief_annotation(void);
void Eval_name_annotation(void);
void Eval_link_annotation(void);
void Eval_color_annotation(void);
void Eval_multiple_annotations(void);
void Eval_annotation_w_trailing_space(void);
void Eval_multiline_string(void);
void Eval_declaration_w_underscore_name(void);
void Eval_annotate_declaration(void);
void Eval_anonymous_entity(void);
void Eval_anonymous_entity_in_scope(void);
void Eval_anonymous_declaration(void);
void Eval_const_var_int(void);
void Eval_const_var_float(void);
void Eval_const_var_bool(void);
void Eval_const_var_string(void);
void Eval_const_var_struct(void);
void Eval_const_var_scoped(void);
void Eval_const_var_int_deprecated_notation(void);
void Eval_assign_component_from_var(void);
void Eval_assign_component_from_var_in_scope(void);
void Eval_scope_w_component_after_const_var(void);
void Eval_component_after_const_paren_expr(void);
void Eval_component_after_const_add_expr(void);
void Eval_component_after_const_sub_expr(void);
void Eval_component_after_const_mul_expr(void);
void Eval_component_after_const_div_expr(void);
void Eval_parse_with(void);
void Eval_parse_with_w_with(void);
void Eval_parse_with_w_tag(void);
void Eval_parse_with_value(void);
void Eval_parse_with_2_values(void);
void Eval_parse_with_2_nested_values(void);
void Eval_parse_with_var(void);
void Eval_parse_with_2_vars(void);
void Eval_parse_with_2_nested_vars(void);
void Eval_parse_with_var_in_scope(void);
void Eval_assign_const_w_expr(void);
void Eval_const_w_type(void);
void Eval_typed_const_w_composite_type(void);
void Eval_assign_var_to_typed_const_w_composite_type(void);
void Eval_using_wildcard(void);
void Eval_single_line_comment_in_value(void);
void Eval_single_line_comment_in_value_after_scope(void);
void Eval_multi_line_comment_in_value(void);
void Eval_multi_line_comment_in_value_after_scope(void);
void Eval_module_stmt(void);
void Eval_nested_module_stmt(void);
void Eval_module_stmt_w_scope(void);
void Eval_module_stmt_w_nested_scope(void);
void Eval_assign_singleton_tag(void);
void Eval_assign_singleton_component(void);
void Eval_assign_singleton_tag_w_scope(void);
void Eval_assign_singleton_2_tags_w_scope(void);
void Eval_assign_singleton_component_w_scope(void);
void Eval_assign_singleton_2_components_w_scope(void);
void Eval_with_pair_in_scope(void);
void Eval_with_pair_component_in_scope(void);
void Eval_pair_w_rel_var(void);
void Eval_pair_w_tgt_var(void);
void Eval_array_component(void);
void Eval_on_set_w_kind_paren_no_reflection(void);
void Eval_on_set_w_kind_paren(void);
void Eval_on_set_w_kind_no_paren_no_reflection(void);
void Eval_on_set_w_kind_no_paren(void);
void Eval_on_set_w_single_assign(void);
void Eval_on_set_w_single_assign_scoped_w_value(void);
void Eval_on_set_w_single_assign_scoped_no_value(void);
void Eval_if_true(void);
void Eval_if_true_no_else(void);
void Eval_if_false(void);
void Eval_if_true_var(void);
void Eval_if_false_var(void);
void Eval_if_10(void);
void Eval_if_0(void);
void Eval_if_true_in_scope(void);
void Eval_if_false_in_scope(void);
void Eval_if_lt(void);
void Eval_if_lt_const(void);
void Eval_if_else_if(void);
void Eval_if_else_if_else(void);
void Eval_if_else_if_else_if(void);
void Eval_if_else_newline_if(void);
void Eval_if_else_space_newline_if(void);
void Eval_isa_in_module(void);
void Eval_isa_hierarchy(void);
void Eval_isa_hierarchy_in_module(void);
void Eval_custom_isa_hierarchy_in_module(void);
void Eval_custom_isa_hierarchy_in_subtree(void);
void Eval_inherit_w_kind(void);
void Eval_inherit_w_kind_scope(void);
void Eval_inherit_w_kind_value(void);
void Eval_inherit_w_kind_value_scope(void);
void Eval_multiple_inherit_w_kind(void);
void Eval_multiple_inherit_w_kind_scope(void);
void Eval_auto_override_tag(void);
void Eval_auto_override_component(void);
void Eval_auto_override_pair(void);
void Eval_auto_override_pair_component(void);
void Eval_lowercase_prefab_kind(void);
void Eval_assign_component_to_const(void);
void Eval_assign_component_member_to_const(void);
void Eval_prefab_w_slot(void);
void Eval_prefab_w_slot_variant(void);
void Eval_const_w_component_expr(void);
void Eval_const_w_component_expr_in_scope(void);
void Eval_const_w_component_expr_in_module(void);
void Eval_const_w_component_in_scope_expr_in_scope(void);
void Eval_const_w_component_in_scope_expr_in_module(void);
void Eval_const_w_component_and_entity_in_scope_expr_in_scope(void);
void Eval_const_w_component_and_entity_in_scope_expr_in_module(void);
void Eval_path_tag_in_scope(void);
void Eval_path_tag_in_module(void);
void Eval_path_tag_in_nested_scope(void);
void Eval_path_tag_in_nested_module(void);
void Eval_dont_inherit_script_pair(void);
void Eval_update_script_w_anonymous(void);
void Eval_update_script_w_anonymous_paren(void);
void Eval_clear_script(void);
void Eval_clear_script_w_anonymous(void);
void Eval_clear_script_w_anonymous_paren(void);
void Eval_partial_assign(void);
void Eval_partial_assign_nontrivial(void);
void Eval_partial_assign_with(void);
void Eval_partial_assign_nontrivial_with(void);
void Eval_partial_assign_with_large_array(void);
void Eval_non_trivial_var_component(void);
void Eval_non_trivial_var_with(void);
void Eval_update_template_w_tag(void);
void Eval_assign_call_func(void);
void Eval_assign_call_scoped_func(void);
void Eval_assign_call_scoped_func_w_using(void);
void Eval_eval_w_vars(void);
void Eval_eval_w_runtime(void);
void Eval_eval_w_other_vars(void);
void Eval_eval_w_vars_different_order(void);
void Eval_eval_w_vars_different_order_var_component(void);
void Eval_eval_w_vars_different_order_with_var(void);
void Eval_eval_w_vars_different_order_pair_w_var(void);
void Eval_eval_w_vars_different_order_pair_scope_w_var(void);
void Eval_component_in_entity_in_with_scope(void);
void Eval_entity_w_string_name(void);
void Eval_entity_w_interpolated_name(void);
void Eval_entity_w_interpolated_name_w_var(void);
void Eval_entity_w_string_name_w_inherit(void);
void Eval_entity_w_string_name_w_inherit_scope(void);
void Eval_entity_w_string_name_w_kind(void);
void Eval_entity_w_string_name_w_kind_value(void);
void Eval_entity_w_string_name_w_kind_scope(void);
void Eval_entity_w_string_name_w_kind_value_scope(void);
void Eval_entity_w_interpolated_name_w_var_in_scope(void);
void Eval_for_range(void);
void Eval_for_range_vars(void);
void Eval_for_range_vars_no_dollar(void);
void Eval_for_range_1_4(void);
void Eval_for_range_min_1_2(void);
void Eval_variable_assign_self(void);
void Eval_func_w_entity_arg(void);
void Eval_func_w_entity_arg_w_using(void);
void Eval_method_w_entity_arg(void);
void Eval_method_w_entity_arg_w_using(void);
void Eval_assign_id(void);
void Eval_assign_id_w_using(void);
void Eval_const_assign_empty_initializer(void);
void Eval_const_assign_empty_collection_initializer(void);
void Eval_const_i32_assign_empty_initializer(void);
void Eval_const_i32_assign_empty_collection_initializer(void);
void Eval_component_w_match(void);
void Eval_component_w_match_invalid(void);
void Eval_pair_component_w_match(void);
void Eval_component_assign_w_match(void);
void Eval_const_w_match(void);
void Eval_component_w_assign_add(void);
void Eval_component_w_assign_mul(void);
void Eval_opaque_struct_component(void);
void Eval_opaque_string_component(void);
void Eval_add_component_w_invalid_ctor(void);
void Eval_add_component_pair_w_invalid_ctor(void);
void Eval_add_component_as_tag_w_invalid_ctor(void);
void Eval_add_component_as_tag_pair_w_invalid_ctor(void);
void Eval_interpolated_name_w_nested_for_loop(void);
void Eval_interpolated_name_w_nested_for_loop_no_dollar_sign(void);
void Eval_interpolated_name_w_nested_for_loop_wrong_dollar_sign(void);
void Eval_interpolated_string_as_tag(void);
void Eval_interpolated_string_as_relationship(void);
void Eval_interpolated_string_as_target(void);
void Eval_interpolated_string_as_relationship_and_target(void);
void Eval_interpolated_string_as_relationship_component(void);
void Eval_interpolated_string_as_target_component(void);
void Eval_interpolated_string_as_relationship_and_target_component(void);
void Eval_dynamic_interpolated_string_as_tag(void);
void Eval_dynamic_interpolated_string_as_relationship(void);
void Eval_dynamic_interpolated_string_as_target(void);
void Eval_assign_new_to_const_to_entity_member(void);
void Eval_assign_new_w_component_to_const_to_entity_member(void);
void Eval_assign_new_w_component_w_vars_to_const_to_entity_member(void);
void Eval_assign_new_w_kind_to_const_to_entity_member(void);
void Eval_assign_new_w_name_to_const_to_entity_member(void);
void Eval_assign_new_w_name_w_component_to_const_to_entity_member(void);
void Eval_assign_new_w_name_w_component_w_vars_to_const_to_entity_member(void);
void Eval_assign_new_w_name_w_kind_to_const_to_entity_member(void);
void Eval_assign_new_to_entity_member(void);
void Eval_assign_new_w_component_to_entity_member(void);
void Eval_assign_new_w_component_to_entity_member_w_newlines(void);
void Eval_assign_new_w_component_w_vars_to_entity_member(void);
void Eval_assign_new_w_kind_to_entity_member(void);
void Eval_assign_new_w_name_to_entity_member(void);
void Eval_assign_new_w_name_w_component_to_entity_member(void);
void Eval_assign_new_w_name_w_component_w_vars_to_entity_member(void);
void Eval_assign_new_w_name_w_kind_to_entity_member(void);
void Eval_assign_new_w_inheritance_to_entity_member(void);
void Eval_assign_new_w_name_w_inheritance_to_entity_member(void);
void Eval_assign_new_w_child_w_name_to_const(void);
void Eval_assign_new_w_child_w_name_to_entity_member(void);
void Eval_assign_new_to_const_in_scope(void);
void Eval_assign_new_to_const_in_for(void);
void Eval_implicit_var_as_tag(void);
void Eval_implicit_var_as_relationship(void);
void Eval_implicit_var_as_target(void);
void Eval_export_const_var(void);
void Eval_export_const_var_w_type(void);
void Eval_export_const_var_w_struct(void);
void Eval_export_const_var_redeclared(void);
void Eval_export_const_var_redeclared_w_local_var(void);
void Eval_local_const_var_redeclared_w_export_var(void);
void Eval_export_const_var_used_by_other_script(void);
void Eval_export_const_var_as_component(void);
void Eval_export_scoped_const_var_as_component(void);
void Eval_export_const_var_in_scope(void);
void Eval_export_scoped_const_var_used_by_other_script(void);
void Eval_run_script_w_module_from_scope(void);
void Eval_run_script_w_nested_module_from_scope(void);
void Eval_run_script_w_nested_module_from_named_scope(void);
void Eval_run_script_w_nested_module_from_module(void);
void Eval_run_script_w_nested_module_from_named_scope_managed(void);
void Eval_vector_i32_component(void);
void Eval_vector_string_component(void);
void Eval_vector_struct_component(void);
void Eval_vector_i32_export_var(void);
void Eval_vector_string_export_var(void);
void Eval_vector_struct_export_var(void);
void Eval_opaque_vector_i32_component(void);
void Eval_opaque_vector_i32_export_var(void);
void Eval_pair_after_const_int(void);
void Eval_pair_after_const_string(void);
void Eval_pair_after_const_identifier(void);
void Eval_tree_parent(void);
void Eval_tree_parent_nested(void);
void Eval_tree_parent_nested_childof(void);
void Eval_tree_childof_nested_parent(void);
void Eval_tree_parent_existing_child(void);
void Eval_tree_parent_existing_child_2(void);
void Eval_tree_prefab_override_child(void);
void Eval_tree_parent_nested_w_if_scope(void);
void Eval_tree_parent_nested_w_if_else_scope(void);
void Eval_tree_parent_nested_w_for_scope(void);
void Eval_tree_parent_nested_w_pair_scope(void);
void Eval_tree_parent_nested_w_with_scope(void);
void Eval_update_after_add_remove_tree_parent(void);

// Testsuite 'Template'
void Template_template_no_scope(void);
void Template_template_no_props(void);
void Template_template_prop(void);
void Template_template_prop_space_colon(void);
void Template_template_2_props(void);
void Template_template_w_using(void);
void Template_template_instance_w_default_values(void);
void Template_template_instance_w_assign_default_values(void);
void Template_template_instance_w_overridden_values(void);
void Template_template_w_prop_implicit_type(void);
void Template_template_w_child(void);
void Template_template_w_child_parse_script(void);
void Template_template_w_child_parse_script_twice(void);
void Template_template_w_child_update_after_parse(void);
void Template_template_w_nested_child(void);
void Template_template_w_prefab(void);
void Template_template_w_prefab_tree(void);
void Template_template_w_nested_template(void);
void Template_instantiate_prefab_w_template(void);
void Template_template_w_prefab_w_template(void);
void Template_3_templates(void);
void Template_template_nested_w_default_var(void);
void Template_template_w_anonymous(void);
void Template_template_w_anonymous_parse_again(void);
void Template_template_w_composite_prop(void);
void Template_template_with_with(void);
void Template_module_w_template(void);
void Template_module_w_nested_template(void);
void Template_template_w_pair_w_this_var(void);
void Template_template_w_pair_w_prop_var(void);
void Template_template_w_pair_w_const_var(void);
void Template_template_w_pair_scope_w_this_var(void);
void Template_template_w_pair_scope_w_prop_var(void);
void Template_template_w_pair_scope_w_const_var(void);
void Template_template_w_pair_w_unresolved_var_first(void);
void Template_template_w_pair_w_unresolved_var_second(void);
void Template_template_w_pair_scope_w_unresolved_var_first(void);
void Template_template_w_pair_scope_w_unresolved_var_second(void);
void Template_prop_without_using_meta(void);
void Template_hoist_var(void);
void Template_hoist_vars_nested(void);
void Template_hoist_vars_nested_w_masked(void);
void Template_anonymous_template_instance(void);
void Template_anonymous_template_instance_no_scope(void);
void Template_anonymous_template_instance_w_prop(void);
void Template_anonymous_template_instance_w_prop_no_scope(void);
void Template_with_after_template(void);
void Template_with_in_scope_after_template(void);
void Template_prefab_w_template(void);
void Template_template_in_scope(void);
void Template_nested_templates_in_prefab(void);
void Template_entity_w_2_template_instances(void);
void Template_template_w_prefab_and_instance(void);
void Template_template_w_with_var(void);
void Template_template_w_with_prop(void);
void Template_template_w_child_w_var(void);
void Template_template_w_child_w_prop(void);
void Template_fold_const(void);
void Template_bulk_create_template(void);
void Template_template_w_expr_w_self_ref(void);
void Template_entity_w_assign_with_nested_template(void);
void Template_template_w_for(void);
void Template_template_w_component_w_undefined_identifier(void);
void Template_template_w_child_component_w_undefined_identifier(void);
void Template_template_w_anonymous_child_component_w_undefined_identifier(void);
void Template_clear_script_w_template_w_on_remove_observer(void);
void Template_clear_script_w_template_w_on_remove_observer_added_after(void);
void Template_component_w_assign_add(void);
void Template_component_w_assign_mul(void);
void Template_prop_after_const(void);
void Template_const_from_prop(void);
void Template_redefine_nested_template_w_prefab(void);
void Template_redefine_nested_template_w_prefab_2(void);
void Template_redefine_nested_template_w_prefab_3(void);
void Template_template_w_script_component(void);
void Template_template_w_script_pair_component(void);
void Template_template_w_prop_and_pair(void);
void Template_template_w_name_annotation(void);
void Template_template_w_name_annotation_update_script(void);
void Template_template_w_tree_parent(void);
void Template_template_w_tree_parent_change_value(void);

// Testsuite 'Error'
void Error_multi_line_comment_after_newline_before_newline_scope_open(void);
void Error_missing_end_of_scope(void);
void Error_with_n_tags_2_levels_invalid_tag(void);
void Error_assignment_to_non_component(void);
void Error_struct_w_member_w_assignment_to_nothing(void);
void Error_struct_w_member_w_assignment_to_empty_scope(void);
void Error_invalid_nested_assignment(void);
void Error_invalid_partial_pair_assignment(void);
void Error_empty_assignment(void);
void Error_empty_assignment_before_end_of_scope(void);
void Error_default_type_with_tag(void);
void Error_invalid_oneof(void);
void Error_default_type_with_tag(void);
void Error_unterminated_multiline_string(void);
void Error_invalid_assign_multiline_string(void);
void Error_const_var_redeclare(void);
void Error_typed_const_w_composite_type_invalid_assignment(void);
void Error_unterminated_multi_line_comment_in_value(void);
void Error_pair_w_rel_var_invalid_type(void);
void Error_pair_w_tgt_var_invalid_type(void);
void Error_with_value_not_a_component(void);
void Error_component_in_with_scope(void);
void Error_component_in_with_scope_nested(void);
void Error_assign_after_with_in_scope(void);
void Error_not_an_array_component(void);
void Error_array_component_w_curly_brackets(void);
void Error_unknown_identifier(void);
void Error_unknown_identifier_for_int_field(void);
void Error_prefab_w_slot_no_parent(void);
void Error_tag_not_found(void);
void Error_component_not_found(void);
void Error_pair_first_not_found(void);
void Error_pair_second_not_found(void);
void Error_kind_not_found(void);
void Error_base_not_found(void);
void Error_entity_w_anonymous_tag(void);
void Error_member_expr_without_value_end_of_scope(void);
void Error_member_expr_without_value_comma(void);
void Error_member_expr_without_value_newline(void);
void Error_2_member_expr_without_value(void);
void Error_expr_junk_after_number(void);
void Error_expr_junk_after_unary_minus(void);
void Error_expr_comma_after_nothing(void);
void Error_expr_digit_with_two_dots(void);
void Error_template_empty(void);
void Error_template_unresolved_tag(void);
void Error_template_unresolved_component(void);
void Error_template_unresolved_pair_relationship(void);
void Error_template_unresolved_pair_target(void);
void Error_template_unresolved_with_tag(void);
void Error_template_unresolved_with_component(void);
void Error_template_unresolved_with_pair_relationship(void);
void Error_template_unresolved_with_pair_target(void);
void Error_template_unresolved_tag_in_child(void);
void Error_template_prop_no_type(void);
void Error_template_w_composite_prop_invalid_assignment(void);
void Error_template_redeclare_prop_as_const(void);
void Error_template_redeclare_prop_as_prop(void);
void Error_template_redeclare_const_as_const(void);
void Error_template_w_invalid_var_in_expr(void);
void Error_run_template_after_error(void);
void Error_update_template_after_error(void);
void Error_template_in_template(void);
void Error_unterminated_binary(void);
void Error_tag_in_with_scope(void);
void Error_tag_in_with_scope_2(void);
void Error_pair_tag_in_with_scope_2(void);
void Error_component_in_with_scope(void);
void Error_component_in_with_scope_2(void);
void Error_component_in_with_scope_3(void);
void Error_component_in_with_scope_4(void);
void Error_component_in_with_scope_5(void);
void Error_component_in_with_scope_nested(void);
void Error_component_in_with_scope_after_entity(void);
void Error_component_in_with_var_scope(void);
void Error_component_in_with_in_template(void);
void Error_reload_script_w_component_w_error(void);
void Error_reload_script_w_component_w_error_again(void);
void Error_initializer_w_int_to_struct(void);
void Error_script_initializer_w_int_to_struct(void);
void Error_capture_error(void);
void Error_unresolved_component_error_w_script_run(void);
void Error_unresolved_component_error_w_script_init(void);
void Error_unresolved_component_error_w_script_init_existing(void);
void Error_unresolved_component_error_w_script_eval(void);
void Error_unresolved_component_error_w_script_eval_multiple_times(void);
void Error_annotation_without_newline(void);
void Error_annotation_without_entity(void);
void Error_annotation_to_unresolved_identifier(void);
void Error_annotation_to_unresolved_identifier_managed_parse_twice(void);
void Error_annotation_to_unresolved_identifier_managed_parse_twice_2(void);
void Error_annotation_to_tag(void);

// Testsuite 'Expr'
void Expr_setup(void);
void Expr_add_2_int_literals(void);
void Expr_add_2_int_literals_twice(void);
void Expr_sub_2_int_literals(void);
void Expr_mul_2_int_literals(void);
void Expr_div_2_int_literals(void);
void Expr_add_3_int_literals(void);
void Expr_add_3_int_literals_dec_hex_bin(void);
void Expr_add_3_int_literals_twice(void);
void Expr_sub_3_int_literals(void);
void Expr_mul_3_int_literals(void);
void Expr_div_3_int_literals(void);
void Expr_mod_2_int_literals(void);
void Expr_mod_2_flt_literals(void);
void Expr_add_no_space(void);
void Expr_sub_no_space(void);
void Expr_div_no_space(void);
void Expr_mul_no_space(void);
void Expr_add_no_space_var(void);
void Expr_sub_no_space_var(void);
void Expr_div_no_space_var(void);
void Expr_mul_no_space_var(void);
void Expr_add_no_space_var_reverse(void);
void Expr_sub_no_space_var_reverse(void);
void Expr_div_no_space_var_reverse(void);
void Expr_mul_no_space_var_reverse(void);
void Expr_div_by_0(void);
void Expr_div_by_0_var(void);
void Expr_mod_by_0(void);
void Expr_mod_by_0_var(void);
void Expr_int_to_bool(void);
void Expr_bool_to_int(void);
void Expr_bool_to_uint(void);
void Expr_char(void);
void Expr_char_to_int(void);
void Expr_add_mul_3_int_literals(void);
void Expr_sub_mul_3_int_literals(void);
void Expr_div_mul_3_int_literals(void);
void Expr_add_div_3_int_literals(void);
void Expr_sub_div_3_int_literals(void);
void Expr_mul_div_3_int_literals(void);
void Expr_mul_add_mul_add_int_literals(void);
void Expr_mul_sub_mul_sub_int_literals(void);
void Expr_mul_div_mul_div_int_literals(void);
void Expr_div_add_div_add_int_literals(void);
void Expr_div_sub_div_sub_int_literals(void);
void Expr_div_sub_div_mul_int_literals(void);
void Expr_div_mul_div_mul_int_literals(void);
void Expr_add_2_flt_literals(void);
void Expr_sub_2_flt_literals(void);
void Expr_mul_2_flt_literals(void);
void Expr_div_2_flt_literals(void);
void Expr_add_2_int_neg_literals(void);
void Expr_sub_2_int_neg_literals(void);
void Expr_mul_2_int_neg_literals(void);
void Expr_div_2_int_neg_literals(void);
void Expr_mul_lparen_add_add_rparen_int_literals(void);
void Expr_mul_lparen_add_add_add_rparen_int_literals(void);
void Expr_mul_lparen_add_add_rparen_add_int_literals(void);
void Expr_lparen_add_add_rparen_mul_int_literals(void);
void Expr_lparen_add_add_add_rparen_mul_int_literals(void);
void Expr_double_paren_add_add(void);
void Expr_double_paren_literal(void);
void Expr_lparen_add_add_rparen_mul_lparen_add_add_rparen(void);
void Expr_float_result_add_2_int_literals(void);
void Expr_struct_result_implicit_members(void);
void Expr_struct_result_explicit_members(void);
void Expr_struct_result_explicit_members_reverse(void);
void Expr_struct_result_nested_implicit_members(void);
void Expr_struct_result_nested_explicit_members(void);
void Expr_struct_result_nested_explicit_members_reverse(void);
void Expr_struct_result_nested_explicit_dotmembers(void);
void Expr_struct_result_add_2_int_literals(void);
void Expr_struct_result_add_2_2_fields_int_literals(void);
void Expr_struct_result_add_3_int_literals(void);
void Expr_struct_result_lparen_int_rparen(void);
void Expr_add_to_var(void);
void Expr_add_var_to(void);
void Expr_var_member(void);
void Expr_var_member_member(void);
void Expr_var_element(void);
void Expr_var_element_element(void);
void Expr_var_member_element(void);
void Expr_var_member_element_inline(void);
void Expr_var_element_member(void);
void Expr_bool_cond_and_bool(void);
void Expr_bool_cond_or_bool(void);
void Expr_int_cond_and_int(void);
void Expr_int_cond_or_int(void);
void Expr_bool_cond_and_int(void);
void Expr_int_cond_and_bool(void);
void Expr_bool_cond_or_int(void);
void Expr_int_cond_or_bool(void);
void Expr_cond_eq_bool(void);
void Expr_cond_eq_int(void);
void Expr_cond_eq_enum(void);
void Expr_cond_eq_string(void);
void Expr_cond_eq_entity(void);
void Expr_cond_neq_bool(void);
void Expr_cond_neq_int(void);
void Expr_cond_neq_enum(void);
void Expr_cond_neq_string(void);
void Expr_cond_neq_entity(void);
void Expr_cond_eq_bool_int(void);
void Expr_cond_eq_int_flt(void);
void Expr_cond_eq_cond_and(void);
void Expr_cond_eq_cond_or(void);
void Expr_cond_gt_bool(void);
void Expr_cond_gt_int(void);
void Expr_cond_gt_flt(void);
void Expr_cond_gteq_bool(void);
void Expr_cond_gteq_int(void);
void Expr_cond_gteq_flt(void);
void Expr_cond_lt_bool(void);
void Expr_cond_lt_int(void);
void Expr_cond_lt_flt(void);
void Expr_cond_lteq_bool(void);
void Expr_cond_lteq_int(void);
void Expr_cond_lteq_flt(void);
void Expr_min_lparen_int_rparen(void);
void Expr_min_lparen_int_add_int_rparen(void);
void Expr_min_number_hex(void);
void Expr_min_number_bin(void);
void Expr_min_var(void);
void Expr_min_lparen_int_rparen_to_i64(void);
void Expr_min_lparen_int_rparen_to_i32(void);
void Expr_min_var_add(void);
void Expr_min_var_sub(void);
void Expr_min_var_mul(void);
void Expr_min_var_div(void);
void Expr_struct_w_min_var(void);
void Expr_struct_w_min_lparen_int_rparen(void);
void Expr_struct_w_min_lparen_var_rparen(void);
void Expr_not_bool(void);
void Expr_not_int(void);
void Expr_not_paren_int(void);
void Expr_not_paren_expr(void);
void Expr_not_var(void);
void Expr_shift_left_int(void);
void Expr_shift_right_int(void);
void Expr_shift_left_int_add_int(void);
void Expr_shift_left_int_mul_int(void);
void Expr_add_int_shift_left_int(void);
void Expr_mul_int_shift_left_int(void);
void Expr_add_int_shift_left_int_add_int(void);
void Expr_mul_int_shift_left_int_mul_int(void);
void Expr_entity_expr(void);
void Expr_entity_0_expr(void);
void Expr_entity_path_expr(void);
void Expr_root_lookup_func(void);
void Expr_root_lookup_func_not_found(void);
void Expr_root_lookup_func_w_entity_arg(void);
void Expr_entity_lookup_func(void);
void Expr_entity_lookup_func_not_found(void);
void Expr_entity_lookup_func_w_entity_arg(void);
void Expr_entity_parent_func(void);
void Expr_entity_name_func(void);
void Expr_entity_has_func(void);
void Expr_entity_has_func_w_pair(void);
void Expr_entity_has_func_w_pair_pair_invalid(void);
void Expr_entity_doc_name_func(void);
void Expr_entity_doc_uuid_func(void);
void Expr_entity_doc_brief_func(void);
void Expr_entity_doc_detail_func(void);
void Expr_entity_doc_link_func(void);
void Expr_entity_doc_color_func(void);
void Expr_entity_path_func(void);
void Expr_entity_chain_func(void);
void Expr_var_parent_func(void);
void Expr_var_name_func(void);
void Expr_var_doc_name_func(void);
void Expr_var_chain_func(void);
void Expr_root_func_void_return(void);
void Expr_root_func(void);
void Expr_scoped_func(void);
void Expr_root_func_w_1_arg(void);
void Expr_root_func_w_1_arg_expr(void);
void Expr_root_func_w_2_args(void);
void Expr_root_func_w_enum_arg(void);
void Expr_root_func_w_struct_arg(void);
void Expr_root_func_w_array_arg(void);
void Expr_root_func_mismatching_args(void);
void Expr_method_w_1_arg(void);
void Expr_vector_func_sum_w_int_literal(void);
void Expr_vector_func_sum_w_float_literal(void);
void Expr_vector_func_sum_w_i32(void);
void Expr_vector_func_sum_w_i64(void);
void Expr_vector_func_sum_w_int_literal_no_int_callback(void);
void Expr_vector_func_sum_w_i32_no_int_callback(void);
void Expr_vector_func_sum_w_i64_no_int_callback(void);
void Expr_vector_func_sum_w_struct(void);
void Expr_vector_func_sum_w_array(void);
void Expr_vector_func_sum_w_vector(void);
void Expr_vector_func_add_w_int_literal(void);
void Expr_vector_func_add_w_float_literal(void);
void Expr_vector_func_add_w_i32(void);
void Expr_vector_func_add_w_i64(void);
void Expr_vector_func_add_w_i32_w_literal(void);
void Expr_vector_func_add_w_i64_w_literal(void);
void Expr_vector_func_add_w_f32_w_literal(void);
void Expr_vector_func_add_w_f64_w_literal(void);
void Expr_vector_func_add_w_struct(void);
void Expr_vector_add_struct_int_literal(void);
void Expr_vector_add_struct_float_literal(void);
void Expr_vector_add_struct_i32(void);
void Expr_vector_add_struct_i64(void);
void Expr_vector_add_struct_f32(void);
void Expr_vector_add_struct_f64(void);
void Expr_vector_add_struct_struct(void);
void Expr_vector_add_struct_incompatible_struct(void);
void Expr_interpolate_string_w_i32_var(void);
void Expr_interpolate_string_w_string_var(void);
void Expr_interpolate_string_w_entity_var(void);
void Expr_interpolate_string_w_id_var(void);
void Expr_interpolate_string_w_var_not_found(void);
void Expr_interpolate_string_w_entity_var_0(void);
void Expr_interpolate_string_w_var_special_chars(void);
void Expr_interpolate_string_w_var_before_after_text(void);
void Expr_interpolate_string_w_curly_brackets_var(void);
void Expr_interpolate_string_w_curly_brackets_expr(void);
void Expr_interpolate_string_w_curly_brackets_expr_w_var(void);
void Expr_interpolate_string_w_curly_brackets_expr_w_composite_var(void);
void Expr_interpolate_string_w_escape_var_operator(void);
void Expr_interpolate_string_w_escape_curly_brackets(void);
void Expr_interpolate_string_w_func(void);
void Expr_interpolate_string_w_func_chain(void);
void Expr_interpolate_in_expr_var_name(void);
void Expr_interpolate_in_expr_var_name_w_pre(void);
void Expr_interpolate_in_expr_var_name_w_post(void);
void Expr_interpolate_in_expr_var_name_w_pre_post(void);
void Expr_interpolate_in_expr_var_name_bool(void);
void Expr_interpolate_in_expr_var_name_char(void);
void Expr_interpolate_in_expr_var_name_i32(void);
void Expr_interpolate_in_expr_var_name_u32(void);
void Expr_interpolate_in_expr_var_name_f32(void);
void Expr_interpolate_in_expr_var_name_entity(void);
void Expr_interpolate_in_expr_w_curly_brackets(void);
void Expr_interpolate_in_expr_w_curly_brackets_w_var(void);
void Expr_iter_to_vars_no_data(void);
void Expr_iter_to_vars_1_comp(void);
void Expr_iter_to_vars_2_comps(void);
void Expr_iter_to_vars_1_comp_1_tag(void);
void Expr_iter_to_vars_w_1_query_var(void);
void Expr_iter_to_vars_w_2_query_vars(void);
void Expr_component_expr(void);
void Expr_component_member_expr(void);
void Expr_component_elem_expr(void);
void Expr_component_expr_string(void);
void Expr_component_member_expr_string(void);
void Expr_component_elem_expr_string(void);
void Expr_component_inline_elem_expr_string(void);
void Expr_component_expr_in_object(void);
void Expr_component_member_expr_in_object(void);
void Expr_var_expr(void);
void Expr_var_member_expr(void);
void Expr_var_elem_expr(void);
void Expr_var_expr_string(void);
void Expr_var_member_expr_string(void);
void Expr_var_elem_expr_string(void);
void Expr_var_inline_elem_expr_string(void);
void Expr_var_expr_no_desc(void);
void Expr_var_expr_desc_w_no_vars(void);
void Expr_parse_eval(void);
void Expr_parse_eval_multiple_times(void);
void Expr_parse_error(void);
void Expr_parse_eval_error(void);
void Expr_remainder_after_number(void);
void Expr_remainder_after_string(void);
void Expr_remainder_after_unary(void);
void Expr_remainder_after_binary(void);
void Expr_remainder_after_parens(void);
void Expr_remainder_after_initializer(void);
void Expr_remainder_after_collection_initializer(void);
void Expr_remainder_after_initializer_w_newlines(void);
void Expr_remainder_after_initializer_w_crlf(void);
void Expr_remainder_after_initializer_before_parens(void);
void Expr_space_at_start(void);
void Expr_newline_at_start(void);
void Expr_crlf_at_start(void);
void Expr_global_const_var(void);
void Expr_scoped_global_const_var(void);
void Expr_escape_newline(void);
void Expr_match_i32_1_i_case(void);
void Expr_match_i32_2_i_cases(void);
void Expr_match_i32_2_i_f_cases(void);
void Expr_match_i32_2_f_i_cases(void);
void Expr_match_i32_3_i_cases(void);
void Expr_match_i32_3_i_i_f_cases(void);
void Expr_match_i32_3_i_f_i_cases(void);
void Expr_match_i32_3_f_i_i_cases(void);
void Expr_match_i32_1_struct_case(void);
void Expr_match_i32_2_struct_cases(void);
void Expr_match_i32_3_struct_cases(void);
void Expr_match_i32_empty_struct_cases(void);
void Expr_match_i32_struct_cases_unknown_type(void);
void Expr_match_i32_1_collection_case(void);
void Expr_match_i32_2_collection_cases(void);
void Expr_match_i32_3_collection_cases(void);
void Expr_match_i32_empty_collection_case(void);
void Expr_match_i32_collection_case_unknown_type(void);
void Expr_match_i32_struct_invalid_case_type(void);
void Expr_match_i32_collection_invalid_case_type(void);
void Expr_match_i32_string(void);
void Expr_match_enum_string(void);
void Expr_match_string_i(void);
void Expr_match_w_any(void);
void Expr_match_w_any_not_last(void);
void Expr_match_w_any_first(void);
void Expr_match_w_any_mismatching_type(void);
void Expr_match_i_w_any_f(void);
void Expr_identifier_as_var(void);
void Expr_member_w_identifier_as_var(void);
void Expr_member_w_identifier_as_var_and_entity(void);
void Expr_nested_member_w_identifier_as_var_and_entity(void);
void Expr_identifier_as_const_var(void);
void Expr_expr_w_identifier_as_var(void);
void Expr_initializer_w_identifier_as_var(void);
void Expr_new_entity(void);
void Expr_new_entity_w_component(void);
void Expr_new_entity_w_component_w_newline(void);
void Expr_new_entity_w_kind(void);
void Expr_new_entity_w_component_w_vars(void);
void Expr_new_named_entity(void);
void Expr_new_named_entity_w_component(void);
void Expr_new_named_entity_w_kind(void);
void Expr_new_name_expr_entity(void);
void Expr_new_name_expr_entity_w_component(void);
void Expr_new_name_expr_entity_w_kind(void);
void Expr_new_entity_w_unterminated_scope(void);

// Testsuite 'ExprAst'
void ExprAst_binary_f32_var_add_f32_var(void);
void ExprAst_binary_f32_var_add_int(void);
void ExprAst_binary_f32_var_div_int(void);
void ExprAst_binary_f32_var_add_flt(void);
void ExprAst_binary_f32_var_div_by_int_sub_int(void);
void ExprAst_interpolated_string_var(void);
void ExprAst_interpolated_string_curly_brackets(void);
void ExprAst_interpolated_string_curly_brackets_w_var(void);
void ExprAst_template_w_foldable_const(void);

// Testsuite 'Vars'
void Vars_declare_1_var(void);
void Vars_declare_2_vars(void);
void Vars_declare_vars_nested_scope(void);
void Vars_declare_vars_2_scopes(void);
void Vars_redeclare_var(void);
void Vars_i32_expr_w_i32_var(void);
void Vars_i32_expr_w_f32_var(void);
void Vars_i32_expr_w_string_var(void);
void Vars_string_expr_w_string_var(void);
void Vars_struct_expr_w_i32_vars(void);
void Vars_struct_expr_w_struct_var(void);
void Vars_nested_struct_expr_w_struct_var(void);
void Vars_redeclare_in_scope(void);
void Vars_init_fini_vars(void);

// Testsuite 'Serialize'
void Serialize_bool(void);
void Serialize_byte(void);
void Serialize_char(void);
void Serialize_i8(void);
void Serialize_i16(void);
void Serialize_i32(void);
void Serialize_i64(void);
void Serialize_iptr(void);
void Serialize_u8(void);
void Serialize_u16(void);
void Serialize_u32(void);
void Serialize_u64(void);
void Serialize_uptr(void);
void Serialize_float(void);
void Serialize_double(void);
void Serialize_string(void);
void Serialize_entity(void);
void Serialize_entity_10k(void);
void Serialize_id(void);
void Serialize_enum(void);
void Serialize_enum_underlying_i8(void);
void Serialize_enum_underlying_i16(void);
void Serialize_enum_underlying_i32(void);
void Serialize_enum_underlying_iptr(void);
void Serialize_enum_underlying_i64(void);
void Serialize_enum_underlying_u8(void);
void Serialize_enum_underlying_u16(void);
void Serialize_enum_underlying_u32(void);
void Serialize_enum_underlying_uptr(void);
void Serialize_enum_underlying_u64(void);
void Serialize_bitmask(void);
void Serialize_float_nan(void);
void Serialize_float_inf(void);
void Serialize_double_nan(void);
void Serialize_double_inf(void);
void Serialize_struct_enum(void);
void Serialize_struct_bitmask(void);
void Serialize_struct_i32(void);
void Serialize_struct_i32_i32(void);
void Serialize_struct_entity(void);
void Serialize_struct_id(void);
void Serialize_array_i32_3(void);
void Serialize_array_struct_i32_i32(void);
void Serialize_array_array_i32_3(void);
void Serialize_vector_i32_3(void);
void Serialize_vector_struct_i32_i32(void);
void Serialize_vector_array_i32_3(void);
void Serialize_entity_entity_after_float(void);
void Serialize_struct_nested_i32(void);
void Serialize_struct_nested_i32_i32(void);
void Serialize_struct_2_nested_i32_i32(void);
void Serialize_struct_i32_array_3(void);
void Serialize_struct_struct_i32_array_3(void);
void Serialize_struct_struct_i32_i32_array_3(void);
void Serialize_struct_w_array_type_i32_i32(void);
void Serialize_struct_w_array_type_struct(void);
void Serialize_struct_w_2_array_type_i32_i32(void);
void Serialize_struct_w_2_array_type_struct(void);
void Serialize_struct_partial(void);
void Serialize_escape_simple_string(void);
void Serialize_escape_newline(void);
void Serialize_escape_2_newlines(void);
void Serialize_escape_string_w_trailing_newline(void);
void Serialize_escape_string_w_2_trailing_newlines(void);
void Serialize_escape_string_w_delim(void);
void Serialize_opaque_i32(void);
void Serialize_opaque_string(void);
void Serialize_opaque_struct(void);
void Serialize_opaque_array(void);
void Serialize_opaque_vector(void);
void Serialize_opaque_string_vector(void);

// Testsuite 'Deserialize'
void Deserialize_setup(void);
void Deserialize_bool(void);
void Deserialize_byte(void);
void Deserialize_char(void);
void Deserialize_char_literal(void);
void Deserialize_i8(void);
void Deserialize_i16(void);
void Deserialize_i32(void);
void Deserialize_i64(void);
void Deserialize_iptr(void);
void Deserialize_u8(void);
void Deserialize_u16(void);
void Deserialize_u32(void);
void Deserialize_u64(void);
void Deserialize_uptr(void);
void Deserialize_float(void);
void Deserialize_double(void);
void Deserialize_negative_int(void);
void Deserialize_negative_float(void);
void Deserialize_invalid_i8(void);
void Deserialize_invalid_i16(void);
void Deserialize_invalid_i32(void);
void Deserialize_invalid_i64(void);
void Deserialize_invalid_iptr(void);
void Deserialize_invalid_u8(void);
void Deserialize_invalid_u16(void);
void Deserialize_invalid_u32(void);
void Deserialize_invalid_u64(void);
void Deserialize_invalid_uptr(void);
void Deserialize_invalid_float(void);
void Deserialize_invalid_double(void);
void Deserialize_string(void);
void Deserialize_entity(void);
void Deserialize_id(void);
void Deserialize_enum(void);
void Deserialize_bitmask(void);
void Deserialize_struct_enum(void);
void Deserialize_struct_bitmask(void);
void Deserialize_struct_i32(void);
void Deserialize_struct_i32_neg(void);
void Deserialize_struct_i32_i32(void);
void Deserialize_struct_string(void);
void Deserialize_struct_string_from_name(void);
void Deserialize_struct_string_from_path(void);
void Deserialize_struct_string_from_var(void);
void Deserialize_struct_entity(void);
void Deserialize_struct_id(void);
void Deserialize_struct_nested_i32(void);
void Deserialize_struct_nested_i32_i32(void);
void Deserialize_struct_2_nested_i32_i32(void);
void Deserialize_struct_nested_empty(void);
void Deserialize_struct_member_i32(void);
void Deserialize_struct_member_i32_neg(void);
void Deserialize_struct_member_i32_i32(void);
void Deserialize_struct_member_nested_i32(void);
void Deserialize_struct_member_nested_i32_i32(void);
void Deserialize_struct_member_2_nested_i32_i32(void);
void Deserialize_struct_member_2_nested_i32_i32_reverse(void);
void Deserialize_struct_from_var(void);
void Deserialize_struct_member_from_var(void);
void Deserialize_struct_member_auto_var(void);
void Deserialize_struct_member_auto_vars(void);
void Deserialize_struct_nested_member_auto_var(void);
void Deserialize_struct_nested_member_auto_vars(void);
void Deserialize_struct_auto_vars(void);
void Deserialize_struct_i32_array_3(void);
void Deserialize_struct_i32_array_3_empty(void);
void Deserialize_struct_struct_i32_array_3(void);
void Deserialize_struct_struct_i32_i32_array_3(void);
void Deserialize_struct_w_array_type_i32_i32(void);
void Deserialize_struct_w_array_type_struct(void);
void Deserialize_struct_w_2_array_type_i32_i32(void);
void Deserialize_struct_w_2_array_type_struct(void);
void Deserialize_struct_w_newline(void);
void Deserialize_struct_w_members_newline(void);
void Deserialize_struct_w_trailing_comma(void);
void Deserialize_struct_w_add_assign_expr(void);
void Deserialize_struct_w_mul_assign_expr(void);
void Deserialize_struct_w_add_assign_expr_invalid_type(void);
void Deserialize_array_w_trailing_comma(void);
void Deserialize_array_i32_2(void);
void Deserialize_array_string_2(void);
void Deserialize_vector_i32_0(void);
void Deserialize_vector_i32_2(void);
void Deserialize_vector_i32_0_into_2(void);
void Deserialize_vector_i32_1_into_2(void);
void Deserialize_vector_string_0(void);
void Deserialize_vector_string_2(void);
void Deserialize_vector_string_0_into_2(void);
void Deserialize_vector_string_1_into_2(void);
void Deserialize_vector_struct_0(void);
void Deserialize_vector_struct_2(void);
void Deserialize_vector_struct_2_empty(void);
void Deserialize_vector_struct_0_into_2(void);
void Deserialize_vector_struct_1_into_2(void);
void Deserialize_discover_type_int(void);
void Deserialize_discover_type_negative_int(void);
void Deserialize_discover_type_float(void);
void Deserialize_discover_type_negative_float(void);
void Deserialize_discover_type_string(void);
void Deserialize_discover_type_multiline_string(void);
void Deserialize_discover_type_entity(void);
void Deserialize_discover_type_bool(void);
void Deserialize_discover_type_unknown(void);
void Deserialize_discover_type_invalid(void);
void Deserialize_opaque_struct(void);
void Deserialize_opaque_struct_w_member(void);
void Deserialize_opaque_struct_w_member_reverse(void);
void Deserialize_struct_w_opaque_member(void);
void Deserialize_opaque_string(void);
void Deserialize_opaque_entity(void);
void Deserialize_struct_w_opaque_entity_member(void);
void Deserialize_opaque_vector_i32_0(void);
void Deserialize_opaque_vector_i32_2(void);
void Deserialize_opaque_vector_i32_0_into_2(void);
void Deserialize_opaque_vector_i32_1_into_2(void);
void Deserialize_opaque_vector_string_0(void);
void Deserialize_opaque_vector_string_2(void);
void Deserialize_opaque_vector_string_0_into_2(void);
void Deserialize_opaque_vector_string_1_into_2(void);
void Deserialize_opaque_vector_opaque_string_0(void);
void Deserialize_opaque_vector_opaque_string_2(void);
void Deserialize_opaque_vector_opaque_string_0_into_2(void);
void Deserialize_opaque_vector_opaque_string_1_into_2(void);
void Deserialize_opaque_vector_struct_0(void);
void Deserialize_opaque_vector_struct_2(void);
void Deserialize_opaque_vector_struct_0_into_2(void);
void Deserialize_opaque_vector_struct_1_into_2(void);

// Testsuite 'Fuzzing'
void Fuzzing_1(void);
void Fuzzing_2(void);
void Fuzzing_3(void);
void Fuzzing_4(void);
void Fuzzing_5(void);
void Fuzzing_6(void);
void Fuzzing_7(void);
void Fuzzing_8(void);
void Fuzzing_9(void);
void Fuzzing_10(void);
void Fuzzing_11(void);
void Fuzzing_12(void);
void Fuzzing_13(void);
void Fuzzing_14(void);

bake_test_case Eval_testcases[] = {
    {
        "null",
        Eval_null
    },
    {
        "empty",
        Eval_empty
    },
    {
        "space",
        Eval_space
    },
    {
        "space_newline",
        Eval_space_newline
    },
    {
        "two_empty_newlines",
        Eval_two_empty_newlines
    },
    {
        "three_empty_newlines",
        Eval_three_empty_newlines
    },
    {
        "newline_trailing_space",
        Eval_newline_trailing_space
    },
    {
        "newline_trailing_spaces",
        Eval_newline_trailing_spaces
    },
    {
        "multiple_trailing_newlines",
        Eval_multiple_trailing_newlines
    },
    {
        "space_crlf",
        Eval_space_crlf
    },
    {
        "two_empty_crlfs",
        Eval_two_empty_crlfs
    },
    {
        "three_empty_crlfs",
        Eval_three_empty_crlfs
    },
    {
        "crlf_trailing_space",
        Eval_crlf_trailing_space
    },
    {
        "crlf_trailing_spaces",
        Eval_crlf_trailing_spaces
    },
    {
        "multiple_trailing_crlfs",
        Eval_multiple_trailing_crlfs
    },
    {
        "entity",
        Eval_entity
    },
    {
        "entity_newline",
        Eval_entity_newline
    },
    {
        "entity_crlf",
        Eval_entity_crlf
    },
    {
        "entity_w_core_name",
        Eval_entity_w_core_name
    },
    {
        "2_entities",
        Eval_2_entities
    },
    {
        "2_entities_crlf",
        Eval_2_entities_crlf
    },
    {
        "line_comment",
        Eval_line_comment
    },
    {
        "line_comment_before_stmt",
        Eval_line_comment_before_stmt
    },
    {
        "line_comment_after_stmt",
        Eval_line_comment_after_stmt
    },
    {
        "line_comment_between_stmt",
        Eval_line_comment_between_stmt
    },
    {
        "multiple_line_comment",
        Eval_multiple_line_comment
    },
    {
        "multiple_line_comment_w_newlines",
        Eval_multiple_line_comment_w_newlines
    },
    {
        "line_comment_after_stmt_same_line",
        Eval_line_comment_after_stmt_same_line
    },
    {
        "line_comment_before_scope_open",
        Eval_line_comment_before_scope_open
    },
    {
        "line_comment_after_newline_before_scope_open",
        Eval_line_comment_after_newline_before_scope_open
    },
    {
        "line_comment_after_newline_before_newline_scope_open",
        Eval_line_comment_after_newline_before_newline_scope_open
    },
    {
        "multi_line_comment",
        Eval_multi_line_comment
    },
    {
        "multi_line_comment_before_stmt",
        Eval_multi_line_comment_before_stmt
    },
    {
        "multi_line_comment_after_stmt",
        Eval_multi_line_comment_after_stmt
    },
    {
        "multi_line_comment_between_stmt",
        Eval_multi_line_comment_between_stmt
    },
    {
        "multiple_multi_line_comment",
        Eval_multiple_multi_line_comment
    },
    {
        "multiple_multi_line_comment_w_newlines",
        Eval_multiple_multi_line_comment_w_newlines
    },
    {
        "multi_line_comment_after_stmt_same_line",
        Eval_multi_line_comment_after_stmt_same_line
    },
    {
        "multi_line_comment_before_scope_open",
        Eval_multi_line_comment_before_scope_open
    },
    {
        "multi_line_comment_after_newline_before_scope_open",
        Eval_multi_line_comment_after_newline_before_scope_open
    },
    {
        "multi_line_comment_multiple_lines",
        Eval_multi_line_comment_multiple_lines
    },
    {
        "hierarchy_1_child",
        Eval_hierarchy_1_child
    },
    {
        "hierarchy_2_children",
        Eval_hierarchy_2_children
    },
    {
        "hierarchy_1_child_same_line",
        Eval_hierarchy_1_child_same_line
    },
    {
        "hierarchy_2_children_same_line",
        Eval_hierarchy_2_children_same_line
    },
    {
        "hierarchy_2_children_same_line_no_trailing_comma",
        Eval_hierarchy_2_children_same_line_no_trailing_comma
    },
    {
        "entity_after_hierarchy",
        Eval_entity_after_hierarchy
    },
    {
        "newline_before_scope_open",
        Eval_newline_before_scope_open
    },
    {
        "newline_w_whitespace_before_scope_open",
        Eval_newline_w_whitespace_before_scope_open
    },
    {
        "2_newline_before_scope_open",
        Eval_2_newline_before_scope_open
    },
    {
        "2_newline_w_whitespace_before_scope_open",
        Eval_2_newline_w_whitespace_before_scope_open
    },
    {
        "hierarchy_2_levels",
        Eval_hierarchy_2_levels
    },
    {
        "hierarchy_2_levels_2_subtrees",
        Eval_hierarchy_2_levels_2_subtrees
    },
    {
        "create_in_scope",
        Eval_create_in_scope
    },
    {
        "hierarchy_w_pred_subj",
        Eval_hierarchy_w_pred_subj
    },
    {
        "hierarchy_custom_relation",
        Eval_hierarchy_custom_relation
    },
    {
        "hierarchy_custom_relation_2_levels",
        Eval_hierarchy_custom_relation_2_levels
    },
    {
        "hierarchy_custom_relation_apply_to_object",
        Eval_hierarchy_custom_relation_apply_to_object
    },
    {
        "hierarchy_custom_relation_apply_to_object_2_levels",
        Eval_hierarchy_custom_relation_apply_to_object_2_levels
    },
    {
        "entity_after_hierarchy_custom_relation",
        Eval_entity_after_hierarchy_custom_relation
    },
    {
        "entity_after_hierarchy_custom_relation_2_levels",
        Eval_entity_after_hierarchy_custom_relation_2_levels
    },
    {
        "with_tag",
        Eval_with_tag
    },
    {
        "with_tag_2_entities",
        Eval_with_tag_2_entities
    },
    {
        "with_tag_same_line",
        Eval_with_tag_same_line
    },
    {
        "with_tag_2_entities_same_line",
        Eval_with_tag_2_entities_same_line
    },
    {
        "with_tag_2_entities_same_line_no_trailing_comma",
        Eval_with_tag_2_entities_same_line_no_trailing_comma
    },
    {
        "with_tag_2_levels",
        Eval_with_tag_2_levels
    },
    {
        "with_tag_2_levels_2_subtrees",
        Eval_with_tag_2_levels_2_subtrees
    },
    {
        "with_n_tags",
        Eval_with_n_tags
    },
    {
        "with_n_tags_2_levels",
        Eval_with_n_tags_2_levels
    },
    {
        "with_after_scope",
        Eval_with_after_scope
    },
    {
        "with_after_with",
        Eval_with_after_with
    },
    {
        "scope_inside_with_inside_scope",
        Eval_scope_inside_with_inside_scope
    },
    {
        "with_inside_scope",
        Eval_with_inside_scope
    },
    {
        "with_tag_core_name",
        Eval_with_tag_core_name
    },
    {
        "inherit",
        Eval_inherit
    },
    {
        "inherit_newline",
        Eval_inherit_newline
    },
    {
        "inherit_w_colon",
        Eval_inherit_w_colon
    },
    {
        "inherit_w_colon_w_scope",
        Eval_inherit_w_colon_w_scope
    },
    {
        "assign_component_w_value",
        Eval_assign_component_w_value
    },
    {
        "assign_tag_in_assign_scope",
        Eval_assign_tag_in_assign_scope
    },
    {
        "assign_tag_in_assign_scope_same_line",
        Eval_assign_tag_in_assign_scope_same_line
    },
    {
        "assign_tag_in_assign_scope_core_name",
        Eval_assign_tag_in_assign_scope_core_name
    },
    {
        "assign_component_value_in_assign_scope",
        Eval_assign_component_value_in_assign_scope
    },
    {
        "assign_2_component_values_in_assign_scope",
        Eval_assign_2_component_values_in_assign_scope
    },
    {
        "type_and_assign_in_plecs",
        Eval_type_and_assign_in_plecs
    },
    {
        "type_and_assign_in_plecs_w_2_members",
        Eval_type_and_assign_in_plecs_w_2_members
    },
    {
        "type_and_assign_in_plecs_w_3_members",
        Eval_type_and_assign_in_plecs_w_3_members
    },
    {
        "type_and_assign_in_plecs_w_enum",
        Eval_type_and_assign_in_plecs_w_enum
    },
    {
        "type_and_assign_in_plecs_w_enum_using_meta",
        Eval_type_and_assign_in_plecs_w_enum_using_meta
    },
    {
        "type_and_assign_in_plecs_w_enum_primitive_using_meta",
        Eval_type_and_assign_in_plecs_w_enum_primitive_using_meta
    },
    {
        "type_and_assign_in_plecs_w_enum_primitive_and_struct",
        Eval_type_and_assign_in_plecs_w_enum_primitive_and_struct
    },
    {
        "type_and_assign_in_plecs_nested_member",
        Eval_type_and_assign_in_plecs_nested_member
    },
    {
        "dot_assign_nested_member",
        Eval_dot_assign_nested_member
    },
    {
        "dot_assign_binary_expr",
        Eval_dot_assign_binary_expr
    },
    {
        "open_scope_no_parent",
        Eval_open_scope_no_parent
    },
    {
        "create_subject_in_root_scope_w_resolvable_id",
        Eval_create_subject_in_root_scope_w_resolvable_id
    },
    {
        "create_subject_in_scope_w_resolvable_id",
        Eval_create_subject_in_scope_w_resolvable_id
    },
    {
        "create_subject_in_scope_w_resolvable_id_using",
        Eval_create_subject_in_scope_w_resolvable_id_using
    },
    {
        "using_scope",
        Eval_using_scope
    },
    {
        "using_nested_scope",
        Eval_using_nested_scope
    },
    {
        "using_nested_in_scope",
        Eval_using_nested_in_scope
    },
    {
        "using_with_scope",
        Eval_using_with_scope
    },
    {
        "using_w_entity_ref_in_value_2_members",
        Eval_using_w_entity_ref_in_value_2_members
    },
    {
        "using_w_entity_ref_in_value_3_members",
        Eval_using_w_entity_ref_in_value_3_members
    },
    {
        "script_w_only_using",
        Eval_script_w_only_using
    },
    {
        "2_using_scope",
        Eval_2_using_scope
    },
    {
        "2_using_in_different_scope",
        Eval_2_using_in_different_scope
    },
    {
        "empty_scope_after_using",
        Eval_empty_scope_after_using
    },
    {
        "scope_after_assign",
        Eval_scope_after_assign
    },
    {
        "assign_after_inherit",
        Eval_assign_after_inherit
    },
    {
        "multiple_tags_single_line",
        Eval_multiple_tags_single_line
    },
    {
        "multiple_pairs_single_line",
        Eval_multiple_pairs_single_line
    },
    {
        "var_single_line_no_newline",
        Eval_var_single_line_no_newline
    },
    {
        "var_single_line_composite_type_no_newline",
        Eval_var_single_line_composite_type_no_newline
    },
    {
        "multiple_vars_single_line",
        Eval_multiple_vars_single_line
    },
    {
        "multiple_assignments_single_line",
        Eval_multiple_assignments_single_line
    },
    {
        "2_stmts_in_scope_w_no_parent",
        Eval_2_stmts_in_scope_w_no_parent
    },
    {
        "assign_tag_to_parent",
        Eval_assign_tag_to_parent
    },
    {
        "assign_component_to_parent",
        Eval_assign_component_to_parent
    },
    {
        "assign_to_parent_pair_w_new_entities_in_scope",
        Eval_assign_to_parent_pair_w_new_entities_in_scope
    },
    {
        "assign_to_parent_pair_w_existing_entities_in_scope",
        Eval_assign_to_parent_pair_w_existing_entities_in_scope
    },
    {
        "default_child_component",
        Eval_default_child_component
    },
    {
        "default_child_component_w_assign",
        Eval_default_child_component_w_assign
    },
    {
        "struct_type_w_default_child_component",
        Eval_struct_type_w_default_child_component
    },
    {
        "struct_type_w_default_child_component_nested_member",
        Eval_struct_type_w_default_child_component_nested_member
    },
    {
        "enum_type_w_default_child_component",
        Eval_enum_type_w_default_child_component
    },
    {
        "enum_type_w_underlying_type",
        Eval_enum_type_w_underlying_type
    },
    {
        "default_type_from_with",
        Eval_default_type_from_with
    },
    {
        "default_type_from_nested_with",
        Eval_default_type_from_nested_with
    },
    {
        "default_type_from_with_in_entity_scope_w_default_type",
        Eval_default_type_from_with_in_entity_scope_w_default_type
    },
    {
        "default_type_from_entity_scope_in_with",
        Eval_default_type_from_entity_scope_in_with
    },
    {
        "scope_w_1_subj_and_2_pairs",
        Eval_scope_w_1_subj_and_2_pairs
    },
    {
        "inherit_from_multiple",
        Eval_inherit_from_multiple
    },
    {
        "assign_pair_component",
        Eval_assign_pair_component
    },
    {
        "assign_pair_component_in_scope",
        Eval_assign_pair_component_in_scope
    },
    {
        "assign_pair_component_in_script",
        Eval_assign_pair_component_in_script
    },
    {
        "assign_pair_component_in_script_update",
        Eval_assign_pair_component_in_script_update
    },
    {
        "assign_pair_component_w_newline",
        Eval_assign_pair_component_w_newline
    },
    {
        "assign_pair_component_w_newline_and_spaces",
        Eval_assign_pair_component_w_newline_and_spaces
    },
    {
        "assign_pair_component_w_empty",
        Eval_assign_pair_component_w_empty
    },
    {
        "assign_pair_component_w_newline_empty",
        Eval_assign_pair_component_w_newline_empty
    },
    {
        "assign_pair_component_w_newline_and_spaces_empty",
        Eval_assign_pair_component_w_newline_and_spaces_empty
    },
    {
        "assign_pair_component_after_component",
        Eval_assign_pair_component_after_component
    },
    {
        "assign_pair_component_after_int_component",
        Eval_assign_pair_component_after_int_component
    },
    {
        "assign_pair_component_after_entity_component",
        Eval_assign_pair_component_after_entity_component
    },
    {
        "set_entity_names",
        Eval_set_entity_names
    },
    {
        "oneof",
        Eval_oneof
    },
    {
        "brief_annotation",
        Eval_brief_annotation
    },
    {
        "name_annotation",
        Eval_name_annotation
    },
    {
        "link_annotation",
        Eval_link_annotation
    },
    {
        "color_annotation",
        Eval_color_annotation
    },
    {
        "multiple_annotations",
        Eval_multiple_annotations
    },
    {
        "annotation_w_trailing_space",
        Eval_annotation_w_trailing_space
    },
    {
        "multiline_string",
        Eval_multiline_string
    },
    {
        "declaration_w_underscore_name",
        Eval_declaration_w_underscore_name
    },
    {
        "annotate_declaration",
        Eval_annotate_declaration
    },
    {
        "anonymous_entity",
        Eval_anonymous_entity
    },
    {
        "anonymous_entity_in_scope",
        Eval_anonymous_entity_in_scope
    },
    {
        "anonymous_declaration",
        Eval_anonymous_declaration
    },
    {
        "const_var_int",
        Eval_const_var_int
    },
    {
        "const_var_float",
        Eval_const_var_float
    },
    {
        "const_var_bool",
        Eval_const_var_bool
    },
    {
        "const_var_string",
        Eval_const_var_string
    },
    {
        "const_var_struct",
        Eval_const_var_struct
    },
    {
        "const_var_scoped",
        Eval_const_var_scoped
    },
    {
        "const_var_int_deprecated_notation",
        Eval_const_var_int_deprecated_notation
    },
    {
        "assign_component_from_var",
        Eval_assign_component_from_var
    },
    {
        "assign_component_from_var_in_scope",
        Eval_assign_component_from_var_in_scope
    },
    {
        "scope_w_component_after_const_var",
        Eval_scope_w_component_after_const_var
    },
    {
        "component_after_const_paren_expr",
        Eval_component_after_const_paren_expr
    },
    {
        "component_after_const_add_expr",
        Eval_component_after_const_add_expr
    },
    {
        "component_after_const_sub_expr",
        Eval_component_after_const_sub_expr
    },
    {
        "component_after_const_mul_expr",
        Eval_component_after_const_mul_expr
    },
    {
        "component_after_const_div_expr",
        Eval_component_after_const_div_expr
    },
    {
        "parse_with",
        Eval_parse_with
    },
    {
        "parse_with_w_with",
        Eval_parse_with_w_with
    },
    {
        "parse_with_w_tag",
        Eval_parse_with_w_tag
    },
    {
        "parse_with_value",
        Eval_parse_with_value
    },
    {
        "parse_with_2_values",
        Eval_parse_with_2_values
    },
    {
        "parse_with_2_nested_values",
        Eval_parse_with_2_nested_values
    },
    {
        "parse_with_var",
        Eval_parse_with_var
    },
    {
        "parse_with_2_vars",
        Eval_parse_with_2_vars
    },
    {
        "parse_with_2_nested_vars",
        Eval_parse_with_2_nested_vars
    },
    {
        "parse_with_var_in_scope",
        Eval_parse_with_var_in_scope
    },
    {
        "assign_const_w_expr",
        Eval_assign_const_w_expr
    },
    {
        "const_w_type",
        Eval_const_w_type
    },
    {
        "typed_const_w_composite_type",
        Eval_typed_const_w_composite_type
    },
    {
        "assign_var_to_typed_const_w_composite_type",
        Eval_assign_var_to_typed_const_w_composite_type
    },
    {
        "using_wildcard",
        Eval_using_wildcard
    },
    {
        "single_line_comment_in_value",
        Eval_single_line_comment_in_value
    },
    {
        "single_line_comment_in_value_after_scope",
        Eval_single_line_comment_in_value_after_scope
    },
    {
        "multi_line_comment_in_value",
        Eval_multi_line_comment_in_value
    },
    {
        "multi_line_comment_in_value_after_scope",
        Eval_multi_line_comment_in_value_after_scope
    },
    {
        "module_stmt",
        Eval_module_stmt
    },
    {
        "nested_module_stmt",
        Eval_nested_module_stmt
    },
    {
        "module_stmt_w_scope",
        Eval_module_stmt_w_scope
    },
    {
        "module_stmt_w_nested_scope",
        Eval_module_stmt_w_nested_scope
    },
    {
        "assign_singleton_tag",
        Eval_assign_singleton_tag
    },
    {
        "assign_singleton_component",
        Eval_assign_singleton_component
    },
    {
        "assign_singleton_tag_w_scope",
        Eval_assign_singleton_tag_w_scope
    },
    {
        "assign_singleton_2_tags_w_scope",
        Eval_assign_singleton_2_tags_w_scope
    },
    {
        "assign_singleton_component_w_scope",
        Eval_assign_singleton_component_w_scope
    },
    {
        "assign_singleton_2_components_w_scope",
        Eval_assign_singleton_2_components_w_scope
    },
    {
        "with_pair_in_scope",
        Eval_with_pair_in_scope
    },
    {
        "with_pair_component_in_scope",
        Eval_with_pair_component_in_scope
    },
    {
        "pair_w_rel_var",
        Eval_pair_w_rel_var
    },
    {
        "pair_w_tgt_var",
        Eval_pair_w_tgt_var
    },
    {
        "array_component",
        Eval_array_component
    },
    {
        "on_set_w_kind_paren_no_reflection",
        Eval_on_set_w_kind_paren_no_reflection
    },
    {
        "on_set_w_kind_paren",
        Eval_on_set_w_kind_paren
    },
    {
        "on_set_w_kind_no_paren_no_reflection",
        Eval_on_set_w_kind_no_paren_no_reflection
    },
    {
        "on_set_w_kind_no_paren",
        Eval_on_set_w_kind_no_paren
    },
    {
        "on_set_w_single_assign",
        Eval_on_set_w_single_assign
    },
    {
        "on_set_w_single_assign_scoped_w_value",
        Eval_on_set_w_single_assign_scoped_w_value
    },
    {
        "on_set_w_single_assign_scoped_no_value",
        Eval_on_set_w_single_assign_scoped_no_value
    },
    {
        "if_true",
        Eval_if_true
    },
    {
        "if_true_no_else",
        Eval_if_true_no_else
    },
    {
        "if_false",
        Eval_if_false
    },
    {
        "if_true_var",
        Eval_if_true_var
    },
    {
        "if_false_var",
        Eval_if_false_var
    },
    {
        "if_10",
        Eval_if_10
    },
    {
        "if_0",
        Eval_if_0
    },
    {
        "if_true_in_scope",
        Eval_if_true_in_scope
    },
    {
        "if_false_in_scope",
        Eval_if_false_in_scope
    },
    {
        "if_lt",
        Eval_if_lt
    },
    {
        "if_lt_const",
        Eval_if_lt_const
    },
    {
        "if_else_if",
        Eval_if_else_if
    },
    {
        "if_else_if_else",
        Eval_if_else_if_else
    },
    {
        "if_else_if_else_if",
        Eval_if_else_if_else_if
    },
    {
        "if_else_newline_if",
        Eval_if_else_newline_if
    },
    {
        "if_else_space_newline_if",
        Eval_if_else_space_newline_if
    },
    {
        "isa_in_module",
        Eval_isa_in_module
    },
    {
        "isa_hierarchy",
        Eval_isa_hierarchy
    },
    {
        "isa_hierarchy_in_module",
        Eval_isa_hierarchy_in_module
    },
    {
        "custom_isa_hierarchy_in_module",
        Eval_custom_isa_hierarchy_in_module
    },
    {
        "custom_isa_hierarchy_in_subtree",
        Eval_custom_isa_hierarchy_in_subtree
    },
    {
        "inherit_w_kind",
        Eval_inherit_w_kind
    },
    {
        "inherit_w_kind_scope",
        Eval_inherit_w_kind_scope
    },
    {
        "inherit_w_kind_value",
        Eval_inherit_w_kind_value
    },
    {
        "inherit_w_kind_value_scope",
        Eval_inherit_w_kind_value_scope
    },
    {
        "multiple_inherit_w_kind",
        Eval_multiple_inherit_w_kind
    },
    {
        "multiple_inherit_w_kind_scope",
        Eval_multiple_inherit_w_kind_scope
    },
    {
        "auto_override_tag",
        Eval_auto_override_tag
    },
    {
        "auto_override_component",
        Eval_auto_override_component
    },
    {
        "auto_override_pair",
        Eval_auto_override_pair
    },
    {
        "auto_override_pair_component",
        Eval_auto_override_pair_component
    },
    {
        "lowercase_prefab_kind",
        Eval_lowercase_prefab_kind
    },
    {
        "assign_component_to_const",
        Eval_assign_component_to_const
    },
    {
        "assign_component_member_to_const",
        Eval_assign_component_member_to_const
    },
    {
        "prefab_w_slot",
        Eval_prefab_w_slot
    },
    {
        "prefab_w_slot_variant",
        Eval_prefab_w_slot_variant
    },
    {
        "const_w_component_expr",
        Eval_const_w_component_expr
    },
    {
        "const_w_component_expr_in_scope",
        Eval_const_w_component_expr_in_scope
    },
    {
        "const_w_component_expr_in_module",
        Eval_const_w_component_expr_in_module
    },
    {
        "const_w_component_in_scope_expr_in_scope",
        Eval_const_w_component_in_scope_expr_in_scope
    },
    {
        "const_w_component_in_scope_expr_in_module",
        Eval_const_w_component_in_scope_expr_in_module
    },
    {
        "const_w_component_and_entity_in_scope_expr_in_scope",
        Eval_const_w_component_and_entity_in_scope_expr_in_scope
    },
    {
        "const_w_component_and_entity_in_scope_expr_in_module",
        Eval_const_w_component_and_entity_in_scope_expr_in_module
    },
    {
        "path_tag_in_scope",
        Eval_path_tag_in_scope
    },
    {
        "path_tag_in_module",
        Eval_path_tag_in_module
    },
    {
        "path_tag_in_nested_scope",
        Eval_path_tag_in_nested_scope
    },
    {
        "path_tag_in_nested_module",
        Eval_path_tag_in_nested_module
    },
    {
        "dont_inherit_script_pair",
        Eval_dont_inherit_script_pair
    },
    {
        "update_script_w_anonymous",
        Eval_update_script_w_anonymous
    },
    {
        "update_script_w_anonymous_paren",
        Eval_update_script_w_anonymous_paren
    },
    {
        "clear_script",
        Eval_clear_script
    },
    {
        "clear_script_w_anonymous",
        Eval_clear_script_w_anonymous
    },
    {
        "clear_script_w_anonymous_paren",
        Eval_clear_script_w_anonymous_paren
    },
    {
        "partial_assign",
        Eval_partial_assign
    },
    {
        "partial_assign_nontrivial",
        Eval_partial_assign_nontrivial
    },
    {
        "partial_assign_with",
        Eval_partial_assign_with
    },
    {
        "partial_assign_nontrivial_with",
        Eval_partial_assign_nontrivial_with
    },
    {
        "partial_assign_with_large_array",
        Eval_partial_assign_with_large_array
    },
    {
        "non_trivial_var_component",
        Eval_non_trivial_var_component
    },
    {
        "non_trivial_var_with",
        Eval_non_trivial_var_with
    },
    {
        "update_template_w_tag",
        Eval_update_template_w_tag
    },
    {
        "assign_call_func",
        Eval_assign_call_func
    },
    {
        "assign_call_scoped_func",
        Eval_assign_call_scoped_func
    },
    {
        "assign_call_scoped_func_w_using",
        Eval_assign_call_scoped_func_w_using
    },
    {
        "eval_w_vars",
        Eval_eval_w_vars
    },
    {
        "eval_w_runtime",
        Eval_eval_w_runtime
    },
    {
        "eval_w_other_vars",
        Eval_eval_w_other_vars
    },
    {
        "eval_w_vars_different_order",
        Eval_eval_w_vars_different_order
    },
    {
        "eval_w_vars_different_order_var_component",
        Eval_eval_w_vars_different_order_var_component
    },
    {
        "eval_w_vars_different_order_with_var",
        Eval_eval_w_vars_different_order_with_var
    },
    {
        "eval_w_vars_different_order_pair_w_var",
        Eval_eval_w_vars_different_order_pair_w_var
    },
    {
        "eval_w_vars_different_order_pair_scope_w_var",
        Eval_eval_w_vars_different_order_pair_scope_w_var
    },
    {
        "component_in_entity_in_with_scope",
        Eval_component_in_entity_in_with_scope
    },
    {
        "entity_w_string_name",
        Eval_entity_w_string_name
    },
    {
        "entity_w_interpolated_name",
        Eval_entity_w_interpolated_name
    },
    {
        "entity_w_interpolated_name_w_var",
        Eval_entity_w_interpolated_name_w_var
    },
    {
        "entity_w_string_name_w_inherit",
        Eval_entity_w_string_name_w_inherit
    },
    {
        "entity_w_string_name_w_inherit_scope",
        Eval_entity_w_string_name_w_inherit_scope
    },
    {
        "entity_w_string_name_w_kind",
        Eval_entity_w_string_name_w_kind
    },
    {
        "entity_w_string_name_w_kind_value",
        Eval_entity_w_string_name_w_kind_value
    },
    {
        "entity_w_string_name_w_kind_scope",
        Eval_entity_w_string_name_w_kind_scope
    },
    {
        "entity_w_string_name_w_kind_value_scope",
        Eval_entity_w_string_name_w_kind_value_scope
    },
    {
        "entity_w_interpolated_name_w_var_in_scope",
        Eval_entity_w_interpolated_name_w_var_in_scope
    },
    {
        "for_range",
        Eval_for_range
    },
    {
        "for_range_vars",
        Eval_for_range_vars
    },
    {
        "for_range_vars_no_dollar",
        Eval_for_range_vars_no_dollar
    },
    {
        "for_range_1_4",
        Eval_for_range_1_4
    },
    {
        "for_range_min_1_2",
        Eval_for_range_min_1_2
    },
    {
        "variable_assign_self",
        Eval_variable_assign_self
    },
    {
        "func_w_entity_arg",
        Eval_func_w_entity_arg
    },
    {
        "func_w_entity_arg_w_using",
        Eval_func_w_entity_arg_w_using
    },
    {
        "method_w_entity_arg",
        Eval_method_w_entity_arg
    },
    {
        "method_w_entity_arg_w_using",
        Eval_method_w_entity_arg_w_using
    },
    {
        "assign_id",
        Eval_assign_id
    },
    {
        "assign_id_w_using",
        Eval_assign_id_w_using
    },
    {
        "const_assign_empty_initializer",
        Eval_const_assign_empty_initializer
    },
    {
        "const_assign_empty_collection_initializer",
        Eval_const_assign_empty_collection_initializer
    },
    {
        "const_i32_assign_empty_initializer",
        Eval_const_i32_assign_empty_initializer
    },
    {
        "const_i32_assign_empty_collection_initializer",
        Eval_const_i32_assign_empty_collection_initializer
    },
    {
        "component_w_match",
        Eval_component_w_match
    },
    {
        "component_w_match_invalid",
        Eval_component_w_match_invalid
    },
    {
        "pair_component_w_match",
        Eval_pair_component_w_match
    },
    {
        "component_assign_w_match",
        Eval_component_assign_w_match
    },
    {
        "const_w_match",
        Eval_const_w_match
    },
    {
        "component_w_assign_add",
        Eval_component_w_assign_add
    },
    {
        "component_w_assign_mul",
        Eval_component_w_assign_mul
    },
    {
        "opaque_struct_component",
        Eval_opaque_struct_component
    },
    {
        "opaque_string_component",
        Eval_opaque_string_component
    },
    {
        "add_component_w_invalid_ctor",
        Eval_add_component_w_invalid_ctor
    },
    {
        "add_component_pair_w_invalid_ctor",
        Eval_add_component_pair_w_invalid_ctor
    },
    {
        "add_component_as_tag_w_invalid_ctor",
        Eval_add_component_as_tag_w_invalid_ctor
    },
    {
        "add_component_as_tag_pair_w_invalid_ctor",
        Eval_add_component_as_tag_pair_w_invalid_ctor
    },
    {
        "interpolated_name_w_nested_for_loop",
        Eval_interpolated_name_w_nested_for_loop
    },
    {
        "interpolated_name_w_nested_for_loop_no_dollar_sign",
        Eval_interpolated_name_w_nested_for_loop_no_dollar_sign
    },
    {
        "interpolated_name_w_nested_for_loop_wrong_dollar_sign",
        Eval_interpolated_name_w_nested_for_loop_wrong_dollar_sign
    },
    {
        "interpolated_string_as_tag",
        Eval_interpolated_string_as_tag
    },
    {
        "interpolated_string_as_relationship",
        Eval_interpolated_string_as_relationship
    },
    {
        "interpolated_string_as_target",
        Eval_interpolated_string_as_target
    },
    {
        "interpolated_string_as_relationship_and_target",
        Eval_interpolated_string_as_relationship_and_target
    },
    {
        "interpolated_string_as_relationship_component",
        Eval_interpolated_string_as_relationship_component
    },
    {
        "interpolated_string_as_target_component",
        Eval_interpolated_string_as_target_component
    },
    {
        "interpolated_string_as_relationship_and_target_component",
        Eval_interpolated_string_as_relationship_and_target_component
    },
    {
        "dynamic_interpolated_string_as_tag",
        Eval_dynamic_interpolated_string_as_tag
    },
    {
        "dynamic_interpolated_string_as_relationship",
        Eval_dynamic_interpolated_string_as_relationship
    },
    {
        "dynamic_interpolated_string_as_target",
        Eval_dynamic_interpolated_string_as_target
    },
    {
        "assign_new_to_const_to_entity_member",
        Eval_assign_new_to_const_to_entity_member
    },
    {
        "assign_new_w_component_to_const_to_entity_member",
        Eval_assign_new_w_component_to_const_to_entity_member
    },
    {
        "assign_new_w_component_w_vars_to_const_to_entity_member",
        Eval_assign_new_w_component_w_vars_to_const_to_entity_member
    },
    {
        "assign_new_w_kind_to_const_to_entity_member",
        Eval_assign_new_w_kind_to_const_to_entity_member
    },
    {
        "assign_new_w_name_to_const_to_entity_member",
        Eval_assign_new_w_name_to_const_to_entity_member
    },
    {
        "assign_new_w_name_w_component_to_const_to_entity_member",
        Eval_assign_new_w_name_w_component_to_const_to_entity_member
    },
    {
        "assign_new_w_name_w_component_w_vars_to_const_to_entity_member",
        Eval_assign_new_w_name_w_component_w_vars_to_const_to_entity_member
    },
    {
        "assign_new_w_name_w_kind_to_const_to_entity_member",
        Eval_assign_new_w_name_w_kind_to_const_to_entity_member
    },
    {
        "assign_new_to_entity_member",
        Eval_assign_new_to_entity_member
    },
    {
        "assign_new_w_component_to_entity_member",
        Eval_assign_new_w_component_to_entity_member
    },
    {
        "assign_new_w_component_to_entity_member_w_newlines",
        Eval_assign_new_w_component_to_entity_member_w_newlines
    },
    {
        "assign_new_w_component_w_vars_to_entity_member",
        Eval_assign_new_w_component_w_vars_to_entity_member
    },
    {
        "assign_new_w_kind_to_entity_member",
        Eval_assign_new_w_kind_to_entity_member
    },
    {
        "assign_new_w_name_to_entity_member",
        Eval_assign_new_w_name_to_entity_member
    },
    {
        "assign_new_w_name_w_component_to_entity_member",
        Eval_assign_new_w_name_w_component_to_entity_member
    },
    {
        "assign_new_w_name_w_component_w_vars_to_entity_member",
        Eval_assign_new_w_name_w_component_w_vars_to_entity_member
    },
    {
        "assign_new_w_name_w_kind_to_entity_member",
        Eval_assign_new_w_name_w_kind_to_entity_member
    },
    {
        "assign_new_w_inheritance_to_entity_member",
        Eval_assign_new_w_inheritance_to_entity_member
    },
    {
        "assign_new_w_name_w_inheritance_to_entity_member",
        Eval_assign_new_w_name_w_inheritance_to_entity_member
    },
    {
        "assign_new_w_child_w_name_to_const",
        Eval_assign_new_w_child_w_name_to_const
    },
    {
        "assign_new_w_child_w_name_to_entity_member",
        Eval_assign_new_w_child_w_name_to_entity_member
    },
    {
        "assign_new_to_const_in_scope",
        Eval_assign_new_to_const_in_scope
    },
    {
        "assign_new_to_const_in_for",
        Eval_assign_new_to_const_in_for
    },
    {
        "implicit_var_as_tag",
        Eval_implicit_var_as_tag
    },
    {
        "implicit_var_as_relationship",
        Eval_implicit_var_as_relationship
    },
    {
        "implicit_var_as_target",
        Eval_implicit_var_as_target
    },
    {
        "export_const_var",
        Eval_export_const_var
    },
    {
        "export_const_var_w_type",
        Eval_export_const_var_w_type
    },
    {
        "export_const_var_w_struct",
        Eval_export_const_var_w_struct
    },
    {
        "export_const_var_redeclared",
        Eval_export_const_var_redeclared
    },
    {
        "export_const_var_redeclared_w_local_var",
        Eval_export_const_var_redeclared_w_local_var
    },
    {
        "local_const_var_redeclared_w_export_var",
        Eval_local_const_var_redeclared_w_export_var
    },
    {
        "export_const_var_used_by_other_script",
        Eval_export_const_var_used_by_other_script
    },
    {
        "export_const_var_as_component",
        Eval_export_const_var_as_component
    },
    {
        "export_scoped_const_var_as_component",
        Eval_export_scoped_const_var_as_component
    },
    {
        "export_const_var_in_scope",
        Eval_export_const_var_in_scope
    },
    {
        "export_scoped_const_var_used_by_other_script",
        Eval_export_scoped_const_var_used_by_other_script
    },
    {
        "run_script_w_module_from_scope",
        Eval_run_script_w_module_from_scope
    },
    {
        "run_script_w_nested_module_from_scope",
        Eval_run_script_w_nested_module_from_scope
    },
    {
        "run_script_w_nested_module_from_named_scope",
        Eval_run_script_w_nested_module_from_named_scope
    },
    {
        "run_script_w_nested_module_from_module",
        Eval_run_script_w_nested_module_from_module
    },
    {
        "run_script_w_nested_module_from_named_scope_managed",
        Eval_run_script_w_nested_module_from_named_scope_managed
    },
    {
        "vector_i32_component",
        Eval_vector_i32_component
    },
    {
        "vector_string_component",
        Eval_vector_string_component
    },
    {
        "vector_struct_component",
        Eval_vector_struct_component
    },
    {
        "vector_i32_export_var",
        Eval_vector_i32_export_var
    },
    {
        "vector_string_export_var",
        Eval_vector_string_export_var
    },
    {
        "vector_struct_export_var",
        Eval_vector_struct_export_var
    },
    {
        "opaque_vector_i32_component",
        Eval_opaque_vector_i32_component
    },
    {
        "opaque_vector_i32_export_var",
        Eval_opaque_vector_i32_export_var
    },
    {
        "pair_after_const_int",
        Eval_pair_after_const_int
    },
    {
        "pair_after_const_string",
        Eval_pair_after_const_string
    },
    {
        "pair_after_const_identifier",
        Eval_pair_after_const_identifier
    },
    {
        "tree_parent",
        Eval_tree_parent
    },
    {
        "tree_parent_nested",
        Eval_tree_parent_nested
    },
    {
        "tree_parent_nested_childof",
        Eval_tree_parent_nested_childof
    },
    {
        "tree_childof_nested_parent",
        Eval_tree_childof_nested_parent
    },
    {
        "tree_parent_existing_child",
        Eval_tree_parent_existing_child
    },
    {
        "tree_parent_existing_child_2",
        Eval_tree_parent_existing_child_2
    },
    {
        "tree_prefab_override_child",
        Eval_tree_prefab_override_child
    },
    {
        "tree_parent_nested_w_if_scope",
        Eval_tree_parent_nested_w_if_scope
    },
    {
        "tree_parent_nested_w_if_else_scope",
        Eval_tree_parent_nested_w_if_else_scope
    },
    {
        "tree_parent_nested_w_for_scope",
        Eval_tree_parent_nested_w_for_scope
    },
    {
        "tree_parent_nested_w_pair_scope",
        Eval_tree_parent_nested_w_pair_scope
    },
    {
        "tree_parent_nested_w_with_scope",
        Eval_tree_parent_nested_w_with_scope
    },
    {
        "update_after_add_remove_tree_parent",
        Eval_update_after_add_remove_tree_parent
    }
};

bake_test_case Template_testcases[] = {
    {
        "template_no_scope",
        Template_template_no_scope
    },
    {
        "template_no_props",
        Template_template_no_props
    },
    {
        "template_prop",
        Template_template_prop
    },
    {
        "template_prop_space_colon",
        Template_template_prop_space_colon
    },
    {
        "template_2_props",
        Template_template_2_props
    },
    {
        "template_w_using",
        Template_template_w_using
    },
    {
        "template_instance_w_default_values",
        Template_template_instance_w_default_values
    },
    {
        "template_instance_w_assign_default_values",
        Template_template_instance_w_assign_default_values
    },
    {
        "template_instance_w_overridden_values",
        Template_template_instance_w_overridden_values
    },
    {
        "template_w_prop_implicit_type",
        Template_template_w_prop_implicit_type
    },
    {
        "template_w_child",
        Template_template_w_child
    },
    {
        "template_w_child_parse_script",
        Template_template_w_child_parse_script
    },
    {
        "template_w_child_parse_script_twice",
        Template_template_w_child_parse_script_twice
    },
    {
        "template_w_child_update_after_parse",
        Template_template_w_child_update_after_parse
    },
    {
        "template_w_nested_child",
        Template_template_w_nested_child
    },
    {
        "template_w_prefab",
        Template_template_w_prefab
    },
    {
        "template_w_prefab_tree",
        Template_template_w_prefab_tree
    },
    {
        "template_w_nested_template",
        Template_template_w_nested_template
    },
    {
        "instantiate_prefab_w_template",
        Template_instantiate_prefab_w_template
    },
    {
        "template_w_prefab_w_template",
        Template_template_w_prefab_w_template
    },
    {
        "3_templates",
        Template_3_templates
    },
    {
        "template_nested_w_default_var",
        Template_template_nested_w_default_var
    },
    {
        "template_w_anonymous",
        Template_template_w_anonymous
    },
    {
        "template_w_anonymous_parse_again",
        Template_template_w_anonymous_parse_again
    },
    {
        "template_w_composite_prop",
        Template_template_w_composite_prop
    },
    {
        "template_with_with",
        Template_template_with_with
    },
    {
        "module_w_template",
        Template_module_w_template
    },
    {
        "module_w_nested_template",
        Template_module_w_nested_template
    },
    {
        "template_w_pair_w_this_var",
        Template_template_w_pair_w_this_var
    },
    {
        "template_w_pair_w_prop_var",
        Template_template_w_pair_w_prop_var
    },
    {
        "template_w_pair_w_const_var",
        Template_template_w_pair_w_const_var
    },
    {
        "template_w_pair_scope_w_this_var",
        Template_template_w_pair_scope_w_this_var
    },
    {
        "template_w_pair_scope_w_prop_var",
        Template_template_w_pair_scope_w_prop_var
    },
    {
        "template_w_pair_scope_w_const_var",
        Template_template_w_pair_scope_w_const_var
    },
    {
        "template_w_pair_w_unresolved_var_first",
        Template_template_w_pair_w_unresolved_var_first
    },
    {
        "template_w_pair_w_unresolved_var_second",
        Template_template_w_pair_w_unresolved_var_second
    },
    {
        "template_w_pair_scope_w_unresolved_var_first",
        Template_template_w_pair_scope_w_unresolved_var_first
    },
    {
        "template_w_pair_scope_w_unresolved_var_second",
        Template_template_w_pair_scope_w_unresolved_var_second
    },
    {
        "prop_without_using_meta",
        Template_prop_without_using_meta
    },
    {
        "hoist_var",
        Template_hoist_var
    },
    {
        "hoist_vars_nested",
        Template_hoist_vars_nested
    },
    {
        "hoist_vars_nested_w_masked",
        Template_hoist_vars_nested_w_masked
    },
    {
        "anonymous_template_instance",
        Template_anonymous_template_instance
    },
    {
        "anonymous_template_instance_no_scope",
        Template_anonymous_template_instance_no_scope
    },
    {
        "anonymous_template_instance_w_prop",
        Template_anonymous_template_instance_w_prop
    },
    {
        "anonymous_template_instance_w_prop_no_scope",
        Template_anonymous_template_instance_w_prop_no_scope
    },
    {
        "with_after_template",
        Template_with_after_template
    },
    {
        "with_in_scope_after_template",
        Template_with_in_scope_after_template
    },
    {
        "prefab_w_template",
        Template_prefab_w_template
    },
    {
        "template_in_scope",
        Template_template_in_scope
    },
    {
        "nested_templates_in_prefab",
        Template_nested_templates_in_prefab
    },
    {
        "entity_w_2_template_instances",
        Template_entity_w_2_template_instances
    },
    {
        "template_w_prefab_and_instance",
        Template_template_w_prefab_and_instance
    },
    {
        "template_w_with_var",
        Template_template_w_with_var
    },
    {
        "template_w_with_prop",
        Template_template_w_with_prop
    },
    {
        "template_w_child_w_var",
        Template_template_w_child_w_var
    },
    {
        "template_w_child_w_prop",
        Template_template_w_child_w_prop
    },
    {
        "fold_const",
        Template_fold_const
    },
    {
        "bulk_create_template",
        Template_bulk_create_template
    },
    {
        "template_w_expr_w_self_ref",
        Template_template_w_expr_w_self_ref
    },
    {
        "entity_w_assign_with_nested_template",
        Template_entity_w_assign_with_nested_template
    },
    {
        "template_w_for",
        Template_template_w_for
    },
    {
        "template_w_component_w_undefined_identifier",
        Template_template_w_component_w_undefined_identifier
    },
    {
        "template_w_child_component_w_undefined_identifier",
        Template_template_w_child_component_w_undefined_identifier
    },
    {
        "template_w_anonymous_child_component_w_undefined_identifier",
        Template_template_w_anonymous_child_component_w_undefined_identifier
    },
    {
        "clear_script_w_template_w_on_remove_observer",
        Template_clear_script_w_template_w_on_remove_observer
    },
    {
        "clear_script_w_template_w_on_remove_observer_added_after",
        Template_clear_script_w_template_w_on_remove_observer_added_after
    },
    {
        "component_w_assign_add",
        Template_component_w_assign_add
    },
    {
        "component_w_assign_mul",
        Template_component_w_assign_mul
    },
    {
        "prop_after_const",
        Template_prop_after_const
    },
    {
        "const_from_prop",
        Template_const_from_prop
    },
    {
        "redefine_nested_template_w_prefab",
        Template_redefine_nested_template_w_prefab
    },
    {
        "redefine_nested_template_w_prefab_2",
        Template_redefine_nested_template_w_prefab_2
    },
    {
        "redefine_nested_template_w_prefab_3",
        Template_redefine_nested_template_w_prefab_3
    },
    {
        "template_w_script_component",
        Template_template_w_script_component
    },
    {
        "template_w_script_pair_component",
        Template_template_w_script_pair_component
    },
    {
        "template_w_prop_and_pair",
        Template_template_w_prop_and_pair
    },
    {
        "template_w_name_annotation",
        Template_template_w_name_annotation
    },
    {
        "template_w_name_annotation_update_script",
        Template_template_w_name_annotation_update_script
    },
    {
        "template_w_tree_parent",
        Template_template_w_tree_parent
    },
    {
        "template_w_tree_parent_change_value",
        Template_template_w_tree_parent_change_value
    }
};

bake_test_case Error_testcases[] = {
    {
        "multi_line_comment_after_newline_before_newline_scope_open",
        Error_multi_line_comment_after_newline_before_newline_scope_open
    },
    {
        "missing_end_of_scope",
        Error_missing_end_of_scope
    },
    {
        "with_n_tags_2_levels_invalid_tag",
        Error_with_n_tags_2_levels_invalid_tag
    },
    {
        "assignment_to_non_component",
        Error_assignment_to_non_component
    },
    {
        "struct_w_member_w_assignment_to_nothing",
        Error_struct_w_member_w_assignment_to_nothing
    },
    {
        "struct_w_member_w_assignment_to_empty_scope",
        Error_struct_w_member_w_assignment_to_empty_scope
    },
    {
        "invalid_nested_assignment",
        Error_invalid_nested_assignment
    },
    {
        "invalid_partial_pair_assignment",
        Error_invalid_partial_pair_assignment
    },
    {
        "empty_assignment",
        Error_empty_assignment
    },
    {
        "empty_assignment_before_end_of_scope",
        Error_empty_assignment_before_end_of_scope
    },
    {
        "default_type_with_tag",
        Error_default_type_with_tag
    },
    {
        "invalid_oneof",
        Error_invalid_oneof
    },
    {
        "default_type_with_tag",
        Error_default_type_with_tag
    },
    {
        "unterminated_multiline_string",
        Error_unterminated_multiline_string
    },
    {
        "invalid_assign_multiline_string",
        Error_invalid_assign_multiline_string
    },
    {
        "const_var_redeclare",
        Error_const_var_redeclare
    },
    {
        "typed_const_w_composite_type_invalid_assignment",
        Error_typed_const_w_composite_type_invalid_assignment
    },
    {
        "unterminated_multi_line_comment_in_value",
        Error_unterminated_multi_line_comment_in_value
    },
    {
        "pair_w_rel_var_invalid_type",
        Error_pair_w_rel_var_invalid_type
    },
    {
        "pair_w_tgt_var_invalid_type",
        Error_pair_w_tgt_var_invalid_type
    },
    {
        "with_value_not_a_component",
        Error_with_value_not_a_component
    },
    {
        "component_in_with_scope",
        Error_component_in_with_scope
    },
    {
        "component_in_with_scope_nested",
        Error_component_in_with_scope_nested
    },
    {
        "assign_after_with_in_scope",
        Error_assign_after_with_in_scope
    },
    {
        "not_an_array_component",
        Error_not_an_array_component
    },
    {
        "array_component_w_curly_brackets",
        Error_array_component_w_curly_brackets
    },
    {
        "unknown_identifier",
        Error_unknown_identifier
    },
    {
        "unknown_identifier_for_int_field",
        Error_unknown_identifier_for_int_field
    },
    {
        "prefab_w_slot_no_parent",
        Error_prefab_w_slot_no_parent
    },
    {
        "tag_not_found",
        Error_tag_not_found
    },
    {
        "component_not_found",
        Error_component_not_found
    },
    {
        "pair_first_not_found",
        Error_pair_first_not_found
    },
    {
        "pair_second_not_found",
        Error_pair_second_not_found
    },
    {
        "kind_not_found",
        Error_kind_not_found
    },
    {
        "base_not_found",
        Error_base_not_found
    },
    {
        "entity_w_anonymous_tag",
        Error_entity_w_anonymous_tag
    },
    {
        "member_expr_without_value_end_of_scope",
        Error_member_expr_without_value_end_of_scope
    },
    {
        "member_expr_without_value_comma",
        Error_member_expr_without_value_comma
    },
    {
        "member_expr_without_value_newline",
        Error_member_expr_without_value_newline
    },
    {
        "2_member_expr_without_value",
        Error_2_member_expr_without_value
    },
    {
        "expr_junk_after_number",
        Error_expr_junk_after_number
    },
    {
        "expr_junk_after_unary_minus",
        Error_expr_junk_after_unary_minus
    },
    {
        "expr_comma_after_nothing",
        Error_expr_comma_after_nothing
    },
    {
        "expr_digit_with_two_dots",
        Error_expr_digit_with_two_dots
    },
    {
        "template_empty",
        Error_template_empty
    },
    {
        "template_unresolved_tag",
        Error_template_unresolved_tag
    },
    {
        "template_unresolved_component",
        Error_template_unresolved_component
    },
    {
        "template_unresolved_pair_relationship",
        Error_template_unresolved_pair_relationship
    },
    {
        "template_unresolved_pair_target",
        Error_template_unresolved_pair_target
    },
    {
        "template_unresolved_with_tag",
        Error_template_unresolved_with_tag
    },
    {
        "template_unresolved_with_component",
        Error_template_unresolved_with_component
    },
    {
        "template_unresolved_with_pair_relationship",
        Error_template_unresolved_with_pair_relationship
    },
    {
        "template_unresolved_with_pair_target",
        Error_template_unresolved_with_pair_target
    },
    {
        "template_unresolved_tag_in_child",
        Error_template_unresolved_tag_in_child
    },
    {
        "template_prop_no_type",
        Error_template_prop_no_type
    },
    {
        "template_w_composite_prop_invalid_assignment",
        Error_template_w_composite_prop_invalid_assignment
    },
    {
        "template_redeclare_prop_as_const",
        Error_template_redeclare_prop_as_const
    },
    {
        "template_redeclare_prop_as_prop",
        Error_template_redeclare_prop_as_prop
    },
    {
        "template_redeclare_const_as_const",
        Error_template_redeclare_const_as_const
    },
    {
        "template_w_invalid_var_in_expr",
        Error_template_w_invalid_var_in_expr
    },
    {
        "run_template_after_error",
        Error_run_template_after_error
    },
    {
        "update_template_after_error",
        Error_update_template_after_error
    },
    {
        "template_in_template",
        Error_template_in_template
    },
    {
        "unterminated_binary",
        Error_unterminated_binary
    },
    {
        "tag_in_with_scope",
        Error_tag_in_with_scope
    },
    {
        "tag_in_with_scope_2",
        Error_tag_in_with_scope_2
    },
    {
        "pair_tag_in_with_scope_2",
        Error_pair_tag_in_with_scope_2
    },
    {
        "component_in_with_scope",
        Error_component_in_with_scope
    },
    {
        "component_in_with_scope_2",
        Error_component_in_with_scope_2
    },
    {
        "component_in_with_scope_3",
        Error_component_in_with_scope_3
    },
    {
        "component_in_with_scope_4",
        Error_component_in_with_scope_4
    },
    {
        "component_in_with_scope_5",
        Error_component_in_with_scope_5
    },
    {
        "component_in_with_scope_nested",
        Error_component_in_with_scope_nested
    },
    {
        "component_in_with_scope_after_entity",
        Error_component_in_with_scope_after_entity
    },
    {
        "component_in_with_var_scope",
        Error_component_in_with_var_scope
    },
    {
        "component_in_with_in_template",
        Error_component_in_with_in_template
    },
    {
        "reload_script_w_component_w_error",
        Error_reload_script_w_component_w_error
    },
    {
        "reload_script_w_component_w_error_again",
        Error_reload_script_w_component_w_error_again
    },
    {
        "initializer_w_int_to_struct",
        Error_initializer_w_int_to_struct
    },
    {
        "script_initializer_w_int_to_struct",
        Error_script_initializer_w_int_to_struct
    },
    {
        "capture_error",
        Error_capture_error
    },
    {
        "unresolved_component_error_w_script_run",
        Error_unresolved_component_error_w_script_run
    },
    {
        "unresolved_component_error_w_script_init",
        Error_unresolved_component_error_w_script_init
    },
    {
        "unresolved_component_error_w_script_init_existing",
        Error_unresolved_component_error_w_script_init_existing
    },
    {
        "unresolved_component_error_w_script_eval",
        Error_unresolved_component_error_w_script_eval
    },
    {
        "unresolved_component_error_w_script_eval_multiple_times",
        Error_unresolved_component_error_w_script_eval_multiple_times
    },
    {
        "annotation_without_newline",
        Error_annotation_without_newline
    },
    {
        "annotation_without_entity",
        Error_annotation_without_entity
    },
    {
        "annotation_to_unresolved_identifier",
        Error_annotation_to_unresolved_identifier
    },
    {
        "annotation_to_unresolved_identifier_managed_parse_twice",
        Error_annotation_to_unresolved_identifier_managed_parse_twice
    },
    {
        "annotation_to_unresolved_identifier_managed_parse_twice_2",
        Error_annotation_to_unresolved_identifier_managed_parse_twice_2
    },
    {
        "annotation_to_tag",
        Error_annotation_to_tag
    }
};

bake_test_case Expr_testcases[] = {
    {
        "add_2_int_literals",
        Expr_add_2_int_literals
    },
    {
        "add_2_int_literals_twice",
        Expr_add_2_int_literals_twice
    },
    {
        "sub_2_int_literals",
        Expr_sub_2_int_literals
    },
    {
        "mul_2_int_literals",
        Expr_mul_2_int_literals
    },
    {
        "div_2_int_literals",
        Expr_div_2_int_literals
    },
    {
        "add_3_int_literals",
        Expr_add_3_int_literals
    },
    {
        "add_3_int_literals_dec_hex_bin",
        Expr_add_3_int_literals_dec_hex_bin
    },
    {
        "add_3_int_literals_twice",
        Expr_add_3_int_literals_twice
    },
    {
        "sub_3_int_literals",
        Expr_sub_3_int_literals
    },
    {
        "mul_3_int_literals",
        Expr_mul_3_int_literals
    },
    {
        "div_3_int_literals",
        Expr_div_3_int_literals
    },
    {
        "mod_2_int_literals",
        Expr_mod_2_int_literals
    },
    {
        "mod_2_flt_literals",
        Expr_mod_2_flt_literals
    },
    {
        "add_no_space",
        Expr_add_no_space
    },
    {
        "sub_no_space",
        Expr_sub_no_space
    },
    {
        "div_no_space",
        Expr_div_no_space
    },
    {
        "mul_no_space",
        Expr_mul_no_space
    },
    {
        "add_no_space_var",
        Expr_add_no_space_var
    },
    {
        "sub_no_space_var",
        Expr_sub_no_space_var
    },
    {
        "div_no_space_var",
        Expr_div_no_space_var
    },
    {
        "mul_no_space_var",
        Expr_mul_no_space_var
    },
    {
        "add_no_space_var_reverse",
        Expr_add_no_space_var_reverse
    },
    {
        "sub_no_space_var_reverse",
        Expr_sub_no_space_var_reverse
    },
    {
        "div_no_space_var_reverse",
        Expr_div_no_space_var_reverse
    },
    {
        "mul_no_space_var_reverse",
        Expr_mul_no_space_var_reverse
    },
    {
        "div_by_0",
        Expr_div_by_0
    },
    {
        "div_by_0_var",
        Expr_div_by_0_var
    },
    {
        "mod_by_0",
        Expr_mod_by_0
    },
    {
        "mod_by_0_var",
        Expr_mod_by_0_var
    },
    {
        "int_to_bool",
        Expr_int_to_bool
    },
    {
        "bool_to_int",
        Expr_bool_to_int
    },
    {
        "bool_to_uint",
        Expr_bool_to_uint
    },
    {
        "char",
        Expr_char
    },
    {
        "char_to_int",
        Expr_char_to_int
    },
    {
        "add_mul_3_int_literals",
        Expr_add_mul_3_int_literals
    },
    {
        "sub_mul_3_int_literals",
        Expr_sub_mul_3_int_literals
    },
    {
        "div_mul_3_int_literals",
        Expr_div_mul_3_int_literals
    },
    {
        "add_div_3_int_literals",
        Expr_add_div_3_int_literals
    },
    {
        "sub_div_3_int_literals",
        Expr_sub_div_3_int_literals
    },
    {
        "mul_div_3_int_literals",
        Expr_mul_div_3_int_literals
    },
    {
        "mul_add_mul_add_int_literals",
        Expr_mul_add_mul_add_int_literals
    },
    {
        "mul_sub_mul_sub_int_literals",
        Expr_mul_sub_mul_sub_int_literals
    },
    {
        "mul_div_mul_div_int_literals",
        Expr_mul_div_mul_div_int_literals
    },
    {
        "div_add_div_add_int_literals",
        Expr_div_add_div_add_int_literals
    },
    {
        "div_sub_div_sub_int_literals",
        Expr_div_sub_div_sub_int_literals
    },
    {
        "div_sub_div_mul_int_literals",
        Expr_div_sub_div_mul_int_literals
    },
    {
        "div_mul_div_mul_int_literals",
        Expr_div_mul_div_mul_int_literals
    },
    {
        "add_2_flt_literals",
        Expr_add_2_flt_literals
    },
    {
        "sub_2_flt_literals",
        Expr_sub_2_flt_literals
    },
    {
        "mul_2_flt_literals",
        Expr_mul_2_flt_literals
    },
    {
        "div_2_flt_literals",
        Expr_div_2_flt_literals
    },
    {
        "add_2_int_neg_literals",
        Expr_add_2_int_neg_literals
    },
    {
        "sub_2_int_neg_literals",
        Expr_sub_2_int_neg_literals
    },
    {
        "mul_2_int_neg_literals",
        Expr_mul_2_int_neg_literals
    },
    {
        "div_2_int_neg_literals",
        Expr_div_2_int_neg_literals
    },
    {
        "mul_lparen_add_add_rparen_int_literals",
        Expr_mul_lparen_add_add_rparen_int_literals
    },
    {
        "mul_lparen_add_add_add_rparen_int_literals",
        Expr_mul_lparen_add_add_add_rparen_int_literals
    },
    {
        "mul_lparen_add_add_rparen_add_int_literals",
        Expr_mul_lparen_add_add_rparen_add_int_literals
    },
    {
        "lparen_add_add_rparen_mul_int_literals",
        Expr_lparen_add_add_rparen_mul_int_literals
    },
    {
        "lparen_add_add_add_rparen_mul_int_literals",
        Expr_lparen_add_add_add_rparen_mul_int_literals
    },
    {
        "double_paren_add_add",
        Expr_double_paren_add_add
    },
    {
        "double_paren_literal",
        Expr_double_paren_literal
    },
    {
        "lparen_add_add_rparen_mul_lparen_add_add_rparen",
        Expr_lparen_add_add_rparen_mul_lparen_add_add_rparen
    },
    {
        "float_result_add_2_int_literals",
        Expr_float_result_add_2_int_literals
    },
    {
        "struct_result_implicit_members",
        Expr_struct_result_implicit_members
    },
    {
        "struct_result_explicit_members",
        Expr_struct_result_explicit_members
    },
    {
        "struct_result_explicit_members_reverse",
        Expr_struct_result_explicit_members_reverse
    },
    {
        "struct_result_nested_implicit_members",
        Expr_struct_result_nested_implicit_members
    },
    {
        "struct_result_nested_explicit_members",
        Expr_struct_result_nested_explicit_members
    },
    {
        "struct_result_nested_explicit_members_reverse",
        Expr_struct_result_nested_explicit_members_reverse
    },
    {
        "struct_result_nested_explicit_dotmembers",
        Expr_struct_result_nested_explicit_dotmembers
    },
    {
        "struct_result_add_2_int_literals",
        Expr_struct_result_add_2_int_literals
    },
    {
        "struct_result_add_2_2_fields_int_literals",
        Expr_struct_result_add_2_2_fields_int_literals
    },
    {
        "struct_result_add_3_int_literals",
        Expr_struct_result_add_3_int_literals
    },
    {
        "struct_result_lparen_int_rparen",
        Expr_struct_result_lparen_int_rparen
    },
    {
        "add_to_var",
        Expr_add_to_var
    },
    {
        "add_var_to",
        Expr_add_var_to
    },
    {
        "var_member",
        Expr_var_member
    },
    {
        "var_member_member",
        Expr_var_member_member
    },
    {
        "var_element",
        Expr_var_element
    },
    {
        "var_element_element",
        Expr_var_element_element
    },
    {
        "var_member_element",
        Expr_var_member_element
    },
    {
        "var_member_element_inline",
        Expr_var_member_element_inline
    },
    {
        "var_element_member",
        Expr_var_element_member
    },
    {
        "bool_cond_and_bool",
        Expr_bool_cond_and_bool
    },
    {
        "bool_cond_or_bool",
        Expr_bool_cond_or_bool
    },
    {
        "int_cond_and_int",
        Expr_int_cond_and_int
    },
    {
        "int_cond_or_int",
        Expr_int_cond_or_int
    },
    {
        "bool_cond_and_int",
        Expr_bool_cond_and_int
    },
    {
        "int_cond_and_bool",
        Expr_int_cond_and_bool
    },
    {
        "bool_cond_or_int",
        Expr_bool_cond_or_int
    },
    {
        "int_cond_or_bool",
        Expr_int_cond_or_bool
    },
    {
        "cond_eq_bool",
        Expr_cond_eq_bool
    },
    {
        "cond_eq_int",
        Expr_cond_eq_int
    },
    {
        "cond_eq_enum",
        Expr_cond_eq_enum
    },
    {
        "cond_eq_string",
        Expr_cond_eq_string
    },
    {
        "cond_eq_entity",
        Expr_cond_eq_entity
    },
    {
        "cond_neq_bool",
        Expr_cond_neq_bool
    },
    {
        "cond_neq_int",
        Expr_cond_neq_int
    },
    {
        "cond_neq_enum",
        Expr_cond_neq_enum
    },
    {
        "cond_neq_string",
        Expr_cond_neq_string
    },
    {
        "cond_neq_entity",
        Expr_cond_neq_entity
    },
    {
        "cond_eq_bool_int",
        Expr_cond_eq_bool_int
    },
    {
        "cond_eq_int_flt",
        Expr_cond_eq_int_flt
    },
    {
        "cond_eq_cond_and",
        Expr_cond_eq_cond_and
    },
    {
        "cond_eq_cond_or",
        Expr_cond_eq_cond_or
    },
    {
        "cond_gt_bool",
        Expr_cond_gt_bool
    },
    {
        "cond_gt_int",
        Expr_cond_gt_int
    },
    {
        "cond_gt_flt",
        Expr_cond_gt_flt
    },
    {
        "cond_gteq_bool",
        Expr_cond_gteq_bool
    },
    {
        "cond_gteq_int",
        Expr_cond_gteq_int
    },
    {
        "cond_gteq_flt",
        Expr_cond_gteq_flt
    },
    {
        "cond_lt_bool",
        Expr_cond_lt_bool
    },
    {
        "cond_lt_int",
        Expr_cond_lt_int
    },
    {
        "cond_lt_flt",
        Expr_cond_lt_flt
    },
    {
        "cond_lteq_bool",
        Expr_cond_lteq_bool
    },
    {
        "cond_lteq_int",
        Expr_cond_lteq_int
    },
    {
        "cond_lteq_flt",
        Expr_cond_lteq_flt
    },
    {
        "min_lparen_int_rparen",
        Expr_min_lparen_int_rparen
    },
    {
        "min_lparen_int_add_int_rparen",
        Expr_min_lparen_int_add_int_rparen
    },
    {
        "min_number_hex",
        Expr_min_number_hex
    },
    {
        "min_number_bin",
        Expr_min_number_bin
    },
    {
        "min_var",
        Expr_min_var
    },
    {
        "min_lparen_int_rparen_to_i64",
        Expr_min_lparen_int_rparen_to_i64
    },
    {
        "min_lparen_int_rparen_to_i32",
        Expr_min_lparen_int_rparen_to_i32
    },
    {
        "min_var_add",
        Expr_min_var_add
    },
    {
        "min_var_sub",
        Expr_min_var_sub
    },
    {
        "min_var_mul",
        Expr_min_var_mul
    },
    {
        "min_var_div",
        Expr_min_var_div
    },
    {
        "struct_w_min_var",
        Expr_struct_w_min_var
    },
    {
        "struct_w_min_lparen_int_rparen",
        Expr_struct_w_min_lparen_int_rparen
    },
    {
        "struct_w_min_lparen_var_rparen",
        Expr_struct_w_min_lparen_var_rparen
    },
    {
        "not_bool",
        Expr_not_bool
    },
    {
        "not_int",
        Expr_not_int
    },
    {
        "not_paren_int",
        Expr_not_paren_int
    },
    {
        "not_paren_expr",
        Expr_not_paren_expr
    },
    {
        "not_var",
        Expr_not_var
    },
    {
        "shift_left_int",
        Expr_shift_left_int
    },
    {
        "shift_right_int",
        Expr_shift_right_int
    },
    {
        "shift_left_int_add_int",
        Expr_shift_left_int_add_int
    },
    {
        "shift_left_int_mul_int",
        Expr_shift_left_int_mul_int
    },
    {
        "add_int_shift_left_int",
        Expr_add_int_shift_left_int
    },
    {
        "mul_int_shift_left_int",
        Expr_mul_int_shift_left_int
    },
    {
        "add_int_shift_left_int_add_int",
        Expr_add_int_shift_left_int_add_int
    },
    {
        "mul_int_shift_left_int_mul_int",
        Expr_mul_int_shift_left_int_mul_int
    },
    {
        "entity_expr",
        Expr_entity_expr
    },
    {
        "entity_0_expr",
        Expr_entity_0_expr
    },
    {
        "entity_path_expr",
        Expr_entity_path_expr
    },
    {
        "root_lookup_func",
        Expr_root_lookup_func
    },
    {
        "root_lookup_func_not_found",
        Expr_root_lookup_func_not_found
    },
    {
        "root_lookup_func_w_entity_arg",
        Expr_root_lookup_func_w_entity_arg
    },
    {
        "entity_lookup_func",
        Expr_entity_lookup_func
    },
    {
        "entity_lookup_func_not_found",
        Expr_entity_lookup_func_not_found
    },
    {
        "entity_lookup_func_w_entity_arg",
        Expr_entity_lookup_func_w_entity_arg
    },
    {
        "entity_parent_func",
        Expr_entity_parent_func
    },
    {
        "entity_name_func",
        Expr_entity_name_func
    },
    {
        "entity_has_func",
        Expr_entity_has_func
    },
    {
        "entity_has_func_w_pair",
        Expr_entity_has_func_w_pair
    },
    {
        "entity_has_func_w_pair_pair_invalid",
        Expr_entity_has_func_w_pair_pair_invalid
    },
    {
        "entity_doc_name_func",
        Expr_entity_doc_name_func
    },
    {
        "entity_doc_uuid_func",
        Expr_entity_doc_uuid_func
    },
    {
        "entity_doc_brief_func",
        Expr_entity_doc_brief_func
    },
    {
        "entity_doc_detail_func",
        Expr_entity_doc_detail_func
    },
    {
        "entity_doc_link_func",
        Expr_entity_doc_link_func
    },
    {
        "entity_doc_color_func",
        Expr_entity_doc_color_func
    },
    {
        "entity_path_func",
        Expr_entity_path_func
    },
    {
        "entity_chain_func",
        Expr_entity_chain_func
    },
    {
        "var_parent_func",
        Expr_var_parent_func
    },
    {
        "var_name_func",
        Expr_var_name_func
    },
    {
        "var_doc_name_func",
        Expr_var_doc_name_func
    },
    {
        "var_chain_func",
        Expr_var_chain_func
    },
    {
        "root_func_void_return",
        Expr_root_func_void_return
    },
    {
        "root_func",
        Expr_root_func
    },
    {
        "scoped_func",
        Expr_scoped_func
    },
    {
        "root_func_w_1_arg",
        Expr_root_func_w_1_arg
    },
    {
        "root_func_w_1_arg_expr",
        Expr_root_func_w_1_arg_expr
    },
    {
        "root_func_w_2_args",
        Expr_root_func_w_2_args
    },
    {
        "root_func_w_enum_arg",
        Expr_root_func_w_enum_arg
    },
    {
        "root_func_w_struct_arg",
        Expr_root_func_w_struct_arg
    },
    {
        "root_func_w_array_arg",
        Expr_root_func_w_array_arg
    },
    {
        "root_func_mismatching_args",
        Expr_root_func_mismatching_args
    },
    {
        "method_w_1_arg",
        Expr_method_w_1_arg
    },
    {
        "vector_func_sum_w_int_literal",
        Expr_vector_func_sum_w_int_literal
    },
    {
        "vector_func_sum_w_float_literal",
        Expr_vector_func_sum_w_float_literal
    },
    {
        "vector_func_sum_w_i32",
        Expr_vector_func_sum_w_i32
    },
    {
        "vector_func_sum_w_i64",
        Expr_vector_func_sum_w_i64
    },
    {
        "vector_func_sum_w_int_literal_no_int_callback",
        Expr_vector_func_sum_w_int_literal_no_int_callback
    },
    {
        "vector_func_sum_w_i32_no_int_callback",
        Expr_vector_func_sum_w_i32_no_int_callback
    },
    {
        "vector_func_sum_w_i64_no_int_callback",
        Expr_vector_func_sum_w_i64_no_int_callback
    },
    {
        "vector_func_sum_w_struct",
        Expr_vector_func_sum_w_struct
    },
    {
        "vector_func_sum_w_array",
        Expr_vector_func_sum_w_array
    },
    {
        "vector_func_sum_w_vector",
        Expr_vector_func_sum_w_vector
    },
    {
        "vector_func_add_w_int_literal",
        Expr_vector_func_add_w_int_literal
    },
    {
        "vector_func_add_w_float_literal",
        Expr_vector_func_add_w_float_literal
    },
    {
        "vector_func_add_w_i32",
        Expr_vector_func_add_w_i32
    },
    {
        "vector_func_add_w_i64",
        Expr_vector_func_add_w_i64
    },
    {
        "vector_func_add_w_i32_w_literal",
        Expr_vector_func_add_w_i32_w_literal
    },
    {
        "vector_func_add_w_i64_w_literal",
        Expr_vector_func_add_w_i64_w_literal
    },
    {
        "vector_func_add_w_f32_w_literal",
        Expr_vector_func_add_w_f32_w_literal
    },
    {
        "vector_func_add_w_f64_w_literal",
        Expr_vector_func_add_w_f64_w_literal
    },
    {
        "vector_func_add_w_struct",
        Expr_vector_func_add_w_struct
    },
    {
        "vector_add_struct_int_literal",
        Expr_vector_add_struct_int_literal
    },
    {
        "vector_add_struct_float_literal",
        Expr_vector_add_struct_float_literal
    },
    {
        "vector_add_struct_i32",
        Expr_vector_add_struct_i32
    },
    {
        "vector_add_struct_i64",
        Expr_vector_add_struct_i64
    },
    {
        "vector_add_struct_f32",
        Expr_vector_add_struct_f32
    },
    {
        "vector_add_struct_f64",
        Expr_vector_add_struct_f64
    },
    {
        "vector_add_struct_struct",
        Expr_vector_add_struct_struct
    },
    {
        "vector_add_struct_incompatible_struct",
        Expr_vector_add_struct_incompatible_struct
    },
    {
        "interpolate_string_w_i32_var",
        Expr_interpolate_string_w_i32_var
    },
    {
        "interpolate_string_w_string_var",
        Expr_interpolate_string_w_string_var
    },
    {
        "interpolate_string_w_entity_var",
        Expr_interpolate_string_w_entity_var
    },
    {
        "interpolate_string_w_id_var",
        Expr_interpolate_string_w_id_var
    },
    {
        "interpolate_string_w_var_not_found",
        Expr_interpolate_string_w_var_not_found
    },
    {
        "interpolate_string_w_entity_var_0",
        Expr_interpolate_string_w_entity_var_0
    },
    {
        "interpolate_string_w_var_special_chars",
        Expr_interpolate_string_w_var_special_chars
    },
    {
        "interpolate_string_w_var_before_after_text",
        Expr_interpolate_string_w_var_before_after_text
    },
    {
        "interpolate_string_w_curly_brackets_var",
        Expr_interpolate_string_w_curly_brackets_var
    },
    {
        "interpolate_string_w_curly_brackets_expr",
        Expr_interpolate_string_w_curly_brackets_expr
    },
    {
        "interpolate_string_w_curly_brackets_expr_w_var",
        Expr_interpolate_string_w_curly_brackets_expr_w_var
    },
    {
        "interpolate_string_w_curly_brackets_expr_w_composite_var",
        Expr_interpolate_string_w_curly_brackets_expr_w_composite_var
    },
    {
        "interpolate_string_w_escape_var_operator",
        Expr_interpolate_string_w_escape_var_operator
    },
    {
        "interpolate_string_w_escape_curly_brackets",
        Expr_interpolate_string_w_escape_curly_brackets
    },
    {
        "interpolate_string_w_func",
        Expr_interpolate_string_w_func
    },
    {
        "interpolate_string_w_func_chain",
        Expr_interpolate_string_w_func_chain
    },
    {
        "interpolate_in_expr_var_name",
        Expr_interpolate_in_expr_var_name
    },
    {
        "interpolate_in_expr_var_name_w_pre",
        Expr_interpolate_in_expr_var_name_w_pre
    },
    {
        "interpolate_in_expr_var_name_w_post",
        Expr_interpolate_in_expr_var_name_w_post
    },
    {
        "interpolate_in_expr_var_name_w_pre_post",
        Expr_interpolate_in_expr_var_name_w_pre_post
    },
    {
        "interpolate_in_expr_var_name_bool",
        Expr_interpolate_in_expr_var_name_bool
    },
    {
        "interpolate_in_expr_var_name_char",
        Expr_interpolate_in_expr_var_name_char
    },
    {
        "interpolate_in_expr_var_name_i32",
        Expr_interpolate_in_expr_var_name_i32
    },
    {
        "interpolate_in_expr_var_name_u32",
        Expr_interpolate_in_expr_var_name_u32
    },
    {
        "interpolate_in_expr_var_name_f32",
        Expr_interpolate_in_expr_var_name_f32
    },
    {
        "interpolate_in_expr_var_name_entity",
        Expr_interpolate_in_expr_var_name_entity
    },
    {
        "interpolate_in_expr_w_curly_brackets",
        Expr_interpolate_in_expr_w_curly_brackets
    },
    {
        "interpolate_in_expr_w_curly_brackets_w_var",
        Expr_interpolate_in_expr_w_curly_brackets_w_var
    },
    {
        "iter_to_vars_no_data",
        Expr_iter_to_vars_no_data
    },
    {
        "iter_to_vars_1_comp",
        Expr_iter_to_vars_1_comp
    },
    {
        "iter_to_vars_2_comps",
        Expr_iter_to_vars_2_comps
    },
    {
        "iter_to_vars_1_comp_1_tag",
        Expr_iter_to_vars_1_comp_1_tag
    },
    {
        "iter_to_vars_w_1_query_var",
        Expr_iter_to_vars_w_1_query_var
    },
    {
        "iter_to_vars_w_2_query_vars",
        Expr_iter_to_vars_w_2_query_vars
    },
    {
        "component_expr",
        Expr_component_expr
    },
    {
        "component_member_expr",
        Expr_component_member_expr
    },
    {
        "component_elem_expr",
        Expr_component_elem_expr
    },
    {
        "component_expr_string",
        Expr_component_expr_string
    },
    {
        "component_member_expr_string",
        Expr_component_member_expr_string
    },
    {
        "component_elem_expr_string",
        Expr_component_elem_expr_string
    },
    {
        "component_inline_elem_expr_string",
        Expr_component_inline_elem_expr_string
    },
    {
        "component_expr_in_object",
        Expr_component_expr_in_object
    },
    {
        "component_member_expr_in_object",
        Expr_component_member_expr_in_object
    },
    {
        "var_expr",
        Expr_var_expr
    },
    {
        "var_member_expr",
        Expr_var_member_expr
    },
    {
        "var_elem_expr",
        Expr_var_elem_expr
    },
    {
        "var_expr_string",
        Expr_var_expr_string
    },
    {
        "var_member_expr_string",
        Expr_var_member_expr_string
    },
    {
        "var_elem_expr_string",
        Expr_var_elem_expr_string
    },
    {
        "var_inline_elem_expr_string",
        Expr_var_inline_elem_expr_string
    },
    {
        "var_expr_no_desc",
        Expr_var_expr_no_desc
    },
    {
        "var_expr_desc_w_no_vars",
        Expr_var_expr_desc_w_no_vars
    },
    {
        "parse_eval",
        Expr_parse_eval
    },
    {
        "parse_eval_multiple_times",
        Expr_parse_eval_multiple_times
    },
    {
        "parse_error",
        Expr_parse_error
    },
    {
        "parse_eval_error",
        Expr_parse_eval_error
    },
    {
        "remainder_after_number",
        Expr_remainder_after_number
    },
    {
        "remainder_after_string",
        Expr_remainder_after_string
    },
    {
        "remainder_after_unary",
        Expr_remainder_after_unary
    },
    {
        "remainder_after_binary",
        Expr_remainder_after_binary
    },
    {
        "remainder_after_parens",
        Expr_remainder_after_parens
    },
    {
        "remainder_after_initializer",
        Expr_remainder_after_initializer
    },
    {
        "remainder_after_collection_initializer",
        Expr_remainder_after_collection_initializer
    },
    {
        "remainder_after_initializer_w_newlines",
        Expr_remainder_after_initializer_w_newlines
    },
    {
        "remainder_after_initializer_w_crlf",
        Expr_remainder_after_initializer_w_crlf
    },
    {
        "remainder_after_initializer_before_parens",
        Expr_remainder_after_initializer_before_parens
    },
    {
        "space_at_start",
        Expr_space_at_start
    },
    {
        "newline_at_start",
        Expr_newline_at_start
    },
    {
        "crlf_at_start",
        Expr_crlf_at_start
    },
    {
        "global_const_var",
        Expr_global_const_var
    },
    {
        "scoped_global_const_var",
        Expr_scoped_global_const_var
    },
    {
        "escape_newline",
        Expr_escape_newline
    },
    {
        "match_i32_1_i_case",
        Expr_match_i32_1_i_case
    },
    {
        "match_i32_2_i_cases",
        Expr_match_i32_2_i_cases
    },
    {
        "match_i32_2_i_f_cases",
        Expr_match_i32_2_i_f_cases
    },
    {
        "match_i32_2_f_i_cases",
        Expr_match_i32_2_f_i_cases
    },
    {
        "match_i32_3_i_cases",
        Expr_match_i32_3_i_cases
    },
    {
        "match_i32_3_i_i_f_cases",
        Expr_match_i32_3_i_i_f_cases
    },
    {
        "match_i32_3_i_f_i_cases",
        Expr_match_i32_3_i_f_i_cases
    },
    {
        "match_i32_3_f_i_i_cases",
        Expr_match_i32_3_f_i_i_cases
    },
    {
        "match_i32_1_struct_case",
        Expr_match_i32_1_struct_case
    },
    {
        "match_i32_2_struct_cases",
        Expr_match_i32_2_struct_cases
    },
    {
        "match_i32_3_struct_cases",
        Expr_match_i32_3_struct_cases
    },
    {
        "match_i32_empty_struct_cases",
        Expr_match_i32_empty_struct_cases
    },
    {
        "match_i32_struct_cases_unknown_type",
        Expr_match_i32_struct_cases_unknown_type
    },
    {
        "match_i32_1_collection_case",
        Expr_match_i32_1_collection_case
    },
    {
        "match_i32_2_collection_cases",
        Expr_match_i32_2_collection_cases
    },
    {
        "match_i32_3_collection_cases",
        Expr_match_i32_3_collection_cases
    },
    {
        "match_i32_empty_collection_case",
        Expr_match_i32_empty_collection_case
    },
    {
        "match_i32_collection_case_unknown_type",
        Expr_match_i32_collection_case_unknown_type
    },
    {
        "match_i32_struct_invalid_case_type",
        Expr_match_i32_struct_invalid_case_type
    },
    {
        "match_i32_collection_invalid_case_type",
        Expr_match_i32_collection_invalid_case_type
    },
    {
        "match_i32_string",
        Expr_match_i32_string
    },
    {
        "match_enum_string",
        Expr_match_enum_string
    },
    {
        "match_string_i",
        Expr_match_string_i
    },
    {
        "match_w_any",
        Expr_match_w_any
    },
    {
        "match_w_any_not_last",
        Expr_match_w_any_not_last
    },
    {
        "match_w_any_first",
        Expr_match_w_any_first
    },
    {
        "match_w_any_mismatching_type",
        Expr_match_w_any_mismatching_type
    },
    {
        "match_i_w_any_f",
        Expr_match_i_w_any_f
    },
    {
        "identifier_as_var",
        Expr_identifier_as_var
    },
    {
        "member_w_identifier_as_var",
        Expr_member_w_identifier_as_var
    },
    {
        "member_w_identifier_as_var_and_entity",
        Expr_member_w_identifier_as_var_and_entity
    },
    {
        "nested_member_w_identifier_as_var_and_entity",
        Expr_nested_member_w_identifier_as_var_and_entity
    },
    {
        "identifier_as_const_var",
        Expr_identifier_as_const_var
    },
    {
        "expr_w_identifier_as_var",
        Expr_expr_w_identifier_as_var
    },
    {
        "initializer_w_identifier_as_var",
        Expr_initializer_w_identifier_as_var
    },
    {
        "new_entity",
        Expr_new_entity
    },
    {
        "new_entity_w_component",
        Expr_new_entity_w_component
    },
    {
        "new_entity_w_component_w_newline",
        Expr_new_entity_w_component_w_newline
    },
    {
        "new_entity_w_kind",
        Expr_new_entity_w_kind
    },
    {
        "new_entity_w_component_w_vars",
        Expr_new_entity_w_component_w_vars
    },
    {
        "new_named_entity",
        Expr_new_named_entity
    },
    {
        "new_named_entity_w_component",
        Expr_new_named_entity_w_component
    },
    {
        "new_named_entity_w_kind",
        Expr_new_named_entity_w_kind
    },
    {
        "new_name_expr_entity",
        Expr_new_name_expr_entity
    },
    {
        "new_name_expr_entity_w_component",
        Expr_new_name_expr_entity_w_component
    },
    {
        "new_name_expr_entity_w_kind",
        Expr_new_name_expr_entity_w_kind
    },
    {
        "new_entity_w_unterminated_scope",
        Expr_new_entity_w_unterminated_scope
    }
};

bake_test_case ExprAst_testcases[] = {
    {
        "binary_f32_var_add_f32_var",
        ExprAst_binary_f32_var_add_f32_var
    },
    {
        "binary_f32_var_add_int",
        ExprAst_binary_f32_var_add_int
    },
    {
        "binary_f32_var_div_int",
        ExprAst_binary_f32_var_div_int
    },
    {
        "binary_f32_var_add_flt",
        ExprAst_binary_f32_var_add_flt
    },
    {
        "binary_f32_var_div_by_int_sub_int",
        ExprAst_binary_f32_var_div_by_int_sub_int
    },
    {
        "interpolated_string_var",
        ExprAst_interpolated_string_var
    },
    {
        "interpolated_string_curly_brackets",
        ExprAst_interpolated_string_curly_brackets
    },
    {
        "interpolated_string_curly_brackets_w_var",
        ExprAst_interpolated_string_curly_brackets_w_var
    },
    {
        "template_w_foldable_const",
        ExprAst_template_w_foldable_const
    }
};

bake_test_case Vars_testcases[] = {
    {
        "declare_1_var",
        Vars_declare_1_var
    },
    {
        "declare_2_vars",
        Vars_declare_2_vars
    },
    {
        "declare_vars_nested_scope",
        Vars_declare_vars_nested_scope
    },
    {
        "declare_vars_2_scopes",
        Vars_declare_vars_2_scopes
    },
    {
        "redeclare_var",
        Vars_redeclare_var
    },
    {
        "i32_expr_w_i32_var",
        Vars_i32_expr_w_i32_var
    },
    {
        "i32_expr_w_f32_var",
        Vars_i32_expr_w_f32_var
    },
    {
        "i32_expr_w_string_var",
        Vars_i32_expr_w_string_var
    },
    {
        "string_expr_w_string_var",
        Vars_string_expr_w_string_var
    },
    {
        "struct_expr_w_i32_vars",
        Vars_struct_expr_w_i32_vars
    },
    {
        "struct_expr_w_struct_var",
        Vars_struct_expr_w_struct_var
    },
    {
        "nested_struct_expr_w_struct_var",
        Vars_nested_struct_expr_w_struct_var
    },
    {
        "redeclare_in_scope",
        Vars_redeclare_in_scope
    },
    {
        "init_fini_vars",
        Vars_init_fini_vars
    }
};

bake_test_case Serialize_testcases[] = {
    {
        "bool",
        Serialize_bool
    },
    {
        "byte",
        Serialize_byte
    },
    {
        "char",
        Serialize_char
    },
    {
        "i8",
        Serialize_i8
    },
    {
        "i16",
        Serialize_i16
    },
    {
        "i32",
        Serialize_i32
    },
    {
        "i64",
        Serialize_i64
    },
    {
        "iptr",
        Serialize_iptr
    },
    {
        "u8",
        Serialize_u8
    },
    {
        "u16",
        Serialize_u16
    },
    {
        "u32",
        Serialize_u32
    },
    {
        "u64",
        Serialize_u64
    },
    {
        "uptr",
        Serialize_uptr
    },
    {
        "float",
        Serialize_float
    },
    {
        "double",
        Serialize_double
    },
    {
        "string",
        Serialize_string
    },
    {
        "entity",
        Serialize_entity
    },
    {
        "entity_10k",
        Serialize_entity_10k
    },
    {
        "id",
        Serialize_id
    },
    {
        "enum",
        Serialize_enum
    },
    {
        "enum_underlying_i8",
        Serialize_enum_underlying_i8
    },
    {
        "enum_underlying_i16",
        Serialize_enum_underlying_i16
    },
    {
        "enum_underlying_i32",
        Serialize_enum_underlying_i32
    },
    {
        "enum_underlying_iptr",
        Serialize_enum_underlying_iptr
    },
    {
        "enum_underlying_i64",
        Serialize_enum_underlying_i64
    },
    {
        "enum_underlying_u8",
        Serialize_enum_underlying_u8
    },
    {
        "enum_underlying_u16",
        Serialize_enum_underlying_u16
    },
    {
        "enum_underlying_u32",
        Serialize_enum_underlying_u32
    },
    {
        "enum_underlying_uptr",
        Serialize_enum_underlying_uptr
    },
    {
        "enum_underlying_u64",
        Serialize_enum_underlying_u64
    },
    {
        "bitmask",
        Serialize_bitmask
    },
    {
        "float_nan",
        Serialize_float_nan
    },
    {
        "float_inf",
        Serialize_float_inf
    },
    {
        "double_nan",
        Serialize_double_nan
    },
    {
        "double_inf",
        Serialize_double_inf
    },
    {
        "struct_enum",
        Serialize_struct_enum
    },
    {
        "struct_bitmask",
        Serialize_struct_bitmask
    },
    {
        "struct_i32",
        Serialize_struct_i32
    },
    {
        "struct_i32_i32",
        Serialize_struct_i32_i32
    },
    {
        "struct_entity",
        Serialize_struct_entity
    },
    {
        "struct_id",
        Serialize_struct_id
    },
    {
        "array_i32_3",
        Serialize_array_i32_3
    },
    {
        "array_struct_i32_i32",
        Serialize_array_struct_i32_i32
    },
    {
        "array_array_i32_3",
        Serialize_array_array_i32_3
    },
    {
        "vector_i32_3",
        Serialize_vector_i32_3
    },
    {
        "vector_struct_i32_i32",
        Serialize_vector_struct_i32_i32
    },
    {
        "vector_array_i32_3",
        Serialize_vector_array_i32_3
    },
    {
        "entity_entity_after_float",
        Serialize_entity_entity_after_float
    },
    {
        "struct_nested_i32",
        Serialize_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        Serialize_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        Serialize_struct_2_nested_i32_i32
    },
    {
        "struct_i32_array_3",
        Serialize_struct_i32_array_3
    },
    {
        "struct_struct_i32_array_3",
        Serialize_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        Serialize_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        Serialize_struct_w_array_type_i32_i32
    },
    {
        "struct_w_array_type_struct",
        Serialize_struct_w_array_type_struct
    },
    {
        "struct_w_2_array_type_i32_i32",
        Serialize_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_struct",
        Serialize_struct_w_2_array_type_struct
    },
    {
        "struct_partial",
        Serialize_struct_partial
    },
    {
        "escape_simple_string",
        Serialize_escape_simple_string
    },
    {
        "escape_newline",
        Serialize_escape_newline
    },
    {
        "escape_2_newlines",
        Serialize_escape_2_newlines
    },
    {
        "escape_string_w_trailing_newline",
        Serialize_escape_string_w_trailing_newline
    },
    {
        "escape_string_w_2_trailing_newlines",
        Serialize_escape_string_w_2_trailing_newlines
    },
    {
        "escape_string_w_delim",
        Serialize_escape_string_w_delim
    },
    {
        "opaque_i32",
        Serialize_opaque_i32
    },
    {
        "opaque_string",
        Serialize_opaque_string
    },
    {
        "opaque_struct",
        Serialize_opaque_struct
    },
    {
        "opaque_array",
        Serialize_opaque_array
    },
    {
        "opaque_vector",
        Serialize_opaque_vector
    },
    {
        "opaque_string_vector",
        Serialize_opaque_string_vector
    }
};

bake_test_case Deserialize_testcases[] = {
    {
        "bool",
        Deserialize_bool
    },
    {
        "byte",
        Deserialize_byte
    },
    {
        "char",
        Deserialize_char
    },
    {
        "char_literal",
        Deserialize_char_literal
    },
    {
        "i8",
        Deserialize_i8
    },
    {
        "i16",
        Deserialize_i16
    },
    {
        "i32",
        Deserialize_i32
    },
    {
        "i64",
        Deserialize_i64
    },
    {
        "iptr",
        Deserialize_iptr
    },
    {
        "u8",
        Deserialize_u8
    },
    {
        "u16",
        Deserialize_u16
    },
    {
        "u32",
        Deserialize_u32
    },
    {
        "u64",
        Deserialize_u64
    },
    {
        "uptr",
        Deserialize_uptr
    },
    {
        "float",
        Deserialize_float
    },
    {
        "double",
        Deserialize_double
    },
    {
        "negative_int",
        Deserialize_negative_int
    },
    {
        "negative_float",
        Deserialize_negative_float
    },
    {
        "invalid_i8",
        Deserialize_invalid_i8
    },
    {
        "invalid_i16",
        Deserialize_invalid_i16
    },
    {
        "invalid_i32",
        Deserialize_invalid_i32
    },
    {
        "invalid_i64",
        Deserialize_invalid_i64
    },
    {
        "invalid_iptr",
        Deserialize_invalid_iptr
    },
    {
        "invalid_u8",
        Deserialize_invalid_u8
    },
    {
        "invalid_u16",
        Deserialize_invalid_u16
    },
    {
        "invalid_u32",
        Deserialize_invalid_u32
    },
    {
        "invalid_u64",
        Deserialize_invalid_u64
    },
    {
        "invalid_uptr",
        Deserialize_invalid_uptr
    },
    {
        "invalid_float",
        Deserialize_invalid_float
    },
    {
        "invalid_double",
        Deserialize_invalid_double
    },
    {
        "string",
        Deserialize_string
    },
    {
        "entity",
        Deserialize_entity
    },
    {
        "id",
        Deserialize_id
    },
    {
        "enum",
        Deserialize_enum
    },
    {
        "bitmask",
        Deserialize_bitmask
    },
    {
        "struct_enum",
        Deserialize_struct_enum
    },
    {
        "struct_bitmask",
        Deserialize_struct_bitmask
    },
    {
        "struct_i32",
        Deserialize_struct_i32
    },
    {
        "struct_i32_neg",
        Deserialize_struct_i32_neg
    },
    {
        "struct_i32_i32",
        Deserialize_struct_i32_i32
    },
    {
        "struct_string",
        Deserialize_struct_string
    },
    {
        "struct_string_from_name",
        Deserialize_struct_string_from_name
    },
    {
        "struct_string_from_path",
        Deserialize_struct_string_from_path
    },
    {
        "struct_string_from_var",
        Deserialize_struct_string_from_var
    },
    {
        "struct_entity",
        Deserialize_struct_entity
    },
    {
        "struct_id",
        Deserialize_struct_id
    },
    {
        "struct_nested_i32",
        Deserialize_struct_nested_i32
    },
    {
        "struct_nested_i32_i32",
        Deserialize_struct_nested_i32_i32
    },
    {
        "struct_2_nested_i32_i32",
        Deserialize_struct_2_nested_i32_i32
    },
    {
        "struct_nested_empty",
        Deserialize_struct_nested_empty
    },
    {
        "struct_member_i32",
        Deserialize_struct_member_i32
    },
    {
        "struct_member_i32_neg",
        Deserialize_struct_member_i32_neg
    },
    {
        "struct_member_i32_i32",
        Deserialize_struct_member_i32_i32
    },
    {
        "struct_member_nested_i32",
        Deserialize_struct_member_nested_i32
    },
    {
        "struct_member_nested_i32_i32",
        Deserialize_struct_member_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32",
        Deserialize_struct_member_2_nested_i32_i32
    },
    {
        "struct_member_2_nested_i32_i32_reverse",
        Deserialize_struct_member_2_nested_i32_i32_reverse
    },
    {
        "struct_from_var",
        Deserialize_struct_from_var
    },
    {
        "struct_member_from_var",
        Deserialize_struct_member_from_var
    },
    {
        "struct_member_auto_var",
        Deserialize_struct_member_auto_var
    },
    {
        "struct_member_auto_vars",
        Deserialize_struct_member_auto_vars
    },
    {
        "struct_nested_member_auto_var",
        Deserialize_struct_nested_member_auto_var
    },
    {
        "struct_nested_member_auto_vars",
        Deserialize_struct_nested_member_auto_vars
    },
    {
        "struct_auto_vars",
        Deserialize_struct_auto_vars
    },
    {
        "struct_i32_array_3",
        Deserialize_struct_i32_array_3
    },
    {
        "struct_i32_array_3_empty",
        Deserialize_struct_i32_array_3_empty
    },
    {
        "struct_struct_i32_array_3",
        Deserialize_struct_struct_i32_array_3
    },
    {
        "struct_struct_i32_i32_array_3",
        Deserialize_struct_struct_i32_i32_array_3
    },
    {
        "struct_w_array_type_i32_i32",
        Deserialize_struct_w_array_type_i32_i32
    },
    {
        "struct_w_array_type_struct",
        Deserialize_struct_w_array_type_struct
    },
    {
        "struct_w_2_array_type_i32_i32",
        Deserialize_struct_w_2_array_type_i32_i32
    },
    {
        "struct_w_2_array_type_struct",
        Deserialize_struct_w_2_array_type_struct
    },
    {
        "struct_w_newline",
        Deserialize_struct_w_newline
    },
    {
        "struct_w_members_newline",
        Deserialize_struct_w_members_newline
    },
    {
        "struct_w_trailing_comma",
        Deserialize_struct_w_trailing_comma
    },
    {
        "struct_w_add_assign_expr",
        Deserialize_struct_w_add_assign_expr
    },
    {
        "struct_w_mul_assign_expr",
        Deserialize_struct_w_mul_assign_expr
    },
    {
        "struct_w_add_assign_expr_invalid_type",
        Deserialize_struct_w_add_assign_expr_invalid_type
    },
    {
        "array_w_trailing_comma",
        Deserialize_array_w_trailing_comma
    },
    {
        "array_i32_2",
        Deserialize_array_i32_2
    },
    {
        "array_string_2",
        Deserialize_array_string_2
    },
    {
        "vector_i32_0",
        Deserialize_vector_i32_0
    },
    {
        "vector_i32_2",
        Deserialize_vector_i32_2
    },
    {
        "vector_i32_0_into_2",
        Deserialize_vector_i32_0_into_2
    },
    {
        "vector_i32_1_into_2",
        Deserialize_vector_i32_1_into_2
    },
    {
        "vector_string_0",
        Deserialize_vector_string_0
    },
    {
        "vector_string_2",
        Deserialize_vector_string_2
    },
    {
        "vector_string_0_into_2",
        Deserialize_vector_string_0_into_2
    },
    {
        "vector_string_1_into_2",
        Deserialize_vector_string_1_into_2
    },
    {
        "vector_struct_0",
        Deserialize_vector_struct_0
    },
    {
        "vector_struct_2",
        Deserialize_vector_struct_2
    },
    {
        "vector_struct_2_empty",
        Deserialize_vector_struct_2_empty
    },
    {
        "vector_struct_0_into_2",
        Deserialize_vector_struct_0_into_2
    },
    {
        "vector_struct_1_into_2",
        Deserialize_vector_struct_1_into_2
    },
    {
        "discover_type_int",
        Deserialize_discover_type_int
    },
    {
        "discover_type_negative_int",
        Deserialize_discover_type_negative_int
    },
    {
        "discover_type_float",
        Deserialize_discover_type_float
    },
    {
        "discover_type_negative_float",
        Deserialize_discover_type_negative_float
    },
    {
        "discover_type_string",
        Deserialize_discover_type_string
    },
    {
        "discover_type_multiline_string",
        Deserialize_discover_type_multiline_string
    },
    {
        "discover_type_entity",
        Deserialize_discover_type_entity
    },
    {
        "discover_type_bool",
        Deserialize_discover_type_bool
    },
    {
        "discover_type_unknown",
        Deserialize_discover_type_unknown
    },
    {
        "discover_type_invalid",
        Deserialize_discover_type_invalid
    },
    {
        "opaque_struct",
        Deserialize_opaque_struct
    },
    {
        "opaque_struct_w_member",
        Deserialize_opaque_struct_w_member
    },
    {
        "opaque_struct_w_member_reverse",
        Deserialize_opaque_struct_w_member_reverse
    },
    {
        "struct_w_opaque_member",
        Deserialize_struct_w_opaque_member
    },
    {
        "opaque_string",
        Deserialize_opaque_string
    },
    {
        "opaque_entity",
        Deserialize_opaque_entity
    },
    {
        "struct_w_opaque_entity_member",
        Deserialize_struct_w_opaque_entity_member
    },
    {
        "opaque_vector_i32_0",
        Deserialize_opaque_vector_i32_0
    },
    {
        "opaque_vector_i32_2",
        Deserialize_opaque_vector_i32_2
    },
    {
        "opaque_vector_i32_0_into_2",
        Deserialize_opaque_vector_i32_0_into_2
    },
    {
        "opaque_vector_i32_1_into_2",
        Deserialize_opaque_vector_i32_1_into_2
    },
    {
        "opaque_vector_string_0",
        Deserialize_opaque_vector_string_0
    },
    {
        "opaque_vector_string_2",
        Deserialize_opaque_vector_string_2
    },
    {
        "opaque_vector_string_0_into_2",
        Deserialize_opaque_vector_string_0_into_2
    },
    {
        "opaque_vector_string_1_into_2",
        Deserialize_opaque_vector_string_1_into_2
    },
    {
        "opaque_vector_opaque_string_0",
        Deserialize_opaque_vector_opaque_string_0
    },
    {
        "opaque_vector_opaque_string_2",
        Deserialize_opaque_vector_opaque_string_2
    },
    {
        "opaque_vector_opaque_string_0_into_2",
        Deserialize_opaque_vector_opaque_string_0_into_2
    },
    {
        "opaque_vector_opaque_string_1_into_2",
        Deserialize_opaque_vector_opaque_string_1_into_2
    },
    {
        "opaque_vector_struct_0",
        Deserialize_opaque_vector_struct_0
    },
    {
        "opaque_vector_struct_2",
        Deserialize_opaque_vector_struct_2
    },
    {
        "opaque_vector_struct_0_into_2",
        Deserialize_opaque_vector_struct_0_into_2
    },
    {
        "opaque_vector_struct_1_into_2",
        Deserialize_opaque_vector_struct_1_into_2
    }
};

bake_test_case Fuzzing_testcases[] = {
    {
        "1",
        Fuzzing_1
    },
    {
        "2",
        Fuzzing_2
    },
    {
        "3",
        Fuzzing_3
    },
    {
        "4",
        Fuzzing_4
    },
    {
        "5",
        Fuzzing_5
    },
    {
        "6",
        Fuzzing_6
    },
    {
        "7",
        Fuzzing_7
    },
    {
        "8",
        Fuzzing_8
    },
    {
        "9",
        Fuzzing_9
    },
    {
        "10",
        Fuzzing_10
    },
    {
        "11",
        Fuzzing_11
    },
    {
        "12",
        Fuzzing_12
    },
    {
        "13",
        Fuzzing_13
    },
    {
        "14",
        Fuzzing_14
    }
};

const char* Expr_folding_param[] = {"enabled", "disabled"};
bake_test_param Expr_params[] = {
    {"folding", (char**)Expr_folding_param, 2}
};
const char* Deserialize_folding_param[] = {"enabled", "disabled"};
bake_test_param Deserialize_params[] = {
    {"folding", (char**)Deserialize_folding_param, 2}
};

static bake_test_suite suites[] = {
    {
        "Eval",
        NULL,
        NULL,
        407,
        Eval_testcases
    },
    {
        "Template",
        NULL,
        NULL,
        81,
        Template_testcases
    },
    {
        "Error",
        NULL,
        NULL,
        92,
        Error_testcases
    },
    {
        "Expr",
        Expr_setup,
        NULL,
        328,
        Expr_testcases,
        1,
        Expr_params
    },
    {
        "ExprAst",
        NULL,
        NULL,
        9,
        ExprAst_testcases
    },
    {
        "Vars",
        NULL,
        NULL,
        14,
        Vars_testcases
    },
    {
        "Serialize",
        NULL,
        NULL,
        71,
        Serialize_testcases
    },
    {
        "Deserialize",
        Deserialize_setup,
        NULL,
        127,
        Deserialize_testcases,
        1,
        Deserialize_params
    },
    {
        "Fuzzing",
        NULL,
        NULL,
        14,
        Fuzzing_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("script", argc, argv, suites, 9);
}
