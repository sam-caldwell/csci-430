// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsMidAssign.ErrorsOnStringIndex
Purpose: Ensure array indices used in MID$ are numeric.
Components Under Test: SemanticAnalyzer::analyze for MidAssignStmt and index expressions.
Expected Behavior: Throws SemanticError when an index expression is string-typed.
*/
TEST(SemanticsMidAssign, ErrorsOnStringIndex) {
    // DIM string array but index is string-typed
    std::string src =
        "10 DIM A$(5)\n"
        "20 S$=\"2\"\n"
        "30 MID$(A$(S$),2)=\"X\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}
