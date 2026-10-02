// do what thou wilt shall be the whole of the law

#include "ctest.h"
#include <stddef.h>

ctest_dcl(bob) {
   ctest_assert_true(1);
   ctest_assert_eq_s32(2, 2);
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

// Exercises the C11 _Generic ctest_assert_eq() dispatch across the integer,
// floating-point and pointer type cases, plus the exact (non-eps) float
// comparisons. All comparisons here are expected to pass.
ctest_dcl(generic_eq) {
   int i = 42;
   long l = -7;
   unsigned int u = 7u;
   unsigned long long big = 0x100000000ull;
   float f = 0.5f;
   double d = 0.25;
   const char* s = "hello";
   int arr[2] = {0, 0};
   int* p = arr;

   ctest_assert_eq(42, i);
   ctest_assert_eq(-7L, l);
   ctest_assert_eq(7u, u);
   ctest_assert_eq(0x100000000ull, big);
   ctest_assert_eq(0.5f, f);
   ctest_assert_eq(0.25, d);
   ctest_assert_eq(s, s); // char*/const char* in _Generic compares pointers, not contents
   ctest_assert_eq(p, p); // any other pointer type is caught by the _Generic default arm

   // Exact float comparisons: values that must be produced bit-for-bit.
   ctest_assert_eq_f32(0.0f, f - 0.5f);
   ctest_assert_eq_f64(1.0, d * 4.0);
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