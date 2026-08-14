#!/bin/sh
# do what thou wilt shall be the whole of the law
set -eu

fail() {
  echo "FAIL: $1" >&2
  exit 1
}

# Ensure expected tests are visible in registry output.
list_output="$(./ctest_test --list)"
printf '%s\n' "$list_output" | grep -qx "bob" || fail "--list missing bob"
printf '%s\n' "$list_output" | grep -qx "anna" || fail "--list missing anna"
printf '%s\n' "$list_output" | grep -qx "rita" || fail "--list missing rita"
printf '%s\n' "$list_output" | grep -qx "milo" || fail "--list missing milo"

# Direct named execution should succeed for existing names.
./ctest_test bob >/dev/null
./ctest_test milo >/dev/null

# Direct named execution should fail for known failing tests.
set +e
./ctest_test rita >/dev/null 2>/dev/null
exit_code=$?
set -e
[ "$exit_code" -eq 1 ] || fail "expected exit code 1 for failing test rita"

# Missing names must be preflight-checked and return bad exit code.
stdout_file=".ctest_stdout.tmp"
stderr_file=".ctest_stderr.tmp"
trap 'rm -f "$stdout_file" "$stderr_file"' EXIT

set +e
./ctest_test bob missing_name >"$stdout_file" 2>"$stderr_file"
exit_code=$?
set -e

[ "$exit_code" -eq 2 ] || fail "expected exit code 2 for missing test"
[ ! -s "$stdout_file" ] || fail "expected no stdout when preflight validation fails"
grep -qx "unknown test: missing_name" "$stderr_file" || fail "missing unknown-test error"
grep -qx "aborting: 1 requested test(s) not found" "$stderr_file" || fail "missing abort summary"

# Trailing '*' should match by prefix.
wildcard_list="$(./ctest_test --list | grep '^r')"
set +e
./ctest_test 'r*' >/dev/null 2>/dev/null
exit_code=$?
set -e
[ "$exit_code" -eq 1 ] || fail "expected exit code 1 running 'r*' (matches failing rita)"
printf '%s\n' "$wildcard_list" | grep -qx "rita" || fail "'r*' should have matched rita"

# Overlapping-prefix wildcards: 'test_a*' should match test_ab/test_abc but
# not test_b or tst_a.
wildcard_output="$(./ctest_test 'test_a*')"
printf '%s\n' "$wildcard_output" | grep -q '\[PASS\] test_ab ' || fail "'test_a*' should have run test_ab"
printf '%s\n' "$wildcard_output" | grep -q '\[PASS\] test_abc ' || fail "'test_a*' should have run test_abc"
if printf '%s\n' "$wildcard_output" | grep -q '\[PASS\] test_b '; then
  fail "'test_a*' should not have run test_b"
fi
if printf '%s\n' "$wildcard_output" | grep -q '\[PASS\] tst_a '; then
  fail "'test_a*' should not have run tst_a"
fi

tst_a_output="$(./ctest_test 'tst_a*')"
printf '%s\n' "$tst_a_output" | grep -q '\[PASS\] tst_a ' || fail "'tst_a*' should have run tst_a"
if printf '%s\n' "$tst_a_output" | grep -q '\[PASS\] test_ab '; then
  fail "'tst_a*' should not have run test_ab"
fi

# A failure inside a matched group must not stop the rest of the group from
# running, and a summary line should be printed for named/wildcard runs too.
set +e
group_output="$(./ctest_test 'grp_*')"
exit_code=$?
set -e
[ "$exit_code" -eq 1 ] || fail "expected exit code 1 when 'grp_*' includes a failing test"
printf '%s\n' "$group_output" | grep -q '\[FAIL\] grp_fail ' || fail "'grp_*' should have run grp_fail"
printf '%s\n' "$group_output" | grep -q '\[PASS\] grp_pass ' || fail "'grp_*' should still run grp_pass after grp_fail"
printf '%s\n' "$group_output" | grep -qx "1/2 tests passed" || fail "'grp_*' should print a 1/2 summary line"

# -h/--help should print usage and exit 0.
./ctest_test --help | grep -q "Usage:" || fail "--help missing usage text"
./ctest_test -h >/dev/null || fail "-h should exit 0"

# -v/--verbose should print each test name before it runs; without it, no
# such line appears. The flag should also work combined with a test name.
verbose_output="$(./ctest_test -v bob)"
printf '%s\n' "$verbose_output" | grep -qx "\[RUN \] bob" || fail "-v should print '[RUN ] bob' before running"

quiet_output="$(./ctest_test bob)"
if printf '%s\n' "$quiet_output" | grep -q '\[RUN \]'; then
  fail "test run without -v should not print '[RUN ]' lines"
fi

verbose_all_output="$(./ctest_test --verbose milo)"
printf '%s\n' "$verbose_all_output" | grep -qx "\[RUN \] milo" || fail "--verbose should print '[RUN ] milo' before running"

echo "PASS: ctest CLI behavior"
