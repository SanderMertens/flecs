#include <cpp.h>

static int32_t test_expected_abort_code = 0;

static void test_abort_w_code(void) {
    if (test_expected_abort_code) {
        test_str(ecs_strerror(ecs_log_last_error()),
            ecs_strerror(test_expected_abort_code));
    }
    test_abort();
}

void test_expect_abort_code(int32_t error_code) {
    test_expected_abort_code = error_code;
    test_expect_abort();
}

void install_test_abort(void) {
    test_expected_abort_code = 0;
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort_w_code;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);
}
