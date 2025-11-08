// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.WIDTH_TypeChecks
Purpose: WIDTH columns must be numeric; optional device must be string.
*/
TEST(Semantics, WIDTH_TypeChecks) {
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 WIDTH 80\n"); });
    EXPECT_NO_THROW({ (void)Compiler::compileString("10 WIDTH \"SCRN:\", 80\n"); });
    EXPECT_THROW({ (void)Compiler::compileString("10 WIDTH \"80\"\n"); }, SemanticError);
    EXPECT_THROW({ (void)Compiler::compileString("10 WIDTH 1, \"80\"\n"); }, SemanticError);
}

