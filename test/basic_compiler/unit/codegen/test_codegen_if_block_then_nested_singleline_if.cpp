// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenIfBlock.Then_Nested_SingleLineIf
Purpose: Cover nested single-line IF inside an IF-block THEN body.
*/
TEST(CodeGenIfBlock, Then_Nested_SingleLineIf) {
    const char* src =
        "10 IF 1<2 THEN\n"
        "20 IF 1=1 THEN 200\n"
        "30 END IF\n"
        "40 END\n"
        "200 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("line200"), std::string::npos);
    ASSERT_NE(ir.find("_cont_"), std::string::npos);
}

