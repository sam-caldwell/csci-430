// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Integration.VAL_LEN_INSTR_IR_Contains_Deps
Inputs: Program using LEN, VAL, and INSTR
Code under test: Compiler::compileString IR generation
Expected behavior: IR contains calls to @strlen, @strtod, and @strstr
*/
TEST(Integration, VAL_LEN_INSTR_IR_Contains_Deps) {
    const char* src =
        "10 PRINT LEN(\"HELLO\")\n"
        "20 PRINT VAL(\"42\")\n"
        "30 PRINT INSTR(\"HELLO\",\"EL\")\n"
        "40 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("declare i64 @strlen(ptr)"), std::string::npos);
    ASSERT_NE(ir.find("declare double @strtod(ptr, ptr)"), std::string::npos);
    ASSERT_NE(ir.find("declare ptr @strstr(ptr, ptr)"), std::string::npos);
    ASSERT_NE(ir.find("call i64 @strlen(ptr"), std::string::npos);
    ASSERT_NE(ir.find("call double @strtod(ptr"), std::string::npos);
    ASSERT_NE(ir.find("call ptr @strstr(ptr"), std::string::npos);
}

