// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
 * Test: SemanticsExpectedArity.MoreStringIntrinsics
 * Purpose: Verify expected arities of additional string intrinsics.
 * Components Under Test: SemanticAnalyzer::expectedArity
 * Expected Behavior: STR$=1; STRING$=2; SPACE$=1; LTRIM$/RTRIM$=1
 */
TEST(SemanticsExpectedArity, MoreStringIntrinsics) {
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("STR$"), 1);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("STRING$"), 2);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("SPACE$"), 1);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("LTRIM$"), 1);
    EXPECT_EQ(SemanticAnalyzerArityAccessorForTests::expectedArity("RTRIM$"), 1);
}

