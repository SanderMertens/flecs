
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <addons.h>

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
void SystemMisc_match_system(void);
void SystemMisc_system_initial_state(void);
void SystemMisc_add_own_component(void);
void SystemMisc_change_system_action(void);
void SystemMisc_system_readeactivate(void);
void SystemMisc_system_readeactivate_w_2_systems(void);
void SystemMisc_add_to_system_in_progress(void);
void SystemMisc_redefine_null_signature(void);
void SystemMisc_redefine_0_signature(void);
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
void SystemMisc_pipeline_custom_run_action(void);
void SystemMisc_change_custom_run_action(void);
void SystemMisc_custom_run_action_call_next(void);
void SystemMisc_system_w_short_notation(void);
void SystemMisc_update_interval_w_system_init(void);
void SystemMisc_update_rate_w_system_init(void);
void SystemMisc_system_w_interval_rate_stop_timer(void);
void SystemMisc_system_same_interval_same_tick(void);
<<<<<<< HEAD

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
void RulesTraversal_self_up_mixed_traversable(void);
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
=======
void SystemMisc_system_no_id_in_scope(void);
>>>>>>> cf1a8ef63 (v4)

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
void Stats_progress_stats_systems(void);

// Testsuite 'Run'
void Run_setup(void);
void Run_run(void);
void Run_run_w_param(void);
void Run_run_no_match(void);
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
void MultiThreadStaging_set_pair_w_new_target_readonly(void);
void MultiThreadStaging_set_pair_w_new_target_tgt_component_readonly(void);
void MultiThreadStaging_set_pair_w_new_target_defer(void);
void MultiThreadStaging_set_pair_w_new_target_tgt_component_defer(void);

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
void Rest_script_error(void);

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
<<<<<<< HEAD
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
        "self_up_mixed_traversable",
        RulesTraversal_self_up_mixed_traversable
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
=======
>>>>>>> cf1a8ef63 (v4)
    },
    {
        "system_no_id_in_scope",
        SystemMisc_system_no_id_in_scope
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
    },
    {
        "progress_stats_systems",
        Stats_progress_stats_systems
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
    },
    {
        "script_error",
        Rest_script_error
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

const char* MultiThread_worker_kind_param[] = {"thread", "task"};
bake_test_param MultiThread_params[] = {
    {"worker_kind", (char**)MultiThread_worker_kind_param, 2}
};
const char* MultiThreadStaging_worker_kind_param[] = {"thread", "task"};
bake_test_param MultiThreadStaging_params[] = {
    {"worker_kind", (char**)MultiThreadStaging_worker_kind_param, 2}
};

static bake_test_suite suites[] = {
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
        64,
<<<<<<< HEAD
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
        93,
        RulesTraversal_testcases
=======
        SystemMisc_testcases
>>>>>>> cf1a8ef63 (v4)
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
        2,
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
        12,
        Stats_testcases
    },
    {
        "Run",
        Run_setup,
        NULL,
        5,
        Run_testcases
    },
    {
        "MultiThread",
        MultiThread_setup,
        NULL,
        50,
        MultiThread_testcases,
        1,
        MultiThread_params
    },
    {
        "MultiThreadStaging",
        MultiThreadStaging_setup,
        NULL,
        12,
        MultiThreadStaging_testcases,
        1,
        MultiThreadStaging_params
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
        14,
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
    return bake_test_run("addons", argc, argv, suites, 22);
}
