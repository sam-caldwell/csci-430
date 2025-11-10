// (c) 2025 Sam Caldwell. All Rights Reserved.
#ifndef BASIC_COMPILER_USAGE_H
#define BASIC_COMPILER_USAGE_H

/**
 * Function: usage
 * Inputs:
 *  - argv0: Program name to display in the help text
 * Outputs:
 *  - void (writes usage/help to stderr)
 * Theory of operation:
 *  - Prints a short synopsis describing supported flags and behavior.
 */
void usage(const char* argv0);

#endif // BASIC_COMPILER_USAGE_H
