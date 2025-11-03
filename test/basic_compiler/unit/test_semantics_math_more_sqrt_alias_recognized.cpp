// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathMore.SqrtAliasSQRTRecognized
 * Purpose: Validate that SQRT is recognized and lowered to the sqrt intrinsic.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic resolution); CodeGen (intrinsic call)
 * Expected Behavior: Compilation succeeds and IR contains a call to @sqrt(double 16.0).
 */
TEST(SemanticsMathMore, SqrtAliasSQRTRecognized) {
    const auto src =
        "10 PRINT SQRT(16)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("call double @sqrt(double 16.0)"), std::string::npos);
}
