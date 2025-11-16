// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenStringFuncs.MoreStringIntrinsicsIR
 * Purpose: Ensure STR$, SPACE$, STRING$, LTRIM$, RTRIM$ emit expected libc calls.
 * Components Under Test: Compiler::compileString() IR generation for string intrinsics.
 * Expected Behavior: IR references snprintf (STR$), memset (SPACE$/STRING$), strlen/strncpy (trim ops).
 */
TEST(CodeGenStringFuncs, MoreStringIntrinsicsIR) {
    const char* src =
        "10 PRINT STR$(42)\n"
        "20 PRINT SPACE$(3)\n"
        "30 PRINT STRING$(4,\"Z\")\n"
        "40 PRINT LTRIM$(\"  HI\")\n"
        "50 PRINT RTRIM$(\"HI  \")\n"
        "60 END\n";
    const std::string ir = Compiler::compileString(src);
    // STR$ uses snprintf into a buffer
    EXPECT_NE(ir.find("@snprintf"), std::string::npos);
    // SPACE$/STRING$ use memset to fill buffers
    EXPECT_NE(ir.find("@memset"), std::string::npos);
    // Trims rely on strlen and strncpy
    EXPECT_NE(ir.find("@strlen"), std::string::npos);
    EXPECT_NE(ir.find("@strncpy"), std::string::npos);
}

