// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"

using namespace gwbasic;

/***
 * Test: ParserInput.MoreEdgeCases
 * Purpose: Validate additional INPUT parsing edges: trailing comma and malformed multiple channels.
 */
TEST(ParserInput, MoreEdgeCases) {
    // Trailing comma after variable list should error
    {
        const std::string src = "10 INPUT #1, A, B, \n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
    // Multiple channels (e.g., INPUT #1,#2, A) is malformed
    {
        const std::string src = "10 INPUT #1, #2, A\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
    // Missing identifier after comma
    {
        const std::string src = "10 INPUT ,X\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
    }
}
