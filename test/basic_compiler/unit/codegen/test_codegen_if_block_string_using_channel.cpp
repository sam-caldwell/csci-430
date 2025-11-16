// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.StringUsing_WithAndWithoutChannel
 * Purpose: Cover emitIfBlock string PRINT items with USING(fmt) in THEN (stdio)
 *          and ELSE (channel) bodies, including next item spacing in THEN.
 * Expected IR markers: @fprintf, @printf, and @snprintf appear.
 */
TEST(CodeGenIfBlock, StringUsing_WithAndWithoutChannel) {
    const char* src =
        "10 IF 1<2 THEN\n"
        // THEN: string using override non-channel, followed by space-prefixed string
        "20 PRINT USING(\"%s\"), \"AA\", \" B\"\n"
        "30 ELSE\n"
        // ELSE: channel string using override
        "40 PRINT #3, USING(\"%s\"), \"CC\"\n"
        "50 END IF\n"
        "60 END\n";

    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
}
