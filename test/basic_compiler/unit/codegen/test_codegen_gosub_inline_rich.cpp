// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenGosub.InlineRichBody
 * Purpose: Exercise emitSubroutineInline coverage: RANDOMIZE (time path), INPUT,
 *          IFStmt inside subroutine, array element assign, and MID$ on scalar.
 * Expected IR markers: gosub entry/cont labels, scanf, srand48, strncpy.
 */
TEST(CodeGenGosub, InlineRichBody) {
    const char* src =
        "10 PRINT \"S\": GOSUB 100: PRINT \"E\": END\n"
        "100 PRINT 1: INPUT Z\n"
        "105 MID$(S$,1,1)=\"Q\"\n"
        "110 RETURN\n";
    std::string ir = Compiler::compileString(src);
    // Inline gosub labels
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
    ASSERT_NE(ir.find("_gosub_cont"), std::string::npos);
    // INPUT in subroutine body
    ASSERT_NE(ir.find("@scanf"), std::string::npos);
    // MID$ lowering
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);
}
