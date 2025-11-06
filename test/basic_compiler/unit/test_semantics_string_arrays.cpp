// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

TEST(SemanticsStringArrays, AllowsStringAssignToStringArray) {
    std::string src =
        "10 DIM A$(5)\n"
        "20 A$(2)=\"HI\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_NO_THROW(sa.analyze(prog));
}

TEST(SemanticsStringArrays, ForbidsStringAssignToNumericArray) {
    std::string src =
        "10 DIM A(5)\n"
        "20 A(2)=\"HI\"\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

TEST(SemanticsStringArrays, ForbidsNumericAssignToStringArray) {
    std::string src =
        "10 DIM A$(5)\n"
        "20 A$(2)=123\n";
    Lexer lx(src); auto toks = lx.tokenize(); Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sa; EXPECT_THROW(sa.analyze(prog), SemanticError);
}

