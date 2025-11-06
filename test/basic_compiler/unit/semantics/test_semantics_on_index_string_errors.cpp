// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/*
Test: Semantics.OnIndex_StringConditionErrors
Inputs: Program with ON "str" GOTO ...
Code under test: SemanticAnalyzer
Expected behavior: TypeError raised for string index
*/
TEST(Semantics, OnIndex_StringConditionErrors) {
    std::string src = "10 ON \"X\" GOTO 100\n100 END\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem; sem.setStrictControlFlow(true);
    EXPECT_THROW({ (void)sem.analyze(prog); }, SemanticError);
}

