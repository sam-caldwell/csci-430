// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenIfBlock.ElseWhile_UsingMidListOverrides
Purpose: Place a WHILE loop in ELSE and emit PRINT with multiple USING(...) in
         the same list to exercise mid-list override with last-one-wins, now
         through IF-block lowering. Also ensure non-channel screen mirroring via
         snprintf occurs.
*/
TEST(CodeGenIfBlock, ElseWhile_UsingMidListOverrides) {
    const char* src =
        "10 I=0\n"
        "20 IF 2>3 THEN\n"
        "30 PRINT \"SKIP\"\n"
        "40 ELSE\n"
        "50 WHILE I<1\n"
        // Multiple USING specs; last one should win for numeric formatting
        "60 PRINT \"L\", USING(\"%d\"), 7, \" M\", USING(\"%f\"), 2.5, \" T\"\n"
        "70 I=I+1\n"
        "80 WEND\n"
        "90 END IF\n"
        "100 END\n";

    std::string ir = Compiler::compileString(src);
    // IF with ELSE and WHILE lowering present
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    ASSERT_NE(ir.find("_while_cond"), std::string::npos);
    // Numeric type split labels and stdio calls present
    ASSERT_NE(ir.find("_print_int_"), std::string::npos);
    ASSERT_NE(ir.find("_print_flt_"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
    // Non-channel mirroring to screen buffer
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
}

