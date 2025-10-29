// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>

namespace gwbasic {

/*
 * Function: Lexer::stringLiteral
 * Inputs:
 *  - none (expects current char to be '"')
 * Outputs:
 *  - Token: STRING token with unescaped contents and source location
 * Theory of operation:
 *  - Consumes the opening quote, then scans until the closing quote,
 *    handling simple escapes (\n, \t, \", \\). Throws on unterminated strings.
 */
Token Lexer::stringLiteral() {
    const int startLine = line_;
    const int startCol = col_;
    std::string buf;
    advance(); // opening quote
    while (!atEnd()) {
        if (char c = advance(); c == '\\') {
            if (atEnd()) break;
            char n = advance();
            switch (n) {
                case 'n': buf.push_back('\n'); break;
                case 't': buf.push_back('\t'); break;
                case '"': buf.push_back('"'); break;
                case '\\': buf.push_back('\\'); break;
                default: buf.push_back(n); break;
            }
        } else if (c == '"') {
            return Token{TokenType::String, buf, startLine, startCol};
        } else {
            buf.push_back(c);
        }
    }
    {
        std::ostringstream m; m << "Unterminated string literal at line " << startLine; throw LexError(m.str());
    }
}

} // namespace gwbasic
