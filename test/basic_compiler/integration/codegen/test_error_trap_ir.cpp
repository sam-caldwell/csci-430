// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.ErrorTrapIR
 * Inputs: Small program using ON ERROR GOTO / ERROR / RESUME NEXT
 * Expected: IR contains trap global stores and switch to handler label
 */
TEST(Integration, ErrorTrapIR) {
    std::string src = R"(10 ON ERROR GOTO 300
20 PRINT 1
30 ERROR 5
40 PRINT 2
300 PRINT "H"
310 RESUME NEXT
320 PRINT 3
)";
    std::string ir = Compiler::compileString(src);
    // Check globals and key patterns
    ASSERT_NE(ir.find("@gwb_err_trap_line"), std::string::npos);
    ASSERT_NE(ir.find("store i32 300, ptr @gwb_err_trap_line"), std::string::npos);
    // ERROR dispatch should switch on trap line and include label %line300
    ASSERT_NE(ir.find("switch i32"), std::string::npos);
    ASSERT_NE(ir.find("label %line300"), std::string::npos);
    // Resume machinery should include gwb_resume globals
    ASSERT_NE(ir.find("@gwb_resume_line"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_resume_stmt"), std::string::npos);
}
