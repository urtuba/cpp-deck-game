#!/bin/sh
# Run every tests/cases/NAME.game against the program given as $1.
# The program gets NAME.game as its only argument, unless the file NAME.args
# exists: then its words are the arguments instead (the .game file is
# ignored). The program runs inside tests/cases, so paths in .args and in
# error messages are relative to that directory.
# Compares stdout with NAME.out, the exit status with NAME.code (default 0)
# and stderr with NAME.err (only if the file exists).

bin=${1:?usage: run.sh PROGRAM}
bin=$(cd "$(dirname "$bin")" && pwd)/$(basename "$bin")
dir=$(dirname "$0")/cases
tmp=$(mktemp -d "${TMPDIR:-/tmp}/deck-test.XXXXXX") || exit 1
trap 'rm -rf "$tmp"' EXIT
cd "$dir" || exit 1

pass=0
fail=0
for game in *.game; do
    name=$(basename "$game" .game)
    ok=1

    if [ -f "$name.args" ]; then
        set -f
        set -- $(cat "$name.args")
        set +f
    else
        set -- "$game"
    fi
    "$bin" "$@" > "$tmp/out" 2> "$tmp/err"
    status=$?

    expected_code=0
    if [ -f "$name.code" ]; then
        expected_code=$(cat "$name.code")
    fi

    if ! cmp -s "$tmp/out" "$name.out"; then
        echo "  stdout differs"
        ok=0
    fi
    if [ "$status" -ne "$expected_code" ]; then
        echo "  exit status $status, expected $expected_code"
        ok=0
    fi
    if [ -f "$name.err" ] && ! cmp -s "$tmp/err" "$name.err"; then
        echo "  stderr differs"
        ok=0
    fi

    if [ "$ok" -eq 1 ]; then
        echo "PASS $name"
        pass=$((pass + 1))
    else
        echo "FAIL $name"
        fail=$((fail + 1))
    fi
done

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
