// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.SqrtNegativeNonConstantPasses
 * Purpose: Validate that SQR of a negative non-constant expression is allowed (evaluated at runtime).
 * Components Under Test: Compiler::compileString; CodeGen (intrinsic calls)
 * Expected Behavior: Compilation succeeds and IR contains a call to @sqrt(double).
 */
TEST(SemanticsMathDomains, SqrtNegativeNonConstantPasses) {
    const auto src =
        "10 LET A = -1\n"
        "20 PRINT SQR(A)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("call double @sqrt(double"), std::string::npos);
}
