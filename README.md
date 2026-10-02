# ctest

A minimal, dependency-free C11 unit test framework. Tests self-register at
startup (via a GCC/Clang constructor attribute), so there's no test-list to
maintain by hand — just declare a test and it runs.

## Writing tests

Declare a test with `ctest_dcl(name)` followed by a brace-delimited body, in
any `.c` file that includes `ctest.h` and links against `libctest.a`:

```c
#include "ctest.h"

ctest_dcl(bob) {
   ctest_assert_true(1);
   ctest_assert_eq_s32(2, 2);
}
```

The name passed to `ctest_dcl` becomes the test's registered name (used by
`--list` and for running/matching it from the command line). Inside the
body you have access to a set of assertion macros — a failed assertion
records a failure and prints a `file:line` message, but does **not** stop
the rest of the test body from running:

```c
ctest_dcl(rita) {
   ctest_assert_true(0);
   ctest_assert_eq_u32(7u, 9u);
   ctest_assert_eq_f32_eps(1.0f, 2.0f, 1e-6f);
   ctest_assert_eq_str("hello", "world");
   ctest_assert_neq_str("same", "same");
   ctest_fail("manual failure, value=%d", 42);
}
```

Available assertions:

```c
ctest_assert_true(cond)
ctest_assert_false(cond)
ctest_assert_eq(expected, actual)       // C11 _Generic: picks the width/type of `actual`
ctest_assert_eq_s8/u8/s16/u16/s32/u32/s64/u64(expected, actual)
ctest_assert_eq_f32(expected, actual)       // exact, bit-for-bit
ctest_assert_eq_f64(expected, actual)       // exact, bit-for-bit
ctest_assert_eq_f32_eps(expected, actual, eps)
ctest_assert_eq_f64_eps(expected, actual, eps)
ctest_assert_eq_str(expected, actual)   // strcmp, NULL-safe
ctest_assert_neq_str(expected, actual)  // strcmp, NULL-safe
ctest_fail(fmt, ...)                    // unconditional, printf-style
```

`ctest_assert_eq()` dispatches on the type of `actual`: integer types compare
exactly, `float`/`double` compare bit-for-bit (use the `_eps` forms for a
tolerance), and **any pointer** type compares by **pointer identity** -- for
`char*`/`const char*` that means it does *not* `strcmp`, so reach for
`ctest_assert_eq_str()` when you want a content comparison. ``_Generic`` does no
implicit conversion when selecting, so arbitrary pointer types are caught by a
`default:` arm (they convert to `const void*` at the call); a non-pointer type
with no explicit case fails to compile.

`ctest_current_test_name()` returns the name of the test currently running,
useful for assertions or diagnostics that want to reference it:

```c
ctest_dcl(ned) {
   ctest_assert_eq_str("ned", ctest_current_test_name());
}
```

Tests run in the order they were declared (source order, within a single
translation unit). A wildcard test name ending in `*` matches every test
sharing that prefix, which is handy for grouping related tests under a
common prefix:

```c
ctest_dcl(test_ab) {
   ctest_assert_true(1);
}

ctest_dcl(test_abc) {
   ctest_assert_true(1);
}
```

Running `ctest_test test_a*` runs both of the above but not a test named
`test_b`. See [test/ctest_test.c](test/ctest_test.c) for the full set of
example tests used to exercise the framework itself.

## Command line parameters

```
Usage: ctest_test [-h|--help] [-v|--verbose] [--list] [name|prefix*...]
  (no args)     run all registered tests
  --list        list all registered test names
  name...       run only the named test(s)
  prefix*       run tests whose name starts with prefix (bare '*' runs all)
  -v, --verbose print each test name as it starts running
  -h, --help    show this help message
```

Exit codes:

- `0` — every requested test (or all tests, if none were named) passed.
- `1` — at least one test ran and failed.
- `2` — a requested name/pattern matched no registered test, so nothing was
  run.

Multiple names/patterns can be given on the command line at once, and every
matching test always runs — one failing test never prevents the rest of a
named or wildcard group from running.

## Philosophy

`ctest` is deliberately small and simple: no test fixtures, no setup/teardown
hooks, no assertion exceptions or longjmp-based aborts, no dynamic test
discovery beyond constructor-based self-registration. It targets Clang on
macOS and GCC on Linux only, and leans on a couple of well-established
extensions from both compilers (`__attribute__((constructor))` and
`__attribute__((format(printf, ...)))`) rather than working around their
absence elsewhere.

The goal is for the entire framework to be small enough to read and
understand in one sitting, and for writing a new test to require no
ceremony beyond `ctest_dcl(name) { ... }`. Features are only added when they
make everyday test-writing easier (as opposed to covering every possible
testing scenario).

Because simplicity is the priority over stability of interface, future
versions of `ctest` may break source and/or binary compatibility with
existing tests or builds if doing so keeps the framework simpler or easier
to use. There is no commitment to a stable ABI or API across versions.
