// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenCircle.FullSignatureAndAngles
 * Purpose: Ensure CIRCLE with color/start/end/aspect emits full-arg gfx call.
 * Components Under Test: Compiler::compileString; CodeGenerator for CIRCLE
 * Expected Behavior: IR defines @gwb_gfx_circle with extended signature and emits a guarded call.
 */
TEST(CodeGenCircle, FullSignatureAndAngles) {
    const auto src =
        "10 SCREEN 1\n"
        "20 CIRCLE (10, 20), 5, 3, 0, 6.28, 1.5\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("define void @gwb_gfx_circle(double %cx, double %cy, double %r, i32 %color, double %start, double %end, double %aspect, i1 %step)"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_gfx_circle(double"), std::string::npos);
}

