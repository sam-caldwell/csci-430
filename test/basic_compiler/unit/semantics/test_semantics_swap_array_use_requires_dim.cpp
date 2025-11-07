// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics_SWAP.ArrayUseRequiresDim
 * Purpose: Ensure SWAP on array elements requires prior DIM of those arrays.
 * Components Under Test: SemanticAnalyzer validation for array use in SWAP.
 * Expected Behavior: Throws SemanticError mentioning array not DIM'd.
 */
TEST(Unit, Semantics_SWAP_ArrayUseRequiresDim) {
    const char* src = "10 SWAP A(1), B(1)\n";
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
        ASSERT_NE(msg.find("array 'A' not DIM'd"), std::string::npos) << msg;
    }
}
