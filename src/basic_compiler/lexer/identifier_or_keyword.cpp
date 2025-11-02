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
    std::string buf;
    while (std::isalnum(static_cast<unsigned char>(peek())) != 0 || peek() == '_') buf.push_back(advance());

    std::string upper;
    upper.reserve(buf.size());
    for (const char c : buf) upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

    if (upper == "LET") return Token{TokenType::KwLet, buf, startLine, startCol};
    if (upper == "PRINT") return Token{TokenType::KwPrint, buf, startLine, startCol};
    if (upper == "IF") return Token{TokenType::KwIf, buf, startLine, startCol};
    if (upper == "THEN") return Token{TokenType::KwThen, buf, startLine, startCol};
    if (upper == "ELSE") return Token{TokenType::KwElse, buf, startLine, startCol};
    if (upper == "GOTO") return Token{TokenType::KwGoto, buf, startLine, startCol};
    if (upper == "END") return Token{TokenType::KwEnd, buf, startLine, startCol};
    if (upper == "FOR") return Token{TokenType::KwFor, buf, startLine, startCol};
    if (upper == "TO") return Token{TokenType::KwTo, buf, startLine, startCol};
    if (upper == "STEP") return Token{TokenType::KwStep, buf, startLine, startCol};
    if (upper == "NEXT") return Token{TokenType::KwNext, buf, startLine, startCol};
    if (upper == "GOSUB") return Token{TokenType::KwGosub, buf, startLine, startCol};
    if (upper == "RETURN") return Token{TokenType::KwReturn, buf, startLine, startCol};
    if (upper == "INPUT") return Token{TokenType::KwInput, buf, startLine, startCol};
    if (upper == "RANDOMIZE") return Token{TokenType::KwRandomize, buf, startLine, startCol};
    if (upper == "WHILE") return Token{TokenType::KwWhile, buf, startLine, startCol};
    if (upper == "WEND") return Token{TokenType::KwWend, buf, startLine, startCol};
    if (upper == "REM") { // treat as comment to EOL
        skipToEOL();
        return Token{TokenType::NewLine, "\n", startLine, startCol};
    }
    return Token{TokenType::Identifier, buf, startLine, startCol};
}

} // namespace gwbasic
