// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.OptionPrintZones_IRContainsPad
Inputs: Program with OPTION PRINTZONES ON and PRINT "A","B"
Code under test: Code generation for zone padding
Expected behavior: IR contains references to @.fmt_pad and @.spaces_14
*/
TEST(Integration, OptionPrintZones_IRContainsPad) {
    const char* src =
        "10 OPTION PRINTZONES ON\n"
        "20 PRINT \"A\", \"B\"\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@.fmt_pad"), std::string::npos);
    ASSERT_NE(ir.find("@.spaces_14"), std::string::npos);
}

