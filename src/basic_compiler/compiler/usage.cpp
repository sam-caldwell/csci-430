// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Usage.h"
#include <iostream>

/**
 * Function: usage
 * Inputs:
 *  - argv0: Program name to display in the help text
 * Outputs:
 *  - void (writes usage/help to stderr)
 * Theory of operation:
 *  - Prints a short synopsis describing supported flags and behavior.
 */
void usage(const char* argv0) {
    std::cerr << "Usage: " << argv0 << " <input.bas> [-ll out.ll] [--bc out.bc] [-o out.exe] [--asm out.asm] [--target <triple>]\n";
    std::cerr << "  -h, --help  : Show this help and exit.\n";
    std::cerr << "  --ll <file>   : Write textual LLVM IR (.ll) to <file>\n";
    std::cerr << "  --bc <file>  : Write LLVM bitcode (.bc) to <file>\n";
    std::cerr << "  -o <file>    : Link a native executable to <file>\n";
    std::cerr << "  --asm <file> : Emit assembly (.asm) for the chosen --target\n";
    std::cerr << "  --target <triple>: aarch64-linux-gnu, x86_64-linux-gnu (default host).\n";
    std::cerr << "  --lex-log, --syntax-log, --semantic-log, --log control phase logs.\n";
    std::cerr << "  Without -ll/--bc/-o/--asm, prints LLVM IR to stdout.\n";
    std::cerr << "  Supported targets: x86_64 or arm64/aarch64 on Linux/macOS (Darwin). FreeBSD and Android are also allowed.\n";
}

