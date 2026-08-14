// do what thou wilt shall be the whole of the law

#ifndef CTEST_H
#define CTEST_H

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
void ctest_assert_eq_int_impl(struct ctest_results* ctest_results__, int32_t expected, int32_t actual, const char* file, int line);
void ctest_assert_eq_u32_impl(struct ctest_results* ctest_results__, uint32_t expected, uint32_t actual, const char* file, int line);
void ctest_assert_eq_f32_eps_impl(struct ctest_results* ctest_results__, float expected, float actual, float eps, const char* file, int line);
void ctest_assert_eq_f64_eps_impl(struct ctest_results* ctest_results__, double expected, double actual, double eps, const char* file, int line);
void ctest_assert_eq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line);
void ctest_assert_neq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line);
void ctest_fail_impl(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, ...) CTEST_PRINTF_FMT(4, 5);
const char* ctest_results_test_name(const struct ctest_results* ctest_results__);

#define ctest_assert_true(cond) do { ctest_assert_true_impl(ctest_results__, !!(cond), #cond, __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_int(expected, actual) do { ctest_assert_eq_int_impl(ctest_results__, (int32_t)(expected), (int32_t)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_u32(expected, actual) do { ctest_assert_eq_u32_impl(ctest_results__, (uint32_t)(expected), (uint32_t)(actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_f32_eps(expected, actual, eps) do { ctest_assert_eq_f32_eps_impl(ctest_results__, (float)(expected), (float)(actual), (float)(eps), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_f64_eps(expected, actual, eps) do { ctest_assert_eq_f64_eps_impl(ctest_results__, (double)(expected), (double)(actual), (double)(eps), __FILE__, __LINE__); } while (0)
#define ctest_assert_eq_str(expected, actual) do { ctest_assert_eq_str_impl(ctest_results__, (expected), (actual), __FILE__, __LINE__); } while (0)
#define ctest_assert_neq_str(expected, actual) do { ctest_assert_neq_str_impl(ctest_results__, (expected), (actual), __FILE__, __LINE__); } while (0)
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

#endif // CTEST_H
