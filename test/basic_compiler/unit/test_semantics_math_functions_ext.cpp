// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMathExt, RecognizesNewIntrinsics) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 PRINT CINT(2.4)\n"
        "30 PRINT CSNG(3.14)\n"
        "40 PRINT CDBL(5)\n"
        "50 END\n";
    // Should compile without semantic errors
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}

TEST(SemanticsMathExt, ArityAndTypeChecks) {
    // Wrong arity should error
    const auto src1 =
        "10 PRINT RND()\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);

    // String arg should error
    const auto src2 =
        "10 PRINT CINT(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}

