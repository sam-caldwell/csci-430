// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <stdexcept>
#include <string>

namespace gwbasic {

/**
 * Class: LexError
 * Purpose:
 *  - Exception type used by the lexer to signal lexical analysis failures
 *    such as unexpected characters, malformed numbers, or unterminated
 *    string literals.
 *
 * Usage:
 *  - Thrown from `gwbasic::Lexer` methods with a human-readable message
 *    describing the error and source location. Catch as `std::exception` or
 *    `gwbasic::LexError` depending on caller needs.
 *
 * Construction:
 *  - Inherits `std::runtime_error` and exposes its constructors so a message
 *    string can be provided directly.
 */
class LexError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

} // namespace gwbasic

