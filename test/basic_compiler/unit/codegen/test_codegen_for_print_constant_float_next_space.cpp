// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenFor.Print_ConstantFloat_NextSpace
 * Purpose: Hit emitFor constant-float PRINT path with nextStartsWithSpace selection.
 */
TEST(CodeGenFor, Print_ConstantFloat_NextSpace) {
    const auto src =
        "10 FOR I=1 TO 1\n"
        "20 PRINT 3.5, \" X\"\n"
        "30 NEXT I\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    // Expect *_ns formats for both int and float categories and a printf call with double literal
    ASSERT_NE(ir.find("@.fmt_num_ns"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_int_ns"), std::string::npos);
    ASSERT_NE(ir.find("call i32 (ptr, ...) @printf(ptr"), std::string::npos);
}
