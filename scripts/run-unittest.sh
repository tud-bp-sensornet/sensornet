#!/usr/bin/env bash

# contiki unit tests do not correctly return zero exit codes on success.
# so use this as a wrapper for running them.


if [ $# != 1 ]; then
	echo "usage: run-test.sh path/to/test.test.native"
	exit 1
fi

out="$(mktemp)"

$1 2>&1| tee "${out}"


if grep -q "Result: failure" "${out}"; then
   echo "test failure detected"
   grep --color -E "Result: failure|$" "${out}"
   exit 1
fi

exit 0

