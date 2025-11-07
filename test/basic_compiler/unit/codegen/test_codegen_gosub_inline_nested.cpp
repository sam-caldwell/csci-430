// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenGosub.InlineNested
Purpose: Cover recursive inline subroutine emission by invoking GOSUB within
         a subroutine body; also include fallthrough between statements around
         the nested call.
Expected IR markers: multiple _gosub_entry/_gosub_cont labels and nested
                     entry labels.
*/
TEST(CodeGenGosub, InlineNested) {
    const char* src =
        "10 GOSUB 100: END\n"
        "100 PRINT \"A\": GOSUB 200: PRINT 1: RETURN\n"
        "200 PRINT \"B\": RETURN\n";

    std::string ir = Compiler::compileString(src);
    // At least two inline gosub entries should exist (outer + nested)
    size_t first = ir.find("_gosub_entry");
    ASSERT_NE(first, std::string::npos);
    size_t second = ir.find("_gosub_entry", first + 1);
    ASSERT_NE(second, std::string::npos);
    // Continuations should be present as well
    ASSERT_NE(ir.find("_gosub_cont"), std::string::npos);
}

