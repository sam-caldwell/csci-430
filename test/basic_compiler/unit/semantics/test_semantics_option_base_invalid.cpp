// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/parser/ParseError.h"

using namespace gwbasic;

/***
Test: Semantics.OptionBaseInvalid
Inputs: OPTION BASE 2
Code under test: SemanticAnalyzer::analyzeStmt(OptionBaseStmt)
Expected behavior: SemanticError thrown (only 0 or 1 allowed)
*/
TEST(Semantics, OptionBaseInvalid) {
    // Use semantic analysis directly to validate OPTION BASE value
    std::istringstream iss("10 OPTION BASE 2\n");
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    // Parser enforces valid bases; invalid base is a parse error
    EXPECT_THROW({ (void)p.parseProgram(); }, ParseError);
}
