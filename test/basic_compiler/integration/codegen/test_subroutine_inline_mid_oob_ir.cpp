// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/**
 * Test: Integration.SubroutineInline_MidAssign_OOB_Error_IR
 * Purpose: Trigger MID$ array index out-of-bounds inside inline GOSUB to cover error path.
 */
TEST(Integration, SubroutineInline_MidAssign_OOB_Error_IR) {
    const char* src =
        "10 DIM B$(1)\n"
        "20 GOSUB 100\n"
        "30 END\n"
        "100 MID$(B$(2),1)=\"X\"\n" // index 2 out of upper bound 1
        "110 RETURN\n";
    std::string ir = Compiler::compileString(src);
    // Error dispatch path: set error code 9 (Subscript out of range) and branch
    EXPECT_NE(ir.find("_mid_err_"), std::string::npos) << ir;
    EXPECT_NE(ir.find("store i32 9, ptr @gwb_err_code"), std::string::npos) << ir;
}

