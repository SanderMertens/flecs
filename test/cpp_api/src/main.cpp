
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
void Entity_remove(void);
void Entity_remove_2(void);
void Entity_set(void);
void Entity_set_2(void);
void Entity_replace(void);

// Testsuite 'System'
void System_action(void);
void System_action_const(void);
void System_action_shared(void);
void System_each(void);
void System_each_const(void);
void System_each_shared(void);
void System_signature(void);
void System_signature_const(void);
void System_signature_shared(void);

static bake_test_suite suites[] = {
    {
        "Entity",
        NULL,
        NULL,
        13,
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
                "remove",
                Entity_remove
            },
            {
                "remove_2",
                Entity_remove_2
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
        "System",
        NULL,
        NULL,
        9,
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
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("cpp_api", argc, argv, suites, 2);
}
