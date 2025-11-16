// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: CodeGenIfBlock.Then_Randomize_SeedAndTime
Purpose: Exercise RANDOMIZE with a seed and without (time path) in THEN body.
*/
TEST(CodeGenIfBlock, Then_Randomize_SeedAndTime) {
    const char* src =
        "10 IF 1<2 THEN\n"
        "20 RANDOMIZE 5\n"
        "30 RANDOMIZE\n"
        "40 END IF\n"
        "50 END\n";
    const std::string ir = Compiler::compileString(src);
    // Look for srand48 call and time+ srandom path
    ASSERT_NE(ir.find("@srand48"), std::string::npos);
    ASSERT_NE(ir.find("@time"), std::string::npos);
}
