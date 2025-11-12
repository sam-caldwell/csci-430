// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringArrays.AllowsStringAssignToStringArray
 * Purpose: Ensure assigning a string to a string array element is allowed.
 * Components Under Test: SemanticAnalyzer array element typing.
 * Expected Behavior: No SemanticError is thrown.
 */
TEST(SemanticsStringArrays, AllowsStringAssignToStringArray) {
    std::string src =
        "10 DIM A$(5)\n"
        "20 A$(2)=\"HI\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_NO_THROW(sa.analyze(prog));
}
