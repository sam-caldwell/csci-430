// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/ToString.h"

namespace gwbasic {

/*
 * Function: Lexer::logToken
 * Inputs:
 *  - t: Token to report
 * Outputs:
 *  - void (writes a single line to the lex log if enabled)
 * Theory of operation:
 *  - Renders the token type and location; includes a readable lexeme for
 *    most tokens except EOF/NEWLINE where lexeme text is not useful.
 */
void Lexer::logToken(const Token& t) {
    if (!lexLogEnabled_ || !lexLog_.is_open()) return;
    lexLog_ << "token " << to_string(t.type) << " @ " << t.line << ":" << t.col;
    switch (t.type) {
        case TokenType::EndOfFile:
        case TokenType::NewLine:
            break;
        default: {
            const std::string esc = escapeForLog(t.lexeme);
            lexLog_ << " \"" << esc << "\"";
            break;
        }
    }
    lexLog_ << "\n";
}

 

} // namespace gwbasic
