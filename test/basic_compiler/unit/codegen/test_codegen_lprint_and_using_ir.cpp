// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenLPRINT.BasicAndUsing
 * Purpose: Ensure LPRINT emits channel (printer) path using @gwb_files[0]
 * and does not emit @printf/@gwb_screen_write mirroring, and that USING
 * formatting is honored via snprintf+fwrite.
 */
TEST(CodeGenLPRINT, BasicAndUsing) {
    const char* src =
        "10 LPRINT \"HELLO\"\n"
        "20 LPRINT USING(\"%d\"), 7\n";
    const std::string ir = Compiler::compileString(src);
    // Channel path references @gwb_files index 0
    ASSERT_NE(ir.find("getelementptr inbounds [16 x ptr], ptr @gwb_files, i64 0, i64 0"), std::string::npos);
    // Should not mirror to screen buffer helper
    ASSERT_EQ(ir.find("call void @gwb_screen_write"), std::string::npos);
    // USING path uses snprintf and fwrite
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
    ASSERT_NE(ir.find("@fwrite"), std::string::npos);
}
