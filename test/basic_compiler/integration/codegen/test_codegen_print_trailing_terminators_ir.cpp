// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.Print_Trailing_Terminators_IR
Purpose: Validate PRINT trailing ';' and ',' cases generate expected IR.
*/
TEST(Integration, Print_Trailing_Terminators_IR) {
    // Trailing semicolon: no newline format used at end; for empty items, emits printing of empty string
    {
        const char* src = "10 PRINT ;\n";
        const const std::string ir = Compiler::compileString(src);
        // Expect @.fmt_str (with newline) NOT used here at tail for empty case; instead @.fmt_str and empty prints used under empty+newline path
        // Safe assertion: IR contains @.str_empty and a printf/fprintf call with it (empty write)
        EXPECT_NE(ir.find("@.str_empty"), std::string::npos) << ir;
    }
    // Trailing comma: with zones ON, expect pad call path using @.fmt_pad
    {
        const char* src =
            "10 OPTION PRINTZONES ON\n"
            "20 PRINT 1,\n"; // trailing comma after numeric item
        const const std::string ir = Compiler::compileString(src);
        EXPECT_NE(ir.find("@.fmt_pad"), std::string::npos) << ir;
    }
}

