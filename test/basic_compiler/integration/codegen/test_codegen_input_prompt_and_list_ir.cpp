// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenInput.PromptAndList_EmitsPrintfAndMultipleScanfs
 * Purpose: Ensure INPUT with a literal prompt and var list prints the prompt and scans multiple variables.
 * Components Under Test: CodeGenerator emission for InputStmt (prompt literal + scanf list).
 * Expected Behavior: IR contains a printf for the prompt and at least two scanf calls.
 */
TEST(CodeGenInput, PromptAndList_EmitsPrintfAndMultipleScanfs) {
    const auto src =
        "10 INPUT \"Vals:\"; A, B\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    // Expect at least two occurrences of scanf
    size_t p1 = ir.find("@scanf"); ASSERT_NE(p1, std::string::npos);
    size_t p2 = ir.find("@scanf", p1 + 1); ASSERT_NE(p2, std::string::npos);
}
