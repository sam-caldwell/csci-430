// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

TEST(SemanticsMidAssign, ErrorsOnArrayNotDimmed) {
    // MID$(A$(1),2) without DIM A$()
    std::string src = "10 MID$(A$(1),2)=\"X\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

TEST(SemanticsMidAssign, ErrorsOnNumericArrayTarget) {
    // DIM numeric array and attempt MID$ on its element
    std::string src =
        "10 DIM A(5)\n"
        "20 MID$(A(1),2)=\"X\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

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

TEST(SemanticsMidAssign, ErrorsOnNonStringValue) {
    // Scalar target but RHS is numeric
    std::string src =
        "10 A$=\"HELLO\"\n"
        "20 MID$(A$,3,1)=5\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

TEST(SemanticsMidAssign, ErrorsOnNonNumericStartOrLen) {
    std::string src1 = "10 A$=\"HELLO\"\n20 MID$(A$,\"3\")=\"X\"\n";
    Lexer lx1(src1); auto toks1 = lx1.tokenize(); Parser p1(std::move(toks1));
    auto prog1 = p1.parseProgram(); SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog1), SemanticError);

    std::string src2 = "10 A$=\"HELLO\"\n20 MID$(A$,3,\"1\")=\"X\"\n";
    Lexer lx2(src2); auto toks2 = lx2.tokenize(); Parser p2(std::move(toks2));
    auto prog2 = p2.parseProgram(); EXPECT_THROW(sa.analyze(prog2), SemanticError);
}
