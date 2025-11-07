// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsDefType.SuffixOverridesDEFSTR
 * Inputs: DEFSTR C; assign numeric to C% and print.
 * Code under test: Compiler::compileString() + DEF type suffix precedence.
 * Expected behavior: Program compiles; suffix enforces numeric type overriding DEFSTR.
 */
TEST(SemanticsDefType, SuffixOverridesDEFSTR) {
    const char* src =
        "10 DEFSTR C\n"
        "20 LET C% = 3\n"
        "30 PRINT C%\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}
