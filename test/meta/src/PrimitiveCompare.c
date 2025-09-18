#include <stdlib.h>
#include <meta.h>
#include "flecs.h"


static
int cmp(const void *a, const void *b, const ecs_type_info_t* ti) {
    return ti->hooks.cmp(a, b, ti);
}

static
bool equals(const void *a, const void *b, const ecs_type_info_t* ti) {
    return ti->hooks.equals(a, b, ti);
}

const ecs_type_info_t *sort_ti = NULL;
static
int compare_element(const void *a, const void *b) {
    return cmp(a, b, sort_ti);
}

static
void sort_array(const ecs_type_info_t* ti, void *arr, ecs_size_t num_elements) {
    sort_ti = ti;
    qsort(arr, num_elements, sort_ti->size, compare_element);
}

static
bool str_equals(const char* a, const char* b) {
    if(a == b) {
        return true;
    }
    if(a == NULL) {
        return false;
    }
    if(b == NULL) {
        return false;
    }
    return ecs_os_strcmp(a,b) == 0;
}

/* Primitive comparer testing 
 * Comparer is demonstrated to work if it successfully sorts an array of primitives.
*/

void PrimitiveCompare_bool(void) {
    ecs_world_t *world = ecs_init();

    ecs_bool_t arr[] = {true, false, true, false};
    ecs_bool_t expected[] = {false, false, true, true};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_bool_t));

    /* test "less" */
    test_assert(cmp(&arr[1], &arr[0], ti) < 0); /* false < true */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[1], ti) > 0); /* true > false */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[0], ti) == 0); /* true == true */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[0], ti)); /* true == true */

    /* further test by sorting the array */
    sort_array(ti, arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_char(void) {
    ecs_world_t *world = ecs_init();

    ecs_char_t arr[] = {'z', 'a', 'm', 'a', 'x', 'b'};
    ecs_char_t expected[] = {'a', 'a', 'b', 'm', 'x', 'z'};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_char_t));

    /* test "less" */
    test_assert(cmp(&arr[1], &arr[0], ti) < 0); /* 'a' < 'z' */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[1], ti) > 0); /* 'z' > 'a' */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[1], ti) == 0); /* 'a' == 'a' */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[1], ti)); /* 'a' == 'a' */

    /* further test by sorting the array */
    sort_array(ti, arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_byte(void) {
    ecs_world_t *world = ecs_init();

    ecs_byte_t arr[] = {0xFF, 0x01, 0x7F, 0x01, 0x00};
    ecs_byte_t expected[] = {0x00, 0x01, 0x01, 0x7F, 0xFF};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_byte_t));

    /* test "less" */
    test_assert(cmp(&arr[1], &arr[0], ti) < 0); /* 0x01 < 0xFF */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[1], ti) > 0); /* 0xFF > 0x01 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[1], ti) == 0); /* 0x01 == 0x01 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[1], ti)); /* 0x01 == 0x01 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_u8(void) {
    ecs_world_t *world = ecs_init();

    ecs_u8_t arr[] = {1, 79, 12, 3, 255, 79, 0, 14};
    ecs_u8_t expected[] = {0, 1, 3, 12, 14, 79, 79, 255};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_u8_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* 1 < 79 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[0], ti) > 0); /* 79 > 1 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[5], &arr[1], ti) == 0); /* 79 == 79 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[5], &arr[1], ti)); /* 79 == 79 */

    /* further test by sorting the array */
    sort_array(ti, arr, 8);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_u16(void) {
    ecs_world_t *world = ecs_init();

    ecs_u16_t arr[] = {1024, 65535, 0, 1, 1024, 256};
    ecs_u16_t expected[] = {0, 1, 256, 1024, 1024, 65535};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_u16_t));

    /* test "less" */
    test_assert(cmp(&arr[2], &arr[1], ti) < 0); /* 0 < 65535 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); /* 65535 > 0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[4], ti) == 0); /* 1024 == 1024 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[4], ti)); /* 1024 == 1024 */

    /* further test by sorting the array */
    sort_array(ti, arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_u32(void) {
    ecs_world_t *world = ecs_init();

    ecs_u32_t arr[] = {100000, 500, 4294967295, 100000, 0};
    ecs_u32_t expected[] = {0, 500, 100000, 100000, 4294967295};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_u32_t));

    /* test "less" */
    test_assert(cmp(&arr[4], &arr[1], ti) < 0); /* 0 < 500 */

    /* test "greater" */
    test_assert(cmp(&arr[2], &arr[1], ti) > 0); /* 4294967295 > 500 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[3], ti) == 0); /* 100000 == 100000 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[3], ti)); /* 100000 == 100000 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_u64(void) {
    ecs_world_t *world = ecs_init();

    ecs_u64_t arr[] = {18446744073709551615ULL, 0, 
        1000, 18446744073709551615ULL, 42};
    ecs_u64_t expected[] = {0, 42, 1000, 
        18446744073709551615ULL, 18446744073709551615ULL};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_u64_t));

    /* test "less" */
    /* 0 < 18446744073709551615 */
    test_assert(cmp(&arr[1], &arr[0], ti) < 0); 

    /* test "greater" */
    /* 18446744073709551615 > 1000 */
    test_assert(cmp(&arr[0], &arr[2], ti) > 0);

    /* test "equal" via cmp hook */
    /* 18446744073709551615 == 18446744073709551615 */
    test_assert(cmp(&arr[0], &arr[3], ti) == 0); 
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[3], ti)); 

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_uptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_uptr_t arr[] = {(ecs_uptr_t)0x1234, (ecs_uptr_t)0x5678, (ecs_uptr_t)0x1234, (ecs_uptr_t)0x9ABC};
    ecs_uptr_t expected[] = {(ecs_uptr_t)0x1234, (ecs_uptr_t)0x1234, (ecs_uptr_t)0x5678, (ecs_uptr_t)0x9ABC};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_uptr_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* 0x1234 < 0x5678 */

    /* test "greater" */
    test_assert(cmp(&arr[3], &arr[1], ti) > 0); /* 0x9ABC > 0x5678 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[2], ti) == 0); /* 0x1234 == 0x1234 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[2], ti)); /* 0x1234 == 0x1234 */

    /* further test by sorting the array */
    sort_array(ti, arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_i8(void) {
    ecs_world_t *world = ecs_init();

    ecs_i8_t arr[] = {-128, 127, 0, -1, 127, 1};
    ecs_i8_t expected[] = {-128, -1, 0, 1, 127, 127};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_i8_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* -128 < 127 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); /* 127 > 0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[4], ti) == 0); /* 127 == 127 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[4], ti)); /* 127 == 127 */

    /* further test by sorting the array */
    sort_array(ti, arr, 6);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_i16(void) {
    ecs_world_t *world = ecs_init();

    ecs_i16_t arr[] = {-32768, 32767, 100, -100, 32767};
    ecs_i16_t expected[] = {-32768, -100, 100, 32767, 32767};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_i16_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* -32768 < 32767 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); /* 32767 > 100 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[4], ti) == 0); /* 32767 == 32767 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[4], ti)); /* 32767 == 32767 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_i32(void) {
    ecs_world_t *world = ecs_init();

    ecs_i32_t arr[] = {-100000, 50000, 0, -100000, 100000};
    ecs_i32_t expected[] = {-100000, -100000, 0, 50000, 100000};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_i32_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* -100000 < 50000 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); /* 50000 > 0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[3], ti) == 0); /* -100000 == -100000 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[3], ti)); /* -100000 == -100000 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_i64(void) {
    ecs_world_t *world = ecs_init();

    ecs_i64_t arr[] = {-9223372036854775807LL, 9223372036854775807LL,
         0, -1000, 9223372036854775807LL};
    ecs_i64_t expected[] = {-9223372036854775807LL, -1000, 0,
         9223372036854775807LL, 9223372036854775807LL};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_i64_t));

    /* test "less" */
    /* -9223372036854775807 < 9223372036854775807 */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); 

    /* test "greater" */
    /* 9223372036854775807 > 0 */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); 

    /* test "equal" via cmp hook */
    /* 9223372036854775807 == 9223372036854775807 */
    test_assert(cmp(&arr[1], &arr[4], ti) == 0); 
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[4], ti)); 

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_iptr(void) {
    ecs_world_t *world = ecs_init();

    ecs_iptr_t arr[] = {(ecs_iptr_t)-1000, (ecs_iptr_t)500, (ecs_iptr_t)0, (ecs_iptr_t)-1000};
    ecs_iptr_t expected[] = {(ecs_iptr_t)-1000, (ecs_iptr_t)-1000, (ecs_iptr_t)0, (ecs_iptr_t)500};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_iptr_t));

    /* test "less" */
    test_assert(cmp(&arr[0], &arr[1], ti) < 0); /* -1000 < 500 */

    /* test "greater" */
    test_assert(cmp(&arr[1], &arr[2], ti) > 0); /* 500 > 0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[3], ti) == 0); /* -1000 == -1000 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[3], ti)); /* -1000 == -1000 */

    /* further test by sorting the array */
    sort_array(ti, arr, 4);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_f32(void) {
    ecs_world_t *world = ecs_init();

    ecs_f32_t arr[] = {3.14f, 2.71f, -1.0f, 2.71f, 0.0f};
    ecs_f32_t expected[] = {-1.0f, 0.0f, 2.71f, 2.71f, 3.14f};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_f32_t));

    /* test "less" */
    test_assert(cmp(&arr[2], &arr[0], ti) < 0); /* -1.0 < 3.14 */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[4], ti) > 0); /* 3.14 > 0.0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[3], ti) == 0); /* 2.71 == 2.71 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[3], ti)); /* 2.71 == 2.71 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_f64(void) {
    ecs_world_t *world = ecs_init();

    ecs_f64_t arr[] = {3.14159, 2.71828, -1.0, 2.71828, 0.0};
    ecs_f64_t expected[] = {-1.0, 0.0, 2.71828, 2.71828, 3.14159};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_f64_t));

    /* test "less" */
    test_assert(cmp(&arr[2], &arr[0], ti) < 0); /* -1.0 < 3.14159 */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[4], ti) > 0); /* 3.14159 > 0.0 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[1], &arr[3], ti) == 0); /* 2.71828 == 2.71828 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[1], &arr[3], ti)); /* 2.71828 == 2.71828 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t arr[] = {1000, 42, 1000, 500, 0};
    ecs_entity_t expected[] = {0, 42, 500, 1000, 1000};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_entity_t));

    /* test "less" */
    test_assert(cmp(&arr[4], &arr[1], ti) < 0); /* 0 < 42 */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[1], ti) > 0); /* 1000 > 42 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[2], ti) == 0); /* 1000 == 1000 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[2], ti)); /* 1000 == 1000 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}

void PrimitiveCompare_id(void) {
    ecs_world_t *world = ecs_init();

    ecs_id_t arr[] = {1000, 42, 1000, 500, 0};
    ecs_id_t expected[] = {0, 42, 500, 1000, 1000};

    const ecs_type_info_t* ti = ecs_get_type_info(world, ecs_id(ecs_id_t));

    /* test "less" */
    test_assert(cmp(&arr[4], &arr[1], ti) < 0); /* 0 < 42 */

    /* test "greater" */
    test_assert(cmp(&arr[0], &arr[1], ti) > 0); /* 1000 > 42 */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[0], &arr[2], ti) == 0); /* 1000 == 1000 */
    /* test "equal" via equals hook */
    test_assert(equals(&arr[0], &arr[2], ti)); /* 1000 == 1000 */

    /* further test by sorting the array */
    sort_array(ti, arr, 5);

    test_assert(memcmp(arr, expected, sizeof(arr)) == 0);

    ecs_fini(world);
}


#define STRING_COUNT 8
void PrimitiveCompare_string(void) {
    ecs_world_t *world = ecs_init();

    char* const_arr[] = {"world", "hello", NULL, "aa", "zz", "aa", "cc", "bb"};
    char* const_expected[] = {NULL, "aa", "aa", "bb", "cc", "hello", "world", "zz"};

    ecs_string_t arr[STRING_COUNT];
    ecs_string_t expected[STRING_COUNT];

    const ecs_type_info_t *ti = ecs_get_type_info(world, ecs_id(ecs_string_t));
    ti->hooks.copy_ctor(arr, const_arr, STRING_COUNT, ti);
    ti->hooks.copy_ctor(expected, const_expected, STRING_COUNT, ti);

    /* test "less" */
    test_assert(cmp(&arr[3], &arr[7], ti) < 0); /* "aa < "bb" */
    test_assert(cmp(&arr[2], &arr[5], ti) < 0); /* NULL < "aa" */

    /* test "greater" */
    test_assert(cmp(&arr[6], &arr[5], ti) > 0); /* "cc" > "aa" */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[3], &arr[5], ti) == 0); /* "aa" == "aa" */
    test_assert(cmp(&arr[2], &arr[2], ti) == 0); /* NULL == NULL */

    /* test "equal" via equals hook */
    test_assert(equals(&arr[3], &arr[5], ti)); /* "aa" == "aa" */
    test_assert(equals(&arr[2], &arr[2], ti)); /* NULL == NULL */

    /* further test by sorting the array */
    sort_array(ti, arr, STRING_COUNT);

    int i;
    for(i = 0; i < STRING_COUNT; i++) {
        test_assert(str_equals(arr[i], expected[i]));
    }

    ti->hooks.dtor(arr, STRING_COUNT, ti);
    ti->hooks.dtor(expected, STRING_COUNT, ti);

    ecs_fini(world);
}

void PrimitiveCompare_const_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t const_string = ecs_lookup(world, "flecs.core.const_string_t");

    char* arr[] = {"world", "hello", NULL, "aa", "zz", "aa", "cc", "bb"};
    char* expected[] = {NULL, "aa", "aa", "bb", "cc", "hello", "world", "zz"};

    const ecs_type_info_t *ti = ecs_get_type_info(world, const_string);


    /* test "less" */
    test_assert(cmp(&arr[3], &arr[7], ti) < 0); /* "aa < "bb" */
    test_assert(cmp(&arr[2], &arr[5], ti) < 0); /* NULL < "aa" */

    /* test "greater" */
    test_assert(cmp(&arr[6], &arr[5], ti) > 0); /* "cc" > "aa" */

    /* test "equal" via cmp hook */
    test_assert(cmp(&arr[3], &arr[5], ti) == 0); /* "aa" == "aa" */
    test_assert(cmp(&arr[2], &arr[2], ti) == 0); /* NULL == NULL */

    /* test "equal" via equals hook */
    test_assert(equals(&arr[3], &arr[5], ti)); /* "aa" == "aa" */
    test_assert(equals(&arr[2], &arr[2], ti)); /* NULL == NULL */

    /* further test by sorting the array */
    sort_array(ti, arr, STRING_COUNT);

    int i;
    for(i = 0; i < STRING_COUNT; i++) {
        test_assert(str_equals(arr[i], expected[i]));
    }

    ecs_fini(world);
}
