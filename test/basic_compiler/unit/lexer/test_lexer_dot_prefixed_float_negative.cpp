// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/lexer/LexError.h"

using namespace gwbasic;

/***
 * Test: Lexer.DotAlone_IsError
 * Inputs: Source containing a lone '.' after PRINT
 * Code under test: Lexer::tokenize error handling for unexpected '.' character
 * Expected behavior: Throws LexError (unexpected character '.')
 */
TEST(Lexer, DotAlone_IsError) {
    const std::string src = "10 PRINT .\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    EXPECT_THROW({ auto toks = lex.tokenize(); (void)toks; }, LexError);
}
