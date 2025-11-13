// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/lexer/LexError.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <sstream>
#include <string>

namespace gwbasic {

/*
 * Function: Lexer::stringLiteral
 * Summary:
 *  Parse a quoted string and return a String token.
 * Parameters:
 *  - none
 * Returns:
 *  - Token: String token with unescaped contents and location
 */
Token Lexer::stringLiteral() { // NOLINT(readability-function-size,readability-function-cognitive-complexity)
    const int startLine = line_;
    const int startCol = col_;
    std::string buf;
    advance(); // opening quote
    while (!atEnd()) {
        const char chr = advance();
        if (chr == Symbols::DOUBLE_QUOTE.first()) {
            // If next char is also a double quote, this encodes a literal quote
            if (!atEnd() && peek() == Symbols::DOUBLE_QUOTE.first()) {
                advance(); // consume the second quote
                buf.push_back(Symbols::DOUBLE_QUOTE.first());
                continue;
            }
            // Otherwise, this terminates the string
            return Token{TokenType::String, buf, startLine, startCol};
        }
        // No escape processing: backslashes are just characters
        buf.push_back(chr);
    }
    {
        std::ostringstream msg;
        msg << "Unterminated string literal at line " << startLine;
        throw LexError(msg.str());
    }
}

} // namespace gwbasic
