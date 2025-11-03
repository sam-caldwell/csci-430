// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

TEST(SemanticsStringFuncs, LEFT_Right_RequireStringAndNumeric) {
    // LEFT$ wrong types
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
    // RIGHT$ wrong types
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
}

TEST(SemanticsStringFuncs, MID_RequiresStringNumericAndOptionalNumeric) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(42,1)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\",\"B\")\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\",1,\"C\")\n20 END\n"); (void)ir; }, SemanticError);
}

TEST(SemanticsStringFuncs, ArityChecks) {
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT LEFT$(\"A\")\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT RIGHT$(\"A\",1,2)\n20 END\n"); (void)ir; }, SemanticError);
    EXPECT_THROW({ auto ir = Compiler::compileString("10 PRINT MID$(\"A\")\n20 END\n"); (void)ir; }, SemanticError);
}

