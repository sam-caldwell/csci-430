// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.If_ThenLine_ElseLine_IR
 * Inputs: IF A=0 THEN 100 ELSE 200
 * Code under test: IR generation
 * Expected behavior: IR branches lead to br label %line100 and %line200 in then/else blocks
 */
TEST(Integration, If_ThenLine_ElseLine_IR) {
    auto const src =
        "10 IF A=0 THEN 100 ELSE 200\n"
        "20 END\n"
        "100 PRINT 1\n"
        "200 PRINT 2\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("line10_if_then"), std::string::npos);
    ASSERT_NE(ir.find("line10_if_else"), std::string::npos);
    ASSERT_NE(ir.find("br label %line100"), std::string::npos);
    ASSERT_NE(ir.find("br label %line200"), std::string::npos);
}
