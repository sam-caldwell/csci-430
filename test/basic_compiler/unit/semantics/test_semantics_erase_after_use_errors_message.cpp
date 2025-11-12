// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/semantics/SemanticAnalyzer.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Unit.Semantics_ERASE_ArrayUseAfterEraseErrorsMessage
 * Purpose: Ensure PRINT A(1) immediately after ERASE A fails with a clear
 *          "array not DIM'd" message instead of an unknown function error.
 * Components: Lexer, Parser, SemanticAnalyzer
 * Expected: SemanticError thrown; message contains "array 'A' not DIM'd".
 */
/*
Test: Unit.Semantics_ERASE_ArrayUseAfterEraseErrorsMessage
Inputs: DIM A(3), ERASE A, PRINT A(1)
Code under test: SemanticAnalyzer::analyze
Expected behavior: Throws SemanticError with message containing "array 'A' not DIM'd"
*/
TEST(Unit, Semantics_ERASE_ArrayUseAfterEraseErrorsMessage) {
    const char* src =
        "10 DIM A(3)\n"
        "20 ERASE A\n"
        "30 PRINT A(1)\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    SemanticAnalyzer sem;
    try {
        (void)sem.analyze(prog);
        FAIL() << "Expected SemanticError for array not DIM'd after ERASE";
    } catch (const SemanticError& e) {
        std::string msg = e.what();
        ASSERT_NE(msg.find("array 'A' not DIM'd"), std::string::npos) << msg;
    }
}

