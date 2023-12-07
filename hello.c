/* 
 * Target library should be compiled with:
 * "gcc -shared -o hello.so -fPIC hello.c"
 *
*/ 

#include <stdio.h>

void _daveMain() {
    printf("Hello, world!\n");
}
