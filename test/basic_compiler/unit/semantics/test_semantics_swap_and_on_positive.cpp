// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;

/***
Test: Semantics.SwapAndOnPositive
Inputs: SWAP with matching types; ON GOTO/GOSUB with existing target lines
Code under test: SemanticAnalyzer::analyzeStmt for Swap and OnGoto/OnGosub
Expected behavior: Programs compile without semantic errors
*/
TEST(Semantics, SwapAndOnPositive) {
    // SWAP numeric vars
    {
        const char* src =
            "10 A=1: B=2\n"
            "20 SWAP A, B\n"
            "30 END\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    // ON GOTO to existing target
    {
        const char* src =
            "10 A=1\n"
            "20 ON A GOTO 100\n"
            "100 END\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
    // ON GOSUB to existing target
    {
        const char* src =
            "10 N=1\n"
            "20 ON N GOSUB 100\n"
            "30 END\n"
            "100 RETURN\n";
        const const std::string ir = Compiler::compileString(src);
        ASSERT_FALSE(ir.empty());
    }
}
