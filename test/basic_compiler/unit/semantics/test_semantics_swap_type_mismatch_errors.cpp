// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics_SWAP.TypeMismatchErrors
 * Purpose: Ensure SWAP between string and numeric variables is rejected.
 * Components Under Test: SemanticAnalyzer type checking for SWAP.
 * Expected Behavior: Throws SemanticError with a message about mixed types.
 */
TEST(Unit, Semantics_SWAP_TypeMismatchErrors) {
    const char* src = "10 SWAP A$, B\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem;
    try {
        (void)sem.analyze(prog);
        FAIL() << "Expected SemanticError";
    } catch (const SemanticError& e) {
        std::string msg = e.what();
        ASSERT_NE(msg.find("SWAP operands must be both numeric or both string"), std::string::npos) << msg;
    }
}
