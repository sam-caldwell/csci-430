// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

using namespace gwbasic;

/***
Test: ParserPrint.ChannelMissingNumber_Error
Purpose: Exercise error path when '#' is not followed by an integer channel.
*/
TEST(ParserPrint, ChannelMissingNumber_Error) {
    const std::string src = "10 PRINT #, 1\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
