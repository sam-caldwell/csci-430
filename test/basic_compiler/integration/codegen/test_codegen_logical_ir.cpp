// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.Codegen_Emits_Logical_AND_OR_NOT
 * Purpose: Verify logical AND/OR/NOT lower to i1 operations with proper truthiness comparisons and casting.
 * Components Under Test: CodeGenerator::emitExpr for logical ops.
 * Expected Behavior: IR shows fcmp for truthiness, and/or i1, and NOT via fcmp .., 0.0 and uitofp i1.
 */
TEST(Integration, Codegen_Emits_Logical_AND_OR_NOT) {
    const std::string src = R"(10 PRINT 1 AND 0
20 PRINT 1 OR 0
30 PRINT NOT 0
)";
    const std::string ir = Compiler::compileString(src);
    // Look for i1 and/or ops
    ASSERT_NE(ir.find(" and i1 "), std::string::npos);
    ASSERT_NE(ir.find(" or i1 "), std::string::npos);
    // NOT emits fcmp oeq double <val>, 0.0 and uitofp
    ASSERT_NE(ir.find("fcmp oeq double"), std::string::npos);
    ASSERT_NE(ir.find("uitofp i1"), std::string::npos);
}
