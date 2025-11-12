// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

static SemanticAnalyzer::Result analyze_src3(const std::string& src) {
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sema;
    return sema.analyze(prog);
}

/***
 * Test: SemanticsScreen.Accepts_Three_Args
 * Inputs: SCREEN(1,1,0)
 * Code under test: Lexer, Parser, SemanticAnalyzer for SCREEN arity/type.
 * Expected behavior: No semantic error when 3rd arg present and numeric.
 */
TEST(SemanticsScreen, Accepts_Three_Args) {
    const std::string src = "10 X=SCREEN(1,1,0)\n";
    EXPECT_NO_THROW({ (void)analyze_src3(src); });
}
