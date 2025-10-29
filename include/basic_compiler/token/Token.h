// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include <utility>

#include "basic_compiler/token/TokenType.h"

namespace gwbasic {

/**
 * Struct: Token
 * Purpose:
 *  - Immutable lexical token with text and source location used by the
 *    parser and for logging/diagnostics.
 * Inputs (ctor):
 *  - type: TokenType classification
 *  - lexeme: Original text (when applicable)
 *  - line/col: 1-based source position
 * Outputs:
 *  - Plain value object moved around by the lexer and parser
 */
struct Token {
    TokenType type{};
    std::string lexeme{};
    int line{1};
    int col{1};

    Token() = default;
    Token(TokenType t, std::string lx, int ln, int cl)
        : type(t), lexeme(std::move(lx)), line(ln), col(cl) {}
};

} // namespace gwbasic

