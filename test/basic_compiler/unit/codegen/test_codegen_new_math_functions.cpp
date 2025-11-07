// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenMathExt.RndCintCsngCdbl
 * Purpose: Validate CodeGen lowering for RND, CINT, CSNG, and CDBL intrinsics.
 * Components Under Test: Compiler; CodeGenerator (IR for RND/CINT/CSNG/CDBL)
 * Expected Behavior: RND calls gwb_rnd helper; CINT declares and calls round(double);
 *                    CSNG emits fptrunc (double->float) and fpext (float->double);
 *                    CDBL compiles as a no-op cast to double (no special pattern asserted).
 */
TEST(CodeGenMathExt, RndCintCsngCdbl) {
    const auto src =
        "10 PRINT RND(1)\n"
        "20 PRINT CINT(1.6)\n"
        "30 PRINT CSNG(1.5)\n"
        "40 PRINT CDBL(2)\n"
        "50 END\n";
    std::string ir = Compiler::compileString(src);
    // RND -> gwb_rnd helper
    EXPECT_NE(ir.find("define double @gwb_rnd(double"), std::string::npos);
    EXPECT_NE(ir.find("call double @gwb_rnd(double"), std::string::npos);
    // CINT -> round
    EXPECT_NE(ir.find("declare double @round(double)"), std::string::npos);
    EXPECT_NE(ir.find("call double @round(double"), std::string::npos);
    // CSNG -> fptrunc/fpext sequence
    EXPECT_NE(ir.find(" = fptrunc double"), std::string::npos);
    EXPECT_NE(ir.find(" = fpext float"), std::string::npos);
}
