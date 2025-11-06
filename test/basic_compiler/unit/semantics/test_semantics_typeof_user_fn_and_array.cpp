// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

static Program parse(const std::string& src) {
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    return p.parseProgram();
}

TEST(SemanticsTypeOf, UserFunctionStringAndNumeric) {
    // String-returning DEF FN (name ends with $)
    auto prog1 = parse("10 DEF FNS$(A$)=A$: PRINT FNS$(\"X\")+\"Y\"\n");
    SemanticAnalyzer sem1;
    EXPECT_NO_THROW({ (void)sem1.analyze(prog1); });

    // Numeric-returning DEF FN (no $)
    auto prog2 = parse("10 DEF FNN(A)=A: PRINT FNN(2)+3\n");
    SemanticAnalyzer sem2;
    EXPECT_NO_THROW({ (void)sem2.analyze(prog2); });
}

TEST(SemanticsTypeOf, ArrayElementStringVsNumeric) {
    // String array element concatenation allowed
    auto p1 = parse("10 DIM A$(3): PRINT A$(1)+\"Z\"\n");
    SemanticAnalyzer s1;
    EXPECT_NO_THROW({ (void)s1.analyze(p1); });

    // Numeric array element arithmetic allowed
    auto p2 = parse("10 DIM A(3): PRINT A(1)+1\n");
    SemanticAnalyzer s2;
    EXPECT_NO_THROW({ (void)s2.analyze(p2); });

    // Mixing string array element with number should error when used in assignment
    auto p3 = parse("10 DIM A$(3)\n20 A$=A$(1)+1\n");
    SemanticAnalyzer s3;
    EXPECT_THROW({ (void)s3.analyze(p3); }, SemanticError);
}
