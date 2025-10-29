// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Control Flow (GOSUB missing target)
 * Purpose: Verify missing subroutine target causes immediate branch to cont.
 * Components Under Test: CodeGenerator emitSubroutineInline.
 * Expected Behavior: Entry label for subroutine and branch to continuation.
 */
TEST(CodeGenFlow, GosubToMissingTargetFallsThrough) {
    const auto src =
        "10 GOSUB 9999\n"
        "20 PRINT 1\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10_gosub_entry1:"), std::string::npos);
    EXPECT_NE(ir.find("  br label %line10_gosub_cont1"), std::string::npos);
}

