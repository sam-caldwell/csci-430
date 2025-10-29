// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen Core (arith/print)
 * Purpose: Validate IR for assignments, arithmetic, unary minus, comparisons,
 *          and PRINT numeric path.
 * Components Under Test: CodeGenerator emitExpr, emitLineBlock; Compiler.
 * Expected Behavior: Presence of fmul/fadd/fdiv/fsub, fcmp+uitofp, and
 *          printf with @.fmt_num; variables allocated and zero-initialized.
 */
TEST(CodeGenCore, AssignAndArithmeticAndPrint) {
    const auto src =
        "10 LET X = 1 + 2 * 3\n"
        "20 PRINT X\n"
        "30 PRINT 4/2\n"
        "40 PRINT -X\n"
        "50 PRINT 1 < 2\n"
        "60 PRINT 2 = 3\n"
        "70 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("define i32 @main()"), std::string::npos);
    EXPECT_NE(ir.find("%X = alloca double"), std::string::npos);
    EXPECT_NE(ir.find("store double 0.0, ptr %X"), std::string::npos);
    EXPECT_NE(ir.find(" = fmul double 2.0, 3.0"), std::string::npos);
    EXPECT_NE(ir.find(" = fadd double"), std::string::npos);
    EXPECT_NE(ir.find(" = fdiv double 4.0, 2.0"), std::string::npos);
    EXPECT_NE(ir.find(" = fsub double 0.0, %"), std::string::npos);
    EXPECT_NE(ir.find(" = fcmp olt double 1.0, 2.0"), std::string::npos);
    EXPECT_NE(ir.find(" = fcmp oeq double 2.0, 3.0"), std::string::npos);
    EXPECT_NE(ir.find(" = uitofp i1 %"), std::string::npos);
    EXPECT_NE(ir.find("getelementptr inbounds i8, ptr @.fmt_num"), std::string::npos);
}

