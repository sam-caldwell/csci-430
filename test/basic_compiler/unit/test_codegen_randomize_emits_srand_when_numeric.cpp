// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenRandomize, EmitsSrandWhenNumeric) {
    const auto src =
        "10 RANDOMIZE 7\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("declare void @srand48(i64)"), std::string::npos);
    EXPECT_NE(ir.find("call void @srand48(i64"), std::string::npos);
}

