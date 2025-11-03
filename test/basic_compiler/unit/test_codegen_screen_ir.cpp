// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenScreen, EmitsGlobalsAndHelper) {
    const char* src =
        "10 PRINT \"AB\"\n"
        "20 X=SCREEN(1,1)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@gwb_screen"), std::string::npos);
    ASSERT_NE(ir.find("declare i32 @snprintf"), std::string::npos);
    ASSERT_NE(ir.find("define void @gwb_screen_write"), std::string::npos);
}

