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
inline std::string to_string(const TokenType t) {
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
        case TokenType::KwElse: return "ELSE";
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
        case TokenType::KwData: return "DATA";
        case TokenType::KwRead: return "READ";
        case TokenType::KwRestore: return "RESTORE";
        case TokenType::KwWrite: return "WRITE";
        case TokenType::KwLine: return "LINE";
        case TokenType::KwRandomize: return "RANDOMIZE";
        case TokenType::KwWhile: return "WHILE";
        case TokenType::KwWend: return "WEND";
        case TokenType::KwRun: return "RUN";
        case TokenType::KwCommon: return "COMMON";
        case TokenType::KwAll: return "ALL";
        case TokenType::KwChain: return "CHAIN";
        case TokenType::KwOn: return "ON";
        case TokenType::KwError: return "ERROR";
        case TokenType::KwResume: return "RESUME";
        case TokenType::KwDim: return "DIM";
        case TokenType::KwOpen: return "OPEN";
        case TokenType::KwClose: return "CLOSE";
        case TokenType::KwAs: return "AS";
        case TokenType::KwOutput: return "OUTPUT";
        case TokenType::KwUsing: return "USING";
        case TokenType::KwDef: return "DEF";
        case TokenType::KwMerge: return "MERGE";
        case TokenType::KwDefInt: return "DEFINT";
        case TokenType::KwDefSng: return "DEFSNG";
        case TokenType::KwDefDbl: return "DEFDBL";
        case TokenType::KwDefStr: return "DEFSTR";
        case TokenType::KwSeg: return "SEG";
        case TokenType::KwBload: return "BLOAD";
        case TokenType::KwBsave: return "BSAVE";
        case TokenType::KwCall: return "CALL";
        case TokenType::KwPoke: return "POKE";
        case TokenType::KwColor: return "COLOR";
        case TokenType::KwChdir: return "CHDIR";
        case TokenType::KwClear: return "CLEAR";
        case TokenType::KwSwap: return "SWAP";
        case TokenType::KwOption: return "OPTION";
        case TokenType::KwBase: return "BASE";
        case TokenType::KwErase: return "ERASE";
        case TokenType::KwAnd: return "AND";
        case TokenType::KwOr: return "OR";
        case TokenType::KwNot: return "NOT";
        case TokenType::KwStop: return "STOP";
        case TokenType::KwSystem: return "SYSTEM";
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Backslash: return "\\";
        case TokenType::Assign: return "=";
        case TokenType::Less: return "<";
        case TokenType::Greater: return ">";
        case TokenType::LessEqual: return "<=";
        case TokenType::GreaterEqual: return ">=";
        case TokenType::NotEqual: return "<>";
        case TokenType::Caret: return "^";
        case TokenType::LParen: return "(";
        case TokenType::RParen: return ")";
        case TokenType::Colon: return ":";
        case TokenType::Comma: return ",";
        case TokenType::Hash: return "#";
        case TokenType::KwMod: return "MOD";
    }
    return "?";
}

} // namespace gwbasic
