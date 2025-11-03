// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(Integration, COLOR_IRContainsSgrFormatAndPrintf) {
    const char* src =
        "10 COLOR 2,4\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_sgr"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
}
