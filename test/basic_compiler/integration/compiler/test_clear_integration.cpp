// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_match.h"

using namespace gwbasic;

/***
 * Test: Integration.CLEAR_IRPatterns
 * Purpose: Ensure CLEAR shows variable reset stores in IR.
 * Components: Parser, Semantics, Codegen integration
 * Expected: IR contains 'store float 0.0, ptr %A' after an assignment to A (single-precision default).
 */
TEST(Integration, CLEAR_IRPatterns) {
    const auto src =
        "10 A=7\n"
        "20 CLEAR\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_TRUE(irtest::irContainsAny(ir, {"fptrunc double 7.0 to float"}));
    ASSERT_NE(ir.find("store float 0.0, ptr %A"), std::string::npos);
}
