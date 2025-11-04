// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsScreenStmt.Accepts_Args
 * Purpose: Validate that SCREEN statement accepts optional numeric arguments.
 * Components Under Test: Compiler::compileString; Semantics for SCREEN stmt
 * Expected Behavior: Compilation succeeds for no/valid numeric args and END.
 */
/*
Test: SemanticsScreenStmt.Accepts_Args
Inputs: BASIC snippets: 'SCREEN', 'SCREEN 0', 'SCREEN 1,0,0,0'
Code under test: Semantics analyzer type checking for SCREEN statement
Expected behavior: All compile without semantic errors
*/
TEST(SemanticsScreenStmt, Accepts_Args) {
    const char* src1 =
        "10 SCREEN\n"
        "20 END\n";
    const std::string ir1 = Compiler::compileString(src1);
    ASSERT_FALSE(ir1.empty());

    const char* src2 =
        "10 SCREEN 0\n"
        "20 END\n";
    const std::string ir2 = Compiler::compileString(src2);
    ASSERT_FALSE(ir2.empty());

    const char* src3 =
        "10 SCREEN 1,0,0,0\n"
        "20 END\n";
    const std::string ir3 = Compiler::compileString(src3);
    ASSERT_FALSE(ir3.empty());
}

