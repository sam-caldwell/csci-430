// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Loop (multi-line body)
 * Purpose: Ensure multi-line FOR loop emits body/cond labels and printf call.
 */
TEST(CodeGenLoops, ForLoopMultilineBody) {
    const auto src =
        "10 FOR I = 1 TO 3\n"
        "20 PRINT I\n"
        "30 NEXT I\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Look for loop structure and a printf call from PRINT in the body
    EXPECT_NE(ir.find("line10_for_cond"), std::string::npos);
    EXPECT_NE(ir.find("line10_for_body"), std::string::npos);
    EXPECT_NE(ir.find("@printf"), std::string::npos);
}

