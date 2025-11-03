// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/DetectDefaultTriple.h"
#include <cstdio>
#include <string>

/*
 * Function: detectDefaultTriple
 * Inputs:
 *  - clangPath: Path to clang executable to query
 * Outputs:
 *  - std::string: Effective IR -triple parsed from clang's -### output
 * Theory of operation:
 *  - Spawns 'clang -### -S -x ir - -o /dev/null' and scans its stderr for a
 *    "-triple" argument, returning the value when found.
 */
std::string detectDefaultTriple(const std::string& clangPath) {
    std::string triple;
    const std::string cmd = clangPath + " -### -S -x ir - -o /dev/null 2>&1";
    if (FILE *pipe = popen(cmd.c_str(), "r")) {
        char buf[256];
        std::string out;
        while (const size_t n = fread(buf, 1, sizeof(buf), pipe)) out.append(buf, buf + n);
        pclose(pipe);
        if (const auto pos = out.find("\"-triple\""); pos != std::string::npos) {
            if (const auto q1 = out.find('"', pos + 9); q1 != std::string::npos) {
                if (const auto q2 = out.find('"', q1 + 1); q2 != std::string::npos && q2 > q1 + 1) {
                    triple = out.substr(q1 + 1, q2 - (q1 + 1));
                }
            }
        }
    }
    return triple;
}
