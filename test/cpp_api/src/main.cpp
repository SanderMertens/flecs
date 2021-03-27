
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpp_api.h>

// Testsuite 'Entity'
void Entity_new(void);
void Entity_new_named(void);
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
void Entity_replace(void);
void Entity_get_generic(void);
void Entity_get_mut_generic(void);
void Entity_get_generic_w_id(void);
void Entity_get_mut_generic_w_id(void);
void Entity_add_role(void);
void Entity_remove_role(void);
void Entity_has_role(void);
void Entity_pair_role(void);
void Entity_equals(void);
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
void Entity_force_owned_type(void);
void Entity_force_owned_type_w_pair(void);
void Entity_tag_has_size_zero(void);
void Entity_get_null_name(void);
void Entity_get_parent(void);
void Entity_get_parent_w_tag(void);
void Entity_is_component_enabled(void);
void Entity_is_enabled_component_enabled(void);
void Entity_is_disabled_component_enabled(void);
void Entity_get_type(void);
void Entity_get_nonempty_type(void);
void Entity_set_no_copy(void);
void Entity_set_copy(void);
void Entity_add_owned(void);
void Entity_set_owned(void);

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
void Pairs_type_w_pair(void);
void Pairs_type_w_pair_tag(void);
void Pairs_type_w_pair_tags(void);
void Pairs_type_w_tag_pair(void);
void Pairs_override_pair_w_type(void);
void Pairs_override_pair_w_type_tag(void);
void Pairs_override_tag_pair_w_type(void);
void Pairs_get_relation_from_id(void);
void Pairs_get_object_from_id(void);
void Pairs_get_recycled_relation_from_id(void);
void Pairs_get_recycled_object_from_id(void);
void Pairs_each(void);
void Pairs_each_pair(void);
void Pairs_each_pair_by_type(void);
void Pairs_each_pair_w_childof(void);
void Pairs_each_pair_w_recycled_rel(void);
void Pairs_each_pair_w_recycled_obj(void);

// Testsuite 'Switch'
void Switch_add_case(void);
void Switch_get_case(void);
void Switch_system_w_case(void);
void Switch_system_w_switch(void);
void Switch_add_case_w_type(void);

// Testsuite 'Paths'
void Paths_name(void);
void Paths_path_depth_1(void);
void Paths_path_depth_2(void);
void Paths_entity_lookup_name(void);
void Paths_entity_lookup_depth_1(void);
void Paths_entity_lookup_depth_2(void);
void Paths_alias_component(void);
void Paths_alias_scoped_component(void);
void Paths_alias_scoped_component_w_name(void);
void Paths_alias_entity(void);
void Paths_alias_entity_by_name(void);
void Paths_alias_entity_by_scoped_name(void);

// Testsuite 'Type'
void Type_add_2(void);
void Type_add_instanceof(void);
void Type_add_childof(void);
void Type_1_component(void);
void Type_2_component(void);
void Type_1_component_signature(void);
void Type_2_component_signature(void);
void Type_type_no_name(void);
void Type_null_args(void);

// Testsuite 'System'
void System_action(void);
void System_action_const(void);
void System_action_shared(void);
void System_action_optional(void);
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
void System_action_tag(void);
void System_iter_tag(void);
void System_each_tag(void);
void System_system_from_id(void);
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

// Testsuite 'Trigger'
void Trigger_on_add(void);
void Trigger_on_remove(void);
void Trigger_on_add_tag_action(void);
void Trigger_on_add_tag_iter(void);
void Trigger_on_add_tag_each(void);

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

// Testsuite 'ComponentLifecycle'
void ComponentLifecycle_ctor_on_add(void);
void ComponentLifecycle_dtor_on_remove(void);
void ComponentLifecycle_copy_on_set(void);
void ComponentLifecycle_copy_on_override(void);
void ComponentLifecycle_non_pod_add(void);
void ComponentLifecycle_non_pod_remove(void);
void ComponentLifecycle_non_pod_set(void);
void ComponentLifecycle_non_pod_override(void);
void ComponentLifecycle_get_mut_new(void);
void ComponentLifecycle_get_mut_existing(void);
void ComponentLifecycle_pod_component(void);
void ComponentLifecycle_relocatable_component(void);
void ComponentLifecycle_implicit_component(void);
void ComponentLifecycle_implicit_after_query(void);

// Testsuite 'Refs'
void Refs_get_ref(void);
void Refs_ref_after_add(void);
void Refs_ref_after_remove(void);
void Refs_ref_after_set(void);
void Refs_ref_before_set(void);

// Testsuite 'Module'
void Module_import(void);
void Module_lookup_from_scope(void);
void Module_nested_module(void);

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
void ImplicitComponents_use_const(void);
void ImplicitComponents_use_const_w_stage(void);
void ImplicitComponents_use_const_w_threads(void);
void ImplicitComponents_implicit_base(void);
void ImplicitComponents_implicit_const(void);
void ImplicitComponents_implicit_ref(void);
void ImplicitComponents_implicit_ptr(void);

// Testsuite 'Snapshot'
void Snapshot_simple_snapshot(void);
void Snapshot_snapshot_iter(void);
void Snapshot_snapshot_iter_w_filter(void);

// Testsuite 'WorldFactory'
void WorldFactory_entity(void);
void WorldFactory_entity_w_name(void);
void WorldFactory_entity_w_id(void);
void WorldFactory_prefab(void);
void WorldFactory_prefab_w_name(void);
void WorldFactory_type(void);
void WorldFactory_type_w_name(void);
void WorldFactory_system(void);
void WorldFactory_system_w_name(void);
void WorldFactory_system_w_expr(void);
void WorldFactory_query(void);
void WorldFactory_query_w_expr(void);
void WorldFactory_snapshot(void);
void WorldFactory_module(void);
void WorldFactory_module_w_name(void);

// Testsuite 'World'
void World_multi_world_empty(void);
void World_multi_world_component(void);
void World_multi_world_component_namespace(void);
void World_type_id(void);
void World_different_comp_same_name(void);
void World_reregister_after_reset(void);
void World_reregister_after_reset_w_namespace(void);
void World_reregister_namespace(void);
void World_implicit_reregister_after_reset(void);
void World_reregister_after_reset_different_name(void);
void World_reimport(void);
void World_reimport_module_after_reset(void);
void World_reimport_module_new_world(void);
void World_reimport_namespaced_module(void);
void World_c_interop_module(void);
void World_c_interop_after_reset(void);
void World_implicit_register_w_new_world(void);

// Testsuite 'Singleton'
void Singleton_set_get_singleton(void);
void Singleton_get_mut_singleton(void);
void Singleton_modified_singleton(void);
void Singleton_patch_singleton(void);
void Singleton_remove_singleton(void);
void Singleton_has_singleton(void);
void Singleton_singleton_system(void);
void Singleton_get_singleton(void);
void Singleton_type_id_from_world(void);

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
        "replace",
        Entity_replace
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
        "get_mut_generic_w_id",
        Entity_get_mut_generic_w_id
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
        "force_owned_type",
        Entity_force_owned_type
    },
    {
        "force_owned_type_w_pair",
        Entity_force_owned_type_w_pair
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
        "get_parent",
        Entity_get_parent
    },
    {
        "get_parent_w_tag",
        Entity_get_parent_w_tag
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
        "add_owned",
        Entity_add_owned
    },
    {
        "set_owned",
        Entity_set_owned
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
        "type_w_pair",
        Pairs_type_w_pair
    },
    {
        "type_w_pair_tag",
        Pairs_type_w_pair_tag
    },
    {
        "type_w_pair_tags",
        Pairs_type_w_pair_tags
    },
    {
        "type_w_tag_pair",
        Pairs_type_w_tag_pair
    },
    {
        "override_pair_w_type",
        Pairs_override_pair_w_type
    },
    {
        "override_pair_w_type_tag",
        Pairs_override_pair_w_type_tag
    },
    {
        "override_tag_pair_w_type",
        Pairs_override_tag_pair_w_type
    },
    {
        "get_relation_from_id",
        Pairs_get_relation_from_id
    },
    {
        "get_object_from_id",
        Pairs_get_object_from_id
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
        "each_pair_w_childof",
        Pairs_each_pair_w_childof
    },
    {
        "each_pair_w_recycled_rel",
        Pairs_each_pair_w_recycled_rel
    },
    {
        "each_pair_w_recycled_obj",
        Pairs_each_pair_w_recycled_obj
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
        "system_w_switch",
        Switch_system_w_switch
    },
    {
        "add_case_w_type",
        Switch_add_case_w_type
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

bake_test_case Type_testcases[] = {
    {
        "add_2",
        Type_add_2
    },
    {
        "add_instanceof",
        Type_add_instanceof
    },
    {
        "add_childof",
        Type_add_childof
    },
    {
        "1_component",
        Type_1_component
    },
    {
        "2_component",
        Type_2_component
    },
    {
        "1_component_signature",
        Type_1_component_signature
    },
    {
        "2_component_signature",
        Type_2_component_signature
    },
    {
        "type_no_name",
        Type_type_no_name
    },
    {
        "null_args",
        Type_null_args
    }
};

bake_test_case System_testcases[] = {
    {
        "action",
        System_action
    },
    {
        "action_const",
        System_action_const
    },
    {
        "action_shared",
        System_action_shared
    },
    {
        "action_optional",
        System_action_optional
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
        "action_tag",
        System_action_tag
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
        "system_from_id",
        System_system_from_id
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
    }
};

bake_test_case Trigger_testcases[] = {
    {
        "on_add",
        Trigger_on_add
    },
    {
        "on_remove",
        Trigger_on_remove
    },
    {
        "on_add_tag_action",
        Trigger_on_add_tag_action
    },
    {
        "on_add_tag_iter",
        Trigger_on_add_tag_iter
    },
    {
        "on_add_tag_each",
        Trigger_on_add_tag_each
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
        "copy_on_set",
        ComponentLifecycle_copy_on_set
    },
    {
        "copy_on_override",
        ComponentLifecycle_copy_on_override
    },
    {
        "non_pod_add",
        ComponentLifecycle_non_pod_add
    },
    {
        "non_pod_remove",
        ComponentLifecycle_non_pod_remove
    },
    {
        "non_pod_set",
        ComponentLifecycle_non_pod_set
    },
    {
        "non_pod_override",
        ComponentLifecycle_non_pod_override
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
        "pod_component",
        ComponentLifecycle_pod_component
    },
    {
        "relocatable_component",
        ComponentLifecycle_relocatable_component
    },
    {
        "implicit_component",
        ComponentLifecycle_implicit_component
    },
    {
        "implicit_after_query",
        ComponentLifecycle_implicit_after_query
    }
};

bake_test_case Refs_testcases[] = {
    {
        "get_ref",
        Refs_get_ref
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
    }
};

bake_test_case Snapshot_testcases[] = {
    {
        "simple_snapshot",
        Snapshot_simple_snapshot
    },
    {
        "snapshot_iter",
        Snapshot_snapshot_iter
    },
    {
        "snapshot_iter_w_filter",
        Snapshot_snapshot_iter_w_filter
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
        "type",
        WorldFactory_type
    },
    {
        "type_w_name",
        WorldFactory_type_w_name
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
    },
    {
        "module_w_name",
        WorldFactory_module_w_name
    }
};

bake_test_case World_testcases[] = {
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
        "modified_singleton",
        Singleton_modified_singleton
    },
    {
        "patch_singleton",
        Singleton_patch_singleton
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
    }
};

static bake_test_suite suites[] = {
    {
        "Entity",
        NULL,
        NULL,
        54,
        Entity_testcases
    },
    {
        "Pairs",
        NULL,
        NULL,
        33,
        Pairs_testcases
    },
    {
        "Switch",
        NULL,
        NULL,
        5,
        Switch_testcases
    },
    {
        "Paths",
        NULL,
        NULL,
        12,
        Paths_testcases
    },
    {
        "Type",
        NULL,
        NULL,
        9,
        Type_testcases
    },
    {
        "System",
        NULL,
        NULL,
        35,
        System_testcases
    },
    {
        "Trigger",
        NULL,
        NULL,
        5,
        Trigger_testcases
    },
    {
        "Query",
        NULL,
        NULL,
        20,
        Query_testcases
    },
    {
        "ComponentLifecycle",
        NULL,
        NULL,
        14,
        ComponentLifecycle_testcases
    },
    {
        "Refs",
        NULL,
        NULL,
        5,
        Refs_testcases
    },
    {
        "Module",
        NULL,
        NULL,
        3,
        Module_testcases
    },
    {
        "ImplicitComponents",
        NULL,
        NULL,
        25,
        ImplicitComponents_testcases
    },
    {
        "Snapshot",
        NULL,
        NULL,
        3,
        Snapshot_testcases
    },
    {
        "WorldFactory",
        NULL,
        NULL,
        15,
        WorldFactory_testcases
    },
    {
        "World",
        NULL,
        NULL,
        17,
        World_testcases
    },
    {
        "Singleton",
        NULL,
        NULL,
        9,
        Singleton_testcases
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("cpp_api", argc, argv, suites, 16);
}
