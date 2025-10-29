// File: src/hello_world/main.c
// (c) 2025 Sam Caldwell.  All Rights Reserved.
// Purpose: Implementation of hello_world module functions.
//          Provides string-returning helper for unit testing and sample app.

#include "hello_world/hello_world.h"

/**
 * Return the canonical "Hello, World" string.
 *
 * Inputs:
 *  - none
 *
 * Outputs:
 *  - const char*: Pointer to a statically allocated C string literal.
 *
 * Purpose:
 *  - Used by the sample program and unit tests to validate build and linkage.
 */
const char* get_hello_world(void) {
    return "Hello, World";
}
