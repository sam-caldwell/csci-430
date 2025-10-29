// File: src/hello_world/app.c
// (c) 2025 Sam Caldwell.  All Rights Reserved.
// Purpose: Minimal hello_world executable to verify toolchain.

#include <stdio.h>
#include "hello_world/hello_world.h"

/*
 * Function: main
 * Inputs:
 *  - argc/argv: Unused
 * Outputs:
 *  - int: Exit status (0=success)
 * Theory of operation:
 *  - Prints the string returned by get_hello_world() to stdout.
 */
int main(int argc, char** argv) {
    (void)argc; (void)argv;
    puts(get_hello_world());
    return 0;
}

