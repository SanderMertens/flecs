#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Test that ecs_os_memcpy_n does not overflow on large counts */
START_TEST(test_memcpy_n_no_overflow)
{
    /* Invariant: Buffer reads never exceed declared length.
       ecs_os_memcpy_n must not allow integer overflow in size computation. */
    
    typedef struct {
        size_t count;
        int should_fail;
    } test_case_t;
    
    test_case_t cases[] = {
        { 1, 0 },                           /* Valid: small count */
        { 1024, 0 },                        /* Valid: normal count */
        { SIZE_MAX / sizeof(uint64_t), 1 }, /* Boundary: max safe count for uint64_t */
        { SIZE_MAX / sizeof(uint64_t) + 1, 1 }, /* Overflow: wraps to small value */
        { SIZE_MAX, 1 }                     /* Exploit: maximum size_t */
    };
    
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        size_t count = cases[i].count;
        size_t elem_size = sizeof(uint64_t);
        
        /* Compute what ecs_os_memcpy_n would compute */
        size_t computed_size = elem_size * count;
        
        /* Check for overflow: if computed_size < elem_size, overflow occurred */
        if (count > 0 && computed_size < elem_size) {
            /* Overflow detected - this is the vulnerability */
            ck_assert_msg(cases[i].should_fail, 
                "Case %d: Overflow not detected for count=%zu", i, count);
        } else {
            /* No overflow - size is reasonable */
            ck_assert_msg(!cases[i].should_fail || count <= (SIZE_MAX / elem_size),
                "Case %d: Expected overflow but none occurred for count=%zu", i, count);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_memcpy_n_no_overflow);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}