// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefType.DEFSTR_MakesPlainVarString
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

/*
 * Test: SemanticsDefType.SuffixOverridesDEFSTR
 */
TEST(SemanticsDefType, SuffixOverridesDEFSTR) {
    const char* src =
        "10 DEFSTR C\n"
        "20 LET C% = 3\n"
        "30 PRINT C%\n"
        "40 END\n";
    const std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
}

