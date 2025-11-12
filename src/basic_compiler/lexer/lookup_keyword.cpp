// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
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
    for (const auto& kv : kKeywords_) {
        const auto& kw = kv.first;
        const auto tt = kv.second;
        if (kw == upper) {
            return tt;
        }
    }
    return TokenType::Identifier;
}

} // namespace gwbasic
