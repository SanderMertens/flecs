
/* A friendly warning from bake.test
 * ----------------------------------------------------------------------------
 * This file is generated. To add/remove testcases modify the 'project.json' of
 * the test project. ANY CHANGE TO THIS FILE IS LOST AFTER (RE)BUILDING!
 * ----------------------------------------------------------------------------
 */

#include <cpptest.h>

// Testsuite 'Api'
void Api_module(void);

static bake_test_suite suites[] = {
    {
        .id = "Api",
        .testcase_count = 1,
        .testcases = (bake_test_case[]){
            {
                .id = "module",
                .function = Api_module
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ut_init(argv[0]);
    return bake_test_run("cpptest", argc, argv, suites, 1);
}
