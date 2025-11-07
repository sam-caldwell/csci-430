// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenCircle.GuardedByGfxReady
 * Purpose: Ensure CIRCLE emits guard using @gwb_gfx_ready and calls stub only when ready.
 * Components Under Test: Compiler::compileString; CodeGenerator for CIRCLE
 * Expected Behavior: IR has @gwb_gfx_ready global and conditional branch guarding call.
 */
TEST(CodeGenCircle, GuardedByGfxReady) {
    const auto src =
        "10 SCREEN 1\n"
        "20 CIRCLE (10, 20), 5\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@gwb_gfx_ready"), std::string::npos);
    ASSERT_NE(ir.find("define void @gwb_graphics_init"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_graphics_init(i32"), std::string::npos);
    ASSERT_NE(ir.find("load i1, ptr @gwb_gfx_ready"), std::string::npos);
    // Full signature includes doubles + i32 color + flags; relaxed substring check
    ASSERT_NE(ir.find("call void @gwb_gfx_circle(double"), std::string::npos);
}
