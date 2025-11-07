// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLineBlock.While_Gosub_ChannelPrint
 * Purpose: Exercise emitLineBlock paths that were undercovered: WHILE as a
 *          single line block, GOSUB inline trampoline, and channel PRINT.
 * Components Under Test: CodeGenerator::emitLineBlock, emitWhile, emitSubroutineInline.
 * Expected Behavior: IR contains while labels, channel printing via @gwb_files,
 *          and inline gosub entry/cont labels.
 */
TEST(CodeGenLineBlock, While_Gosub_ChannelPrint) {
    const std::string src =
        "10 X=0: WHILE X<1: PRINT #1, \"Z\": X=X+1: WEND: GOSUB 100\n"
        "20 END\n"
        "100 PRINT 1: RETURN\n";
    const std::string ir = Compiler::compileString(src);
    // WHILE labels present
    EXPECT_NE(ir.find("line10_while_cond"), std::string::npos);
    EXPECT_NE(ir.find("line10_while_body"), std::string::npos);
    EXPECT_NE(ir.find("line10_while_end"), std::string::npos);
    // Channel printing evidence
    EXPECT_NE(ir.find("@gwb_files"), std::string::npos);
    // Inline gosub trampoline labels
    EXPECT_NE(ir.find("_gosub_entry"), std::string::npos);
    EXPECT_NE(ir.find("_gosub_cont"), std::string::npos);
}

