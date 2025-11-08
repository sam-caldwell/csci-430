// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/cli/TakeOptValue.h"

namespace gwbasic::cli {

/*
 * Function: takeOptValue
 * Inputs:
 *  - arg: Current argument token (argv[i])
 *  - name: Exact flag name to match (e.g., "--bc", "-o")
 *  - i: Index into argv; incremented when value is consumed
 *  - argc/argv: Full argument vector
 *  - out: Destination optional receiving the consumed value
 * Outputs:
 *  - bool: true if the flag matched and a value was consumed
 * Theory of operation:
 *  - If 'arg' equals 'name' and a following token exists, stores it in 'out'
 *    and increments 'i' to skip the consumed value; otherwise returns false.
 */
bool takeOptValue(const std::string& arg,
                  const char* name,
                  int& i,
                  const int argc,
                  char** argv,
                  std::optional<std::string>& out) {
    if (arg == name) {
        if (i + 1 < argc) {
            ++i;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            out = argv[i];
            return true;
        }
        // Missing value; do not consume. Let the caller handle error/reporting.
    }
    return false;
}

} // namespace gwbasic::cli
