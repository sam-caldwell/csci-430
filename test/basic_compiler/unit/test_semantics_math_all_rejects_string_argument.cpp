// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathAll.RejectsStringArgument
 * Purpose: Validate that all math intrinsics reject string arguments.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic argument type checking)
 * Expected Behavior: Compilation throws SemanticError for each intrinsic when invoked with a string.
 */
TEST(SemanticsMathAll, RejectsStringArgument) {
    static const std::vector<std::string> kFns = {
        "SQR","SQRT","ABS","SIN","COS","TAN","ATN","LOG","EXP",
        "INT","FIX","SGN","RND","CINT","CSNG","CDBL"
    };
    for (const auto& fn : kFns) {
        std::string src = std::string("10 PRINT ") + fn + "(\"A\")\n20 END\n";
        EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError) << fn;
    }
}
