#include <core.h>
#include <stdlib.h>

void Error_setup(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

void Error_abort(void) {
    test_expect_abort();
    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

void Error_abort_w_param(void) {
    test_expect_abort();
    ecs_abort(ECS_INTERNAL_ERROR, "some parameter");
}

static bool my_abort_called = false;
static void my_abort(void) {
    my_abort_called = true;
}

void Error_override_abort(void) {
    /* The real reason this tests exists is to achieve 100% coverage. Without
     * this test, the last line of the 'abort' function would never be covered
     * because abort always exits before it gets there. */

    /* hack, because the setup already set the OS API */
    ecs_os_api.abort_ = my_abort;
    ecs_os_abort();
    test_assert(my_abort_called == true);
}

void Error_assert_true(void) {
    ecs_assert(true, ECS_INTERNAL_ERROR, NULL);

    /* Assert should not trigger */
    test_assert(true);
}

void Error_assert_false(void) {
    test_expect_abort();
    ecs_assert(false, ECS_INTERNAL_ERROR, NULL);

    /* Assert should not trigger */
    test_assert(false);
}

void Error_assert_false_w_param(void) {
    test_expect_abort();
    ecs_assert(false, ECS_INTERNAL_ERROR, "some parameter");

    /* Assert should not trigger */
    test_assert(false);
}

void Error_error_codes(void) {
    test_assert(ecs_strerror(ECS_INVALID_PARAMETER) != NULL);
    test_assert(ecs_strerror(ECS_INTERNAL_ERROR) != NULL);
    test_assert(ecs_strerror(ECS_ALREADY_DEFINED) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_COMPONENT_SIZE) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_COMPONENT_ALIGNMENT) != NULL);
    test_assert(ecs_strerror(ECS_OUT_OF_MEMORY) != NULL);
    test_assert(ecs_strerror(ECS_MODULE_UNDEFINED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_INDEX_OUT_OF_RANGE) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_IS_NOT_SHARED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_IS_SHARED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_TYPE_MISMATCH) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_WHILE_READONLY) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_FROM_WORKER) != NULL);
    test_assert(ecs_strerror(ECS_OUT_OF_RANGE) != NULL);
    test_assert(ecs_strerror(ECS_OPERATION_FAILED) != NULL);
    test_assert(ecs_strerror(ECS_MISSING_OS_API) != NULL);
    test_assert(ecs_strerror(ECS_UNSUPPORTED) != NULL);
    test_assert(ecs_strerror(ECS_ACCESS_VIOLATION) != NULL);
}

void Error_log_dbg(void) {
    ecs_dbg("test debug message");
    test_assert(true);
}

void Error_log_log(void) {
    ecs_trace("test trace message");
    test_assert(true);
}

void Error_log_warning(void) {
    ecs_warn("test warning message");
    test_assert(true);
}

void Error_log_error(void) {
    ecs_err("test error message");
    test_assert(true);
}

void Error_last_error(void) {
    ecs_os_api.log_last_error_ = 10;
    int err = ecs_log_last_error();
    test_int(10, err);
    test_int(0, ecs_log_last_error());
}

void Error_set_log_level_return(void) {
    ecs_log_set_level(1);
    int prev = ecs_log_set_level(-1);
    test_int(prev, 1);
}

void Error_print_backtrace(void) {
    flecs_dump_backtrace(stderr);
    test_assert(true);
}

void Error_log_capture_nested_recursion(void) {
    ecs_log_start_capture(true);
    ecs_log_start_capture(true);

    ecs_log_(-3, __FILE__, __LINE__, "captured error");

    char *inner = ecs_log_stop_capture();
    char *outer = ecs_log_stop_capture();

    test_assert(inner == NULL);
    test_assert(outer != NULL);
    test_str(outer, "captured error");
    ecs_os_free(outer);
}

void Error_log_collect_errors(void) {
    ecs_vec_t errors;
    ecs_vec_init_t(NULL, &errors, char*, 0);

    ecs_log_start_collect_errors(&errors);
    ecs_err("first error");
    ecs_err("second error %d", 2);
    ecs_log_stop_collect_errors();

    test_int(ecs_vec_count(&errors), 2);
    test_str(ecs_vec_get_t(&errors, char*, 0)[0], "first error");
    test_str(ecs_vec_get_t(&errors, char*, 1)[0], "second error 2");

    ecs_os_free(ecs_vec_get_t(&errors, char*, 0)[0]);
    ecs_os_free(ecs_vec_get_t(&errors, char*, 1)[0]);
    ecs_vec_fini_t(NULL, &errors, char*);
}

void Error_log_collect_errors_nested_capture(void) {
    ecs_vec_t errors;
    ecs_vec_init_t(NULL, &errors, char*, 0);

    ecs_log_start_collect_errors(&errors);
    ecs_log_start_capture(false);
    ecs_err("captured error");
    char *captured = ecs_log_stop_capture();
    ecs_err("collected error");
    ecs_log_stop_collect_errors();

    test_str(captured, "captured error");
    ecs_os_free(captured);

    test_int(ecs_vec_count(&errors), 1);
    test_str(ecs_vec_get_t(&errors, char*, 0)[0], "collected error");

    ecs_os_free(ecs_vec_get_t(&errors, char*, 0)[0]);
    ecs_vec_fini_t(NULL, &errors, char*);
}

void Error_log_collect_errors_nested(void) {
    ecs_vec_t outer, inner;
    ecs_vec_init_t(NULL, &outer, char*, 0);
    ecs_vec_init_t(NULL, &inner, char*, 0);

    ecs_log_start_collect_errors(&outer);
    ecs_err("outer error");
    ecs_log_start_collect_errors(&inner);
    ecs_err("inner error");
    ecs_log_stop_collect_errors();
    ecs_err("outer error 2");
    ecs_log_stop_collect_errors();

    test_int(ecs_vec_count(&outer), 2);
    test_str(ecs_vec_get_t(&outer, char*, 0)[0], "outer error");
    test_str(ecs_vec_get_t(&outer, char*, 1)[0], "outer error 2");
    test_int(ecs_vec_count(&inner), 1);
    test_str(ecs_vec_get_t(&inner, char*, 0)[0], "inner error");

    ecs_os_free(ecs_vec_get_t(&outer, char*, 0)[0]);
    ecs_os_free(ecs_vec_get_t(&outer, char*, 1)[0]);
    ecs_os_free(ecs_vec_get_t(&inner, char*, 0)[0]);
    ecs_vec_fini_t(NULL, &outer, char*);
    ecs_vec_fini_t(NULL, &inner, char*);
}

void Error_log_collect_errors_restores_level(void) {
    ecs_vec_t errors;
    ecs_vec_init_t(NULL, &errors, char*, 0);

    int prev = ecs_log_set_level(-4);
    ecs_log_start_collect_errors(&errors);
    ecs_err("error");
    ecs_log_stop_collect_errors();
    test_int(ecs_log_get_level(), -4);
    ecs_log_set_level(prev);

    test_int(ecs_vec_count(&errors), 1);
    ecs_os_free(ecs_vec_get_t(&errors, char*, 0)[0]);
    ecs_vec_fini_t(NULL, &errors, char*);
}
