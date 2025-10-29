// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Core (print string)
 * Purpose: Validate IR for PRINT string path and global string emission.
 * Components Under Test: CodeGenerator emitGlobals, emitLineBlock.
 * Expected Behavior: @.fmt_str declaration, @.str.N global, and GEP to
 *          @.fmt_str in emitted IR.
 */
TEST(CodeGenCore, PrintStringAndGlobals) {
    const auto src = "10 PRINT \"Hello\"\n20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.fmt_str = private unnamed_addr constant"), std::string::npos);
    EXPECT_NE(ir.find("@.str.0 = private unnamed_addr constant"), std::string::npos);
    EXPECT_NE(ir.find("getelementptr inbounds i8, ptr @.fmt_str"), std::string::npos);
}

