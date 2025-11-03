// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenRun.RunNoArg_BranchesToFirstLine
 * Purpose: Validate CodeGen lowering for RUN branches to the first program line.
 * Components Under Test: Compiler; CodeGenerator (RunStmt lowering)
 * Expected Behavior: IR contains the RUN site label (line20) and a branch to
 *                    the first line label (br label %line10).
 */
TEST(CodeGenRun, RunNoArg_BranchesToFirstLine) {
    const auto src =
        "10 LET X = 5\n"
        "20 RUN \"demo.bas\"\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    // Expect branch back to the first line label (line10)
    EXPECT_NE(ir.find("line20:"), std::string::npos);
    EXPECT_NE(ir.find("br label %line10"), std::string::npos);
}
