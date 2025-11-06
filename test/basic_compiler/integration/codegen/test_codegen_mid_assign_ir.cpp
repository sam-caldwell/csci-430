// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGen, MidAssign_EmitsStrncpyAndBounds) {
    const char* src =
        "10 S$=\"ABCDE\"\n"
        "20 MID$(S$,3)=\"xyz\"\n"
        "30 PRINT S$\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Decl and call present
    ASSERT_NE(ir.find("declare ptr @strncpy(ptr, ptr, i64)"), std::string::npos);
    ASSERT_NE(ir.find("call ptr @strncpy(ptr"), std::string::npos);
    // 1-based to 0-based offset subtract
    ASSERT_NE(ir.find("sub i64 "), std::string::npos);
    // length queries for both dest and src
    ASSERT_NE(ir.find("call i64 @strlen(ptr"), std::string::npos);
}

