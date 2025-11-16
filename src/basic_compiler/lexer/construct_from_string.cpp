// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include <memory>
#include <string>
#include <utility>

namespace gwbasic {

/*
 * Function: Lexer::Lexer
 * Summary:
 *  Construct a lexer from a full source string.
 * Parameters:
 *  - source: Complete GW-BASIC program text
 * Returns:
 *  - none
 */
Lexer::Lexer(std::string source)
    : src_(std::move(source)) {}

} // namespace gwbasic
