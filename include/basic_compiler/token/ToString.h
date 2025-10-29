// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

#include <string>
#include "basic_compiler/token/TokenType.h"

namespace gwbasic {

/**
 * Function: to_string(TokenType)
 * Purpose:
 *  - Produce a concise, human-readable label for a token type for logs and
 *    diagnostics.
 * Inputs:
 *  - t: TokenType to render
 * Outputs:
 *  - std::string: Short name (e.g., "PRINT", "+", "IDENT")
 */
inline std::string to_string(TokenType t) {
    switch (t) {
        case TokenType::EndOfFile: return "EOF";
        case TokenType::NewLine: return "NEWLINE";
        case TokenType::Integer: return "INT";
        case TokenType::Float: return "FLOAT";
        case TokenType::String: return "STRING";
        case TokenType::Identifier: return "IDENT";
        case TokenType::KwLet: return "LET";
        case TokenType::KwPrint: return "PRINT";
        case TokenType::KwIf: return "IF";
        case TokenType::KwThen: return "THEN";
        case TokenType::KwGoto: return "GOTO";
        case TokenType::KwEnd: return "END";
        case TokenType::KwRem: return "REM";
        case TokenType::KwFor: return "FOR";
        case TokenType::KwTo: return "TO";
        case TokenType::KwStep: return "STEP";
        case TokenType::KwNext: return "NEXT";
        case TokenType::KwGosub: return "GOSUB";
        case TokenType::KwReturn: return "RETURN";
        case TokenType::KwInput: return "INPUT";
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Assign: return "=";
        case TokenType::Less: return "<";
        case TokenType::Greater: return ">";
        case TokenType::LessEqual: return "<=";
        case TokenType::GreaterEqual: return ">=";
        case TokenType::NotEqual: return "<>";
        case TokenType::LParen: return "(";
        case TokenType::RParen: return ")";
        case TokenType::Colon: return ":";
        case TokenType::Comma: return ",";
    }
    return "?";
}

} // namespace gwbasic

