// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/ToString.h"

namespace gwbasic {

/*
 * Function: Lexer::logToken
 * Summary:
 *  Log a single token to the lex log if logging is enabled.
 * Parameters:
 *  - t: Token to render into the log
 * Returns:
 *  - void
 */
void Lexer::logToken(const Token& t) {
    // Use a chained logging pattern: single stream chain for the whole line
    auto& L = log();
    L << "token" << Symbols::SPACE.first() << to_string(t.type) << Symbols::SPACE.first() << "@" << Symbols::SPACE.first() << t.line << ":" << t.col;
    switch (t.type) {
        case TokenType::EndOfFile:
        case TokenType::NewLine:
            break;
        default: {
            const std::string esc = escapeForLog(t.lexeme);
            L << Symbols::SPACE.first() << Symbols::DOUBLE_QUOTE << esc << Symbols::DOUBLE_QUOTE;
            break;
        }
    }
    L << Symbols::LF;
}

} // namespace gwbasic
