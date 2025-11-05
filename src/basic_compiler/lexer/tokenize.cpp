// (c) 2025 Sam Caldwell. All Rights Reserved.
#include "basic_compiler/Lexer.h"
#include <sstream>

namespace gwbasic {
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
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!atEnd()) {
        skipWhitespace();
        if (atEnd()) break;

        char c = peek();
        if (c == CH_LF) {
            advance();
            emitFixed<TokenType::NewLine>(tokens, STR_LF, line_ - 1, 1);
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
            case '&': {
                // Hexadecimal literal prefix &H...
                advance();
                if (auto n = static_cast<char>(std::toupper(static_cast<unsigned char>(peek()))); n == 'H') {
                    advance();
                    // accumulate hex digits
                    unsigned long long val = 0ULL;
                    int digits = 0;
                    while (true) {
                        char h = peek();
                        int v;
                        if (h >= '0' && h <= '9') v = h - '0';
                        else if (h >= 'A' && h <= 'F') v = 10 + (h - 'A');
                        else if (h >= 'a' && h <= 'f') v = 10 + (h - 'a');
                        else break;
                        val = (val << 4) + static_cast<unsigned long long>(v);
                        advance();
                        ++digits;
                    }
                    if (digits == 0) {
                        std::ostringstream oss; oss << "Invalid hex literal at " << tline << ":" << tcol; throw LexError(oss.str());
                    }
                    std::string dec = std::to_string(val);
                    Token t(TokenType::Integer, dec, tline, tcol);
                    tokens.emplace_back(t); logToken(t); bol_ = false; break;
                } else {
                    std::ostringstream oss; oss << "Unexpected '&' at " << tline << ":" << tcol; throw LexError(oss.str());
                }
            }
            case '+': advance(); emitFixed<TokenType::Plus>(tokens, "+", tline, tcol); break;
            case '-': advance(); emitFixed<TokenType::Minus>(tokens, "-", tline, tcol); break;
            case '*': advance(); emitFixed<TokenType::Star>(tokens, "*", tline, tcol); break;
            case '/': advance(); emitFixed<TokenType::Slash>(tokens, "/", tline, tcol); break;
            case '(': advance(); emitFixed<TokenType::LParen>(tokens, "(", tline, tcol); break;
            case ')': advance(); emitFixed<TokenType::RParen>(tokens, ")", tline, tcol); break;
            case ':': advance(); emitFixed<TokenType::Colon>(tokens, ":", tline, tcol); break;
            case ',': advance(); emitFixed<TokenType::Comma>(tokens, ",", tline, tcol); break;
            case '#': advance(); emitFixed<TokenType::Hash>(tokens, "#", tline, tcol); break;
            case '=': advance(); emitFixed<TokenType::Assign>(tokens, "=", tline, tcol); break;
            case '<':
                advance();
                if (peek() == '>') { advance(); emitFixed<TokenType::NotEqual>(tokens, "<>", tline, tcol); }
                else { emitPairOrSingle<TokenType::Less, TokenType::LessEqual, '='>(tokens, "<", "<=", tline, tcol); }
                break;
            case '>':
                advance();
                emitPairOrSingle<TokenType::Greater, TokenType::GreaterEqual, '='>(tokens, ">", ">=", tline, tcol);
                break;
            default: {
                std::ostringstream oss;
                oss << "Unexpected character '" << c << "' at " << line_ << ":" << col_;
                throw LexError(oss.str());
            }
        }
        bol_ = false;
    }
    emitFixed<TokenType::EndOfFile>(tokens, "", line_, col_);
    // Ensure end-of-input path in advance() is covered (no-op when at end)
    if (atEnd()) (void)advance();
    return tokens;
}

} // namespace gwbasic
