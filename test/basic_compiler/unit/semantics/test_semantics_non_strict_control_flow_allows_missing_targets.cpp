// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: Semantics.NonStrictControlFlowAllowsMissingTargets
Inputs: GOTO/GOSUB/ON GOTO/ON GOSUB with missing target lines; strict=false
Code under test: AnalyzeStmt branches that warn instead of throwing when strictControlFlow=false
Expected behavior: No exception thrown during analysis
*/
TEST(Semantics, NonStrictControlFlowAllowsMissingTargets) {
    const char* src =
        "10 GOTO 100\n"
        "20 GOSUB 200\n"
        "30 ON 1 GOTO 300,400\n"
        "40 ON 1 GOSUB 500,600\n";
    std::istringstream iss(src);
    Lexer lx(iss); auto toks = lx.tokenize();
    Parser p(std::move(toks)); auto prog = p.parseProgram();
    SemanticAnalyzer sem; sem.setStrictControlFlow(false);
    EXPECT_NO_THROW(sem.analyze(prog));
}
