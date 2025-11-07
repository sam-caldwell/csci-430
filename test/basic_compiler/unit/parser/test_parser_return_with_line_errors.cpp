// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"

/***
Test: Parser.ReturnWithLineNumber_Errors
Inputs: Program containing "RETURN 200" (nonlocal return)
Expected behavior: Parser throws because RETURN does not take a line operand.
*/
TEST(Parser, ReturnWithLineNumber_Errors) {
    const std::string src =
        "10 RETURN 200\n";
    std::istringstream in(src);
    gwbasic::Lexer lx(in);
    auto toks = lx.tokenize();
    gwbasic::Parser p(std::move(toks));
    EXPECT_THROW({ (void)p.parseProgram(); }, gwbasic::ParseError);
}
