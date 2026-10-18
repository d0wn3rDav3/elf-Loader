#!/bin/sh
# Self-check for elfLoader. Run via `make test`.
# Covers: happy path, bare-filename lookup, missing file, missing symbol,
# a forking library, and (if the toolchain is present) an aarch64 cross
# build run under qemu to prove the code is not x86/glibc-specific.
set -u
cd "$(dirname "$0")"
fail=0
check() { # check <description> <expected_rc> <expected_stderr_substring|-> cmd...
    desc=$1; want_rc=$2; want_msg=$3; shift 3
    out=$("$@" 2>&1); rc=$?
    if [ "$rc" -ne "$want_rc" ]; then
        echo "FAIL: $desc (rc=$rc, want $want_rc): $out"; fail=1
    elif [ "$want_msg" != - ] && ! printf '%s' "$out" | grep -q -- "$want_msg"; then
        echo "FAIL: $desc (missing '$want_msg'): $out"; fail=1
    else
        echo "ok:   $desc"
    fi
}

cat > forker.c <<'C'
#include <stdio.h>
#include <unistd.h>
void entry(void) {
    if (fork() == 0) { puts("child alive"); _exit(0); }
    puts("parent returning");
}
C
make -s elfLoader hello.so forker.so || exit 1

check "usage on wrong argc"        1 "Usage"          ./elfLoader
check "happy path, explicit ./"    0 "Hello, world!"  ./elfLoader ./hello.so _daveMain
check "happy path, bare filename"  0 "Hello, world!"  ./elfLoader hello.so _daveMain
check "missing file"               1 "Error loading"  ./elfLoader ./nope.so _daveMain
check "missing symbol"             1 "Error finding"  ./elfLoader ./hello.so nope
check "forking library"            0 "parent returning" ./elfLoader ./forker.so entry

# Cross build: proves portability rather than assuming it. Skipped if absent.
XCC=aarch64-linux-gnu-gcc; QEMU=qemu-aarch64; SYSROOT=/usr/aarch64-linux-gnu
if command -v $XCC >/dev/null && command -v $QEMU >/dev/null && [ -d $SYSROOT ]; then
    $XCC -std=c99 -Wall -Wextra -Wpedantic -o elfLoader.aarch64 elfLoad.c -ldl &&
    $XCC -shared -fPIC -o hello.aarch64.so hello.c &&
    check "aarch64 under qemu" 0 "Hello, world!" $QEMU -L $SYSROOT ./elfLoader.aarch64 ./hello.aarch64.so _daveMain
    rm -f elfLoader.aarch64 hello.aarch64.so
else
    echo "skip: aarch64 cross build (toolchain not found)"
fi

rm -f forker.c forker.so
[ $fail -eq 0 ] && echo "all tests passed"
exit $fail
