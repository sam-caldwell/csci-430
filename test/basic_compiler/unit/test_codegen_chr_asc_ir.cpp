// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

TEST(CodeGenBuiltins, CHR_EmitsMallocAndStores) {
    const char* src =
        "10 PRINT CHR$(65)\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("call ptr @malloc(i64 2)"), std::string::npos);
}

TEST(CodeGenBuiltins, ASC_LoadsFirstByte) {
    const char* src =
        "10 PRINT ASC(\"C\")\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_NE(ir.find("load i8, ptr"), std::string::npos);
}

