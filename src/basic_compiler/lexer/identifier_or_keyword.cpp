// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {
/*
 * Function: Lexer::identifierOrKeyword
 * Inputs:
 *  - none (reads from current position)
 * Outputs:
 *  - Token: Identifier or specific keyword token with text and location
 * Theory of operation:
 *  - Accumulates alphanumeric/underscore characters, uppercases a copy
 *    to compare against known GW-BASIC keywords; otherwise returns IDENT.
 */
Token Lexer::identifierOrKeyword() {

    const int startLine = line_;
    const int startCol = col_;
    auto buf = scanWhile([](const char ch){
        const auto uch = static_cast<unsigned char>(ch);
        return std::isalnum(uch) != 0 || ch == '_';
    });
    if (peek() == Symbols::DOLLARSIGN.first() || peek() == Symbols::PERCENT.first() || peek() == Symbols::EXCLAMATION.first() || peek() == Symbols::HASH.first() || peek() == Symbols::AMPERSAND.first())
        buf.push_back(advance());

    std::string upper;
    upper.reserve(buf.size());
    for (const char c : buf) upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

    if (upper == KW_REM) { // treat as comment to EOL
        skipToEOL();
        return Token{TokenType::NewLine, Symbols::LF.to_string(), startLine, startCol};
    }
    if (const TokenType kw = lookupKeyword(upper); kw != TokenType::Identifier)
        return Token{kw, buf, startLine, startCol};
    return Token{TokenType::Identifier, buf, startLine, startCol};
}

} // namespace gwbasic
