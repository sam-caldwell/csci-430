// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenMathMore.IntFixSgn
 * Purpose: Validate CodeGen lowering for INT, FIX, and SGN intrinsics.
 * Components Under Test: Compiler; CodeGenerator (IR for INT/FIX/SGN)
 * Expected Behavior: INT uses floor, FIX emits fptosi/sitofp, SGN emits compares and add.
 */
TEST(CodeGenMathMore, IntFixSgn) {
    const auto src =
        "10 PRINT INT(1.9)\n"
        "20 PRINT FIX(-1.9)\n"
        "30 PRINT SGN(-2)\n"
        "40 PRINT SGN(0)\n"
        "50 PRINT SGN(2)\n"
        "60 END\n";
    const std::string ir = Compiler::compileString(src);
    // INT -> floor
    EXPECT_NE(ir.find("declare double @floor(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @floor(double"), std::string::npos);
    // FIX -> fptosi/sitofp sequence
    EXPECT_NE(ir.find(" = fptosi double"), std::string::npos);
    EXPECT_NE(ir.find(" = sitofp i64"), std::string::npos);
    // SGN -> comparisons and add
    EXPECT_NE(ir.find(" = fcmp ogt double"), std::string::npos);
    EXPECT_NE(ir.find(" = fcmp olt double"), std::string::npos);
}
