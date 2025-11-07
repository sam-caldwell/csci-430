// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: OptimizerExpr.IdentitiesAndFoldsBroad
Purpose: Exercise AstOptimizer::optExpr rules: unary +/- elimination/fold,
         add/sub/mul/div constant folding, and identities (x+0, x-0, x*1,
         x*0, x/1), plus a couple of comparisons.
*/
TEST(OptimizerExpr, IdentitiesAndFoldsBroad) {
    const char* src =
        "10 A=2\n"
        // Unary
        "11 PRINT -3\n"
        "12 PRINT +A\n"
        // Add/Sub identities and folds
        "20 PRINT 1+2\n"
        "21 PRINT 0+A\n"
        "22 PRINT A+0\n"
        "23 PRINT 5-3\n"
        "24 PRINT A-0\n"
        // Mul/Div identities and folds
        "30 PRINT 2*3\n"
        "31 PRINT 0*A\n"
        "32 PRINT A*0\n"
        "33 PRINT 4/2\n"
        "34 PRINT A/1\n"
        // Comparisons folded
        "40 PRINT 1=1\n"
        "41 PRINT 2<>1\n"
        "50 END\n";

    const std::string ir = Compiler::compileStringOptimized(src);
    // Spot-check for absence of typical operations that should fold away
    EXPECT_EQ(ir.find(" = fadd double 1.0, 2.0"), std::string::npos);
    EXPECT_EQ(ir.find(" = fsub double 5.0, 3.0"), std::string::npos);
    EXPECT_EQ(ir.find(" = fmul double 2.0, 3.0"), std::string::npos);
    EXPECT_EQ(ir.find(" = fdiv double 4.0, 2.0"), std::string::npos);
    // Ensure integer printing path exists for folded integer results
    EXPECT_NE(ir.find("_print_int_"), std::string::npos);
}

