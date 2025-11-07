// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

TEST(SemanticsMidAssign, ErrorsOnNumericArrayTarget) {
    // DIM numeric array and attempt MID$ on its element
    std::string src =
        "10 DIM A(5)\n"
        "20 MID$(A(1),2)=\"X\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

