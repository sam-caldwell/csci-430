// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/token/TokenType.h"
#include <string_view>

namespace gwbasic {

/*
 * Function: Lexer::lookupKeyword
 * Summary:
 *  Map an uppercase identifier to a keyword token type.
 * Parameters:
 *  - upper: Uppercase candidate identifier
 * Returns:
 *  - TokenType: Matching keyword type, or Identifier if not matched
 */
TokenType Lexer::lookupKeyword(const std::string_view upper) {
    for (const auto& entry : kKeywords_) {
        const auto& keyword = entry.first;
        const auto tokenType = entry.second;
        if (keyword == upper) {
            return tokenType;
        }
    }
    return TokenType::Identifier;
}

} // namespace gwbasic
