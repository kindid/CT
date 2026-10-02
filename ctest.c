// do what thou wilt shall be the whole of the law

#include "ctest.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct ctest_test;

struct ctest_results {
   const struct ctest_test* test;
   uint32_t assertions;
   uint32_t failures;
};

struct ctest_test {
   const char* name;
   void (*fn)(struct ctest_results* ctest_results__);
   struct ctest_test* next;
};

static struct ctest_test * head = NULL;
static struct ctest_test * tail = NULL;
static int ctest_verbose = 0;

const char* ctest_results_test_name(const struct ctest_results* ctest_results__)
{
   assert(ctest_results__ != NULL);
   assert(ctest_results__->test != NULL);
   return ctest_results__->test->name;
}

static struct ctest_results ctest_results_begin(const struct ctest_test* test)
{
   struct ctest_results ctest_results__;
   assert(test != NULL);
   assert(test->name != NULL);
   ctest_results__.test = test;
   ctest_results__.assertions = 0u;
   ctest_results__.failures = 0u;
   return ctest_results__;
}

static int ctest_results_ok(const struct ctest_results* ctest_results__)
{
   return ctest_results__ && ctest_results__->failures == 0u;
}

static void ctest_results_report(const struct ctest_results* ctest_results__)
{
   assert(ctest_results__ != NULL);
   assert(ctest_results__->test != NULL);
   assert(ctest_results__->test->name != NULL);

   if (ctest_results__->failures == 0u) {
      printf("[PASS] %s (%u assertions)\n", ctest_results__->test->name, ctest_results__->assertions);
   } else {
      printf("[FAIL] %s (%u failures / %u assertions)\n",
         ctest_results__->test->name,
         ctest_results__->failures,
         ctest_results__->assertions);
   }
}

static int ctest_f32_near(float expected, float actual, float eps)
{
   return fabsf(expected - actual) <= eps;
}

static int ctest_f64_near(double expected, double actual, double eps)
{
   return fabs(expected - actual) <= eps;
}

static int ctest_str_eq(const char* a, const char* b)
{
   if (a == b) {
      return 1;
   }
   if (!a || !b) {
      return 0;
   }
   return strcmp(a, b) == 0;
}

static void ctest_failv(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, va_list args)
{
   assert(ctest_results__ != NULL);
   ctest_results__->failures += 1u;
   fprintf(stderr, "  %s:%d: ", file, line);
   vfprintf(stderr, fmt, args);
   fputc('\n', stderr);
}

static void ctest_failf(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, ...) CTEST_PRINTF_FMT(4, 5);

static void ctest_failf(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   ctest_failv(ctest_results__, file, line, fmt, args);
   va_end(args);
}

void ctest_assert_true_impl(struct ctest_results* ctest_results__, int cond, const char* cond_text, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (!cond) {
      ctest_failf(ctest_results__, file, line, "assert true failed: %s", cond_text);
   }
}

void ctest_assert_eq_s64_impl(struct ctest_results* ctest_results__, int64_t expected, int64_t actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (expected != actual) {
      ctest_failf(ctest_results__, file, line, "assert eq (signed) failed: expected=%lld actual=%lld", (long long)expected, (long long)actual);
   }
}

void ctest_assert_eq_u64_impl(struct ctest_results* ctest_results__, uint64_t expected, uint64_t actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (expected != actual) {
      ctest_failf(ctest_results__, file, line, "assert eq (unsigned) failed: expected=%llu actual=%llu", (unsigned long long)expected, (unsigned long long)actual);
   }
}

void ctest_assert_eq_f32_impl(struct ctest_results* ctest_results__, float expected, float actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (expected != actual) {
      ctest_failf(ctest_results__, file, line, "assert f32 (exact) failed: expected=%0.9g actual=%0.9g", (double)expected, (double)actual);
   }
}

void ctest_assert_eq_f64_impl(struct ctest_results* ctest_results__, double expected, double actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (expected != actual) {
      ctest_failf(ctest_results__, file, line, "assert f64 (exact) failed: expected=%0.17g actual=%0.17g", expected, actual);
   }
}

void ctest_assert_eq_f32_eps_impl(struct ctest_results* ctest_results__, float expected, float actual, float eps, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (!ctest_f32_near(expected, actual, eps)) {
      ctest_failf(ctest_results__, file, line, "assert f32 failed: expected=%0.9g actual=%0.9g eps=%0.9g", (double)expected, (double)actual, (double)eps);
   }
}

void ctest_assert_eq_f64_eps_impl(struct ctest_results* ctest_results__, double expected, double actual, double eps, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (!ctest_f64_near(expected, actual, eps)) {
      ctest_failf(ctest_results__, file, line, "assert f64 failed: expected=%0.17g actual=%0.17g eps=%0.17g", expected, actual, eps);
   }
}

void ctest_assert_eq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (!ctest_str_eq(expected, actual)) {
      ctest_failf(ctest_results__, file, line, "assert eq_str failed: expected=\"%s\" actual=\"%s\"",
         expected ? expected : "(null)", actual ? actual : "(null)");
   }
}

void ctest_assert_neq_str_impl(struct ctest_results* ctest_results__, const char* expected, const char* actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (ctest_str_eq(expected, actual)) {
      ctest_failf(ctest_results__, file, line, "assert neq_str failed: both equal \"%s\"", expected ? expected : "(null)");
   }
}

void ctest_assert_eq_ptr_impl(struct ctest_results* ctest_results__, const void* expected, const void* actual, const char* file, int line)
{
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   if (expected != actual) {
      ctest_failf(ctest_results__, file, line, "assert eq (pointer) failed: expected=%p actual=%p", (void*)expected, (void*)actual);
   }
}

void ctest_fail_impl(struct ctest_results* ctest_results__, const char* file, int line, const char* fmt, ...)
{
   va_list args;
   assert(ctest_results__ != NULL);
   ctest_results__->assertions += 1u;
   va_start(args, fmt);
   ctest_failv(ctest_results__, file, line, fmt, args);
   va_end(args);
}

static int ctest_run_one(struct ctest_test* test)
{
   assert(test != NULL);
   assert(test->name != NULL);
   if (ctest_verbose) {
      printf("[RUN ] %s\n", test->name);
   }
   struct ctest_results ctest_results__ = ctest_results_begin(test);
   test->fn(&ctest_results__);
   ctest_results_report(&ctest_results__);
   return ctest_results_ok(&ctest_results__) ? 0 : 1;
}

// A pattern ending in '*' matches any test name sharing that prefix;
// otherwise the pattern must match the test name exactly.
static int ctest_name_matches(const char* pattern, const char* name)
{
   size_t plen = strlen(pattern);
   if (plen > 0 && pattern[plen - 1] == '*') {
      return strncmp(name, pattern, plen - 1) == 0;
   }
   return strcmp(pattern, name) == 0;
}

struct ctest_run_stats {
   int total;
   int passed;
};

static int ctest_any_match(const char* pattern)
{
   struct ctest_test* test = head;
   while (test) {
      if (ctest_name_matches(pattern, test->name)) {
         return 1;
      }
      test = test->next;
   }
   return 0;
}

// Runs every registered test matching pattern, in registration order, and
// never stops early on failure -- one bad test can't hide results for the
// rest of a named/wildcard group.
static struct ctest_run_stats ctest_run_matching(const char* pattern)
{
   struct ctest_run_stats stats;
   struct ctest_test* test = head;
   stats.total = 0;
   stats.passed = 0;
   while (test) {
      if (ctest_name_matches(pattern, test->name)) {
         stats.total++;
         if (ctest_run_one(test) == 0) {
            stats.passed++;
         }
      }
      test = test->next;
   }
   return stats;
}

void ctest_register(const char* name, void (*fn)(struct ctest_results* ctest_results__))
{
   struct ctest_test* test = (struct ctest_test*)malloc(sizeof(struct ctest_test));
   if (!test) {
      fprintf(stderr, "ctest_register: out of memory registering test '%s'\n", name);
      abort();
   }
   test->name = name;
   test->fn = fn;
   test->next = NULL;
   if (tail) {
      tail->next = test;
   } else {
      head = test;
   }
   tail = test;
}

// A bare "*" pattern matches every test (strncmp with length 0 always
// compares equal), so the full-suite run is just that pattern.
static int ctest_run_all(void)
{
   struct ctest_run_stats stats = ctest_run_matching("*");
   printf("%d/%d tests passed\n", stats.passed, stats.total);
   return stats.passed == stats.total ? 0 : 1;
}

static void ctest_list(void)
{
   struct ctest_test* test = head;
   while (test) {
      printf("%s\n", test->name);
      test = test->next;
   }
}

static void ctest_print_usage(const char* argv0)
{
   printf("Usage: %s [-h|--help] [-v|--verbose] [--list] [name|prefix*...]\n", argv0);
   printf("  (no args)     run all registered tests\n");
   printf("  --list        list all registered test names\n");
   printf("  name...       run only the named test(s)\n");
   printf("  prefix*       run tests whose name starts with prefix (bare '*' runs all)\n");
   printf("  -v, --verbose print each test name as it starts running\n");
   printf("  -h, --help    show this help message\n");
}

// Exit codes: 0 = every requested test (or all tests, if none named) passed;
// 1 = at least one test ran and failed; 2 = a requested name/pattern matched
// no registered test, so nothing was run.
int main(int argc, char** argv)
{
   // Strip -v/--verbose out of argv wherever it appears, leaving the
   // remaining arguments (flags/test names/patterns) in their original order.
   {
      int i = 0;
      int out = 1;
      for (i = 1; i < argc; i++) {
         if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            ctest_verbose = 1;
         } else {
            argv[out++] = argv[i];
         }
      }
      argc = out;
   }

   if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
      ctest_print_usage(argv[0]);
      return 0;
   }

   if (argc == 2 && strcmp(argv[1], "--list") == 0) {
      ctest_list();
      return 0;
   }

   if (argc > 1) {
      int i = 0;
      int missing_count = 0;
      struct ctest_run_stats stats;
      stats.total = 0;
      stats.passed = 0;

      // Preflight every requested test name/pattern before executing anything.
      for (i = 1; i < argc; i++) {
         if (!ctest_any_match(argv[i])) {
            fprintf(stderr, "unknown test: %s\n", argv[i]);
            missing_count++;
         }
      }

      if (missing_count > 0) {
         fprintf(stderr, "aborting: %d requested test(s) not found\n", missing_count);
         return 2;
      }

      // Every matching test runs even if an earlier one fails.
      for (i = 1; i < argc; i++) {
         struct ctest_run_stats s = ctest_run_matching(argv[i]);
         stats.total += s.total;
         stats.passed += s.passed;
      }
      printf("%d/%d tests passed\n", stats.passed, stats.total);
      return stats.passed == stats.total ? 0 : 1;
   }

   return ctest_run_all();
}
