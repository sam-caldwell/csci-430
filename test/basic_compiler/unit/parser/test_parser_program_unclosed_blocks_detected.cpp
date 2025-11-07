// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.ParseProgram_UnclosedBlocksDetected
Purpose: Ensure parser reports errors for unclosed WHILE/IF/FOR blocks.
Components Under Test: Parser::parseProgram; block stack error detection.
Expected Behavior: Throws ParseError for each unclosed construct.
*/
TEST(Parser, ParseProgram_UnclosedBlocksDetected) {
    // Unclosed FOR
    {
        std::string src = "10 FOR I=1 TO 2\n"; // missing NEXT
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
    // Unclosed IF
    {
        std::string src = "10 IF 1 THEN\n"; // missing END IF
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
    // Unclosed WHILE
    {
        std::string src = "10 WHILE 1\n"; // missing WEND
        Lexer lx(src); auto toks = lx.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
    }
}
