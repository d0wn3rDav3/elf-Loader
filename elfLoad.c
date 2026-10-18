/*
 * elfLoad.c - minimal shared-object loader for exercising a library's
 * entry point without writing a per-library harness.
 *
 * Build:   make            (or: cc -o elfLoader elfLoad.c -ldl)
 * Target:  cc -shared -fPIC -o target.so target.c
 * Run:     ./elfLoader target.so entry_function
 *
 * Entry point contract: void entry(void).
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <shared_object_file> <entry_point_function>\n",
                argc > 0 && argv[0] ? argv[0] : "elfLoader");
        return 1;
    }

    const char *sharedObjectName = argv[1];
    const char *entryPointName = argv[2];

    /* dlopen() treats a name with no '/' as a library-search-path lookup,
     * not a file in the current directory, so "elfLoader hello.so f" fails
     * even when hello.so is right here. If it exists in cwd, prefix "./". */
    char localPath[4096];
    if (strchr(sharedObjectName, '/') == NULL && access(sharedObjectName, F_OK) == 0) {
        int n = snprintf(localPath, sizeof localPath, "./%s", sharedObjectName);
        if (n > 0 && (size_t)n < sizeof localPath)
            sharedObjectName = localPath;
    }

    dlerror(); /* clear stale error state */
    void *sharedObject = dlopen(sharedObjectName, RTLD_NOW);
    if (!sharedObject) {
        const char *error = dlerror();
        fprintf(stderr, "Error loading %s: %s\n", sharedObjectName,
                error ? error : "unknown error");
        return 1;
    }

    /* POSIX: a NULL return from dlsym is ambiguous, so clear dlerror()
     * beforehand and check it afterwards rather than testing the pointer alone. */
    dlerror();
    void (*entryPoint)(void);
    /* ISO C forbids object-pointer -> function-pointer casts; this is the
     * POSIX-sanctioned way to assign a dlsym() result. */
    *(void **)(&entryPoint) = dlsym(sharedObject, entryPointName);
    const char *error = dlerror();
    if (error != NULL || entryPoint == NULL) {
        fprintf(stderr, "Error finding entry point %s: %s\n",
                entryPointName, error ? error : "symbol is NULL");
        return 1;
    }

    entryPoint();

    /* Deliberately no dlclose(): the process exits next, so unmapping buys
     * nothing. If the library left a thread running in its own code,
     * dlclose() unmaps that code underneath it and the process dies with
     * SIGSEGV. Forked children are unaffected either way. Destructors still
     * run at exit. */
    return 0;
}
