/*
 * Test Suite: Integration CodeGen Layout
 * Purpose: Validate overall IR structure and labeling across multiple lines
 *          and statements within a small program.
 * Components Under Test: Compiler::compileString end-to-end; CodeGenerator
 *          module/label emission.
 * Expected Behavior: IR defines main, labels for each BASIC line, and
 *          declares @.fmt_num and @.fmt_str.
 */
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGen, GeneratesIRWithBlocks) {
    const std::string src = R"(10 LET A = 1+2*3
20 PRINT A
30 IF A > 3 THEN 50
40 PRINT "Done"
50 END
)";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("define i32 @main()"), std::string::npos);
    EXPECT_NE(ir.find("line10:"), std::string::npos);
    EXPECT_NE(ir.find("line20:"), std::string::npos);
    EXPECT_NE(ir.find("line30:"), std::string::npos);
    EXPECT_NE(ir.find("line40:"), std::string::npos);
    EXPECT_NE(ir.find("line50:"), std::string::npos);
    EXPECT_NE(ir.find("@.fmt_num"), std::string::npos);
    EXPECT_NE(ir.find("@.fmt_str"), std::string::npos);
}
