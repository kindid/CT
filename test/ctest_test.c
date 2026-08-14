// do what thou wilt shall be the whole of the law

#include "ctest.h"
#include <stddef.h>

ctest_dcl(bob) {
   ctest_assert_true(1);
   ctest_assert_eq_int(2, 2);
}

ctest_dcl(anna) {
   ctest_assert_true(1);
   ctest_assert_eq_u32(7u, 7u);
}

ctest_dcl(rita) {
   ctest_assert_true(0);
   ctest_assert_eq_u32(7u, 9u);
   ctest_assert_eq_f32_eps(1.0f, 2.0f, 1e-6f);
   ctest_assert_eq_str("hello", "world");
   ctest_assert_neq_str("same", "same");
   ctest_fail("manual failure, value=%d", 42);
}

ctest_dcl(milo) {
   ctest_assert_eq_str("hello", "hello");
   ctest_assert_neq_str("hello", "world");
   ctest_assert_eq_str(NULL, NULL);
   ctest_assert_neq_str(NULL, "x");
}

ctest_dcl(ned) {
   ctest_assert_eq_str("ned", ctest_current_test_name());
}

// Simple passing tests with overlapping name prefixes, used to exercise
// trailing '*' wildcard matching (e.g. "test_a*" should match test_ab and
// test_abc but not test_b).
ctest_dcl(tst_a) {
   ctest_assert_true(1);
}

ctest_dcl(test_ab) {
   ctest_assert_true(1);
}

ctest_dcl(test_abc) {
   ctest_assert_true(1);
}

ctest_dcl(test_b) {
   ctest_assert_true(1);
}

// Used to verify that running a wildcard-matched group keeps going after an
// internal failure instead of stopping early (grp_fail must run before
// grp_pass, which append-order registration guarantees).
ctest_dcl(grp_fail) {
   ctest_assert_true(0);
}

ctest_dcl(grp_pass) {
   ctest_assert_true(1);
}