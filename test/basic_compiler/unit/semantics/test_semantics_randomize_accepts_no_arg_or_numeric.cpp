// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsRandomize.AcceptsNoArgOrNumeric
 * Purpose: Validate that RANDOMIZE accepts either no argument or a numeric seed.
 * Components Under Test: Compiler::compileString; Semantics (argument type checking)
 * Expected Behavior: Compilation succeeds for bare RANDOMIZE and RANDOMIZE 123.
 */
/*
Test: SemanticsRandomize.AcceptsNoArgOrNumeric
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsRandomize, AcceptsNoArgOrNumeric) {
    const auto src1 =
        "10 RANDOMIZE\n"
        "20 END\n";
    const std::string ir1 = Compiler::compileString(src1);
    ASSERT_NE(ir1.size(), 0u);
    const auto src2 =
        "10 RANDOMIZE 123\n"
        "20 END\n";
    const std::string ir2 = Compiler::compileString(src2);
    ASSERT_NE(ir2.size(), 0u);
}
