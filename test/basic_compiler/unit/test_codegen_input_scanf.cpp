// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Input (scanf)
 * Purpose: Validate INPUT lowers to a scanf call with @.fmt_in.
 * Components Under Test: CodeGenerator emitLineBlock; ensureVarAllocated.
 * Expected Behavior: GEP on @.fmt_in and call to @scanf present in IR.
 */
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenLoopsInput, InputStatementScanf) {
    const auto src =
        "10 INPUT X\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("getelementptr inbounds i8, ptr @.fmt_in"), std::string::npos);
    EXPECT_NE(ir.find("call i32 (ptr, ...) @scanf"), std::string::npos);
}

