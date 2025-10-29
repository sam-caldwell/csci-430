// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/Token.h"
#include "basic_compiler/token/ToString.h"
#include <cstdio>

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
            std::string esc = escapeForLog(t.lexeme);
            lexLog_ << " \"" << esc << "\"";
            break;
        }
    }
    lexLog_ << "\n";
}

/*
 * Function: Lexer::escapeForLog
 * Inputs:
 *  - s: Raw input string
 * Outputs:
 *  - std::string: Escaped representation suitable for single-line logs
 * Theory of operation:
 *  - Replaces special characters with C-style escape sequences.
 */
std::string Lexer::escapeForLog(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char ch : s) {
        switch (ch) {
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            case '"':  out += "\\\""; break;
            default:
                if (ch < 0x20 || ch == 0x7F) {
                    char buf[5];
                    std::snprintf(buf, sizeof(buf), "\\x%02X", ch);
                    out += buf;
                } else {
                    out.push_back(static_cast<char>(ch));
                }
        }
    }
    return out;
}

} // namespace gwbasic
