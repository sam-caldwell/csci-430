// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_match.h"

using namespace gwbasic;

/***
 * Test: CodeGenClear.EmitsVariableResets
 * Purpose: Ensure CLEAR resets scalars to 0.0 in IR after prior assignments.
 * Components Under Test: Compiler::compileString; Codegen for CLEAR
 * Expected: IR contains store of non-zero followed by store of 0.0 (typed) for vars.
 */
TEST(CodeGenClear, EmitsVariableResets) {
    const auto src =
        "10 A=5: B=9\n"
        "20 CLEAR\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_TRUE(irtest::irContainsAny(ir, {"fptrunc double 5.0 to float"}));
    ASSERT_TRUE(irtest::irContainsAny(ir, {"fptrunc double 9.0 to float"}));
    ASSERT_NE(ir.find("store float 0.0, ptr %A"), std::string::npos);
    ASSERT_NE(ir.find("store float 0.0, ptr %B"), std::string::npos);
}
