// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenCALL, EmitsHelperAndCall) {
    const char* src =
        "10 POKE 100,1\n"
        "20 CALL 100\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("define void @gwb_call(i64"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_call(i64"), std::string::npos);
    ASSERT_NE(ir.find("@.call_msg"), std::string::npos);
}

