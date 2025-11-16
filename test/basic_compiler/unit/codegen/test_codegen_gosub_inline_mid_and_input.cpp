// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenGosub.InlineBodyMidAndInput
 * Purpose: Drive emitSubroutineInline through INPUT, MID$ on array element, and control flow.
 * Components Under Test: CodeGenerator emitSubroutineInline
 * Expected Behavior: IR contains gosub entry/return labels, scanf for INPUT, strncpy for MID$.
 */
TEST(CodeGenGosub, InlineBodyMidAndInput) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM T$(3)\n"
        "30 GOSUB 100\n"
        "40 END\n"
        "100 INPUT X\n"
        "110 MID$(T$(2),2,1)=\"A\"\n"
        "120 PRINT T$(2)\n"
        "130 RETURN\n";
    const std::string ir = Compiler::compileString(src);
    // Inline gosub emits entry and continuation labels
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
    ASSERT_NE(ir.find("_gosub_cont"), std::string::npos);
    // INPUT lowers to scanf
    ASSERT_NE(ir.find("@scanf"), std::string::npos);
    // MID$ lowering uses strncpy
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
