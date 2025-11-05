// (c) 2025 Sam Caldwell. All Rights Reserved.
#pragma once

namespace gwbasic {

/**
 * Enum: TokenType
 * Purpose:
 *  - Typed classification for all lexical tokens produced by the lexer,
 *    consumed by the parser to drive grammar decisions.
 * Members:
 *  - Special: EndOfFile, NewLine
 *  - Literals: Integer, Float, String, Identifier
 *  - Keywords: Let, Print, If, Then, Goto, End, Rem, For, To, Step, Next,
 *              Gosub, Return, Input
 *  - Operators/punct: arithmetic, comparison, parens, colon, comma
 */
enum class TokenType {
    // Special
    EndOfFile,
    NewLine,

    // Literals
    Integer,
    Float,
    String,
    Identifier,

    // Keywords
    KwLet,
    KwPrint,
    KwIf,
    KwThen,
    KwElse,
    KwGoto,
    KwEnd,
    KwRem,
    KwFor,
    KwTo,
    KwStep,
    KwNext,
    KwGosub,
    KwReturn,
    KwInput,
    KwData,
    KwRead,
    KwRestore,
    KwWrite,
    KwLine,
    KwRandomize,
    KwWhile,
    KwWend,
    KwRun,
    KwCommon,
    KwAll,
    KwMerge,
    KwChain,
    KwDim,
    KwOpen,
    KwClose,
    KwAs,
    KwOutput,
    KwUsing,
    KwDef,
    KwDefInt,
    KwDefSng,
    KwDefDbl,
    KwDefStr,
    KwSeg,
    KwBload,
    KwBsave,
    KwCall,
    KwPoke,
    KwColor,
    KwChdir,
    KwClear,

    // Operators / punctuation
    Plus,
    Minus,
    Star,
    Slash,
    Assign,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    NotEqual,
    LParen,
    RParen,
    Colon,
    Comma,
    Hash,
};

} // namespace gwbasic
