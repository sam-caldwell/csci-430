// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Integration.LineInput_Channel_IR_UsesFgets
Inputs: OPEN "in.txt" FOR INPUT AS #1 : LINE INPUT #1, S$
Code under test: CodeGenerator (emit_line_block LINE INPUT channel path)
Expected behavior: IR contains call to @fgets and loads from @gwb_files.
*/
TEST(Integration, LineInput_Channel_IR_UsesFgets) {
    const char* src =
        "10 OPEN \"in.txt\" FOR INPUT AS #1\n"
        "20 LINE INPUT #1, S$\n";
    const const std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("@fgets"), std::string::npos);
    EXPECT_NE(ir.find("@gwb_files"), std::string::npos);
}

