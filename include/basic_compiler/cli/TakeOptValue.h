// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <optional>
#include <string>

namespace gwbasic::cli {

/**
 * Function: takeOptValue
 * Purpose:
 *  - Parse a single-value CLI option of the form "--flag <value>" or
 *    "-f <value>", updating the output and advancing the index when found.
 * Inputs:
 *  - arg: Current token (argv[i]) to compare against the expected name
 *  - name: Exact flag name to match (e.g., "--bc" or "-o")
 *  - i: Index into argv; incremented if a value is consumed
 *  - argc/argv: Argument vector from main
 *  - out: Destination optional to receive the consumed string value
 * Outputs:
 *  - bool: true if the flag matched and a value was consumed into 'out'
 * Theory of operation:
 *  - Matches 'arg' to 'name'; if it matches and a following token exists,
 *    stores argv[++i] into 'out' and returns true. Missing values are not
 *    consumed; the caller can issue a usage error.
 */
bool takeOptValue(const std::string& arg,
                  const char* name,
                  int& i,
                  int argc,
                  char** argv,
                  std::optional<std::string>& out);

} // namespace gwbasic::cli

