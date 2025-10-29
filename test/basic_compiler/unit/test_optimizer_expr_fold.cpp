// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: Optimizer Expressions
 * Purpose: Validate AST constant folding and algebraic identities to reduce
 *          the amount of IR emitted for arithmetic and unary operators.
 * Components Under Test: AstOptimizer::optimize/optExpr, Compiler::compileStringOptimized.
 * Expected Behavior: Pure-constant expressions fold to immediates, identity
 *          rewrites hold (x+0, x*1, x/1, x*0), and unnecessary fmul/fadd are absent.
 */
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(OptimizerExpr, ConstantFoldingArithmetic) {
    // 1 + 2 * 3 => 7.0, no fmul/fadd for constants
    const char* src = "10 PRINT 1 + 2 * 3\n20 END\n";
    auto ir = Compiler::compileStringOptimized(src);
    EXPECT_NE(ir.find("call i32 (ptr, ...) @printf"), std::string::npos);
    EXPECT_NE(ir.find(", double 7.0)"), std::string::npos);
    EXPECT_EQ(ir.find(" = fmul double 2.0, 3.0"), std::string::npos);
    EXPECT_EQ(ir.find(" = fadd double"), std::string::npos);
}

// Single-test-per-file policy: additional cases moved to dedicated files.
