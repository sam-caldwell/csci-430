// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <cctype>

namespace gwbasic {

// Note: identifierOrKeyword() is only entered when tokenize() has already
// verified the first character is alphabetic. No separate isIdentStart() is
// required here, so we only keep isIdentChar() below.

/*
 * Function: isIdentChar
 * Inputs:
 *  - c: character to classify
 * Outputs:
 *  - bool: true if character is alphanumeric or underscore
 * Theory of operation:
 *  - Uses std::isalnum to allow letters and digits, and admits '_' to support
 *    a conventional superset while keeping the lexer simple.
 */
static bool isIdentChar(char c) { return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_'; }

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
    while (isIdentChar(peek())) buf.push_back(advance());

    std::string upper;
    upper.reserve(buf.size());
    for (const char c : buf) upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

    if (upper == "LET") return Token{TokenType::KwLet, buf, startLine, startCol};
    if (upper == "PRINT") return Token{TokenType::KwPrint, buf, startLine, startCol};
    if (upper == "IF") return Token{TokenType::KwIf, buf, startLine, startCol};
    if (upper == "THEN") return Token{TokenType::KwThen, buf, startLine, startCol};
    if (upper == "GOTO") return Token{TokenType::KwGoto, buf, startLine, startCol};
    if (upper == "END") return Token{TokenType::KwEnd, buf, startLine, startCol};
    if (upper == "FOR") return Token{TokenType::KwFor, buf, startLine, startCol};
    if (upper == "TO") return Token{TokenType::KwTo, buf, startLine, startCol};
    if (upper == "STEP") return Token{TokenType::KwStep, buf, startLine, startCol};
    if (upper == "NEXT") return Token{TokenType::KwNext, buf, startLine, startCol};
    if (upper == "GOSUB") return Token{TokenType::KwGosub, buf, startLine, startCol};
    if (upper == "RETURN") return Token{TokenType::KwReturn, buf, startLine, startCol};
    if (upper == "INPUT") return Token{TokenType::KwInput, buf, startLine, startCol};
    if (upper == "REM") { // treat as comment to EOL
        skipToEOL();
        return Token{TokenType::NewLine, "\n", startLine, startCol};
    }
    return Token{TokenType::Identifier, buf, startLine, startCol};
}

} // namespace gwbasic
