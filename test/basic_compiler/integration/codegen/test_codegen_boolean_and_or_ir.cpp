// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenBoolean.AndOr_UseI1PredicatesAndBitOps
 * Purpose: Verify AND/OR generate i1 predicates and bitwise ops, with uitofp back to double.
 * Components Under Test: CodeGenerator::emitExpr for BinaryOp::And/BinaryOp::Or.
 * Expected Behavior: IR includes fcmp one -> i1, and/or i1, and uitofp i1 -> double.
 */
TEST(CodeGenBoolean, AndOr_UseI1PredicatesAndBitOps) {
    const char* src =
        "10 PRINT 1 AND 0\n"
        "20 PRINT 1 OR 0\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    // Spot check presence of boolean lowering and bitwise ops
    ASSERT_NE(ir.find("fcmp one double"), std::string::npos);
    ASSERT_NE(ir.find("and i1"), std::string::npos);
    ASSERT_NE(ir.find("or i1"), std::string::npos);
    ASSERT_NE(ir.find("uitofp i1"), std::string::npos);
}
