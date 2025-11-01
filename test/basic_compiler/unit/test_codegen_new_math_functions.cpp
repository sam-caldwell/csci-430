// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenMathExt, RndCintCsngCdbl) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 PRINT CINT(1.6)\n"
        "30 PRINT CSNG(1.5)\n"
        "40 PRINT CDBL(2)\n"
        "50 END\n";
    std::string ir = Compiler::compileString(src);
    // RND -> drand48
    EXPECT_NE(ir.find("declare double @drand48()"), std::string::npos);
    EXPECT_NE(ir.find("call double @drand48()"), std::string::npos);
    // CINT -> round
    EXPECT_NE(ir.find("declare double @round(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @round(double"), std::string::npos);
    // CSNG -> fptrunc/fpext sequence
    EXPECT_NE(ir.find(" = fptrunc double"), std::string::npos);
    EXPECT_NE(ir.find(" = fpext float"), std::string::npos);
}

