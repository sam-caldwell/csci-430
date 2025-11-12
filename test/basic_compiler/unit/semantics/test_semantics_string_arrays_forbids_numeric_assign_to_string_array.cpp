// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringArrays.ForbidsNumericAssignToStringArray
 * Purpose: Ensure assigning a number to a string array element is rejected.
 * Components Under Test: SemanticAnalyzer type checking for array elements.
 * Expected Behavior: Throws SemanticError.
 */
TEST(SemanticsStringArrays, ForbidsNumericAssignToStringArray) {
    std::string src =
        "10 DIM A$(5)\n"
        "20 A$(2)=123\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}
