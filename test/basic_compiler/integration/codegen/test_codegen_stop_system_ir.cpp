// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.CodegenStopSystemIR
 * Inputs: Programs using STOP and SYSTEM.
 * Expected: IR contains break message global and branches to exit.
 */
TEST(Integration, CodegenStopSystemIR) {
    {
        std::string src = "10 PRINT 1\n20 STOP\n30 PRINT 2\n";
        const std::string ir = Compiler::compileString(src);
        // STOP uses @.msg_break and call to printf with current line
        ASSERT_NE(ir.find("@.msg_break"), std::string::npos);
        ASSERT_NE(ir.find("call i32 (ptr, ...) @printf(ptr %"), std::string::npos);
        ASSERT_NE(ir.find("Break in %d"), std::string::npos);
        // Should branch to exit somewhere after STOP
        ASSERT_NE(ir.find("br label %exit"), std::string::npos);
    }
    {
        std::string src = "10 PRINT 1\n20 SYSTEM\n30 PRINT 2\n";
        const std::string ir = Compiler::compileString(src);
        // SYSTEM should branch to exit but not include break message
        ASSERT_EQ(ir.find("@.msg_break"), std::string::npos);
        ASSERT_NE(ir.find("br label %exit"), std::string::npos);
    }
}
