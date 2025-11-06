// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Unit.Semantics_SWAP_TypeMismatchErrors
 * Purpose: Ensure SWAP rejects mixing string and numeric operands.
 * Components: Lexer, Parser, SemanticAnalyzer
 * Expected: SemanticError with message containing "SWAP operands must be both numeric or both string".
 */
/*
Test: Unit.Semantics_SWAP_TypeMismatchErrors
Inputs: "10 SWAP A$, B\n"
Code under test: SemanticAnalyzer::analyze
Expected behavior: Throws SemanticError for mixing string and numeric in SWAP
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

/***
 * Test: Unit.Semantics_SWAP_ArrayUseRequiresDim
 * Purpose: Ensure SWAP on array elements requires prior DIM and correct arity.
 * Components: Lexer, Parser, SemanticAnalyzer
 * Expected: SemanticError with message containing "array 'A' not DIM'd".
 */
/*
Test: Unit.Semantics_SWAP_ArrayUseRequiresDim
Inputs: "10 SWAP A(1), B(1)\n"
Code under test: SemanticAnalyzer::analyze
Expected behavior: Throws SemanticError for array not DIM'd
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

