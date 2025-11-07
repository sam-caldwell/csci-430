// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

static Program parse_arr(const std::string& src) {
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    return p.parseProgram();
}

/***
 * Test: SemanticsTypeOf.ArrayElementStringVsNumeric
 * Purpose: Ensure type system allows string concat on string array elements and numeric arithmetic on numeric arrays.
 * Components Under Test: SemanticAnalyzer::typeOf and analyze on array element expressions.
 * Expected Behavior: No error for valid cases; error when mixing string array element with number in assignment.
 */
TEST(SemanticsTypeOf, ArrayElementStringVsNumeric) {
    // String array element concatenation allowed
    auto p1 = parse_arr("10 DIM A$(3): PRINT A$(1)+\"Z\"\n");
    SemanticAnalyzer s1;
    EXPECT_NO_THROW({ (void)s1.analyze(p1); });

    // Numeric array element arithmetic allowed
    auto p2 = parse_arr("10 DIM A(3): PRINT A(1)+1\n");
    SemanticAnalyzer s2;
    EXPECT_NO_THROW({ (void)s2.analyze(p2); });

    // Mixing string array element with number should error when used in assignment
    auto p3 = parse_arr("10 DIM A$(3)\n20 A$=A$(1)+1\n");
    SemanticAnalyzer s3;
    EXPECT_THROW({ (void)s3.analyze(p3); }, SemanticError);
}
