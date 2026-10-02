// do what thou wilt shall be the whole of the law

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Enables compile-time printf-style format-string checking on ctest_fail()
// (a GCC/Clang extension); expands to nothing on other compilers.
#if defined(__GNUC__) || defined(__clang__)
#define CTEST_PRINTF_FMT(fmt_idx, args_idx) __attribute__((format(printf, fmt_idx, args_idx)))
#else
#define CTEST_PRINTF_FMT(fmt_idx, args_idx)
#endif

struct ctest_results;

void ctest_assert_true_impl(struct ctest_results* ctest_results__, int cond, const char* cond_text, const char* file, int line);
void ctest_assert_eq_s64_impl(struct ctest_results* ctest_results__, int64_t expected, int64_t actual, const char* file, int line);
void ctest_assert_eq_u64_impl(struct ctest_results* ctest_results__, uint64_t expected, uint64_t actual, const char* file, int line);
void ctest_assert_eq_f32_impl(struct ctest_results* ctest_results__, float expected, float actual, const char* file, int line);
void ctest_assert_eq_f64_impl(struct ctest_results* ctest_results__, double expected, double actual, const char* file, int line);
void ctest_assert_eq_f32_eps_impl(struct ctest_results* ctest_results__, float expected, float actual, float eps, const char* file, int line);
void ctest_assert_eq_f64_eps_impl(struct ctest_results* ctest_results__, double expected, double actual, double eps, const char* file, int line);
void ctest_assert_eq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line);
void ctest_assert_neq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line);
void ctest_assert_eq_ptr_impl(struct ctest_results* ctest_results__, const void* expected, const void* actual, const char* file, int line);
void ctest_fail_impl(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, ...) CTEST_PRINTF_FMT(4, 5);
const char* ctest_results_test_name(const struct ctest_results* ctest_results__);

#define ctest_assert_true(cond) do { ctest_assert_true_impl(ctest_results__, !!(cond), #cond, __FILE__, __LINE__); } while (0)
#define ctest_assert_false(cond) do { ctest_assert_true_impl(ctest_results__, !(cond), "!(" #cond ")", __FILE__, __LINE__); } while (0)
// Sized-integer equality. The width in the name is only a spelling: every
// signed width funnels to one int64 comparison and every unsigned width to one
// uint64 comparison. Each operand is first narrowed to the named width (so an
// out-of-range literal wraps exactly as that type would) and then widened for
// the compare/message. Reach for ctest_assert_eq() below and _Generic picks
// the right width for you.
#define ctest_assert_eq_s8(expected, actual)  do { ctest_assert_eq_s64_impl(ctest_results__, (int64_t)(int8_t)(expected),    (int64_t)(int8_t)(actual),    __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_u8(expected, actual)  do { ctest_assert_eq_u64_impl(ctest_results__, (uint64_t)(uint8_t)(expected),  (uint64_t)(uint8_t)(actual),  __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_s16(expected, actual) do { ctest_assert_eq_s64_impl(ctest_results__, (int64_t)(int16_t)(expected),   (int64_t)(int16_t)(actual),   __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_u16(expected, actual) do { ctest_assert_eq_u64_impl(ctest_results__, (uint64_t)(uint16_t)(expected), (uint64_t)(uint16_t)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_s32(expected, actual) do { ctest_assert_eq_s64_impl(ctest_results__, (int64_t)(int32_t)(expected),   (int64_t)(int32_t)(actual),   __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_u32(expected, actual) do { ctest_assert_eq_u64_impl(ctest_results__, (uint64_t)(uint32_t)(expected), (uint64_t)(uint32_t)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_s64(expected, actual) do { ctest_assert_eq_s64_impl(ctest_results__, (int64_t)(expected),            (int64_t)(actual),            __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_u64(expected, actual) do { ctest_assert_eq_u64_impl(ctest_results__, (uint64_t)(expected),           (uint64_t)(actual),           __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_f32_eps(expected, actual, eps) do { ctest_assert_eq_f32_eps_impl(ctest_results__, (float)(expected), (float)(actual), (float)(eps), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_f64_eps(expected, actual, eps) do { ctest_assert_eq_f64_eps_impl(ctest_results__, (double)(expected), (double)(actual), (double)(eps), __FILE__, __LINE__); } while (0)
// Exact (bit-for-bit value) floating-point equality: no tolerance. Reach for
// the _eps variants above when rounding error is expected; use these when a
// value must be produced exactly (e.g. 0.0f pass-through, integer-valued
// results). Note +0.0 == -0.0 compares equal and NaN never compares equal.
#define ctest_assert_eq_f32(expected, actual) do { ctest_assert_eq_f32_impl(ctest_results__, (float)(expected), (float)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_f64(expected, actual) do { ctest_assert_eq_f64_impl(ctest_results__, (double)(expected), (double)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_str(expected, actual) do { ctest_assert_eq_str_impl(ctest_results__, (expected), (actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_neq_str(expected, actual) do { ctest_assert_neq_str_impl(ctest_results__, (expected), (actual), __FILE__, __LINE__); } while (0)
// Type-generic equality (C11 _Generic): dispatches on the type of `actual` to
// the matching typed assertion. Integers compare exactly (widened to 64-bit
// for a correct message regardless of width/signedness); float/double compare
// EXACTLY (use ctest_assert_eq_f32_eps / _f64_eps for a tolerance); every
// pointer type -- including char*/const char* -- compares by IDENTITY, not
// contents, so strings are NOT strcmp'd here (reach for ctest_assert_eq_str()
// for that). All pointers funnel through the `default` arm, which converts
// them to const void* at the call.
//
// _Generic performs NO implicit conversions when selecting: an association
// matches only the exact (lvalue-converted, top-level-qualifier-stripped)
// type, so pointer types cannot be enumerated and there is no C "is_pointer".
// The `default` arm is what makes "any pointer" work -- selection lands there,
// then the normal call-argument conversion turns the pointer into const void*.
// A non-pointer type with no explicit case also lands in `default` and fails
// to compile when it can't convert to const void* (integer-ish types may only
// warn unless built with -Werror).
//
// _Generic must resolve to a plain function (a function-like macro would not
// re-expand after selection), so this calls the *_impl functions directly and
// threads ctest_results__/__FILE__/__LINE__.
#define ctest_assert_eq(expected, actual) do { \
   _Generic((actual), \
      _Bool:              ctest_assert_eq_s64_impl, \
      char:               ctest_assert_eq_s64_impl, \
      signed char:        ctest_assert_eq_s64_impl, \
      short:              ctest_assert_eq_s64_impl, \
      int:                ctest_assert_eq_s64_impl, \
      long:               ctest_assert_eq_s64_impl, \
      long long:          ctest_assert_eq_s64_impl, \
      unsigned char:      ctest_assert_eq_u64_impl, \
      unsigned short:     ctest_assert_eq_u64_impl, \
      unsigned int:       ctest_assert_eq_u64_impl, \
      unsigned long:      ctest_assert_eq_u64_impl, \
      unsigned long long: ctest_assert_eq_u64_impl, \
      float:              ctest_assert_eq_f32_impl, \
      double:             ctest_assert_eq_f64_impl, \
      default:            ctest_assert_eq_ptr_impl \
   )(ctest_results__, (expected), (actual), __FILE__, __LINE__); \
   } while (0)
#define ctest_fail(...) do { ctest_fail_impl(ctest_results__, __FILE__, __LINE__, __VA_ARGS__); } while (0)
// Derived from the ctest_results__ parameter implicitly in scope inside a
// ctest_dcl body (same trick as the assert macros above), so there is no
// shared global state and this stays safe if tests ever run in parallel.
#define ctest_current_test_name() ctest_results_test_name(ctest_results__)

void ctest_register(const char* name, void (*fn)(struct ctest_results* ctest_results__));

// Relies on GCC/Clang running same-priority constructors within one
// translation unit in source (top-to-bottom) order; not an ISO C guarantee,
// but consistent in practice on both compilers. Ordering across translation
// units depends on link order and is not something to rely on.
#define ctest_dcl(name) \
   static void ctest_fn_##name(struct ctest_results* ctest_results__); \
   static void __attribute__((constructor)) ctest_reg_##name(void) { \
      ctest_register(#name, ctest_fn_##name); \
   } \
   static void ctest_fn_##name(struct ctest_results* ctest_results__)

#ifdef __cplusplus
}
#endif
