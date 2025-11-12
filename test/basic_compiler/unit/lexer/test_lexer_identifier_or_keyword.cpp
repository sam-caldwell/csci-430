// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/LexerAccessorForTests.h"

using namespace gwbasic;

/***
Test: Lexer.IdentifierOrKeyword_HappyPaths
Purpose: Verify identifiers, type suffixes, and keywords (case-insensitive),
         including REM handling that yields a synthetic NewLine token.
*/
TEST(Lexer, IdentifierOrKeyword_HappyPaths) {
    // Simple identifier
    {
        Lexer lx("Foo ");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "Foo");
        EXPECT_EQ(t.line, 1);
        EXPECT_EQ(t.col, 1);
    }
    // Identifier with type suffix
    {
        Lexer lx("A$ ");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "A$");
    }
    {
        Lexer lx("I% ");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "I%");
    }
    {
        Lexer lx("R! ");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "R!");
    }
    {
        Lexer lx("L& ");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "L&");
    }
    // Keyword recognition is case-insensitive; lexeme preserves original case
    {
        Lexer lx("print");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::KwPrint);
        EXPECT_EQ(t.lexeme, "print");
    }
    {
        Lexer lx("WhIlE");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::KwWhile);
        EXPECT_EQ(t.lexeme, "WhIlE");
    }
    // REM comment handling produces a NewLine token and leaves LF unconsumed
    {
        Lexer lx("REM this is a comment\nX");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::NewLine);
        EXPECT_EQ(t.lexeme, "\n");
        // Next char remains the newline (skipToEOL stops before LF)
        EXPECT_EQ(LexerAccessorForTests::peek(lx), '\n');
    }
    // REM at end-of-input: still returns NewLine token (no throw)
    {
        Lexer lx("REM");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::NewLine);
        EXPECT_EQ(t.lexeme, "\n");
        EXPECT_TRUE(LexerAccessorForTests::atEnd(lx));
    }
}

/***
Test: Lexer.IdentifierOrKeyword_SadPaths
Purpose: Validate non-identifier trailing punctuation is not consumed, and
         degenerate/edge inputs do not crash and return reasonable tokens.
Security: Ensures no overconsumption beyond intended identifier/suffix, and
          large inputs do not overflow internal buffers.
*/
TEST(Lexer, IdentifierOrKeyword_SadPaths) {
    // Trailing punctuation '@' should not be consumed as part of identifier
    {
        Lexer lx("AB@C");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "AB");
        // '@' remains for the caller
        EXPECT_EQ(LexerAccessorForTests::peek(lx), '@');
    }
    // Multiple suffix characters: only first recognized suffix is consumed
    {
        Lexer lx("X$$");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "X$");
        EXPECT_EQ(LexerAccessorForTests::peek(lx), '$');
    }
    // Identifier starting with underscore is allowed
    {
        Lexer lx("_tmp");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme, "_tmp");
    }
    // Empty input: returns Identifier with empty lexeme, no crash
    {
        Lexer lx("");
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_TRUE(t.lexeme.empty());
        EXPECT_TRUE(LexerAccessorForTests::atEnd(lx));
    }
    // Large input: long identifier does not overflow
    {
        std::string big(4096, 'A');
        Lexer lx(big);
        Token t = LexerAccessorForTests::identifierOrKeyword(lx);
        EXPECT_EQ(t.type, TokenType::Identifier);
        EXPECT_EQ(t.lexeme.size(), big.size());
        EXPECT_TRUE(LexerAccessorForTests::atEnd(lx));
    }
}
