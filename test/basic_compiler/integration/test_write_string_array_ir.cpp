// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.StringArray_WRITE_IR_UsesFprintf
 * Purpose: Verify WRITE #n with string array lowers to fprintf and fmt strings.
 */
TEST(Integration, StringArray_WRITE_IR_UsesFprintf) {
    const char* src =
        "10 DIM A$(2)\n"
        "20 A$(1)=\"ONE\"\n"
        "30 OPEN \"out.txt\" FOR OUTPUT AS #1\n"
        "40 WRITE #1, A$(1)\n"
        "50 CLOSE #1\n"
        "60 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("declare i32 @fprintf(ptr, ptr, ...)"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [16 x ptr], ptr @gwb_files"), std::string::npos);
    ASSERT_NE(ir.find("call i32 (ptr, ...) @fprintf"), std::string::npos);
}

