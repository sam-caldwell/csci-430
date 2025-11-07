// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/semantics/SemanticAnalyzer.h"

using namespace gwbasic;

/***
Test: SemanticsExpectedArity.FallbackAndKeyMappings
Purpose: Provide a micro safety net around the arity table.
 - Assert unknown names fall back to 1.
 - Sanity check select known entries with optional-arity semantics.
*/
TEST(SemanticsExpectedArity, FallbackAndKeyMappings) {
    // Unknown intrinsic names should default to unary arity (1)
    EXPECT_EQ(1, SemanticAnalyzerArityAccessorForTests::expectedArity("UNKNOWN"));
    EXPECT_EQ(1, SemanticAnalyzerArityAccessorForTests::expectedArity("FOO$"));
    EXPECT_EQ(1, SemanticAnalyzerArityAccessorForTests::expectedArity("XYZ123"));

    // Known intrinsics with optional args should report the minimum arity
    EXPECT_EQ(2, SemanticAnalyzerArityAccessorForTests::expectedArity("INSTR"));
    EXPECT_EQ(2, SemanticAnalyzerArityAccessorForTests::expectedArity("SCREEN"));
}

