// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.UsingFloat_IntFloat_And_Channel
 * Purpose: Drive emitIfBlock's numeric USING override paths for both integer and
 *          float values, including channel printing and nextStartsWithSpace format
 *          selection in BOTH THEN and ELSE bodies.
 * Expected IR markers: _if_then/_if_else, _print_int_, _print_flt_, @fprintf,
 *                      @printf, and @snprintf for screen buffer writes.
 */
TEST(CodeGenIfBlock, UsingFloat_IntFloat_And_Channel) {
    const char* src =
        "10 IF 1<2 THEN\n"
        // THEN: float using (non-channel) with a following space-prefixed string
        "20 PRINT USING(\"%f\"), 2.5, \" X\"\n"
        // THEN: channel integer using
        "25 PRINT #1, USING(\"%d\"), 7\n"
        "30 ELSE\n"
        // ELSE: integer using (non-channel) and float using with channel + spacing
        "40 PRINT USING(\"%d\"), 9, \" Y\"\n"
        "45 PRINT #2, USING(\"%f\"), 3.75\n"
        "50 END IF\n"
        "60 END\n";

    std::string ir = Compiler::compileString(src);
    // IF block is present
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // Numeric type split labels emitted at least once
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
    // Channel and stdio printing touched
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    // Screen mirroring present for non-channel
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
    // Spacing-sensitive formats referenced
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
}
