// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: CodeGenIfBlock.StringConcat_And_PrintSpacing
 * Purpose: Drive emit_if_block string-detection in BinaryExpr(Add) and both
 *          string/numeric PRINT format branches with last/non-last items.
 */
TEST(CodeGenIfBlock, StringConcat_And_PrintSpacing) {
    const auto src =
        "10 A$=\"Q\"\n"
        "20 IF 2<3 THEN\n"
        "30 PRINT \"A\"+\"B$\", 1, \" C\"\n"  // concat of two literals; space-prefixed next item
        "40 PRINT \"L\"+A$, 3\n"      // mixed concat with string var; last numeric item
        "50 ELSE\n"
        "60 PRINT \"D\"+\"E$\", 4, \" F\"\n"
        "70 PRINT A$+\"M\", 5\n"
        "75 INPUT Z\n"
        "80 END IF\n"
        "90 END\n";
    std::string ir = Compiler::compileString(src);
    // String concatenation uses strcpy/strcat
    ASSERT_NE(ir.find("@strcpy"), std::string::npos);
    ASSERT_NE(ir.find("@strcat"), std::string::npos);
    // THEN and ELSE blocks should be present
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // Expect both string and numeric formats for last and non-last items
    ASSERT_NE(ir.find("@.fmt_str_sp"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num_sp"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_str"), std::string::npos);
    ASSERT_NE(ir.find("@.fmt_num"), std::string::npos);
}
