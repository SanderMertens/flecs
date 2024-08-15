
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <core.h>

// Testsuite 'Id'
void Id_0_is_wildcard(void);
void Id_wildcard_is_wildcard(void);
void Id_any_is_wildcard(void);
void Id_entity_is_wildcard(void);
void Id_pair_is_wildcard(void);
void Id_pair_w_rel_wildcard_is_wildcard(void);
void Id_pair_w_obj_wildcard_is_wildcard(void);
void Id_pair_w_wildcard_wildcard_is_wildcard(void);
void Id_pair_w_rel_any_is_wildcard(void);
void Id_pair_w_obj_any_is_wildcard(void);
void Id_pair_w_any_any_is_wildcard(void);
void Id_pair_w_override_is_wildcard(void);
void Id_pair_w_toggle_is_wildcard(void);
void Id_tag_id_is_tag(void);
void Id_component_id_is_tag(void);
void Id_pair_id_is_tag(void);
void Id_pair_id_w_rel_component_is_tag(void);
void Id_pair_id_w_obj_component_is_tag(void);
void Id_pair_id_w_rel_component_obj_wildcard_is_tag(void);
void Id_pair_id_w_obj_wildcard_is_tag(void);
void Id_pair_id_w_tag_property_w_obj_component_is_tag(void);
void Id_pair_id_w_tag_property_w_obj_wildcard_is_tag(void);
void Id_pair_w_rel_wildcard_is_tag(void);
void Id_pair_w_obj_wildcard_is_tag(void);
void Id_pair_w_rel_tag_obj_wildcard_is_tag(void);
void Id_pair_w_wildcard_wildcard_is_tag(void);
void Id_pair_w_rel_any_is_tag(void);
void Id_pair_w_obj_any_is_tag(void);
void Id_pair_w_rel_tag_obj_any_is_tag(void);
void Id_pair_w_any_any_is_tag(void);
void Id_id_w_override_is_tag(void);
void Id_id_w_toggle_is_tag(void);
void Id_pair_id_override_is_tag(void);
void Id_pair_id_toggle_is_tag(void);
void Id_make_pair(void);
void Id_make_pair_of_pair(void);
void Id_make_pair_of_pair_tgt(void);

// Testsuite 'Entity'
void Entity_init_id(void);
void Entity_init_id_name(void);
void Entity_init_id_path(void);
void Entity_init_id_add_1_comp(void);
void Entity_init_id_add_2_comp(void);
void Entity_init_id_w_scope(void);
void Entity_init_id_name_w_scope(void);
void Entity_init_id_path_w_scope(void);
void Entity_init_id_fullpath_w_scope(void);
void Entity_init_id_fullpath_w_scope_existing(void);
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
void Entity_init_w_name_twice_deferred(void);
void Entity_init_w_nested_name_twice_deferred(void);
void Entity_init_w_scope_name_twice_deferred(void);
void Entity_init_w_childof_name_twice_deferred(void);
void Entity_init_w_childof_nested_name_twice_deferred(void);
void Entity_init_w_name_staged(void);
void Entity_record_find_for_empty(void);
void Entity_record_find(void);
void Entity_record_find_from_stage(void);
void Entity_make_alive_zero_gen(void);
void Entity_make_alive_nonzero_gen(void);
void Entity_make_alive_zero_gen_exists(void);
void Entity_make_alive_nonzero_gen_exists(void);
void Entity_make_alive_zero_gen_exists_alive(void);
void Entity_make_alive_nonzero_gen_exists_alive(void);
void Entity_set_scope_w_entity_init_from_stage(void);
void Entity_entity_init_w_scope_twice(void);
void Entity_entity_init_w_childof_twice(void);
void Entity_entity_init_w_childof_nested_name_twice(void);
void Entity_entity_init_w_childof_and_scope(void);
void Entity_entity_init_w_childof_and_scope_and_scoped_name(void);
void Entity_entity_init_w_childof_and_no_name(void);
void Entity_deferred_entity_init_w_childof_and_scope(void);
void Entity_deferred_entity_init_w_childof_and_scope_and_scoped_name(void);
void Entity_deferred_entity_init_w_childof_and_no_name(void);
void Entity_entity_init_w_empty_string_name(void);
void Entity_entity_init_w_empty_string_symbol(void);
void Entity_set_name_w_0_twice(void);
void Entity_new_entity_twice(void);
void Entity_new_entity_scoped_twice(void);
void Entity_defer_component_init(void);
void Entity_defer_component_init_w_symbol(void);
void Entity_defer_entity_init_w_symbol(void);
void Entity_staged_set_name_n_stages(void);
void Entity_staged_set_symbol_n_stages(void);
void Entity_entity_init_w_add_childof_no_name(void);
void Entity_entity_w_short_notation(void);
void Entity_override_inherited_symbol(void);
void Entity_use_low_id_for_component(void);
void Entity_get_depth(void);
void Entity_get_depth_non_acyclic(void);
void Entity_get_depth_empty(void);
void Entity_get_depth_2_paths(void);
void Entity_entity_init_w_empty_sep(void);
void Entity_entity_init_w_empty_sep_from_scope(void);
void Entity_entity_init_w_empty_sep_w_prefix(void);
void Entity_set_name_w_same_ptr(void);
void Entity_set_name_w_overlapping_ptr(void);
void Entity_defer_set_name_w_overlapping_ptr(void);
void Entity_make_alive_from_stage(void);
void Entity_make_alive_after_deleted_1_entity(void);
void Entity_make_alive_after_deleted_2_entities(void);
void Entity_defer_entity_init_w_set_name_w_add_childof(void);
void Entity_entity_w_digit_name(void);
void Entity_entity_w_existing_digit_name(void);
void Entity_entity_from_digit(void);
void Entity_entity_from_existing_digit(void);
void Entity_entity_from_digit_path(void);
void Entity_entity_from_existing_digit_path(void);
void Entity_entity_from_digit_0_path(void);
void Entity_entity_from_conflicting_digit(void);
void Entity_set_version_on_nonempty_entity(void);
void Entity_set_version_while_deferred(void);
void Entity_commit_w_on_add(void);
void Entity_commit_w_on_remove(void);
void Entity_commit_w_cmd_in_observer(void);
void Entity_entity_init_existing_no_sep(void);
void Entity_entity_init_w_set_1_comp(void);
void Entity_entity_init_w_set_2_comp(void);
void Entity_entity_init_w_set_1_comp_1_tag(void);
void Entity_entity_init_w_set_2_comp_2_tag(void);
void Entity_entity_init_w_set_1_comp_w_name(void);
void Entity_entity_init_w_set_1_comp_existing(void);
void Entity_entity_init_w_set_1_comp_existing_empty(void);
void Entity_entity_init_w_set_1_comp_1_tag_w_set(void);
void Entity_entity_init_w_set_w_hook(void);
void Entity_entity_init_w_set_w_observer(void);
void Entity_entity_init_w_set_1_comp_defer(void);
void Entity_entity_init_w_set_2_comp_defer(void);
void Entity_entity_init_w_set_1_comp_1_tag_defer(void);
void Entity_entity_init_w_set_2_comp_2_tag_defer(void);
void Entity_entity_init_w_set_1_comp_w_name_defer(void);
void Entity_entity_init_w_set_1_comp_existing_defer(void);
void Entity_entity_init_w_set_1_comp_existing_empty_defer(void);
void Entity_entity_init_w_set_1_comp_1_tag_w_set_defer(void);
void Entity_insert_1_comp(void);
void Entity_insert_2_comp(void);
void Entity_insert_1_comp_1_tag(void);
void Entity_entity_w_parent(void);
void Entity_entity_w_parent_w_name(void);
void Entity_entity_w_parent_w_add(void);
void Entity_entity_w_parent_w_add_w_parent(void);
void Entity_entity_w_parent_w_set(void);
void Entity_entity_w_parent_w_set_w_parent(void);

// Testsuite 'Each'
void Each_each_tag(void);
void Each_each_component(void);
void Each_each_pair(void);
void Each_each_pair_rel_wildcard(void);
void Each_each_pair_tgt_wildcard(void);

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
void Iter_worker_iter_w_singleton_component_instanced(void);
void Iter_paged_iter_w_singleton(void);
void Iter_paged_iter_w_singleton_instanced(void);
void Iter_paged_iter_w_singleton_component_instanced(void);
void Iter_page_iter_w_offset_skip_1_archetype(void);
void Iter_page_iter_w_offset_skip_1_archetype_plus_one(void);
void Iter_page_iter_w_offset_skip_2_archetypes(void);
void Iter_page_iter_w_limit_skip_1_archetype(void);
void Iter_page_iter_w_limit_skip_1_archetype_minus_one(void);
void Iter_page_iter_w_limit_skip_2_archetypes(void);
void Iter_page_iter_w_offset_1_limit_max(void);
void Iter_page_iter_w_offset_1_limit_minus_1(void);
void Iter_page_iter_w_offset_2_type_limit_max(void);
void Iter_page_iter_w_offset_2_type_limit_minus_1(void);
void Iter_page_iter_w_limit_1_all_offsets(void);
void Iter_page_iter_w_offset_out_of_bounds(void);
void Iter_page_iter_w_limit_out_of_bounds(void);
void Iter_page_iter_comb_10_entities_1_type(void);
void Iter_page_iter_comb_10_entities_2_types(void);
void Iter_count(void);
void Iter_iter_restore_stack_iter(void);
void Iter_interleaved_iter(void);
void Iter_get_first(void);
void Iter_page_iter_w_only_tag(void);
void Iter_worker_iter_w_only_tag(void);
void Iter_page_iter_w_inout_none(void);
void Iter_worker_iter_w_inout_none(void);
void Iter_page_iter_w_ctx(void);
void Iter_page_iter_w_binding_ctx(void);
void Iter_worker_iter_w_ctx(void);
void Iter_worker_iter_w_binding_ctx(void);
void Iter_column_index_owned(void);
void Iter_column_index_shared(void);
void Iter_column_index_not(void);
void Iter_page_iter_w_fini(void);
void Iter_worker_iter_w_fini(void);
void Iter_rule_page_iter_w_fini(void);
void Iter_rule_worker_iter_w_fini(void);
void Iter_to_str_before_next(void);
void Iter_to_str(void);

// Testsuite 'Search'
void Search_search(void);
void Search_search_wildcard(void);
void Search_search_wildcard_w_offset(void);
void Search_search_relation_wildcard_w_offset(void);
void Search_search_pair_w_any_rel(void);
void Search_search_pair_w_any_obj(void);
void Search_search_follow_relation_lvl_0(void);
void Search_search_follow_relation_lvl_1(void);
void Search_search_follow_relation_lvl_2(void);
void Search_search_follow_relation_lvl_3(void);
void Search_search_first_lvl_0(void);
void Search_search_first_lvl_1(void);
void Search_search_first_lvl_2(void);
void Search_search_first_lvl_3(void);
void Search_search_relation_wildcard(void);
void Search_search_relation_at_offset(void);
void Search_search_relation_inherit_from_parent(void);
void Search_search_relation_dont_inherit(void);
void Search_search_relation_dont_inherit_from_parent(void);
void Search_search_relation_exclusive(void);

// Testsuite 'Event'
void Event_table_1_id_w_trigger(void);
void Event_table_2_ids_w_trigger(void);
void Event_table_1_id_w_observer(void);
void Event_table_2_ids_w_observer(void);
void Event_emit_event_for_empty_table(void);
void Event_emit_table_event(void);
void Event_emit_staged_from_world(void);
void Event_emit_staged_from_stage(void);
void Event_emit_staged_from_world_observer(void);
void Event_emit_staged_from_stage_observer(void);
void Event_emit_for_entity(void);
void Event_emit_custom_for_any(void);
void Event_emit_custom_implicit_any(void);
void Event_emit_custom_empty_type(void);
void Event_emit_w_param(void);
void Event_emit_w_param_multi_observer(void);
void Event_emit_w_const_param(void);
void Event_emit_nested(void);
void Event_enqueue_event_1_id(void);
void Event_enqueue_event_2_ids(void);
void Event_enqueue_event_w_data(void);
void Event_enqueue_event_w_data_move(void);
void Event_enqueue_event_w_data_copy(void);
void Event_enqueue_event_w_const_data_no_copy(void);
void Event_enqueue_event_not_alive(void);
void Event_enqueue_event_not_alive_w_data_move(void);
void Event_enqueue_event_not_alive_w_data_copy(void);
void Event_enqueue_event_not_alive_after_delete_during_merge(void);
void Event_enqueue_event_not_alive_w_data_move_after_delete_during_merge(void);
void Event_enqueue_event_not_alive_w_data_copy_after_delete_during_merge(void);
void Event_enqueue_event_not_deferred(void);
void Event_enqueue_event_not_deferred_to_async(void);
void Event_enqueue_custom_implicit_any(void);
void Event_enqueue_custom_after_large_cmd(void);
void Event_enqueue_on_readonly_world(void);

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
void New_w_Count_add_after_bulk(void);
void New_w_Count_add_after_bulk_w_component(void);
void New_w_Count_add_after_bulk_w_ctor(void);
void New_w_Count_recycle_1_of_2(void);
void New_w_Count_recycle_1_of_3(void);
void New_w_Count_recycle_2_of_3(void);
void New_w_Count_bulk_init_w_table(void);

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
void Add_invalid_add_wildcard(void);
void Add_invalid_add_pair_w_wildcard(void);
void Add_invalid_add_pair_w_wildcard_rel(void);
void Add_invalid_add_pair_w_wildcard_obj(void);
void Add_invalid_add_any(void);
void Add_invalid_add_pair_w_any(void);
void Add_invalid_add_pair_w_any_rel(void);
void Add_invalid_add_pair_w_any_obj(void);
void Add_invalid_pair_w_0(void);
void Add_invalid_pair_w_0_rel(void);
void Add_invalid_pair_w_0_obj(void);
void Add_add_random_id(void);

// Testsuite 'Remove'
void Remove_zero(void);
void Remove_1_of_1(void);
void Remove_1_of_2(void);
void Remove_2_of_2(void);
void Remove_2_of_3(void);
void Remove_1_of_1_again(void);
void Remove_2_again(void);
void Remove_2_overlap(void);
void Remove_1_from_empty(void);
void Remove_not_added(void);

// Testsuite 'GlobalComponentIds'
void GlobalComponentIds_declare(void);
void GlobalComponentIds_declare_w_entity(void);
void GlobalComponentIds_declare_2_world(void);
void GlobalComponentIds_declare_tag(void);
void GlobalComponentIds_declare_tag_w_entity(void);
void GlobalComponentIds_declare_entity(void);
void GlobalComponentIds_reuse_300_component_ids(void);

// Testsuite 'Sparse'
void Sparse_has(void);
void Sparse_owns(void);
void Sparse_get(void);
void Sparse_get_mut(void);
void Sparse_ensure(void);
void Sparse_emplace(void);
void Sparse_set(void);
void Sparse_modified_no_on_set(void);
void Sparse_insert_1(void);
void Sparse_insert_2(void);
void Sparse_get_ref(void);
void Sparse_update_ref(void);
void Sparse_get_recycled(void);
void Sparse_get_mut_recycled(void);
void Sparse_ensure_recycled(void);
void Sparse_emplace_recycled(void);
void Sparse_set_recycled(void);
void Sparse_get_ref_recycled(void);
void Sparse_test_stable_ptr(void);
void Sparse_has_after_remove(void);
void Sparse_get_after_remove(void);
void Sparse_get_mut_after_remove(void);
void Sparse_sparse_w_hole(void);
void Sparse_record_get(void);
void Sparse_has_inherited(void);
void Sparse_owns_inherited(void);
void Sparse_get_inherited(void);
void Sparse_get_mut_inherited(void);
void Sparse_ensure_inherited(void);
void Sparse_emplace_inherited(void);
void Sparse_ctor_after_emplace(void);
void Sparse_ctor_dtor_after_remove(void);
void Sparse_ctor_dtor_after_clear(void);
void Sparse_ctor_dtor_after_delete(void);
void Sparse_ctor_dtor_after_fini(void);
void Sparse_on_add_remove_after_remove(void);
void Sparse_on_add_remove_after_clear(void);
void Sparse_on_add_remove_after_delete(void);
void Sparse_on_add_remove_after_fini(void);
void Sparse_on_set_after_set(void);
void Sparse_on_set_after_modified(void);
void Sparse_sparse_relationship(void);
void Sparse_defer_ensure(void);
void Sparse_defer_ensure_w_modified(void);
void Sparse_defer_ensure_modified(void);
void Sparse_defer_emplace(void);
void Sparse_defer_emplace_w_modified(void);
void Sparse_defer_set(void);
void Sparse_defer_ensure_existing(void);
void Sparse_defer_ensure_existing_twice(void);
void Sparse_defer_ensure_w_modified_existing(void);
void Sparse_defer_ensure_modified_existing(void);
void Sparse_defer_emplace_existing(void);
void Sparse_defer_emplace_w_modified_existing(void);
void Sparse_defer_set_existing(void);
void Sparse_defer_batched_ensure(void);
void Sparse_defer_batched_ensure_w_modified(void);
void Sparse_defer_batched_ensure_modified(void);
void Sparse_defer_batched_emplace(void);
void Sparse_defer_batched_emplace_w_modified(void);
void Sparse_defer_batched_set(void);
void Sparse_defer_batched_ensure_existing(void);
void Sparse_defer_batched_ensure_existing_twice(void);
void Sparse_defer_batched_ensure_w_modified_existing(void);
void Sparse_defer_batched_ensure_modified_existing(void);
void Sparse_defer_batched_emplace_existing(void);
void Sparse_defer_batched_emplace_w_modified_existing(void);
void Sparse_defer_batched_set_existing(void);
void Sparse_defer_batched_set_remove(void);
void Sparse_defer_batched_set_remove_existing(void);

// Testsuite 'Union'
void Union_add(void);
void Union_add_twice(void);
void Union_add_replace(void);
void Union_add_remove(void);
void Union_add_remove_recycled(void);
void Union_add_remove_add(void);
void Union_get_target_none(void);
void Union_get_target(void);
void Union_get_recycled_target(void);
void Union_get_target_after_replace(void);
void Union_get_target_after_remove(void);
void Union_has_wildcard(void);
void Union_has_any(void);
void Union_add_remove_2_tgts(void);
void Union_add_remove_2_tgts_join(void);
void Union_add_remove_3_tgts(void);
void Union_add_remove_3_tgts_join(void);
void Union_remove_w_union_tgt(void);
void Union_get_non_union_tgt_from_table_w_union(void);
void Union_has_non_union_from_table_w_union(void);
void Union_get_case_no_switch(void);
void Union_get_case_set(void);
void Union_get_case_change(void);
void Union_remove_case(void);
void Union_remove_last(void);
void Union_delete_first(void);
void Union_delete_last(void);
void Union_delete_first_last(void);
void Union_3_entities_same_case(void);
void Union_2_entities_1_change_case(void);
void Union_3_entities_change_case(void);
void Union_add_case_in_stage(void);
void Union_change_case_in_stage(void);
void Union_change_one_case_in_stage(void);
void Union_remove_switch_in_stage(void);
void Union_switch_no_match_for_case(void);
void Union_empty_entity_has_case(void);
void Union_zero_entity_has_case(void);
void Union_add_to_entity_w_switch(void);
void Union_add_pair_to_entity_w_switch(void);
void Union_recycled_tags(void);
void Union_same_table_after_change(void);
void Union_add_2(void);
void Union_add_2_reverse(void);
void Union_add_switch_to_prefab_instance(void);
void Union_get_case_w_generation(void);
void Union_get_case_w_generation_not_alive(void);
void Union_defer_add_union_relationship(void);
void Union_defer_add_existing_union_relationship(void);
void Union_defer_add_union_relationship_2_ops(void);
void Union_defer_add_existing_union_relationship_2_ops(void);

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
void Hierarchies_path_core(void);
void Hierarchies_path_core_w_prefix(void);
void Hierarchies_path_core_w_empty_prefix(void);
void Hierarchies_path_this_w_empty_prefix(void);
void Hierarchies_path_wildcard_w_empty_prefix(void);
void Hierarchies_path_any_w_empty_prefix(void);
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
void Hierarchies_path_w_entity_id(void);
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
void Hierarchies_lookup_entity_id(void);
void Hierarchies_delete_children(void);
void Hierarchies_scope_set(void);
void Hierarchies_scope_set_again(void);
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
void Hierarchies_add_root_path_to_child(void);
void Hierarchies_add_parent_path_from_root_to_child(void);
void Hierarchies_delete_child(void);
void Hierarchies_delete_2_children(void);
void Hierarchies_delete_2_children_different_type(void);
void Hierarchies_delete_tree_2_levels(void);
void Hierarchies_delete_tree_3_levels(void);
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
void Hierarchies_long_name_depth_0(void);
void Hierarchies_long_name_depth_1(void);
void Hierarchies_long_name_depth_2(void);
void Hierarchies_ensure_1_parent_after_adding_2(void);
void Hierarchies_ensure_child_alive_after_deleting_prev_parent(void);
void Hierarchies_lookup_after_root_to_parent_move(void);
void Hierarchies_lookup_after_parent_to_root_move(void);
void Hierarchies_lookup_after_parent_to_parent_move(void);
void Hierarchies_lookup_after_clear_from_root(void);
void Hierarchies_lookup_after_clear_from_parent(void);
void Hierarchies_lookup_after_delete_from_root(void);
void Hierarchies_lookup_after_delete_from_parent(void);
void Hierarchies_defer_batch_remove_name_w_add_childof(void);
void Hierarchies_defer_batch_remove_childof_w_add_name(void);

// Testsuite 'Has'
void Has_zero(void);
void Has_1_of_0(void);
void Has_1_of_1(void);
void Has_1_of_2(void);
void Has_1_of_empty(void);
void Has_has_in_progress(void);
void Has_has_of_zero(void);
void Has_owns(void);
void Has_owns_wildcard(void);
void Has_owns_wildcard_pair(void);
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
void Count_count_disabled(void);
void Count_count_prefab(void);

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
void Get_component_ensure_equal_get(void);
void Get_component_get_tag(void);
void Get_component_get_pair_tag(void);
void Get_component_get_wildcard(void);

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
void Reference_get_ref_w_low_id_tag(void);
void Reference_get_ref_w_low_id_tag_after_add(void);
void Reference_get_nonexisting(void);
void Reference_aba_table(void);

// Testsuite 'Delete'
void Delete_setup(void);
void Delete_delete_1(void);
void Delete_delete_1_again(void);
void Delete_delete_recycled_tag_again(void);
void Delete_delete_empty(void);
void Delete_delete_nonexist(void);
void Delete_delete_1st_of_3(void);
void Delete_delete_2nd_of_3(void);
void Delete_delete_3rd_of_3(void);
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
void OnDelete_flags(void);
void OnDelete_id_default(void);
void OnDelete_id_remove(void);
void OnDelete_id_delete(void);
void OnDelete_relation_default(void);
void OnDelete_relation_remove(void);
void OnDelete_relation_delete(void);
void OnDelete_object_default(void);
void OnDelete_object_remove(void);
void OnDelete_object_delete(void);
void OnDelete_id_throw(void);
void OnDelete_relation_throw(void);
void OnDelete_object_throw(void);
void OnDelete_object_mixed(void);
void OnDelete_id_remove_no_instances(void);
void OnDelete_id_delete_no_instances(void);
void OnDelete_id_throw_no_instances(void);
void OnDelete_cyclic_self(void);
void OnDelete_nonempty_cyclic_self(void);
void OnDelete_cyclic_id_default(void);
void OnDelete_cyclic_id_remove(void);
void OnDelete_cyclic_id_remove_both(void);
void OnDelete_cyclic_id_delete(void);
void OnDelete_cyclic_id_delete_both(void);
void OnDelete_cyclic_relation_default(void);
void OnDelete_cyclic_relation_remove(void);
void OnDelete_cyclic_relation_remove_both(void);
void OnDelete_cyclic_relation_delete(void);
void OnDelete_cyclic_relation_delete_both(void);
void OnDelete_cyclic_object_default(void);
void OnDelete_cyclic_object_remove(void);
void OnDelete_cyclic_object_delete(void);
void OnDelete_cyclic_overlapping_table(void);
void OnDelete_cyclic_overlapping_new_tables(void);
void OnDelete_cyclic_object_mixed(void);
void OnDelete_cyclic_storage_table(void);
void OnDelete_cyclic_storage_table_2(void);
void OnDelete_cyclic_storage_table_3(void);
void OnDelete_cyclic_set_empty(void);
void OnDelete_2_acyclic_relations_w_cycle(void);
void OnDelete_remove_2_comps(void);
void OnDelete_remove_2_comps_to_existing_table(void);
void OnDelete_delete_recursive(void);
void OnDelete_component_throw(void);
void OnDelete_remove_2_relations(void);
void OnDelete_remove_object_w_2_relations(void);
void OnDelete_remove_object_w_5_relations(void);
void OnDelete_remove_object_w_50_relations(void);
void OnDelete_remove_object_w_50_relations_3_tables(void);
void OnDelete_remove_object_w_3_relations_interleaved(void);
void OnDelete_remove_id_from_2_tables(void);
void OnDelete_remove_relation_from_2_tables(void);
void OnDelete_remove_object_from_2_tables(void);
void OnDelete_remove_id_and_relation(void);
void OnDelete_remove_id_and_relation_from_2_tables(void);
void OnDelete_stresstest_many_objects(void);
void OnDelete_stresstest_many_relations(void);
void OnDelete_stresstest_many_objects_on_delete(void);
void OnDelete_stresstest_many_relations_on_delete(void);
void OnDelete_empty_table_w_on_remove(void);
void OnDelete_delete_table_in_on_remove_during_fini(void);
void OnDelete_delete_other_in_on_remove_during_fini(void);
void OnDelete_remove_id_w_role(void);
void OnDelete_remove_rel_w_override_pair(void);
void OnDelete_remove_obj_w_override_pair(void);
void OnDelete_remove_rel_w_override_pair_after_on_delete_target(void);
void OnDelete_remove_rel_w_override_pair_2_ids(void);
void OnDelete_remove_obj_w_override_pair_2_ids(void);
void OnDelete_remove_obj_w_override_pair_3_ids(void);
void OnDelete_remove_mixed_w_override_pair_3_ids(void);
void OnDelete_merge_pair_component(void);
void OnDelete_delete_with_tag(void);
void OnDelete_delete_with_component(void);
void OnDelete_delete_with_pair(void);
void OnDelete_delete_with_object_wildcard(void);
void OnDelete_delete_with_relation_wildcard(void);
void OnDelete_delete_with_component_after_delete_cyclic_self(void);
void OnDelete_delete_with_component_after_delete_cyclic(void);
void OnDelete_delete_with_component_after_delete_cyclic_w_alive_moved(void);
void OnDelete_delete_all_with_entity(void);
void OnDelete_remove_childof_entity(void);
void OnDelete_remove_childof_wildcard(void);
void OnDelete_delete_child_of_delete_with(void);
void OnDelete_deep_clean_64(void);
void OnDelete_deep_clean_256(void);
void OnDelete_id_w_disabled(void);
void OnDelete_id_to_no_disabled(void);
void OnDelete_remove_on_delete_action(void);
void OnDelete_delete_with_w_relation(void);
void OnDelete_delete_self_in_on_remove(void);
void OnDelete_delete_nested_in_on_remove(void);
void OnDelete_add_deleted_in_on_remove(void);
void OnDelete_delete_tree_w_query(void);
void OnDelete_fini_cleanup_order(void);
void OnDelete_fini_cleanup_order_root_id_w_trait(void);
void OnDelete_fini_cleanup_order_entity_after_singleton(void);
void OnDelete_fini_cleanup_order_entity_after_component(void);
void OnDelete_on_delete_parent_w_in_use_id_w_remove(void);
void OnDelete_on_delete_parent_w_in_use_id_w_delete(void);
void OnDelete_create_after_delete_with(void);
void OnDelete_delete_with_inherited_tag(void);
void OnDelete_delete_with_inherited_tag_w_query(void);
void OnDelete_delete_with_inherited_tag_w_observer(void);
void OnDelete_delete_symmetric_relation(void);
void OnDelete_delete_observed_symmetric_relation(void);
void OnDelete_nested_delete_with(void);
void OnDelete_deferred_delete_with_after_create_named(void);
void OnDelete_deferred_delete_with_childof_after_create_named(void);
void OnDelete_match_marked_for_deletion(void);
void OnDelete_delete_w_low_rel_mixed_cleanup(void);
void OnDelete_delete_w_low_rel_mixed_cleanup_interleaved_ids(void);
void OnDelete_fini_query_w_singleton_in_scope_no_module(void);
void OnDelete_fini_query_w_singleton_in_module(void);
void OnDelete_fini_observer_w_relationship_in_scope(void);
void OnDelete_add_on_delete_from_prefab(void);
void OnDelete_add_on_delete_from_disabled(void);
void OnDelete_delete_on_delete_from_prefab(void);
void OnDelete_delete_on_delete_from_disabled(void);
void OnDelete_remove_all_1(void);
void OnDelete_remove_all_2(void);
void OnDelete_remove_all_3(void);
void OnDelete_delete_with_1(void);
void OnDelete_delete_with_2(void);
void OnDelete_delete_with_3(void);

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
void Set_ensure_new(void);
void Set_ensure_existing(void);
void Set_ensure_tag_new(void);
void Set_ensure_tag_existing(void);
void Set_ensure_tag_new_w_comp(void);
void Set_ensure_tag_existing_w_comp(void);
void Set_ensure_tag_new_w_pair(void);
void Set_ensure_tag_existing_w_pair(void);
void Set_get_mut_not_existing(void);
void Set_get_mut_existing(void);
void Set_get_mut_tag(void);
void Set_get_mut_pair(void);
void Set_get_mut_pair_second(void);
void Set_modified_w_on_set(void);
void Set_modified_no_component(void);
void Set_ensure_w_add_in_on_add(void);
void Set_ensure_w_remove_in_on_add(void);
void Set_ensure_w_realloc_in_on_add(void);
void Set_emplace(void);
void Set_emplace_2(void);
void Set_emplace_existing(void);
void Set_emplace_w_move(void);
void Set_emplace_w_observer_w_add(void);
void Set_emplace_existing_w_check(void);

// Testsuite 'ReadWrite'
void ReadWrite_read(void);
void ReadWrite_nested_read(void);
void ReadWrite_write(void);
void ReadWrite_nested_write(void);
void ReadWrite_add_while_read(void);
void ReadWrite_add_while_write(void);
void ReadWrite_read_from_stage(void);
void ReadWrite_write_from_stage(void);

// Testsuite 'Lookup'
void Lookup_setup(void);
void Lookup_lookup(void);
void Lookup_lookup_component(void);
void Lookup_lookup_not_found(void);
void Lookup_lookup_child(void);
void Lookup_lookup_w_null_name(void);
void Lookup_lookup_after_name_reset(void);
void Lookup_get_name(void);
void Lookup_get_name_no_name(void);
void Lookup_get_name_from_empty(void);
void Lookup_lookup_by_id(void);
void Lookup_lookup_path_anonymous_parent(void);
void Lookup_lookup_path_0_parent(void);
void Lookup_lookup_path_0_parent_w_scope(void);
void Lookup_lookup_recycled_by_id(void);
void Lookup_lookup_symbol_by_id(void);
void Lookup_lookup_name_w_digit(void);
void Lookup_lookup_symbol_w_digit(void);
void Lookup_lookup_path_w_digit(void);
void Lookup_lookup_name_w_spaces(void);
void Lookup_lookup_path_w_spaces(void);
void Lookup_lookup_number(void);
void Lookup_lookup_number_path(void);
void Lookup_lookup_number_0(void);
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
void Lookup_lookup_variable(void);
void Lookup_lookup_path_this(void);
void Lookup_lookup_path_wildcard(void);
void Lookup_lookup_path_this_from_scope(void);
void Lookup_lookup_path_wildcard_from_scope(void);
void Lookup_resolve_builtin_symbols(void);
void Lookup_lookup_from_scope_staged(void);
void Lookup_lookup_core(void);
void Lookup_lookup_core_from_stage(void);
void Lookup_lookup_custom_search_path(void);
void Lookup_lookup_custom_search_path_from_stage(void);
void Lookup_lookup_custom_search_path_n_elems(void);
void Lookup_set_same_name(void);
void Lookup_set_same_name_after_reparenting(void);
void Lookup_defer_set_name(void);
void Lookup_defer_set_same_name(void);
void Lookup_lookup_invalid_digit(void);
void Lookup_lookup_child_invalid_digit(void);
void Lookup_lookup_digit_from_wrong_scope(void);
void Lookup_lookup_core_entity_from_wrong_scope(void);
void Lookup_lookup_alias_w_number(void);
void Lookup_lookup_symbol_path(void);
void Lookup_lookup_name_escaped_sep(void);
void Lookup_lookup_path_escaped_sep(void);
void Lookup_lookup_name_63_chars(void);
void Lookup_lookup_name_64_chars(void);
void Lookup_lookup_name_65_chars(void);
void Lookup_lookup_path_63_chars(void);
void Lookup_lookup_path_64_chars(void);
void Lookup_lookup_path_65_chars(void);

// Testsuite 'Singleton'
void Singleton_add_singleton(void);
void Singleton_remove_singleton(void);
void Singleton_set_get_singleton(void);
void Singleton_ensure_singleton(void);
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
void Clone_clone_w_name(void);

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
void ComponentLifecycle_ctor_on_tag(void);
void ComponentLifecycle_dtor_on_tag(void);
void ComponentLifecycle_copy_on_tag(void);
void ComponentLifecycle_move_on_tag(void);
void ComponentLifecycle_merge_to_different_table(void);
void ComponentLifecycle_merge_to_new_table(void);
void ComponentLifecycle_delete_in_stage(void);
void ComponentLifecycle_ctor_on_add_pair(void);
void ComponentLifecycle_ctor_on_add_pair_tag(void);
void ComponentLifecycle_ctor_on_move_pair(void);
void ComponentLifecycle_move_on_realloc(void);
void ComponentLifecycle_move_on_bulk_new(void);
void ComponentLifecycle_on_add_on_bulk_new(void);
void ComponentLifecycle_move_on_delete(void);
void ComponentLifecycle_move_dtor_on_delete(void);
void ComponentLifecycle_copy_on_override_pair(void);
void ComponentLifecycle_copy_on_override_pair_tag(void);
void ComponentLifecycle_copy_on_set_pair(void);
void ComponentLifecycle_copy_on_set_pair_tag(void);
void ComponentLifecycle_allow_lifecycle_overwrite_equal_callbacks(void);
void ComponentLifecycle_set_lifecycle_after_trigger(void);
void ComponentLifecycle_valid_entity_in_dtor_after_delete(void);
void ComponentLifecycle_ctor_w_emplace(void);
void ComponentLifecycle_ctor_w_emplace_defer(void);
void ComponentLifecycle_ctor_w_emplace_defer_use_move_ctor(void);
void ComponentLifecycle_ctor_w_emplace_defer_twice(void);
void ComponentLifecycle_ctor_w_emplace_defer_existing(void);
void ComponentLifecycle_on_add_w_emplace(void);
void ComponentLifecycle_on_add_w_emplace_existing(void);
void ComponentLifecycle_on_add_w_emplace_defer(void);
void ComponentLifecycle_merge_async_stage_w_emplace(void);
void ComponentLifecycle_merge_async_stage_w_emplace_to_deferred_world(void);
void ComponentLifecycle_emplace_grow_w_existing_component(void);
void ComponentLifecycle_dtor_on_fini(void);
void ComponentLifecycle_valid_type_in_dtor_on_fini(void);
void ComponentLifecycle_valid_other_type_of_entity_in_dtor_on_fini(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_fini(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent(void);
void ComponentLifecycle_delete_in_dtor_other_type_on_delete(void);
void ComponentLifecycle_delete_self_in_dtor_on_delete(void);
void ComponentLifecycle_on_set_after_set(void);
void ComponentLifecycle_on_add_after_new(void);
void ComponentLifecycle_on_add_after_add(void);
void ComponentLifecycle_on_add_after_set(void);
void ComponentLifecycle_on_remove_after_remove(void);
void ComponentLifecycle_on_remove_after_clear(void);
void ComponentLifecycle_on_remove_after_delete(void);
void ComponentLifecycle_free_component_new_id_while_fini(void);
void ComponentLifecycle_dtor_component_new_id_while_fini(void);
void ComponentLifecycle_free_component_new_pair_id_while_fini(void);
void ComponentLifecycle_dtor_component_new_pair_id_while_fini(void);
void ComponentLifecycle_free_component_new_obj_pair_id_while_fini(void);
void ComponentLifecycle_dtor_component_new_obj_pair_id_while_fini(void);
void ComponentLifecycle_ctor_move_dtor_after_resize(void);
void ComponentLifecycle_ctx_free(void);
void ComponentLifecycle_binding_ctx_free(void);
void ComponentLifecycle_ctx_free_after_delete_component(void);
void ComponentLifecycle_binding_ctx_free_after_delete_component(void);
void ComponentLifecycle_on_add_ctx(void);
void ComponentLifecycle_on_remove_ctx(void);
void ComponentLifecycle_on_set_ctx(void);
void ComponentLifecycle_on_add_w_existing_component(void);
void ComponentLifecycle_on_remove_w_existing_component(void);
void ComponentLifecycle_component_init_set_hooks(void);
void ComponentLifecycle_component_init_name_from_type_info(void);
void ComponentLifecycle_component_init_scoped_name_from_type_info(void);
void ComponentLifecycle_component_init_w_recycled_id(void);
void ComponentLifecycle_component_init_w_recycled_non_component_id(void);
void ComponentLifecycle_on_add_after_ctor_w_add(void);
void ComponentLifecycle_on_add_after_ctor_w_add_to(void);
void ComponentLifecycle_with_before_hooks(void);
void ComponentLifecycle_with_component_on_add(void);
void ComponentLifecycle_move_ctor_on_move(void);
void ComponentLifecycle_ptr_to_self(void);
void ComponentLifecycle_ctor_move_dtor_from_move_ctor(void);
void ComponentLifecycle_on_add_hook_check_offset(void);
void ComponentLifecycle_on_remove_hook_check_offset(void);
void ComponentLifecycle_on_set_hook_check_offset(void);
void ComponentLifecycle_on_set_hook_on_override(void);
void ComponentLifecycle_on_set_hook_on_auto_override(void);
void ComponentLifecycle_batched_set_new_component_w_lifecycle(void);
void ComponentLifecycle_batched_ensure_new_component_w_lifecycle(void);
void ComponentLifecycle_on_nested_prefab_copy_test_invokes_copy_count(void);
void ComponentLifecycle_no_move_no_move_ctor_with_move_dtor_with_ctor_move_dtor(void);

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
void Pairs_typeid_from_pair_w_override(void);
void Pairs_typeid_from_pair_w_toggle(void);
void Pairs_tag_pair_w_rel_comp(void);
void Pairs_tag_pair_w_obj_comp(void);
void Pairs_tag_pair_w_rel_obj_comp(void);
void Pairs_get_tag_pair_w_rel_comp(void);
void Pairs_get_tag_pair_w_obj_comp(void);
void Pairs_get_tag_pair_w_rel_obj_comp(void);
void Pairs_tag_pair_w_childof_w_comp(void);
void Pairs_tag_pair_w_isa_w_comp(void);
void Pairs_get_1_target(void);
void Pairs_get_1_target_not_found(void);
void Pairs_get_n_targets(void);
void Pairs_get_target_from_base(void);
void Pairs_get_target_from_2nd_base(void);
void Pairs_get_target_from_base_w_pair_on_instance(void);
void Pairs_get_childof_target_from_base(void);
void Pairs_get_dontinherit_target_from_base(void);
void Pairs_get_target_for_id_from_self(void);
void Pairs_get_target_for_id_from_base(void);
void Pairs_get_target_for_id_from_nested_base(void);
void Pairs_get_target_for_id_not_found(void);
void Pairs_get_target_for_wildcard_from_self(void);
void Pairs_get_target_for_wildcard_from_base(void);
void Pairs_get_target_for_wildcard_from_nested_base(void);
void Pairs_get_target_for_id_from_empty(void);
void Pairs_get_target_for_id_from_empty_no_rel(void);
void Pairs_get_target_for_id_not_empty_not_found(void);
void Pairs_get_target_for_id_from_stage(void);
void Pairs_get_target_for_id_no_id(void);
void Pairs_get_parent(void);
void Pairs_get_parent_from_root(void);
void Pairs_ignore_childof_from_base(void);
void Pairs_add_exclusive_relation_twice(void);
void Pairs_add_same_exclusive_relation_twice(void);
void Pairs_set_exclusive_relation_twice(void);
void Pairs_add_exclusive_non_empty_table(void);
void Pairs_add_exclusive_non_empty_table_w_pairs(void);
void Pairs_add_pair_to_entity_w_exclusive_pair(void);
void Pairs_add_pair_to_entity_w_scope(void);
void Pairs_add_existing_exclusive_pair_after_pair(void);
void Pairs_add_remove_exclusive_property(void);
void Pairs_add_symmetric_relation(void);
void Pairs_remove_symmetric_relation(void);
void Pairs_delete_entity_w_symmetric_relation(void);
void Pairs_add_symmetric_exclusive_relation(void);
void Pairs_add_symmetric_recycled_relation(void);
void Pairs_symmetric_w_childof(void);
void Pairs_with(void);
void Pairs_2_with(void);
void Pairs_nested_with(void);
void Pairs_with_relation(void);
void Pairs_2_with_relation(void);
void Pairs_with_for_existing(void);
void Pairs_nested_with_relation(void);
void Pairs_65k_relations(void);
void Pairs_add_not_alive_relation(void);
void Pairs_remove_wildcard(void);
void Pairs_remove_relation_wildcard(void);
void Pairs_remove_wildcard_all(void);
void Pairs_inherit_exclusive(void);
void Pairs_dont_inherit(void);
void Pairs_has_wildcard_w_pair(void);
void Pairs_has_pair_wildcard_w_tag(void);
void Pairs_oneof_self(void);
void Pairs_oneof_other(void);
void Pairs_oneof_self_constraint_violated(void);
void Pairs_oneof_other_constraint_violated(void);
void Pairs_oneof_other_rel_parent_constraint_violated(void);
void Pairs_set_w_recycled_rel(void);
void Pairs_set_w_recycled_tgt(void);
void Pairs_force_relationship_on_component(void);
void Pairs_force_relationship_on_target(void);
void Pairs_force_relationship_on_target_trait(void);
void Pairs_force_relationship_on_relationship(void);
void Pairs_force_target_on_component(void);
void Pairs_force_target_on_relationship(void);
void Pairs_force_target_on_target(void);

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
void Trigger_on_add_any(void);
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
void Trigger_on_set_w_tag(void);
void Trigger_create_triggers_in_trigger(void);
void Trigger_on_add_superset_w_component(void);
void Trigger_on_set_superset_w_component(void);
void Trigger_on_add_base_superset_w_owned(void);
void Trigger_on_add_base_self_superset_w_owned(void);
void Trigger_on_set_self_from_child_of_prefab(void);
void Trigger_on_set_self_superset_from_child_of_prefab(void);
void Trigger_on_set_self_from_child_base_of_prefab(void);
void Trigger_on_set_self_superset_from_child_base_of_prefab(void);
void Trigger_on_set_self_auto_override(void);
void Trigger_on_set_self_superset_auto_override(void);
void Trigger_on_set_superset_auto_override(void);
void Trigger_not_only(void);
void Trigger_not_only_w_base(void);
void Trigger_not_only_w_base_no_match(void);
void Trigger_on_set_superset_after_filter_observer(void);
void Trigger_on_set_superset_after_filter_observer_w_on_add(void);
void Trigger_on_set_superset_after_filter_observer_w_on_add_isa_after_set(void);
void Trigger_on_set_superset_after_filter_observer_w_on_add_2(void);

// Testsuite 'Observer'
void Observer_2_terms_w_on_add(void);
void Observer_2_terms_w_on_remove(void);
void Observer_2_terms_w_on_set_value(void);
void Observer_2_terms_w_on_remove_value(void);
void Observer_2_terms_w_on_set_value_self_up(void);
void Observer_2_terms_w_on_remove_value_self_up(void);
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
void Observer_add_after_delete_observer(void);
void Observer_remove_after_delete_observer(void);
void Observer_delete_observer_w_ctx(void);
void Observer_update_ctx(void);
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
void Observer_and_from_empty(void);
void Observer_or_from_empty(void);
void Observer_and_from_empty_w_tag(void);
void Observer_or_from_empty_w_tag(void);
void Observer_notify_propagated_twice(void);
void Observer_on_add_yield_existing(void);
void Observer_on_add_yield_existing_2_tables(void);
void Observer_on_add_yield_existing_2_terms(void);
void Observer_on_add_yield_existing_wildcard(void);
void Observer_on_add_yield_existing_wildcard_multi(void);
void Observer_on_add_yield_existing_wildcard_multi_w_wildcard_pivot(void);
void Observer_observer_superset_wildcard(void);
void Observer_observer_superset_wildcard_add_isa(void);
void Observer_observer_superset_wildcard_add_isa_at_offset(void);
void Observer_on_set_w_tag(void);
void Observer_mixed_on_set_w_tag(void);
void Observer_mixed_un_set_w_tag(void);
void Observer_match_base_w_id_at_offset(void);
void Observer_custom_run_action(void);
void Observer_custom_run_action_2_terms(void);
void Observer_custom_run_action_w_iter_next_2_terms(void);
void Observer_custom_run_action_w_field(void);
void Observer_custom_run_action_w_2_fields(void);
void Observer_custom_run_w_yield_existing(void);
void Observer_custom_run_w_yield_existing_1_field(void);
void Observer_custom_run_w_yield_existing_1_field_w_callback(void);
void Observer_custom_run_w_yield_existing_2_fields(void);
void Observer_custom_run_w_yield_existing_2_fields_w_callback(void);
void Observer_read_in_on_remove_after_add_other_w_not(void);
void Observer_observer_w_short_notation(void);
void Observer_observer_w_filter_term(void);
void Observer_multi_observer_w_ctx_free(void);
void Observer_propagate_after_on_delete_clear_action(void);
void Observer_on_add_after_batch_w_exclusive_adds(void);
void Observer_propagate_match_relationship_w_self_up(void);
void Observer_propagate_match_relationship_w_up(void);
void Observer_propagate_isa_of_parent_add(void);
void Observer_propagate_isa_of_parent_remove(void);
void Observer_propagate_isa_of_parent_set(void);
void Observer_propagate_add_childof_of_parent(void);
void Observer_propagate_add_childof_of_parent_w_siblings(void);
void Observer_propagate_add_childof_w_parent_w_base(void);
void Observer_propagate_remove_childof_w_parent_w_base(void);
void Observer_propagate_remove_childof_of_parent(void);
void Observer_propagate_add_isa_of_parent(void);
void Observer_propagate_remove_isa_of_parent(void);
void Observer_propagate_add_childof_of_base(void);
void Observer_propagate_remove_childof_of_base(void);
void Observer_emit_for_parent_w_prefab_child_and_instance(void);
void Observer_observer_w_2_fixed_src(void);
void Observer_emit_for_recreated_id_after_remove_all(void);
void Observer_emit_for_recreated_id_after_remove_all_wildcard(void);
void Observer_emit_for_recreated_id_after_delete_with(void);
void Observer_emit_for_recreated_id_after_delete_with_wildcard(void);
void Observer_delete_observed_id(void);
void Observer_delete_observed_rel(void);
void Observer_delete_observed_tgt(void);
void Observer_on_add_2_pairs_w_uni_observer(void);
void Observer_on_add_2_pairs_w_multi_observer(void);
void Observer_on_set_2_pairs_w_uni_observer(void);
void Observer_on_set_2_pairs_w_multi_observer(void);
void Observer_on_remove_target_from_base_at_offset(void);
void Observer_on_remove_target_component_from_base_at_offset(void);
void Observer_filter_observer_after_observer(void);
void Observer_notify_after_defer_batched(void);
void Observer_notify_after_defer_batched_2_entities_in_table(void);
void Observer_notify_after_defer_batched_2_entities_in_table_w_tgt(void);
void Observer_multi_observer_table_fill_w_singleton(void);
void Observer_wildcard_propagate_w_other_table(void);
void Observer_add_in_yield_existing(void);
void Observer_add_in_yield_existing_multi(void);
void Observer_disable_observer(void);
void Observer_disable_observer_module(void);
void Observer_disable_observer_module_nested(void);
void Observer_disable_observer_and_module(void);
void Observer_disable_multi_observer(void);
void Observer_disable_multi_observer_module(void);
void Observer_disable_multi_observer_module_nested(void);
void Observer_disable_multi_observer_and_module(void);
void Observer_tag_w_on_set_and_on_add(void);
void Observer_tag_w_on_set_and_on_add_reverse(void);
void Observer_on_add_pair_w_rel_any(void);
void Observer_on_remove_pair_w_rel_any(void);
void Observer_on_add_pair_w_tgt_any(void);
void Observer_on_remove_pair_w_tgt_any(void);
void Observer_on_add_pair_w_any_any(void);
void Observer_on_remove_pair_w_any_any(void);
void Observer_on_add_any(void);
void Observer_on_remove_any(void);
void Observer_get_filter(void);
void Observer_uni_observer_eval_count(void);
void Observer_multi_observer_eval_count(void);
void Observer_yield_existing_uni_no_this(void);
void Observer_yield_existing_multi_no_this(void);
void Observer_observer_no_id_in_scope(void);
void Observer_register_comp_in_emit_named_entity(void);
void Observer_register_comp_w_macro_in_emit_named_entity(void);
void Observer_add_to_self_in_emit_entity(void);
void Observer_on_set_w_not_tag(void);
void Observer_on_set_w_not_component(void);
void Observer_wildcard_event(void);
void Observer_register_callback_after_run(void);
void Observer_register_run_after_callback(void);
void Observer_register_callback_after_run_ctx(void);
void Observer_register_run_after_callback_ctx(void);
void Observer_cache_test_1(void);
void Observer_cache_test_2(void);
void Observer_cache_test_3(void);
void Observer_cache_test_4(void);
void Observer_cache_test_5(void);
void Observer_cache_test_6(void);
void Observer_cache_test_7(void);
void Observer_cache_test_8(void);
void Observer_cache_test_9(void);
void Observer_cache_test_10(void);
void Observer_cache_test_11(void);
void Observer_cache_test_12(void);
void Observer_cache_test_13(void);
void Observer_cache_test_14(void);
void Observer_cache_test_15(void);
void Observer_cache_test_16(void);

// Testsuite 'ObserverOnSet'
void ObserverOnSet_set_1_of_1(void);
void ObserverOnSet_set_1_of_2(void);
void ObserverOnSet_set_1_of_3(void);
void ObserverOnSet_set_1_of_2_1_from_base(void);
void ObserverOnSet_set_1_of_3_1_from_base(void);
void ObserverOnSet_add_base(void);
void ObserverOnSet_add_base_to_1_overridden(void);
void ObserverOnSet_add_base_to_2_overridden(void);
void ObserverOnSet_add_base_to_1_of_2_overridden(void);
void ObserverOnSet_on_set_after_remove_override(void);
void ObserverOnSet_no_set_after_remove_base(void);
void ObserverOnSet_un_set_after_remove(void);
void ObserverOnSet_un_set_after_remove_base(void);
void ObserverOnSet_add_to_current_in_on_set(void);
void ObserverOnSet_remove_from_current_in_on_set(void);
void ObserverOnSet_remove_set_component_in_on_set(void);
void ObserverOnSet_match_table_created_w_add_in_on_set(void);
void ObserverOnSet_set_optional(void);
void ObserverOnSet_set_from_nothing(void);
void ObserverOnSet_add_0_entity_in_on_set(void);
void ObserverOnSet_on_set_prefab(void);

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
void TriggerOnAdd_set_after_add_in_on_add(void);
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
void TriggerOnRemove_on_remove_in_on_update(void);
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
void TriggerOnSet_on_set_after_remove_override(void);
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
void Monitor_monitor_at_fini(void);
void Monitor_monitor_other_table(void);
void Monitor_monitor_component(void);

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
void Prefab_inherit_disabled(void);
void Prefab_clone_after_inherit_in_on_add(void);
void Prefab_override_from_nested(void);
void Prefab_create_multiple_nested_w_on_set(void);
void Prefab_create_multiple_nested_w_on_set_in_progress(void);
void Prefab_single_on_set_on_child_w_override(void);
void Prefab_auto_override(void);
void Prefab_auto_override_2(void);
void Prefab_auto_override_nested(void);
void Prefab_auto_override_pair(void);
void Prefab_auto_override_pair_w_component(void);
void Prefab_auto_override_2_pairs(void);
void Prefab_auto_override_2_pairs_w_component(void);
void Prefab_auto_override_2_pairs_same_obj(void);
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
void Prefab_override_dont_inherit(void);
void Prefab_prefab_child_w_dont_inherit_component(void);
void Prefab_prefab_child_override(void);
void Prefab_prefab_child_override_w_exclusive_pair(void);
void Prefab_prefab_1_slot(void);
void Prefab_prefab_2_slots(void);
void Prefab_prefab_w_nested_slot(void);
void Prefab_prefab_w_mixed_slots(void);
void Prefab_prefab_variant_w_slot(void);
void Prefab_prefab_variant_w_base_slot(void);
void Prefab_prefab_variant_w_mixed_slots(void);
void Prefab_override_slot(void);
void Prefab_2_instances_w_slots_same_table(void);
void Prefab_slot_has_union(void);
void Prefab_slot_override(void);
void Prefab_base_slot_override(void);
void Prefab_override_twice_w_add(void);
void Prefab_override_twice_w_set(void);
void Prefab_auto_override_copy_once(void);
void Prefab_always_override(void);
void Prefab_always_override_pair(void);
void Prefab_child_of_prefab_is_prefab(void);
void Prefab_child_of_prefab_w_prefab_is_prefab(void);
void Prefab_child_of_prefab_w_prefab_is_prefab_w_component(void);
void Prefab_override_exclusive(void);
void Prefab_override_exclusive_2_lvls(void);
void Prefab_hierarchy_w_recycled_id(void);
void Prefab_disable_ids(void);
void Prefab_disable_nested_ids(void);

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
void World_delete_empty_tables_after_mini(void);
void World_delete_empty_tables_after_init(void);
void World_delete_1000_empty_tables(void);
void World_delete_empty_tables_for_id(void);
void World_use_after_delete_empty(void);
void World_use_after_clear_empty(void);
void World_use_after_delete_empty_w_component(void);
void World_use_after_clear_empty_w_component(void);
void World_use_after_clear_empty_w_component_w_lifecycle(void);
void World_use_after_clear_unused(void);
void World_ensure_in_at_fini(void);
void World_get_type_info(void);
void World_get_type_info_after_delete_with(void);
void World_get_type_info_after_reuse(void);
void World_no_name_prefix_after_init(void);
void World_component_init_w_name_prefix(void);
void World_component_macro_w_name_prefix(void);
void World_set_get_context(void);
void World_set_get_binding_context(void);
void World_set_get_context_w_free(void);
void World_set_get_binding_context_w_free(void);
void World_get_entities(void);

// Testsuite 'WorldInfo'
void WorldInfo_get_tick(void);
void WorldInfo_table_count(void);
void WorldInfo_empty_table_count(void);
void WorldInfo_table_create_count(void);
void WorldInfo_table_delete_count(void);
void WorldInfo_id_tag_component_count(void);
void WorldInfo_id_pair_count(void);

// Testsuite 'Type'
void Type_setup(void);
void Type_type_of_1_tostr(void);
void Type_type_of_2_tostr(void);
void Type_type_of_2_tostr_no_id(void);
void Type_invalid_entity_type_expression(void);
void Type_invalid_system_type_expression(void);
void Type_get_type(void);
void Type_get_type_from_empty(void);
void Type_get_type_from_0(void);
void Type_type_to_expr_pair(void);
void Type_type_to_expr_pair_w_comp(void);
void Type_type_to_expr_scope(void);
void Type_type_to_expr_pair_w_override(void);
void Type_entity_str(void);
void Type_entity_path_str(void);
void Type_entity_instanceof_str(void);
void Type_entity_childof_str(void);
void Type_entity_pair_str(void);
void Type_entity_str_small_buffer(void);
void Type_role_pair_str(void);
void Type_role_owned_str(void);
void Type_role_disabled_str(void);
void Type_large_type_expr(void);
void Type_large_type_expr_limit(void);

// Testsuite 'Commands'
void Commands_defer_new(void);
void Commands_defer_bulk_new(void);
void Commands_defer_add(void);
void Commands_defer_add_two(void);
void Commands_defer_remove(void);
void Commands_defer_remove_two(void);
void Commands_defer_set(void);
void Commands_defer_delete(void);
void Commands_defer_twice(void);
void Commands_defer_twice_in_progress(void);
void Commands_run_w_defer(void);
void Commands_system_in_progress_w_defer(void);
void Commands_defer_ensure(void);
void Commands_defer_ensure_no_modify(void);
void Commands_defer_ensure_w_modify(void);
void Commands_defer_modify(void);
void Commands_defer_set_pair(void);
void Commands_defer_clear(void);
void Commands_defer_add_after_delete(void);
void Commands_defer_set_after_delete(void);
void Commands_defer_ensure_after_delete(void);
void Commands_defer_ensure_after_delete_2nd_to_last(void);
void Commands_defer_add_child_to_deleted_parent(void);
void Commands_recreate_deleted_entity_while_deferred(void);
void Commands_defer_add_to_recycled_id(void);
void Commands_defer_add_to_recycled_id_w_role(void);
void Commands_defer_add_to_recycled_relation(void);
void Commands_defer_add_to_recycled_object(void);
void Commands_defer_add_to_recycled_object_childof(void);
void Commands_defer_add_to_deleted_id(void);
void Commands_defer_add_to_deleted_id_w_role(void);
void Commands_defer_add_to_deleted_relation(void);
void Commands_defer_add_to_deleted_object(void);
void Commands_defer_add_to_deleted_object_childof(void);
void Commands_defer_delete_added_id(void);
void Commands_defer_delete_added_id_w_role(void);
void Commands_defer_delete_added_relation(void);
void Commands_defer_delete_added_object(void);
void Commands_defer_delete_added_object_childof(void);
void Commands_discard_add(void);
void Commands_discard_remove(void);
void Commands_discard_add_two(void);
void Commands_discard_remove_two(void);
void Commands_discard_child(void);
void Commands_discard_child_w_add(void);
void Commands_defer_return_value(void);
void Commands_defer_ensure_pair(void);
void Commands_async_stage_add(void);
void Commands_async_stage_add_twice(void);
void Commands_async_stage_remove(void);
void Commands_async_stage_clear(void);
void Commands_async_stage_delete(void);
void Commands_async_stage_new(void);
void Commands_async_stage_readonly(void);
void Commands_register_component_while_in_progress(void);
void Commands_register_component_while_staged(void);
void Commands_register_component_while_deferred(void);
void Commands_defer_enable(void);
void Commands_defer_disable(void);
void Commands_defer_enable_from_stage(void);
void Commands_defer_toggle(void);
void Commands_defer_toggle_from_stage(void);
void Commands_defer_delete_with(void);
void Commands_defer_remove_all(void);
void Commands_deferred_modified_after_remove(void);
void Commands_merge_cleanup_ops_before_delete(void);
void Commands_merge_nested_cleanup_ops_before_delete(void);
void Commands_defer_suspend_resume(void);
void Commands_create_observer_while_deferred(void);
void Commands_create_query_while_deferred(void);
void Commands_update_observer_while_deferred(void);
void Commands_defer_set_large_component(void);
void Commands_defer_while_suspend_readonly(void);
void Commands_defer_while_suspend_readonly_w_existing_commands(void);
void Commands_defer_remove_after_set(void);
void Commands_defer_remove_after_set_w_observer(void);
void Commands_defer_override_after_remove(void);
void Commands_defer_override_after_remove_3_ops(void);
void Commands_flush_stage_to_deferred_world(void);
void Commands_add_in_observer_during_merge(void);
void Commands_add_in_observer_during_merge_2_commands(void);
void Commands_add_2_in_observer_while_on_remove_for_delete(void);
void Commands_add_2_in_observer_while_on_remove_for_delete_child(void);
void Commands_add_2_in_observer_while_on_remove_for_delete_recycled_id(void);
void Commands_add_2_in_observer_while_on_remove_for_deferred_delete_recycled_id(void);
void Commands_defer_add_after_clear(void);
void Commands_defer_cmd_after_modified(void);
void Commands_defer_remove_after_emplace_different_id(void);
void Commands_defer_remove_after_set_and_emplace_different_id(void);
void Commands_clear_after_add_to_nonempty(void);
void Commands_remove_after_add_to_nonempty(void);
void Commands_register_while_deferred_with_n_stages(void);
void Commands_defer_2_sets_w_multi_observer(void);
void Commands_defer_2_ensures_w_multi_observer(void);
void Commands_defer_2_ensures_no_modified_w_multi_observer(void);
void Commands_exists_remove_set(void);
void Commands_absent_remove_set(void);
void Commands_exists_set_remove(void);
void Commands_absent_set_remove(void);
void Commands_exists_remove_ensure(void);
void Commands_absent_remove_ensure(void);
void Commands_exists_ensure_remove(void);
void Commands_absent_ensure_remove(void);
void Commands_exists_set_w_ensure(void);
void Commands_absent_set_invoke_on_set(void);
void Commands_exists_set_invoke_on_set(void);
void Commands_defer_ensure_no_on_set(void);
void Commands_defer_existing_ensure_no_on_set(void);
void Commands_ensure_override(void);
void Commands_set_override(void);
void Commands_absent_ensure_for_entity_w_tag(void);
void Commands_on_set_hook_before_on_add_for_existing_component(void);
void Commands_defer_2_sets_w_observer_same_component(void);
void Commands_defer_2_sets_w_observer_other_component(void);
void Commands_on_remove_after_deferred_clear_and_add(void);
void Commands_defer_delete_recycle_same_id(void);
void Commands_observer_while_defer_suspended(void);
void Commands_on_add_hook_while_defer_suspended(void);
void Commands_on_set_hook_while_defer_suspended(void);
void Commands_on_remove_hook_while_defer_suspended(void);
void Commands_on_set_hook_batched_is_deferred(void);
void Commands_add_path(void);
void Commands_add_path_to_deleted_parent(void);
void Commands_add_path_nested(void);
void Commands_add_path_nested_to_deleted_parent(void);
void Commands_add_path_nested_to_created_deleted_parent(void);
void Commands_add_path_w_stage(void);
void Commands_add_path_to_deleted_parent_w_stage(void);
void Commands_add_path_nested_w_stage(void);
void Commands_add_path_nested_to_deleted_parent_w_stage(void);
void Commands_add_path_nested_to_created_deleted_parent_w_stage(void);
void Commands_defer_emplace_w_arg(void);
void Commands_defer_emplace_existing_w_arg(void);
void Commands_mixed_on_add_on_set_w_set_w_batching(void);
void Commands_mixed_on_add_on_set_w_emplace(void);
void Commands_add_isa_set_w_override_batched(void);
void Commands_add_set_isa_w_override_batched(void);
void Commands_add_batched_set_with(void);
void Commands_defer_emplace_after_remove(void);

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
void SingleThreadStaging_ensureable(void);
void SingleThreadStaging_ensureable_from_main(void);
void SingleThreadStaging_ensureable_w_add(void);
void SingleThreadStaging_on_add_after_new_type_in_progress(void);
void SingleThreadStaging_lock_table(void);
void SingleThreadStaging_recursive_lock_table(void);
void SingleThreadStaging_modify_after_lock(void);
void SingleThreadStaging_get_object_from_stage(void);
void SingleThreadStaging_add_to_world_while_readonly(void);
void SingleThreadStaging_add_to_world_and_stage_while_readonly(void);
void SingleThreadStaging_lookup_after_stage_count_change(void);
void SingleThreadStaging_lookup_w_scope_after_stage_count_change(void);
void SingleThreadStaging_with_after_stage_count_change(void);

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

// Testsuite 'Table'
void Table_get_index(void);
void Table_get_index_after_tag(void);
void Table_get_index_not_in_table(void);
void Table_get_index_tag(void);
void Table_get_index_pair(void);
void Table_get_index_pair_tag(void);
void Table_get_column(void);
void Table_get_column_for_tag(void);
void Table_get_column_for_low_tag(void);
void Table_get_column_for_high_component(void);
void Table_get_column_for_component_after_tag(void);
void Table_get_column_w_offset(void);
void Table_get_id(void);
void Table_get_component(void);
void Table_get_pair(void);
void Table_get_from_stage(void);
void Table_get_depth(void);
void Table_get_depth_non_acyclic(void);
void Table_get_depth_2_paths(void);
void Table_get_column_size(void);

// Testsuite 'Poly'
void Poly_on_set_poly_observer(void);
void Poly_on_set_poly_query(void);
void Poly_on_set_poly_system(void);

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
void Internals_records_resize_on_override(void);
void Internals_table_observed_after_add(void);
void Internals_table_observed_after_remove(void);
void Internals_table_observed_after_clear(void);
void Internals_table_observed_after_delete(void);
void Internals_table_observed_after_on_remove(void);
void Internals_table_observed_after_entity_flag(void);
void Internals_table_create_leak_check(void);

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

// Testsuite 'StackAlloc'
void StackAlloc_init_fini(void);
void StackAlloc_multiple_overlapping_cursors(void);

bake_test_case Id_testcases[] = {
    {
        "0_is_wildcard",
        Id_0_is_wildcard
    },
    {
        "wildcard_is_wildcard",
        Id_wildcard_is_wildcard
    },
    {
        "any_is_wildcard",
        Id_any_is_wildcard
    },
    {
        "entity_is_wildcard",
        Id_entity_is_wildcard
    },
    {
        "pair_is_wildcard",
        Id_pair_is_wildcard
    },
    {
        "pair_w_rel_wildcard_is_wildcard",
        Id_pair_w_rel_wildcard_is_wildcard
    },
    {
        "pair_w_obj_wildcard_is_wildcard",
        Id_pair_w_obj_wildcard_is_wildcard
    },
    {
        "pair_w_wildcard_wildcard_is_wildcard",
        Id_pair_w_wildcard_wildcard_is_wildcard
    },
    {
        "pair_w_rel_any_is_wildcard",
        Id_pair_w_rel_any_is_wildcard
    },
    {
        "pair_w_obj_any_is_wildcard",
        Id_pair_w_obj_any_is_wildcard
    },
    {
        "pair_w_any_any_is_wildcard",
        Id_pair_w_any_any_is_wildcard
    },
    {
        "pair_w_override_is_wildcard",
        Id_pair_w_override_is_wildcard
    },
    {
        "pair_w_toggle_is_wildcard",
        Id_pair_w_toggle_is_wildcard
    },
    {
        "tag_id_is_tag",
        Id_tag_id_is_tag
    },
    {
        "component_id_is_tag",
        Id_component_id_is_tag
    },
    {
        "pair_id_is_tag",
        Id_pair_id_is_tag
    },
    {
        "pair_id_w_rel_component_is_tag",
        Id_pair_id_w_rel_component_is_tag
    },
    {
        "pair_id_w_obj_component_is_tag",
        Id_pair_id_w_obj_component_is_tag
    },
    {
        "pair_id_w_rel_component_obj_wildcard_is_tag",
        Id_pair_id_w_rel_component_obj_wildcard_is_tag
    },
    {
        "pair_id_w_obj_wildcard_is_tag",
        Id_pair_id_w_obj_wildcard_is_tag
    },
    {
        "pair_id_w_tag_property_w_obj_component_is_tag",
        Id_pair_id_w_tag_property_w_obj_component_is_tag
    },
    {
        "pair_id_w_tag_property_w_obj_wildcard_is_tag",
        Id_pair_id_w_tag_property_w_obj_wildcard_is_tag
    },
    {
        "pair_w_rel_wildcard_is_tag",
        Id_pair_w_rel_wildcard_is_tag
    },
    {
        "pair_w_obj_wildcard_is_tag",
        Id_pair_w_obj_wildcard_is_tag
    },
    {
        "pair_w_rel_tag_obj_wildcard_is_tag",
        Id_pair_w_rel_tag_obj_wildcard_is_tag
    },
    {
        "pair_w_wildcard_wildcard_is_tag",
        Id_pair_w_wildcard_wildcard_is_tag
    },
    {
        "pair_w_rel_any_is_tag",
        Id_pair_w_rel_any_is_tag
    },
    {
        "pair_w_obj_any_is_tag",
        Id_pair_w_obj_any_is_tag
    },
    {
        "pair_w_rel_tag_obj_any_is_tag",
        Id_pair_w_rel_tag_obj_any_is_tag
    },
    {
        "pair_w_any_any_is_tag",
        Id_pair_w_any_any_is_tag
    },
    {
        "id_w_override_is_tag",
        Id_id_w_override_is_tag
    },
    {
        "id_w_toggle_is_tag",
        Id_id_w_toggle_is_tag
    },
    {
        "pair_id_override_is_tag",
        Id_pair_id_override_is_tag
    },
    {
        "pair_id_toggle_is_tag",
        Id_pair_id_toggle_is_tag
    },
    {
        "make_pair",
        Id_make_pair
    },
    {
        "make_pair_of_pair",
        Id_make_pair_of_pair
    },
    {
        "make_pair_of_pair_tgt",
        Id_make_pair_of_pair_tgt
    }
};

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
        "init_id_fullpath_w_scope",
        Entity_init_id_fullpath_w_scope
    },
    {
        "init_id_fullpath_w_scope_existing",
        Entity_init_id_fullpath_w_scope_existing
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
        "init_w_name_twice_deferred",
        Entity_init_w_name_twice_deferred
    },
    {
        "init_w_nested_name_twice_deferred",
        Entity_init_w_nested_name_twice_deferred
    },
    {
        "init_w_scope_name_twice_deferred",
        Entity_init_w_scope_name_twice_deferred
    },
    {
        "init_w_childof_name_twice_deferred",
        Entity_init_w_childof_name_twice_deferred
    },
    {
        "init_w_childof_nested_name_twice_deferred",
        Entity_init_w_childof_nested_name_twice_deferred
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
        "make_alive_zero_gen",
        Entity_make_alive_zero_gen
    },
    {
        "make_alive_nonzero_gen",
        Entity_make_alive_nonzero_gen
    },
    {
        "make_alive_zero_gen_exists",
        Entity_make_alive_zero_gen_exists
    },
    {
        "make_alive_nonzero_gen_exists",
        Entity_make_alive_nonzero_gen_exists
    },
    {
        "make_alive_zero_gen_exists_alive",
        Entity_make_alive_zero_gen_exists_alive
    },
    {
        "make_alive_nonzero_gen_exists_alive",
        Entity_make_alive_nonzero_gen_exists_alive
    },
    {
        "set_scope_w_entity_init_from_stage",
        Entity_set_scope_w_entity_init_from_stage
    },
    {
        "entity_init_w_scope_twice",
        Entity_entity_init_w_scope_twice
    },
    {
        "entity_init_w_childof_twice",
        Entity_entity_init_w_childof_twice
    },
    {
        "entity_init_w_childof_nested_name_twice",
        Entity_entity_init_w_childof_nested_name_twice
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
        "entity_init_w_childof_and_no_name",
        Entity_entity_init_w_childof_and_no_name
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
        "deferred_entity_init_w_childof_and_no_name",
        Entity_deferred_entity_init_w_childof_and_no_name
    },
    {
        "entity_init_w_empty_string_name",
        Entity_entity_init_w_empty_string_name
    },
    {
        "entity_init_w_empty_string_symbol",
        Entity_entity_init_w_empty_string_symbol
    },
    {
        "set_name_w_0_twice",
        Entity_set_name_w_0_twice
    },
    {
        "new_entity_twice",
        Entity_new_entity_twice
    },
    {
        "new_entity_scoped_twice",
        Entity_new_entity_scoped_twice
    },
    {
        "defer_component_init",
        Entity_defer_component_init
    },
    {
        "defer_component_init_w_symbol",
        Entity_defer_component_init_w_symbol
    },
    {
        "defer_entity_init_w_symbol",
        Entity_defer_entity_init_w_symbol
    },
    {
        "staged_set_name_n_stages",
        Entity_staged_set_name_n_stages
    },
    {
        "staged_set_symbol_n_stages",
        Entity_staged_set_symbol_n_stages
    },
    {
        "entity_init_w_add_childof_no_name",
        Entity_entity_init_w_add_childof_no_name
    },
    {
        "entity_w_short_notation",
        Entity_entity_w_short_notation
    },
    {
        "override_inherited_symbol",
        Entity_override_inherited_symbol
    },
    {
        "use_low_id_for_component",
        Entity_use_low_id_for_component
    },
    {
        "get_depth",
        Entity_get_depth
    },
    {
        "get_depth_non_acyclic",
        Entity_get_depth_non_acyclic
    },
    {
        "get_depth_empty",
        Entity_get_depth_empty
    },
    {
        "get_depth_2_paths",
        Entity_get_depth_2_paths
    },
    {
        "entity_init_w_empty_sep",
        Entity_entity_init_w_empty_sep
    },
    {
        "entity_init_w_empty_sep_from_scope",
        Entity_entity_init_w_empty_sep_from_scope
    },
    {
        "entity_init_w_empty_sep_w_prefix",
        Entity_entity_init_w_empty_sep_w_prefix
    },
    {
        "set_name_w_same_ptr",
        Entity_set_name_w_same_ptr
    },
    {
        "set_name_w_overlapping_ptr",
        Entity_set_name_w_overlapping_ptr
    },
    {
        "defer_set_name_w_overlapping_ptr",
        Entity_defer_set_name_w_overlapping_ptr
    },
    {
        "make_alive_from_stage",
        Entity_make_alive_from_stage
    },
    {
        "make_alive_after_deleted_1_entity",
        Entity_make_alive_after_deleted_1_entity
    },
    {
        "make_alive_after_deleted_2_entities",
        Entity_make_alive_after_deleted_2_entities
    },
    {
        "defer_entity_init_w_set_name_w_add_childof",
        Entity_defer_entity_init_w_set_name_w_add_childof
    },
    {
        "entity_w_digit_name",
        Entity_entity_w_digit_name
    },
    {
        "entity_w_existing_digit_name",
        Entity_entity_w_existing_digit_name
    },
    {
        "entity_from_digit",
        Entity_entity_from_digit
    },
    {
        "entity_from_existing_digit",
        Entity_entity_from_existing_digit
    },
    {
        "entity_from_digit_path",
        Entity_entity_from_digit_path
    },
    {
        "entity_from_existing_digit_path",
        Entity_entity_from_existing_digit_path
    },
    {
        "entity_from_digit_0_path",
        Entity_entity_from_digit_0_path
    },
    {
        "entity_from_conflicting_digit",
        Entity_entity_from_conflicting_digit
    },
    {
        "set_version_on_nonempty_entity",
        Entity_set_version_on_nonempty_entity
    },
    {
        "set_version_while_deferred",
        Entity_set_version_while_deferred
    },
    {
        "commit_w_on_add",
        Entity_commit_w_on_add
    },
    {
        "commit_w_on_remove",
        Entity_commit_w_on_remove
    },
    {
        "commit_w_cmd_in_observer",
        Entity_commit_w_cmd_in_observer
    },
    {
        "entity_init_existing_no_sep",
        Entity_entity_init_existing_no_sep
    },
    {
        "entity_init_w_set_1_comp",
        Entity_entity_init_w_set_1_comp
    },
    {
        "entity_init_w_set_2_comp",
        Entity_entity_init_w_set_2_comp
    },
    {
        "entity_init_w_set_1_comp_1_tag",
        Entity_entity_init_w_set_1_comp_1_tag
    },
    {
        "entity_init_w_set_2_comp_2_tag",
        Entity_entity_init_w_set_2_comp_2_tag
    },
    {
        "entity_init_w_set_1_comp_w_name",
        Entity_entity_init_w_set_1_comp_w_name
    },
    {
        "entity_init_w_set_1_comp_existing",
        Entity_entity_init_w_set_1_comp_existing
    },
    {
        "entity_init_w_set_1_comp_existing_empty",
        Entity_entity_init_w_set_1_comp_existing_empty
    },
    {
        "entity_init_w_set_1_comp_1_tag_w_set",
        Entity_entity_init_w_set_1_comp_1_tag_w_set
    },
    {
        "entity_init_w_set_w_hook",
        Entity_entity_init_w_set_w_hook
    },
    {
        "entity_init_w_set_w_observer",
        Entity_entity_init_w_set_w_observer
    },
    {
        "entity_init_w_set_1_comp_defer",
        Entity_entity_init_w_set_1_comp_defer
    },
    {
        "entity_init_w_set_2_comp_defer",
        Entity_entity_init_w_set_2_comp_defer
    },
    {
        "entity_init_w_set_1_comp_1_tag_defer",
        Entity_entity_init_w_set_1_comp_1_tag_defer
    },
    {
        "entity_init_w_set_2_comp_2_tag_defer",
        Entity_entity_init_w_set_2_comp_2_tag_defer
    },
    {
        "entity_init_w_set_1_comp_w_name_defer",
        Entity_entity_init_w_set_1_comp_w_name_defer
    },
    {
        "entity_init_w_set_1_comp_existing_defer",
        Entity_entity_init_w_set_1_comp_existing_defer
    },
    {
        "entity_init_w_set_1_comp_existing_empty_defer",
        Entity_entity_init_w_set_1_comp_existing_empty_defer
    },
    {
        "entity_init_w_set_1_comp_1_tag_w_set_defer",
        Entity_entity_init_w_set_1_comp_1_tag_w_set_defer
    },
    {
        "insert_1_comp",
        Entity_insert_1_comp
    },
    {
        "insert_2_comp",
        Entity_insert_2_comp
    },
    {
        "insert_1_comp_1_tag",
        Entity_insert_1_comp_1_tag
    },
    {
        "entity_w_parent",
        Entity_entity_w_parent
    },
    {
        "entity_w_parent_w_name",
        Entity_entity_w_parent_w_name
    },
    {
        "entity_w_parent_w_add",
        Entity_entity_w_parent_w_add
    },
    {
        "entity_w_parent_w_add_w_parent",
        Entity_entity_w_parent_w_add_w_parent
    },
    {
        "entity_w_parent_w_set",
        Entity_entity_w_parent_w_set
    },
    {
        "entity_w_parent_w_set_w_parent",
        Entity_entity_w_parent_w_set_w_parent
    }
};

bake_test_case Each_testcases[] = {
    {
        "each_tag",
        Each_each_tag
    },
    {
        "each_component",
        Each_each_component
    },
    {
        "each_pair",
        Each_each_pair
    },
    {
        "each_pair_rel_wildcard",
        Each_each_pair_rel_wildcard
    },
    {
        "each_pair_tgt_wildcard",
        Each_each_pair_tgt_wildcard
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
        "worker_iter_w_singleton_component_instanced",
        Iter_worker_iter_w_singleton_component_instanced
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
        "paged_iter_w_singleton_component_instanced",
        Iter_paged_iter_w_singleton_component_instanced
    },
    {
        "page_iter_w_offset_skip_1_archetype",
        Iter_page_iter_w_offset_skip_1_archetype
    },
    {
        "page_iter_w_offset_skip_1_archetype_plus_one",
        Iter_page_iter_w_offset_skip_1_archetype_plus_one
    },
    {
        "page_iter_w_offset_skip_2_archetypes",
        Iter_page_iter_w_offset_skip_2_archetypes
    },
    {
        "page_iter_w_limit_skip_1_archetype",
        Iter_page_iter_w_limit_skip_1_archetype
    },
    {
        "page_iter_w_limit_skip_1_archetype_minus_one",
        Iter_page_iter_w_limit_skip_1_archetype_minus_one
    },
    {
        "page_iter_w_limit_skip_2_archetypes",
        Iter_page_iter_w_limit_skip_2_archetypes
    },
    {
        "page_iter_w_offset_1_limit_max",
        Iter_page_iter_w_offset_1_limit_max
    },
    {
        "page_iter_w_offset_1_limit_minus_1",
        Iter_page_iter_w_offset_1_limit_minus_1
    },
    {
        "page_iter_w_offset_2_type_limit_max",
        Iter_page_iter_w_offset_2_type_limit_max
    },
    {
        "page_iter_w_offset_2_type_limit_minus_1",
        Iter_page_iter_w_offset_2_type_limit_minus_1
    },
    {
        "page_iter_w_limit_1_all_offsets",
        Iter_page_iter_w_limit_1_all_offsets
    },
    {
        "page_iter_w_offset_out_of_bounds",
        Iter_page_iter_w_offset_out_of_bounds
    },
    {
        "page_iter_w_limit_out_of_bounds",
        Iter_page_iter_w_limit_out_of_bounds
    },
    {
        "page_iter_comb_10_entities_1_type",
        Iter_page_iter_comb_10_entities_1_type
    },
    {
        "page_iter_comb_10_entities_2_types",
        Iter_page_iter_comb_10_entities_2_types
    },
    {
        "count",
        Iter_count
    },
    {
        "iter_restore_stack_iter",
        Iter_iter_restore_stack_iter
    },
    {
        "interleaved_iter",
        Iter_interleaved_iter
    },
    {
        "get_first",
        Iter_get_first
    },
    {
        "page_iter_w_only_tag",
        Iter_page_iter_w_only_tag
    },
    {
        "worker_iter_w_only_tag",
        Iter_worker_iter_w_only_tag
    },
    {
        "page_iter_w_inout_none",
        Iter_page_iter_w_inout_none
    },
    {
        "worker_iter_w_inout_none",
        Iter_worker_iter_w_inout_none
    },
    {
        "page_iter_w_ctx",
        Iter_page_iter_w_ctx
    },
    {
        "page_iter_w_binding_ctx",
        Iter_page_iter_w_binding_ctx
    },
    {
        "worker_iter_w_ctx",
        Iter_worker_iter_w_ctx
    },
    {
        "worker_iter_w_binding_ctx",
        Iter_worker_iter_w_binding_ctx
    },
    {
        "column_index_owned",
        Iter_column_index_owned
    },
    {
        "column_index_shared",
        Iter_column_index_shared
    },
    {
        "column_index_not",
        Iter_column_index_not
    },
    {
        "page_iter_w_fini",
        Iter_page_iter_w_fini
    },
    {
        "worker_iter_w_fini",
        Iter_worker_iter_w_fini
    },
    {
        "rule_page_iter_w_fini",
        Iter_rule_page_iter_w_fini
    },
    {
        "rule_worker_iter_w_fini",
        Iter_rule_worker_iter_w_fini
    },
    {
        "to_str_before_next",
        Iter_to_str_before_next
    },
    {
        "to_str",
        Iter_to_str
    }
};

bake_test_case Search_testcases[] = {
    {
        "search",
        Search_search
    },
    {
        "search_wildcard",
        Search_search_wildcard
    },
    {
        "search_wildcard_w_offset",
        Search_search_wildcard_w_offset
    },
    {
        "search_relation_wildcard_w_offset",
        Search_search_relation_wildcard_w_offset
    },
    {
        "search_pair_w_any_rel",
        Search_search_pair_w_any_rel
    },
    {
        "search_pair_w_any_obj",
        Search_search_pair_w_any_obj
    },
    {
        "search_follow_relation_lvl_0",
        Search_search_follow_relation_lvl_0
    },
    {
        "search_follow_relation_lvl_1",
        Search_search_follow_relation_lvl_1
    },
    {
        "search_follow_relation_lvl_2",
        Search_search_follow_relation_lvl_2
    },
    {
        "search_follow_relation_lvl_3",
        Search_search_follow_relation_lvl_3
    },
    {
        "search_first_lvl_0",
        Search_search_first_lvl_0
    },
    {
        "search_first_lvl_1",
        Search_search_first_lvl_1
    },
    {
        "search_first_lvl_2",
        Search_search_first_lvl_2
    },
    {
        "search_first_lvl_3",
        Search_search_first_lvl_3
    },
    {
        "search_relation_wildcard",
        Search_search_relation_wildcard
    },
    {
        "search_relation_at_offset",
        Search_search_relation_at_offset
    },
    {
        "search_relation_inherit_from_parent",
        Search_search_relation_inherit_from_parent
    },
    {
        "search_relation_dont_inherit",
        Search_search_relation_dont_inherit
    },
    {
        "search_relation_dont_inherit_from_parent",
        Search_search_relation_dont_inherit_from_parent
    },
    {
        "search_relation_exclusive",
        Search_search_relation_exclusive
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
        "emit_table_event",
        Event_emit_table_event
    },
    {
        "emit_staged_from_world",
        Event_emit_staged_from_world
    },
    {
        "emit_staged_from_stage",
        Event_emit_staged_from_stage
    },
    {
        "emit_staged_from_world_observer",
        Event_emit_staged_from_world_observer
    },
    {
        "emit_staged_from_stage_observer",
        Event_emit_staged_from_stage_observer
    },
    {
        "emit_for_entity",
        Event_emit_for_entity
    },
    {
        "emit_custom_for_any",
        Event_emit_custom_for_any
    },
    {
        "emit_custom_implicit_any",
        Event_emit_custom_implicit_any
    },
    {
        "emit_custom_empty_type",
        Event_emit_custom_empty_type
    },
    {
        "emit_w_param",
        Event_emit_w_param
    },
    {
        "emit_w_param_multi_observer",
        Event_emit_w_param_multi_observer
    },
    {
        "emit_w_const_param",
        Event_emit_w_const_param
    },
    {
        "emit_nested",
        Event_emit_nested
    },
    {
        "enqueue_event_1_id",
        Event_enqueue_event_1_id
    },
    {
        "enqueue_event_2_ids",
        Event_enqueue_event_2_ids
    },
    {
        "enqueue_event_w_data",
        Event_enqueue_event_w_data
    },
    {
        "enqueue_event_w_data_move",
        Event_enqueue_event_w_data_move
    },
    {
        "enqueue_event_w_data_copy",
        Event_enqueue_event_w_data_copy
    },
    {
        "enqueue_event_w_const_data_no_copy",
        Event_enqueue_event_w_const_data_no_copy
    },
    {
        "enqueue_event_not_alive",
        Event_enqueue_event_not_alive
    },
    {
        "enqueue_event_not_alive_w_data_move",
        Event_enqueue_event_not_alive_w_data_move
    },
    {
        "enqueue_event_not_alive_w_data_copy",
        Event_enqueue_event_not_alive_w_data_copy
    },
    {
        "enqueue_event_not_alive_after_delete_during_merge",
        Event_enqueue_event_not_alive_after_delete_during_merge
    },
    {
        "enqueue_event_not_alive_w_data_move_after_delete_during_merge",
        Event_enqueue_event_not_alive_w_data_move_after_delete_during_merge
    },
    {
        "enqueue_event_not_alive_w_data_copy_after_delete_during_merge",
        Event_enqueue_event_not_alive_w_data_copy_after_delete_during_merge
    },
    {
        "enqueue_event_not_deferred",
        Event_enqueue_event_not_deferred
    },
    {
        "enqueue_event_not_deferred_to_async",
        Event_enqueue_event_not_deferred_to_async
    },
    {
        "enqueue_custom_implicit_any",
        Event_enqueue_custom_implicit_any
    },
    {
        "enqueue_custom_after_large_cmd",
        Event_enqueue_custom_after_large_cmd
    },
    {
        "enqueue_on_readonly_world",
        Event_enqueue_on_readonly_world
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
    },
    {
        "add_after_bulk",
        New_w_Count_add_after_bulk
    },
    {
        "add_after_bulk_w_component",
        New_w_Count_add_after_bulk_w_component
    },
    {
        "add_after_bulk_w_ctor",
        New_w_Count_add_after_bulk_w_ctor
    },
    {
        "recycle_1_of_2",
        New_w_Count_recycle_1_of_2
    },
    {
        "recycle_1_of_3",
        New_w_Count_recycle_1_of_3
    },
    {
        "recycle_2_of_3",
        New_w_Count_recycle_2_of_3
    },
    {
        "bulk_init_w_table",
        New_w_Count_bulk_init_w_table
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
    },
    {
        "invalid_add_wildcard",
        Add_invalid_add_wildcard
    },
    {
        "invalid_add_pair_w_wildcard",
        Add_invalid_add_pair_w_wildcard
    },
    {
        "invalid_add_pair_w_wildcard_rel",
        Add_invalid_add_pair_w_wildcard_rel
    },
    {
        "invalid_add_pair_w_wildcard_obj",
        Add_invalid_add_pair_w_wildcard_obj
    },
    {
        "invalid_add_any",
        Add_invalid_add_any
    },
    {
        "invalid_add_pair_w_any",
        Add_invalid_add_pair_w_any
    },
    {
        "invalid_add_pair_w_any_rel",
        Add_invalid_add_pair_w_any_rel
    },
    {
        "invalid_add_pair_w_any_obj",
        Add_invalid_add_pair_w_any_obj
    },
    {
        "invalid_pair_w_0",
        Add_invalid_pair_w_0
    },
    {
        "invalid_pair_w_0_rel",
        Add_invalid_pair_w_0_rel
    },
    {
        "invalid_pair_w_0_obj",
        Add_invalid_pair_w_0_obj
    },
    {
        "add_random_id",
        Add_add_random_id
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

bake_test_case Sparse_testcases[] = {
    {
        "has",
        Sparse_has
    },
    {
        "owns",
        Sparse_owns
    },
    {
        "get",
        Sparse_get
    },
    {
        "get_mut",
        Sparse_get_mut
    },
    {
        "ensure",
        Sparse_ensure
    },
    {
        "emplace",
        Sparse_emplace
    },
    {
        "set",
        Sparse_set
    },
    {
        "modified_no_on_set",
        Sparse_modified_no_on_set
    },
    {
        "insert_1",
        Sparse_insert_1
    },
    {
        "insert_2",
        Sparse_insert_2
    },
    {
        "get_ref",
        Sparse_get_ref
    },
    {
        "update_ref",
        Sparse_update_ref
    },
    {
        "get_recycled",
        Sparse_get_recycled
    },
    {
        "get_mut_recycled",
        Sparse_get_mut_recycled
    },
    {
        "ensure_recycled",
        Sparse_ensure_recycled
    },
    {
        "emplace_recycled",
        Sparse_emplace_recycled
    },
    {
        "set_recycled",
        Sparse_set_recycled
    },
    {
        "get_ref_recycled",
        Sparse_get_ref_recycled
    },
    {
        "test_stable_ptr",
        Sparse_test_stable_ptr
    },
    {
        "has_after_remove",
        Sparse_has_after_remove
    },
    {
        "get_after_remove",
        Sparse_get_after_remove
    },
    {
        "get_mut_after_remove",
        Sparse_get_mut_after_remove
    },
    {
        "sparse_w_hole",
        Sparse_sparse_w_hole
    },
    {
        "record_get",
        Sparse_record_get
    },
    {
        "has_inherited",
        Sparse_has_inherited
    },
    {
        "owns_inherited",
        Sparse_owns_inherited
    },
    {
        "get_inherited",
        Sparse_get_inherited
    },
    {
        "get_mut_inherited",
        Sparse_get_mut_inherited
    },
    {
        "ensure_inherited",
        Sparse_ensure_inherited
    },
    {
        "emplace_inherited",
        Sparse_emplace_inherited
    },
    {
        "ctor_after_emplace",
        Sparse_ctor_after_emplace
    },
    {
        "ctor_dtor_after_remove",
        Sparse_ctor_dtor_after_remove
    },
    {
        "ctor_dtor_after_clear",
        Sparse_ctor_dtor_after_clear
    },
    {
        "ctor_dtor_after_delete",
        Sparse_ctor_dtor_after_delete
    },
    {
        "ctor_dtor_after_fini",
        Sparse_ctor_dtor_after_fini
    },
    {
        "on_add_remove_after_remove",
        Sparse_on_add_remove_after_remove
    },
    {
        "on_add_remove_after_clear",
        Sparse_on_add_remove_after_clear
    },
    {
        "on_add_remove_after_delete",
        Sparse_on_add_remove_after_delete
    },
    {
        "on_add_remove_after_fini",
        Sparse_on_add_remove_after_fini
    },
    {
        "on_set_after_set",
        Sparse_on_set_after_set
    },
    {
        "on_set_after_modified",
        Sparse_on_set_after_modified
    },
    {
        "sparse_relationship",
        Sparse_sparse_relationship
    },
    {
        "defer_ensure",
        Sparse_defer_ensure
    },
    {
        "defer_ensure_w_modified",
        Sparse_defer_ensure_w_modified
    },
    {
        "defer_ensure_modified",
        Sparse_defer_ensure_modified
    },
    {
        "defer_emplace",
        Sparse_defer_emplace
    },
    {
        "defer_emplace_w_modified",
        Sparse_defer_emplace_w_modified
    },
    {
        "defer_set",
        Sparse_defer_set
    },
    {
        "defer_ensure_existing",
        Sparse_defer_ensure_existing
    },
    {
        "defer_ensure_existing_twice",
        Sparse_defer_ensure_existing_twice
    },
    {
        "defer_ensure_w_modified_existing",
        Sparse_defer_ensure_w_modified_existing
    },
    {
        "defer_ensure_modified_existing",
        Sparse_defer_ensure_modified_existing
    },
    {
        "defer_emplace_existing",
        Sparse_defer_emplace_existing
    },
    {
        "defer_emplace_w_modified_existing",
        Sparse_defer_emplace_w_modified_existing
    },
    {
        "defer_set_existing",
        Sparse_defer_set_existing
    },
    {
        "defer_batched_ensure",
        Sparse_defer_batched_ensure
    },
    {
        "defer_batched_ensure_w_modified",
        Sparse_defer_batched_ensure_w_modified
    },
    {
        "defer_batched_ensure_modified",
        Sparse_defer_batched_ensure_modified
    },
    {
        "defer_batched_emplace",
        Sparse_defer_batched_emplace
    },
    {
        "defer_batched_emplace_w_modified",
        Sparse_defer_batched_emplace_w_modified
    },
    {
        "defer_batched_set",
        Sparse_defer_batched_set
    },
    {
        "defer_batched_ensure_existing",
        Sparse_defer_batched_ensure_existing
    },
    {
        "defer_batched_ensure_existing_twice",
        Sparse_defer_batched_ensure_existing_twice
    },
    {
        "defer_batched_ensure_w_modified_existing",
        Sparse_defer_batched_ensure_w_modified_existing
    },
    {
        "defer_batched_ensure_modified_existing",
        Sparse_defer_batched_ensure_modified_existing
    },
    {
        "defer_batched_emplace_existing",
        Sparse_defer_batched_emplace_existing
    },
    {
        "defer_batched_emplace_w_modified_existing",
        Sparse_defer_batched_emplace_w_modified_existing
    },
    {
        "defer_batched_set_existing",
        Sparse_defer_batched_set_existing
    },
    {
        "defer_batched_set_remove",
        Sparse_defer_batched_set_remove
    },
    {
        "defer_batched_set_remove_existing",
        Sparse_defer_batched_set_remove_existing
    }
};

bake_test_case Union_testcases[] = {
    {
        "add",
        Union_add
    },
    {
        "add_twice",
        Union_add_twice
    },
    {
        "add_replace",
        Union_add_replace
    },
    {
        "add_remove",
        Union_add_remove
    },
    {
        "add_remove_recycled",
        Union_add_remove_recycled
    },
    {
        "add_remove_add",
        Union_add_remove_add
    },
    {
        "get_target_none",
        Union_get_target_none
    },
    {
        "get_target",
        Union_get_target
    },
    {
        "get_recycled_target",
        Union_get_recycled_target
    },
    {
        "get_target_after_replace",
        Union_get_target_after_replace
    },
    {
        "get_target_after_remove",
        Union_get_target_after_remove
    },
    {
        "has_wildcard",
        Union_has_wildcard
    },
    {
        "has_any",
        Union_has_any
    },
    {
        "add_remove_2_tgts",
        Union_add_remove_2_tgts
    },
    {
        "add_remove_2_tgts_join",
        Union_add_remove_2_tgts_join
    },
    {
        "add_remove_3_tgts",
        Union_add_remove_3_tgts
    },
    {
        "add_remove_3_tgts_join",
        Union_add_remove_3_tgts_join
    },
    {
        "remove_w_union_tgt",
        Union_remove_w_union_tgt
    },
    {
        "get_non_union_tgt_from_table_w_union",
        Union_get_non_union_tgt_from_table_w_union
    },
    {
        "has_non_union_from_table_w_union",
        Union_has_non_union_from_table_w_union
    },
    {
        "get_case_no_switch",
        Union_get_case_no_switch
    },
    {
        "get_case_set",
        Union_get_case_set
    },
    {
        "get_case_change",
        Union_get_case_change
    },
    {
        "remove_case",
        Union_remove_case
    },
    {
        "remove_last",
        Union_remove_last
    },
    {
        "delete_first",
        Union_delete_first
    },
    {
        "delete_last",
        Union_delete_last
    },
    {
        "delete_first_last",
        Union_delete_first_last
    },
    {
        "3_entities_same_case",
        Union_3_entities_same_case
    },
    {
        "2_entities_1_change_case",
        Union_2_entities_1_change_case
    },
    {
        "3_entities_change_case",
        Union_3_entities_change_case
    },
    {
        "add_case_in_stage",
        Union_add_case_in_stage
    },
    {
        "change_case_in_stage",
        Union_change_case_in_stage
    },
    {
        "change_one_case_in_stage",
        Union_change_one_case_in_stage
    },
    {
        "remove_switch_in_stage",
        Union_remove_switch_in_stage
    },
    {
        "switch_no_match_for_case",
        Union_switch_no_match_for_case
    },
    {
        "empty_entity_has_case",
        Union_empty_entity_has_case
    },
    {
        "zero_entity_has_case",
        Union_zero_entity_has_case
    },
    {
        "add_to_entity_w_switch",
        Union_add_to_entity_w_switch
    },
    {
        "add_pair_to_entity_w_switch",
        Union_add_pair_to_entity_w_switch
    },
    {
        "recycled_tags",
        Union_recycled_tags
    },
    {
        "same_table_after_change",
        Union_same_table_after_change
    },
    {
        "add_2",
        Union_add_2
    },
    {
        "add_2_reverse",
        Union_add_2_reverse
    },
    {
        "add_switch_to_prefab_instance",
        Union_add_switch_to_prefab_instance
    },
    {
        "get_case_w_generation",
        Union_get_case_w_generation
    },
    {
        "get_case_w_generation_not_alive",
        Union_get_case_w_generation_not_alive
    },
    {
        "defer_add_union_relationship",
        Union_defer_add_union_relationship
    },
    {
        "defer_add_existing_union_relationship",
        Union_defer_add_existing_union_relationship
    },
    {
        "defer_add_union_relationship_2_ops",
        Union_defer_add_union_relationship_2_ops
    },
    {
        "defer_add_existing_union_relationship_2_ops",
        Union_defer_add_existing_union_relationship_2_ops
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
        "path_core",
        Hierarchies_path_core
    },
    {
        "path_core_w_prefix",
        Hierarchies_path_core_w_prefix
    },
    {
        "path_core_w_empty_prefix",
        Hierarchies_path_core_w_empty_prefix
    },
    {
        "path_this_w_empty_prefix",
        Hierarchies_path_this_w_empty_prefix
    },
    {
        "path_wildcard_w_empty_prefix",
        Hierarchies_path_wildcard_w_empty_prefix
    },
    {
        "path_any_w_empty_prefix",
        Hierarchies_path_any_w_empty_prefix
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
        "path_w_entity_id",
        Hierarchies_path_w_entity_id
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
        "lookup_entity_id",
        Hierarchies_lookup_entity_id
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
        "add_root_path_to_child",
        Hierarchies_add_root_path_to_child
    },
    {
        "add_parent_path_from_root_to_child",
        Hierarchies_add_parent_path_from_root_to_child
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
    },
    {
        "lookup_after_root_to_parent_move",
        Hierarchies_lookup_after_root_to_parent_move
    },
    {
        "lookup_after_parent_to_root_move",
        Hierarchies_lookup_after_parent_to_root_move
    },
    {
        "lookup_after_parent_to_parent_move",
        Hierarchies_lookup_after_parent_to_parent_move
    },
    {
        "lookup_after_clear_from_root",
        Hierarchies_lookup_after_clear_from_root
    },
    {
        "lookup_after_clear_from_parent",
        Hierarchies_lookup_after_clear_from_parent
    },
    {
        "lookup_after_delete_from_root",
        Hierarchies_lookup_after_delete_from_root
    },
    {
        "lookup_after_delete_from_parent",
        Hierarchies_lookup_after_delete_from_parent
    },
    {
        "defer_batch_remove_name_w_add_childof",
        Hierarchies_defer_batch_remove_name_w_add_childof
    },
    {
        "defer_batch_remove_childof_w_add_name",
        Hierarchies_defer_batch_remove_childof_w_add_name
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
        "owns_wildcard",
        Has_owns_wildcard
    },
    {
        "owns_wildcard_pair",
        Has_owns_wildcard_pair
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
    },
    {
        "count_disabled",
        Count_count_disabled
    },
    {
        "count_prefab",
        Count_count_prefab
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
    },
    {
        "ensure_equal_get",
        Get_component_ensure_equal_get
    },
    {
        "get_tag",
        Get_component_get_tag
    },
    {
        "get_pair_tag",
        Get_component_get_pair_tag
    },
    {
        "get_wildcard",
        Get_component_get_wildcard
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
        "get_ref_w_low_id_tag",
        Reference_get_ref_w_low_id_tag
    },
    {
        "get_ref_w_low_id_tag_after_add",
        Reference_get_ref_w_low_id_tag_after_add
    },
    {
        "get_nonexisting",
        Reference_get_nonexisting
    },
    {
        "aba_table",
        Reference_aba_table
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
        "delete_recycled_tag_again",
        Delete_delete_recycled_tag_again
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
        "delete_3rd_of_3",
        Delete_delete_3rd_of_3
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
        "flags",
        OnDelete_flags
    },
    {
        "id_default",
        OnDelete_id_default
    },
    {
        "id_remove",
        OnDelete_id_remove
    },
    {
        "id_delete",
        OnDelete_id_delete
    },
    {
        "relation_default",
        OnDelete_relation_default
    },
    {
        "relation_remove",
        OnDelete_relation_remove
    },
    {
        "relation_delete",
        OnDelete_relation_delete
    },
    {
        "object_default",
        OnDelete_object_default
    },
    {
        "object_remove",
        OnDelete_object_remove
    },
    {
        "object_delete",
        OnDelete_object_delete
    },
    {
        "id_throw",
        OnDelete_id_throw
    },
    {
        "relation_throw",
        OnDelete_relation_throw
    },
    {
        "object_throw",
        OnDelete_object_throw
    },
    {
        "object_mixed",
        OnDelete_object_mixed
    },
    {
        "id_remove_no_instances",
        OnDelete_id_remove_no_instances
    },
    {
        "id_delete_no_instances",
        OnDelete_id_delete_no_instances
    },
    {
        "id_throw_no_instances",
        OnDelete_id_throw_no_instances
    },
    {
        "cyclic_self",
        OnDelete_cyclic_self
    },
    {
        "nonempty_cyclic_self",
        OnDelete_nonempty_cyclic_self
    },
    {
        "cyclic_id_default",
        OnDelete_cyclic_id_default
    },
    {
        "cyclic_id_remove",
        OnDelete_cyclic_id_remove
    },
    {
        "cyclic_id_remove_both",
        OnDelete_cyclic_id_remove_both
    },
    {
        "cyclic_id_delete",
        OnDelete_cyclic_id_delete
    },
    {
        "cyclic_id_delete_both",
        OnDelete_cyclic_id_delete_both
    },
    {
        "cyclic_relation_default",
        OnDelete_cyclic_relation_default
    },
    {
        "cyclic_relation_remove",
        OnDelete_cyclic_relation_remove
    },
    {
        "cyclic_relation_remove_both",
        OnDelete_cyclic_relation_remove_both
    },
    {
        "cyclic_relation_delete",
        OnDelete_cyclic_relation_delete
    },
    {
        "cyclic_relation_delete_both",
        OnDelete_cyclic_relation_delete_both
    },
    {
        "cyclic_object_default",
        OnDelete_cyclic_object_default
    },
    {
        "cyclic_object_remove",
        OnDelete_cyclic_object_remove
    },
    {
        "cyclic_object_delete",
        OnDelete_cyclic_object_delete
    },
    {
        "cyclic_overlapping_table",
        OnDelete_cyclic_overlapping_table
    },
    {
        "cyclic_overlapping_new_tables",
        OnDelete_cyclic_overlapping_new_tables
    },
    {
        "cyclic_object_mixed",
        OnDelete_cyclic_object_mixed
    },
    {
        "cyclic_storage_table",
        OnDelete_cyclic_storage_table
    },
    {
        "cyclic_storage_table_2",
        OnDelete_cyclic_storage_table_2
    },
    {
        "cyclic_storage_table_3",
        OnDelete_cyclic_storage_table_3
    },
    {
        "cyclic_set_empty",
        OnDelete_cyclic_set_empty
    },
    {
        "2_acyclic_relations_w_cycle",
        OnDelete_2_acyclic_relations_w_cycle
    },
    {
        "remove_2_comps",
        OnDelete_remove_2_comps
    },
    {
        "remove_2_comps_to_existing_table",
        OnDelete_remove_2_comps_to_existing_table
    },
    {
        "delete_recursive",
        OnDelete_delete_recursive
    },
    {
        "component_throw",
        OnDelete_component_throw
    },
    {
        "remove_2_relations",
        OnDelete_remove_2_relations
    },
    {
        "remove_object_w_2_relations",
        OnDelete_remove_object_w_2_relations
    },
    {
        "remove_object_w_5_relations",
        OnDelete_remove_object_w_5_relations
    },
    {
        "remove_object_w_50_relations",
        OnDelete_remove_object_w_50_relations
    },
    {
        "remove_object_w_50_relations_3_tables",
        OnDelete_remove_object_w_50_relations_3_tables
    },
    {
        "remove_object_w_3_relations_interleaved",
        OnDelete_remove_object_w_3_relations_interleaved
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
        "empty_table_w_on_remove",
        OnDelete_empty_table_w_on_remove
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
        "remove_id_w_role",
        OnDelete_remove_id_w_role
    },
    {
        "remove_rel_w_override_pair",
        OnDelete_remove_rel_w_override_pair
    },
    {
        "remove_obj_w_override_pair",
        OnDelete_remove_obj_w_override_pair
    },
    {
        "remove_rel_w_override_pair_after_on_delete_target",
        OnDelete_remove_rel_w_override_pair_after_on_delete_target
    },
    {
        "remove_rel_w_override_pair_2_ids",
        OnDelete_remove_rel_w_override_pair_2_ids
    },
    {
        "remove_obj_w_override_pair_2_ids",
        OnDelete_remove_obj_w_override_pair_2_ids
    },
    {
        "remove_obj_w_override_pair_3_ids",
        OnDelete_remove_obj_w_override_pair_3_ids
    },
    {
        "remove_mixed_w_override_pair_3_ids",
        OnDelete_remove_mixed_w_override_pair_3_ids
    },
    {
        "merge_pair_component",
        OnDelete_merge_pair_component
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
        "delete_with_component_after_delete_cyclic_self",
        OnDelete_delete_with_component_after_delete_cyclic_self
    },
    {
        "delete_with_component_after_delete_cyclic",
        OnDelete_delete_with_component_after_delete_cyclic
    },
    {
        "delete_with_component_after_delete_cyclic_w_alive_moved",
        OnDelete_delete_with_component_after_delete_cyclic_w_alive_moved
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
    },
    {
        "deep_clean_64",
        OnDelete_deep_clean_64
    },
    {
        "deep_clean_256",
        OnDelete_deep_clean_256
    },
    {
        "id_w_disabled",
        OnDelete_id_w_disabled
    },
    {
        "id_to_no_disabled",
        OnDelete_id_to_no_disabled
    },
    {
        "remove_on_delete_action",
        OnDelete_remove_on_delete_action
    },
    {
        "delete_with_w_relation",
        OnDelete_delete_with_w_relation
    },
    {
        "delete_self_in_on_remove",
        OnDelete_delete_self_in_on_remove
    },
    {
        "delete_nested_in_on_remove",
        OnDelete_delete_nested_in_on_remove
    },
    {
        "add_deleted_in_on_remove",
        OnDelete_add_deleted_in_on_remove
    },
    {
        "delete_tree_w_query",
        OnDelete_delete_tree_w_query
    },
    {
        "fini_cleanup_order",
        OnDelete_fini_cleanup_order
    },
    {
        "fini_cleanup_order_root_id_w_trait",
        OnDelete_fini_cleanup_order_root_id_w_trait
    },
    {
        "fini_cleanup_order_entity_after_singleton",
        OnDelete_fini_cleanup_order_entity_after_singleton
    },
    {
        "fini_cleanup_order_entity_after_component",
        OnDelete_fini_cleanup_order_entity_after_component
    },
    {
        "on_delete_parent_w_in_use_id_w_remove",
        OnDelete_on_delete_parent_w_in_use_id_w_remove
    },
    {
        "on_delete_parent_w_in_use_id_w_delete",
        OnDelete_on_delete_parent_w_in_use_id_w_delete
    },
    {
        "create_after_delete_with",
        OnDelete_create_after_delete_with
    },
    {
        "delete_with_inherited_tag",
        OnDelete_delete_with_inherited_tag
    },
    {
        "delete_with_inherited_tag_w_query",
        OnDelete_delete_with_inherited_tag_w_query
    },
    {
        "delete_with_inherited_tag_w_observer",
        OnDelete_delete_with_inherited_tag_w_observer
    },
    {
        "delete_symmetric_relation",
        OnDelete_delete_symmetric_relation
    },
    {
        "delete_observed_symmetric_relation",
        OnDelete_delete_observed_symmetric_relation
    },
    {
        "nested_delete_with",
        OnDelete_nested_delete_with
    },
    {
        "deferred_delete_with_after_create_named",
        OnDelete_deferred_delete_with_after_create_named
    },
    {
        "deferred_delete_with_childof_after_create_named",
        OnDelete_deferred_delete_with_childof_after_create_named
    },
    {
        "match_marked_for_deletion",
        OnDelete_match_marked_for_deletion
    },
    {
        "delete_w_low_rel_mixed_cleanup",
        OnDelete_delete_w_low_rel_mixed_cleanup
    },
    {
        "delete_w_low_rel_mixed_cleanup_interleaved_ids",
        OnDelete_delete_w_low_rel_mixed_cleanup_interleaved_ids
    },
    {
        "fini_query_w_singleton_in_scope_no_module",
        OnDelete_fini_query_w_singleton_in_scope_no_module
    },
    {
        "fini_query_w_singleton_in_module",
        OnDelete_fini_query_w_singleton_in_module
    },
    {
        "fini_observer_w_relationship_in_scope",
        OnDelete_fini_observer_w_relationship_in_scope
    },
    {
        "add_on_delete_from_prefab",
        OnDelete_add_on_delete_from_prefab
    },
    {
        "add_on_delete_from_disabled",
        OnDelete_add_on_delete_from_disabled
    },
    {
        "delete_on_delete_from_prefab",
        OnDelete_delete_on_delete_from_prefab
    },
    {
        "delete_on_delete_from_disabled",
        OnDelete_delete_on_delete_from_disabled
    },
    {
        "remove_all_1",
        OnDelete_remove_all_1
    },
    {
        "remove_all_2",
        OnDelete_remove_all_2
    },
    {
        "remove_all_3",
        OnDelete_remove_all_3
    },
    {
        "delete_with_1",
        OnDelete_delete_with_1
    },
    {
        "delete_with_2",
        OnDelete_delete_with_2
    },
    {
        "delete_with_3",
        OnDelete_delete_with_3
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
        "ensure_new",
        Set_ensure_new
    },
    {
        "ensure_existing",
        Set_ensure_existing
    },
    {
        "ensure_tag_new",
        Set_ensure_tag_new
    },
    {
        "ensure_tag_existing",
        Set_ensure_tag_existing
    },
    {
        "ensure_tag_new_w_comp",
        Set_ensure_tag_new_w_comp
    },
    {
        "ensure_tag_existing_w_comp",
        Set_ensure_tag_existing_w_comp
    },
    {
        "ensure_tag_new_w_pair",
        Set_ensure_tag_new_w_pair
    },
    {
        "ensure_tag_existing_w_pair",
        Set_ensure_tag_existing_w_pair
    },
    {
        "get_mut_not_existing",
        Set_get_mut_not_existing
    },
    {
        "get_mut_existing",
        Set_get_mut_existing
    },
    {
        "get_mut_tag",
        Set_get_mut_tag
    },
    {
        "get_mut_pair",
        Set_get_mut_pair
    },
    {
        "get_mut_pair_second",
        Set_get_mut_pair_second
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
        "ensure_w_add_in_on_add",
        Set_ensure_w_add_in_on_add
    },
    {
        "ensure_w_remove_in_on_add",
        Set_ensure_w_remove_in_on_add
    },
    {
        "ensure_w_realloc_in_on_add",
        Set_ensure_w_realloc_in_on_add
    },
    {
        "emplace",
        Set_emplace
    },
    {
        "emplace_2",
        Set_emplace_2
    },
    {
        "emplace_existing",
        Set_emplace_existing
    },
    {
        "emplace_w_move",
        Set_emplace_w_move
    },
    {
        "emplace_w_observer_w_add",
        Set_emplace_w_observer_w_add
    },
    {
        "emplace_existing_w_check",
        Set_emplace_existing_w_check
    }
};

bake_test_case ReadWrite_testcases[] = {
    {
        "read",
        ReadWrite_read
    },
    {
        "nested_read",
        ReadWrite_nested_read
    },
    {
        "write",
        ReadWrite_write
    },
    {
        "nested_write",
        ReadWrite_nested_write
    },
    {
        "add_while_read",
        ReadWrite_add_while_read
    },
    {
        "add_while_write",
        ReadWrite_add_while_write
    },
    {
        "read_from_stage",
        ReadWrite_read_from_stage
    },
    {
        "write_from_stage",
        ReadWrite_write_from_stage
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
        "lookup_after_name_reset",
        Lookup_lookup_after_name_reset
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
        "lookup_path_anonymous_parent",
        Lookup_lookup_path_anonymous_parent
    },
    {
        "lookup_path_0_parent",
        Lookup_lookup_path_0_parent
    },
    {
        "lookup_path_0_parent_w_scope",
        Lookup_lookup_path_0_parent_w_scope
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
        "lookup_name_w_spaces",
        Lookup_lookup_name_w_spaces
    },
    {
        "lookup_path_w_spaces",
        Lookup_lookup_path_w_spaces
    },
    {
        "lookup_number",
        Lookup_lookup_number
    },
    {
        "lookup_number_path",
        Lookup_lookup_number_path
    },
    {
        "lookup_number_0",
        Lookup_lookup_number_0
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
        "lookup_variable",
        Lookup_lookup_variable
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
    },
    {
        "lookup_from_scope_staged",
        Lookup_lookup_from_scope_staged
    },
    {
        "lookup_core",
        Lookup_lookup_core
    },
    {
        "lookup_core_from_stage",
        Lookup_lookup_core_from_stage
    },
    {
        "lookup_custom_search_path",
        Lookup_lookup_custom_search_path
    },
    {
        "lookup_custom_search_path_from_stage",
        Lookup_lookup_custom_search_path_from_stage
    },
    {
        "lookup_custom_search_path_n_elems",
        Lookup_lookup_custom_search_path_n_elems
    },
    {
        "set_same_name",
        Lookup_set_same_name
    },
    {
        "set_same_name_after_reparenting",
        Lookup_set_same_name_after_reparenting
    },
    {
        "defer_set_name",
        Lookup_defer_set_name
    },
    {
        "defer_set_same_name",
        Lookup_defer_set_same_name
    },
    {
        "lookup_invalid_digit",
        Lookup_lookup_invalid_digit
    },
    {
        "lookup_child_invalid_digit",
        Lookup_lookup_child_invalid_digit
    },
    {
        "lookup_digit_from_wrong_scope",
        Lookup_lookup_digit_from_wrong_scope
    },
    {
        "lookup_core_entity_from_wrong_scope",
        Lookup_lookup_core_entity_from_wrong_scope
    },
    {
        "lookup_alias_w_number",
        Lookup_lookup_alias_w_number
    },
    {
        "lookup_symbol_path",
        Lookup_lookup_symbol_path
    },
    {
        "lookup_name_escaped_sep",
        Lookup_lookup_name_escaped_sep
    },
    {
        "lookup_path_escaped_sep",
        Lookup_lookup_path_escaped_sep
    },
    {
        "lookup_name_63_chars",
        Lookup_lookup_name_63_chars
    },
    {
        "lookup_name_64_chars",
        Lookup_lookup_name_64_chars
    },
    {
        "lookup_name_65_chars",
        Lookup_lookup_name_65_chars
    },
    {
        "lookup_path_63_chars",
        Lookup_lookup_path_63_chars
    },
    {
        "lookup_path_64_chars",
        Lookup_lookup_path_64_chars
    },
    {
        "lookup_path_65_chars",
        Lookup_lookup_path_65_chars
    }
};

bake_test_case Singleton_testcases[] = {
    {
        "add_singleton",
        Singleton_add_singleton
    },
    {
        "remove_singleton",
        Singleton_remove_singleton
    },
    {
        "set_get_singleton",
        Singleton_set_get_singleton
    },
    {
        "ensure_singleton",
        Singleton_ensure_singleton
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
    },
    {
        "clone_w_name",
        Clone_clone_w_name
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
        "ctor_on_add_pair_tag",
        ComponentLifecycle_ctor_on_add_pair_tag
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
        "on_add_on_bulk_new",
        ComponentLifecycle_on_add_on_bulk_new
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
        "ctor_w_emplace_defer",
        ComponentLifecycle_ctor_w_emplace_defer
    },
    {
        "ctor_w_emplace_defer_use_move_ctor",
        ComponentLifecycle_ctor_w_emplace_defer_use_move_ctor
    },
    {
        "ctor_w_emplace_defer_twice",
        ComponentLifecycle_ctor_w_emplace_defer_twice
    },
    {
        "ctor_w_emplace_defer_existing",
        ComponentLifecycle_ctor_w_emplace_defer_existing
    },
    {
        "on_add_w_emplace",
        ComponentLifecycle_on_add_w_emplace
    },
    {
        "on_add_w_emplace_existing",
        ComponentLifecycle_on_add_w_emplace_existing
    },
    {
        "on_add_w_emplace_defer",
        ComponentLifecycle_on_add_w_emplace_defer
    },
    {
        "merge_async_stage_w_emplace",
        ComponentLifecycle_merge_async_stage_w_emplace
    },
    {
        "merge_async_stage_w_emplace_to_deferred_world",
        ComponentLifecycle_merge_async_stage_w_emplace_to_deferred_world
    },
    {
        "emplace_grow_w_existing_component",
        ComponentLifecycle_emplace_grow_w_existing_component
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
        "delete_in_dtor_other_type_on_fini",
        ComponentLifecycle_delete_in_dtor_other_type_on_fini
    },
    {
        "delete_in_dtor_other_type_on_delete_parent",
        ComponentLifecycle_delete_in_dtor_other_type_on_delete_parent
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
    },
    {
        "on_add_after_new",
        ComponentLifecycle_on_add_after_new
    },
    {
        "on_add_after_add",
        ComponentLifecycle_on_add_after_add
    },
    {
        "on_add_after_set",
        ComponentLifecycle_on_add_after_set
    },
    {
        "on_remove_after_remove",
        ComponentLifecycle_on_remove_after_remove
    },
    {
        "on_remove_after_clear",
        ComponentLifecycle_on_remove_after_clear
    },
    {
        "on_remove_after_delete",
        ComponentLifecycle_on_remove_after_delete
    },
    {
        "free_component_new_id_while_fini",
        ComponentLifecycle_free_component_new_id_while_fini
    },
    {
        "dtor_component_new_id_while_fini",
        ComponentLifecycle_dtor_component_new_id_while_fini
    },
    {
        "free_component_new_pair_id_while_fini",
        ComponentLifecycle_free_component_new_pair_id_while_fini
    },
    {
        "dtor_component_new_pair_id_while_fini",
        ComponentLifecycle_dtor_component_new_pair_id_while_fini
    },
    {
        "free_component_new_obj_pair_id_while_fini",
        ComponentLifecycle_free_component_new_obj_pair_id_while_fini
    },
    {
        "dtor_component_new_obj_pair_id_while_fini",
        ComponentLifecycle_dtor_component_new_obj_pair_id_while_fini
    },
    {
        "ctor_move_dtor_after_resize",
        ComponentLifecycle_ctor_move_dtor_after_resize
    },
    {
        "ctx_free",
        ComponentLifecycle_ctx_free
    },
    {
        "binding_ctx_free",
        ComponentLifecycle_binding_ctx_free
    },
    {
        "ctx_free_after_delete_component",
        ComponentLifecycle_ctx_free_after_delete_component
    },
    {
        "binding_ctx_free_after_delete_component",
        ComponentLifecycle_binding_ctx_free_after_delete_component
    },
    {
        "on_add_ctx",
        ComponentLifecycle_on_add_ctx
    },
    {
        "on_remove_ctx",
        ComponentLifecycle_on_remove_ctx
    },
    {
        "on_set_ctx",
        ComponentLifecycle_on_set_ctx
    },
    {
        "on_add_w_existing_component",
        ComponentLifecycle_on_add_w_existing_component
    },
    {
        "on_remove_w_existing_component",
        ComponentLifecycle_on_remove_w_existing_component
    },
    {
        "component_init_set_hooks",
        ComponentLifecycle_component_init_set_hooks
    },
    {
        "component_init_name_from_type_info",
        ComponentLifecycle_component_init_name_from_type_info
    },
    {
        "component_init_scoped_name_from_type_info",
        ComponentLifecycle_component_init_scoped_name_from_type_info
    },
    {
        "component_init_w_recycled_id",
        ComponentLifecycle_component_init_w_recycled_id
    },
    {
        "component_init_w_recycled_non_component_id",
        ComponentLifecycle_component_init_w_recycled_non_component_id
    },
    {
        "on_add_after_ctor_w_add",
        ComponentLifecycle_on_add_after_ctor_w_add
    },
    {
        "on_add_after_ctor_w_add_to",
        ComponentLifecycle_on_add_after_ctor_w_add_to
    },
    {
        "with_before_hooks",
        ComponentLifecycle_with_before_hooks
    },
    {
        "with_component_on_add",
        ComponentLifecycle_with_component_on_add
    },
    {
        "move_ctor_on_move",
        ComponentLifecycle_move_ctor_on_move
    },
    {
        "ptr_to_self",
        ComponentLifecycle_ptr_to_self
    },
    {
        "ctor_move_dtor_from_move_ctor",
        ComponentLifecycle_ctor_move_dtor_from_move_ctor
    },
    {
        "on_add_hook_check_offset",
        ComponentLifecycle_on_add_hook_check_offset
    },
    {
        "on_remove_hook_check_offset",
        ComponentLifecycle_on_remove_hook_check_offset
    },
    {
        "on_set_hook_check_offset",
        ComponentLifecycle_on_set_hook_check_offset
    },
    {
        "on_set_hook_on_override",
        ComponentLifecycle_on_set_hook_on_override
    },
    {
        "on_set_hook_on_auto_override",
        ComponentLifecycle_on_set_hook_on_auto_override
    },
    {
        "batched_set_new_component_w_lifecycle",
        ComponentLifecycle_batched_set_new_component_w_lifecycle
    },
    {
        "batched_ensure_new_component_w_lifecycle",
        ComponentLifecycle_batched_ensure_new_component_w_lifecycle
    },
    {
        "on_nested_prefab_copy_test_invokes_copy_count",
        ComponentLifecycle_on_nested_prefab_copy_test_invokes_copy_count
    },
    {
        "no_move_no_move_ctor_with_move_dtor_with_ctor_move_dtor",
        ComponentLifecycle_no_move_no_move_ctor_with_move_dtor_with_ctor_move_dtor
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
        "typeid_from_pair_w_override",
        Pairs_typeid_from_pair_w_override
    },
    {
        "typeid_from_pair_w_toggle",
        Pairs_typeid_from_pair_w_toggle
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
        "get_1_target",
        Pairs_get_1_target
    },
    {
        "get_1_target_not_found",
        Pairs_get_1_target_not_found
    },
    {
        "get_n_targets",
        Pairs_get_n_targets
    },
    {
        "get_target_from_base",
        Pairs_get_target_from_base
    },
    {
        "get_target_from_2nd_base",
        Pairs_get_target_from_2nd_base
    },
    {
        "get_target_from_base_w_pair_on_instance",
        Pairs_get_target_from_base_w_pair_on_instance
    },
    {
        "get_childof_target_from_base",
        Pairs_get_childof_target_from_base
    },
    {
        "get_dontinherit_target_from_base",
        Pairs_get_dontinherit_target_from_base
    },
    {
        "get_target_for_id_from_self",
        Pairs_get_target_for_id_from_self
    },
    {
        "get_target_for_id_from_base",
        Pairs_get_target_for_id_from_base
    },
    {
        "get_target_for_id_from_nested_base",
        Pairs_get_target_for_id_from_nested_base
    },
    {
        "get_target_for_id_not_found",
        Pairs_get_target_for_id_not_found
    },
    {
        "get_target_for_wildcard_from_self",
        Pairs_get_target_for_wildcard_from_self
    },
    {
        "get_target_for_wildcard_from_base",
        Pairs_get_target_for_wildcard_from_base
    },
    {
        "get_target_for_wildcard_from_nested_base",
        Pairs_get_target_for_wildcard_from_nested_base
    },
    {
        "get_target_for_id_from_empty",
        Pairs_get_target_for_id_from_empty
    },
    {
        "get_target_for_id_from_empty_no_rel",
        Pairs_get_target_for_id_from_empty_no_rel
    },
    {
        "get_target_for_id_not_empty_not_found",
        Pairs_get_target_for_id_not_empty_not_found
    },
    {
        "get_target_for_id_from_stage",
        Pairs_get_target_for_id_from_stage
    },
    {
        "get_target_for_id_no_id",
        Pairs_get_target_for_id_no_id
    },
    {
        "get_parent",
        Pairs_get_parent
    },
    {
        "get_parent_from_root",
        Pairs_get_parent_from_root
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
        "add_remove_exclusive_property",
        Pairs_add_remove_exclusive_property
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
        "add_symmetric_recycled_relation",
        Pairs_add_symmetric_recycled_relation
    },
    {
        "symmetric_w_childof",
        Pairs_symmetric_w_childof
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
        "with_for_existing",
        Pairs_with_for_existing
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
    },
    {
        "has_wildcard_w_pair",
        Pairs_has_wildcard_w_pair
    },
    {
        "has_pair_wildcard_w_tag",
        Pairs_has_pair_wildcard_w_tag
    },
    {
        "oneof_self",
        Pairs_oneof_self
    },
    {
        "oneof_other",
        Pairs_oneof_other
    },
    {
        "oneof_self_constraint_violated",
        Pairs_oneof_self_constraint_violated
    },
    {
        "oneof_other_constraint_violated",
        Pairs_oneof_other_constraint_violated
    },
    {
        "oneof_other_rel_parent_constraint_violated",
        Pairs_oneof_other_rel_parent_constraint_violated
    },
    {
        "set_w_recycled_rel",
        Pairs_set_w_recycled_rel
    },
    {
        "set_w_recycled_tgt",
        Pairs_set_w_recycled_tgt
    },
    {
        "force_relationship_on_component",
        Pairs_force_relationship_on_component
    },
    {
        "force_relationship_on_target",
        Pairs_force_relationship_on_target
    },
    {
        "force_relationship_on_target_trait",
        Pairs_force_relationship_on_target_trait
    },
    {
        "force_relationship_on_relationship",
        Pairs_force_relationship_on_relationship
    },
    {
        "force_target_on_component",
        Pairs_force_target_on_component
    },
    {
        "force_target_on_relationship",
        Pairs_force_target_on_relationship
    },
    {
        "force_target_on_target",
        Pairs_force_target_on_target
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
        "on_add_any",
        Trigger_on_add_any
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
    },
    {
        "on_set_w_tag",
        Trigger_on_set_w_tag
    },
    {
        "create_triggers_in_trigger",
        Trigger_create_triggers_in_trigger
    },
    {
        "on_add_superset_w_component",
        Trigger_on_add_superset_w_component
    },
    {
        "on_set_superset_w_component",
        Trigger_on_set_superset_w_component
    },
    {
        "on_add_base_superset_w_owned",
        Trigger_on_add_base_superset_w_owned
    },
    {
        "on_add_base_self_superset_w_owned",
        Trigger_on_add_base_self_superset_w_owned
    },
    {
        "on_set_self_from_child_of_prefab",
        Trigger_on_set_self_from_child_of_prefab
    },
    {
        "on_set_self_superset_from_child_of_prefab",
        Trigger_on_set_self_superset_from_child_of_prefab
    },
    {
        "on_set_self_from_child_base_of_prefab",
        Trigger_on_set_self_from_child_base_of_prefab
    },
    {
        "on_set_self_superset_from_child_base_of_prefab",
        Trigger_on_set_self_superset_from_child_base_of_prefab
    },
    {
        "on_set_self_auto_override",
        Trigger_on_set_self_auto_override
    },
    {
        "on_set_self_superset_auto_override",
        Trigger_on_set_self_superset_auto_override
    },
    {
        "on_set_superset_auto_override",
        Trigger_on_set_superset_auto_override
    },
    {
        "not_only",
        Trigger_not_only
    },
    {
        "not_only_w_base",
        Trigger_not_only_w_base
    },
    {
        "not_only_w_base_no_match",
        Trigger_not_only_w_base_no_match
    },
    {
        "on_set_superset_after_filter_observer",
        Trigger_on_set_superset_after_filter_observer
    },
    {
        "on_set_superset_after_filter_observer_w_on_add",
        Trigger_on_set_superset_after_filter_observer_w_on_add
    },
    {
        "on_set_superset_after_filter_observer_w_on_add_isa_after_set",
        Trigger_on_set_superset_after_filter_observer_w_on_add_isa_after_set
    },
    {
        "on_set_superset_after_filter_observer_w_on_add_2",
        Trigger_on_set_superset_after_filter_observer_w_on_add_2
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
        "2_terms_w_on_set_value_self_up",
        Observer_2_terms_w_on_set_value_self_up
    },
    {
        "2_terms_w_on_remove_value_self_up",
        Observer_2_terms_w_on_remove_value_self_up
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
        "update_ctx",
        Observer_update_ctx
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
        "and_from_empty",
        Observer_and_from_empty
    },
    {
        "or_from_empty",
        Observer_or_from_empty
    },
    {
        "and_from_empty_w_tag",
        Observer_and_from_empty_w_tag
    },
    {
        "or_from_empty_w_tag",
        Observer_or_from_empty_w_tag
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
        "on_add_yield_existing_wildcard",
        Observer_on_add_yield_existing_wildcard
    },
    {
        "on_add_yield_existing_wildcard_multi",
        Observer_on_add_yield_existing_wildcard_multi
    },
    {
        "on_add_yield_existing_wildcard_multi_w_wildcard_pivot",
        Observer_on_add_yield_existing_wildcard_multi_w_wildcard_pivot
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
    },
    {
        "on_set_w_tag",
        Observer_on_set_w_tag
    },
    {
        "mixed_on_set_w_tag",
        Observer_mixed_on_set_w_tag
    },
    {
        "mixed_un_set_w_tag",
        Observer_mixed_un_set_w_tag
    },
    {
        "match_base_w_id_at_offset",
        Observer_match_base_w_id_at_offset
    },
    {
        "custom_run_action",
        Observer_custom_run_action
    },
    {
        "custom_run_action_2_terms",
        Observer_custom_run_action_2_terms
    },
    {
        "custom_run_action_w_iter_next_2_terms",
        Observer_custom_run_action_w_iter_next_2_terms
    },
    {
        "custom_run_action_w_field",
        Observer_custom_run_action_w_field
    },
    {
        "custom_run_action_w_2_fields",
        Observer_custom_run_action_w_2_fields
    },
    {
        "custom_run_w_yield_existing",
        Observer_custom_run_w_yield_existing
    },
    {
        "custom_run_w_yield_existing_1_field",
        Observer_custom_run_w_yield_existing_1_field
    },
    {
        "custom_run_w_yield_existing_1_field_w_callback",
        Observer_custom_run_w_yield_existing_1_field_w_callback
    },
    {
        "custom_run_w_yield_existing_2_fields",
        Observer_custom_run_w_yield_existing_2_fields
    },
    {
        "custom_run_w_yield_existing_2_fields_w_callback",
        Observer_custom_run_w_yield_existing_2_fields_w_callback
    },
    {
        "read_in_on_remove_after_add_other_w_not",
        Observer_read_in_on_remove_after_add_other_w_not
    },
    {
        "observer_w_short_notation",
        Observer_observer_w_short_notation
    },
    {
        "observer_w_filter_term",
        Observer_observer_w_filter_term
    },
    {
        "multi_observer_w_ctx_free",
        Observer_multi_observer_w_ctx_free
    },
    {
        "propagate_after_on_delete_clear_action",
        Observer_propagate_after_on_delete_clear_action
    },
    {
        "on_add_after_batch_w_exclusive_adds",
        Observer_on_add_after_batch_w_exclusive_adds
    },
    {
        "propagate_match_relationship_w_self_up",
        Observer_propagate_match_relationship_w_self_up
    },
    {
        "propagate_match_relationship_w_up",
        Observer_propagate_match_relationship_w_up
    },
    {
        "propagate_isa_of_parent_add",
        Observer_propagate_isa_of_parent_add
    },
    {
        "propagate_isa_of_parent_remove",
        Observer_propagate_isa_of_parent_remove
    },
    {
        "propagate_isa_of_parent_set",
        Observer_propagate_isa_of_parent_set
    },
    {
        "propagate_add_childof_of_parent",
        Observer_propagate_add_childof_of_parent
    },
    {
        "propagate_add_childof_of_parent_w_siblings",
        Observer_propagate_add_childof_of_parent_w_siblings
    },
    {
        "propagate_add_childof_w_parent_w_base",
        Observer_propagate_add_childof_w_parent_w_base
    },
    {
        "propagate_remove_childof_w_parent_w_base",
        Observer_propagate_remove_childof_w_parent_w_base
    },
    {
        "propagate_remove_childof_of_parent",
        Observer_propagate_remove_childof_of_parent
    },
    {
        "propagate_add_isa_of_parent",
        Observer_propagate_add_isa_of_parent
    },
    {
        "propagate_remove_isa_of_parent",
        Observer_propagate_remove_isa_of_parent
    },
    {
        "propagate_add_childof_of_base",
        Observer_propagate_add_childof_of_base
    },
    {
        "propagate_remove_childof_of_base",
        Observer_propagate_remove_childof_of_base
    },
    {
        "emit_for_parent_w_prefab_child_and_instance",
        Observer_emit_for_parent_w_prefab_child_and_instance
    },
    {
        "observer_w_2_fixed_src",
        Observer_observer_w_2_fixed_src
    },
    {
        "emit_for_recreated_id_after_remove_all",
        Observer_emit_for_recreated_id_after_remove_all
    },
    {
        "emit_for_recreated_id_after_remove_all_wildcard",
        Observer_emit_for_recreated_id_after_remove_all_wildcard
    },
    {
        "emit_for_recreated_id_after_delete_with",
        Observer_emit_for_recreated_id_after_delete_with
    },
    {
        "emit_for_recreated_id_after_delete_with_wildcard",
        Observer_emit_for_recreated_id_after_delete_with_wildcard
    },
    {
        "delete_observed_id",
        Observer_delete_observed_id
    },
    {
        "delete_observed_rel",
        Observer_delete_observed_rel
    },
    {
        "delete_observed_tgt",
        Observer_delete_observed_tgt
    },
    {
        "on_add_2_pairs_w_uni_observer",
        Observer_on_add_2_pairs_w_uni_observer
    },
    {
        "on_add_2_pairs_w_multi_observer",
        Observer_on_add_2_pairs_w_multi_observer
    },
    {
        "on_set_2_pairs_w_uni_observer",
        Observer_on_set_2_pairs_w_uni_observer
    },
    {
        "on_set_2_pairs_w_multi_observer",
        Observer_on_set_2_pairs_w_multi_observer
    },
    {
        "on_remove_target_from_base_at_offset",
        Observer_on_remove_target_from_base_at_offset
    },
    {
        "on_remove_target_component_from_base_at_offset",
        Observer_on_remove_target_component_from_base_at_offset
    },
    {
        "filter_observer_after_observer",
        Observer_filter_observer_after_observer
    },
    {
        "notify_after_defer_batched",
        Observer_notify_after_defer_batched
    },
    {
        "notify_after_defer_batched_2_entities_in_table",
        Observer_notify_after_defer_batched_2_entities_in_table
    },
    {
        "notify_after_defer_batched_2_entities_in_table_w_tgt",
        Observer_notify_after_defer_batched_2_entities_in_table_w_tgt
    },
    {
        "multi_observer_table_fill_w_singleton",
        Observer_multi_observer_table_fill_w_singleton
    },
    {
        "wildcard_propagate_w_other_table",
        Observer_wildcard_propagate_w_other_table
    },
    {
        "add_in_yield_existing",
        Observer_add_in_yield_existing
    },
    {
        "add_in_yield_existing_multi",
        Observer_add_in_yield_existing_multi
    },
    {
        "disable_observer",
        Observer_disable_observer
    },
    {
        "disable_observer_module",
        Observer_disable_observer_module
    },
    {
        "disable_observer_module_nested",
        Observer_disable_observer_module_nested
    },
    {
        "disable_observer_and_module",
        Observer_disable_observer_and_module
    },
    {
        "disable_multi_observer",
        Observer_disable_multi_observer
    },
    {
        "disable_multi_observer_module",
        Observer_disable_multi_observer_module
    },
    {
        "disable_multi_observer_module_nested",
        Observer_disable_multi_observer_module_nested
    },
    {
        "disable_multi_observer_and_module",
        Observer_disable_multi_observer_and_module
    },
    {
        "tag_w_on_set_and_on_add",
        Observer_tag_w_on_set_and_on_add
    },
    {
        "tag_w_on_set_and_on_add_reverse",
        Observer_tag_w_on_set_and_on_add_reverse
    },
    {
        "on_add_pair_w_rel_any",
        Observer_on_add_pair_w_rel_any
    },
    {
        "on_remove_pair_w_rel_any",
        Observer_on_remove_pair_w_rel_any
    },
    {
        "on_add_pair_w_tgt_any",
        Observer_on_add_pair_w_tgt_any
    },
    {
        "on_remove_pair_w_tgt_any",
        Observer_on_remove_pair_w_tgt_any
    },
    {
        "on_add_pair_w_any_any",
        Observer_on_add_pair_w_any_any
    },
    {
        "on_remove_pair_w_any_any",
        Observer_on_remove_pair_w_any_any
    },
    {
        "on_add_any",
        Observer_on_add_any
    },
    {
        "on_remove_any",
        Observer_on_remove_any
    },
    {
        "get_filter",
        Observer_get_filter
    },
    {
        "uni_observer_eval_count",
        Observer_uni_observer_eval_count
    },
    {
        "multi_observer_eval_count",
        Observer_multi_observer_eval_count
    },
    {
        "yield_existing_uni_no_this",
        Observer_yield_existing_uni_no_this
    },
    {
        "yield_existing_multi_no_this",
        Observer_yield_existing_multi_no_this
    },
    {
        "observer_no_id_in_scope",
        Observer_observer_no_id_in_scope
    },
    {
        "register_comp_in_emit_named_entity",
        Observer_register_comp_in_emit_named_entity
    },
    {
        "register_comp_w_macro_in_emit_named_entity",
        Observer_register_comp_w_macro_in_emit_named_entity
    },
    {
        "add_to_self_in_emit_entity",
        Observer_add_to_self_in_emit_entity
    },
    {
        "on_set_w_not_tag",
        Observer_on_set_w_not_tag
    },
    {
        "on_set_w_not_component",
        Observer_on_set_w_not_component
    },
    {
        "wildcard_event",
        Observer_wildcard_event
    },
    {
        "register_callback_after_run",
        Observer_register_callback_after_run
    },
    {
        "register_run_after_callback",
        Observer_register_run_after_callback
    },
    {
        "register_callback_after_run_ctx",
        Observer_register_callback_after_run_ctx
    },
    {
        "register_run_after_callback_ctx",
        Observer_register_run_after_callback_ctx
    },
    {
        "cache_test_1",
        Observer_cache_test_1
    },
    {
        "cache_test_2",
        Observer_cache_test_2
    },
    {
        "cache_test_3",
        Observer_cache_test_3
    },
    {
        "cache_test_4",
        Observer_cache_test_4
    },
    {
        "cache_test_5",
        Observer_cache_test_5
    },
    {
        "cache_test_6",
        Observer_cache_test_6
    },
    {
        "cache_test_7",
        Observer_cache_test_7
    },
    {
        "cache_test_8",
        Observer_cache_test_8
    },
    {
        "cache_test_9",
        Observer_cache_test_9
    },
    {
        "cache_test_10",
        Observer_cache_test_10
    },
    {
        "cache_test_11",
        Observer_cache_test_11
    },
    {
        "cache_test_12",
        Observer_cache_test_12
    },
    {
        "cache_test_13",
        Observer_cache_test_13
    },
    {
        "cache_test_14",
        Observer_cache_test_14
    },
    {
        "cache_test_15",
        Observer_cache_test_15
    },
    {
        "cache_test_16",
        Observer_cache_test_16
    }
};

bake_test_case ObserverOnSet_testcases[] = {
    {
        "set_1_of_1",
        ObserverOnSet_set_1_of_1
    },
    {
        "set_1_of_2",
        ObserverOnSet_set_1_of_2
    },
    {
        "set_1_of_3",
        ObserverOnSet_set_1_of_3
    },
    {
        "set_1_of_2_1_from_base",
        ObserverOnSet_set_1_of_2_1_from_base
    },
    {
        "set_1_of_3_1_from_base",
        ObserverOnSet_set_1_of_3_1_from_base
    },
    {
        "add_base",
        ObserverOnSet_add_base
    },
    {
        "add_base_to_1_overridden",
        ObserverOnSet_add_base_to_1_overridden
    },
    {
        "add_base_to_2_overridden",
        ObserverOnSet_add_base_to_2_overridden
    },
    {
        "add_base_to_1_of_2_overridden",
        ObserverOnSet_add_base_to_1_of_2_overridden
    },
    {
        "on_set_after_remove_override",
        ObserverOnSet_on_set_after_remove_override
    },
    {
        "no_set_after_remove_base",
        ObserverOnSet_no_set_after_remove_base
    },
    {
        "un_set_after_remove",
        ObserverOnSet_un_set_after_remove
    },
    {
        "un_set_after_remove_base",
        ObserverOnSet_un_set_after_remove_base
    },
    {
        "add_to_current_in_on_set",
        ObserverOnSet_add_to_current_in_on_set
    },
    {
        "remove_from_current_in_on_set",
        ObserverOnSet_remove_from_current_in_on_set
    },
    {
        "remove_set_component_in_on_set",
        ObserverOnSet_remove_set_component_in_on_set
    },
    {
        "match_table_created_w_add_in_on_set",
        ObserverOnSet_match_table_created_w_add_in_on_set
    },
    {
        "set_optional",
        ObserverOnSet_set_optional
    },
    {
        "set_from_nothing",
        ObserverOnSet_set_from_nothing
    },
    {
        "add_0_entity_in_on_set",
        ObserverOnSet_add_0_entity_in_on_set
    },
    {
        "on_set_prefab",
        ObserverOnSet_on_set_prefab
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
        "set_after_add_in_on_add",
        TriggerOnAdd_set_after_add_in_on_add
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
        "on_remove_in_on_update",
        TriggerOnRemove_on_remove_in_on_update
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
        "on_set_after_remove_override",
        TriggerOnSet_on_set_after_remove_override
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
    },
    {
        "monitor_at_fini",
        Monitor_monitor_at_fini
    },
    {
        "monitor_other_table",
        Monitor_monitor_other_table
    },
    {
        "monitor_component",
        Monitor_monitor_component
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
        "inherit_disabled",
        Prefab_inherit_disabled
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
        "auto_override",
        Prefab_auto_override
    },
    {
        "auto_override_2",
        Prefab_auto_override_2
    },
    {
        "auto_override_nested",
        Prefab_auto_override_nested
    },
    {
        "auto_override_pair",
        Prefab_auto_override_pair
    },
    {
        "auto_override_pair_w_component",
        Prefab_auto_override_pair_w_component
    },
    {
        "auto_override_2_pairs",
        Prefab_auto_override_2_pairs
    },
    {
        "auto_override_2_pairs_w_component",
        Prefab_auto_override_2_pairs_w_component
    },
    {
        "auto_override_2_pairs_same_obj",
        Prefab_auto_override_2_pairs_same_obj
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
    },
    {
        "override_dont_inherit",
        Prefab_override_dont_inherit
    },
    {
        "prefab_child_w_dont_inherit_component",
        Prefab_prefab_child_w_dont_inherit_component
    },
    {
        "prefab_child_override",
        Prefab_prefab_child_override
    },
    {
        "prefab_child_override_w_exclusive_pair",
        Prefab_prefab_child_override_w_exclusive_pair
    },
    {
        "prefab_1_slot",
        Prefab_prefab_1_slot
    },
    {
        "prefab_2_slots",
        Prefab_prefab_2_slots
    },
    {
        "prefab_w_nested_slot",
        Prefab_prefab_w_nested_slot
    },
    {
        "prefab_w_mixed_slots",
        Prefab_prefab_w_mixed_slots
    },
    {
        "prefab_variant_w_slot",
        Prefab_prefab_variant_w_slot
    },
    {
        "prefab_variant_w_base_slot",
        Prefab_prefab_variant_w_base_slot
    },
    {
        "prefab_variant_w_mixed_slots",
        Prefab_prefab_variant_w_mixed_slots
    },
    {
        "override_slot",
        Prefab_override_slot
    },
    {
        "2_instances_w_slots_same_table",
        Prefab_2_instances_w_slots_same_table
    },
    {
        "slot_has_union",
        Prefab_slot_has_union
    },
    {
        "slot_override",
        Prefab_slot_override
    },
    {
        "base_slot_override",
        Prefab_base_slot_override
    },
    {
        "override_twice_w_add",
        Prefab_override_twice_w_add
    },
    {
        "override_twice_w_set",
        Prefab_override_twice_w_set
    },
    {
        "auto_override_copy_once",
        Prefab_auto_override_copy_once
    },
    {
        "always_override",
        Prefab_always_override
    },
    {
        "always_override_pair",
        Prefab_always_override_pair
    },
    {
        "child_of_prefab_is_prefab",
        Prefab_child_of_prefab_is_prefab
    },
    {
        "child_of_prefab_w_prefab_is_prefab",
        Prefab_child_of_prefab_w_prefab_is_prefab
    },
    {
        "child_of_prefab_w_prefab_is_prefab_w_component",
        Prefab_child_of_prefab_w_prefab_is_prefab_w_component
    },
    {
        "override_exclusive",
        Prefab_override_exclusive
    },
    {
        "override_exclusive_2_lvls",
        Prefab_override_exclusive_2_lvls
    },
    {
        "hierarchy_w_recycled_id",
        Prefab_hierarchy_w_recycled_id
    },
    {
        "disable_ids",
        Prefab_disable_ids
    },
    {
        "disable_nested_ids",
        Prefab_disable_nested_ids
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
    },
    {
        "delete_empty_tables_after_mini",
        World_delete_empty_tables_after_mini
    },
    {
        "delete_empty_tables_after_init",
        World_delete_empty_tables_after_init
    },
    {
        "delete_1000_empty_tables",
        World_delete_1000_empty_tables
    },
    {
        "delete_empty_tables_for_id",
        World_delete_empty_tables_for_id
    },
    {
        "use_after_delete_empty",
        World_use_after_delete_empty
    },
    {
        "use_after_clear_empty",
        World_use_after_clear_empty
    },
    {
        "use_after_delete_empty_w_component",
        World_use_after_delete_empty_w_component
    },
    {
        "use_after_clear_empty_w_component",
        World_use_after_clear_empty_w_component
    },
    {
        "use_after_clear_empty_w_component_w_lifecycle",
        World_use_after_clear_empty_w_component_w_lifecycle
    },
    {
        "use_after_clear_unused",
        World_use_after_clear_unused
    },
    {
        "ensure_in_at_fini",
        World_ensure_in_at_fini
    },
    {
        "get_type_info",
        World_get_type_info
    },
    {
        "get_type_info_after_delete_with",
        World_get_type_info_after_delete_with
    },
    {
        "get_type_info_after_reuse",
        World_get_type_info_after_reuse
    },
    {
        "no_name_prefix_after_init",
        World_no_name_prefix_after_init
    },
    {
        "component_init_w_name_prefix",
        World_component_init_w_name_prefix
    },
    {
        "component_macro_w_name_prefix",
        World_component_macro_w_name_prefix
    },
    {
        "set_get_context",
        World_set_get_context
    },
    {
        "set_get_binding_context",
        World_set_get_binding_context
    },
    {
        "set_get_context_w_free",
        World_set_get_context_w_free
    },
    {
        "set_get_binding_context_w_free",
        World_set_get_binding_context_w_free
    },
    {
        "get_entities",
        World_get_entities
    }
};

bake_test_case WorldInfo_testcases[] = {
    {
        "get_tick",
        WorldInfo_get_tick
    },
    {
        "table_count",
        WorldInfo_table_count
    },
    {
        "empty_table_count",
        WorldInfo_empty_table_count
    },
    {
        "table_create_count",
        WorldInfo_table_create_count
    },
    {
        "table_delete_count",
        WorldInfo_table_delete_count
    },
    {
        "id_tag_component_count",
        WorldInfo_id_tag_component_count
    },
    {
        "id_pair_count",
        WorldInfo_id_pair_count
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
        "invalid_entity_type_expression",
        Type_invalid_entity_type_expression
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
        "type_to_expr_pair_w_override",
        Type_type_to_expr_pair_w_override
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
        "entity_str_small_buffer",
        Type_entity_str_small_buffer
    },
    {
        "role_pair_str",
        Type_role_pair_str
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

bake_test_case Commands_testcases[] = {
    {
        "defer_new",
        Commands_defer_new
    },
    {
        "defer_bulk_new",
        Commands_defer_bulk_new
    },
    {
        "defer_add",
        Commands_defer_add
    },
    {
        "defer_add_two",
        Commands_defer_add_two
    },
    {
        "defer_remove",
        Commands_defer_remove
    },
    {
        "defer_remove_two",
        Commands_defer_remove_two
    },
    {
        "defer_set",
        Commands_defer_set
    },
    {
        "defer_delete",
        Commands_defer_delete
    },
    {
        "defer_twice",
        Commands_defer_twice
    },
    {
        "defer_twice_in_progress",
        Commands_defer_twice_in_progress
    },
    {
        "run_w_defer",
        Commands_run_w_defer
    },
    {
        "system_in_progress_w_defer",
        Commands_system_in_progress_w_defer
    },
    {
        "defer_ensure",
        Commands_defer_ensure
    },
    {
        "defer_ensure_no_modify",
        Commands_defer_ensure_no_modify
    },
    {
        "defer_ensure_w_modify",
        Commands_defer_ensure_w_modify
    },
    {
        "defer_modify",
        Commands_defer_modify
    },
    {
        "defer_set_pair",
        Commands_defer_set_pair
    },
    {
        "defer_clear",
        Commands_defer_clear
    },
    {
        "defer_add_after_delete",
        Commands_defer_add_after_delete
    },
    {
        "defer_set_after_delete",
        Commands_defer_set_after_delete
    },
    {
        "defer_ensure_after_delete",
        Commands_defer_ensure_after_delete
    },
    {
        "defer_ensure_after_delete_2nd_to_last",
        Commands_defer_ensure_after_delete_2nd_to_last
    },
    {
        "defer_add_child_to_deleted_parent",
        Commands_defer_add_child_to_deleted_parent
    },
    {
        "recreate_deleted_entity_while_deferred",
        Commands_recreate_deleted_entity_while_deferred
    },
    {
        "defer_add_to_recycled_id",
        Commands_defer_add_to_recycled_id
    },
    {
        "defer_add_to_recycled_id_w_role",
        Commands_defer_add_to_recycled_id_w_role
    },
    {
        "defer_add_to_recycled_relation",
        Commands_defer_add_to_recycled_relation
    },
    {
        "defer_add_to_recycled_object",
        Commands_defer_add_to_recycled_object
    },
    {
        "defer_add_to_recycled_object_childof",
        Commands_defer_add_to_recycled_object_childof
    },
    {
        "defer_add_to_deleted_id",
        Commands_defer_add_to_deleted_id
    },
    {
        "defer_add_to_deleted_id_w_role",
        Commands_defer_add_to_deleted_id_w_role
    },
    {
        "defer_add_to_deleted_relation",
        Commands_defer_add_to_deleted_relation
    },
    {
        "defer_add_to_deleted_object",
        Commands_defer_add_to_deleted_object
    },
    {
        "defer_add_to_deleted_object_childof",
        Commands_defer_add_to_deleted_object_childof
    },
    {
        "defer_delete_added_id",
        Commands_defer_delete_added_id
    },
    {
        "defer_delete_added_id_w_role",
        Commands_defer_delete_added_id_w_role
    },
    {
        "defer_delete_added_relation",
        Commands_defer_delete_added_relation
    },
    {
        "defer_delete_added_object",
        Commands_defer_delete_added_object
    },
    {
        "defer_delete_added_object_childof",
        Commands_defer_delete_added_object_childof
    },
    {
        "discard_add",
        Commands_discard_add
    },
    {
        "discard_remove",
        Commands_discard_remove
    },
    {
        "discard_add_two",
        Commands_discard_add_two
    },
    {
        "discard_remove_two",
        Commands_discard_remove_two
    },
    {
        "discard_child",
        Commands_discard_child
    },
    {
        "discard_child_w_add",
        Commands_discard_child_w_add
    },
    {
        "defer_return_value",
        Commands_defer_return_value
    },
    {
        "defer_ensure_pair",
        Commands_defer_ensure_pair
    },
    {
        "async_stage_add",
        Commands_async_stage_add
    },
    {
        "async_stage_add_twice",
        Commands_async_stage_add_twice
    },
    {
        "async_stage_remove",
        Commands_async_stage_remove
    },
    {
        "async_stage_clear",
        Commands_async_stage_clear
    },
    {
        "async_stage_delete",
        Commands_async_stage_delete
    },
    {
        "async_stage_new",
        Commands_async_stage_new
    },
    {
        "async_stage_readonly",
        Commands_async_stage_readonly
    },
    {
        "register_component_while_in_progress",
        Commands_register_component_while_in_progress
    },
    {
        "register_component_while_staged",
        Commands_register_component_while_staged
    },
    {
        "register_component_while_deferred",
        Commands_register_component_while_deferred
    },
    {
        "defer_enable",
        Commands_defer_enable
    },
    {
        "defer_disable",
        Commands_defer_disable
    },
    {
        "defer_enable_from_stage",
        Commands_defer_enable_from_stage
    },
    {
        "defer_toggle",
        Commands_defer_toggle
    },
    {
        "defer_toggle_from_stage",
        Commands_defer_toggle_from_stage
    },
    {
        "defer_delete_with",
        Commands_defer_delete_with
    },
    {
        "defer_remove_all",
        Commands_defer_remove_all
    },
    {
        "deferred_modified_after_remove",
        Commands_deferred_modified_after_remove
    },
    {
        "merge_cleanup_ops_before_delete",
        Commands_merge_cleanup_ops_before_delete
    },
    {
        "merge_nested_cleanup_ops_before_delete",
        Commands_merge_nested_cleanup_ops_before_delete
    },
    {
        "defer_suspend_resume",
        Commands_defer_suspend_resume
    },
    {
        "create_observer_while_deferred",
        Commands_create_observer_while_deferred
    },
    {
        "create_query_while_deferred",
        Commands_create_query_while_deferred
    },
    {
        "update_observer_while_deferred",
        Commands_update_observer_while_deferred
    },
    {
        "defer_set_large_component",
        Commands_defer_set_large_component
    },
    {
        "defer_while_suspend_readonly",
        Commands_defer_while_suspend_readonly
    },
    {
        "defer_while_suspend_readonly_w_existing_commands",
        Commands_defer_while_suspend_readonly_w_existing_commands
    },
    {
        "defer_remove_after_set",
        Commands_defer_remove_after_set
    },
    {
        "defer_remove_after_set_w_observer",
        Commands_defer_remove_after_set_w_observer
    },
    {
        "defer_override_after_remove",
        Commands_defer_override_after_remove
    },
    {
        "defer_override_after_remove_3_ops",
        Commands_defer_override_after_remove_3_ops
    },
    {
        "flush_stage_to_deferred_world",
        Commands_flush_stage_to_deferred_world
    },
    {
        "add_in_observer_during_merge",
        Commands_add_in_observer_during_merge
    },
    {
        "add_in_observer_during_merge_2_commands",
        Commands_add_in_observer_during_merge_2_commands
    },
    {
        "add_2_in_observer_while_on_remove_for_delete",
        Commands_add_2_in_observer_while_on_remove_for_delete
    },
    {
        "add_2_in_observer_while_on_remove_for_delete_child",
        Commands_add_2_in_observer_while_on_remove_for_delete_child
    },
    {
        "add_2_in_observer_while_on_remove_for_delete_recycled_id",
        Commands_add_2_in_observer_while_on_remove_for_delete_recycled_id
    },
    {
        "add_2_in_observer_while_on_remove_for_deferred_delete_recycled_id",
        Commands_add_2_in_observer_while_on_remove_for_deferred_delete_recycled_id
    },
    {
        "defer_add_after_clear",
        Commands_defer_add_after_clear
    },
    {
        "defer_cmd_after_modified",
        Commands_defer_cmd_after_modified
    },
    {
        "defer_remove_after_emplace_different_id",
        Commands_defer_remove_after_emplace_different_id
    },
    {
        "defer_remove_after_set_and_emplace_different_id",
        Commands_defer_remove_after_set_and_emplace_different_id
    },
    {
        "clear_after_add_to_nonempty",
        Commands_clear_after_add_to_nonempty
    },
    {
        "remove_after_add_to_nonempty",
        Commands_remove_after_add_to_nonempty
    },
    {
        "register_while_deferred_with_n_stages",
        Commands_register_while_deferred_with_n_stages
    },
    {
        "defer_2_sets_w_multi_observer",
        Commands_defer_2_sets_w_multi_observer
    },
    {
        "defer_2_ensures_w_multi_observer",
        Commands_defer_2_ensures_w_multi_observer
    },
    {
        "defer_2_ensures_no_modified_w_multi_observer",
        Commands_defer_2_ensures_no_modified_w_multi_observer
    },
    {
        "exists_remove_set",
        Commands_exists_remove_set
    },
    {
        "absent_remove_set",
        Commands_absent_remove_set
    },
    {
        "exists_set_remove",
        Commands_exists_set_remove
    },
    {
        "absent_set_remove",
        Commands_absent_set_remove
    },
    {
        "exists_remove_ensure",
        Commands_exists_remove_ensure
    },
    {
        "absent_remove_ensure",
        Commands_absent_remove_ensure
    },
    {
        "exists_ensure_remove",
        Commands_exists_ensure_remove
    },
    {
        "absent_ensure_remove",
        Commands_absent_ensure_remove
    },
    {
        "exists_set_w_ensure",
        Commands_exists_set_w_ensure
    },
    {
        "absent_set_invoke_on_set",
        Commands_absent_set_invoke_on_set
    },
    {
        "exists_set_invoke_on_set",
        Commands_exists_set_invoke_on_set
    },
    {
        "defer_ensure_no_on_set",
        Commands_defer_ensure_no_on_set
    },
    {
        "defer_existing_ensure_no_on_set",
        Commands_defer_existing_ensure_no_on_set
    },
    {
        "ensure_override",
        Commands_ensure_override
    },
    {
        "set_override",
        Commands_set_override
    },
    {
        "absent_ensure_for_entity_w_tag",
        Commands_absent_ensure_for_entity_w_tag
    },
    {
        "on_set_hook_before_on_add_for_existing_component",
        Commands_on_set_hook_before_on_add_for_existing_component
    },
    {
        "defer_2_sets_w_observer_same_component",
        Commands_defer_2_sets_w_observer_same_component
    },
    {
        "defer_2_sets_w_observer_other_component",
        Commands_defer_2_sets_w_observer_other_component
    },
    {
        "on_remove_after_deferred_clear_and_add",
        Commands_on_remove_after_deferred_clear_and_add
    },
    {
        "defer_delete_recycle_same_id",
        Commands_defer_delete_recycle_same_id
    },
    {
        "observer_while_defer_suspended",
        Commands_observer_while_defer_suspended
    },
    {
        "on_add_hook_while_defer_suspended",
        Commands_on_add_hook_while_defer_suspended
    },
    {
        "on_set_hook_while_defer_suspended",
        Commands_on_set_hook_while_defer_suspended
    },
    {
        "on_remove_hook_while_defer_suspended",
        Commands_on_remove_hook_while_defer_suspended
    },
    {
        "on_set_hook_batched_is_deferred",
        Commands_on_set_hook_batched_is_deferred
    },
    {
        "add_path",
        Commands_add_path
    },
    {
        "add_path_to_deleted_parent",
        Commands_add_path_to_deleted_parent
    },
    {
        "add_path_nested",
        Commands_add_path_nested
    },
    {
        "add_path_nested_to_deleted_parent",
        Commands_add_path_nested_to_deleted_parent
    },
    {
        "add_path_nested_to_created_deleted_parent",
        Commands_add_path_nested_to_created_deleted_parent
    },
    {
        "add_path_w_stage",
        Commands_add_path_w_stage
    },
    {
        "add_path_to_deleted_parent_w_stage",
        Commands_add_path_to_deleted_parent_w_stage
    },
    {
        "add_path_nested_w_stage",
        Commands_add_path_nested_w_stage
    },
    {
        "add_path_nested_to_deleted_parent_w_stage",
        Commands_add_path_nested_to_deleted_parent_w_stage
    },
    {
        "add_path_nested_to_created_deleted_parent_w_stage",
        Commands_add_path_nested_to_created_deleted_parent_w_stage
    },
    {
        "defer_emplace_w_arg",
        Commands_defer_emplace_w_arg
    },
    {
        "defer_emplace_existing_w_arg",
        Commands_defer_emplace_existing_w_arg
    },
    {
        "mixed_on_add_on_set_w_set_w_batching",
        Commands_mixed_on_add_on_set_w_set_w_batching
    },
    {
        "mixed_on_add_on_set_w_emplace",
        Commands_mixed_on_add_on_set_w_emplace
    },
    {
        "add_isa_set_w_override_batched",
        Commands_add_isa_set_w_override_batched
    },
    {
        "add_set_isa_w_override_batched",
        Commands_add_set_isa_w_override_batched
    },
    {
        "add_batched_set_with",
        Commands_add_batched_set_with
    },
    {
        "defer_emplace_after_remove",
        Commands_defer_emplace_after_remove
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
        "ensureable",
        SingleThreadStaging_ensureable
    },
    {
        "ensureable_from_main",
        SingleThreadStaging_ensureable_from_main
    },
    {
        "ensureable_w_add",
        SingleThreadStaging_ensureable_w_add
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
        "get_object_from_stage",
        SingleThreadStaging_get_object_from_stage
    },
    {
        "add_to_world_while_readonly",
        SingleThreadStaging_add_to_world_while_readonly
    },
    {
        "add_to_world_and_stage_while_readonly",
        SingleThreadStaging_add_to_world_and_stage_while_readonly
    },
    {
        "lookup_after_stage_count_change",
        SingleThreadStaging_lookup_after_stage_count_change
    },
    {
        "lookup_w_scope_after_stage_count_change",
        SingleThreadStaging_lookup_w_scope_after_stage_count_change
    },
    {
        "with_after_stage_count_change",
        SingleThreadStaging_with_after_stage_count_change
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
    }
};

bake_test_case Table_testcases[] = {
    {
        "get_index",
        Table_get_index
    },
    {
        "get_index_after_tag",
        Table_get_index_after_tag
    },
    {
        "get_index_not_in_table",
        Table_get_index_not_in_table
    },
    {
        "get_index_tag",
        Table_get_index_tag
    },
    {
        "get_index_pair",
        Table_get_index_pair
    },
    {
        "get_index_pair_tag",
        Table_get_index_pair_tag
    },
    {
        "get_column",
        Table_get_column
    },
    {
        "get_column_for_tag",
        Table_get_column_for_tag
    },
    {
        "get_column_for_low_tag",
        Table_get_column_for_low_tag
    },
    {
        "get_column_for_high_component",
        Table_get_column_for_high_component
    },
    {
        "get_column_for_component_after_tag",
        Table_get_column_for_component_after_tag
    },
    {
        "get_column_w_offset",
        Table_get_column_w_offset
    },
    {
        "get_id",
        Table_get_id
    },
    {
        "get_component",
        Table_get_component
    },
    {
        "get_pair",
        Table_get_pair
    },
    {
        "get_from_stage",
        Table_get_from_stage
    },
    {
        "get_depth",
        Table_get_depth
    },
    {
        "get_depth_non_acyclic",
        Table_get_depth_non_acyclic
    },
    {
        "get_depth_2_paths",
        Table_get_depth_2_paths
    },
    {
        "get_column_size",
        Table_get_column_size
    }
};

bake_test_case Poly_testcases[] = {
    {
        "on_set_poly_observer",
        Poly_on_set_poly_observer
    },
    {
        "on_set_poly_query",
        Poly_on_set_poly_query
    },
    {
        "on_set_poly_system",
        Poly_on_set_poly_system
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
    },
    {
        "records_resize_on_override",
        Internals_records_resize_on_override
    },
    {
        "table_observed_after_add",
        Internals_table_observed_after_add
    },
    {
        "table_observed_after_remove",
        Internals_table_observed_after_remove
    },
    {
        "table_observed_after_clear",
        Internals_table_observed_after_clear
    },
    {
        "table_observed_after_delete",
        Internals_table_observed_after_delete
    },
    {
        "table_observed_after_on_remove",
        Internals_table_observed_after_on_remove
    },
    {
        "table_observed_after_entity_flag",
        Internals_table_observed_after_entity_flag
    },
    {
        "table_create_leak_check",
        Internals_table_create_leak_check
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

bake_test_case StackAlloc_testcases[] = {
    {
        "init_fini",
        StackAlloc_init_fini
    },
    {
        "multiple_overlapping_cursors",
        StackAlloc_multiple_overlapping_cursors
    }
};


static bake_test_suite suites[] = {
    {
        "Id",
        NULL,
        NULL,
        37,
        Id_testcases
    },
    {
        "Entity",
        NULL,
        NULL,
        138,
        Entity_testcases
    },
    {
        "Each",
        NULL,
        NULL,
        5,
        Each_testcases
    },
    {
        "Iter",
        NULL,
        NULL,
        58,
        Iter_testcases
    },
    {
        "Search",
        NULL,
        NULL,
        20,
        Search_testcases
    },
    {
        "Event",
        NULL,
        NULL,
        35,
        Event_testcases
    },
    {
        "New",
        New_setup,
        NULL,
        24,
        New_testcases
    },
    {
        "New_w_Count",
        NULL,
        NULL,
        20,
        New_w_Count_testcases
    },
    {
        "Add",
        NULL,
        NULL,
        26,
        Add_testcases
    },
    {
        "Remove",
        NULL,
        NULL,
        10,
        Remove_testcases
    },
    {
        "GlobalComponentIds",
        NULL,
        NULL,
        7,
        GlobalComponentIds_testcases
    },
    {
        "Sparse",
        NULL,
        NULL,
        70,
        Sparse_testcases
    },
    {
        "Union",
        NULL,
        NULL,
        51,
        Union_testcases
    },
    {
        "Hierarchies",
        Hierarchies_setup,
        NULL,
        95,
        Hierarchies_testcases
    },
    {
        "Has",
        NULL,
        NULL,
        18,
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
        Get_component_setup,
        NULL,
        14,
        Get_component_testcases
    },
    {
        "Reference",
        Reference_setup,
        NULL,
        13,
        Reference_testcases
    },
    {
        "Delete",
        Delete_setup,
        NULL,
        32,
        Delete_testcases
    },
    {
        "OnDelete",
        NULL,
        NULL,
        124,
        OnDelete_testcases
    },
    {
        "Set",
        NULL,
        NULL,
        37,
        Set_testcases
    },
    {
        "ReadWrite",
        NULL,
        NULL,
        8,
        ReadWrite_testcases
    },
    {
        "Lookup",
        Lookup_setup,
        NULL,
        63,
        Lookup_testcases
    },
    {
        "Singleton",
        NULL,
        NULL,
        5,
        Singleton_testcases
    },
    {
        "Clone",
        NULL,
        NULL,
        15,
        Clone_testcases
    },
    {
        "ComponentLifecycle",
        ComponentLifecycle_setup,
        NULL,
        92,
        ComponentLifecycle_testcases
    },
    {
        "Pairs",
        NULL,
        NULL,
        125,
        Pairs_testcases
    },
    {
        "Trigger",
        NULL,
        NULL,
        116,
        Trigger_testcases
    },
    {
        "Observer",
        NULL,
        NULL,
        174,
        Observer_testcases
    },
    {
        "ObserverOnSet",
        NULL,
        NULL,
        21,
        ObserverOnSet_testcases
    },
    {
        "TriggerOnAdd",
        TriggerOnAdd_setup,
        NULL,
        30,
        TriggerOnAdd_testcases
    },
    {
        "TriggerOnRemove",
        NULL,
        NULL,
        12,
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
        17,
        Monitor_testcases
    },
    {
        "Prefab",
        Prefab_setup,
        NULL,
        129,
        Prefab_testcases
    },
    {
        "World",
        World_setup,
        NULL,
        58,
        World_testcases
    },
    {
        "WorldInfo",
        NULL,
        NULL,
        7,
        WorldInfo_testcases
    },
    {
        "Type",
        Type_setup,
        NULL,
        23,
        Type_testcases
    },
    {
        "Commands",
        NULL,
        NULL,
        139,
        Commands_testcases
    },
    {
        "SingleThreadStaging",
        SingleThreadStaging_setup,
        NULL,
        67,
        SingleThreadStaging_testcases
    },
    {
        "Stresstests",
        Stresstests_setup,
        NULL,
        14,
        Stresstests_testcases
    },
    {
        "Table",
        NULL,
        NULL,
        20,
        Table_testcases
    },
    {
        "Poly",
        NULL,
        NULL,
        3,
        Poly_testcases
    },
    {
        "Internals",
        Internals_setup,
        NULL,
        18,
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
        "StackAlloc",
        NULL,
        NULL,
        2,
        StackAlloc_testcases
    }
};

int main(int argc, char *argv[]) {
    return bake_test_run("core", argc, argv, suites, 46);
}
