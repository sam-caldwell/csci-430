// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(Integration, StringArray_IR_UsesPtrElements) {
    const char* src =
        "10 DIM A$(5)\n"
        "20 A$(2)=\"HI\"\n"
        "30 PRINT A$(2)\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    // Expect element type as ptr in gep and store ptr
    ASSERT_NE(ir.find("alloca [5 x ptr]"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [5 x ptr]"), std::string::npos);
    ASSERT_NE(ir.find("store ptr"), std::string::npos);
}

