// File: src/main.c
// (c) 2025 Sam Caldwell.  All Rights Reserved.
// Purpose: Minimal C entry point that prints "Hello, World" (with quotes).
// Purpose: First C project target named 'hello_world'.
#include <stdio.h>
#include "hello_world.h"

int main(void) {
    printf("\"%s\"\n", get_hello_world());
    return 0;
}
