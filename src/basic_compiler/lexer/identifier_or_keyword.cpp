// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <cctype>
#include <string>

namespace gwbasic {
/*
 * Function: Lexer::identifierOrKeyword
 * Summary:
 *  Scan an identifier or keyword and return the corresponding token.
 * Parameters:
 *  - none
 * Returns:
 *  - Token: Identifier or keyword token with text and location
 */
Token Lexer::identifierOrKeyword() {

    const int startLine = line_;
    const int startCol = col_;
    auto buf = scanWhile([](const char chr){
        const auto uch = static_cast<unsigned char>(chr);
        return std::isalnum(uch) != 0 || chr == '_';
    });
    if (peek() == Symbols::DOLLARSIGN.first() || peek() == Symbols::PERCENT.first() || peek() == Symbols::EXCLAMATION.first() || peek() == Symbols::HASH.first() || peek() == Symbols::AMPERSAND.first()) {
        buf.push_back(advance());
    }

    std::string upper;
    upper.reserve(buf.size());
    for (const char chr : buf) {
        upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(chr))));
    }

    if (upper == KW_REM) { // treat as comment to EOL
        skipToEOL();
        return Token{TokenType::NewLine, Symbols::LF.to_string(), startLine, startCol};
    }
    if (const TokenType kwType = lookupKeyword(upper); kwType != TokenType::Identifier) {
        return Token{kwType, buf, startLine, startCol};
    }
    return Token{TokenType::Identifier, buf, startLine, startCol};
}

} // namespace gwbasic
