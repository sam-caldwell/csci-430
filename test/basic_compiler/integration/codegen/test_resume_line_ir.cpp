// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "../../helper/ir_match.h"

using namespace gwbasic;
using namespace irtest;

/***
 * Test: CodeGenResume.ResumeLine_GeneratesTrampolineToTarget
 * Inputs: Program with ON ERROR, ERROR, and RESUME 30 in handler
 * Code under test: CodeGenerator::emitLineBlock RESUME line lowering
 * Expected behavior: IR contains a resume-case trampoline that branches to %line30.
 */
TEST(CodeGenResume, ResumeLine_GeneratesTrampolineToTarget) {
    auto const src =
        "10 ON ERROR GOTO 100\n"
        "20 PRINT 1: ERROR 5: PRINT 2\n"
        "30 PRINT 3\n"
        "100 PRINT \"H\"\n"
        "110 RESUME 30\n";
    const const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    // Expect a direct branch to %line30 with handler flag cleared
    ASSERT_NE(ir.find("store i1 false, ptr @gwb_in_handler"), std::string::npos);
    ASSERT_NE(ir.find("br label %line30"), std::string::npos);
}
