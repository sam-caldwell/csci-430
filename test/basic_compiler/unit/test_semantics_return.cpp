// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
 * Test Suite: Semantics RETURN handling
 * Purpose: Exercise the ReturnStmt path in semantic analyzer.
 */
TEST(Semantics, ReturnStatementCompiles) {
    const auto src =
        "10 RETURN\n"
        "20 END\n";
    std::string ir = Compiler::compileString(src);
    EXPECT_NE(ir.find("ret i32 0"), std::string::npos);
}

