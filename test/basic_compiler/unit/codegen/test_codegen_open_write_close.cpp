// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: CodeGenLineBlock.OpenWriteClose
Purpose: Cover OPEN/WRITE/CLOSE codegen paths.
*/
TEST(CodeGenLineBlock, OpenWriteClose) {
    const char* src =
        "10 OPEN \"OUT.TXT\" FOR OUTPUT AS #1\n"
        "20 WRITE #1, \"A\", 2\n"
        "30 CLOSE #1\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("@fopen"), std::string::npos);
    ASSERT_NE(ir.find("@fprintf"), std::string::npos);
    ASSERT_NE(ir.find("@fclose"), std::string::npos);
    ASSERT_NE(ir.find("@gwb_files"), std::string::npos);
}

