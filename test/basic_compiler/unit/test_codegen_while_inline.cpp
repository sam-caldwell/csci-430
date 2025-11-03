// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen WHILE (inline)
 * Purpose: Validate WHILE emits cond/body/end labels and branches.
 */
TEST(CodeGenWhile, WhileInlineGeneratesLabels) {
    const auto src =
        "10 LET I = 1: WHILE I < 3: PRINT I: LET I = I + 1: WEND\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("_while_cond"), std::string::npos);
    EXPECT_NE(ir.find("_while_body"), std::string::npos);
    EXPECT_NE(ir.find("_while_end"), std::string::npos);
}

