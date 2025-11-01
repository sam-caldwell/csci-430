// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(Semantics, GotoValidTargetCompiles) {
    const auto src =
        "10 GOTO 30\n"
        "20 PRINT 1\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
}

