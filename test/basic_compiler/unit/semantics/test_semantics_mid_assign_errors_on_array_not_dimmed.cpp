// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsMidAssign.ErrorsOnArrayNotDimmed
Purpose: MID$ assignment to a string array element should require DIM prior to use.
Components Under Test: SemanticAnalyzer::analyze for MidAssignStmt.
Expected Behavior: Throws SemanticError when array has not been DIM'd.
*/
TEST(SemanticsMidAssign, ErrorsOnArrayNotDimmed) {
    // MID$(A$(1),2) without DIM A$()
    std::string src = "10 MID$(A$(1),2)=\"X\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}
