# elf-Loader

Minimal loader for exercising a shared object's entry point without writing a
per-library harness. Plain POSIX C, no dependencies beyond libc and libdl.

## Build

    make                # builds elfLoader and the hello.so example
    make CC=clang       # any compiler works; cross compilers too
    make test           # runs test.sh
    make clean

Or by hand:

    cc -o elfLoader elfLoad.c -ldl
    cc -shared -fPIC -o target.so target.c

## Use

    ./elfLoader <shared_object> <entry_function>
    ./elfLoader hello.so _daveMain

The entry function must have the signature `void entry(void)`. Exit status is
0 if the entry function was called, 1 on usage, load, or symbol errors, with
the dlerror() text printed to stderr.

A bare filename such as `hello.so` is resolved in the current directory if it
exists there. Otherwise dlopen's normal library search applies, so use a path
containing `/` to load from anywhere else.

## Notes

- The loader never calls `dlclose()`. The process exits immediately after the
  entry point returns, and unmapping a library while one of its threads is
  still running crashes the process. Forked children are unaffected either
  way, and library destructors still run at exit.
- Tested with gcc, clang, and an aarch64 cross build under qemu-user. See
  `test.sh`.
