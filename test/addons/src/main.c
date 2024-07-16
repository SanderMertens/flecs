
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
void Pipeline_disable_component_from_immediate_system(void);

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
void SystemMisc_system_no_id_in_scope(void);
void SystemMisc_register_callback_after_run(void);
void SystemMisc_register_run_after_callback(void);
void SystemMisc_register_callback_after_run_ctx(void);
void SystemMisc_register_run_after_callback_ctx(void);

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
void System_w_FromParent_3_column_1_from_container_1_from_container_w_not(void);
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
void Rest_import_rest_after_mini(void);
void Rest_get_pipeline_stats_after_delete_system(void);

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
    },
    {
        "disable_component_from_immediate_system",
        Pipeline_disable_component_from_immediate_system
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
    },
    {
        "system_no_id_in_scope",
        SystemMisc_system_no_id_in_scope
    },
    {
        "register_callback_after_run",
        SystemMisc_register_callback_after_run
    },
    {
        "register_run_after_callback",
        SystemMisc_register_run_after_callback
    },
    {
        "register_callback_after_run_ctx",
        SystemMisc_register_callback_after_run_ctx
    },
    {
        "register_run_after_callback_ctx",
        SystemMisc_register_run_after_callback_ctx
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
        "3_column_1_from_container_1_from_container_w_not",
        System_w_FromParent_3_column_1_from_container_1_from_container_w_not
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
    },
    {
        "import_rest_after_mini",
        Rest_import_rest_after_mini
    },
    {
        "get_pipeline_stats_after_delete_system",
        Rest_get_pipeline_stats_after_delete_system
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
        83,
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
        16,
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
