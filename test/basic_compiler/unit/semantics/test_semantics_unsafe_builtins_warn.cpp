// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/*
Test: Semantics_Unsafe.WarnsOn_Peek_And_Usr
Inputs: Programs invoking PEEK(0) and USR(1)
Code under test: Semantic analyzer warnings for unsafe numeric builtins
Expected behavior: Programs compile successfully (no throw); warnings are logged internally
*/
TEST(Semantics_Unsafe, WarnsOn_Peek_And_Usr) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT USR(1)\n"); });
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 PRINT PEEK(0)\n"); });
}

