// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <initializer_list>
#include <optional>
#include <string>

namespace gwbasic::cli {

/**
 * Function: takeOptValue (multi-name overload)
 * Purpose:
 *  - Convenience wrapper that supports multiple acceptable flag names
 *    for a single-value option (e.g., {"-ll", "--ll"}).
 * Inputs:
 *  - arg: Current token (argv[i])
 *  - names: List of acceptable flag names to match against
 *  - i: Index into argv; incremented if a value is consumed
 *  - argc/argv: Argument vector from main
 *  - out: Destination optional to receive the consumed string value
 * Outputs:
 *  - bool: true if any of the names matched and a value was consumed
 * Theory of operation:
 *  - Iterates over 'names', delegating to the single-name takeOptValue.
 */
bool takeOptValue(const std::string& arg,
                  std::initializer_list<const char*> names,
                  int& i,
                  int argc,
                  char** argv,
                  std::optional<std::string>& out);

} // namespace gwbasic::cli

