# DEVLOG

## 2026-10-18 — portability and resilience pass

**Done**
- Dropped `dlclose()`. Tested empirically: a library that forks is unaffected
  either way (child has its own address space; parent destructor runs at exit
  instead of at dlclose). A library that leaves a thread executing its own code
  dies with SIGSEGV when dlclose unmaps it. The original loader only looked safe
  because it raced the fault with exit.
- Converted elfLoad.c from CRLF to LF; `.gitattributes` enforces LF.
- Replaced the object→function pointer cast with the POSIX `*(void **)&fn`
  idiom (ISO C forbids the cast, `-Wpedantic` warned).
- Bare filenames are now resolved in cwd by prefixing `./` when the file exists,
  since dlopen otherwise searches the library path.
- Makefile (`CC ?= cc`), test.sh covering happy/error/fork paths plus an
  aarch64 cross build under qemu-user when the toolchain is present.

**Skipped**
- Entry-point extensions (return value, argv passthrough). User wants plain
  `void fn(void)` POSIX C.
- macOS/Windows. ELF only.
