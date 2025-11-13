// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/lexer/Lexer.h"
#include "basic_compiler/Symbols.h"
#include "basic_compiler/token/ToString.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/TokenType.h"
#include <string>

namespace gwbasic {

/*
 * Function: Lexer::logToken
 * Summary:
 *  Log a single token to the lex log if logging is enabled.
 * Parameters:
 *  - token: Token to render into the log
 * Returns:
 *  - void
 */
void Lexer::logToken(const Token& token) {
    // Use a chained logging pattern: single stream chain for the whole line
    auto& logger = log();
    logger << "token" << Symbols::SPACE.first() << to_string(token.type) << Symbols::SPACE.first() << "@" << Symbols::SPACE.first() << token.line << ":" << token.col;
    switch (token.type) {
        case TokenType::EndOfFile:
        case TokenType::NewLine:
            break;
        default: {
            const std::string escaped = escapeForLog(token.lexeme);
            logger << Symbols::SPACE.first() << Symbols::DOUBLE_QUOTE << escaped << Symbols::DOUBLE_QUOTE;
            break;
        }
    }
    logger << Symbols::LF;
}

} // namespace gwbasic
