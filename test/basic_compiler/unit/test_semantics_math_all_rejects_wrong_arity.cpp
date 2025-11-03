// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathAll.RejectsWrongArityZeroAndTwo
 * Purpose: Validate that math intrinsics reject arities other than one (zero or two args).
 * Components Under Test: Compiler::compileString; Semantics (intrinsic arity checking)
 * Expected Behavior: Compilation throws SemanticError for calls with no args and with two args.
 */
TEST(SemanticsMathAll, RejectsWrongArityZeroAndTwo) {
    static const std::vector<std::string> kFns = {
        "SQR","SQRT","ABS","SIN","COS","TAN","ATN","LOG","EXP",
        "INT","FIX","SGN","RND","CINT","CSNG","CDBL"
    };
    for (const auto& fn : kFns) {
        std::string src0 = std::string("10 PRINT ") + fn + "()\n20 END\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src0); (void)ir; }, SemanticError) << fn;
        std::string src2 = std::string("10 PRINT ") + fn + "(1,2)\n20 END\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError) << fn;
    }
}
