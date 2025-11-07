// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Parser.ParseProgram_ForNextVarMismatch
Purpose: Ensure NEXT variable must match the corresponding FOR loop variable.
Components Under Test: Parser::parseProgram; FOR/NEXT marker validation.
Expected Behavior: Throws ParseError when NEXT J closes FOR I.
*/
TEST(Parser, ParseProgram_ForNextVarMismatch) {
    std::string src =
        "10 FOR I=1 TO 2\n"
        "20 NEXT J\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    EXPECT_THROW({ auto _ = p.parseProgram(); }, ParseError);
}
