// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenGosub.MidArrayBounds_ErrorPath
 * Purpose: Exercise emitSubroutineInline's MID$ handling on string array elements,
 *          including bounds checks and error-path plumbing inside an inline
 *          subroutine body.
 * Expected IR markers: mid_ok_/mid_err_ labels, stores to gwb_err_* globals,
 *                      and handler switch scaffolding.
 */
TEST(CodeGenGosub, MidArrayBounds_ErrorPath) {
    const char* src =
        "10 OPTION BASE 1\n"
        "20 DIM S$(2,2)\n"
        "30 GOSUB 100: END\n"
        // Out-of-bounds on first index (3 > 2)
        "100 MID$(S$(3,1),1,1)=\"Z\": RETURN\n";

    std::string ir = Compiler::compileString(src);
    // Expect mid error labels/handler scaffolding
    ASSERT_NE(ir.find("_mid_err_"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_in_handler"), std::string::npos);
    ASSERT_NE(ir.find("switch i32"), std::string::npos);
}
