#include <api.h>

void Error_setup() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_tracing_enable(-5);
}

void Error_abort() {
    test_expect_abort();
    ecs_abort(ECS_INTERNAL_ERROR, NULL);
}

void Error_abort_w_param() {
    test_expect_abort();
    ecs_abort(ECS_INTERNAL_ERROR, "some parameter");
}

static bool my_abort_called = false;
static
void my_abort() {
    my_abort_called = true;
}

void Error_override_abort() {
    /* The real reason this tests exists is to achieve 100% coverage. Without
     * this test, the last line of the 'abort' function would never be covered
     * because abort always exits before it gets there. */

    /* hack, because the setup already set the OS API */
    ((ecs_os_api_t*)&ecs_os_api)->abort_ = my_abort;
    _ecs_abort(ECS_INTERNAL_ERROR, __FILE__, __LINE__, NULL);
    test_assert(my_abort_called == true);
}

void Error_assert_true() {
    ecs_assert(true, ECS_INTERNAL_ERROR, NULL);

    /* Assert should not trigger */
    test_assert(true);
}

void Error_assert_false() {
    test_expect_abort();
    ecs_assert(false, ECS_INTERNAL_ERROR, NULL);

    /* Assert should not trigger */
    test_assert(false);
}

void Error_assert_false_w_param() {
    test_expect_abort();
    ecs_assert(false, ECS_INTERNAL_ERROR, "some parameter");

    /* Assert should not trigger */
    test_assert(false);
}

void Error_error_codes() {
    test_assert(ecs_strerror(ECS_INVALID_PARAMETER) != NULL);
    test_assert(ecs_strerror(ECS_NOT_A_COMPONENT) != NULL);
    test_assert(ecs_strerror(ECS_TYPE_NOT_AN_ENTITY) != NULL);
    test_assert(ecs_strerror(ECS_INTERNAL_ERROR) != NULL);
    test_assert(ecs_strerror(ECS_ALREADY_DEFINED) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_COMPONENT_SIZE) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_COMPONENT_ALIGNMENT) != NULL);
    test_assert(ecs_strerror(ECS_OUT_OF_MEMORY) != NULL);
    test_assert(ecs_strerror(ECS_MODULE_UNDEFINED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_INDEX_OUT_OF_RANGE) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_IS_NOT_SHARED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_IS_SHARED) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_HAS_NO_DATA) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_TYPE_MISMATCH) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_WHILE_ITERATING) != NULL);
    test_assert(ecs_strerror(ECS_INVALID_FROM_WORKER) != NULL);
    test_assert(ecs_strerror(ECS_OUT_OF_RANGE) != NULL);
    test_assert(ecs_strerror(ECS_THREAD_ERROR) != NULL);
    test_assert(ecs_strerror(ECS_MISSING_OS_API) != NULL);
    test_assert(ecs_strerror(ECS_UNSUPPORTED) != NULL);
    test_assert(ecs_strerror(ECS_NO_OUT_COLUMNS) != NULL);
    test_assert(ecs_strerror(ECS_COLUMN_ACCESS_VIOLATION) != NULL);
    test_assert(ecs_strerror(ECS_DESERIALIZE_FORMAT_ERROR) != NULL);
}

void Error_log_dbg() {
    ecs_os_dbg("test debug message");
    test_assert(true);
}

void Error_log_log() {
    ecs_os_log("test log message");
    test_assert(true);
}

void Error_log_warning() {
    ecs_os_warn("test warning message");
    test_assert(true);
}

void Error_log_error() {
    ecs_os_err("test error message");
    test_assert(true);
}
