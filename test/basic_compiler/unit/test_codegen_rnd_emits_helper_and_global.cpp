// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenRND, EmitsHelperAndGlobal) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@gwb_last_rnd = global double 0.0"), std::string::npos);
    EXPECT_NE(ir.find("define double @gwb_rnd(double"), std::string::npos);
    EXPECT_NE(ir.find("call double @gwb_rnd(double"), std::string::npos);
}

