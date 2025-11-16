// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.CALL_IRContainsHelper
 * Purpose: Ensure CALL emits the helper definition and an invocation.
 */
TEST(Integration, CALL_IRContainsHelper) {
    const auto src =
        "10 POKE 100,1\n"
        "20 CALL 100\n"
        "30 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("define void @gwb_call(i64"), std::string::npos);
    ASSERT_NE(ir.find("call void @gwb_call(i64"), std::string::npos);
}
