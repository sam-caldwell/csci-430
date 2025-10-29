// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>

namespace gwbasic {

std::vector<Token> Lexer::tokenize() {
    /*
     * Function: Lexer::tokenize
     * Inputs:
     *  - none (operates over the entire provided source string)
     * Outputs:
     *  - std::vector<Token>: Complete token stream including NewLine tokens
     *    and a final EndOfFile marker
     * Theory of operation:
     *  - Iteratively skips whitespace/comments, classifies the next lexeme
     *    as number, identifier/keyword, string, or operator/punctuation, and
     *    appends corresponding tokens, tracking source positions.
     */
    std::vector<Token> tokens;
    while (!atEnd()) {
        skipWhitespace();
        if (atEnd()) break;

        char c = peek();
        if (c == '\n') {
            advance();
            Token t(TokenType::NewLine, "\n", line_ - 1, 1);
            tokens.emplace_back(t);
            logToken(t);
            bol_ = true;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            Token t = number();
            tokens.push_back(t);
            logToken(t);
            bol_ = false;
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c))) {
            Token t = identifierOrKeyword();
            tokens.push_back(t);
            logToken(t);
            bol_ = false;
            continue;
        }
        if (c == '"') {
            Token t = stringLiteral();
            tokens.push_back(t);
            logToken(t);
            bol_ = false;
            continue;
        }

        int tline = line_;
        int tcol = col_;
        switch (c) {
            case '+': advance(); { Token t(TokenType::Plus, "+", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '-': advance(); { Token t(TokenType::Minus, "-", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '*': advance(); { Token t(TokenType::Star, "*", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '/': advance(); { Token t(TokenType::Slash, "/", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '(': advance(); { Token t(TokenType::LParen, "(", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case ')': advance(); { Token t(TokenType::RParen, ")", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case ':': advance(); { Token t(TokenType::Colon, ":", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case ',': advance(); { Token t(TokenType::Comma, ",", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '=': advance(); { Token t(TokenType::Assign, "=", tline, tcol); tokens.emplace_back(t); logToken(t); } break;
            case '<':
                advance();
                if (peek() == '=') { advance(); Token t(TokenType::LessEqual, "<=", tline, tcol); tokens.emplace_back(t); logToken(t); }
                else if (peek() == '>') { advance(); Token t(TokenType::NotEqual, "<>", tline, tcol); tokens.emplace_back(t); logToken(t); }
                else { Token t(TokenType::Less, "<", tline, tcol); tokens.emplace_back(t); logToken(t); }
                break;
            case '>':
                advance();
                if (peek() == '=') { advance(); Token t(TokenType::GreaterEqual, ">=", tline, tcol); tokens.emplace_back(t); logToken(t); }
                else { Token t(TokenType::Greater, ">", tline, tcol); tokens.emplace_back(t); logToken(t); }
                break;
            default: {
                std::ostringstream oss;
                oss << "Unexpected character '" << c << "' at " << line_ << ":" << col_;
                throw LexError(oss.str());
            }
        }
        bol_ = false;
    }
    { Token t(TokenType::EndOfFile, "", line_, col_); tokens.emplace_back(t); logToken(t); }
    return tokens;
}

} // namespace gwbasic
