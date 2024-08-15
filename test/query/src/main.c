
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <query.h>

// Testsuite 'Validator'
void Validator_validate_1_term(void);
void Validator_validate_1_term_component(void);
void Validator_validate_2_terms(void);
void Validator_validate_3_terms(void);
void Validator_validate_3_terms_w_or(void);
void Validator_validate_4_terms_w_or_at_1(void);
void Validator_validate_1_term_wildcard(void);
void Validator_validate_1_term_any(void);
void Validator_validate_1_term_same_subj_obj(void);
void Validator_validate_1_term_acyclic_same_subj_obj(void);
void Validator_validate_1_term_acyclic_reflexive_same_subj_obj(void);
void Validator_validate_1_term_same_subj_obj_var(void);
void Validator_validate_1_term_acyclic_same_subj_obj_var(void);
void Validator_validate_1_term_acyclic_reflexive_same_subj_obj_var(void);
void Validator_validate_1_term_non_acyclic_superset(void);
void Validator_validate_1_term_dont_inherit_default_set(void);
void Validator_validate_1_term_dont_inherit_pair_default_set(void);
void Validator_validate_1_term_inherit_default_set(void);
void Validator_validate_1_term_inherit_pair_default_set(void);
void Validator_validate_1_term_override_default_set(void);
void Validator_validate_1_term_override_pair_default_set(void);
void Validator_validate_1_term_up_no_inherit(void);
void Validator_validate_1_term_up_no_inherit_pair(void);
void Validator_validate_1_term_up_override(void);
void Validator_validate_1_term_up_override_pair(void);
void Validator_validate_1_term_up_isa_no_inherit(void);
void Validator_validate_1_term_up_isa_no_inherit_pair(void);
void Validator_validate_1_term_up_isa_override(void);
void Validator_validate_1_term_up_isa_override_pair(void);
void Validator_validate_1_term_cascade_implicit_trav(void);
void Validator_validate_1_term_cascade_isa(void);
void Validator_validate_1_term_cascade_childof(void);
void Validator_validate_1_term_cascade_down(void);
void Validator_validate_1_term_optional_only(void);
void Validator_validate_1_term_transitive_pair(void);
void Validator_validate_1_variable_as_pred_only(void);
void Validator_validate_1_variable_as_pred_w_subj(void);
void Validator_validate_1_variable_as_pred_w_pair(void);
void Validator_validate_1_variable_as_subj(void);
void Validator_validate_1_variable_as_obj(void);
void Validator_validate_2_terms_or_w_dontinherit(void);
void Validator_validate_2_terms_or_w_both_dontinherit(void);
void Validator_validate_w_pair_id(void);
void Validator_validate_w_pred_obj(void);
void Validator_validate_w_pair_id_and_subj(void);
void Validator_validate_1_w_pred_name(void);
void Validator_validate_1_w_final_pred_name(void);
void Validator_validate_1_w_subj_name(void);
void Validator_validate_1_w_obj_name(void);
void Validator_validate_w_this_implicit_variable(void);
void Validator_validate_w_this_explicit_entity(void);
void Validator_validate_w_first_this_implicit_variable(void);
void Validator_validate_w_first_this_explicit_entity(void);
void Validator_validate_w_second_this_implicit_variable(void);
void Validator_validate_w_second_this_explicit_entity(void);
void Validator_validate_w_this_variable_name(void);
void Validator_validate_w_src_var(void);
void Validator_validate_w_first_var(void);
void Validator_validate_w_second_var(void);
void Validator_validate_w_src_var_from_name(void);
void Validator_validate_w_first_first_var(void);
void Validator_validate_w_second_second_var(void);
void Validator_validate_w_0_source(void);
void Validator_validate_w_0_target(void);
void Validator_validate_2_terms_w_or(void);
void Validator_validate_2_terms_w_or_mixed_src_flags(void);
void Validator_validate_2_terms_w_or_mixed_src_id(void);
void Validator_validate_2_terms_w_or_mixed_src_name(void);
void Validator_validate_2_terms_w_or_same_src_w_id_and_name(void);
void Validator_validate_w_and_flag(void);
void Validator_validate_w_or_flag(void);
void Validator_validate_w_not_flag(void);
void Validator_validate_filter(void);
void Validator_validate_double_init(void);
void Validator_validate_double_init_w_expr(void);
void Validator_validate_double_init_w_expr_optional(void);
void Validator_validate_w_tag_term_is_no_data(void);
void Validator_validate_w_inout_none_term_is_no_data(void);
void Validator_validate_w_tag_and_inout_none_term_is_no_data(void);
void Validator_validate_w_not_term_is_no_data(void);
void Validator_validate_w_no_transitive_pair(void);
void Validator_validate_w_transitive_pair_any_src(void);
void Validator_validate_w_transitive_pair(void);
void Validator_validate_w_transitive_tag_no_pair(void);
void Validator_validate_w_transitive_tag_self_tgt(void);
void Validator_validate_w_transitive_tag_any_tgt(void);
void Validator_validate_w_pair_same_vars(void);
void Validator_validate_w_pair_not_same_vars(void);
void Validator_validate_w_pair_no_vars_not_same_vars(void);
void Validator_validate_w_pair_wildcard_not_same_vars(void);
void Validator_validate_w_pair_any_not_same_vars(void);
void Validator_validate_w_no_pair_not_same_vars(void);
void Validator_validate_not_childof_any(void);
void Validator_validate_w_inherited_id(void);
void Validator_validate_w_inherited_pair(void);
void Validator_validate_w_non_inherited_id(void);
void Validator_validate_w_non_inherited_pair(void);
void Validator_validate_w_first_wildcard_inout_none(void);
void Validator_validate_w_first_var_inout_none(void);
void Validator_validate_w_pair_wildcard_inout_none(void);
void Validator_validate_w_pair_var_inout_none(void);
void Validator_validate_w_unresolved_by_name(void);
void Validator_validate_w_unresolved_by_name_eq(void);
void Validator_validate_childof_this(void);
void Validator_validate_childof_this_entity(void);
void Validator_validate_childof_this_by_id(void);
void Validator_validate_filter_flag(void);
void Validator_validate_first_0_name(void);
void Validator_validate_src_0_name(void);
void Validator_validate_second_0_name(void);
void Validator_validate_singleton_src_w_first_name(void);
void Validator_validate_singleton_second_w_first_name(void);
void Validator_not_wildcard(void);
void Validator_not_first_wildcard(void);
void Validator_not_second_wildcard(void);
void Validator_not_wildcard_id(void);
void Validator_not_wildcard_first_pair(void);
void Validator_not_wildcard_second_pair(void);
void Validator_validate_or_same_type(void);
void Validator_validate_or_different_types(void);
void Validator_validate_or_different_types(void);
void Validator_validate_or_different_types_1_and_2_or(void);
void Validator_validate_trav_isa_w_wildcard(void);
void Validator_validate_trav_isa_w_any(void);
void Validator_validate_custom_trav_w_inherit_id(void);
void Validator_validate_custom_trav_w_inherit_id_w_self_up(void);
void Validator_validate_custom_trav_w_inherit_id_w_up(void);
void Validator_validate_simple_1_term_is_cacheable(void);
void Validator_validate_simple_1_term_pair_is_cacheable(void);
void Validator_validate_simple_1_term_pair_recycled_is_cacheable(void);
void Validator_validate_simple_1_term_tag_is_cacheable(void);
void Validator_validate_simple_2_term_is_cacheable(void);
void Validator_validate_simple_w_can_inherit(void);
void Validator_validate_simple_w_can_toggle(void);
void Validator_validate_simple_w_sparse(void);
void Validator_validate_simple_w_union(void);
void Validator_validate_simple_w_union_pair(void);
void Validator_validate_simple_w_transitive(void);
void Validator_validate_simple_w_transitive_pair(void);
void Validator_validate_simple_w_reflexive(void);
void Validator_validate_simple_w_reflexive_pair(void);
void Validator_validate_simple_w_inherited_component(void);

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
void Parser_pair_implicit_subject_pred_w_invalid_flags(void);
void Parser_pair_implicit_subject_obj_w_invalid_flags(void);
void Parser_pair_explicit_subject(void);
void Parser_pair_explicit_subject_this(void);
void Parser_pair_explicit_subject_this_by_name(void);
void Parser_pair_explicit_subject_this_by_var_name(void);
void Parser_pair_explicit_subject_wildcard_pred(void);
void Parser_pair_explicit_subject_wildcard_subj(void);
void Parser_pair_explicit_subject_wildcard_obj(void);
void Parser_pair_implicit_subject_0_name_object(void);
void Parser_pair_implicit_subject_0_name_not_found_object(void);
void Parser_pair_implicit_subject_0_digit_object(void);
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
void Parser_inout_w_not_operator(void);
void Parser_inout_w_optional_operator(void);
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
void Parser_2_or_no_space(void);
void Parser_2_or_w_not_1st_arg(void);
void Parser_2_or_w_not_2nd_arg(void);
void Parser_2_or_w_optional_1st_arg(void);
void Parser_2_or_w_optional_2nd_arg(void);
void Parser_1_entity_id_pred_implicit_subj(void);
void Parser_1_entity_id_pred_no_subj(void);
void Parser_1_entity_id_pred_explicit_subj(void);
void Parser_1_entity_id_pair_implicit_subj(void);
void Parser_1_entity_id_pair_explicit_subj(void);
void Parser_1_digit_pred_implicit_subj(void);
void Parser_1_digit_pred_no_subj(void);
void Parser_1_digit_pred_explicit_subj(void);
void Parser_1_digit_pair_implicit_subj(void);
void Parser_1_digit_pair_explicit_subj(void);
void Parser_pred_implicit_subject_self(void);
void Parser_pred_implicit_subject_superset(void);
void Parser_pred_implicit_subject_superset_inclusive(void);
void Parser_pred_implicit_subject_superset_cascade(void);
void Parser_pred_implicit_subject_superset_inclusive_cascade(void);
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
void Parser_pred_entity_no_args_w_explicit_self(void);
void Parser_pred_entity_no_args_w_explicit_self_superset(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_self(void);
void Parser_pred_entity_no_args_2_terms_w_explicit_self_superset(void);
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
void Parser_template_type_nested(void);
void Parser_template_type_multiple_args(void);
void Parser_template_type_multiple_args_nested(void);
void Parser_template_type_unbalanced_open(void);
void Parser_template_type_unbalanced_close(void);
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
void Parser_eq_var_table(void);
void Parser_eq_var(void);
void Parser_neq_id(void);
void Parser_neq_id_var(void);
void Parser_neq_var_id(void);
void Parser_neq_var_table(void);
void Parser_neq_var(void);
void Parser_eq_name(void);
void Parser_eq_name_var(void);
void Parser_eq_var_name(void);
void Parser_neq_name(void);
void Parser_neq_name_var(void);
void Parser_neq_var_name(void);
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
void Parser_eq_w_not(void);
void Parser_neq_w_not(void);
void Parser_match_w_not(void);
void Parser_eq_w_optional(void);
void Parser_neq_w_optional(void);
void Parser_match_w_optional(void);
void Parser_eq_0(void);
void Parser_neq_0(void);
void Parser_eq_wildcard(void);
void Parser_neq_wildcard(void);
void Parser_eq_any(void);
void Parser_neq_any(void);
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
void Parser_query_term_after_scope(void);
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
void Parser_pair_3_args_w_vars(void);
void Parser_pair_4_args_w_vars(void);
void Parser_pair_3_args_w_vars_w_term_after(void);
void Parser_pair_4_args_w_vars_w_term_after(void);
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
void Parser_empty_term(void);
void Parser_empty_term_w_space(void);
void Parser_empty_term_w_newline(void);
void Parser_mixed_1_desc_and_1_expr(void);
void Parser_mixed_2_desc_and_1_expr(void);
void Parser_mixed_1_desc_and_2_expr(void);
void Parser_not_wildcard(void);
void Parser_not_first_wildcard(void);
void Parser_not_second_wildcard(void);
void Parser_unterminated_paren(void);
void Parser_unterminated_after_first_paren(void);
void Parser_unterminated_after_src_var(void);
void Parser_eq_name_existing_entity(void);
void Parser_match_existing_entity(void);
void Parser_match_wildcard(void);
void Parser_match_any(void);
void Parser_match_variable_oper(void);
void Parser_escaped_identifier(void);
void Parser_escaped_identifier_first(void);
void Parser_escaped_identifier_second(void);
void Parser_n_tokens_test(void);

// Testsuite 'Basic'
void Basic_setup(void);
void Basic_0_query(void);
void Basic_1_fact_w_tag(void);
void Basic_1_fact_w_component(void);
void Basic_1_fact_w_tag_pair(void);
void Basic_1_fact_w_component_pair(void);
void Basic_2_facts_same_src_w_tag(void);
void Basic_2_facts_same_src_w_component(void);
void Basic_2_facts_same_src_w_tag_pair(void);
void Basic_2_facts_same_src_w_component_pair(void);
void Basic_2_facts_other_src_w_tag(void);
void Basic_2_facts_other_src_w_component(void);
void Basic_2_facts_other_src_w_tag_pair(void);
void Basic_2_facts_other_src_w_component_pair(void);
void Basic_1_fact_w_any(void);
void Basic_1_fact_w_pair_any_tgt(void);
void Basic_1_fact_w_pair_any_rel(void);
void Basic_1_fact_w_pair_any_rel_tgt(void);
void Basic_2_facts_same_src_w_any(void);
void Basic_2_facts_same_src_w_pair_any_tgt(void);
void Basic_2_facts_same_src_w_pair_any_rel(void);
void Basic_2_facts_same_src_w_pair_any_rel_tgt(void);
void Basic_2_facts_other_src_w_any(void);
void Basic_2_facts_other_src_w_pair_any_tgt(void);
void Basic_2_facts_other_src_w_pair_any_rel(void);
void Basic_2_facts_other_src_w_pair_any_rel_tgt(void);
void Basic_1_this_src_w_tag(void);
void Basic_1_this_src_w_component(void);
void Basic_1_this_src_w_tag_pair(void);
void Basic_1_this_src_w_component_pair(void);
void Basic_1_this_src_w_tag_2_tables(void);
void Basic_1_this_src_w_component_2_tables(void);
void Basic_1_this_src_w_tag_pair_2_tables(void);
void Basic_1_this_src_w_component_pair_2_tables(void);
void Basic_2_this_src_w_tag(void);
void Basic_2_this_src_w_component(void);
void Basic_2_this_src_ent_src_w_tag(void);
void Basic_2_this_src_ent_src_w_component(void);
void Basic_2_ent_src_this_src_w_tag(void);
void Basic_2_ent_src_this_src_w_component(void);
void Basic_recycled_tag(void);
void Basic_recycled_src(void);
void Basic_recycled_pair_rel(void);
void Basic_recycled_pair_tgt(void);
void Basic_this_src_w_wildcard(void);
void Basic_this_src_w_pair_rel_wildcard(void);
void Basic_this_src_w_pair_tgt_wildcard(void);
void Basic_this_src_w_pair_rel_tgt_wildcard(void);
void Basic_this_src_w_any(void);
void Basic_this_src_w_any_written(void);
void Basic_this_src_w_pair_rel_any(void);
void Basic_this_src_w_pair_tgt_any(void);
void Basic_this_src_w_pair_rel_tgt_any(void);
void Basic_ent_src_w_wildcard(void);
void Basic_ent_src_w_pair_rel_wildcard(void);
void Basic_ent_src_w_pair_tgt_wildcard(void);
void Basic_ent_src_w_pair_rel_tgt_wildcard(void);
void Basic_1_wildcard_src(void);
void Basic_1_wildcard_src_w_pair(void);
void Basic_2_wildcard_src(void);
void Basic_2_wildcard_src_w_pair(void);
void Basic_1_wildcard_src_w_pair_tgt_var(void);
void Basic_1_wildcard_src_w_pair_rel_var(void);
void Basic_1_wildcard_src_w_pair_tgt_this(void);
void Basic_1_wildcard_src_w_pair_rel_this(void);
void Basic_1_any_src(void);
void Basic_1_any_src_w_pair(void);
void Basic_2_any_src(void);
void Basic_2_any_src_w_pair(void);
void Basic_1_any_src_w_pair_tgt_var(void);
void Basic_1_any_src_w_pair_rel_var(void);
void Basic_1_any_src_w_pair_tgt_this(void);
void Basic_1_any_src_w_pair_rel_this(void);
void Basic_not_any(void);
void Basic_rule_w_iter_next(void);
void Basic_empty_rule(void);
void Basic_invalid_rule(void);
void Basic_instanced_table_src(void);
void Basic_instanced_entity_src(void);
void Basic_instanced_mixed_src(void);
void Basic_instanced_inherited(void);
void Basic_in_term(void);
void Basic_out_term(void);
void Basic_inout_term(void);
void Basic_nodata_term(void);
void Basic_find_this_lowercase(void);
void Basic_find_this_uppercase(void);
void Basic_find_this_tgt_lowercase(void);
void Basic_find_this_tgt_uppercase(void);
void Basic_get_filter(void);
void Basic_iter_empty_source(void);
void Basic_iter_empty_source_2_terms(void);
void Basic_iter_empty_source_wildcard(void);
void Basic_iter_empty_source_pair(void);
void Basic_iter_empty_source_pair_wildcard(void);
void Basic_iter_empty_source_2_terms_pair(void);
void Basic_iter_empty_source_2_terms_mixed(void);
void Basic_iter_empty_source_2_terms_mixed_pair(void);
void Basic_iter_empty_source_2_terms_mixed_pair_wildcard(void);
void Basic_this_var_w_empty_entity(void);
void Basic_match_disabled(void);
void Basic_match_prefab(void);
void Basic_match_disabled_prefab(void);
void Basic_match_disabled_this_tgt(void);
void Basic_match_prefab_this_tgt(void);
void Basic_match_disabled_prefab_this_tgt(void);
void Basic_match_self_disabled(void);
void Basic_match_self_prefab(void);
void Basic_match_self_disabled_prefab(void);
void Basic_match_optional_disabled(void);
void Basic_match_optional_prefab(void);
void Basic_match_optional_disabled_prefab(void);
void Basic_match_optional_disabled_this_tgt(void);
void Basic_match_optional_prefab_this_tgt(void);
void Basic_match_optional_disabled_prefab_this_tgt(void);
void Basic_match_optional_self_disabled(void);
void Basic_match_optional_self_prefab(void);
void Basic_match_optional_self_disabled_prefab(void);
void Basic_inout_none_first_term(void);
void Basic_inout_none_first_term_self_up(void);
void Basic_inout_none_second_term(void);
void Basic_inout_none_second_term_self_up(void);
void Basic_inout_none_singleton(void);
void Basic_inout_none_singleton_w_or(void);
void Basic_inout_none_component_w_or(void);
void Basic_no_data_rule(void);
void Basic_frame_offset(void);
void Basic_frame_offset_no_data(void);
void Basic_match_empty_tables(void);
void Basic_match_empty_tables_no_data(void);
void Basic_match_empty_tables_w_not(void);
void Basic_match_empty_tables_w_wildcard(void);
void Basic_match_empty_tables_w_no_empty_tables(void);
void Basic_match_empty_tables_trivial(void);
void Basic_oneof_wildcard(void);
void Basic_oneof_any(void);
void Basic_instanced_w_singleton(void);
void Basic_instanced_w_base(void);
void Basic_unknown_before_known(void);
void Basic_unknown_before_known_after_or(void);
void Basic_unknown_before_known_after_not(void);
void Basic_unknown_before_known_after_optional(void);
void Basic_unknown_before_known_after_scope(void);
void Basic_2_trivial_mixed_2_tables(void);
void Basic_2_trivial_mixed_2_tables_component(void);
void Basic_2_trivial_mixed_2_tables_wildcard(void);
void Basic_2_trivial_1_unused_id(void);
void Basic_2_trivial_one_regular(void);
void Basic_1_trivial_one_regular_one_trivial(void);
void Basic_one_regular_2_trivial(void);
void Basic_2_trivial_w_prefab(void);
void Basic_3_trivial_w_prefab(void);
void Basic_2_trivial_w_disabled(void);
void Basic_3_trivial_w_disabled(void);
void Basic_2_this_w_fixed_src(void);
void Basic_2_fixed_src_w_this(void);
void Basic_2_this_w_fixed_src_no_match_fixed(void);
void Basic_2_fixed_src_w_this_no_match_fixed(void);
void Basic_2_this_w_fixed_src_no_match_this(void);
void Basic_2_fixed_src_w_this_no_match_this(void);
void Basic_query_count_results(void);
void Basic_query_count_entities(void);
void Basic_query_is_true(void);
void Basic_implicit_cleanup_1_term(void);
void Basic_implicit_cleanup_2_terms(void);
void Basic_implicit_cleanup_1_term_w_up(void);
void Basic_implicit_cleanup_2_terms_w_up(void);
void Basic_implicit_cleanup_2_queries(void);
void Basic_implicit_cleanup_2_queries_1_cleanup(void);
void Basic_iter_valid(void);
void Basic_iter_frame_offset(void);
void Basic_iter_nested_1(void);
void Basic_iter_nested_2(void);
void Basic_iter_interleaved(void);
void Basic_set_get_context(void);
void Basic_set_get_binding_context(void);
void Basic_set_get_context_w_free(void);
void Basic_set_get_binding_context_w_free(void);
void Basic_create_query_w_existing_entity(void);
void Basic_create_query_w_existing_entity_different_term_count(void);
void Basic_create_multi_component_query_w_existing_entity(void);
void Basic_delete_query_by_entity(void);
void Basic_eval_count(void);
void Basic_no_results_after_delete_tree(void);
void Basic_no_results_after_delete_tree_deferred(void);
void Basic_add_on_self_ref(void);
void Basic_add_on_self_ref_by_name(void);
void Basic_delete_id_after_delete_query(void);
void Basic_pair_sweep_tag(void);
void Basic_pair_sweep_wildcard_first(void);
void Basic_pair_sweep_wildcard_second(void);
void Basic_create_w_entity_deferred(void);
void Basic_32_terms(void);
void Basic_33_terms_expr(void);
void Basic_stage_query(void);
void Basic_world_query_w_stage_iter(void);
void Basic_stage_query_w_nth_stage(void);
void Basic_world_query_w_nth_stage_iter(void);
void Basic_match_empty(void);
void Basic_match_new_empty(void);
void Basic_match_empty_w_component(void);
void Basic_match_new_empty_w_component(void);
void Basic_match_empty_w_ref(void);
void Basic_match_new_empty_w_ref(void);
void Basic_match_empty_w_order_by(void);
void Basic_match_new_empty_w_order_by(void);
void Basic_match_empty_w_bitset(void);
void Basic_default_query_flags(void);

// Testsuite 'Combinations'
void Combinations_setup(void);
void Combinations_singleton_and_self_and_self(void);
void Combinations_self_and_singleton_and_self(void);
void Combinations_self_and_self_and_singleton(void);
void Combinations_singleton_and_trav_and_self(void);
void Combinations_trav_and_singleton_and_self(void);
void Combinations_self_and_singleton_and_trav(void);
void Combinations_self_and_trav_and_singleton(void);
void Combinations_singleton_and_self_and_trav(void);
void Combinations_trav_and_self_and_singleton(void);

// Testsuite 'Plan'
void Plan_reordered_plan_1(void);
void Plan_reordered_plan_2(void);
void Plan_reordered_plan_3(void);
void Plan_reordered_plan_4(void);
void Plan_reordered_plan_5(void);
void Plan_reordered_plan_6(void);
void Plan_reordered_plan_7(void);
void Plan_1_trivial_plan(void);
void Plan_2_trivial_plan(void);
void Plan_1_trivial_plan_component(void);
void Plan_2_trivial_plan_component(void);
void Plan_3_trivial_plan_w_pair(void);
void Plan_3_trivial_plan_w_wildcard(void);
void Plan_3_trivial_plan_w_any(void);
void Plan_3_trivial_plan_w_pair_component(void);
void Plan_3_trivial_plan_w_wildcard_component(void);
void Plan_3_trivial_plan_w_any_component(void);
void Plan_1_trivial_component_w_none(void);
void Plan_2_trivial_component_w_none(void);
void Plan_2_trivial_plan_w_wildcard(void);
void Plan_this_before_fixed_src(void);
void Plan_fixed_src_before_this(void);
void Plan_var_before_fixed_src(void);
void Plan_fixed_src_before_var(void);
void Plan_this_before_fixed_src_w_not(void);
void Plan_this_before_fixed_src_w_first_var(void);
void Plan_this_before_fixed_src_w_first_var_w_not(void);
void Plan_this_before_fixed_src_w_second_var(void);
void Plan_this_before_fixed_src_w_second_var_w_not(void);
void Plan_populate_1_fixed(void);
void Plan_populate_2_fixed(void);
void Plan_populate_1_fixed_1_this_self(void);
void Plan_populate_2_fixed_2_this_self(void);
void Plan_populate_2_fixed_2_this_self_interleaved(void);
void Plan_populate_2_this_self_2_fixed(void);
void Plan_populate_1_fixed_1_this_up(void);
void Plan_populate_2_fixed_2_this_up(void);
void Plan_populate_2_fixed_2_this_up_interleaved(void);
void Plan_populate_2_this_up_2_fixed(void);
void Plan_populate_1_fixed_1_this_self_cached(void);
void Plan_populate_2_fixed_2_this_self_cached(void);
void Plan_populate_2_fixed_2_this_self_interleaved_cached(void);
void Plan_populate_2_this_self_2_fixed_cached(void);
void Plan_populate_1_fixed_1_this_up_cached(void);
void Plan_populate_2_fixed_2_this_up_cached(void);
void Plan_populate_2_fixed_2_this_up_interleaved_cached(void);
void Plan_populate_2_this_up_2_fixed_cached(void);
void Plan_populate_1_fixed_1_var_self(void);
void Plan_populate_2_fixed_2_var_self(void);
void Plan_populate_2_fixed_2_var_self_interleaved(void);
void Plan_populate_2_var_self_2_fixed(void);
void Plan_populate_1_fixed_1_var_up(void);
void Plan_populate_2_fixed_2_var_up(void);
void Plan_populate_2_fixed_2_var_up_interleaved(void);
void Plan_populate_2_var_up_2_fixed(void);
void Plan_cache_2_or(void);
void Plan_cache_2_or_w_not(void);
void Plan_1_plan_any_src(void);
void Plan_1_plan_not_any_src(void);
void Plan_1_plan_optional_any_src(void);
void Plan_pair_first_wildcard(void);
void Plan_pair_first_wildcard_cached(void);
void Plan_pair_second_wildcard(void);
void Plan_pair_second_wildcard_cached(void);
void Plan_0_src_tag(void);
void Plan_0_src_component(void);
void Plan_0_src_w_sparse(void);
void Plan_0_src_w_toggle(void);
void Plan_0_src_w_union(void);
void Plan_0_src_w_sparse_and_component(void);
void Plan_0_src_w_toggle_and_component(void);
void Plan_0_src_w_union_and_component(void);

// Testsuite 'Variables'
void Variables_setup(void);
void Variables_1_ent_src_w_var(void);
void Variables_1_ent_src_w_pair_rel_var(void);
void Variables_1_ent_src_w_pair_tgt_var(void);
void Variables_1_ent_src_w_pair_rel_tgt_var(void);
void Variables_1_ent_src_w_pair_rel_tgt_same_var(void);
void Variables_1_ent_src_w_pair_rel_tgt_same_var_after_write(void);
void Variables_1_this_src_w_var(void);
void Variables_1_this_src_w_pair_rel_var(void);
void Variables_1_this_src_w_pair_tgt_var(void);
void Variables_1_this_src_w_pair_rel_tgt_var(void);
void Variables_1_this_src_w_pair_rel_tgt_same_var(void);
void Variables_1_this_src_w_pair_rel_tgt_same_var_after_write(void);
void Variables_1_src_id_same_var(void);
void Variables_1_src_pair_first_same_var(void);
void Variables_1_src_pair_second_same_var(void);
void Variables_1_src_pair_first_and_second_same_var(void);
void Variables_1_src_id_same_var_after_write(void);
void Variables_1_src_pair_first_same_var_after_write(void);
void Variables_1_src_pair_second_same_var_after_write(void);
void Variables_1_src_pair_first_and_second_same_var_after_write(void);
void Variables_1_src_pair_first_same_var_this(void);
void Variables_1_src_pair_second_same_var_this(void);
void Variables_1_src_pair_first_and_second_same_var_this(void);
void Variables_1_src_id_same_var_this_after_write(void);
void Variables_1_src_pair_first_same_var_this_after_write(void);
void Variables_1_src_pair_second_same_var_this_after_write(void);
void Variables_1_src_pair_first_and_second_same_var_this_after_write(void);
void Variables_1_ent_src_w_this_var(void);
void Variables_1_ent_src_w_pair_this_rel(void);
void Variables_1_ent_src_w_pair_this_tgt(void);
void Variables_1_ent_src_w_pair_this_rel_tgt(void);
void Variables_1_this_src_w_this(void);
void Variables_1_this_src_w_pair_this_rel_tgt(void);
void Variables_1_this_src_w_this_after_write(void);
void Variables_1_this_src_w_pair_this_rel_tgt_after_write(void);
void Variables_2_constrain_src_from_src(void);
void Variables_2_constrain_rel_from_src_w_ent(void);
void Variables_2_constrain_rel_from_src_w_var(void);
void Variables_2_constrain_rel_from_src_w_this(void);
void Variables_2_constrain_pair_rel_from_src_w_ent(void);
void Variables_2_constrain_pair_rel_from_src_w_var(void);
void Variables_2_constrain_pair_rel_from_src_w_this(void);
void Variables_2_constrain_pair_tgt_from_src_w_ent(void);
void Variables_2_constrain_pair_tgt_from_src_w_var(void);
void Variables_2_constrain_pair_tgt_from_src_w_this(void);
void Variables_2_constrain_pair_rel_tgt_from_src_w_ent(void);
void Variables_2_constrain_pair_rel_tgt_from_src_w_var(void);
void Variables_2_constrain_pair_rel_tgt_from_src_w_this(void);
void Variables_1_ent_src_set_rel_var(void);
void Variables_1_ent_src_set_pair_rel_var(void);
void Variables_1_ent_src_set_pair_tgt_var(void);
void Variables_1_set_src_var(void);
void Variables_1_set_src_var_w_pair(void);
void Variables_1_set_src_var_w_pair_set_rel(void);
void Variables_1_set_src_var_w_pair_set_tgt(void);
void Variables_1_set_src_var_w_pair_set_rel_tgt(void);
void Variables_1_set_src_this(void);
void Variables_1_set_src_this_w_pair(void);
void Variables_1_set_src_this_w_pair_set_rel(void);
void Variables_1_set_src_this_w_pair_set_tgt(void);
void Variables_1_set_src_this_w_pair_set_rel_tgt(void);
void Variables_1_set_src_this_to_empty_table(void);
void Variables_1_set_src_this_to_empty_table_w_component(void);
void Variables_1_set_src_this_to_empty_table_w_component_self(void);
void Variables_1_set_src_this_to_entity_in_table(void);
void Variables_1_set_src_this_to_entity_in_table_self(void);
void Variables_2_set_src_this(void);
void Variables_2_set_src_this_self(void);
void Variables_2_set_src_this_component(void);
void Variables_2_set_src_this_self_component(void);
void Variables_2_set_src_this_w_up(void);
void Variables_2_set_src_this_self_w_up(void);
void Variables_2_set_src_this_component_w_up(void);
void Variables_2_set_src_this_self_component_w_up(void);
void Variables_2_set_src_this_w_exclusive_wildcard(void);
void Variables_2_set_src_this_self_w_exclusive_wildcard(void);
void Variables_2_set_src_this_self_w_exclusive_wildcard_w_up(void);
void Variables_1_set_src_this_is_true(void);
void Variables_3_set_src_this_w_uncacheable_tag_tag_tag(void);
void Variables_3_set_src_this_w_uncacheable_tag_component_tag(void);
void Variables_3_set_src_this_w_uncacheable_tag_component_component(void);
void Variables_3_set_src_this_w_tag_uncacheable_tag_tag(void);
void Variables_3_set_src_this_w_component_uncacheable_tag_tag(void);
void Variables_3_set_src_this_w_component_uncacheable_tag_component(void);
void Variables_3_set_src_this_w_tag_tag_uncacheable_tag(void);
void Variables_3_set_src_this_w_component_tag_uncacheable_tag(void);
void Variables_3_set_src_this_w_component_component_uncacheable_tag(void);
void Variables_3_set_src_this_w_uncacheable_component_tag_tag(void);
void Variables_3_set_src_this_w_uncacheable_component_component_tag(void);
void Variables_3_set_src_this_w_uncacheable_component_component_component(void);
void Variables_3_set_src_this_w_tag_uncacheable_component_tag(void);
void Variables_3_set_src_this_w_component_uncacheable_component_tag(void);
void Variables_3_set_src_this_w_component_uncacheable_component_component(void);
void Variables_3_set_src_this_w_tag_tag_uncacheable_component(void);
void Variables_3_set_src_this_w_component_tag_uncacheable_component(void);
void Variables_3_set_src_this_w_component_component_uncacheable_component(void);
void Variables_2_set_src_this_w_wildcard(void);
void Variables_1_src_this_var_as_entity(void);
void Variables_1_src_this_var_as_table(void);
void Variables_1_src_this_var_as_table_range(void);
void Variables_1_set_src_this_w_any(void);
void Variables_1_set_src_this_w_any_fixed(void);
void Variables_1_set_src_this_w_fixed_any(void);
void Variables_2_join_by_rel_var(void);
void Variables_2_join_by_pair_rel_var(void);
void Variables_2_join_by_pair_tgt_var(void);
void Variables_2_cycle_w_var(void);
void Variables_2_cycle_w_this_var(void);
void Variables_2_cycle_w_var_this(void);
void Variables_2_cycle_pair_w_var(void);
void Variables_2_cycle_pair_w_this_var_var(void);
void Variables_2_cycle_pair_w_var_this_var(void);
void Variables_2_cycle_pair_w_var_var_this(void);
void Variables_2_cycle_pair_ent_var_var(void);
void Variables_2_cycle_pair_ent_this_var(void);
void Variables_2_cycle_pair_ent_var_this(void);
void Variables_parse_0_var(void);
void Variables_parse_1_var(void);
void Variables_parse_2_vars(void);
void Variables_parse_0_var_paren(void);
void Variables_parse_1_var_paren(void);
void Variables_parse_2_vars_paren(void);
void Variables_parse_1_vars_w_path(void);
void Variables_parse_missing_close_paren(void);
void Variables_parse_missing_open_paren(void);
void Variables_parse_missing_value(void);
void Variables_parse_0_var_w_spaces(void);
void Variables_parse_1_var_w_spaces(void);
void Variables_parse_2_vars_w_spaces(void);
void Variables_parse_0_var_paren_w_spaces(void);
void Variables_parse_1_var_paren_w_spaces(void);
void Variables_parse_2_vars_paren_w_spaces(void);
void Variables_var_count(void);
void Variables_var_name(void);
void Variables_var_is_entity(void);
void Variables_no_this_anonymous_src(void);
void Variables_no_this_anonymous_src_w_pair(void);
void Variables_no_this_anonymous_component_src(void);
void Variables_no_this_anonymous_component_src_w_pair(void);
void Variables_lookup_from_table_this(void);
void Variables_lookup_from_entity_this(void);
void Variables_lookup_from_table(void);
void Variables_lookup_from_entity(void);
void Variables_lookup_from_not_written(void);
void Variables_lookup_from_table_this_component(void);
void Variables_lookup_from_entity_this_component(void);
void Variables_lookup_from_table_component(void);
void Variables_lookup_from_entity_component(void);
void Variables_lookup_from_table_two_children(void);
void Variables_lookup_from_entity_two_children(void);
void Variables_lookup_from_table_same_child_twice(void);
void Variables_lookup_from_entity_same_child_twice(void);
void Variables_lookup_from_table_not(void);
void Variables_lookup_from_entity_not(void);
void Variables_lookup_from_table_w_any_component(void);
void Variables_lookup_from_entity_w_any_component(void);
void Variables_lookup_as_tag(void);
void Variables_lookup_as_relationship(void);
void Variables_lookup_as_target(void);
void Variables_lookup_assign_var(void);
void Variables_lookup_eq_var(void);
void Variables_lookup_neq_var(void);
void Variables_lookup_unresolved_dependent(void);
void Variables_check_vars_this(void);
void Variables_check_vars_var(void);
void Variables_check_vars_wildcard(void);
void Variables_check_vars_any(void);
void Variables_check_vars_var_as_tgt(void);
void Variables_check_vars_this_as_tgt(void);
void Variables_check_vars_anonymous_var_as_tgt(void);
void Variables_check_vars_wildcard_as_tgt(void);
void Variables_check_vars_any_as_tgt(void);
void Variables_check_vars_this_w_lookup_var(void);
void Variables_check_vars_var_w_lookup_var(void);
void Variables_1_trivial_1_var(void);
void Variables_2_trivial_1_var(void);
void Variables_1_trivial_1_var_component(void);
void Variables_2_trivial_1_var_component(void);
void Variables_1_trivial_1_wildcard(void);
void Variables_2_trivial_1_wildcard(void);
void Variables_1_trivial_1_wildcard_component(void);
void Variables_2_trivial_1_wildcard_component(void);
void Variables_1_trivial_1_any(void);
void Variables_2_trivial_1_any(void);
void Variables_1_trivial_1_any_component(void);
void Variables_2_trivial_1_any_component(void);
void Variables_first_invalid_var_name_and_id(void);
void Variables_src_invalid_var_name_and_id(void);
void Variables_second_invalid_var_name_and_id(void);

// Testsuite 'Operators'
void Operators_setup(void);
void Operators_2_and_not(void);
void Operators_2_and_not_component(void);
void Operators_2_and_out_not(void);
void Operators_2_and_out_not_component(void);
void Operators_3_and_not_not(void);
void Operators_2_and_not_pair_rel_wildcard(void);
void Operators_2_and_not_pair_tgt_wildcard(void);
void Operators_2_and_not_pair_rel_tgt_wildcard(void);
void Operators_2_and_not_pair_rel_var(void);
void Operators_2_and_not_pair_tgt_var(void);
void Operators_2_and_not_pair_rel_tgt_var(void);
void Operators_2_and_not_pair_rel_tgt_same_var(void);
void Operators_2_and_not_pair_rel_var_written(void);
void Operators_2_and_not_pair_tgt_var_written(void);
void Operators_2_and_not_pair_rel_tgt_var_written(void);
void Operators_2_and_not_pair_rel_tgt_same_var_written(void);
void Operators_2_and_not_pair_rel_src_tgt_same_var_written(void);
void Operators_2_and_not_pair_any_rel(void);
void Operators_2_and_not_pair_any_tgt(void);
void Operators_2_and_not_pair_any_src(void);
void Operators_1_not_any_src_fixed_first(void);
void Operators_1_not_any_src_any_tgt_fixed_first(void);
void Operators_1_not_any_src_any_first_fixed_tgt(void);
void Operators_1_not_any_src_any_childof_pair_any_tgt(void);
void Operators_1_not_any_src_any_isa_pair_any_tgt(void);
void Operators_2_and_optional(void);
void Operators_3_and_optional_optional(void);
void Operators_2_and_optional_pair_rel_wildcard(void);
void Operators_2_and_optional_pair_tgt_wildcard(void);
void Operators_2_and_optional_pair_rel_var(void);
void Operators_2_and_optional_pair_tgt_var(void);
void Operators_2_and_optional_pair_rel_tgt_var(void);
void Operators_2_and_optional_pair_rel_tgt_same_var(void);
void Operators_2_and_optional_pair_rel_var_written(void);
void Operators_2_and_optional_pair_tgt_var_written(void);
void Operators_2_and_optional_pair_rel_tgt_var_written(void);
void Operators_2_and_optional_pair_rel_tgt_same_var_written(void);
void Operators_2_and_optional_pair_rel_src_tgt_same_var_written(void);
void Operators_3_and_optional_optional_pair_w_var(void);
void Operators_2_and_optional_pair_any_rel(void);
void Operators_2_and_optional_pair_any_tgt(void);
void Operators_2_and_optional_pair_any_src(void);
void Operators_3_and_optional_dependent_and_pair_rel(void);
void Operators_3_and_optional_dependent_and_pair_tgt(void);
void Operators_3_and_optional_dependent_and_pair_rel_tgt(void);
void Operators_3_and_optional_dependent_and_pair_rel_tgt_same_var(void);
void Operators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var(void);
void Operators_3_and_optional_dependent_and_pair_src(void);
void Operators_3_and_optional_dependent_optional_pair_rel(void);
void Operators_3_and_optional_dependent_optional_pair_tgt(void);
void Operators_3_and_optional_dependent_optional_pair_src(void);
void Operators_3_and_optional_dependent_not_pair_rel(void);
void Operators_3_and_optional_dependent_not_pair_tgt(void);
void Operators_3_and_optional_dependent_not_pair_src(void);
void Operators_1_optional_any_src_fixed_first(void);
void Operators_1_optional_any_src_any_tgt_fixed_first(void);
void Operators_1_optional_any_src_any_first_fixed_tgt(void);
void Operators_1_optional_any_src_any_childof_pair_any_tgt(void);
void Operators_1_optional_any_src_any_isa_pair_any_tgt(void);
void Operators_2_or(void);
void Operators_3_or(void);
void Operators_2_or_w_and(void);
void Operators_and_w_2_or_w_and(void);
void Operators_and_w_2_or_w_and_components(void);
void Operators_and_w_2_or_w_and_set_this(void);
void Operators_and_w_2_or_w_and_components_set_this(void);
void Operators_3_or_w_and(void);
void Operators_2_or_written(void);
void Operators_3_or_written(void);
void Operators_2_or_written_w_rel_var(void);
void Operators_3_or_written_w_rel_var(void);
void Operators_2_or_written_w_tgt_var(void);
void Operators_2_or_written_w_rel_tgt_var(void);
void Operators_2_or_written_w_rel_tgt_same_var(void);
void Operators_3_or_written_w_tgt_var(void);
void Operators_2_or_chains(void);
void Operators_2_or_chains_written(void);
void Operators_2_or_dependent(void);
void Operators_2_or_dependent_reverse(void);
void Operators_2_or_dependent_2_vars(void);
void Operators_2_or_written_dependent(void);
void Operators_2_or_written_dependent_2_vars(void);
void Operators_2_or_w_dependent(void);
void Operators_2_or_w_both(void);
void Operators_3_or_w_both(void);
void Operators_2_or_w_not(void);
void Operators_2_or_w_not_component(void);
void Operators_2_or_w_not_out_component(void);
void Operators_2_or_w_not_out_all_components(void);
void Operators_2_not_first(void);
void Operators_2_optional_first(void);
void Operators_only_not(void);
void Operators_only_not_component(void);
void Operators_only_not_out_component(void);
void Operators_only_optional(void);
void Operators_only_optional_component(void);
void Operators_not_after_fixed_src(void);
void Operators_optional_after_fixed_src(void);
void Operators_root_entities_empty(void);
void Operators_root_entities(void);
void Operators_root_entities_w_children(void);
void Operators_root_entities_w_optional_children(void);
void Operators_core_entities_w_optional_children(void);
void Operators_root_entities_w_not_children(void);
void Operators_core_entities_w_not_children(void);
void Operators_1_ent_src_not(void);
void Operators_1_ent_src_not_pair(void);
void Operators_1_ent_src_not_pair_rel_wildcard(void);
void Operators_1_ent_src_not_pair_tgt_wildcard(void);
void Operators_1_ent_src_not_pair_rel_tgt_wildcard(void);
void Operators_1_ent_src_not_pair_rel_any(void);
void Operators_1_ent_src_not_pair_tgt_any(void);
void Operators_1_ent_src_not_pair_rel_tgt_any(void);
void Operators_1_ent_src_not_pair_rel_var(void);
void Operators_1_ent_src_not_pair_tgt_var(void);
void Operators_1_ent_src_not_pair_rel_tgt_var(void);
void Operators_1_ent_src_not_pair_rel_tgt_same_var(void);
void Operators_1_this_src_not_pair_rel_var(void);
void Operators_1_this_src_not_pair_tgt_var(void);
void Operators_1_this_src_not_pair_rel_tgt_var(void);
void Operators_1_this_src_not_pair_rel_tgt_same_var(void);
void Operators_1_ent_src_not_pair_rel_var_written(void);
void Operators_1_ent_src_not_pair_tgt_var_written(void);
void Operators_1_ent_src_not_pair_rel_tgt_var_written(void);
void Operators_1_ent_src_not_pair_rel_tgt_same_var_written(void);
void Operators_and_from_fixed_src(void);
void Operators_not_from_fixed_src(void);
void Operators_or_from_fixed_src(void);
void Operators_and_from_this(void);
void Operators_not_from_this(void);
void Operators_or_from_this(void);
void Operators_and_from_this_written(void);
void Operators_not_from_this_written(void);
void Operators_or_from_this_written(void);
void Operators_and_from_empty(void);
void Operators_not_from_empty(void);
void Operators_or_from_empty(void);
void Operators_and_from_empty_w_tag(void);
void Operators_not_from_empty_w_tag(void);
void Operators_or_from_empty_w_tag(void);
void Operators_or_w_wildcard(void);
void Operators_or_w_component_and_tag(void);
void Operators_or_w_tag_and_component(void);

// Testsuite 'Transitive'
void Transitive_1_fact_0_lvl_true(void);
void Transitive_1_fact_1_lvl_true(void);
void Transitive_1_fact_2_lvl_true(void);
void Transitive_1_fact_0_lvl_false(void);
void Transitive_1_fact_1_lvl_false(void);
void Transitive_1_fact_2_lvl_false(void);
void Transitive_1_fact_reflexive(void);
void Transitive_1_isa(void);
void Transitive_1_childof(void);
void Transitive_1_this_src_written_0_lvl(void);
void Transitive_1_this_src_written_1_lvl(void);
void Transitive_1_this_src_written_2_lvl(void);
void Transitive_1_this_src_written_reflexive(void);
void Transitive_1_this_src_0_lvl(void);
void Transitive_1_this_src_1_lvl(void);
void Transitive_1_this_src_2_lvl(void);
void Transitive_1_this_src_reflexive(void);
void Transitive_1_ent_src_tgt_var_0_lvl(void);
void Transitive_1_ent_src_tgt_var_1_lvl(void);
void Transitive_1_ent_src_tgt_var_2_lvl(void);
void Transitive_1_ent_src_tgt_var_reflexive(void);
void Transitive_1_this_src_tgt_var(void);
void Transitive_1_this_src_tgt_var_reflexive(void);
void Transitive_1_var_src_written_0_lvl(void);
void Transitive_1_var_src_written_1_lvl(void);
void Transitive_1_var_src_written_2_lvl(void);
void Transitive_1_var_src_written_reflexive(void);
void Transitive_1_var_src_0_lvl(void);
void Transitive_1_var_src_1_lvl(void);
void Transitive_1_var_src_2_lvl(void);
void Transitive_1_var_src_reflexive(void);
void Transitive_1_var_src_tgt_var(void);
void Transitive_1_var_src_tgt_var_reflexive(void);
void Transitive_1_ent_src_tgt_this_0_lvl(void);
void Transitive_1_ent_src_tgt_this_1_lvl(void);
void Transitive_1_ent_src_tgt_this_2_lvl(void);
void Transitive_1_ent_src_tgt_this_reflexive(void);
void Transitive_1_var_src_tgt_this(void);
void Transitive_1_var_src_tgt_this_reflexive(void);
void Transitive_2_ent_src_constrain_tgt_var_before_0_lvl(void);
void Transitive_2_ent_src_constrain_tgt_var_before_1_lvl(void);
void Transitive_2_ent_src_constrain_tgt_var_before_2_lvl(void);
void Transitive_2_ent_src_constrain_tgt_var_after_0_lvl(void);
void Transitive_2_ent_src_constrain_tgt_var_after_1_lvl(void);
void Transitive_2_ent_src_constrain_tgt_var_after_2_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_before_0_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_before_1_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_before_2_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_after_0_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_after_1_lvl(void);
void Transitive_2_this_src_constrain_tgt_var_after_2_lvl(void);
void Transitive_1_src_tgt_same_var(void);
void Transitive_1_src_tgt_same_var_reflexive(void);
void Transitive_1_src_tgt_same_this_var_reflexive(void);
void Transitive_1_any_src_tgt_var(void);
void Transitive_not_transitive_ent_tgt(void);
void Transitive_not_transitive_var_tgt(void);
void Transitive_not_transitive_ent_tgt_written(void);
void Transitive_not_transitive_var_tgt_written(void);
void Transitive_optional_transitive_ent_tgt(void);
void Transitive_optional_transitive_var_tgt(void);
void Transitive_optional_transitive_ent_tgt_written(void);
void Transitive_optional_transitive_var_tgt_written(void);
void Transitive_2_var_src_w_same_tgt_ent(void);
void Transitive_self_target(void);
void Transitive_any_target(void);
void Transitive_isa_prefab(void);
void Transitive_isa_disabled(void);
void Transitive_isa_prefab_match_prefab_flag(void);
void Transitive_isa_prefab_match_prefab_term(void);
void Transitive_isa_disabled_match_disabled_flag(void);
void Transitive_isa_disabled_match_disabled_term(void);

// Testsuite 'ComponentInheritance'
void ComponentInheritance_1_ent_0_lvl(void);
void ComponentInheritance_1_ent_1_lvl(void);
void ComponentInheritance_1_ent_2_lvl(void);
void ComponentInheritance_1_ent_3_lvl(void);
void ComponentInheritance_1_this_0_lvl(void);
void ComponentInheritance_1_this_1_lvl(void);
void ComponentInheritance_1_this_2_lvl(void);
void ComponentInheritance_1_this_3_lvl(void);
void ComponentInheritance_1_this_0_lvl_written(void);
void ComponentInheritance_1_this_1_lvl_written(void);
void ComponentInheritance_1_this_2_lvl_written(void);
void ComponentInheritance_1_this_3_lvl_written(void);
void ComponentInheritance_1_var_0_lvl(void);
void ComponentInheritance_1_var_1_lvl(void);
void ComponentInheritance_1_var_2_lvl(void);
void ComponentInheritance_1_var_3_lvl(void);
void ComponentInheritance_1_var_0_lvl_written(void);
void ComponentInheritance_1_var_1_lvl_written(void);
void ComponentInheritance_1_var_2_lvl_written(void);
void ComponentInheritance_1_var_3_lvl_written(void);
void ComponentInheritance_1_ent_1_lvl_self(void);
void ComponentInheritance_1_this_1_lvl_self(void);
void ComponentInheritance_1_this_1_lvl_written_self(void);
void ComponentInheritance_1_var_1_lvl_self(void);
void ComponentInheritance_1_var_1_lvl_written_self(void);
void ComponentInheritance_1_ent_src_not(void);
void ComponentInheritance_1_this_src_not(void);
void ComponentInheritance_1_var_src_not(void);
void ComponentInheritance_1_this_src_not_written(void);
void ComponentInheritance_1_var_src_not_written(void);
void ComponentInheritance_first_self(void);

// Testsuite 'Recycled'
void Recycled_setup(void);
void Recycled_recycled_vars(void);
void Recycled_recycled_pair_vars(void);
void Recycled_recycled_this_ent_var(void);
void Recycled_has_recycled_id_from_pair(void);
void Recycled_recycled_pair(void);
void Recycled_recycled_component_id(void);

// Testsuite 'BuiltinPredicates'
void BuiltinPredicates_setup(void);
void BuiltinPredicates_this_eq_id(void);
void BuiltinPredicates_this_eq_name(void);
void BuiltinPredicates_this_eq_var(void);
void BuiltinPredicates_this_eq_id_written(void);
void BuiltinPredicates_this_eq_id_written_no_match(void);
void BuiltinPredicates_this_eq_name_written(void);
void BuiltinPredicates_this_eq_name_written_no_match(void);
void BuiltinPredicates_this_eq_var_written(void);
void BuiltinPredicates_var_eq_id(void);
void BuiltinPredicates_var_eq_name(void);
void BuiltinPredicates_var_eq_var(void);
void BuiltinPredicates_var_eq_id_written(void);
void BuiltinPredicates_var_eq_id_written_no_match(void);
void BuiltinPredicates_var_eq_name_written(void);
void BuiltinPredicates_var_eq_name_written_no_match(void);
void BuiltinPredicates_var_eq_var_written(void);
void BuiltinPredicates_var_eq_this(void);
void BuiltinPredicates_this_neq_id(void);
void BuiltinPredicates_this_neq_name(void);
void BuiltinPredicates_this_neq_var(void);
void BuiltinPredicates_this_neq_id_written(void);
void BuiltinPredicates_this_neq_id_written_no_match(void);
void BuiltinPredicates_this_neq_name_written(void);
void BuiltinPredicates_this_neq_name_written_no_match(void);
void BuiltinPredicates_this_neq_var_written(void);
void BuiltinPredicates_var_neq_id(void);
void BuiltinPredicates_var_neq_name(void);
void BuiltinPredicates_var_neq_var(void);
void BuiltinPredicates_var_neq_id_written(void);
void BuiltinPredicates_var_neq_id_written_no_match(void);
void BuiltinPredicates_var_neq_name_written(void);
void BuiltinPredicates_var_neq_name_written_no_match(void);
void BuiltinPredicates_var_neq_var_written(void);
void BuiltinPredicates_var_neq_this(void);
void BuiltinPredicates_this_2_neq_id(void);
void BuiltinPredicates_this_2_neq_name(void);
void BuiltinPredicates_var_2_neq_id(void);
void BuiltinPredicates_var_2_neq_name(void);
void BuiltinPredicates_this_2_neq_id_written(void);
void BuiltinPredicates_this_2_neq_name_written(void);
void BuiltinPredicates_var_2_neq_id_written(void);
void BuiltinPredicates_var_2_neq_name_written(void);
void BuiltinPredicates_this_2_or_id(void);
void BuiltinPredicates_this_3_or_id(void);
void BuiltinPredicates_this_2_or_name(void);
void BuiltinPredicates_this_3_or_name(void);
void BuiltinPredicates_this_2_or_match(void);
void BuiltinPredicates_this_3_or_match(void);
void BuiltinPredicates_var_2_or_id(void);
void BuiltinPredicates_var_2_or_name(void);
void BuiltinPredicates_this_2_or_id_written(void);
void BuiltinPredicates_this_3_or_id_written(void);
void BuiltinPredicates_this_2_or_name_written(void);
void BuiltinPredicates_var_2_or_id_written(void);
void BuiltinPredicates_var_2_or_name_written(void);
void BuiltinPredicates_this_match_eq(void);
void BuiltinPredicates_var_match_eq(void);
void BuiltinPredicates_this_match_eq_written(void);
void BuiltinPredicates_this_match_eq_written_self(void);
void BuiltinPredicates_var_match_eq_written(void);
void BuiltinPredicates_this_match_neq(void);
void BuiltinPredicates_var_match_neq(void);
void BuiltinPredicates_this_match_neq_written(void);
void BuiltinPredicates_var_match_neq_written(void);
void BuiltinPredicates_this_match_2_neq(void);
void BuiltinPredicates_var_match_2_neq(void);
void BuiltinPredicates_this_match_2_neq_written(void);
void BuiltinPredicates_var_match_2_neq_written(void);
void BuiltinPredicates_this_match_2_or(void);
void BuiltinPredicates_this_match_2_or_written(void);
void BuiltinPredicates_this_match_3_or(void);
void BuiltinPredicates_this_match_3_or_written(void);
void BuiltinPredicates_unresolved_by_name(void);
void BuiltinPredicates_var_eq_wildcard(void);
void BuiltinPredicates_var_eq_any(void);
void BuiltinPredicates_var_eq_wildcard_after_write(void);
void BuiltinPredicates_var_eq_any_after_write(void);
void BuiltinPredicates_var_eq_after_var_0_src(void);
void BuiltinPredicates_2_or_w_eq_this(void);
void BuiltinPredicates_2_or_w_eq_lookup_var(void);
void BuiltinPredicates_3_or_w_eq_lookup_var(void);
void BuiltinPredicates_eq_variable(void);
void BuiltinPredicates_eq_wildcard(void);
void BuiltinPredicates_eq_any(void);
void BuiltinPredicates_neq_variable(void);
void BuiltinPredicates_neq_wildcard(void);
void BuiltinPredicates_neq_any(void);
void BuiltinPredicates_match_variable(void);
void BuiltinPredicates_match_wildcard(void);
void BuiltinPredicates_match_any(void);

// Testsuite 'Scopes'
void Scopes_setup(void);
void Scopes_term_w_not_scope_1_term(void);
void Scopes_term_w_not_scope_2_terms(void);
void Scopes_term_w_not_scope_1_term_w_not(void);
void Scopes_term_w_not_scope_2_terms_w_not(void);
void Scopes_term_w_not_scope_1_term_w_var(void);
void Scopes_term_w_not_scope_2_terms_w_var(void);
void Scopes_term_w_not_scope_1_term_w_not_w_var(void);
void Scopes_term_w_not_scope_2_terms_w_not_w_var(void);
void Scopes_term_w_not_scope_2_terms_w_or(void);
void Scopes_term_w_not_scope_3_terms_w_or(void);
void Scopes_term_w_not_scope_2_terms_w_before_after(void);

// Testsuite 'Traversal'
void Traversal_setup(void);
void Traversal_this_self_up_childof(void);
void Traversal_this_up_childof(void);
void Traversal_this_written_self_up_childof(void);
void Traversal_this_written_up_childof(void);
void Traversal_var_self_up_childof(void);
void Traversal_var_up_childof(void);
void Traversal_var_written_self_up_childof(void);
void Traversal_var_written_up_childof(void);
void Traversal_set_var_self_up_childof(void);
void Traversal_set_var_up_childof(void);
void Traversal_set_var_written_self_up_childof(void);
void Traversal_set_var_written_up_childof(void);
void Traversal_ent_self_up_childof(void);
void Traversal_ent_up_childof(void);
void Traversal_implicit_this_self_up_isa(void);
void Traversal_implicit_this_up_isa(void);
void Traversal_implicit_var_self_up_isa(void);
void Traversal_implicit_var_up_isa(void);
void Traversal_implicit_ent_self_up_isa(void);
void Traversal_implicit_ent_up_isa(void);
void Traversal_self_up_2_targets(void);
void Traversal_up_2_targets(void);
void Traversal_self_up_2_targets_diamond(void);
void Traversal_up_2_targets_diamond(void);
void Traversal_written_self_up_2_targets(void);
void Traversal_written_up_2_targets(void);
void Traversal_written_self_up_2_targets_diamond(void);
void Traversal_written_up_2_targets_diamond(void);
void Traversal_2_self_up_terms(void);
void Traversal_2_self_up_terms_2_targets(void);
void Traversal_self_up_empty_table(void);
void Traversal_up_empty_table(void);
void Traversal_self_up_match_empty_table(void);
void Traversal_up_match_empty_table(void);
void Traversal_self_up_all_owned(void);
void Traversal_up_all_owned(void);
void Traversal_this_self_up_childof_inherited(void);
void Traversal_this_up_childof_inherited(void);
void Traversal_this_written_self_up_childof_inherited(void);
void Traversal_this_written_up_childof_inherited(void);
void Traversal_var_self_up_childof_inherited(void);
void Traversal_var_up_childof_inherited(void);
void Traversal_var_written_self_up_childof_inherited(void);
void Traversal_var_written_up_childof_inherited(void);
void Traversal_ent_self_up_childof_inherited(void);
void Traversal_ent_up_childof_inherited(void);
void Traversal_ent_written_self_up_childof_inherited(void);
void Traversal_ent_written_up_childof_inherited(void);
void Traversal_this_self_up_childof_component(void);
void Traversal_this_up_childof_component(void);
void Traversal_this_written_self_up_childof_component(void);
void Traversal_this_written_up_childof_component(void);
void Traversal_var_self_up_childof_component(void);
void Traversal_var_up_childof_component(void);
void Traversal_var_written_self_up_childof_component(void);
void Traversal_var_written_up_childof_component(void);
void Traversal_this_self_up_childof_recycled_parent(void);
void Traversal_this_up_childof_recycled_parent(void);
void Traversal_this_written_self_up_childof_recycled_parent(void);
void Traversal_this_written_up_childof_recycled_parent(void);
void Traversal_this_self_up_childof_recycled_parent_component(void);
void Traversal_this_up_childof_recycled_parent_component(void);
void Traversal_this_written_self_up_childof_recycled_parent_component(void);
void Traversal_this_written_up_childof_recycled_parent_component(void);
void Traversal_this_self_up_childof_pair(void);
void Traversal_this_up_childof_pair(void);
void Traversal_this_written_self_up_childof_pair(void);
void Traversal_this_written_up_childof_pair(void);
void Traversal_this_self_up_childof_pair_wildcard(void);
void Traversal_this_up_childof_pair_wildcard(void);
void Traversal_this_written_self_up_childof_pair_wildcard(void);
void Traversal_this_written_up_childof_pair_wildcard(void);
void Traversal_this_self_up_childof_pair_tgt_var(void);
void Traversal_this_written_self_up_childof_pair_tgt_var(void);
void Traversal_this_self_up_childof_pair_rel_var(void);
void Traversal_this_written_self_up_childof_pair_rel_var(void);
void Traversal_this_self_up_childof_pair_for_var_written(void);
void Traversal_this_up_childof_pair_for_var_written(void);
void Traversal_this_written_self_up_childof_pair_for_var_written(void);
void Traversal_this_self_up_childof_pair_for_var_written_n_targets(void);
void Traversal_this_written_self_up_childof_pair_for_var_written_n_targets(void);
void Traversal_self_up_2_levels_w_prefab(void);
void Traversal_self_up_2_levels_other_trav_rel_w_prefab(void);
void Traversal_up_2_levels_w_prefab(void);
void Traversal_up_2_levels_other_trav_rel_w_prefab(void);
void Traversal_self_up_2_levels(void);
void Traversal_self_up_2_levels_other_trav_rel(void);
void Traversal_up_2_levels(void);
void Traversal_up_2_levels_other_trav_rel(void);
void Traversal_self_up_mixed_traversable(void);
void Traversal_not_up(void);
void Traversal_not_self_up(void);
void Traversal_not_up_wildcard(void);
void Traversal_not_self_up_wildcard(void);
void Traversal_not_up_disabled(void);
void Traversal_up_2_rel_instances(void);
void Traversal_up_2_rel_instances_match_2nd(void);
void Traversal_up_only_w_owned(void);
void Traversal_this_up_trav_unused_rel(void);
void Traversal_this_optional_self(void);
void Traversal_this_optional_up(void);
void Traversal_this_optional_self_up(void);
void Traversal_this_written_optional_self(void);
void Traversal_this_written_optional_up(void);
void Traversal_this_written_optional_self_up(void);
void Traversal_fixed_src_w_up(void);
void Traversal_match_empty_table_up(void);
void Traversal_match_empty_table_up_written(void);
void Traversal_match_empty_table_up_implicit_isa(void);
void Traversal_match_empty_table_up_written_implicit_isa(void);
void Traversal_match_empty_table_up_isa(void);
void Traversal_match_empty_table_up_written_isa(void);
void Traversal_up_after_add_batched_to_parent(void);
void Traversal_up_component_after_parent_table_change(void);
void Traversal_up_component_w_singleton_after_parent_table_change(void);
void Traversal_up_component_w_var_after_parent_table_change(void);
void Traversal_test_up_component_after_parent_table_change(void);
void Traversal_test_up_component_w_singleton_after_parent_table_change(void);
void Traversal_up_component_after_parent_table_change_no_data(void);
void Traversal_up_component_w_singleton_after_parent_table_change_no_data(void);
void Traversal_up_component_w_var_after_parent_table_change_no_data(void);
void Traversal_test_up_component_after_parent_table_change_no_data(void);
void Traversal_test_up_component_w_singleton_after_parent_table_change_no_data(void);

// Testsuite 'Cascade'
void Cascade_this_self_cascade_childof_uncached(void);
void Cascade_this_cascade_childof_uncached(void);
void Cascade_this_written_self_cascade_childof_uncached(void);
void Cascade_this_written_cascade_childof_uncached(void);
void Cascade_this_self_cascade_childof_w_parent_flag_uncached(void);
void Cascade_this_cascade_childof_w_parent_flag_uncached(void);
void Cascade_this_written_self_cascade_childof_w_parent_flag_uncached(void);
void Cascade_this_written_cascade_childof_w_parent_flag_uncached(void);
void Cascade_parent_cascade(void);
void Cascade_existing_custom_rel_cascade(void);
void Cascade_new_custom_rel_cascade(void);
void Cascade_cascade_w_2_depths(void);
void Cascade_cascade_w_3_depths(void);
void Cascade_cascade_w_2_depths_desc(void);
void Cascade_cascade_w_3_depths_desc(void);
void Cascade_existing_isa_cascade(void);
void Cascade_new_isa_cascade(void);
void Cascade_childof_cascade(void);
void Cascade_cascade_rematch_2_lvls(void);
void Cascade_cascade_rematch_2_lvls_2_relations(void);
void Cascade_cascade_topological(void);
void Cascade_cascade_desc_rematch_2_lvls(void);
void Cascade_cascade_desc_rematch_2_lvls_2_relations(void);
void Cascade_cascade_desc_topological(void);
void Cascade_cascade_after_recycled_parent_change(void);

// Testsuite 'Cached'
void Cached_simple_query_existing_table(void);
void Cached_simple_query_2_existing_tables(void);
void Cached_simple_query_new_table(void);
void Cached_simple_query_2_new_tables(void);
void Cached_simple_query_existing_and_new_table(void);
void Cached_wildcard_query_existing_table(void);
void Cached_wildcard_query_new_table(void);
void Cached_wildcard_query_existing_table_2_results_p_table(void);
void Cached_wildcard_query_new_table_2_results_p_table(void);
void Cached_wildcard_query_2nd_term(void);
void Cached_wildcard_query_2nd_term_self(void);
void Cached_simple_query_existing_empty_table(void);
void Cached_simple_query_existing_empty_type(void);
void Cached_simple_query_new_empty_table(void);
void Cached_component_query_existing_table(void);
void Cached_component_query_new_table(void);
void Cached_component_query_existing_empty_table(void);
void Cached_2_component_query_existing_empty_table(void);
void Cached_2_component_query_existing_empty_type(void);
void Cached_only_optional(void);
void Cached_only_optional_new_empty_table(void);
void Cached_only_optional_new_empty_non_empty_table(void);
void Cached_only_optional_new_unset_tables(void);
void Cached_singleton_w_optional_new_empty_table(void);
void Cached_singleton_w_optional_new_empty_non_empty_table(void);
void Cached_singleton_w_optional_new_unset_tables(void);
void Cached_query_w_from_entity_match_after(void);
void Cached_query_w_from_singleton_match_after(void);
void Cached_query_rematch_optional_after_add(void);
void Cached_get_owned_tag(void);
void Cached_get_shared_tag(void);
void Cached_explicit_delete(void);
void Cached_get_column_size(void);
void Cached_stresstest_query_free(void);
void Cached_query_optional_tag(void);
void Cached_query_optional_shared_tag(void);
void Cached_query_iter_10_tags(void);
void Cached_query_iter_10_components(void);
void Cached_iter_type_set(void);
void Cached_filter_term(void);
void Cached_2_terms_1_filter(void);
void Cached_3_terms_2_filter(void);
void Cached_add_singleton_after_query(void);
void Cached_query_w_component_from_parent_from_non_this(void);
void Cached_create_query_while_pending(void);
void Cached_empty_query(void);
void Cached_not_pair_relation_wildcard(void);
void Cached_not_pair_object_wildcard(void);
void Cached_two_pair_wildcards_one_not(void);
void Cached_two_pair_wildcards_one_not_any(void);
void Cached_implicit_existing_isa_superset(void);
void Cached_implicit_new_isa_superset(void);
void Cached_isa_superset(void);
void Cached_isa_superset_2_lvls(void);
void Cached_isa_superset_3_lvls(void);
void Cached_isa_superset_2_lvls_owned(void);
void Cached_isa_superset_3_lvls_owned(void);
void Cached_isa_superset_owned_empty_table_after_match(void);
void Cached_isa_self_superset(void);
void Cached_childof_superset(void);
void Cached_superset_2_targets(void);
void Cached_superset_2_relations(void);
void Cached_superset_2_relations_instanced(void);
void Cached_superset_2_relations_w_component(void);
void Cached_superset_2_relations_instanced_w_component(void);
void Cached_parent(void);
void Cached_isa_rematch(void);
void Cached_childof_rematch(void);
void Cached_isa_unmatch(void);
void Cached_childof_unmatch(void);
void Cached_isa_rematch_2_lvls(void);
void Cached_childof_rematch_2_lvls(void);
void Cached_childof_rematch_from_isa(void);
void Cached_rematch_optional_ref(void);
void Cached_rematch_optional_ref_w_2_refs(void);
void Cached_rematch_optional_ref_tag_w_ref_component(void);
void Cached_rematch_after_add_to_recycled_parent(void);
void Cached_match_query_expr_from_scope(void);
void Cached_query_long_or_w_ref(void);
void Cached_query_w_pair_id_and_subj(void);
void Cached_rematch_after_delete_inherited_tag(void);
void Cached_rematch_after_delete_rel_of_inherited_pair(void);
void Cached_rematch_after_delete_obj_of_inherited_pair(void);
void Cached_rematch_empty(void);
void Cached_rematch_empty_table_w_superset(void);
void Cached_2_self_up_terms_new_tables(void);
void Cached_this_self_up_childof_pair_new_tables(void);

// Testsuite 'ChangeDetection'
void ChangeDetection_query_changed_after_new(void);
void ChangeDetection_query_changed_after_delete(void);
void ChangeDetection_query_changed_after_add(void);
void ChangeDetection_query_changed_after_remove(void);
void ChangeDetection_query_changed_after_set(void);
void ChangeDetection_query_change_after_modified(void);
void ChangeDetection_query_change_after_out_system(void);
void ChangeDetection_query_change_after_out_query_no_data_flag(void);
void ChangeDetection_query_change_after_in_system(void);
void ChangeDetection_query_change_after_modified_out_term(void);
void ChangeDetection_query_change_check_iter(void);
void ChangeDetection_query_change_check_iter_after_skip_read(void);
void ChangeDetection_query_change_check_iter_after_skip_write(void);
void ChangeDetection_query_change_parent_term(void);
void ChangeDetection_query_change_prefab_term(void);
void ChangeDetection_query_change_parent_term_w_tag(void);
void ChangeDetection_query_change_prefab_term_w_tag(void);
void ChangeDetection_query_changed_w_or(void);
void ChangeDetection_query_changed_or(void);
void ChangeDetection_query_changed_w_singleton(void);
void ChangeDetection_query_changed_w_only_singleton(void);
void ChangeDetection_query_changed_w_only_singleton_after_set(void);
void ChangeDetection_query_changed_w_only_singleton_after_out_term(void);
void ChangeDetection_query_changed_w_only_singleton_after_singleton_out_term(void);
void ChangeDetection_query_changed_w_only_parent(void);
void ChangeDetection_query_changed_w_only_parent_after_set(void);
void ChangeDetection_query_changed_w_only_parent_after_out_term(void);
void ChangeDetection_query_changed_w_only_parent_after_parent_out_term(void);
void ChangeDetection_query_changed_tag(void);
void ChangeDetection_query_changed_no_source(void);
void ChangeDetection_query_changed_no_source_component(void);
void ChangeDetection_query_changed_w_not_out(void);
void ChangeDetection_query_change_w_optional(void);
void ChangeDetection_query_changed_after_count(void);

// Testsuite 'GroupBy'
void GroupBy_group_by(void);
void GroupBy_group_by_w_ctx(void);
void GroupBy_group_by_w_sort_reverse_group_creation(void);
void GroupBy_group_by_iter_one(void);
void GroupBy_group_by_iter_one_all_groups(void);
void GroupBy_group_by_iter_one_empty(void);
void GroupBy_group_by_iter_one_empty_query(void);
void GroupBy_group_by_iter_one_empty_table(void);
void GroupBy_group_by_w_deleted_group_id(void);
void GroupBy_group_by_callbacks(void);
void GroupBy_group_by_default_action(void);
void GroupBy_group_table_count(void);

// Testsuite 'MemberTarget'
void MemberTarget_setup(void);
void MemberTarget_this_member_eq_1(void);
void MemberTarget_this_member_eq_2(void);
void MemberTarget_this_member_eq_no_matches(void);
void MemberTarget_this_member_eq_all_matches(void);
void MemberTarget_this_member_wildcard(void);
void MemberTarget_this_member_var(void);
void MemberTarget_this_member_var_written(void);
void MemberTarget_this_member_var_read(void);
void MemberTarget_this_member_eq_1_2nd_member(void);
void MemberTarget_this_member_eq_2_2nd_member(void);
void MemberTarget_this_member_var_same_1(void);
void MemberTarget_this_member_var_same_2(void);
void MemberTarget_this_written_member_eq_1(void);
void MemberTarget_this_written_member_eq_2(void);
void MemberTarget_this_written_member_wildcard(void);
void MemberTarget_this_written_member_var(void);
void MemberTarget_this_member_neq_1(void);
void MemberTarget_this_member_neq_2(void);
void MemberTarget_this_member_neq_no_matches(void);
void MemberTarget_this_member_neq_all_matches(void);
void MemberTarget_this_member_neq_wildcard(void);
void MemberTarget_this_written_member_neq_1(void);
void MemberTarget_this_written_member_neq_2(void);
void MemberTarget_this_written_member_neq_no_matches(void);
void MemberTarget_this_written_member_neq_all_matches(void);
void MemberTarget_this_written_member_neq_wildcard(void);
void MemberTarget_this_member_eq_optional(void);
void MemberTarget_this_member_eq_optional_wildcard(void);
void MemberTarget_this_written_member_eq_optional(void);
void MemberTarget_this_written_member_eq_optional_wildcard(void);
void MemberTarget_this_member_eq_w_other_tag(void);
void MemberTarget_this_member_eq_w_other_component(void);
void MemberTarget_this_written_member_eq_w_other_tag(void);
void MemberTarget_this_written_member_eq_w_other_component(void);
void MemberTarget_this_written_member_eq_w_other_inherit_component(void);
void MemberTarget_this_2_or(void);
void MemberTarget_this_3_or(void);
void MemberTarget_this_written_2_or(void);
void MemberTarget_this_written_3_or(void);
void MemberTarget_var_2_or(void);
void MemberTarget_var_3_or(void);
void MemberTarget_this_2_or_w_2_members(void);
void MemberTarget_this_2_or_w_2_types(void);
void MemberTarget_this_written_2_or_w_2_members(void);
void MemberTarget_this_written_2_or_w_2_types(void);
void MemberTarget_this_2_or_2_types_wildcard(void);
void MemberTarget_this_2_or_2_types_dep_var(void);
void MemberTarget_var_written_2_or(void);
void MemberTarget_var_written_3_or(void);
void MemberTarget_var_member_eq(void);
void MemberTarget_var_member_eq_no_matches(void);
void MemberTarget_var_member_eq_all_matches(void);
void MemberTarget_var_member_wildcard(void);
void MemberTarget_var_member_neq(void);
void MemberTarget_var_member_neq_no_matches(void);
void MemberTarget_var_member_neq_all_matches(void);
void MemberTarget_var_written_member_eq(void);
void MemberTarget_var_written_member_eq_no_matches(void);
void MemberTarget_var_written_member_eq_all_matches(void);
void MemberTarget_var_written_member_wildcard(void);
void MemberTarget_var_written_member_neq(void);
void MemberTarget_var_written_member_neq_no_matches(void);
void MemberTarget_var_written_member_neq_all_matches(void);

// Testsuite 'Toggle'
void Toggle_setup(void);
void Toggle_fixed_src_1_tag_toggle(void);
void Toggle_fixed_src_1_component_toggle(void);
void Toggle_fixed_src_2_tag_toggle(void);
void Toggle_fixed_src_2_component_toggle(void);
void Toggle_fixed_2_src_w_toggle(void);
void Toggle_this_w_fixed_src_w_toggle(void);
void Toggle_fixed_src_w_this_w_toggle(void);
void Toggle_this_from_nothing(void);
void Toggle_this(void);
void Toggle_this_skip_initial(void);
void Toggle_this_pair(void);
void Toggle_this_pair_skip_initial(void);
void Toggle_this_tag(void);
void Toggle_this_tag_pair(void);
void Toggle_this_tag_pair_wildcard(void);
void Toggle_this_toggle_shared_self_up(void);
void Toggle_this_toggle_shared_up(void);
void Toggle_this_toggle_shared_self_up_w_self(void);
void Toggle_this_toggle_shared_up_w_self(void);
void Toggle_this_toggle_shared_self_up_w_self_reverse(void);
void Toggle_this_toggle_shared_up_w_self_reverse(void);
void Toggle_this_toggle_shared_self_up_w_self_toggle(void);
void Toggle_this_toggle_shared_up_w_self_toggle(void);
void Toggle_this_toggle_shared_self_up_w_self_toggle_reverse(void);
void Toggle_this_toggle_shared_up_w_self_toggle_reverse(void);
void Toggle_this_toggle_not_shared_self_up(void);
void Toggle_this_toggle_not_shared_up(void);
void Toggle_this_toggle_optional_shared_self_up(void);
void Toggle_this_toggle_optional_shared_up(void);
void Toggle_this_64_mod_1(void);
void Toggle_this_64_mod_2(void);
void Toggle_this_64_mod_3(void);
void Toggle_this_64_mod_7(void);
void Toggle_this_64_mod_8(void);
void Toggle_this_64_mod_10(void);
void Toggle_this_64_mod_64(void);
void Toggle_this_64_mod_256(void);
void Toggle_this_64_mod_1024(void);
void Toggle_this_100_mod_1(void);
void Toggle_this_100_mod_2(void);
void Toggle_this_100_mod_3(void);
void Toggle_this_100_mod_7(void);
void Toggle_this_100_mod_8(void);
void Toggle_this_100_mod_10(void);
void Toggle_this_100_mod_64(void);
void Toggle_this_100_mod_256(void);
void Toggle_this_100_mod_1024(void);
void Toggle_this_128_mod_1(void);
void Toggle_this_128_mod_2(void);
void Toggle_this_128_mod_3(void);
void Toggle_this_128_mod_7(void);
void Toggle_this_128_mod_8(void);
void Toggle_this_128_mod_10(void);
void Toggle_this_128_mod_64(void);
void Toggle_this_128_mod_256(void);
void Toggle_this_128_mod_1024(void);
void Toggle_this_200_mod_1(void);
void Toggle_this_200_mod_2(void);
void Toggle_this_200_mod_3(void);
void Toggle_this_200_mod_7(void);
void Toggle_this_200_mod_8(void);
void Toggle_this_200_mod_10(void);
void Toggle_this_200_mod_64(void);
void Toggle_this_200_mod_256(void);
void Toggle_this_200_mod_1024(void);
void Toggle_this_1024_mod_1(void);
void Toggle_this_1024_mod_2(void);
void Toggle_this_1024_mod_3(void);
void Toggle_this_1024_mod_7(void);
void Toggle_this_1024_mod_8(void);
void Toggle_this_1024_mod_10(void);
void Toggle_this_1024_mod_64(void);
void Toggle_this_1024_mod_256(void);
void Toggle_this_1024_mod_1024(void);
void Toggle_this_enabled_64_mod_1(void);
void Toggle_this_enabled_64_mod_2(void);
void Toggle_this_enabled_64_mod_3(void);
void Toggle_this_enabled_64_mod_7(void);
void Toggle_this_enabled_64_mod_8(void);
void Toggle_this_enabled_64_mod_10(void);
void Toggle_this_enabled_64_mod_64(void);
void Toggle_this_enabled_64_mod_256(void);
void Toggle_this_enabled_64_mod_1024(void);
void Toggle_this_enabled_100_mod_1(void);
void Toggle_this_enabled_100_mod_2(void);
void Toggle_this_enabled_100_mod_3(void);
void Toggle_this_enabled_100_mod_7(void);
void Toggle_this_enabled_100_mod_8(void);
void Toggle_this_enabled_100_mod_10(void);
void Toggle_this_enabled_100_mod_64(void);
void Toggle_this_enabled_100_mod_256(void);
void Toggle_this_enabled_100_mod_1024(void);
void Toggle_this_enabled_128_mod_1(void);
void Toggle_this_enabled_128_mod_2(void);
void Toggle_this_enabled_128_mod_3(void);
void Toggle_this_enabled_128_mod_7(void);
void Toggle_this_enabled_128_mod_8(void);
void Toggle_this_enabled_128_mod_10(void);
void Toggle_this_enabled_128_mod_64(void);
void Toggle_this_enabled_128_mod_256(void);
void Toggle_this_enabled_128_mod_1024(void);
void Toggle_this_enabled_200_mod_1(void);
void Toggle_this_enabled_200_mod_2(void);
void Toggle_this_enabled_200_mod_3(void);
void Toggle_this_enabled_200_mod_7(void);
void Toggle_this_enabled_200_mod_8(void);
void Toggle_this_enabled_200_mod_10(void);
void Toggle_this_enabled_200_mod_64(void);
void Toggle_this_enabled_200_mod_256(void);
void Toggle_this_enabled_200_mod_1024(void);
void Toggle_this_enabled_1024_mod_1(void);
void Toggle_this_enabled_1024_mod_2(void);
void Toggle_this_enabled_1024_mod_3(void);
void Toggle_this_enabled_1024_mod_7(void);
void Toggle_this_enabled_1024_mod_8(void);
void Toggle_this_enabled_1024_mod_10(void);
void Toggle_this_enabled_1024_mod_64(void);
void Toggle_this_enabled_1024_mod_256(void);
void Toggle_this_enabled_1024_mod_1024(void);
void Toggle_this_mod_2_2_bitsets(void);
void Toggle_this_mod_8_2_bitsets(void);
void Toggle_this_mod_64_2_bitsets(void);
void Toggle_this_mod_256_2_bitsets(void);
void Toggle_this_mod_1024_2_bitsets(void);
void Toggle_this_randomized_2_bitsets(void);
void Toggle_this_randomized_3_bitsets(void);
void Toggle_this_randomized_4_bitsets(void);
void Toggle_this_w_other_tag(void);
void Toggle_this_w_other_component(void);
void Toggle_this_not(void);
void Toggle_this_written_not_1024_mod_2(void);
void Toggle_this_written_not_1024_mod_3(void);
void Toggle_this_written_not_1024_mod_7(void);
void Toggle_this_written_not_1024_mod_8(void);
void Toggle_this_written_not_1024_mod_10(void);
void Toggle_this_written_not_1024_mod_64(void);
void Toggle_this_written_not_1024_mod_256(void);
void Toggle_this_written_not_1024_mod_1024(void);
void Toggle_this_optional(void);
void Toggle_this_written_optional_1024_mod_2(void);
void Toggle_this_written_optional_1024_mod_3(void);
void Toggle_this_written_optional_1024_mod_7(void);
void Toggle_this_written_optional_1024_mod_8(void);
void Toggle_this_written_optional_1024_mod_10(void);
void Toggle_this_written_optional_1024_mod_64(void);
void Toggle_this_written_optional_1024_mod_256(void);
void Toggle_this_written_optional_1024_mod_1024(void);
void Toggle_this_written_toggle_w_not_toggle(void);
void Toggle_this_written_not_toggle_w_toggle(void);
void Toggle_this_written_toggle_w_optional_toggle(void);
void Toggle_this_written_optional_toggle_w_toggle(void);
void Toggle_this_written_not_w_optional_toggle(void);
void Toggle_this_written_optional_w_not_toggle(void);
void Toggle_this_written_2_not_toggle(void);
void Toggle_this_written_2_optional_toggle(void);
void Toggle_this_written_toggle_w_2_not_toggle(void);
void Toggle_this_written_toggle_w_2_optional_toggle(void);
void Toggle_this_written_2_toggle_w_not_toggle(void);
void Toggle_this_written_2_toggle_w_optional_toggle(void);
void Toggle_this_sort(void);
void Toggle_this_table_move_2_from_3(void);
void Toggle_toggle_0_src_only_term(void);
void Toggle_toggle_0_src(void);

// Testsuite 'Sparse'
void Sparse_setup(void);
void Sparse_1_fixed_sparse(void);
void Sparse_1_fixed_sparse_none(void);
void Sparse_1_this_sparse(void);
void Sparse_1_this_sparse_none(void);
void Sparse_1_this_sparse_written(void);
void Sparse_1_this_sparse_written_none(void);
void Sparse_1_var_sparse(void);
void Sparse_1_var_sparse_none(void);
void Sparse_1_var_sparse_written(void);
void Sparse_1_var_sparse_written_none(void);
void Sparse_2_sparse(void);
void Sparse_2_sparse_and_regular(void);
void Sparse_2_regular_and_sparse(void);
void Sparse_1_sparse_self(void);
void Sparse_1_sparse_up(void);
void Sparse_1_sparse_self_up(void);
void Sparse_1_sparse_written_self(void);
void Sparse_1_sparse_written_up(void);
void Sparse_1_sparse_written_self_up(void);
void Sparse_sparse_0_src_only_term(void);
void Sparse_sparse_0_src(void);

// Testsuite 'Union'
void Union_setup(void);
void Union_1_fixed_union_any(void);
void Union_1_fixed_union_wildcard(void);
void Union_1_fixed_union_tgt(void);
void Union_1_fixed_union_tgt_var(void);
void Union_1_fixed_union_tgt_var_written(void);
void Union_1_this_union_any(void);
void Union_1_this_union_wildcard(void);
void Union_1_this_union_tgt(void);
void Union_1_this_union_tgt_var(void);
void Union_1_this_union_tgt_var_written(void);
void Union_1_var_union_any(void);
void Union_1_var_union_wildcard(void);
void Union_1_var_union_tgt(void);
void Union_1_var_union_tgt_var(void);
void Union_1_var_union_tgt_var_written(void);
void Union_1_this_written_union_any(void);
void Union_1_this_written_union_wildcard(void);
void Union_1_this_written_union_tgt(void);
void Union_1_this_written_union_tgt_var(void);
void Union_1_this_written_union_tgt_var_written(void);
void Union_1_var_written_union_any(void);
void Union_1_var_written_union_wildcard(void);
void Union_1_var_written_union_tgt(void);
void Union_1_var_written_union_tgt_var(void);
void Union_1_var_written_union_tgt_var_written(void);
void Union_not_fixed_union_any(void);
void Union_not_fixed_union_wildcard(void);
void Union_not_fixed_union_tgt(void);
void Union_not_fixed_union_var(void);
void Union_not_fixed_union_var_written(void);
void Union_not_this_written_union_any(void);
void Union_not_this_written_union_wildcard(void);
void Union_not_this_written_union_tgt(void);
void Union_not_this_written_union_var(void);
void Union_not_this_written_union_var_written(void);
void Union_query_switch(void);
void Union_query_1_case_1_type(void);
void Union_query_1_case_2_types(void);
void Union_query_2_cases_1_type(void);
void Union_query_2_cases_2_types(void);
void Union_query_after_remove(void);
void Union_sort(void);
void Union_query_recycled_tags(void);
void Union_query_single_case(void);
void Union_match_switch_on_base_instance(void);
void Union_switch_w_bitset_query(void);
void Union_switch_w_bitset_query_inv(void);
void Union_switch_w_bitset_query_2_elems(void);
void Union_switch_w_bitset_query_2_elems_skip(void);
void Union_switch_w_bitset_query_elems_interleaved(void);
void Union_switch_w_bitset_query_elems_interleaved_2_types(void);
void Union_component_relation(void);
void Union_switch_term_filter(void);
void Union_2_terms_switch_term_filter(void);
void Union_match_switch_w_switch(void);
void Union_match_switch_w_case(void);
void Union_match_switch_w_case_2_terms(void);
void Union_up(void);
void Union_self_up(void);
void Union_up_written(void);
void Union_self_up_written(void);
void Union_existing_union_table(void);
void Union_new_union_table(void);
void Union_existing_union_table_w_tgt(void);
void Union_new_union_table_w_tgt(void);
void Union_tgt_w_generation(void);
void Union_tgt_w_not_alive(void);
void Union_for_switch_filter_term(void);
void Union_union_from_nothing(void);
void Union_union_tgt_from_nothing(void);
void Union_tgt_inherited(void);

// Testsuite 'OrderBy'
void OrderBy_sort_by_component(void);
void OrderBy_sort_by_component_2_tables(void);
void OrderBy_sort_by_component_3_tables(void);
void OrderBy_sort_by_entity(void);
void OrderBy_sort_after_add(void);
void OrderBy_sort_after_remove(void);
void OrderBy_sort_after_delete(void);
void OrderBy_sort_after_set(void);
void OrderBy_sort_after_system(void);
void OrderBy_sort_after_query(void);
void OrderBy_sort_by_component_same_value_1(void);
void OrderBy_sort_by_component_same_value_2(void);
void OrderBy_sort_by_component_move_pivot(void);
void OrderBy_sort_1000_entities(void);
void OrderBy_sort_1000_entities_w_duplicates(void);
void OrderBy_sort_1000_entities_again(void);
void OrderBy_sort_1000_entities_2_types(void);
void OrderBy_sort_1500_entities_3_types(void);
void OrderBy_sort_2000_entities_4_types(void);
void OrderBy_sort_2_entities_2_types(void);
void OrderBy_sort_3_entities_3_types(void);
void OrderBy_sort_3_entities_3_types_2(void);
void OrderBy_sort_4_entities_4_types(void);
void OrderBy_sort_1000_entities_2_types_again(void);
void OrderBy_sort_1000_entities_add_type_after_sort(void);
void OrderBy_sort_shared_component(void);
void OrderBy_sort_shared_component_childof(void);
void OrderBy_sort_w_tags_only(void);
void OrderBy_sort_childof_marked(void);
void OrderBy_sort_isa_marked(void);
void OrderBy_sort_relation_marked(void);
void OrderBy_dont_resort_after_set_unsorted_component(void);
void OrderBy_dont_resort_after_set_unsorted_component_w_tag(void);
void OrderBy_dont_resort_after_set_unsorted_component_w_tag_w_out_term(void);
void OrderBy_sort_component_not_queried_for(void);
void OrderBy_sort_by_wildcard(void);
void OrderBy_sort_shared_w_delete(void);
void OrderBy_sort_w_nontrivial_component(void);
void OrderBy_sort_by_wildcard(void);
void OrderBy_sort_not_term(void);
void OrderBy_sort_or_term(void);
void OrderBy_sort_optional_term(void);

// Testsuite 'OrderByEntireTable'
void OrderByEntireTable_sort_by_component(void);
void OrderByEntireTable_sort_by_component_2_tables(void);
void OrderByEntireTable_sort_by_component_3_tables(void);
void OrderByEntireTable_sort_by_entity(void);
void OrderByEntireTable_sort_after_add(void);
void OrderByEntireTable_sort_after_remove(void);
void OrderByEntireTable_sort_after_delete(void);
void OrderByEntireTable_sort_after_set(void);
void OrderByEntireTable_sort_after_system(void);
void OrderByEntireTable_sort_after_query(void);
void OrderByEntireTable_sort_by_component_same_value_1(void);
void OrderByEntireTable_sort_by_component_same_value_2(void);
void OrderByEntireTable_sort_by_component_move_pivot(void);
void OrderByEntireTable_sort_1000_entities(void);
void OrderByEntireTable_sort_1000_entities_w_duplicates(void);
void OrderByEntireTable_sort_1000_entities_again(void);
void OrderByEntireTable_sort_1000_entities_2_types(void);
void OrderByEntireTable_sort_1500_entities_3_types(void);
void OrderByEntireTable_sort_2000_entities_4_types(void);
void OrderByEntireTable_sort_2_entities_2_types(void);
void OrderByEntireTable_sort_3_entities_3_types(void);
void OrderByEntireTable_sort_3_entities_3_types_2(void);
void OrderByEntireTable_sort_4_entities_4_types(void);
void OrderByEntireTable_sort_1000_entities_2_types_again(void);
void OrderByEntireTable_sort_1000_entities_add_type_after_sort(void);
void OrderByEntireTable_sort_shared_component(void);
void OrderByEntireTable_sort_w_tags_only(void);
void OrderByEntireTable_sort_childof_marked(void);
void OrderByEntireTable_sort_isa_marked(void);
void OrderByEntireTable_sort_relation_marked(void);
void OrderByEntireTable_dont_resort_after_set_unsorted_component(void);
void OrderByEntireTable_dont_resort_after_set_unsorted_component_w_tag(void);
void OrderByEntireTable_dont_resort_after_set_unsorted_component_w_tag_w_out_term(void);
void OrderByEntireTable_sort_shared_w_delete(void);
void OrderByEntireTable_sort_not_term(void);
void OrderByEntireTable_sort_or_term(void);
void OrderByEntireTable_sort_optional_term(void);

// Testsuite 'QueryStr'
void QueryStr_one_term(void);
void QueryStr_one_term_w_inout(void);
void QueryStr_two_terms(void);
void QueryStr_two_terms_w_inout(void);
void QueryStr_three_terms_w_or(void);
void QueryStr_three_terms_w_or_inout(void);
void QueryStr_four_terms_three_w_or_inout(void);
void QueryStr_one_term_w_pair(void);
void QueryStr_one_term_w_pair_entity_src(void);
void QueryStr_one_term_w_self(void);
void QueryStr_one_term_w_up(void);
void QueryStr_one_term_w_self_up(void);
void QueryStr_one_term_w_cascade(void);
void QueryStr_one_term_w_0(void);
void QueryStr_one_term_w_singleton(void);
void QueryStr_one_term_w_final_pair(void);
void QueryStr_one_term_w_final_dont_inherit(void);
void QueryStr_one_term_w_final_inherit(void);
void QueryStr_one_term_w_final_override(void);
void QueryStr_one_term_w_src_var(void);
void QueryStr_one_term_w_first_var(void);
void QueryStr_one_term_w_second_var(void);
void QueryStr_one_term_w_first_var_entity_src(void);
void QueryStr_one_term_w_pair_w_0_entity(void);
void QueryStr_not_term(void);
void QueryStr_wildcard_term(void);
void QueryStr_scopes(void);
void QueryStr_pred_eq(void);
void QueryStr_pred_neq(void);
void QueryStr_pred_eq_name(void);
void QueryStr_pred_neq_name(void);
void QueryStr_pred_eq_m(void);
void QueryStr_pred_neq_m(void);

bake_test_case Validator_testcases[] = {
    {
        "validate_1_term",
        Validator_validate_1_term
    },
    {
        "validate_1_term_component",
        Validator_validate_1_term_component
    },
    {
        "validate_2_terms",
        Validator_validate_2_terms
    },
    {
        "validate_3_terms",
        Validator_validate_3_terms
    },
    {
        "validate_3_terms_w_or",
        Validator_validate_3_terms_w_or
    },
    {
        "validate_4_terms_w_or_at_1",
        Validator_validate_4_terms_w_or_at_1
    },
    {
        "validate_1_term_wildcard",
        Validator_validate_1_term_wildcard
    },
    {
        "validate_1_term_any",
        Validator_validate_1_term_any
    },
    {
        "validate_1_term_same_subj_obj",
        Validator_validate_1_term_same_subj_obj
    },
    {
        "validate_1_term_acyclic_same_subj_obj",
        Validator_validate_1_term_acyclic_same_subj_obj
    },
    {
        "validate_1_term_acyclic_reflexive_same_subj_obj",
        Validator_validate_1_term_acyclic_reflexive_same_subj_obj
    },
    {
        "validate_1_term_same_subj_obj_var",
        Validator_validate_1_term_same_subj_obj_var
    },
    {
        "validate_1_term_acyclic_same_subj_obj_var",
        Validator_validate_1_term_acyclic_same_subj_obj_var
    },
    {
        "validate_1_term_acyclic_reflexive_same_subj_obj_var",
        Validator_validate_1_term_acyclic_reflexive_same_subj_obj_var
    },
    {
        "validate_1_term_non_acyclic_superset",
        Validator_validate_1_term_non_acyclic_superset
    },
    {
        "validate_1_term_dont_inherit_default_set",
        Validator_validate_1_term_dont_inherit_default_set
    },
    {
        "validate_1_term_dont_inherit_pair_default_set",
        Validator_validate_1_term_dont_inherit_pair_default_set
    },
    {
        "validate_1_term_inherit_default_set",
        Validator_validate_1_term_inherit_default_set
    },
    {
        "validate_1_term_inherit_pair_default_set",
        Validator_validate_1_term_inherit_pair_default_set
    },
    {
        "validate_1_term_override_default_set",
        Validator_validate_1_term_override_default_set
    },
    {
        "validate_1_term_override_pair_default_set",
        Validator_validate_1_term_override_pair_default_set
    },
    {
        "validate_1_term_up_no_inherit",
        Validator_validate_1_term_up_no_inherit
    },
    {
        "validate_1_term_up_no_inherit_pair",
        Validator_validate_1_term_up_no_inherit_pair
    },
    {
        "validate_1_term_up_override",
        Validator_validate_1_term_up_override
    },
    {
        "validate_1_term_up_override_pair",
        Validator_validate_1_term_up_override_pair
    },
    {
        "validate_1_term_up_isa_no_inherit",
        Validator_validate_1_term_up_isa_no_inherit
    },
    {
        "validate_1_term_up_isa_no_inherit_pair",
        Validator_validate_1_term_up_isa_no_inherit_pair
    },
    {
        "validate_1_term_up_isa_override",
        Validator_validate_1_term_up_isa_override
    },
    {
        "validate_1_term_up_isa_override_pair",
        Validator_validate_1_term_up_isa_override_pair
    },
    {
        "validate_1_term_cascade_implicit_trav",
        Validator_validate_1_term_cascade_implicit_trav
    },
    {
        "validate_1_term_cascade_isa",
        Validator_validate_1_term_cascade_isa
    },
    {
        "validate_1_term_cascade_childof",
        Validator_validate_1_term_cascade_childof
    },
    {
        "validate_1_term_cascade_down",
        Validator_validate_1_term_cascade_down
    },
    {
        "validate_1_term_optional_only",
        Validator_validate_1_term_optional_only
    },
    {
        "validate_1_term_transitive_pair",
        Validator_validate_1_term_transitive_pair
    },
    {
        "validate_1_variable_as_pred_only",
        Validator_validate_1_variable_as_pred_only
    },
    {
        "validate_1_variable_as_pred_w_subj",
        Validator_validate_1_variable_as_pred_w_subj
    },
    {
        "validate_1_variable_as_pred_w_pair",
        Validator_validate_1_variable_as_pred_w_pair
    },
    {
        "validate_1_variable_as_subj",
        Validator_validate_1_variable_as_subj
    },
    {
        "validate_1_variable_as_obj",
        Validator_validate_1_variable_as_obj
    },
    {
        "validate_2_terms_or_w_dontinherit",
        Validator_validate_2_terms_or_w_dontinherit
    },
    {
        "validate_2_terms_or_w_both_dontinherit",
        Validator_validate_2_terms_or_w_both_dontinherit
    },
    {
        "validate_w_pair_id",
        Validator_validate_w_pair_id
    },
    {
        "validate_w_pred_obj",
        Validator_validate_w_pred_obj
    },
    {
        "validate_w_pair_id_and_subj",
        Validator_validate_w_pair_id_and_subj
    },
    {
        "validate_1_w_pred_name",
        Validator_validate_1_w_pred_name
    },
    {
        "validate_1_w_final_pred_name",
        Validator_validate_1_w_final_pred_name
    },
    {
        "validate_1_w_subj_name",
        Validator_validate_1_w_subj_name
    },
    {
        "validate_1_w_obj_name",
        Validator_validate_1_w_obj_name
    },
    {
        "validate_w_this_implicit_variable",
        Validator_validate_w_this_implicit_variable
    },
    {
        "validate_w_this_explicit_entity",
        Validator_validate_w_this_explicit_entity
    },
    {
        "validate_w_first_this_implicit_variable",
        Validator_validate_w_first_this_implicit_variable
    },
    {
        "validate_w_first_this_explicit_entity",
        Validator_validate_w_first_this_explicit_entity
    },
    {
        "validate_w_second_this_implicit_variable",
        Validator_validate_w_second_this_implicit_variable
    },
    {
        "validate_w_second_this_explicit_entity",
        Validator_validate_w_second_this_explicit_entity
    },
    {
        "validate_w_this_variable_name",
        Validator_validate_w_this_variable_name
    },
    {
        "validate_w_src_var",
        Validator_validate_w_src_var
    },
    {
        "validate_w_first_var",
        Validator_validate_w_first_var
    },
    {
        "validate_w_second_var",
        Validator_validate_w_second_var
    },
    {
        "validate_w_src_var_from_name",
        Validator_validate_w_src_var_from_name
    },
    {
        "validate_w_first_first_var",
        Validator_validate_w_first_first_var
    },
    {
        "validate_w_second_second_var",
        Validator_validate_w_second_second_var
    },
    {
        "validate_w_0_source",
        Validator_validate_w_0_source
    },
    {
        "validate_w_0_target",
        Validator_validate_w_0_target
    },
    {
        "validate_2_terms_w_or",
        Validator_validate_2_terms_w_or
    },
    {
        "validate_2_terms_w_or_mixed_src_flags",
        Validator_validate_2_terms_w_or_mixed_src_flags
    },
    {
        "validate_2_terms_w_or_mixed_src_id",
        Validator_validate_2_terms_w_or_mixed_src_id
    },
    {
        "validate_2_terms_w_or_mixed_src_name",
        Validator_validate_2_terms_w_or_mixed_src_name
    },
    {
        "validate_2_terms_w_or_same_src_w_id_and_name",
        Validator_validate_2_terms_w_or_same_src_w_id_and_name
    },
    {
        "validate_w_and_flag",
        Validator_validate_w_and_flag
    },
    {
        "validate_w_or_flag",
        Validator_validate_w_or_flag
    },
    {
        "validate_w_not_flag",
        Validator_validate_w_not_flag
    },
    {
        "validate_filter",
        Validator_validate_filter
    },
    {
        "validate_double_init",
        Validator_validate_double_init
    },
    {
        "validate_double_init_w_expr",
        Validator_validate_double_init_w_expr
    },
    {
        "validate_double_init_w_expr_optional",
        Validator_validate_double_init_w_expr_optional
    },
    {
        "validate_w_tag_term_is_no_data",
        Validator_validate_w_tag_term_is_no_data
    },
    {
        "validate_w_inout_none_term_is_no_data",
        Validator_validate_w_inout_none_term_is_no_data
    },
    {
        "validate_w_tag_and_inout_none_term_is_no_data",
        Validator_validate_w_tag_and_inout_none_term_is_no_data
    },
    {
        "validate_w_not_term_is_no_data",
        Validator_validate_w_not_term_is_no_data
    },
    {
        "validate_w_no_transitive_pair",
        Validator_validate_w_no_transitive_pair
    },
    {
        "validate_w_transitive_pair_any_src",
        Validator_validate_w_transitive_pair_any_src
    },
    {
        "validate_w_transitive_pair",
        Validator_validate_w_transitive_pair
    },
    {
        "validate_w_transitive_tag_no_pair",
        Validator_validate_w_transitive_tag_no_pair
    },
    {
        "validate_w_transitive_tag_self_tgt",
        Validator_validate_w_transitive_tag_self_tgt
    },
    {
        "validate_w_transitive_tag_any_tgt",
        Validator_validate_w_transitive_tag_any_tgt
    },
    {
        "validate_w_pair_same_vars",
        Validator_validate_w_pair_same_vars
    },
    {
        "validate_w_pair_not_same_vars",
        Validator_validate_w_pair_not_same_vars
    },
    {
        "validate_w_pair_no_vars_not_same_vars",
        Validator_validate_w_pair_no_vars_not_same_vars
    },
    {
        "validate_w_pair_wildcard_not_same_vars",
        Validator_validate_w_pair_wildcard_not_same_vars
    },
    {
        "validate_w_pair_any_not_same_vars",
        Validator_validate_w_pair_any_not_same_vars
    },
    {
        "validate_w_no_pair_not_same_vars",
        Validator_validate_w_no_pair_not_same_vars
    },
    {
        "validate_not_childof_any",
        Validator_validate_not_childof_any
    },
    {
        "validate_w_inherited_id",
        Validator_validate_w_inherited_id
    },
    {
        "validate_w_inherited_pair",
        Validator_validate_w_inherited_pair
    },
    {
        "validate_w_non_inherited_id",
        Validator_validate_w_non_inherited_id
    },
    {
        "validate_w_non_inherited_pair",
        Validator_validate_w_non_inherited_pair
    },
    {
        "validate_w_first_wildcard_inout_none",
        Validator_validate_w_first_wildcard_inout_none
    },
    {
        "validate_w_first_var_inout_none",
        Validator_validate_w_first_var_inout_none
    },
    {
        "validate_w_pair_wildcard_inout_none",
        Validator_validate_w_pair_wildcard_inout_none
    },
    {
        "validate_w_pair_var_inout_none",
        Validator_validate_w_pair_var_inout_none
    },
    {
        "validate_w_unresolved_by_name",
        Validator_validate_w_unresolved_by_name
    },
    {
        "validate_w_unresolved_by_name_eq",
        Validator_validate_w_unresolved_by_name_eq
    },
    {
        "validate_childof_this",
        Validator_validate_childof_this
    },
    {
        "validate_childof_this_entity",
        Validator_validate_childof_this_entity
    },
    {
        "validate_childof_this_by_id",
        Validator_validate_childof_this_by_id
    },
    {
        "validate_filter_flag",
        Validator_validate_filter_flag
    },
    {
        "validate_first_0_name",
        Validator_validate_first_0_name
    },
    {
        "validate_src_0_name",
        Validator_validate_src_0_name
    },
    {
        "validate_second_0_name",
        Validator_validate_second_0_name
    },
    {
        "validate_singleton_src_w_first_name",
        Validator_validate_singleton_src_w_first_name
    },
    {
        "validate_singleton_second_w_first_name",
        Validator_validate_singleton_second_w_first_name
    },
    {
        "not_wildcard",
        Validator_not_wildcard
    },
    {
        "not_first_wildcard",
        Validator_not_first_wildcard
    },
    {
        "not_second_wildcard",
        Validator_not_second_wildcard
    },
    {
        "not_wildcard_id",
        Validator_not_wildcard_id
    },
    {
        "not_wildcard_first_pair",
        Validator_not_wildcard_first_pair
    },
    {
        "not_wildcard_second_pair",
        Validator_not_wildcard_second_pair
    },
    {
        "validate_or_same_type",
        Validator_validate_or_same_type
    },
    {
        "validate_or_different_types",
        Validator_validate_or_different_types
    },
    {
        "validate_or_different_types",
        Validator_validate_or_different_types
    },
    {
        "validate_or_different_types_1_and_2_or",
        Validator_validate_or_different_types_1_and_2_or
    },
    {
        "validate_trav_isa_w_wildcard",
        Validator_validate_trav_isa_w_wildcard
    },
    {
        "validate_trav_isa_w_any",
        Validator_validate_trav_isa_w_any
    },
    {
        "validate_custom_trav_w_inherit_id",
        Validator_validate_custom_trav_w_inherit_id
    },
    {
        "validate_custom_trav_w_inherit_id_w_self_up",
        Validator_validate_custom_trav_w_inherit_id_w_self_up
    },
    {
        "validate_custom_trav_w_inherit_id_w_up",
        Validator_validate_custom_trav_w_inherit_id_w_up
    },
    {
        "validate_simple_1_term_is_cacheable",
        Validator_validate_simple_1_term_is_cacheable
    },
    {
        "validate_simple_1_term_pair_is_cacheable",
        Validator_validate_simple_1_term_pair_is_cacheable
    },
    {
        "validate_simple_1_term_pair_recycled_is_cacheable",
        Validator_validate_simple_1_term_pair_recycled_is_cacheable
    },
    {
        "validate_simple_1_term_tag_is_cacheable",
        Validator_validate_simple_1_term_tag_is_cacheable
    },
    {
        "validate_simple_2_term_is_cacheable",
        Validator_validate_simple_2_term_is_cacheable
    },
    {
        "validate_simple_w_can_inherit",
        Validator_validate_simple_w_can_inherit
    },
    {
        "validate_simple_w_can_toggle",
        Validator_validate_simple_w_can_toggle
    },
    {
        "validate_simple_w_sparse",
        Validator_validate_simple_w_sparse
    },
    {
        "validate_simple_w_union",
        Validator_validate_simple_w_union
    },
    {
        "validate_simple_w_union_pair",
        Validator_validate_simple_w_union_pair
    },
    {
        "validate_simple_w_transitive",
        Validator_validate_simple_w_transitive
    },
    {
        "validate_simple_w_transitive_pair",
        Validator_validate_simple_w_transitive_pair
    },
    {
        "validate_simple_w_reflexive",
        Validator_validate_simple_w_reflexive
    },
    {
        "validate_simple_w_reflexive_pair",
        Validator_validate_simple_w_reflexive_pair
    },
    {
        "validate_simple_w_inherited_component",
        Validator_validate_simple_w_inherited_component
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
        "pair_implicit_subject_0_name_object",
        Parser_pair_implicit_subject_0_name_object
    },
    {
        "pair_implicit_subject_0_name_not_found_object",
        Parser_pair_implicit_subject_0_name_not_found_object
    },
    {
        "pair_implicit_subject_0_digit_object",
        Parser_pair_implicit_subject_0_digit_object
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
        "inout_w_not_operator",
        Parser_inout_w_not_operator
    },
    {
        "inout_w_optional_operator",
        Parser_inout_w_optional_operator
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
        "2_or_no_space",
        Parser_2_or_no_space
    },
    {
        "2_or_w_not_1st_arg",
        Parser_2_or_w_not_1st_arg
    },
    {
        "2_or_w_not_2nd_arg",
        Parser_2_or_w_not_2nd_arg
    },
    {
        "2_or_w_optional_1st_arg",
        Parser_2_or_w_optional_1st_arg
    },
    {
        "2_or_w_optional_2nd_arg",
        Parser_2_or_w_optional_2nd_arg
    },
    {
        "1_entity_id_pred_implicit_subj",
        Parser_1_entity_id_pred_implicit_subj
    },
    {
        "1_entity_id_pred_no_subj",
        Parser_1_entity_id_pred_no_subj
    },
    {
        "1_entity_id_pred_explicit_subj",
        Parser_1_entity_id_pred_explicit_subj
    },
    {
        "1_entity_id_pair_implicit_subj",
        Parser_1_entity_id_pair_implicit_subj
    },
    {
        "1_entity_id_pair_explicit_subj",
        Parser_1_entity_id_pair_explicit_subj
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
        "pred_implicit_subject_superset_inclusive",
        Parser_pred_implicit_subject_superset_inclusive
    },
    {
        "pred_implicit_subject_superset_cascade",
        Parser_pred_implicit_subject_superset_cascade
    },
    {
        "pred_implicit_subject_superset_inclusive_cascade",
        Parser_pred_implicit_subject_superset_inclusive_cascade
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
        "pred_entity_no_args_w_explicit_self",
        Parser_pred_entity_no_args_w_explicit_self
    },
    {
        "pred_entity_no_args_w_explicit_self_superset",
        Parser_pred_entity_no_args_w_explicit_self_superset
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
        "template_type_nested",
        Parser_template_type_nested
    },
    {
        "template_type_multiple_args",
        Parser_template_type_multiple_args
    },
    {
        "template_type_multiple_args_nested",
        Parser_template_type_multiple_args_nested
    },
    {
        "template_type_unbalanced_open",
        Parser_template_type_unbalanced_open
    },
    {
        "template_type_unbalanced_close",
        Parser_template_type_unbalanced_close
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
        "eq_var_table",
        Parser_eq_var_table
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
        "neq_var_table",
        Parser_neq_var_table
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
        "eq_w_not",
        Parser_eq_w_not
    },
    {
        "neq_w_not",
        Parser_neq_w_not
    },
    {
        "match_w_not",
        Parser_match_w_not
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
        "eq_0",
        Parser_eq_0
    },
    {
        "neq_0",
        Parser_neq_0
    },
    {
        "eq_wildcard",
        Parser_eq_wildcard
    },
    {
        "neq_wildcard",
        Parser_neq_wildcard
    },
    {
        "eq_any",
        Parser_eq_any
    },
    {
        "neq_any",
        Parser_neq_any
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
        "query_term_after_scope",
        Parser_query_term_after_scope
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
        "pair_3_args_w_vars",
        Parser_pair_3_args_w_vars
    },
    {
        "pair_4_args_w_vars",
        Parser_pair_4_args_w_vars
    },
    {
        "pair_3_args_w_vars_w_term_after",
        Parser_pair_3_args_w_vars_w_term_after
    },
    {
        "pair_4_args_w_vars_w_term_after",
        Parser_pair_4_args_w_vars_w_term_after
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
    },
    {
        "empty_term",
        Parser_empty_term
    },
    {
        "empty_term_w_space",
        Parser_empty_term_w_space
    },
    {
        "empty_term_w_newline",
        Parser_empty_term_w_newline
    },
    {
        "mixed_1_desc_and_1_expr",
        Parser_mixed_1_desc_and_1_expr
    },
    {
        "mixed_2_desc_and_1_expr",
        Parser_mixed_2_desc_and_1_expr
    },
    {
        "mixed_1_desc_and_2_expr",
        Parser_mixed_1_desc_and_2_expr
    },
    {
        "not_wildcard",
        Parser_not_wildcard
    },
    {
        "not_first_wildcard",
        Parser_not_first_wildcard
    },
    {
        "not_second_wildcard",
        Parser_not_second_wildcard
    },
    {
        "unterminated_paren",
        Parser_unterminated_paren
    },
    {
        "unterminated_after_first_paren",
        Parser_unterminated_after_first_paren
    },
    {
        "unterminated_after_src_var",
        Parser_unterminated_after_src_var
    },
    {
        "eq_name_existing_entity",
        Parser_eq_name_existing_entity
    },
    {
        "match_existing_entity",
        Parser_match_existing_entity
    },
    {
        "match_wildcard",
        Parser_match_wildcard
    },
    {
        "match_any",
        Parser_match_any
    },
    {
        "match_variable_oper",
        Parser_match_variable_oper
    },
    {
        "escaped_identifier",
        Parser_escaped_identifier
    },
    {
        "escaped_identifier_first",
        Parser_escaped_identifier_first
    },
    {
        "escaped_identifier_second",
        Parser_escaped_identifier_second
    },
    {
        "n_tokens_test",
        Parser_n_tokens_test
    }
};

bake_test_case Basic_testcases[] = {
    {
        "0_query",
        Basic_0_query
    },
    {
        "1_fact_w_tag",
        Basic_1_fact_w_tag
    },
    {
        "1_fact_w_component",
        Basic_1_fact_w_component
    },
    {
        "1_fact_w_tag_pair",
        Basic_1_fact_w_tag_pair
    },
    {
        "1_fact_w_component_pair",
        Basic_1_fact_w_component_pair
    },
    {
        "2_facts_same_src_w_tag",
        Basic_2_facts_same_src_w_tag
    },
    {
        "2_facts_same_src_w_component",
        Basic_2_facts_same_src_w_component
    },
    {
        "2_facts_same_src_w_tag_pair",
        Basic_2_facts_same_src_w_tag_pair
    },
    {
        "2_facts_same_src_w_component_pair",
        Basic_2_facts_same_src_w_component_pair
    },
    {
        "2_facts_other_src_w_tag",
        Basic_2_facts_other_src_w_tag
    },
    {
        "2_facts_other_src_w_component",
        Basic_2_facts_other_src_w_component
    },
    {
        "2_facts_other_src_w_tag_pair",
        Basic_2_facts_other_src_w_tag_pair
    },
    {
        "2_facts_other_src_w_component_pair",
        Basic_2_facts_other_src_w_component_pair
    },
    {
        "1_fact_w_any",
        Basic_1_fact_w_any
    },
    {
        "1_fact_w_pair_any_tgt",
        Basic_1_fact_w_pair_any_tgt
    },
    {
        "1_fact_w_pair_any_rel",
        Basic_1_fact_w_pair_any_rel
    },
    {
        "1_fact_w_pair_any_rel_tgt",
        Basic_1_fact_w_pair_any_rel_tgt
    },
    {
        "2_facts_same_src_w_any",
        Basic_2_facts_same_src_w_any
    },
    {
        "2_facts_same_src_w_pair_any_tgt",
        Basic_2_facts_same_src_w_pair_any_tgt
    },
    {
        "2_facts_same_src_w_pair_any_rel",
        Basic_2_facts_same_src_w_pair_any_rel
    },
    {
        "2_facts_same_src_w_pair_any_rel_tgt",
        Basic_2_facts_same_src_w_pair_any_rel_tgt
    },
    {
        "2_facts_other_src_w_any",
        Basic_2_facts_other_src_w_any
    },
    {
        "2_facts_other_src_w_pair_any_tgt",
        Basic_2_facts_other_src_w_pair_any_tgt
    },
    {
        "2_facts_other_src_w_pair_any_rel",
        Basic_2_facts_other_src_w_pair_any_rel
    },
    {
        "2_facts_other_src_w_pair_any_rel_tgt",
        Basic_2_facts_other_src_w_pair_any_rel_tgt
    },
    {
        "1_this_src_w_tag",
        Basic_1_this_src_w_tag
    },
    {
        "1_this_src_w_component",
        Basic_1_this_src_w_component
    },
    {
        "1_this_src_w_tag_pair",
        Basic_1_this_src_w_tag_pair
    },
    {
        "1_this_src_w_component_pair",
        Basic_1_this_src_w_component_pair
    },
    {
        "1_this_src_w_tag_2_tables",
        Basic_1_this_src_w_tag_2_tables
    },
    {
        "1_this_src_w_component_2_tables",
        Basic_1_this_src_w_component_2_tables
    },
    {
        "1_this_src_w_tag_pair_2_tables",
        Basic_1_this_src_w_tag_pair_2_tables
    },
    {
        "1_this_src_w_component_pair_2_tables",
        Basic_1_this_src_w_component_pair_2_tables
    },
    {
        "2_this_src_w_tag",
        Basic_2_this_src_w_tag
    },
    {
        "2_this_src_w_component",
        Basic_2_this_src_w_component
    },
    {
        "2_this_src_ent_src_w_tag",
        Basic_2_this_src_ent_src_w_tag
    },
    {
        "2_this_src_ent_src_w_component",
        Basic_2_this_src_ent_src_w_component
    },
    {
        "2_ent_src_this_src_w_tag",
        Basic_2_ent_src_this_src_w_tag
    },
    {
        "2_ent_src_this_src_w_component",
        Basic_2_ent_src_this_src_w_component
    },
    {
        "recycled_tag",
        Basic_recycled_tag
    },
    {
        "recycled_src",
        Basic_recycled_src
    },
    {
        "recycled_pair_rel",
        Basic_recycled_pair_rel
    },
    {
        "recycled_pair_tgt",
        Basic_recycled_pair_tgt
    },
    {
        "this_src_w_wildcard",
        Basic_this_src_w_wildcard
    },
    {
        "this_src_w_pair_rel_wildcard",
        Basic_this_src_w_pair_rel_wildcard
    },
    {
        "this_src_w_pair_tgt_wildcard",
        Basic_this_src_w_pair_tgt_wildcard
    },
    {
        "this_src_w_pair_rel_tgt_wildcard",
        Basic_this_src_w_pair_rel_tgt_wildcard
    },
    {
        "this_src_w_any",
        Basic_this_src_w_any
    },
    {
        "this_src_w_any_written",
        Basic_this_src_w_any_written
    },
    {
        "this_src_w_pair_rel_any",
        Basic_this_src_w_pair_rel_any
    },
    {
        "this_src_w_pair_tgt_any",
        Basic_this_src_w_pair_tgt_any
    },
    {
        "this_src_w_pair_rel_tgt_any",
        Basic_this_src_w_pair_rel_tgt_any
    },
    {
        "ent_src_w_wildcard",
        Basic_ent_src_w_wildcard
    },
    {
        "ent_src_w_pair_rel_wildcard",
        Basic_ent_src_w_pair_rel_wildcard
    },
    {
        "ent_src_w_pair_tgt_wildcard",
        Basic_ent_src_w_pair_tgt_wildcard
    },
    {
        "ent_src_w_pair_rel_tgt_wildcard",
        Basic_ent_src_w_pair_rel_tgt_wildcard
    },
    {
        "1_wildcard_src",
        Basic_1_wildcard_src
    },
    {
        "1_wildcard_src_w_pair",
        Basic_1_wildcard_src_w_pair
    },
    {
        "2_wildcard_src",
        Basic_2_wildcard_src
    },
    {
        "2_wildcard_src_w_pair",
        Basic_2_wildcard_src_w_pair
    },
    {
        "1_wildcard_src_w_pair_tgt_var",
        Basic_1_wildcard_src_w_pair_tgt_var
    },
    {
        "1_wildcard_src_w_pair_rel_var",
        Basic_1_wildcard_src_w_pair_rel_var
    },
    {
        "1_wildcard_src_w_pair_tgt_this",
        Basic_1_wildcard_src_w_pair_tgt_this
    },
    {
        "1_wildcard_src_w_pair_rel_this",
        Basic_1_wildcard_src_w_pair_rel_this
    },
    {
        "1_any_src",
        Basic_1_any_src
    },
    {
        "1_any_src_w_pair",
        Basic_1_any_src_w_pair
    },
    {
        "2_any_src",
        Basic_2_any_src
    },
    {
        "2_any_src_w_pair",
        Basic_2_any_src_w_pair
    },
    {
        "1_any_src_w_pair_tgt_var",
        Basic_1_any_src_w_pair_tgt_var
    },
    {
        "1_any_src_w_pair_rel_var",
        Basic_1_any_src_w_pair_rel_var
    },
    {
        "1_any_src_w_pair_tgt_this",
        Basic_1_any_src_w_pair_tgt_this
    },
    {
        "1_any_src_w_pair_rel_this",
        Basic_1_any_src_w_pair_rel_this
    },
    {
        "not_any",
        Basic_not_any
    },
    {
        "rule_w_iter_next",
        Basic_rule_w_iter_next
    },
    {
        "empty_rule",
        Basic_empty_rule
    },
    {
        "invalid_rule",
        Basic_invalid_rule
    },
    {
        "instanced_table_src",
        Basic_instanced_table_src
    },
    {
        "instanced_entity_src",
        Basic_instanced_entity_src
    },
    {
        "instanced_mixed_src",
        Basic_instanced_mixed_src
    },
    {
        "instanced_inherited",
        Basic_instanced_inherited
    },
    {
        "in_term",
        Basic_in_term
    },
    {
        "out_term",
        Basic_out_term
    },
    {
        "inout_term",
        Basic_inout_term
    },
    {
        "nodata_term",
        Basic_nodata_term
    },
    {
        "find_this_lowercase",
        Basic_find_this_lowercase
    },
    {
        "find_this_uppercase",
        Basic_find_this_uppercase
    },
    {
        "find_this_tgt_lowercase",
        Basic_find_this_tgt_lowercase
    },
    {
        "find_this_tgt_uppercase",
        Basic_find_this_tgt_uppercase
    },
    {
        "get_filter",
        Basic_get_filter
    },
    {
        "iter_empty_source",
        Basic_iter_empty_source
    },
    {
        "iter_empty_source_2_terms",
        Basic_iter_empty_source_2_terms
    },
    {
        "iter_empty_source_wildcard",
        Basic_iter_empty_source_wildcard
    },
    {
        "iter_empty_source_pair",
        Basic_iter_empty_source_pair
    },
    {
        "iter_empty_source_pair_wildcard",
        Basic_iter_empty_source_pair_wildcard
    },
    {
        "iter_empty_source_2_terms_pair",
        Basic_iter_empty_source_2_terms_pair
    },
    {
        "iter_empty_source_2_terms_mixed",
        Basic_iter_empty_source_2_terms_mixed
    },
    {
        "iter_empty_source_2_terms_mixed_pair",
        Basic_iter_empty_source_2_terms_mixed_pair
    },
    {
        "iter_empty_source_2_terms_mixed_pair_wildcard",
        Basic_iter_empty_source_2_terms_mixed_pair_wildcard
    },
    {
        "this_var_w_empty_entity",
        Basic_this_var_w_empty_entity
    },
    {
        "match_disabled",
        Basic_match_disabled
    },
    {
        "match_prefab",
        Basic_match_prefab
    },
    {
        "match_disabled_prefab",
        Basic_match_disabled_prefab
    },
    {
        "match_disabled_this_tgt",
        Basic_match_disabled_this_tgt
    },
    {
        "match_prefab_this_tgt",
        Basic_match_prefab_this_tgt
    },
    {
        "match_disabled_prefab_this_tgt",
        Basic_match_disabled_prefab_this_tgt
    },
    {
        "match_self_disabled",
        Basic_match_self_disabled
    },
    {
        "match_self_prefab",
        Basic_match_self_prefab
    },
    {
        "match_self_disabled_prefab",
        Basic_match_self_disabled_prefab
    },
    {
        "match_optional_disabled",
        Basic_match_optional_disabled
    },
    {
        "match_optional_prefab",
        Basic_match_optional_prefab
    },
    {
        "match_optional_disabled_prefab",
        Basic_match_optional_disabled_prefab
    },
    {
        "match_optional_disabled_this_tgt",
        Basic_match_optional_disabled_this_tgt
    },
    {
        "match_optional_prefab_this_tgt",
        Basic_match_optional_prefab_this_tgt
    },
    {
        "match_optional_disabled_prefab_this_tgt",
        Basic_match_optional_disabled_prefab_this_tgt
    },
    {
        "match_optional_self_disabled",
        Basic_match_optional_self_disabled
    },
    {
        "match_optional_self_prefab",
        Basic_match_optional_self_prefab
    },
    {
        "match_optional_self_disabled_prefab",
        Basic_match_optional_self_disabled_prefab
    },
    {
        "inout_none_first_term",
        Basic_inout_none_first_term
    },
    {
        "inout_none_first_term_self_up",
        Basic_inout_none_first_term_self_up
    },
    {
        "inout_none_second_term",
        Basic_inout_none_second_term
    },
    {
        "inout_none_second_term_self_up",
        Basic_inout_none_second_term_self_up
    },
    {
        "inout_none_singleton",
        Basic_inout_none_singleton
    },
    {
        "inout_none_singleton_w_or",
        Basic_inout_none_singleton_w_or
    },
    {
        "inout_none_component_w_or",
        Basic_inout_none_component_w_or
    },
    {
        "no_data_rule",
        Basic_no_data_rule
    },
    {
        "frame_offset",
        Basic_frame_offset
    },
    {
        "frame_offset_no_data",
        Basic_frame_offset_no_data
    },
    {
        "match_empty_tables",
        Basic_match_empty_tables
    },
    {
        "match_empty_tables_no_data",
        Basic_match_empty_tables_no_data
    },
    {
        "match_empty_tables_w_not",
        Basic_match_empty_tables_w_not
    },
    {
        "match_empty_tables_w_wildcard",
        Basic_match_empty_tables_w_wildcard
    },
    {
        "match_empty_tables_w_no_empty_tables",
        Basic_match_empty_tables_w_no_empty_tables
    },
    {
        "match_empty_tables_trivial",
        Basic_match_empty_tables_trivial
    },
    {
        "oneof_wildcard",
        Basic_oneof_wildcard
    },
    {
        "oneof_any",
        Basic_oneof_any
    },
    {
        "instanced_w_singleton",
        Basic_instanced_w_singleton
    },
    {
        "instanced_w_base",
        Basic_instanced_w_base
    },
    {
        "unknown_before_known",
        Basic_unknown_before_known
    },
    {
        "unknown_before_known_after_or",
        Basic_unknown_before_known_after_or
    },
    {
        "unknown_before_known_after_not",
        Basic_unknown_before_known_after_not
    },
    {
        "unknown_before_known_after_optional",
        Basic_unknown_before_known_after_optional
    },
    {
        "unknown_before_known_after_scope",
        Basic_unknown_before_known_after_scope
    },
    {
        "2_trivial_mixed_2_tables",
        Basic_2_trivial_mixed_2_tables
    },
    {
        "2_trivial_mixed_2_tables_component",
        Basic_2_trivial_mixed_2_tables_component
    },
    {
        "2_trivial_mixed_2_tables_wildcard",
        Basic_2_trivial_mixed_2_tables_wildcard
    },
    {
        "2_trivial_1_unused_id",
        Basic_2_trivial_1_unused_id
    },
    {
        "2_trivial_one_regular",
        Basic_2_trivial_one_regular
    },
    {
        "1_trivial_one_regular_one_trivial",
        Basic_1_trivial_one_regular_one_trivial
    },
    {
        "one_regular_2_trivial",
        Basic_one_regular_2_trivial
    },
    {
        "2_trivial_w_prefab",
        Basic_2_trivial_w_prefab
    },
    {
        "3_trivial_w_prefab",
        Basic_3_trivial_w_prefab
    },
    {
        "2_trivial_w_disabled",
        Basic_2_trivial_w_disabled
    },
    {
        "3_trivial_w_disabled",
        Basic_3_trivial_w_disabled
    },
    {
        "2_this_w_fixed_src",
        Basic_2_this_w_fixed_src
    },
    {
        "2_fixed_src_w_this",
        Basic_2_fixed_src_w_this
    },
    {
        "2_this_w_fixed_src_no_match_fixed",
        Basic_2_this_w_fixed_src_no_match_fixed
    },
    {
        "2_fixed_src_w_this_no_match_fixed",
        Basic_2_fixed_src_w_this_no_match_fixed
    },
    {
        "2_this_w_fixed_src_no_match_this",
        Basic_2_this_w_fixed_src_no_match_this
    },
    {
        "2_fixed_src_w_this_no_match_this",
        Basic_2_fixed_src_w_this_no_match_this
    },
    {
        "query_count_results",
        Basic_query_count_results
    },
    {
        "query_count_entities",
        Basic_query_count_entities
    },
    {
        "query_is_true",
        Basic_query_is_true
    },
    {
        "implicit_cleanup_1_term",
        Basic_implicit_cleanup_1_term
    },
    {
        "implicit_cleanup_2_terms",
        Basic_implicit_cleanup_2_terms
    },
    {
        "implicit_cleanup_1_term_w_up",
        Basic_implicit_cleanup_1_term_w_up
    },
    {
        "implicit_cleanup_2_terms_w_up",
        Basic_implicit_cleanup_2_terms_w_up
    },
    {
        "implicit_cleanup_2_queries",
        Basic_implicit_cleanup_2_queries
    },
    {
        "implicit_cleanup_2_queries_1_cleanup",
        Basic_implicit_cleanup_2_queries_1_cleanup
    },
    {
        "iter_valid",
        Basic_iter_valid
    },
    {
        "iter_frame_offset",
        Basic_iter_frame_offset
    },
    {
        "iter_nested_1",
        Basic_iter_nested_1
    },
    {
        "iter_nested_2",
        Basic_iter_nested_2
    },
    {
        "iter_interleaved",
        Basic_iter_interleaved
    },
    {
        "set_get_context",
        Basic_set_get_context
    },
    {
        "set_get_binding_context",
        Basic_set_get_binding_context
    },
    {
        "set_get_context_w_free",
        Basic_set_get_context_w_free
    },
    {
        "set_get_binding_context_w_free",
        Basic_set_get_binding_context_w_free
    },
    {
        "create_query_w_existing_entity",
        Basic_create_query_w_existing_entity
    },
    {
        "create_query_w_existing_entity_different_term_count",
        Basic_create_query_w_existing_entity_different_term_count
    },
    {
        "create_multi_component_query_w_existing_entity",
        Basic_create_multi_component_query_w_existing_entity
    },
    {
        "delete_query_by_entity",
        Basic_delete_query_by_entity
    },
    {
        "eval_count",
        Basic_eval_count
    },
    {
        "no_results_after_delete_tree",
        Basic_no_results_after_delete_tree
    },
    {
        "no_results_after_delete_tree_deferred",
        Basic_no_results_after_delete_tree_deferred
    },
    {
        "add_on_self_ref",
        Basic_add_on_self_ref
    },
    {
        "add_on_self_ref_by_name",
        Basic_add_on_self_ref_by_name
    },
    {
        "delete_id_after_delete_query",
        Basic_delete_id_after_delete_query
    },
    {
        "pair_sweep_tag",
        Basic_pair_sweep_tag
    },
    {
        "pair_sweep_wildcard_first",
        Basic_pair_sweep_wildcard_first
    },
    {
        "pair_sweep_wildcard_second",
        Basic_pair_sweep_wildcard_second
    },
    {
        "create_w_entity_deferred",
        Basic_create_w_entity_deferred
    },
    {
        "32_terms",
        Basic_32_terms
    },
    {
        "33_terms_expr",
        Basic_33_terms_expr
    },
    {
        "stage_query",
        Basic_stage_query
    },
    {
        "world_query_w_stage_iter",
        Basic_world_query_w_stage_iter
    },
    {
        "stage_query_w_nth_stage",
        Basic_stage_query_w_nth_stage
    },
    {
        "world_query_w_nth_stage_iter",
        Basic_world_query_w_nth_stage_iter
    },
    {
        "match_empty",
        Basic_match_empty
    },
    {
        "match_new_empty",
        Basic_match_new_empty
    },
    {
        "match_empty_w_component",
        Basic_match_empty_w_component
    },
    {
        "match_new_empty_w_component",
        Basic_match_new_empty_w_component
    },
    {
        "match_empty_w_ref",
        Basic_match_empty_w_ref
    },
    {
        "match_new_empty_w_ref",
        Basic_match_new_empty_w_ref
    },
    {
        "match_empty_w_order_by",
        Basic_match_empty_w_order_by
    },
    {
        "match_new_empty_w_order_by",
        Basic_match_new_empty_w_order_by
    },
    {
        "match_empty_w_bitset",
        Basic_match_empty_w_bitset
    },
    {
        "default_query_flags",
        Basic_default_query_flags
    }
};

bake_test_case Combinations_testcases[] = {
    {
        "singleton_and_self_and_self",
        Combinations_singleton_and_self_and_self
    },
    {
        "self_and_singleton_and_self",
        Combinations_self_and_singleton_and_self
    },
    {
        "self_and_self_and_singleton",
        Combinations_self_and_self_and_singleton
    },
    {
        "singleton_and_trav_and_self",
        Combinations_singleton_and_trav_and_self
    },
    {
        "trav_and_singleton_and_self",
        Combinations_trav_and_singleton_and_self
    },
    {
        "self_and_singleton_and_trav",
        Combinations_self_and_singleton_and_trav
    },
    {
        "self_and_trav_and_singleton",
        Combinations_self_and_trav_and_singleton
    },
    {
        "singleton_and_self_and_trav",
        Combinations_singleton_and_self_and_trav
    },
    {
        "trav_and_self_and_singleton",
        Combinations_trav_and_self_and_singleton
    }
};

bake_test_case Plan_testcases[] = {
    {
        "reordered_plan_1",
        Plan_reordered_plan_1
    },
    {
        "reordered_plan_2",
        Plan_reordered_plan_2
    },
    {
        "reordered_plan_3",
        Plan_reordered_plan_3
    },
    {
        "reordered_plan_4",
        Plan_reordered_plan_4
    },
    {
        "reordered_plan_5",
        Plan_reordered_plan_5
    },
    {
        "reordered_plan_6",
        Plan_reordered_plan_6
    },
    {
        "reordered_plan_7",
        Plan_reordered_plan_7
    },
    {
        "1_trivial_plan",
        Plan_1_trivial_plan
    },
    {
        "2_trivial_plan",
        Plan_2_trivial_plan
    },
    {
        "1_trivial_plan_component",
        Plan_1_trivial_plan_component
    },
    {
        "2_trivial_plan_component",
        Plan_2_trivial_plan_component
    },
    {
        "3_trivial_plan_w_pair",
        Plan_3_trivial_plan_w_pair
    },
    {
        "3_trivial_plan_w_wildcard",
        Plan_3_trivial_plan_w_wildcard
    },
    {
        "3_trivial_plan_w_any",
        Plan_3_trivial_plan_w_any
    },
    {
        "3_trivial_plan_w_pair_component",
        Plan_3_trivial_plan_w_pair_component
    },
    {
        "3_trivial_plan_w_wildcard_component",
        Plan_3_trivial_plan_w_wildcard_component
    },
    {
        "3_trivial_plan_w_any_component",
        Plan_3_trivial_plan_w_any_component
    },
    {
        "1_trivial_component_w_none",
        Plan_1_trivial_component_w_none
    },
    {
        "2_trivial_component_w_none",
        Plan_2_trivial_component_w_none
    },
    {
        "2_trivial_plan_w_wildcard",
        Plan_2_trivial_plan_w_wildcard
    },
    {
        "this_before_fixed_src",
        Plan_this_before_fixed_src
    },
    {
        "fixed_src_before_this",
        Plan_fixed_src_before_this
    },
    {
        "var_before_fixed_src",
        Plan_var_before_fixed_src
    },
    {
        "fixed_src_before_var",
        Plan_fixed_src_before_var
    },
    {
        "this_before_fixed_src_w_not",
        Plan_this_before_fixed_src_w_not
    },
    {
        "this_before_fixed_src_w_first_var",
        Plan_this_before_fixed_src_w_first_var
    },
    {
        "this_before_fixed_src_w_first_var_w_not",
        Plan_this_before_fixed_src_w_first_var_w_not
    },
    {
        "this_before_fixed_src_w_second_var",
        Plan_this_before_fixed_src_w_second_var
    },
    {
        "this_before_fixed_src_w_second_var_w_not",
        Plan_this_before_fixed_src_w_second_var_w_not
    },
    {
        "populate_1_fixed",
        Plan_populate_1_fixed
    },
    {
        "populate_2_fixed",
        Plan_populate_2_fixed
    },
    {
        "populate_1_fixed_1_this_self",
        Plan_populate_1_fixed_1_this_self
    },
    {
        "populate_2_fixed_2_this_self",
        Plan_populate_2_fixed_2_this_self
    },
    {
        "populate_2_fixed_2_this_self_interleaved",
        Plan_populate_2_fixed_2_this_self_interleaved
    },
    {
        "populate_2_this_self_2_fixed",
        Plan_populate_2_this_self_2_fixed
    },
    {
        "populate_1_fixed_1_this_up",
        Plan_populate_1_fixed_1_this_up
    },
    {
        "populate_2_fixed_2_this_up",
        Plan_populate_2_fixed_2_this_up
    },
    {
        "populate_2_fixed_2_this_up_interleaved",
        Plan_populate_2_fixed_2_this_up_interleaved
    },
    {
        "populate_2_this_up_2_fixed",
        Plan_populate_2_this_up_2_fixed
    },
    {
        "populate_1_fixed_1_this_self_cached",
        Plan_populate_1_fixed_1_this_self_cached
    },
    {
        "populate_2_fixed_2_this_self_cached",
        Plan_populate_2_fixed_2_this_self_cached
    },
    {
        "populate_2_fixed_2_this_self_interleaved_cached",
        Plan_populate_2_fixed_2_this_self_interleaved_cached
    },
    {
        "populate_2_this_self_2_fixed_cached",
        Plan_populate_2_this_self_2_fixed_cached
    },
    {
        "populate_1_fixed_1_this_up_cached",
        Plan_populate_1_fixed_1_this_up_cached
    },
    {
        "populate_2_fixed_2_this_up_cached",
        Plan_populate_2_fixed_2_this_up_cached
    },
    {
        "populate_2_fixed_2_this_up_interleaved_cached",
        Plan_populate_2_fixed_2_this_up_interleaved_cached
    },
    {
        "populate_2_this_up_2_fixed_cached",
        Plan_populate_2_this_up_2_fixed_cached
    },
    {
        "populate_1_fixed_1_var_self",
        Plan_populate_1_fixed_1_var_self
    },
    {
        "populate_2_fixed_2_var_self",
        Plan_populate_2_fixed_2_var_self
    },
    {
        "populate_2_fixed_2_var_self_interleaved",
        Plan_populate_2_fixed_2_var_self_interleaved
    },
    {
        "populate_2_var_self_2_fixed",
        Plan_populate_2_var_self_2_fixed
    },
    {
        "populate_1_fixed_1_var_up",
        Plan_populate_1_fixed_1_var_up
    },
    {
        "populate_2_fixed_2_var_up",
        Plan_populate_2_fixed_2_var_up
    },
    {
        "populate_2_fixed_2_var_up_interleaved",
        Plan_populate_2_fixed_2_var_up_interleaved
    },
    {
        "populate_2_var_up_2_fixed",
        Plan_populate_2_var_up_2_fixed
    },
    {
        "cache_2_or",
        Plan_cache_2_or
    },
    {
        "cache_2_or_w_not",
        Plan_cache_2_or_w_not
    },
    {
        "1_plan_any_src",
        Plan_1_plan_any_src
    },
    {
        "1_plan_not_any_src",
        Plan_1_plan_not_any_src
    },
    {
        "1_plan_optional_any_src",
        Plan_1_plan_optional_any_src
    },
    {
        "pair_first_wildcard",
        Plan_pair_first_wildcard
    },
    {
        "pair_first_wildcard_cached",
        Plan_pair_first_wildcard_cached
    },
    {
        "pair_second_wildcard",
        Plan_pair_second_wildcard
    },
    {
        "pair_second_wildcard_cached",
        Plan_pair_second_wildcard_cached
    },
    {
        "0_src_tag",
        Plan_0_src_tag
    },
    {
        "0_src_component",
        Plan_0_src_component
    },
    {
        "0_src_w_sparse",
        Plan_0_src_w_sparse
    },
    {
        "0_src_w_toggle",
        Plan_0_src_w_toggle
    },
    {
        "0_src_w_union",
        Plan_0_src_w_union
    },
    {
        "0_src_w_sparse_and_component",
        Plan_0_src_w_sparse_and_component
    },
    {
        "0_src_w_toggle_and_component",
        Plan_0_src_w_toggle_and_component
    },
    {
        "0_src_w_union_and_component",
        Plan_0_src_w_union_and_component
    }
};

bake_test_case Variables_testcases[] = {
    {
        "1_ent_src_w_var",
        Variables_1_ent_src_w_var
    },
    {
        "1_ent_src_w_pair_rel_var",
        Variables_1_ent_src_w_pair_rel_var
    },
    {
        "1_ent_src_w_pair_tgt_var",
        Variables_1_ent_src_w_pair_tgt_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_var",
        Variables_1_ent_src_w_pair_rel_tgt_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_same_var",
        Variables_1_ent_src_w_pair_rel_tgt_same_var
    },
    {
        "1_ent_src_w_pair_rel_tgt_same_var_after_write",
        Variables_1_ent_src_w_pair_rel_tgt_same_var_after_write
    },
    {
        "1_this_src_w_var",
        Variables_1_this_src_w_var
    },
    {
        "1_this_src_w_pair_rel_var",
        Variables_1_this_src_w_pair_rel_var
    },
    {
        "1_this_src_w_pair_tgt_var",
        Variables_1_this_src_w_pair_tgt_var
    },
    {
        "1_this_src_w_pair_rel_tgt_var",
        Variables_1_this_src_w_pair_rel_tgt_var
    },
    {
        "1_this_src_w_pair_rel_tgt_same_var",
        Variables_1_this_src_w_pair_rel_tgt_same_var
    },
    {
        "1_this_src_w_pair_rel_tgt_same_var_after_write",
        Variables_1_this_src_w_pair_rel_tgt_same_var_after_write
    },
    {
        "1_src_id_same_var",
        Variables_1_src_id_same_var
    },
    {
        "1_src_pair_first_same_var",
        Variables_1_src_pair_first_same_var
    },
    {
        "1_src_pair_second_same_var",
        Variables_1_src_pair_second_same_var
    },
    {
        "1_src_pair_first_and_second_same_var",
        Variables_1_src_pair_first_and_second_same_var
    },
    {
        "1_src_id_same_var_after_write",
        Variables_1_src_id_same_var_after_write
    },
    {
        "1_src_pair_first_same_var_after_write",
        Variables_1_src_pair_first_same_var_after_write
    },
    {
        "1_src_pair_second_same_var_after_write",
        Variables_1_src_pair_second_same_var_after_write
    },
    {
        "1_src_pair_first_and_second_same_var_after_write",
        Variables_1_src_pair_first_and_second_same_var_after_write
    },
    {
        "1_src_pair_first_same_var_this",
        Variables_1_src_pair_first_same_var_this
    },
    {
        "1_src_pair_second_same_var_this",
        Variables_1_src_pair_second_same_var_this
    },
    {
        "1_src_pair_first_and_second_same_var_this",
        Variables_1_src_pair_first_and_second_same_var_this
    },
    {
        "1_src_id_same_var_this_after_write",
        Variables_1_src_id_same_var_this_after_write
    },
    {
        "1_src_pair_first_same_var_this_after_write",
        Variables_1_src_pair_first_same_var_this_after_write
    },
    {
        "1_src_pair_second_same_var_this_after_write",
        Variables_1_src_pair_second_same_var_this_after_write
    },
    {
        "1_src_pair_first_and_second_same_var_this_after_write",
        Variables_1_src_pair_first_and_second_same_var_this_after_write
    },
    {
        "1_ent_src_w_this_var",
        Variables_1_ent_src_w_this_var
    },
    {
        "1_ent_src_w_pair_this_rel",
        Variables_1_ent_src_w_pair_this_rel
    },
    {
        "1_ent_src_w_pair_this_tgt",
        Variables_1_ent_src_w_pair_this_tgt
    },
    {
        "1_ent_src_w_pair_this_rel_tgt",
        Variables_1_ent_src_w_pair_this_rel_tgt
    },
    {
        "1_this_src_w_this",
        Variables_1_this_src_w_this
    },
    {
        "1_this_src_w_pair_this_rel_tgt",
        Variables_1_this_src_w_pair_this_rel_tgt
    },
    {
        "1_this_src_w_this_after_write",
        Variables_1_this_src_w_this_after_write
    },
    {
        "1_this_src_w_pair_this_rel_tgt_after_write",
        Variables_1_this_src_w_pair_this_rel_tgt_after_write
    },
    {
        "2_constrain_src_from_src",
        Variables_2_constrain_src_from_src
    },
    {
        "2_constrain_rel_from_src_w_ent",
        Variables_2_constrain_rel_from_src_w_ent
    },
    {
        "2_constrain_rel_from_src_w_var",
        Variables_2_constrain_rel_from_src_w_var
    },
    {
        "2_constrain_rel_from_src_w_this",
        Variables_2_constrain_rel_from_src_w_this
    },
    {
        "2_constrain_pair_rel_from_src_w_ent",
        Variables_2_constrain_pair_rel_from_src_w_ent
    },
    {
        "2_constrain_pair_rel_from_src_w_var",
        Variables_2_constrain_pair_rel_from_src_w_var
    },
    {
        "2_constrain_pair_rel_from_src_w_this",
        Variables_2_constrain_pair_rel_from_src_w_this
    },
    {
        "2_constrain_pair_tgt_from_src_w_ent",
        Variables_2_constrain_pair_tgt_from_src_w_ent
    },
    {
        "2_constrain_pair_tgt_from_src_w_var",
        Variables_2_constrain_pair_tgt_from_src_w_var
    },
    {
        "2_constrain_pair_tgt_from_src_w_this",
        Variables_2_constrain_pair_tgt_from_src_w_this
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_ent",
        Variables_2_constrain_pair_rel_tgt_from_src_w_ent
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_var",
        Variables_2_constrain_pair_rel_tgt_from_src_w_var
    },
    {
        "2_constrain_pair_rel_tgt_from_src_w_this",
        Variables_2_constrain_pair_rel_tgt_from_src_w_this
    },
    {
        "1_ent_src_set_rel_var",
        Variables_1_ent_src_set_rel_var
    },
    {
        "1_ent_src_set_pair_rel_var",
        Variables_1_ent_src_set_pair_rel_var
    },
    {
        "1_ent_src_set_pair_tgt_var",
        Variables_1_ent_src_set_pair_tgt_var
    },
    {
        "1_set_src_var",
        Variables_1_set_src_var
    },
    {
        "1_set_src_var_w_pair",
        Variables_1_set_src_var_w_pair
    },
    {
        "1_set_src_var_w_pair_set_rel",
        Variables_1_set_src_var_w_pair_set_rel
    },
    {
        "1_set_src_var_w_pair_set_tgt",
        Variables_1_set_src_var_w_pair_set_tgt
    },
    {
        "1_set_src_var_w_pair_set_rel_tgt",
        Variables_1_set_src_var_w_pair_set_rel_tgt
    },
    {
        "1_set_src_this",
        Variables_1_set_src_this
    },
    {
        "1_set_src_this_w_pair",
        Variables_1_set_src_this_w_pair
    },
    {
        "1_set_src_this_w_pair_set_rel",
        Variables_1_set_src_this_w_pair_set_rel
    },
    {
        "1_set_src_this_w_pair_set_tgt",
        Variables_1_set_src_this_w_pair_set_tgt
    },
    {
        "1_set_src_this_w_pair_set_rel_tgt",
        Variables_1_set_src_this_w_pair_set_rel_tgt
    },
    {
        "1_set_src_this_to_empty_table",
        Variables_1_set_src_this_to_empty_table
    },
    {
        "1_set_src_this_to_empty_table_w_component",
        Variables_1_set_src_this_to_empty_table_w_component
    },
    {
        "1_set_src_this_to_empty_table_w_component_self",
        Variables_1_set_src_this_to_empty_table_w_component_self
    },
    {
        "1_set_src_this_to_entity_in_table",
        Variables_1_set_src_this_to_entity_in_table
    },
    {
        "1_set_src_this_to_entity_in_table_self",
        Variables_1_set_src_this_to_entity_in_table_self
    },
    {
        "2_set_src_this",
        Variables_2_set_src_this
    },
    {
        "2_set_src_this_self",
        Variables_2_set_src_this_self
    },
    {
        "2_set_src_this_component",
        Variables_2_set_src_this_component
    },
    {
        "2_set_src_this_self_component",
        Variables_2_set_src_this_self_component
    },
    {
        "2_set_src_this_w_up",
        Variables_2_set_src_this_w_up
    },
    {
        "2_set_src_this_self_w_up",
        Variables_2_set_src_this_self_w_up
    },
    {
        "2_set_src_this_component_w_up",
        Variables_2_set_src_this_component_w_up
    },
    {
        "2_set_src_this_self_component_w_up",
        Variables_2_set_src_this_self_component_w_up
    },
    {
        "2_set_src_this_w_exclusive_wildcard",
        Variables_2_set_src_this_w_exclusive_wildcard
    },
    {
        "2_set_src_this_self_w_exclusive_wildcard",
        Variables_2_set_src_this_self_w_exclusive_wildcard
    },
    {
        "2_set_src_this_self_w_exclusive_wildcard_w_up",
        Variables_2_set_src_this_self_w_exclusive_wildcard_w_up
    },
    {
        "1_set_src_this_is_true",
        Variables_1_set_src_this_is_true
    },
    {
        "3_set_src_this_w_uncacheable_tag_tag_tag",
        Variables_3_set_src_this_w_uncacheable_tag_tag_tag
    },
    {
        "3_set_src_this_w_uncacheable_tag_component_tag",
        Variables_3_set_src_this_w_uncacheable_tag_component_tag
    },
    {
        "3_set_src_this_w_uncacheable_tag_component_component",
        Variables_3_set_src_this_w_uncacheable_tag_component_component
    },
    {
        "3_set_src_this_w_tag_uncacheable_tag_tag",
        Variables_3_set_src_this_w_tag_uncacheable_tag_tag
    },
    {
        "3_set_src_this_w_component_uncacheable_tag_tag",
        Variables_3_set_src_this_w_component_uncacheable_tag_tag
    },
    {
        "3_set_src_this_w_component_uncacheable_tag_component",
        Variables_3_set_src_this_w_component_uncacheable_tag_component
    },
    {
        "3_set_src_this_w_tag_tag_uncacheable_tag",
        Variables_3_set_src_this_w_tag_tag_uncacheable_tag
    },
    {
        "3_set_src_this_w_component_tag_uncacheable_tag",
        Variables_3_set_src_this_w_component_tag_uncacheable_tag
    },
    {
        "3_set_src_this_w_component_component_uncacheable_tag",
        Variables_3_set_src_this_w_component_component_uncacheable_tag
    },
    {
        "3_set_src_this_w_uncacheable_component_tag_tag",
        Variables_3_set_src_this_w_uncacheable_component_tag_tag
    },
    {
        "3_set_src_this_w_uncacheable_component_component_tag",
        Variables_3_set_src_this_w_uncacheable_component_component_tag
    },
    {
        "3_set_src_this_w_uncacheable_component_component_component",
        Variables_3_set_src_this_w_uncacheable_component_component_component
    },
    {
        "3_set_src_this_w_tag_uncacheable_component_tag",
        Variables_3_set_src_this_w_tag_uncacheable_component_tag
    },
    {
        "3_set_src_this_w_component_uncacheable_component_tag",
        Variables_3_set_src_this_w_component_uncacheable_component_tag
    },
    {
        "3_set_src_this_w_component_uncacheable_component_component",
        Variables_3_set_src_this_w_component_uncacheable_component_component
    },
    {
        "3_set_src_this_w_tag_tag_uncacheable_component",
        Variables_3_set_src_this_w_tag_tag_uncacheable_component
    },
    {
        "3_set_src_this_w_component_tag_uncacheable_component",
        Variables_3_set_src_this_w_component_tag_uncacheable_component
    },
    {
        "3_set_src_this_w_component_component_uncacheable_component",
        Variables_3_set_src_this_w_component_component_uncacheable_component
    },
    {
        "2_set_src_this_w_wildcard",
        Variables_2_set_src_this_w_wildcard
    },
    {
        "1_src_this_var_as_entity",
        Variables_1_src_this_var_as_entity
    },
    {
        "1_src_this_var_as_table",
        Variables_1_src_this_var_as_table
    },
    {
        "1_src_this_var_as_table_range",
        Variables_1_src_this_var_as_table_range
    },
    {
        "1_set_src_this_w_any",
        Variables_1_set_src_this_w_any
    },
    {
        "1_set_src_this_w_any_fixed",
        Variables_1_set_src_this_w_any_fixed
    },
    {
        "1_set_src_this_w_fixed_any",
        Variables_1_set_src_this_w_fixed_any
    },
    {
        "2_join_by_rel_var",
        Variables_2_join_by_rel_var
    },
    {
        "2_join_by_pair_rel_var",
        Variables_2_join_by_pair_rel_var
    },
    {
        "2_join_by_pair_tgt_var",
        Variables_2_join_by_pair_tgt_var
    },
    {
        "2_cycle_w_var",
        Variables_2_cycle_w_var
    },
    {
        "2_cycle_w_this_var",
        Variables_2_cycle_w_this_var
    },
    {
        "2_cycle_w_var_this",
        Variables_2_cycle_w_var_this
    },
    {
        "2_cycle_pair_w_var",
        Variables_2_cycle_pair_w_var
    },
    {
        "2_cycle_pair_w_this_var_var",
        Variables_2_cycle_pair_w_this_var_var
    },
    {
        "2_cycle_pair_w_var_this_var",
        Variables_2_cycle_pair_w_var_this_var
    },
    {
        "2_cycle_pair_w_var_var_this",
        Variables_2_cycle_pair_w_var_var_this
    },
    {
        "2_cycle_pair_ent_var_var",
        Variables_2_cycle_pair_ent_var_var
    },
    {
        "2_cycle_pair_ent_this_var",
        Variables_2_cycle_pair_ent_this_var
    },
    {
        "2_cycle_pair_ent_var_this",
        Variables_2_cycle_pair_ent_var_this
    },
    {
        "parse_0_var",
        Variables_parse_0_var
    },
    {
        "parse_1_var",
        Variables_parse_1_var
    },
    {
        "parse_2_vars",
        Variables_parse_2_vars
    },
    {
        "parse_0_var_paren",
        Variables_parse_0_var_paren
    },
    {
        "parse_1_var_paren",
        Variables_parse_1_var_paren
    },
    {
        "parse_2_vars_paren",
        Variables_parse_2_vars_paren
    },
    {
        "parse_1_vars_w_path",
        Variables_parse_1_vars_w_path
    },
    {
        "parse_missing_close_paren",
        Variables_parse_missing_close_paren
    },
    {
        "parse_missing_open_paren",
        Variables_parse_missing_open_paren
    },
    {
        "parse_missing_value",
        Variables_parse_missing_value
    },
    {
        "parse_0_var_w_spaces",
        Variables_parse_0_var_w_spaces
    },
    {
        "parse_1_var_w_spaces",
        Variables_parse_1_var_w_spaces
    },
    {
        "parse_2_vars_w_spaces",
        Variables_parse_2_vars_w_spaces
    },
    {
        "parse_0_var_paren_w_spaces",
        Variables_parse_0_var_paren_w_spaces
    },
    {
        "parse_1_var_paren_w_spaces",
        Variables_parse_1_var_paren_w_spaces
    },
    {
        "parse_2_vars_paren_w_spaces",
        Variables_parse_2_vars_paren_w_spaces
    },
    {
        "var_count",
        Variables_var_count
    },
    {
        "var_name",
        Variables_var_name
    },
    {
        "var_is_entity",
        Variables_var_is_entity
    },
    {
        "no_this_anonymous_src",
        Variables_no_this_anonymous_src
    },
    {
        "no_this_anonymous_src_w_pair",
        Variables_no_this_anonymous_src_w_pair
    },
    {
        "no_this_anonymous_component_src",
        Variables_no_this_anonymous_component_src
    },
    {
        "no_this_anonymous_component_src_w_pair",
        Variables_no_this_anonymous_component_src_w_pair
    },
    {
        "lookup_from_table_this",
        Variables_lookup_from_table_this
    },
    {
        "lookup_from_entity_this",
        Variables_lookup_from_entity_this
    },
    {
        "lookup_from_table",
        Variables_lookup_from_table
    },
    {
        "lookup_from_entity",
        Variables_lookup_from_entity
    },
    {
        "lookup_from_not_written",
        Variables_lookup_from_not_written
    },
    {
        "lookup_from_table_this_component",
        Variables_lookup_from_table_this_component
    },
    {
        "lookup_from_entity_this_component",
        Variables_lookup_from_entity_this_component
    },
    {
        "lookup_from_table_component",
        Variables_lookup_from_table_component
    },
    {
        "lookup_from_entity_component",
        Variables_lookup_from_entity_component
    },
    {
        "lookup_from_table_two_children",
        Variables_lookup_from_table_two_children
    },
    {
        "lookup_from_entity_two_children",
        Variables_lookup_from_entity_two_children
    },
    {
        "lookup_from_table_same_child_twice",
        Variables_lookup_from_table_same_child_twice
    },
    {
        "lookup_from_entity_same_child_twice",
        Variables_lookup_from_entity_same_child_twice
    },
    {
        "lookup_from_table_not",
        Variables_lookup_from_table_not
    },
    {
        "lookup_from_entity_not",
        Variables_lookup_from_entity_not
    },
    {
        "lookup_from_table_w_any_component",
        Variables_lookup_from_table_w_any_component
    },
    {
        "lookup_from_entity_w_any_component",
        Variables_lookup_from_entity_w_any_component
    },
    {
        "lookup_as_tag",
        Variables_lookup_as_tag
    },
    {
        "lookup_as_relationship",
        Variables_lookup_as_relationship
    },
    {
        "lookup_as_target",
        Variables_lookup_as_target
    },
    {
        "lookup_assign_var",
        Variables_lookup_assign_var
    },
    {
        "lookup_eq_var",
        Variables_lookup_eq_var
    },
    {
        "lookup_neq_var",
        Variables_lookup_neq_var
    },
    {
        "lookup_unresolved_dependent",
        Variables_lookup_unresolved_dependent
    },
    {
        "check_vars_this",
        Variables_check_vars_this
    },
    {
        "check_vars_var",
        Variables_check_vars_var
    },
    {
        "check_vars_wildcard",
        Variables_check_vars_wildcard
    },
    {
        "check_vars_any",
        Variables_check_vars_any
    },
    {
        "check_vars_var_as_tgt",
        Variables_check_vars_var_as_tgt
    },
    {
        "check_vars_this_as_tgt",
        Variables_check_vars_this_as_tgt
    },
    {
        "check_vars_anonymous_var_as_tgt",
        Variables_check_vars_anonymous_var_as_tgt
    },
    {
        "check_vars_wildcard_as_tgt",
        Variables_check_vars_wildcard_as_tgt
    },
    {
        "check_vars_any_as_tgt",
        Variables_check_vars_any_as_tgt
    },
    {
        "check_vars_this_w_lookup_var",
        Variables_check_vars_this_w_lookup_var
    },
    {
        "check_vars_var_w_lookup_var",
        Variables_check_vars_var_w_lookup_var
    },
    {
        "1_trivial_1_var",
        Variables_1_trivial_1_var
    },
    {
        "2_trivial_1_var",
        Variables_2_trivial_1_var
    },
    {
        "1_trivial_1_var_component",
        Variables_1_trivial_1_var_component
    },
    {
        "2_trivial_1_var_component",
        Variables_2_trivial_1_var_component
    },
    {
        "1_trivial_1_wildcard",
        Variables_1_trivial_1_wildcard
    },
    {
        "2_trivial_1_wildcard",
        Variables_2_trivial_1_wildcard
    },
    {
        "1_trivial_1_wildcard_component",
        Variables_1_trivial_1_wildcard_component
    },
    {
        "2_trivial_1_wildcard_component",
        Variables_2_trivial_1_wildcard_component
    },
    {
        "1_trivial_1_any",
        Variables_1_trivial_1_any
    },
    {
        "2_trivial_1_any",
        Variables_2_trivial_1_any
    },
    {
        "1_trivial_1_any_component",
        Variables_1_trivial_1_any_component
    },
    {
        "2_trivial_1_any_component",
        Variables_2_trivial_1_any_component
    },
    {
        "first_invalid_var_name_and_id",
        Variables_first_invalid_var_name_and_id
    },
    {
        "src_invalid_var_name_and_id",
        Variables_src_invalid_var_name_and_id
    },
    {
        "second_invalid_var_name_and_id",
        Variables_second_invalid_var_name_and_id
    }
};

bake_test_case Operators_testcases[] = {
    {
        "2_and_not",
        Operators_2_and_not
    },
    {
        "2_and_not_component",
        Operators_2_and_not_component
    },
    {
        "2_and_out_not",
        Operators_2_and_out_not
    },
    {
        "2_and_out_not_component",
        Operators_2_and_out_not_component
    },
    {
        "3_and_not_not",
        Operators_3_and_not_not
    },
    {
        "2_and_not_pair_rel_wildcard",
        Operators_2_and_not_pair_rel_wildcard
    },
    {
        "2_and_not_pair_tgt_wildcard",
        Operators_2_and_not_pair_tgt_wildcard
    },
    {
        "2_and_not_pair_rel_tgt_wildcard",
        Operators_2_and_not_pair_rel_tgt_wildcard
    },
    {
        "2_and_not_pair_rel_var",
        Operators_2_and_not_pair_rel_var
    },
    {
        "2_and_not_pair_tgt_var",
        Operators_2_and_not_pair_tgt_var
    },
    {
        "2_and_not_pair_rel_tgt_var",
        Operators_2_and_not_pair_rel_tgt_var
    },
    {
        "2_and_not_pair_rel_tgt_same_var",
        Operators_2_and_not_pair_rel_tgt_same_var
    },
    {
        "2_and_not_pair_rel_var_written",
        Operators_2_and_not_pair_rel_var_written
    },
    {
        "2_and_not_pair_tgt_var_written",
        Operators_2_and_not_pair_tgt_var_written
    },
    {
        "2_and_not_pair_rel_tgt_var_written",
        Operators_2_and_not_pair_rel_tgt_var_written
    },
    {
        "2_and_not_pair_rel_tgt_same_var_written",
        Operators_2_and_not_pair_rel_tgt_same_var_written
    },
    {
        "2_and_not_pair_rel_src_tgt_same_var_written",
        Operators_2_and_not_pair_rel_src_tgt_same_var_written
    },
    {
        "2_and_not_pair_any_rel",
        Operators_2_and_not_pair_any_rel
    },
    {
        "2_and_not_pair_any_tgt",
        Operators_2_and_not_pair_any_tgt
    },
    {
        "2_and_not_pair_any_src",
        Operators_2_and_not_pair_any_src
    },
    {
        "1_not_any_src_fixed_first",
        Operators_1_not_any_src_fixed_first
    },
    {
        "1_not_any_src_any_tgt_fixed_first",
        Operators_1_not_any_src_any_tgt_fixed_first
    },
    {
        "1_not_any_src_any_first_fixed_tgt",
        Operators_1_not_any_src_any_first_fixed_tgt
    },
    {
        "1_not_any_src_any_childof_pair_any_tgt",
        Operators_1_not_any_src_any_childof_pair_any_tgt
    },
    {
        "1_not_any_src_any_isa_pair_any_tgt",
        Operators_1_not_any_src_any_isa_pair_any_tgt
    },
    {
        "2_and_optional",
        Operators_2_and_optional
    },
    {
        "3_and_optional_optional",
        Operators_3_and_optional_optional
    },
    {
        "2_and_optional_pair_rel_wildcard",
        Operators_2_and_optional_pair_rel_wildcard
    },
    {
        "2_and_optional_pair_tgt_wildcard",
        Operators_2_and_optional_pair_tgt_wildcard
    },
    {
        "2_and_optional_pair_rel_var",
        Operators_2_and_optional_pair_rel_var
    },
    {
        "2_and_optional_pair_tgt_var",
        Operators_2_and_optional_pair_tgt_var
    },
    {
        "2_and_optional_pair_rel_tgt_var",
        Operators_2_and_optional_pair_rel_tgt_var
    },
    {
        "2_and_optional_pair_rel_tgt_same_var",
        Operators_2_and_optional_pair_rel_tgt_same_var
    },
    {
        "2_and_optional_pair_rel_var_written",
        Operators_2_and_optional_pair_rel_var_written
    },
    {
        "2_and_optional_pair_tgt_var_written",
        Operators_2_and_optional_pair_tgt_var_written
    },
    {
        "2_and_optional_pair_rel_tgt_var_written",
        Operators_2_and_optional_pair_rel_tgt_var_written
    },
    {
        "2_and_optional_pair_rel_tgt_same_var_written",
        Operators_2_and_optional_pair_rel_tgt_same_var_written
    },
    {
        "2_and_optional_pair_rel_src_tgt_same_var_written",
        Operators_2_and_optional_pair_rel_src_tgt_same_var_written
    },
    {
        "3_and_optional_optional_pair_w_var",
        Operators_3_and_optional_optional_pair_w_var
    },
    {
        "2_and_optional_pair_any_rel",
        Operators_2_and_optional_pair_any_rel
    },
    {
        "2_and_optional_pair_any_tgt",
        Operators_2_and_optional_pair_any_tgt
    },
    {
        "2_and_optional_pair_any_src",
        Operators_2_and_optional_pair_any_src
    },
    {
        "3_and_optional_dependent_and_pair_rel",
        Operators_3_and_optional_dependent_and_pair_rel
    },
    {
        "3_and_optional_dependent_and_pair_tgt",
        Operators_3_and_optional_dependent_and_pair_tgt
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt",
        Operators_3_and_optional_dependent_and_pair_rel_tgt
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt_same_var",
        Operators_3_and_optional_dependent_and_pair_rel_tgt_same_var
    },
    {
        "3_and_optional_dependent_and_pair_rel_tgt_same_other_var",
        Operators_3_and_optional_dependent_and_pair_rel_tgt_same_other_var
    },
    {
        "3_and_optional_dependent_and_pair_src",
        Operators_3_and_optional_dependent_and_pair_src
    },
    {
        "3_and_optional_dependent_optional_pair_rel",
        Operators_3_and_optional_dependent_optional_pair_rel
    },
    {
        "3_and_optional_dependent_optional_pair_tgt",
        Operators_3_and_optional_dependent_optional_pair_tgt
    },
    {
        "3_and_optional_dependent_optional_pair_src",
        Operators_3_and_optional_dependent_optional_pair_src
    },
    {
        "3_and_optional_dependent_not_pair_rel",
        Operators_3_and_optional_dependent_not_pair_rel
    },
    {
        "3_and_optional_dependent_not_pair_tgt",
        Operators_3_and_optional_dependent_not_pair_tgt
    },
    {
        "3_and_optional_dependent_not_pair_src",
        Operators_3_and_optional_dependent_not_pair_src
    },
    {
        "1_optional_any_src_fixed_first",
        Operators_1_optional_any_src_fixed_first
    },
    {
        "1_optional_any_src_any_tgt_fixed_first",
        Operators_1_optional_any_src_any_tgt_fixed_first
    },
    {
        "1_optional_any_src_any_first_fixed_tgt",
        Operators_1_optional_any_src_any_first_fixed_tgt
    },
    {
        "1_optional_any_src_any_childof_pair_any_tgt",
        Operators_1_optional_any_src_any_childof_pair_any_tgt
    },
    {
        "1_optional_any_src_any_isa_pair_any_tgt",
        Operators_1_optional_any_src_any_isa_pair_any_tgt
    },
    {
        "2_or",
        Operators_2_or
    },
    {
        "3_or",
        Operators_3_or
    },
    {
        "2_or_w_and",
        Operators_2_or_w_and
    },
    {
        "and_w_2_or_w_and",
        Operators_and_w_2_or_w_and
    },
    {
        "and_w_2_or_w_and_components",
        Operators_and_w_2_or_w_and_components
    },
    {
        "and_w_2_or_w_and_set_this",
        Operators_and_w_2_or_w_and_set_this
    },
    {
        "and_w_2_or_w_and_components_set_this",
        Operators_and_w_2_or_w_and_components_set_this
    },
    {
        "3_or_w_and",
        Operators_3_or_w_and
    },
    {
        "2_or_written",
        Operators_2_or_written
    },
    {
        "3_or_written",
        Operators_3_or_written
    },
    {
        "2_or_written_w_rel_var",
        Operators_2_or_written_w_rel_var
    },
    {
        "3_or_written_w_rel_var",
        Operators_3_or_written_w_rel_var
    },
    {
        "2_or_written_w_tgt_var",
        Operators_2_or_written_w_tgt_var
    },
    {
        "2_or_written_w_rel_tgt_var",
        Operators_2_or_written_w_rel_tgt_var
    },
    {
        "2_or_written_w_rel_tgt_same_var",
        Operators_2_or_written_w_rel_tgt_same_var
    },
    {
        "3_or_written_w_tgt_var",
        Operators_3_or_written_w_tgt_var
    },
    {
        "2_or_chains",
        Operators_2_or_chains
    },
    {
        "2_or_chains_written",
        Operators_2_or_chains_written
    },
    {
        "2_or_dependent",
        Operators_2_or_dependent
    },
    {
        "2_or_dependent_reverse",
        Operators_2_or_dependent_reverse
    },
    {
        "2_or_dependent_2_vars",
        Operators_2_or_dependent_2_vars
    },
    {
        "2_or_written_dependent",
        Operators_2_or_written_dependent
    },
    {
        "2_or_written_dependent_2_vars",
        Operators_2_or_written_dependent_2_vars
    },
    {
        "2_or_w_dependent",
        Operators_2_or_w_dependent
    },
    {
        "2_or_w_both",
        Operators_2_or_w_both
    },
    {
        "3_or_w_both",
        Operators_3_or_w_both
    },
    {
        "2_or_w_not",
        Operators_2_or_w_not
    },
    {
        "2_or_w_not_component",
        Operators_2_or_w_not_component
    },
    {
        "2_or_w_not_out_component",
        Operators_2_or_w_not_out_component
    },
    {
        "2_or_w_not_out_all_components",
        Operators_2_or_w_not_out_all_components
    },
    {
        "2_not_first",
        Operators_2_not_first
    },
    {
        "2_optional_first",
        Operators_2_optional_first
    },
    {
        "only_not",
        Operators_only_not
    },
    {
        "only_not_component",
        Operators_only_not_component
    },
    {
        "only_not_out_component",
        Operators_only_not_out_component
    },
    {
        "only_optional",
        Operators_only_optional
    },
    {
        "only_optional_component",
        Operators_only_optional_component
    },
    {
        "not_after_fixed_src",
        Operators_not_after_fixed_src
    },
    {
        "optional_after_fixed_src",
        Operators_optional_after_fixed_src
    },
    {
        "root_entities_empty",
        Operators_root_entities_empty
    },
    {
        "root_entities",
        Operators_root_entities
    },
    {
        "root_entities_w_children",
        Operators_root_entities_w_children
    },
    {
        "root_entities_w_optional_children",
        Operators_root_entities_w_optional_children
    },
    {
        "core_entities_w_optional_children",
        Operators_core_entities_w_optional_children
    },
    {
        "root_entities_w_not_children",
        Operators_root_entities_w_not_children
    },
    {
        "core_entities_w_not_children",
        Operators_core_entities_w_not_children
    },
    {
        "1_ent_src_not",
        Operators_1_ent_src_not
    },
    {
        "1_ent_src_not_pair",
        Operators_1_ent_src_not_pair
    },
    {
        "1_ent_src_not_pair_rel_wildcard",
        Operators_1_ent_src_not_pair_rel_wildcard
    },
    {
        "1_ent_src_not_pair_tgt_wildcard",
        Operators_1_ent_src_not_pair_tgt_wildcard
    },
    {
        "1_ent_src_not_pair_rel_tgt_wildcard",
        Operators_1_ent_src_not_pair_rel_tgt_wildcard
    },
    {
        "1_ent_src_not_pair_rel_any",
        Operators_1_ent_src_not_pair_rel_any
    },
    {
        "1_ent_src_not_pair_tgt_any",
        Operators_1_ent_src_not_pair_tgt_any
    },
    {
        "1_ent_src_not_pair_rel_tgt_any",
        Operators_1_ent_src_not_pair_rel_tgt_any
    },
    {
        "1_ent_src_not_pair_rel_var",
        Operators_1_ent_src_not_pair_rel_var
    },
    {
        "1_ent_src_not_pair_tgt_var",
        Operators_1_ent_src_not_pair_tgt_var
    },
    {
        "1_ent_src_not_pair_rel_tgt_var",
        Operators_1_ent_src_not_pair_rel_tgt_var
    },
    {
        "1_ent_src_not_pair_rel_tgt_same_var",
        Operators_1_ent_src_not_pair_rel_tgt_same_var
    },
    {
        "1_this_src_not_pair_rel_var",
        Operators_1_this_src_not_pair_rel_var
    },
    {
        "1_this_src_not_pair_tgt_var",
        Operators_1_this_src_not_pair_tgt_var
    },
    {
        "1_this_src_not_pair_rel_tgt_var",
        Operators_1_this_src_not_pair_rel_tgt_var
    },
    {
        "1_this_src_not_pair_rel_tgt_same_var",
        Operators_1_this_src_not_pair_rel_tgt_same_var
    },
    {
        "1_ent_src_not_pair_rel_var_written",
        Operators_1_ent_src_not_pair_rel_var_written
    },
    {
        "1_ent_src_not_pair_tgt_var_written",
        Operators_1_ent_src_not_pair_tgt_var_written
    },
    {
        "1_ent_src_not_pair_rel_tgt_var_written",
        Operators_1_ent_src_not_pair_rel_tgt_var_written
    },
    {
        "1_ent_src_not_pair_rel_tgt_same_var_written",
        Operators_1_ent_src_not_pair_rel_tgt_same_var_written
    },
    {
        "and_from_fixed_src",
        Operators_and_from_fixed_src
    },
    {
        "not_from_fixed_src",
        Operators_not_from_fixed_src
    },
    {
        "or_from_fixed_src",
        Operators_or_from_fixed_src
    },
    {
        "and_from_this",
        Operators_and_from_this
    },
    {
        "not_from_this",
        Operators_not_from_this
    },
    {
        "or_from_this",
        Operators_or_from_this
    },
    {
        "and_from_this_written",
        Operators_and_from_this_written
    },
    {
        "not_from_this_written",
        Operators_not_from_this_written
    },
    {
        "or_from_this_written",
        Operators_or_from_this_written
    },
    {
        "and_from_empty",
        Operators_and_from_empty
    },
    {
        "not_from_empty",
        Operators_not_from_empty
    },
    {
        "or_from_empty",
        Operators_or_from_empty
    },
    {
        "and_from_empty_w_tag",
        Operators_and_from_empty_w_tag
    },
    {
        "not_from_empty_w_tag",
        Operators_not_from_empty_w_tag
    },
    {
        "or_from_empty_w_tag",
        Operators_or_from_empty_w_tag
    },
    {
        "or_w_wildcard",
        Operators_or_w_wildcard
    },
    {
        "or_w_component_and_tag",
        Operators_or_w_component_and_tag
    },
    {
        "or_w_tag_and_component",
        Operators_or_w_tag_and_component
    }
};

bake_test_case Transitive_testcases[] = {
    {
        "1_fact_0_lvl_true",
        Transitive_1_fact_0_lvl_true
    },
    {
        "1_fact_1_lvl_true",
        Transitive_1_fact_1_lvl_true
    },
    {
        "1_fact_2_lvl_true",
        Transitive_1_fact_2_lvl_true
    },
    {
        "1_fact_0_lvl_false",
        Transitive_1_fact_0_lvl_false
    },
    {
        "1_fact_1_lvl_false",
        Transitive_1_fact_1_lvl_false
    },
    {
        "1_fact_2_lvl_false",
        Transitive_1_fact_2_lvl_false
    },
    {
        "1_fact_reflexive",
        Transitive_1_fact_reflexive
    },
    {
        "1_isa",
        Transitive_1_isa
    },
    {
        "1_childof",
        Transitive_1_childof
    },
    {
        "1_this_src_written_0_lvl",
        Transitive_1_this_src_written_0_lvl
    },
    {
        "1_this_src_written_1_lvl",
        Transitive_1_this_src_written_1_lvl
    },
    {
        "1_this_src_written_2_lvl",
        Transitive_1_this_src_written_2_lvl
    },
    {
        "1_this_src_written_reflexive",
        Transitive_1_this_src_written_reflexive
    },
    {
        "1_this_src_0_lvl",
        Transitive_1_this_src_0_lvl
    },
    {
        "1_this_src_1_lvl",
        Transitive_1_this_src_1_lvl
    },
    {
        "1_this_src_2_lvl",
        Transitive_1_this_src_2_lvl
    },
    {
        "1_this_src_reflexive",
        Transitive_1_this_src_reflexive
    },
    {
        "1_ent_src_tgt_var_0_lvl",
        Transitive_1_ent_src_tgt_var_0_lvl
    },
    {
        "1_ent_src_tgt_var_1_lvl",
        Transitive_1_ent_src_tgt_var_1_lvl
    },
    {
        "1_ent_src_tgt_var_2_lvl",
        Transitive_1_ent_src_tgt_var_2_lvl
    },
    {
        "1_ent_src_tgt_var_reflexive",
        Transitive_1_ent_src_tgt_var_reflexive
    },
    {
        "1_this_src_tgt_var",
        Transitive_1_this_src_tgt_var
    },
    {
        "1_this_src_tgt_var_reflexive",
        Transitive_1_this_src_tgt_var_reflexive
    },
    {
        "1_var_src_written_0_lvl",
        Transitive_1_var_src_written_0_lvl
    },
    {
        "1_var_src_written_1_lvl",
        Transitive_1_var_src_written_1_lvl
    },
    {
        "1_var_src_written_2_lvl",
        Transitive_1_var_src_written_2_lvl
    },
    {
        "1_var_src_written_reflexive",
        Transitive_1_var_src_written_reflexive
    },
    {
        "1_var_src_0_lvl",
        Transitive_1_var_src_0_lvl
    },
    {
        "1_var_src_1_lvl",
        Transitive_1_var_src_1_lvl
    },
    {
        "1_var_src_2_lvl",
        Transitive_1_var_src_2_lvl
    },
    {
        "1_var_src_reflexive",
        Transitive_1_var_src_reflexive
    },
    {
        "1_var_src_tgt_var",
        Transitive_1_var_src_tgt_var
    },
    {
        "1_var_src_tgt_var_reflexive",
        Transitive_1_var_src_tgt_var_reflexive
    },
    {
        "1_ent_src_tgt_this_0_lvl",
        Transitive_1_ent_src_tgt_this_0_lvl
    },
    {
        "1_ent_src_tgt_this_1_lvl",
        Transitive_1_ent_src_tgt_this_1_lvl
    },
    {
        "1_ent_src_tgt_this_2_lvl",
        Transitive_1_ent_src_tgt_this_2_lvl
    },
    {
        "1_ent_src_tgt_this_reflexive",
        Transitive_1_ent_src_tgt_this_reflexive
    },
    {
        "1_var_src_tgt_this",
        Transitive_1_var_src_tgt_this
    },
    {
        "1_var_src_tgt_this_reflexive",
        Transitive_1_var_src_tgt_this_reflexive
    },
    {
        "2_ent_src_constrain_tgt_var_before_0_lvl",
        Transitive_2_ent_src_constrain_tgt_var_before_0_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_before_1_lvl",
        Transitive_2_ent_src_constrain_tgt_var_before_1_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_before_2_lvl",
        Transitive_2_ent_src_constrain_tgt_var_before_2_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_0_lvl",
        Transitive_2_ent_src_constrain_tgt_var_after_0_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_1_lvl",
        Transitive_2_ent_src_constrain_tgt_var_after_1_lvl
    },
    {
        "2_ent_src_constrain_tgt_var_after_2_lvl",
        Transitive_2_ent_src_constrain_tgt_var_after_2_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_0_lvl",
        Transitive_2_this_src_constrain_tgt_var_before_0_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_1_lvl",
        Transitive_2_this_src_constrain_tgt_var_before_1_lvl
    },
    {
        "2_this_src_constrain_tgt_var_before_2_lvl",
        Transitive_2_this_src_constrain_tgt_var_before_2_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_0_lvl",
        Transitive_2_this_src_constrain_tgt_var_after_0_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_1_lvl",
        Transitive_2_this_src_constrain_tgt_var_after_1_lvl
    },
    {
        "2_this_src_constrain_tgt_var_after_2_lvl",
        Transitive_2_this_src_constrain_tgt_var_after_2_lvl
    },
    {
        "1_src_tgt_same_var",
        Transitive_1_src_tgt_same_var
    },
    {
        "1_src_tgt_same_var_reflexive",
        Transitive_1_src_tgt_same_var_reflexive
    },
    {
        "1_src_tgt_same_this_var_reflexive",
        Transitive_1_src_tgt_same_this_var_reflexive
    },
    {
        "1_any_src_tgt_var",
        Transitive_1_any_src_tgt_var
    },
    {
        "not_transitive_ent_tgt",
        Transitive_not_transitive_ent_tgt
    },
    {
        "not_transitive_var_tgt",
        Transitive_not_transitive_var_tgt
    },
    {
        "not_transitive_ent_tgt_written",
        Transitive_not_transitive_ent_tgt_written
    },
    {
        "not_transitive_var_tgt_written",
        Transitive_not_transitive_var_tgt_written
    },
    {
        "optional_transitive_ent_tgt",
        Transitive_optional_transitive_ent_tgt
    },
    {
        "optional_transitive_var_tgt",
        Transitive_optional_transitive_var_tgt
    },
    {
        "optional_transitive_ent_tgt_written",
        Transitive_optional_transitive_ent_tgt_written
    },
    {
        "optional_transitive_var_tgt_written",
        Transitive_optional_transitive_var_tgt_written
    },
    {
        "2_var_src_w_same_tgt_ent",
        Transitive_2_var_src_w_same_tgt_ent
    },
    {
        "self_target",
        Transitive_self_target
    },
    {
        "any_target",
        Transitive_any_target
    },
    {
        "isa_prefab",
        Transitive_isa_prefab
    },
    {
        "isa_disabled",
        Transitive_isa_disabled
    },
    {
        "isa_prefab_match_prefab_flag",
        Transitive_isa_prefab_match_prefab_flag
    },
    {
        "isa_prefab_match_prefab_term",
        Transitive_isa_prefab_match_prefab_term
    },
    {
        "isa_disabled_match_disabled_flag",
        Transitive_isa_disabled_match_disabled_flag
    },
    {
        "isa_disabled_match_disabled_term",
        Transitive_isa_disabled_match_disabled_term
    }
};

bake_test_case ComponentInheritance_testcases[] = {
    {
        "1_ent_0_lvl",
        ComponentInheritance_1_ent_0_lvl
    },
    {
        "1_ent_1_lvl",
        ComponentInheritance_1_ent_1_lvl
    },
    {
        "1_ent_2_lvl",
        ComponentInheritance_1_ent_2_lvl
    },
    {
        "1_ent_3_lvl",
        ComponentInheritance_1_ent_3_lvl
    },
    {
        "1_this_0_lvl",
        ComponentInheritance_1_this_0_lvl
    },
    {
        "1_this_1_lvl",
        ComponentInheritance_1_this_1_lvl
    },
    {
        "1_this_2_lvl",
        ComponentInheritance_1_this_2_lvl
    },
    {
        "1_this_3_lvl",
        ComponentInheritance_1_this_3_lvl
    },
    {
        "1_this_0_lvl_written",
        ComponentInheritance_1_this_0_lvl_written
    },
    {
        "1_this_1_lvl_written",
        ComponentInheritance_1_this_1_lvl_written
    },
    {
        "1_this_2_lvl_written",
        ComponentInheritance_1_this_2_lvl_written
    },
    {
        "1_this_3_lvl_written",
        ComponentInheritance_1_this_3_lvl_written
    },
    {
        "1_var_0_lvl",
        ComponentInheritance_1_var_0_lvl
    },
    {
        "1_var_1_lvl",
        ComponentInheritance_1_var_1_lvl
    },
    {
        "1_var_2_lvl",
        ComponentInheritance_1_var_2_lvl
    },
    {
        "1_var_3_lvl",
        ComponentInheritance_1_var_3_lvl
    },
    {
        "1_var_0_lvl_written",
        ComponentInheritance_1_var_0_lvl_written
    },
    {
        "1_var_1_lvl_written",
        ComponentInheritance_1_var_1_lvl_written
    },
    {
        "1_var_2_lvl_written",
        ComponentInheritance_1_var_2_lvl_written
    },
    {
        "1_var_3_lvl_written",
        ComponentInheritance_1_var_3_lvl_written
    },
    {
        "1_ent_1_lvl_self",
        ComponentInheritance_1_ent_1_lvl_self
    },
    {
        "1_this_1_lvl_self",
        ComponentInheritance_1_this_1_lvl_self
    },
    {
        "1_this_1_lvl_written_self",
        ComponentInheritance_1_this_1_lvl_written_self
    },
    {
        "1_var_1_lvl_self",
        ComponentInheritance_1_var_1_lvl_self
    },
    {
        "1_var_1_lvl_written_self",
        ComponentInheritance_1_var_1_lvl_written_self
    },
    {
        "1_ent_src_not",
        ComponentInheritance_1_ent_src_not
    },
    {
        "1_this_src_not",
        ComponentInheritance_1_this_src_not
    },
    {
        "1_var_src_not",
        ComponentInheritance_1_var_src_not
    },
    {
        "1_this_src_not_written",
        ComponentInheritance_1_this_src_not_written
    },
    {
        "1_var_src_not_written",
        ComponentInheritance_1_var_src_not_written
    },
    {
        "first_self",
        ComponentInheritance_first_self
    }
};

bake_test_case Recycled_testcases[] = {
    {
        "recycled_vars",
        Recycled_recycled_vars
    },
    {
        "recycled_pair_vars",
        Recycled_recycled_pair_vars
    },
    {
        "recycled_this_ent_var",
        Recycled_recycled_this_ent_var
    },
    {
        "has_recycled_id_from_pair",
        Recycled_has_recycled_id_from_pair
    },
    {
        "recycled_pair",
        Recycled_recycled_pair
    },
    {
        "recycled_component_id",
        Recycled_recycled_component_id
    }
};

bake_test_case BuiltinPredicates_testcases[] = {
    {
        "this_eq_id",
        BuiltinPredicates_this_eq_id
    },
    {
        "this_eq_name",
        BuiltinPredicates_this_eq_name
    },
    {
        "this_eq_var",
        BuiltinPredicates_this_eq_var
    },
    {
        "this_eq_id_written",
        BuiltinPredicates_this_eq_id_written
    },
    {
        "this_eq_id_written_no_match",
        BuiltinPredicates_this_eq_id_written_no_match
    },
    {
        "this_eq_name_written",
        BuiltinPredicates_this_eq_name_written
    },
    {
        "this_eq_name_written_no_match",
        BuiltinPredicates_this_eq_name_written_no_match
    },
    {
        "this_eq_var_written",
        BuiltinPredicates_this_eq_var_written
    },
    {
        "var_eq_id",
        BuiltinPredicates_var_eq_id
    },
    {
        "var_eq_name",
        BuiltinPredicates_var_eq_name
    },
    {
        "var_eq_var",
        BuiltinPredicates_var_eq_var
    },
    {
        "var_eq_id_written",
        BuiltinPredicates_var_eq_id_written
    },
    {
        "var_eq_id_written_no_match",
        BuiltinPredicates_var_eq_id_written_no_match
    },
    {
        "var_eq_name_written",
        BuiltinPredicates_var_eq_name_written
    },
    {
        "var_eq_name_written_no_match",
        BuiltinPredicates_var_eq_name_written_no_match
    },
    {
        "var_eq_var_written",
        BuiltinPredicates_var_eq_var_written
    },
    {
        "var_eq_this",
        BuiltinPredicates_var_eq_this
    },
    {
        "this_neq_id",
        BuiltinPredicates_this_neq_id
    },
    {
        "this_neq_name",
        BuiltinPredicates_this_neq_name
    },
    {
        "this_neq_var",
        BuiltinPredicates_this_neq_var
    },
    {
        "this_neq_id_written",
        BuiltinPredicates_this_neq_id_written
    },
    {
        "this_neq_id_written_no_match",
        BuiltinPredicates_this_neq_id_written_no_match
    },
    {
        "this_neq_name_written",
        BuiltinPredicates_this_neq_name_written
    },
    {
        "this_neq_name_written_no_match",
        BuiltinPredicates_this_neq_name_written_no_match
    },
    {
        "this_neq_var_written",
        BuiltinPredicates_this_neq_var_written
    },
    {
        "var_neq_id",
        BuiltinPredicates_var_neq_id
    },
    {
        "var_neq_name",
        BuiltinPredicates_var_neq_name
    },
    {
        "var_neq_var",
        BuiltinPredicates_var_neq_var
    },
    {
        "var_neq_id_written",
        BuiltinPredicates_var_neq_id_written
    },
    {
        "var_neq_id_written_no_match",
        BuiltinPredicates_var_neq_id_written_no_match
    },
    {
        "var_neq_name_written",
        BuiltinPredicates_var_neq_name_written
    },
    {
        "var_neq_name_written_no_match",
        BuiltinPredicates_var_neq_name_written_no_match
    },
    {
        "var_neq_var_written",
        BuiltinPredicates_var_neq_var_written
    },
    {
        "var_neq_this",
        BuiltinPredicates_var_neq_this
    },
    {
        "this_2_neq_id",
        BuiltinPredicates_this_2_neq_id
    },
    {
        "this_2_neq_name",
        BuiltinPredicates_this_2_neq_name
    },
    {
        "var_2_neq_id",
        BuiltinPredicates_var_2_neq_id
    },
    {
        "var_2_neq_name",
        BuiltinPredicates_var_2_neq_name
    },
    {
        "this_2_neq_id_written",
        BuiltinPredicates_this_2_neq_id_written
    },
    {
        "this_2_neq_name_written",
        BuiltinPredicates_this_2_neq_name_written
    },
    {
        "var_2_neq_id_written",
        BuiltinPredicates_var_2_neq_id_written
    },
    {
        "var_2_neq_name_written",
        BuiltinPredicates_var_2_neq_name_written
    },
    {
        "this_2_or_id",
        BuiltinPredicates_this_2_or_id
    },
    {
        "this_3_or_id",
        BuiltinPredicates_this_3_or_id
    },
    {
        "this_2_or_name",
        BuiltinPredicates_this_2_or_name
    },
    {
        "this_3_or_name",
        BuiltinPredicates_this_3_or_name
    },
    {
        "this_2_or_match",
        BuiltinPredicates_this_2_or_match
    },
    {
        "this_3_or_match",
        BuiltinPredicates_this_3_or_match
    },
    {
        "var_2_or_id",
        BuiltinPredicates_var_2_or_id
    },
    {
        "var_2_or_name",
        BuiltinPredicates_var_2_or_name
    },
    {
        "this_2_or_id_written",
        BuiltinPredicates_this_2_or_id_written
    },
    {
        "this_3_or_id_written",
        BuiltinPredicates_this_3_or_id_written
    },
    {
        "this_2_or_name_written",
        BuiltinPredicates_this_2_or_name_written
    },
    {
        "var_2_or_id_written",
        BuiltinPredicates_var_2_or_id_written
    },
    {
        "var_2_or_name_written",
        BuiltinPredicates_var_2_or_name_written
    },
    {
        "this_match_eq",
        BuiltinPredicates_this_match_eq
    },
    {
        "var_match_eq",
        BuiltinPredicates_var_match_eq
    },
    {
        "this_match_eq_written",
        BuiltinPredicates_this_match_eq_written
    },
    {
        "this_match_eq_written_self",
        BuiltinPredicates_this_match_eq_written_self
    },
    {
        "var_match_eq_written",
        BuiltinPredicates_var_match_eq_written
    },
    {
        "this_match_neq",
        BuiltinPredicates_this_match_neq
    },
    {
        "var_match_neq",
        BuiltinPredicates_var_match_neq
    },
    {
        "this_match_neq_written",
        BuiltinPredicates_this_match_neq_written
    },
    {
        "var_match_neq_written",
        BuiltinPredicates_var_match_neq_written
    },
    {
        "this_match_2_neq",
        BuiltinPredicates_this_match_2_neq
    },
    {
        "var_match_2_neq",
        BuiltinPredicates_var_match_2_neq
    },
    {
        "this_match_2_neq_written",
        BuiltinPredicates_this_match_2_neq_written
    },
    {
        "var_match_2_neq_written",
        BuiltinPredicates_var_match_2_neq_written
    },
    {
        "this_match_2_or",
        BuiltinPredicates_this_match_2_or
    },
    {
        "this_match_2_or_written",
        BuiltinPredicates_this_match_2_or_written
    },
    {
        "this_match_3_or",
        BuiltinPredicates_this_match_3_or
    },
    {
        "this_match_3_or_written",
        BuiltinPredicates_this_match_3_or_written
    },
    {
        "unresolved_by_name",
        BuiltinPredicates_unresolved_by_name
    },
    {
        "var_eq_wildcard",
        BuiltinPredicates_var_eq_wildcard
    },
    {
        "var_eq_any",
        BuiltinPredicates_var_eq_any
    },
    {
        "var_eq_wildcard_after_write",
        BuiltinPredicates_var_eq_wildcard_after_write
    },
    {
        "var_eq_any_after_write",
        BuiltinPredicates_var_eq_any_after_write
    },
    {
        "var_eq_after_var_0_src",
        BuiltinPredicates_var_eq_after_var_0_src
    },
    {
        "2_or_w_eq_this",
        BuiltinPredicates_2_or_w_eq_this
    },
    {
        "2_or_w_eq_lookup_var",
        BuiltinPredicates_2_or_w_eq_lookup_var
    },
    {
        "3_or_w_eq_lookup_var",
        BuiltinPredicates_3_or_w_eq_lookup_var
    },
    {
        "eq_variable",
        BuiltinPredicates_eq_variable
    },
    {
        "eq_wildcard",
        BuiltinPredicates_eq_wildcard
    },
    {
        "eq_any",
        BuiltinPredicates_eq_any
    },
    {
        "neq_variable",
        BuiltinPredicates_neq_variable
    },
    {
        "neq_wildcard",
        BuiltinPredicates_neq_wildcard
    },
    {
        "neq_any",
        BuiltinPredicates_neq_any
    },
    {
        "match_variable",
        BuiltinPredicates_match_variable
    },
    {
        "match_wildcard",
        BuiltinPredicates_match_wildcard
    },
    {
        "match_any",
        BuiltinPredicates_match_any
    }
};

bake_test_case Scopes_testcases[] = {
    {
        "term_w_not_scope_1_term",
        Scopes_term_w_not_scope_1_term
    },
    {
        "term_w_not_scope_2_terms",
        Scopes_term_w_not_scope_2_terms
    },
    {
        "term_w_not_scope_1_term_w_not",
        Scopes_term_w_not_scope_1_term_w_not
    },
    {
        "term_w_not_scope_2_terms_w_not",
        Scopes_term_w_not_scope_2_terms_w_not
    },
    {
        "term_w_not_scope_1_term_w_var",
        Scopes_term_w_not_scope_1_term_w_var
    },
    {
        "term_w_not_scope_2_terms_w_var",
        Scopes_term_w_not_scope_2_terms_w_var
    },
    {
        "term_w_not_scope_1_term_w_not_w_var",
        Scopes_term_w_not_scope_1_term_w_not_w_var
    },
    {
        "term_w_not_scope_2_terms_w_not_w_var",
        Scopes_term_w_not_scope_2_terms_w_not_w_var
    },
    {
        "term_w_not_scope_2_terms_w_or",
        Scopes_term_w_not_scope_2_terms_w_or
    },
    {
        "term_w_not_scope_3_terms_w_or",
        Scopes_term_w_not_scope_3_terms_w_or
    },
    {
        "term_w_not_scope_2_terms_w_before_after",
        Scopes_term_w_not_scope_2_terms_w_before_after
    }
};

bake_test_case Traversal_testcases[] = {
    {
        "this_self_up_childof",
        Traversal_this_self_up_childof
    },
    {
        "this_up_childof",
        Traversal_this_up_childof
    },
    {
        "this_written_self_up_childof",
        Traversal_this_written_self_up_childof
    },
    {
        "this_written_up_childof",
        Traversal_this_written_up_childof
    },
    {
        "var_self_up_childof",
        Traversal_var_self_up_childof
    },
    {
        "var_up_childof",
        Traversal_var_up_childof
    },
    {
        "var_written_self_up_childof",
        Traversal_var_written_self_up_childof
    },
    {
        "var_written_up_childof",
        Traversal_var_written_up_childof
    },
    {
        "set_var_self_up_childof",
        Traversal_set_var_self_up_childof
    },
    {
        "set_var_up_childof",
        Traversal_set_var_up_childof
    },
    {
        "set_var_written_self_up_childof",
        Traversal_set_var_written_self_up_childof
    },
    {
        "set_var_written_up_childof",
        Traversal_set_var_written_up_childof
    },
    {
        "ent_self_up_childof",
        Traversal_ent_self_up_childof
    },
    {
        "ent_up_childof",
        Traversal_ent_up_childof
    },
    {
        "implicit_this_self_up_isa",
        Traversal_implicit_this_self_up_isa
    },
    {
        "implicit_this_up_isa",
        Traversal_implicit_this_up_isa
    },
    {
        "implicit_var_self_up_isa",
        Traversal_implicit_var_self_up_isa
    },
    {
        "implicit_var_up_isa",
        Traversal_implicit_var_up_isa
    },
    {
        "implicit_ent_self_up_isa",
        Traversal_implicit_ent_self_up_isa
    },
    {
        "implicit_ent_up_isa",
        Traversal_implicit_ent_up_isa
    },
    {
        "self_up_2_targets",
        Traversal_self_up_2_targets
    },
    {
        "up_2_targets",
        Traversal_up_2_targets
    },
    {
        "self_up_2_targets_diamond",
        Traversal_self_up_2_targets_diamond
    },
    {
        "up_2_targets_diamond",
        Traversal_up_2_targets_diamond
    },
    {
        "written_self_up_2_targets",
        Traversal_written_self_up_2_targets
    },
    {
        "written_up_2_targets",
        Traversal_written_up_2_targets
    },
    {
        "written_self_up_2_targets_diamond",
        Traversal_written_self_up_2_targets_diamond
    },
    {
        "written_up_2_targets_diamond",
        Traversal_written_up_2_targets_diamond
    },
    {
        "2_self_up_terms",
        Traversal_2_self_up_terms
    },
    {
        "2_self_up_terms_2_targets",
        Traversal_2_self_up_terms_2_targets
    },
    {
        "self_up_empty_table",
        Traversal_self_up_empty_table
    },
    {
        "up_empty_table",
        Traversal_up_empty_table
    },
    {
        "self_up_match_empty_table",
        Traversal_self_up_match_empty_table
    },
    {
        "up_match_empty_table",
        Traversal_up_match_empty_table
    },
    {
        "self_up_all_owned",
        Traversal_self_up_all_owned
    },
    {
        "up_all_owned",
        Traversal_up_all_owned
    },
    {
        "this_self_up_childof_inherited",
        Traversal_this_self_up_childof_inherited
    },
    {
        "this_up_childof_inherited",
        Traversal_this_up_childof_inherited
    },
    {
        "this_written_self_up_childof_inherited",
        Traversal_this_written_self_up_childof_inherited
    },
    {
        "this_written_up_childof_inherited",
        Traversal_this_written_up_childof_inherited
    },
    {
        "var_self_up_childof_inherited",
        Traversal_var_self_up_childof_inherited
    },
    {
        "var_up_childof_inherited",
        Traversal_var_up_childof_inherited
    },
    {
        "var_written_self_up_childof_inherited",
        Traversal_var_written_self_up_childof_inherited
    },
    {
        "var_written_up_childof_inherited",
        Traversal_var_written_up_childof_inherited
    },
    {
        "ent_self_up_childof_inherited",
        Traversal_ent_self_up_childof_inherited
    },
    {
        "ent_up_childof_inherited",
        Traversal_ent_up_childof_inherited
    },
    {
        "ent_written_self_up_childof_inherited",
        Traversal_ent_written_self_up_childof_inherited
    },
    {
        "ent_written_up_childof_inherited",
        Traversal_ent_written_up_childof_inherited
    },
    {
        "this_self_up_childof_component",
        Traversal_this_self_up_childof_component
    },
    {
        "this_up_childof_component",
        Traversal_this_up_childof_component
    },
    {
        "this_written_self_up_childof_component",
        Traversal_this_written_self_up_childof_component
    },
    {
        "this_written_up_childof_component",
        Traversal_this_written_up_childof_component
    },
    {
        "var_self_up_childof_component",
        Traversal_var_self_up_childof_component
    },
    {
        "var_up_childof_component",
        Traversal_var_up_childof_component
    },
    {
        "var_written_self_up_childof_component",
        Traversal_var_written_self_up_childof_component
    },
    {
        "var_written_up_childof_component",
        Traversal_var_written_up_childof_component
    },
    {
        "this_self_up_childof_recycled_parent",
        Traversal_this_self_up_childof_recycled_parent
    },
    {
        "this_up_childof_recycled_parent",
        Traversal_this_up_childof_recycled_parent
    },
    {
        "this_written_self_up_childof_recycled_parent",
        Traversal_this_written_self_up_childof_recycled_parent
    },
    {
        "this_written_up_childof_recycled_parent",
        Traversal_this_written_up_childof_recycled_parent
    },
    {
        "this_self_up_childof_recycled_parent_component",
        Traversal_this_self_up_childof_recycled_parent_component
    },
    {
        "this_up_childof_recycled_parent_component",
        Traversal_this_up_childof_recycled_parent_component
    },
    {
        "this_written_self_up_childof_recycled_parent_component",
        Traversal_this_written_self_up_childof_recycled_parent_component
    },
    {
        "this_written_up_childof_recycled_parent_component",
        Traversal_this_written_up_childof_recycled_parent_component
    },
    {
        "this_self_up_childof_pair",
        Traversal_this_self_up_childof_pair
    },
    {
        "this_up_childof_pair",
        Traversal_this_up_childof_pair
    },
    {
        "this_written_self_up_childof_pair",
        Traversal_this_written_self_up_childof_pair
    },
    {
        "this_written_up_childof_pair",
        Traversal_this_written_up_childof_pair
    },
    {
        "this_self_up_childof_pair_wildcard",
        Traversal_this_self_up_childof_pair_wildcard
    },
    {
        "this_up_childof_pair_wildcard",
        Traversal_this_up_childof_pair_wildcard
    },
    {
        "this_written_self_up_childof_pair_wildcard",
        Traversal_this_written_self_up_childof_pair_wildcard
    },
    {
        "this_written_up_childof_pair_wildcard",
        Traversal_this_written_up_childof_pair_wildcard
    },
    {
        "this_self_up_childof_pair_tgt_var",
        Traversal_this_self_up_childof_pair_tgt_var
    },
    {
        "this_written_self_up_childof_pair_tgt_var",
        Traversal_this_written_self_up_childof_pair_tgt_var
    },
    {
        "this_self_up_childof_pair_rel_var",
        Traversal_this_self_up_childof_pair_rel_var
    },
    {
        "this_written_self_up_childof_pair_rel_var",
        Traversal_this_written_self_up_childof_pair_rel_var
    },
    {
        "this_self_up_childof_pair_for_var_written",
        Traversal_this_self_up_childof_pair_for_var_written
    },
    {
        "this_up_childof_pair_for_var_written",
        Traversal_this_up_childof_pair_for_var_written
    },
    {
        "this_written_self_up_childof_pair_for_var_written",
        Traversal_this_written_self_up_childof_pair_for_var_written
    },
    {
        "this_self_up_childof_pair_for_var_written_n_targets",
        Traversal_this_self_up_childof_pair_for_var_written_n_targets
    },
    {
        "this_written_self_up_childof_pair_for_var_written_n_targets",
        Traversal_this_written_self_up_childof_pair_for_var_written_n_targets
    },
    {
        "self_up_2_levels_w_prefab",
        Traversal_self_up_2_levels_w_prefab
    },
    {
        "self_up_2_levels_other_trav_rel_w_prefab",
        Traversal_self_up_2_levels_other_trav_rel_w_prefab
    },
    {
        "up_2_levels_w_prefab",
        Traversal_up_2_levels_w_prefab
    },
    {
        "up_2_levels_other_trav_rel_w_prefab",
        Traversal_up_2_levels_other_trav_rel_w_prefab
    },
    {
        "self_up_2_levels",
        Traversal_self_up_2_levels
    },
    {
        "self_up_2_levels_other_trav_rel",
        Traversal_self_up_2_levels_other_trav_rel
    },
    {
        "up_2_levels",
        Traversal_up_2_levels
    },
    {
        "up_2_levels_other_trav_rel",
        Traversal_up_2_levels_other_trav_rel
    },
    {
        "self_up_mixed_traversable",
        Traversal_self_up_mixed_traversable
    },
    {
        "not_up",
        Traversal_not_up
    },
    {
        "not_self_up",
        Traversal_not_self_up
    },
    {
        "not_up_wildcard",
        Traversal_not_up_wildcard
    },
    {
        "not_self_up_wildcard",
        Traversal_not_self_up_wildcard
    },
    {
        "not_up_disabled",
        Traversal_not_up_disabled
    },
    {
        "up_2_rel_instances",
        Traversal_up_2_rel_instances
    },
    {
        "up_2_rel_instances_match_2nd",
        Traversal_up_2_rel_instances_match_2nd
    },
    {
        "up_only_w_owned",
        Traversal_up_only_w_owned
    },
    {
        "this_up_trav_unused_rel",
        Traversal_this_up_trav_unused_rel
    },
    {
        "this_optional_self",
        Traversal_this_optional_self
    },
    {
        "this_optional_up",
        Traversal_this_optional_up
    },
    {
        "this_optional_self_up",
        Traversal_this_optional_self_up
    },
    {
        "this_written_optional_self",
        Traversal_this_written_optional_self
    },
    {
        "this_written_optional_up",
        Traversal_this_written_optional_up
    },
    {
        "this_written_optional_self_up",
        Traversal_this_written_optional_self_up
    },
    {
        "fixed_src_w_up",
        Traversal_fixed_src_w_up
    },
    {
        "match_empty_table_up",
        Traversal_match_empty_table_up
    },
    {
        "match_empty_table_up_written",
        Traversal_match_empty_table_up_written
    },
    {
        "match_empty_table_up_implicit_isa",
        Traversal_match_empty_table_up_implicit_isa
    },
    {
        "match_empty_table_up_written_implicit_isa",
        Traversal_match_empty_table_up_written_implicit_isa
    },
    {
        "match_empty_table_up_isa",
        Traversal_match_empty_table_up_isa
    },
    {
        "match_empty_table_up_written_isa",
        Traversal_match_empty_table_up_written_isa
    },
    {
        "up_after_add_batched_to_parent",
        Traversal_up_after_add_batched_to_parent
    },
    {
        "up_component_after_parent_table_change",
        Traversal_up_component_after_parent_table_change
    },
    {
        "up_component_w_singleton_after_parent_table_change",
        Traversal_up_component_w_singleton_after_parent_table_change
    },
    {
        "up_component_w_var_after_parent_table_change",
        Traversal_up_component_w_var_after_parent_table_change
    },
    {
        "test_up_component_after_parent_table_change",
        Traversal_test_up_component_after_parent_table_change
    },
    {
        "test_up_component_w_singleton_after_parent_table_change",
        Traversal_test_up_component_w_singleton_after_parent_table_change
    },
    {
        "up_component_after_parent_table_change_no_data",
        Traversal_up_component_after_parent_table_change_no_data
    },
    {
        "up_component_w_singleton_after_parent_table_change_no_data",
        Traversal_up_component_w_singleton_after_parent_table_change_no_data
    },
    {
        "up_component_w_var_after_parent_table_change_no_data",
        Traversal_up_component_w_var_after_parent_table_change_no_data
    },
    {
        "test_up_component_after_parent_table_change_no_data",
        Traversal_test_up_component_after_parent_table_change_no_data
    },
    {
        "test_up_component_w_singleton_after_parent_table_change_no_data",
        Traversal_test_up_component_w_singleton_after_parent_table_change_no_data
    }
};

bake_test_case Cascade_testcases[] = {
    {
        "this_self_cascade_childof_uncached",
        Cascade_this_self_cascade_childof_uncached
    },
    {
        "this_cascade_childof_uncached",
        Cascade_this_cascade_childof_uncached
    },
    {
        "this_written_self_cascade_childof_uncached",
        Cascade_this_written_self_cascade_childof_uncached
    },
    {
        "this_written_cascade_childof_uncached",
        Cascade_this_written_cascade_childof_uncached
    },
    {
        "this_self_cascade_childof_w_parent_flag_uncached",
        Cascade_this_self_cascade_childof_w_parent_flag_uncached
    },
    {
        "this_cascade_childof_w_parent_flag_uncached",
        Cascade_this_cascade_childof_w_parent_flag_uncached
    },
    {
        "this_written_self_cascade_childof_w_parent_flag_uncached",
        Cascade_this_written_self_cascade_childof_w_parent_flag_uncached
    },
    {
        "this_written_cascade_childof_w_parent_flag_uncached",
        Cascade_this_written_cascade_childof_w_parent_flag_uncached
    },
    {
        "parent_cascade",
        Cascade_parent_cascade
    },
    {
        "existing_custom_rel_cascade",
        Cascade_existing_custom_rel_cascade
    },
    {
        "new_custom_rel_cascade",
        Cascade_new_custom_rel_cascade
    },
    {
        "cascade_w_2_depths",
        Cascade_cascade_w_2_depths
    },
    {
        "cascade_w_3_depths",
        Cascade_cascade_w_3_depths
    },
    {
        "cascade_w_2_depths_desc",
        Cascade_cascade_w_2_depths_desc
    },
    {
        "cascade_w_3_depths_desc",
        Cascade_cascade_w_3_depths_desc
    },
    {
        "existing_isa_cascade",
        Cascade_existing_isa_cascade
    },
    {
        "new_isa_cascade",
        Cascade_new_isa_cascade
    },
    {
        "childof_cascade",
        Cascade_childof_cascade
    },
    {
        "cascade_rematch_2_lvls",
        Cascade_cascade_rematch_2_lvls
    },
    {
        "cascade_rematch_2_lvls_2_relations",
        Cascade_cascade_rematch_2_lvls_2_relations
    },
    {
        "cascade_topological",
        Cascade_cascade_topological
    },
    {
        "cascade_desc_rematch_2_lvls",
        Cascade_cascade_desc_rematch_2_lvls
    },
    {
        "cascade_desc_rematch_2_lvls_2_relations",
        Cascade_cascade_desc_rematch_2_lvls_2_relations
    },
    {
        "cascade_desc_topological",
        Cascade_cascade_desc_topological
    },
    {
        "cascade_after_recycled_parent_change",
        Cascade_cascade_after_recycled_parent_change
    }
};

bake_test_case Cached_testcases[] = {
    {
        "simple_query_existing_table",
        Cached_simple_query_existing_table
    },
    {
        "simple_query_2_existing_tables",
        Cached_simple_query_2_existing_tables
    },
    {
        "simple_query_new_table",
        Cached_simple_query_new_table
    },
    {
        "simple_query_2_new_tables",
        Cached_simple_query_2_new_tables
    },
    {
        "simple_query_existing_and_new_table",
        Cached_simple_query_existing_and_new_table
    },
    {
        "wildcard_query_existing_table",
        Cached_wildcard_query_existing_table
    },
    {
        "wildcard_query_new_table",
        Cached_wildcard_query_new_table
    },
    {
        "wildcard_query_existing_table_2_results_p_table",
        Cached_wildcard_query_existing_table_2_results_p_table
    },
    {
        "wildcard_query_new_table_2_results_p_table",
        Cached_wildcard_query_new_table_2_results_p_table
    },
    {
        "wildcard_query_2nd_term",
        Cached_wildcard_query_2nd_term
    },
    {
        "wildcard_query_2nd_term_self",
        Cached_wildcard_query_2nd_term_self
    },
    {
        "simple_query_existing_empty_table",
        Cached_simple_query_existing_empty_table
    },
    {
        "simple_query_existing_empty_type",
        Cached_simple_query_existing_empty_type
    },
    {
        "simple_query_new_empty_table",
        Cached_simple_query_new_empty_table
    },
    {
        "component_query_existing_table",
        Cached_component_query_existing_table
    },
    {
        "component_query_new_table",
        Cached_component_query_new_table
    },
    {
        "component_query_existing_empty_table",
        Cached_component_query_existing_empty_table
    },
    {
        "2_component_query_existing_empty_table",
        Cached_2_component_query_existing_empty_table
    },
    {
        "2_component_query_existing_empty_type",
        Cached_2_component_query_existing_empty_type
    },
    {
        "only_optional",
        Cached_only_optional
    },
    {
        "only_optional_new_empty_table",
        Cached_only_optional_new_empty_table
    },
    {
        "only_optional_new_empty_non_empty_table",
        Cached_only_optional_new_empty_non_empty_table
    },
    {
        "only_optional_new_unset_tables",
        Cached_only_optional_new_unset_tables
    },
    {
        "singleton_w_optional_new_empty_table",
        Cached_singleton_w_optional_new_empty_table
    },
    {
        "singleton_w_optional_new_empty_non_empty_table",
        Cached_singleton_w_optional_new_empty_non_empty_table
    },
    {
        "singleton_w_optional_new_unset_tables",
        Cached_singleton_w_optional_new_unset_tables
    },
    {
        "query_w_from_entity_match_after",
        Cached_query_w_from_entity_match_after
    },
    {
        "query_w_from_singleton_match_after",
        Cached_query_w_from_singleton_match_after
    },
    {
        "query_rematch_optional_after_add",
        Cached_query_rematch_optional_after_add
    },
    {
        "get_owned_tag",
        Cached_get_owned_tag
    },
    {
        "get_shared_tag",
        Cached_get_shared_tag
    },
    {
        "explicit_delete",
        Cached_explicit_delete
    },
    {
        "get_column_size",
        Cached_get_column_size
    },
    {
        "stresstest_query_free",
        Cached_stresstest_query_free
    },
    {
        "query_optional_tag",
        Cached_query_optional_tag
    },
    {
        "query_optional_shared_tag",
        Cached_query_optional_shared_tag
    },
    {
        "query_iter_10_tags",
        Cached_query_iter_10_tags
    },
    {
        "query_iter_10_components",
        Cached_query_iter_10_components
    },
    {
        "iter_type_set",
        Cached_iter_type_set
    },
    {
        "filter_term",
        Cached_filter_term
    },
    {
        "2_terms_1_filter",
        Cached_2_terms_1_filter
    },
    {
        "3_terms_2_filter",
        Cached_3_terms_2_filter
    },
    {
        "add_singleton_after_query",
        Cached_add_singleton_after_query
    },
    {
        "query_w_component_from_parent_from_non_this",
        Cached_query_w_component_from_parent_from_non_this
    },
    {
        "create_query_while_pending",
        Cached_create_query_while_pending
    },
    {
        "empty_query",
        Cached_empty_query
    },
    {
        "not_pair_relation_wildcard",
        Cached_not_pair_relation_wildcard
    },
    {
        "not_pair_object_wildcard",
        Cached_not_pair_object_wildcard
    },
    {
        "two_pair_wildcards_one_not",
        Cached_two_pair_wildcards_one_not
    },
    {
        "two_pair_wildcards_one_not_any",
        Cached_two_pair_wildcards_one_not_any
    },
    {
        "implicit_existing_isa_superset",
        Cached_implicit_existing_isa_superset
    },
    {
        "implicit_new_isa_superset",
        Cached_implicit_new_isa_superset
    },
    {
        "isa_superset",
        Cached_isa_superset
    },
    {
        "isa_superset_2_lvls",
        Cached_isa_superset_2_lvls
    },
    {
        "isa_superset_3_lvls",
        Cached_isa_superset_3_lvls
    },
    {
        "isa_superset_2_lvls_owned",
        Cached_isa_superset_2_lvls_owned
    },
    {
        "isa_superset_3_lvls_owned",
        Cached_isa_superset_3_lvls_owned
    },
    {
        "isa_superset_owned_empty_table_after_match",
        Cached_isa_superset_owned_empty_table_after_match
    },
    {
        "isa_self_superset",
        Cached_isa_self_superset
    },
    {
        "childof_superset",
        Cached_childof_superset
    },
    {
        "superset_2_targets",
        Cached_superset_2_targets
    },
    {
        "superset_2_relations",
        Cached_superset_2_relations
    },
    {
        "superset_2_relations_instanced",
        Cached_superset_2_relations_instanced
    },
    {
        "superset_2_relations_w_component",
        Cached_superset_2_relations_w_component
    },
    {
        "superset_2_relations_instanced_w_component",
        Cached_superset_2_relations_instanced_w_component
    },
    {
        "parent",
        Cached_parent
    },
    {
        "isa_rematch",
        Cached_isa_rematch
    },
    {
        "childof_rematch",
        Cached_childof_rematch
    },
    {
        "isa_unmatch",
        Cached_isa_unmatch
    },
    {
        "childof_unmatch",
        Cached_childof_unmatch
    },
    {
        "isa_rematch_2_lvls",
        Cached_isa_rematch_2_lvls
    },
    {
        "childof_rematch_2_lvls",
        Cached_childof_rematch_2_lvls
    },
    {
        "childof_rematch_from_isa",
        Cached_childof_rematch_from_isa
    },
    {
        "rematch_optional_ref",
        Cached_rematch_optional_ref
    },
    {
        "rematch_optional_ref_w_2_refs",
        Cached_rematch_optional_ref_w_2_refs
    },
    {
        "rematch_optional_ref_tag_w_ref_component",
        Cached_rematch_optional_ref_tag_w_ref_component
    },
    {
        "rematch_after_add_to_recycled_parent",
        Cached_rematch_after_add_to_recycled_parent
    },
    {
        "match_query_expr_from_scope",
        Cached_match_query_expr_from_scope
    },
    {
        "query_long_or_w_ref",
        Cached_query_long_or_w_ref
    },
    {
        "query_w_pair_id_and_subj",
        Cached_query_w_pair_id_and_subj
    },
    {
        "rematch_after_delete_inherited_tag",
        Cached_rematch_after_delete_inherited_tag
    },
    {
        "rematch_after_delete_rel_of_inherited_pair",
        Cached_rematch_after_delete_rel_of_inherited_pair
    },
    {
        "rematch_after_delete_obj_of_inherited_pair",
        Cached_rematch_after_delete_obj_of_inherited_pair
    },
    {
        "rematch_empty",
        Cached_rematch_empty
    },
    {
        "rematch_empty_table_w_superset",
        Cached_rematch_empty_table_w_superset
    },
    {
        "2_self_up_terms_new_tables",
        Cached_2_self_up_terms_new_tables
    },
    {
        "this_self_up_childof_pair_new_tables",
        Cached_this_self_up_childof_pair_new_tables
    }
};

bake_test_case ChangeDetection_testcases[] = {
    {
        "query_changed_after_new",
        ChangeDetection_query_changed_after_new
    },
    {
        "query_changed_after_delete",
        ChangeDetection_query_changed_after_delete
    },
    {
        "query_changed_after_add",
        ChangeDetection_query_changed_after_add
    },
    {
        "query_changed_after_remove",
        ChangeDetection_query_changed_after_remove
    },
    {
        "query_changed_after_set",
        ChangeDetection_query_changed_after_set
    },
    {
        "query_change_after_modified",
        ChangeDetection_query_change_after_modified
    },
    {
        "query_change_after_out_system",
        ChangeDetection_query_change_after_out_system
    },
    {
        "query_change_after_out_query_no_data_flag",
        ChangeDetection_query_change_after_out_query_no_data_flag
    },
    {
        "query_change_after_in_system",
        ChangeDetection_query_change_after_in_system
    },
    {
        "query_change_after_modified_out_term",
        ChangeDetection_query_change_after_modified_out_term
    },
    {
        "query_change_check_iter",
        ChangeDetection_query_change_check_iter
    },
    {
        "query_change_check_iter_after_skip_read",
        ChangeDetection_query_change_check_iter_after_skip_read
    },
    {
        "query_change_check_iter_after_skip_write",
        ChangeDetection_query_change_check_iter_after_skip_write
    },
    {
        "query_change_parent_term",
        ChangeDetection_query_change_parent_term
    },
    {
        "query_change_prefab_term",
        ChangeDetection_query_change_prefab_term
    },
    {
        "query_change_parent_term_w_tag",
        ChangeDetection_query_change_parent_term_w_tag
    },
    {
        "query_change_prefab_term_w_tag",
        ChangeDetection_query_change_prefab_term_w_tag
    },
    {
        "query_changed_w_or",
        ChangeDetection_query_changed_w_or
    },
    {
        "query_changed_or",
        ChangeDetection_query_changed_or
    },
    {
        "query_changed_w_singleton",
        ChangeDetection_query_changed_w_singleton
    },
    {
        "query_changed_w_only_singleton",
        ChangeDetection_query_changed_w_only_singleton
    },
    {
        "query_changed_w_only_singleton_after_set",
        ChangeDetection_query_changed_w_only_singleton_after_set
    },
    {
        "query_changed_w_only_singleton_after_out_term",
        ChangeDetection_query_changed_w_only_singleton_after_out_term
    },
    {
        "query_changed_w_only_singleton_after_singleton_out_term",
        ChangeDetection_query_changed_w_only_singleton_after_singleton_out_term
    },
    {
        "query_changed_w_only_parent",
        ChangeDetection_query_changed_w_only_parent
    },
    {
        "query_changed_w_only_parent_after_set",
        ChangeDetection_query_changed_w_only_parent_after_set
    },
    {
        "query_changed_w_only_parent_after_out_term",
        ChangeDetection_query_changed_w_only_parent_after_out_term
    },
    {
        "query_changed_w_only_parent_after_parent_out_term",
        ChangeDetection_query_changed_w_only_parent_after_parent_out_term
    },
    {
        "query_changed_tag",
        ChangeDetection_query_changed_tag
    },
    {
        "query_changed_no_source",
        ChangeDetection_query_changed_no_source
    },
    {
        "query_changed_no_source_component",
        ChangeDetection_query_changed_no_source_component
    },
    {
        "query_changed_w_not_out",
        ChangeDetection_query_changed_w_not_out
    },
    {
        "query_change_w_optional",
        ChangeDetection_query_change_w_optional
    },
    {
        "query_changed_after_count",
        ChangeDetection_query_changed_after_count
    }
};

bake_test_case GroupBy_testcases[] = {
    {
        "group_by",
        GroupBy_group_by
    },
    {
        "group_by_w_ctx",
        GroupBy_group_by_w_ctx
    },
    {
        "group_by_w_sort_reverse_group_creation",
        GroupBy_group_by_w_sort_reverse_group_creation
    },
    {
        "group_by_iter_one",
        GroupBy_group_by_iter_one
    },
    {
        "group_by_iter_one_all_groups",
        GroupBy_group_by_iter_one_all_groups
    },
    {
        "group_by_iter_one_empty",
        GroupBy_group_by_iter_one_empty
    },
    {
        "group_by_iter_one_empty_query",
        GroupBy_group_by_iter_one_empty_query
    },
    {
        "group_by_iter_one_empty_table",
        GroupBy_group_by_iter_one_empty_table
    },
    {
        "group_by_w_deleted_group_id",
        GroupBy_group_by_w_deleted_group_id
    },
    {
        "group_by_callbacks",
        GroupBy_group_by_callbacks
    },
    {
        "group_by_default_action",
        GroupBy_group_by_default_action
    },
    {
        "group_table_count",
        GroupBy_group_table_count
    }
};

bake_test_case MemberTarget_testcases[] = {
    {
        "this_member_eq_1",
        MemberTarget_this_member_eq_1
    },
    {
        "this_member_eq_2",
        MemberTarget_this_member_eq_2
    },
    {
        "this_member_eq_no_matches",
        MemberTarget_this_member_eq_no_matches
    },
    {
        "this_member_eq_all_matches",
        MemberTarget_this_member_eq_all_matches
    },
    {
        "this_member_wildcard",
        MemberTarget_this_member_wildcard
    },
    {
        "this_member_var",
        MemberTarget_this_member_var
    },
    {
        "this_member_var_written",
        MemberTarget_this_member_var_written
    },
    {
        "this_member_var_read",
        MemberTarget_this_member_var_read
    },
    {
        "this_member_eq_1_2nd_member",
        MemberTarget_this_member_eq_1_2nd_member
    },
    {
        "this_member_eq_2_2nd_member",
        MemberTarget_this_member_eq_2_2nd_member
    },
    {
        "this_member_var_same_1",
        MemberTarget_this_member_var_same_1
    },
    {
        "this_member_var_same_2",
        MemberTarget_this_member_var_same_2
    },
    {
        "this_written_member_eq_1",
        MemberTarget_this_written_member_eq_1
    },
    {
        "this_written_member_eq_2",
        MemberTarget_this_written_member_eq_2
    },
    {
        "this_written_member_wildcard",
        MemberTarget_this_written_member_wildcard
    },
    {
        "this_written_member_var",
        MemberTarget_this_written_member_var
    },
    {
        "this_member_neq_1",
        MemberTarget_this_member_neq_1
    },
    {
        "this_member_neq_2",
        MemberTarget_this_member_neq_2
    },
    {
        "this_member_neq_no_matches",
        MemberTarget_this_member_neq_no_matches
    },
    {
        "this_member_neq_all_matches",
        MemberTarget_this_member_neq_all_matches
    },
    {
        "this_member_neq_wildcard",
        MemberTarget_this_member_neq_wildcard
    },
    {
        "this_written_member_neq_1",
        MemberTarget_this_written_member_neq_1
    },
    {
        "this_written_member_neq_2",
        MemberTarget_this_written_member_neq_2
    },
    {
        "this_written_member_neq_no_matches",
        MemberTarget_this_written_member_neq_no_matches
    },
    {
        "this_written_member_neq_all_matches",
        MemberTarget_this_written_member_neq_all_matches
    },
    {
        "this_written_member_neq_wildcard",
        MemberTarget_this_written_member_neq_wildcard
    },
    {
        "this_member_eq_optional",
        MemberTarget_this_member_eq_optional
    },
    {
        "this_member_eq_optional_wildcard",
        MemberTarget_this_member_eq_optional_wildcard
    },
    {
        "this_written_member_eq_optional",
        MemberTarget_this_written_member_eq_optional
    },
    {
        "this_written_member_eq_optional_wildcard",
        MemberTarget_this_written_member_eq_optional_wildcard
    },
    {
        "this_member_eq_w_other_tag",
        MemberTarget_this_member_eq_w_other_tag
    },
    {
        "this_member_eq_w_other_component",
        MemberTarget_this_member_eq_w_other_component
    },
    {
        "this_written_member_eq_w_other_tag",
        MemberTarget_this_written_member_eq_w_other_tag
    },
    {
        "this_written_member_eq_w_other_component",
        MemberTarget_this_written_member_eq_w_other_component
    },
    {
        "this_written_member_eq_w_other_inherit_component",
        MemberTarget_this_written_member_eq_w_other_inherit_component
    },
    {
        "this_2_or",
        MemberTarget_this_2_or
    },
    {
        "this_3_or",
        MemberTarget_this_3_or
    },
    {
        "this_written_2_or",
        MemberTarget_this_written_2_or
    },
    {
        "this_written_3_or",
        MemberTarget_this_written_3_or
    },
    {
        "var_2_or",
        MemberTarget_var_2_or
    },
    {
        "var_3_or",
        MemberTarget_var_3_or
    },
    {
        "this_2_or_w_2_members",
        MemberTarget_this_2_or_w_2_members
    },
    {
        "this_2_or_w_2_types",
        MemberTarget_this_2_or_w_2_types
    },
    {
        "this_written_2_or_w_2_members",
        MemberTarget_this_written_2_or_w_2_members
    },
    {
        "this_written_2_or_w_2_types",
        MemberTarget_this_written_2_or_w_2_types
    },
    {
        "this_2_or_2_types_wildcard",
        MemberTarget_this_2_or_2_types_wildcard
    },
    {
        "this_2_or_2_types_dep_var",
        MemberTarget_this_2_or_2_types_dep_var
    },
    {
        "var_written_2_or",
        MemberTarget_var_written_2_or
    },
    {
        "var_written_3_or",
        MemberTarget_var_written_3_or
    },
    {
        "var_member_eq",
        MemberTarget_var_member_eq
    },
    {
        "var_member_eq_no_matches",
        MemberTarget_var_member_eq_no_matches
    },
    {
        "var_member_eq_all_matches",
        MemberTarget_var_member_eq_all_matches
    },
    {
        "var_member_wildcard",
        MemberTarget_var_member_wildcard
    },
    {
        "var_member_neq",
        MemberTarget_var_member_neq
    },
    {
        "var_member_neq_no_matches",
        MemberTarget_var_member_neq_no_matches
    },
    {
        "var_member_neq_all_matches",
        MemberTarget_var_member_neq_all_matches
    },
    {
        "var_written_member_eq",
        MemberTarget_var_written_member_eq
    },
    {
        "var_written_member_eq_no_matches",
        MemberTarget_var_written_member_eq_no_matches
    },
    {
        "var_written_member_eq_all_matches",
        MemberTarget_var_written_member_eq_all_matches
    },
    {
        "var_written_member_wildcard",
        MemberTarget_var_written_member_wildcard
    },
    {
        "var_written_member_neq",
        MemberTarget_var_written_member_neq
    },
    {
        "var_written_member_neq_no_matches",
        MemberTarget_var_written_member_neq_no_matches
    },
    {
        "var_written_member_neq_all_matches",
        MemberTarget_var_written_member_neq_all_matches
    }
};

bake_test_case Toggle_testcases[] = {
    {
        "fixed_src_1_tag_toggle",
        Toggle_fixed_src_1_tag_toggle
    },
    {
        "fixed_src_1_component_toggle",
        Toggle_fixed_src_1_component_toggle
    },
    {
        "fixed_src_2_tag_toggle",
        Toggle_fixed_src_2_tag_toggle
    },
    {
        "fixed_src_2_component_toggle",
        Toggle_fixed_src_2_component_toggle
    },
    {
        "fixed_2_src_w_toggle",
        Toggle_fixed_2_src_w_toggle
    },
    {
        "this_w_fixed_src_w_toggle",
        Toggle_this_w_fixed_src_w_toggle
    },
    {
        "fixed_src_w_this_w_toggle",
        Toggle_fixed_src_w_this_w_toggle
    },
    {
        "this_from_nothing",
        Toggle_this_from_nothing
    },
    {
        "this",
        Toggle_this
    },
    {
        "this_skip_initial",
        Toggle_this_skip_initial
    },
    {
        "this_pair",
        Toggle_this_pair
    },
    {
        "this_pair_skip_initial",
        Toggle_this_pair_skip_initial
    },
    {
        "this_tag",
        Toggle_this_tag
    },
    {
        "this_tag_pair",
        Toggle_this_tag_pair
    },
    {
        "this_tag_pair_wildcard",
        Toggle_this_tag_pair_wildcard
    },
    {
        "this_toggle_shared_self_up",
        Toggle_this_toggle_shared_self_up
    },
    {
        "this_toggle_shared_up",
        Toggle_this_toggle_shared_up
    },
    {
        "this_toggle_shared_self_up_w_self",
        Toggle_this_toggle_shared_self_up_w_self
    },
    {
        "this_toggle_shared_up_w_self",
        Toggle_this_toggle_shared_up_w_self
    },
    {
        "this_toggle_shared_self_up_w_self_reverse",
        Toggle_this_toggle_shared_self_up_w_self_reverse
    },
    {
        "this_toggle_shared_up_w_self_reverse",
        Toggle_this_toggle_shared_up_w_self_reverse
    },
    {
        "this_toggle_shared_self_up_w_self_toggle",
        Toggle_this_toggle_shared_self_up_w_self_toggle
    },
    {
        "this_toggle_shared_up_w_self_toggle",
        Toggle_this_toggle_shared_up_w_self_toggle
    },
    {
        "this_toggle_shared_self_up_w_self_toggle_reverse",
        Toggle_this_toggle_shared_self_up_w_self_toggle_reverse
    },
    {
        "this_toggle_shared_up_w_self_toggle_reverse",
        Toggle_this_toggle_shared_up_w_self_toggle_reverse
    },
    {
        "this_toggle_not_shared_self_up",
        Toggle_this_toggle_not_shared_self_up
    },
    {
        "this_toggle_not_shared_up",
        Toggle_this_toggle_not_shared_up
    },
    {
        "this_toggle_optional_shared_self_up",
        Toggle_this_toggle_optional_shared_self_up
    },
    {
        "this_toggle_optional_shared_up",
        Toggle_this_toggle_optional_shared_up
    },
    {
        "this_64_mod_1",
        Toggle_this_64_mod_1
    },
    {
        "this_64_mod_2",
        Toggle_this_64_mod_2
    },
    {
        "this_64_mod_3",
        Toggle_this_64_mod_3
    },
    {
        "this_64_mod_7",
        Toggle_this_64_mod_7
    },
    {
        "this_64_mod_8",
        Toggle_this_64_mod_8
    },
    {
        "this_64_mod_10",
        Toggle_this_64_mod_10
    },
    {
        "this_64_mod_64",
        Toggle_this_64_mod_64
    },
    {
        "this_64_mod_256",
        Toggle_this_64_mod_256
    },
    {
        "this_64_mod_1024",
        Toggle_this_64_mod_1024
    },
    {
        "this_100_mod_1",
        Toggle_this_100_mod_1
    },
    {
        "this_100_mod_2",
        Toggle_this_100_mod_2
    },
    {
        "this_100_mod_3",
        Toggle_this_100_mod_3
    },
    {
        "this_100_mod_7",
        Toggle_this_100_mod_7
    },
    {
        "this_100_mod_8",
        Toggle_this_100_mod_8
    },
    {
        "this_100_mod_10",
        Toggle_this_100_mod_10
    },
    {
        "this_100_mod_64",
        Toggle_this_100_mod_64
    },
    {
        "this_100_mod_256",
        Toggle_this_100_mod_256
    },
    {
        "this_100_mod_1024",
        Toggle_this_100_mod_1024
    },
    {
        "this_128_mod_1",
        Toggle_this_128_mod_1
    },
    {
        "this_128_mod_2",
        Toggle_this_128_mod_2
    },
    {
        "this_128_mod_3",
        Toggle_this_128_mod_3
    },
    {
        "this_128_mod_7",
        Toggle_this_128_mod_7
    },
    {
        "this_128_mod_8",
        Toggle_this_128_mod_8
    },
    {
        "this_128_mod_10",
        Toggle_this_128_mod_10
    },
    {
        "this_128_mod_64",
        Toggle_this_128_mod_64
    },
    {
        "this_128_mod_256",
        Toggle_this_128_mod_256
    },
    {
        "this_128_mod_1024",
        Toggle_this_128_mod_1024
    },
    {
        "this_200_mod_1",
        Toggle_this_200_mod_1
    },
    {
        "this_200_mod_2",
        Toggle_this_200_mod_2
    },
    {
        "this_200_mod_3",
        Toggle_this_200_mod_3
    },
    {
        "this_200_mod_7",
        Toggle_this_200_mod_7
    },
    {
        "this_200_mod_8",
        Toggle_this_200_mod_8
    },
    {
        "this_200_mod_10",
        Toggle_this_200_mod_10
    },
    {
        "this_200_mod_64",
        Toggle_this_200_mod_64
    },
    {
        "this_200_mod_256",
        Toggle_this_200_mod_256
    },
    {
        "this_200_mod_1024",
        Toggle_this_200_mod_1024
    },
    {
        "this_1024_mod_1",
        Toggle_this_1024_mod_1
    },
    {
        "this_1024_mod_2",
        Toggle_this_1024_mod_2
    },
    {
        "this_1024_mod_3",
        Toggle_this_1024_mod_3
    },
    {
        "this_1024_mod_7",
        Toggle_this_1024_mod_7
    },
    {
        "this_1024_mod_8",
        Toggle_this_1024_mod_8
    },
    {
        "this_1024_mod_10",
        Toggle_this_1024_mod_10
    },
    {
        "this_1024_mod_64",
        Toggle_this_1024_mod_64
    },
    {
        "this_1024_mod_256",
        Toggle_this_1024_mod_256
    },
    {
        "this_1024_mod_1024",
        Toggle_this_1024_mod_1024
    },
    {
        "this_enabled_64_mod_1",
        Toggle_this_enabled_64_mod_1
    },
    {
        "this_enabled_64_mod_2",
        Toggle_this_enabled_64_mod_2
    },
    {
        "this_enabled_64_mod_3",
        Toggle_this_enabled_64_mod_3
    },
    {
        "this_enabled_64_mod_7",
        Toggle_this_enabled_64_mod_7
    },
    {
        "this_enabled_64_mod_8",
        Toggle_this_enabled_64_mod_8
    },
    {
        "this_enabled_64_mod_10",
        Toggle_this_enabled_64_mod_10
    },
    {
        "this_enabled_64_mod_64",
        Toggle_this_enabled_64_mod_64
    },
    {
        "this_enabled_64_mod_256",
        Toggle_this_enabled_64_mod_256
    },
    {
        "this_enabled_64_mod_1024",
        Toggle_this_enabled_64_mod_1024
    },
    {
        "this_enabled_100_mod_1",
        Toggle_this_enabled_100_mod_1
    },
    {
        "this_enabled_100_mod_2",
        Toggle_this_enabled_100_mod_2
    },
    {
        "this_enabled_100_mod_3",
        Toggle_this_enabled_100_mod_3
    },
    {
        "this_enabled_100_mod_7",
        Toggle_this_enabled_100_mod_7
    },
    {
        "this_enabled_100_mod_8",
        Toggle_this_enabled_100_mod_8
    },
    {
        "this_enabled_100_mod_10",
        Toggle_this_enabled_100_mod_10
    },
    {
        "this_enabled_100_mod_64",
        Toggle_this_enabled_100_mod_64
    },
    {
        "this_enabled_100_mod_256",
        Toggle_this_enabled_100_mod_256
    },
    {
        "this_enabled_100_mod_1024",
        Toggle_this_enabled_100_mod_1024
    },
    {
        "this_enabled_128_mod_1",
        Toggle_this_enabled_128_mod_1
    },
    {
        "this_enabled_128_mod_2",
        Toggle_this_enabled_128_mod_2
    },
    {
        "this_enabled_128_mod_3",
        Toggle_this_enabled_128_mod_3
    },
    {
        "this_enabled_128_mod_7",
        Toggle_this_enabled_128_mod_7
    },
    {
        "this_enabled_128_mod_8",
        Toggle_this_enabled_128_mod_8
    },
    {
        "this_enabled_128_mod_10",
        Toggle_this_enabled_128_mod_10
    },
    {
        "this_enabled_128_mod_64",
        Toggle_this_enabled_128_mod_64
    },
    {
        "this_enabled_128_mod_256",
        Toggle_this_enabled_128_mod_256
    },
    {
        "this_enabled_128_mod_1024",
        Toggle_this_enabled_128_mod_1024
    },
    {
        "this_enabled_200_mod_1",
        Toggle_this_enabled_200_mod_1
    },
    {
        "this_enabled_200_mod_2",
        Toggle_this_enabled_200_mod_2
    },
    {
        "this_enabled_200_mod_3",
        Toggle_this_enabled_200_mod_3
    },
    {
        "this_enabled_200_mod_7",
        Toggle_this_enabled_200_mod_7
    },
    {
        "this_enabled_200_mod_8",
        Toggle_this_enabled_200_mod_8
    },
    {
        "this_enabled_200_mod_10",
        Toggle_this_enabled_200_mod_10
    },
    {
        "this_enabled_200_mod_64",
        Toggle_this_enabled_200_mod_64
    },
    {
        "this_enabled_200_mod_256",
        Toggle_this_enabled_200_mod_256
    },
    {
        "this_enabled_200_mod_1024",
        Toggle_this_enabled_200_mod_1024
    },
    {
        "this_enabled_1024_mod_1",
        Toggle_this_enabled_1024_mod_1
    },
    {
        "this_enabled_1024_mod_2",
        Toggle_this_enabled_1024_mod_2
    },
    {
        "this_enabled_1024_mod_3",
        Toggle_this_enabled_1024_mod_3
    },
    {
        "this_enabled_1024_mod_7",
        Toggle_this_enabled_1024_mod_7
    },
    {
        "this_enabled_1024_mod_8",
        Toggle_this_enabled_1024_mod_8
    },
    {
        "this_enabled_1024_mod_10",
        Toggle_this_enabled_1024_mod_10
    },
    {
        "this_enabled_1024_mod_64",
        Toggle_this_enabled_1024_mod_64
    },
    {
        "this_enabled_1024_mod_256",
        Toggle_this_enabled_1024_mod_256
    },
    {
        "this_enabled_1024_mod_1024",
        Toggle_this_enabled_1024_mod_1024
    },
    {
        "this_mod_2_2_bitsets",
        Toggle_this_mod_2_2_bitsets
    },
    {
        "this_mod_8_2_bitsets",
        Toggle_this_mod_8_2_bitsets
    },
    {
        "this_mod_64_2_bitsets",
        Toggle_this_mod_64_2_bitsets
    },
    {
        "this_mod_256_2_bitsets",
        Toggle_this_mod_256_2_bitsets
    },
    {
        "this_mod_1024_2_bitsets",
        Toggle_this_mod_1024_2_bitsets
    },
    {
        "this_randomized_2_bitsets",
        Toggle_this_randomized_2_bitsets
    },
    {
        "this_randomized_3_bitsets",
        Toggle_this_randomized_3_bitsets
    },
    {
        "this_randomized_4_bitsets",
        Toggle_this_randomized_4_bitsets
    },
    {
        "this_w_other_tag",
        Toggle_this_w_other_tag
    },
    {
        "this_w_other_component",
        Toggle_this_w_other_component
    },
    {
        "this_not",
        Toggle_this_not
    },
    {
        "this_written_not_1024_mod_2",
        Toggle_this_written_not_1024_mod_2
    },
    {
        "this_written_not_1024_mod_3",
        Toggle_this_written_not_1024_mod_3
    },
    {
        "this_written_not_1024_mod_7",
        Toggle_this_written_not_1024_mod_7
    },
    {
        "this_written_not_1024_mod_8",
        Toggle_this_written_not_1024_mod_8
    },
    {
        "this_written_not_1024_mod_10",
        Toggle_this_written_not_1024_mod_10
    },
    {
        "this_written_not_1024_mod_64",
        Toggle_this_written_not_1024_mod_64
    },
    {
        "this_written_not_1024_mod_256",
        Toggle_this_written_not_1024_mod_256
    },
    {
        "this_written_not_1024_mod_1024",
        Toggle_this_written_not_1024_mod_1024
    },
    {
        "this_optional",
        Toggle_this_optional
    },
    {
        "this_written_optional_1024_mod_2",
        Toggle_this_written_optional_1024_mod_2
    },
    {
        "this_written_optional_1024_mod_3",
        Toggle_this_written_optional_1024_mod_3
    },
    {
        "this_written_optional_1024_mod_7",
        Toggle_this_written_optional_1024_mod_7
    },
    {
        "this_written_optional_1024_mod_8",
        Toggle_this_written_optional_1024_mod_8
    },
    {
        "this_written_optional_1024_mod_10",
        Toggle_this_written_optional_1024_mod_10
    },
    {
        "this_written_optional_1024_mod_64",
        Toggle_this_written_optional_1024_mod_64
    },
    {
        "this_written_optional_1024_mod_256",
        Toggle_this_written_optional_1024_mod_256
    },
    {
        "this_written_optional_1024_mod_1024",
        Toggle_this_written_optional_1024_mod_1024
    },
    {
        "this_written_toggle_w_not_toggle",
        Toggle_this_written_toggle_w_not_toggle
    },
    {
        "this_written_not_toggle_w_toggle",
        Toggle_this_written_not_toggle_w_toggle
    },
    {
        "this_written_toggle_w_optional_toggle",
        Toggle_this_written_toggle_w_optional_toggle
    },
    {
        "this_written_optional_toggle_w_toggle",
        Toggle_this_written_optional_toggle_w_toggle
    },
    {
        "this_written_not_w_optional_toggle",
        Toggle_this_written_not_w_optional_toggle
    },
    {
        "this_written_optional_w_not_toggle",
        Toggle_this_written_optional_w_not_toggle
    },
    {
        "this_written_2_not_toggle",
        Toggle_this_written_2_not_toggle
    },
    {
        "this_written_2_optional_toggle",
        Toggle_this_written_2_optional_toggle
    },
    {
        "this_written_toggle_w_2_not_toggle",
        Toggle_this_written_toggle_w_2_not_toggle
    },
    {
        "this_written_toggle_w_2_optional_toggle",
        Toggle_this_written_toggle_w_2_optional_toggle
    },
    {
        "this_written_2_toggle_w_not_toggle",
        Toggle_this_written_2_toggle_w_not_toggle
    },
    {
        "this_written_2_toggle_w_optional_toggle",
        Toggle_this_written_2_toggle_w_optional_toggle
    },
    {
        "this_sort",
        Toggle_this_sort
    },
    {
        "this_table_move_2_from_3",
        Toggle_this_table_move_2_from_3
    },
    {
        "toggle_0_src_only_term",
        Toggle_toggle_0_src_only_term
    },
    {
        "toggle_0_src",
        Toggle_toggle_0_src
    }
};

bake_test_case Sparse_testcases[] = {
    {
        "1_fixed_sparse",
        Sparse_1_fixed_sparse
    },
    {
        "1_fixed_sparse_none",
        Sparse_1_fixed_sparse_none
    },
    {
        "1_this_sparse",
        Sparse_1_this_sparse
    },
    {
        "1_this_sparse_none",
        Sparse_1_this_sparse_none
    },
    {
        "1_this_sparse_written",
        Sparse_1_this_sparse_written
    },
    {
        "1_this_sparse_written_none",
        Sparse_1_this_sparse_written_none
    },
    {
        "1_var_sparse",
        Sparse_1_var_sparse
    },
    {
        "1_var_sparse_none",
        Sparse_1_var_sparse_none
    },
    {
        "1_var_sparse_written",
        Sparse_1_var_sparse_written
    },
    {
        "1_var_sparse_written_none",
        Sparse_1_var_sparse_written_none
    },
    {
        "2_sparse",
        Sparse_2_sparse
    },
    {
        "2_sparse_and_regular",
        Sparse_2_sparse_and_regular
    },
    {
        "2_regular_and_sparse",
        Sparse_2_regular_and_sparse
    },
    {
        "1_sparse_self",
        Sparse_1_sparse_self
    },
    {
        "1_sparse_up",
        Sparse_1_sparse_up
    },
    {
        "1_sparse_self_up",
        Sparse_1_sparse_self_up
    },
    {
        "1_sparse_written_self",
        Sparse_1_sparse_written_self
    },
    {
        "1_sparse_written_up",
        Sparse_1_sparse_written_up
    },
    {
        "1_sparse_written_self_up",
        Sparse_1_sparse_written_self_up
    },
    {
        "sparse_0_src_only_term",
        Sparse_sparse_0_src_only_term
    },
    {
        "sparse_0_src",
        Sparse_sparse_0_src
    }
};

bake_test_case Union_testcases[] = {
    {
        "1_fixed_union_any",
        Union_1_fixed_union_any
    },
    {
        "1_fixed_union_wildcard",
        Union_1_fixed_union_wildcard
    },
    {
        "1_fixed_union_tgt",
        Union_1_fixed_union_tgt
    },
    {
        "1_fixed_union_tgt_var",
        Union_1_fixed_union_tgt_var
    },
    {
        "1_fixed_union_tgt_var_written",
        Union_1_fixed_union_tgt_var_written
    },
    {
        "1_this_union_any",
        Union_1_this_union_any
    },
    {
        "1_this_union_wildcard",
        Union_1_this_union_wildcard
    },
    {
        "1_this_union_tgt",
        Union_1_this_union_tgt
    },
    {
        "1_this_union_tgt_var",
        Union_1_this_union_tgt_var
    },
    {
        "1_this_union_tgt_var_written",
        Union_1_this_union_tgt_var_written
    },
    {
        "1_var_union_any",
        Union_1_var_union_any
    },
    {
        "1_var_union_wildcard",
        Union_1_var_union_wildcard
    },
    {
        "1_var_union_tgt",
        Union_1_var_union_tgt
    },
    {
        "1_var_union_tgt_var",
        Union_1_var_union_tgt_var
    },
    {
        "1_var_union_tgt_var_written",
        Union_1_var_union_tgt_var_written
    },
    {
        "1_this_written_union_any",
        Union_1_this_written_union_any
    },
    {
        "1_this_written_union_wildcard",
        Union_1_this_written_union_wildcard
    },
    {
        "1_this_written_union_tgt",
        Union_1_this_written_union_tgt
    },
    {
        "1_this_written_union_tgt_var",
        Union_1_this_written_union_tgt_var
    },
    {
        "1_this_written_union_tgt_var_written",
        Union_1_this_written_union_tgt_var_written
    },
    {
        "1_var_written_union_any",
        Union_1_var_written_union_any
    },
    {
        "1_var_written_union_wildcard",
        Union_1_var_written_union_wildcard
    },
    {
        "1_var_written_union_tgt",
        Union_1_var_written_union_tgt
    },
    {
        "1_var_written_union_tgt_var",
        Union_1_var_written_union_tgt_var
    },
    {
        "1_var_written_union_tgt_var_written",
        Union_1_var_written_union_tgt_var_written
    },
    {
        "not_fixed_union_any",
        Union_not_fixed_union_any
    },
    {
        "not_fixed_union_wildcard",
        Union_not_fixed_union_wildcard
    },
    {
        "not_fixed_union_tgt",
        Union_not_fixed_union_tgt
    },
    {
        "not_fixed_union_var",
        Union_not_fixed_union_var
    },
    {
        "not_fixed_union_var_written",
        Union_not_fixed_union_var_written
    },
    {
        "not_this_written_union_any",
        Union_not_this_written_union_any
    },
    {
        "not_this_written_union_wildcard",
        Union_not_this_written_union_wildcard
    },
    {
        "not_this_written_union_tgt",
        Union_not_this_written_union_tgt
    },
    {
        "not_this_written_union_var",
        Union_not_this_written_union_var
    },
    {
        "not_this_written_union_var_written",
        Union_not_this_written_union_var_written
    },
    {
        "query_switch",
        Union_query_switch
    },
    {
        "query_1_case_1_type",
        Union_query_1_case_1_type
    },
    {
        "query_1_case_2_types",
        Union_query_1_case_2_types
    },
    {
        "query_2_cases_1_type",
        Union_query_2_cases_1_type
    },
    {
        "query_2_cases_2_types",
        Union_query_2_cases_2_types
    },
    {
        "query_after_remove",
        Union_query_after_remove
    },
    {
        "sort",
        Union_sort
    },
    {
        "query_recycled_tags",
        Union_query_recycled_tags
    },
    {
        "query_single_case",
        Union_query_single_case
    },
    {
        "match_switch_on_base_instance",
        Union_match_switch_on_base_instance
    },
    {
        "switch_w_bitset_query",
        Union_switch_w_bitset_query
    },
    {
        "switch_w_bitset_query_inv",
        Union_switch_w_bitset_query_inv
    },
    {
        "switch_w_bitset_query_2_elems",
        Union_switch_w_bitset_query_2_elems
    },
    {
        "switch_w_bitset_query_2_elems_skip",
        Union_switch_w_bitset_query_2_elems_skip
    },
    {
        "switch_w_bitset_query_elems_interleaved",
        Union_switch_w_bitset_query_elems_interleaved
    },
    {
        "switch_w_bitset_query_elems_interleaved_2_types",
        Union_switch_w_bitset_query_elems_interleaved_2_types
    },
    {
        "component_relation",
        Union_component_relation
    },
    {
        "switch_term_filter",
        Union_switch_term_filter
    },
    {
        "2_terms_switch_term_filter",
        Union_2_terms_switch_term_filter
    },
    {
        "match_switch_w_switch",
        Union_match_switch_w_switch
    },
    {
        "match_switch_w_case",
        Union_match_switch_w_case
    },
    {
        "match_switch_w_case_2_terms",
        Union_match_switch_w_case_2_terms
    },
    {
        "up",
        Union_up
    },
    {
        "self_up",
        Union_self_up
    },
    {
        "up_written",
        Union_up_written
    },
    {
        "self_up_written",
        Union_self_up_written
    },
    {
        "existing_union_table",
        Union_existing_union_table
    },
    {
        "new_union_table",
        Union_new_union_table
    },
    {
        "existing_union_table_w_tgt",
        Union_existing_union_table_w_tgt
    },
    {
        "new_union_table_w_tgt",
        Union_new_union_table_w_tgt
    },
    {
        "tgt_w_generation",
        Union_tgt_w_generation
    },
    {
        "tgt_w_not_alive",
        Union_tgt_w_not_alive
    },
    {
        "for_switch_filter_term",
        Union_for_switch_filter_term
    },
    {
        "union_from_nothing",
        Union_union_from_nothing
    },
    {
        "union_tgt_from_nothing",
        Union_union_tgt_from_nothing
    },
    {
        "tgt_inherited",
        Union_tgt_inherited
    }
};

bake_test_case OrderBy_testcases[] = {
    {
        "sort_by_component",
        OrderBy_sort_by_component
    },
    {
        "sort_by_component_2_tables",
        OrderBy_sort_by_component_2_tables
    },
    {
        "sort_by_component_3_tables",
        OrderBy_sort_by_component_3_tables
    },
    {
        "sort_by_entity",
        OrderBy_sort_by_entity
    },
    {
        "sort_after_add",
        OrderBy_sort_after_add
    },
    {
        "sort_after_remove",
        OrderBy_sort_after_remove
    },
    {
        "sort_after_delete",
        OrderBy_sort_after_delete
    },
    {
        "sort_after_set",
        OrderBy_sort_after_set
    },
    {
        "sort_after_system",
        OrderBy_sort_after_system
    },
    {
        "sort_after_query",
        OrderBy_sort_after_query
    },
    {
        "sort_by_component_same_value_1",
        OrderBy_sort_by_component_same_value_1
    },
    {
        "sort_by_component_same_value_2",
        OrderBy_sort_by_component_same_value_2
    },
    {
        "sort_by_component_move_pivot",
        OrderBy_sort_by_component_move_pivot
    },
    {
        "sort_1000_entities",
        OrderBy_sort_1000_entities
    },
    {
        "sort_1000_entities_w_duplicates",
        OrderBy_sort_1000_entities_w_duplicates
    },
    {
        "sort_1000_entities_again",
        OrderBy_sort_1000_entities_again
    },
    {
        "sort_1000_entities_2_types",
        OrderBy_sort_1000_entities_2_types
    },
    {
        "sort_1500_entities_3_types",
        OrderBy_sort_1500_entities_3_types
    },
    {
        "sort_2000_entities_4_types",
        OrderBy_sort_2000_entities_4_types
    },
    {
        "sort_2_entities_2_types",
        OrderBy_sort_2_entities_2_types
    },
    {
        "sort_3_entities_3_types",
        OrderBy_sort_3_entities_3_types
    },
    {
        "sort_3_entities_3_types_2",
        OrderBy_sort_3_entities_3_types_2
    },
    {
        "sort_4_entities_4_types",
        OrderBy_sort_4_entities_4_types
    },
    {
        "sort_1000_entities_2_types_again",
        OrderBy_sort_1000_entities_2_types_again
    },
    {
        "sort_1000_entities_add_type_after_sort",
        OrderBy_sort_1000_entities_add_type_after_sort
    },
    {
        "sort_shared_component",
        OrderBy_sort_shared_component
    },
    {
        "sort_shared_component_childof",
        OrderBy_sort_shared_component_childof
    },
    {
        "sort_w_tags_only",
        OrderBy_sort_w_tags_only
    },
    {
        "sort_childof_marked",
        OrderBy_sort_childof_marked
    },
    {
        "sort_isa_marked",
        OrderBy_sort_isa_marked
    },
    {
        "sort_relation_marked",
        OrderBy_sort_relation_marked
    },
    {
        "dont_resort_after_set_unsorted_component",
        OrderBy_dont_resort_after_set_unsorted_component
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag",
        OrderBy_dont_resort_after_set_unsorted_component_w_tag
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag_w_out_term",
        OrderBy_dont_resort_after_set_unsorted_component_w_tag_w_out_term
    },
    {
        "sort_component_not_queried_for",
        OrderBy_sort_component_not_queried_for
    },
    {
        "sort_by_wildcard",
        OrderBy_sort_by_wildcard
    },
    {
        "sort_shared_w_delete",
        OrderBy_sort_shared_w_delete
    },
    {
        "sort_w_nontrivial_component",
        OrderBy_sort_w_nontrivial_component
    },
    {
        "sort_by_wildcard",
        OrderBy_sort_by_wildcard
    },
    {
        "sort_not_term",
        OrderBy_sort_not_term
    },
    {
        "sort_or_term",
        OrderBy_sort_or_term
    },
    {
        "sort_optional_term",
        OrderBy_sort_optional_term
    }
};

bake_test_case OrderByEntireTable_testcases[] = {
    {
        "sort_by_component",
        OrderByEntireTable_sort_by_component
    },
    {
        "sort_by_component_2_tables",
        OrderByEntireTable_sort_by_component_2_tables
    },
    {
        "sort_by_component_3_tables",
        OrderByEntireTable_sort_by_component_3_tables
    },
    {
        "sort_by_entity",
        OrderByEntireTable_sort_by_entity
    },
    {
        "sort_after_add",
        OrderByEntireTable_sort_after_add
    },
    {
        "sort_after_remove",
        OrderByEntireTable_sort_after_remove
    },
    {
        "sort_after_delete",
        OrderByEntireTable_sort_after_delete
    },
    {
        "sort_after_set",
        OrderByEntireTable_sort_after_set
    },
    {
        "sort_after_system",
        OrderByEntireTable_sort_after_system
    },
    {
        "sort_after_query",
        OrderByEntireTable_sort_after_query
    },
    {
        "sort_by_component_same_value_1",
        OrderByEntireTable_sort_by_component_same_value_1
    },
    {
        "sort_by_component_same_value_2",
        OrderByEntireTable_sort_by_component_same_value_2
    },
    {
        "sort_by_component_move_pivot",
        OrderByEntireTable_sort_by_component_move_pivot
    },
    {
        "sort_1000_entities",
        OrderByEntireTable_sort_1000_entities
    },
    {
        "sort_1000_entities_w_duplicates",
        OrderByEntireTable_sort_1000_entities_w_duplicates
    },
    {
        "sort_1000_entities_again",
        OrderByEntireTable_sort_1000_entities_again
    },
    {
        "sort_1000_entities_2_types",
        OrderByEntireTable_sort_1000_entities_2_types
    },
    {
        "sort_1500_entities_3_types",
        OrderByEntireTable_sort_1500_entities_3_types
    },
    {
        "sort_2000_entities_4_types",
        OrderByEntireTable_sort_2000_entities_4_types
    },
    {
        "sort_2_entities_2_types",
        OrderByEntireTable_sort_2_entities_2_types
    },
    {
        "sort_3_entities_3_types",
        OrderByEntireTable_sort_3_entities_3_types
    },
    {
        "sort_3_entities_3_types_2",
        OrderByEntireTable_sort_3_entities_3_types_2
    },
    {
        "sort_4_entities_4_types",
        OrderByEntireTable_sort_4_entities_4_types
    },
    {
        "sort_1000_entities_2_types_again",
        OrderByEntireTable_sort_1000_entities_2_types_again
    },
    {
        "sort_1000_entities_add_type_after_sort",
        OrderByEntireTable_sort_1000_entities_add_type_after_sort
    },
    {
        "sort_shared_component",
        OrderByEntireTable_sort_shared_component
    },
    {
        "sort_w_tags_only",
        OrderByEntireTable_sort_w_tags_only
    },
    {
        "sort_childof_marked",
        OrderByEntireTable_sort_childof_marked
    },
    {
        "sort_isa_marked",
        OrderByEntireTable_sort_isa_marked
    },
    {
        "sort_relation_marked",
        OrderByEntireTable_sort_relation_marked
    },
    {
        "dont_resort_after_set_unsorted_component",
        OrderByEntireTable_dont_resort_after_set_unsorted_component
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag",
        OrderByEntireTable_dont_resort_after_set_unsorted_component_w_tag
    },
    {
        "dont_resort_after_set_unsorted_component_w_tag_w_out_term",
        OrderByEntireTable_dont_resort_after_set_unsorted_component_w_tag_w_out_term
    },
    {
        "sort_shared_w_delete",
        OrderByEntireTable_sort_shared_w_delete
    },
    {
        "sort_not_term",
        OrderByEntireTable_sort_not_term
    },
    {
        "sort_or_term",
        OrderByEntireTable_sort_or_term
    },
    {
        "sort_optional_term",
        OrderByEntireTable_sort_optional_term
    }
};

bake_test_case QueryStr_testcases[] = {
    {
        "one_term",
        QueryStr_one_term
    },
    {
        "one_term_w_inout",
        QueryStr_one_term_w_inout
    },
    {
        "two_terms",
        QueryStr_two_terms
    },
    {
        "two_terms_w_inout",
        QueryStr_two_terms_w_inout
    },
    {
        "three_terms_w_or",
        QueryStr_three_terms_w_or
    },
    {
        "three_terms_w_or_inout",
        QueryStr_three_terms_w_or_inout
    },
    {
        "four_terms_three_w_or_inout",
        QueryStr_four_terms_three_w_or_inout
    },
    {
        "one_term_w_pair",
        QueryStr_one_term_w_pair
    },
    {
        "one_term_w_pair_entity_src",
        QueryStr_one_term_w_pair_entity_src
    },
    {
        "one_term_w_self",
        QueryStr_one_term_w_self
    },
    {
        "one_term_w_up",
        QueryStr_one_term_w_up
    },
    {
        "one_term_w_self_up",
        QueryStr_one_term_w_self_up
    },
    {
        "one_term_w_cascade",
        QueryStr_one_term_w_cascade
    },
    {
        "one_term_w_0",
        QueryStr_one_term_w_0
    },
    {
        "one_term_w_singleton",
        QueryStr_one_term_w_singleton
    },
    {
        "one_term_w_final_pair",
        QueryStr_one_term_w_final_pair
    },
    {
        "one_term_w_final_dont_inherit",
        QueryStr_one_term_w_final_dont_inherit
    },
    {
        "one_term_w_final_inherit",
        QueryStr_one_term_w_final_inherit
    },
    {
        "one_term_w_final_override",
        QueryStr_one_term_w_final_override
    },
    {
        "one_term_w_src_var",
        QueryStr_one_term_w_src_var
    },
    {
        "one_term_w_first_var",
        QueryStr_one_term_w_first_var
    },
    {
        "one_term_w_second_var",
        QueryStr_one_term_w_second_var
    },
    {
        "one_term_w_first_var_entity_src",
        QueryStr_one_term_w_first_var_entity_src
    },
    {
        "one_term_w_pair_w_0_entity",
        QueryStr_one_term_w_pair_w_0_entity
    },
    {
        "not_term",
        QueryStr_not_term
    },
    {
        "wildcard_term",
        QueryStr_wildcard_term
    },
    {
        "scopes",
        QueryStr_scopes
    },
    {
        "pred_eq",
        QueryStr_pred_eq
    },
    {
        "pred_neq",
        QueryStr_pred_neq
    },
    {
        "pred_eq_name",
        QueryStr_pred_eq_name
    },
    {
        "pred_neq_name",
        QueryStr_pred_neq_name
    },
    {
        "pred_eq_m",
        QueryStr_pred_eq_m
    },
    {
        "pred_neq_m",
        QueryStr_pred_neq_m
    }
};

const char* Basic_cache_kind_param[] = {"default", "auto"};
bake_test_param Basic_params[] = {
    {"cache_kind", (char**)Basic_cache_kind_param, 2}
};
const char* Combinations_cache_kind_param[] = {"default", "auto"};
const char* Combinations_on_instantiate_param[] = {"override", "inherit", "dont_inherit"};
bake_test_param Combinations_params[] = {
    {"cache_kind", (char**)Combinations_cache_kind_param, 2},
    {"on_instantiate", (char**)Combinations_on_instantiate_param, 3}
};
const char* Variables_cache_kind_param[] = {"default", "auto"};
bake_test_param Variables_params[] = {
    {"cache_kind", (char**)Variables_cache_kind_param, 2}
};
const char* Operators_cache_kind_param[] = {"default", "auto"};
bake_test_param Operators_params[] = {
    {"cache_kind", (char**)Operators_cache_kind_param, 2}
};
const char* Recycled_cache_kind_param[] = {"default", "auto"};
bake_test_param Recycled_params[] = {
    {"cache_kind", (char**)Recycled_cache_kind_param, 2}
};
const char* BuiltinPredicates_cache_kind_param[] = {"default", "auto"};
bake_test_param BuiltinPredicates_params[] = {
    {"cache_kind", (char**)BuiltinPredicates_cache_kind_param, 2}
};
const char* Scopes_cache_kind_param[] = {"default", "auto"};
const char* Scopes_on_instantiate_param[] = {"override", "inherit"};
bake_test_param Scopes_params[] = {
    {"cache_kind", (char**)Scopes_cache_kind_param, 2},
    {"on_instantiate", (char**)Scopes_on_instantiate_param, 2}
};
const char* Traversal_cache_kind_param[] = {"default", "auto"};
bake_test_param Traversal_params[] = {
    {"cache_kind", (char**)Traversal_cache_kind_param, 2}
};
const char* MemberTarget_cache_kind_param[] = {"default", "auto"};
bake_test_param MemberTarget_params[] = {
    {"cache_kind", (char**)MemberTarget_cache_kind_param, 2}
};
const char* Toggle_cache_kind_param[] = {"default", "auto"};
bake_test_param Toggle_params[] = {
    {"cache_kind", (char**)Toggle_cache_kind_param, 2}
};
const char* Sparse_cache_kind_param[] = {"default", "auto"};
bake_test_param Sparse_params[] = {
    {"cache_kind", (char**)Sparse_cache_kind_param, 2}
};
const char* Union_cache_kind_param[] = {"default", "auto"};
bake_test_param Union_params[] = {
    {"cache_kind", (char**)Union_cache_kind_param, 2}
};

static bake_test_suite suites[] = {
    {
        "Validator",
        NULL,
        NULL,
        142,
        Validator_testcases
    },
    {
        "Parser",
        NULL,
        NULL,
        300,
        Parser_testcases
    },
    {
        "Basic",
        Basic_setup,
        NULL,
        207,
        Basic_testcases,
        1,
        Basic_params
    },
    {
        "Combinations",
        Combinations_setup,
        NULL,
        9,
        Combinations_testcases,
        2,
        Combinations_params
    },
    {
        "Plan",
        NULL,
        NULL,
        72,
        Plan_testcases
    },
    {
        "Variables",
        Variables_setup,
        NULL,
        189,
        Variables_testcases,
        1,
        Variables_params
    },
    {
        "Operators",
        Operators_setup,
        NULL,
        143,
        Operators_testcases,
        1,
        Operators_params
    },
    {
        "Transitive",
        NULL,
        NULL,
        72,
        Transitive_testcases
    },
    {
        "ComponentInheritance",
        NULL,
        NULL,
        31,
        ComponentInheritance_testcases
    },
    {
        "Recycled",
        Recycled_setup,
        NULL,
        6,
        Recycled_testcases,
        1,
        Recycled_params
    },
    {
        "BuiltinPredicates",
        BuiltinPredicates_setup,
        NULL,
        90,
        BuiltinPredicates_testcases,
        1,
        BuiltinPredicates_params
    },
    {
        "Scopes",
        Scopes_setup,
        NULL,
        11,
        Scopes_testcases,
        2,
        Scopes_params
    },
    {
        "Traversal",
        Traversal_setup,
        NULL,
        123,
        Traversal_testcases,
        1,
        Traversal_params
    },
    {
        "Cascade",
        NULL,
        NULL,
        25,
        Cascade_testcases
    },
    {
        "Cached",
        NULL,
        NULL,
        87,
        Cached_testcases
    },
    {
        "ChangeDetection",
        NULL,
        NULL,
        34,
        ChangeDetection_testcases
    },
    {
        "GroupBy",
        NULL,
        NULL,
        12,
        GroupBy_testcases
    },
    {
        "MemberTarget",
        MemberTarget_setup,
        NULL,
        63,
        MemberTarget_testcases,
        1,
        MemberTarget_params
    },
    {
        "Toggle",
        Toggle_setup,
        NULL,
        163,
        Toggle_testcases,
        1,
        Toggle_params
    },
    {
        "Sparse",
        Sparse_setup,
        NULL,
        21,
        Sparse_testcases,
        1,
        Sparse_params
    },
    {
        "Union",
        Union_setup,
        NULL,
        71,
        Union_testcases,
        1,
        Union_params
    },
    {
        "OrderBy",
        NULL,
        NULL,
        42,
        OrderBy_testcases
    },
    {
        "OrderByEntireTable",
        NULL,
        NULL,
        37,
        OrderByEntireTable_testcases
    },
    {
        "QueryStr",
        NULL,
        NULL,
        33,
        QueryStr_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("query", argc, argv, suites, 24);
}
