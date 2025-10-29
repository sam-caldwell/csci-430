// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Control Flow (GOSUB inline)
 * Purpose: Verify GOSUB inlines a subroutine with entry/cont labels and
 *          returns to continuation.
 * Components Under Test: CodeGenerator emitSubroutineInline.
 * Expected Behavior: IR contains line10_gosub_entry1/cont1 labels and
 *          branch from inlined body back to continuation.
 */
TEST(CodeGenFlow, GosubInlineAndReturn) {
    const auto src =
        "10 GOSUB 300\n"
        "20 PRINT \"Done\"\n"
        "30 END\n"
        "300 PRINT \"Sub\"\n"
        "310 RETURN\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10_gosub_entry1:"), std::string::npos);
    EXPECT_NE(ir.find("line10_gosub_cont1:"), std::string::npos);
    EXPECT_NE(ir.find("  br label %line10_gosub_cont1"), std::string::npos);
}

