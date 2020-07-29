
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

// Testsuite 'Type'
void Type_add_2(void);
void Type_add_instanceof(void);
void Type_add_childof(void);
void Type_1_component(void);
void Type_2_component(void);
void Type_1_component_signature(void);
void Type_2_component_signature(void);

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

// Testsuite 'ComponentLifecycle'
void ComponentLifecycle_ctor_on_add(void);
void ComponentLifecycle_dtor_on_remove(void);
void ComponentLifecycle_copy_on_set(void);
void ComponentLifecycle_copy_on_override(void);
void ComponentLifecycle_move_on_merge(void);
void ComponentLifecycle_non_pod_add(void);
void ComponentLifecycle_non_pod_remove(void);
void ComponentLifecycle_non_pod_set(void);
void ComponentLifecycle_non_pod_override(void);

static bake_test_suite suites[] = {
    {
        "Entity",
        NULL,
        NULL,
        19,
        (bake_test_case[]){
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
            }
        }
    },
    {
        "Type",
        NULL,
        NULL,
        7,
        (bake_test_case[]){
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
            }
        }
    },
    {
        "System",
        NULL,
        NULL,
        12,
        (bake_test_case[]){
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
            }
        }
    },
    {
        "Query",
        NULL,
        NULL,
        12,
        (bake_test_case[]){
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
            }
        }
    },
    {
        "ComponentLifecycle",
        NULL,
        NULL,
        9,
        (bake_test_case[]){
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
                "move_on_merge",
                ComponentLifecycle_move_on_merge
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
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("cpp_api", argc, argv, suites, 5);
}
