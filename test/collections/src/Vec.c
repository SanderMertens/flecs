#include <collections.h>

static ecs_size_t vec_alloc_size;
static int64_t vec_alloc_data[8];

static void* vec_test_malloc(ecs_size_t size) {
    vec_alloc_size = size;
    return vec_alloc_data;
}

static void* vec_test_realloc(void *ptr, ecs_size_t size) {
    (void)ptr;
    vec_alloc_size = size;
    return vec_alloc_data;
}

static void vec_test_free(void *ptr) {
    (void)ptr;
}

static void vec_install_test_alloc(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.malloc_ = vec_test_malloc;
    os_api.realloc_ = vec_test_realloc;
    os_api.free_ = vec_test_free;
    ecs_os_set_api(&os_api);
    vec_alloc_size = 0;
}

void Vec_setup(void) {
    ecs_os_set_api_defaults();
}

void Vec_set_size_large_elem_count(void) {
    vec_install_test_alloc();

    ecs_vec_t v;
    ecs_vec_init(NULL, &v, 56, 0);

    ecs_vec_set_size(NULL, &v, 56, (1 << 25) + 1);

    test_assert(vec_alloc_size > 0);
    test_assert(v.size >= (1 << 25) + 1);
    test_int(vec_alloc_size, 56 * v.size);

    ecs_vec_fini(NULL, &v, 56);

    ecs_os_set_api_defaults();
}

void Vec_set_size_elem_count_out_of_range(void) {
    install_test_abort();

    ecs_vec_t v;
    ecs_vec_init(NULL, &v, 1 << 22, 0);

    test_expect_abort();
    ecs_vec_set_size(NULL, &v, 1 << 22, 1024);
}
