// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefType.NumericDefsParse
 * Purpose: Ensure DEFINT/DEFSNG/DEFDBL parse and programs compile.
 */
/***
Test: SemanticsDefType.NumericDefsParse
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsDefType, NumericDefsParse) {
    const char* src =
        "10 DEFINT A-C\n"
        "20 DEFSNG D-F\n"
        "30 DEFDBL X-Z\n"
        "40 LET A = 2\n"
        "50 LET D = 3\n"
        "60 LET X = 4\n"
        "70 PRINT A + D + X\n"
        "80 END\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
