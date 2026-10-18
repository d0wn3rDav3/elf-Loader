/* 
 * elfLoad.c should be compiled with:
 * "gcc -o elfLoader elfLoad.c -ldl"
 *
 * Target libraries should be Compiled with:
 * "gcc -shared -o $target.so -fPIC $target.c"
*/

#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <shared_object_file> <entry_point_function>\n", argv[0]);
        return 1;
    }

    // Which .so we're attempting to load from the cmd line.
    // This can be hard coded outside of testing if appropriate
    const char* sharedObjectName = argv[1];
    
    // Get the entry point function name from command line
    const char* entryPointName = argv[2];

    // Load the shared object
    dlerror(); // clear any stale error state from prior dl* calls
    void* sharedObject = dlopen(sharedObjectName, RTLD_NOW);
    if (!sharedObject) {
        fprintf(stderr, "Error loading %s: %s\n", sharedObjectName, dlerror());
        return 1;
    }

    // Find the entry point we're attempting to call.
    // POSIX: a NULL return from dlsym is ambiguous, so clear dlerror()
    // beforehand and check it afterwards rather than testing the pointer alone.
    dlerror();
    void* symbol = dlsym(sharedObject, entryPointName);
    char* error = dlerror();
    if (error != NULL || symbol == NULL) {
        fprintf(stderr, "Error finding entry point %s: %s\n",
                entryPointName, error ? error : "symbol is NULL");
        // Unload the shared object
        dlclose(sharedObject);
        return 1;
    }
    void (*entryPoint)() = (void (*)())symbol;

    // Call the specified entry point.
    entryPoint();

    // Unload the shared object
    // For most of our libraries, they'll fork into anoter process
    dlclose(sharedObject);

    return 0;
}
