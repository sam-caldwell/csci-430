// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.CIRCLE_IRPatterns
 * Purpose: Ensure a program using CIRCLE emits guarded gfx call and gfx helper.
 * Components: Parser, Semantics, Codegen integration
 * Expected: IR contains @gwb_gfx_circle definition, gfx_ready guard, and a call.
 */
/*
Test: Integration.CIRCLE_IRPatterns
Inputs: Small program with SCREEN + CIRCLE + END
Code under test: Full compile pipeline
Expected behavior: IR contains graphics helper and usage
*/
TEST(Integration, CIRCLE_IRPatterns) {
    const char* src =
        "10 SCREEN 1\n"
        "20 CIRCLE (5,5), 2,, 0, 3.14\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("define void @gwb_gfx_circle"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_gfx_ready"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_gfx_circle(double"), std::string::npos);
}

