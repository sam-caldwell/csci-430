// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>

namespace gwbasic {

/**
 * Class: ParseError
 * Purpose:
 *  - Dedicated exception type used by the parser to signal syntactic errors
 *    (unexpected tokens, missing delimiters/keywords, malformed structures).
 * Usage:
 *  - Throw when the grammar expectation fails and parsing cannot proceed.
 *  - Unit tests and callers catch this to assert error conditions.
 * Notes:
 *  - Stores a human-readable message describing the failure and location.
 */
class ParseError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error; // inherit constructors
};

} // namespace gwbasic
