/* 
 * elfLoad.c should be compiled with:
 * "gcc elfLoader elfLoad.c -ldl"
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
    void* sharedObject = dlopen(sharedObjectName, RTLD_NOW);
    if (!sharedObject) {
        fprintf(stderr, "Error loading %s: %s\n", sharedObjectName, dlerror());
        return 1;
    }

    // Find the entry point we're attempting to call
    void (*entryPoint)() = (void (*)())dlsym(sharedObject, entryPointName);
    if (!entryPoint) {
        fprintf(stderr, "Error finding entry point %s: %s\n", entryPointName, dlerror());
        // Unload the shared object
        dlclose(sharedObject);
        return 1;
    }

    // Call the specified entry point.
    entryPoint();

    // Unload the shared object
    // For most of our libraries, they'll fork into anoter process
    dlclose(sharedObject);

    return 0;
}
