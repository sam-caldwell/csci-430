// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenFor.VarTypesAndChannelPrint
 * Purpose: Cover numKind load/store branches for Long32 (&), Single (!), Double (#) and
 *          PRINT USING/ channel paths inside FOR body.
 */
TEST(CodeGenFor, VarTypesAndChannelPrint) {
    const auto src =
        "10 FOR L& = 1 TO 2\n"
        "20 PRINT USING(\"X=#\"); L&\n"
        "30 NEXT L&\n"
        "40 FOR S! = 1 TO 2\n"
        "50 PRINT #1, S!\n"
        "60 NEXT S!\n"
        "70 FOR D# = 1 TO 2\n"
        "80 PRINT D# + 0.5\n"
        "90 NEXT D#\n"
        "100 END\n";
    std::string ir = Compiler::compileString(src);
    // Look for signatures implying all numeric kinds were handled
    ASSERT_NE(ir.find("sitofp i32"), std::string::npos);   // Long32 path
    ASSERT_NE(ir.find("fpext float"), std::string::npos);  // Single path
    ASSERT_NE(ir.find("load double"), std::string::npos);  // Double path
    // Channel printing and USING override
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@snprintf"), std::string::npos);
}

