// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.CLEAR_IRPatterns
 * Purpose: Ensure CLEAR shows variable reset stores in IR.
 * Components: Parser, Semantics, Codegen integration
 * Expected: IR contains 'store double 0.0, ptr %A' after an assignment to A.
 */
/*
Test: Integration.CLEAR_IRPatterns
Inputs: Small program with assignment, CLEAR, END
Code under test: Full compile pipeline
Expected behavior: IR contains reset store to A
*/
TEST(Integration, CLEAR_IRPatterns) {
    const char* src =
        "10 A=7\n"
        "20 CLEAR\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("store double 7.0"), std::string::npos);
    ASSERT_NE(ir.find("store double 0.0, ptr %A"), std::string::npos);
}

