// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/cli/TakeOptValues.h"
#include "basic_compiler/cli/TakeOptValue.h"
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>

namespace gwbasic::cli {

/*
 * Function: takeOptValue (multi-name overload)
 * Inputs:
 *  - arg: Current argument token (argv[i])
 *  - names: List of acceptable flag names
 *  - i: Index into argv; incremented when value is consumed
 *  - argc/argv: Full argument vector
 *  - out: Destination optional receiving the consumed value
 * Outputs:
 *  - bool: true if any name matched and a value was consumed
 * Theory of operation:
 *  - Iterates 'names', delegating to the single-name overload until one
 *    matches; returns false if none match.
 */
bool takeOptValue(std::string_view arg,
                  std::initializer_list<const char*> names,
                  int& index,
                  const int argc,
                  char** argv,
                  std::optional<std::string>& out) {
    for (const auto* const name : names) {
        if (takeOptValue(arg, name, index, argc, argv, out)) {
            return true;
        }
    }
    return false;
}

} // namespace gwbasic::cli
