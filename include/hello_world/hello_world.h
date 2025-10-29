// File: include/hello_world/hello_world.h
// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the canonical "Hello, World" string.
 *
 * Inputs:
 *  - none
 *
 * Outputs:
 *  - const char*: Pointer to a statically allocated C string "Hello, World".
 *
 * Purpose:
 *  - Provide a simple function used by the sample program and tests.
 */
const char* get_hello_world(void);

#ifdef __cplusplus
}
#endif

