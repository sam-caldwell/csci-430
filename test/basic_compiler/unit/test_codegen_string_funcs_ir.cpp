// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenStringFuncs, EmitsStrncpyAndMalloc) {
    const char* src =
        "10 PRINT LEFT$(\"HELLO\",2)\n"
        "20 PRINT RIGHT$(\"HELLO\",3)\n"
        "30 PRINT MID$(\"HELLO\",2)\n"
        "40 PRINT MID$(\"HELLO\",2,2)\n"
        "50 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare ptr @strncpy(ptr, ptr, i64)"), std::string::npos);
    ASSERT_NE(ir.find("call ptr @malloc(i64"), std::string::npos);
    ASSERT_NE(ir.find("call ptr @strncpy(ptr"), std::string::npos);
}

