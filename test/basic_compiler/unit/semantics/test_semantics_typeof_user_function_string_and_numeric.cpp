// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

static Program parse_tf(const std::string& src) {
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    return p.parseProgram();
}

/***
Test: SemanticsTypeOf.UserFunctionStringAndNumeric
Purpose: Validate type-of classification for user-defined numeric and string functions.
Components Under Test: SemanticAnalyzer::analyze with DEF FN (string and numeric).
Expected Behavior: Both programs analyze without semantic errors.
*/
TEST(SemanticsTypeOf, UserFunctionStringAndNumeric) {
    // String-returning DEF FN (name ends with $)
    auto prog1 = parse_tf("10 DEF FNS$(A$)=A$: PRINT FNS$(\"X\")+\"Y\"\n");
    SemanticAnalyzer sem1;
    EXPECT_NO_THROW({ (void)sem1.analyze(prog1); });

    // Numeric-returning DEF FN (no $)
    auto prog2 = parse_tf("10 DEF FNN(A)=A: PRINT FNN(2)+3\n");
    SemanticAnalyzer sem2;
    EXPECT_NO_THROW({ (void)sem2.analyze(prog2); });
}
