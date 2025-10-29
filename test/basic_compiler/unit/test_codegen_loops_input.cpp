// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Loops & Input
 * Purpose: Validate lowering of FOR/NEXT loops (default and explicit STEP)
 *          and INPUT statements using scanf format.
 * Components Under Test: CodeGenerator emitFor, emitLineBlock,
 *          ensureVarAllocated.
 * Expected Behavior: Loop emits cond/body/inc/end blocks with inclusive
 *          compare (ole) and fadd increment; INPUT uses @.fmt_in and scanf.
 */
TEST(CodeGenLoopsInput, ForLoopDefaultStepLabelsAndOps) {
    const auto src =
        "10 FOR I = 1 TO 3: PRINT I: NEXT\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    // Check loop blocks and operations
    EXPECT_NE(ir.find("line10_for_cond1:"), std::string::npos);
    EXPECT_NE(ir.find("line10_for_body1:"), std::string::npos);
    EXPECT_NE(ir.find("line10_for_inc1:"), std::string::npos);
    EXPECT_NE(ir.find("line10_for_end1:"), std::string::npos);
    EXPECT_NE(ir.find(" = fcmp ole double"), std::string::npos); // inclusive end
    EXPECT_NE(ir.find(" = fadd double"), std::string::npos);     // increment
}

// Single-test-per-file policy: additional cases moved to dedicated files.
