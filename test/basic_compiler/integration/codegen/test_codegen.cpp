// (c) 2025 Sam Caldwell. All Rights Reserved.
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/***
 * Test: CodeGen.GeneratesIRWithBlocks
 * Purpose: Validate overall IR structure and line labels across a multi-line program.
 * Components Under Test: Compiler::compileString; CodeGenerator module/label emission.
 * Expected Behavior: IR defines main, has labels for lines 10..50, and declares @.fmt_num/@.fmt_str.
 */
TEST(CodeGen, GeneratesIRWithBlocks) {
    const std::string src = R"(10 LET A = 1+2*3
20 PRINT A
30 IF A > 3 THEN 50
40 PRINT "Done"
50 END
)";
    const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("define i32 @main()"), std::string::npos);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
    EXPECT_NE(ir.find("line20:"), std::string::npos);
    EXPECT_NE(ir.find("line30:"), std::string::npos);
    EXPECT_NE(ir.find("line40:"), std::string::npos);
    EXPECT_NE(ir.find("line50:"), std::string::npos);
    EXPECT_NE(ir.find("@.fmt_num"), std::string::npos);
    EXPECT_NE(ir.find("@.fmt_str"), std::string::npos);
}
