// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsExpectedArity.StringAndScreen_Minimums
Inputs: Names: CHR$, LEFT$, RIGHT$, MID$, SCREEN
Code under test: SemanticAnalyzer::expectedArity
Expected behavior: CHR$=1; LEFT$=2; RIGHT$=2; MID$=2; SCREEN(min)=2
*/
TEST(SemanticsExpectedArity, StringAndScreen_Minimums) {
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("CHR$"), 1);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("LEFT$"), 2);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("RIGHT$"), 2);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("MID$"), 2);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("SCREEN"), 2);
}
