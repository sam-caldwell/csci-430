// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

static const std::vector<std::string> kAllMathFns = {
    "SQR","SQRT","ABS","SIN","COS","TAN","ATN","LOG","EXP",
    "INT","FIX","SGN","RND","CINT","CSNG","CDBL"
};

TEST(SemanticsMathAll, RejectsWrongArityZeroAndTwo) {
    for (const auto& fn : kAllMathFns) {
        std::string src0 = std::string("10 PRINT ") + fn + "()\n20 END\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src0); (void)ir; }, SemanticError) << fn;
        std::string src2 = std::string("10 PRINT ") + fn + "(1,2)\n20 END\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError) << fn;
    }
}

TEST(SemanticsMathAll, RejectsStringArgument) {
    for (const auto& fn : kAllMathFns) {
        std::string src = std::string("10 PRINT ") + fn + "(\"A\")\n20 END\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError) << fn;
    }
}

