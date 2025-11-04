// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.SCREEN_IRPatterns
 * Purpose: Ensure a program using PRINT and SCREEN emits screen globals,
 *          snprintf declaration, and the screen write helper.
 */
/*
Test: Integration.SCREEN_IRPatterns
Inputs: BASIC snippet compiled through multiple stages
Code under test: Parser + Semantics + Codegen integration
Expected behavior: Emitted IR/state contains expected constructs and values
*/
TEST(Integration, SCREEN_IRPatterns) {
    const char* src =
        "10 PRINT \"A\"\n"
        "20 X=SCREEN(1,1)\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("@gwb_screen"), std::string::npos);
    ASSERT_NE(ir.find("declare i32 @snprintf"), std::string::npos);
    ASSERT_NE(ir.find("define void @gwb_screen_write"), std::string::npos);
    ASSERT_NE(ir.find("getelementptr inbounds [2000 x i8], ptr @gwb_screen"), std::string::npos);
}

