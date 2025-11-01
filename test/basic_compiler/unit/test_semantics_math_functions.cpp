// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsMath, SqrtRequiresOneNumericArg) {
    // Too few args
    const auto src1 =
        "10 PRINT SQR()\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);
    // Non-numeric arg
    const auto src2 =
        "10 PRINT SQR(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}

