// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.Then_NumericNonUsing_Spacing
 * Purpose: In THEN body, print numeric items without USING to exercise
 *          int/float split and nextStartsWithSpace formats. Also include a
 *          channel print to cover fprintf path in THEN.
 */
TEST(CodeGenIfBlock, Then_NumericNonUsing_Spacing) {
    const char* src =
        "10 IF 2>1 THEN\n"
        "20 PRINT 1, \" X\", 2.5\n"
        "25 PRINT #1, 3\n"
        "30 END IF\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    // THEN label present
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    // Integer/float split labels appear
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
    // Non-channel printf and channel fprintf present
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    // Spacing-sensitive numeric formats are referenced
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
}
