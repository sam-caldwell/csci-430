// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsMidAssign.ErrorsOnNonStringValue
Purpose: Ensure MID$ assignment RHS must be string-typed.
Components Under Test: SemanticAnalyzer::analyze (type checking for MidAssignStmt).
Expected Behavior: Throws SemanticError when RHS is numeric.
*/
TEST(SemanticsMidAssign, ErrorsOnNonStringValue) {
    // Scalar target but RHS is numeric
    std::string src =
        "10 A$=\"HELLO\"\n"
        "20 MID$(A$,3,1)=5\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}
