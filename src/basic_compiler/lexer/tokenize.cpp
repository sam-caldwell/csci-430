// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>

namespace gwbasic {
/*
 * Function: Lexer::tokenize
 * Summary:
 *  Convert the source text into a sequence of tokens including EOF.
 * Parameters:
 *  - none
 * Returns:
 *  - std::vector<Token>: Token stream with NewLine tokens and EndOfFile
 */
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!atEnd()) {
        skipWhitespace();
        if (atEnd()) break;

        if (tryEmitNewline(tokens)) continue;

        if (tryEmitPrimary(tokens)) continue;

        const int tline = line_;
        const int tcol = col_;
        const char c = peek();
        if (c == Symbols::AMPERSAND.first()) { emitAmpLiteral(tokens, tline, tcol); continue; }
        if (tryEmitOperatorOrPunct(tokens, tline, tcol, c)) continue;

        std::ostringstream oss;
        oss << "Unexpected character '" << c << "' at " << line_ << ':' << col_;
        throw LexError(oss.str());
    }
    emitFixed<TokenType::EndOfFile>(tokens, "", line_, col_);
    // Ensure end-of-input path in advance() is covered (no-op when at end)
    if (atEnd()) (void)advance();
    return tokens;
}

} // namespace gwbasic
