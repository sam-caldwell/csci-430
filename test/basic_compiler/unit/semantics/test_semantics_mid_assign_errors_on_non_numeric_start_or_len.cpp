// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsMidAssign.ErrorsOnNonNumericStartOrLen
Purpose: Validate MID$ requires numeric start (and optional length) arguments.
Components Under Test: SemanticAnalyzer::analyze for MidAssignStmt.
Expected Behavior: Throws SemanticError when start or length are non-numeric.
*/
TEST(SemanticsMidAssign, ErrorsOnNonNumericStartOrLen) {
    std::string src1 = "10 A$=\"HELLO\"\n20 MID$(A$,\"3\")=\"X\"\n";
    Lexer lx1(src1); auto toks1 = lx1.tokenize(); Parser p1(std::move(toks1));
    auto prog1 = p1.parseProgram(); SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog1), SemanticError);

    std::string src2 = "10 A$=\"HELLO\"\n20 MID$(A$,3,\"1\")=\"X\"\n";
    Lexer lx2(src2); auto toks2 = lx2.tokenize(); Parser p2(std::move(toks2));
    auto prog2 = p2.parseProgram(); EXPECT_THROW(sa.analyze(prog2), SemanticError);
}
