// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(Integration, StringFuncs_IRContainsStrncpy) {
    const char* src =
        "10 PRINT LEFT$(\"HELLO\",2)\n"
        "20 PRINT RIGHT$(\"HELLO\",3)\n"
        "30 PRINT MID$(\"HELLO\",2,2)\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}

