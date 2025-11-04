// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen RUN
 * Purpose: Verify RUN resets variables to 0.0 and branches to the first or
 *          specified line label.
 * Components Under Test: CodeGenerator emitLineBlock (RunStmt lowering).
 */
/*
Test: CodeGenRun.RunWithTarget_ResetsAndBranches
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenRun, RunWithTarget_ResetsAndBranches) {
    const auto src =
        "10 LET A = 1 : LET B = 2\n"
        "20 RUN \"demo.bas\", 100\n"
        "30 END\n"
        "100 PRINT A, B\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    // Expect stores to zero for A and B on the RUN line (line20 block)
    EXPECT_NE(ir.find("line20:"), std::string::npos);
    EXPECT_NE(ir.find("store double 0.0, ptr %A"), std::string::npos);
    EXPECT_NE(ir.find("store double 0.0, ptr %B"), std::string::npos);
    // And branch to line100
    EXPECT_NE(ir.find("br label %line100"), std::string::npos);
}

// moved: RunNoArg_BranchesToFirstLine (see test_codegen_run_noarg_branches_first_line.cpp)
