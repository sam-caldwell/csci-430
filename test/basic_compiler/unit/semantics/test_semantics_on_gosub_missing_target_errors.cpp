// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: Semantics.OnGosub_MissingTargetErrors
Inputs: Program with ON 1 GOSUB <missing>
Code under test: SemanticAnalyzer
Expected behavior: ControlFlowError on missing target
*/
TEST(Semantics, OnGosub_MissingTargetErrors) {
    std::string src = "10 ON 1 GOSUB 9999\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem; sem.setStrictControlFlow(true);
    EXPECT_THROW({ (void)sem.analyze(prog); }, SemanticError);
}
