// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenDateTime.IntrinsicsIR
 * Purpose: Ensure DATE$ and TIME$ emit strftime/localtime/time calls with format globals.
 */
TEST(CodeGenDateTime, IntrinsicsIR) {
    const char* src =
        "10 PRINT DATE$\n"
        "20 PRINT TIME$\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i64 @strftime(ptr, i64, ptr, ptr)"), std::string::npos);
    ASSERT_NE(ir.find("declare ptr @localtime(ptr)"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_date"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_time"), std::string::npos);
    ASSERT_NE(ir.find("call i64 @time("), std::string::npos);
    ASSERT_NE(ir.find("call i64 @strftime"), std::string::npos);
}
