#include <stdint.h>
#include <stdlib.h>
#include "flecs.h"
#include <meta.h>



int cmp(const void *a, const void *b, const ecs_type_info_t* ti) {
    return ti->hooks.comp(a, b, ti);
}

const ecs_type_info_t *sort_ti = NULL;
int compare_element(const void *a, const void *b) {
    return cmp(a, b, sort_ti);
}

void sort_array(const ecs_world_t* world, ecs_entity_t component, void *arr, ecs_size_t num_elements) {
    sort_ti = ecs_get_type_info(world, component);
    qsort(arr, num_elements, sort_ti->size, compare_element);
}

/* Primitive comparer testing 
 * Comparer is demonstrated to work if it successfully sorts an array of primitives.
*/

void Compare_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_bool_t arr[] = {true, false, true, false};
    ecs_bool_t expected[] = {false, false, true, true};

    sort_array(world, ecs_id(ecs_bool_t), arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t arr[] = {'z', 'a', 'm', 'a', 'x', 'b'};
    ecs_char_t expected[] = {'a', 'a', 'b', 'm', 'x', 'z'};

    sort_array(world, ecs_id(ecs_char_t), arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_byte(void) {
    ecs_world_t *world = ecs_init();

    ecs_byte_t arr[] = {0xFF, 0x01, 0x7F, 0x01, 0x00};
    ecs_byte_t expected[] = {0x00, 0x01, 0x01, 0x7F, 0xFF};

    sort_array(world, ecs_id(ecs_byte_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}


void Compare_u8(void) {
    ecs_world_t *world = ecs_init();
    
    ecs_u8_t arr[] = {1, 79, 12, 3, 255, 79, 0, 14 };
    ecs_u8_t expected[] = {0, 1, 3, 12, 14, 79, 79, 255 };

    sort_array(world, ecs_id(ecs_u8_t), arr, 8);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_u16(void) {
    ecs_world_t *world = ecs_init();

    ecs_u16_t arr[] = {1024, 65535, 0, 1, 1024, 256};
    ecs_u16_t expected[] = {0, 1, 256, 1024, 1024, 65535};

    sort_array(world, ecs_id(ecs_u16_t), arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t arr[] = {100000, 500, 4294967295, 100000, 0};
    ecs_u32_t expected[] = {0, 500, 100000, 100000, 4294967295};

    sort_array(world, ecs_id(ecs_u32_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_u64(void) {
    ecs_world_t *world = ecs_init();

    ecs_u64_t arr[] = {18446744073709551615ULL, 0, 1000, 18446744073709551615ULL, 42};
    ecs_u64_t expected[] = {0, 42, 1000, 18446744073709551615ULL, 18446744073709551615ULL};

    sort_array(world, ecs_id(ecs_u64_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_uptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t arr[] = {(ecs_uptr_t)0x1234, (ecs_uptr_t)0x5678, (ecs_uptr_t)0x1234, (ecs_uptr_t)0x9ABC};
    ecs_uptr_t expected[] = {(ecs_uptr_t)0x1234, (ecs_uptr_t)0x1234, (ecs_uptr_t)0x5678, (ecs_uptr_t)0x9ABC};

    sort_array(world, ecs_id(ecs_uptr_t), arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t arr[] = {-128, 127, 0, -1, 127, 1};
    ecs_i8_t expected[] = {-128, -1, 0, 1, 127, 127};

    sort_array(world, ecs_id(ecs_i8_t), arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t arr[] = {-32768, 32767, 100, -100, 32767};
    ecs_i16_t expected[] = {-32768, -100, 100, 32767, 32767};

    sort_array(world, ecs_id(ecs_i16_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t arr[] = {-100000, 50000, 0, -100000, 100000};
    ecs_i32_t expected[] = {-100000, -100000, 0, 50000, 100000};

    sort_array(world, ecs_id(ecs_i32_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t arr[] = {-9223372036854775807LL, 9223372036854775807LL, 0, -1000,
         9223372036854775807LL};
    ecs_i64_t expected[] = {-9223372036854775807LL, -1000, 0,
        9223372036854775807LL, 9223372036854775807LL};

    sort_array(world, ecs_id(ecs_i64_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_iptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t arr[] = {(ecs_iptr_t)-1000, (ecs_iptr_t)500, (ecs_iptr_t)0,
        (ecs_iptr_t)-1000};
    ecs_iptr_t expected[] = {(ecs_iptr_t)-1000, (ecs_iptr_t)-1000, (ecs_iptr_t)0,
         (ecs_iptr_t)500};

    sort_array(world, ecs_id(ecs_iptr_t), arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t arr[] = {3.14f, 2.71f, -1.0f, 2.71f, 0.0f};
    ecs_f32_t expected[] = {-1.0f, 0.0f, 2.71f, 2.71f, 3.14f};

    sort_array(world, ecs_id(ecs_f32_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t arr[] = {3.14159, 2.71828, -1.0, 2.71828, 0.0};
    ecs_f64_t expected[] = {-1.0, 0.0, 2.71828, 2.71828, 3.14159};

    sort_array(world, ecs_id(ecs_f64_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr[] = {1000, 42, 1000, 500, 0};
    ecs_entity_t expected[] = {0, 42, 500, 1000, 1000};

    sort_array(world, ecs_id(ecs_entity_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t arr[] = {1000, 42, 1000, 500, 0};
    ecs_id_t expected[] = {0, 42, 500, 1000, 1000};

    sort_array(world, ecs_id(ecs_id_t), arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void Compare_string(void) {

    ecs_world_t *world = ecs_init();

    const char* const_arr[] = {"world", "hello", NULL, "aa", "zz" ,"aa", "cc", "bb"};
    const char* const_expected[] = {NULL, "aa", "aa", "bb", "cc", "hello", "world", "zz"};

    ecs_size_t count = sizeof(const_arr) / sizeof(const char*);
    test_assert(count == sizeof(const_expected) / sizeof(const char*));

    ecs_string_t arr[count];
    ecs_string_t expected[count];

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(ecs_string_t));
    ti->hooks.copy_ctor(arr, const_arr, count, ti);
    ti->hooks.copy_ctor(expected, const_expected,count , ti);

    sort_array(world, ecs_id(ecs_string_t), arr, count);

    int i;
    for(i = 0; i < count; i++) {
        test_assert((arr[i] == NULL && expected[i] == NULL) || 
            ecs_os_strcmp(arr[i], expected[i]) == 0);
    }

    ti->hooks.dtor(arr, count, ti);
    ti->hooks.dtor(expected, count, ti);

    ecs_fini(world);
}

void Compare_const_string(void) {

    ecs_world_t *world = ecs_init();

    // Lookup const string type:
    ecs_entity_t const_string = ecs_lookup(world, "flecs.core.const_string_t");

    const char* arr[] = {"world", "hello", NULL, "aa", "zz" ,"aa", "cc", "bb"};
    const char* expected[] = {NULL, "aa", "aa", "bb", "cc", "hello", "world", "zz"};

    ecs_size_t count = sizeof(arr) / sizeof(const char*);
    test_assert(count == sizeof(expected) / sizeof(const char*));

    sort_array(world, const_string, arr, count);

    int i;
    for(i = 0; i < count; i++) {
        test_assert((arr[i] == NULL && expected[i] == NULL) || 
            ecs_os_strcmp(arr[i], expected[i]) == 0);
    }


    ecs_fini(world);
}