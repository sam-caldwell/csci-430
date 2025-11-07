// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsDefType.DEFSTR_MakesPlainVarString
 * Inputs: DEFSTR A; assign string; then assign numeric (error case).
 * Code under test: Compiler::compileString() + semantics for DEFSTR typing.
 * Expected behavior: Valid string assignment passes; numeric assignment errors.
 */
TEST(SemanticsDefType, DEFSTR_MakesPlainVarString) {
    const char* src_ok =
        "10 DEFSTR A\n"
        "20 LET A = \"hi\"\n"
        "30 PRINT A\n"
        "40 END\n";
    const std::string ir1 = Compiler::compileString(src_ok);
    ASSERT_FALSE(ir1.empty());

    const char* src_err =
        "10 DEFSTR B\n"
        "20 LET B = 5\n"
        "30 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src_err); (void)ir; }, SemanticError);
}
