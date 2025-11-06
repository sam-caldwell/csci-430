// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.MidAssign_ArrayBoundsErrors
 * Purpose: Exercise emitIfBlock MID$ assignment to string array element with out-of-bounds indices
 *          to drive the error branch generation for MID$ array path in THEN.
 */
TEST(CodeGenIfBlock, MidAssign_ArrayBoundsErrors) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM Y$(2,2,2)\n"
        "30 IF 1<2 THEN\n"
        "40 MID$(Y$(3,1,1),1)=\"X\"\n"  // upper OOB on first dim
        "50 ELSE\n"
        "60 PRINT \"ALT\"\n"
        "70 END IF\n"
        "80 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_mid_err2_"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
}

