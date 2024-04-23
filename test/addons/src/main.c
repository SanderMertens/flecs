
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <addons.h>

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
void Parser_component_explicit_subject_this_by_var_name(void);
void Parser_component_explicit_subject_wildcard(void);
void Parser_component_explicit_subject_any(void);
void Parser_component_explicit_subject_0(void);
void Parser_this_as_predicate(void);
void Parser_this_var_as_predicate(void);
void Parser_this_lowercase_var_as_predicate(void);
void Parser_this_as_object(void);
void Parser_this_var_as_object(void);
void Parser_pair_implicit_subject(void);
void Parser_pair_implicit_subject_wildcard_pred(void);
void Parser_pair_implicit_subject_wildcard_obj(void);
void Parser_pair_implicit_subject_any_pred(void);
void Parser_pair_implicit_subject_any_obj(void);
void Parser_pair_implicit_subject_this_pred(void);
void Parser_pair_implicit_subject_this_obj(void);
void Parser_pair_implicit_subject_pred_w_self(void);
void Parser_pair_implicit_subject_obj_w_self(void);
void Parser_pair_implicit_subject_pred_w_up(void);
void Parser_pair_implicit_subject_obj_w_up(void);
void Parser_pair_implicit_subject_pred_w_self_up(void);
void Parser_pair_implicit_subject_obj_w_self_up(void);
void Parser_pair_implicit_subject_pred_w_up_trav(void);
void Parser_pair_implicit_subject_obj_w_up_trav(void);
void Parser_pair_implicit_subject_pred_w_invalid_flags(void);
void Parser_pair_implicit_subject_obj_w_invalid_flags(void);
void Parser_pair_explicit_subject(void);
void Parser_pair_explicit_subject_this(void);
void Parser_pair_explicit_subject_this_by_name(void);
void Parser_pair_explicit_subject_this_by_var_name(void);
void Parser_pair_explicit_subject_wildcard_pred(void);
void Parser_pair_explicit_subject_wildcard_subj(void);
void Parser_pair_explicit_subject_wildcard_obj(void);
void Parser_pair_implicit_subject_0_object(void);
void Parser_pair_explicit_subject_0_object(void);
void Parser_pair_explicit_subject_0(void);
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
void Parser_pred_var(void);
void Parser_obj_var(void);
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
void Parser_not_alive_pred(void);
void Parser_not_alive_subj(void);
void Parser_not_alive_obj(void);
void Parser_this_subj_var_kind(void);
void Parser_this_obj_var_kind(void);
void Parser_this_subj_obj_var_kind(void);
void Parser_var_w_name(void);
void Parser_entity_pred_no_name(void);
void Parser_entity_subj_no_name(void);
void Parser_entity_obj_no_name(void);
void Parser_this_pred_no_name(void);
void Parser_this_subj_no_name(void);
void Parser_this_obj_no_name(void);
void Parser_auto_object_variable(void);
void Parser_auto_object_variable_w_subj(void);
void Parser_auto_scoped_variable(void);
void Parser_invalid_variable_only(void);
void Parser_oneof_self_pred_w_relative_obj(void);
void Parser_oneof_other_pred_w_relative_obj(void);
void Parser_oneof_self_pred_w_invalid_obj(void);
void Parser_oneof_other_pred_w_invalid_obj(void);
void Parser_oneof_w_other_entity_w_same_name(void);
void Parser_oneof_w_other_entity_w_same_name_w_set_scope(void);
void Parser_oneof_w_wildcard(void);
void Parser_oneof_w_any(void);
void Parser_oneof_w_fullpath(void);
void Parser_pair_implicit_src_missing_rel(void);
void Parser_pair_implicit_src_missing_obj(void);
void Parser_pair_explicit_src_missing_src(void);
void Parser_pair_explicit_src_missing_obj(void);
void Parser_eq_id(void);
void Parser_eq_id_var(void);
void Parser_eq_var_id(void);
void Parser_eq_var(void);
void Parser_neq_id(void);
void Parser_neq_id_var(void);
void Parser_neq_var_id(void);
void Parser_neq_var(void);
void Parser_eq_name(void);
void Parser_eq_name_var(void);
void Parser_eq_var_name(void);
void Parser_eq_var(void);
void Parser_neq_name(void);
void Parser_neq_name_var(void);
void Parser_neq_var_name(void);
void Parser_neq_var(void);
void Parser_match_name(void);
void Parser_match_name_var(void);
void Parser_match_var_name(void);
void Parser_match_var(void);
void Parser_nmatch_name(void);
void Parser_nmatch_name_var(void);
void Parser_nmatch_var_name(void);
void Parser_eq_same_var(void);
void Parser_neq_same_var(void);
void Parser_eq_same_var_this(void);
void Parser_neq_same_var_this(void);
void Parser_eq_w_optional(void);
void Parser_neq_w_optional(void);
void Parser_match_w_optional(void);
void Parser_query_scope_1_term(void);
void Parser_query_scope_1_term_spaces(void);
void Parser_query_scope_2_terms(void);
void Parser_query_nested_scope(void);
void Parser_query_nested_scope_spaces(void);
void Parser_query_scope_unbalanced(void);
void Parser_query_not_scope(void);
void Parser_query_empty_scope(void);
void Parser_query_scope_newline_after_open(void);
void Parser_query_scope_newline_after_close(void);
void Parser_override_tag(void);
void Parser_override_pair(void);
void Parser_pair_3_args(void);
void Parser_pair_3_args_implicit_this(void);
void Parser_pair_4_args(void);
void Parser_pair_4_args_implicit_this(void);
void Parser_pair_3_args_2_terms(void);
void Parser_pair_3_args_this_tgt(void);
void Parser_pair_3_args_2_terms_this_tgt(void);
void Parser_pair_3_args_2_terms_this_tgt_implicit_this(void);
void Parser_pair_3_or_args(void);
void Parser_pair_3_or_args_implicit_this(void);
void Parser_pair_4_or_args(void);
void Parser_pair_4_or_args_implicit_this(void);
void Parser_pair_or_before_and_oper(void);
void Parser_pair_and_before_or_oper(void);
void Parser_cascade_desc(void);
void Parser_newline_after_inout(void);
void Parser_newline_after_term_open(void);
void Parser_newline_after_term_src(void);
void Parser_newline_after_term_src_pair(void);
void Parser_newline_after_term_pair_comma(void);
void Parser_newline_after_term_pair_second(void);
void Parser_tag_w_space_implicit_this(void);
void Parser_tag_w_space(void);
void Parser_pair_first_w_space_implicit_this(void);
void Parser_pair_first_w_space(void);
void Parser_pair_second_w_space_implicit_this(void);
void Parser_pair_second_w_space(void);
void Parser_pair_src_w_space(void);

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
void Plecs_dot_assign_nested_member(void);
void Plecs_dot_assign_binary_expr(void);
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
void Plecs_default_type_from_with(void);
void Plecs_scope_w_1_subj_and_2_pairs(void);
void Plecs_inherit_from_multiple(void);
void Plecs_assign_pair_component(void);
void Plecs_assign_pair_component_in_scope(void);
void Plecs_assign_pair_component_in_script(void);
void Plecs_assign_pair_component_in_script_update(void);
void Plecs_set_entity_names(void);
void Plecs_oneof(void);
void Plecs_invalid_oneof(void);
void Plecs_brief_annotation(void);
void Plecs_name_annotation(void);
void Plecs_link_annotation(void);
void Plecs_color_annotation(void);
void Plecs_multiple_annotations(void);
void Plecs_annotation_w_trailing_space(void);
void Plecs_multiline_string(void);
void Plecs_unterminated_multiline_string(void);
void Plecs_declaration_w_underscore_name(void);
void Plecs_annotate_declaration(void);
void Plecs_anonymous_entity(void);
void Plecs_anonymous_entity_in_scope(void);
void Plecs_anonymous_declaration(void);
void Plecs_const_var_int(void);
void Plecs_const_var_float(void);
void Plecs_const_var_bool(void);
void Plecs_const_var_string(void);
void Plecs_const_var_struct(void);
void Plecs_const_var_redeclare(void);
void Plecs_const_var_scoped(void);
void Plecs_assign_component_from_var(void);
void Plecs_assign_component_from_var_in_scope(void);
void Plecs_scope_w_component_after_const_var(void);
void Plecs_component_after_const_paren_expr(void);
void Plecs_component_after_const_add_expr(void);
void Plecs_component_after_const_sub_expr(void);
void Plecs_component_after_const_mul_expr(void);
void Plecs_component_after_const_div_expr(void);
void Plecs_parse_with(void);
void Plecs_parse_with_w_with(void);
void Plecs_parse_with_w_tag(void);
void Plecs_parse_with_value(void);
void Plecs_parse_with_2_values(void);
void Plecs_parse_with_2_nested_values(void);
void Plecs_parse_with_var(void);
void Plecs_parse_with_2_vars(void);
void Plecs_parse_with_2_nested_vars(void);
void Plecs_parse_with_var_in_scope(void);
void Plecs_assign_const_w_expr(void);
void Plecs_const_w_type(void);
void Plecs_assembly_no_scope(void);
void Plecs_assembly_empty(void);
void Plecs_assembly_no_props(void);
void Plecs_assembly_prop_no_type(void);
void Plecs_assembly_prop_no_default(void);
void Plecs_assembly_prop(void);
void Plecs_assembly_prop_space_colon(void);
void Plecs_assembly_2_props(void);
void Plecs_assembly_instance_w_default_values(void);
void Plecs_assembly_instance_w_assign_default_values(void);
void Plecs_assembly_instance_w_overridden_values(void);
void Plecs_assembly_w_child(void);
void Plecs_assembly_w_child_parse_script(void);
void Plecs_assembly_w_child_parse_script_twice(void);
void Plecs_assembly_w_child_update_after_parse(void);
void Plecs_assembly_w_nested_child(void);
void Plecs_assembly_w_prefab(void);
void Plecs_assembly_w_prefab_tree(void);
void Plecs_assembly_w_nested_assembly(void);
void Plecs_instantiate_prefab_w_assembly(void);
void Plecs_assembly_w_prefab_w_assembly(void);
void Plecs_3_assemblies(void);
void Plecs_assembly_nested_w_default_var(void);
void Plecs_assembly_w_anonymous(void);
void Plecs_assembly_w_anonymous_parse_again(void);
void Plecs_typed_const_w_composite_type_invalid_assignment(void);
void Plecs_typed_const_w_composite_type(void);
void Plecs_assign_var_to_typed_const_w_composite_type(void);
void Plecs_typed_const_w_composite_type_invalid_assignment(void);
void Plecs_assembly_w_composite_prop_invalid_assignment(void);
void Plecs_assembly_w_composite_prop(void);
void Plecs_assembly_with_with(void);
void Plecs_using_wildcard(void);
void Plecs_single_line_comment_in_value(void);
void Plecs_single_line_comment_in_value_after_scope(void);
void Plecs_multi_line_comment_in_value(void);
void Plecs_multi_line_comment_in_value_after_scope(void);
void Plecs_unterminated_multi_line_comment_in_value(void);
void Plecs_module_stmt(void);
void Plecs_nested_module_stmt(void);
void Plecs_module_stmt_w_scope(void);
void Plecs_module_stmt_w_nested_scope(void);
void Plecs_module_w_assembly(void);
void Plecs_module_w_nested_assembly(void);
void Plecs_assign_singleton_tag(void);
void Plecs_assign_singleton_component(void);
void Plecs_assign_singleton_tag_w_scope(void);
void Plecs_assign_singleton_2_tags_w_scope(void);
void Plecs_assign_singleton_component_w_scope(void);
void Plecs_assign_singleton_2_components_w_scope(void);
void Plecs_with_pair_in_scope(void);
void Plecs_assembly_redeclare_prop_as_const(void);
void Plecs_assembly_redeclare_prop_as_prop(void);
void Plecs_assembly_redeclare_const_as_const(void);
void Plecs_add_auto_override(void);
void Plecs_add_auto_override_pair(void);
void Plecs_scope_w_auto_override(void);
void Plecs_scope_w_auto_override_pair(void);
void Plecs_pair_w_rel_var(void);
void Plecs_pair_w_tgt_var(void);
void Plecs_assembly_w_pair_w_this_var(void);
void Plecs_with_value_not_a_component(void);
void Plecs_component_in_with_scope(void);
void Plecs_component_in_with_scope_nested(void);
void Plecs_component_in_with_scope_in_scope(void);
void Plecs_assign_after_with_in_scope(void);
void Plecs_array_component(void);

// Testsuite 'Doc'
void Doc_get_set_name(void);
void Doc_get_entity_name(void);
void Doc_get_set_brief(void);
void Doc_get_set_detail(void);
void Doc_get_set_link(void);
void Doc_set_name_nullptr(void);
void Doc_set_brief_nullptr(void);
void Doc_set_detail_nullptr(void);
void Doc_set_link_nullptr(void);
void Doc_set_color_nullptr(void);

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
void Pipeline_mixed_multithreaded_tasks(void);
void Pipeline_mixed_staging(void);
void Pipeline_no_staging_system_create_query(void);
void Pipeline_single_threaded_pipeline_change(void);
void Pipeline_multi_threaded_pipeline_change(void);
void Pipeline_multi_threaded_pipeline_change_tasks(void);
void Pipeline_activate_after_add(void);
void Pipeline_match_all_after_pipeline_rebuild(void);
void Pipeline_empty_pipeline(void);
void Pipeline_custom_pipeline_w_system_macro(void);
void Pipeline_pipeline_w_short_notation(void);
void Pipeline_stack_allocator_after_progress(void);
void Pipeline_stack_allocator_after_progress_w_pipeline_change(void);
void Pipeline_iter_from_world_in_singlethread_system_multitead_app(void);
void Pipeline_iter_from_world_in_singlethread_system_multitead_app_tasks(void);
void Pipeline_no_staging_after_inactive_system(void);
void Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter(void);
void Pipeline_inactive_system_after_2_no_staging_system_no_defer_w_filter(void);
void Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter_w_no_staging_at_end(void);
void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer(void);
void Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer(void);
void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end(void);
void Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer_w_no_staging_at_end(void);
void Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded(void);
void Pipeline_multi_threaded_tasks_pipeline_change_w_only_singlethreaded(void);
void Pipeline_sync_after_not_out_for_out(void);
void Pipeline_pair_wildcard_read_after_staged_write(void);
void Pipeline_pair_read_after_staged_wildcard_write(void);
void Pipeline_no_sync_after_pair_wildcard_read_after_unmatching_staged_write(void);
void Pipeline_no_merge_after_from_nothing_w_default_inout(void);
void Pipeline_on_merge_activate_system_before_merge(void);
void Pipeline_disable_phase(void);
void Pipeline_disable_parent(void);
void Pipeline_multi_threaded_no_staging_w_add_after_read(void);
void Pipeline_multi_threaded_tasks_no_staging_w_add_after_read(void);
void Pipeline_1_startup_system(void);
void Pipeline_2_startup_systems(void);
void Pipeline_2_startup_phases(void);
void Pipeline_2_startup_systems_w_merge(void);
void Pipeline_inactive_last_system_merge_count(void);
void Pipeline_inactive_middle_system_merge_count(void);
void Pipeline_last_no_readonly_system_merge_count(void);
void Pipeline_2_pipelines_1_system(void);
void Pipeline_builtin_pipeline_w_self_system_term(void);
void Pipeline_custom_pipeline_w_self_system_term(void);
void Pipeline_switch_from_threads_to_tasks(void);
void Pipeline_switch_from_tasks_to_threads(void);
void Pipeline_run_pipeline_multithreaded(void);
void Pipeline_run_pipeline_multithreaded_tasks(void);
void Pipeline_pipeline_init_no_terms(void);
void Pipeline_pipeline_init_no_system_term(void);

// Testsuite 'SystemMisc'
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
void SystemMisc_invalid_0_w_from_entity(void);
void SystemMisc_invalid_component_id(void);
void SystemMisc_invalid_entity_id(void);
void SystemMisc_invalid_null_string(void);
void SystemMisc_invalid_empty_string(void);
void SystemMisc_invalid_empty_string_w_space(void);
void SystemMisc_redefine_row_system(void);
void SystemMisc_system_w_or_prefab(void);
void SystemMisc_system_w_or_disabled(void);
void SystemMisc_system_w_or_disabled_and_prefab(void);
void SystemMisc_table_columns_access(void);
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
void SystemMisc_delete_system(void);
void SystemMisc_delete_pipeline_system(void);
void SystemMisc_delete_system_w_ctx(void);
void SystemMisc_update_ctx(void);
void SystemMisc_run_custom_run_action(void);
void SystemMisc_run_w_offset_limit_custom_run_action(void);
void SystemMisc_pipeline_custom_run_action(void);
void SystemMisc_change_custom_run_action(void);
void SystemMisc_custom_run_action_call_next(void);
void SystemMisc_system_w_short_notation(void);
void SystemMisc_update_interval_w_system_init(void);
void SystemMisc_update_rate_w_system_init(void);
void SystemMisc_system_w_interval_rate_stop_timer(void);
void SystemMisc_system_same_interval_same_tick(void);

// Testsuite 'RulesBasic'
void RulesBasic_1_fact_w_tag(void);
void RulesBasic_1_fact_w_component(void);
void RulesBasic_1_fact_w_tag_pair(void);
void RulesBasic_1_fact_w_component_pair(void);
void RulesBasic_2_facts_same_src_w_tag(void);
void RulesBasic_2_facts_same_src_w_component(void);
void RulesBasic_2_facts_same_src_w_tag_pair(void);
void RulesBasic_2_facts_same_src_w_component_pair(void);
void RulesBasic_2_facts_other_src_w_tag(void);
void RulesBasic_2_facts_other_src_w_component(void);
void RulesBasic_2_facts_other_src_w_tag_pair(void);
void RulesBasic_2_facts_other_src_w_component_pair(void);
void RulesBasic_1_fact_w_any(void);
void RulesBasic_1_fact_w_pair_any_tgt(void);
void RulesBasic_1_fact_w_pair_any_rel(void);
void RulesBasic_1_fact_w_pair_any_rel_tgt(void);
void RulesBasic_2_facts_same_src_w_any(void);
void RulesBasic_2_facts_same_src_w_pair_any_tgt(void);
void RulesBasic_2_facts_same_src_w_pair_any_rel(void);
void RulesBasic_2_facts_same_src_w_pair_any_rel_tgt(void);
void RulesBasic_2_facts_other_src_w_any(void);
void RulesBasic_2_facts_other_src_w_pair_any_tgt(void);
void RulesBasic_2_facts_other_src_w_pair_any_rel(void);
void RulesBasic_2_facts_other_src_w_pair_any_rel_tgt(void);
void RulesBasic_1_this_src_w_tag(void);
void RulesBasic_1_this_src_w_component(void);
void RulesBasic_1_this_src_w_tag_pair(void);
void RulesBasic_1_this_src_w_component_pair(void);
void RulesBasic_1_this_src_w_tag_2_tables(void);
void RulesBasic_1_this_src_w_component_2_tables(void);
void RulesBasic_1_this_src_w_tag_pair_2_tables(void);
void RulesBasic_1_this_src_w_component_pair_2_tables(void);
void RulesBasic_2_this_src_w_tag(void);
void RulesBasic_2_this_src_w_component(void);
void RulesBasic_2_this_src_ent_src_w_tag(void);
void RulesBasic_2_this_src_ent_src_w_component(void);
void RulesBasic_2_ent_src_this_src_w_tag(void);
void RulesBasic_2_ent_src_this_src_w_component(void);
void RulesBasic_recycled_tag(void);
void RulesBasic_recycled_src(void);
void RulesBasic_recycled_pair_rel(void);
void RulesBasic_recycled_pair_tgt(void);
void RulesBasic_this_src_w_wildcard(void);
void RulesBasic_this_src_w_pair_rel_wildcard(void);
void RulesBasic_this_src_w_pair_tgt_wildcard(void);
void RulesBasic_this_src_w_pair_rel_tgt_wildcard(void);
void RulesBasic_this_src_w_any(void);
void RulesBasic_this_src_w_any_written(void);
void RulesBasic_this_src_w_pair_rel_any(void);
void RulesBasic_this_src_w_pair_tgt_any(void);
void RulesBasic_this_src_w_pair_rel_tgt_any(void);
void RulesBasic_ent_src_w_wildcard(void);
void RulesBasic_ent_src_w_pair_rel_wildcard(void);
void RulesBasic_ent_src_w_pair_tgt_wildcard(void);
void RulesBasic_ent_src_w_pair_rel_tgt_wildcard(void);
void RulesBasic_1_wildcard_src(void);
void RulesBasic_1_wildcard_src_w_pair(void);
void RulesBasic_2_wildcard_src(void);
void RulesBasic_2_wildcard_src_w_pair(void);
void RulesBasic_1_wildcard_src_w_pair_tgt_var(void);
void RulesBasic_1_wildcard_src_w_pair_rel_var(void);
void RulesBasic_1_wildcard_src_w_pair_tgt_this(void);
void RulesBasic_1_wildcard_src_w_pair_rel_this(void);
void RulesBasic_1_any_src(void);
void RulesBasic_1_any_src_component(void);
void RulesBasic_1_any_src_component_w_this_component(void);
void RulesBasic_1_any_src_w_pair(void);
void RulesBasic_2_any_src(void);
void RulesBasic_2_any_src_w_pair(void);
void RulesBasic_1_any_src_w_pair_tgt_var(void);
void RulesBasic_1_any_src_w_pair_rel_var(void);
void RulesBasic_1_any_src_w_pair_tgt_this(void);
void RulesBasic_1_any_src_w_pair_rel_this(void);
void RulesBasic_1_any_src_any_tgt_w_rel_fixed(void);
void RulesBasic_1_any_src_any_rel_w_tgt_fixed(void);
void RulesBasic_1_any_src_w_childof_pair_any_tgt(void);
void RulesBasic_1_any_src_empty(void);
void RulesBasic_1_any_src_pair_tgt_wildcard_empty(void);
void RulesBasic_1_any_src_pair_rel_wildcard_empty(void);
void RulesBasic_1_any_src_any_tgt_w_rel_fixed_empty(void);
void RulesBasic_1_any_src_any_rel_w_tgt_fixed_empty(void);
void RulesBasic_1_any_src_pair_w_tag_query_empty(void);
void RulesBasic_1_any_src_tag_w_pair_query_empty(void);
void RulesBasic_not_any(void);
void RulesBasic_rule_w_iter_next(void);
void RulesBasic_empty_rule(void);
void RulesBasic_invalid_rule(void);
void RulesBasic_not_instanced_table_src(void);
void RulesBasic_not_instanced_entity_src(void);
void RulesBasic_not_instanced_mixed_src(void);
void RulesBasic_instanced_table_src(void);
void RulesBasic_instanced_entity_src(void);
void RulesBasic_instanced_mixed_src(void);
void RulesBasic_in_term(void);
void RulesBasic_out_term(void);
void RulesBasic_inout_term(void);
void RulesBasic_nodata_term(void);
void RulesBasic_find_this_lowercase(void);
void RulesBasic_find_this_uppercase(void);
void RulesBasic_find_this_tgt_lowercase(void);
void RulesBasic_find_this_tgt_uppercase(void);
void RulesBasic_get_filter(void);
void RulesBasic_iter_empty_source(void);
void RulesBasic_iter_empty_source_2_terms(void);
void RulesBasic_iter_empty_source_wildcard(void);
void RulesBasic_iter_empty_source_pair(void);
void RulesBasic_iter_empty_source_pair_wildcard(void);
void RulesBasic_iter_empty_source_2_terms_pair(void);
void RulesBasic_iter_empty_source_2_terms_mixed(void);
void RulesBasic_iter_empty_source_2_terms_mixed_pair(void);
void RulesBasic_iter_empty_source_2_terms_mixed_pair_wildcard(void);
void RulesBasic_this_var_w_empty_entity(void);
void RulesBasic_match_disabled(void);
void RulesBasic_match_prefab(void);
void RulesBasic_match_disabled_prefab(void);
void RulesBasic_match_disabled_this_tgt(void);
void RulesBasic_match_prefab_this_tgt(void);
void RulesBasic_match_disabled_prefab_this_tgt(void);
void RulesBasic_match_self_disabled(void);
void RulesBasic_match_self_prefab(void);
void RulesBasic_match_self_disabled_prefab(void);
void RulesBasic_inout_none_first_term(void);
void RulesBasic_inout_none_second_term(void);
void RulesBasic_no_data_rule(void);
void RulesBasic_frame_offset(void);
void RulesBasic_frame_offset_no_data(void);
void RulesBasic_match_empty_tables(void);
void RulesBasic_match_empty_tables_no_data(void);
void RulesBasic_match_empty_tables_w_not(void);
void RulesBasic_match_empty_tables_w_wildcard(void);
void RulesBasic_match_empty_tables_w_no_empty_tables(void);
void RulesBasic_match_empty_tables_trivial(void);
void RulesBasic_oneof_wildcard(void);
void RulesBasic_oneof_any(void);
void RulesBasic_instanced_w_singleton(void);
void RulesBasic_instanced_w_base(void);
void RulesBasic_not_instanced_w_singleton(void);
void RulesBasic_not_instanced_w_base(void);
void RulesBasic_unknown_before_known(void);
void RulesBasic_unknown_before_known_after_or(void);
void RulesBasic_unknown_before_known_after_not(void);
void RulesBasic_unknown_before_known_after_optional(void);
void RulesBasic_unknown_before_known_after_scope(void);
void RulesBasic_reordered_plan_1(void);
void RulesBasic_reordered_plan_2(void);
void RulesBasic_1_trivial_plan(void);
void RulesBasic_2_trivial_plan(void);
void RulesBasic_1_trivial_plan_component(void);
void RulesBasic_2_trivial_plan_component(void);
void RulesBasic_2_trivial_plan_w_wildcard(void);
void RulesBasic_3_trivial_plan_w_pair(void);
void RulesBasic_3_trivial_plan_w_wildcard(void);
void RulesBasic_3_trivial_plan_w_any(void);
void RulesBasic_3_trivial_plan_w_pair_component(void);
void RulesBasic_3_trivial_plan_w_wildcard_component(void);
void RulesBasic_3_trivial_plan_w_any_component(void);
void RulesBasic_1_trivial_component_w_none(void);
void RulesBasic_2_trivial_component_w_none(void);
void RulesBasic_2_trivial_mixed_2_tables(void);
void RulesBasic_2_trivial_mixed_2_tables_component(void);
void RulesBasic_2_trivial_mixed_2_tables_wildcard(void);
void RulesBasic_1_plan_any_src(void);
void RulesBasic_1_plan_not_any_src(void);
void RulesBasic_1_plan_optional_any_src(void);

// Testsuite 'RulesVariables'
void RulesVariables_1_ent_src_w_var(void);
void RulesVariables_1_ent_src_w_pair_rel_var(void);
void RulesVariables_1_ent_src_w_pair_tgt_var(void);
void RulesVariables_1_ent_src_w_pair_rel_tgt_var(void);
void RulesVariables_1_ent_src_w_pair_rel_tgt_same_var(void);
void RulesVariables_1_ent_src_w_pair_rel_tgt_same_var_after_write(void);
void RulesVariables_1_this_src_w_var(void);
void RulesVariables_1_this_src_w_pair_rel_var(void);
void RulesVariables_1_this_src_w_pair_tgt_var(void);
void RulesVariables_1_this_src_w_pair_rel_tgt_var(void);
void RulesVariables_1_this_src_w_pair_rel_tgt_same_var(void);
void RulesVariables_1_this_src_w_pair_rel_tgt_same_var_after_write(void);
void RulesVariables_1_src_id_same_var(void);
void RulesVariables_1_src_pair_first_same_var(void);
void RulesVariables_1_src_pair_second_same_var(void);
void RulesVariables_1_src_pair_first_and_second_same_var(void);
void RulesVariables_1_src_id_same_var_after_write(void);
void RulesVariables_1_src_pair_first_same_var_after_write(void);
void RulesVariables_1_src_pair_second_same_var_after_write(void);
void RulesVariables_1_src_pair_first_and_second_same_var_after_write(void);
void RulesVariables_1_src_pair_first_same_var_this(void);
void RulesVariables_1_src_pair_second_same_var_this(void);
void RulesVariables_1_src_pair_first_and_second_same_var_this(void);
void RulesVariables_1_src_id_same_var_this_after_write(void);
void RulesVariables_1_src_pair_first_same_var_this_after_write(void);
void RulesVariables_1_src_pair_second_same_var_this_after_write(void);
void RulesVariables_1_src_pair_first_and_second_same_var_this_after_write(void);
void RulesVariables_1_ent_src_w_this_var(void);
void RulesVariables_1_ent_src_w_pair_this_rel(void);
void RulesVariables_1_ent_src_w_pair_this_tgt(void);
void RulesVariables_1_ent_src_w_pair_this_rel_tgt(void);
void RulesVariables_1_this_src_w_this(void);
void RulesVariables_1_this_src_w_pair_this_rel_tgt(void);
void RulesVariables_1_this_src_w_this_after_write(void);
void RulesVariables_1_this_src_w_pair_this_rel_tgt_after_write(void);
void RulesVariables_2_constrain_src_from_src(void);
void RulesVariables_2_constrain_rel_from_src_w_ent(void);
void RulesVariables_2_constrain_rel_from_src_w_var(void);
void RulesVariables_2_constrain_rel_from_src_w_this(void);
void RulesVariables_2_constrain_pair_rel_from_src_w_ent(void);
void RulesVariables_2_constrain_pair_rel_from_src_w_var(void);
void RulesVariables_2_constrain_pair_rel_from_src_w_this(void);
void RulesVariables_2_constrain_pair_tgt_from_src_w_ent(void);
void RulesVariables_2_constrain_pair_tgt_from_src_w_var(void);
void RulesVariables_2_constrain_pair_tgt_from_src_w_this(void);
void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_ent(void);
void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_var(void);
void RulesVariables_2_constrain_pair_rel_tgt_from_src_w_this(void);
void RulesVariables_1_ent_src_set_rel_var(void);
void RulesVariables_1_ent_src_set_pair_rel_var(void);
void RulesVariables_1_ent_src_set_pair_tgt_var(void);
void RulesVariables_1_set_src_var(void);
void RulesVariables_1_set_src_var_w_pair(void);
void RulesVariables_1_set_src_var_w_pair_set_rel(void);
void RulesVariables_1_set_src_var_w_pair_set_tgt(void);
void RulesVariables_1_set_src_var_w_pair_set_rel_tgt(void);
void RulesVariables_1_set_src_this(void);
void RulesVariables_1_set_src_this_w_pair(void);
void RulesVariables_1_set_src_this_w_pair_set_rel(void);
void RulesVariables_1_set_src_this_w_pair_set_tgt(void);
void RulesVariables_1_set_src_this_w_pair_set_rel_tgt(void);
void RulesVariables_1_set_src_this_to_empty_table(void);
void RulesVariables_1_set_src_this_to_empty_table_w_component(void);
void RulesVariables_1_set_src_this_to_empty_table_w_component_self(void);
void RulesVariables_1_set_src_this_to_entiy_in_table(void);
void RulesVariables_1_set_src_this_to_entiy_in_table_self(void);
void RulesVariables_2_set_src_this(void);
void RulesVariables_2_set_src_this_self(void);
void RulesVariables_2_set_src_this_component(void);
void RulesVariables_2_set_src_this_self_component(void);
void RulesVariables_2_set_src_this_w_up(void);
void RulesVariables_2_set_src_this_self_w_up(void);
void RulesVariables_2_set_src_this_component_w_up(void);
void RulesVariables_2_set_src_this_self_component_w_up(void);
void RulesVariables_2_set_src_this_w_exclusive_wildcard(void);
void RulesVariables_2_set_src_this_self_w_exclusive_wildcard(void);
void RulesVariables_1_src_this_var_as_entity(void);
void RulesVariables_1_src_this_var_as_table(void);
void RulesVariables_1_src_this_var_as_table_range(void);
void RulesVariables_2_join_by_rel_var(void);
void RulesVariables_2_join_by_pair_rel_var(void);
void RulesVariables_2_join_by_pair_tgt_var(void);
void RulesVariables_2_cycle_w_var(void);
void RulesVariables_2_cycle_w_this_var(void);
void RulesVariables_2_cycle_w_var_this(void);
void RulesVariables_2_cycle_pair_w_var(void);
void RulesVariables_2_cycle_pair_w_this_var_var(void);
void RulesVariables_2_cycle_pair_w_var_this_var(void);
void RulesVariables_2_cycle_pair_w_var_var_this(void);
void RulesVariables_2_cycle_pair_ent_var_var(void);
void RulesVariables_2_cycle_pair_ent_this_var(void);
void RulesVariables_2_cycle_pair_ent_var_this(void);
void RulesVariables_parse_0_var(void);
void RulesVariables_parse_1_var(void);
void RulesVariables_parse_2_vars(void);
void RulesVariables_parse_0_var_paren(void);
void RulesVariables_parse_1_var_paren(void);
void RulesVariables_parse_2_vars_paren(void);
void RulesVariables_parse_1_vars_w_path(void);
void RulesVariables_parse_missing_close_paren(void);
void RulesVariables_parse_missing_open_paren(void);
void RulesVariables_parse_missing_value(void);
void RulesVariables_parse_0_var_w_spaces(void);
void RulesVariables_parse_1_var_w_spaces(void);
void RulesVariables_parse_2_vars_w_spaces(void);
void RulesVariables_parse_0_var_paren_w_spaces(void);
void RulesVariables_parse_1_var_paren_w_spaces(void);
void RulesVariables_parse_2_vars_paren_w_spaces(void);
void RulesVariables_var_count(void);
void RulesVariables_var_name(void);
void RulesVariables_var_is_entity(void);
void RulesVariables_no_this_anonymous_src(void);
void RulesVariables_no_this_anonymous_src_w_pair(void);
void RulesVariables_no_this_anonymous_component_src(void);
void RulesVariables_no_this_anonymous_component_src_w_pair(void);
void RulesVariables_lookup_from_table_this(void);
void RulesVariables_lookup_from_entity_this(void);
void RulesVariables_lookup_from_table(void);
void RulesVariables_lookup_from_entity(void);
void RulesVariables_lookup_from_not_written(void);
void RulesVariables_lookup_from_table_this_component(void);
void RulesVariables_lookup_from_entity_this_component(void);
void RulesVariables_lookup_from_table_component(void);
void RulesVariables_lookup_from_entity_component(void);
void RulesVariables_lookup_from_table_two_children(void);
void RulesVariables_lookup_from_entity_two_children(void);
void RulesVariables_lookup_from_table_same_child_twice(void);
void RulesVariables_lookup_from_entity_same_child_twice(void);
void RulesVariables_lookup_from_table_not(void);
void RulesVariables_lookup_from_entity_not(void);
void RulesVariables_lookup_from_table_w_any_component(void);
void RulesVariables_lookup_from_entity_w_any_component(void);
void RulesVariables_lookup_as_tag(void);
void RulesVariables_lookup_as_relationship(void);
void RulesVariables_lookup_as_target(void);
void RulesVariables_lookup_assign_var(void);
void RulesVariables_lookup_eq_var(void);
void RulesVariables_lookup_neq_var(void);
void RulesVariables_lookup_unresolved_dependent(void);
void RulesVariables_check_vars_this(void);
void RulesVariables_check_vars_var(void);
void RulesVariables_check_vars_wildcard(void);
void RulesVariables_check_vars_any(void);
void RulesVariables_check_vars_var_as_tgt(void);
void RulesVariables_check_vars_this_as_tgt(void);
void RulesVariables_check_vars_anonymous_var_as_tgt(void);
void RulesVariables_check_vars_wildcard_as_tgt(void);
void RulesVariables_check_vars_any_as_tgt(void);
void RulesVariables_check_vars_this_w_lookup_var(void);
void RulesVariables_check_vars_var_w_lookup_var(void);
void RulesVariables_1_trivial_1_var(void);
void RulesVariables_2_trivial_1_var(void);
void RulesVariables_1_trivial_1_var_component(void);
void RulesVariables_2_trivial_1_var_component(void);
void RulesVariables_1_trivial_1_wildcard(void);
void RulesVariables_2_trivial_1_wildcard(void);
void RulesVariables_1_trivial_1_wildcard_component(void);
void RulesVariables_2_trivial_1_wildcard_component(void);
void RulesVariables_1_trivial_1_any(void);
void RulesVariables_2_trivial_1_any(void);
void RulesVariables_1_trivial_1_any_component(void);
void RulesVariables_2_trivial_1_any_component(void);

// Testsuite 'RulesOperators'
void RulesOperators_2_and_not(void);
void RulesOperators_3_and_not_not(void);
void RulesOperators_2_and_not_pair_rel_wildcard(void);
void RulesOperators_2_and_not_pair_tgt_wildcard(void);
void RulesOperators_2_and_not_pair_rel_tgt_wildcard(void);
void RulesOperators_2_and_not_pair_rel_var(void);
void RulesOperators_2_and_not_pair_tgt_var(void);
void RulesOperators_2_and_not_pair_rel_tgt_var(void);
void RulesOperators_2_and_not_pair_rel_tgt_same_var(void);
void RulesOperators_2_and_not_pair_rel_var_written(void);
void RulesOperators_2_and_not_pair_tgt_var_written(void);
void RulesOperators_2_and_not_pair_rel_tgt_var_written(void);
void RulesOperators_2_and_not_pair_rel_tgt_same_var_written(void);
void RulesOperators_2_and_not_pair_rel_src_tgt_same_var_written(void);
void RulesOperators_2_and_not_pair_any_rel(void);
void RulesOperators_2_and_not_pair_any_tgt(void);
void RulesOperators_2_and_not_pair_any_src(void);
void RulesOperators_1_not_any_src_fixed_first(void);
void RulesOperators_1_not_any_src_any_tgt_fixed_first(void);
void RulesOperators_1_not_any_src_any_first_fixed_tgt(void);
void RulesOperators_1_not_any_src_any_childof_pair_any_tgt(void);
void RulesOperators_1_not_any_src_any_isa_pair_any_tgt(void);
void RulesOperators_2_and_optional(void);
void RulesOperators_3_and_optional_optional(void);
void RulesOperators_2_and_optional_pair_rel_wildcard(void);
void RulesOperators_2_and_optional_pair_tgt_wildcard(void);
void RulesOperators_2_and_optional_pair_rel_var(void);
void RulesOperators_2_and_optional_pair_tgt_var(void);
void RulesOperators_2_and_optional_pair_rel_tgt_var(void);
void RulesOperators_2_and_optional_pair_rel_tgt_same_var(void);
void RulesOperators_2_and_optional_pair_rel_var_written(void);
void RulesOperators_2_and_optional_pair_tgt_var_written(void);
void RulesOperators_2_and_optional_pair_rel_tgt_var_written(void);
void RulesOperators_2_and_optional_pair_rel_tgt_same_var_written(void);
void RulesOperators_2_and_optional_pair_rel_src_tgt_same_var_written(void);
void RulesOperators_3_and_optional_optional_pair_w_var(void);
void RulesOperators_2_and_optional_pair_any_rel(void);
void RulesOperators_2_and_optional_pair_any_tgt(void);
void RulesOperators_2_and_optional_pair_any_src(void);
void RulesOperators_3_and_optional_dependent_and_pair_rel(void);
void RulesOperators_3_and_optional_dependent_and_pair_tgt(void);
void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt(void);
void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_var(void);
void RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var(void);
void RulesOperators_3_and_optional_dependent_and_pair_src(void);
void RulesOperators_3_and_optional_dependent_optional_pair_rel(void);
void RulesOperators_3_and_optional_dependent_optional_pair_tgt(void);
void RulesOperators_3_and_optional_dependent_optional_pair_src(void);
void RulesOperators_3_and_optional_dependent_not_pair_rel(void);
void RulesOperators_3_and_optional_dependent_not_pair_tgt(void);
void RulesOperators_3_and_optional_dependent_not_pair_src(void);
void RulesOperators_1_optional_any_src_fixed_first(void);
void RulesOperators_1_optional_any_src_any_tgt_fixed_first(void);
void RulesOperators_1_optional_any_src_any_first_fixed_tgt(void);
void RulesOperators_1_optional_any_src_any_childof_pair_any_tgt(void);
void RulesOperators_1_optional_any_src_any_isa_pair_any_tgt(void);
void RulesOperators_2_or(void);
void RulesOperators_3_or(void);
void RulesOperators_2_or_written(void);
void RulesOperators_3_or_written(void);
void RulesOperators_2_or_written_w_rel_var(void);
void RulesOperators_3_or_written_w_rel_var(void);
void RulesOperators_2_or_written_w_tgt_var(void);
void RulesOperators_2_or_written_w_rel_tgt_var(void);
void RulesOperators_2_or_written_w_rel_tgt_same_var(void);
void RulesOperators_3_or_written_w_tgt_var(void);
void RulesOperators_2_or_chains(void);
void RulesOperators_2_or_chains_written(void);
void RulesOperators_2_or_dependent(void);
void RulesOperators_2_or_dependent_reverse(void);
void RulesOperators_2_or_dependent_2_vars(void);
void RulesOperators_2_or_written_dependent(void);
void RulesOperators_2_or_written_dependent_2_vars(void);
void RulesOperators_2_or_w_dependent(void);
void RulesOperators_2_or_w_both(void);
void RulesOperators_3_or_w_both(void);
void RulesOperators_2_not_first(void);
void RulesOperators_2_optional_first(void);
void RulesOperators_root_entities_empty(void);
void RulesOperators_root_entities(void);
void RulesOperators_root_entities_w_children(void);
void RulesOperators_root_entities_w_optional_children(void);
void RulesOperators_core_entities_w_optional_children(void);
void RulesOperators_root_entities_w_not_children(void);
void RulesOperators_core_entities_w_not_children(void);
void RulesOperators_1_ent_src_not(void);
void RulesOperators_1_ent_src_not_pair(void);
void RulesOperators_1_ent_src_not_pair_rel_wildcard(void);
void RulesOperators_1_ent_src_not_pair_tgt_wildcard(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_wildcard(void);
void RulesOperators_1_ent_src_not_pair_rel_any(void);
void RulesOperators_1_ent_src_not_pair_tgt_any(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_any(void);
void RulesOperators_1_ent_src_not_pair_rel_var(void);
void RulesOperators_1_ent_src_not_pair_tgt_var(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_var(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_same_var(void);
void RulesOperators_1_this_src_not_pair_rel_var(void);
void RulesOperators_1_this_src_not_pair_tgt_var(void);
void RulesOperators_1_this_src_not_pair_rel_tgt_var(void);
void RulesOperators_1_this_src_not_pair_rel_tgt_same_var(void);
void RulesOperators_1_ent_src_not_pair_rel_var_written(void);
void RulesOperators_1_ent_src_not_pair_tgt_var_written(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_var_written(void);
void RulesOperators_1_ent_src_not_pair_rel_tgt_same_var_written(void);

// Testsuite 'RulesTransitive'
void RulesTransitive_1_fact_0_lvl_true(void);
void RulesTransitive_1_fact_1_lvl_true(void);
void RulesTransitive_1_fact_2_lvl_true(void);
void RulesTransitive_1_fact_0_lvl_false(void);
void RulesTransitive_1_fact_1_lvl_false(void);
void RulesTransitive_1_fact_2_lvl_false(void);
void RulesTransitive_1_fact_reflexive(void);
void RulesTransitive_1_this_src_written_0_lvl(void);
void RulesTransitive_1_this_src_written_1_lvl(void);
void RulesTransitive_1_this_src_written_2_lvl(void);
void RulesTransitive_1_this_src_written_reflexive(void);
void RulesTransitive_1_this_src_0_lvl(void);
void RulesTransitive_1_this_src_1_lvl(void);
void RulesTransitive_1_this_src_2_lvl(void);
void RulesTransitive_1_this_src_reflexive(void);
void RulesTransitive_1_ent_src_tgt_var_0_lvl(void);
void RulesTransitive_1_ent_src_tgt_var_1_lvl(void);
void RulesTransitive_1_ent_src_tgt_var_2_lvl(void);
void RulesTransitive_1_ent_src_tgt_var_reflexive(void);
void RulesTransitive_1_this_src_tgt_var(void);
void RulesTransitive_1_this_src_tgt_var_reflexive(void);
void RulesTransitive_1_var_src_written_0_lvl(void);
void RulesTransitive_1_var_src_written_1_lvl(void);
void RulesTransitive_1_var_src_written_2_lvl(void);
void RulesTransitive_1_var_src_written_reflexive(void);
void RulesTransitive_1_var_src_0_lvl(void);
void RulesTransitive_1_var_src_1_lvl(void);
void RulesTransitive_1_var_src_2_lvl(void);
void RulesTransitive_1_var_src_reflexive(void);
void RulesTransitive_1_var_src_tgt_var(void);
void RulesTransitive_1_var_src_tgt_var_reflexive(void);
void RulesTransitive_1_ent_src_tgt_this_0_lvl(void);
void RulesTransitive_1_ent_src_tgt_this_1_lvl(void);
void RulesTransitive_1_ent_src_tgt_this_2_lvl(void);
void RulesTransitive_1_ent_src_tgt_this_reflexive(void);
void RulesTransitive_1_var_src_tgt_this(void);
void RulesTransitive_1_var_src_tgt_this_reflexive(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_before_0_lvl(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_before_1_lvl(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_before_2_lvl(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_after_0_lvl(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_after_1_lvl(void);
void RulesTransitive_2_ent_src_constrain_tgt_var_after_2_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_before_0_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_before_1_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_before_2_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_after_0_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_after_1_lvl(void);
void RulesTransitive_2_this_src_constrain_tgt_var_after_2_lvl(void);
void RulesTransitive_1_src_tgt_same_var(void);
void RulesTransitive_1_src_tgt_same_var_reflexive(void);
void RulesTransitive_1_src_tgt_same_this_var_reflexive(void);
void RulesTransitive_1_any_src_tgt_var(void);
void RulesTransitive_not_transitive_ent_tgt(void);
void RulesTransitive_not_transitive_var_tgt(void);
void RulesTransitive_not_transitive_ent_tgt_written(void);
void RulesTransitive_not_transitive_var_tgt_written(void);
void RulesTransitive_optional_transitive_ent_tgt(void);
void RulesTransitive_optional_transitive_var_tgt(void);
void RulesTransitive_optional_transitive_ent_tgt_written(void);
void RulesTransitive_optional_transitive_var_tgt_written(void);
void RulesTransitive_2_var_src_w_same_tgt_ent(void);
void RulesTransitive_self_target(void);
void RulesTransitive_any_target(void);

// Testsuite 'RulesComponentInheritance'
void RulesComponentInheritance_1_ent_0_lvl(void);
void RulesComponentInheritance_1_ent_1_lvl(void);
void RulesComponentInheritance_1_ent_2_lvl(void);
void RulesComponentInheritance_1_ent_3_lvl(void);
void RulesComponentInheritance_1_this_0_lvl(void);
void RulesComponentInheritance_1_this_1_lvl(void);
void RulesComponentInheritance_1_this_2_lvl(void);
void RulesComponentInheritance_1_this_3_lvl(void);
void RulesComponentInheritance_1_this_0_lvl_written(void);
void RulesComponentInheritance_1_this_1_lvl_written(void);
void RulesComponentInheritance_1_this_2_lvl_written(void);
void RulesComponentInheritance_1_this_3_lvl_written(void);
void RulesComponentInheritance_1_var_0_lvl(void);
void RulesComponentInheritance_1_var_1_lvl(void);
void RulesComponentInheritance_1_var_2_lvl(void);
void RulesComponentInheritance_1_var_3_lvl(void);
void RulesComponentInheritance_1_var_0_lvl_written(void);
void RulesComponentInheritance_1_var_1_lvl_written(void);
void RulesComponentInheritance_1_var_2_lvl_written(void);
void RulesComponentInheritance_1_var_3_lvl_written(void);
void RulesComponentInheritance_1_ent_1_lvl_self(void);
void RulesComponentInheritance_1_this_1_lvl_self(void);
void RulesComponentInheritance_1_this_1_lvl_written_self(void);
void RulesComponentInheritance_1_var_1_lvl_self(void);
void RulesComponentInheritance_1_var_1_lvl_written_self(void);
void RulesComponentInheritance_1_ent_src_not(void);
void RulesComponentInheritance_1_this_src_not(void);
void RulesComponentInheritance_1_var_src_not(void);
void RulesComponentInheritance_1_this_src_not_written(void);
void RulesComponentInheritance_1_var_src_not_written(void);
void RulesComponentInheritance_first_self(void);
void RulesComponentInheritance_first_down(void);
void RulesComponentInheritance_first_self_down(void);
void RulesComponentInheritance_first_rel_self(void);
void RulesComponentInheritance_first_rel_down(void);
void RulesComponentInheritance_first_rel_self_down(void);

// Testsuite 'RulesRecycled'
void RulesRecycled_recycled_vars(void);
void RulesRecycled_recycled_pair_vars(void);
void RulesRecycled_recycled_this_ent_var(void);
void RulesRecycled_has_recycled_id_from_pair(void);

// Testsuite 'RulesBuiltinPredicates'
void RulesBuiltinPredicates_this_eq_id(void);
void RulesBuiltinPredicates_this_eq_name(void);
void RulesBuiltinPredicates_this_eq_var(void);
void RulesBuiltinPredicates_this_eq_id_written(void);
void RulesBuiltinPredicates_this_eq_id_written_no_match(void);
void RulesBuiltinPredicates_this_eq_name_written(void);
void RulesBuiltinPredicates_this_eq_name_written_no_match(void);
void RulesBuiltinPredicates_this_eq_var_written(void);
void RulesBuiltinPredicates_var_eq_id(void);
void RulesBuiltinPredicates_var_eq_name(void);
void RulesBuiltinPredicates_var_eq_var(void);
void RulesBuiltinPredicates_var_eq_id_written(void);
void RulesBuiltinPredicates_var_eq_id_written_no_match(void);
void RulesBuiltinPredicates_var_eq_name_written(void);
void RulesBuiltinPredicates_var_eq_name_written_no_match(void);
void RulesBuiltinPredicates_var_eq_var_written(void);
void RulesBuiltinPredicates_var_eq_this(void);
void RulesBuiltinPredicates_this_neq_id(void);
void RulesBuiltinPredicates_this_neq_name(void);
void RulesBuiltinPredicates_this_neq_var(void);
void RulesBuiltinPredicates_this_neq_id_written(void);
void RulesBuiltinPredicates_this_neq_id_written_no_match(void);
void RulesBuiltinPredicates_this_neq_name_written(void);
void RulesBuiltinPredicates_this_neq_name_written_no_match(void);
void RulesBuiltinPredicates_this_neq_var_written(void);
void RulesBuiltinPredicates_var_neq_id(void);
void RulesBuiltinPredicates_var_neq_name(void);
void RulesBuiltinPredicates_var_neq_var(void);
void RulesBuiltinPredicates_var_neq_id_written(void);
void RulesBuiltinPredicates_var_neq_id_written_no_match(void);
void RulesBuiltinPredicates_var_neq_name_written(void);
void RulesBuiltinPredicates_var_neq_name_written_no_match(void);
void RulesBuiltinPredicates_var_neq_var_written(void);
void RulesBuiltinPredicates_var_neq_this(void);
void RulesBuiltinPredicates_this_2_neq_id(void);
void RulesBuiltinPredicates_this_2_neq_name(void);
void RulesBuiltinPredicates_var_2_neq_id(void);
void RulesBuiltinPredicates_var_2_neq_name(void);
void RulesBuiltinPredicates_this_2_neq_id_written(void);
void RulesBuiltinPredicates_this_2_neq_name_written(void);
void RulesBuiltinPredicates_var_2_neq_id_written(void);
void RulesBuiltinPredicates_var_2_neq_name_written(void);
void RulesBuiltinPredicates_this_2_or_id(void);
void RulesBuiltinPredicates_this_2_or_name(void);
void RulesBuiltinPredicates_var_2_or_id(void);
void RulesBuiltinPredicates_var_2_or_name(void);
void RulesBuiltinPredicates_this_2_or_id_written(void);
void RulesBuiltinPredicates_this_2_or_name_written(void);
void RulesBuiltinPredicates_var_2_or_id_written(void);
void RulesBuiltinPredicates_var_2_or_name_written(void);
void RulesBuiltinPredicates_this_match_eq(void);
void RulesBuiltinPredicates_var_match_eq(void);
void RulesBuiltinPredicates_this_match_eq_written(void);
void RulesBuiltinPredicates_this_match_eq_written_self(void);
void RulesBuiltinPredicates_var_match_eq_written(void);
void RulesBuiltinPredicates_this_match_neq(void);
void RulesBuiltinPredicates_var_match_neq(void);
void RulesBuiltinPredicates_this_match_neq_written(void);
void RulesBuiltinPredicates_var_match_neq_written(void);
void RulesBuiltinPredicates_this_match_2_neq(void);
void RulesBuiltinPredicates_var_match_2_neq(void);
void RulesBuiltinPredicates_this_match_2_neq_written(void);
void RulesBuiltinPredicates_var_match_2_neq_written(void);
void RulesBuiltinPredicates_this_match_2_or(void);
void RulesBuiltinPredicates_this_match_2_or_written(void);
void RulesBuiltinPredicates_this_match_3_or(void);
void RulesBuiltinPredicates_this_match_3_or_written(void);
void RulesBuiltinPredicates_unresolved_by_name(void);
void RulesBuiltinPredicates_var_eq_wildcard(void);
void RulesBuiltinPredicates_var_eq_any(void);
void RulesBuiltinPredicates_var_eq_wildcard_after_write(void);
void RulesBuiltinPredicates_var_eq_any_after_write(void);
void RulesBuiltinPredicates_var_eq_after_var_0_src(void);
void RulesBuiltinPredicates_2_or_w_eq_this(void);
void RulesBuiltinPredicates_2_or_w_eq_lookup_var(void);
void RulesBuiltinPredicates_3_or_w_eq_lookup_var(void);
void RulesBuiltinPredicates_unwritten_var_error_neq(void);
void RulesBuiltinPredicates_unwritten_var_error_match(void);

// Testsuite 'RulesScopes'
void RulesScopes_term_w_not_scope_1_term(void);
void RulesScopes_term_w_not_scope_2_terms(void);
void RulesScopes_term_w_not_scope_1_term_w_not(void);
void RulesScopes_term_w_not_scope_2_terms_w_not(void);
void RulesScopes_term_w_not_scope_1_term_w_var(void);
void RulesScopes_term_w_not_scope_2_terms_w_var(void);
void RulesScopes_term_w_not_scope_1_term_w_not_w_var(void);
void RulesScopes_term_w_not_scope_2_terms_w_not_w_var(void);
void RulesScopes_term_w_not_scope_2_terms_w_or(void);
void RulesScopes_term_w_not_scope_3_terms_w_or(void);

// Testsuite 'RulesTraversal'
void RulesTraversal_this_self_up_childof(void);
void RulesTraversal_this_up_childof(void);
void RulesTraversal_this_written_self_up_childof(void);
void RulesTraversal_this_written_up_childof(void);
void RulesTraversal_var_self_up_childof(void);
void RulesTraversal_var_up_childof(void);
void RulesTraversal_var_written_self_up_childof(void);
void RulesTraversal_var_written_up_childof(void);
void RulesTraversal_set_var_self_up_childof(void);
void RulesTraversal_set_var_up_childof(void);
void RulesTraversal_set_var_written_self_up_childof(void);
void RulesTraversal_set_var_written_up_childof(void);
void RulesTraversal_ent_self_up_childof(void);
void RulesTraversal_ent_up_childof(void);
void RulesTraversal_implicit_this_self_up_isa(void);
void RulesTraversal_implicit_this_up_isa(void);
void RulesTraversal_implicit_var_self_up_isa(void);
void RulesTraversal_implicit_var_up_isa(void);
void RulesTraversal_implicit_ent_self_up_isa(void);
void RulesTraversal_implicit_ent_up_isa(void);
void RulesTraversal_self_up_2_targets(void);
void RulesTraversal_up_2_targets(void);
void RulesTraversal_self_up_2_targets_diamond(void);
void RulesTraversal_up_2_targets_diamond(void);
void RulesTraversal_written_self_up_2_targets(void);
void RulesTraversal_written_up_2_targets(void);
void RulesTraversal_written_self_up_2_targets_diamond(void);
void RulesTraversal_written_up_2_targets_diamond(void);
void RulesTraversal_2_self_up_terms(void);
void RulesTraversal_2_self_up_terms_2_targets(void);
void RulesTraversal_self_up_empty_table(void);
void RulesTraversal_up_empty_table(void);
void RulesTraversal_self_up_all_owned(void);
void RulesTraversal_up_all_owned(void);
void RulesTraversal_this_self_up_childof_inherited(void);
void RulesTraversal_this_up_childof_inherited(void);
void RulesTraversal_this_written_self_up_childof_inherited(void);
void RulesTraversal_this_written_up_childof_inherited(void);
void RulesTraversal_var_self_up_childof_inherited(void);
void RulesTraversal_var_up_childof_inherited(void);
void RulesTraversal_var_written_self_up_childof_inherited(void);
void RulesTraversal_var_written_up_childof_inherited(void);
void RulesTraversal_ent_self_up_childof_inherited(void);
void RulesTraversal_ent_up_childof_inherited(void);
void RulesTraversal_ent_written_self_up_childof_inherited(void);
void RulesTraversal_ent_written_up_childof_inherited(void);
void RulesTraversal_this_self_up_childof_component(void);
void RulesTraversal_this_up_childof_component(void);
void RulesTraversal_this_written_self_up_childof_component(void);
void RulesTraversal_this_written_up_childof_component(void);
void RulesTraversal_var_self_up_childof_component(void);
void RulesTraversal_var_up_childof_component(void);
void RulesTraversal_var_written_self_up_childof_component(void);
void RulesTraversal_var_written_up_childof_component(void);
void RulesTraversal_this_self_up_childof_recycled_parent(void);
void RulesTraversal_this_up_childof_recycled_parent(void);
void RulesTraversal_this_written_self_up_childof_recycled_parent(void);
void RulesTraversal_this_written_up_childof_recycled_parent(void);
void RulesTraversal_this_self_up_childof_recycled_parent_component(void);
void RulesTraversal_this_up_childof_recycled_parent_component(void);
void RulesTraversal_this_written_self_up_childof_recycled_parent_component(void);
void RulesTraversal_this_written_up_childof_recycled_parent_component(void);
void RulesTraversal_this_self_up_childof_pair(void);
void RulesTraversal_this_up_childof_pair(void);
void RulesTraversal_this_written_self_up_childof_pair(void);
void RulesTraversal_this_written_up_childof_pair(void);
void RulesTraversal_this_self_up_childof_pair_wildcard(void);
void RulesTraversal_this_up_childof_pair_wildcard(void);
void RulesTraversal_this_written_self_up_childof_pair_wildcard(void);
void RulesTraversal_this_written_up_childof_pair_wildcard(void);
void RulesTraversal_this_self_up_childof_pair_tgt_var(void);
void RulesTraversal_this_written_self_up_childof_pair_tgt_var(void);
void RulesTraversal_this_self_up_childof_pair_rel_var(void);
void RulesTraversal_this_written_self_up_childof_pair_rel_var(void);
void RulesTraversal_this_self_up_childof_pair_for_var_written(void);
void RulesTraversal_this_up_childof_pair_for_var_written(void);
void RulesTraversal_this_written_self_up_childof_pair_for_var_written(void);
void RulesTraversal_this_self_up_childof_pair_for_var_written_n_targets(void);
void RulesTraversal_this_written_self_up_childof_pair_for_var_written_n_targets(void);
void RulesTraversal_self_up_2_levels(void);
void RulesTraversal_not_up_disabled(void);
void RulesTraversal_up_2_rel_instances(void);
void RulesTraversal_up_2_rel_instances_match_2nd(void);
void RulesTraversal_up_only_w_owned(void);
void RulesTraversal_this_self_cascade_childof(void);
void RulesTraversal_this_cascade_childof(void);
void RulesTraversal_this_written_self_cascade_childof(void);
void RulesTraversal_this_written_cascade_childof(void);
void RulesTraversal_this_self_cascade_childof_w_parent_flag(void);
void RulesTraversal_this_cascade_childof_w_parent_flag(void);
void RulesTraversal_this_written_self_cascade_childof_w_parent_flag(void);
void RulesTraversal_this_written_cascade_childof_w_parent_flag(void);

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
void Timer_stop_timer_w_rate(void);
void Timer_stop_timer_w_rate_same_src(void);
void Timer_randomize_timers(void);

// Testsuite 'SystemCascade'
void SystemCascade_cascade_depth_1(void);
void SystemCascade_cascade_depth_2(void);
void SystemCascade_cascade_depth_2_new_syntax(void);
void SystemCascade_add_after_match(void);
void SystemCascade_adopt_after_match(void);
void SystemCascade_custom_relation_cascade_depth_1(void);
void SystemCascade_custom_relation_cascade_depth_2(void);
void SystemCascade_custom_relation_add_after_match(void);
void SystemCascade_custom_relation_adopt_after_match(void);

// Testsuite 'SystemManual'
void SystemManual_setup(void);
void SystemManual_1_type_1_component(void);
void SystemManual_no_automerge(void);
void SystemManual_dont_run_w_unmatching_entity_query(void);

// Testsuite 'Tasks'
void Tasks_no_components(void);
void Tasks_one_tag(void);
void Tasks_from_system(void);
void Tasks_tasks_in_phases(void);

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

// Testsuite 'Stats'
void Stats_get_world_stats(void);
void Stats_get_pipeline_stats_before_progress_mini_world(void);
void Stats_get_pipeline_stats_before_progress(void);
void Stats_get_pipeline_stats_after_progress_no_systems(void);
void Stats_get_pipeline_stats_after_progress_1_system(void);
void Stats_get_pipeline_stats_after_progress_1_inactive_system(void);
void Stats_get_pipeline_stats_after_progress_2_systems(void);
void Stats_get_pipeline_stats_after_progress_2_systems_one_merge(void);
void Stats_get_entity_count(void);
void Stats_get_pipeline_stats_w_task_system(void);
void Stats_get_not_alive_entity_count(void);

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
void MultiThread_no_staging_w_multithread(void);
void MultiThread_multithread_w_monitor_addon(void);
void MultiThread_get_ctx(void);
void MultiThread_get_binding_ctx(void);
void MultiThread_get_ctx_w_run(void);
void MultiThread_get_binding_ctx_w_run(void);
void MultiThread_bulk_new_in_no_readonly_w_multithread(void);
void MultiThread_bulk_new_in_no_readonly_w_multithread_2(void);
void MultiThread_run_first_worker_on_main(void);
void MultiThread_run_single_thread_on_main(void);

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
void MultiThreadStaging_set_pair_w_new_target_readonly(void);
void MultiThreadStaging_set_pair_w_new_target_tgt_component_readonly(void);
void MultiThreadStaging_set_pair_w_new_target_defer(void);
void MultiThreadStaging_set_pair_w_new_target_tgt_component_defer(void);

// Testsuite 'MultiTaskThread'
void MultiTaskThread_setup(void);
void MultiTaskThread_2_thread_1_entity(void);
void MultiTaskThread_2_thread_2_entity(void);
void MultiTaskThread_2_thread_5_entity(void);
void MultiTaskThread_2_thread_10_entity(void);
void MultiTaskThread_3_thread_1_entity(void);
void MultiTaskThread_3_thread_2_entity(void);
void MultiTaskThread_3_thread_5_entity(void);
void MultiTaskThread_3_thread_10_entity(void);
void MultiTaskThread_4_thread_1_entity(void);
void MultiTaskThread_4_thread_2_entity(void);
void MultiTaskThread_4_thread_5_entity(void);
void MultiTaskThread_4_thread_10_entity(void);
void MultiTaskThread_5_thread_1_entity(void);
void MultiTaskThread_5_thread_2_entity(void);
void MultiTaskThread_5_thread_5_entity(void);
void MultiTaskThread_5_thread_10_entity(void);
void MultiTaskThread_6_thread_1_entity(void);
void MultiTaskThread_6_thread_2_entity(void);
void MultiTaskThread_6_thread_5_entity(void);
void MultiTaskThread_6_thread_10_entity(void);
void MultiTaskThread_2_thread_1_entity_instanced(void);
void MultiTaskThread_2_thread_5_entity_instanced(void);
void MultiTaskThread_2_thread_10_entity_instanced(void);
void MultiTaskThread_2_thread_test_combs_100_entity_w_next_worker(void);
void MultiTaskThread_2_thread_test_combs_100_entity(void);
void MultiTaskThread_3_thread_test_combs_100_entity(void);
void MultiTaskThread_4_thread_test_combs_100_entity(void);
void MultiTaskThread_5_thread_test_combs_100_entity(void);
void MultiTaskThread_6_thread_test_combs_100_entity(void);
void MultiTaskThread_2_thread_test_combs_100_entity_2_types(void);
void MultiTaskThread_3_thread_test_combs_100_entity_2_types(void);
void MultiTaskThread_4_thread_test_combs_100_entity_2_types(void);
void MultiTaskThread_5_thread_test_combs_100_entity_2_types(void);
void MultiTaskThread_6_thread_test_combs_100_entity_2_types(void);
void MultiTaskThread_change_thread_count(void);
void MultiTaskThread_multithread_quit(void);
void MultiTaskThread_schedule_w_tasks(void);
void MultiTaskThread_reactive_system(void);
void MultiTaskThread_fini_after_set_threads(void);
void MultiTaskThread_2_threads_single_threaded_system(void);
void MultiTaskThread_no_staging_w_multithread(void);
void MultiTaskThread_multithread_w_monitor_addon(void);
void MultiTaskThread_get_ctx(void);
void MultiTaskThread_get_binding_ctx(void);
void MultiTaskThread_get_ctx_w_run(void);
void MultiTaskThread_get_binding_ctx_w_run(void);
void MultiTaskThread_bulk_new_in_no_readonly_w_multithread(void);
void MultiTaskThread_bulk_new_in_no_readonly_w_multithread_2(void);
void MultiTaskThread_run_first_worker_on_main(void);
void MultiTaskThread_run_single_thread_on_main(void);

// Testsuite 'MultiTaskThreadStaging'
void MultiTaskThreadStaging_setup(void);
void MultiTaskThreadStaging_2_threads_add_to_current(void);
void MultiTaskThreadStaging_3_threads_add_to_current(void);
void MultiTaskThreadStaging_4_threads_add_to_current(void);
void MultiTaskThreadStaging_5_threads_add_to_current(void);
void MultiTaskThreadStaging_6_threads_add_to_current(void);
void MultiTaskThreadStaging_2_threads_on_add(void);
void MultiTaskThreadStaging_new_w_count(void);
void MultiTaskThreadStaging_custom_thread_auto_merge(void);
void MultiTaskThreadStaging_custom_thread_manual_merge(void);
void MultiTaskThreadStaging_custom_thread_partial_manual_merge(void);
void MultiTaskThreadStaging_set_pair_w_new_target_readonly(void);
void MultiTaskThreadStaging_set_pair_w_new_target_tgt_component_readonly(void);
void MultiTaskThreadStaging_set_pair_w_new_target_defer(void);
void MultiTaskThreadStaging_set_pair_w_new_target_tgt_component_defer(void);

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
void Modules_import_monitor_2_worlds(void);
void Modules_import_monitor_after_mini(void);
void Modules_import_2_worlds(void);
void Modules_component_parent_becomes_module(void);

// Testsuite 'App'
void App_app_w_frame_action(void);
void App_app_w_default_frame_action(void);
void App_app_w_set_threads(void);
void App_app_w_set_task_threads(void);
void App_app_w_set_target_fps(void);
void App_app_w_set_frames(void);

// Testsuite 'Http'
void Http_teardown(void);
void Http_teardown_started(void);
void Http_teardown_stopped(void);
void Http_stop_start(void);

// Testsuite 'Rest'
void Rest_teardown(void);
void Rest_get(void);
void Rest_get_cached(void);
void Rest_get_cached_invalid(void);
void Rest_try_query(void);
void Rest_query(void);
void Rest_named_query(void);
void Rest_tables(void);
void Rest_request_commands(void);
void Rest_request_commands_2_syncs(void);
void Rest_request_commands_no_frames(void);
void Rest_request_commands_no_commands(void);
void Rest_request_commands_garbage_collect(void);

// Testsuite 'Metrics'
void Metrics_member_gauge_1_entity(void);
void Metrics_member_gauge_2_entities(void);
void Metrics_member_gauge_2_entities_1_existing(void);
void Metrics_member_gauge_2_entities_update(void);
void Metrics_member_gauge_w_remove(void);
void Metrics_member_gauge_w_clear(void);
void Metrics_member_gauge_w_delete(void);
void Metrics_id_gauge_1_entity(void);
void Metrics_id_gauge_2_entities(void);
void Metrics_id_gauge_2_entities_1_existing(void);
void Metrics_id_gauge_w_remove(void);
void Metrics_id_gauge_w_clear(void);
void Metrics_id_gauge_w_delete(void);
void Metrics_oneof_gauge_3_entities(void);
void Metrics_oneof_gauge_3_entities_1_existing(void);
void Metrics_oneof_gauge_w_remove(void);
void Metrics_oneof_gauge_w_clear(void);
void Metrics_oneof_gauge_w_delete(void);
void Metrics_oneof_to_snake_case(void);
void Metrics_member_counter(void);
void Metrics_member_auto_counter(void);
void Metrics_id_counter(void);
void Metrics_oneof_counter(void);
void Metrics_metric_description(void);
void Metrics_id_count(void);
void Metrics_id_target_count(void);
void Metrics_metric_instance_has_doc_name(void);
void Metrics_metric_nested_member(void);
void Metrics_metric_nested_member_counter(void);
void Metrics_metric_nested_member_counter_increment(void);
void Metrics_id_w_member_same_type(void);
void Metrics_id_w_member_mismatching_type(void);
void Metrics_pair_member_rel_type(void);
void Metrics_pair_member_tgt_type(void);
void Metrics_pair_dotmember_rel_type(void);
void Metrics_pair_dotmember_tgt_type(void);
void Metrics_pair_member_counter_increment(void);

// Testsuite 'Alerts'
void Alerts_one_active_alert(void);
void Alerts_two_active_alerts(void);
void Alerts_alert_message(void);
void Alerts_alert_message_w_this_var(void);
void Alerts_alert_message_w_var(void);
void Alerts_alert_message_w_changed_var(void);
void Alerts_set_doc_name(void);
void Alerts_set_brief(void);
void Alerts_alert_instance_has_doc_name(void);
void Alerts_reraise_alert(void);
void Alerts_info_severity(void);
void Alerts_warning_severity(void);
void Alerts_error_severity(void);
void Alerts_expire_after_retain(void);
void Alerts_revive_w_retain(void);
void Alerts_severity_filter(void);
void Alerts_two_severity_filters(void);
void Alerts_severity_filter_w_var(void);
void Alerts_severity_filter_w_var_change_var(void);
void Alerts_member_range_warning(void);
void Alerts_member_range_error(void);
void Alerts_member_range_warning_error(void);
void Alerts_member_range_error_w_warning_severity(void);
void Alerts_member_range_error_w_severity_filter(void);
void Alerts_member_range_warning_w_severity_filter(void);
void Alerts_member_range_pair_id(void);
void Alerts_member_range_invalid_member(void);
void Alerts_member_range_invalid_member_child(void);
void Alerts_member_range_invalid_type(void);
void Alerts_member_range_invalid_member_type(void);
void Alerts_member_range_no_range(void);
void Alerts_member_range_alert_two_instances(void);
void Alerts_member_range_from_var(void);
void Alerts_member_range_from_var_after_remove(void);
void Alerts_retained_alert_w_dead_source(void);
void Alerts_alert_counts(void);

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
        "component_explicit_subject_this_by_var_name",
        Parser_component_explicit_subject_this_by_var_name
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
        "component_explicit_subject_0",
        Parser_component_explicit_subject_0
    },
    {
        "this_as_predicate",
        Parser_this_as_predicate
    },
    {
        "this_var_as_predicate",
        Parser_this_var_as_predicate
    },
    {
        "this_lowercase_var_as_predicate",
        Parser_this_lowercase_var_as_predicate
    },
    {
        "this_as_object",
        Parser_this_as_object
    },
    {
        "this_var_as_object",
        Parser_this_var_as_object
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
        "pair_implicit_subject_pred_w_self",
        Parser_pair_implicit_subject_pred_w_self
    },
    {
        "pair_implicit_subject_obj_w_self",
        Parser_pair_implicit_subject_obj_w_self
    },
    {
        "pair_implicit_subject_pred_w_up",
        Parser_pair_implicit_subject_pred_w_up
    },
    {
        "pair_implicit_subject_obj_w_up",
        Parser_pair_implicit_subject_obj_w_up
    },
    {
        "pair_implicit_subject_pred_w_self_up",
        Parser_pair_implicit_subject_pred_w_self_up
    },
    {
        "pair_implicit_subject_obj_w_self_up",
        Parser_pair_implicit_subject_obj_w_self_up
    },
    {
        "pair_implicit_subject_pred_w_up_trav",
        Parser_pair_implicit_subject_pred_w_up_trav
    },
    {
        "pair_implicit_subject_obj_w_up_trav",
        Parser_pair_implicit_subject_obj_w_up_trav
    },
    {
        "pair_implicit_subject_pred_w_invalid_flags",
        Parser_pair_implicit_subject_pred_w_invalid_flags
    },
    {
        "pair_implicit_subject_obj_w_invalid_flags",
        Parser_pair_implicit_subject_obj_w_invalid_flags
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
        "pair_explicit_subject_this_by_var_name",
        Parser_pair_explicit_subject_this_by_var_name
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
        "pair_explicit_subject_0",
        Parser_pair_explicit_subject_0
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
        "pred_var",
        Parser_pred_var
    },
    {
        "obj_var",
        Parser_obj_var
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
        "not_alive_pred",
        Parser_not_alive_pred
    },
    {
        "not_alive_subj",
        Parser_not_alive_subj
    },
    {
        "not_alive_obj",
        Parser_not_alive_obj
    },
    {
        "this_subj_var_kind",
        Parser_this_subj_var_kind
    },
    {
        "this_obj_var_kind",
        Parser_this_obj_var_kind
    },
    {
        "this_subj_obj_var_kind",
        Parser_this_subj_obj_var_kind
    },
    {
        "var_w_name",
        Parser_var_w_name
    },
    {
        "entity_pred_no_name",
        Parser_entity_pred_no_name
    },
    {
        "entity_subj_no_name",
        Parser_entity_subj_no_name
    },
    {
        "entity_obj_no_name",
        Parser_entity_obj_no_name
    },
    {
        "this_pred_no_name",
        Parser_this_pred_no_name
    },
    {
        "this_subj_no_name",
        Parser_this_subj_no_name
    },
    {
        "this_obj_no_name",
        Parser_this_obj_no_name
    },
    {
        "auto_object_variable",
        Parser_auto_object_variable
    },
    {
        "auto_object_variable_w_subj",
        Parser_auto_object_variable_w_subj
    },
    {
        "auto_scoped_variable",
        Parser_auto_scoped_variable
    },
    {
        "invalid_variable_only",
        Parser_invalid_variable_only
    },
    {
        "oneof_self_pred_w_relative_obj",
        Parser_oneof_self_pred_w_relative_obj
    },
    {
        "oneof_other_pred_w_relative_obj",
        Parser_oneof_other_pred_w_relative_obj
    },
    {
        "oneof_self_pred_w_invalid_obj",
        Parser_oneof_self_pred_w_invalid_obj
    },
    {
        "oneof_other_pred_w_invalid_obj",
        Parser_oneof_other_pred_w_invalid_obj
    },
    {
        "oneof_w_other_entity_w_same_name",
        Parser_oneof_w_other_entity_w_same_name
    },
    {
        "oneof_w_other_entity_w_same_name_w_set_scope",
        Parser_oneof_w_other_entity_w_same_name_w_set_scope
    },
    {
        "oneof_w_wildcard",
        Parser_oneof_w_wildcard
    },
    {
        "oneof_w_any",
        Parser_oneof_w_any
    },
    {
        "oneof_w_fullpath",
        Parser_oneof_w_fullpath
    },
    {
        "pair_implicit_src_missing_rel",
        Parser_pair_implicit_src_missing_rel
    },
    {
        "pair_implicit_src_missing_obj",
        Parser_pair_implicit_src_missing_obj
    },
    {
        "pair_explicit_src_missing_src",
        Parser_pair_explicit_src_missing_src
    },
    {
        "pair_explicit_src_missing_obj",
        Parser_pair_explicit_src_missing_obj
    },
    {
        "eq_id",
        Parser_eq_id
    },
    {
        "eq_id_var",
        Parser_eq_id_var
    },
    {
        "eq_var_id",
        Parser_eq_var_id
    },
    {
        "eq_var",
        Parser_eq_var
    },
    {
        "neq_id",
        Parser_neq_id
    },
    {
        "neq_id_var",
        Parser_neq_id_var
    },
    {
        "neq_var_id",
        Parser_neq_var_id
    },
    {
        "neq_var",
        Parser_neq_var
    },
    {
        "eq_name",
        Parser_eq_name
    },
    {
        "eq_name_var",
        Parser_eq_name_var
    },
    {
        "eq_var_name",
        Parser_eq_var_name
    },
    {
        "eq_var",
        Parser_eq_var
    },
    {
        "neq_name",
        Parser_neq_name
    },
    {
        "neq_name_var",
        Parser_neq_name_var
    },
    {
        "neq_var_name",
        Parser_neq_var_name
    },
    {
        "neq_var",
        Parser_neq_var
    },
    {
        "match_name",
        Parser_match_name
    },
    {
        "match_name_var",
        Parser_match_name_var
    },
    {
        "match_var_name",
        Parser_match_var_name
    },
    {
        "match_var",
        Parser_match_var
    },
    {
        "nmatch_name",
        Parser_nmatch_name
    },
    {
        "nmatch_name_var",
        Parser_nmatch_name_var
    },
    {
        "nmatch_var_name",
        Parser_nmatch_var_name
    },
    {
        "eq_same_var",
        Parser_eq_same_var
    },
    {
        "neq_same_var",
        Parser_neq_same_var
    },
    {
        "eq_same_var_this",
        Parser_eq_same_var_this
    },
    {
        "neq_same_var_this",
        Parser_neq_same_var_this
    },
    {
        "eq_w_optional",
        Parser_eq_w_optional
    },
    {
        "neq_w_optional",
        Parser_neq_w_optional
    },
    {
        "match_w_optional",
        Parser_match_w_optional
    },
    {
        "query_scope_1_term",
        Parser_query_scope_1_term
    },
    {
        "query_scope_1_term_spaces",
        Parser_query_scope_1_term_spaces
    },
    {
        "query_scope_2_terms",
        Parser_query_scope_2_terms
    },
    {
        "query_nested_scope",
        Parser_query_nested_scope
    },
    {
        "query_nested_scope_spaces",
        Parser_query_nested_scope_spaces
    },
    {
        "query_scope_unbalanced",
        Parser_query_scope_unbalanced
    },
    {
        "query_not_scope",
        Parser_query_not_scope
    },
    {
        "query_empty_scope",
        Parser_query_empty_scope
    },
    {
        "query_scope_newline_after_open",
        Parser_query_scope_newline_after_open
    },
    {
        "query_scope_newline_after_close",
        Parser_query_scope_newline_after_close
    },
    {
        "override_tag",
        Parser_override_tag
    },
    {
        "override_pair",
        Parser_override_pair
    },
    {
        "pair_3_args",
        Parser_pair_3_args
    },
    {
        "pair_3_args_implicit_this",
        Parser_pair_3_args_implicit_this
    },
    {
        "pair_4_args",
        Parser_pair_4_args
    },
    {
        "pair_4_args_implicit_this",
        Parser_pair_4_args_implicit_this
    },
    {
        "pair_3_args_2_terms",
        Parser_pair_3_args_2_terms
    },
    {
        "pair_3_args_this_tgt",
        Parser_pair_3_args_this_tgt
    },
    {
        "pair_3_args_2_terms_this_tgt",
        Parser_pair_3_args_2_terms_this_tgt
    },
    {
        "pair_3_args_2_terms_this_tgt_implicit_this",
        Parser_pair_3_args_2_terms_this_tgt_implicit_this
    },
    {
        "pair_3_or_args",
        Parser_pair_3_or_args
    },
    {
        "pair_3_or_args_implicit_this",
        Parser_pair_3_or_args_implicit_this
    },
    {
        "pair_4_or_args",
        Parser_pair_4_or_args
    },
    {
        "pair_4_or_args_implicit_this",
        Parser_pair_4_or_args_implicit_this
    },
    {
        "pair_or_before_and_oper",
        Parser_pair_or_before_and_oper
    },
    {
        "pair_and_before_or_oper",
        Parser_pair_and_before_or_oper
    },
    {
        "cascade_desc",
        Parser_cascade_desc
    },
    {
        "newline_after_inout",
        Parser_newline_after_inout
    },
    {
        "newline_after_term_open",
        Parser_newline_after_term_open
    },
    {
        "newline_after_term_src",
        Parser_newline_after_term_src
    },
    {
        "newline_after_term_src_pair",
        Parser_newline_after_term_src_pair
    },
    {
        "newline_after_term_pair_comma",
        Parser_newline_after_term_pair_comma
    },
    {
        "newline_after_term_pair_second",
        Parser_newline_after_term_pair_second
    },
    {
        "tag_w_space_implicit_this",
        Parser_tag_w_space_implicit_this
    },
    {
        "tag_w_space",
        Parser_tag_w_space
    },
    {
        "pair_first_w_space_implicit_this",
        Parser_pair_first_w_space_implicit_this
    },
    {
        "pair_first_w_space",
        Parser_pair_first_w_space
    },
    {
        "pair_second_w_space_implicit_this",
        Parser_pair_second_w_space_implicit_this
    },
    {
        "pair_second_w_space",
        Parser_pair_second_w_space
    },
    {
        "pair_src_w_space",
        Parser_pair_src_w_space
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
        "dot_assign_nested_member",
        Plecs_dot_assign_nested_member
    },
    {
        "dot_assign_binary_expr",
        Plecs_dot_assign_binary_expr
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
        "assign_pair_component_in_script",
        Plecs_assign_pair_component_in_script
    },
    {
        "assign_pair_component_in_script_update",
        Plecs_assign_pair_component_in_script_update
    },
    {
        "set_entity_names",
        Plecs_set_entity_names
    },
    {
        "oneof",
        Plecs_oneof
    },
    {
        "invalid_oneof",
        Plecs_invalid_oneof
    },
    {
        "brief_annotation",
        Plecs_brief_annotation
    },
    {
        "name_annotation",
        Plecs_name_annotation
    },
    {
        "link_annotation",
        Plecs_link_annotation
    },
    {
        "color_annotation",
        Plecs_color_annotation
    },
    {
        "multiple_annotations",
        Plecs_multiple_annotations
    },
    {
        "annotation_w_trailing_space",
        Plecs_annotation_w_trailing_space
    },
    {
        "multiline_string",
        Plecs_multiline_string
    },
    {
        "unterminated_multiline_string",
        Plecs_unterminated_multiline_string
    },
    {
        "declaration_w_underscore_name",
        Plecs_declaration_w_underscore_name
    },
    {
        "annotate_declaration",
        Plecs_annotate_declaration
    },
    {
        "anonymous_entity",
        Plecs_anonymous_entity
    },
    {
        "anonymous_entity_in_scope",
        Plecs_anonymous_entity_in_scope
    },
    {
        "anonymous_declaration",
        Plecs_anonymous_declaration
    },
    {
        "const_var_int",
        Plecs_const_var_int
    },
    {
        "const_var_float",
        Plecs_const_var_float
    },
    {
        "const_var_bool",
        Plecs_const_var_bool
    },
    {
        "const_var_string",
        Plecs_const_var_string
    },
    {
        "const_var_struct",
        Plecs_const_var_struct
    },
    {
        "const_var_redeclare",
        Plecs_const_var_redeclare
    },
    {
        "const_var_scoped",
        Plecs_const_var_scoped
    },
    {
        "assign_component_from_var",
        Plecs_assign_component_from_var
    },
    {
        "assign_component_from_var_in_scope",
        Plecs_assign_component_from_var_in_scope
    },
    {
        "scope_w_component_after_const_var",
        Plecs_scope_w_component_after_const_var
    },
    {
        "component_after_const_paren_expr",
        Plecs_component_after_const_paren_expr
    },
    {
        "component_after_const_add_expr",
        Plecs_component_after_const_add_expr
    },
    {
        "component_after_const_sub_expr",
        Plecs_component_after_const_sub_expr
    },
    {
        "component_after_const_mul_expr",
        Plecs_component_after_const_mul_expr
    },
    {
        "component_after_const_div_expr",
        Plecs_component_after_const_div_expr
    },
    {
        "parse_with",
        Plecs_parse_with
    },
    {
        "parse_with_w_with",
        Plecs_parse_with_w_with
    },
    {
        "parse_with_w_tag",
        Plecs_parse_with_w_tag
    },
    {
        "parse_with_value",
        Plecs_parse_with_value
    },
    {
        "parse_with_2_values",
        Plecs_parse_with_2_values
    },
    {
        "parse_with_2_nested_values",
        Plecs_parse_with_2_nested_values
    },
    {
        "parse_with_var",
        Plecs_parse_with_var
    },
    {
        "parse_with_2_vars",
        Plecs_parse_with_2_vars
    },
    {
        "parse_with_2_nested_vars",
        Plecs_parse_with_2_nested_vars
    },
    {
        "parse_with_var_in_scope",
        Plecs_parse_with_var_in_scope
    },
    {
        "assign_const_w_expr",
        Plecs_assign_const_w_expr
    },
    {
        "const_w_type",
        Plecs_const_w_type
    },
    {
        "assembly_no_scope",
        Plecs_assembly_no_scope
    },
    {
        "assembly_empty",
        Plecs_assembly_empty
    },
    {
        "assembly_no_props",
        Plecs_assembly_no_props
    },
    {
        "assembly_prop_no_type",
        Plecs_assembly_prop_no_type
    },
    {
        "assembly_prop_no_default",
        Plecs_assembly_prop_no_default
    },
    {
        "assembly_prop",
        Plecs_assembly_prop
    },
    {
        "assembly_prop_space_colon",
        Plecs_assembly_prop_space_colon
    },
    {
        "assembly_2_props",
        Plecs_assembly_2_props
    },
    {
        "assembly_instance_w_default_values",
        Plecs_assembly_instance_w_default_values
    },
    {
        "assembly_instance_w_assign_default_values",
        Plecs_assembly_instance_w_assign_default_values
    },
    {
        "assembly_instance_w_overridden_values",
        Plecs_assembly_instance_w_overridden_values
    },
    {
        "assembly_w_child",
        Plecs_assembly_w_child
    },
    {
        "assembly_w_child_parse_script",
        Plecs_assembly_w_child_parse_script
    },
    {
        "assembly_w_child_parse_script_twice",
        Plecs_assembly_w_child_parse_script_twice
    },
    {
        "assembly_w_child_update_after_parse",
        Plecs_assembly_w_child_update_after_parse
    },
    {
        "assembly_w_nested_child",
        Plecs_assembly_w_nested_child
    },
    {
        "assembly_w_prefab",
        Plecs_assembly_w_prefab
    },
    {
        "assembly_w_prefab_tree",
        Plecs_assembly_w_prefab_tree
    },
    {
        "assembly_w_nested_assembly",
        Plecs_assembly_w_nested_assembly
    },
    {
        "instantiate_prefab_w_assembly",
        Plecs_instantiate_prefab_w_assembly
    },
    {
        "assembly_w_prefab_w_assembly",
        Plecs_assembly_w_prefab_w_assembly
    },
    {
        "3_assemblies",
        Plecs_3_assemblies
    },
    {
        "assembly_nested_w_default_var",
        Plecs_assembly_nested_w_default_var
    },
    {
        "assembly_w_anonymous",
        Plecs_assembly_w_anonymous
    },
    {
        "assembly_w_anonymous_parse_again",
        Plecs_assembly_w_anonymous_parse_again
    },
    {
        "typed_const_w_composite_type_invalid_assignment",
        Plecs_typed_const_w_composite_type_invalid_assignment
    },
    {
        "typed_const_w_composite_type",
        Plecs_typed_const_w_composite_type
    },
    {
        "assign_var_to_typed_const_w_composite_type",
        Plecs_assign_var_to_typed_const_w_composite_type
    },
    {
        "typed_const_w_composite_type_invalid_assignment",
        Plecs_typed_const_w_composite_type_invalid_assignment
    },
    {
        "assembly_w_composite_prop_invalid_assignment",
        Plecs_assembly_w_composite_prop_invalid_assignment
    },
    {
        "assembly_w_composite_prop",
        Plecs_assembly_w_composite_prop
    },
    {
        "assembly_with_with",
        Plecs_assembly_with_with
    },
    {
        "using_wildcard",
        Plecs_using_wildcard
    },
    {
        "single_line_comment_in_value",
        Plecs_single_line_comment_in_value
    },
    {
        "single_line_comment_in_value_after_scope",
        Plecs_single_line_comment_in_value_after_scope
    },
    {
        "multi_line_comment_in_value",
        Plecs_multi_line_comment_in_value
    },
    {
        "multi_line_comment_in_value_after_scope",
        Plecs_multi_line_comment_in_value_after_scope
    },
    {
        "unterminated_multi_line_comment_in_value",
        Plecs_unterminated_multi_line_comment_in_value
    },
    {
        "module_stmt",
        Plecs_module_stmt
    },
    {
        "nested_module_stmt",
        Plecs_nested_module_stmt
    },
    {
        "module_stmt_w_scope",
        Plecs_module_stmt_w_scope
    },
    {
        "module_stmt_w_nested_scope",
        Plecs_module_stmt_w_nested_scope
    },
    {
        "module_w_assembly",
        Plecs_module_w_assembly
    },
    {
        "module_w_nested_assembly",
        Plecs_module_w_nested_assembly
    },
    {
        "assign_singleton_tag",
        Plecs_assign_singleton_tag
    },
    {
        "assign_singleton_component",
        Plecs_assign_singleton_component
    },
    {
        "assign_singleton_tag_w_scope",
        Plecs_assign_singleton_tag_w_scope
    },
    {
        "assign_singleton_2_tags_w_scope",
        Plecs_assign_singleton_2_tags_w_scope
    },
    {
        "assign_singleton_component_w_scope",
        Plecs_assign_singleton_component_w_scope
    },
    {
        "assign_singleton_2_components_w_scope",
        Plecs_assign_singleton_2_components_w_scope
    },
    {
        "with_pair_in_scope",
        Plecs_with_pair_in_scope
    },
    {
        "assembly_redeclare_prop_as_const",
        Plecs_assembly_redeclare_prop_as_const
    },
    {
        "assembly_redeclare_prop_as_prop",
        Plecs_assembly_redeclare_prop_as_prop
    },
    {
        "assembly_redeclare_const_as_const",
        Plecs_assembly_redeclare_const_as_const
    },
    {
        "add_auto_override",
        Plecs_add_auto_override
    },
    {
        "add_auto_override_pair",
        Plecs_add_auto_override_pair
    },
    {
        "scope_w_auto_override",
        Plecs_scope_w_auto_override
    },
    {
        "scope_w_auto_override_pair",
        Plecs_scope_w_auto_override_pair
    },
    {
        "pair_w_rel_var",
        Plecs_pair_w_rel_var
    },
    {
        "pair_w_tgt_var",
        Plecs_pair_w_tgt_var
    },
    {
        "assembly_w_pair_w_this_var",
        Plecs_assembly_w_pair_w_this_var
    },
    {
        "with_value_not_a_component",
        Plecs_with_value_not_a_component
    },
    {
        "component_in_with_scope",
        Plecs_component_in_with_scope
    },
    {
        "component_in_with_scope_nested",
        Plecs_component_in_with_scope_nested
    },
    {
        "component_in_with_scope_in_scope",
        Plecs_component_in_with_scope_in_scope
    },
    {
        "assign_after_with_in_scope",
        Plecs_assign_after_with_in_scope
    },
    {
        "array_component",
        Plecs_array_component
    }
};

bake_test_case Doc_testcases[] = {
    {
        "get_set_name",
        Doc_get_set_name
    },
    {
        "get_entity_name",
        Doc_get_entity_name
    },
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
    },
    {
        "set_name_nullptr",
        Doc_set_name_nullptr
    },
    {
        "set_brief_nullptr",
        Doc_set_brief_nullptr
    },
    {
        "set_detail_nullptr",
        Doc_set_detail_nullptr
    },
    {
        "set_link_nullptr",
        Doc_set_link_nullptr
    },
    {
        "set_color_nullptr",
        Doc_set_color_nullptr
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
        "mixed_multithreaded_tasks",
        Pipeline_mixed_multithreaded_tasks
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
        "multi_threaded_pipeline_change_tasks",
        Pipeline_multi_threaded_pipeline_change_tasks
    },
    {
        "activate_after_add",
        Pipeline_activate_after_add
    },
    {
        "match_all_after_pipeline_rebuild",
        Pipeline_match_all_after_pipeline_rebuild
    },
    {
        "empty_pipeline",
        Pipeline_empty_pipeline
    },
    {
        "custom_pipeline_w_system_macro",
        Pipeline_custom_pipeline_w_system_macro
    },
    {
        "pipeline_w_short_notation",
        Pipeline_pipeline_w_short_notation
    },
    {
        "stack_allocator_after_progress",
        Pipeline_stack_allocator_after_progress
    },
    {
        "stack_allocator_after_progress_w_pipeline_change",
        Pipeline_stack_allocator_after_progress_w_pipeline_change
    },
    {
        "iter_from_world_in_singlethread_system_multitead_app",
        Pipeline_iter_from_world_in_singlethread_system_multitead_app
    },
    {
        "iter_from_world_in_singlethread_system_multitead_app_tasks",
        Pipeline_iter_from_world_in_singlethread_system_multitead_app_tasks
    },
    {
        "no_staging_after_inactive_system",
        Pipeline_no_staging_after_inactive_system
    },
    {
        "inactive_system_after_no_staging_system_no_defer_w_filter",
        Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter
    },
    {
        "inactive_system_after_2_no_staging_system_no_defer_w_filter",
        Pipeline_inactive_system_after_2_no_staging_system_no_defer_w_filter
    },
    {
        "inactive_system_after_no_staging_system_no_defer_w_filter_w_no_staging_at_end",
        Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter_w_no_staging_at_end
    },
    {
        "inactive_multithread_system_after_no_staging_system_no_defer",
        Pipeline_inactive_multithread_system_after_no_staging_system_no_defer
    },
    {
        "inactive_multithread_tasks_system_after_no_staging_system_no_defer",
        Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer
    },
    {
        "inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end",
        Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end
    },
    {
        "inactive_multithread_tasks_system_after_no_staging_system_no_defer_w_no_staging_at_end",
        Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer_w_no_staging_at_end
    },
    {
        "multi_threaded_pipeline_change_w_only_singlethreaded",
        Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded
    },
    {
        "multi_threaded_tasks_pipeline_change_w_only_singlethreaded",
        Pipeline_multi_threaded_tasks_pipeline_change_w_only_singlethreaded
    },
    {
        "sync_after_not_out_for_out",
        Pipeline_sync_after_not_out_for_out
    },
    {
        "pair_wildcard_read_after_staged_write",
        Pipeline_pair_wildcard_read_after_staged_write
    },
    {
        "pair_read_after_staged_wildcard_write",
        Pipeline_pair_read_after_staged_wildcard_write
    },
    {
        "no_sync_after_pair_wildcard_read_after_unmatching_staged_write",
        Pipeline_no_sync_after_pair_wildcard_read_after_unmatching_staged_write
    },
    {
        "no_merge_after_from_nothing_w_default_inout",
        Pipeline_no_merge_after_from_nothing_w_default_inout
    },
    {
        "on_merge_activate_system_before_merge",
        Pipeline_on_merge_activate_system_before_merge
    },
    {
        "disable_phase",
        Pipeline_disable_phase
    },
    {
        "disable_parent",
        Pipeline_disable_parent
    },
    {
        "multi_threaded_no_staging_w_add_after_read",
        Pipeline_multi_threaded_no_staging_w_add_after_read
    },
    {
        "multi_threaded_tasks_no_staging_w_add_after_read",
        Pipeline_multi_threaded_tasks_no_staging_w_add_after_read
    },
    {
        "1_startup_system",
        Pipeline_1_startup_system
    },
    {
        "2_startup_systems",
        Pipeline_2_startup_systems
    },
    {
        "2_startup_phases",
        Pipeline_2_startup_phases
    },
    {
        "2_startup_systems_w_merge",
        Pipeline_2_startup_systems_w_merge
    },
    {
        "inactive_last_system_merge_count",
        Pipeline_inactive_last_system_merge_count
    },
    {
        "inactive_middle_system_merge_count",
        Pipeline_inactive_middle_system_merge_count
    },
    {
        "last_no_readonly_system_merge_count",
        Pipeline_last_no_readonly_system_merge_count
    },
    {
        "2_pipelines_1_system",
        Pipeline_2_pipelines_1_system
    },
    {
        "builtin_pipeline_w_self_system_term",
        Pipeline_builtin_pipeline_w_self_system_term
    },
    {
        "custom_pipeline_w_self_system_term",
        Pipeline_custom_pipeline_w_self_system_term
    },
    {
        "switch_from_threads_to_tasks",
        Pipeline_switch_from_threads_to_tasks
    },
    {
        "switch_from_tasks_to_threads",
        Pipeline_switch_from_tasks_to_threads
    },
    {
        "run_pipeline_multithreaded",
        Pipeline_run_pipeline_multithreaded
    },
    {
        "run_pipeline_multithreaded_tasks",
        Pipeline_run_pipeline_multithreaded_tasks
    },
    {
        "pipeline_init_no_terms",
        Pipeline_pipeline_init_no_terms
    },
    {
        "pipeline_init_no_system_term",
        Pipeline_pipeline_init_no_system_term
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
        "invalid_0_w_from_entity",
        SystemMisc_invalid_0_w_from_entity
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
    },
    {
        "update_ctx",
        SystemMisc_update_ctx
    },
    {
        "run_custom_run_action",
        SystemMisc_run_custom_run_action
    },
    {
        "run_w_offset_limit_custom_run_action",
        SystemMisc_run_w_offset_limit_custom_run_action
    },
    {
        "pipeline_custom_run_action",
        SystemMisc_pipeline_custom_run_action
    },
    {
        "change_custom_run_action",
        SystemMisc_change_custom_run_action
    },
    {
        "custom_run_action_call_next",
        SystemMisc_custom_run_action_call_next
    },
    {
        "system_w_short_notation",
        SystemMisc_system_w_short_notation
    },
    {
        "update_interval_w_system_init",
        SystemMisc_update_interval_w_system_init
    },
    {
        "update_rate_w_system_init",
        SystemMisc_update_rate_w_system_init
    },
    {
        "system_w_interval_rate_stop_timer",
        SystemMisc_system_w_interval_rate_stop_timer
    },
    {
        "system_same_interval_same_tick",
        SystemMisc_system_same_interval_same_tick
    }
};

bake_test_case RulesBasic_testcases[] = {
    {
        "1_fact_w_tag",
        RulesBasic_1_fact_w_tag
    },
    {
        "1_fact_w_component",
        RulesBasic_1_fact_w_component
    },
    {
        "1_fact_w_tag_pair",
        RulesBasic_1_fact_w_tag_pair
    },
    {
        "1_fact_w_component_pair",
        RulesBasic_1_fact_w_component_pair
    },
    {
        "2_facts_same_src_w_tag",
        RulesBasic_2_facts_same_src_w_tag
    },
    {
        "2_facts_same_src_w_component",
        RulesBasic_2_facts_same_src_w_component
    },
    {
        "2_facts_same_src_w_tag_pair",
        RulesBasic_2_facts_same_src_w_tag_pair
    },
    {
        "2_facts_same_src_w_component_pair",
        RulesBasic_2_facts_same_src_w_component_pair
    },
    {
        "2_facts_other_src_w_tag",
        RulesBasic_2_facts_other_src_w_tag
    },
    {
        "2_facts_other_src_w_component",
        RulesBasic_2_facts_other_src_w_component
    },
    {
        "2_facts_other_src_w_tag_pair",
        RulesBasic_2_facts_other_src_w_tag_pair
    },
    {
        "2_facts_other_src_w_component_pair",
        RulesBasic_2_facts_other_src_w_component_pair
    },
    {
        "1_fact_w_any",
        RulesBasic_1_fact_w_any
    },
    {
        "1_fact_w_pair_any_tgt",
        RulesBasic_1_fact_w_pair_any_tgt
    },
    {
        "1_fact_w_pair_any_rel",
        RulesBasic_1_fact_w_pair_any_rel
    },
    {
        "1_fact_w_pair_any_rel_tgt",
        RulesBasic_1_fact_w_pair_any_rel_tgt
    },
    {
        "2_facts_same_src_w_any",
        RulesBasic_2_facts_same_src_w_any
    },
    {
        "2_facts_same_src_w_pair_any_tgt",
        RulesBasic_2_facts_same_src_w_pair_any_tgt
    },
    {
        "2_facts_same_src_w_pair_any_rel",
        RulesBasic_2_facts_same_src_w_pair_any_rel
    },
    {
        "2_facts_same_src_w_pair_any_rel_tgt",
        RulesBasic_2_facts_same_src_w_pair_any_rel_tgt
    },
    {
        "2_facts_other_src_w_any",
        RulesBasic_2_facts_other_src_w_any
    },
    {
        "2_facts_other_src_w_pair_any_tgt",
        RulesBasic_2_facts_other_src_w_pair_any_tgt
    },
    {
        "2_facts_other_src_w_pair_any_rel",
        RulesBasic_2_facts_other_src_w_pair_any_rel
    },
    {
        "2_facts_other_src_w_pair_any_rel_tgt",
        RulesBasic_2_facts_other_src_w_pair_any_rel_tgt
    },
    {
        "1_this_src_w_tag",
        RulesBasic_1_this_src_w_tag
    },
    {
        "1_this_src_w_component",
        RulesBasic_1_this_src_w_component
    },
    {
        "1_this_src_w_tag_pair",
        RulesBasic_1_this_src_w_tag_pair
    },
    {
        "1_this_src_w_component_pair",
        RulesBasic_1_this_src_w_component_pair
    },
    {
        "1_this_src_w_tag_2_tables",
        RulesBasic_1_this_src_w_tag_2_tables
    },
    {
        "1_this_src_w_component_2_tables",
        RulesBasic_1_this_src_w_component_2_tables
    },
    {
        "1_this_src_w_tag_pair_2_tables",
        RulesBasic_1_this_src_w_tag_pair_2_tables
    },
    {
        "1_this_src_w_component_pair_2_tables",
        RulesBasic_1_this_src_w_component_pair_2_tables
    },
    {
        "2_this_src_w_tag",
        RulesBasic_2_this_src_w_tag
    },
    {
        "2_this_src_w_component",
        RulesBasic_2_this_src_w_component
    },
    {
        "2_this_src_ent_src_w_tag",
        RulesBasic_2_this_src_ent_src_w_tag
    },
    {
        "2_this_src_ent_src_w_component",
        RulesBasic_2_this_src_ent_src_w_component
    },
    {
        "2_ent_src_this_src_w_tag",
        RulesBasic_2_ent_src_this_src_w_tag
    },
    {
        "2_ent_src_this_src_w_component",
        RulesBasic_2_ent_src_this_src_w_component
    },
    {
        "recycled_tag",
        RulesBasic_recycled_tag
    },
    {
        "recycled_src",
        RulesBasic_recycled_src
    },
    {
        "recycled_pair_rel",
        RulesBasic_recycled_pair_rel
    },
    {
        "recycled_pair_tgt",
        RulesBasic_recycled_pair_tgt
    },
    {
        "this_src_w_wildcard",
        RulesBasic_this_src_w_wildcard
    },
    {
        "this_src_w_pair_rel_wildcard",
        RulesBasic_this_src_w_pair_rel_wildcard
    },
    {
        "this_src_w_pair_tgt_wildcard",
        RulesBasic_this_src_w_pair_tgt_wildcard
    },
    {
        "this_src_w_pair_rel_tgt_wildcard",
        RulesBasic_this_src_w_pair_rel_tgt_wildcard
    },
    {
        "this_src_w_any",
        RulesBasic_this_src_w_any
    },
    {
        "this_src_w_any_written",
        RulesBasic_this_src_w_any_written
    },
    {
        "this_src_w_pair_rel_any",
        RulesBasic_this_src_w_pair_rel_any
    },
    {
        "this_src_w_pair_tgt_any",
        RulesBasic_this_src_w_pair_tgt_any
    },
    {
        "this_src_w_pair_rel_tgt_any",
        RulesBasic_this_src_w_pair_rel_tgt_any
    },
    {
        "ent_src_w_wildcard",
        RulesBasic_ent_src_w_wildcard
    },
    {
        "ent_src_w_pair_rel_wildcard",
        RulesBasic_ent_src_w_pair_rel_wildcard
    },
    {
        "ent_src_w_pair_tgt_wildcard",
        RulesBasic_ent_src_w_pair_tgt_wildcard
    },
    {
        "ent_src_w_pair_rel_tgt_wildcard",
        RulesBasic_ent_src_w_pair_rel_tgt_wildcard
    },
    {
        "1_wildcard_src",
        RulesBasic_1_wildcard_src
    },
    {
        "1_wildcard_src_w_pair",
        RulesBasic_1_wildcard_src_w_pair
    },
    {
        "2_wildcard_src",
        RulesBasic_2_wildcard_src
    },
    {
        "2_wildcard_src_w_pair",
        RulesBasic_2_wildcard_src_w_pair
    },
    {
        "1_wildcard_src_w_pair_tgt_var",
        RulesBasic_1_wildcard_src_w_pair_tgt_var
    },
    {
        "1_wildcard_src_w_pair_rel_var",
        RulesBasic_1_wildcard_src_w_pair_rel_var
    },
    {
        "1_wildcard_src_w_pair_tgt_this",
        RulesBasic_1_wildcard_src_w_pair_tgt_this
    },
    {
        "1_wildcard_src_w_pair_rel_this",
        RulesBasic_1_wildcard_src_w_pair_rel_this
    },
    {
        "1_any_src",
        RulesBasic_1_any_src
    },
    {
        "1_any_src_component",
        RulesBasic_1_any_src_component
    },
    {
        "1_any_src_component_w_this_component",
        RulesBasic_1_any_src_component_w_this_component
    },
    {
        "1_any_src_w_pair",
        RulesBasic_1_any_src_w_pair
    },
    {
        "2_any_src",
        RulesBasic_2_any_src
    },
    {
        "2_any_src_w_pair",
        RulesBasic_2_any_src_w_pair
    },
    {
        "1_any_src_w_pair_tgt_var",
        RulesBasic_1_any_src_w_pair_tgt_var
    },
    {
        "1_any_src_w_pair_rel_var",
        RulesBasic_1_any_src_w_pair_rel_var
    },
    {
        "1_any_src_w_pair_tgt_this",
        RulesBasic_1_any_src_w_pair_tgt_this
    },
    {
        "1_any_src_w_pair_rel_this",
        RulesBasic_1_any_src_w_pair_rel_this
    },
    {
        "1_any_src_any_tgt_w_rel_fixed",
        RulesBasic_1_any_src_any_tgt_w_rel_fixed
    },
    {
        "1_any_src_any_rel_w_tgt_fixed",
        RulesBasic_1_any_src_any_rel_w_tgt_fixed
    },
    {
        "1_any_src_w_childof_pair_any_tgt",
        RulesBasic_1_any_src_w_childof_pair_any_tgt
    },
    {
        "1_any_src_empty",
        RulesBasic_1_any_src_empty
    },
    {
        "1_any_src_pair_tgt_wildcard_empty",
        RulesBasic_1_any_src_pair_tgt_wildcard_empty
    },
    {
        "1_any_src_pair_rel_wildcard_empty",
        RulesBasic_1_any_src_pair_rel_wildcard_empty
    },
    {
        "1_any_src_any_tgt_w_rel_fixed_empty",
        RulesBasic_1_any_src_any_tgt_w_rel_fixed_empty
    },
    {
        "1_any_src_any_rel_w_tgt_fixed_empty",
        RulesBasic_1_any_src_any_rel_w_tgt_fixed_empty
    },
    {
        "1_any_src_pair_w_tag_query_empty",
        RulesBasic_1_any_src_pair_w_tag_query_empty
    },
    {
        "1_any_src_tag_w_pair_query_empty",
        RulesBasic_1_any_src_tag_w_pair_query_empty
    },
    {
        "not_any",
        RulesBasic_not_any
    },
    {
        "rule_w_iter_next",
        RulesBasic_rule_w_iter_next
    },
    {
        "empty_rule",
        RulesBasic_empty_rule
    },
    {
        "invalid_rule",
        RulesBasic_invalid_rule
    },
    {
        "not_instanced_table_src",
        RulesBasic_not_instanced_table_src
    },
    {
        "not_instanced_entity_src",
        RulesBasic_not_instanced_entity_src
    },
    {
        "not_instanced_mixed_src",
        RulesBasic_not_instanced_mixed_src
    },
    {
        "instanced_table_src",
        RulesBasic_instanced_table_src
    },
    {
        "instanced_entity_src",
        RulesBasic_instanced_entity_src
    },
    {
        "instanced_mixed_src",
        RulesBasic_instanced_mixed_src
    },
    {
        "in_term",
        RulesBasic_in_term
    },
    {
        "out_term",
        RulesBasic_out_term
    },
    {
        "inout_term",
        RulesBasic_inout_term
    },
    {
        "nodata_term",
        RulesBasic_nodata_term
    },
    {
        "find_this_lowercase",
        RulesBasic_find_this_lowercase
    },
    {
        "find_this_uppercase",
        RulesBasic_find_this_uppercase
    },
    {
        "find_this_tgt_lowercase",
        RulesBasic_find_this_tgt_lowercase
    },
    {
        "find_this_tgt_uppercase",
        RulesBasic_find_this_tgt_uppercase
    },
    {
        "get_filter",
        RulesBasic_get_filter
    },
    {
        "iter_empty_source",
        RulesBasic_iter_empty_source
    },
    {
        "iter_empty_source_2_terms",
        RulesBasic_iter_empty_source_2_terms
    },
    {
        "iter_empty_source_wildcard",
        RulesBasic_iter_empty_source_wildcard
    },
    {
        "iter_empty_source_pair",
        RulesBasic_iter_empty_source_pair
    },
    {
        "iter_empty_source_pair_wildcard",
        RulesBasic_iter_empty_source_pair_wildcard
    },
    {
        "iter_empty_source_2_terms_pair",
        RulesBasic_iter_empty_source_2_terms_pair
    },
    {
        "iter_empty_source_2_terms_mixed",
        RulesBasic_iter_empty_source_2_terms_mixed
    },
    {
        "iter_empty_source_2_terms_mixed_pair",
        RulesBasic_iter_empty_source_2_terms_mixed_pair
    },
    {
        "iter_empty_source_2_terms_mixed_pair_wildcard",
        RulesBasic_iter_empty_source_2_terms_mixed_pair_wildcard
    },
    {
        "this_var_w_empty_entity",
        RulesBasic_this_var_w_empty_entity
    },
    {
        "match_disabled",
        RulesBasic_match_disabled
    },
    {
        "match_prefab",
        RulesBasic_match_prefab
    },
    {
        "match_disabled_prefab",
        RulesBasic_match_disabled_prefab
    },
    {
        "match_disabled_this_tgt",
        RulesBasic_match_disabled_this_tgt
    },
    {
        "match_prefab_this_tgt",
        RulesBasic_match_prefab_this_tgt
    },
    {
        "match_disabled_prefab_this_tgt",
        RulesBasic_match_disabled_prefab_this_tgt
    },
    {
        "match_self_disabled",
        RulesBasic_match_self_disabled
    },
    {
        "match_self_prefab",
        RulesBasic_match_self_prefab
    },
    {
        "match_self_disabled_prefab",
        RulesBasic_match_self_disabled_prefab
    },
    {
        "inout_none_first_term",
        RulesBasic_inout_none_first_term
    },
    {
        "inout_none_second_term",
        RulesBasic_inout_none_second_term
    },
    {
        "no_data_rule",
        RulesBasic_no_data_rule
    },
    {
        "frame_offset",
        RulesBasic_frame_offset
    },
    {
        "frame_offset_no_data",
        RulesBasic_frame_offset_no_data
    },
    {
        "match_empty_tables",
        RulesBasic_match_empty_tables
    },
    {
        "match_empty_tables_no_data",
        RulesBasic_match_empty_tables_no_data
    },
    {
        "match_empty_tables_w_not",
        RulesBasic_match_empty_tables_w_not
    },
    {
        "match_empty_tables_w_wildcard",
        RulesBasic_match_empty_tables_w_wildcard
    },
    {
        "match_empty_tables_w_no_empty_tables",
        RulesBasic_match_empty_tables_w_no_empty_tables
    },
    {
        "match_empty_tables_trivial",
        RulesBasic_match_empty_tables_trivial
    },
    {
        "oneof_wildcard",
        RulesBasic_oneof_wildcard
    },
    {
        "oneof_any",
        RulesBasic_oneof_any
    },
    {
        "instanced_w_singleton",
        RulesBasic_instanced_w_singleton
    },
    {
        "instanced_w_base",
        RulesBasic_instanced_w_base
    },
    {
        "not_instanced_w_singleton",
        RulesBasic_not_instanced_w_singleton
    },
    {
        "not_instanced_w_base",
        RulesBasic_not_instanced_w_base
    },
    {
        "unknown_before_known",
        RulesBasic_unknown_before_known
    },
    {
        "unknown_before_known_after_or",
        RulesBasic_unknown_before_known_after_or
    },
    {
        "unknown_before_known_after_not",
        RulesBasic_unknown_before_known_after_not
    },
    {
        "unknown_before_known_after_optional",
        RulesBasic_unknown_before_known_after_optional
    },
    {
        "unknown_before_known_after_scope",
        RulesBasic_unknown_before_known_after_scope
    },
    {
        "reordered_plan_1",
        RulesBasic_reordered_plan_1
    },
    {
        "reordered_plan_2",
        RulesBasic_reordered_plan_2
    },
    {
        "1_trivial_plan",
        RulesBasic_1_trivial_plan
    },
    {
        "2_trivial_plan",
        RulesBasic_2_trivial_plan
    },
    {
        "1_trivial_plan_component",
        RulesBasic_1_trivial_plan_component
    },
    {
        "2_trivial_plan_component",
        RulesBasic_2_trivial_plan_component
    },
    {
        "2_trivial_plan_w_wildcard",
        RulesBasic_2_trivial_plan_w_wildcard
    },
    {
        "3_trivial_plan_w_pair",
        RulesBasic_3_trivial_plan_w_pair
    },
    {
        "3_trivial_plan_w_wildcard",
        RulesBasic_3_trivial_plan_w_wildcard
    },
    {
        "3_trivial_plan_w_any",
        RulesBasic_3_trivial_plan_w_any
    },
    {
        "3_trivial_plan_w_pair_component",
        RulesBasic_3_trivial_plan_w_pair_component
    },
    {
        "3_trivial_plan_w_wildcard_component",
        RulesBasic_3_trivial_plan_w_wildcard_component
    },
    {
        "3_trivial_plan_w_any_component",
        RulesBasic_3_trivial_plan_w_any_component
    },
    {
        "1_trivial_component_w_none",
        RulesBasic_1_trivial_component_w_none
    },
    {
        "2_trivial_component_w_none",
        RulesBasic_2_trivial_component_w_none
    },
    {
        "2_trivial_mixed_2_tables",
        RulesBasic_2_trivial_mixed_2_tables
    },
    {
        "2_trivial_mixed_2_tables_component",
        RulesBasic_2_trivial_mixed_2_tables_component
    },
    {
        "2_trivial_mixed_2_tables_wildcard",
        RulesBasic_2_trivial_mixed_2_tables_wildcard
    },
    {
        "1_plan_any_src",
        RulesBasic_1_plan_any_src
    },
    {
        "1_plan_not_any_src",
        RulesBasic_1_plan_not_any_src
    },
    {
        "1_plan_optional_any_src",
        RulesBasic_1_plan_optional_any_src
    }
};

bake_test_case RulesVariables_testcases[] = {
    {
        "1_ent_src_w_var",
        RulesVariables_1_ent_src_w_var
    },
    {
        "1_ent_src_w_pair_rel_var",
        RulesVariables_1_ent_src_w_pair_rel_var
    },
    {
        "1_ent_src_w_pair_tgt_var",
        RulesVariables_1_ent_src_w_pair_tgt_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_var",
        RulesVariables_1_ent_src_w_pair_rel_tgt_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_same_var",
        RulesVariables_1_ent_src_w_pair_rel_tgt_same_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_same_var_after_write",
        RulesVariables_1_ent_src_w_pair_rel_tgt_same_var_after_write
    },
    {
        "1_this_src_w_var",
        RulesVariables_1_this_src_w_var
    },
    {
        "1_this_src_w_pair_rel_var",
        RulesVariables_1_this_src_w_pair_rel_var
    },
    {
        "1_this_src_w_pair_tgt_var",
        RulesVariables_1_this_src_w_pair_tgt_var
    },
    {
        "1_this_src_w_pair_rel_tgt_var",
        RulesVariables_1_this_src_w_pair_rel_tgt_var
    },
    {
        "1_this_src_w_pair_rel_tgt_same_var",
        RulesVariables_1_this_src_w_pair_rel_tgt_same_var
    },
    {
        "1_this_src_w_pair_rel_tgt_same_var_after_write",
        RulesVariables_1_this_src_w_pair_rel_tgt_same_var_after_write
    },
    {
        "1_src_id_same_var",
        RulesVariables_1_src_id_same_var
    },
    {
        "1_src_pair_first_same_var",
        RulesVariables_1_src_pair_first_same_var
    },
    {
        "1_src_pair_second_same_var",
        RulesVariables_1_src_pair_second_same_var
    },
    {
        "1_src_pair_first_and_second_same_var",
        RulesVariables_1_src_pair_first_and_second_same_var
    },
    {
        "1_src_id_same_var_after_write",
        RulesVariables_1_src_id_same_var_after_write
    },
    {
        "1_src_pair_first_same_var_after_write",
        RulesVariables_1_src_pair_first_same_var_after_write
    },
    {
        "1_src_pair_second_same_var_after_write",
        RulesVariables_1_src_pair_second_same_var_after_write
    },
    {
        "1_src_pair_first_and_second_same_var_after_write",
        RulesVariables_1_src_pair_first_and_second_same_var_after_write
    },
    {
        "1_src_pair_first_same_var_this",
        RulesVariables_1_src_pair_first_same_var_this
    },
    {
        "1_src_pair_second_same_var_this",
        RulesVariables_1_src_pair_second_same_var_this
    },
    {
        "1_src_pair_first_and_second_same_var_this",
        RulesVariables_1_src_pair_first_and_second_same_var_this
    },
    {
        "1_src_id_same_var_this_after_write",
        RulesVariables_1_src_id_same_var_this_after_write
    },
    {
        "1_src_pair_first_same_var_this_after_write",
        RulesVariables_1_src_pair_first_same_var_this_after_write
    },
    {
        "1_src_pair_second_same_var_this_after_write",
        RulesVariables_1_src_pair_second_same_var_this_after_write
    },
    {
        "1_src_pair_first_and_second_same_var_this_after_write",
        RulesVariables_1_src_pair_first_and_second_same_var_this_after_write
    },
    {
        "1_ent_src_w_this_var",
        RulesVariables_1_ent_src_w_this_var
    },
    {
        "1_ent_src_w_pair_this_rel",
        RulesVariables_1_ent_src_w_pair_this_rel
    },
    {
        "1_ent_src_w_pair_this_tgt",
        RulesVariables_1_ent_src_w_pair_this_tgt
    },
    {
        "1_ent_src_w_pair_this_rel_tgt",
        RulesVariables_1_ent_src_w_pair_this_rel_tgt
    },
    {
        "1_this_src_w_this",
        RulesVariables_1_this_src_w_this
    },
    {
        "1_this_src_w_pair_this_rel_tgt",
        RulesVariables_1_this_src_w_pair_this_rel_tgt
    },
    {
        "1_this_src_w_this_after_write",
        RulesVariables_1_this_src_w_this_after_write
    },
    {
        "1_this_src_w_pair_this_rel_tgt_after_write",
        RulesVariables_1_this_src_w_pair_this_rel_tgt_after_write
    },
    {
        "2_constrain_src_from_src",
        RulesVariables_2_constrain_src_from_src
    },
    {
        "2_constrain_rel_from_src_w_ent",
        RulesVariables_2_constrain_rel_from_src_w_ent
    },
    {
        "2_constrain_rel_from_src_w_var",
        RulesVariables_2_constrain_rel_from_src_w_var
    },
    {
        "2_constrain_rel_from_src_w_this",
        RulesVariables_2_constrain_rel_from_src_w_this
    },
    {
        "2_constrain_pair_rel_from_src_w_ent",
        RulesVariables_2_constrain_pair_rel_from_src_w_ent
    },
    {
        "2_constrain_pair_rel_from_src_w_var",
        RulesVariables_2_constrain_pair_rel_from_src_w_var
    },
    {
        "2_constrain_pair_rel_from_src_w_this",
        RulesVariables_2_constrain_pair_rel_from_src_w_this
    },
    {
        "2_constrain_pair_tgt_from_src_w_ent",
        RulesVariables_2_constrain_pair_tgt_from_src_w_ent
    },
    {
        "2_constrain_pair_tgt_from_src_w_var",
        RulesVariables_2_constrain_pair_tgt_from_src_w_var
    },
    {
        "2_constrain_pair_tgt_from_src_w_this",
        RulesVariables_2_constrain_pair_tgt_from_src_w_this
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_ent",
        RulesVariables_2_constrain_pair_rel_tgt_from_src_w_ent
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_var",
        RulesVariables_2_constrain_pair_rel_tgt_from_src_w_var
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_this",
        RulesVariables_2_constrain_pair_rel_tgt_from_src_w_this
    },
    {
        "1_ent_src_set_rel_var",
        RulesVariables_1_ent_src_set_rel_var
    },
    {
        "1_ent_src_set_pair_rel_var",
        RulesVariables_1_ent_src_set_pair_rel_var
    },
    {
        "1_ent_src_set_pair_tgt_var",
        RulesVariables_1_ent_src_set_pair_tgt_var
    },
    {
        "1_set_src_var",
        RulesVariables_1_set_src_var
    },
    {
        "1_set_src_var_w_pair",
        RulesVariables_1_set_src_var_w_pair
    },
    {
        "1_set_src_var_w_pair_set_rel",
        RulesVariables_1_set_src_var_w_pair_set_rel
    },
    {
        "1_set_src_var_w_pair_set_tgt",
        RulesVariables_1_set_src_var_w_pair_set_tgt
    },
    {
        "1_set_src_var_w_pair_set_rel_tgt",
        RulesVariables_1_set_src_var_w_pair_set_rel_tgt
    },
    {
        "1_set_src_this",
        RulesVariables_1_set_src_this
    },
    {
        "1_set_src_this_w_pair",
        RulesVariables_1_set_src_this_w_pair
    },
    {
        "1_set_src_this_w_pair_set_rel",
        RulesVariables_1_set_src_this_w_pair_set_rel
    },
    {
        "1_set_src_this_w_pair_set_tgt",
        RulesVariables_1_set_src_this_w_pair_set_tgt
    },
    {
        "1_set_src_this_w_pair_set_rel_tgt",
        RulesVariables_1_set_src_this_w_pair_set_rel_tgt
    },
    {
        "1_set_src_this_to_empty_table",
        RulesVariables_1_set_src_this_to_empty_table
    },
    {
        "1_set_src_this_to_empty_table_w_component",
        RulesVariables_1_set_src_this_to_empty_table_w_component
    },
    {
        "1_set_src_this_to_empty_table_w_component_self",
        RulesVariables_1_set_src_this_to_empty_table_w_component_self
    },
    {
        "1_set_src_this_to_entiy_in_table",
        RulesVariables_1_set_src_this_to_entiy_in_table
    },
    {
        "1_set_src_this_to_entiy_in_table_self",
        RulesVariables_1_set_src_this_to_entiy_in_table_self
    },
    {
        "2_set_src_this",
        RulesVariables_2_set_src_this
    },
    {
        "2_set_src_this_self",
        RulesVariables_2_set_src_this_self
    },
    {
        "2_set_src_this_component",
        RulesVariables_2_set_src_this_component
    },
    {
        "2_set_src_this_self_component",
        RulesVariables_2_set_src_this_self_component
    },
    {
        "2_set_src_this_w_up",
        RulesVariables_2_set_src_this_w_up
    },
    {
        "2_set_src_this_self_w_up",
        RulesVariables_2_set_src_this_self_w_up
    },
    {
        "2_set_src_this_component_w_up",
        RulesVariables_2_set_src_this_component_w_up
    },
    {
        "2_set_src_this_self_component_w_up",
        RulesVariables_2_set_src_this_self_component_w_up
    },
    {
        "2_set_src_this_w_exclusive_wildcard",
        RulesVariables_2_set_src_this_w_exclusive_wildcard
    },
    {
        "2_set_src_this_self_w_exclusive_wildcard",
        RulesVariables_2_set_src_this_self_w_exclusive_wildcard
    },
    {
        "1_src_this_var_as_entity",
        RulesVariables_1_src_this_var_as_entity
    },
    {
        "1_src_this_var_as_table",
        RulesVariables_1_src_this_var_as_table
    },
    {
        "1_src_this_var_as_table_range",
        RulesVariables_1_src_this_var_as_table_range
    },
    {
        "2_join_by_rel_var",
        RulesVariables_2_join_by_rel_var
    },
    {
        "2_join_by_pair_rel_var",
        RulesVariables_2_join_by_pair_rel_var
    },
    {
        "2_join_by_pair_tgt_var",
        RulesVariables_2_join_by_pair_tgt_var
    },
    {
        "2_cycle_w_var",
        RulesVariables_2_cycle_w_var
    },
    {
        "2_cycle_w_this_var",
        RulesVariables_2_cycle_w_this_var
    },
    {
        "2_cycle_w_var_this",
        RulesVariables_2_cycle_w_var_this
    },
    {
        "2_cycle_pair_w_var",
        RulesVariables_2_cycle_pair_w_var
    },
    {
        "2_cycle_pair_w_this_var_var",
        RulesVariables_2_cycle_pair_w_this_var_var
    },
    {
        "2_cycle_pair_w_var_this_var",
        RulesVariables_2_cycle_pair_w_var_this_var
    },
    {
        "2_cycle_pair_w_var_var_this",
        RulesVariables_2_cycle_pair_w_var_var_this
    },
    {
        "2_cycle_pair_ent_var_var",
        RulesVariables_2_cycle_pair_ent_var_var
    },
    {
        "2_cycle_pair_ent_this_var",
        RulesVariables_2_cycle_pair_ent_this_var
    },
    {
        "2_cycle_pair_ent_var_this",
        RulesVariables_2_cycle_pair_ent_var_this
    },
    {
        "parse_0_var",
        RulesVariables_parse_0_var
    },
    {
        "parse_1_var",
        RulesVariables_parse_1_var
    },
    {
        "parse_2_vars",
        RulesVariables_parse_2_vars
    },
    {
        "parse_0_var_paren",
        RulesVariables_parse_0_var_paren
    },
    {
        "parse_1_var_paren",
        RulesVariables_parse_1_var_paren
    },
    {
        "parse_2_vars_paren",
        RulesVariables_parse_2_vars_paren
    },
    {
        "parse_1_vars_w_path",
        RulesVariables_parse_1_vars_w_path
    },
    {
        "parse_missing_close_paren",
        RulesVariables_parse_missing_close_paren
    },
    {
        "parse_missing_open_paren",
        RulesVariables_parse_missing_open_paren
    },
    {
        "parse_missing_value",
        RulesVariables_parse_missing_value
    },
    {
        "parse_0_var_w_spaces",
        RulesVariables_parse_0_var_w_spaces
    },
    {
        "parse_1_var_w_spaces",
        RulesVariables_parse_1_var_w_spaces
    },
    {
        "parse_2_vars_w_spaces",
        RulesVariables_parse_2_vars_w_spaces
    },
    {
        "parse_0_var_paren_w_spaces",
        RulesVariables_parse_0_var_paren_w_spaces
    },
    {
        "parse_1_var_paren_w_spaces",
        RulesVariables_parse_1_var_paren_w_spaces
    },
    {
        "parse_2_vars_paren_w_spaces",
        RulesVariables_parse_2_vars_paren_w_spaces
    },
    {
        "var_count",
        RulesVariables_var_count
    },
    {
        "var_name",
        RulesVariables_var_name
    },
    {
        "var_is_entity",
        RulesVariables_var_is_entity
    },
    {
        "no_this_anonymous_src",
        RulesVariables_no_this_anonymous_src
    },
    {
        "no_this_anonymous_src_w_pair",
        RulesVariables_no_this_anonymous_src_w_pair
    },
    {
        "no_this_anonymous_component_src",
        RulesVariables_no_this_anonymous_component_src
    },
    {
        "no_this_anonymous_component_src_w_pair",
        RulesVariables_no_this_anonymous_component_src_w_pair
    },
    {
        "lookup_from_table_this",
        RulesVariables_lookup_from_table_this
    },
    {
        "lookup_from_entity_this",
        RulesVariables_lookup_from_entity_this
    },
    {
        "lookup_from_table",
        RulesVariables_lookup_from_table
    },
    {
        "lookup_from_entity",
        RulesVariables_lookup_from_entity
    },
    {
        "lookup_from_not_written",
        RulesVariables_lookup_from_not_written
    },
    {
        "lookup_from_table_this_component",
        RulesVariables_lookup_from_table_this_component
    },
    {
        "lookup_from_entity_this_component",
        RulesVariables_lookup_from_entity_this_component
    },
    {
        "lookup_from_table_component",
        RulesVariables_lookup_from_table_component
    },
    {
        "lookup_from_entity_component",
        RulesVariables_lookup_from_entity_component
    },
    {
        "lookup_from_table_two_children",
        RulesVariables_lookup_from_table_two_children
    },
    {
        "lookup_from_entity_two_children",
        RulesVariables_lookup_from_entity_two_children
    },
    {
        "lookup_from_table_same_child_twice",
        RulesVariables_lookup_from_table_same_child_twice
    },
    {
        "lookup_from_entity_same_child_twice",
        RulesVariables_lookup_from_entity_same_child_twice
    },
    {
        "lookup_from_table_not",
        RulesVariables_lookup_from_table_not
    },
    {
        "lookup_from_entity_not",
        RulesVariables_lookup_from_entity_not
    },
    {
        "lookup_from_table_w_any_component",
        RulesVariables_lookup_from_table_w_any_component
    },
    {
        "lookup_from_entity_w_any_component",
        RulesVariables_lookup_from_entity_w_any_component
    },
    {
        "lookup_as_tag",
        RulesVariables_lookup_as_tag
    },
    {
        "lookup_as_relationship",
        RulesVariables_lookup_as_relationship
    },
    {
        "lookup_as_target",
        RulesVariables_lookup_as_target
    },
    {
        "lookup_assign_var",
        RulesVariables_lookup_assign_var
    },
    {
        "lookup_eq_var",
        RulesVariables_lookup_eq_var
    },
    {
        "lookup_neq_var",
        RulesVariables_lookup_neq_var
    },
    {
        "lookup_unresolved_dependent",
        RulesVariables_lookup_unresolved_dependent
    },
    {
        "check_vars_this",
        RulesVariables_check_vars_this
    },
    {
        "check_vars_var",
        RulesVariables_check_vars_var
    },
    {
        "check_vars_wildcard",
        RulesVariables_check_vars_wildcard
    },
    {
        "check_vars_any",
        RulesVariables_check_vars_any
    },
    {
        "check_vars_var_as_tgt",
        RulesVariables_check_vars_var_as_tgt
    },
    {
        "check_vars_this_as_tgt",
        RulesVariables_check_vars_this_as_tgt
    },
    {
        "check_vars_anonymous_var_as_tgt",
        RulesVariables_check_vars_anonymous_var_as_tgt
    },
    {
        "check_vars_wildcard_as_tgt",
        RulesVariables_check_vars_wildcard_as_tgt
    },
    {
        "check_vars_any_as_tgt",
        RulesVariables_check_vars_any_as_tgt
    },
    {
        "check_vars_this_w_lookup_var",
        RulesVariables_check_vars_this_w_lookup_var
    },
    {
        "check_vars_var_w_lookup_var",
        RulesVariables_check_vars_var_w_lookup_var
    },
    {
        "1_trivial_1_var",
        RulesVariables_1_trivial_1_var
    },
    {
        "2_trivial_1_var",
        RulesVariables_2_trivial_1_var
    },
    {
        "1_trivial_1_var_component",
        RulesVariables_1_trivial_1_var_component
    },
    {
        "2_trivial_1_var_component",
        RulesVariables_2_trivial_1_var_component
    },
    {
        "1_trivial_1_wildcard",
        RulesVariables_1_trivial_1_wildcard
    },
    {
        "2_trivial_1_wildcard",
        RulesVariables_2_trivial_1_wildcard
    },
    {
        "1_trivial_1_wildcard_component",
        RulesVariables_1_trivial_1_wildcard_component
    },
    {
        "2_trivial_1_wildcard_component",
        RulesVariables_2_trivial_1_wildcard_component
    },
    {
        "1_trivial_1_any",
        RulesVariables_1_trivial_1_any
    },
    {
        "2_trivial_1_any",
        RulesVariables_2_trivial_1_any
    },
    {
        "1_trivial_1_any_component",
        RulesVariables_1_trivial_1_any_component
    },
    {
        "2_trivial_1_any_component",
        RulesVariables_2_trivial_1_any_component
    }
};

bake_test_case RulesOperators_testcases[] = {
    {
        "2_and_not",
        RulesOperators_2_and_not
    },
    {
        "3_and_not_not",
        RulesOperators_3_and_not_not
    },
    {
        "2_and_not_pair_rel_wildcard",
        RulesOperators_2_and_not_pair_rel_wildcard
    },
    {
        "2_and_not_pair_tgt_wildcard",
        RulesOperators_2_and_not_pair_tgt_wildcard
    },
    {
        "2_and_not_pair_rel_tgt_wildcard",
        RulesOperators_2_and_not_pair_rel_tgt_wildcard
    },
    {
        "2_and_not_pair_rel_var",
        RulesOperators_2_and_not_pair_rel_var
    },
    {
        "2_and_not_pair_tgt_var",
        RulesOperators_2_and_not_pair_tgt_var
    },
    {
        "2_and_not_pair_rel_tgt_var",
        RulesOperators_2_and_not_pair_rel_tgt_var
    },
    {
        "2_and_not_pair_rel_tgt_same_var",
        RulesOperators_2_and_not_pair_rel_tgt_same_var
    },
    {
        "2_and_not_pair_rel_var_written",
        RulesOperators_2_and_not_pair_rel_var_written
    },
    {
        "2_and_not_pair_tgt_var_written",
        RulesOperators_2_and_not_pair_tgt_var_written
    },
    {
        "2_and_not_pair_rel_tgt_var_written",
        RulesOperators_2_and_not_pair_rel_tgt_var_written
    },
    {
        "2_and_not_pair_rel_tgt_same_var_written",
        RulesOperators_2_and_not_pair_rel_tgt_same_var_written
    },
    {
        "2_and_not_pair_rel_src_tgt_same_var_written",
        RulesOperators_2_and_not_pair_rel_src_tgt_same_var_written
    },
    {
        "2_and_not_pair_any_rel",
        RulesOperators_2_and_not_pair_any_rel
    },
    {
        "2_and_not_pair_any_tgt",
        RulesOperators_2_and_not_pair_any_tgt
    },
    {
        "2_and_not_pair_any_src",
        RulesOperators_2_and_not_pair_any_src
    },
    {
        "1_not_any_src_fixed_first",
        RulesOperators_1_not_any_src_fixed_first
    },
    {
        "1_not_any_src_any_tgt_fixed_first",
        RulesOperators_1_not_any_src_any_tgt_fixed_first
    },
    {
        "1_not_any_src_any_first_fixed_tgt",
        RulesOperators_1_not_any_src_any_first_fixed_tgt
    },
    {
        "1_not_any_src_any_childof_pair_any_tgt",
        RulesOperators_1_not_any_src_any_childof_pair_any_tgt
    },
    {
        "1_not_any_src_any_isa_pair_any_tgt",
        RulesOperators_1_not_any_src_any_isa_pair_any_tgt
    },
    {
        "2_and_optional",
        RulesOperators_2_and_optional
    },
    {
        "3_and_optional_optional",
        RulesOperators_3_and_optional_optional
    },
    {
        "2_and_optional_pair_rel_wildcard",
        RulesOperators_2_and_optional_pair_rel_wildcard
    },
    {
        "2_and_optional_pair_tgt_wildcard",
        RulesOperators_2_and_optional_pair_tgt_wildcard
    },
    {
        "2_and_optional_pair_rel_var",
        RulesOperators_2_and_optional_pair_rel_var
    },
    {
        "2_and_optional_pair_tgt_var",
        RulesOperators_2_and_optional_pair_tgt_var
    },
    {
        "2_and_optional_pair_rel_tgt_var",
        RulesOperators_2_and_optional_pair_rel_tgt_var
    },
    {
        "2_and_optional_pair_rel_tgt_same_var",
        RulesOperators_2_and_optional_pair_rel_tgt_same_var
    },
    {
        "2_and_optional_pair_rel_var_written",
        RulesOperators_2_and_optional_pair_rel_var_written
    },
    {
        "2_and_optional_pair_tgt_var_written",
        RulesOperators_2_and_optional_pair_tgt_var_written
    },
    {
        "2_and_optional_pair_rel_tgt_var_written",
        RulesOperators_2_and_optional_pair_rel_tgt_var_written
    },
    {
        "2_and_optional_pair_rel_tgt_same_var_written",
        RulesOperators_2_and_optional_pair_rel_tgt_same_var_written
    },
    {
        "2_and_optional_pair_rel_src_tgt_same_var_written",
        RulesOperators_2_and_optional_pair_rel_src_tgt_same_var_written
    },
    {
        "3_and_optional_optional_pair_w_var",
        RulesOperators_3_and_optional_optional_pair_w_var
    },
    {
        "2_and_optional_pair_any_rel",
        RulesOperators_2_and_optional_pair_any_rel
    },
    {
        "2_and_optional_pair_any_tgt",
        RulesOperators_2_and_optional_pair_any_tgt
    },
    {
        "2_and_optional_pair_any_src",
        RulesOperators_2_and_optional_pair_any_src
    },
    {
        "3_and_optional_dependent_and_pair_rel",
        RulesOperators_3_and_optional_dependent_and_pair_rel
    },
    {
        "3_and_optional_dependent_and_pair_tgt",
        RulesOperators_3_and_optional_dependent_and_pair_tgt
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt",
        RulesOperators_3_and_optional_dependent_and_pair_rel_tgt
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt_same_var",
        RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_var
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt_same_other_var",
        RulesOperators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var
    },
    {
        "3_and_optional_dependent_and_pair_src",
        RulesOperators_3_and_optional_dependent_and_pair_src
    },
    {
        "3_and_optional_dependent_optional_pair_rel",
        RulesOperators_3_and_optional_dependent_optional_pair_rel
    },
    {
        "3_and_optional_dependent_optional_pair_tgt",
        RulesOperators_3_and_optional_dependent_optional_pair_tgt
    },
    {
        "3_and_optional_dependent_optional_pair_src",
        RulesOperators_3_and_optional_dependent_optional_pair_src
    },
    {
        "3_and_optional_dependent_not_pair_rel",
        RulesOperators_3_and_optional_dependent_not_pair_rel
    },
    {
        "3_and_optional_dependent_not_pair_tgt",
        RulesOperators_3_and_optional_dependent_not_pair_tgt
    },
    {
        "3_and_optional_dependent_not_pair_src",
        RulesOperators_3_and_optional_dependent_not_pair_src
    },
    {
        "1_optional_any_src_fixed_first",
        RulesOperators_1_optional_any_src_fixed_first
    },
    {
        "1_optional_any_src_any_tgt_fixed_first",
        RulesOperators_1_optional_any_src_any_tgt_fixed_first
    },
    {
        "1_optional_any_src_any_first_fixed_tgt",
        RulesOperators_1_optional_any_src_any_first_fixed_tgt
    },
    {
        "1_optional_any_src_any_childof_pair_any_tgt",
        RulesOperators_1_optional_any_src_any_childof_pair_any_tgt
    },
    {
        "1_optional_any_src_any_isa_pair_any_tgt",
        RulesOperators_1_optional_any_src_any_isa_pair_any_tgt
    },
    {
        "2_or",
        RulesOperators_2_or
    },
    {
        "3_or",
        RulesOperators_3_or
    },
    {
        "2_or_written",
        RulesOperators_2_or_written
    },
    {
        "3_or_written",
        RulesOperators_3_or_written
    },
    {
        "2_or_written_w_rel_var",
        RulesOperators_2_or_written_w_rel_var
    },
    {
        "3_or_written_w_rel_var",
        RulesOperators_3_or_written_w_rel_var
    },
    {
        "2_or_written_w_tgt_var",
        RulesOperators_2_or_written_w_tgt_var
    },
    {
        "2_or_written_w_rel_tgt_var",
        RulesOperators_2_or_written_w_rel_tgt_var
    },
    {
        "2_or_written_w_rel_tgt_same_var",
        RulesOperators_2_or_written_w_rel_tgt_same_var
    },
    {
        "3_or_written_w_tgt_var",
        RulesOperators_3_or_written_w_tgt_var
    },
    {
        "2_or_chains",
        RulesOperators_2_or_chains
    },
    {
        "2_or_chains_written",
        RulesOperators_2_or_chains_written
    },
    {
        "2_or_dependent",
        RulesOperators_2_or_dependent
    },
    {
        "2_or_dependent_reverse",
        RulesOperators_2_or_dependent_reverse
    },
    {
        "2_or_dependent_2_vars",
        RulesOperators_2_or_dependent_2_vars
    },
    {
        "2_or_written_dependent",
        RulesOperators_2_or_written_dependent
    },
    {
        "2_or_written_dependent_2_vars",
        RulesOperators_2_or_written_dependent_2_vars
    },
    {
        "2_or_w_dependent",
        RulesOperators_2_or_w_dependent
    },
    {
        "2_or_w_both",
        RulesOperators_2_or_w_both
    },
    {
        "3_or_w_both",
        RulesOperators_3_or_w_both
    },
    {
        "2_not_first",
        RulesOperators_2_not_first
    },
    {
        "2_optional_first",
        RulesOperators_2_optional_first
    },
    {
        "root_entities_empty",
        RulesOperators_root_entities_empty
    },
    {
        "root_entities",
        RulesOperators_root_entities
    },
    {
        "root_entities_w_children",
        RulesOperators_root_entities_w_children
    },
    {
        "root_entities_w_optional_children",
        RulesOperators_root_entities_w_optional_children
    },
    {
        "core_entities_w_optional_children",
        RulesOperators_core_entities_w_optional_children
    },
    {
        "root_entities_w_not_children",
        RulesOperators_root_entities_w_not_children
    },
    {
        "core_entities_w_not_children",
        RulesOperators_core_entities_w_not_children
    },
    {
        "1_ent_src_not",
        RulesOperators_1_ent_src_not
    },
    {
        "1_ent_src_not_pair",
        RulesOperators_1_ent_src_not_pair
    },
    {
        "1_ent_src_not_pair_rel_wildcard",
        RulesOperators_1_ent_src_not_pair_rel_wildcard
    },
    {
        "1_ent_src_not_pair_tgt_wildcard",
        RulesOperators_1_ent_src_not_pair_tgt_wildcard
    },
    {
        "1_ent_src_not_pair_rel_tgt_wildcard",
        RulesOperators_1_ent_src_not_pair_rel_tgt_wildcard
    },
    {
        "1_ent_src_not_pair_rel_any",
        RulesOperators_1_ent_src_not_pair_rel_any
    },
    {
        "1_ent_src_not_pair_tgt_any",
        RulesOperators_1_ent_src_not_pair_tgt_any
    },
    {
        "1_ent_src_not_pair_rel_tgt_any",
        RulesOperators_1_ent_src_not_pair_rel_tgt_any
    },
    {
        "1_ent_src_not_pair_rel_var",
        RulesOperators_1_ent_src_not_pair_rel_var
    },
    {
        "1_ent_src_not_pair_tgt_var",
        RulesOperators_1_ent_src_not_pair_tgt_var
    },
    {
        "1_ent_src_not_pair_rel_tgt_var",
        RulesOperators_1_ent_src_not_pair_rel_tgt_var
    },
    {
        "1_ent_src_not_pair_rel_tgt_same_var",
        RulesOperators_1_ent_src_not_pair_rel_tgt_same_var
    },
    {
        "1_this_src_not_pair_rel_var",
        RulesOperators_1_this_src_not_pair_rel_var
    },
    {
        "1_this_src_not_pair_tgt_var",
        RulesOperators_1_this_src_not_pair_tgt_var
    },
    {
        "1_this_src_not_pair_rel_tgt_var",
        RulesOperators_1_this_src_not_pair_rel_tgt_var
    },
    {
        "1_this_src_not_pair_rel_tgt_same_var",
        RulesOperators_1_this_src_not_pair_rel_tgt_same_var
    },
    {
        "1_ent_src_not_pair_rel_var_written",
        RulesOperators_1_ent_src_not_pair_rel_var_written
    },
    {
        "1_ent_src_not_pair_tgt_var_written",
        RulesOperators_1_ent_src_not_pair_tgt_var_written
    },
    {
        "1_ent_src_not_pair_rel_tgt_var_written",
        RulesOperators_1_ent_src_not_pair_rel_tgt_var_written
    },
    {
        "1_ent_src_not_pair_rel_tgt_same_var_written",
        RulesOperators_1_ent_src_not_pair_rel_tgt_same_var_written
    }
};

bake_test_case RulesTransitive_testcases[] = {
    {
        "1_fact_0_lvl_true",
        RulesTransitive_1_fact_0_lvl_true
    },
    {
        "1_fact_1_lvl_true",
        RulesTransitive_1_fact_1_lvl_true
    },
    {
        "1_fact_2_lvl_true",
        RulesTransitive_1_fact_2_lvl_true
    },
    {
        "1_fact_0_lvl_false",
        RulesTransitive_1_fact_0_lvl_false
    },
    {
        "1_fact_1_lvl_false",
        RulesTransitive_1_fact_1_lvl_false
    },
    {
        "1_fact_2_lvl_false",
        RulesTransitive_1_fact_2_lvl_false
    },
    {
        "1_fact_reflexive",
        RulesTransitive_1_fact_reflexive
    },
    {
        "1_this_src_written_0_lvl",
        RulesTransitive_1_this_src_written_0_lvl
    },
    {
        "1_this_src_written_1_lvl",
        RulesTransitive_1_this_src_written_1_lvl
    },
    {
        "1_this_src_written_2_lvl",
        RulesTransitive_1_this_src_written_2_lvl
    },
    {
        "1_this_src_written_reflexive",
        RulesTransitive_1_this_src_written_reflexive
    },
    {
        "1_this_src_0_lvl",
        RulesTransitive_1_this_src_0_lvl
    },
    {
        "1_this_src_1_lvl",
        RulesTransitive_1_this_src_1_lvl
    },
    {
        "1_this_src_2_lvl",
        RulesTransitive_1_this_src_2_lvl
    },
    {
        "1_this_src_reflexive",
        RulesTransitive_1_this_src_reflexive
    },
    {
        "1_ent_src_tgt_var_0_lvl",
        RulesTransitive_1_ent_src_tgt_var_0_lvl
    },
    {
        "1_ent_src_tgt_var_1_lvl",
        RulesTransitive_1_ent_src_tgt_var_1_lvl
    },
    {
        "1_ent_src_tgt_var_2_lvl",
        RulesTransitive_1_ent_src_tgt_var_2_lvl
    },
    {
        "1_ent_src_tgt_var_reflexive",
        RulesTransitive_1_ent_src_tgt_var_reflexive
    },
    {
        "1_this_src_tgt_var",
        RulesTransitive_1_this_src_tgt_var
    },
    {
        "1_this_src_tgt_var_reflexive",
        RulesTransitive_1_this_src_tgt_var_reflexive
    },
    {
        "1_var_src_written_0_lvl",
        RulesTransitive_1_var_src_written_0_lvl
    },
    {
        "1_var_src_written_1_lvl",
        RulesTransitive_1_var_src_written_1_lvl
    },
    {
        "1_var_src_written_2_lvl",
        RulesTransitive_1_var_src_written_2_lvl
    },
    {
        "1_var_src_written_reflexive",
        RulesTransitive_1_var_src_written_reflexive
    },
    {
        "1_var_src_0_lvl",
        RulesTransitive_1_var_src_0_lvl
    },
    {
        "1_var_src_1_lvl",
        RulesTransitive_1_var_src_1_lvl
    },
    {
        "1_var_src_2_lvl",
        RulesTransitive_1_var_src_2_lvl
    },
    {
        "1_var_src_reflexive",
        RulesTransitive_1_var_src_reflexive
    },
    {
        "1_var_src_tgt_var",
        RulesTransitive_1_var_src_tgt_var
    },
    {
        "1_var_src_tgt_var_reflexive",
        RulesTransitive_1_var_src_tgt_var_reflexive
    },
    {
        "1_ent_src_tgt_this_0_lvl",
        RulesTransitive_1_ent_src_tgt_this_0_lvl
    },
    {
        "1_ent_src_tgt_this_1_lvl",
        RulesTransitive_1_ent_src_tgt_this_1_lvl
    },
    {
        "1_ent_src_tgt_this_2_lvl",
        RulesTransitive_1_ent_src_tgt_this_2_lvl
    },
    {
        "1_ent_src_tgt_this_reflexive",
        RulesTransitive_1_ent_src_tgt_this_reflexive
    },
    {
        "1_var_src_tgt_this",
        RulesTransitive_1_var_src_tgt_this
    },
    {
        "1_var_src_tgt_this_reflexive",
        RulesTransitive_1_var_src_tgt_this_reflexive
    },
    {
        "2_ent_src_constrain_tgt_var_before_0_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_before_0_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_before_1_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_before_1_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_before_2_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_before_2_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_0_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_after_0_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_1_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_after_1_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_2_lvl",
        RulesTransitive_2_ent_src_constrain_tgt_var_after_2_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_0_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_before_0_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_1_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_before_1_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_2_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_before_2_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_0_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_after_0_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_1_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_after_1_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_2_lvl",
        RulesTransitive_2_this_src_constrain_tgt_var_after_2_lvl
    },
    {
        "1_src_tgt_same_var",
        RulesTransitive_1_src_tgt_same_var
    },
    {
        "1_src_tgt_same_var_reflexive",
        RulesTransitive_1_src_tgt_same_var_reflexive
    },
    {
        "1_src_tgt_same_this_var_reflexive",
        RulesTransitive_1_src_tgt_same_this_var_reflexive
    },
    {
        "1_any_src_tgt_var",
        RulesTransitive_1_any_src_tgt_var
    },
    {
        "not_transitive_ent_tgt",
        RulesTransitive_not_transitive_ent_tgt
    },
    {
        "not_transitive_var_tgt",
        RulesTransitive_not_transitive_var_tgt
    },
    {
        "not_transitive_ent_tgt_written",
        RulesTransitive_not_transitive_ent_tgt_written
    },
    {
        "not_transitive_var_tgt_written",
        RulesTransitive_not_transitive_var_tgt_written
    },
    {
        "optional_transitive_ent_tgt",
        RulesTransitive_optional_transitive_ent_tgt
    },
    {
        "optional_transitive_var_tgt",
        RulesTransitive_optional_transitive_var_tgt
    },
    {
        "optional_transitive_ent_tgt_written",
        RulesTransitive_optional_transitive_ent_tgt_written
    },
    {
        "optional_transitive_var_tgt_written",
        RulesTransitive_optional_transitive_var_tgt_written
    },
    {
        "2_var_src_w_same_tgt_ent",
        RulesTransitive_2_var_src_w_same_tgt_ent
    },
    {
        "self_target",
        RulesTransitive_self_target
    },
    {
        "any_target",
        RulesTransitive_any_target
    }
};

bake_test_case RulesComponentInheritance_testcases[] = {
    {
        "1_ent_0_lvl",
        RulesComponentInheritance_1_ent_0_lvl
    },
    {
        "1_ent_1_lvl",
        RulesComponentInheritance_1_ent_1_lvl
    },
    {
        "1_ent_2_lvl",
        RulesComponentInheritance_1_ent_2_lvl
    },
    {
        "1_ent_3_lvl",
        RulesComponentInheritance_1_ent_3_lvl
    },
    {
        "1_this_0_lvl",
        RulesComponentInheritance_1_this_0_lvl
    },
    {
        "1_this_1_lvl",
        RulesComponentInheritance_1_this_1_lvl
    },
    {
        "1_this_2_lvl",
        RulesComponentInheritance_1_this_2_lvl
    },
    {
        "1_this_3_lvl",
        RulesComponentInheritance_1_this_3_lvl
    },
    {
        "1_this_0_lvl_written",
        RulesComponentInheritance_1_this_0_lvl_written
    },
    {
        "1_this_1_lvl_written",
        RulesComponentInheritance_1_this_1_lvl_written
    },
    {
        "1_this_2_lvl_written",
        RulesComponentInheritance_1_this_2_lvl_written
    },
    {
        "1_this_3_lvl_written",
        RulesComponentInheritance_1_this_3_lvl_written
    },
    {
        "1_var_0_lvl",
        RulesComponentInheritance_1_var_0_lvl
    },
    {
        "1_var_1_lvl",
        RulesComponentInheritance_1_var_1_lvl
    },
    {
        "1_var_2_lvl",
        RulesComponentInheritance_1_var_2_lvl
    },
    {
        "1_var_3_lvl",
        RulesComponentInheritance_1_var_3_lvl
    },
    {
        "1_var_0_lvl_written",
        RulesComponentInheritance_1_var_0_lvl_written
    },
    {
        "1_var_1_lvl_written",
        RulesComponentInheritance_1_var_1_lvl_written
    },
    {
        "1_var_2_lvl_written",
        RulesComponentInheritance_1_var_2_lvl_written
    },
    {
        "1_var_3_lvl_written",
        RulesComponentInheritance_1_var_3_lvl_written
    },
    {
        "1_ent_1_lvl_self",
        RulesComponentInheritance_1_ent_1_lvl_self
    },
    {
        "1_this_1_lvl_self",
        RulesComponentInheritance_1_this_1_lvl_self
    },
    {
        "1_this_1_lvl_written_self",
        RulesComponentInheritance_1_this_1_lvl_written_self
    },
    {
        "1_var_1_lvl_self",
        RulesComponentInheritance_1_var_1_lvl_self
    },
    {
        "1_var_1_lvl_written_self",
        RulesComponentInheritance_1_var_1_lvl_written_self
    },
    {
        "1_ent_src_not",
        RulesComponentInheritance_1_ent_src_not
    },
    {
        "1_this_src_not",
        RulesComponentInheritance_1_this_src_not
    },
    {
        "1_var_src_not",
        RulesComponentInheritance_1_var_src_not
    },
    {
        "1_this_src_not_written",
        RulesComponentInheritance_1_this_src_not_written
    },
    {
        "1_var_src_not_written",
        RulesComponentInheritance_1_var_src_not_written
    },
    {
        "first_self",
        RulesComponentInheritance_first_self
    },
    {
        "first_down",
        RulesComponentInheritance_first_down
    },
    {
        "first_self_down",
        RulesComponentInheritance_first_self_down
    },
    {
        "first_rel_self",
        RulesComponentInheritance_first_rel_self
    },
    {
        "first_rel_down",
        RulesComponentInheritance_first_rel_down
    },
    {
        "first_rel_self_down",
        RulesComponentInheritance_first_rel_self_down
    }
};

bake_test_case RulesRecycled_testcases[] = {
    {
        "recycled_vars",
        RulesRecycled_recycled_vars
    },
    {
        "recycled_pair_vars",
        RulesRecycled_recycled_pair_vars
    },
    {
        "recycled_this_ent_var",
        RulesRecycled_recycled_this_ent_var
    },
    {
        "has_recycled_id_from_pair",
        RulesRecycled_has_recycled_id_from_pair
    }
};

bake_test_case RulesBuiltinPredicates_testcases[] = {
    {
        "this_eq_id",
        RulesBuiltinPredicates_this_eq_id
    },
    {
        "this_eq_name",
        RulesBuiltinPredicates_this_eq_name
    },
    {
        "this_eq_var",
        RulesBuiltinPredicates_this_eq_var
    },
    {
        "this_eq_id_written",
        RulesBuiltinPredicates_this_eq_id_written
    },
    {
        "this_eq_id_written_no_match",
        RulesBuiltinPredicates_this_eq_id_written_no_match
    },
    {
        "this_eq_name_written",
        RulesBuiltinPredicates_this_eq_name_written
    },
    {
        "this_eq_name_written_no_match",
        RulesBuiltinPredicates_this_eq_name_written_no_match
    },
    {
        "this_eq_var_written",
        RulesBuiltinPredicates_this_eq_var_written
    },
    {
        "var_eq_id",
        RulesBuiltinPredicates_var_eq_id
    },
    {
        "var_eq_name",
        RulesBuiltinPredicates_var_eq_name
    },
    {
        "var_eq_var",
        RulesBuiltinPredicates_var_eq_var
    },
    {
        "var_eq_id_written",
        RulesBuiltinPredicates_var_eq_id_written
    },
    {
        "var_eq_id_written_no_match",
        RulesBuiltinPredicates_var_eq_id_written_no_match
    },
    {
        "var_eq_name_written",
        RulesBuiltinPredicates_var_eq_name_written
    },
    {
        "var_eq_name_written_no_match",
        RulesBuiltinPredicates_var_eq_name_written_no_match
    },
    {
        "var_eq_var_written",
        RulesBuiltinPredicates_var_eq_var_written
    },
    {
        "var_eq_this",
        RulesBuiltinPredicates_var_eq_this
    },
    {
        "this_neq_id",
        RulesBuiltinPredicates_this_neq_id
    },
    {
        "this_neq_name",
        RulesBuiltinPredicates_this_neq_name
    },
    {
        "this_neq_var",
        RulesBuiltinPredicates_this_neq_var
    },
    {
        "this_neq_id_written",
        RulesBuiltinPredicates_this_neq_id_written
    },
    {
        "this_neq_id_written_no_match",
        RulesBuiltinPredicates_this_neq_id_written_no_match
    },
    {
        "this_neq_name_written",
        RulesBuiltinPredicates_this_neq_name_written
    },
    {
        "this_neq_name_written_no_match",
        RulesBuiltinPredicates_this_neq_name_written_no_match
    },
    {
        "this_neq_var_written",
        RulesBuiltinPredicates_this_neq_var_written
    },
    {
        "var_neq_id",
        RulesBuiltinPredicates_var_neq_id
    },
    {
        "var_neq_name",
        RulesBuiltinPredicates_var_neq_name
    },
    {
        "var_neq_var",
        RulesBuiltinPredicates_var_neq_var
    },
    {
        "var_neq_id_written",
        RulesBuiltinPredicates_var_neq_id_written
    },
    {
        "var_neq_id_written_no_match",
        RulesBuiltinPredicates_var_neq_id_written_no_match
    },
    {
        "var_neq_name_written",
        RulesBuiltinPredicates_var_neq_name_written
    },
    {
        "var_neq_name_written_no_match",
        RulesBuiltinPredicates_var_neq_name_written_no_match
    },
    {
        "var_neq_var_written",
        RulesBuiltinPredicates_var_neq_var_written
    },
    {
        "var_neq_this",
        RulesBuiltinPredicates_var_neq_this
    },
    {
        "this_2_neq_id",
        RulesBuiltinPredicates_this_2_neq_id
    },
    {
        "this_2_neq_name",
        RulesBuiltinPredicates_this_2_neq_name
    },
    {
        "var_2_neq_id",
        RulesBuiltinPredicates_var_2_neq_id
    },
    {
        "var_2_neq_name",
        RulesBuiltinPredicates_var_2_neq_name
    },
    {
        "this_2_neq_id_written",
        RulesBuiltinPredicates_this_2_neq_id_written
    },
    {
        "this_2_neq_name_written",
        RulesBuiltinPredicates_this_2_neq_name_written
    },
    {
        "var_2_neq_id_written",
        RulesBuiltinPredicates_var_2_neq_id_written
    },
    {
        "var_2_neq_name_written",
        RulesBuiltinPredicates_var_2_neq_name_written
    },
    {
        "this_2_or_id",
        RulesBuiltinPredicates_this_2_or_id
    },
    {
        "this_2_or_name",
        RulesBuiltinPredicates_this_2_or_name
    },
    {
        "var_2_or_id",
        RulesBuiltinPredicates_var_2_or_id
    },
    {
        "var_2_or_name",
        RulesBuiltinPredicates_var_2_or_name
    },
    {
        "this_2_or_id_written",
        RulesBuiltinPredicates_this_2_or_id_written
    },
    {
        "this_2_or_name_written",
        RulesBuiltinPredicates_this_2_or_name_written
    },
    {
        "var_2_or_id_written",
        RulesBuiltinPredicates_var_2_or_id_written
    },
    {
        "var_2_or_name_written",
        RulesBuiltinPredicates_var_2_or_name_written
    },
    {
        "this_match_eq",
        RulesBuiltinPredicates_this_match_eq
    },
    {
        "var_match_eq",
        RulesBuiltinPredicates_var_match_eq
    },
    {
        "this_match_eq_written",
        RulesBuiltinPredicates_this_match_eq_written
    },
    {
        "this_match_eq_written_self",
        RulesBuiltinPredicates_this_match_eq_written_self
    },
    {
        "var_match_eq_written",
        RulesBuiltinPredicates_var_match_eq_written
    },
    {
        "this_match_neq",
        RulesBuiltinPredicates_this_match_neq
    },
    {
        "var_match_neq",
        RulesBuiltinPredicates_var_match_neq
    },
    {
        "this_match_neq_written",
        RulesBuiltinPredicates_this_match_neq_written
    },
    {
        "var_match_neq_written",
        RulesBuiltinPredicates_var_match_neq_written
    },
    {
        "this_match_2_neq",
        RulesBuiltinPredicates_this_match_2_neq
    },
    {
        "var_match_2_neq",
        RulesBuiltinPredicates_var_match_2_neq
    },
    {
        "this_match_2_neq_written",
        RulesBuiltinPredicates_this_match_2_neq_written
    },
    {
        "var_match_2_neq_written",
        RulesBuiltinPredicates_var_match_2_neq_written
    },
    {
        "this_match_2_or",
        RulesBuiltinPredicates_this_match_2_or
    },
    {
        "this_match_2_or_written",
        RulesBuiltinPredicates_this_match_2_or_written
    },
    {
        "this_match_3_or",
        RulesBuiltinPredicates_this_match_3_or
    },
    {
        "this_match_3_or_written",
        RulesBuiltinPredicates_this_match_3_or_written
    },
    {
        "unresolved_by_name",
        RulesBuiltinPredicates_unresolved_by_name
    },
    {
        "var_eq_wildcard",
        RulesBuiltinPredicates_var_eq_wildcard
    },
    {
        "var_eq_any",
        RulesBuiltinPredicates_var_eq_any
    },
    {
        "var_eq_wildcard_after_write",
        RulesBuiltinPredicates_var_eq_wildcard_after_write
    },
    {
        "var_eq_any_after_write",
        RulesBuiltinPredicates_var_eq_any_after_write
    },
    {
        "var_eq_after_var_0_src",
        RulesBuiltinPredicates_var_eq_after_var_0_src
    },
    {
        "2_or_w_eq_this",
        RulesBuiltinPredicates_2_or_w_eq_this
    },
    {
        "2_or_w_eq_lookup_var",
        RulesBuiltinPredicates_2_or_w_eq_lookup_var
    },
    {
        "3_or_w_eq_lookup_var",
        RulesBuiltinPredicates_3_or_w_eq_lookup_var
    },
    {
        "unwritten_var_error_neq",
        RulesBuiltinPredicates_unwritten_var_error_neq
    },
    {
        "unwritten_var_error_match",
        RulesBuiltinPredicates_unwritten_var_error_match
    }
};

bake_test_case RulesScopes_testcases[] = {
    {
        "term_w_not_scope_1_term",
        RulesScopes_term_w_not_scope_1_term
    },
    {
        "term_w_not_scope_2_terms",
        RulesScopes_term_w_not_scope_2_terms
    },
    {
        "term_w_not_scope_1_term_w_not",
        RulesScopes_term_w_not_scope_1_term_w_not
    },
    {
        "term_w_not_scope_2_terms_w_not",
        RulesScopes_term_w_not_scope_2_terms_w_not
    },
    {
        "term_w_not_scope_1_term_w_var",
        RulesScopes_term_w_not_scope_1_term_w_var
    },
    {
        "term_w_not_scope_2_terms_w_var",
        RulesScopes_term_w_not_scope_2_terms_w_var
    },
    {
        "term_w_not_scope_1_term_w_not_w_var",
        RulesScopes_term_w_not_scope_1_term_w_not_w_var
    },
    {
        "term_w_not_scope_2_terms_w_not_w_var",
        RulesScopes_term_w_not_scope_2_terms_w_not_w_var
    },
    {
        "term_w_not_scope_2_terms_w_or",
        RulesScopes_term_w_not_scope_2_terms_w_or
    },
    {
        "term_w_not_scope_3_terms_w_or",
        RulesScopes_term_w_not_scope_3_terms_w_or
    }
};

bake_test_case RulesTraversal_testcases[] = {
    {
        "this_self_up_childof",
        RulesTraversal_this_self_up_childof
    },
    {
        "this_up_childof",
        RulesTraversal_this_up_childof
    },
    {
        "this_written_self_up_childof",
        RulesTraversal_this_written_self_up_childof
    },
    {
        "this_written_up_childof",
        RulesTraversal_this_written_up_childof
    },
    {
        "var_self_up_childof",
        RulesTraversal_var_self_up_childof
    },
    {
        "var_up_childof",
        RulesTraversal_var_up_childof
    },
    {
        "var_written_self_up_childof",
        RulesTraversal_var_written_self_up_childof
    },
    {
        "var_written_up_childof",
        RulesTraversal_var_written_up_childof
    },
    {
        "set_var_self_up_childof",
        RulesTraversal_set_var_self_up_childof
    },
    {
        "set_var_up_childof",
        RulesTraversal_set_var_up_childof
    },
    {
        "set_var_written_self_up_childof",
        RulesTraversal_set_var_written_self_up_childof
    },
    {
        "set_var_written_up_childof",
        RulesTraversal_set_var_written_up_childof
    },
    {
        "ent_self_up_childof",
        RulesTraversal_ent_self_up_childof
    },
    {
        "ent_up_childof",
        RulesTraversal_ent_up_childof
    },
    {
        "implicit_this_self_up_isa",
        RulesTraversal_implicit_this_self_up_isa
    },
    {
        "implicit_this_up_isa",
        RulesTraversal_implicit_this_up_isa
    },
    {
        "implicit_var_self_up_isa",
        RulesTraversal_implicit_var_self_up_isa
    },
    {
        "implicit_var_up_isa",
        RulesTraversal_implicit_var_up_isa
    },
    {
        "implicit_ent_self_up_isa",
        RulesTraversal_implicit_ent_self_up_isa
    },
    {
        "implicit_ent_up_isa",
        RulesTraversal_implicit_ent_up_isa
    },
    {
        "self_up_2_targets",
        RulesTraversal_self_up_2_targets
    },
    {
        "up_2_targets",
        RulesTraversal_up_2_targets
    },
    {
        "self_up_2_targets_diamond",
        RulesTraversal_self_up_2_targets_diamond
    },
    {
        "up_2_targets_diamond",
        RulesTraversal_up_2_targets_diamond
    },
    {
        "written_self_up_2_targets",
        RulesTraversal_written_self_up_2_targets
    },
    {
        "written_up_2_targets",
        RulesTraversal_written_up_2_targets
    },
    {
        "written_self_up_2_targets_diamond",
        RulesTraversal_written_self_up_2_targets_diamond
    },
    {
        "written_up_2_targets_diamond",
        RulesTraversal_written_up_2_targets_diamond
    },
    {
        "2_self_up_terms",
        RulesTraversal_2_self_up_terms
    },
    {
        "2_self_up_terms_2_targets",
        RulesTraversal_2_self_up_terms_2_targets
    },
    {
        "self_up_empty_table",
        RulesTraversal_self_up_empty_table
    },
    {
        "up_empty_table",
        RulesTraversal_up_empty_table
    },
    {
        "self_up_all_owned",
        RulesTraversal_self_up_all_owned
    },
    {
        "up_all_owned",
        RulesTraversal_up_all_owned
    },
    {
        "this_self_up_childof_inherited",
        RulesTraversal_this_self_up_childof_inherited
    },
    {
        "this_up_childof_inherited",
        RulesTraversal_this_up_childof_inherited
    },
    {
        "this_written_self_up_childof_inherited",
        RulesTraversal_this_written_self_up_childof_inherited
    },
    {
        "this_written_up_childof_inherited",
        RulesTraversal_this_written_up_childof_inherited
    },
    {
        "var_self_up_childof_inherited",
        RulesTraversal_var_self_up_childof_inherited
    },
    {
        "var_up_childof_inherited",
        RulesTraversal_var_up_childof_inherited
    },
    {
        "var_written_self_up_childof_inherited",
        RulesTraversal_var_written_self_up_childof_inherited
    },
    {
        "var_written_up_childof_inherited",
        RulesTraversal_var_written_up_childof_inherited
    },
    {
        "ent_self_up_childof_inherited",
        RulesTraversal_ent_self_up_childof_inherited
    },
    {
        "ent_up_childof_inherited",
        RulesTraversal_ent_up_childof_inherited
    },
    {
        "ent_written_self_up_childof_inherited",
        RulesTraversal_ent_written_self_up_childof_inherited
    },
    {
        "ent_written_up_childof_inherited",
        RulesTraversal_ent_written_up_childof_inherited
    },
    {
        "this_self_up_childof_component",
        RulesTraversal_this_self_up_childof_component
    },
    {
        "this_up_childof_component",
        RulesTraversal_this_up_childof_component
    },
    {
        "this_written_self_up_childof_component",
        RulesTraversal_this_written_self_up_childof_component
    },
    {
        "this_written_up_childof_component",
        RulesTraversal_this_written_up_childof_component
    },
    {
        "var_self_up_childof_component",
        RulesTraversal_var_self_up_childof_component
    },
    {
        "var_up_childof_component",
        RulesTraversal_var_up_childof_component
    },
    {
        "var_written_self_up_childof_component",
        RulesTraversal_var_written_self_up_childof_component
    },
    {
        "var_written_up_childof_component",
        RulesTraversal_var_written_up_childof_component
    },
    {
        "this_self_up_childof_recycled_parent",
        RulesTraversal_this_self_up_childof_recycled_parent
    },
    {
        "this_up_childof_recycled_parent",
        RulesTraversal_this_up_childof_recycled_parent
    },
    {
        "this_written_self_up_childof_recycled_parent",
        RulesTraversal_this_written_self_up_childof_recycled_parent
    },
    {
        "this_written_up_childof_recycled_parent",
        RulesTraversal_this_written_up_childof_recycled_parent
    },
    {
        "this_self_up_childof_recycled_parent_component",
        RulesTraversal_this_self_up_childof_recycled_parent_component
    },
    {
        "this_up_childof_recycled_parent_component",
        RulesTraversal_this_up_childof_recycled_parent_component
    },
    {
        "this_written_self_up_childof_recycled_parent_component",
        RulesTraversal_this_written_self_up_childof_recycled_parent_component
    },
    {
        "this_written_up_childof_recycled_parent_component",
        RulesTraversal_this_written_up_childof_recycled_parent_component
    },
    {
        "this_self_up_childof_pair",
        RulesTraversal_this_self_up_childof_pair
    },
    {
        "this_up_childof_pair",
        RulesTraversal_this_up_childof_pair
    },
    {
        "this_written_self_up_childof_pair",
        RulesTraversal_this_written_self_up_childof_pair
    },
    {
        "this_written_up_childof_pair",
        RulesTraversal_this_written_up_childof_pair
    },
    {
        "this_self_up_childof_pair_wildcard",
        RulesTraversal_this_self_up_childof_pair_wildcard
    },
    {
        "this_up_childof_pair_wildcard",
        RulesTraversal_this_up_childof_pair_wildcard
    },
    {
        "this_written_self_up_childof_pair_wildcard",
        RulesTraversal_this_written_self_up_childof_pair_wildcard
    },
    {
        "this_written_up_childof_pair_wildcard",
        RulesTraversal_this_written_up_childof_pair_wildcard
    },
    {
        "this_self_up_childof_pair_tgt_var",
        RulesTraversal_this_self_up_childof_pair_tgt_var
    },
    {
        "this_written_self_up_childof_pair_tgt_var",
        RulesTraversal_this_written_self_up_childof_pair_tgt_var
    },
    {
        "this_self_up_childof_pair_rel_var",
        RulesTraversal_this_self_up_childof_pair_rel_var
    },
    {
        "this_written_self_up_childof_pair_rel_var",
        RulesTraversal_this_written_self_up_childof_pair_rel_var
    },
    {
        "this_self_up_childof_pair_for_var_written",
        RulesTraversal_this_self_up_childof_pair_for_var_written
    },
    {
        "this_up_childof_pair_for_var_written",
        RulesTraversal_this_up_childof_pair_for_var_written
    },
    {
        "this_written_self_up_childof_pair_for_var_written",
        RulesTraversal_this_written_self_up_childof_pair_for_var_written
    },
    {
        "this_self_up_childof_pair_for_var_written_n_targets",
        RulesTraversal_this_self_up_childof_pair_for_var_written_n_targets
    },
    {
        "this_written_self_up_childof_pair_for_var_written_n_targets",
        RulesTraversal_this_written_self_up_childof_pair_for_var_written_n_targets
    },
    {
        "self_up_2_levels",
        RulesTraversal_self_up_2_levels
    },
    {
        "not_up_disabled",
        RulesTraversal_not_up_disabled
    },
    {
        "up_2_rel_instances",
        RulesTraversal_up_2_rel_instances
    },
    {
        "up_2_rel_instances_match_2nd",
        RulesTraversal_up_2_rel_instances_match_2nd
    },
    {
        "up_only_w_owned",
        RulesTraversal_up_only_w_owned
    },
    {
        "this_self_cascade_childof",
        RulesTraversal_this_self_cascade_childof
    },
    {
        "this_cascade_childof",
        RulesTraversal_this_cascade_childof
    },
    {
        "this_written_self_cascade_childof",
        RulesTraversal_this_written_self_cascade_childof
    },
    {
        "this_written_cascade_childof",
        RulesTraversal_this_written_cascade_childof
    },
    {
        "this_self_cascade_childof_w_parent_flag",
        RulesTraversal_this_self_cascade_childof_w_parent_flag
    },
    {
        "this_cascade_childof_w_parent_flag",
        RulesTraversal_this_cascade_childof_w_parent_flag
    },
    {
        "this_written_self_cascade_childof_w_parent_flag",
        RulesTraversal_this_written_self_cascade_childof_w_parent_flag
    },
    {
        "this_written_cascade_childof_w_parent_flag",
        RulesTraversal_this_written_cascade_childof_w_parent_flag
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
    },
    {
        "stop_timer_w_rate",
        Timer_stop_timer_w_rate
    },
    {
        "stop_timer_w_rate_same_src",
        Timer_stop_timer_w_rate_same_src
    },
    {
        "randomize_timers",
        Timer_randomize_timers
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
    }
};

bake_test_case SystemManual_testcases[] = {
    {
        "1_type_1_component",
        SystemManual_1_type_1_component
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
    },
    {
        "get_entity_count",
        Stats_get_entity_count
    },
    {
        "get_pipeline_stats_w_task_system",
        Stats_get_pipeline_stats_w_task_system
    },
    {
        "get_not_alive_entity_count",
        Stats_get_not_alive_entity_count
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
    },
    {
        "no_staging_w_multithread",
        MultiThread_no_staging_w_multithread
    },
    {
        "multithread_w_monitor_addon",
        MultiThread_multithread_w_monitor_addon
    },
    {
        "get_ctx",
        MultiThread_get_ctx
    },
    {
        "get_binding_ctx",
        MultiThread_get_binding_ctx
    },
    {
        "get_ctx_w_run",
        MultiThread_get_ctx_w_run
    },
    {
        "get_binding_ctx_w_run",
        MultiThread_get_binding_ctx_w_run
    },
    {
        "bulk_new_in_no_readonly_w_multithread",
        MultiThread_bulk_new_in_no_readonly_w_multithread
    },
    {
        "bulk_new_in_no_readonly_w_multithread_2",
        MultiThread_bulk_new_in_no_readonly_w_multithread_2
    },
    {
        "run_first_worker_on_main",
        MultiThread_run_first_worker_on_main
    },
    {
        "run_single_thread_on_main",
        MultiThread_run_single_thread_on_main
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
    },
    {
        "set_pair_w_new_target_readonly",
        MultiThreadStaging_set_pair_w_new_target_readonly
    },
    {
        "set_pair_w_new_target_tgt_component_readonly",
        MultiThreadStaging_set_pair_w_new_target_tgt_component_readonly
    },
    {
        "set_pair_w_new_target_defer",
        MultiThreadStaging_set_pair_w_new_target_defer
    },
    {
        "set_pair_w_new_target_tgt_component_defer",
        MultiThreadStaging_set_pair_w_new_target_tgt_component_defer
    }
};

bake_test_case MultiTaskThread_testcases[] = {
    {
        "2_thread_1_entity",
        MultiTaskThread_2_thread_1_entity
    },
    {
        "2_thread_2_entity",
        MultiTaskThread_2_thread_2_entity
    },
    {
        "2_thread_5_entity",
        MultiTaskThread_2_thread_5_entity
    },
    {
        "2_thread_10_entity",
        MultiTaskThread_2_thread_10_entity
    },
    {
        "3_thread_1_entity",
        MultiTaskThread_3_thread_1_entity
    },
    {
        "3_thread_2_entity",
        MultiTaskThread_3_thread_2_entity
    },
    {
        "3_thread_5_entity",
        MultiTaskThread_3_thread_5_entity
    },
    {
        "3_thread_10_entity",
        MultiTaskThread_3_thread_10_entity
    },
    {
        "4_thread_1_entity",
        MultiTaskThread_4_thread_1_entity
    },
    {
        "4_thread_2_entity",
        MultiTaskThread_4_thread_2_entity
    },
    {
        "4_thread_5_entity",
        MultiTaskThread_4_thread_5_entity
    },
    {
        "4_thread_10_entity",
        MultiTaskThread_4_thread_10_entity
    },
    {
        "5_thread_1_entity",
        MultiTaskThread_5_thread_1_entity
    },
    {
        "5_thread_2_entity",
        MultiTaskThread_5_thread_2_entity
    },
    {
        "5_thread_5_entity",
        MultiTaskThread_5_thread_5_entity
    },
    {
        "5_thread_10_entity",
        MultiTaskThread_5_thread_10_entity
    },
    {
        "6_thread_1_entity",
        MultiTaskThread_6_thread_1_entity
    },
    {
        "6_thread_2_entity",
        MultiTaskThread_6_thread_2_entity
    },
    {
        "6_thread_5_entity",
        MultiTaskThread_6_thread_5_entity
    },
    {
        "6_thread_10_entity",
        MultiTaskThread_6_thread_10_entity
    },
    {
        "2_thread_1_entity_instanced",
        MultiTaskThread_2_thread_1_entity_instanced
    },
    {
        "2_thread_5_entity_instanced",
        MultiTaskThread_2_thread_5_entity_instanced
    },
    {
        "2_thread_10_entity_instanced",
        MultiTaskThread_2_thread_10_entity_instanced
    },
    {
        "2_thread_test_combs_100_entity_w_next_worker",
        MultiTaskThread_2_thread_test_combs_100_entity_w_next_worker
    },
    {
        "2_thread_test_combs_100_entity",
        MultiTaskThread_2_thread_test_combs_100_entity
    },
    {
        "3_thread_test_combs_100_entity",
        MultiTaskThread_3_thread_test_combs_100_entity
    },
    {
        "4_thread_test_combs_100_entity",
        MultiTaskThread_4_thread_test_combs_100_entity
    },
    {
        "5_thread_test_combs_100_entity",
        MultiTaskThread_5_thread_test_combs_100_entity
    },
    {
        "6_thread_test_combs_100_entity",
        MultiTaskThread_6_thread_test_combs_100_entity
    },
    {
        "2_thread_test_combs_100_entity_2_types",
        MultiTaskThread_2_thread_test_combs_100_entity_2_types
    },
    {
        "3_thread_test_combs_100_entity_2_types",
        MultiTaskThread_3_thread_test_combs_100_entity_2_types
    },
    {
        "4_thread_test_combs_100_entity_2_types",
        MultiTaskThread_4_thread_test_combs_100_entity_2_types
    },
    {
        "5_thread_test_combs_100_entity_2_types",
        MultiTaskThread_5_thread_test_combs_100_entity_2_types
    },
    {
        "6_thread_test_combs_100_entity_2_types",
        MultiTaskThread_6_thread_test_combs_100_entity_2_types
    },
    {
        "change_thread_count",
        MultiTaskThread_change_thread_count
    },
    {
        "multithread_quit",
        MultiTaskThread_multithread_quit
    },
    {
        "schedule_w_tasks",
        MultiTaskThread_schedule_w_tasks
    },
    {
        "reactive_system",
        MultiTaskThread_reactive_system
    },
    {
        "fini_after_set_threads",
        MultiTaskThread_fini_after_set_threads
    },
    {
        "2_threads_single_threaded_system",
        MultiTaskThread_2_threads_single_threaded_system
    },
    {
        "no_staging_w_multithread",
        MultiTaskThread_no_staging_w_multithread
    },
    {
        "multithread_w_monitor_addon",
        MultiTaskThread_multithread_w_monitor_addon
    },
    {
        "get_ctx",
        MultiTaskThread_get_ctx
    },
    {
        "get_binding_ctx",
        MultiTaskThread_get_binding_ctx
    },
    {
        "get_ctx_w_run",
        MultiTaskThread_get_ctx_w_run
    },
    {
        "get_binding_ctx_w_run",
        MultiTaskThread_get_binding_ctx_w_run
    },
    {
        "bulk_new_in_no_readonly_w_multithread",
        MultiTaskThread_bulk_new_in_no_readonly_w_multithread
    },
    {
        "bulk_new_in_no_readonly_w_multithread_2",
        MultiTaskThread_bulk_new_in_no_readonly_w_multithread_2
    },
    {
        "run_first_worker_on_main",
        MultiTaskThread_run_first_worker_on_main
    },
    {
        "run_single_thread_on_main",
        MultiTaskThread_run_single_thread_on_main
    }
};

bake_test_case MultiTaskThreadStaging_testcases[] = {
    {
        "2_threads_add_to_current",
        MultiTaskThreadStaging_2_threads_add_to_current
    },
    {
        "3_threads_add_to_current",
        MultiTaskThreadStaging_3_threads_add_to_current
    },
    {
        "4_threads_add_to_current",
        MultiTaskThreadStaging_4_threads_add_to_current
    },
    {
        "5_threads_add_to_current",
        MultiTaskThreadStaging_5_threads_add_to_current
    },
    {
        "6_threads_add_to_current",
        MultiTaskThreadStaging_6_threads_add_to_current
    },
    {
        "2_threads_on_add",
        MultiTaskThreadStaging_2_threads_on_add
    },
    {
        "new_w_count",
        MultiTaskThreadStaging_new_w_count
    },
    {
        "custom_thread_auto_merge",
        MultiTaskThreadStaging_custom_thread_auto_merge
    },
    {
        "custom_thread_manual_merge",
        MultiTaskThreadStaging_custom_thread_manual_merge
    },
    {
        "custom_thread_partial_manual_merge",
        MultiTaskThreadStaging_custom_thread_partial_manual_merge
    },
    {
        "set_pair_w_new_target_readonly",
        MultiTaskThreadStaging_set_pair_w_new_target_readonly
    },
    {
        "set_pair_w_new_target_tgt_component_readonly",
        MultiTaskThreadStaging_set_pair_w_new_target_tgt_component_readonly
    },
    {
        "set_pair_w_new_target_defer",
        MultiTaskThreadStaging_set_pair_w_new_target_defer
    },
    {
        "set_pair_w_new_target_tgt_component_defer",
        MultiTaskThreadStaging_set_pair_w_new_target_tgt_component_defer
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
    },
    {
        "import_monitor_2_worlds",
        Modules_import_monitor_2_worlds
    },
    {
        "import_monitor_after_mini",
        Modules_import_monitor_after_mini
    },
    {
        "import_2_worlds",
        Modules_import_2_worlds
    },
    {
        "component_parent_becomes_module",
        Modules_component_parent_becomes_module
    }
};

bake_test_case App_testcases[] = {
    {
        "app_w_frame_action",
        App_app_w_frame_action
    },
    {
        "app_w_default_frame_action",
        App_app_w_default_frame_action
    },
    {
        "app_w_set_threads",
        App_app_w_set_threads
    },
    {
        "app_w_set_task_threads",
        App_app_w_set_task_threads
    },
    {
        "app_w_set_target_fps",
        App_app_w_set_target_fps
    },
    {
        "app_w_set_frames",
        App_app_w_set_frames
    }
};

bake_test_case Http_testcases[] = {
    {
        "teardown",
        Http_teardown
    },
    {
        "teardown_started",
        Http_teardown_started
    },
    {
        "teardown_stopped",
        Http_teardown_stopped
    },
    {
        "stop_start",
        Http_stop_start
    }
};

bake_test_case Rest_testcases[] = {
    {
        "teardown",
        Rest_teardown
    },
    {
        "get",
        Rest_get
    },
    {
        "get_cached",
        Rest_get_cached
    },
    {
        "get_cached_invalid",
        Rest_get_cached_invalid
    },
    {
        "try_query",
        Rest_try_query
    },
    {
        "query",
        Rest_query
    },
    {
        "named_query",
        Rest_named_query
    },
    {
        "tables",
        Rest_tables
    },
    {
        "request_commands",
        Rest_request_commands
    },
    {
        "request_commands_2_syncs",
        Rest_request_commands_2_syncs
    },
    {
        "request_commands_no_frames",
        Rest_request_commands_no_frames
    },
    {
        "request_commands_no_commands",
        Rest_request_commands_no_commands
    },
    {
        "request_commands_garbage_collect",
        Rest_request_commands_garbage_collect
    }
};

bake_test_case Metrics_testcases[] = {
    {
        "member_gauge_1_entity",
        Metrics_member_gauge_1_entity
    },
    {
        "member_gauge_2_entities",
        Metrics_member_gauge_2_entities
    },
    {
        "member_gauge_2_entities_1_existing",
        Metrics_member_gauge_2_entities_1_existing
    },
    {
        "member_gauge_2_entities_update",
        Metrics_member_gauge_2_entities_update
    },
    {
        "member_gauge_w_remove",
        Metrics_member_gauge_w_remove
    },
    {
        "member_gauge_w_clear",
        Metrics_member_gauge_w_clear
    },
    {
        "member_gauge_w_delete",
        Metrics_member_gauge_w_delete
    },
    {
        "id_gauge_1_entity",
        Metrics_id_gauge_1_entity
    },
    {
        "id_gauge_2_entities",
        Metrics_id_gauge_2_entities
    },
    {
        "id_gauge_2_entities_1_existing",
        Metrics_id_gauge_2_entities_1_existing
    },
    {
        "id_gauge_w_remove",
        Metrics_id_gauge_w_remove
    },
    {
        "id_gauge_w_clear",
        Metrics_id_gauge_w_clear
    },
    {
        "id_gauge_w_delete",
        Metrics_id_gauge_w_delete
    },
    {
        "oneof_gauge_3_entities",
        Metrics_oneof_gauge_3_entities
    },
    {
        "oneof_gauge_3_entities_1_existing",
        Metrics_oneof_gauge_3_entities_1_existing
    },
    {
        "oneof_gauge_w_remove",
        Metrics_oneof_gauge_w_remove
    },
    {
        "oneof_gauge_w_clear",
        Metrics_oneof_gauge_w_clear
    },
    {
        "oneof_gauge_w_delete",
        Metrics_oneof_gauge_w_delete
    },
    {
        "oneof_to_snake_case",
        Metrics_oneof_to_snake_case
    },
    {
        "member_counter",
        Metrics_member_counter
    },
    {
        "member_auto_counter",
        Metrics_member_auto_counter
    },
    {
        "id_counter",
        Metrics_id_counter
    },
    {
        "oneof_counter",
        Metrics_oneof_counter
    },
    {
        "metric_description",
        Metrics_metric_description
    },
    {
        "id_count",
        Metrics_id_count
    },
    {
        "id_target_count",
        Metrics_id_target_count
    },
    {
        "metric_instance_has_doc_name",
        Metrics_metric_instance_has_doc_name
    },
    {
        "metric_nested_member",
        Metrics_metric_nested_member
    },
    {
        "metric_nested_member_counter",
        Metrics_metric_nested_member_counter
    },
    {
        "metric_nested_member_counter_increment",
        Metrics_metric_nested_member_counter_increment
    },
    {
        "id_w_member_same_type",
        Metrics_id_w_member_same_type
    },
    {
        "id_w_member_mismatching_type",
        Metrics_id_w_member_mismatching_type
    },
    {
        "pair_member_rel_type",
        Metrics_pair_member_rel_type
    },
    {
        "pair_member_tgt_type",
        Metrics_pair_member_tgt_type
    },
    {
        "pair_dotmember_rel_type",
        Metrics_pair_dotmember_rel_type
    },
    {
        "pair_dotmember_tgt_type",
        Metrics_pair_dotmember_tgt_type
    },
    {
        "pair_member_counter_increment",
        Metrics_pair_member_counter_increment
    }
};

bake_test_case Alerts_testcases[] = {
    {
        "one_active_alert",
        Alerts_one_active_alert
    },
    {
        "two_active_alerts",
        Alerts_two_active_alerts
    },
    {
        "alert_message",
        Alerts_alert_message
    },
    {
        "alert_message_w_this_var",
        Alerts_alert_message_w_this_var
    },
    {
        "alert_message_w_var",
        Alerts_alert_message_w_var
    },
    {
        "alert_message_w_changed_var",
        Alerts_alert_message_w_changed_var
    },
    {
        "set_doc_name",
        Alerts_set_doc_name
    },
    {
        "set_brief",
        Alerts_set_brief
    },
    {
        "alert_instance_has_doc_name",
        Alerts_alert_instance_has_doc_name
    },
    {
        "reraise_alert",
        Alerts_reraise_alert
    },
    {
        "info_severity",
        Alerts_info_severity
    },
    {
        "warning_severity",
        Alerts_warning_severity
    },
    {
        "error_severity",
        Alerts_error_severity
    },
    {
        "expire_after_retain",
        Alerts_expire_after_retain
    },
    {
        "revive_w_retain",
        Alerts_revive_w_retain
    },
    {
        "severity_filter",
        Alerts_severity_filter
    },
    {
        "two_severity_filters",
        Alerts_two_severity_filters
    },
    {
        "severity_filter_w_var",
        Alerts_severity_filter_w_var
    },
    {
        "severity_filter_w_var_change_var",
        Alerts_severity_filter_w_var_change_var
    },
    {
        "member_range_warning",
        Alerts_member_range_warning
    },
    {
        "member_range_error",
        Alerts_member_range_error
    },
    {
        "member_range_warning_error",
        Alerts_member_range_warning_error
    },
    {
        "member_range_error_w_warning_severity",
        Alerts_member_range_error_w_warning_severity
    },
    {
        "member_range_error_w_severity_filter",
        Alerts_member_range_error_w_severity_filter
    },
    {
        "member_range_warning_w_severity_filter",
        Alerts_member_range_warning_w_severity_filter
    },
    {
        "member_range_pair_id",
        Alerts_member_range_pair_id
    },
    {
        "member_range_invalid_member",
        Alerts_member_range_invalid_member
    },
    {
        "member_range_invalid_member_child",
        Alerts_member_range_invalid_member_child
    },
    {
        "member_range_invalid_type",
        Alerts_member_range_invalid_type
    },
    {
        "member_range_invalid_member_type",
        Alerts_member_range_invalid_member_type
    },
    {
        "member_range_no_range",
        Alerts_member_range_no_range
    },
    {
        "member_range_alert_two_instances",
        Alerts_member_range_alert_two_instances
    },
    {
        "member_range_from_var",
        Alerts_member_range_from_var
    },
    {
        "member_range_from_var_after_remove",
        Alerts_member_range_from_var_after_remove
    },
    {
        "retained_alert_w_dead_source",
        Alerts_retained_alert_w_dead_source
    },
    {
        "alert_counts",
        Alerts_alert_counts
    }
};


static bake_test_suite suites[] = {
    {
        "Parser",
        NULL,
        NULL,
        261,
        Parser_testcases
    },
    {
        "Plecs",
        NULL,
        NULL,
        237,
        Plecs_testcases
    },
    {
        "Doc",
        NULL,
        NULL,
        10,
        Doc_testcases
    },
    {
        "Pipeline",
        NULL,
        NULL,
        82,
        Pipeline_testcases
    },
    {
        "SystemMisc",
        NULL,
        NULL,
        68,
        SystemMisc_testcases
    },
    {
        "RulesBasic",
        NULL,
        NULL,
        164,
        RulesBasic_testcases
    },
    {
        "RulesVariables",
        NULL,
        NULL,
        162,
        RulesVariables_testcases
    },
    {
        "RulesOperators",
        NULL,
        NULL,
        105,
        RulesOperators_testcases
    },
    {
        "RulesTransitive",
        NULL,
        NULL,
        64,
        RulesTransitive_testcases
    },
    {
        "RulesComponentInheritance",
        NULL,
        NULL,
        36,
        RulesComponentInheritance_testcases
    },
    {
        "RulesRecycled",
        NULL,
        NULL,
        4,
        RulesRecycled_testcases
    },
    {
        "RulesBuiltinPredicates",
        NULL,
        NULL,
        78,
        RulesBuiltinPredicates_testcases
    },
    {
        "RulesScopes",
        NULL,
        NULL,
        10,
        RulesScopes_testcases
    },
    {
        "RulesTraversal",
        NULL,
        NULL,
        92,
        RulesTraversal_testcases
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
        19,
        Timer_testcases
    },
    {
        "SystemCascade",
        NULL,
        NULL,
        9,
        SystemCascade_testcases
    },
    {
        "SystemManual",
        SystemManual_setup,
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
        "Stats",
        NULL,
        NULL,
        11,
        Stats_testcases
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
        50,
        MultiThread_testcases
    },
    {
        "MultiThreadStaging",
        MultiThreadStaging_setup,
        NULL,
        14,
        MultiThreadStaging_testcases
    },
    {
        "MultiTaskThread",
        MultiTaskThread_setup,
        NULL,
        50,
        MultiTaskThread_testcases
    },
    {
        "MultiTaskThreadStaging",
        MultiTaskThreadStaging_setup,
        NULL,
        14,
        MultiTaskThreadStaging_testcases
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
        24,
        Modules_testcases
    },
    {
        "App",
        NULL,
        NULL,
        6,
        App_testcases
    },
    {
        "Http",
        NULL,
        NULL,
        4,
        Http_testcases
    },
    {
        "Rest",
        NULL,
        NULL,
        13,
        Rest_testcases
    },
    {
        "Metrics",
        NULL,
        NULL,
        37,
        Metrics_testcases
    },
    {
        "Alerts",
        NULL,
        NULL,
        36,
        Alerts_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("addons", argc, argv, suites, 36);
}
