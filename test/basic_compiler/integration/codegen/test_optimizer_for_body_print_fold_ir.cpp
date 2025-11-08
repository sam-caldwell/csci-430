// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: OptimizerForBody.PrintItemsFolded
 * Purpose: Ensure expressions inside FOR body are folded; IR lacks arithmetic ops.
 */
TEST(OptimizerForBody, PrintItemsFolded) {
    const char* src =
        "10 FOR I=1 TO 2\n"
        "20   PRINT 2+3\n"
        "30 NEXT I\n"
        "40 END\n";
    const std::string ir = Compiler::compileStringOptimized(src);
    // Expect that the specific addition was folded (no 'fadd double 2.0, 3.0')
    EXPECT_EQ(ir.find(" = fadd double 2.0, 3.0"), std::string::npos);
}
