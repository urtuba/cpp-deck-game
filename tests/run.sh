#!/bin/sh
# Run every tests/cases/NAME.game against the program given as $1.
# Compares stdout with NAME.out, the exit status with NAME.code (default 0)
# and stderr with NAME.err (only if the file exists).

bin=${1:?usage: run.sh PROGRAM}
dir=$(dirname "$0")/cases
tmp=$(mktemp -d "${TMPDIR:-/tmp}/deck-test.XXXXXX") || exit 1
trap 'rm -rf "$tmp"' EXIT

pass=0
fail=0
for game in "$dir"/*.game; do
    name=$(basename "$game" .game)
    ok=1

    "$bin" "$game" > "$tmp/out" 2> "$tmp/err"
    status=$?

    expected_code=0
    if [ -f "$dir/$name.code" ]; then
        expected_code=$(cat "$dir/$name.code")
    fi

    if ! cmp -s "$tmp/out" "$dir/$name.out"; then
        echo "  stdout differs"
        ok=0
    fi
    if [ "$status" -ne "$expected_code" ]; then
        echo "  exit status $status, expected $expected_code"
        ok=0
    fi
    if [ -f "$dir/$name.err" ] && ! cmp -s "$tmp/err" "$dir/$name.err"; then
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
