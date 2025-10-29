// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Loop (STEP 2)
 * Purpose: Validate FOR with explicit STEP 2 emits increment code.
 * Components Under Test: CodeGenerator emitFor.
 * Expected Behavior: Presence of cond label and fadd increment using step.
 */
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenLoopsInput, ForLoopWithStepTwo) {
    const auto src =
        "10 FOR I = 1 TO 5 STEP 2: NEXT\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10_for_cond1:"), std::string::npos);
    EXPECT_NE(ir.find(" = fadd double %"), std::string::npos);
}

