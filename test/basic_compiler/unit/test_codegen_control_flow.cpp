// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Control Flow
 * Purpose: Verify lowering for IF/THEN, GOTO, and GOSUB inlining with
 *          synthesized entry/continuation labels and correct branch targets.
 * Components Under Test: CodeGenerator emitLineBlock, emitComparison,
 *          emitSubroutineInline.
 * Expected Behavior: IF emits fcmp and conditional branch to target/cont;
 *          GOTO emits an unconditional branch; GOSUB expands inline with
 *          entry/cont labels; missing target GOSUB falls through to return.
 */
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenFlow, IfThenComparisonGeneratesBranch) {
    const auto src =
        "10 IF 1 < 2 THEN 100\n"
        "20 END\n"
        "100 PRINT 1\n"
        "110 END\n";
    std::string ir = Compiler::compileString(src);
    // Check for conditional fcmp and branch to line100 and cont label
    EXPECT_NE(ir.find(" = fcmp olt double 1.0, 2.0"), std::string::npos);
    EXPECT_NE(ir.find("br i1 %"), std::string::npos);
    EXPECT_NE(ir.find("label %line100"), std::string::npos);
    EXPECT_NE(ir.find("line10_cont1:"), std::string::npos);
}

// Single-test-per-file policy: additional cases moved to dedicated files.
