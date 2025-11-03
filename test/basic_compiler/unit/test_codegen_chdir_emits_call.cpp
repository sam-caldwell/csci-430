// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenCHDIR, EmitsChdirCallAndDecl) {
    const char* src =
        "10 CHDIR \"/tmp\"\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i32 @chdir(ptr)"), std::string::npos);
    ASSERT_NE(ir.find("call i32 @chdir(ptr"), std::string::npos);
}

