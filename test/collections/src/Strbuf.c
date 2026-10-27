#include <collections.h>
#include <math.h>
#include <stdint.h>

void Strbuf_setup(void) {
    ecs_os_set_api_defaults();
}

void Strbuf_append(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_append(&b, "Bar %d", 10);
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar 10");
    ecs_os_free(str);
}

void Strbuf_appendstr(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr(&b, "Bar");
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar");
    ecs_os_free(str);
}

void Strbuf_appendstrn(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_appendstrn(&b, "Bar", 1);
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooB");
    ecs_os_free(str);
}

void Strbuf_appendstr_null(void) {
    install_test_abort();
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    test_expect_abort();
    ecs_strbuf_appendstr(&b, NULL);
}

void Strbuf_append_list(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_list_push(&b, "{", ",");
    ecs_strbuf_list_append(&b, "Foo %d", 10);
    ecs_strbuf_list_appendstr(&b, "Bar");
    ecs_strbuf_list_pop(&b, "}");
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "Foo{Foo 10,Bar}");
    ecs_os_free(str);
}

void Strbuf_append_nested_list(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_list_push(&b, "{", ",");
    ecs_strbuf_list_append(&b, "Foo %d", 10);
    ecs_strbuf_list_appendstr(&b, "Bar");

    ecs_strbuf_list_push(&b, "[", ",");
    ecs_strbuf_list_appendstr(&b, "Hello");
    ecs_strbuf_list_appendstr(&b, "World");
    ecs_strbuf_list_pop(&b, "]");

    ecs_strbuf_list_next(&b);
    ecs_strbuf_list_push(&b, "[", ",");
    ecs_strbuf_list_appendstr(&b, "Hello");
    ecs_strbuf_list_appendstr(&b, "World");
    ecs_strbuf_list_pop(&b, "]");    

    ecs_strbuf_list_pop(&b, "}");
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "Foo{Foo 10,Bar[Hello,World],[Hello,World]}");
    ecs_os_free(str);
}

void Strbuf_large_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");

    int i;
    for (i = 0; i < 200; i ++) {
        ecs_strbuf_appendstr(&b, "Bar");    
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_assert(!strncmp(str, "Foo", 3));
    for (i = 1; i < 201; i ++) {
        test_assert(!strncmp(&str[i * 3], "Bar", 3));
    }

    ecs_os_free(str);
}

void Strbuf_empty_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    char *str = ecs_strbuf_get(&b);
    test_assert(str == NULL);
}

void Strbuf_reset(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr(&b, "Bar");
    ecs_strbuf_reset(&b);
    char *str = ecs_strbuf_get(&b);
    test_assert(str == NULL);
}

void Strbuf_merge(void) {
    ecs_strbuf_t b1 = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b1, "Foo");
    ecs_strbuf_appendstr(&b1, "Bar");

    ecs_strbuf_t b2 = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b2, "Hello");
    ecs_strbuf_appendstr(&b2, "World");
    ecs_strbuf_mergebuff(&b1, &b2);

    char *str = ecs_strbuf_get(&b1);
    test_str(str, "FooBarHelloWorld");
    ecs_os_free(str);

    str = ecs_strbuf_get(&b2);
    test_assert(str == NULL);
}

void Strbuf_merge_empty(void) {
    ecs_strbuf_t b1 = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b1, "Foo");
    ecs_strbuf_appendstr(&b1, "Bar");

    ecs_strbuf_t b2 = ECS_STRBUF_INIT;
    ecs_strbuf_mergebuff(&b1, &b2);

    char *str = ecs_strbuf_get(&b1);
    test_str(str, "FooBar");
    ecs_os_free(str);

    str = ecs_strbuf_get(&b2);
    test_assert(str == NULL);
}

void Strbuf_merge_heap_unterminated(void) {
    ecs_strbuf_t src = ECS_STRBUF_INIT;
    int32_t n = ECS_STRBUF_SMALL_STRING_SIZE + 100;
    char *payload = ecs_os_malloc_n(char, n);
    ecs_os_memset(payload, 'A', n);
    ecs_strbuf_appendstrn(&src, payload, n);
    ecs_os_free(payload);

    test_assert(src.size > src.length);
    src.content[src.length] = 'X';
    if (src.size > src.length + 1) {
        src.content[src.length + 1] = '\0';
    }

    ecs_strbuf_t dst = ECS_STRBUF_INIT;
    ecs_strbuf_mergebuff(&dst, &src);

    char *str = ecs_strbuf_get(&dst);
    test_assert(str != NULL);
    test_int((int32_t)ecs_os_strlen(str), n);
    int32_t i;
    for (i = 0; i < n; i ++) {
        test_assert(str[i] == 'A');
    }
    ecs_os_free(str);
}

void Strbuf_append_char(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendch(&b, 'a');
    ecs_strbuf_appendch(&b, 'b');
    ecs_strbuf_appendch(&b, 'c');

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "abc");
    ecs_os_free(str);
}

void Strbuf_append_511_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 511; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 511; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_512_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 512; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 512; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_513_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 513; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 513; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_1023_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1023; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 1023; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_1024_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1024; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 1024; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_1025_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1025; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 1025; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_2047_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2047; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 2047; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_2048_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2048; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 2048; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_2049_chars(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2049; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    for (int i = 0; i < 2049; i ++) {
        test_assert(str[i] == ('a' + i % 26));
    }
    ecs_os_free(str);
}

void Strbuf_append_511_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 511; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_512_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 512; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_513_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 513; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_1023_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1023; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_1024_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1024; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_1025_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 1025; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_2047_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2047; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_2048_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2048; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_2049_str(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;

    for (int i = 0; i < 2049; i ++) {
        ecs_strbuf_appendch(&b, 'a' + (i % 26));
    }

    char *str = ecs_strbuf_get(&b);

    ecs_strbuf_appendstr(&b, str);
    char *str2 = ecs_strbuf_get(&b);

    test_str(str, str2);

    ecs_os_free(str);
    ecs_os_free(str2);
}

void Strbuf_append_flt(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, 10.5, 0);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "10.5");
    ecs_os_free(str);
}

void Strbuf_append_nan(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, NAN, 0);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "NaN");
    ecs_os_free(str);
}

void Strbuf_append_inf(void) {
    {
        ecs_strbuf_t b = ECS_STRBUF_INIT;
        ecs_strbuf_appendflt(&b, INFINITY, 0);

        char *str = ecs_strbuf_get(&b);
        test_assert(str != NULL);
        test_str(str, "Inf");
        ecs_os_free(str);
    }
    {
        ecs_strbuf_t b = ECS_STRBUF_INIT;
        ecs_strbuf_appendflt(&b, -INFINITY, 0);

        char *str = ecs_strbuf_get(&b);
        test_assert(str != NULL);
        test_str(str, "Inf");
        ecs_os_free(str);
    }
}

void Strbuf_append_nan_delim(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, NAN, '"');

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "\"NaN\"");
    ecs_os_free(str);
}

void Strbuf_append_inf_delim(void) {
    {
        ecs_strbuf_t b = ECS_STRBUF_INIT;
        ecs_strbuf_appendflt(&b, INFINITY, '"');

        char *str = ecs_strbuf_get(&b);
        test_assert(str != NULL);
        test_str(str, "\"Inf\"");
        ecs_os_free(str);
    }
    {
        ecs_strbuf_t b = ECS_STRBUF_INIT;
        ecs_strbuf_appendflt(&b, -INFINITY, '"');

        char *str = ecs_strbuf_get(&b);
        test_assert(str != NULL);
        test_str(str, "\"Inf\"");
        ecs_os_free(str);
    }
}

void Strbuf_append_int64_min(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendint(&b, INT64_MIN);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "-9223372036854775808");
    ecs_os_free(str);
}

void Strbuf_append_flt_2_pow_63(void) {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, 9223372036854775808.0, 0);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "9.22337203e18");
    ecs_os_free(str);
}

/* Allocations of at least this size fail, so that a string buffer cannot
 * grow out of its small string. Smaller ones (such as the abort message) go
 * to the default allocator. */
#define STRBUF_FAIL_SIZE (ECS_STRBUF_SMALL_STRING_SIZE * 2)

static ecs_os_api_malloc_t strbuf_default_malloc;
static ecs_os_api_realloc_t strbuf_default_realloc;
static char strbuf_fake_content[1];

static void* strbuf_test_malloc(ecs_size_t size) {
    if (size >= STRBUF_FAIL_SIZE) {
        return NULL;
    }
    return strbuf_default_malloc(size);
}

static void* strbuf_test_realloc(void *ptr, ecs_size_t size) {
    if (ptr == strbuf_fake_content) {
        /* The requested size must not have wrapped around */
        test_int(size, INT32_MAX);
        return NULL;
    }
    if (size >= STRBUF_FAIL_SIZE) {
        return NULL;
    }
    return strbuf_default_realloc(ptr, size);
}

static void strbuf_install_failing_alloc(void) {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    strbuf_default_malloc = os_api.malloc_;
    strbuf_default_realloc = os_api.realloc_;
    os_api.malloc_ = strbuf_test_malloc;
    os_api.realloc_ = strbuf_test_realloc;
    ecs_os_set_api(&os_api);
    ecs_log_set_level(-5);
}

void Strbuf_grow_alloc_fails(void) {
    strbuf_install_failing_alloc();

    ecs_strbuf_t b = ECS_STRBUF_INIT;
    char str[ECS_STRBUF_SMALL_STRING_SIZE * 2];
    ecs_os_memset(str, 'a', (int)sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    /* Growing out of the small string fails: abort instead of writing to a
     * NULL buffer. */
    test_expect_abort();
    ecs_strbuf_appendstr(&b, str);
}

void Strbuf_grow_past_1gb(void) {
    strbuf_install_failing_alloc();

    /* A buffer of 1GB that is full. Doubling its size wraps an int32_t. */
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    b.content = strbuf_fake_content;
    b.size = 1 << 30;
    b.length = b.size;

    test_expect_abort();
    ecs_strbuf_appendch(&b, 'a');
}
