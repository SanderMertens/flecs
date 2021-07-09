#include <cpp_api.h>

void Misc_setup() {
    ecs_os_set_api_defaults();
}

void Misc_string_compare_w_char_ptr() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string(ecs_os_strdup(ptr_1));

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
}

void Misc_string_view_compare_w_char_ptr() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "bar";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);   
}

void Misc_string_compare_w_char_ptr_length_diff() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "foo";
    const char *ptr_3 = "barrr";
    const char *ptr_4 = "ba";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str == ptr_1);
    test_assert(str == ptr_2);
    test_assert(str != ptr_3);
    test_assert(str != ptr_4);
}

void Misc_string_compare_w_string() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_2 = flecs::string(ecs_os_strdup(ptr_2));

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_view_compare_w_string() {
    const char *ptr_1 = "foo";
    const char *ptr_2 = "bar";

    flecs::string str_1 = flecs::string_view(ptr_1);
    flecs::string str_2 = flecs::string_view(ptr_2);

    flecs::string str_3 = flecs::string(ecs_os_strdup(ptr_1));
    flecs::string str_4 = flecs::string(ecs_os_strdup(ptr_2));

    test_assert(str_1 == str_1);
    test_assert(str_1 == str_3);

    test_assert(str_2 == str_2);
    test_assert(str_2 == str_4);

    test_assert(str_1 != str_2);
    test_assert(str_2 != str_1);
}

void Misc_string_compare_nullptr() {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(ptr_1);

    test_assert(str != nullptr);
}

void Misc_nullptr_string_compare() {
    const char *ptr = "foo";

    flecs::string str_1 = flecs::string_view(nullptr);
    flecs::string str_2 = flecs::string_view(ptr);

    test_assert(str_1 == "");
    test_assert(str_1 != ptr);
    test_assert(str_1 != str_2);
}

void Misc_nullptr_string_compare_nullptr() {
    const char *ptr_1 = "foo";

    flecs::string str = flecs::string_view(nullptr);

    test_assert(str == "");
    test_assert(str != ptr_1);
}
