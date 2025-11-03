// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

// Purpose: Verify COLOR emits SGR printf usage instead of static tables.
TEST(CodeGenColor, EmitsSgrPrintf) {
    const char* src =
        "10 COLOR 2,4\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_sgr"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    // First vararg should be literal 27 (ESC)
    ASSERT_NE(ir.find("call i32 (ptr, ...) @printf(ptr %"), std::string::npos);
    ASSERT_NE(ir.find(", i32 27, i32"), std::string::npos);
}

