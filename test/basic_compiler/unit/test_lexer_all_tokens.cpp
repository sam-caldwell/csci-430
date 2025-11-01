// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <set>
#include <string>
#include <vector>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/token/TokenType.h"

using namespace gwbasic;

/*
 * Test Suite: Lexer All Tokens
 * Purpose: Ensure the lexer can emit every supported token kind it defines
 *          and treat REM/apostrophe as comments to EOL.
 * Components Under Test: Lexer::tokenize; TokenType enumeration coverage.
 * Expected Behavior: All token kinds are observed in the token stream; no
 *          KwRem token is produced (comments map to NewLine).
 *
 *  Ensure the lexer can produce every recognized token type it is designed to emit.
 *  This covers: EndOfFile, NewLine, Integer, Float, String, Identifier,
 *  keywords (LET, PRINT, IF, THEN, GOTO, END, FOR, TO, STEP, NEXT, GOSUB, RETURN, INPUT),
 *  and operators/punct (+ - * / = < > <= >= <> ( ) : ,).
 *  Note: REM is recognized but treated as a comment-to-EOL, yielding NewLine rather than a KwRem token.
 */
TEST(Lexer, AllRecognizedTokens) {
    std::string src =
        // arithmetic, assignment, parens, comma, colon
        "10 LET A = (1 + 2) * 3 / 4 - 5 : PRINT A, 6\n"
        // string literal
        "20 PRINT \"Hello\"\n"
        // comparisons (<=, <>, >=, <, >) and THEN/GOTO
        "30 IF A <= 10 THEN 50\n"
        "40 IF A <> 10 THEN 50\n"
        "50 IF A >= 10 THEN 50\n"
        "60 IF A < 10 THEN 50\n"
        "70 IF A > 10 THEN 50\n"
        // FOR / TO / STEP / NEXT
        "80 FOR I = 1 TO 10 STEP 2 : NEXT I\n"
        // GOTO / GOSUB / RETURN / INPUT
        "90 GOTO 200\n"
        "100 GOSUB 300\n"
        "110 RETURN\n"
        "120 INPUT X\n"
        // REM -> comment-to-EOL (produces NewLine)
        "130 REM this is a remark\n"
        // apostrophe comment -> skip to EOL
        "140 ' comment with apostrophe style\n"
        // float literal
        "150 LET F = 1.23\n"
        // END
        "160 END\n";

    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();

    // Collect observed token types
    std::set<TokenType> seen;
    for (const auto& t : toks) seen.insert(t.type);

    // Expected tokens (those the lexer actually emits)
    std::vector<TokenType> expected = {
        TokenType::EndOfFile,
        TokenType::NewLine,
        TokenType::Integer,
        TokenType::Float,
        TokenType::String,
        TokenType::Identifier,
        TokenType::KwLet,
        TokenType::KwPrint,
        TokenType::KwIf,
        TokenType::KwThen,
        TokenType::KwGoto,
        TokenType::KwEnd,
        TokenType::KwFor,
        TokenType::KwTo,
        TokenType::KwStep,
        TokenType::KwNext,
        TokenType::KwGosub,
        TokenType::KwReturn,
        TokenType::KwInput,
        TokenType::Plus,
        TokenType::Minus,
        TokenType::Star,
        TokenType::Slash,
        TokenType::Assign,
        TokenType::Less,
        TokenType::Greater,
        TokenType::LessEqual,
        TokenType::GreaterEqual,
        TokenType::NotEqual,
        TokenType::LParen,
        TokenType::RParen,
        TokenType::Colon,
        TokenType::Comma,
    };

    for (auto tt : expected) {
        EXPECT_TRUE(seen.count(tt) == 1u) << "TokenType not observed: " << static_cast<int>(tt);
    }

    // Additional behavioral checks
    // - Ensure no KwRem token is produced (REM acts as comment → NewLine)
    EXPECT_EQ(seen.count(TokenType::KwRem), 0u);
}
