// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Control Flow (GOTO)
 * Purpose: Verify GOTO lowers to an unconditional branch to the target label.
 * Components Under Test: CodeGenerator emitLineBlock.
 * Expected Behavior: line10 contains a br label %line200.
 */
TEST(CodeGenFlow, GotoBranchesToTarget) {
    const auto src =
        "10 GOTO 200\n"
        "100 PRINT 1\n"
        "200 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
    EXPECT_NE(ir.find("  br label %line200"), std::string::npos);
}

