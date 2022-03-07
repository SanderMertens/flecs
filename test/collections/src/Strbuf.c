#include <collections.h>
#include <math.h>

void Strbuf_setup() {
    ecs_os_set_api_defaults();
}

void Strbuf_append() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_append(&b, "Bar %d", 10);
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar 10");
    ecs_os_free(str);
}

void Strbuf_appendstr() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr(&b, "Bar");
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar");
    ecs_os_free(str);
}

void Strbuf_appendstrn() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    ecs_strbuf_appendstrn(&b, "Bar", 1);
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooB");
    ecs_os_free(str);
}

void Strbuf_appendstr_null() {
    install_test_abort();
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_append(&b, "Foo");
    test_expect_abort();
    ecs_strbuf_appendstr(&b, NULL);
}

void Strbuf_append_list() {
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

void Strbuf_append_nested_list() {
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

void Strbuf_large_str() {
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

void Strbuf_empty_str() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    char *str = ecs_strbuf_get(&b);
    test_assert(str == NULL);
}

void Strbuf_append_zerocopy() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr_zerocpy(&b, ecs_os_strdup("Bar"));
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar");
    ecs_os_free(str);
}

void Strbuf_append_zerocopy_const() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr_zerocpy_const(&b, "Bar");
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar");
    ecs_os_free(str);
}

void Strbuf_append_zerocopy_only() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr_zerocpy(&b, ecs_os_strdup("Bar"));
    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "Bar");
    ecs_os_free(str);
}

void Strbuf_reset() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr(&b, "Bar");
    ecs_strbuf_reset(&b);
    char *str = ecs_strbuf_get(&b);
    test_assert(str == NULL);
}

void Strbuf_merge() {
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

void Strbuf_app_buffer() {
    char buf[256];
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    b.buf = buf;
    b.max = 256;
    ecs_strbuf_appendstr(&b, "Foo");
    ecs_strbuf_appendstr(&b, "Bar");

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "FooBar");
    ecs_os_free(str);
}

void Strbuf_append_char() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendch(&b, 'a');
    ecs_strbuf_appendch(&b, 'b');
    ecs_strbuf_appendch(&b, 'c');

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "abc");
    ecs_os_free(str);
}

void Strbuf_append_511_chars() {
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

void Strbuf_append_512_chars() {
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

void Strbuf_append_513_chars() {
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

void Strbuf_append_flt() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, 10.5, 0);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "10.5");
    ecs_os_free(str);
}

void Strbuf_append_nan() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, NAN, 0);

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "NaN");
    ecs_os_free(str);
}

void Strbuf_append_inf() {
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

void Strbuf_append_nan_delim() {
    ecs_strbuf_t b = ECS_STRBUF_INIT;
    ecs_strbuf_appendflt(&b, NAN, '"');

    char *str = ecs_strbuf_get(&b);
    test_assert(str != NULL);
    test_str(str, "\"NaN\"");
    ecs_os_free(str);
}

void Strbuf_append_inf_delim() {
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
