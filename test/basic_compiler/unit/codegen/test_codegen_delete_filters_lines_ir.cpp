// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
 * Test: CodeGenDELETE.FiltersLinesFromIR
 * Purpose: Ensure DELETE directives remove affected lines from emitted IR
 * (labels and list iteration). Deleted line labels should not appear.
 */
TEST(CodeGenDELETE, FiltersLinesFromIR) {
    const char* src =
        "10 PRINT \"A\"\n"
        "20 PRINT \"B\"\n"
        "30 DELETE 20-30\n"
        "40 PRINT \"C\"\n";
    const std::string ir = Compiler::compileString(src);
    // Labels for 20 and 30 must be absent
    ASSERT_EQ(ir.find("label %line20"), std::string::npos) << ir;
    ASSERT_EQ(ir.find("label %line30"), std::string::npos) << ir;
    // Labels for 10 and 40 must exist
    ASSERT_NE(ir.find("label %line10"), std::string::npos) << ir;
    ASSERT_NE(ir.find("label %line40"), std::string::npos) << ir;
}

