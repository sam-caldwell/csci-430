// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"

namespace gwbasic {

// Note: identifierOrKeyword() is only entered when tokenize() has already
// verified the first character is alphabetic. No separate isIdentStart() is
// required here, so we only keep isIdentChar() below.

Token Lexer::identifierOrKeyword() {
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
    const int startLine = line_;
    const int startCol = col_;
    auto buf = scanWhile([](const char ch){
        const auto uch = static_cast<unsigned char>(ch);
        return std::isalnum(uch) != 0 || ch == '_';
    });
    if (peek() == '$' || peek() == '%' || peek() == '!' || peek() == '#') buf.push_back(advance());

    std::string upper;
    upper.reserve(buf.size());
    for (const char c : buf) upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

    if (upper == KW_REM) { // treat as comment to EOL
        skipToEOL();
        return Token{TokenType::NewLine, STR_LF, startLine, startCol};
    }
    if (const TokenType kw = lookupKeyword(upper); kw != TokenType::Identifier)
        return Token{kw, buf, startLine, startCol};
    return Token{TokenType::Identifier, buf, startLine, startCol};
}

} // namespace gwbasic
