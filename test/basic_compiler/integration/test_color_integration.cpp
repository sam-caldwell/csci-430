// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.COLOR_IRContainsSgrFormatAndPrintf
 * Inputs: Program with COLOR statement only.
 * Code under test: Compiler::compileString() IR generation for COLOR.
 * Expected behavior: IR defines @.fmt_sgr and references @printf for SGR.
 */
TEST(Integration, COLOR_IRContainsSgrFormatAndPrintf) {
    const char* src =
        "10 COLOR 2,4\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_sgr"), std::string::npos);
    ASSERT_NE(ir.find("@printf"), std::string::npos);
}
