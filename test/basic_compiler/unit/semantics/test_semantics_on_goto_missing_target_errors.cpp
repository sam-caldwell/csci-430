// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/*
Test: Semantics.OnGoto_MissingTargetErrors
Inputs: Program with ON 1 GOTO <missing>
Code under test: SemanticAnalyzer
Expected behavior: ControlFlowError on missing target
*/
TEST(Semantics, OnGoto_MissingTargetErrors) {
    std::string src = "10 ON 1 GOTO 9999\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem; sem.setStrictControlFlow(true);
    EXPECT_THROW({ (void)sem.analyze(prog); }, SemanticError);
}

