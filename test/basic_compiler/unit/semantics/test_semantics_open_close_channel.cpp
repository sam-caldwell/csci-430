// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
Test: Semantics.OpenCloseChannelRange
Inputs: OPEN/CLOSE with valid and invalid channel numbers
Code under test: SemanticAnalyzer::analyzeStmt(OpenStmt/CloseStmt)
Expected behavior: Channels 1..16 allowed; others error
*/
TEST(Semantics, OpenCloseChannelRange) {
    // Valid pair
    {
        const char* src =
            "10 OPEN \"f\" FOR INPUT AS #1\n"
            "20 CLOSE #1\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    // Invalid OPEN (#0)
    {
        const char* src = "10 OPEN \"f\" FOR INPUT AS #0\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
    }
    // Invalid CLOSE (#17)
    {
        const char* src = "10 CLOSE #17\n";
        EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
    }
}
