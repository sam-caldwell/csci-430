// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.Arrays3D_And_BoundsErrors
 * Purpose: Exercise emit_if_block array index plumbing for 3D numeric and string arrays,
 *          including out-of-bounds error branches and MID$ on array/scalar destinations.
 */
TEST(CodeGenIfBlock, Arrays3D_And_BoundsErrors) {
    const auto src =
        "10 OPTION BASE 1\n"
        "20 DIM X(2,2,2)\n"
        "22 DIM Y$(2,2,2)\n"
        "21 S$=\"\"\n"
        "30 I=2\n"
        "40 IF I<3 THEN\n"
        "50 X(1,2,2)=42\n"
        "51 Y$(2,1,1)=\"S\"\n"
        "52 X(3,1,1)=7\n"     // upper OOB to build err branch
        "53 MID$(Y$(2,2,2),1)=\"Q\"\n"
        "54 MID$(S$,2,1)=\"Z\"\n"
        "60 ELSE\n"
        "61 X(0,1,1)=5\n"      // lower OOB (OPTION BASE 1)
        "62 Y$(2,2,3)=\"T\"\n"  // upper OOB third dim
        "70 END IF\n"
        "80 END\n";
    const std::string ir = Compiler::compileString(src);
    // Expect array error handling scaffolding
    ASSERT_NE(ir.find("_arr_err_"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_err_code"), std::string::npos);
    ASSERT_NE(ir.find("switch i32"), std::string::npos);
    // Expect string and numeric array geps
    ASSERT_NE(ir.find("getelementptr inbounds ["), std::string::npos);
    // Index conversion plumbing (fptosi)
    ASSERT_NE(ir.find("fptosi double"), std::string::npos);
    // MID$ path uses strncpy
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
