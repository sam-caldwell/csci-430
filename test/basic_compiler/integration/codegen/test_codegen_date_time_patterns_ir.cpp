// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.DateTime_StrftimePatternsIR
 * Purpose: Check IR contains the date/time strftime format strings and calls.
 */
TEST(Integration, DateTime_StrftimePatternsIR) {
    const char* src =
        "10 PRINT DATE$\n"
        "20 PRINT TIME$\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("c\"%m-%d-%y\\00\""), std::string::npos);
    ASSERT_NE(ir.find("c\"%H:%M:%S\\00\""), std::string::npos);
    ASSERT_NE(ir.find("@strftime"), std::string::npos);
}

