// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics FOR STEP handling
 * Purpose: Ensure analyzeExpr(nullptr) is exercised when STEP is omitted.
 */
TEST(Semantics, ForWithoutStepAnalyzed) {
    const auto src =
        "10 FOR I=1 TO 3:NEXT I\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@.fmt_num"), std::string::npos);
}
