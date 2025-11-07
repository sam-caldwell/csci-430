// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/LexError.h"

using namespace gwbasic;

/***
Test: Lexer.AmpLiteral_InvalidCases
Inputs: "&H\n" (missing digits), "&Q1\n" (invalid base designator)
Code under test: Lexer::emitAmpLiteral error paths
Expected behavior: LexError thrown.
*/
TEST(Lexer, AmpLiteral_InvalidCases) {
    {
        std::istringstream iss("&H\n");
        Lexer lex(iss);
        EXPECT_THROW({ (void)lex.tokenize(); }, LexError);
    }
    {
        std::istringstream iss("&Q1\n");
        Lexer lex(iss);
        EXPECT_THROW({ (void)lex.tokenize(); }, LexError);
    }
}
