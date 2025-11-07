// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: Integration.ErrorResume_Rich_IR
 * Purpose: Exercise ON ERROR GOTO, ERROR, and RESUME (line/next/re-execute) codegen paths in emit_line_block.
 * Components Under Test: CodeGenerator::emitLineBlock error/handler & resume logic
 * Expected Behavior: IR stores to @gwb_err_trap_line, updates ERR/ERL, switches on resume line, and branches appropriately.
 */
TEST(Integration, ErrorResume_Rich_IR) {
    const char* src =
        "10 ON ERROR GOTO 100\n"
        "20 ERROR 7\n"
        "30 END\n"
        "100 RESUME NEXT\n"
        "110 RESUME 30\n"
        "120 RESUME\n";
    std::string ir = Compiler::compileString(src);
    // Trap setup and error code handling present
    EXPECT_NE(ir.find("@gwb_err_trap_line"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_err_code"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_err_line"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_resume_line"), std::string::npos);
    EXPECT_NE(ir.find("ERR"), std::string::npos);
    EXPECT_NE(ir.find("ERL"), std::string::npos);
    // Resume dispatch switch and case trampolines
    EXPECT_NE(ir.find("_resume_case_"), std::string::npos);
}

