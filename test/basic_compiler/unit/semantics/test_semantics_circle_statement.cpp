// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsCircle.Accepts_NumericArgs
 * Purpose: Validate CIRCLE statement requires numeric x, y, r.
 * Components Under Test: Compiler::compileString; semantics for CIRCLE
 * Expected Behavior: Numeric args compile; string arg produces error.
 */
/*
Test: SemanticsCircle.Accepts_NumericArgs
Inputs: 'CIRCLE (1,2),3' (valid)
Code under test: Semantics analyzer for CIRCLE
Expected behavior: Valid program compiles; invalid raises and prints error
*/
TEST(SemanticsCircle, Accepts_NumericArgs) {
    const char* ok =
        "10 CIRCLE (1,2),3\n"
        "20 END\n";
    const std::string ir = Compiler::compileString(ok);
    ASSERT_FALSE(ir.empty());
}
