// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/*
Test: SemanticsExpectedArity.NumericIntrinsicsAreUnary
Inputs: Intrinsic names (uppercase) across numeric set
Code under test: SemanticAnalyzer::expectedArity
Expected behavior: Returns 1 for all recognized numeric intrinsics
*/
TEST(SemanticsExpectedArity, NumericIntrinsicsAreUnary) {
    static const std::vector<std::string> kFns = {
        "SQR","SQRT","ABS","SIN","COS","TAN","ATN","LOG","EXP",
        "INT","FIX","SGN","CINT","CSNG","CDBL","RND","PEEK","USR","ASC"
    };
    for (const auto& fn : kFns) {
        EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity(fn), 1) << fn;
    }
}
