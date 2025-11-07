// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsStringArrays.ForbidsStringAssignToNumericArray
 * Purpose: Ensure assigning a string to a numeric array element is rejected.
 * Components Under Test: SemanticAnalyzer type checking for array elements.
 * Expected Behavior: Throws SemanticError.
 */
TEST(SemanticsStringArrays, ForbidsStringAssignToNumericArray) {
    std::string src =
        "10 DIM A(5)\n"
        "20 A(2)=\"HI\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}
