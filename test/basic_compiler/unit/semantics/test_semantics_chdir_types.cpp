// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.CHDIR_TypeChecks
Inputs: CHDIR with string literal and var (happy); CHDIR with number (sad)
Expected: Happy cases compile; numeric path errors.
*/
TEST(Semantics, CHDIR_TypeChecks) {
    // Happy: literal
    {
        const char* src = "10 CHDIR \"/tmp\"\n";
        const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    // Happy: variable
    {
        const char* src = "10 P$=\"/tmp\"\n20 CHDIR P$\n";
        const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    // Sad: numeric
    {
        const char* src = "10 CHDIR 1\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
    }
}

