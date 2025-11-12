// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: Unit.Semantics_ERASE_AllowsRedim
 * Purpose: Ensure that after ERASE, a subsequent DIM re-enables array usage.
 * Components: Lexer, Parser, SemanticAnalyzer
 * Expected: No semantic error for PRINT A(1) after ERASE and re-DIM.
 */
/*
Test: Unit.Semantics_ERASE_AllowsRedim
Inputs: Program DIM A(3), ERASE A, DIM A(3), PRINT A(1)
Code under test: SemanticAnalyzer::analyze
Expected behavior: analyze() completes without throwing
*/
TEST(Unit, Semantics_ERASE_AllowsRedim) {
    const char* src =
        "10 DIM A(3)\n"
        "20 ERASE A\n"
        "30 DIM A(3)\n"
        "40 PRINT A(1)\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem;
    EXPECT_NO_THROW({ (void)sem.analyze(prog); });
}

