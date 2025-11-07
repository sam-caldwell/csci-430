// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test: Integration.CHR_ASC_IRPatterns
 * Purpose: Ensure CHR$/ASC produce characteristic IR patterns.
 */
TEST(Integration, CHR_ASC_IRPatterns) {
    const auto src =
        "10 PRINT CHR$(65)\n"
        "20 PRINT ASC(\"B\")\n"
        "30 END\n";
    std::string ir = Compiler::compileString(src);
    ASSERT_FALSE(ir.empty());
    ASSERT_NE(ir.find("call ptr @malloc(i64 2)"), std::string::npos);
    ASSERT_NE(ir.find("load i8, ptr"), std::string::npos);
}

